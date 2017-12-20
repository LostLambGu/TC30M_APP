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

#include "uart_api.h"
#include "wedgedatadefine.h"

/* Defines -------------------------------------------------------------------*/
#define EVENT_ALERT_FLOW_LOG DebugLog
#define EVENT_ALERT_FLOW_PRINT DebugPrintf

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

typedef enum
{
    WEDGE_IGNITION_STATE_GET = 0,
    WEDGE_POWER_ON_OFF_STATE_GET,
    WEDGE_GPS_LASTFIX_STATE_GET,
    WEDGE_GPS_FIX_STATE_GET,
    WEDGE_GPS_VOLOCITY_GET,
    WEDGE_LONGTITUDE_GET,
    WEDGE_LATITUDE_GET,
    WEDGE_SODO_LASTREPORT_MILEAGE_GET,
    WEDGE_VOLTAGE_VALUE_GET,
    WEDGE_TOW_ALERTGEOFENCE_GET,
    WEDGE_HEADING_LASTREPORT_DEG_GET,
    WEDGE_MQSTAT_GET,

    WEDGE_SYSSTAT_GET_MAX
} WEDGESysStateGetTypeDef;

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
} WEDGESysStateTypeDef;

extern uint8_t WedgeSysStateInit(void);
extern void *WedgeSysStateGet(WEDGESysStateGetTypeDef SysStateGet);
extern void WedgeSysStateSet(WEDGESysStateGetTypeDef SysStateSet, const void *pvData);
extern void WedgeIgnitionStateProcess(void);


#ifdef __cplusplus
}
#endif

#endif /* _EVENT_ALERT_FLOW_ */

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2017. All rights reserved
                                End Of The File
*******************************************************************************/
