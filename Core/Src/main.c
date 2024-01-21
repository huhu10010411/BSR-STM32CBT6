/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */


#include <stdio.h>
#include "Serial_CFG.h"

#include "flash_storage.h"
#include "Lora.h"
#include "Task.h"
#include "button.h"

#include "App_MCU.h"
#include "App_MQTT.h"
#include "App_SMS.h"
#include "App_MBA_stepmor.h"
#include "App_Display.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define FLASH_ID_ADDR  0x0801FC00

#define DELAY_TIME_OFF_MBA_CALIB		3
#define DELAY_TIME_ON_MBA_CALIB			4
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */


SIM_t mySIM;

SMS_t mySMS;

Station_t myStation = STATION_T_INIT;

// Create list of sensor node
s_list* SSnode_list;

uint8_t volatile sync_flag = 0;       // 0 : calib  ; 1: synchronize sensor node

uint32_t volatile  tmpadc= 0;
uint16_t volatile  adccount = 0;
uint32_t volatile  tmpadc1 = 0;
uint16_t volatile  adccount1 = 0;

uint32_t curtick = 0;
uint32_t pretick = 0;

uint32_t turnonMBAtick = 0;
uint8_t turnonMBAflag = 0;

uint32_t turnOFFMBAtick = 0;
uint8_t turnOFFMBAflag = 0;

uint8_t flag = 0;


uint32_t getgpstick = 0;
uint8_t gpsflag = 0;

uint32_t ticktimerpre = 0;
uint32_t ticktimercur = 0;
uint32_t ticktimer2pre = 0;
uint32_t ticktimer2cur = 0;

//extern uint8_t alarmflag;
//uint8_t flag = 0;
//uint8_t debug = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static void ADC_Convert(uint32_t volatile *adcval, uint8_t CurorVol);
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if (huart->Instance == USART1 )
	{
		SIM_callback(Size);
	}
	if (huart->Instance== USART2)
	{
		Serial_CFG_Callback(Size);
	}

	if (huart->Instance == USART3) {
		Lora_callback(Size);
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == RTC_ALARM_TRIGGER_Pin)	{
//		alarmflag = 0;

		if(!sync_flag)	{

				// Set Mode Measure for Sensor node
				Lora_Setmode(MEASURE, 0);

				DS3231_ClearAlarm1();
				// Set alarm for Turn OFF MBA

				ticktimerpre = HAL_GetTick();
				HAL_TIM_Base_Start_IT(&htim3);

				triggerTaskflag(TASK_START_CALIB, FLAG_EN);

				// Display after OFF Contactor
				displayCalibFlag = 0;
		}
		else {
			Lora_Setmode(SLEEP, 0);
			Lora_Setmode(SLEEP, 0);
			DS3231_ClearAlarm1();
			sync_flag = 0;
		}

	}
	curtick = HAL_GetTick();

	if (curtick - pretick > 20)	{
		pretick = curtick;

		if (GPIO_Pin == BUTTON_MENU_Pin) {
			// Button Menu handler
			buttonMENU_handler();
//			ticktimerpre = HAL_GetTick();
//			HAL_TIM_Base_Start_IT(&htim3);
		}

		if (GPIO_Pin == BUTTON_OK_Pin) {
			// Button OK handler
			buttonOK_handler();
		}

		if (GPIO_Pin == BUTTON_UP_Pin) {
			// Button UP handler
			buttonUP_handler();
		}

		if (GPIO_Pin == BUTTON_DOWN_Pin) {
			// Button DOWN handler
			buttonDOWN_handler();
		}

//		if (GPIO_Pin == LIMIT_SWITCH_MAX_Pin) {
//			// Button Limit MAX handler
//			SW_LIMIT_MAX_handler();
//		}

		if (GPIO_Pin == LIMIT_SWITCH_Pin) {
			// Button Limit MIN handler
			SW_LIMIT_handler();
		}
	}
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if (hadc->Instance == hadc1.Instance)	{
		tmpadc += HAL_ADC_GetValue(&hadc1);
		adccount++;
		ADC_Convert(&tmpadc, 0);

	}
	if (hadc->Instance == hadc2.Instance)	{
		tmpadc1 += HAL_ADC_GetValue(&hadc2);
		adccount1++;
		ADC_Convert(&tmpadc1, 1);
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM3 )	{

		static uint8_t count = 0;

		if(count == 1)	{

			ticktimercur = HAL_GetTick();
//			HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

			// Turn OFF MBA for Calib
			myStation.MBAstate = switchContactor(MBA_OFF);

			// Set flag for display
			displayAfterSwitchoff = 1;

			// Stop timer Turn OFF
			HAL_TIM_Base_Stop_IT(&htim3);

			// Start timer 1s for turn ON contactor
			ticktimer2pre = HAL_GetTick();
			HAL_TIM_Base_Start_IT(&htim2);
		}

		if(count < 1) {
			count ++;
		}

	}
	else if (htim == &htim2)	{

//		ticktimer2cur = HAL_GetTick();

		static uint8_t count1 = 0;

		if (count1 == 1)	{

			ticktimer2cur = HAL_GetTick();

			// Turn ON contactor
			myStation.MBAstate = switchContactor(MBA_ON);
//			HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

			// Stop timer Turn ON
			HAL_TIM_Base_Stop_IT(&htim2);

		}
		if (count1 < 1)	{
			count1 ++;
		}

	}
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC2_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_I2C1_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  SSnode_list =list_create();

  myStation.ssNode_list = SSnode_list;

  // Get station ID from flash
//  Flash_Write_NUM(FLASH_PAGE_127, 0x03);
  myStation.stID = (uint8_t)Flash_Read_NUM(FLASH_PAGE_127);

  /********************Configure MQTT********************************/
	mySIM.mqttServer.host = "tcp://broker.hivemq.com";
	mySIM.mqttServer.port = 1883;
	mySIM.mqttServer.willtopic = "unnormal_disconnect";
	mySIM.mqttServer.willmsg = (uint8_t*)malloc(sizeof(myStation.stID));
	sprintf((char*)mySIM.mqttServer.willmsg,"%d",myStation.stID);
	mySIM.mqttClient.keepAliveInterval = 180;
	sprintf(mySIM.mqttClient.clientID,"bsrclientII&IL%d",myStation.stID);
	mySIM.mqttReceive.qos =1;
	mySIM.mqttServer.connect=0;

	mySIM.sms = mySMS;

	// ADC
	HAL_ADC_Start_IT(&hadc1);
	HAL_ADC_Start_IT(&hadc2);

	initTask();

	// MBA and Step motor
	initApp_MBA_stepmor();
	// Lora
	initLora();

	// GPS
	initSerial_CFG();

	initGPS(&myRTC);

	initSIM();

	// Init MQTT app
	initApp_MQTT();



//	init_App_Serial();

	// Init RTC module (DS3231)
	DS3231_Init(&hi2c1);

	initApp_Display();

	initApp_SMS();

//	ticktimerpre = HAL_GetTick();
//	HAL_TIM_Base_Start_IT(&htim3);

//	triggerTaskflag(TASK_GET_GPS_TIME, FLAG_EN);

//    DS3231_GetTime(&myRTC);
//    DS3231_SetAlarm1(ALARM_MODE_HOUR_MIN_SEC_MATCHED, myRTC.Date, myRTC.Hour, myRTC.Min, myRTC.Sec + 5);
//	myRTC.Year = 24;
//	myRTC.Month = 1;
//	myRTC.Date = 21;
//	myRTC.DaysOfWeek = 1;
//	myRTC.Hour = 23;
//	myRTC.Min = 59;
//	myRTC.Sec = 59;
//	DS3231_SetTime(&myRTC);

//	myStation.calibTime.hour = 1;
//	myStation.calibTime.min = 40;
//	myStation.calibTime.sec = 0;
//
//	displayCalibFlag = 1;
//	displayAfterSwitchoff = 1;


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  //Turn OFF contactor
//	  if (turnOFFMBAflag && (HAL_GetTick() - turnOFFMBAtick >= 10000 ))	{
//		  myStation.MBAstate = switchContactor(MBA_OFF);
//		  turnonMBAtick = HAL_GetTick();
//		  turnOFFMBAflag = 0;
//		  turnonMBAflag = 1;
//	  }
//
//	  if (turnonMBAflag && (HAL_GetTick() - turnonMBAtick >= 1000))	{
//		  myStation.MBAstate = switchContactor(MBA_ON);
//		  turnonMBAflag = 0;
//	  }


	  if (HAL_GetTick() - getgpstick >= 180000 && !gpsflag)	{
		  gpsflag = 1;
		  triggerTaskflag(TASK_GET_GPS_TIME, FLAG_EN);
	  }
	  processApp_MCU();
	  processingApp_display();
	  processApp_MQTT();
	  processingApp_MBA_stepmor();
	  processApp_SMS();

//	  HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
//	  testSynchronize();

	  HAL_Delay(20);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/*
 *  @Para:  CurorVol : 0 Current, 1 Voltage
 */
static void ADC_Convert(uint32_t volatile *adcval, uint8_t CurorVol)
{
	switch (CurorVol)	{

	case 0:
		if (adccount == 1000)	{
			myStation.stCurrent = (uint16_t)( ( ( (*adcval)*3/4095) + 50)/6);
			*adcval = 0;
			adccount = 0;
		}
		break;

	case 1:
		if (adccount1 == 1000)	{
			myStation.stVoltage = (uint16_t)( ( *adcval)*3/68*1568/4095 );
//			myStation.stVoltage = (uint16_t)( ( ( (*adcval)*3/4095) ));
			*adcval = 0;
			adccount1 = 0;
		}
		break;
	default:
		break;
	}

}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
