/*
 * Lora.h
 *
 *  Created on: Dec 24, 2023
 *      Author: Admin
 */

#ifndef INC_PROTOCOL_LORA_H_
#define INC_PROTOCOL_LORA_H_

#include "stm32f1xx.h"
#include "main.h"

#define START_POS		0
#define ID_LENGTH_POS	1
#define ID_STARTPOS		3



#define START_VALUE		1

//extern UART_HandleTypeDef *__LORA_UART;

void initLora();

void initmyLora();

void enableReceiveDMAtoIdle_Lora(void);

void Lora_callback(uint16_t Size);

uint8_t Lora_Setmode(sensor_mode_t mode, uint8_t checkres);

void Lora_receive(uint8_t *Msg, uint8_t msglen);

void MarkAsReadData_LORA(void);

void testLora_receive (void);

#endif /* INC_PROTOCOL_LORA_H_ */
