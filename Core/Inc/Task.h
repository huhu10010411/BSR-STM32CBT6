/*
 * Task.h
 *
 *  Created on: Dec 27, 2023
 *      Author: Admin
 */

#ifndef INC_TASK_H_
#define INC_TASK_H_

#include "main.h"

typedef enum {
	FLAG_DIS,
	FLAG_EN
}Flag_t;

typedef enum {
	TASK_REGISTER,
	TASK_SUBSCRIBE,
	TASK_PREPARE_CALIB,
	TASK_START_CALIB,
	TASK_CTRL_STEPMOR,
	TASK_GET_GPS_TIME,
	TASK_SEND_NWREADY,
	TASK_SEND_DATAPERIOD,
	TASK_SEND_DATACALIB,
	TASK_SEND_MBA_STATUS,
	TASK_SEND_CTRLMBA,
	TASK_SEND_CTRLSTEPMOR,
	TASK_SEND_CALIB,
	TASK_SEND_GETSTATUS,
	TASK_SEND_GETSTATION,
	TASK_SEND_GETSENSOR,
	TASK_SEND_STEP_LIMIT,
}Task_t;


void initTask();

bool checkTaskflag (Task_t task);

//bool checkTaskStatus (Task_t task);

void triggerTaskflag (Task_t task, Flag_t ENorDIS);

//void triggerTaskStatus (Task_t task, Status_t DONEorNDONE);

#endif /* INC_TASK_H_ */
