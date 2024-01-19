/*
 * Serial_CFG.h
 *
 *  Created on: Oct 2, 2023
 *      Author: Admin
 */

#ifndef INC_SERIAL_CFG_H_
#define INC_SERIAL_CFG_H_

//#include "main.h"
#include <stm32f1xx.h>

#include <string.h>

#include "ds3231.h"
#include "main.h"


#define MY_TIME_ZONE	7

#define GPS_RXBUFF_MAXLEN	512

extern uint8_t volatile isSerialConfig;
extern UART_HandleTypeDef *__SCFG_UART;

extern uint8_t GPS_rxbuffer[GPS_RXBUFF_MAXLEN];
//extern DMA_HandleTypeDef  *__SCFG_DMA_UART;

typedef struct {
	_RTC  time;
	uint8_t getFlag : 1;
}GPS_t;

/****************************************************/
void initGPS( _RTC *rtc);
void initSerial_CFG();

void enableReceiveDMAtoIdle_CFG(void);

void Serial_CFG_Callback(uint16_t Size);

//void processing_CMD(uint8_t *myID);

void MarkAsReadData_CFG(void);

uint8_t getGPS_time(_RTC *myRTC);

void testSynchronize();

#endif /* INC_SERIAL_CFG_H_ */
