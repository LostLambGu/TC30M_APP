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

#include "initialization.h"
#include "deepsleep.h"
#include "rtcclock.h"
#include "ublox_driver.h"

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
  
}

void HaltPowerConsumption(void)
{

}

void GpioEixtLowPower(void)
{

}

void RestorePowerConsumption(void)
{

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

  SetRTCAlarmTime(seconds, TRUE);

  /* enter stop mode */
	HAL_SuspendTick();
  HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

  HAL_Init();
  SystemClock_Config();

  RestorePowerConsumption();
  // extern void SystemClock_Config_PostStop(void);
  // SystemClock_Config_PostStop();

  /* Re-enable interrupts - see comments above __disable_irq() call
			above. */
  // __enable_irq();
}

void MCUReset(void)
{
  // extern void SoftwareJumping(uint32_t Address);
  // SoftwareJumping(ROM_START_ADDRESS);
  // __disable_irq();
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
