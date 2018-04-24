/*******************************************************************************
* File Name          : deepsleep.h
* Author             : Yangjie Gu
* Description        : This file provides all the deepsleep functions.

* History:
*  10/23/2017 : deepsleep V1.00
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DEEPSLEEP_H_
#define __DEEPSLEEP_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

/* Exported constants --------------------------------------------------------*/
#define ROM_START_ADDRESS 0x8000000

/* Exported functions --------------------------------------------------------*/
extern void MCUDeepSleep(uint32_t seconds);
extern void MCUEnterDeepSleep(void);
extern void MCUExitDeepSleep(void);

extern void SoftwareJumping(uint32_t Address);
extern void MCUReset(void);

#ifdef __cplusplus
}
#endif

#endif /* __DEEPSLEEP_H_ */

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2018. All rights reserved
                                End Of The File
*******************************************************************************/
