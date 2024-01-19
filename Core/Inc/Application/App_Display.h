/*
 * App_Display.h
 *
 *  Created on: Jan 10, 2024
 *      Author: Admin
 */

#ifndef INC_APPLICATION_APP_DISPLAY_H_
#define INC_APPLICATION_APP_DISPLAY_H_

#include "ds3231.h"

typedef enum	{
	HOME,
	COUNTDOWN_SW_OFF,
	AFTER_SW_OFF,
	MONITOR,
	MENU_CONTROL,
	VOLTAGE_CONTROL,
	ON_OFF_CONTROL,
	COMPLETE_CONTROL,
	STEP_VAL_CONTROL,
	STEP_DIR_CONTROL
}DISPLAY_MODE_t;

extern DISPLAY_MODE_t myDisplayMode;

typedef enum {
	CONTROL_STEP,
	CONTROL_TRANS

}CONTROL_t;

typedef enum {
	SW_OFF,
	SW_ON
}SWITCH_t;

typedef enum {
	STEP_VAL,
	STEP_DIR,
	STEP_CONFIRM
}STEP_t;

extern uint8_t displayCalibFlag;
extern uint8_t displayAfterSwitchoff;

void initApp_Display ();

void processingApp_display();

void setSwitchtime();

_RTC getSwitchtime();
#endif /* INC_APPLICATION_APP_DISPLAY_H_ */
