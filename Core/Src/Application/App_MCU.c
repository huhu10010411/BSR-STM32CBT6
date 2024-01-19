/*
 * App_MCU.c
 *
 *  Created on: Dec 27, 2023
 *      Author: Admin
 */


#include "App_MCU.h"
#include "ds3231.h"
//#include "Serial_CFG.h"
#include "Task.h"
#include "main.h"
#include "Lora.h"
#include "ServerMessage.h"
#include "App_MQTT.h"
#include "App_Display.h"

//static SIM_t *mySIM;

#define TIMEOUT_WAIT_DATACALIB		100000            //100s


uint32_t tick = 0;
uint32_t NWRDtick = 0;
uint32_t tickSendDataCalib = 0;
//uint8_t wake_sensorflag = 0 ;

void initApp_MCU()
{
//	mySIM = mySIM;
}

void setSensorMode (sensor_mode_t mode)
{
	Node * current =myStation.ssNode_list->head->next;
		while (current != myStation.ssNode_list->tail)	{
			if (current->SSnode.sensorMode == mode ) 	{
			}
			current = current->next;
		}
}
void processApp_MCU(void)
{
		/*Check for task*/
		if ( checkTaskflag(TASK_PREPARE_CALIB) )	{

			//Synchronize RTC time for Station and Sensor
			triggerTaskflag(TASK_GET_GPS_TIME, FLAG_EN);

			// Send WAKEUP command for Sensor
			Lora_Setmode(WAKE , 0);

			// This flag for check Sensor ready
			myStation.prepare_flag = 1;

			// Start timer 100s
			NWRDtick = HAL_GetTick();

			// Trigger send NETWORK READY message to Server
			triggerTaskflag(TASK_SEND_NWREADY, FLAG_EN);

			triggerTaskflag(TASK_PREPARE_CALIB, FLAG_DIS);
		}
		// Send Netwok Ready to Server after 100s since received Prepare calib command
		if ((HAL_GetTick() - NWRDtick > 100000) && checkTaskflag(TASK_SEND_NWREADY))	{
			sendData2Server(DATA_NETWREADY);
		}

		if( checkTaskflag(TASK_START_CALIB) ) {

			// Set display mode
			displayCalibFlag = 1;

			// Trigger task Send DATA CALIBRATION to Server
			triggerTaskflag(TASK_SEND_DATACALIB, FLAG_EN);
			tickSendDataCalib = HAL_GetTick();
			triggerTaskflag(TASK_START_CALIB, FLAG_DIS);
		}

		// Send data calib to Server after TIMEOUT wait for Data Calib from Sensors
		if ( (HAL_GetTick() - tickSendDataCalib >= TIMEOUT_WAIT_DATACALIB) && checkTaskflag(TASK_SEND_CALIB))	{
			sendData2Server(DATA_CALIB);
			// Set mode of Sensor to Sleep
			setSensorMode(SLEEP);
		}

		// Send data period
		if (HAL_GetTick() - tick >= UPDATE_DATA_PERIOD *1000)	{
			triggerTaskflag(TASK_SEND_DATAPERIOD, FLAG_EN);
			tick = HAL_GetTick();
		}

}
