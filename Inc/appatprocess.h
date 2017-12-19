/*******************************************************************************
* File Name          : appatprocess.h
* Author             : 
* Description        : This file provides all the appatprocess functions.

* History:
*  12/12/2017 : appatprocess V1.00
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TC30M_APPATPROCESS_H__
#define __TC30M_APPATPROCESS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

#include "include.h"

#define APP_AT_LOG DebugLog
#define APP_AT_PRINT DebugPrintf

extern uint8_t *AppAtProcess(uint8_t * Data, uint8_t DataLen);
extern void AppUartAtProcess(uint8_t * UartData, uint8_t DataLen);
extern void AppUdpAtProcess(uint8_t * UdpData, uint8_t DataLen);
extern void AppSmsAtProcess(uint8_t * SmsData, uint8_t DataLen);

#ifdef __cplusplus
}
#endif

#endif /* __TC30M_APPATPROCESS_H__ */

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2017. All rights reserved
                                End Of The File
*******************************************************************************/
