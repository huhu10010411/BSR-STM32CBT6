/*
 * App_SMS.c
 *
 *  Created on: Dec 30, 2023
 *      Author: Admin
 */

#include "App_SMS.h"
#include "SIM.h"
#include "ServerMessage.h"
#include "Task.h"
#include "main.h"

//static SMS_t *mySIM.sms;
void initApp_SMS()
{
	SIM_sendCMD((uint8_t*)"AT+CMGD=1,1", (uint8_t*)"OK",
				ENABLE_SIM_CHECKRES, ENABLE_MARKASREAD, 1000);
}

uint8_t SMSreturn (SMS_CMD_t smsCMD)
{
	SMS_request_t *tmpSMScmd;
	switch (smsCMD) 	{

	case SMS_CMD_GET_STATUS:
		tmpSMScmd = &mySIM.sms.GetStatus;
		break;
	case SMS_CMD_GET_STATION:
		tmpSMScmd = &mySIM.sms.GetStation;
		break;
	case SMS_CMD_GET_SENSOR:
		tmpSMScmd = &mySIM.sms.GetSensor;
		break;
	default:
		break;
	}
	if (tmpSMScmd == NULL) 	return 0;
//	uint8_t msglen = tmpSMScmd->datalength;
//	uint8_t returnmsg[msglen+1];
//	uint8_t phonenumb[13];
//	strcpy()
//	strcpy(returnmsg,tmpSMScmd->data);
	SMS_sendMsg(tmpSMScmd->data, tmpSMScmd->datalength, tmpSMScmd->phonenumb);
	return 1;
}
void processApp_SMS(void)
{
	processingSMS();

	// check for new SMS request
	if (checkSMSrequest(SMS_CMD_CTRL_ON) || checkSMSrequest(SMS_CMD_CTRL_OFF) )	{
		triggerTaskflag(TASK_SEND_CTRLMBA, FLAG_EN);
	}
	if (checkSMSrequest(SMS_CMD_CTRL_INC) || checkSMSrequest(SMS_CMD_CTRL_DEC) ) {
		triggerTaskflag(TASK_SEND_CTRLSTEPMOR, FLAG_EN);
	}
	if (checkSMSrequest(SMS_CMD_CTRL_CALIB))	{
		triggerTaskflag(TASK_SEND_CALIB, FLAG_EN);
	}
	if (checkSMSrequest(SMS_CMD_GET_STATUS))	{
		triggerTaskflag(TASK_SEND_GETSTATUS, FLAG_EN);
	}
	if (checkSMSrequest(SMS_CMD_GET_STATION))	{
		triggerTaskflag(TASK_SEND_GETSTATION, FLAG_EN);
	}
	if (checkSMSrequest(SMS_CMD_GET_SENSOR))	{
		triggerTaskflag(TASK_SEND_GETSENSOR, FLAG_EN);
	}
// Check for return SMS
	if (checkSMSreturn(SMS_CMD_GET_STATUS))	{
		SMSreturn(SMS_CMD_GET_STATUS);
		triggerSMSreturn(SMS_CMD_GET_STATUS, SMS_CMD_DISABLE);
	}
	if (checkSMSreturn(SMS_CMD_GET_STATION))	{
		SMSreturn(SMS_CMD_GET_STATION);
		triggerSMSreturn(SMS_CMD_GET_STATION, SMS_CMD_DISABLE);
	}
	if (checkSMSreturn(SMS_CMD_GET_SENSOR))	{
		SMSreturn(SMS_CMD_GET_SENSOR);
		triggerSMSreturn(SMS_CMD_GET_SENSOR, SMS_CMD_DISABLE);
	}


}

