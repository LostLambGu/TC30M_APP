/*******************************************************************************
* File Name          : wedgeapplication.h
* Author             : Yangjie Gu
* Description        : This file provides all the wedgeapplication functions.

* History:
*  12/12/2017 : wedgeapplication V1.00
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TC30M_APPLICATION_H__
#define __TC30M_APPLICATION_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

#include "initialization.h"
#include "iocontrol.h"
#include "uart_api.h"
#include "atcmd.h"
#include "limifsm.h"
#include "lis3dh_driver.h"
#include "rtcclock.h"
#include "iqmgr.h"
#include "usrtimer.h"
#include "sendatcmd.h"
#include "ltecatm.h"
#include "network.h"
#include "parseatat.h"

#include "wedgedatadefine.h"
#include "wedgecommonapi.h"
#include "wedgeeventalertflow.h"
#include "wedgedeviceinfo.h"
#include "wedgertctimer.h"
#include "wedgemsgque.h"

typedef struct
{
    WEDGECfgTypeDef WEDGECfg;
    WEDGESysStateTypeDef WEDGESysState;
    RTCTimerListTypeDef RTCTimerList;
} WEDGEDeviceInfoTypeDef;

extern void ApplicationProcess(void);

#ifdef __cplusplus
}
#endif

#endif /* __TC30M_APPLICATION_H__ */

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2018. All rights reserved
                                End Of The File
*******************************************************************************/
