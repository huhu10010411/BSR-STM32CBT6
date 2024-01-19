/*
 * App_Display.c
 *
 *  Created on: Jan 10, 2024
 *      Author: Admin
 */


#include "user_screen.h"
#include "App_Display.h"
#include "button.h"
#include "Step_motor.h"
#include "MQTT.h"



DISPLAY_MODE_t myDisplayMode = HOME;

uint8_t displayCalibFlag = 0;
uint8_t displayAfterSwitchoff = 0;

static uint16_t countdowntime;
static _RTC switchtime;

void initApp_Display ()
{
	LCD_Init();
	Screen_Init(&myRTC);
}

void display_SensorX(uint8_t sensorIndex)
{
	if (sensorIndex > myStation.ssNode_list->length)	{
		Screen_Monitor_Station(myStation.stID, myStation.stCurrent);
		return ;

	}

	Node * current = myStation.ssNode_list->head;
	for (uint8_t i = 0; i < sensorIndex; i++)
	{
		current = current->next;
	}
	Screen_Monitor_Node(current->SSnode.SSnode_ID, current->SSnode.sensorMode,
			current->SSnode.Battery, current->SSnode.V_type, current->SSnode.V_value);

}

void setSwitchtime()
{
	DS3231_GetTime(&switchtime);
}
_RTC getSwitchtime()
{
	return switchtime;
}
void processingApp_display()
{

	if (getClearflag()) 	{
		LCD_Clear();
		setClearflag(DISABLE);
	}
	switch(myDisplayMode)	{
		case HOME:
			Screen_Home_Origin(myStation.stID, MQTT_getConnectflag(), myStation.task.getGPStimeflag);
			break;
		case COUNTDOWN_SW_OFF:
			countdowntime = getCountdowntime(myStation.calibTime.hour,
												myStation.calibTime.min, myStation.calibTime.sec);
			Screen_Home_Sync(countdowntime);
			break;
		case AFTER_SW_OFF:
			displayAfterSwitchoff = 0;
			Screen_Home_Calib(myStation.calibTime.hour, myStation.calibTime.min, myStation.calibTime.sec +3);
			break;
		case MONITOR:

			if (get_curMonitor() == 0) 	{
				Screen_Monitor_Station(myStation.MBAstate, myStation.stCurrent);
			}
			else {
				display_SensorX(get_curMonitor());
			}
			break;
		case MENU_CONTROL:
			Screen_Function(getcurControl());
			break;
		case VOLTAGE_CONTROL:
			Screen_Voltage_Control(myMotor.limit, getStepValorDir());
			Screen_Voltage_Control_Control_Motor(myMotor.stepChange, myMotor.dir);
			break;
		case STEP_DIR_CONTROL:
			Screen_Dir_Control(myMotor.limit);
			Screen_Voltage_Control_Control_Motor(myMotor.stepChange, myMotor.dir);
			break;
		case STEP_VAL_CONTROL:
			Screen_StepVal_Control(myMotor.limit);
			Screen_Voltage_Control_Control_Motor(myMotor.stepChange, myMotor.dir);
			break;
		case ON_OFF_CONTROL:
			Screen_Control_Relay(1);
			Screen_Control_Relay_Change_Mode(myStation.MBAstate, getCurswitch());
			break;
		case COMPLETE_CONTROL:
			Screen_Accept_Relay(getSwitchtime(), getCurswitch());
			break;
		default:
			break;
		}
//	Screen_Home_Origin(myStation.stID);

//	Screen_Home_Sync(countdowntime);
}

