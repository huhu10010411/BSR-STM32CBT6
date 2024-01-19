/*
 * Step_motor.c
 *
 *  Created on: Jan 12, 2024
 *      Author: Admin
 */
#include "Step_motor.h"
#include "user_a4988.h"

#define MAX_STEP	400

//static LIMIT_t limit = LIMIT_NONE;
//static Stepmotor_dir_t step_dir = STEPM_DIR_INC;
//static uint16_t volatile stepChange = 0;
//static uint8_t steppercentChange = 0;
//static Stepmotor_change_mode_t stepMode = STEPM_MODE_STEP;

//uint8_t previousDIR = 0;    // 0 : DEC , 1: INC

step_motor_t myMotor;
//init

void initMymotor()
{
	myMotor.dir = STEPM_DIR_INC;
	myMotor.limit = LIMIT_NONE;
	myMotor.mode = STEPM_MODE_STEP;
	myMotor.stepChange = 0;
	myMotor.steppercentChange = 0;
	myMotor.previousDIR = STEPM_DIR_INC;
}


//void setLimit(LIMIT_t Limit)
//{
//	limit = Limit;
//}
//
//LIMIT_t getLimit ()
//{
//	return limit;
//}
//
//Stepmotor_dir_t Step_getDir()
//{
//	return step_dir;
//}
//
//void Step_setDir(Stepmotor_dir_t dir)
//{
//	step_dir = dir;
//}

void Step_toggleDir()
{
	switch (myMotor.dir)	{
	case STEPM_DIR_INC:
		myMotor.dir = STEPM_DIR_DEC;
		break;
	case STEPM_DIR_DEC:
		myMotor.dir =STEPM_DIR_INC;
		break;
	default:
		break;
	}
}

void increaseStepchange()
{
	if (myMotor.stepChange >= MAX_STEP )	{
		myMotor.stepChange = 0;
	}
	else {
		myMotor.stepChange ++;
	}
}

void decreaseStepchange()
{
	if (myMotor.stepChange == 0  )	{
		myMotor.stepChange = MAX_STEP;
	}
	else {
		myMotor.stepChange--;
	}
}

//uint16_t getStepchange()
//{
//	return stepChange;
//}
//void Step_setStepChange(uint16_t value)
//{
//	myMotor.stepChange = value;
//}
//uint8_t Step_getPercentChange()
//{
//	return steppercentChange;
//}

//void Step_setPercentChange(uint8_t percent)
//{
//	steppercentChange = percent;
//}

//void Step_setChangeMode(Stepmotor_change_mode_t mode)
//{
//	stepMode = mode;
//}

//Stepmotor_change_mode_t Step_getChangemode()
//{
//	return stepMode;
//}

void Step_step()
{
	uint8_t dir = myMotor.dir;
	uint16_t stepchange = myMotor.stepChange;
	if (HAL_GPIO_ReadPin(LIMIT_SWITCH_GPIO_Port, LIMIT_SWITCH_Pin) == 0 && myMotor.previousDIR == STEPM_DIR_INC && dir == STEPM_DIR_INC )	return ;
	if (HAL_GPIO_ReadPin(LIMIT_SWITCH_GPIO_Port, LIMIT_SWITCH_Pin) == 0 && myMotor.previousDIR == STEPM_DIR_DEC && dir == STEPM_DIR_DEC )	return ;
	A4988_EnableDriver();
	A4988_StepByStep(stepchange, dir );
	A4988_DisableDriver();
	myMotor.previousDIR = dir;

}

void Step_percent()
{
	uint8_t dir = myMotor.dir;
	uint8_t percent = myMotor.steppercentChange;
	if (HAL_GPIO_ReadPin(LIMIT_SWITCH_GPIO_Port, LIMIT_SWITCH_Pin) == 0 && myMotor.previousDIR == STEPM_DIR_INC && dir == STEPM_DIR_INC )	return ;
	if (HAL_GPIO_ReadPin(LIMIT_SWITCH_GPIO_Port, LIMIT_SWITCH_Pin) == 0 && myMotor.previousDIR == STEPM_DIR_DEC && dir == STEPM_DIR_DEC )	return ;
	uint16_t stepChange = (uint16_t)(percent * MAX_STEP / 100);
	A4988_EnableDriver();
	A4988_StepByStep(stepChange, dir);
	A4988_DisableDriver();
	myMotor.previousDIR = dir;

}
