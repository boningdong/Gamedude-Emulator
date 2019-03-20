#include "NES/mapper.h"
#include "NES/ppu.h"
#include "NES/mappers/mapper0.h"

static u8 *rom, *prg, *chr, *prgRam = NULL;
static u32 prgMap[4], chrMap[8];
static int prgSize, chrSize, prgRamSize;
static bool hasChrRam, hasPrgRam, vram = false;
static u8 mapper;

u8 mapper_rd(u16 addr) {
	if (addr >= 0x8000) {
		int slot = (addr - PRG_DATA_OFFSET) / PRG_SLOT_SIZE;
		int offset = (addr - PRG_DATA_OFFSET) % PRG_SLOT_SIZE;
		return prg[prgMap[slot] + offset];
	}
	else {
		return hasPrgRam ? prgRam[addr - PRG_RAM_OFFSET] : 0;
	}
}

void mapper_wr(u16 addr, u8 data) {
	// Use mapper's write implementation
}

u8 chr_rd(u16 addr) {
	int slot = addr  / CHR_SLOT_SIZE;
	int offset = addr % CHR_SLOT_SIZE;
	return chr[chrMap[slot] + offset];
}

void chr_wr(u16 addr, u8 data) {
	if (hasChrRam) chr[addr] = data;
}

int loadRomFromSD(FATFS* fs, char* filename) {
	// Read the ROM from SD card
	FIL romfile;
	UINT r;
	if (f_mount(fs, "", 0) != FR_OK) {
		return -1;
	}
	if (f_open(&romfile, filename, FA_READ) != FR_OK) {
		return -2;
	}
	rom = malloc(f_size(&romfile) * sizeof(u8));
	f_read(&romfile, rom, f_size(&romfile), &r);
	if (r != f_size(&romfile)) {
		return -3;
	}
	f_close(&romfile);

	/* Header - 16 bytes */
	// 4 byte magic number
	if (
		rom[0] != 'N' ||
		rom[1] != 'E' ||
		rom[2] != 'S' ||
		rom[3] != '\x1a'
		)
		return -3;
	// PRG-ROM size in 16 kb blocks
	prgSize = rom[4];
	if (prgSize <= 0) return -5;
	// CHR-ROM in 8 kb blocks
	if (rom[5] != 0) {
		chrSize = rom[5];
	}
	else {
		chrSize = 1;
		hasChrRam = true;
	}
	// Flags 6
	PPU_SetMirror(rom[6] & 0x01 ? VERTICAL : HORIZONTAL);
	// Presence of PRG RAM
	hasPrgRam = ((rom[6] & 0x02) >> 1) ? true : false;
	// 512 byte trainer before PRG data
	if ((rom[6] & 0x04) >> 2) return -6;
	// Ignore nametable mirroring, provide 4-screen VRAM
	vram = ((rom[6] & 0x08) >> 3) ? true : false;
	// Mapper lower nybble
	mapper = rom[6] >> 4;
	// Flags 7
	// Mapper upper nybble
	mapper |= (rom[7] & 0xF0);
	// Flags 8
	// PRG RAM size
	prgRamSize = (rom[8] != 0) ? rom[8] : 1;
	// Flags 9
	// NTSC or PAL
	if (rom[9] != 0) return -7;
	// Flags 10-15

	// Set PRG offset
	prg = rom + 0x10;
	// Set CHR offset
	chr = rom + 0x10 + prgSize * 0x4000;

	// Allocate PRG RAM
	if (hasPrgRam) prgRam = malloc(prgRamSize * 0x2000 * sizeof(u8));

	switch (mapper) {
		case 0:
			// Temporary initialization for mapper 0
			mapper0_init(prgMap, chrMap, prgSize);
			break;
		default:
			printf("Mapper not supported!\n");
			return -8;
	}
	return 0;
}

void reset() {
	free(rom);
	free(prgRam);
}
