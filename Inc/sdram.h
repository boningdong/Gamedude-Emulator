/*
 * sdram.h
 *
 *  Created on: Mar 19, 2019
 *      Author: boning
 */

#ifndef SDRAM_H_
#define SDRAM_H_

#include "fmc.h"

#define SDRAM_BASE_ADDR 0xC0000000

void SDRAM_Write(uint32_t addr, uint16_t data);
uint8_t SDRAM_Read(uint32_t addr);
uint16_t* SDRAM_CreatePointer();
uint16_t SDRAM_ArrayRead(uint16_t* ptr, int index);
void SDRAM_ArrayInit(uint16_t* ptr, int v, int size) ;

#endif /* SDRAM_H_ */
