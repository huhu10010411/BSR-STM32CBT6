/*
 * ServerMessage.h
 *
 *  Created on: Nov 7, 2023
 *      Author: Admin
 */

#ifndef INC_SERVERMESSAGE_H_
#define INC_SERVERMESSAGE_H_

#include "MQTT.h"
#include "linkedlist.h"
//#include "main.h"
#include "Step_motor.h"
#include "Contactor.h"




#define BROADCAST_ID 	0xFF
#define NODEID_POS		0
#define PACKT_POS		1
#define DATAT_POS		2
#define CMD_POS			2
#define DATA_POS		3
#define RESSTATUS_POS	2
#define DATAREST_POS	3
#define CMDREST_POS		3
#define ADDDATA_POS		3
#define MBA_STATE_POS	3
#define DIR_POS			3
#define CHANGE_MODEPOS	4
#define CHANGE_VALUE    5

#define USER_MSG_HEADER_NUMBOF_ACT_STATION		(char*)"ACTIVE STATION:"
#define USER_MSG_HEADER_NUMBOF_ACT_SENSOR		(char*)"ACTIVE SENSOR:"
#define USER_MSG_HEADER_NUMBOF_FAIL_STATION		(char*)"FAIL STATION:"
#define USER_MSG_HEADER_NUMBOF_FAIL_SENSOR		(char*)"FAIL SENSOR:"

#define PACKBUFF_MAXLEN		256

extern uint8_t Pack_buff [PACKBUFF_MAXLEN];

typedef enum {
	RES_OK = 0x01,
	RES_ERROR
}RES_STATUS_t;
typedef enum
{
	CMD_NONE,
	CMD_PREPARE_CALIB,
	CMD_START_CALIB ,
	CMD_SMS_CALIB,
	CMD_SMS_GETSTATUS,
	CMD_SLEEP,
	CMD_WAKEUP,
	CMD_CTRL_MBA,
	CMD_CTRL_STEP_MOTOR,
	CMD_SWITCH_MODE,
	CMD_GET_LATEST_DATA_SENSOR,
	CMD_GET_LASTEST_DATA_STATION,
	CMD_SYN_SENSOR
}CMD_t;

typedef enum
{
	PACKT_DATA= 0xF1,
	PACKT_CMD,
	PACKT_RESDATA,
	PACKT_RESCMD,
	PACKT_REGISTER,
	PACKT_RESREGISTER
}PACK_t;

typedef enum
{
	DATA_NONE,
	DATA_PERIOD,
	DATA_NETWREADY,
	DATA_STATUS,
	DATA_REGISTER,
	DATA_CALIB,
	DATA_LATEST,
	DATA_AFTERCALIB,
	DATA_MBA_STATE,
	DATA_STEP_REACH_LIMIT
}DATA_t;

void initServerMsg ();

void triggerCMD (CMD_t cmdType);

uint8_t Register2Server(void);

uint8_t sendCMDtoServer( CMD_t CMDtype,uint8_t *SMSdatabuffer, uint16_t datalen, MBA_state_t MBAstate,
		Stepmotor_dir_t Stepm_DIR, Stepmotor_change_mode_t Stepm_changeMode,
		uint16_t Stepm_changeValue );
uint8_t sendData2Server( DATA_t dataType);

uint8_t sendRespond(CMD_t cmdType, RES_STATUS_t resStatus);

void processingComingMsg(uint8_t *Msg, uint16_t Msg_len, uint8_t stID);

void testProcessingMsg(void);
#endif /* INC_SERVERMESSAGE_H_ */
