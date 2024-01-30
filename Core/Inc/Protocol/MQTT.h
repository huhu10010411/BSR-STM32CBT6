/*
 * SIM_MQTT.h
 *
 *  Created on: Oct 9, 2023
 *      Author: Admin
 */

#ifndef INC_MQTT_H_
#define INC_MQTT_H_

#include "stm32f1xx.h"
//#include "SIM.h"

#define TOPIC_PUB		"BSR/STATION2SERVER"
#define TOPIC_SUB		"BSR/SERVER2STATION/"
#define TOPIC_SUB_BROADCAST		"BSR/SERVER2STATION"

#define MQTT_TXBUFF_SIZE   256



typedef enum {
	CONTENT_TOPIC,
	CONTENT_PAYLOAD
}content_t;

//extern SIM_t *__MY_SIM;
void init_MQTT();
uint8_t MQTT_connect();
uint8_t MQTT_connected(void);
uint8_t MQTT_disconnect ();
uint8_t MQTT_publish(uint8_t *topic,uint8_t *msg,uint16_t msglen);
uint8_t MQTT_subcribe (uint8_t *topic);
uint8_t checkSubcribe(uint8_t *topic);
uint8_t MQTT_unsubcribe (uint8_t *topic);
uint8_t MQTT_receive(uint8_t *MQTTbuff,uint16_t size);
uint8_t MQTT_getConnectflag();
void MQTT_testReceive (void);


#endif /* INC_MQTT_H_ */
