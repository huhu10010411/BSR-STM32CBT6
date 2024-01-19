/*
 * Serial_log.c
 *
 *  Created on: Dec 12, 2023
 *      Author: Admin
 */

#include "Serial_log.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define LOG_TIMEOUT		1000

static UART_HandleTypeDef *__SERIAL_LOG_UART;

void init_Serial_log (UART_HandleTypeDef *huart)
{
	__SERIAL_LOG_UART = huart;
}

void Serial_log_string(char *string)
{
	HAL_UART_Transmit(__SERIAL_LOG_UART, (uint8_t*)string, strlen(string), LOG_TIMEOUT);
}

void Serial_log_stringln(char *string)
{
	uint16_t stlen = strlen(string);
	uint8_t *tmpbuff = (uint8_t*)malloc(stlen+2);
	uint16_t len = sprintf( (char*)tmpbuff, "%s\n", string);
	HAL_UART_Transmit(__SERIAL_LOG_UART, tmpbuff, len, LOG_TIMEOUT);
	free(tmpbuff);
}

void Serial_log_buffer(uint8_t *buffer, uint16_t buffersize)
{
	HAL_UART_Transmit(__SERIAL_LOG_UART, buffer, buffersize, LOG_TIMEOUT);
}

void Serial_log_number(uint16_t number)
{
	uint8_t tmpbuffer[10];
	uint8_t len = sprintf( (char*)tmpbuffer, "%d", number);
//	HAL_UART_Transmit_DMA(__SERIAL_LOG_UART, tmpbuffer, len);
	HAL_UART_Transmit(__SERIAL_LOG_UART, tmpbuffer, len, LOG_TIMEOUT);
}

void Serial_log_testOperation (void)
{
	Serial_log_string("This is test Serial log string function\r\n");

	Serial_log_number(2001);
}
