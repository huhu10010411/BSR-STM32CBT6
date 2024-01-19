/*
 * App_MBA_stepmor.c
 *
 *  Created on: Jan 12, 2024
 *      Author: Admin
 */
//#include "button.h"
#include "Task.h"
#include "Step_motor.h"
#include "user_a4988.h"

//static Stepmotor_change_mode_t changeMode;
void initApp_MBA_stepmor()
{
	initMymotor();
}

void processingApp_MBA_stepmor()
{
	if(checkTaskflag(TASK_CTRL_STEPMOR))	{
		switch ( myMotor.limit )	{
		case LIMIT_MAX:
			if (myMotor.dir == STEPM_DIR_DEC)	{
				myMotor.limit = LIMIT_NONE;
			}
			break;
		case LIMIT_MIN:
			if (myMotor.dir == STEPM_DIR_INC)	{
				myMotor.limit = LIMIT_NONE;
			}
			break;
		default :
			break;
		}
		switch (myMotor.mode)	{
		case STEPM_MODE_STEP:
			Step_step();
			break;
		case STEPM_MODE_PERCENTAGE:
			Step_percent();
			break;
		default:
			break;
		}
		// trigger task send output data to server
		triggerTaskflag(TASK_SEND_DATAPERIOD, FLAG_EN);
		// disable task control step motor
		triggerTaskflag(TASK_CTRL_STEPMOR, FLAG_DIS);
	}
}
