/*
 * ppu.h
 *
 *  Created on: Mar 14, 2019
 *      Author: boning
 */

#ifndef NES_PPU_H_
#define NES_PPU_H_

#include "typedef.h"

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

/* Configuration */
typedef enum mirroring {VERTICAL, HORIZONTAL} Mirroring;
typedef enum readwrite {WRITE, READ} Rw;
typedef enum scanline {VISIBLE, POST, NMI, PRE} Scanline;

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

/* Function Declaration */
void PPU_SetMirror(Mirroring mode);
u16 NT_Mirror(u16 addr);
u8 PPU_MemRead(u16 addr);
void PPU_MemWrite(u16 addr, u8 v);
u8 PPU_RegAccess (u16 index, u8 v, Rw rw);
u16 PPU_GetNtAddr();
u16 PPU_GetAtAddr();
u16 PPU_GetBgAddr();
void PPU_HScroll();
void PPU_VScroll();
void PPU_HUpdate() ;
void PPU_VUpdate();
void PPU_ReloadShift();
void PPU_ClearOam();
void PPU_EvalSprites();
void PPU_LoadSprites();
void PPU_UpdatePixels();
void PPU_TickScanline(Scanline type);
void PPU_Tick();
void PPU_Reset();

#endif /* NES_PPU_H_ */
