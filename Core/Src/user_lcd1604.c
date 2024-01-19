/**
  * @file    user_lcd1604.c
  * @author  Sagit
  * @date    Dec 10, 2023
  * @brief   
  */

/* INCLUDES ------------------------------------------------------------------*/
#include "user_lcd1604.h"
#include <stdio.h>
#include <string.h>

/* EXTERN VARIABLES ----------------------------------------------------------*/
StructManageLCD1604		myLCD	=
{
		.D4		= LCD_DB4_Pin,
		.D4P	= LCD_DB4_GPIO_Port,
		.D5		= LCD_DB5_Pin,
		.D5P    = LCD_DB5_GPIO_Port,
		.D6		= LCD_DB6_Pin,
		.D6P	= LCD_DB6_GPIO_Port,
		.D7		= LCD_DB7_Pin,
		.D7P	= LCD_DB7_GPIO_Port,
		.EN		= LCD_EN_Pin,
		.ENP	= LCD_EN_GPIO_Port,
		.RS		= LCD_RS_Pin,
		.RSP	= LCD_RS_GPIO_Port,
};

/* PRIVATE MACROS AND DEFINES ------------------------------------------------*/

/* PRIVATE TYPEDEFS ----------------------------------------------------------*/

/* STATIC VARIABLES ----------------------------------------------------------*/

/* GLOBAL VARIABLES ----------------------------------------------------------*/

/* STATIC FUNCTION PROTOTYPES --- --------------------------------------------*/
static void LCD_SpecialChar(void)
{
	uint8_t Right[] = {0x00, 0x04, 0x06, 0x1F, 0x1F, 0x06, 0x04, 0x00}; // Go to right
	LCD_SendCommand(0x40);
	for (int i=0; i<8; i++) LCD_SendData(Right[i]);
	uint8_t RightInvert[] = {0x1F, 0x1B, 0x19, 0x00, 0x00, 0x19, 0x1B, 0x1F};
	LCD_SendCommand(0x40+8);
	for (int i=0; i<8; i++) LCD_SendData(RightInvert[i]);
	uint8_t Left[] = {0x00, 0x04, 0x0C, 0x1F, 0x1F, 0x0C, 0x04, 0x00};
	LCD_SendCommand(0x40+16);
	for (int i=0; i<8; i++) LCD_SendData(Left[i]);
	uint8_t LeftInvert[] = {0x1F, 0x1B, 0x13, 0x00, 0x00, 0x13, 0x1B, 0x1F};
	LCD_SendCommand(0x40+24);
	for (int i=0; i<8; i++) LCD_SendData(LeftInvert[i]);
	uint8_t Empty[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	LCD_SendCommand(0x40+32);
	for (int i=0; i<8; i++) LCD_SendData(Empty[i]);
	uint8_t Full[] = {0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F};
	LCD_SendCommand(0x40+40);
	for (int i=0; i<8; i++) LCD_SendData(Full[i]);
	uint8_t Up[] = {0x00, 0x00, 0x04, 0x0E, 0x1F, 0x00, 0x00, 0x00};
	LCD_SendCommand(0x40+48);
	for (int i=0; i<8; i++) LCD_SendData(Up[i]);
	uint8_t Down[] = {0x00, 0x00, 0x00, 0x1F, 0x0E, 0x04, 0x00, 0x00};
	LCD_SendCommand(0x40+56);
	for (int i=0; i<8; i++) LCD_SendData(Down[i]);
}
/* STATIC FUNCTIONS ----------------------------------------------------------*/

/* GLOBAL FUNCTIONS ----------------------------------------------------------*/
void LCD_Init(void){
	  HAL_Delay(50);
	  LCD_SendCommand(0x03);
	  HAL_Delay(5);
	  LCD_SendCommand(0x03);
	  HAL_Delay(1);
	  LCD_SendCommand(0x03);
	  HAL_Delay(1);
	  LCD_SendCommand(0x02);
	  HAL_Delay(1);

	  LCD_SendCommand(0x28);
	  HAL_Delay(1);

	  LCD_SendCommand(0x0C);
	  HAL_Delay(1);

	  LCD_SendCommand(0x01);
	  HAL_Delay(2);

	  LCD_SendCommand(0x06);
	  HAL_Delay(1);
	  LCD_SpecialChar();
}

void LCD_SendCommand(uint8_t cmd){
	  // Send high nibble
	  HAL_GPIO_WritePin(myLCD.RSP, myLCD.RS, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(myLCD.D4P, myLCD.D4, (cmd & 0x10) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(myLCD.D5P, myLCD.D5, (cmd & 0x20) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(myLCD.D6P, myLCD.D6, (cmd & 0x40) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(myLCD.D7P, myLCD.D7, (cmd & 0x80) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(myLCD.ENP, myLCD.EN, GPIO_PIN_SET);
	  HAL_Delay(1);
	  HAL_GPIO_WritePin(myLCD.ENP, myLCD.EN, GPIO_PIN_RESET);

	  // Send low nibble
	  HAL_GPIO_WritePin(myLCD.D4P, myLCD.D4, (cmd & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(myLCD.D5P, myLCD.D5, (cmd & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(myLCD.D6P, myLCD.D6, (cmd & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(myLCD.D7P, myLCD.D7, (cmd & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(myLCD.ENP, myLCD.EN, GPIO_PIN_SET);
	  HAL_Delay(1);
	  HAL_GPIO_WritePin(myLCD.ENP, myLCD.EN, GPIO_PIN_RESET);
}

void LCD_SendData(uint8_t data){
	  // Send high nibble
	  HAL_GPIO_WritePin(myLCD.RSP, myLCD.RS, GPIO_PIN_SET);
	  HAL_GPIO_WritePin(myLCD.D4P, myLCD.D4, (data & 0x10) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(myLCD.D5P, myLCD.D5, (data & 0x20) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(myLCD.D6P, myLCD.D6, (data & 0x40) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(myLCD.D7P, myLCD.D7, (data & 0x80) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(myLCD.ENP, myLCD.EN, GPIO_PIN_SET);
	  HAL_Delay(1);
	  HAL_GPIO_WritePin(myLCD.ENP, myLCD.EN, GPIO_PIN_RESET);

	  // Send low nibble
	  HAL_GPIO_WritePin(myLCD.D4P, myLCD.D4, (data & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(myLCD.D5P, myLCD.D5, (data & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(myLCD.D6P, myLCD.D6, (data & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(myLCD.D7P, myLCD.D7, (data & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(myLCD.ENP, myLCD.EN, GPIO_PIN_SET);
	  HAL_Delay(1);
	  HAL_GPIO_WritePin(myLCD.ENP, myLCD.EN, GPIO_PIN_RESET);
}

void LCD_GotoXY(uint8_t x, uint8_t y){
    uint8_t address;
    if (y == 0) {
        address = 0x80 + x;
    } else if (y == 1) {
        address = 0xC0 + x;
    } else if (y == 2){
        address = 0x80 + x + 16;
    } else if (y == 3){
        address = 0xC0 + x + 16;
    }
    LCD_SendCommand(address);
}

void LCD_Print(char *str){
    while (*str) {
        LCD_SendData(*str++);
    }
}

void LCD_PrintNumber(uint16_t number)
{
	uint8_t tmpbuffer[10];
	memset(tmpbuffer, 0, 10);
	sprintf((char*)tmpbuffer, "%d", number);
	LCD_Print((char*)tmpbuffer);
}

void LCD_Clear(){
	LCD_SendCommand(0x01);
}

void LCD_CallSpecialChar(uint8_t num)
{
	LCD_SendData(num);
}
/* End of file ----------------------------------------------------------------*/
