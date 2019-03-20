#ifndef MAPPER_H_
#define MAPPER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "fatfs.h"
#include "typedef.h"

#define PRG_DATA_OFFSET 0x8000
#define PRG_RAM_OFFSET 0x6000
#define PRG_SLOT_SIZE 0x2000
#define CHR_SLOT_SIZE 0x400

u8 mapper_rd(u16 addr);
void mapper_wr(u16 addr, u8 data);
int loadRomFromSD(FATFS* fs, char* filename);
void mapPrg(u8 bank);
void mapChr(u8 bank);
void reset();

#endif // MAPPER_H_
