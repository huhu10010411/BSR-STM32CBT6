/*
 * App_MQTT.h
 *
 *  Created on: Dec 15, 2023
 *      Author: Admin
 */

#ifndef INC_APPLICATION_APP_MQTT_H_
#define INC_APPLICATION_APP_MQTT_H_


#include "SIM.h"
#include "main.h"


void initApp_MQTT();

void processApp_MQTT(void);

uint8_t checkSensorMode(sensor_mode_t mode);

uint8_t checkSensorState(sensor_state_t state);

#endif /* INC_APPLICATION_APP_MQTT_H_ */
