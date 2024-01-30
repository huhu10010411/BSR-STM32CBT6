/*
 * linkedlist.h
 *
 *  Created on: Oct 27, 2023
 *      Author: Admin
 */

#ifndef INC_LINKEDLIST_H_
#define INC_LINKEDLIST_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#define DATACALIB_SIZE 101

typedef enum {
	V_p = 0x01,
	V_na
}voltage_t;

typedef enum {
	SENSOR_ACTIVE = 0x01,
	SENSOR_DEACTIVE,
	SENSOR_READY
}sensor_state_t;

typedef enum {
	SLEEP,
	WAKE,
	MEASURE
}sensor_mode_t;

typedef struct{
	uint8_t  	SSnode_ID;          // Sensor node ID
	uint8_t    Battery;
	uint16_t volatile V_value;			// Voltage value
	voltage_t		V_type;				// Voltage type
	sensor_state_t	Sensor_state;
	uint32_t tickupdate;
	volatile uint8_t dataCalibAvailable;
	uint8_t Ready : 1;
	volatile sensor_mode_t sensorMode;
	uint8_t dataCalibBuffer[DATACALIB_SIZE];
}SensorNode_t;

#define SENSORNODE_T_INIT 	{0, 0,  0, V_p, SENSOR_ACTIVE, 0, 0, 0, SLEEP}

typedef struct Node {
	SensorNode_t  SSnode;
	struct Node*  next;
}Node;

typedef struct list_t {
    Node *head;
    Node *tail;
    unsigned int length;
} s_list;

Node* create_node(SensorNode_t  SSnode);
s_list * list_create(void);
void list_append (s_list* list, SensorNode_t SSnode_data);
#endif /* INC_LINKEDLIST_H_ */
