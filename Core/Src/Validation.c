/*
 * Validation.c
 *
 *  Created on: Dec 26, 2023
 *      Author: Admin
 */
#include "Validation.h"

#define MAX_STATION_ID		0x50
#define MIN_STATION_ID		0x01

#define MAX_SENSOR_ID		0xFE
#define MIN_SENSOR_ID		0x51

bool validationID(uint16_t ID, ID_t idtype)
{
	bool res = false;
	switch (idtype)	{
	case ID_SENSOR:
		res = sensorID_validation(ID);
		break;
	case ID_STATION:
		res = stationID_validation(ID);
		break;
	default:
		break;
	}
	return res;
}

bool stationID_validation(uint8_t ID)
{
	if (ID >= MIN_STATION_ID && ID <= MAX_STATION_ID)	return true;
	return false;
}

bool sensorID_validation(uint8_t ID)
{
	if (ID >= MIN_SENSOR_ID && ID <= MAX_SENSOR_ID)	return true;
	return false;
}
