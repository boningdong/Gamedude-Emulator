#include "NES/mappers/mapper0.h"

void mapper0_init(u32* prgMap, u32* chrMap, int prgSize) {
	// Perform 1:1 mapping for prg and chr
	for (int i = 0; i < 4; i++) {
		prgMap[i] = (0x2000 * i) % (prgSize * 0x4000);
	}
	for (int i = 0; i < 8; i++) {
		chrMap[i] = 0x400 * i;
	}
}
