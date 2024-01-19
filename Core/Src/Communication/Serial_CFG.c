/*
 * Serial_CFG.c
 *
 *  Created on: Oct 2, 2023
 *      Author: Admin
 */

#include "Serial_CFG.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <String_process.h>
#include <flash_storage.h>
#include "MQTT.h"
#include "Task.h"
#include "ds3231.h"
#include "Validation.h"
#include "main.h"
#include "usart.h"

#define __SCFG_UART &huart2
#define __SCFG_DMA_UART &hdma_usart2_rx
#define Rx_SIZE_CFG     512
//#define Main_SIZE_CFG    512


//UART_HandleTypeDef *__SCFG_UART;
//DMA_HandleTypeDef  *__SCFG_DMA_UART;

uint8_t Rxbuff_CFG[Rx_SIZE_CFG];
//uint8_t Mainbuff_CFG[Main_SIZE_CFG];
uint8_t GPS_rxbuffer[GPS_RXBUFF_MAXLEN];
//uint16_t oldPos_CFG=0, newPos_CFG =0;
//uint16_t head_CFG = 0, tail_CFG = 0;

uint16_t GPS_size = 0;
uint8_t tx_buff[30];

uint8_t volatile isSerialConfig = 0;

static uint32_t tick = 0;

//Station_t *myStation;
_RTC *__MY_RTC;

uint8_t alarmflag = 0;


uint8_t volatile getflag = 0;

void initSerial_CFG()
{
//	__SCFG_UART = huart;
//	__SCFG_DMA_UART = hdma;
	enableReceiveDMAtoIdle_CFG();
}

void initGPS(_RTC *rtc)
{
//	myStation = station;
	__MY_RTC = rtc;
}

void enableReceiveDMAtoIdle_CFG(void)
{
	  HAL_UARTEx_ReceiveToIdle_DMA(__SCFG_UART, Rxbuff_CFG, Rx_SIZE_CFG);
	  __HAL_DMA_DISABLE_IT(__SCFG_DMA_UART,DMA_IT_HT);
}
void Serial_CFG_Callback(uint16_t Size)
{

//	oldPos_CFG = newPos_CFG;  // Update the last position before copying new data
//
//	/* If the data in large and it is about to exceed the buffer size, we have to route it to the start of the buffer
//	 * This is to maintain the circular buffer
//	 * The old data in the main buffer will be overlapped
//	 */
//	if (oldPos_CFG+Size > Main_SIZE_CFG)  // If the current position + new data size is greater than the main buffer
//	{
//		uint8_t datatocopy = Main_SIZE_CFG-oldPos_CFG;  // find out how much space is left in the main buffer
//		memcpy ((uint8_t *)Mainbuff_CFG+oldPos_CFG, (uint8_t *)Rxbuff_CFG, datatocopy);  // copy data in that remaining space
//
//		oldPos_CFG = 0;  // point to the start of the buffer
//		memcpy ((uint8_t *)Mainbuff_CFG, (uint8_t *)Rxbuff_CFG+datatocopy, (Size-datatocopy));  // copy the remaining data
//		newPos_CFG = (Size-datatocopy);  // update the position
//	}
//
//	/* if the current position + new data size is less than the main buffer
//	 * we will simply copy the data into the buffer and update the position
//	 */
//	else
//	{
//		memcpy ((uint8_t *)Mainbuff_CFG+oldPos_CFG, (uint8_t *)Rxbuff_CFG, Size);
//		newPos_CFG = Size+oldPos_CFG;
//	}
//
//	/* Update the position of the Head
//	 * If the current position + new size is less then the buffer size, Head will update normally
//	 * Or else the head will be at the new position from the beginning
//	 */
//	if (head_CFG+Size < Main_SIZE_CFG) head_CFG = head_CFG+Size;
//	else head_CFG = head_CFG + Size - Main_SIZE_CFG;

	if ( checkTaskflag(TASK_GET_GPS_TIME) )	{
		memset(GPS_rxbuffer, 0, GPS_RXBUFF_MAXLEN);
		memcpy(GPS_rxbuffer, Rxbuff_CFG, Size);
		GPS_size = Size;
//		getflag = 1;
		getGPS_time(__MY_RTC);
	}

	enableReceiveDMAtoIdle_CFG();
	/*
	 * Processing data
	 */

//	if (isWordinBuff(Rxbuff_CFG, Size, (uint8_t*)"MCFG+")) {
//		isSerialConfig = 1;
//	}
}


//void processing_CMD(uint8_t *myID)
//{
//
//	if (isSerialConfig)	{
//
//	uint8_t len;
//	uint16_t size = 0;
//	uint8_t tmpID;
//	uint8_t tmpbuff[10];
//	uint8_t *Processingbuff;
//
//	//Copy data to processing buffer
//	if(head_CFG > tail_CFG)
//	{
//		size = head_CFG - tail_CFG;
//		Processingbuff = (uint8_t*)malloc( size*sizeof(uint8_t) );
//		memcpy((uint8_t*)Processingbuff, (uint8_t*)Mainbuff_CFG+ tail_CFG, size);
//	}
//	else if (head_CFG < tail_CFG)
//	{
//		size = Main_SIZE_CFG - tail_CFG + head_CFG;
//		Processingbuff = (uint8_t*)malloc( size*sizeof(uint8_t) );
//		memcpy((uint8_t*)Processingbuff, (uint8_t*)Mainbuff_CFG + tail_CFG, Main_SIZE_CFG - tail_CFG);
//		memcpy((uint8_t*)Processingbuff + Main_SIZE_CFG - tail_CFG, (uint8_t*)Mainbuff_CFG, head_CFG);
//	}
//	else return;
//
//	// Check whether the Command "SET ID" in the buffer
//	uint8_t *currPOS = isWordinBuff(Processingbuff, size, (uint8_t*)"MCFG+SETID:");
//	if (  currPOS != NULL)	{
//		uint16_t currsize = getRemainsize(currPOS, Processingbuff, size);
//		memset(tmpbuff, 0, 10);
//		// Get ID to buffer
//		if ( !getBetween( (uint8_t*)":", (uint8_t*)";", currPOS, currsize, tmpbuff) ) {
//			free(Processingbuff);
//			MarkAsReadData_CFG();
//			return;
//		}
//		//Convert buffer ID to uint8_t value
//		tmpID = atoi( (char*)tmpbuff );
//
//		// Validation the ID
//		if ( stationID_validation(tmpID) )	{
//			// Save ID
//			*myID = tmpID;
//			// Save ID to flash
//			Flash_Write_NUM(FLASH_PAGE_127, tmpID);
//			// Respond OK to user
//			len =sprintf((char*)tx_buff, "MCFG+SETID:%d OK", tmpID);
//			HAL_UART_Transmit(__SCFG_UART, tx_buff, len, 2000);
//		}
//		else {
//			HAL_UART_Transmit(__SCFG_UART, (uint8_t*)"MCFG+SETID ERROR", 16, 2000);
//		}
//
//	}
//
//	// Check whether the Command "GET ID" in the buffer
//	if ( isWordinBuff(Processingbuff, size, (uint8_t*)"MCFG+GETID?") != NULL )	{
//		// Respond ID to user
//		len = sprintf( (char*)tx_buff,"MCFG+GETID:%d OK", *myID);
//		HAL_UART_Transmit(__SCFG_UART, (uint8_t*)tx_buff, len, 2000);
//	}
//	free(Processingbuff);
//
//	MarkAsReadData_CFG();
//	isSerialConfig = 0;
//	}
//}

//void MarkAsReadData_CFG(void)
//{
//	tail_CFG = head_CFG;
//}

uint8_t getGPS_time(_RTC *myRTC)
{
//	triggerTaskflag(TASK_GET_GPS_TIME, FLAG_DIS);
//	if (__MY_GPS->getFlag) return 0;
//	uint32_t tmp1 = HAL_GetTick();
//	uint32_t tmp2 ;
	if ( !DS3231_GetTime(myRTC) )		return 0;
	uint16_t gpslen = GPS_size;
//	uint8_t *currPos = isWordinBuff(GPS_rxbuffer, gpslen, (uint8_t*)"$GPRMC");
	uint8_t *currPos = (uint8_t*)strstr((char*)GPS_rxbuffer, "$GPRMC" );
	if ( currPos == NULL )	{
		return 0;
	}
	uint16_t remainlen = getRemainsize(currPos, GPS_rxbuffer, gpslen);
	if (remainlen < 7) {
		return 0;
	}
	currPos+= 7;

	// Get time to buffer
	uint8_t timebuffer [10];
	memcpy(timebuffer, currPos, 6);
	for( uint8_t i = 0 ; i < 6; i++)
	{
		if (timebuffer[i] > '9' || timebuffer[i] < '0'){
			return 0;
		}
	}

	// Convert time from buffer and save to myRTC
	uint8_t tmphour[3];
	uint8_t tmpmin[3];
	uint8_t tmpsec[3];
	memset(tmphour, 0, 3);
	memset(tmpmin, 0, 3);
	memset(tmpsec, 0, 3);
	memcpy(tmphour, timebuffer, 2);
	memcpy(tmpmin, timebuffer + 2, 2);
	memcpy(tmpsec, timebuffer + 4, 2);

	if (atoi((char*)tmphour) + (uint8_t)MY_TIME_ZONE >= 24)	{
		myRTC->Hour = atoi((char*)tmphour) + (uint8_t)MY_TIME_ZONE - 24;
	}
	else {
		myRTC->Hour = atoi((char*)tmphour) + (uint8_t)MY_TIME_ZONE;
	}

	myRTC->Min = atoi((char*)tmpmin);
	myRTC->Sec = atoi((char*)tmpsec);
//	tmp2 = HAL_GetTick() - tmp1;
	if ( !DS3231_SetTime(myRTC) )	return 0;
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	triggerTaskflag(TASK_GET_GPS_TIME, FLAG_DIS);
	return 1;
}

void testSynchronize()
{
	DS3231_GetTime(__MY_RTC);
	if (HAL_GetTick() - tick > 5000)	{
		tick = HAL_GetTick();
		triggerTaskflag(TASK_GET_GPS_TIME, FLAG_EN);
	}
	static uint8_t i = 35;
	if (!alarmflag)	{
		alarmflag = 1;
		DS3231_ClearAlarm1();
		DS3231_SetAlarm1(ALARM_MODE_ALL_MATCHED, __MY_RTC->Date, __MY_RTC->Hour , i++ , 0);
	}
}
