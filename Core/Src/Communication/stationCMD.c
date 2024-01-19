/*
 * stationCMD.c
 *
 *  Created on: Dec 10, 2023
 *      Author: Admin
 */

#include "stationCMD.h"
#include "Serial_log.h"


uint8_t CMD_SMS_calib (uint8_t *cmddata, uint16_t datalen)
{
	if (sendCMDtoServer( CMD_SMS_CALIB, cmddata, datalen, MBA_NULL,
			STEPM_DIR_DEFAULT , STEPM_MODE_DEFAULT, 0) )	{
		Serial_log_string("Sent \"Calibration\" message to server\r\n");
		return 1;
	}
	return 0;
}

uint8_t CMD_SMS_getStatus ( uint8_t *cmddata, uint16_t datalen)
{
	if (sendCMDtoServer(CMD_SMS_GETSTATUS, cmddata, datalen, MBA_NULL,
			STEPM_DIR_DEFAULT , STEPM_MODE_DEFAULT, 0) )	{
		Serial_log_string("Sent \"Get status\" message to server\r\n");
		return 1;
	}
	return 0;
}

uint8_t CMD_SMS_Ctrl_MBA ( uint8_t *cmddata, uint16_t datalen, MBA_state_t MBAstate)
{
	if (sendCMDtoServer(CMD_CTRL_MBA, cmddata, datalen, MBAstate,
			STEPM_DIR_DEFAULT , STEPM_MODE_DEFAULT, 0) )	{
		Serial_log_string("Sent \"Control MBA\" message to server\r\n");
		return 1;
	}
	return 0;


}

uint8_t CMD_SMS_Ctrl_StepM ( uint8_t *cmddata, uint16_t datalen, Stepmotor_dir_t stepDIR, Stepmotor_change_mode_t stepMode, uint8_t changeValue)
{
	if (sendCMDtoServer(CMD_CTRL_STEP_MOTOR, cmddata, datalen, MBA_NULL,stepDIR , stepMode, changeValue) )	{
		Serial_log_string("Sent \"Control Step motor\" message to server\r\n");
		return 1;
	}
	return 0;
}

uint8_t CMD_SMS_getStationLatestData ( uint8_t *cmddata, uint16_t datalen)
{
	if (sendCMDtoServer(CMD_SMS_GETSTATUS, cmddata, datalen, MBA_NULL,
			STEPM_DIR_DEFAULT , STEPM_MODE_DEFAULT, 0) )	{
		Serial_log_string("Sent \"Get latest station data\" message to server\r\n");
		return 1;
	}
	return 0;
}

uint8_t CMD_SMS_getSensorLatestData ( uint8_t *cmddata, uint16_t datalen)
{
	if (sendCMDtoServer( CMD_SMS_GETSTATUS, cmddata, datalen, MBA_NULL,STEPM_DIR_DEFAULT , STEPM_MODE_DEFAULT, 0) )
		return 1;
	return 0;
}

//uint8_t CMD_sleep(void)
//{
//	if (sendCMDtoServer( CMD_SLEEP, NULL, 0, NULL, MBA_NULL,STEPM_DIR_DEFAULT , STEPM_MODE_DEFAULT, 0) )
//		return 1;
//	return 0;
//}
//
//uint8_t CMD_wakeup(void)
//{
//	if (sendCMDtoServer(CMD_WAKEUP, NULL, 0, NULL, MBA_NULL,STEPM_DIR_DEFAULT , STEPM_MODE_DEFAULT, 0) )
//		return 1;
//	return 0;
//}
