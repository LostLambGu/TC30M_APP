/*******************************************************************************
* File Name          : eventalertflow.h
* Author             : Yangjie Gu
* Description        : This file provides all the eventalertflow functions.

* History:
*  12/14/2017 : eventalertflow V1.00
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _EVENT_ALERT_FLOW_
#define _EVENT_ALERT_FLOW_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

#include "wedgedatadefine.h"

/* Defines -------------------------------------------------------------------*/
typedef enum
{
    WEDGE_IGN_IGNORE_STATE = 0,
    WEDGE_IGN_OFF_STATE,
    WEDGE_IGN_OFF_TO_ON_STATE,
    WEDGE_IGN_ON_STATE,
    WEDGE_IGN_ON_TO_OFF_STATE,

    WEDGE_IGN_STATE_MAX
} WEDGEIgnitionStateTypeDef;

typedef struct
{
    double Longitude;
    double Latitude;
} TowAlertGeoFenceTypedef;

typedef struct
{
    WEDGEIgnitionStateTypeDef WEDGEIgnitionState;
    uint8_t PowerOnOff;
    uint8_t GpsLastFix;
    uint8_t GpsFix;
    double GpsVelocity;
    double Longitude;
    double Latitude;
    uint32_t SerOdoLastReportMileage;
    float VoltageValue;
    TowAlertGeoFenceTypedef TowAlertGeoFence;
    float HeadingLastReportDeg;
    MQSTATTypeDef MQSTAT;
} WEDGESysState;

extern void WedgeIgnitionStateProcess(void);


#ifdef __cplusplus
}
#endif

#endif /* _EVENT_ALERT_FLOW_ */

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2017. All rights reserved
                                End Of The File
*******************************************************************************/
