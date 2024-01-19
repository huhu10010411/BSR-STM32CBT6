/**
  * @file    user_screen.h
  * @author  Sagit
  * @date    Jan 7, 2024
  * @brief   
  */

#ifndef USER_APP_INC_USER_SCREEN_H_
#define USER_APP_INC_USER_SCREEN_H_

/* INCLUDES ------------------------------------------------------------------*/
#include "user_lcd1604.h"
#include "ds3231.h"
//#include "user_event_drive.h"

/* MACROS AND DEFINES --------------------------------------------------------*/
typedef enum
{
	_SCREEN_HOME_ORIGIN = 0,
	_SCREEN_HOME_SYNC,
	_SCREEN_HOME_CALIB,
	_SCREEN_MONITOR_STATION,
	_SCREEN_MONITOR_NODE,
	_SCREEN_FUNCTION,
	_SCREEN_VOLTAGE_CONTROL,
	_SCREEN_CONTROL_RELAY,
	_SCREEN_ACCEPT_RELAY,
} eKindScreen;

/* TYPEDEFS ------------------------------------------------------------------*/

/* EXTERNAL VARIABLES --------------------------------------------------------*/

/* GLOBAL FUNCTION PROTOTYPES ------------------------------------------------*/
void Screen_Init(_RTC *myRTC);

void Screen_Home_Origin(uint8_t ID, uint8_t connectflag, uint8_t gpsflag);
void Screen_Home_Sync(uint16_t time);
void Screen_Home_Calib(uint8_t hour, uint8_t min, uint8_t sec);
void Screen_Monitor_Station(uint8_t mode, uint16_t current);
void Screen_Monitor_Node(uint8_t ID, uint8_t mode, uint16_t battery, uint8_t type, uint16_t voltage);
void Screen_Function(uint8_t position);
void Screen_Voltage_Control(uint8_t limit , uint8_t ValorDir);
void Screen_Dir_Control(uint8_t limit);
void Screen_StepVal_Control(uint8_t limit);
void Screen_Control_Relay(uint8_t position);
void Screen_Accept_Relay(_RTC switchtime, uint8_t status);
void Screen_Test(uint8_t button);

void Screen_Home_Sync_Set_Countdown(uint8_t time);
void Screen_Voltage_Control_Control_Motor(uint16_t step, uint8_t dir);
void Screen_Control_Relay_Change_Mode(uint8_t curStatus, uint8_t status);
#endif /*USER_APP_INC_USER_SCREEN_H_*/

/* End of file ----------------------------------------------------------------*/
