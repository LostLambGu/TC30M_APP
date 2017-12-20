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

void *WedgeSysStateGet(WEDGESysStateGetTypeDef SysStateGet)
{
    switch (SysStateGet)
    {
    case WEDGE_IGNITION_STATE_GET:
        return &(WEDGESysState.WEDGEIgnitionState);
        // break;

    case WEDGE_POWER_ON_OFF_STATE_GET:
        return &(WEDGESysState.PowerOnOff);
        // break;

    case WEDGE_GPS_LASTFIX_STATE_GET:
        return &(WEDGESysState.GpsLastFix);
        // break;

    case WEDGE_GPS_FIX_STATE_GET:
        return &(WEDGESysState.GpsFix);
        // break;

    case WEDGE_GPS_VOLOCITY_GET:
        return &(WEDGESysState.GpsVelocity);
        // break;

    case WEDGE_LONGTITUDE_GET:
        return &(WEDGESysState.Longitude);
        // break;

    case WEDGE_LATITUDE_GET:
        return &(WEDGESysState.Latitude);
        // break;

    case WEDGE_SODO_LASTREPORT_MILEAGE_GET:
        return &(WEDGESysState.SerOdoLastReportMileage);
        // break;

    case WEDGE_VOLTAGE_VALUE_GET:
        return &(WEDGESysState.VoltageValue);
        // break;

    case WEDGE_TOW_ALERTGEOFENCE_GET:
        return &(WEDGESysState.TowAlertGeoFence);
        // break;

    case WEDGE_HEADING_LASTREPORT_DEG_GET:
        return &(WEDGESysState.HeadingLastReportDeg);
        // break;

    case WEDGE_MQSTAT_GET:
        return &(WEDGESysState.MQSTAT);
        // break;

    default:
        EVENT_ALERT_FLOW_LOG("WEDGE SYS Get: Param err");
        return NULL;
        // break;
    }
}

void WedgeSysStateSet(WEDGESysStateGetTypeDef SysStateSet, const void *pvData)
{
    switch (SysStateSet)
    {
    case WEDGE_IGNITION_STATE_GET:
        WEDGESysState.WEDGEIgnitionState = *((WEDGEIgnitionStateTypeDef *)pvData);
        break;

    case WEDGE_POWER_ON_OFF_STATE_GET:
        WEDGESysState.PowerOnOff = *((uint8_t *)pvData);
        break;

    case WEDGE_GPS_LASTFIX_STATE_GET:
        WEDGESysState.GpsLastFix = *((uint8_t *)pvData);
        break;

    case WEDGE_GPS_FIX_STATE_GET:
        WEDGESysState.GpsFix = *((uint8_t *)pvData);
        break;

    case WEDGE_GPS_VOLOCITY_GET:
        WEDGESysState.GpsVelocity = *((double *)pvData);
        break;

    case WEDGE_LONGTITUDE_GET:
        WEDGESysState.Longitude = *((double *)pvData);
        break;

    case WEDGE_LATITUDE_GET:
        WEDGESysState.Latitude = *((double *)pvData);
        break;

    case WEDGE_SODO_LASTREPORT_MILEAGE_GET:
        WEDGESysState.SerOdoLastReportMileage = *((uint32_t *)pvData);
        break;

    case WEDGE_VOLTAGE_VALUE_GET:
        WEDGESysState.VoltageValue = *((float *)pvData);
        break;

    case WEDGE_TOW_ALERTGEOFENCE_GET:
        WEDGESysState.TowAlertGeoFence = *((TowAlertGeoFenceTypedef *)pvData);
        break;

    case WEDGE_HEADING_LASTREPORT_DEG_GET:
        WEDGESysState.HeadingLastReportDeg = *((float *)pvData);
        break;

    case WEDGE_MQSTAT_GET:
        WEDGESysState.MQSTAT = *((MQSTATTypeDef *)pvData);
        break;

    default:
        EVENT_ALERT_FLOW_LOG("WEDGE SYS Set: Param err");
        break;
    }

    return;
}

void WedgeIgnitionStateProcess(void)
{
    switch (*((WEDGEIgnitionStateTypeDef *)WedgeSysStateGet(WEDGE_IGNITION_STATE_GET)))
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
