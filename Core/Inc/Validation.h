/*
 * Validation.h
 *
 *  Created on: Dec 26, 2023
 *      Author: Admin
 */

#ifndef INC_VALIDATION_H_
#define INC_VALIDATION_H_

#include "stdint.h"
#include "stdbool.h"


#define MIN_ID		0x00
#define MAX_ID		0xFF

typedef enum {
	ID_STATION = 0x01,
	ID_SENSOR
}ID_t;

bool validationID(uint16_t ID, ID_t idtype);

bool stationID_validation(uint8_t ID);

bool sensorID_validation(uint8_t ID);

#endif /* INC_VALIDATION_H_ */
