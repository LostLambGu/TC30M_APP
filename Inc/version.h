/*******************************************************************************
* File Name          : version.c
* Author             : Yangjie Gu
* Description        : This file provides all the version functions.

* History:
*  10/25/2017 : version V1.00
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _VERSION_SOFTWARE_H
#define _VERSION_SOFTWARE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

//Function Declare
extern void ShowSoftVersion(void);
extern void GetAppVersion(uint32_t *pMain, uint32_t *pSub);

#ifdef __cplusplus
}
#endif

#endif /* _VERSION_SOFTWARE_H */

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2018. All rights reserved
                                End Of The File
*******************************************************************************/
