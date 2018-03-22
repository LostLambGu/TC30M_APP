/*******************************************************************************
* File Name          : deepsleep.c
* Author             : Yangjie Gu
* Description        : This file provides all the deepsleep functions.

* History:
*  10/23/2017 : deepsleep V1.00
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "usart.h"
#include "i2c.h"
#include "adc.h"
#include "iwdg.h"
#include "tim.h"

#include "initialization.h"
#include "deepsleep.h"
#include "rtcclock.h"
#include "ublox_driver.h"
#include "lis3dh_driver.h"

#include "wedgertctimer.h"

/* Private define ------------------------------------------------------------*/
#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* Function prototypes -------------------------------------------------------*/
void GpioLowPower(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /*Configure GPIO pins : PCPin PCPin PCPin PCPin 
                           PCPin */
  GPIO_InitStruct.Pin = /*PC2_PWR_EN_Pin|*/PC8_LED_G_Pin|PC9_LED_R_Pin/*|PC11_MCU_OD_Pin 
                          |PC12_MCU_STARTER_Pin*/;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PAPin PAPin PAPin PAPin 
                           PAPin PAPin */
  GPIO_InitStruct.Pin = /*PA0_RESETN_Pin|*/PA4_FLASH_CS_N_Pin|PA5_FLASH_SCK_Pin|PA7_FLASH_MOSI_Pin 
                          |PA8_FLASH_WP_N_Pin/*|PA15_MCU_RELAY_Pin*/;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = PA6_FLASH_MISO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(PA6_FLASH_MISO_GPIO_Port, &GPIO_InitStruct);
}

void HaltPowerConsumption(void)
{
  // ADC
  HAL_ADC_Stop(&hadc);
  HAL_ADC_Stop_IT(&hadc);
  HAL_ADC_Stop_DMA(&hadc);
  HAL_ADC_DeInit(&hadc);

  // dma
  __HAL_RCC_DMA1_CLK_DISABLE();

  // PWM
  HAL_TIM_PWM_DeInit(&htim2);

  // uart
  UART_Interrupt_DeInit(&huart1);
  HAL_UART_DeInit(&huart1);
  UART_Interrupt_DeInit(&huart2);
  HAL_UART_DeInit(&huart2);
  UART_Interrupt_DeInit(&huart3);
  HAL_UART_DeInit(&huart3);

  // spi flash powerdown

  // Modem
  // Disable RTS
  ModemRTSEnControl(DISABLE);
  // //module disable
  ModemPowerEnControl(DISABLE);

  // AccGyro
  if (GetMcuDeepSleepAccState() == MCU_DEEPSLEEP_ACC_STATUS_LOW_POWER)
  {
    LIS3DH_SetMode(LIS3DH_LOW_POWER);
  }
  else if (GetMcuDeepSleepAccState() == MCU_DEEPSLEEP_ACC_STATUS_NORMAL)
  {
    LIS3DH_SetMode(LIS3DH_NORMAL);
  }
  else
  {
    LIS3DH_SetMode(LIS3DH_POWER_DOWN);
  }

  // i2c
  HAL_I2C_DeInit(&hi2c1);
  HAL_I2C_DeInit(&hi2c2);

  // Gps
  UbloxGPSStop();

  // Gpio
  GpioLowPower();
}

void GpioEixtLowPower(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /*Configure GPIO pins : PCPin PCPin PCPin PCPin 
                           PCPin */
  GPIO_InitStruct.Pin = /*PC2_PWR_EN_Pin|*/PC8_LED_G_Pin|PC9_LED_R_Pin/*|PC11_MCU_OD_Pin 
                          |PC12_MCU_STARTER_Pin*/;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PAPin PAPin PAPin PAPin 
                           PAPin PAPin */
  GPIO_InitStruct.Pin = /*PA0_RESETN_Pin|*/PA4_FLASH_CS_N_Pin|PA5_FLASH_SCK_Pin|PA7_FLASH_MOSI_Pin 
                          |PA8_FLASH_WP_N_Pin/*|PA15_MCU_RELAY_Pin*/;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = PA6_FLASH_MISO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(PA6_FLASH_MISO_GPIO_Port, &GPIO_InitStruct);
}

/** System Clock Configuration after resume from stop mode
*/
void SystemClock_Config_PostStop(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSI14
                              |RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.HSI14CalibrationValue = 16;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI48;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART3|RCC_PERIPHCLK_USART1
                              |RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_I2C1
                              |RCC_PERIPHCLK_RTC;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
}

void RestorePowerConsumption(void)
{
  /* Configure Flash prefetch */ 
#if (PREFETCH_ENABLE != 0)
  __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
#endif /* PREFETCH_ENABLE */

  /* Use systick as time base source and configure 1ms tick (default clock after Reset is HSI) */
  // HAL_InitTick(TICK_INT_PRIORITY);

  /* Init the low level hardware */
  HAL_MspInit();

  // Configure the system clock
  SystemClock_Config_PostStop();

  // Gpio
  GpioEixtLowPower();

  // dma
  __HAL_RCC_DMA1_CLK_ENABLE();

  // ADC
  MX_ADC_Init();

  // // After resume form stop mode i2c works abnormally
  MX_I2C2_Init();
  MX_I2C1_Init();

  // PWM
  MX_TIM2_Init();

  // Uart
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  MX_USART2_UART_Init();

  // Modem
  // Enable RTS
  ModemRTSEnControl(ENABLE);
  // Module ensable
  ModemPowerEnControl(ENABLE);

  // Gps
  UbloxGPSStart();

  // AccGyro
  if (GetMcuDeepSleepAccState() != MCU_DEEPSLEEP_ACC_STATUS_NORMAL)
  {
    LIS3DH_SetMode(LIS3DH_NORMAL);
  }
}

#define portSY_FULL_READ_WRITE (15)
void MCUDeepSleep(uint32_t seconds)
{
  /* Enter a critical section but don't use the taskENTER_CRITICAL()
		method as that will mask interrupts that should exit sleep mode. */
  // __disable_irq();
  // __dsb(portSY_FULL_READ_WRITE);
  // __isb(portSY_FULL_READ_WRITE);

  HaltPowerConsumption();

  WedgeRtcSetBeforeDeepsleep(seconds);

  /* enter stop mode */
	HAL_SuspendTick();
  HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

  RestorePowerConsumption();
  
  WedgeRtcSetAfterDeepsleep();

  /* Re-enable interrupts - see comments above __disable_irq() call
			above. */
  // __enable_irq();
}

void MCUReset(void)
{
  // extern void SoftwareJumping(uint32_t Address);
  // SoftwareJumping(ROM_START_ADDRESS);
  __disable_irq();
  // __dsb(portSY_FULL_READ_WRITE);
  // __isb(portSY_FULL_READ_WRITE);
  // SoftwareJumping(ROM_START_ADDRESS);
  NVIC_SystemReset();
}

typedef void (*ApplicationInitTypedef)(void);
void SoftwareJumping(uint32_t Address)
{
  uint32_t JumpAddress;
  ApplicationInitTypedef Jump_To_Application;

  // Lock
  HAL_FLASH_Lock();
  // Jump to Address
  JumpAddress = *(__IO uint32_t *)(Address + 4);
  Jump_To_Application = (ApplicationInitTypedef)JumpAddress;

  // Initialize user application's Stack Pointer
  __set_MSP(*(__IO uint32_t *)Address);

  // Jump to application
  Jump_To_Application();
}

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2018. All rights reserved
                                End Of The File
*******************************************************************************/
