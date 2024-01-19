/*
 * Task.c
 *
 *  Created on: Dec 27, 2023
 *      Author: Admin
 */

#include "Task.h"
#include "main.h"


//static Station_t *myStation;

void initTask()
{
//	myStation = station;
}

bool checkTaskflag (Task_t task)
{
	bool res = false;
	switch (task) {
	case TASK_REGISTER:
		res = (bool)myStation.task.register2server ;
		break;
	case TASK_SUBSCRIBE:
		res = (bool)myStation.task.subscribe;
		break;
	case TASK_SEND_MBA_STATUS:
		res = (bool)myStation.task.sendMBAstatus;
		break;
	case TASK_CTRL_STEPMOR:
		res = (bool)myStation.task.CtrlStepmotor;
		break;
	case TASK_PREPARE_CALIB:
		res = (bool)myStation.task.PrepareCalib;
		break;
	case TASK_START_CALIB:
		res = (bool)myStation.task.StartCalib;
		break;
	case TASK_GET_GPS_TIME:
		res = (bool)myStation.task.getGPStimeflag;
		break;
	case TASK_SEND_NWREADY:
		res = (bool)myStation.task.sendNWready;
		break;
	case TASK_SEND_DATACALIB:
		res = (bool)myStation.task.sendDATACALIB;
		break;
	case TASK_SEND_DATAPERIOD:
		res = (bool)myStation.task.sendDataPeriod;
		break;
	case TASK_SEND_CTRLMBA:
		res = (bool)myStation.task.sendcmdCtrlMBA;
		break;
	case TASK_SEND_CTRLSTEPMOR:
		res = (bool)myStation.task.sendcmdCtrlStepmotor;
		break;
	case TASK_SEND_CALIB:
		res = (bool)myStation.task.sendcmdCalib;
		break;
	case TASK_SEND_GETSTATUS:
		res = (bool)myStation.task.sendcmdGetstatus;
		break;
	case TASK_SEND_GETSTATION:
		res = (bool)myStation.task.sendcmdGetstation;
		break;
	case TASK_SEND_GETSENSOR:
		res = (bool)myStation.task.sendcmdGetsensor;
		break;
	case TASK_SEND_STEP_LIMIT:
		res = (bool)myStation.task.sendSTEPLIMIT;
		break;
	default:
		break;
	}
	return res;
}



void triggerTaskflag (Task_t task, Flag_t ENorDIS)
{
	switch (task) {
	case TASK_REGISTER:
		myStation.task.register2server = ENorDIS;
		break;
	case TASK_SUBSCRIBE:
		myStation.task.subscribe = ENorDIS;
		break;
	case TASK_SEND_MBA_STATUS:
		myStation.task.sendMBAstatus = ENorDIS;
		break;
	case TASK_CTRL_STEPMOR:
		myStation.task.CtrlStepmotor = ENorDIS;
		break;
	case TASK_PREPARE_CALIB:
		myStation.task.PrepareCalib = ENorDIS;
		break;
	case TASK_START_CALIB:
		myStation.task.StartCalib = ENorDIS;
		break;
	case TASK_GET_GPS_TIME:
		myStation.task.getGPStimeflag = ENorDIS;
		break;
	case TASK_SEND_NWREADY:
		myStation.task.sendNWready = ENorDIS;
		break;
	case TASK_SEND_DATACALIB:
		myStation.task.sendDATACALIB = ENorDIS;
		break;
	case TASK_SEND_DATAPERIOD:
		myStation.task.sendDataPeriod = ENorDIS;
		break;
	case TASK_SEND_CTRLMBA:
		myStation.task.sendcmdCtrlMBA = ENorDIS;
		break;
	case TASK_SEND_CTRLSTEPMOR:
		myStation.task.sendcmdCtrlStepmotor = ENorDIS;
		break;
	case TASK_SEND_CALIB:
		myStation.task.sendcmdCalib = ENorDIS;
		break;
	case TASK_SEND_GETSTATUS:
		myStation.task.sendcmdGetstatus = ENorDIS;
		break;
	case TASK_SEND_GETSTATION:
		myStation.task.sendcmdGetstation = ENorDIS;
		break;
	case TASK_SEND_GETSENSOR:
		myStation.task.sendcmdGetsensor = ENorDIS;
		break;
	case TASK_SEND_STEP_LIMIT:
		myStation.task.sendSTEPLIMIT = ENorDIS;
	default:
		break;
	}
	return ;
}





