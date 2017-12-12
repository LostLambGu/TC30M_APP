/*******************************************************************************
* File Name          : PwmAudio.h
* Author             : Taotao Yan
* Description        : This file provides all the PwmAudio functions.

* History:
*  12/20/2014 : PwmAudioV1.00
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PWMAUDIO_H
#define _PWMAUDIO_H

#include "stm32f0xx_hal.h"

// Range (20Hz--20kHz)
#define PERIOD_SOURCE 16000000

#define PERIOD_0050Hz (uint32_t)((PERIOD_SOURCE / 50) - 1)
#define PERIOD_0100Hz (uint32_t)((PERIOD_SOURCE / 100) - 1)
#define PERIOD_0300Hz (uint32_t)((PERIOD_SOURCE / 300) - 1)
#define PERIOD_0500Hz (uint32_t)((PERIOD_SOURCE / 500) - 1)
#define PERIOD_0800Hz (uint32_t)((PERIOD_SOURCE / 800) - 1)
#define PERIOD_1000Hz (uint32_t)((PERIOD_SOURCE / 1000) - 1)
#define PERIOD_1300Hz (uint32_t)((PERIOD_SOURCE / 1300) - 1)
#define PERIOD_1500Hz (uint32_t)((PERIOD_SOURCE / 1500) - 1)
#define PERIOD_1800Hz (uint32_t)((PERIOD_SOURCE / 1800) - 1)
#define PERIOD_2000Hz (uint32_t)((PERIOD_SOURCE / 2000) - 1)
#define PERIOD_2300Hz (uint32_t)((PERIOD_SOURCE / 2300) - 1)
#define PERIOD_2500Hz (uint32_t)((PERIOD_SOURCE / 2500) - 1)
#define PERIOD_2700Hz (uint32_t)((PERIOD_SOURCE / 2700) - 1)
#define PERIOD_2800Hz (uint32_t)((PERIOD_SOURCE / 2800) - 1)
#define PERIOD_3000Hz (uint32_t)((PERIOD_SOURCE / 3000) - 1)
#define PERIOD_3300Hz (uint32_t)((PERIOD_SOURCE / 3300) - 1)
#define PERIOD_3500Hz (uint32_t)((PERIOD_SOURCE / 3500) - 1)
#define PERIOD_3800Hz (uint32_t)((PERIOD_SOURCE / 3800) - 1)
#define PERIOD_4000Hz (uint32_t)((PERIOD_SOURCE / 4000) - 1)
#define PERIOD_4300Hz (uint32_t)((PERIOD_SOURCE / 4300) - 1)
#define PERIOD_4500Hz (uint32_t)((PERIOD_SOURCE / 4500) - 1)
#define PERIOD_4800Hz (uint32_t)((PERIOD_SOURCE / 4800) - 1)
#define PERIOD_5000Hz (uint32_t)((PERIOD_SOURCE / 5000) - 1)
#define PERIOD_12000HZ (uint32_t)((PERIOD_SOURCE / 12000) - 1)
// Variable Declared

//Function Declare
extern void StartPWMAudio(uint32_t freq, uint8_t ratio);
extern void StopPWMAudio(void);

#endif

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2017. All rights reserved
                                End Of The File
*******************************************************************************/
