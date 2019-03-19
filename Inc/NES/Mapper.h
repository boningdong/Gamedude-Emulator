/*
 * Mapper.h
 *
 *  Created on: Mar 16, 2019
 *      Author: boning
 */

#ifndef NES_MAPPER_H_
#define NES_MAPPER_H_

void Mapper_Initialize(u8 *romAddr, u32 _prgSize, u32 _chrSize, u32 _prgramSize);
void Mapper_LoadMapping();
void Mapper_MapPrg(u8 slot, u8 bank, u16 pageKBs);
void Mapper_MapChr(u8 slot, u8 bank, u16 pageKBs);
u8 Mapper_Read(u16 addr);
u8 Mapper_Write(u16 addr, u8 v);
u8 Mapper_ReadChr(u16 addr);
u8 Mapper_WriteChr(u16 addr, u8 v);

#endif /* NES_MAPPER_H_ */
