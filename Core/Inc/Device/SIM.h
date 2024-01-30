/*
 * SIM.h
 *
 *  Created on: Oct 9, 2023
 *      Author: Admin
 */

#ifndef INC_SIM_H_
#define INC_SIM_H_

#include "stm32f1xx.h"
#include "stdbool.h"
//#include "usart.h"
//#include "main.h"



#define SIM_STA_CSQ_CMD				"AT+CSQ"
#define SIM_NW_CREG_CMD				"AT+CREG?"
#define SIM_NW_CPSI_CMD				"AT+CGREG?"
#define SIM_PACKDOM_CGREG_CMD		"AT+CPSI?"

#define SMS_MSG_CTRL_ON			"CTRL+ON"
#define SMS_MSG_CTRL_OFF		"CTRL+OFF"
#define SMS_MSG_CTRL_CALIB		"CTRL+CALIB"
#define SMS_MSG_CTRL_INC		"CTRL+INC"
#define SMS_MSG_CTRL_DEC		"CTRL+DEC"
#define SMS_MSG_GET_STATUS		"GET+STATUSALL"
#define SMS_MSG_GET_SENSOR		"GET+SENSOR"
#define SMS_MSG_GET_STATION		"GET+STATION"

#define SMS_DATA_MAXLEN	256




#define ENABLE_SIM_CHECKRES	 	1
#define DISABLE_SIM_CHECKRES  	0
#define ENABLE_MARKASREAD		1
#define DISABLE_MARKASREAD		0

#define SIM_TIMEOUT_SHORT		500
#define SIM_TIMEOUT_MEDIUM		1000
#define SIM_TIMEOUT_LONG		2000

#define MAX_PAYLOAD_LEN		256
#define MAX_TOPIC_LEN		40

#define MQTT_RXBUFF_SIZE     300

#define PHONENUMB_LEN		13

//extern UART_HandleTypeDef *__SIM_UART;
//extern DMA_HandleTypeDef  *__SIM_DMA_UART;
extern uint8_t MQTT_Rxbuff [MQTT_RXBUFF_SIZE];

//typedef enum {
//	PDU,
//	TEXT
//}SMS_MSG_FORMAT_t;

typedef enum {
	SIM_CMD_SIMCARD_PIN,
	SIM_CMD_STA_CSQ,
	SIM_CMD_NW_CREG,
	SIM_CMD_NW_CPSI,
	SIM_CMD_PACKDOM_CGREG,
}SIM_CMD_t;
typedef enum {
	SIM_ERROR,
	SIM_RES_MSG,
	SIM_NO_RES
}SIM_res_t;

typedef enum {
	SMS_CMD_DISABLE,
	SMS_CMD_ENABLE
}SMS_CMD_FLAG_t;
typedef struct {
	uint8_t requestflag : 1;
	uint8_t returnflag : 1;
	uint16_t datalength;
	uint8_t data[SMS_DATA_MAXLEN];
	uint8_t phonenumb[PHONENUMB_LEN];
}SMS_request_t;
typedef struct
{
	SMS_request_t CtrlON;
	SMS_request_t CtrlOFF ;
	SMS_request_t CtrlCALIB ;
	SMS_request_t CtrlINC ;
	SMS_request_t CtrlDEC ;
	SMS_request_t GetStatus ;
	SMS_request_t GetSensor ;
	SMS_request_t GetStation ;
}SMS_t;

typedef enum {
	SMS_CMD_CTRL_ON,
	SMS_CMD_CTRL_OFF,
	SMS_CMD_CTRL_CALIB,
	SMS_CMD_CTRL_INC,
	SMS_CMD_CTRL_DEC,
	SMS_CMD_GET_STATUS,
	SMS_CMD_GET_SENSOR,
	SMS_CMD_GET_STATION,
	SMS_CMD_NONE
}SMS_CMD_t;

typedef struct {
    char *host;
    char *willtopic;
    uint8_t *willmsg;
    uint16_t port;
    uint8_t volatile connect;
} mqttServer_t;

typedef struct {
//    char *username;
//    char *pass;
    char clientID[20];
    unsigned short keepAliveInterval;
} mqttClient_t;

typedef struct {
    uint8_t newEvent;
    unsigned char dup;
    int qos;
    unsigned char retained;
    unsigned short msgId;
    uint8_t payload[MAX_PAYLOAD_LEN];
    uint16_t payloadLen;
    uint8_t topic[MAX_TOPIC_LEN];
    uint16_t topicLen;
} mqttReceive_t;

typedef struct {
    char *apn;
    char *apn_user;
    char *apn_pass;
} sim_t;

typedef struct {
//    sim_t sim;
    mqttServer_t mqttServer;
    mqttClient_t mqttClient;
    mqttReceive_t mqttReceive;
    SMS_t  sms;
} SIM_t;



//extern volatile uint8_t newMQTTmsg ;
/****************************************************/
void initSIM();

void SIM_callback(uint16_t Size);

void enableReceiveDMAtoIdle_SIM(void);

SIM_res_t SIM_sendCMD(const char *cmd, uint8_t *checkResMsg, uint8_t CheckResENorDIS, uint8_t ENorDISmarkasread,uint32_t timeout);

SIM_res_t SIM_checkMsg(uint8_t *Msg, uint16_t timeout);

SIM_res_t SIM_checkMQTTmess(uint8_t *Msg, uint8_t *mqttRxbuff);

void MarkAsReadData_SIM(void);

void SIM_checkOperation(void);

void triggerSMSrequest (SMS_CMD_t smsCMD, SMS_CMD_FLAG_t ENorDIS);

void triggerSMSreturn (SMS_CMD_t smsCMD, SMS_CMD_FLAG_t ENorDIS);

uint8_t SIM_checkCMD (SIM_CMD_t cmd);

uint8_t SMS_config();

uint8_t SMS_sendMsg(uint8_t *Msg, uint16_t msglen, uint8_t *phonenumber );

uint8_t processingSMS(void);

bool checkSMSrequest (SMS_CMD_t smsCMD);

bool checkSMSreturn (SMS_CMD_t smsCMD);

void testSMS(void);


#endif /* INC_SIM_H_ */
