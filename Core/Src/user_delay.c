/**
  * @file    user_delay.c
  * @author  Sagit
  * @date    Dec 10, 2023
  * @brief   
  */

/* INCLUDES ------------------------------------------------------------------*/
#include "user_delay.h"

/* EXTERN VARIABLES ----------------------------------------------------------*/

/* PRIVATE MACROS AND DEFINES ------------------------------------------------*/

/* PRIVATE TYPEDEFS ----------------------------------------------------------*/

/* STATIC VARIABLES ----------------------------------------------------------*/

/* GLOBAL VARIABLES ----------------------------------------------------------*/

/* STATIC FUNCTION PROTOTYPES --- --------------------------------------------*/

/* STATIC FUNCTIONS ----------------------------------------------------------*/

/* GLOBAL FUNCTIONS ----------------------------------------------------------*/
void delay_init(void)
{

}

void delay_1ms(void)
{
	__HAL_TIM_SetCounter(&htim1, 0);
    while (__HAL_TIM_GetCounter(&htim1)<1);
}

void delay_ms(uint16_t nms)
{
	while(nms--)
	{
		delay_1ms();
	}
}
/* End of file ----------------------------------------------------------------*/
