/*
 * Serial_log.h
 *
 *  Created on: Dec 12, 2023
 *      Author: Admin
 */

#ifndef INC_SERIAL_LOG_H_
#define INC_SERIAL_LOG_H_

#include <stm32f1xx.h>

void init_Serial_log (UART_HandleTypeDef *huart);

void Serial_log_stringln(char *string);

void Serial_log_string(char *string);

void Serial_log_buffer(uint8_t *buffer, uint16_t buffersize);

void Serial_log_number(uint16_t number);

void Serial_log_testOperation (void);
#endif /* INC_SERIAL_LOG_H_ */
