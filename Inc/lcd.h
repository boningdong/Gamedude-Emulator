/*
 * lcd.h
 *
 *  Created on: Mar 4, 2019
 *      Author: Boning
 */

#ifndef LCD_H_
#define LCD_H_

#define LCD_WIDTH 240
#define LCD_HEIGHT 320

#define LCD_NCS_GPIO GPIOG
#define LCD_DCX_GPIO GPIOD
#define LCD_NCS_PIN GPIO_PIN_3
#define LCD_DCX_PIN GPIO_PIN_13

typedef enum {
	SELECT,
	DESELECT
} CsMode;

typedef enum {
	COMMAND,
	DATA
} DcMode;

void LCD_GpioInit();
void LCD_LtdcInit();
void LCD_DispInit_Spi();
void LCD_DispInit_Ltdc();
void LCD_ChipSelect(CsMode state);
void LCD_ModeSelect(DcMode mode);
void LCD_WriteCommand(uint8_t data);
void LCD_WriteData(uint8_t data);
void LCD_WriteBulkData(uint8_t *data, uint8_t size);
void LCD_WriteGRAM(uint8_t data, uint32_t addr);

#endif /* LCD_H_ */
