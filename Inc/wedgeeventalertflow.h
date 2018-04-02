/*******************************************************************************
* File Name          : wedgeeventalertflow.h
* Author             : Yangjie Gu
* Description        : This file provides all the wedgeeventalertflow functions.

* History:
*  12/14/2017 : wedgeeventalertflow V1.00
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef WEDGE_EVENT_ALERT_FLOW_
#define WEDGE_EVENT_ALERT_FLOW_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

#include "uart_api.h"
#include "wedgedatadefine.h"

/* Defines -------------------------------------------------------------------*/
typedef enum
{
    WEDGE_IGN_TO_IGNORE_STATE = 0,
    WEDGE_IGN_IGNORE_STATE,
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
    uint16_t unsent;
    uint16_t sent;
    uint16_t queinindex;
    uint16_t queoutindex;
} MQSTATTypeDef;

typedef enum
{
    WEDGE_IGNITION_STATE = 0,
    WEDGE_IGNITION_CHANGE_DETECTED,
    WEDGE_IGN_OFFTOON_TIMER_START,
    WEDGE_IGN_ONTOOFF_TIMER_START,
    WEDGE_POWER_ON_OFF_STATE,
    WEDGE_GPS_LASTFIX_STATE,
    WEDGE_GPS_FIX_STATE,
    WEDGE_GPS_VOLOCITY,
    WEDGE_LONGTITUDE,
    WEDGE_LATITUDE,
    WEDGE_SODO_LASTREPORT_MILEAGE,
    WEDGE_VOLTAGE_VALUE,
    WEDGE_IDLE_DETECT_TIMER_START,
    WEDGE_TOW_ALERT_ONCE_ALREADY,
    WEDGE_OUT_TOW_GEOFNC_COUNT,
    WEDGE_TOW_ALERTGEOFENCE,
    WEDGE_GEOFENCE_BIT_MAP,
    WEDGE_STARTER_DISABLE_CMD_RECEIVED,
    WEDGE_STOP_REPORT_ONTIME_RTC_TIMER_ADDED,
    WEDGE_OVER_SPEED_ALERT_TIMER_START,
    WEDGE_HEADING_LASTREPORT_DEG,
    WEDGE_PERIODIC_OFF_EVENT_TIMER_START,
    WEDGE_PERIODIC_HEALTEH_EVENT_TIMER_START,
    WEDGE_PERIODIC_HARDWARE_RESET_TIMER_START,
    WEDGE_MQSTAT,
    WEDGE_BASE_RTC_TIME,
    WEDGE_LAST_HWRST_RTC_TIME,

    WEDGE_SYSSTAT_MAX
} WEDGESysStateOperateTypeDef;

typedef struct
{
    WEDGEIgnitionStateTypeDef WEDGEIgnitionState;
    uint8_t WedgeIgnitionChangeDetected;
    uint8_t WedgeIgnOffToOnTimerStart;
    uint8_t WedgeIgnOnToOffTimerStart;
    uint8_t PowerOnOff;
    uint8_t GpsLastFix;
    uint8_t GpsFix;
    double GpsVelocity;
    double Longitude;
    double Latitude;
    uint32_t SerOdoLastReportMileage;
    float VoltageValue;
    uint8_t IDLEDtectTimerStart;
    uint8_t TowAlertOnceAlready;
    uint8_t OutTowGeoFncCount;
    TowAlertGeoFenceTypedef TowAlertGeoFence;
    uint16_t GeofenceDefinedBitMap;
    uint8_t StarterDisableCmdRec;
    uint8_t StopReportOnetimeRtcTimerAdded;
    uint8_t OverSpeedTimerStart;
    float HeadingLastReportDeg;
    uint8_t PeriodicOffEventTimerStart;
    uint8_t PeriodicHealthEventTimerStart;
    uint8_t PeriodicHardwareResetTimerStart;
    MQSTATTypeDef MQSTAT;
    uint32_t RTCBaseTime;
    uint32_t LastHWRSTRTCTime;
} WEDGESysStateTypeDef;

extern uint8_t WedgeSysStateInit(WEDGESysStateTypeDef *pWEDGESysState);
extern void WedgeIgnitionStateSet(WEDGEIgnitionStateTypeDef IgnitionState);
extern WEDGEIgnitionStateTypeDef WedgeIgnitionStateGet(void);
extern void WedgeSysStateGetTotal(uint8_t *pBuf, uint32_t *pSize);
extern void *WedgeSysStateGet(WEDGESysStateOperateTypeDef SysStateGet);
extern void WedgeSysStateSet(WEDGESysStateOperateTypeDef SysStateSet, const void *pvData);

extern void WedgeServiceOdometerAlert(void);

extern void WedgeLowBatteryAlert(void);

extern void WedgeIDLEDetectAlertReset(void);
extern void WedgeIDLEDetectAlert(void);

extern void WedgeTowAlertReset(void);
extern void WedgeTowAlert(void);

extern void WedgeGeofenceAlert(void);

extern void WedgeLocationOfDisabledVehicleOnToOff(void);

extern void WedgeLocationOfDisabledVehicleReset(void);
extern void WedgeLocationOfDisabledVehicle(void);

extern void WedgeStopReportOnToOffDisable(void);
extern void WedgeStopReportOnToOff(void);

extern void WedgeOverSpeedAlertReset(void);
extern void WedgeOverSpeedAlert(void);

extern void WedgeIgnitionOffToOnCheckReset(void);
extern void WedgeIgnitionOffToOnCheck(void);

extern void WedgeIgnitionOnToOffCheckReset(void);
extern void WedgeIgnitionOnToOffCheck(void);

extern void WedgeHeadingChangeDetect(void);

extern void WedgePeriodicMovingEventInit(void);

extern void WedgePeriodicOffEventReset(void);
extern void WedgePeriodicOffEvent(void);

extern void WedgePeriodicHealthEventReset(void);
extern void WedgePeriodicHealthEvent(void);

extern void WedgePeriodicHardwareResetReinit(void);
extern void WedgePeriodicHardwareResetInit(void);

extern void WedgeGpsRequestDataProcess(void);

#ifdef __cplusplus
}
#endif

#endif /* WEDGE_EVENT_ALERT_FLOW_ */

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2018. All rights reserved
                                End Of The File
*******************************************************************************/
