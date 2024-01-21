/*
 * ServerMessage.c
 *
 *  Created on: Nov 7, 2023
 *      Author: Admin
 */
//#include "stm32f1xx_hal.h"
#include "ServerMessage.h"
#include "linkedlist.h"
#include <string.h>
#include "ds3231.h"
#include "time.h"
#include <stdio.h>
//#include "Serial_log.h"
#include "crc32.h"
#include "Task.h"
#include "String_process.h"
#include "Validation.h"
#include "main.h"
#include "App_Display.h"




#define DATABUFF_MAXLEN		512

uint8_t Pack_buff [PACKBUFF_MAXLEN];


void initServerMsg ()
{
//	myStation = Station;
//	mySIM.sms = mySMS;
//	mySIM = mySIM;
}

uint32_t buffer2num(uint8_t *buffer)
{
	return (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
}

uint8_t addCRCtoPack(uint8_t *dataBuff, uint8_t bufflen)
{
	uint32_t crc;
//	MQTT_publish( (uint8_t*)TOPIC_PUB, dataBuff, bufflen);
	crc= crc32( (char*)dataBuff, bufflen);

	uint8_t crc_buff[4];
	FourbytenumbertoBuff( crc, crc_buff);

//	MQTT_publish( (uint8_t*)TOPIC_PUB, crc_buff, 4);

	memcpy(dataBuff+bufflen,crc_buff,4);

	return 4;    // crc length
}
uint8_t addTimetobuff(uint8_t *buff, uint16_t position)
{
	_RTC RTCtime ;
//	RTCtime.Year= 23;
//	RTCtime.Month=11;
//	RTCtime.Date = 15;
//	RTCtime.Hour = 7;
//	RTCtime.Min = 56;
//	RTCtime.Sec = 35;
	if (DS3231_GetTime(&RTCtime))
	{
		time_t epochtime = RTC2epochtime(&RTCtime);
		uint8_t time_buff[4];
		FourbytenumbertoBuff((uint32_t)epochtime,time_buff);
		memcpy(buff+position,time_buff,4);
		return 1;
	}
	return 0;
}

uint8_t createPack(PACK_t PackType, DATA_t DataType, CMD_t CMDType )
{
	memset(Pack_buff, 0, PACKBUFF_MAXLEN);
	uint8_t pack_len = 0;

	Pack_buff[pack_len++] = myStation.stID;
	Pack_buff[pack_len++] = PackType;

	if ( DataType != DATA_NONE )
	{
		Pack_buff[pack_len++] = DataType;
	}
	else if ( CMDType != CMD_NONE )
	{
		Pack_buff[pack_len++] = CMDType;
	}
	return pack_len;
}

uint16_t Serialize_SSnodedata(uint8_t* Serial_buff, DATA_t dataType)
{

	uint16_t buff_len = 0;
	Node * current =myStation.ssNode_list->head->next;
	while (current != myStation.ssNode_list->tail)
	{
		switch ( dataType ) {
		case DATA_REGISTER:
			Serial_buff[buff_len++] = current->SSnode.SSnode_ID;
			Serial_buff[buff_len++]= current->SSnode.Battery;
			Serial_buff[buff_len++]= current->SSnode.V_type;
			break;
		case DATA_PERIOD:
			Serial_buff[buff_len++] = current->SSnode.SSnode_ID;
			Serial_buff[buff_len++] = current->SSnode.Battery;
			Serial_buff[buff_len++] = ( current->SSnode.V_value >> 8 ) & 0xFF;
			Serial_buff[buff_len++] = current->SSnode.V_value & 0xFF;
			Serial_buff[buff_len++] = current->SSnode.sensorMode;
			break;
		case DATA_NETWREADY:
//			if (current->SSnode.sensorMode == SLEEP) 	{
//				current->SSnode.Sensor_state = SENSOR_DEACTIVE;
//			}
			Serial_buff[buff_len++] = current->SSnode.SSnode_ID;
			Serial_buff[buff_len++]= current->SSnode.Ready;
			current->SSnode.Ready = 0;
			myStation.prepare_flag = 0;
			break;
		case DATA_CALIB:
			if (current->SSnode.dataCalibAvailable)	{
				Serial_buff[buff_len++] = current->SSnode.SSnode_ID;
				memcpy(Serial_buff + buff_len, current->SSnode.dataCalibBuffer, 100);
				buff_len += 100;
				current->SSnode.dataCalibAvailable = 0;
//				current->SSnode.sentDatacalib = 1;
			}
			break;
//		case DATA_AFTERCALIB:
//			break;
		default:
			break;
		}
		current = current->next;
	}
//	Serial_buff[buff_len] = '\0';
	return buff_len;
}

uint8_t Serialize_Stationdata( uint8_t *Buffer, DATA_t dataType)
{
	uint8_t buff_len = 0;
	switch (dataType) {
		case DATA_REGISTER:
			Buffer[buff_len++] = myStation.stID;
			Buffer[buff_len++] = (uint8_t)( ( myStation.stCurrent >> 8 ) & 0xFF);
			Buffer[buff_len++] = (uint8_t)( myStation.stCurrent & 0xFF );
			Buffer[buff_len++] = (uint8_t) (myStation.MBAstate);
			Buffer[buff_len++] = (uint8_t)( ( myStation.stVoltage >> 8 ) & 0xFF );
			Buffer[buff_len++] = (uint8_t)( myStation.stVoltage & 0xFF );
			break;
		case DATA_NETWREADY:
			Buffer[buff_len++] = myStation.stID;
			Buffer[buff_len++] = (uint8_t) ACTIVE;
			break;
		case DATA_PERIOD:
			Buffer[buff_len++] = myStation.stID;
			Buffer[buff_len++] = (uint8_t)( ( myStation.stCurrent >> 8 ) & 0xFF);
			Buffer[buff_len++] = (uint8_t)( myStation.stCurrent & 0xFF );
			Buffer[buff_len++] = (uint8_t)(myStation.MBAstate);
			Buffer[buff_len++] = (uint8_t)( ( myStation.stVoltage >> 8 ) & 0xFF );
			Buffer[buff_len++] = (uint8_t)( myStation.stVoltage & 0xFF );
			break;
		case DATA_CALIB:
			Buffer[buff_len++] = myStation.stID;
			Buffer[buff_len++] = (uint8_t)( ( myStation.stCurrent >> 8 ) & 0xFF);
			Buffer[buff_len++] = (uint8_t)( myStation.stCurrent & 0xFF );
			Buffer[buff_len++] = (uint8_t)( ( myStation.stVoltage >> 8 ) & 0xFF );
			Buffer[buff_len++] = (uint8_t)( myStation.stVoltage & 0xFF );
			break;
//		case DATA_AFTERCALIB:
//			Buffer[buff_len++] = (uint8_t)( ( myStation.stCurrent >> 8 ) & 0xFF);
//			Buffer[buff_len++] = (uint8_t)( myStation.stCurrent & 0xFF );
//			break;
		case DATA_MBA_STATE:
			Buffer[buff_len++] = (uint8_t) (myStation.MBAstate);
			break;
		case DATA_STEP_REACH_LIMIT:
			Buffer[buff_len++] = (uint8_t)myMotor.limit;
			break;
		default:
			break;
	}

	return buff_len;
}
uint8_t Serialize_Data(uint8_t *databuff, DATA_t dataType)
{
	uint16_t data_len = 0 ;
	data_len = Serialize_Stationdata(databuff, dataType );
	data_len += Serialize_SSnodedata(databuff + data_len, dataType);
	if ( !addTimetobuff( databuff, data_len ) )		return 0;
	data_len += 4;      // 4 bytes of time + buffer length
	return data_len;
}

uint8_t addDatatoPack(uint8_t *Pack, uint16_t Pack_len, uint8_t *Databuff, uint16_t dataLen)
{
	memcpy(Pack+ Pack_len, Databuff, dataLen);
	return dataLen ;
}

uint8_t Serialize_addtionaldata( CMD_t CMDtype, uint8_t *Getbuffer, uint8_t *databuffer, uint16_t datalen, MBA_state_t MBAstate, Stepmotor_dir_t Stepm_DIR,
							Stepmotor_change_mode_t Stepm_changeMode, uint16_t Stepm_changeValue )
{
	uint8_t buff_len = 0;
	switch ( CMDtype ) {
		case CMD_SMS_CALIB:
			// Time delay + Phone number
			memcpy(Getbuffer + buff_len, databuffer, datalen);
			buff_len += datalen;
			break;
		case CMD_SMS_GETSTATUS:
			// Phone number	(11 byte)
			memcpy(Getbuffer + buff_len, databuffer, datalen);
			buff_len += datalen;
			break;
		case CMD_CTRL_MBA:
			// First byte: ON/OFF
			Getbuffer[buff_len++] = MBAstate;
			// Station ID list + time delay + phone number
			memcpy(Getbuffer + buff_len, databuffer, datalen);
			buff_len += datalen;
			break;
		case CMD_CTRL_STEP_MOTOR:
			// First byte: Increase/Decrease (1 byte)
			Getbuffer[buff_len++] = Stepm_DIR ;
			// Second byte: Change Percentage/Step (1 byte)
			Getbuffer[buff_len++] = Stepm_changeMode;
			// Third + fourth byte : Value
//			Getbuffer[buff_len++] = Stepm_changeValue;
			Getbuffer[buff_len++] = (uint8_t)( ( Stepm_changeValue >> 8 ) & 0xFF);
			Getbuffer[buff_len++] = (uint8_t)( Stepm_changeValue & 0xFF );
			// Station ID list + time delay + phone number
			memcpy(Getbuffer + buff_len, databuffer, datalen);
			buff_len += datalen;
			break;
		case CMD_GET_LATEST_DATA_SENSOR:
			// Sensor ID list + phone number
			memcpy(Getbuffer + buff_len, databuffer, datalen);
			buff_len += datalen;
			break;
		case CMD_GET_LASTEST_DATA_STATION:
			// Station ID list + phone number
			memcpy(Getbuffer + buff_len, databuffer, datalen);
			buff_len += datalen;
			break;
		default:
			break;
	}
	return buff_len;
}

uint8_t Register2Server()
{
	uint8_t tmp_databuff[128] ;
	uint16_t pack_len = 0;
	// Create package
	pack_len = createPack(PACKT_REGISTER, DATA_REGISTER, CMD_NONE );
	if ( !pack_len ) {
//		free(tmp_databuff);
		return 0;
		}
	 //Convert Register data to temp buffer
	uint8_t datalen= Serialize_Data(tmp_databuff, DATA_REGISTER );
	if ( !datalen )	{
//		free(tmp_databuff);
		return 0;
	}
	// Add Register data  temp buffer to package
	pack_len += addDatatoPack( Pack_buff, pack_len, tmp_databuff, datalen );
	// Add CRC to package
	pack_len += addCRCtoPack( Pack_buff, pack_len );
	// Publish
	if ( MQTT_publish((uint8_t*)TOPIC_PUB, Pack_buff, pack_len) )	{
//		Serial_log_string(" Sent \"Register\" message to server\r\n");
//		free(tmp_databuff);
		return 1;
	}
//	free(tmp_databuff);
	return 0;
}

uint8_t sendCMDtoServer(CMD_t CMDtype, uint8_t *SMSdatabuffer, uint16_t datalen, MBA_state_t MBAstate,
		Stepmotor_dir_t Stepm_DIR, Stepmotor_change_mode_t Stepm_changeMode,
		uint16_t Stepm_changeValue)
{
	uint8_t  pack_len = 0;
	pack_len = createPack(PACKT_CMD, DATA_NONE, CMDtype );
	if ( !pack_len )	return 0; // Create package

	uint8_t tmpAbuff [256];
	uint8_t Abuff_len = Serialize_addtionaldata(CMDtype, tmpAbuff, SMSdatabuffer, datalen, MBAstate, Stepm_DIR, Stepm_changeMode, Stepm_changeValue);
	pack_len += addDatatoPack(Pack_buff, pack_len, tmpAbuff, Abuff_len);    // Add additional data to package

	pack_len += addCRCtoPack( Pack_buff, pack_len );		// Add CRC to package

	if ( !MQTT_publish( (uint8_t*)TOPIC_PUB, Pack_buff , pack_len ) )	return 0;
	return 1;
}

uint8_t sendData2Server( DATA_t dataType)
{
	uint8_t databuff[DATABUFF_MAXLEN];
	uint16_t data_len = 0;
	memset(databuff, 0, DATABUFF_MAXLEN);
	memset(Pack_buff, 0, PACKBUFF_MAXLEN);
	// Creat package
	uint16_t pack_len = createPack(PACKT_DATA, dataType, CMD_NONE );
	// Add data to package
	data_len = Serialize_Data(databuff, dataType );
	pack_len += addDatatoPack(Pack_buff, pack_len, databuff, data_len);
	// Add CRC to package
	pack_len += addCRCtoPack(Pack_buff, pack_len);
	// Publish
	if ( !MQTT_publish( (uint8_t*)TOPIC_PUB, Pack_buff, pack_len) ) return 0;
	return 1;
}
//
//uint8_t sendRespond(CMD_t cmdType, RES_STATUS_t resStatus)
//{
//	uint16_t packlen = 0;
//	memset(Pack_buff, 0, PACKBUFF_MAXLEN);
//	packlen += createPack(PACKT_RESCMD, DATA_NONE, CMD_NONE);
//	Pack_buff[packlen++] = resStatus;
//	Pack_buff[packlen++] = cmdType;
//	switch (cmdType)	{
//	case CMD_CTRL_MBA:
//		Pack_buff[packlen++] = myStation.MBAstate;
//		break;
//	case CMD_CTRL_STEP_MOTOR:
//		twobyte2buff(Pack_buff + packlen, myStation.stepPosition);
//		packlen+= 2;
//		break;
//	default:
//		break;
//	}
//	packlen += addCRCtoPack(Pack_buff, packlen);
//
//	if ( !MQTT_publish( (uint8_t*)TOPIC_PUB, Pack_buff, packlen) ) return 0;
//	return 1;
//}
uint8_t checkCRC(uint8_t *buffer, uint16_t bufferlen)
{
	if ( buffer2num(buffer + bufferlen - 4) != crc32( (char*)buffer, bufferlen - 4) ) return 0;
	return 1;
}

uint8_t checkNodeID(uint8_t *Msg, uint8_t nodeID)
{
	if ( Msg[NODEID_POS] != nodeID && Msg[NODEID_POS] != BROADCAST_ID )		return 0;
	return 1;
}

PACK_t checkPacktype(uint8_t *Msg)
{
	return Msg[PACKT_POS];
}

DATA_t checkDatatype(uint8_t *Msg)
{
	return Msg[DATAT_POS];
}
DATA_t checkDataREStype(uint8_t *Msg)
{
	return Msg[DATAREST_POS];
}
CMD_t checkCmdREStype( uint8_t *Msg)
{
	return Msg[CMDREST_POS];
}
CMD_t checkCMDtype (uint8_t *Msg)
{
	return Msg[CMD_POS];
}


void getDataStatus(uint8_t *Msg, uint16_t Msglen)
{
	uint8_t numbofActiveStation;
	uint8_t numbofActiveSensor;
	uint8_t numbofFailStation;
	uint8_t numbofFailSensor;
	uint8_t datapos = DATA_POS;
	numbofActiveStation = Msg[datapos++];
	numbofFailStation = Msg[datapos++];
	numbofActiveSensor = Msg[datapos++];
	numbofFailSensor = Msg[datapos++];
	memset(mySIM.sms.GetStatus.data, 0, SMS_DATA_MAXLEN);
	uint16_t len = sprintf((char*)mySIM.sms.GetStatus.data,"%s %d,%s %d,%s %d,%s %d.",
			USER_MSG_HEADER_NUMBOF_ACT_STATION, numbofActiveStation,
			USER_MSG_HEADER_NUMBOF_FAIL_STATION, numbofFailStation,
			USER_MSG_HEADER_NUMBOF_ACT_SENSOR, numbofActiveSensor,
			USER_MSG_HEADER_NUMBOF_FAIL_SENSOR, numbofFailSensor);
	mySIM.sms.GetStatus.datalength = len;
}

ID_t getDatalatest(uint8_t *Msg, uint16_t Msg_len)
{
	uint8_t datapos = DATA_POS;
	uint16_t crcpos = Msg_len - 4;
	uint8_t len ;
	uint16_t tmpvalue;

	// get ID type ( Station or Sensor)
	ID_t IDtype = Msg[datapos++];

	switch (IDtype){
	case ID_STATION:
		mySIM.sms.GetStation.datalength = 0;
		while (datapos < crcpos)
		{
			// Get value to SMS return buffer
			len = sprintf((char*)(mySIM.sms.GetStation.data + mySIM.sms.GetStation.datalength), "I%d:", Msg[datapos]);
			mySIM.sms.GetStation.datalength += len;
			datapos++;
			tmpvalue = buff2twobyte(Msg+ datapos);
			len = sprintf((char*)(mySIM.sms.GetStation.data + mySIM.sms.GetStation.datalength), "%d;", tmpvalue);
			mySIM.sms.GetStation.datalength += len;
			datapos += 2;
		}
		break;
	case ID_SENSOR:
		// Get value to SMS return buffer
		mySIM.sms.GetSensor.datalength = 0;
		while (datapos < crcpos){

		len = sprintf((char*)(mySIM.sms.GetSensor.data + mySIM.sms.GetSensor.datalength), "V%d", Msg[datapos]);
		mySIM.sms.GetSensor.datalength += len;
		datapos++;
		switch (Msg[datapos++])	{
		case V_p:
			len = sprintf((char*)(mySIM.sms.GetSensor.data + mySIM.sms.GetSensor.datalength), "p:");
			mySIM.sms.GetSensor.datalength += len;
			break;
		case V_na:
			len = sprintf((char*)(mySIM.sms.GetSensor.data + mySIM.sms.GetSensor.datalength), "na:");
			mySIM.sms.GetSensor.datalength += len;
			break;
		default:
			break;
		}
		tmpvalue = buff2twobyte(Msg+ datapos);
		len = sprintf((char*)(mySIM.sms.GetSensor.data + mySIM.sms.GetSensor.datalength), "%d,", tmpvalue);
		mySIM.sms.GetStation.datalength += len;
		datapos += 2;
		}
		break;
	default:
		break;
	}
	return IDtype;
}

static MBA_state_t getMBAstate(uint8_t *Msg)
{
	MBA_state_t res = MBA_NULL;
	switch (Msg[MBA_STATE_POS])		{
	case MBA_OFF:
		res = MBA_OFF;
		break;
	case MBA_ON:
		res = MBA_ON;
		break;
	default:
		break;
	}
	return res;
}
static void  getCtrlStepinfor(uint8_t *Msg)
{
	Stepmotor_dir_t dir = Msg[DIR_POS];
	if (dir < STEPM_DIR_INC && dir > STEPM_DIR_DEFAULT )	return;
	Stepmotor_change_mode_t changemode = Msg[CHANGE_MODEPOS];
	if (changemode < STEPM_MODE_PERCENTAGE && changemode > STEPM_MODE_DEFAULT) return;
	uint8_t valuebuff[3] ;
	memset(valuebuff, 0, 3);
	memcpy(valuebuff, Msg + CHANGE_VALUE, 2);
	uint16_t value = buff2twobyte(valuebuff);

	myMotor.dir = dir;
	myMotor.mode = changemode;
	if (changemode == STEPM_MODE_PERCENTAGE)	{
		myMotor.steppercentChange = value;
	}
	else {
		myMotor.stepChange = value;
	}

}

void processingComingMsg(uint8_t *Msg, uint16_t Msg_len, uint8_t stID)
{
	if ( !checkCRC(Msg, Msg_len) )	return;

	if ( !checkNodeID(Msg, stID) )	return;

	PACK_t packageType = checkPacktype(Msg);
	DATA_t dataType;
	CMD_t cmdType;
	DATA_t dataREStype;
	ID_t idType;
	uint32_t tmptime;
	_RTC tmpRTC;

	switch (packageType) {
		case PACKT_DATA:
			// Check data type
			dataType = checkDatatype(Msg);
			switch (dataType) {

				case DATA_STATUS:

					//Get status data from package
					getDataStatus(Msg, Msg_len);

					// Trigger return SMS task
					triggerSMSreturn(SMS_CMD_GET_STATUS, SMS_CMD_ENABLE);
					break;

				case DATA_LATEST:

					//Get data from package
					idType = getDatalatest(Msg, Msg_len);

					// Trigger return SMS task
					if (idType == ID_SENSOR)	{
						triggerSMSreturn(SMS_CMD_GET_SENSOR, SMS_CMD_ENABLE);
					}
					else if (idType == ID_STATION)	{
						triggerSMSreturn(SMS_CMD_GET_STATION, SMS_CMD_DISABLE);
					}
					break;

				default:
					break;
			}
			break;
		case PACKT_CMD:
			cmdType = checkCMDtype(Msg);
			switch (cmdType) {
				case CMD_PREPARE_CALIB:
					triggerTaskflag(TASK_PREPARE_CALIB, FLAG_EN);
					break;
				case CMD_START_CALIB:
					// Get time in package
					 tmptime = buff2Fourbyte( Msg+ (uint8_t)ADDDATA_POS );
					epochtine2RTC(tmptime, &tmpRTC);

					// Save Calib time
					myStation.calibTime.hour = tmpRTC.Hour;
					myStation.calibTime.min = tmpRTC.Min;
					myStation.calibTime.sec = tmpRTC.Sec;

					// Set alarm for Calib
					DS3231_ClearAlarm1();
					DS3231_SetAlarm1(ALARM_MODE_ALL_MATCHED, tmpRTC.Date, tmpRTC.Hour, tmpRTC.Min, tmpRTC.Sec);


					break;
				case CMD_CTRL_MBA:
					//Get MBA state from package and switch contactor
					myStation.MBAstate = switchContactor(getMBAstate(Msg));
					break;
				case CMD_CTRL_STEP_MOTOR:
					//Get data to control step motor
					getCtrlStepinfor(Msg);
					triggerTaskflag(TASK_CTRL_STEPMOR, FLAG_EN);
					break;
				case CMD_SYN_SENSOR:
					// get synchronize time and set alarm
					tmptime = buff2Fourbyte( Msg+ (uint8_t)ADDDATA_POS );
					epochtine2RTC(tmptime, &tmpRTC);
					// set synchronize sensor flag
					sync_flag = 1;

					//
					triggerTaskflag(TASK_GET_GPS_TIME, FLAG_EN);
					// Set alarm for synchronize
					DS3231_ClearAlarm1();
					DS3231_SetAlarm1(ALARM_MODE_ALL_MATCHED, tmpRTC.Date, tmpRTC.Hour, tmpRTC.Min, tmpRTC.Sec);
					break;
				default:
					break;
			}
			break;
		case PACKT_RESDATA:
			dataREStype = checkDataREStype(Msg);
			switch (dataREStype) {
			case DATA_PERIOD:
				if ( Msg[RESSTATUS_POS] == RES_OK)	{
					triggerTaskflag(TASK_SEND_DATAPERIOD, FLAG_DIS);
				}
				break;
			case DATA_NETWREADY:
				if ( Msg[RESSTATUS_POS] == RES_OK)	{
					triggerTaskflag(TASK_SEND_NWREADY, FLAG_DIS);

				}
				break;
			case DATA_CALIB:
				if ( Msg[RESSTATUS_POS] == RES_OK)	{
					triggerTaskflag(TASK_SEND_DATACALIB, FLAG_DIS);
				}
				break;
			default:
				break;
			}
			break;
//		case PACKT_RESCMD:
//			cmdType = Msg[3];
//			switch (cmdType)	{
//			case CMD_CTRL_MBA:
//				if (Msg[RESSTATUS_POS] == RES_OK)	{
//					uint8_t len= sprintf((char*)mySIM.sms.CtrlON.data,"Ctrl MBA success");
//					mySIM.sms.CtrlON.datalength = len;
//					triggerSMSreturn(SMS_CMD_CTRL_ON, ENABLE);
//				}
//				break;
//			case CMD_CTRL_STEP_MOTOR:
//				if (Msg[RESSTATUS_POS] == RES_OK)	{
//					uint8_t len= sprintf((char*)mySIM.sms.CtrlDEC.data,"Ctrl StepMor success");
//					mySIM.sms.CtrlDEC.datalength = len;
//					triggerSMSreturn(SMS_CMD_CTRL_DEC, ENABLE);
//				}
//				break;
//			case CMD_SMS_CALIB:
//
//				break;
//			default:
//				break;
//			}
//			break;
		case PACKT_RESREGISTER:
			// Get Register status
			if (Msg[RESSTATUS_POS] == RES_OK)	{
				triggerTaskflag(TASK_REGISTER, FLAG_DIS);
			}
			break;
		default:
			break;
	}
}

void testProcessingMsg(void)
{
	uint8_t Msg[] = {0x1e, 0xf6, 0x01, 0xdc, 0x18, 0x21, 0xc5};
//	Serial_log_number(myStation.task.register2server);
	processingComingMsg(Msg, 7, 0x1e);
//	Serial_log_number(myStation.task.register2server);
}
