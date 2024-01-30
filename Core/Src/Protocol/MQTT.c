/*
 * SIM_MQTT.c
 *
 *  Created on: Oct 9, 2023
 *      Author: Admin
 */

#include "MQTT.h"
#include "SIM.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include "Serial_log.h"
#include "String_process.h"
#include "usart.h"
#include "main.h"
#include "user_lcd1604.h"
#include "ServerMessage.h"


#define MQTT_TIMEOUT_LONG	5000
#define MQTT_TIMEOUT_MEDIUM	2000
#define MQTT_TIMEOUT_SHORT	800
#define MQTT_TIMEOUT_VERYSHORT	400



//static SIM_t *mySIM;

uint8_t MQTT_Txbuff [MQTT_TXBUFF_SIZE];

extern uint8_t Pack_buff [PACKBUFF_MAXLEN];

void init_MQTT()
{
//	mySIM = SIM;
}

uint8_t MQTT_getConnectflag()
{
	return mySIM.mqttServer.connect;
}
uint8_t SumofOccurances(uint8_t * word, uint8_t *buffer, uint16_t buffer_size)
{
	uint8_t sum =0;
	uint8_t wordlen = strlen( (char*)word );
	uint8_t *p = isWordinBuff(buffer, buffer_size, word);
	while (p != NULL)
	{
		sum++;
		uint16_t remain_size = getRemainsize(p, buffer, buffer_size);
		p = isWordinBuff(buffer+wordlen, remain_size, word);
	}
	return sum;

}


static uint8_t getContent(content_t contentType, uint8_t *databuffer, uint16_t datalen, uint8_t *getbuffer, uint16_t contentlen)
{
	uint16_t gotcontent_len = 0;

	uint8_t *currentPOS ;
	uint16_t databuffer_Remainlen = datalen;

	uint16_t subcontent_len;
	uint8_t subContentlen_buff[10];
	uint8_t contentsignature[20];
	if (contentType == CONTENT_PAYLOAD) {
		strcpy( (char*)contentsignature, "+CMQTTRXPAYLOAD:");
 	}
	else if (contentType == CONTENT_TOPIC) {
		strcpy( (char*)contentsignature, "+CMQTTRXTOPIC:");
	}
	else return 0;

	while ( gotcontent_len < contentlen )
	{
		currentPOS = isWordinBuff(databuffer, databuffer_Remainlen, contentsignature);
		if (currentPOS == NULL)		return  0;
		databuffer_Remainlen = getRemainsize( currentPOS, databuffer, databuffer_Remainlen );

		// Get subcontent length
		getBetween( (uint8_t*)",", (uint8_t*) "\r", currentPOS, databuffer_Remainlen, subContentlen_buff);
		subcontent_len = atoi( (char*)subContentlen_buff );

		// Get subcontent
		while ( *currentPOS != '\n'){
			currentPOS++;
		}
		currentPOS++;
		for (uint16_t i = 0; i< subcontent_len; i++)
		{
			getbuffer[gotcontent_len++] = currentPOS[i];
		}

		// Update databuffer
		databuffer = ++currentPOS;
	}
	return 1;
}

uint8_t MQTT_receiveGetTopicandPayloadLength( uint8_t *MQTTbuff, uint16_t buffsize)
{
	uint8_t topiclen_buff[10];
	uint8_t payloadlen_buff[10];
//	Serial_log_string("MQTTbuffer: ");
//	Serial_log_buffer(MQTTbuff, buffsize);
//	Serial_log_string(" ");
	uint8_t * currentPos = isWordinBuff(MQTTbuff, buffsize, (uint8_t*)"+CMQTTRXSTART:");

	uint16_t remainlen = getRemainsize(currentPos, MQTTbuff, buffsize);

	getBetween( (uint8_t*)",", (uint8_t*)",", currentPos, remainlen, topiclen_buff);
	mySIM.mqttReceive.topicLen = atoi( (char*)topiclen_buff );

	currentPos = isWordinBuff(currentPos, remainlen,(uint8_t*)",");
	currentPos ++;

	getBetween( (uint8_t*)",", (uint8_t*)"\r", currentPos, remainlen, payloadlen_buff);
	mySIM.mqttReceive.payloadLen = atoi( (char*)payloadlen_buff );

	return 1;
}

uint8_t MQTT_receiveGetTopic (uint8_t *MQTTbuff,uint16_t buffsize)
{
	return getContent(CONTENT_TOPIC, MQTTbuff, buffsize, mySIM.mqttReceive.topic, mySIM.mqttReceive.topicLen);
}

uint8_t MQTT_receiveGetPayload (uint8_t *MQTTbuff,uint16_t buffsize)
{
	return getContent(CONTENT_PAYLOAD, MQTTbuff, buffsize, mySIM.mqttReceive.payload, mySIM.mqttReceive.payloadLen);
}

uint8_t MQTT_receive(uint8_t *MQTTbuff,uint16_t buffsize)
{
	if ( !MQTT_receiveGetTopicandPayloadLength(MQTTbuff, buffsize) ) return 0;

	memset( mySIM.mqttReceive.payload, 0, MAX_PAYLOAD_LEN );
	memset( mySIM.mqttReceive.topic, 0, MAX_TOPIC_LEN );

	if (! MQTT_receiveGetTopic(MQTTbuff, buffsize))		return 0;

	if (! MQTT_receiveGetPayload(MQTTbuff, buffsize))	return 0;

	mySIM.mqttReceive.newEvent = 1;
	return 1;
}

uint8_t MQTT_connected(void)
{
	if ( SIM_sendCMD("AT+CMQTTDISC?", (uint8_t*)"+CMQTTDISC: 0,0", ENABLE_SIM_CHECKRES, ENABLE_MARKASREAD, 1000) == SIM_RES_MSG )
	{
//		mySIM.mqttServer.connect = 1;
//		Serial_log_string("MQTT is connected to broker\r\n");
		return 1;
	}

//	Serial_log_string("MQTT is disconnected to broker\r\n");
//	mySIM.mqttServer.connect = 0;
	return 0;
}

//uint8_t networkAvailable() {
//    // Check network availability
//    // Implement your network check logic here
//    return 1; // Replace with your logic
//}
//
uint8_t startMQTT(void) {
    // Implement starting MQTT logic
	// Return 1 on success, 0 on failure

	uint8_t check = SIM_sendCMD("AT+CMQTTSTART", (uint8_t*)"+CMQTTSTART: 0", ENABLE_SIM_CHECKRES, ENABLE_MARKASREAD, MQTT_TIMEOUT_SHORT);
	if ( check == SIM_RES_MSG )	{
//		Serial_log_string("start MQTT success\r\n");
		return 1;
	}

//	check = SIM_sendCMD((uint8_t*)"AT+CMQTTSTART", (uint8_t*)"ERROR", ENABLE_SIM_CHECKRES, ENABLE_MARKASREAD, 2000);
	else if ( check == SIM_ERROR )	{
//		Serial_log_string("start MQTT success\r\n");
		return 1;
	}
//	Serial_log_number(check);
//	Serial_log_string("\r\nstart MQTT fail\r\n");
	return 0;
}

uint8_t acquireMQTTclient (uint8_t *clientID)
{
	sprintf((char*)MQTT_Txbuff,"AT+CMQTTACCQ=0,\"%s\"", clientID);
	if (SIM_sendCMD((char*)MQTT_Txbuff, (uint8_t*)"OK", ENABLE_SIM_CHECKRES,
			ENABLE_MARKASREAD, MQTT_TIMEOUT_MEDIUM)== SIM_RES_MSG)	{
//		Serial_log_string("acquire MQTT client success\r\n");
		return 1;
	}
//	Serial_log_string("acquire MQTT client fail\r\n");
    return 0;
}

uint8_t releaseMQTTclient (void)
{
	if (SIM_sendCMD("AT+CMQTTREL=0", (uint8_t*)"OK",
			ENABLE_SIM_CHECKRES, ENABLE_MARKASREAD, MQTT_TIMEOUT_SHORT) == SIM_RES_MSG) {
//		Serial_log_string("release MQTT client success\r\n");
		return 1;
	}
//	Serial_log_string("release MQTT client fail\r\n");
	return 0;
}
uint8_t configureMQTT() {
    // Implement MQTT configuration logic
    // Return 1 on success, 0 on failure
	uint8_t wtplen = strlen(mySIM.mqttServer.willtopic);
	sprintf((char*)MQTT_Txbuff,"AT+CMQTTWILLTOPIC=0,%d",wtplen);
	if (SIM_sendCMD((char*)MQTT_Txbuff, (uint8_t*)">", ENABLE_SIM_CHECKRES, ENABLE_MARKASREAD, MQTT_TIMEOUT_SHORT) != SIM_RES_MSG)	return 0;

	sprintf((char*)MQTT_Txbuff,"%s",mySIM.mqttServer.willtopic);
	if (SIM_sendCMD((char*)MQTT_Txbuff, (uint8_t*)"OK", ENABLE_SIM_CHECKRES, ENABLE_MARKASREAD, MQTT_TIMEOUT_SHORT) != SIM_RES_MSG)	return 0;

	uint8_t wmsglen = strlen((char*)mySIM.mqttServer.willmsg);
	sprintf((char*)MQTT_Txbuff,"AT+CMQTTWILLMSG=0,%d,1",wmsglen);
	if (SIM_sendCMD((char*)MQTT_Txbuff, (uint8_t*)">", ENABLE_SIM_CHECKRES, ENABLE_MARKASREAD, MQTT_TIMEOUT_SHORT)!= SIM_RES_MSG)	return 0;

	sprintf((char*)MQTT_Txbuff,"%s",mySIM.mqttServer.willmsg);
	if (SIM_sendCMD((char*)MQTT_Txbuff, (uint8_t*)"OK", ENABLE_SIM_CHECKRES, ENABLE_MARKASREAD, MQTT_TIMEOUT_SHORT)!= SIM_RES_MSG)	return 0;

//	if (! SIM_sendCMD((uint8_t*)"AT+CMQTTCFG?", (uint8_t*)"+CMQTTCFG: 0,0", ENABLE_SIM_CHECKRES, ENABLE_MARKASREAD, MQTT_TIMEOUT_SHORT))	{

		sprintf((char*)MQTT_Txbuff,"AT+CMQTTCFG=\"checkUTF8\",0,0");
		if (! SIM_sendCMD((char*)MQTT_Txbuff, (uint8_t*)"OK", ENABLE_SIM_CHECKRES, ENABLE_MARKASREAD, MQTT_TIMEOUT_SHORT))	return 0;
//	}



//	Serial_log_string("config MQTT success\r\n");
    return 1;
}

uint8_t sendConnectMessage() {
    // Implement sending the CONNECT message
    // Return 1 on success, 0 on failure
	sprintf((char*)MQTT_Txbuff,"AT+CMQTTCONNECT=0,\"%s:%d\",%d,1",mySIM.mqttServer.host,mySIM.mqttServer.port,mySIM.mqttClient.keepAliveInterval);
	if (SIM_sendCMD((char*)MQTT_Txbuff, (uint8_t*)"+CMQTTCONNECT: 0,0", ENABLE_SIM_CHECKRES, ENABLE_MARKASREAD, MQTT_TIMEOUT_LONG) == SIM_RES_MSG)
	{
//		Serial_log_string("Connect to broker success\r\n");
		return 1;
	}
//	Serial_log_string("Connect to broker fail\r\n");
    return 0;
}


uint8_t MQTT_checkNWavailable (void)
{
	static uint8_t count = 0;
	if ( !SIM_checkCMD(SIM_CMD_SIMCARD_PIN) ) {
		count++;
		if (count == 10)	{
			count = 0;
			LCD_Clear();
			LCD_GotoXY(3,0);
			LCD_Print("NO SIM");

			SIM_sendCMD( "AT+CRESET", (uint8_t*)"OK", ENABLE_SIM_CHECKRES,
						ENABLE_MARKASREAD, SIM_TIMEOUT_LONG);
			HAL_Delay(10000);
		}
		return 1;
	}
//	count = 0;

	if ( !SIM_checkCMD(SIM_CMD_STA_CSQ) ) {
		return 2;
	}

	if ( !SIM_checkCMD(SIM_CMD_NW_CREG) ) {
		return 3;
	}

	if ( !SIM_checkCMD(SIM_CMD_NW_CPSI) ) {
		return 4;
	}
	return 0;
}
uint8_t MQTT_connect()
{
	if ( SIM_sendCMD("AT+CMQTTCONNECT?", (uint8_t*)mySIM.mqttServer.host, ENABLE_SIM_CHECKRES, ENABLE_MARKASREAD, MQTT_TIMEOUT_MEDIUM) == SIM_RES_MSG )	{
		mySIM.mqttServer.connect = 1;
		return 1;
	}

	if ( MQTT_checkNWavailable() )  return 0;

	if ( !startMQTT() )		return 0;

	if ( !acquireMQTTclient( (uint8_t*)mySIM.mqttClient.clientID) )
	{
		if (MQTT_connected())
		{
			MQTT_disconnect();
		}
		else
		{
			releaseMQTTclient();
			SIM_sendCMD("AT+CMQTTSTOP", (uint8_t*)"+CMQTTSTOP: 0",
						ENABLE_SIM_CHECKRES, ENABLE_MARKASREAD, MQTT_TIMEOUT_SHORT);
		}
		return 0;

	}
	if ( !(configureMQTT()) )		return 0;

	if ( !sendConnectMessage() )	{
//		HAL_Delay(500);
		return 0;
	}

	mySIM.mqttServer.connect = 1;
	return 1;

}

uint8_t MQTT_disconnect ()
{
	if (SIM_sendCMD("AT+CMQTTDISC=0,120", (uint8_t*)"+CMQTTDISC: 0,0",
			ENABLE_SIM_CHECKRES, ENABLE_MARKASREAD, MQTT_TIMEOUT_SHORT) != SIM_RES_MSG )		return 0;

	if ( !releaseMQTTclient() )		return 0;

	if (SIM_sendCMD("AT+CMQTTSTOP", (uint8_t*)"+CMQTTSTOP: 0",
			ENABLE_SIM_CHECKRES, ENABLE_MARKASREAD, MQTT_TIMEOUT_SHORT) != SIM_RES_MSG )		return 0;

	mySIM.mqttServer.connect = 0;
	return 1;
}
uint8_t MQTT_publish(uint8_t *topic, uint8_t *msg, uint16_t msglen)
{
//	uint8_t txbuff[100];
//	memset(txbuff, 0, 100);

	uint8_t check =0;
	uint8_t topiclen= (uint8_t) strlen((char*)topic);
//	uint8_t topicbuff[topiclen +1];
	memset(MQTT_Txbuff, 0, MQTT_TXBUFF_SIZE);
	sprintf((char*)MQTT_Txbuff,"AT+CMQTTTOPIC=0,%d", topiclen);

	if (SIM_sendCMD((char*)MQTT_Txbuff, (uint8_t*)">", ENABLE_SIM_CHECKRES, ENABLE_MARKASREAD, MQTT_TIMEOUT_SHORT)!= SIM_RES_MSG)	return 0;

//	memcpy(topicbuff, topic, topiclen);
	HAL_UART_Transmit(&huart1, topic , topiclen+1, 0xFFFF);
	check = SIM_checkMsg((uint8_t*)"OK", MQTT_TIMEOUT_VERYSHORT);
	MarkAsReadData_SIM();
	if (check != SIM_RES_MSG)	return 0;
//	if (SIM_sendCMD(topic, (uint8_t*)"OK", ENABLE_SIM_CHECKRES, ENABLE_MARKASREAD, 2000) != SIM_RES_MSG)		return 0;

	sprintf((char*)MQTT_Txbuff, "AT+CMQTTPAYLOAD=0,%d", msglen);
	if (SIM_sendCMD((char*)MQTT_Txbuff, (uint8_t*)">", ENABLE_SIM_CHECKRES, ENABLE_MARKASREAD, MQTT_TIMEOUT_SHORT)!= SIM_RES_MSG)	return 0;

	uint8_t tmpbuff[msglen+1];
	memset(tmpbuff, 0, msglen +1);
	memcpy(tmpbuff, msg, msglen);
	HAL_UART_Transmit(&huart1, tmpbuff, msglen, 0xFFFF);
	check = SIM_checkMsg((uint8_t*)"OK", MQTT_TIMEOUT_VERYSHORT);
	MarkAsReadData_SIM();
	if (check != SIM_RES_MSG)	return 0;

	if (SIM_sendCMD("AT+CMQTTPUB=0,1,60", (uint8_t*)"+CMQTTPUB: 0,0",
			ENABLE_SIM_CHECKRES, ENABLE_MARKASREAD, MQTT_TIMEOUT_SHORT)!= SIM_RES_MSG)	return 0;

	return 1;
}
uint8_t MQTT_subcribe (uint8_t *topic)
{
	uint8_t topicbuff[30];
	uint8_t topiclen;
	if (strcmp((char*)topic, (char*)TOPIC_SUB) == 0)	{
		topiclen = sprintf((char*)topicbuff, "%s%d",topic,myStation.stID);
	}
	else {
		strcpy((char*)topicbuff,(char*)topic);
		topiclen = strlen((char*)topic);
	}

	sprintf((char*)MQTT_Txbuff,"AT+CMQTTSUBTOPIC=0,%d,1",topiclen);
	if (SIM_sendCMD((char*)MQTT_Txbuff, (uint8_t*)">", ENABLE_SIM_CHECKRES, ENABLE_MARKASREAD, MQTT_TIMEOUT_SHORT)!= SIM_RES_MSG)	return 0;

	if (SIM_sendCMD((char*)topicbuff, (uint8_t*)"OK", ENABLE_SIM_CHECKRES, ENABLE_MARKASREAD, MQTT_TIMEOUT_VERYSHORT) != SIM_RES_MSG)		return 0;

	if (SIM_sendCMD("AT+CMQTTSUB=0", (uint8_t*)"+CMQTTSUB: 0,0", ENABLE_SIM_CHECKRES,ENABLE_MARKASREAD, MQTT_TIMEOUT_SHORT)!= SIM_RES_MSG) 	return 0;

	return 1;
}

uint8_t checkSubcribe(uint8_t *topic)
{
	if (SIM_sendCMD("AT+CMQTTSUB?", topic,
			ENABLE_SIM_CHECKRES,ENABLE_MARKASREAD, MQTT_TIMEOUT_SHORT) != SIM_RES_MSG) 	return 0;
	return 1;
}

void MQTT_testReceive (void)
{


}
