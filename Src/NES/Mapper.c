/*
 * Mapper.c
 *
 *  Created on: Mar 16, 2019
 *      Author: boning
 */

#include "NES/typedef.h"
#include "NES/Mapper.h"

/* Memory Area Size
 * All the size have unit in bytes
 * prgSize: size of game instructions in bytes
 * chrSize: size of pattern table in bytes
 */
u32 prgSize;
u32 chrSize;
u32 prgRamSize;		// [!] Implement later

/* Memory Offsets
 * These are the physical addresses on the cartridge memory
 * - ptrPrg: Game instruction address offset
 * - ptrChr: Pattern table address offset
 * - ptrPrgRam: [?] For saving game data
 */
u8 *romBase;
u8 *prgBase;
u8 *chrBase;
u8 *prgRamBase;

/* Memory Maps
 * - prgMap: For Game instruction mapping
 * - chrMap: For pattern table mapping
 */
u32 prgMap[4];
u32 chrMap[8];

// Set the size of CHR area stored in Cartridge
void Mapper_Initialize(u8 *romAddr, u32 _prgSize, u32 _chrSize, u32 _prgramSize)
{
	// [!] Can be replaced by parsing from romBase directly
	romBase = romAddr;
	prgSize = _prgSize;
	chrSize = _chrSize;
	prgRamSize = _prgramSize;

	// Calculate offset
	prgBase = romBase + 0x10;
	//[!] Allocate memory for prgRam
	//[!] Didn't implement RAM mapper
	chrBase = romBase + 0x10 + prgSize;
	Mapper_LoadMapping();
}

void Mapper_LoadMapping()
{
	// Mapper 0 Settings
	Mapper_MapPrg(0, 0, 32);
	Mapper_MapChr(0, 0, 8);
}

void Mapper_MapPrg(u8 slot, u8 bank, u16 pageKBs)
{
	return;
}

void Mapper_MapChr(u8 slot, u8 bank, u16 pageKBs)
{
	for (int i = 0; i < pageKBs; i++)
		chrMap[pageKBs * slot + i] = (pageKBs * 0x400 * bank + 0x400 * i) % chrSize;
}

u8 Mapper_Read(u16 addr)
{
	return 0;
}

u8 Mapper_Write(u16 addr, u8 v)
{
	// Not needed by Mapper 0
	return v;
}

u8 Mapper_ReadChr(u16 addr)
{
	return chrBase[chrMap[addr / 0x400] + (addr % 0x400)];
}

u8 Mapper_WriteChr(u16 addr, u8 v)
{
	// Not needed by Mapper 0
	return v;
}

