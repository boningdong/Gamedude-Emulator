/*
 * lcd.c
 *
 *  Created on: Mar 4, 2019
 *      Author: Boning
 */

#include "spi.h"
#include "main.h"
#include "lcd.h"
#include "ltdc.h"
#include "clut.h"
#include "ili9341.h"

extern SPI_HandleTypeDef hspi1;
extern LTDC_HandleTypeDef hltdc;

uint8_t GRAM[LCD_WIDTH * LCD_HEIGHT] = {0};

void LCD_GpioInit() {
	MX_SPI1_Init();
	// Enable Port Clock
	__HAL_RCC_GPIOG_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	// Initialize NCS and DCX Port
	GPIO_InitTypeDef Gpio_InitStruct;
	Gpio_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	Gpio_InitStruct.Pull = GPIO_PULLUP;
	Gpio_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	Gpio_InitStruct.Pin = LCD_NCS_PIN;
	HAL_GPIO_Init(LCD_NCS_GPIO, &Gpio_InitStruct);
	Gpio_InitStruct.Pin = LCD_DCX_PIN;
	HAL_GPIO_Init(LCD_DCX_GPIO, &Gpio_InitStruct);
	// Initialize NCS and DCX value
	HAL_GPIO_WritePin(LCD_NCS_GPIO, LCD_NCS_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_DCX_GPIO, LCD_DCX_PIN, GPIO_PIN_SET);
}

void LCD_LtdcInit() {
	MX_LTDC_Init();
	HAL_LTDC_SetAddress(&hltdc, (uint32_t)GRAM, LTDC_LAYER_1);
	HAL_LTDC_ConfigCLUT(&hltdc, CLUT, 256, LTDC_LAYER_1);
	HAL_LTDC_EnableCLUT(&hltdc, LTDC_LAYER_1);
	HAL_LTDC_Reload(&hltdc, LTDC_RELOAD_VERTICAL_BLANKING);
}

void LCD_DispInit_Spi() {
	//SOFTWARE RESET
	LCD_WriteCommand(0x01);
	HAL_Delay(1000);

	//POWER CONTROL A
	LCD_WriteCommand(0xCB);
	LCD_WriteData(0x39);
	LCD_WriteData(0x2C);
	LCD_WriteData(0x00);
	LCD_WriteData(0x34);
	LCD_WriteData(0x02);

	//POWER CONTROL B
	LCD_WriteCommand(0xCF);
	LCD_WriteData(0x00);
	LCD_WriteData(0xC1);
	LCD_WriteData(0x30);

	//DRIVER TIMING CONTROL A
	LCD_WriteCommand(0xE8);
	LCD_WriteData(0x85);
	LCD_WriteData(0x00);
	LCD_WriteData(0x78);

	//DRIVER TIMING CONTROL B
	LCD_WriteCommand(0xEA);
	LCD_WriteData(0x00);
	LCD_WriteData(0x00);

	//POWER ON SEQUENCE CONTROL
	LCD_WriteCommand(0xED);
	LCD_WriteData(0x64);
	LCD_WriteData(0x03);
	LCD_WriteData(0x12);
	LCD_WriteData(0x81);

	//PUMP RATIO CONTROL
	LCD_WriteCommand(0xF7);
	LCD_WriteData(0x20);

	//POWER CONTROL,VRH[5:0]
	LCD_WriteCommand(0xC0);
	LCD_WriteData(0x23);

	//POWER CONTROL,SAP[2:0];BT[3:0]
	LCD_WriteCommand(0xC1);
	LCD_WriteData(0x10);

	//VCM CONTROL
	LCD_WriteCommand(0xC5);
	LCD_WriteData(0x3E);
	LCD_WriteData(0x28);

	//VCM CONTROL 2
	LCD_WriteCommand(0xC7);
	LCD_WriteData(0x86);

	//MEMORY ACCESS CONTROL
	LCD_WriteCommand(0x36);
	LCD_WriteData(0x48);

	//PIXEL FORMAT
	LCD_WriteCommand(0x3A);
	LCD_WriteData(0x55);

	//FRAME RATIO CONTROL, STANDARD RGB COLOR
	LCD_WriteCommand(0xB1);
	LCD_WriteData(0x00);
	LCD_WriteData(0x18);

	//DISPLAY FUNCTION CONTROL
	LCD_WriteCommand(0xB6);
	LCD_WriteData(0x08);
	LCD_WriteData(0x82);
	LCD_WriteData(0x27);

	//3GAMMA FUNCTION DISABLE
	LCD_WriteCommand(0xF2);
	LCD_WriteData(0x00);

	//GAMMA CURVE SELECTED
	LCD_WriteCommand(0x26);
	LCD_WriteData(0x01);

	//POSITIVE GAMMA CORRECTION
	LCD_WriteCommand(0xE0);
	LCD_WriteData(0x0F);
	LCD_WriteData(0x31);
	LCD_WriteData(0x2B);
	LCD_WriteData(0x0C);
	LCD_WriteData(0x0E);
	LCD_WriteData(0x08);
	LCD_WriteData(0x4E);
	LCD_WriteData(0xF1);
	LCD_WriteData(0x37);
	LCD_WriteData(0x07);
	LCD_WriteData(0x10);
	LCD_WriteData(0x03);
	LCD_WriteData(0x0E);
	LCD_WriteData(0x09);
	LCD_WriteData(0x00);

	//NEGATIVE GAMMA CORRECTION
	LCD_WriteCommand(0xE1);
	LCD_WriteData(0x00);
	LCD_WriteData(0x0E);
	LCD_WriteData(0x14);
	LCD_WriteData(0x03);
	LCD_WriteData(0x11);
	LCD_WriteData(0x07);
	LCD_WriteData(0x31);
	LCD_WriteData(0xC1);
	LCD_WriteData(0x48);
	LCD_WriteData(0x08);
	LCD_WriteData(0x0F);
	LCD_WriteData(0x0C);
	LCD_WriteData(0x31);
	LCD_WriteData(0x36);
	LCD_WriteData(0x0F);

	//EXIT SLEEP
	LCD_WriteCommand(0x11);
	HAL_Delay(120);

	//TURN ON DISPLAY
	LCD_WriteCommand(0x29);
}

void LCD_DispInit_Ltdc() {
	LCD_WriteCommand(0xCA);
	LCD_WriteData(0xC3);
	LCD_WriteData(0x08);
	LCD_WriteData(0x50);

	// LCD_POWERB
	LCD_WriteCommand(LCD_POWERB);
	LCD_WriteData(0x00);
	LCD_WriteData(0xC1);
	LCD_WriteData(0x30);

	// LCD_POWER_SEQ
	LCD_WriteCommand(LCD_POWER_SEQ);
	LCD_WriteData(0x64);
	LCD_WriteData(0x03);
	LCD_WriteData(0x12);
	LCD_WriteData(0x81);

	// LCD_DTCA
	LCD_WriteCommand(LCD_DTCA);
	LCD_WriteData(0x85);
	LCD_WriteData(0x00);
	LCD_WriteData(0x78);

	// LCD_POWERA
	LCD_WriteCommand(LCD_POWERA);
	LCD_WriteData(0x39);
	LCD_WriteData(0x2C);
	LCD_WriteData(0x00);
	LCD_WriteData(0x34);
	LCD_WriteData(0x02);

	// LCD_RPC
	LCD_WriteCommand(LCD_PRC);
	LCD_WriteData(0x20);

	// LCD_DTCB
	LCD_WriteCommand(LCD_DTCB);
	LCD_WriteData(0x00);
	LCD_WriteData(0x00);

	// LCD_FRMCTR1
	LCD_WriteCommand(LCD_FRMCTR1);
	LCD_WriteData(0x00);
	LCD_WriteData(0x1B);

	// LCD_DFC
	LCD_WriteCommand(LCD_DFC);
	LCD_WriteData(0x0A);
	LCD_WriteData(0xA2);

	// LCD_POWER1
	LCD_WriteCommand(LCD_POWER1);
	LCD_WriteData(0x10);
	LCD_WriteCommand(LCD_POWER2);
	LCD_WriteData(0x10);

	// LCD_VCOM
	LCD_WriteCommand(LCD_VCOM1);
	LCD_WriteData(0x45);
	LCD_WriteData(0x15);
	LCD_WriteCommand(LCD_VCOM2);
	LCD_WriteData(0x90);

	// LCD_MAC
	LCD_WriteCommand(LCD_MAC);
	LCD_WriteData(0xC8);

	// LCD_GAMMA
	LCD_WriteCommand(LCD_3GAMMA_EN);
	LCD_WriteData(0x00);

	// LCD_RGB_INTERFACE
	LCD_WriteCommand(LCD_RGB_INTERFACE);
	LCD_WriteData(0xC2);

	// LCD_DFC
	LCD_WriteCommand(LCD_DFC);
	LCD_WriteData(0x0A);
	LCD_WriteData(0xA7);
	LCD_WriteData(0x27);
	LCD_WriteData(0x04);

	// COLUMN_ADDR
	LCD_WriteCommand(LCD_COLUMN_ADDR);
	LCD_WriteData(0x00);
	LCD_WriteData(0x00);
	LCD_WriteData(0x00);
	LCD_WriteData(0xEF);

	// PAGE_ADDR
	LCD_WriteCommand(LCD_PAGE_ADDR);
	LCD_WriteData(0x00);
	LCD_WriteData(0x00);
	LCD_WriteData(0x01);
	LCD_WriteData(0x3F);

	// INERFACE
	LCD_WriteCommand(LCD_INTERFACE);
	LCD_WriteData(0x01);
	LCD_WriteData(0x00);
	LCD_WriteData(0x06);

	// LCD_GRAM
	LCD_WriteCommand(LCD_GRAM);
	HAL_Delay(200);

	// LCD_GAMMA
	LCD_WriteCommand(LCD_GAMMA);
	LCD_WriteData(0x01);

	// LCD_PGAMMA
	LCD_WriteCommand(LCD_PGAMMA);
	LCD_WriteData(0x0F);
	LCD_WriteData(0x29);
	LCD_WriteData(0x24);
	LCD_WriteData(0x0C);
	LCD_WriteData(0x0E);
	LCD_WriteData(0x09);
	LCD_WriteData(0x4E);
	LCD_WriteData(0x78);
	LCD_WriteData(0x3C);
	LCD_WriteData(0x09);
	LCD_WriteData(0x13);
	LCD_WriteData(0x05);
	LCD_WriteData(0x17);
	LCD_WriteData(0x11);
	LCD_WriteData(0x00);
	LCD_WriteCommand(LCD_NGAMMA);
	LCD_WriteData(0x00);
	LCD_WriteData(0x16);
	LCD_WriteData(0x1B);
	LCD_WriteData(0x04);
	LCD_WriteData(0x11);
	LCD_WriteData(0x07);
	LCD_WriteData(0x31);
	LCD_WriteData(0x33);
	LCD_WriteData(0x42);
	LCD_WriteData(0x05);
	LCD_WriteData(0x0C);
	LCD_WriteData(0x0A);
	LCD_WriteData(0x28);
	LCD_WriteData(0x2F);
	LCD_WriteData(0x0F);

	// LCD_SLEEP_OUT
	LCD_WriteCommand(LCD_SLEEP_OUT);
	HAL_Delay(200);

	// LCD_DISP_ON
	LCD_WriteCommand(LCD_DISPLAY_ON);

	// LCD_WRITE_RAM
	LCD_WriteCommand(LCD_GRAM);
}

void LCD_ChipSelect(CsMode state){
	HAL_GPIO_WritePin(LCD_NCS_GPIO, LCD_NCS_PIN, state);
}

void LCD_ModeSelect(DcMode mode){
	HAL_GPIO_WritePin(LCD_DCX_GPIO, LCD_DCX_PIN, mode);
}

void LCD_WriteCommand(uint8_t data) {
	HAL_GPIO_WritePin(LCD_NCS_GPIO, LCD_NCS_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_DCX_GPIO, LCD_DCX_PIN, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, &data, 1, 1);
	while(!((hspi1.Instance->SR) & SPI_SR_TXE));
	HAL_GPIO_WritePin(LCD_NCS_GPIO, LCD_NCS_PIN, GPIO_PIN_SET);
}

void LCD_WriteData(uint8_t data) {
	HAL_GPIO_WritePin(LCD_NCS_GPIO, LCD_NCS_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_DCX_GPIO, LCD_DCX_PIN, GPIO_PIN_SET);
	HAL_SPI_Transmit(&hspi1, &data, 1, 10);
	while(!((hspi1.Instance->SR) & SPI_SR_TXE));
	HAL_GPIO_WritePin(LCD_NCS_GPIO, LCD_NCS_PIN, GPIO_PIN_SET);
}

void LCD_WriteBulkData(uint8_t *data, uint8_t size) {
	HAL_GPIO_WritePin(LCD_NCS_GPIO, LCD_NCS_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_DCX_GPIO, LCD_DCX_PIN, GPIO_PIN_SET);
	HAL_SPI_Transmit(&hspi1, data, 2, 10);
	while(!((hspi1.Instance->SR) & SPI_SR_TXE));
	HAL_GPIO_WritePin(LCD_NCS_GPIO, LCD_NCS_PIN, GPIO_PIN_SET);
}

void LCD_WriteGRAM(uint8_t data, uint32_t addr) {
	if (addr >= LCD_WIDTH * LCD_HEIGHT)
		return;
	GRAM[addr] = data;
}
