/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/*
#include "stm32f4_discovery.h"
#include "stm32f4_discovery_accelerometer.h"
#include "lowpass.h"
*/
#include "usbd_cdc_if.h"
#include "uartSlave.h"
#include "UartSlaveAccel.h"

#include "stm32f4_discovery.h"
#include "stm32f4_discovery_accelerometer.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define IDX_X (0U)
#define IDX_Y (1U)
#define IDX_Z (2U)
#define UARTSLAVE_DEVICE_COUNT (1U)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t accelId = 0;
int16_t accelXYZ [3] = {0};
Lowpass_TypeDef accelX;
Lowpass_TypeDef accelY;
Lowpass_TypeDef accelZ;

uint32_t idxUartSlave;
uint32_t start_tick, end_tick;

uint8_t tim9_elapsed = 0;
uint8_t cdc_data_received = 0;
float time;


volatile uint32_t * const pTim2Counter = (volatile uint32_t *) 0x40000024;
uint32_t tim2Counter;

UartSlave_TypeDef * uartSlaveDevices [UARTSLAVE_DEVICE_COUNT];
uint32_t uartSlaveDeviceIdx;
UartSlaveAccel_TypeDef uartSlaveAccelDevice;
UartSlaveIo_TypeDef uartSlaveAccelDeviceIoContext
=
			{
					NULL,
					NULL,
					CDC_Transmit_FS,
					NULL
			};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
/*	lowpass_Init(&accelX, 0.02);
	lowpass_Init(&accelY, 0.02);
	lowpass_Init(&accelZ, 0.02);
	*/
//	uartSlaveAccelDeviceIoContext uartSlaveAccelIoContext =
//			{
//					NULL,
//					NULL,
//					CDC_Transmit_FS,
//					NULL
//			};
	UartSlaveAccel_Init (& uartSlaveAccelDevice, & uartSlaveAccelDeviceIoContext, 0x16U);
	uartSlaveDevices [0] = & uartSlaveAccelDevice;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  // BSP_ACCELERO_Init();
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  HAL_Delay(1000);
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CYCCNT = 0;
  DWT->CTRL = DWT_CTRL_CYCCNTENA_Msk;
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_TIM9_Init();
  MX_USB_DEVICE_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */

  BSP_ACCELERO_Init();
  HAL_TIM_Base_Init(&htim9);
  HAL_TIM_Base_Start_IT(&htim9);
  accelId = BSP_ACCELERO_ReadID();

  HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  HAL_GetTick();
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  tim2Counter = * pTim2Counter;

    if (tim9_elapsed || cdc_data_received)
    {
//        BSP_ACCELERO_GetXYZ(&accelXYZ[0]);
//
//        lowpass_update(&accelX, (float)accelXYZ[IDX_X]);
//        lowpass_update(&accelY, (float)accelXYZ[IDX_Y]);
//        lowpass_update(&accelZ, (float)accelXYZ[IDX_Z]);
//        end_tick = DWT->CYCCNT;
//        time = (float)(end_tick-   start_tick)/(SystemCoreClock/1000.0);

    	tim9_elapsed = 0;
    	cdc_data_received = 0;
    	uartSlaveDevices [0] -> pfvStateMachine (uartSlaveDevices [0] );



    }
    //HAL_Delay(1500);
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/**
  * @brief  Period elapsed callback in non-blocking mode
  * @param  htim TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(htim);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_TIM_PeriodElapsedCallback could be implemented in the user file
   */

  if (&htim9 == htim)
  {

  	start_tick = DWT->CYCCNT;
	  tim9_elapsed = 1;
  }

}

void USB_CDC_RxHandler(uint8_t*pBuffer, uint32_t Length)
{
	// Only uart Slave is in cdc
	RingBuffer_Write_XBit(&uartSlaveAccelDevice.UartSlaveInstance.rxBufferManager, pBuffer, Length);
//	tim9_elapsed = 1;
	cdc_data_received = 1;
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
