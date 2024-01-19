/*
 * button.c
 *
 *  Created on: Jan 10, 2024
 *      Author: Admin
 */


#include "button.h"
#include "Task.h"
#include  "user_lcd1604.h"
#include "user_a4988.h"
#include "Step_motor.h"
#include "App_Display.h"



static CONTROL_t curControl = CONTROL_STEP;
static SWITCH_t curSwitch = SW_ON;
static uint8_t volatile curMonitor = 0;

static uint8_t volatile clearflag = 0;

//static uint8_t clearPosX = 0;
//static uint8_t clearPosY = 0;
//static uint8_t clearPosflag = 0;

static STEP_t stepValorDir = STEP_VAL;


void initButton()
{

}

void setStepValorDir (STEP_t ValorDir)
{
	stepValorDir = ValorDir;
}

STEP_t getStepValorDir()
{
	return stepValorDir;
}

SWITCH_t getCurswitch()
{
	return curSwitch;
}
void setCurswitch(SWITCH_t ONorOFF)
{
	curSwitch = ONorOFF;
}


uint8_t getClearflag()
{
	return clearflag;
}

void setClearflag(uint8_t ENorDIS)
{
	clearflag = ENorDIS;
}


void switchcurControl()
{
	switch (curControl)	{
	case CONTROL_TRANS:
		curControl = CONTROL_STEP;
		break;
	case CONTROL_STEP:
		curControl = CONTROL_TRANS;
		break;
	default:
		break;
	}
}

CONTROL_t getcurControl()
{
	return curControl;
}

void nextMonitor(uint8_t volatile  * curMonitor, uint8_t max)	{
	if (*curMonitor >= max )		{
		*curMonitor = 0;
	}
	else {
		(*curMonitor)++;
	}
}

void previousMonitor(uint8_t volatile  * curMonitor, uint8_t max)	{
	if (*curMonitor == 0 )		{
		*curMonitor = max ;
	}
	else {
		(*curMonitor)--;
	}
}

uint8_t get_curMonitor(void)
{
	return curMonitor;
}
void setcurMonitor(uint8_t index)
{
	curMonitor = index;
}



void buttonMENU_handler()
{
	switch(myDisplayMode)	{
	case HOME:
		if (displayCalibFlag)	{
			myDisplayMode = COUNTDOWN_SW_OFF;
		}
		else {
			myDisplayMode = MONITOR;
			setcurMonitor(0);
		}
		setClearflag(ENABLE);
		break;
	case COUNTDOWN_SW_OFF:
		if (displayAfterSwitchoff)	{
			myDisplayMode = AFTER_SW_OFF;
		}
		else {
			myDisplayMode = MONITOR;
		}
		setClearflag(ENABLE);
		break;
	case AFTER_SW_OFF:
		myDisplayMode = MONITOR;
		setClearflag(ENABLE);
		break;
	case MONITOR:
		myDisplayMode = MENU_CONTROL;
		setClearflag(ENABLE);
		break;
	case MENU_CONTROL:
		myDisplayMode = HOME;
		setClearflag(ENABLE);
		break;
	case VOLTAGE_CONTROL:
		myDisplayMode = HOME;
		setClearflag(ENABLE);
		break;
	case STEP_VAL_CONTROL:
		myDisplayMode = HOME;
		setClearflag(ENABLE);
		break;
	case STEP_DIR_CONTROL:
		myDisplayMode = HOME;
		setClearflag(ENABLE);
			break;
	case ON_OFF_CONTROL:
		myDisplayMode = HOME;
		setClearflag(ENABLE);
		break;
	case COMPLETE_CONTROL:
		myDisplayMode = HOME;
		setClearflag(ENABLE);
		break;
	default:
		break;
	}
}
void buttonOK_handler()
{
	switch(myDisplayMode)	{
	case HOME:
		break;
	case COUNTDOWN_SW_OFF:
		break;
	case AFTER_SW_OFF:
		break;
	case MONITOR:
		break;
	case MENU_CONTROL:
		if (getcurControl() == CONTROL_TRANS)	{
			myDisplayMode = ON_OFF_CONTROL;
			setClearflag(ENABLE);
		}
		else if (getcurControl() == CONTROL_STEP)	{
			myDisplayMode = VOLTAGE_CONTROL;
			setClearflag(ENABLE);
		}
		break;
	case VOLTAGE_CONTROL:
		switch (getStepValorDir())	{
		case STEP_VAL:
			myDisplayMode = STEP_VAL_CONTROL;
			break;
		case STEP_DIR:
			myDisplayMode = STEP_DIR_CONTROL;
			break;
		case STEP_CONFIRM:
			triggerTaskflag(TASK_CTRL_STEPMOR, FLAG_EN);
			break;
		default:
			break;
		}
		setClearflag(ENABLE);
		break;
	case STEP_VAL_CONTROL:
		myDisplayMode = VOLTAGE_CONTROL;
		setClearflag(ENABLE);
		break;
	case STEP_DIR_CONTROL:
		myDisplayMode = VOLTAGE_CONTROL;
		setClearflag(ENABLE);
		break;
	case ON_OFF_CONTROL:
		// switch MBA
		if(getCurswitch() == SW_ON)	{
			myStation.MBAstate = switchContactor(MBA_ON);


		}
		else {
			myStation.MBAstate = switchContactor(MBA_OFF);
		}

		setSwitchtime();
		myDisplayMode = COMPLETE_CONTROL;
		setClearflag(ENABLE);
		break;
	case COMPLETE_CONTROL:
		myDisplayMode = ON_OFF_CONTROL;
		setClearflag(ENABLE);
		break;
	default:
		break;
	}
}
void buttonDOWN_handler()
{
	switch(myDisplayMode)	{
	case HOME:
		break;
	case COUNTDOWN_SW_OFF:
		break;
	case AFTER_SW_OFF:
		break;
	case MONITOR:
		// move to next sensor ID
		nextMonitor(&curMonitor, myStation.ssNode_list->length);
		setClearflag(ENABLE);
		break;
	case MENU_CONTROL:
		switchcurControl();
		setClearflag(ENABLE);
		break;
	case VOLTAGE_CONTROL:
		switch(getStepValorDir())	{
		case STEP_VAL:
			setStepValorDir(STEP_DIR);
			break;
		case STEP_DIR:
			setStepValorDir(STEP_CONFIRM);
			break;
		case STEP_CONFIRM:
			setStepValorDir(STEP_VAL);
			break;
		default:
			break;
		}
		setClearflag(ENABLE);
		break;
	case STEP_VAL_CONTROL:
		// decrease step value
		decreaseStepchange();
		setClearflag(ENABLE);
		break;
	case STEP_DIR_CONTROL:
		Step_toggleDir();
		setClearflag(ENABLE);
		break;
	case ON_OFF_CONTROL:
		//  switch ON/OFF
		switch (curSwitch) {
		case SW_ON:
			curSwitch = SW_OFF;
			break;
		case SW_OFF:
			curSwitch = SW_ON;
			break;
		default:
			break;
		}
		setClearflag(ENABLE);
		break;
	case COMPLETE_CONTROL:
		break;
	default:
		break;
	}
}
void buttonUP_handler()
{
	switch(myDisplayMode)	{
	case HOME:
		break;
	case COUNTDOWN_SW_OFF:
		break;
	case AFTER_SW_OFF:
		break;
	case MONITOR:
		previousMonitor(&curMonitor, myStation.ssNode_list->length);
		setClearflag(ENABLE);
		break;
	case MENU_CONTROL:
		switchcurControl();
		setClearflag(ENABLE);
		break;
	case VOLTAGE_CONTROL:
		switch (getStepValorDir())	{
		case STEP_DIR:
			setStepValorDir(STEP_VAL);
			break;
		case STEP_VAL:
			setStepValorDir(STEP_CONFIRM);
			break;
		case STEP_CONFIRM:
			setStepValorDir(STEP_DIR);
			break;
		default:
			break;
		}
		setClearflag(ENABLE);
		break;
	case STEP_VAL_CONTROL:
		// increase step value
		increaseStepchange();
		setClearflag(ENABLE);
		break;
	case STEP_DIR_CONTROL:
		Step_toggleDir();
		break;
	case ON_OFF_CONTROL:
		switch (curSwitch) {
		case SW_ON:
			curSwitch = SW_OFF;
			break;
		case SW_OFF:
			curSwitch = SW_ON;
			break;
		default:
			break;
		}
		setClearflag(ENABLE);
		break;
	case COMPLETE_CONTROL:
		break;
	default:
		break;
	}
}
//void SW_LIMIT_MAX_handler()
//{
//	setClearflag(ENABLE);
//	A4988_DisableDriver();
//	setLimit(LIMIT_MAX);
//	triggerTaskflag(TASK_SEND_STEP_LIMIT, FLAG_EN);
//}
void SW_LIMIT_handler()
{
	setClearflag(ENABLE);
	A4988_DisableDriver();
	switch (myMotor.dir) {
	case STEPM_DIR_INC:
		myMotor.limit = LIMIT_MAX;
		break;
	case STEPM_DIR_DEC:
		myMotor.limit = LIMIT_MIN;
		break;
	default:
		break;
	}
	triggerTaskflag(TASK_SEND_STEP_LIMIT, FLAG_EN);
}
