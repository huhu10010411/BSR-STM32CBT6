/*
 * Contactor.c
 *
 *  Created on: Jan 12, 2024
 *      Author: Admin
 */

#include "Contactor.h"
#include  "gpio.h"
#include "Task.h"


MBA_state_t switchContactor (MBA_state_t ONofOFF)
{
	switch (ONofOFF)	{
	case MBA_ON:
		HAL_GPIO_WritePin(MBA_CONTACTOR_GPIO_Port, MBA_CONTACTOR_Pin, GPIO_PIN_SET);
		break;
	case MBA_OFF:
		HAL_GPIO_WritePin(MBA_CONTACTOR_GPIO_Port, MBA_CONTACTOR_Pin, GPIO_PIN_RESET);
		break;
	default:
		break;
	}
	// send MBA status to Server
	triggerTaskflag(TASK_SEND_MBA_STATUS, FLAG_EN);
	return ONofOFF;
}
