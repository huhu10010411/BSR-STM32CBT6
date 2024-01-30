/*
 * Lora.c
 *
 *  Created on: Dec 24, 2023
 *      Author: Admin
 */
#include "Lora.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Validation.h"
#include "linkedlist.h"
#include "String_process.h"
//#include "Serial_log.h"
#include  "usart.h"
#include "main.h"
#include "Task.h"
//#include "App_MQTT.h"

#define __LORA_UART 		&huart3
#define __LORA_DMA_UART 	&hdma_usart3_rx

#define LORARXBUFF_MAXLEN		128
#define LORABUFF_MAXLEN			128
#define LORATXBUFF_MAXLEN		100

#define LORA_TIMEOUT		0xFFFF

//UART_HandleTypeDef *__LORA_UART;
//DMA_HandleTypeDef *__LORA_DMA_UART;


//static Station_t *myStation;

uint8_t LoraRxbuff[LORARXBUFF_MAXLEN];
uint8_t Lorabuff[LORABUFF_MAXLEN];
uint8_t LoraTxbuff[LORATXBUFF_MAXLEN];
//static uint16_t oldPos = 0, newPos = 0;
static uint16_t head = 0, tail = 0;
uint8_t isOK = 0;

void initLora()
{
//	__LORA_UART = huart;
//	__LORA_DMA_UART = hdma;
	enableReceiveDMAtoIdle_Lora();
//	Lora_Setmode(SLEEP, 1);

}

void initmyLora()
{
//	initLora(huart, hdma);
//	myStation = myStation;
}
void enableReceiveDMAtoIdle_Lora(void)
{
	  HAL_UARTEx_ReceiveToIdle_DMA(__LORA_UART, LoraRxbuff, LORARXBUFF_MAXLEN);
	  __HAL_DMA_DISABLE_IT(__LORA_DMA_UART,DMA_IT_HT);
}

void Lora_callback(uint16_t Size)
{

		memcpy ((uint8_t *)Lorabuff, (uint8_t *)LoraRxbuff, Size);
	/* start the DMA again */
	enableReceiveDMAtoIdle_Lora();

	//Processing data
	if (isWordinBuff(Lorabuff, Size, (uint8_t*)"OK")) {
		isOK = 1;
	}
	Lora_receive(Lorabuff, Size);

}

void MarkAsReadData_LORA(void)
{
	tail = head;
}

bool isIDExist (uint8_t ID)
{
	Node * current = myStation.ssNode_list->head->next;
	while (current != myStation.ssNode_list->tail)
	{
		if ( ID == current->SSnode.SSnode_ID)	return true;
		current = current->next;
	}
	return false;
}
void Lora_receive(uint8_t *Msg, uint8_t msglen)
{
	if (Msg[START_POS] != START_VALUE)	return ;
	uint8_t id_len = Msg[ID_LENGTH_POS] - 1;
	// Get sensor ID
	uint8_t tmpidbuff[id_len+1] ;
	memset(tmpidbuff, 0, id_len+1);
	memcpy(tmpidbuff, Msg +ID_STARTPOS, id_len);
	uint8_t id = atoi((char*)tmpidbuff);
	if ( !sensorID_validation(id) )	return;

	// Get voltage type or mode
	uint8_t flag = 0;
	voltage_t vtype = 0;
	uint8_t modevalue = 0;
	uint16_t Vperiod = 0;
	uint8_t Vcalib[101];
	uint8_t vbat = 0;
	uint8_t mode = 0;
	uint8_t tmp = Msg[ID_STARTPOS+id_len];
	switch (tmp)	{
	case 7:
		vtype = V_na;
		break;
	case 8:
		vtype = V_p;
		break;
	case 4:
		mode = 1;
		break;
	default:
		break;
	}
	if (!vtype && !mode)	return;
	if (mode)	{
		// Get mode
		modevalue = Msg[ID_STARTPOS+id_len +2];
		flag = 1;
	}
	else {
		// check whether Data Period or Data Calib
		switch	(Msg[ID_STARTPOS +id_len +1]) {
		case 2:
			// Get data period value
			Vperiod = buff2twobyte(Msg + ID_STARTPOS +id_len +2);
			// get Vbat
			vbat = Msg[ID_STARTPOS +id_len +6];
			flag = 2;
			break;
		case 100:
			// Get data calib value
			memcpy(Vcalib , Msg + ID_STARTPOS +id_len +2, 100);
			flag = 3;
			break;
		default:
			break;

		}

	}

	// If Sensor ID is  not already saved in the Sensor node list
	if ( !isIDExist(id) )		{
		triggerTaskflag(TASK_REGISTER, FLAG_EN);
		SensorNode_t newSensor = SENSORNODE_T_INIT;
		newSensor.SSnode_ID = id;
		newSensor.tickupdate = HAL_GetTick();
		switch (flag)	{
		case 1:
			newSensor.sensorMode = modevalue;
			if (myStation.prepare_flag) {
				if (modevalue == WAKE)
				newSensor.Ready = 1;
			}
			break;
		case 2:
			newSensor.V_type = vtype;
			newSensor.V_value = Vperiod;
			newSensor.Battery = vbat;
			break;
		case 3:
			newSensor.V_type = vtype;
			memcpy(newSensor.dataCalibBuffer, Vcalib, 100);
			newSensor.dataCalibAvailable = 1;
			break;
		default:
			break;
		}
		list_append(myStation.ssNode_list, newSensor);
	}
	// If Sensor ID is already saved in the Sensor node list
	else {
		Node * current = myStation.ssNode_list->head->next;
		while (current != myStation.ssNode_list->tail)	{
			if (current->SSnode.SSnode_ID == id) {
				switch (flag)	{

				case 1:		// Mode
					current->SSnode.sensorMode = modevalue;
					if (myStation.prepare_flag) {
						if (modevalue == WAKE)
						current->SSnode.Ready = 1;
					}
					break;

				case 2:		// Data period
					current->SSnode.V_type = vtype;
					current->SSnode.V_value = Vperiod;
					current->SSnode.Battery = vbat;



					break;

				case 3:				// Data calib
					current->SSnode.V_type = vtype;
					memcpy(current->SSnode.dataCalibBuffer, Vcalib, 100);
					current->SSnode.dataCalibAvailable = 1;
					break;
				default:
					break;
				}
				current->SSnode.tickupdate = HAL_GetTick();
				return;
			}
			current = current->next;
		}
	}
}
/*
 *  @para: checkres : 0 no check
 *  				  1 check
 */
uint8_t Lora_Setmode(sensor_mode_t mode, uint8_t checkres)
{
	uint8_t len = sprintf((char*)LoraTxbuff, "AT+MODE=%d", mode);
	isOK = 0;
	HAL_UART_Transmit(__LORA_UART, LoraTxbuff, len, LORA_TIMEOUT);
	// Wait for responding
	switch (checkres)	{
	case 0:
		break;
	case 1:
		for (uint16_t i = 0; i < 500; i++)	{
			HAL_Delay(2);
			if (isOK) {
				return 1;
			}
		}
		break;
	default :
		break;
	}
	return 0;
}
void testLora_receive (void)
{
//	Lora_Setmode(WAKE,1);

//	Lora_Setmode(SLEEP, 1);

//	uint8_t modemsg [] = {0x01, 0x04, 0x53,0x30, 0x39, 0x31, 0x04, 0x01, 0x00, 0x00, 0xD6};
//	uint8_t msg[] = { 0x01, 0x04, 0x53, 0x30, 0x39, 0x31, 0x07, 0x02, 0x05, 0xD6, 0x09, 0x02, 0x02, 0xF4, 0x00, 0x64};
//	Lora_receive(modemsg, 20);
//	Serial_log_number(myStation->stID);
}


