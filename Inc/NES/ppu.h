/*
 * ppu.h
 *
 *  Created on: Mar 14, 2019
 *      Author: boning
 */

#ifndef NES_PPU_H_
#define NES_PPU_H_

#include "typedef.h"

/* Configuration */
typedef enum mirroring {VERTICAL, HORIZONTAL} Mirroring;
typedef enum readwrite {WRITE, READ} Rw;
typedef enum scanline {VISIBLE, POST, NMI, PRE} Scanline;

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
