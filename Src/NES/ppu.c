/*
 * ppu.c
 *
 *  Created on: Mar 14, 2019
 *      Author: boning
 */
#include "main.h"
#include "NES/ppu.h"
#include "NES/cpu.h"
#include "NES/mapper.h"
#include "stdio.h"
#include "string.h"
#include "lcd.h"

#define PPU_RENDERING 	(PPUMASK.bg || PPUMASK.spr)
#define PPU_SPRITE_H	(PPUCTRL.sprSz ? 16 : 8)
#define NTH_BIT(x, n)	(((x) >> (n)) & 1)

typedef struct {
    u8 id;     // Index in OAM.
    u8 x;      // X position.
    u8 y;      // Y position.
    u8 tile;   // Tile index.
    u8 attr;   // Attributes.
    u8 dataL;  // Tile data (low).
    u8 dataH;  // Tile data (high).
} Sprite;

/* Register Files */
// PPUCTRL ($2000) register
union
{
    struct
    {
        unsigned nt     : 2;  // Nametable ($2000 / $2400 / $2800 / $2C00).
        unsigned incr   : 1;  // Address increment (1 / 32).
        unsigned sprTbl : 1;  // Sprite pattern table ($0000 / $1000).
        unsigned bgTbl  : 1;  // BG pattern table ($0000 / $1000).
        unsigned sprSz  : 1;  // Sprite size (8x8 / 8x16).
        unsigned slave  : 1;  // PPU master/slave.
        unsigned nmi    : 1;  // Enable NMI.
    };
    u8 r;
} PPUCTRL;

// PPUMASK ($2001) register
union
{
    struct
    {
        unsigned gray    : 1;  // Grayscale.
        unsigned bgLeft  : 1;  // Show background in leftmost 8 pixels.
        unsigned sprLeft : 1;  // Show sprite in leftmost 8 pixels.
        unsigned bg      : 1;  // Show background.
        unsigned spr     : 1;  // Show sprites.
        unsigned red     : 1;  // Intensify reds.
        unsigned green   : 1;  // Intensify greens.
        unsigned blue    : 1;  // Intensify blues.
    };
    u8 r;
} PPUMASK;

// PPUSTATUS ($2002) register
union
{
    struct
    {
        unsigned bus    : 5;  // Not significant.
        unsigned sprOvf : 1;  // Sprite overflow.
        unsigned sprHit : 1;  // Sprite 0 Hit.
        unsigned vBlank : 1;  // In VBlank?
    };
    u8 r;
} PPUSTATUS;

// PPUSCROLL ($2005) and PPUADDR ($2006)
typedef union ADDR
{
    struct
    {
        unsigned cX : 5;  // Coarse X.
        unsigned cY : 5;  // Coarse Y.
        unsigned nt : 2;  // Nametable.
        unsigned fY : 3;  // Fine Y.
    };
    struct
    {
        unsigned l : 8;
        unsigned h : 7;
    };
    unsigned addr : 14;
    unsigned r : 15;
} PPUADDR;

struct {
	PPUADDR vAddr;
	PPUADDR tAddr;
	unsigned fX : 3;
	unsigned w : 1;
} PPUINTER = {.w = 0};


/* Registers Declaration
 * Register files that are defined in ppu.h
 * - PPUCTRL					$2000
 * - PPUMASK					$2001
 * - PPUSATUS					$2002
 * - PPUSCROLL					$2005
 * - PPUADDR					$2006
 *
 * Register files that are not defined explicitly (Implemented through functions)
 * - OAMADDR					$2003
 * - OAMDATA					$2004
 * - PPUDATA					$2007
 * - OAMDAM						$4014
 */

/*	Memory Definitions
 *	ciRam: nametable memory space (2048 bytes)
 *	- There are two physical nametables.
 *  - Each nametables has Background Area (960 bytes)+ Attribute Table (64 bytes)
 *  - Four nametable addressing spaces.
 *
 *  cgRam: palettes (64 bytes)
 *  - Used for palettes. Can be replaced by the CLUT (color look up table)
 *
 *	oamMem: Sprite properties (256 bytes)
 *	- Memory for sprite properties
 *	- Each sprite needs 4 bytes (Y position, tile index, attributes, X position).
 *	- Supports 64 sprites in total.
 */

u8 ciRam[0x800];
u8 cgRam[0x20];				// [?] palettes may be replaced using hardware solution
u8 oamMem[0x100];
Sprite oam[8], secOam[8]; 	// [?] why need two? [?] Second OAM functions like a buffer

u8 oamAddr;

// Background latches:
u8 nt, at, bgL, bgH;
// Background shift registers:
u8 atShiftL, atShiftH; u16 bgShiftL, bgShiftH;
u8 atLatchL, atLatchH;

u8 frameOdd;
u16 scanline, dot;

/* Graphic Memory (LCD) */
// Width: 240 Height: 320
extern uint8_t GRAM[LCD_WIDTH * LCD_HEIGHT];	// [!] Can be moved to external memory

/* Configurations */
Mirroring mirroring;

/* Configurations Access */
void PPU_SetMirror(Mirroring mode)
{
	mirroring = mode;
}

u16 NT_Mirror(u16 addr)
{
	// [?] Should the input address be 0x0XXX instead of 0x2XXX
	// [!] Horizontal Implementation != LaiNES
	switch (mirroring) {
	case VERTICAL:		return addr % 0x800;						// Use the top two ($2000 and $2400)
	case HORIZONTAL: 	return ((addr & 0x800) + addr % 0x400);		// Use the left two ($2000 and $2800)
	default: 			return addr - 0x2000;						// [?] Why need this
	}
}

/* PPU Memory Access
 * Memory Mapping
 * - $0000 - $0FFF		Pattern Table 0
 * - $1000 - $1FFF		Pattern Table 1
 * - $2000 - $23FF		Nametable 0
 * - $2400 - $27FF		Nametable 1
 * - $2800 - $2BFF		Nametable 2
 * - $2C00 - $2FFF		Nametable 3
 * - $3000 - $3EFF		Mirrors of $2000 - $2EFF
 * - $3F00 - $3F1F		Palette RAM indexes
 * - $3F20 - $3FFF		Mirrors of $3F00 - $3F1F
 * */
u8 PPU_MemRead(u16 addr)
{
	switch (addr)
	{
		case 0x0000 ... 0x1FFF:
			return mapper_rd(addr);
		case 0x2000 ... 0x3EFF:
			return ciRam[NT_Mirror(addr)];
		case 0x3F00 ... 0x3FFF:
			// 0x3F10 0x3F14 ... 0x3F1C are the mirrors of 0x3F00 ... 0x3F0C
			if ((addr & 0x13) == 0x10) addr &= ~0x10;
			return cgRam[addr & 0x1F] & (PPUMASK.gray ? 0x30 : 0xFF);
		default:
			return 0x00;
	}
}

void PPU_MemWrite(u16 addr, u8 v)
{
	switch (addr)
	{
		case 0x0000 ... 0x1FFF:
			mapper_wr(addr, v);
			break;
		case 0x2000 ... 0x3EFF:
			ciRam[NT_Mirror(addr)] = v;
			break;
		case 0x3F00 ... 0x3FFF:
			if((addr & 0x13) == 0x10) addr &= ~0x10;
			cgRam[addr & 0x1F] = v;
			break;
	}
}

/* PPU Registers Access */
u8 PPU_RegAccess (u16 index, u8 v, Rw rw)
{
	static u8 res = 0;		// Result of the operation
	static u8 buffer = 0;	// VRAM read buffer
	if (rw == WRITE)
	{
		res = v;
		switch (index)
		{
		case 0:	//PPUCTRL	($2000)
			PPUCTRL.r = v; PPUINTER.tAddr.nt = PPUCTRL.nt;
			break;
		case 1:	//PPUMASK	($2001)
			PPUMASK.r = v;
			break;
		case 3:	//OAMADDR	($2003)
			oamAddr = v;
			break;
		case 4: //OAMDATA	($2004)
			oamMem[oamAddr++] = v;
			break;
		case 5:	//PPUSCROLL	($2005)
			if (!PPUINTER.w)
			{
				PPUINTER.fX = v & 0x07;
				PPUINTER.tAddr.cX = v >> 3;
			}
			else
			{
				PPUINTER.tAddr.fY = v & 0x07;
				PPUINTER.tAddr.cY = v >> 3;
			}
			PPUINTER.w = !PPUINTER.w;
			break;
		case 6: //PPUADDR 	($2006)
			if (!PPUINTER.w)
			{
				PPUINTER.tAddr.h = v & 0x3F;
			}
			else
			{
				PPUINTER.tAddr.l = v;
				PPUINTER.vAddr.r = PPUINTER.vAddr.r;
			}
			PPUINTER.w = !PPUINTER.w;
			break;
		}
	}
	else
	{
		switch (index)
		{
		case 2:
			res = (res & 0x1F) | (PPUSTATUS.r & ~0x1F);
			PPUSTATUS.vBlank = 0;
			PPUINTER.w = 0;
			break;
		case 4:
			res = oamMem[oamAddr];
			break;
		case 7:
			if (PPUINTER.vAddr.addr <=  0x3EFF)
			{
				res = buffer;
				buffer = PPU_MemRead(PPUINTER.vAddr.addr);
			}
			else
				res = buffer = PPU_MemRead(PPUINTER.vAddr.addr);
			PPUINTER.vAddr.addr += PPUCTRL.incr ? 32 : 1;
		}
	}
	return res;
}

/* Calculate graphics addresses */
// Get PPU nametable address
u16 PPU_GetNtAddr()
{
	return 0x2000 | (PPUINTER.vAddr.r & 0xFFF);
}
// Get PPU Attribute table address
u16 PPU_GetAtAddr()
{
	// [?] Why coarse X and Y are divided by 4
	return 0x23C0 | (PPUINTER.vAddr.nt << 10) | ((PPUINTER.vAddr.cY / 4) << 3) | (PPUINTER.vAddr.cX / 4);
}
// Get Background Tile Address
u16 PPU_GetBgAddr()
{
	return (PPUCTRL.bgTbl * 0x1000) + (nt * 16) + PPUINTER.vAddr.fY;
}

/*
 * PPU_HScroll()
 * Horizontal Scroll
 *
 * PPU_VScroll()
 * Vertical Scroll happens if rendering is enabled. It's called @ dot 256
 * */
void PPU_HScroll()
{
	if(!PPU_RENDERING)
		return;
	if(PPUINTER.vAddr.cX == 31)
		PPUINTER.vAddr.r ^= 0x41F;	// Switch horizontal nametable and wrap coarse X.
	else
		PPUINTER.vAddr.cX++;
}

void PPU_VScroll(){
	if (!PPU_RENDERING)
		return;
	if (PPUINTER.vAddr.fY < 7)		// Check if still in the same tile after scroll
		PPUINTER.vAddr.fY ++;
	else
	{
		PPUINTER.vAddr.fY = 0;		// Wrap if move to the next tile.
		if (PPUINTER.vAddr.cY == 31)
			PPUINTER.vAddr.cY = 0;	// Wrap the tile to the top row if needed
		else if (PPUINTER.vAddr.cY == 29)
		{
			PPUINTER.vAddr.cY = 0;
			PPUINTER.vAddr.nt ^= 0b10;
		}
		else
			PPUINTER.vAddr.cY++;
	}
}

/*
 * PPU_HUpdate()
 * Load the horizontal location from temporary address to current address.
 *
 * PPU_Vupdate()
 * Load the vertical location from temporary address to current address.
 * */
void PPU_HUpdate()
{
	if (!PPU_RENDERING)
		return;
	PPUINTER.vAddr.r = (PPUINTER.vAddr.r & ~0x041F) | (PPUINTER.tAddr.r & 0x041F);	// Set the NT and the coarse X from tAddr
}

void PPU_VUpdate()
{
	if (!PPU_RENDERING)
		return;
	PPUINTER.vAddr.r = (PPUINTER.vAddr.r & ~0x7BE0) | (PPUINTER.tAddr.r & 0x7BE0);	// Set the NT and fine and coarse X from tAddr
}

void PPU_ReloadShift()
{
	bgShiftL = (bgShiftL & 0xFF00) | bgL;
	bgShiftH = (bgShiftH & 0xFF00) | bgH;
	atLatchH = (at & 1);
	atLatchH = (at & 2);
}

/* Clear Secondary OAM */
void PPU_ClearOam()
{
	for (int i = 0; i < 8; i++)
	{
        secOam[i].id    = 64;
        secOam[i].y     = 0xFF;
        secOam[i].tile  = 0xFF;
        secOam[i].attr  = 0xFF;
        secOam[i].x     = 0xFF;
        secOam[i].dataL = 0;
        secOam[i].dataH = 0;
	}
}

/* Fill secondary OAM with the sprite info for the next scanline */
void PPU_EvalSprites()
{
	int n = 0;
	for (int i = 0; i < 64; i++)
	{
		/*
		 * - Starting from -1 because sprite cannot be drawn on the first line.
		 * - Here is measures if the current scanline will across any sprite
		 * */
		int line = (scanline == 261 ? -1 : scanline) - oamMem[i*4 + 0];	// Each sprite takes 4 bytes in oamMem
		if (line >= 0 && line < PPU_SPRITE_H)
		{
            secOam[n].id   = i;
            secOam[n].y    = oamMem[i*4 + 0];
            secOam[n].tile = oamMem[i*4 + 1];
            secOam[n].attr = oamMem[i*4 + 2];
            secOam[n].x    = oamMem[i*4 + 3];

            /* Max number of sprites in a scanline is 8.
             * If more than 8 sprites are founded in one line, sprites overflow interrupt is triggered.
             * */
            if (++n > 8)
            {
            	PPUSTATUS.sprOvf = 1;
            	break;
            }
		}
	}
}

/* Load the sprite info into primary OAM and fetch their tile data */
void PPU_LoadSprites()
{
	u16 addr;
	for (int i = 0; i < 8; i++)
	{
		oam[i] = secOam[i];		// Load sprite data
		// Sprite hight setting
		if (PPU_SPRITE_H == 16)
			addr = ((oam[i].tile & 1) * 0x1000) + ((oam[i].tile & ~1) * 16); // Bit 0 determined the bank index.
		else
			addr = (PPUCTRL.sprTbl * 0x1000) + (oam[i].tile * 16);	// Each tile is 16B in pattern table.

		u8 sprY = (scanline - oam[i].y) % PPU_SPRITE_H;
		if (oam[i].attr & 0x80)
			sprY ^= PPU_SPRITE_H - 1;	// [?] Why veritical flip can be achieved in this way?
		addr += sprY + (sprY & 8);		// Check if the addr is on the second part of the tile. Add the offset if it is
		oam[i].dataL = PPU_MemRead(addr + 0);
		oam[i].dataH = PPU_MemRead(addr + 8);
	}
}

/* Process a pixel, draw it if it's on screen */
void PPU_UpdatePixels()
{
	u8 palette = 0;
	u8 objPalette = 0;
	u8 objPriority = 0;
	int x = dot - 2;	// [?] Why need to decrement by 2?

	if (scanline < 240 && x >= 0 && x < 256)
	{
		// Background
		if(PPUMASK.bg && !(!PPUMASK.bgLeft && x < 8))
		{
			// Background:
			palette = (NTH_BIT(bgShiftH, 15 - PPUINTER.fX) << 1) |
					   NTH_BIT(bgShiftL, 15 - PPUINTER.fX);
			if(palette)
				palette |= ((NTH_BIT(atShiftH, 7 - PPUINTER.fX) << 1) |
							NTH_BIT(atShiftL, 7 - PPUINTER.fX)) << 2;
		}

		// Sprites
		if (PPUMASK.spr && !(!PPUMASK.sprLeft && x < 8))
		{
			for(int i = 7; i >= 0; i++)	// [?] Why start from i = 7
			{
				if (oam[i].id == 64)
					continue;
				u8 sprX = x - oam[i].x;
				if (sprX >= 8)
					continue;
				if (oam[i].attr & 0x40)
					sprX ^= 7;	// Horizontal flip
				u8 sprPalette = (NTH_BIT(oam[i].dataH, 7 - sprX) << 1) |
								 NTH_BIT(oam[i].dataL, 7 - sprX);
				if (sprPalette == 0)
					continue;
				if (oam[i].id == 0 && palette && x != 255)	// check palette is not transparent && hit the first pixel.
					PPUSTATUS.sprHit = 1;
				sprPalette |= (oam[i].attr & 3) << 2;	// Add color to sprite pixel
				objPalette = sprPalette + 0x10;			// [?] Why add 16 to sprPalette
				objPriority = oam[i].attr & 0x20;
			}
		}

		// Evaluate Priority
		if (objPalette && (palette == 0 || objPriority == 0))
			palette = objPalette;
		GRAM[(x + 32)*240 + 239 - scanline] = palette % 256;	// load the CLUP index
	}
	// Perform background shifts;
	bgShiftL <<= 1; bgShiftH <<=1;
	atShiftL = (atShiftL << 1) | (atLatchL & 0x01);
	atShiftH = (atShiftH << 1) | (atLatchH & 0x01);
}

void PPU_TickScanline(Scanline type)
{
	static u16 addr;

	if (type == NMI && dot == 1)
	{
		PPUSTATUS.vBlank = 1;
		if (PPUCTRL.nmi)
		{
			cpu_setNMI(1);
		}
	}
	else if (type == POST && dot == 0)
	{
		// [!] Bascially we do nothing on our platform
		//int updateGui = 0;
	}
	else if (type == VISIBLE || type == PRE)
	{
		// Sprites
		switch (dot)
		{
			case 1:
				PPU_ClearOam();
				if (type == PRE)
					PPUSTATUS.sprOvf = PPUSTATUS.sprHit = 0;
				break;
			case 257:
				PPU_EvalSprites();
				break;
			case 321:
				PPU_LoadSprites();
		}
		// Background
		switch (dot)
		{
			case 2 ... 255:
			case 322 ... 337:
				PPU_UpdatePixels();
				switch (dot % 8)
				{
					// Nametable
					case 1:
						addr = PPU_GetNtAddr();
						PPU_ReloadShift();
						break;
					case 2:
						nt = PPU_MemRead(addr);
						break;
					// Attribute table
					case 3:
						addr = PPU_GetAtAddr();
						break;
					case 4:
						at = PPU_MemRead(addr);
						if (PPUINTER.vAddr.cY & 2) at >>= 4;
						if (PPUINTER.vAddr.cX & 2) at >>= 2;
						break;
					case 5:
						addr = PPU_GetBgAddr();
						break;
					case 6:
						bgL = PPU_MemRead(addr);
						break;
					case 7:
						addr += 8;
					case 0:
						bgH = PPU_MemRead(addr);
						PPU_HScroll();
						break;
				}
				break;
			case 256:
				PPU_UpdatePixels();
				bgH = PPU_MemRead(addr);
				PPU_VScroll();
				break;
			case 257:
				PPU_UpdatePixels();
				PPU_ReloadShift();
				PPU_HUpdate();
				break;
			case 280 ... 304:
				if (type == PRE)
					PPU_VUpdate();
				break;

			// No shift reloading
			case 1:
				addr = PPU_GetNtAddr();
				if (type == PRE)
					PPUSTATUS.vBlank = 0;
				break;
			case 321:
			case 339:
				addr = PPU_GetNtAddr();
				break;
			case 338:
				nt = PPU_MemRead(addr);
				break;
			case 340:
				nt = PPU_MemRead(addr);
				if (type == PRE && PPU_RENDERING && frameOdd)
					dot++;
		}
		if (dot == 260 && PPU_RENDERING)
		{
			// [!] Signal scanline to cartridge
			//int cartrige_int = 0;
		}
	}

}

/* Execute a PPU cycle */
void PPU_Tick()
{
	switch(scanline)
	{
	case 0 ... 239: break;
	case 	   240: break;
	case       241: break;
	case       261: break;
	}

	if (++dot > 340)
	{
		dot %= 341;
		if (++scanline > 261)
		{
			scanline = 0;
			frameOdd = (frameOdd & 0x01) ^ 1;
		}
	}
}

void PPU_Reset()
{
	frameOdd = 0x00;
	scanline = dot = 0;
	PPUCTRL.r = PPUMASK.r = PPUSTATUS.r = 0;
	memset(GRAM, 0x00, LCD_WIDTH * LCD_HEIGHT);
	memset(ciRam, 0xFF, sizeof(ciRam));
	memset(oamMem, 0x00, sizeof(oamMem));
}


