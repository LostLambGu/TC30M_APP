/*******************************************************************************
* File Name          : ltecatm_arrow.h
* Author             : Yangjie Gu
* Description        : This file provides all the ltecatm_arrow functions.

* History:
*  11/10/2017 : ltecatm_arrow V1.00
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LTECATM_ARROW_H__
#define __LTECATM_ARROW_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "include.h"

//Function Declare
extern u8 GetModemPoweronStat(void);
extern void SetModemNeedInitStat(u8 Status);
extern u8 GetModemNeedInitStat(void);
extern void SetSimCardReadyStat(u8 Status);
extern u8 GetSimCardReadyStat(void);
extern void SetUDPDataCanSendStat(u8 Status);
extern u8 GetUDPDataCanSendStat(void);
extern void SetSMSDataCanSendStat(u8 Status);
extern u8 GetSMSDataCanSendStat(void);

extern void LteCmdDetection(void);

#ifdef __cplusplus
}
#endif

#endif /* __LTECATM_ARROW_H__ */

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2017. All rights reserved
                                End Of The File
*******************************************************************************/
