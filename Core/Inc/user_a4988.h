/**
  * @file    user_a4988.h
  * @author  Sagit
  * @date    Dec 10, 2023
  * @brief   
  */

#ifndef USER_A4988_H_
#define USER_A4988_H_

/* INCLUDES ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "gpio.h"

/* MACROS AND DEFINES --------------------------------------------------------*/

#define DIR_RIGHT	1
#define DIR_LEFT	0
/* TYPEDEFS ------------------------------------------------------------------*/
typedef struct
{
	uint16_t DIR, STEP, ENABLE, SLEEP, RESET;
	GPIO_TypeDef* PDIR;
	GPIO_TypeDef* PSTEP;
	GPIO_TypeDef* PENABLE;
	GPIO_TypeDef* PSLEEP;
	GPIO_TypeDef* PRESET;
} StructManageMotorA4988;

/* EXTERNAL VARIABLES --------------------------------------------------------*/

/* GLOBAL FUNCTION PROTOTYPES ------------------------------------------------*/
void A4988_Init();
void A4988_EnableDriver();
void A4988_DisableDriver();
void A4988_SetDirection(int direction);
void A4988_StepByStep(int steps, int direction);
void A4988_StepByDegrees(float degrees, int direction);
void A4988_ResetDriver();

#endif /*USER_A4988_H_*/

/* End of file ----------------------------------------------------------------*/
