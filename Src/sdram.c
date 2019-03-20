/*
 * sdram.c
 *
 *  Created on: Mar 19, 2019
 *      Author: boning
 */

#include "sdram.h"

void SDRAM_Write(uint32_t addr, uint16_t data)
{
	*(uint8_t*)(SDRAM_BASE_ADDR +  addr * sizeof(uint8_t)) = data;
}

uint8_t SDRAM_Read(uint32_t addr)
{
	return *(uint8_t*)(SDRAM_BASE_ADDR + addr * sizeof(uint8_t));
}

uint16_t* SDRAM_CreatePointer()
{
	return (uint16_t*)SDRAM_BASE_ADDR;
}

uint16_t SDRAM_ArrayRead(uint16_t* ptr, int index)
{
	return ptr[index];
}

void SDRAM_ArrayInit(uint16_t* ptr, int v, int size) {
	for (int i = 0; i < size; i ++)
		ptr[i] = v;

}
