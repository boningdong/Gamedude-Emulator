#include "NES/controller.h"

extern SPI_HandleTypeDef hspi4;
extern SPI_HandleTypeDef hspi5;
static u8 p1data, p2data;

u8 controller_rd(u8 controller) {
	u8 data = 0;
	// P1
	if (controller == 0) {
		// 7th bit is always 1, retrieve current button state
		data = 0x40 | (p1data & 1);
		// Shift the current button out and shift in a 1
		p1data >>= 1;
		p1data |= 0x80;
	}
	// P2
	else {
		// 7th bit is always 1, retrieve current button state
		data = 0x40 | (p2data & 1);
		// Shift the current button out and shift in a 1
		p2data >>= 1;
		p2data |= 0x80;
	}
	return data;
}

void controller_wr(u8 data) {
	if (data & 1) {
		HAL_GPIO_WritePin(P1_SPI_SS_GPIO_Port, P1_SPI_SS_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(P2_SPI_SS_GPIO_Port, P2_SPI_SS_Pin, GPIO_PIN_SET);
	}
	else {
		HAL_GPIO_WritePin(P1_SPI_SS_GPIO_Port, P1_SPI_SS_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(P2_SPI_SS_GPIO_Port, P2_SPI_SS_Pin, GPIO_PIN_RESET);
		HAL_SPI_Receive(&hspi4, &p1data, 1, 50);
		HAL_SPI_Receive(&hspi5, &p2data, 1, 50);
		// Swap the module bits due to a hardware error
		p1data = (p1data >> 4) | (p1data << 4);
		p2data = (p2data >> 4) | (p2data << 4);
	}
}
