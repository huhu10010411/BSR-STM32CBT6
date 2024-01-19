/*
 * Contactor.h
 *
 *  Created on: Jan 12, 2024
 *      Author: Admin
 */

#ifndef INC_DEVICE_CONTACTOR_H_
#define INC_DEVICE_CONTACTOR_H_

#include "stm32f1xx.h"
typedef enum {
	MBA_NULL= 0x00,
	MBA_ON = 0x01,
	MBA_OFF
}MBA_state_t;

MBA_state_t switchContactor (MBA_state_t ONofOFF);

#endif /* INC_DEVICE_CONTACTOR_H_ */
