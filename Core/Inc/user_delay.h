/**
  * @file    user_delay.h
  * @author  Sagit
  * @date    Dec 10, 2023
  * @brief   
  */

#ifndef USER_DELAY_H_
#define USER_DELAY_H_

/* INCLUDES ------------------------------------------------------------------*/
#include "stdint.h"
#include "stm32f1xx_hal.h"
//#include "tim.h"

/* MACROS AND DEFINES --------------------------------------------------------*/

/* TYPEDEFS ------------------------------------------------------------------*/

/* EXTERNAL VARIABLES --------------------------------------------------------*/

/* GLOBAL FUNCTION PROTOTYPES ------------------------------------------------*/
void delay_init(void);
void delay_1ms(void);
void delay_ms(uint16_t nms);

#endif /*USER_DELAY_H_*/

/* End of file ----------------------------------------------------------------*/
