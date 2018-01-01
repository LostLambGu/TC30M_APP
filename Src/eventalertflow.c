/*******************************************************************************
* File Name          : eventalertflow.c
* Author             : Yangjie Gu
* Description        : This file provides all the eventalertflow functions.

* History:
*  12/14/2017 : eventalertflow V1.00
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "eventalertflow.h"
#include "eventmsgque_process.h"
#include "wedgertctimer.h"
#include "flash.h"
#include "ublox_driver.h"
#include "iocontrol.h"

/* Defines -------------------------------------------------------------------*/
#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define EVENT_ALERT_FLOW_LOG DebugLog
#define EVENT_ALERT_FLOW_PRINT DebugPrintf

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

    case WEDGE_IGNITION_CHANGE_DETECTED:
        return &(WEDGESysState.WedgeIgnitionChangeDetected);
        // break;

    case WEDGE_IGN_OFFTOON_TIMER_START:
        return &(WEDGESysState.WedgeIgnOffToOnTimerStart);
        // break;

    case WEDGE_IGN_ONTOOFF_TIMER_START:
        return &(WEDGESysState.WedgeIgnOnToOffTimerStart);
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

    case WEDGE_IDLE_DETECT_TIMER_START:
        return &(WEDGESysState.IDLEDtectTimerStart);
        // break;

    case WEDGE_TOW_ALERT_ONCE_ALREADY:
        return &(WEDGESysState.TowAlertOnceAlready);
        // break;

    case WEDGE_OUT_TOW_GEOFNC_COUNT:
        return &(WEDGESysState.OutTowGeoFncCount);
        // break;

    case WEDGE_TOW_ALERTGEOFENCE:
        return &(WEDGESysState.TowAlertGeoFence);
        // break;
    
    case WEDGE_GEOFENCE_BIT_MAP:
        return &(WEDGESysState.GeofenceDefinedBitMap);
        // break;

    case WEDGE_STARTER_DISABLE_CMD_RECEIVED:
        return &(WEDGESysState.StarterDisableCmdRec);
        // break;

    case WEDGE_OVER_SPEED_ALERT_TIMER_START:
        return &(WEDGESysState.OverSpeedTimerStart);
        // break;

    case WEDGE_HEADING_LASTREPORT_DEG:
        return &(WEDGESysState.HeadingLastReportDeg);
        // break;

    case WEDGE_PERIODIC_OFF_EVENT_TIMER_START:
        return &(WEDGESysState.PeriodicOffEventTimerStart);
        // break;

    case WEDGE_PERIODIC_HEALTEH_EVENT_TIMER_START:
        return &(WEDGESysState.PeriodicHealthEventStart);
        // break;

    case WEDGE_PERIODIC_HARDWARE_RESET_TIMER_START:
        return &(WEDGESysState.PeriodicHardwareResetTimerStart);
        // break;

    case WEDGE_MQSTAT:
        return &(WEDGESysState.MQSTAT);
        // break;

    case WEDGE_BASE_RTC_TIME:
        return &(WEDGESysState.RTCBaseTime);
        // break;

    case WEDGE_LAST_HWRST_RTC_TIME:
        return &(WEDGESysState.LastHWRSTRTCTime);
        // break;

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

    case WEDGE_IGNITION_CHANGE_DETECTED:
        WEDGESysState.WedgeIgnitionChangeDetected = *((uint8_t *)pvData);
        break;

    case WEDGE_IGN_OFFTOON_TIMER_START:
        WEDGESysState.WedgeIgnOffToOnTimerStart = *((uint8_t *)pvData);
        break;

    case WEDGE_IGN_ONTOOFF_TIMER_START:
        WEDGESysState.WedgeIgnOnToOffTimerStart = *((uint8_t *)pvData);
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

    case WEDGE_IDLE_DETECT_TIMER_START:
        WEDGESysState.IDLEDtectTimerStart = *((uint8_t *)pvData);
        break;

    case WEDGE_TOW_ALERT_ONCE_ALREADY:
        WEDGESysState.TowAlertOnceAlready = *((uint8_t *)pvData);
        break;

    case WEDGE_OUT_TOW_GEOFNC_COUNT:
        WEDGESysState.OutTowGeoFncCount = *((uint8_t *)pvData);
        break;

    case WEDGE_TOW_ALERTGEOFENCE:
        WEDGESysState.TowAlertGeoFence = *((TowAlertGeoFenceTypedef *)pvData);
        break;

    case WEDGE_GEOFENCE_BIT_MAP:
        WEDGESysState.GeofenceDefinedBitMap = *((uint16_t *)pvData);
        break;

    case WEDGE_STARTER_DISABLE_CMD_RECEIVED:
        WEDGESysState.StarterDisableCmdRec = *((uint8_t *)pvData);
        break;

    case WEDGE_OVER_SPEED_ALERT_TIMER_START:
        WEDGESysState.OverSpeedTimerStart = *((uint8_t *)pvData);
        break;

    case WEDGE_HEADING_LASTREPORT_DEG:
        WEDGESysState.HeadingLastReportDeg = *((float *)pvData);
        break;

    case WEDGE_PERIODIC_OFF_EVENT_TIMER_START:
        WEDGESysState.PeriodicOffEventTimerStart = *((uint8_t *)pvData);
        break;

    case WEDGE_PERIODIC_HEALTEH_EVENT_TIMER_START:
        WEDGESysState.PeriodicHealthEventStart = *((uint8_t *)pvData);
        break;

    case WEDGE_PERIODIC_HARDWARE_RESET_TIMER_START:
        WEDGESysState.PeriodicHardwareResetTimerStart = *((uint8_t *)pvData);
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

void WedgeServiceOdometerAlert(void)
{
    VODOTypeDef VODO = {0};
    SODOTypeDef SODO = {0};
    uint32_t SerOdoLastReportMileage = 0;

    VODO = *((VODOTypeDef *)WedgeCfgGet(WEDGE_CFG_VODO));

    SerOdoLastReportMileage = *((uint32_t *)WedgeSysStateGet(WEDGE_SODO_LASTREPORT_MILEAGE));

    if (SerOdoLastReportMileage > VODO.meters)
    {
        EVENT_ALERT_FLOW_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Evt Alrt Flw SODO err"
                                , FmtTimeShow());
        WedgeSysStateSet(WEDGE_SODO_LASTREPORT_MILEAGE, &(VODO.meters));

        return;
    }

    SODO = *((SODOTypeDef *)WedgeCfgGet(WEDGE_CFG_SODO));
    if ((VODO.meters - SerOdoLastReportMileage) >= SODO.meters)
    {
        EVENT_ALERT_FLOW_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Ser Odo Alrt"
                                , FmtTimeShow());
        WedgeResponseUdpBinary(WEDGEPYLD_STATUS, Service_Alert);
        WedgeSysStateSet(WEDGE_SODO_LASTREPORT_MILEAGE, &(VODO.meters));
        return;
    }
}

extern double ADCGetVinVoltage(void);
void WedgeLowBatteryAlert(void)
{
    LVATypeDef LVA = {0};
    float voltage = 0.0;

    LVA = *((LVATypeDef *)WedgeCfgGet(WEDGE_CFG_LVA));
    voltage = (float)ADCGetVinVoltage();

    if (voltage < LVA.battlvl)
    {
        WedgeResponseUdpBinary(WEDGEPYLD_STATUS, Low_Battery_Alert);
        return;
    }
}

#define WEDGE_IDLE_DETECT_SPPED_MILE (3)
#define MILE_TO_KM_FACTOR (1.6093)
extern TIMER WedgeIDLETimer;

static void CheckWedgeIDLETimerCallback(uint8_t status)
{
    uint8_t IDLEDtectTimerStart = 0;

    EVENT_ALERT_FLOW_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE IDLE Alrt"
                                , FmtTimeShow());
    WedgeResponseUdpBinary(WEDGEPYLD_STATUS, IDLE_Detect);

    WedgeSysStateSet(WEDGE_IDLE_DETECT_TIMER_START, &IDLEDtectTimerStart);
}

void WedgeIDLEDetectAlert(void)
{
    IDLETypeDef IDLE = {0};
    double speedkm = 0.0;
    uint8_t IDLEDtectTimerStart = 0;
    static uint32_t SystickRec = 0;

    // if (UbloxFixStateGet() == FALSE)
    // {
    //     return;
    // }

    if ((CHECK_UBLOX_STAT_TIMEOUT + 100) < (HAL_GetTick() - SystickRec))
    {
        return;
    }
    else
    {
        SystickRec = HAL_GetTick();
    }

    IDLE = *((IDLETypeDef *)WedgeCfgGet(WEDGE_CFG_IDLE));

    if (IDLE.duration == 0)
    {
        return;
    }

    IDLEDtectTimerStart = *((uint32_t *)WedgeSysStateGet(WEDGE_IDLE_DETECT_TIMER_START));
    speedkm = UbloxSpeedKM();
    if (UbloxFixStateGet())
    {
        if (speedkm <= (WEDGE_IDLE_DETECT_SPPED_MILE * MILE_TO_KM_FACTOR))
        {
            if (IDLEDtectTimerStart == 0)
            {
                SoftwareTimerCreate(&WedgeIDLETimer, 1, CheckWedgeIDLETimerCallback, TimeMsec(IDLE.duration));
                SoftwareTimerStart(&WedgeIDLETimer);

                IDLEDtectTimerStart = TRUE;
                WedgeSysStateSet(WEDGE_IDLE_DETECT_TIMER_START, &IDLEDtectTimerStart);
            }
            else
            {
                // Check Wedge IDLE Detect Timer
                IsSoftwareTimeOut(&WedgeIDLETimer);
            }
        }
        else
        {
            if (IDLEDtectTimerStart == TRUE)
            {
                SoftwareTimerReset(&WedgeIDLETimer, CheckWedgeIDLETimerCallback, TimeMsec(IDLE.duration));
                SoftwareTimerStop(&WedgeIDLETimer);

                IDLEDtectTimerStart = 0;
                WedgeSysStateSet(WEDGE_IDLE_DETECT_TIMER_START, &IDLEDtectTimerStart);
            }
        }
    }
    else
    {
        if (IDLEDtectTimerStart == 0)
        {
           SoftwareTimerCreate(&WedgeIDLETimer, 1, CheckWedgeIDLETimerCallback, TimeMsec(IDLE.duration));
           SoftwareTimerStart(&WedgeIDLETimer);

           IDLEDtectTimerStart = TRUE;
           WedgeSysStateSet(WEDGE_IDLE_DETECT_TIMER_START, &IDLEDtectTimerStart);
        }
        else
        {
            // Check Wedge IDLE Detect Timer
            IsSoftwareTimeOut(&WedgeIDLETimer);
        }
    }
}

#define WEDGE_OUT_TOW_GEOFENCE_CONSECUTIVE_MAX_TIMES (10)

static uint8_t WedgeIsOutsideTowGeoFnc(TowAlertGeoFenceTypedef TowAlertGeoFence, uint16_t radius)
{







    return 1; // Yes
}

void WedgeTowAlert(void)
{
    TOWTypeDef TOW;
    TowAlertGeoFenceTypedef TowAlertGeoFence;
    uint8_t TowAlertOnceAlready;
    uint8_t OutTowGeoFncCount;
    static uint32_t SystickRec = 0;

    // if (UbloxFixStateGet() == FALSE)
    // {
    //     return;
    // }

    if ((CHECK_UBLOX_STAT_TIMEOUT + 100) < (HAL_GetTick() - SystickRec))
    {
        return;
    }
    else
    {
        SystickRec = HAL_GetTick();
    }

    TowAlertOnceAlready = *((uint8_t *)WedgeSysStateGet(WEDGE_TOW_ALERT_ONCE_ALREADY));
    if (TRUE == TowAlertOnceAlready)
    {
        return;
    }

    TOW = *((TOWTypeDef *)WedgeCfgGet(WEDGE_CFG_TOW));

    if (TOW.enable == 0)
    {
        return;
    }

    OutTowGeoFncCount = *((uint8_t *)WedgeSysStateGet(WEDGE_OUT_TOW_GEOFNC_COUNT));

    TowAlertGeoFence = *((TowAlertGeoFenceTypedef *)WedgeSysStateGet(WEDGE_TOW_ALERTGEOFENCE));
    if (WedgeIsOutsideTowGeoFnc(TowAlertGeoFence, TOW.radius))
    {
        OutTowGeoFncCount++;

        if (WEDGE_OUT_TOW_GEOFENCE_CONSECUTIVE_MAX_TIMES <= OutTowGeoFncCount)
        {
            EVENT_ALERT_FLOW_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Tow Alrt"
                                    , FmtTimeShow());
            WedgeResponseUdpBinary(WEDGEPYLD_STATUS, Tow_Alert_Exited);

            TowAlertOnceAlready = TRUE;
            WedgeSysStateSet(WEDGE_TOW_ALERT_ONCE_ALREADY, &TowAlertOnceAlready);
        }
    }
    else
    {
        OutTowGeoFncCount = 0;
    }

    WedgeSysStateSet(WEDGE_OUT_TOW_GEOFNC_COUNT, &OutTowGeoFncCount);
}

static uint8_t WedgeIsGeofenceViolation(GFNCTypeDef GFNC)
{






    return 1; // Enter 1, Exit 2.
}

void WedgeGeofenceAlert(void)
{
    static uint32_t SystickRec = 0;
    uint16_t GeofenceDefinedBitMap;
    uint8_t i = 0;
    GFNCTypeDef GFNC = {0};
    static uint8_t GfncViolationCount[WEDGE_GEOFENCES_NUM_MAX] = {0};
    uint8_t ViolationType = 0;

    // if (UbloxFixStateGet() == FALSE)
    // {
    //     return;
    // }

    GeofenceDefinedBitMap = *((uint16_t *)WedgeSysStateGet(WEDGE_GEOFENCE_BIT_MAP));

    if (0 == GeofenceDefinedBitMap)
    {
        return;
    }

    if ((CHECK_UBLOX_STAT_TIMEOUT + 100) < (HAL_GetTick() - SystickRec))
    {
        return;
    }
    else
    {
        SystickRec = HAL_GetTick();
    }

    for (i = 0; i < WEDGE_GEOFENCES_NUM_MAX; i++)
    {
        if ((GeofenceDefinedBitMap >> i) & 0x01)
        {
            GFNC = *((GFNCTypeDef *)WedgeCfgGet((WEDGECfgOperateTypeDef)(WEDGE_CFG_GFNC1 + i)));
            ViolationType = WedgeIsGeofenceViolation(GFNC);
            if (ViolationType != 0)
            {
                GfncViolationCount[i]++;
                if (GFNC_CONSECUTIVE_VIOLATION_TIMES_MAX <= GfncViolationCount[i])
                {
                    EVENT_ALERT_FLOW_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Geofence Alrt, Index: %d"
                                    , FmtTimeShow(), i + 1);
                    WedgeResponseUdpBinary(WEDGEPYLD_STATUS, (WEDGEEVIDTypeDef)(Geofence1_entered + i + (10 * (ViolationType - 1))));
                    GfncViolationCount[i] = 0;
                }
            }
            else
            {
                GfncViolationCount[i] = 0;
            }
        }
    }
}

void WedgeLocationOfDisabledVehicleOnToOff(void)
{
    EVENT_ALERT_FLOW_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Location of Disabled Vehicle, ontooff"
                                    , FmtTimeShow());
    WedgeResponseUdpBinary(WEDGEPYLD_STATUS, Location_of_Disabled_Vehicle);
}

void WedgeLocationOfDisabledVehicle(void)
{
    uint8_t StarterDisableCmdRec = 0;

    StarterDisableCmdRec = *((uint8_t *)WedgeSysStateGet(WEDGE_STARTER_DISABLE_CMD_RECEIVED));

    if (0 != StarterDisableCmdRec)
    {
        EVENT_ALERT_FLOW_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Location of Disabled Vehicle, starter command"
                                    , FmtTimeShow());
        WedgeResponseUdpBinary(WEDGEPYLD_STATUS, Location_of_Disabled_Vehicle);

        StarterDisableCmdRec = 0;

        WedgeSysStateSet(WEDGE_STARTER_DISABLE_CMD_RECEIVED, &StarterDisableCmdRec);
    }
}

void WedgeStopReportOnToOff(void)
{
    STPINTVLTypeDef STPINTVL = {0};
    RTCTimerListCellTypeDef Instance;

    STPINTVL = *((STPINTVLTypeDef *)WedgeCfgGet(WEDGE_CFG_STPINTVL));

    if (STPINTVL.interval == 0)
    {
        return;
    }

    Instance.RTCTimerType = WEDGE_RTC_TIMER_ONETIME;
    Instance.RTCTimerInstance = Stop_Report_Onetime_Event;
    Instance.settime = WedgeRtcCurrentSeconds() + 60 * STPINTVL.interval;
    if (0 != WedgeRtcTimerInstanceAdd(Instance))
    {
        EVENT_ALERT_FLOW_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Stop Report Add Timer err"
                                    , FmtTimeShow());
    }
    else
    {
        EVENT_ALERT_FLOW_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Stop Report Add Timer ok"
                                    , FmtTimeShow());
    }
}

extern TIMER WedgeOSPDTimer;

static void CheckWedgeOSPDTimerCallback(uint8_t status)
{
    uint8_t OverSpeedTimerStart = FALSE;

    if (status != 0)
    {
        EVENT_ALERT_FLOW_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Over Speed Alert"
                                    , FmtTimeShow());
        WedgeResponseUdpBinary(WEDGEPYLD_STATUS, Over_Speed_Threshold_Detect);
    }
    
    WedgeSysStateSet(WEDGE_OVER_SPEED_ALERT_TIMER_START, &OverSpeedTimerStart);
}

void WedgeOverSpeedAlert(void)
{
    OSPDTypeDef OSPD;
    double speedkm = 0.0;
    uint8_t OverSpeedTimerStart = FALSE;
    static uint32_t SystickRec = 0;

    // if (UbloxFixStateGet() == FALSE)
    // {
    //     return;
    // }

    if ((CHECK_UBLOX_STAT_TIMEOUT + 100) < (HAL_GetTick() - SystickRec))
    {
        return;
    }
    else
    {
        SystickRec = HAL_GetTick();
    }

    OSPD = *((OSPDTypeDef *)WedgeCfgGet(WEDGE_CFG_OSPD));

    if ((OSPD.speed == 0) || (OSPD.debounce == 0))
    {
        return;
    }

    speedkm = UbloxSpeedKM();
    OverSpeedTimerStart = *((uint8_t *)WedgeSysStateGet(WEDGE_OVER_SPEED_ALERT_TIMER_START));

    if (speedkm >= (OSPD.speed * MILE_TO_KM_FACTOR))
    {
        if (OverSpeedTimerStart == FALSE)
        {
            SoftwareTimerCreate(&WedgeOSPDTimer, 1, CheckWedgeOSPDTimerCallback, TimeMsec(OSPD.debounce));
            SoftwareTimerStart(&WedgeOSPDTimer);

            OverSpeedTimerStart = TRUE;
            WedgeSysStateSet(WEDGE_OVER_SPEED_ALERT_TIMER_START, &OverSpeedTimerStart);
        }
        else
        {
            if (WedgeOSPDTimer.TimeId == 0)
            {
                SoftwareTimerCreate(&WedgeOSPDTimer, 1, CheckWedgeOSPDTimerCallback, TimeMsec(OSPD.debounce));
                SoftwareTimerStart(&WedgeOSPDTimer);
            }
            else
            {
                // Check Wedge OSPD Timer
                IsSoftwareTimeOut(&WedgeOSPDTimer);
            }
        }
    }
    else
    {
        if (OverSpeedTimerStart == TRUE)
        {
            if (WedgeOSPDTimer.TimeId != 0)
            {
                SoftwareTimerCreate(&WedgeOSPDTimer, 0, CheckWedgeOSPDTimerCallback, TimeMsec(OSPD.debounce));
                SoftwareTimerStart(&WedgeOSPDTimer);
            }
            else
            {
                // Check Wedge OSPD Timer
                IsSoftwareTimeOut(&WedgeOSPDTimer);
            }
        }
    }
}

#define WEDGE_IGNITION_OFFTOON_DBNC_MS (250)
extern TIMER WedgeOffToOnTimer;

static void CheckWedgeOffToOnTimerCallback(uint8_t status)
{

}

void WedgeIgnitionOffToOnCheck(void)
{
    uint8_t WedgeIgnitionChangeDetected = FALSE;
    uint8_t WedgeIgnOffToOnTimerStart = FALSE;
    IGNTYPETypeDef IGNTYPE;
    static uint16_t ValidMeasureCount = 0;

    IGNTYPE = *((IGNTYPETypeDef *)WedgeCfgGet(WEDGE_CFG_IGNTYPE));
    if (0 == IGNTYPE.dbnc)
    {
        return;
    }

    WedgeIgnOffToOnTimerStart = *((uint8_t *)WedgeSysStateGet(WEDGE_IGN_OFFTOON_TIMER_START));
    WedgeIgnitionChangeDetected = *((uint8_t *)WedgeSysStateGet(WEDGE_IGNITION_CHANGE_DETECTED));
    if (WedgeIgnitionChangeDetected == FALSE)
    {
        if (WedgeIgnOffToOnTimerStart == FALSE)
        {
            return;
        }
    }
    else
    {
        WedgeIgnitionChangeDetected = FALSE;
        WedgeSysStateSet(WEDGE_IGNITION_CHANGE_DETECTED, &WedgeIgnitionChangeDetected);

        if (WedgeIgnOffToOnTimerStart == FALSE)
        {
            SoftwareTimerCreate(&WedgeOffToOnTimer, 1, CheckWedgeOffToOnTimerCallback, WEDGE_IGNITION_OFFTOON_DBNC_MS);
            SoftwareTimerStart(&WedgeOffToOnTimer);

            WedgeIgnOffToOnTimerStart = TRUE;
            WedgeSysStateSet(WEDGE_IGN_OFFTOON_TIMER_START, &WedgeIgnOffToOnTimerStart);
        }
    }

    if (FALSE != IsSoftwareTimeOut(&WedgeOffToOnTimer))
    {
        if (GPIO_PIN_RESET == READ_IO(PC10_MCU_IGN_GPIO_Port, PC10_MCU_IGN_Pin))
        {
            ValidMeasureCount = 0;

            SoftwareTimerReset(&WedgeOffToOnTimer, CheckWedgeOffToOnTimerCallback, WEDGE_IGNITION_OFFTOON_DBNC_MS);
            SoftwareTimerStop(&WedgeOffToOnTimer);

            WedgeIgnOffToOnTimerStart = FALSE;
            WedgeSysStateSet(WEDGE_IGN_OFFTOON_TIMER_START, &WedgeIgnOffToOnTimerStart);
        }
        else
        {
            ValidMeasureCount++;
            if (ValidMeasureCount < (TimeMsec(IGNTYPE.dbnc) / WEDGE_IGNITION_OFFTOON_DBNC_MS))
            {
                SoftwareTimerReset(&WedgeOffToOnTimer, CheckWedgeOffToOnTimerCallback, WEDGE_IGNITION_OFFTOON_DBNC_MS);
                SoftwareTimerStart(&WedgeOffToOnTimer);
            }
            else
            {
                WEDGEIgnitionStateTypeDef WEDGEIgnitionState = WEDGE_IGN_OFF_TO_ON_STATE;

                ValidMeasureCount = 0;

                SoftwareTimerReset(&WedgeOffToOnTimer, CheckWedgeOffToOnTimerCallback, WEDGE_IGNITION_OFFTOON_DBNC_MS);
                SoftwareTimerStop(&WedgeOffToOnTimer);

                WedgeIgnOffToOnTimerStart = FALSE;
                WedgeSysStateSet(WEDGE_IGN_OFFTOON_TIMER_START, &WedgeIgnOffToOnTimerStart);

                WedgeSysStateSet(WEDGE_IGNITION_STATE, &WEDGEIgnitionState);

                EVENT_ALERT_FLOW_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Ignition ON Alert"
                                    , FmtTimeShow());
                WedgeResponseUdpBinary(WEDGEPYLD_STATUS, Ignition_ON);

                //Off To On State, do some reset action at Off State
            }
        }
    }
}

#define WEDGE_IGNITION_ONTOOFF_DBNC_MS (250)
extern TIMER WedgeOnToOffTimer;

static void CheckWedgeOnToOffTimerCallback(uint8_t status)
{

}

void WedgeIgnitionOnToOffCheck(void)
{
    uint8_t WedgeIgnitionChangeDetected = FALSE;
    uint8_t WedgeIgnOnToOffTimerStart = FALSE;
    IGNTYPETypeDef IGNTYPE;
    static uint16_t ValidMeasureCount = 0;

    IGNTYPE = *((IGNTYPETypeDef *)WedgeCfgGet(WEDGE_CFG_IGNTYPE));
    if (0 == IGNTYPE.dbnc)
    {
        return;
    }

    WedgeIgnOnToOffTimerStart = *((uint8_t *)WedgeSysStateGet(WEDGE_IGN_ONTOOFF_TIMER_START));
    WedgeIgnitionChangeDetected = *((uint8_t *)WedgeSysStateGet(WEDGE_IGNITION_CHANGE_DETECTED));
    if (WedgeIgnitionChangeDetected == FALSE)
    {
        if (WedgeIgnOnToOffTimerStart == FALSE)
        {
            return;
        }
    }
    else
    {
        WedgeIgnitionChangeDetected = FALSE;
        WedgeSysStateSet(WEDGE_IGNITION_CHANGE_DETECTED, &WedgeIgnitionChangeDetected);

        if (WedgeIgnOnToOffTimerStart == FALSE)
        {
            SoftwareTimerCreate(&WedgeOnToOffTimer, 1, CheckWedgeOnToOffTimerCallback, WEDGE_IGNITION_ONTOOFF_DBNC_MS);
            SoftwareTimerStart(&WedgeOnToOffTimer);

            WedgeIgnOnToOffTimerStart = TRUE;
            WedgeSysStateSet(WEDGE_IGN_ONTOOFF_TIMER_START, &WedgeIgnOnToOffTimerStart);
        }
    }

    if (FALSE != IsSoftwareTimeOut(&WedgeOnToOffTimer))
    {
        if (GPIO_PIN_RESET != READ_IO(PC10_MCU_IGN_GPIO_Port, PC10_MCU_IGN_Pin))
        {
            ValidMeasureCount = 0;

            SoftwareTimerReset(&WedgeOnToOffTimer, CheckWedgeOnToOffTimerCallback, WEDGE_IGNITION_ONTOOFF_DBNC_MS);
            SoftwareTimerStop(&WedgeOnToOffTimer);

            WedgeIgnOnToOffTimerStart = FALSE;
            WedgeSysStateSet(WEDGE_IGN_ONTOOFF_TIMER_START, &WedgeIgnOnToOffTimerStart);
        }
        else
        {
            ValidMeasureCount++;
            if (ValidMeasureCount < (TimeMsec(IGNTYPE.dbnc) / WEDGE_IGNITION_ONTOOFF_DBNC_MS))
            {
                SoftwareTimerReset(&WedgeOnToOffTimer, CheckWedgeOnToOffTimerCallback, WEDGE_IGNITION_ONTOOFF_DBNC_MS);
                SoftwareTimerStart(&WedgeOnToOffTimer);
            }
            else
            {
                WEDGEIgnitionStateTypeDef WEDGEIgnitionState = WEDGE_IGN_ON_TO_OFF_STATE;

                ValidMeasureCount = 0;

                SoftwareTimerReset(&WedgeOnToOffTimer, CheckWedgeOnToOffTimerCallback, WEDGE_IGNITION_ONTOOFF_DBNC_MS);
                SoftwareTimerStop(&WedgeOnToOffTimer);

                WedgeIgnOnToOffTimerStart = FALSE;
                WedgeSysStateSet(WEDGE_IGN_ONTOOFF_TIMER_START, &WedgeIgnOnToOffTimerStart);

                WedgeSysStateSet(WEDGE_IGNITION_STATE, &WEDGEIgnitionState);

                EVENT_ALERT_FLOW_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Ignition OFF Alert"
                                    , FmtTimeShow());
                WedgeResponseUdpBinary(WEDGEPYLD_STATUS, Ignition_OFF);

                //Off To On State, do some reset action at Off State
            }
        }
    }
}

void WedgeHeadingChangeDetect(void)
{
    DIRCHGTypeDef DIRCHG = {0};
    double HeadingLastReportDeg = 0.0, HeadingTmp = 0.0;
    static uint32_t SystickRec = 0;

    // if (UbloxFixStateGet() == FALSE)
    // {
    //     return;
    // }

    if ((CHECK_UBLOX_STAT_TIMEOUT + 100) < (HAL_GetTick() - SystickRec))
    {
        return;
    }
    else
    {
        SystickRec = HAL_GetTick();
    }

    DIRCHG = *((DIRCHGTypeDef *)WedgeCfgGet(WEDGE_CFG_DIRCHG));
    if (DIRCHG.deg == 0)
    {
        return;
    }

    HeadingLastReportDeg = *((float *)WedgeSysStateGet(WEDGE_HEADING_LASTREPORT_DEG));
    HeadingTmp = (UbloxGetHeading() > HeadingLastReportDeg) ? 
                (UbloxGetHeading() - HeadingLastReportDeg): (HeadingLastReportDeg - UbloxGetHeading());
    if ((float)HeadingTmp < DIRCHG.deg)
    {
        return;
    }

    EVENT_ALERT_FLOW_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Heading Change Detect"
                                    , FmtTimeShow());
    WedgeResponseUdpBinary(WEDGEPYLD_STATUS, Heading_Change_Detect);
}

void WedgePeriodicMovingEventInit(void)
{
    RPTINTVLTypeDef RPTINTVL = {0};
    RTCTimerListCellTypeDef Instance;

    RPTINTVL = *((RPTINTVLTypeDef *)WedgeCfgGet(WEDGE_CFG_RPTINTVL));

    if (RPTINTVL.perint == 0)
    {
        return;
    }

    Instance.RTCTimerType = WEDGE_RTC_TIMER_PERIODIC;
    Instance.RTCTimerInstance = Periodic_Moving_Event;
    Instance.settime = WedgeRtcCurrentSeconds() + 60 * RPTINTVL.perint;
    if (0 != WedgeRtcTimerInstanceAdd(Instance))
    {
        EVENT_ALERT_FLOW_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Periodic Moving Add Timer err"
                                    , FmtTimeShow());
    }
    else
    {
        EVENT_ALERT_FLOW_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Periodic Moving Add Timer ok"
                                    , FmtTimeShow());
    }
}

void WedgePeriodicOffEvent(void)
{
    uint8_t PeriodicOffEventTimerStart = FALSE;
    RPTINTVLTypeDef RPTINTVL = {0};
    RTCTimerListCellTypeDef Instance;

    PeriodicOffEventTimerStart = *((uint8_t *)WedgeSysStateGet(WEDGE_PERIODIC_OFF_EVENT_TIMER_START));
    if (PeriodicOffEventTimerStart == TRUE)
    {
        return;
    }

    RPTINTVL = *((RPTINTVLTypeDef *)WedgeCfgGet(WEDGE_CFG_RPTINTVL));
    if (RPTINTVL.ioffint == 0)
    {
        return;
    }

    Instance.RTCTimerType = WEDGE_RTC_TIMER_PERIODIC;
    Instance.RTCTimerInstance = Periodic_OFF_Event;
    Instance.settime = WedgeRtcCurrentSeconds() + 60 * RPTINTVL.ioffint;
    if (0 != WedgeRtcTimerInstanceAdd(Instance))
    {
        EVENT_ALERT_FLOW_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Periodic Off Add Timer err"
                                    , FmtTimeShow());
        PeriodicOffEventTimerStart = FALSE;
        WedgeSysStateSet(WEDGE_PERIODIC_OFF_EVENT_TIMER_START, &PeriodicOffEventTimerStart);
    }
    else
    {
        EVENT_ALERT_FLOW_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Periodic Off Add Timer ok"
                                    , FmtTimeShow());
        PeriodicOffEventTimerStart = TRUE;
        WedgeSysStateSet(WEDGE_PERIODIC_OFF_EVENT_TIMER_START, &PeriodicOffEventTimerStart);
    }
}

void WedgePeriodicHealthEvent(void)
{
    uint8_t PeriodicHealthEventStart = FALSE;
    RPTINTVLTypeDef RPTINTVL = {0};
    RTCTimerListCellTypeDef Instance;

    PeriodicHealthEventStart = *((uint8_t *)WedgeSysStateGet(WEDGE_PERIODIC_HEALTEH_EVENT_TIMER_START));
    if (PeriodicHealthEventStart == TRUE)
    {
        return;
    }

    RPTINTVL = *((RPTINTVLTypeDef *)WedgeCfgGet(WEDGE_CFG_RPTINTVL));
    if (RPTINTVL.perint == 0)
    {
        return;
    }

    Instance.RTCTimerType = WEDGE_RTC_TIMER_PERIODIC;
    Instance.RTCTimerInstance = Periodic_Health_Event;
    Instance.settime = WedgeRtcCurrentSeconds() + 60 * RPTINTVL.perint;
    if (0 != WedgeRtcTimerInstanceAdd(Instance))
    {
        EVENT_ALERT_FLOW_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Periodic Health Add Timer err"
                                    , FmtTimeShow());
        PeriodicHealthEventStart = FALSE;
        WedgeSysStateSet(WEDGE_PERIODIC_HEALTEH_EVENT_TIMER_START, &PeriodicHealthEventStart);
    }
    else
    {
        EVENT_ALERT_FLOW_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Periodic Health Add Timer ok"
                                    , FmtTimeShow());
        PeriodicHealthEventStart = TRUE;
        WedgeSysStateSet(WEDGE_PERIODIC_HEALTEH_EVENT_TIMER_START, &PeriodicHealthEventStart);
    }
}

void WedgePeriodicHardwareResetInit(void)
{
    HWRSTTypeDef HWRST = {0};
    RTCTimerListCellTypeDef Instance;
    uint8_t PeriodicHardwareResetTimerStart = FALSE;

    HWRST = *((HWRSTTypeDef *)WedgeCfgGet(WEDGE_CFG_HWRST));
    if (HWRST.interval == 0)
    {
        return;
    }

    Instance.RTCTimerType = WEDGE_RTC_TIMER_ONETIME;
    Instance.RTCTimerInstance = Periodic_Hardware_Reset_Onetime;
    Instance.settime = WedgeRtcCurrentSeconds() + 60 * HWRST.interval;
    if (0 != WedgeRtcTimerInstanceAdd(Instance))
    {
        EVENT_ALERT_FLOW_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Periodic HWRST Add Timer err"
                                    , FmtTimeShow());
        PeriodicHardwareResetTimerStart = FALSE;
        WedgeSysStateSet(WEDGE_PERIODIC_HARDWARE_RESET_TIMER_START, &PeriodicHardwareResetTimerStart);
    }
    else
    {
        EVENT_ALERT_FLOW_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Periodic HWRST Add Timer ok"
                                    , FmtTimeShow());
        PeriodicHardwareResetTimerStart = TRUE;
        WedgeSysStateSet(WEDGE_PERIODIC_HARDWARE_RESET_TIMER_START, &PeriodicHardwareResetTimerStart);
    }
}

static uint32_t WedgePowerLostAddr = 0;
static void WedgePowerLostAddrSet(uint32_t address)
{
    WedgePowerLostAddr = address;
}

static uint32_t WedgePowerLostAddrGet(void)
{
    return WedgePowerLostAddr;
}

static uint8_t WedgePowerLostRecordErase(void)
{
    int32_t k = 0;
    uint32_t address = 0;
    char *pon = "O";

    for (k = 0; k < (WEDGE_POWER_LOST_INDICATE_TOTAL_SIZE / WEDGE_STORAGE_SECTOR_SIZE); k++)
    {
        address = (WEDGE_POWER_LOST_INDICATE_START_ADDR + k * WEDGE_STORAGE_SECTOR_SIZE) & (~WEDGE_STORAGE_SECTOR_ALIGN_MASK);
        if (0 != WedgeFlashEraseSector(address))
        {

            return 1;
        }
    }

    if (0 != WedgeFlashWriteData(WEDGE_POWER_LOST_INDICATE_START_ADDR, (uint8_t *)pon, 1))
    {
        return 2;
    }

    WedgePowerLostAddrSet(WEDGE_POWER_LOST_INDICATE_START_ADDR + 1);

    return 0;
}

uint8_t WedgeSetPowerLostBeforeReset(void)
{
    uint32_t address = WedgePowerLostAddrGet();
    char *poff = "F";

    if (0 != WedgeFlashWriteData(address, (uint8_t *)poff, 1))
    {
        return 1;
    }

    return 0;
}

uint8_t WedgeIsStartFromPowerLost(void)
{
    #define WEDGE_IS_POWER_LOST_READ_BUF_SIZE_BYTES (256)
    uint32_t oncount = 0, offcount = 0, address = 0;
    int32_t i = 0, j = 0, loops = 0;
    uint8_t readbuf[WEDGE_IS_POWER_LOST_READ_BUF_SIZE_BYTES] = {0}, ret = 0, breakflag = 0;
    char *WedgeIsStartFromPowerLostErrStr = "WEDGE Is Power Lost", *pon = "O";

    loops = WEDGE_POWER_LOST_INDICATE_TOTAL_SIZE / WEDGE_IS_POWER_LOST_READ_BUF_SIZE_BYTES;
    for (i = 0; i < loops; i++)
    {
        memset(readbuf, 0, sizeof(readbuf));
        if (0 != WedgeFlashReadData(WEDGE_POWER_LOST_INDICATE_START_ADDR + (i * sizeof(readbuf)), (uint8_t *)readbuf, sizeof(readbuf)))
        {
            EVENT_ALERT_FLOW_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] %s Err2",
                                      FmtTimeShow(), WedgeIsStartFromPowerLostErrStr);
            return 2;
        }

        for (j = 0; j < sizeof(readbuf); j++)
        {
            if (readbuf[j] == WEDGE_POWER_LOST_INDICATE_ON)
            {
                oncount++;
            }
            else if (readbuf[j] == WEDGE_POWER_LOST_INDICATE_OFF)
            {
                offcount++;
            }
            else if (readbuf[j] == 0xFF)
            {
                breakflag = 1;
                break;
            }
            else
            {
                EVENT_ALERT_FLOW_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] %s Err3",
                                   FmtTimeShow(), WedgeIsStartFromPowerLostErrStr);

                if (0 != WedgePowerLostRecordErase())
                {
                    EVENT_ALERT_FLOW_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] %s Erase Err1",
                                   FmtTimeShow(), WedgeIsStartFromPowerLostErrStr);
                }

                return 3;
            }
        }

        address = WEDGE_POWER_LOST_INDICATE_START_ADDR + (i * sizeof(readbuf)) + j;
        if (breakflag == 1)
        {
            break;
        }
    }

    if (oncount != offcount)
    {
        ret = 1;
    }

    if (breakflag == 1)
    {
        if (ret == 0)
        {
            if (0 != WedgeFlashWriteData(address, (uint8_t *)pon, 1))
            {
                EVENT_ALERT_FLOW_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] %s Err4",
                                   FmtTimeShow(), WedgeIsStartFromPowerLostErrStr);

                return 4;
            }

            WedgePowerLostAddrSet(address + 1);
        }
        else
        {
            WedgePowerLostAddrSet(address);
        }
    }
    
    if ((breakflag == 0) || (address >= WEDGE_POWER_LOST_INDICATE_END_ADDR))
    {
        if (0 != WedgePowerLostRecordErase())
        {
            EVENT_ALERT_FLOW_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] %s Erase Err2",
                               FmtTimeShow(), WedgeIsStartFromPowerLostErrStr);
        }
    }

    return ret;
}

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2017. All rights reserved
                                End Of The File
*******************************************************************************/
