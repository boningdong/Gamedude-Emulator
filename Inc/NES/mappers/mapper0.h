#ifndef MAPPER0_H_
#define MAPPER0_H_

#include <stdio.h>
#include <stdint.h>

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

void mapper0_init(u32* prgMap, u32* chrMap, int prgSize);

#endif // MAPPER0_H_
