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

void MCUDeepSleep(uint32_t seconds)
{

}

void MCUReset(void)
{
  // extern void SoftwareJumping(uint32_t Address);
  // SoftwareJumping(ROM_START_ADDRESS);
  // __disable_irq();
  // __dsb(portSY_FULL_READ_WRITE);
  // __isb(portSY_FULL_READ_WRITE);
  SoftwareJumping(ROM_START_ADDRESS);
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
    Copyrights (C) Asiatelco Technologies Co., 2003-2017. All rights reserved
                                End Of The File
*******************************************************************************/
