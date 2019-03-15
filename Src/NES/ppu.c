/*
 * ppu.c
 *
 *  Created on: Mar 14, 2019
 *      Author: boning
 */
#include "main.h"
#include "NES/ppu.h"
#include "lcd.h"




/* Registers Declaration
 * Register files that are defined in ppu.h
 * - PPUCTRL					$2000
 * - PPUMASK					$2001
 * - PPUSATUS					$2002
 * - PPUADDR | PPUSCROLL		$2005 | $2006
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
Sprite oam[8], secOam[8]; 	// [?] why need two?

/* Graphic Memory (LCD) */
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
		case 0x0000 ... 0x1FFF: return 0x00;
		case 0x2000 ... 0x3EFF: return 0x00;
		case 0x3F00 ... 0x3FFF: return 0x00;
		default: return 0x00;
	}
}

void PPU_MemWrite(u16 addr, u8 val)
{
	switch (addr)
	{
		case 0x0000 ... 0x1FFF: break;
		case 0x2000 ... 0x3EFF: break;
		case 0x3F00 ... 0x3FFF: break;
	}
}

/* PPU Registers Access */
u8 PPU_RegAccess (u16 index, u8 val, u8 rw)
{

}








