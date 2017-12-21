/*******************************************************************************
* File Name          : eventalertflow.c
* Author             : Yangjie Gu
* Description        : This file provides all the eventalertflow functions.

* History:
*  12/14/2017 : eventalertflow V1.00
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "eventalertflow.h"

/* Defines -------------------------------------------------------------------*/
#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

static WEDGESysStateTypeDef WEDGESysState;

uint8_t WedgeSysStateInit(void)
{
    return TRUE;
}

void *WedgeSysStateGet(WEDGESysStateOperateTypeDef SysStateGet)
{
    switch (SysStateGet)
    {
    case WEDGE_IGNITION_STATE:
        return &(WEDGESysState.WEDGEIgnitionState);
        // break;

    case WEDGE_POWER_ON_OFF_STATE:
        return &(WEDGESysState.PowerOnOff);
        // break;

    case WEDGE_GPS_LASTFIX_STATE:
        return &(WEDGESysState.GpsLastFix);
        // break;

    case WEDGE_GPS_FIX_STATE:
        return &(WEDGESysState.GpsFix);
        // break;

    case WEDGE_GPS_VOLOCITY:
        return &(WEDGESysState.GpsVelocity);
        // break;

    case WEDGE_LONGTITUDE:
        return &(WEDGESysState.Longitude);
        // break;

    case WEDGE_LATITUDE:
        return &(WEDGESysState.Latitude);
        // break;

    case WEDGE_SODO_LASTREPORT_MILEAGE:
        return &(WEDGESysState.SerOdoLastReportMileage);
        // break;

    case WEDGE_VOLTAGE_VALUE:
        return &(WEDGESysState.VoltageValue);
        // break;

    case WEDGE_TOW_ALERTGEOFENCE:
        return &(WEDGESysState.TowAlertGeoFence);
        // break;

    case WEDGE_HEADING_LASTREPORT_DEG:
        return &(WEDGESysState.HeadingLastReportDeg);
        // break;

    case WEDGE_MQSTAT:
        return &(WEDGESysState.MQSTAT);
        // break;

    case WEDGE_BASE_RTC_TIME:
        return &(WEDGESysState.RTCBaseTime);
        // break;

    case WEDGE_LAST_HWRST_RTC_TIME:
        return &(WEDGESysState.LastHWRSTRTCTime);

    default:
        EVENT_ALERT_FLOW_LOG("WEDGE SYS Get: Param err");
        return NULL;
        // break;
    }
}

void WedgeSysStateSet(WEDGESysStateOperateTypeDef SysStateSet, const void *pvData)
{
    switch (SysStateSet)
    {
    case WEDGE_IGNITION_STATE:
        WEDGESysState.WEDGEIgnitionState = *((WEDGEIgnitionStateTypeDef *)pvData);
        break;

    case WEDGE_POWER_ON_OFF_STATE:
        WEDGESysState.PowerOnOff = *((uint8_t *)pvData);
        break;

    case WEDGE_GPS_LASTFIX_STATE:
        WEDGESysState.GpsLastFix = *((uint8_t *)pvData);
        break;

    case WEDGE_GPS_FIX_STATE:
        WEDGESysState.GpsFix = *((uint8_t *)pvData);
        break;

    case WEDGE_GPS_VOLOCITY:
        WEDGESysState.GpsVelocity = *((double *)pvData);
        break;

    case WEDGE_LONGTITUDE:
        WEDGESysState.Longitude = *((double *)pvData);
        break;

    case WEDGE_LATITUDE:
        WEDGESysState.Latitude = *((double *)pvData);
        break;

    case WEDGE_SODO_LASTREPORT_MILEAGE:
        WEDGESysState.SerOdoLastReportMileage = *((uint32_t *)pvData);
        break;

    case WEDGE_VOLTAGE_VALUE:
        WEDGESysState.VoltageValue = *((float *)pvData);
        break;

    case WEDGE_TOW_ALERTGEOFENCE:
        WEDGESysState.TowAlertGeoFence = *((TowAlertGeoFenceTypedef *)pvData);
        break;

    case WEDGE_HEADING_LASTREPORT_DEG:
        WEDGESysState.HeadingLastReportDeg = *((float *)pvData);
        break;

    case WEDGE_MQSTAT:
        WEDGESysState.MQSTAT = *((MQSTATTypeDef *)pvData);
        break;

    case WEDGE_BASE_RTC_TIME:
        WEDGESysState.RTCBaseTime = *((uint32_t *)pvData);
        break;

    case WEDGE_LAST_HWRST_RTC_TIME:
        WEDGESysState.LastHWRSTRTCTime = *((uint32_t *)pvData);
        break;

    default:
        EVENT_ALERT_FLOW_LOG("WEDGE SYS Set: Param err");
        break;
    }

    return;
}

void WedgeIgnitionStateProcess(void)
{
    switch (*((WEDGEIgnitionStateTypeDef *)WedgeSysStateGet(WEDGE_IGNITION_STATE)))
    {
    case WEDGE_IGN_IGNORE_STATE:
        break;

    case WEDGE_IGN_OFF_STATE:
        break;

    case WEDGE_IGN_OFF_TO_ON_STATE:
        break;

    case WEDGE_IGN_ON_STATE:
        break;

    case WEDGE_IGN_ON_TO_OFF_STATE:
        break;

    default:
        EVENT_ALERT_FLOW_LOG("WEDGE IGN STAT: err");
        break;
    }

    return;
}

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2017. All rights reserved
                                End Of The File
*******************************************************************************/
