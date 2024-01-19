/*
 * stationCMD.h
 *
 *  Created on: Dec 10, 2023
 *      Author: Admin
 */

#ifndef INC_STATIONCMD_H_
#define INC_STATIONCMD_H_

#include "ServerMessage.h"

uint8_t CMD_SMS_calib ( uint8_t *cmddata, uint16_t datalen);

uint8_t CMD_SMS_getStatus ( uint8_t *cmddat, uint16_t datalen);

uint8_t CMD_SMS_Ctrl_MBA ( uint8_t *cmddata, uint16_t datalen, MBA_state_t MBAstate);

uint8_t CMD_SMS_Ctrl_StepM ( uint8_t *cmddata, uint16_t datalen, Stepmotor_dir_t stepDIR, Stepmotor_change_mode_t stepMode, uint8_t changeValue);

uint8_t CMD_SMS_getStationLatestData (uint8_t *cmddata, uint16_t datalen);

uint8_t CMD_SMS_getSensorLatestData ( uint8_t *cmddata, uint16_t datalen);

uint8_t CMD_sleep(void);

uint8_t CMD_wakeup(void);

#endif /* INC_STATIONCMD_H_ */
