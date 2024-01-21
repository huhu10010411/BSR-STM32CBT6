/*
 * SIM.c
 *
 *  Created on: Oct 9, 2023
 *      Author: Admin
 */

#include "SIM.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "MQTT.h"
#include "Serial_log.h"
#include "String_process.h"
#include "Validation.h"
#include "Task.h"
#include "main.h"
#include "usart.h"
#include "ServerMessage.h"


#define SIM_UART		&huart1
#define SIM_DMA_UART	&hdma_usart1_rx

#define SIM_BUFF_SIZE        512
#define SIM_RXBUFF_SIZE      150
#define SIM_SMS_RXSIZE       150



//UART_HandleTypeDef *SIM_UART;
//DMA_HandleTypeDef  *SIM_DMA_UART;
//SIM_t	*mySIM;

uint16_t oldPos =0, newPos =0;
uint8_t SIMbuff[SIM_BUFF_SIZE];
uint8_t SIMRxbuff[SIM_RXBUFF_SIZE];

uint8_t SMS_Rxbuff [SIM_SMS_RXSIZE];
uint8_t MQTT_Rxbuff [MQTT_RXBUFF_SIZE];
uint16_t head =0, tail=0;
uint16_t mqtttail=0, mqtthead= 0;

static volatile uint8_t newSMS = 0;

uint8_t getSMS = 0;
uint16_t SMS_len = 0;
//uint8_t connlost = 0;



void initSIM()
{
	enableReceiveDMAtoIdle_SIM();
//	SIM_sendCMD((uint8_t*)"ATE0",(uint8_t*)"OK", ENABLE_SIM_CHECKRES, ENABLE_MARKASREAD, 1000);
}
void enableReceiveDMAtoIdle_SIM(void)
{
	 HAL_UARTEx_ReceiveToIdle_DMA(SIM_UART, SIMRxbuff, SIM_RXBUFF_SIZE);
	  __HAL_DMA_DISABLE_IT(SIM_DMA_UART,DMA_IT_HT);
}

void SIM_callback(uint16_t Size)
{


		oldPos = newPos;  // Update the last position before copying new data

		/* If the data in large and it is about to exceed the buffer size, we have to route it to the start of the buffer
		 * This is to maintain the circular buffer
		 * The old data in the main buffer will be overlapped
		 */
		if (oldPos+Size > SIM_BUFF_SIZE)  // If the current position + new data size is greater than the main buffer
		{
			uint16_t datatocopy = SIM_BUFF_SIZE-oldPos;  // find out how much space is left in the main buffer
			memcpy ((uint8_t *)SIMbuff+oldPos, (uint8_t *)SIMRxbuff, datatocopy);  // copy data in that remaining space

			oldPos = 0;  // point to the start of the buffer
			memcpy ((uint8_t *)SIMbuff, (uint8_t *)SIMRxbuff+datatocopy, (Size-datatocopy));  // copy the remaining data
			newPos = (Size-datatocopy);  // update the position
		}

		/* if the current position + new data size is less than the main buffer
		 * we will simply copy the data into the buffer and update the position
		 */
		else
		{
			memcpy ((uint8_t *)SIMbuff+oldPos, (uint8_t *)SIMRxbuff, Size);
			newPos = Size+oldPos;
		}

		/* Update the position of the Head
		 * If the current position + new size is less then the buffer size, Head will update normally
		 * Or else the head will be at the new position from the beginning
		 */
		if (head + Size < SIM_BUFF_SIZE ) head = head + Size;
		else head = head + Size - SIM_BUFF_SIZE;

		/* start the DMA again */
		enableReceiveDMAtoIdle_SIM();

		// New SMS coming
		if (isWordinBuff(SIMRxbuff, Size, (uint8_t*)"+CMTI: \"SM\"") != NULL)
		{
			newSMS = 1;
		}

		// Read SMS
		if (isWordinBuff(SIMRxbuff, Size, (uint8_t*)"+CMGR:") != NULL)	{
			memcpy(SMS_Rxbuff, SIMRxbuff, Size);
			SMS_len = Size;
		}
		if (isWordinBuff(SIMRxbuff, Size, (uint8_t*)"+CMGL: ") != NULL)	{
			memcpy(SMS_Rxbuff, SIMRxbuff, Size);
			SMS_len = Size;
		}
		// New MQTT message
		if ( (isWordinBuff(SIMRxbuff, Size, (uint8_t*) "+CMQTTRXSTART:") != NULL)
				&& ( isWordinBuff(SIMRxbuff, Size,(uint8_t*) "+CMQTTRXEND:") != NULL ) )  	{
			memcpy(MQTT_Rxbuff, SIMRxbuff, Size);
			MQTT_receive(MQTT_Rxbuff, Size);
		}

		if  (isWordinBuff(SIMRxbuff, Size, (uint8_t*)"+CMQTTCONNLOST:") != NULL) 	{
			mySIM.mqttServer.connect = 0;
		}
//		memset(SIMRxbuff, 0, SIM_RXBUFF_SIZE);
//		Serial_log_string("Rx: ");
//		Serial_log_buffer(SIMRxbuff, Size);
//		Serial_log_string(" ");


}

SIM_res_t SIM_checkMsg(uint8_t *Msg, uint16_t timeout)
{
	SIM_res_t res= SIM_NO_RES;
	uint8_t dataSize = 0;
	uint8_t tmpdbuff[SIM_BUFF_SIZE];
	memset( (char*)tmpdbuff, 0, SIM_BUFF_SIZE );
	timeout /= 20;

	for (uint8_t i = 0; i < 20 ; i++)
	{
		HAL_Delay(timeout);

		if (head > tail)
		{
			dataSize = head - tail ;

			memcpy(tmpdbuff, SIMbuff + tail, dataSize);
		}
		else if ( head < tail )
		{
			dataSize = SIM_BUFF_SIZE - tail + head ;
			memcpy(tmpdbuff, SIMbuff + tail, SIM_BUFF_SIZE - tail);
			memcpy(tmpdbuff + SIM_BUFF_SIZE - tail, SIMbuff, head);
		}
		else {
			continue;
		}

		if ( isWordinBuff(tmpdbuff, dataSize, Msg) != NULL )	{
			return SIM_RES_MSG ;
		}

		if ( isWordinBuff(tmpdbuff, dataSize, (uint8_t*)"ERROR") != NULL )	{
			return SIM_ERROR ;
		}
	}

 	return res;
}

SIM_res_t SIM_sendCMD(uint8_t *cmd, uint8_t *checkResMsg, uint8_t CheckResENorDIS, uint8_t ENorDISmarkasread, uint32_t timeout)
{
	uint8_t SIM_Txbuff[256];
	uint8_t len = sprintf( (char*)SIM_Txbuff, "%s\r\n", cmd);
	HAL_UART_Transmit(SIM_UART, SIM_Txbuff, len, 0xFFFF) ;

	if (CheckResENorDIS == ENABLE_SIM_CHECKRES)
	{
		SIM_res_t res = SIM_checkMsg(checkResMsg, timeout);
		if (ENorDISmarkasread == ENABLE_MARKASREAD)
		{
			MarkAsReadData_SIM();
		}
//		Serial_log_string("Res of CMD: ");
//		Serial_log_buffer(cmd, len);
//		Serial_log_number(res);
		return res;
	}
	return SIM_NO_RES;
}

/*
 * Return : 1 success
 * 			0 fail
 */
uint8_t SIM_checkCMD (SIM_CMD_t cmd)
{
	uint8_t res = 0;
//	SIM_res_t check;
	switch (cmd) {
		case SIM_CMD_SIMCARD_PIN:

			if ( SIM_sendCMD( (uint8_t*)"AT+CPIN?", (uint8_t*)"+CPIN: READY", ENABLE_SIM_CHECKRES, ENABLE_MARKASREAD, SIM_TIMEOUT_SHORT) == SIM_RES_MSG ) {
				res = 1;
//				Serial_log_string("SIM card READY\r\n");
			}
			break;
		case SIM_CMD_NW_CPSI:
			if ( SIM_sendCMD( (uint8_t*)"AT+CPSI?", (uint8_t*)"+CPSI: NO SERVICE", ENABLE_SIM_CHECKRES, ENABLE_MARKASREAD, SIM_TIMEOUT_SHORT) == SIM_RES_MSG ) {
//				Serial_log_string("NO SERVICE, network status has some problem");
			}
			else {
				res = 1;
//				Serial_log_string("SERVICE available\r\n");
			}
			break;
		case SIM_CMD_NW_CREG:
			if ( SIM_sendCMD( (uint8_t*)"AT+CREG?", (uint8_t*)"+CREG: 0,1", ENABLE_SIM_CHECKRES, ENABLE_MARKASREAD, SIM_TIMEOUT_SHORT) == SIM_RES_MSG ) {
				res = 1;
//				Serial_log_string("Module is registered to CS domain\r\n");
			}
			break;
		case SIM_CMD_PACKDOM_CGREG:

			break;
		case SIM_CMD_STA_CSQ:
			if ( SIM_sendCMD( (uint8_t*)"AT+CSQ", (uint8_t*)"+CSQ: 99", ENABLE_SIM_CHECKRES, ENABLE_MARKASREAD, SIM_TIMEOUT_SHORT) == SIM_RES_MSG ) {
//				Serial_log_string("Signal quality is bad, please check SIM card or reboot the module\r\n");
			}
			else {
				res = 1;
//				Serial_log_string("Signal quality is good\r\n");
			}
			break;
		default :
			break;
	}
	return res;
}

void SIM_checkOperation(void)
{
		 SIM_sendCMD((uint8_t*)"AT+CMGL=\"REC UNREAD\"", (uint8_t*)"OK",
				 ENABLE_SIM_CHECKRES, ENABLE_MARKASREAD, 1000);

}

void MarkAsReadData_SIM(void)
{
	tail = head;
}

uint16_t getAfterword(uint8_t *srcBuffer, uint16_t srcBufferlen, uint8_t *word, uint8_t *getBuffer)
{
	uint16_t remainlen = 0;
	uint8_t *currPOS = isWordinBuff(srcBuffer, srcBufferlen, word);
	if ( currPOS == NULL ) return	0;
	currPOS += strlen((char*)word);
	remainlen = getRemainsize(currPOS, srcBuffer, srcBufferlen);
	memcpy(getBuffer, currPOS, remainlen);
	return remainlen;
}

uint16_t stringIDtoHexID (uint8_t *stringIDbuffer, uint8_t *hexIDbuffer, const char *separator, ID_t idtype)
{
	uint8_t *token = (uint8_t*)strtok((char*)stringIDbuffer, separator);
	uint16_t hexbufferlen = 0;

	while (token != NULL)	{
		uint16_t tmp = atoi((char*)token);
		if ( validationID(tmp, idtype) )	{
			hexIDbuffer[hexbufferlen++] = tmp;
		}
		token = (uint8_t*)strtok(NULL, separator);
	}
	return hexbufferlen;
}

uint16_t SMS_getindex(uint8_t *SMSbuffer,uint16_t SMS_bufferlen)
{
	uint8_t contentbuffer[SMS_bufferlen];
	uint16_t contentlen = getAfterword(SMSbuffer, SMS_bufferlen, (uint8_t*)"+CMGL:", contentbuffer);
	if (!contentlen) {
		contentlen = getAfterword(SMSbuffer, SMS_bufferlen, (uint8_t*)"+CMGR:", contentbuffer);
	}
	if (!contentlen) return 200;

	uint8_t indexbuffer[10];
	getBetween((uint8_t*)" ", (uint8_t*)",", contentbuffer, contentlen, indexbuffer);
	return atoi((char*)indexbuffer);
}

uint8_t SMS_getPhonenumb (uint8_t *SMSbuffer, uint16_t SMS_bufferlen, uint8_t *Phonenumb_buffer )
{
	uint8_t contentbuffer[SMS_bufferlen];
	uint16_t contentlen = getAfterword(SMSbuffer, SMS_bufferlen, (uint8_t*)"+CMGL: ", contentbuffer);
	if (!contentlen) {
		contentlen = getAfterword(SMSbuffer, SMS_bufferlen, (uint8_t*)"+CMGR: ", contentbuffer);
	}
	if (!contentlen) return 0;
//	uint8_t *currPOS;
//	uint16_t remainlen;
//	for (uint8_t i = 0; i<2 ; i++)
//	{
//		currPOS = isWordinBuff(contentbuffer, contentlen, (uint8_t*)",");
//		if ( currPOS == NULL ) return	0;
//		remainlen = getRemainsize(currPOS, contentbuffer, contentlen);
//		currPOS++;
//	}
	return getBetween((uint8_t*)"+", (uint8_t*)"\"", contentbuffer, contentlen, Phonenumb_buffer);
}

uint16_t SMS_getContent (uint8_t *SMSbuffer, uint16_t SMS_bufferlen, uint8_t *Content_buffer )
{
	uint8_t contentbuffer[SMS_bufferlen];
	uint16_t contentlen = getAfterword(SMSbuffer, SMS_bufferlen, (uint8_t*)"+CMGL: ", contentbuffer);
	if (!contentlen) {
		contentlen = getAfterword(SMSbuffer, SMS_bufferlen, (uint8_t*)"+CMGR: ", contentbuffer);
	}
	if (!contentlen) return 0;

	return getBetween((uint8_t*)"\n", (uint8_t*)"\r", contentbuffer, contentlen, Content_buffer);
}





uint8_t SMSaddData(SMS_CMD_t smsCMD, uint8_t *contentbuffer, uint16_t contentlen, uint8_t *phonenumb)
{
	uint8_t *currPOS;
	uint16_t remainlen;
	uint8_t IDbuffer[contentlen];
	uint8_t Timebuffer[10];
	uint16_t time_delay;
	uint16_t datalen = 0;
	uint8_t phonenumblen = strlen((char*)phonenumb);

	uint8_t *Detect_msgType;
	SMS_request_t *Detect_Request;
	uint8_t getIDflag = 0;
	uint8_t getTimeflag = 0;
	uint8_t getSflag = 0;
	ID_t idtype = ID_STATION;
	switch (smsCMD)	{
		case SMS_CMD_CTRL_ON:
			Detect_msgType = (uint8_t*)SMS_MSG_CTRL_ON;
			Detect_Request = &mySIM.sms.CtrlON;
			getIDflag = 1;
			getTimeflag = 1;
			break;
		case SMS_CMD_CTRL_OFF:
			Detect_msgType = (uint8_t*)SMS_MSG_CTRL_OFF;
			Detect_Request = &mySIM.sms.CtrlOFF;
			getIDflag = 1;
			getTimeflag = 1;
			break;
		case SMS_CMD_CTRL_DEC:
			Detect_msgType = (uint8_t*)SMS_MSG_CTRL_DEC;
			Detect_Request = &mySIM.sms.CtrlDEC;
			getIDflag = 1;
			getTimeflag = 1;
			break;
		case SMS_CMD_CTRL_INC:
			Detect_msgType = (uint8_t*)SMS_MSG_CTRL_INC;
			Detect_Request = &mySIM.sms.CtrlINC;
			getIDflag = 1;
			getTimeflag = 1;
			break;
		case SMS_CMD_CTRL_CALIB:
			Detect_msgType = (uint8_t*)SMS_MSG_CTRL_CALIB;
			Detect_Request = &mySIM.sms.CtrlCALIB;
			getTimeflag = 1;
			break;
		case SMS_CMD_GET_STATUS:
			Detect_msgType = (uint8_t*)SMS_MSG_GET_STATUS;
			Detect_Request = &mySIM.sms.GetStatus;
			break;
		case SMS_CMD_GET_STATION:
			Detect_msgType = (uint8_t*)SMS_MSG_GET_STATION;
			Detect_Request = &mySIM.sms.GetStation;
			getIDflag = 1;
			getSflag = 1;
			break;
		case SMS_CMD_GET_SENSOR:
			Detect_msgType = (uint8_t*)SMS_MSG_GET_SENSOR;
			Detect_Request = &mySIM.sms.GetSensor;
			getIDflag = 1;
			getSflag = 1;
			idtype = ID_SENSOR;
			break;
		default:
			break;
		}
	if (Detect_msgType == NULL)	return 0;

	currPOS = isWordinBuff(contentbuffer, contentlen, (uint8_t*)Detect_msgType);
	if (currPOS == NULL) 	return 0;

	remainlen = getRemainsize(currPOS, contentbuffer, contentlen);

	/* Get ID list */
	if (getIDflag)	{
		uint8_t tmplen;
		if (getSflag)	{
			tmplen = getBetween((uint8_t*)":", (uint8_t*)";", currPOS, remainlen, IDbuffer);
		}
		else {
			tmplen = getBetween((uint8_t*)":", (uint8_t*)":", currPOS, remainlen, IDbuffer);
		}
		currPOS += strlen((char*)Detect_msgType) + tmplen + 1;
		remainlen = getRemainsize(currPOS, contentbuffer, contentlen);
		datalen = stringIDtoHexID(IDbuffer, Detect_Request->data, ",", idtype);
	}

	/* Get time delay*/
	if (getTimeflag)	{
		getBetween((uint8_t*)":", (uint8_t*)";", currPOS, remainlen, Timebuffer);
		time_delay = atoi((char*)Timebuffer);
		twobyte2buff(Detect_Request->data +datalen, time_delay);
		datalen += 2;
	}
	memcpy(Detect_Request->phonenumb, phonenumb, phonenumblen);
	memcpy(Detect_Request->data + datalen, phonenumb, phonenumblen);
	datalen += phonenumblen;
	Detect_Request->datalength = datalen;
	return 1;
}
static uint8_t SMS_checkCMD(uint8_t *contentbuffer, uint16_t contentlen, uint8_t *phonenumb)
{
	uint8_t *currPOS;

	currPOS = isWordinBuff(contentbuffer, contentlen, (uint8_t*)SMS_MSG_CTRL_ON);
	if (currPOS != NULL) 	{
		if (checkSMSrequest(SMS_CMD_CTRL_ON)) return 2;
		if ( !SMSaddData(SMS_CMD_CTRL_ON, contentbuffer, contentlen, phonenumb) ) return 0;
		// Trigger cmd flag
		triggerSMSrequest(SMS_CMD_CTRL_ON, FLAG_EN);
		return 1;
	}

	currPOS = isWordinBuff(contentbuffer, contentlen, (uint8_t*)SMS_MSG_CTRL_OFF);
	if (currPOS != NULL) 	{
		if (checkSMSrequest(SMS_CMD_CTRL_OFF)) return 2;
		if ( !SMSaddData(SMS_CMD_CTRL_OFF, contentbuffer, contentlen, phonenumb) )	return 0;
		// Trigger cmd flag
		triggerSMSrequest(SMS_CMD_CTRL_OFF, FLAG_EN);
		return 1;
	}

	currPOS = isWordinBuff(contentbuffer, contentlen, (uint8_t*)SMS_MSG_CTRL_DEC);
	if (currPOS != NULL) 	{
		if (checkSMSrequest(SMS_CMD_CTRL_DEC)) return 2;
		if ( !SMSaddData(SMS_CMD_CTRL_DEC, contentbuffer, contentlen, phonenumb) )	return 0;
		// Trigger cmd flag
		triggerSMSrequest(SMS_CMD_CTRL_DEC, FLAG_EN);
		return 1;
	}

	currPOS = isWordinBuff(contentbuffer, contentlen, (uint8_t*)SMS_MSG_CTRL_INC);
	if (currPOS != NULL) 	{
		if (checkSMSrequest(SMS_CMD_CTRL_INC)) return 2;
		SMSaddData(SMS_CMD_CTRL_INC, contentbuffer, contentlen, phonenumb);
		// Trigger cmd flag
		triggerSMSrequest(SMS_CMD_CTRL_INC, FLAG_EN);
		return 1;
	}

	currPOS = isWordinBuff(contentbuffer, contentlen, (uint8_t*)SMS_MSG_CTRL_CALIB);
	if (currPOS != NULL) 	{
		if (checkSMSrequest(SMS_CMD_CTRL_CALIB)) return 2;
		if ( !SMSaddData(SMS_CMD_CTRL_CALIB, contentbuffer, contentlen, phonenumb) )	return 0;
		// Trigger cmd flag
		triggerSMSrequest(SMS_CMD_CTRL_CALIB, FLAG_EN);
		return 1;
	}

	currPOS = isWordinBuff(contentbuffer, contentlen, (uint8_t*)SMS_MSG_GET_STATUS);
	if (currPOS != NULL) 	{
		if (checkSMSrequest(SMS_CMD_GET_STATUS)) return 2;
		if ( !SMSaddData(SMS_CMD_GET_STATUS, contentbuffer, contentlen, phonenumb) )	return 0;
		// Trigger cmd flag
		triggerSMSrequest(SMS_CMD_GET_STATUS, FLAG_EN);
//		triggerSMSreturn(SMS_CMD_GET_STATUS, FLAG_EN);
		return 1;
	}

	currPOS = isWordinBuff(contentbuffer, contentlen, (uint8_t*)SMS_MSG_GET_STATION);
	if (currPOS != NULL) 	{
		if (checkSMSrequest(SMS_CMD_GET_STATION)) return 2;
		if ( !SMSaddData(SMS_CMD_GET_STATION, contentbuffer, contentlen, phonenumb) ) return 0;
		// Trigger cmd flag
		triggerSMSrequest(SMS_CMD_GET_STATION, FLAG_EN);
//		triggerSMSreturn(SMS_CMD_GET_STATION, FLAG_EN);
		return 1;
	}

	currPOS = isWordinBuff(contentbuffer, contentlen, (uint8_t*)SMS_MSG_GET_SENSOR);
	if (currPOS != NULL) 	{
		if (checkSMSrequest(SMS_CMD_GET_SENSOR)) return 2;
		if ( !SMSaddData(SMS_CMD_GET_SENSOR, contentbuffer, contentlen, phonenumb) ) return 0;
		// Trigger cmd flag
		triggerSMSrequest(SMS_CMD_GET_SENSOR, FLAG_EN);
//		triggerSMSreturn(SMS_CMD_GET_SENSOR, FLAG_EN);
		return 1;
	}
	return 0;
}

uint8_t SMS_read()
{
	SIM_sendCMD((uint8_t*)"AT+CMGF=1", (uint8_t*)"OK", ENABLE_SIM_CHECKRES, ENABLE_MARKASREAD, SIM_TIMEOUT_SHORT);
	if ( SIM_sendCMD((uint8_t*)"AT+CMGL=\"REC UNREAD\"", (uint8_t*)"OK",ENABLE_SIM_CHECKRES,
			ENABLE_MARKASREAD, 1000) != SIM_RES_MSG )	{
		if ( SIM_sendCMD((uint8_t*)"AT+CMGL=\"REC UNREAD\"", (uint8_t*)"OK",ENABLE_SIM_CHECKRES,
					ENABLE_MARKASREAD, 1000) != SIM_RES_MSG )
		newSMS = 0;
		return 0;
	}
	return 1;
}

uint8_t SMS_readAgain(uint8_t SMSindex)
{
	uint8_t SIM_Txbuff[100];
	sprintf((char*)SIM_Txbuff, "AT+CMGR=%d",SMSindex);
	if  ( SIM_sendCMD(SIM_Txbuff, (uint8_t*)"OK",ENABLE_SIM_CHECKRES,
					  ENABLE_MARKASREAD, 1000) != SIM_RES_MSG)	{
		newSMS = 0;
		return 0;
	}
	return 1;
}
uint8_t processingSMS(void)
{

	if (!newSMS) return 0;

//	static uint8_t readagain = 0;
//	static uint16_t SMSindex = 0;

	MarkAsReadData_SIM();
//	if ( readagain ) 	{
//		SMS_readAgain(SMSindex);
//	}
//	else 	{
		SMS_read();
//	}
	/* Processing SMS*/
	//Get SMS index
//	SMSindex = SMS_getindex(SMS_Rxbuff, SMS_len);
//	if (SMSindex == 200)	{
//		newSMS = 0;
//		return 0;
//	}
	//Get phone number
	uint8_t phonenumb[PHONENUMB_LEN];
	if ( !SMS_getPhonenumb(SMS_Rxbuff, SMS_len, phonenumb) ) {
		SIM_sendCMD((uint8_t*)"AT+CMGD=1,1", (uint8_t*)"OK",
					ENABLE_SIM_CHECKRES, ENABLE_MARKASREAD, 1000);
		newSMS = 0;
		return 0;
	}
	//Get SMS content
	uint8_t tmpSMSdatabuffer[SMS_len];
	memset(tmpSMSdatabuffer, 0, SMS_len);
	uint16_t contentlen = SMS_getContent(SMS_Rxbuff, SMS_len, tmpSMSdatabuffer);
//	MQTT_publish((uint8_t*)TOPIC_PUB , tmpSMSdatabuffer, contentlen);
	if ( !contentlen)	{
		SIM_sendCMD((uint8_t*)"AT+CMGD=1,1", (uint8_t*)"OK",
					ENABLE_SIM_CHECKRES, ENABLE_MARKASREAD, 1000);
		newSMS = 0;
		return 0;
	}
	//Check SMS command
	uint8_t checkres = SMS_checkCMD(tmpSMSdatabuffer, contentlen, phonenumb);
	if ( !checkres ) 	{
		SIM_sendCMD((uint8_t*)"AT+CMGD=1,1", (uint8_t*)"OK",
					ENABLE_SIM_CHECKRES, ENABLE_MARKASREAD, 1000);
		newSMS = 0;
		return 0;
	}
//	if ( checkres == 2 )	{
//		readagain = 1;
//	}
	SIM_sendCMD((uint8_t*)"AT+CMGD=1,1", (uint8_t*)"OK",
				ENABLE_SIM_CHECKRES, ENABLE_MARKASREAD, 1000);
	newSMS = 0;

	return 1;
}



bool checkSMSrequest (SMS_CMD_t smsCMD)
{
	bool res = false;
	switch (smsCMD)	{
		case SMS_CMD_CTRL_ON:
			res = mySIM.sms.CtrlON.requestflag ;
			break;
		case SMS_CMD_CTRL_OFF:
			res = mySIM.sms.CtrlOFF.requestflag ;
			break;
		case SMS_CMD_CTRL_DEC:
			res = mySIM.sms.CtrlDEC.requestflag ;
			break;
		case SMS_CMD_CTRL_INC:
			res = mySIM.sms.CtrlINC.requestflag ;
			break;
		case SMS_CMD_CTRL_CALIB:
			res = mySIM.sms.CtrlCALIB.requestflag ;
			break;
		case SMS_CMD_GET_STATUS:
			res = mySIM.sms.GetStatus.requestflag ;
			break;
		case SMS_CMD_GET_STATION:
			res = mySIM.sms.GetStation.requestflag ;
			break;
		case SMS_CMD_GET_SENSOR:
			res = mySIM.sms.GetSensor.requestflag ;
			break;
		default:
			break;
	}
	return res;
}

void triggerSMSrequest (SMS_CMD_t smsCMD, SMS_CMD_FLAG_t ENorDIS)
{
	switch (smsCMD)	{
	case SMS_CMD_CTRL_ON:
		mySIM.sms.CtrlON.requestflag = ENorDIS;
		break;
	case SMS_CMD_CTRL_OFF:
		mySIM.sms.CtrlOFF.requestflag = ENorDIS;
		break;
	case SMS_CMD_CTRL_DEC:
		mySIM.sms.CtrlDEC.requestflag = ENorDIS;
		break;
	case SMS_CMD_CTRL_INC:
		mySIM.sms.CtrlINC.requestflag = ENorDIS;
		break;
	case SMS_CMD_CTRL_CALIB:
		mySIM.sms.CtrlCALIB.requestflag = ENorDIS;
		break;
	case SMS_CMD_GET_STATUS:
		mySIM.sms.GetStatus.requestflag = ENorDIS;
		break;
	case SMS_CMD_GET_STATION:
		mySIM.sms.GetStation.requestflag = ENorDIS;
		break;
	case SMS_CMD_GET_SENSOR:
		mySIM.sms.GetSensor.requestflag = ENorDIS;
		break;
	default:
		break;
	}
}

bool checkSMSreturn (SMS_CMD_t smsCMD)
{
	bool res = false;
	switch (smsCMD)	{
		case SMS_CMD_CTRL_ON:
			res = mySIM.sms.CtrlON.returnflag ;
			break;
		case SMS_CMD_CTRL_OFF:
			res = mySIM.sms.CtrlOFF.returnflag ;
			break;
		case SMS_CMD_CTRL_DEC:
			res = mySIM.sms.CtrlDEC.returnflag ;
			break;
		case SMS_CMD_CTRL_INC:
			res = mySIM.sms.CtrlINC.returnflag ;
			break;
		case SMS_CMD_CTRL_CALIB:
			res = mySIM.sms.CtrlCALIB.returnflag ;
			break;
		case SMS_CMD_GET_STATUS:
			res = mySIM.sms.GetStatus.returnflag ;
			break;
		case SMS_CMD_GET_STATION:
			res = mySIM.sms.GetStation.returnflag ;
			break;
		case SMS_CMD_GET_SENSOR:
			res = mySIM.sms.GetSensor.returnflag ;
			break;
		default:
			break;
		}
	return res;
}
void triggerSMSreturn (SMS_CMD_t smsCMD, SMS_CMD_FLAG_t ENorDIS)
{
	switch (smsCMD)	{
	case SMS_CMD_CTRL_ON:
		mySIM.sms.CtrlON.returnflag = ENorDIS;
		break;
	case SMS_CMD_CTRL_OFF:
		mySIM.sms.CtrlOFF.returnflag = ENorDIS;
		break;
	case SMS_CMD_CTRL_DEC:
		mySIM.sms.CtrlDEC.returnflag = ENorDIS;
		break;
	case SMS_CMD_CTRL_INC:
		mySIM.sms.CtrlINC.returnflag = ENorDIS;
		break;
	case SMS_CMD_CTRL_CALIB:
		mySIM.sms.CtrlCALIB.returnflag = ENorDIS;
		break;
	case SMS_CMD_GET_STATUS:
		mySIM.sms.GetStatus.returnflag = ENorDIS;
		break;
	case SMS_CMD_GET_STATION:
		mySIM.sms.GetStation.returnflag = ENorDIS;
		break;
	case SMS_CMD_GET_SENSOR:
		mySIM.sms.GetSensor.returnflag = ENorDIS;
		break;
	default:
		break;
	}
}

uint8_t SMS_config()
{
	if ( SIM_sendCMD("AT+CMGF=1", (uint8_t*)"OK", ENABLE_SIM_CHECKRES, ENABLE_MARKASREAD, SIM_TIMEOUT_SHORT) != SIM_RES_MSG ) return 0;

	if ( SIM_sendCMD("AT+CSCS=\"GSM\"", (uint8_t*)"OK", ENABLE_SIM_CHECKRES, ENABLE_MARKASREAD, SIM_TIMEOUT_SHORT) != SIM_RES_MSG)		return 0;

	return 1;
}

uint8_t SMS_sendMsg(uint8_t *Msg, uint16_t msglen, uint8_t *phonenumber )
{
	if ( !SMS_config() )	return 0;
	uint8_t SIM_Txbuff[128];
	sprintf((char*)SIM_Txbuff, "AT+CMGS=\"%s\"", phonenumber);
	if ( SIM_sendCMD(SIM_Txbuff, (uint8_t*)">", ENABLE_SIM_CHECKRES, ENABLE_MARKASREAD, SIM_TIMEOUT_MEDIUM) != SIM_RES_MSG)	return 0;

	Msg[msglen++] = 0x1A;
//	sprintf(SIM_Txbuff, "%s", Msg);
	memcpy(SIM_Txbuff, Msg, msglen);
	HAL_UART_Transmit(SIM_UART, SIM_Txbuff, msglen, 0xFFFF);
	if ( SIM_checkMsg((uint8_t*)"OK", SIM_TIMEOUT_SHORT) != SIM_RES_MSG)	{
		MarkAsReadData_SIM();
		return 0;
	}
//	HAL_UART_Transmit(SIM_UART, (uint8_t*), Size, Timeout)
	MarkAsReadData_SIM();
	return 1;
}


void testSMS(void)
{
	uint8_t msg[] = "hi hi hi";
	SMS_sendMsg(msg, strlen((char*)msg), (uint8_t*)"84332560144");
//	uint8_t SMSbuff[]= "CTRL+ON:10,2,50,18,100,23:47;";
//	SMS_checkCMD(SMSbuff, strlen((char*)SMSbuff),(uint8_t*)"84332560144");
//	Serial_log_number(mySIM.sms.CtrlON.requestflag);
//	Serial_log_string(" ");
//	Serial_log_buffer(mySIM.sms.CtrlON.data, mySIM.sms.CtrlON.datalength);
//	Serial_log_string(" ");
}
