/*
 * graphic.c
 *
 *  Created on: Mar 4, 2019
 *      Author: Boning
 */

#include "main.h"
#include "lcd.h"

uint16_t COLOR = 0x5566;
uint8_t COLOR_LTDC = 0x00;

void LCD_SetDrawAddr(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
	// Set Column Address
	LCD_WriteCommand(0x2A);
	LCD_WriteData( (x0 >> 8) & 0xFF);
	LCD_WriteData( x0 & 0xFF);
	LCD_WriteData((x1 >> 8) & 0xFF);
	LCD_WriteData(x1 & 0xFF);
	// Set Line Address
	LCD_WriteCommand(0x2B);
	LCD_WriteData((y0 >> 8) & 0xFF);
	LCD_WriteData(y0 & 0xFF);
	LCD_WriteData((y1 >> 8) & 0xFF);
	LCD_WriteData(y1 & 0xFF);
	// Memory Write
	LCD_WriteCommand(0x2c);
}

void LCD_SetColor(uint16_t c) {
	COLOR = c;
}

void LCD_SetColorLtdc(uint8_t c) {
	COLOR_LTDC = c;
}

void LCD_DrawRect_Spi(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
	LCD_SetDrawAddr(x0, y0, x1, y1);
	uint8_t buffer[] = {COLOR >> 8, COLOR};
	for(int y = y0; y <= y1; y++){
		for(int x = x0; x <= x1; x++) {
			LCD_WriteBulkData(buffer, 2);
		}
	}
}

void LCD_DrawRect_Ltdc(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
	for(int y = y0; y <= y1; y++){
		for(int x = x0; x <= x1; x++) {
			LCD_WriteGRAM(COLOR_LTDC, y * LCD_WIDTH + x);
		}
	}
}
