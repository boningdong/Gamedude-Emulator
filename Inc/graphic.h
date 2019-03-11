/*
 * graphic.h
 *
 *  Created on: Mar 4, 2019
 *      Author: Boning
 */

#ifndef GRAPHIC_H_
#define GRAPHIC_H_

void LCD_SetDrawAddr(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void LCD_SetColor(uint16_t c);
void LCD_SetColorLtdc(uint8_t c);
void LCD_DrawRect_Spi(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void LCD_DrawRect_Ltdc(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);


#endif /* GRAPHIC_H_ */
