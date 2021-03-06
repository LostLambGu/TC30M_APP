/*******************************************************************************
* File Name          : wedgeeventalertflow.c
* Author             : Yangjie Gu
* Description        : This file provides all the wedgeeventalertflow functions.

* History:
*  12/14/2017 : wedgeeventalertflow V1.00
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "wedgeeventalertflow.h"
#include "wedgecommonapi.h"
#include "wedgertctimer.h"
#include "ublox_driver.h"
#include "iocontrol.h"

/* Defines -------------------------------------------------------------------*/
#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define WEDGE_EVENT_ALERT_LOG DebugLog
#define WEDGE_EVENT_ALERT_PRINT DebugPrintf

static WEDGESysStateTypeDef WEDGESysState;

uint8_t WedgeSysStateInit(WEDGESysStateTypeDef *pWEDGESysState)
{
    IGNTYPETypeDef IGNTYPE = *((IGNTYPETypeDef *)WedgeCfgGet(WEDGE_CFG_IGNTYPE));

    memset(&WEDGESysState, 0, sizeof(WEDGESysStateTypeDef));
    if (pWEDGESysState == NULL)
    {
        if (IGNTYPE.itype == No_Ignition_detect)
        {
            WEDGESysState.WEDGEIgnitionState = WEDGE_IGN_TO_IGNORE_STATE;
        }
        else if (IGNTYPE.itype == Virtual_Ignition_Battery_Voltage)
        {
            // Reserved
        }
        else if (IGNTYPE.itype == Virtual_Ignition_GPS_velocity)
        {
            // Reserved
        }
        else if (IGNTYPE.itype == Wired_Ignition)
        {
            if (GPIO_PIN_RESET == READ_IO(PC10_MCU_IGN_GPIO_Port, PC10_MCU_IGN_Pin))
            {
                WEDGE_EVENT_ALERT_LOG("WEDGE Sys State Init WEDGE_IGN_ON_TO_OFF_STATE");
                WEDGESysState.WEDGEIgnitionState = WEDGE_IGN_ON_TO_OFF_STATE;
            }
            else
            {
                WEDGE_EVENT_ALERT_LOG("WEDGE Sys State Init WEDGE_IGN_OFF_TO_ON_STATE");
                WEDGESysState.WEDGEIgnitionState = WEDGE_IGN_OFF_TO_ON_STATE;
            }
        }
        else
        {
            // Error
            WEDGE_EVENT_ALERT_LOG("WEDGE Sys State Init Err1");
            return 1;
        }

        WEDGESysState.GeofenceDefinedBitMap = WedgeGetGeofenceBitMap();
    }
    else
    {
        WEDGESysState = *pWEDGESysState;

        if (WEDGESysState.WEDGEIgnitionState == WEDGE_IGN_OFF_STATE)
        {
            if (GPIO_PIN_RESET != READ_IO(PC10_MCU_IGN_GPIO_Port, PC10_MCU_IGN_Pin))
            {
                WEDGESysState.WedgeIgnitionChangeDetected = TRUE;
            }
        }
        else if (WEDGESysState.WEDGEIgnitionState == WEDGE_IGN_ON_STATE)
        {
            if (GPIO_PIN_RESET == READ_IO(PC10_MCU_IGN_GPIO_Port, PC10_MCU_IGN_Pin))
            {
                WEDGESysState.WedgeIgnitionChangeDetected = TRUE;
            }
        }
        else if (WEDGESysState.WEDGEIgnitionState >= WEDGE_IGN_STATE_MAX)
        {
            WEDGE_EVENT_ALERT_LOG("WEDGE Sys State Init Err2");
            return 2;
        }

        WEDGESysState.WedgeIgnOffToOnTimerStart = 0;
        WEDGESysState.WedgeIgnOnToOffTimerStart = 0;
        WEDGESysState.GpsVelocity = 0.0;
        WEDGESysState.Longitude = 0.0;
        WEDGESysState.Latitude = 0.0;
        WEDGESysState.IDLEDtectTimerStart = 0;
        WEDGESysState.OverSpeedTimerStart = 0;
        WEDGESysState.PeriodicHealthEventTimerStart = 0;
    }

    if (WedgeIsPowerLostGet() == FALSE)
    {
        // TRUE means wedge is power on without power lost.
        WEDGESysState.PowerOnOff = TRUE;
    }

    return 0;
}

void WedgeIgnitionStateSet(WEDGEIgnitionStateTypeDef IgnitionState)
{
    WEDGESysState.WEDGEIgnitionState = IgnitionState;
}

WEDGEIgnitionStateTypeDef WedgeIgnitionStateGet(void)
{
    return WEDGESysState.WEDGEIgnitionState;
}

void WedgeSysStateGetTotal(uint8_t *pBuf, uint32_t *pSize)
{
    if ((pBuf == NULL) || (pSize == NULL))
    {
        return;
    }

    memcpy(pBuf, &WEDGESysState, sizeof(WEDGESysState));
    *pSize = sizeof(WEDGESysState);
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

    case WEDGE_STOP_REPORT_ONTIME_RTC_TIMER_ADDED:
        return &(WEDGESysState.StopReportOnetimeRtcTimerAdded);
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
        return &(WEDGESysState.PeriodicHealthEventTimerStart);
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
        WEDGE_EVENT_ALERT_LOG("WEDGE SYS Get: Param err");
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

    case WEDGE_STOP_REPORT_ONTIME_RTC_TIMER_ADDED:
        WEDGESysState.StopReportOnetimeRtcTimerAdded = *((uint8_t *)pvData);
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
        WEDGESysState.PeriodicHealthEventTimerStart = *((uint8_t *)pvData);
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
        WEDGE_EVENT_ALERT_LOG("WEDGE SYS Set: Param err");
        break;
    }

    return;
}

void WedgeServiceOdometerAlert(void)
{
    VODOTypeDef VODO = {0};
    SODOTypeDef SODO = {0};
    static uint32_t SystickRec = 0;

    // if (UbloxFixStateGet() == FALSE)
    // {
    //     return;
    // }

    if (WEDGE_GPS_DATA_PERIOD_MS > (HAL_GetTick() - SystickRec))
    {
        return;
    }
    else
    {
        SystickRec = HAL_GetTick();
    }

    VODO = *((VODOTypeDef *)WedgeCfgGet(WEDGE_CFG_VODO));

    if (WEDGESysState.SerOdoLastReportMileage > VODO.meters)
    {
        WEDGE_EVENT_ALERT_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Evt Alrt Flw SODO err"
                                , FmtTimeShow());
        WEDGESysState.SerOdoLastReportMileage = VODO.meters;
        return;
    }

    SODO = *((SODOTypeDef *)WedgeCfgGet(WEDGE_CFG_SODO));
    if ((VODO.meters - WEDGESysState.SerOdoLastReportMileage) > SODO.meters)
    {
        WEDGE_EVENT_ALERT_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Ser Odo Alrt -->(UdpBinary)"
                                , FmtTimeShow());
        WedgeResponseUdpBinary(WEDGEPYLD_STATUS, Service_Alert);
        WEDGESysState.SerOdoLastReportMileage = VODO.meters;
        return;
    }
}

extern double ADCGetVinVoltage(void);
void WedgeLowBatteryAlert(void)
{
#if TC30M_TEST_CONFIG_OFF
#define WEDGE_LOW_VOLTAGE_DETECT_PERIOD_MS (600 * 1000) /* 10 minutes */
#else
#define WEDGE_LOW_VOLTAGE_DETECT_PERIOD_MS (10 * 1000) /* Just for test */
#endif
    static uint32_t SystickRec = 0;

    if (WEDGE_LOW_VOLTAGE_DETECT_PERIOD_MS > (HAL_GetTick() - SystickRec))
    {
        return;
    }
    else
    {
        LVATypeDef LVA = {0};
        float voltage = 0.0;

        SystickRec = HAL_GetTick();

        LVA = *((LVATypeDef *)WedgeCfgGet(WEDGE_CFG_LVA));
        voltage = (float)ADCGetVinVoltage() / 1000;// MV To V

        WEDGE_EVENT_ALERT_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE LVA Alrt LVA.battlvl (%f) voltage(%f)"
                                , FmtTimeShow(), LVA.battlvl, voltage);

        if (voltage < LVA.battlvl)
        {
            WEDGE_EVENT_ALERT_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE LVA Alrt-->(UdpBinary)"
                                , FmtTimeShow());
            WedgeResponseUdpBinary(WEDGEPYLD_STATUS, Low_Battery_Alert);
            return;
        }
    }
}

#define WEDGE_IDLE_DETECT_SPPED_MILE (3)
#define MILE_TO_KM_FACTOR (1.6093)
extern TIMER WedgeIDLETimer;

static void CheckWedgeIDLETimerCallback(uint8_t status)
{
    WEDGE_EVENT_ALERT_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE IDLE(CallBack) Alrt-->(UdpBinary)"
                                , FmtTimeShow());
    WedgeResponseUdpBinary(WEDGEPYLD_STATUS, IDLE_Detect);

    WEDGESysState.IDLEDtectTimerStart = FALSE;
}

void WedgeIDLEDetectAlertReset(void)
{
    WEDGESysState.IDLEDtectTimerStart = FALSE;

    SoftwareTimerStop(&WedgeIDLETimer);
}

void WedgeIDLEDetectAlert(void)
{
    IDLETypeDef IDLE = {0};
    double speedkm = 0.0;
    static uint32_t SystickRec = 0;

    if (WEDGE_GPS_DATA_PERIOD_MS > (HAL_GetTick() - SystickRec))
    {
        return;
    }
    else
    {
        SystickRec = HAL_GetTick();
    }

    IDLE = *((IDLETypeDef *)WedgeCfgGet(WEDGE_CFG_IDLE));

    WEDGE_EVENT_ALERT_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE IDLE Alrt IDLE.duration(%d)"
                                , FmtTimeShow(), IDLE.duration);
    if (IDLE.duration == 0)
    {
        return;
    }

    speedkm = UbloxSpeedKM();
    if (UbloxFixStateGet())
    {
        if (speedkm <= (WEDGE_IDLE_DETECT_SPPED_MILE * MILE_TO_KM_FACTOR))
        {
            if (WEDGESysState.IDLEDtectTimerStart == FALSE)
            {
                SoftwareTimerCreate(&WedgeIDLETimer, 1, CheckWedgeIDLETimerCallback, TimeMsec(IDLE.duration));
                SoftwareTimerStart(&WedgeIDLETimer);

                WEDGESysState.IDLEDtectTimerStart = TRUE;
            }
            else
            {
                // Check Wedge IDLE Detect Timer
                IsSoftwareTimeOut(&WedgeIDLETimer);
            }
        }
        else
        {
            if (WEDGESysState.IDLEDtectTimerStart == TRUE)
            {
                SoftwareTimerReset(&WedgeIDLETimer, CheckWedgeIDLETimerCallback, TimeMsec(IDLE.duration));
                SoftwareTimerStop(&WedgeIDLETimer);

                WEDGESysState.IDLEDtectTimerStart = FALSE;
            }
        }
    }
    else
    {
        WEDGE_EVENT_ALERT_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE IDLE Alrt Gps No Fix"
                                , FmtTimeShow());
        // if (WEDGESysState.IDLEDtectTimerStart == FALSE)
        // {
        //    SoftwareTimerCreate(&WedgeIDLETimer, 1, CheckWedgeIDLETimerCallback, TimeMsec(IDLE.duration));
        //    SoftwareTimerStart(&WedgeIDLETimer);

        //    WEDGESysState.IDLEDtectTimerStart = TRUE;
        // }
        // else
        // {
        //     // Check Wedge IDLE Detect Timer
        //     IsSoftwareTimeOut(&WedgeIDLETimer);
        // }
    }
}

#define WEDGE_OUT_TOW_GEOFENCE_CONSECUTIVE_MAX_TIMES (10)
#define WEDGE_OUT_TOW_ALERT_GEOFENCE (1)
static uint8_t WedgeIsOutsideTowGeoFnc(TowAlertGeoFenceTypedef TowAlertGeoFence, uint16_t radius)
{
    if (UbloxFixStateGet() == FALSE)
    {
        WEDGE_EVENT_ALERT_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Tow Alrt Gps not Fix"
                                    , FmtTimeShow());
        return 0;
    }
    else
    {
        double Latitude1 = 0.0;
        double Longitude1 = 0.0;
        float Distance = 0.0;

        UBloxGetGpsPoint(&Latitude1, &Longitude1);
        Distance = UBloxGpsPointDistance((double long)Latitude1, (double long)Longitude1, TowAlertGeoFence.Latitude, TowAlertGeoFence.Longitude);
        if (Distance > radius)
        {
            // Yes
            return WEDGE_OUT_TOW_ALERT_GEOFENCE;
        }
        else
        {
            return 0;
        }
    }
}

void WedgeTowAlertReset(void)
{
    WEDGESysState.TowAlertOnceAlready = FALSE;
    WEDGESysState.OutTowGeoFncCount = 0;
}

void WedgeTowAlert(void)
{
    TOWTypeDef TOW;
    TowAlertGeoFenceTypedef TowAlertGeoFence;
    static uint32_t SystickRec = 0;

    // if (UbloxFixStateGet() == FALSE)
    // {
    //     return;
    // }

    if (WEDGE_GPS_DATA_PERIOD_MS > (HAL_GetTick() - SystickRec))
    {
        return;
    }
    else
    {
        SystickRec = HAL_GetTick();
    }

    if (TRUE == WEDGESysState.TowAlertOnceAlready)
    {
        return;
    }

    TOW = *((TOWTypeDef *)WedgeCfgGet(WEDGE_CFG_TOW));

    if (TOW.enable == 0)
    {
        return;
    }

    TowAlertGeoFence = WEDGESysState.TowAlertGeoFence;
    if (WedgeIsOutsideTowGeoFnc(TowAlertGeoFence, TOW.radius) == WEDGE_OUT_TOW_ALERT_GEOFENCE)
    {
        WEDGESysState.OutTowGeoFncCount++;

        if (WEDGE_OUT_TOW_GEOFENCE_CONSECUTIVE_MAX_TIMES <= WEDGESysState.OutTowGeoFncCount)
        {
            WEDGE_EVENT_ALERT_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Tow Alrt-->(UdpBinary)"
                                    , FmtTimeShow());
            WedgeResponseUdpBinary(WEDGEPYLD_STATUS, Tow_Alert_Exited);

            WEDGESysState.TowAlertOnceAlready = TRUE;
        }
    }
    else
    {
        WEDGESysState.OutTowGeoFncCount = 0;
    }
}

#define WEDGE_EXIT_ALERT_GEOFENCE (1)
#define WEDGE_ENTER_ALERT_GEOFENCE (2)
static uint8_t WedgeIsGeofenceViolation(GFNCTypeDef *pGFNC)
{
    if (UbloxFixStateGet() == FALSE)
    {
        WEDGE_EVENT_ALERT_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Gfnc Violation Gps not Fix"
                                    , FmtTimeShow());
        return 0;
    }
    else
    {
        double Latitude1 = 0.0;
        double Longitude1 = 0.0;
        float Distance = 0.0;
        float radius = (float)(pGFNC->radius);
        uint8_t dirtmp = 0;

        // Enter 2, Exit 1, Enter and Exit 0
        if (pGFNC->dir > 2)
        {
            WEDGE_EVENT_ALERT_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Gfnc Violtation Param Err", FmtTimeShow());
            return 0;
        }

        UBloxGetGpsPoint(&Latitude1, &Longitude1);
        Distance = UBloxGpsPointDistance((double long)Latitude1, (double long)Longitude1
            , (double long)pGFNC->lat_ctr, (double long)pGFNC->lon_ctr);
        if (Distance > radius)
        {
            // Yes
            dirtmp = WEDGE_EXIT_ALERT_GEOFENCE;
        }
        else
        {
            dirtmp = WEDGE_ENTER_ALERT_GEOFENCE;
        }

        if (pGFNC->dir == 0)
        {
            return dirtmp;
        }
        else
        {
            if (((0x01 & dirtmp) ^ (0x01 & pGFNC->dir)) & 0x01)
            {
                return 0;
            }
            else
            {
                return dirtmp;
            }
        }
    }
}

void WedgeGeofenceAlert(void)
{
    static uint32_t SystickRec = 0;
    uint8_t i = 0;
    GFNCTypeDef GFNC = {0};
    static uint8_t GfncViolationCount[WEDGE_GEOFENCES_NUM_MAX] = {0};
    uint8_t ViolationType = 0;

    // if (UbloxFixStateGet() == FALSE)
    // {
    //     return;
    // }

    if (0 == WEDGESysState.GeofenceDefinedBitMap)
    {
        return;
    }

    if (WEDGE_GPS_DATA_PERIOD_MS > (HAL_GetTick() - SystickRec))
    {
        return;
    }
    else
    {
        SystickRec = HAL_GetTick();
    }

    for (i = 0; i < WEDGE_GEOFENCES_NUM_MAX; i++)
    {
        if ((WEDGESysState.GeofenceDefinedBitMap >> i) & 0x01)
        {
            GFNC = *((GFNCTypeDef *)WedgeCfgGet((WEDGECfgOperateTypeDef)(WEDGE_CFG_GFNC1 + i)));
            ViolationType = WedgeIsGeofenceViolation(&GFNC);
            if (ViolationType != 0)
            {
                GfncViolationCount[i]++;
                if (GFNC_CONSECUTIVE_VIOLATION_TIMES_MAX <= GfncViolationCount[i])
                {
                    WEDGE_EVENT_ALERT_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Geofence Alrt, Index: %d-->(UdpBinary)"
                                    , FmtTimeShow(), i + 1);
                    WedgeResponseUdpBinary(WEDGEPYLD_STATUS, (WEDGEEVIDTypeDef)(Geofence1_entered + i + (10 * (ViolationType % 2))));
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
    WEDGE_EVENT_ALERT_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Location of Disabled Vehicle Ontooff-->(UdpBinary)"
                                    , FmtTimeShow());
    WedgeResponseUdpBinary(WEDGEPYLD_STATUS, Location_of_Disabled_Vehicle);
}

void WedgeLocationOfDisabledVehicleReset(void)
{
    WEDGESysState.StarterDisableCmdRec = FALSE;
}

void WedgeLocationOfDisabledVehicle(void)
{
    if (FALSE != WEDGESysState.StarterDisableCmdRec)
    {
        WEDGE_EVENT_ALERT_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Location of Disabled Vehicle Starter Command-->(UdpBinary)"
                                    , FmtTimeShow());
        WedgeResponseUdpBinary(WEDGEPYLD_STATUS, Location_of_Disabled_Vehicle);

        WEDGESysState.StarterDisableCmdRec = FALSE;
    }
}

void WedgeStopReportOnToOffDisable(void)
{
    uint8_t ret = 0;

    WEDGESysState.StopReportOnetimeRtcTimerAdded = FALSE;

    ret = WedgeRtcTimerInstanceDel(Stop_Report_Onetime_Event);

    WEDGE_EVENT_ALERT_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Stop Report Reset Ret%d"
                                    , FmtTimeShow(), ret);
}

void WedgeStopReportOnToOff(void)
{
    STPINTVLTypeDef STPINTVL = {0};
    RTCTimerListCellTypeDef Instance;
    char *WedgeStopReportOnToOffStr = " WEDGE Stop Report Add Timer ";

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
        WEDGE_EVENT_ALERT_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s]%sErr"
                                    , FmtTimeShow(), WedgeStopReportOnToOffStr);
    }
    else
    {
        WEDGE_EVENT_ALERT_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s]%sOk"
                                    , FmtTimeShow(), WedgeStopReportOnToOffStr);
        WEDGESysState.StopReportOnetimeRtcTimerAdded = TRUE;
    }
}

extern TIMER WedgeOSPDTimer;

static void CheckWedgeOSPDTimerCallback(uint8_t status)
{
    if (status != 0)
    {
        WEDGE_EVENT_ALERT_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Over Speed Alert-->(UdpBinary)"
                                    , FmtTimeShow());
        WedgeResponseUdpBinary(WEDGEPYLD_STATUS, Over_Speed_Threshold_Detect);
    }
    
    WEDGESysState.OverSpeedTimerStart = FALSE;
}

void WedgeOverSpeedAlertReset(void)
{
    WEDGE_EVENT_ALERT_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Over Speed Alert Reset"
                                    , FmtTimeShow());
    WEDGESysState.OverSpeedTimerStart = FALSE;
    SoftwareTimerStop(&WedgeOSPDTimer);
}

void WedgeOverSpeedAlert(void)
{
    OSPDTypeDef OSPD;
    double speedkm = 0.0;
    static uint32_t SystickRec = 0;

    if (WEDGE_GPS_DATA_PERIOD_MS > (HAL_GetTick() - SystickRec))
    {
        return;
    }
    else
    {
        SystickRec = HAL_GetTick();
    }

    if (UbloxFixStateGet() == FALSE)
    {
        WEDGE_EVENT_ALERT_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Over Speed Gps Not Fix"
                                    , FmtTimeShow());
        return;
    }

    OSPD = *((OSPDTypeDef *)WedgeCfgGet(WEDGE_CFG_OSPD));

    if ((OSPD.speed == 0) || (OSPD.debounce == 0))
    {
        return;
    }

    speedkm = UbloxSpeedKM();

    if (speedkm >= ((double)(OSPD.speed * MILE_TO_KM_FACTOR)))
    {
        if (WEDGESysState.OverSpeedTimerStart == FALSE)
        {
            // 1 Over Speed Timer; 0 Normal Speed Timer
            SoftwareTimerCreate(&WedgeOSPDTimer, 1, CheckWedgeOSPDTimerCallback, TimeMsec(OSPD.debounce));
            SoftwareTimerStart(&WedgeOSPDTimer);

            WEDGESysState.OverSpeedTimerStart = TRUE;
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
        if (WEDGESysState.OverSpeedTimerStart == TRUE)
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
static uint16_t WedgeValidMeasureCountOffToOn = 0;

static void CheckWedgeOffToOnTimerCallback(uint8_t status)
{
}

void WedgeIgnitionOffToOnCheckReset(void)
{
    WEDGESysState.WedgeIgnitionChangeDetected = FALSE;
    WEDGESysState.WedgeIgnOffToOnTimerStart = FALSE;

    SoftwareTimerReset(&WedgeOffToOnTimer, CheckWedgeOffToOnTimerCallback, WEDGE_IGNITION_OFFTOON_DBNC_MS);
    SoftwareTimerStop(&WedgeOffToOnTimer);

    WedgeValidMeasureCountOffToOn = 0;
}

void WedgeIgnitionOffToOnCheck(void)
{
    IGNTYPETypeDef IGNTYPE;

    IGNTYPE = *((IGNTYPETypeDef *)WedgeCfgGet(WEDGE_CFG_IGNTYPE));

    if (WEDGESysState.WedgeIgnitionChangeDetected == FALSE)
    {
        if (WEDGESysState.WedgeIgnOffToOnTimerStart == FALSE)
        {
            return;
        }
    }
    else
    {
        // If the dbnc is changed to 0, while the timer is running, the setting is not working!
        if (0 == IGNTYPE.dbnc)
        {
            if (GPIO_PIN_RESET != READ_IO(PC10_MCU_IGN_GPIO_Port, PC10_MCU_IGN_Pin))
            {
                WEDGESysState.WEDGEIgnitionState = WEDGE_IGN_OFF_TO_ON_STATE;
            }
            return;
        }

        WEDGESysState.WedgeIgnitionChangeDetected = FALSE;

        if (WEDGESysState.WedgeIgnOffToOnTimerStart == FALSE)
        {
            SoftwareTimerCreate(&WedgeOffToOnTimer, 1, CheckWedgeOffToOnTimerCallback, WEDGE_IGNITION_OFFTOON_DBNC_MS);
            SoftwareTimerStart(&WedgeOffToOnTimer);

            WEDGESysState.WedgeIgnOffToOnTimerStart = TRUE;
        }
    }

    if (FALSE != IsSoftwareTimeOut(&WedgeOffToOnTimer))
    {
        if (GPIO_PIN_RESET == READ_IO(PC10_MCU_IGN_GPIO_Port, PC10_MCU_IGN_Pin))
        {
            WedgeValidMeasureCountOffToOn = 0;

            SoftwareTimerReset(&WedgeOffToOnTimer, CheckWedgeOffToOnTimerCallback, WEDGE_IGNITION_OFFTOON_DBNC_MS);
            SoftwareTimerStop(&WedgeOffToOnTimer);

            WEDGESysState.WedgeIgnOffToOnTimerStart = FALSE;
        }
        else
        {
            WedgeValidMeasureCountOffToOn++;
            if (WedgeValidMeasureCountOffToOn < (TimeMsec(IGNTYPE.dbnc) / WEDGE_IGNITION_OFFTOON_DBNC_MS))
            {
                SoftwareTimerReset(&WedgeOffToOnTimer, CheckWedgeOffToOnTimerCallback, WEDGE_IGNITION_OFFTOON_DBNC_MS);
                SoftwareTimerStart(&WedgeOffToOnTimer);
            }
            else
            {
                WedgeValidMeasureCountOffToOn = 0;

                SoftwareTimerReset(&WedgeOffToOnTimer, CheckWedgeOffToOnTimerCallback, WEDGE_IGNITION_OFFTOON_DBNC_MS);
                SoftwareTimerStop(&WedgeOffToOnTimer);

                WEDGESysState.WedgeIgnOffToOnTimerStart = FALSE;
                WEDGESysState.WEDGEIgnitionState = WEDGE_IGN_OFF_TO_ON_STATE;

                WEDGE_EVENT_ALERT_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Ignition ON Alert-->(UdpBinary)"
                                    , FmtTimeShow());
                WedgeResponseUdpBinary(WEDGEPYLD_STATUS, Ignition_ON);

                //Off To On State, do some reset action at Off State
            }
        }
    }
}

#define WEDGE_IGNITION_ONTOOFF_DBNC_MS (250)
extern TIMER WedgeOnToOffTimer;
static uint16_t WedgeValidMeasureCountOnToOff = 0;

static void CheckWedgeOnToOffTimerCallback(uint8_t status)
{
}

void WedgeIgnitionOnToOffCheckReset(void)
{
    WEDGESysState.WedgeIgnitionChangeDetected = FALSE;
    WEDGESysState.WedgeIgnOnToOffTimerStart = FALSE;

    SoftwareTimerReset(&WedgeOnToOffTimer, CheckWedgeOnToOffTimerCallback, WEDGE_IGNITION_ONTOOFF_DBNC_MS);
    SoftwareTimerStop(&WedgeOnToOffTimer);

    WedgeValidMeasureCountOnToOff = 0;
}

void WedgeIgnitionOnToOffCheck(void)
{
    IGNTYPETypeDef IGNTYPE;

    IGNTYPE = *((IGNTYPETypeDef *)WedgeCfgGet(WEDGE_CFG_IGNTYPE));

    if (WEDGESysState.WedgeIgnitionChangeDetected == FALSE)
    {
        if (WEDGESysState.WedgeIgnOnToOffTimerStart == FALSE)
        {
            return;
        }
    }
    else
    {
        if (0 == IGNTYPE.dbnc)
        {
            if (GPIO_PIN_RESET == READ_IO(PC10_MCU_IGN_GPIO_Port, PC10_MCU_IGN_Pin))
            {
                WEDGESysState.WEDGEIgnitionState = WEDGE_IGN_ON_TO_OFF_STATE;
            }
            return;
        }

        WEDGESysState.WedgeIgnitionChangeDetected = FALSE;

        if (WEDGESysState.WedgeIgnOnToOffTimerStart == FALSE)
        {
            SoftwareTimerCreate(&WedgeOnToOffTimer, 1, CheckWedgeOnToOffTimerCallback, WEDGE_IGNITION_ONTOOFF_DBNC_MS);
            SoftwareTimerStart(&WedgeOnToOffTimer);

            WEDGESysState.WedgeIgnOnToOffTimerStart = TRUE;
        }
    }

    if (FALSE != IsSoftwareTimeOut(&WedgeOnToOffTimer))
    {
        if (GPIO_PIN_RESET != READ_IO(PC10_MCU_IGN_GPIO_Port, PC10_MCU_IGN_Pin))
        {
            WedgeValidMeasureCountOnToOff = 0;

            SoftwareTimerReset(&WedgeOnToOffTimer, CheckWedgeOnToOffTimerCallback, WEDGE_IGNITION_ONTOOFF_DBNC_MS);
            SoftwareTimerStop(&WedgeOnToOffTimer);

            WEDGESysState.WedgeIgnOnToOffTimerStart = FALSE;
        }
        else
        {
            WedgeValidMeasureCountOnToOff++;
            if (WedgeValidMeasureCountOnToOff < (TimeMsec(IGNTYPE.dbnc) / WEDGE_IGNITION_ONTOOFF_DBNC_MS))
            {
                SoftwareTimerReset(&WedgeOnToOffTimer, CheckWedgeOnToOffTimerCallback, WEDGE_IGNITION_ONTOOFF_DBNC_MS);
                SoftwareTimerStart(&WedgeOnToOffTimer);
            }
            else
            {
                WedgeValidMeasureCountOnToOff = 0;

                SoftwareTimerReset(&WedgeOnToOffTimer, CheckWedgeOnToOffTimerCallback, WEDGE_IGNITION_ONTOOFF_DBNC_MS);
                SoftwareTimerStop(&WedgeOnToOffTimer);

                WEDGESysState.WedgeIgnOnToOffTimerStart = FALSE;
                WEDGESysState.WEDGEIgnitionState = WEDGE_IGN_ON_TO_OFF_STATE;

                WEDGE_EVENT_ALERT_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Ignition OFF Alert-->(UdpBinary)"
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
    double HeadingTmp = 0.0;
    static uint32_t SystickRec = 0;
    char * WedgeHeadingChangeDetectStr= " WEDGE Heading Change Detect ";

    if (WEDGE_GPS_DATA_PERIOD_MS > (HAL_GetTick() - SystickRec))
    {
        return;
    }
    else
    {
        SystickRec = HAL_GetTick();
    }

    if (UbloxFixStateGet() == FALSE)
    {
        WEDGE_EVENT_ALERT_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s]%sGps Not Fix"
                                    , FmtTimeShow(), WedgeHeadingChangeDetectStr);
        return;
    }

    DIRCHG = *((DIRCHGTypeDef *)WedgeCfgGet(WEDGE_CFG_DIRCHG));
    if (DIRCHG.deg == 0)
    {
        return;
    }

    HeadingTmp = (UbloxGetHeading() > WEDGESysState.HeadingLastReportDeg) ? 
                (UbloxGetHeading() - WEDGESysState.HeadingLastReportDeg): (WEDGESysState.HeadingLastReportDeg - UbloxGetHeading());
    if ((float)HeadingTmp < DIRCHG.deg)
    {
        return;
    }

    WEDGE_EVENT_ALERT_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s]%sOk-->(UdpBinary)"
                                    , FmtTimeShow(), WedgeHeadingChangeDetectStr);
    WedgeResponseUdpBinary(WEDGEPYLD_STATUS, Heading_Change_Detect);
}

void WedgePeriodicMovingEventInit(void)
{
    RPTINTVLTypeDef RPTINTVL = {0};
    RTCTimerListCellTypeDef Instance;
    char * WedgePeriodicMovingEventInitStr= " WEDGE Periodic Moving Add Timer ";

    RPTINTVL = *((RPTINTVLTypeDef *)WedgeCfgGet(WEDGE_CFG_RPTINTVL));

    if (RPTINTVL.perint == 0)
    {
        return;
    }

    Instance.RTCTimerType = WEDGE_RTC_TIMER_PERIODIC;
    Instance.RTCTimerInstance = Periodic_Moving_Event;
    Instance.settime = WedgeRtcCurrentSeconds() + WEDGE_MINUTE_TO_SECOND_FACTOR * RPTINTVL.perint;
    if (0 != WedgeRtcTimerInstanceAdd(Instance))
    {
        WEDGE_EVENT_ALERT_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s]%sErr"
                                    , FmtTimeShow(), WedgePeriodicMovingEventInitStr);
    }
    else
    {
        WEDGE_EVENT_ALERT_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s]%sOk"
                                    , FmtTimeShow(), WedgePeriodicMovingEventInitStr);
    }
}

void WedgePeriodicOffEventReset(void)
{
    uint8_t ret = 0;
    WEDGESysState.PeriodicOffEventTimerStart = FALSE;

    ret = WedgeRtcTimerInstanceDel(Periodic_OFF_Event);

    WEDGE_EVENT_ALERT_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Period Off Event Reset Ret%d"
                                    , FmtTimeShow(), ret);
}

void WedgePeriodicOffEvent(void)
{
    RPTINTVLTypeDef RPTINTVL = {0};
    RTCTimerListCellTypeDef Instance;
    char *WedgePeriodicOffEventErr = " WEDGE Periodic Off Add Timer ";

    if (WEDGESysState.PeriodicOffEventTimerStart == TRUE)
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
    Instance.settime = WedgeRtcCurrentSeconds() + WEDGE_MINUTE_TO_SECOND_FACTOR * RPTINTVL.ioffint;
    if (0 != WedgeRtcTimerInstanceAdd(Instance))
    {
        WEDGE_EVENT_ALERT_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s]%sErr"
                                    , FmtTimeShow(), WedgePeriodicOffEventErr);
        WEDGESysState.PeriodicOffEventTimerStart = FALSE;
    }
    else
    {
        WEDGE_EVENT_ALERT_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s]%sOk"
                                    , FmtTimeShow(), WedgePeriodicOffEventErr);
        WEDGESysState.PeriodicOffEventTimerStart = TRUE;
    }
}

void WedgePeriodicHealthEventReset(void)
{
    uint8_t ret = 0;

    WEDGESysState.PeriodicHealthEventTimerStart = FALSE;

    ret = WedgeRtcTimerInstanceDel(Periodic_Health_Event);

    WEDGE_EVENT_ALERT_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Period Health Event Reset Ret%d"
                                    , FmtTimeShow(), ret);
}

void WedgePeriodicHealthEvent(void)
{
    RPTINTVLTypeDef RPTINTVL = {0};
    RTCTimerListCellTypeDef Instance;
    char * WedgePeriodicHealthEventStr= " WEDGE Periodic Health Add Timer ";
    
    if (WEDGESysState.PeriodicHealthEventTimerStart == TRUE)
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
    Instance.settime = WedgeRtcCurrentSeconds() + WEDGE_MINUTE_TO_SECOND_FACTOR * RPTINTVL.perint;
    if (0 != WedgeRtcTimerInstanceAdd(Instance))
    {
        WEDGE_EVENT_ALERT_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s]%sErr"
                                    , FmtTimeShow(), WedgePeriodicHealthEventStr);
        WEDGESysState.PeriodicHealthEventTimerStart = FALSE;
    }
    else
    {
        WEDGE_EVENT_ALERT_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s]%sOk"
                                    , FmtTimeShow(), WedgePeriodicHealthEventStr);
        WEDGESysState.PeriodicHealthEventTimerStart = TRUE;
    }
}

void WedgePeriodicHardwareResetReinit(void)
{
    WEDGE_EVENT_ALERT_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Period Hardware Reset Reinit"
                                    , FmtTimeShow());
    WEDGESysState.PeriodicHardwareResetTimerStart = FALSE;
    WedgeRtcTimerInstanceDel(Periodic_Hardware_Reset_Onetime);
    WedgePeriodicHardwareResetInit();
}

void WedgePeriodicHardwareResetInit(void)
{
    HWRSTTypeDef HWRST = {0};
    RTCTimerListCellTypeDef Instance;

    HWRST = *((HWRSTTypeDef *)WedgeCfgGet(WEDGE_CFG_HWRST));
    if (HWRST.interval == 0)
    {
        return;
    }

    Instance.RTCTimerType = WEDGE_RTC_TIMER_ONETIME;
    Instance.RTCTimerInstance = Periodic_Hardware_Reset_Onetime;
    Instance.settime = WedgeRtcCurrentSeconds() + WEDGE_MINUTE_TO_SECOND_FACTOR * HWRST.interval;
    if (0 != WedgeRtcTimerInstanceAdd(Instance))
    {
        WEDGE_EVENT_ALERT_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Periodic HWRST Add Timer err"
                                    , FmtTimeShow());
        WEDGESysState.PeriodicHardwareResetTimerStart = FALSE;
    }
    else
    {
        WEDGE_EVENT_ALERT_PRINT(DbgCtl.WedgeEvtAlrtFlwInfoEn, "\r\n[%s] WEDGE Periodic HWRST Add Timer ok"
                                    , FmtTimeShow());
        WEDGESysState.PeriodicHardwareResetTimerStart = TRUE;
    }
}

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2018. All rights reserved
                                End Of The File
*******************************************************************************/
