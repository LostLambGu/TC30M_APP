/*******************************************************************************
* File Name          : wedgeapplication.c
* Author             : Yangjie Gu
* Description        : This file provides all the wedgeapplication functions.

* History:
*  12/12/2017 : wedgeapplication V1.00
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "wedgeapplication.h"

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
#include "deepsleep.h"

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define APP_LOG DebugLog
#define APP_PRINT DebugPrintf

static void WedgeInit(void);
static void WedgeIgnitionStateProcess(void);
static void WedgeIGNOnStateReset(void);
static void WedgeIGNOffStateReset(void);
static void WedgeMsgQueProcess(void);
static void WedgePowerModeProcess(void);
static void WedgePowerModeInit(void);

static void WedgeCfgChgStateProcess(void);
static void WedgeSMSAddrCfgChg(void);
static void WedgeSVRCFGCfgChg(WEDGECfgChangeTypeDef CfgChg);
static void WedgeAPNCfgChg(void);
static void WedgeHWRRSTCfgChg(void);
static void WedgePWRMGTCfgChg(void);
static void WedgeUSRDATCfgChg(void);
static void WedgeCFGALLCfgChg(void);
static void WedgeResetDefaultCfgChg(void);
static void WedgeIGNTYPCfgChg(void);
static void WedgeRPTINTVLCfgChg(void);
static void WedgeLVACfgChg(void);
static void WedgeIDLECfgChg(void);
static void WedgeSODOCfgChg(void);
static void WedgeDIRCHGCfgChg(void);
static void WedgeTOWCfgChg(void);
static void WedgeSTPINTVLCfgChg(void);
static void WedgeVODOCfgChg(void);
static void WedgeGEOFENCESCfgChg(WEDGECfgChangeTypeDef CfgChg);
static void WedgeRELAYCfgChg(void);
static void WedgePLSRLYCfgChg(void);
static void WedgeOSPDCfgChg(void);
static void WedgeGPSDIAGCfgChg(void);

#define TC30M_LTE_PROCESS_PERIOD_MS (10)
void ApplicationProcess(void)
{
    static uint32_t LteProcSystickRec = 0;

    DebugPrintf(TRUE, "\r\n########Code Date: 20180403########\r\n");

    WedgeInit();

    while (1)
    {
        // Debug Info
        ATCmdDetection();
        // LTE Info
        if (TC30M_LTE_PROCESS_PERIOD_MS < (HAL_GetTick() - LteProcSystickRec))
        {
            LteCmdDetection();
            LteProcSystickRec = HAL_GetTick();
        }
        // Soft Timer
        SoftwareCheckTimerStatus();
        // // Gsensor Interupt
        GsensorIntProcess();

        // Wedge Service Odometer Alert
        WedgeServiceOdometerAlert();
        // Wedge Low Battery Alert
        WedgeLowBatteryAlert();
        // Wedge Geofence Alert
        WedgeGeofenceAlert();
        // Wedge Over Speed Alert
        WedgeOverSpeedAlert();
        // Wedge Heading Change Detect
        WedgeHeadingChangeDetect();
        // Wedge Event and Alert Process
        WedgeIgnitionStateProcess();
        // Wedge Configuration Change Process
        WedgeCfgChgStateProcess();
        // Wedge Msg Que Process
        WedgeMsgQueProcess();
        // Wedge Socket Manage Process
        WedgeUdpSocketManageProcess();
        // Wedge Rtc Timer Event Process
        WedgeRTCTimerEventProcess();
        // Wedge GPS data Request Process
        WedgeGpsRequestDataProcess();
        // Wedge Power Mode Process
        WedgePowerModeProcess();
    }
}

static void WedgeIgnitionStateProcess(void)
{
    IGNTYPETypeDef IGNTYPE;

    memset(&IGNTYPE, 0, sizeof(IGNTYPE));
    IGNTYPE = *((IGNTYPETypeDef *)WedgeCfgGet(WEDGE_CFG_IGNTYPE));
    if (IGNTYPE.itype > Wired_Ignition)
    {
        APP_LOG("WEDGE Ign Stat IgnType Err");
    }

    switch (*((WEDGEIgnitionStateTypeDef *)WedgeSysStateGet(WEDGE_IGNITION_STATE)))
    {
    case WEDGE_IGN_TO_IGNORE_STATE:
    {
        WedgeIGNOffStateReset();
        WedgeIGNOnStateReset();
        WedgeStopReportOnToOffDisable();

        WedgeIgnitionStateSet(WEDGE_IGN_IGNORE_STATE);
    }
    break;

    case WEDGE_IGN_IGNORE_STATE:
    {
        #if TC30M_TEST_CONFIG_OFF
        #define WEDGE_IGNITION_IGNORE_PROCESS_PERIOD (20000)
        #else
        #define WEDGE_IGNITION_IGNORE_PROCESS_PERIOD (5000)
        #endif /* TC30M_TEST_CONFIG_OFF */
        static uint32_t WedgeIgniIgnorLastMs = 0;
        if (WEDGE_IGNITION_IGNORE_PROCESS_PERIOD > (HAL_GetTick() - WedgeIgniIgnorLastMs))
        {
            return;
        }
        else
        {
            WedgeIgniIgnorLastMs = HAL_GetTick();
        }
        APP_LOG("WEDGE Ign Stat IGN IGNORE_STATE Period(%d)", WEDGE_IGNITION_IGNORE_PROCESS_PERIOD);
    }
    break;

    case WEDGE_IGN_OFF_STATE:
    {
        if (IGNTYPE.itype == Wired_Ignition)
        {
            WedgeTowAlert();

            WedgeLocationOfDisabledVehicle();
        }

        // if (IGNTYPE.itype != No_Ignition_detect)
        // {
            WedgeIgnitionOffToOnCheck();
        // }

        WedgePeriodicOffEvent();

        WedgePeriodicHealthEvent();
    }
    break;

    case WEDGE_IGN_OFF_TO_ON_STATE:
    {
        APP_LOG("WEDGE Ign Stat IGN OFF_TO_ON");

        WedgeIGNOffStateReset();

        WedgeStopReportOnToOffDisable();

        WedgeIgnitionStateSet(WEDGE_IGN_ON_STATE);
    }
    break;

    case WEDGE_IGN_ON_STATE:
    {
        if (IGNTYPE.itype == Wired_Ignition)
        {
            WedgeIDLEDetectAlert();
        }

        // if (IGNTYPE.itype != No_Ignition_detect)
        // {
            WedgeIgnitionOnToOffCheck();
        // }
    }
    break;

    case WEDGE_IGN_ON_TO_OFF_STATE:
    {
        APP_LOG("WEDGE Ign Stat IGN ON_TO_OFF");

        if (IGNTYPE.itype == Wired_Ignition)
        {
            WedgeLocationOfDisabledVehicleOnToOff();
        }

        // When Ignition on, WEDGESysState.StopReportOnetimeRtcTimerAdded need to be reset!
        WedgeStopReportOnToOff();

        WedgeIGNOnStateReset();

        WedgeIgnitionStateSet(WEDGE_IGN_OFF_STATE);
    }
    break;

    default:
        APP_LOG("WEDGE Ign Stat Err");
        break;
    }

    return;
}

static void WedgeIGNOnStateReset(void)
{
    WedgeIDLEDetectAlertReset();
    WedgeIgnitionOnToOffCheckReset();
}

static void WedgeIGNOffStateReset(void)
{
    WedgeTowAlertReset();
    WedgeLocationOfDisabledVehicleReset();
    WedgeIgnitionOffToOnCheckReset();
    WedgePeriodicOffEventReset();
    WedgePeriodicHealthEventReset();
}

static void WedgeInit(void)
{
    uint8_t ret = FALSE;
    WEDGEDeviceInfoTypeDef WEDGEDeviceInfo;
    char *WedgeInitStr = " WEDGE Init ";

    WedgeResponseUdpBinaryInit();
    WedgeResponseUdpAsciiInit();

    ret = WedgeIsStartFromPowerLost();
    if (ret != FALSE)
    {
        APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s]%sFrom Power Lost",
                                      FmtTimeShow(), WedgeInitStr);
        WedgeIsPowerLostSet(TRUE);
    }
    else
    {
        WedgeIsPowerLostSet(FALSE);
        APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s]%s",
                                      FmtTimeShow(), WedgeInitStr);
    }

    memset(&WEDGEDeviceInfo, 0, sizeof(WEDGEDeviceInfo));
    if (0 == WedgeDeviceInfoRead((uint8_t *)&WEDGEDeviceInfo, sizeof(WEDGEDeviceInfo)))
    {
        // Normal
        APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s]%sDevice Info Read",
                                      FmtTimeShow(), WedgeInitStr);

        WedgeCfgInit(&(WEDGEDeviceInfo.WEDGECfg));

        WedgeSysStateInit(&(WEDGEDeviceInfo.WEDGESysState));

        WedgeRtcTimerInit(&(WEDGEDeviceInfo.RTCTimerList));
    }
    else
    {
        // Abnormal
        APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s]%sDevice Info Not Read",
                                      FmtTimeShow(), WedgeInitStr);

        WedgeCfgInit(NULL);

        WedgeSysStateInit(NULL);

        WedgeRtcTimerInit(NULL);
    }

    WedgeCfgChgStateResetAll();

    if (WedgeIsPowerLostGet() == TRUE)
    {
        WedgeMsgQueInit();
    }

    WedgePeriodicHardwareResetInit();

    WedgePeriodicMovingEventInit();

    WedgePowerModeInit();
}

static void WedgeMsgQueProcess(void)
{
    NetworkStatT networkstat = GetNetworkMachineStatus();
    static uint32_t SystickRec = 0, WedgeMsgUnsent = FALSE;
    static WEDGEMsgQueCellTypeDef WEDGEMsgQueCell;
    #define WEDGE_MSG_QUE_PROCESS_PERIOD_MS (20000)
    #define WEDGE_MSG_QUE_PROCESS_PERIOD_MS_NET_ON (500)
    static uint32_t wedgeMsgQueProcPeriod = WEDGE_MSG_QUE_PROCESS_PERIOD_MS_NET_ON;

    if (networkstat != NET_CONNECTED_STAT)
    {
        if (WEDGE_MSG_QUE_PROCESS_PERIOD_MS > (HAL_GetTick() - SystickRec))
        {
            return;
        }
        else
        {
            SystickRec = HAL_GetTick();
        }

        APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s] WEDGE MsgQueProcess Net Disconnect Period(%d)", FmtTimeShow(), WEDGE_MSG_QUE_PROCESS_PERIOD_MS);
        return;
    }
    else
    {
        if (wedgeMsgQueProcPeriod > (HAL_GetTick() - SystickRec))
        {
            return;
        }
        else
        {
            SystickRec = HAL_GetTick();
        }

        //APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s] WEDGE MsgQueProcess(Period %d) Net Connect", FmtTimeShow(), WEDGE_MSG_QUE_PROCESS_PERIOD_MS);
        if (WedgeMsgUnsent == FALSE)
        {
            memset(&WEDGEMsgQueCell, 0, sizeof(WEDGEMsgQueCell));
            if (0 == WedgeMsgQueOutRead(&WEDGEMsgQueCell))
            {
                APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s] WEDGE MsgQueProcess Readout", FmtTimeShow());
                WedgeMsgUnsent = TRUE;
                wedgeMsgQueProcPeriod = WEDGE_MSG_QUE_PROCESS_PERIOD_MS_NET_ON;
            }
            else
            {
                // APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s] WEDGE MsgQueProcess Readout Err", FmtTimeShow());
                WedgeMsgUnsent = FALSE;
                wedgeMsgQueProcPeriod = WEDGE_MSG_QUE_PROCESS_PERIOD_MS;
            }
        }
        
        if (WedgeMsgUnsent == TRUE)
        {
            if (WEDGEMsgQueCell.type == WEDGE_MSG_QUE_UDP_TYPE)
            {
                if (WedgeMsgProcessResponseUdp(WEDGEMsgQueCell.data, WEDGEMsgQueCell.size) > 0)
                {
                    APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s] WEDGE MsgQueProcess Udp Fail Size(%d)", FmtTimeShow(), WEDGEMsgQueCell.size);
                    WedgeMsgUnsent = TRUE;
                    wedgeMsgQueProcPeriod = WEDGE_MSG_QUE_PROCESS_PERIOD_MS;
                }
                else
                {
                    APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s] WEDGE MsgQueProcess Udp Ok", FmtTimeShow());
                    WedgeMsgUnsent = FALSE;
                    wedgeMsgQueProcPeriod = WEDGE_MSG_QUE_PROCESS_PERIOD_MS_NET_ON;
                }
            }
            else if (WEDGEMsgQueCell.type == WEDGE_MSG_QUE_SMS_TYPE)
            {
                if (WedgeMsgProcessResponseSms(WEDGEMsgQueCell.data, WEDGEMsgQueCell.size) > 0)
                {
                    APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s] WEDGE MsgQueProcess Sms Fail", FmtTimeShow());
                    WedgeMsgUnsent = TRUE;
                    wedgeMsgQueProcPeriod = WEDGE_MSG_QUE_PROCESS_PERIOD_MS;
                }
                else
                {
                    APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s] WEDGE MsgQueProcess Sms Ok", FmtTimeShow());
                    WedgeMsgUnsent = FALSE;
                    wedgeMsgQueProcPeriod = WEDGE_MSG_QUE_PROCESS_PERIOD_MS_NET_ON;
                }
            }
            else
            {
                APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s] WEDGE MsgQueProcess MsgQueCell Type Err", FmtTimeShow());
                WedgeMsgUnsent = FALSE;
            }
        }
    }
}

static uint8_t isNotFirstEnterPowerMode = FALSE;
static void WedgeUpdateTickValue(uint8_t *pIsNotFirstEnter, uint32_t *pSysTick)
{
    if (*pIsNotFirstEnter == FALSE)
    {
        *pSysTick = HAL_GetTick();
        *pIsNotFirstEnter = TRUE;
    }
}

static void WedgeUpdateRtcTimeValue(uint8_t IsNotFirstEnter, uint32_t *pRtcTime)
{
    if (IsNotFirstEnter == FALSE)
    {
        *pRtcTime = WedgeRtcCurrentSeconds();
    }
}

static uint8_t WedgeIsSystickOutTime(uint32_t SysTickVal, uint32_t timeInterval)
{
    if ((HAL_GetTick() - SysTickVal) > timeInterval)
    {
        return TRUE;
    }
    
    return FALSE;
}

static uint8_t WedgeIsRtcTimeSleepOutTime(uint32_t RtcTime, uint32_t timeInterval)
{
    uint32_t RtcIntervalTmp = 0;

    RtcIntervalTmp = (WedgeRtcCurrentSeconds() - RtcTime);
    if (RtcIntervalTmp < timeInterval)
    {
        APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s] WEDGE IsRtcTimeSleepOutTime(%d)", FmtTimeShow(), timeInterval - RtcIntervalTmp);
        MCUDeepSleep(timeInterval - RtcIntervalTmp);
    }

    RtcIntervalTmp = (WedgeRtcCurrentSeconds() - RtcTime);

    if (RtcIntervalTmp >= timeInterval)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static void WedgePowerModeInit(void)
{
    PWRMGTTypeDef *pPWRMGT = (PWRMGTTypeDef *)(WedgeCfgGet(WEDGE_CFG_PWRMGT));

    if ((pPWRMGT->mode == TC30M_POWER_MODE_VIBRATION_DETECT2) || (pPWRMGT->mode == TC30M_POWER_MODE_VIBRATION_DETECT3) 
    || (pPWRMGT->mode == TC30M_POWER_MODE_VIBRATION_DETECT4))
    {
        GSensorI2cInit();
        APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s] WEDGE PowerModeInit Gsensor Start", FmtTimeShow());
    }
    else
    {
        APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s] WEDGE PowerModeInit", FmtTimeShow());
    }
}

static void WedgePowerModeProcess(void)
{
    static uint32_t SysTickVal = 0;
    PWRMGTTypeDef *pPWRMGT = (PWRMGTTypeDef *)(WedgeCfgGet(WEDGE_CFG_PWRMGT));
    #define TC30M_POWER_MODE_ON (0)
    #define TC30M_POWER_MODE_OFF (1)
    static __IO uint8_t onOffState = TC30M_POWER_MODE_ON;
    static uint32_t RtcTime = 0;

    #define WEDGE_POWER_MODE_PROCESS_PERIOD_MS (1000)
    static uint32_t SystickRec = 0;
    if (WEDGE_POWER_MODE_PROCESS_PERIOD_MS > (HAL_GetTick() - SystickRec))
    {
        return;
    }
    else
    {
        SystickRec = HAL_GetTick();
    }



    WedgeUpdateRtcTimeValue(isNotFirstEnterPowerMode, &RtcTime);
    WedgeUpdateTickValue(&isNotFirstEnterPowerMode, &SysTickVal);

    switch (pPWRMGT->mode)
    {
        case TC30M_POWER_MODE_ALWAYS_RUN:
        {
            APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s] WEDGE PowerModeProcess ALWAYS_RUN", FmtTimeShow());
            return;
        }
        // break;

        case TC30M_POWER_MODE_TIMER_METHOD:
        {
            if (onOffState == TC30M_POWER_MODE_ON)
            {
                APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s] WEDGE PowerModeProcess TIMER_METHOD MODE_ON", FmtTimeShow());
                if (TRUE == WedgeIsSystickOutTime(SysTickVal, pPWRMGT->ontime * WEDGE_SECOND_TO_MS_FACTOR))
                {
                    isNotFirstEnterPowerMode = FALSE;
                    onOffState = TC30M_POWER_MODE_OFF;
                }
                else
                {
                    return;
                }
            }
            else
            {
                APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s] WEDGE PowerModeProcess TIMER_METHOD MODE_OFF", FmtTimeShow());
                if (FALSE == WedgeIsRtcTimeSleepOutTime(RtcTime, pPWRMGT->dbcslp.sleeptime))
                {
                    return;
                }
                else
                {
                    isNotFirstEnterPowerMode = FALSE;
                    onOffState = TC30M_POWER_MODE_ON;
                }
                APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s] WEDGE PowerModeProcess TIMER_METHOD MODE_OFF WAKE", FmtTimeShow());
            }
        }
        break;

        case TC30M_POWER_MODE_VIBRATION_DETECT2:
        {

        }
        break;

        case TC30M_POWER_MODE_VIBRATION_DETECT3:
        {

        }
        break;

        case TC30M_POWER_MODE_VIBRATION_DETECT4:
        {

        }
        break;
    }
}

static void WedgeCfgChgStateProcess(void)
{
    uint8_t i = 0;
    char *WedgeCfgChgStateProcessStr = " WEDGE Cfg Chg State Process ";

    if (WedgeCfgChgStateIsChanged() == 0)
    {
        return;
    }

    for (i = 0; i < CFG_CHG_INVALIAD_MAX; i++)
    {
        if (WedgeCfgChgStateGet((WEDGECfgChangeTypeDef)i) != FALSE)
        {
            switch ((WEDGECfgChangeTypeDef)i)
            {
                case SMS_ADDR_CFG_CHG:
                    WedgeSMSAddrCfgChg();
                    break;

                case SVRCFG_CFG_CHG_ALL:
                case SVRCFG_CFG_CHG_1:
                case SVRCFG_CFG_CHG_2:
                case SVRCFG_CFG_CHG_3:
                case SVRCFG_CFG_CHG_4:
                case SVRCFG_CFG_CHG_5:
                    WedgeSVRCFGCfgChg((WEDGECfgChangeTypeDef)i);
                    break;

                case FTPCFG_CFG_CHG:
                    APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s]%sFTPCFG Cfg Chg Reserved",
                                      FmtTimeShow(), WedgeCfgChgStateProcessStr);
                    break;

                case APNCFG_CFG_CHG:
                    WedgeAPNCfgChg();
                    break;

                case HWRST_CFG_CHG:
                    WedgeHWRRSTCfgChg();
                    break;

                case PWRMGT_CFG_CHG:
                    WedgePWRMGTCfgChg();
                    break;

                case USRDAT_CFG_CHG:
                    WedgeUSRDATCfgChg();
                    break;

                case CFGALL_CFG_CHG:
                    WedgeCFGALLCfgChg();
                    break;

                case RESET_DEFAULT_CFG_CHG:
                    WedgeResetDefaultCfgChg();
                    break;

                case IGNTYP_CFG_CHG:
                    WedgeIGNTYPCfgChg();
                    break;

                case RPTINTVL_CFG_CHG:
                    WedgeRPTINTVLCfgChg();
                    break;

                case ALARM1_CFG_CHG:
                case ALARM2_CFG_CHG:
                    APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s]%sAlarm Cfg Chg Reserved",
                                      FmtTimeShow(), WedgeCfgChgStateProcessStr);
                    break;

                case LVA_CFG_CHG:
                    WedgeLVACfgChg();
                    break;

                case IDLE_CFG_CHG:
                    WedgeIDLECfgChg();
                    break;

                case SODO_CFG_CHG:
                    WedgeSODOCfgChg();
                    break;

                case DIRCHG_CFG_CHG:
                    WedgeDIRCHGCfgChg();
                    break;

                case TOW_CFG_CHG:
                    WedgeTOWCfgChg();
                    break;

                case STPINTVL_CFG_CHG:
                    WedgeSTPINTVLCfgChg();
                    break;

                case VODO_CFG_CHG:
                    WedgeVODOCfgChg();
                    break;

                case GEOFENCES1_CFG_CHG:
                case GEOFENCES2_CFG_CHG:
                case GEOFENCES3_CFG_CHG:
                case GEOFENCES4_CFG_CHG:
                case GEOFENCES5_CFG_CHG:
                case GEOFENCES6_CFG_CHG:
                case GEOFENCES7_CFG_CHG:
                case GEOFENCES8_CFG_CHG:
                case GEOFENCES9_CFG_CHG:
                case GEOFENCES10_CFG_CHG:
                    WedgeGEOFENCESCfgChg((WEDGECfgChangeTypeDef)i);
                    break;

                case RELAY_CFG_CHG:
                    WedgeRELAYCfgChg();
                    break;

                case PLSRLY_CFG_CHG:
                    WedgePLSRLYCfgChg();
                    break;

                case OPSOD_CFG_CHG:
                    WedgeOSPDCfgChg();
                    break;

                case GPSDIAG_CFG_CHG:
                    WedgeGPSDIAGCfgChg();
                    break;

                default:
                    APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s]%sDefault",
                                      FmtTimeShow(), WedgeCfgChgStateProcessStr);
                    break;
            }
        }
    }

    WedgeCfgChgStateResetAll();
    // Wedge Device Info Save
    WedgeDeviceInfoSave();
}

void WedgeDeviceInfoSave(void)
{
    WEDGEDeviceInfoTypeDef WEDGEDeviceInfo;
    uint32_t size = 0;
    uint8_t ret = 0;
    char *WedgeDeviceInfoSaveStr = " WEDGE Device Info Save ";
    memset(&WEDGEDeviceInfo, 0, sizeof(WEDGEDeviceInfo));

    WedgeCfgGetTotal((uint8_t *)&WEDGEDeviceInfo.WEDGECfg, &size);
    WedgeSysStateGetTotal((uint8_t *)&WEDGEDeviceInfo.WEDGESysState, &size);
    WedgeRTCTimerListGet((uint8_t *)&WEDGEDeviceInfo.RTCTimerList, &size);

    ret = WedgeDeviceInfoWrite((uint8_t *)&WEDGEDeviceInfo, sizeof(WEDGEDeviceInfo));
    if (0 != ret)
    {
        APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s]%sErr%d",
                    FmtTimeShow(), WedgeDeviceInfoSaveStr, ret);
    }
    else
    {
        APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s]%sOk",
                    FmtTimeShow(), WedgeDeviceInfoSaveStr);
    }
}

static void WedgeSMSAddrCfgChg(void)
{




    APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s] WEDGE SMS Addr Cfg Chg Reserved", FmtTimeShow());
}

static void WedgeSVRCFGCfgChg(WEDGECfgChangeTypeDef CfgChg)
{



    APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s] WEDGE SVRCFG Cfg Chg Reserved", FmtTimeShow());
}

static void WedgeAPNCfgChg(void)
{
    #define DEFAULT_PDP_TYPE (0)
    APNCFGTypeDef *pAPNCFG = ((APNCFGTypeDef *)WedgeCfgGet(WEDGE_CFG_APNCFG));
    extern void SetModemApn(char *pApn, uint8_t pdpType);
    SetModemApn((char *)pAPNCFG->apn, DEFAULT_PDP_TYPE);
    APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s] WEDGE Cfg Chg APN(%s)", FmtTimeShow(), pAPNCFG->apn);
}

static void WedgeHWRRSTCfgChg(void)
{
    APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s] WEDGE HWRRST Cfg Chg", FmtTimeShow());
    WedgePeriodicHardwareResetReinit();
}

static void WedgePWRMGTCfgChg(void)
{
    isNotFirstEnterPowerMode = FALSE;
    APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s] WEDGE PWRMGT Cfg Chg", FmtTimeShow());
}

static void WedgeUSRDATCfgChg(void)
{




    APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s] WEDGE USRDAT Cfg Chg Reserved", FmtTimeShow());
}

static void WedgeCFGALLCfgChg(void)
{
    WedgeConfigureALLThresholds();
    APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s] WEDGE CFGALL Cfg Chg", FmtTimeShow());
}

static void WedgeResetDefaultCfgChg(void)
{
    WedgeResettoFactoryDefaults();
    APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s] WEDGE Reset Default Cfg Chg", FmtTimeShow());
}

static void WedgeIGNTYPCfgChg(void)
{
    char *WedgeIGNTYPCfgChgStr= " WEDGE IGNTYP Cfg Chg Stat";
    IGNTYPETypeDef IGNTYPE = *((IGNTYPETypeDef *)WedgeCfgGet(WEDGE_CFG_IGNTYPE));

    if (IGNTYPE.itype == No_Ignition_detect)
    {
        APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s]%s1", FmtTimeShow(), WedgeIGNTYPCfgChgStr);
        WedgeIgnitionStateSet(WEDGE_IGN_TO_IGNORE_STATE);
    }
    else if (IGNTYPE.itype == Virtual_Ignition_Battery_Voltage)
    {



        APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s]%s2", FmtTimeShow(), WedgeIGNTYPCfgChgStr);
    }
    else if (IGNTYPE.itype == Virtual_Ignition_GPS_velocity)
    {



        APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s]%s3", FmtTimeShow(), WedgeIGNTYPCfgChgStr);
    }
    else if (IGNTYPE.itype == Wired_Ignition)
    {   
        if (GPIO_PIN_RESET == READ_IO(PC10_MCU_IGN_GPIO_Port, PC10_MCU_IGN_Pin))
        {
            APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s]%s4", FmtTimeShow(), WedgeIGNTYPCfgChgStr);
            WedgeIGNOffStateReset();
            WedgeIgnitionStateSet(WEDGE_IGN_ON_TO_OFF_STATE);
        }
        else
        {
            APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s]%s5", FmtTimeShow(), WedgeIGNTYPCfgChgStr);
            WedgeIGNOnStateReset();
            WedgeIgnitionStateSet(WEDGE_IGN_OFF_TO_ON_STATE);
        }
    }
    else
    {
        APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s]%s6", FmtTimeShow(), WedgeIGNTYPCfgChgStr);
    }
}

static void WedgeRPTINTVLCfgChg(void)
{
    RPTINTVLTypeDef RPTINTVL = {0};
    RPTINTVL = *((RPTINTVLTypeDef *)WedgeCfgGet(WEDGE_CFG_RPTINTVL));
    uint8_t ret = 0;
    char *WedgeRPTINTVLCfgChgStr= " WEDGE RPTINTVL Cfg Chg ";

    if (RPTINTVL.perint == 0)
    {
        ret = WedgeRtcTimerInstanceDel(Periodic_Moving_Event);
        APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s]%sRet%d", FmtTimeShow(), WedgeRPTINTVLCfgChgStr, ret);
    }
    else
    {
        APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s]%sPeriodic Moving Event Reinit"
                                , FmtTimeShow(), WedgeRPTINTVLCfgChgStr);
        WedgePeriodicMovingEventInit();
    }

    WedgePeriodicOffEventReset();
    WedgePeriodicHealthEventReset();
}

static void WedgeLVACfgChg(void)
{
    APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s] WEDGE LVA Cfg Chg", FmtTimeShow());
}

static void WedgeIDLECfgChg(void)
{
    APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s] WEDGE IDLE Cfg Chg", FmtTimeShow());
    WedgeIDLEDetectAlertReset();
}

static void WedgeSODOCfgChg(void)
{
    APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s] WEDGE SODO Cfg Chg", FmtTimeShow());
}

static void WedgeDIRCHGCfgChg(void)
{
    APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s] WEDGE DIRCHG Cfg Chg", FmtTimeShow());
}

static void WedgeTOWCfgChg(void)
{
    APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s] WEDGE TOW Cfg Chg", FmtTimeShow());
    WedgeTowAlertReset();
}

static void WedgeSTPINTVLCfgChg(void)
{
    WEDGEIgnitionStateTypeDef IgnitionState = WedgeIgnitionStateGet();
    IGNTYPETypeDef IGNTYPE = *((IGNTYPETypeDef *)WedgeCfgGet(WEDGE_CFG_IGNTYPE));

    APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s] WEDGE STPINTVL Cfg Chg", FmtTimeShow());
    if (IgnitionState == WEDGE_IGN_OFF_STATE)
    {
        if (IGNTYPE.itype != No_Ignition_detect)
        {
            // If at first the stop report had been sent, this will cause another report.
            WedgeStopReportOnToOffDisable();
            WedgeStopReportOnToOff();
        }
    }
}

static void WedgeVODOCfgChg(void)
{
    APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s] WEDGE VODO Cfg Chg", FmtTimeShow());
}

static void WedgeGEOFENCESCfgChg(WEDGECfgChangeTypeDef CfgChg)
{
    GFNCTypeDef GFNC = *((GFNCTypeDef *)WedgeCfgGet((WEDGECfgOperateTypeDef)(WEDGE_CFG_GFNC1 
                                                            + CfgChg - GEOFENCES1_CFG_CHG)));
    uint16_t GeofenceDefinedBitMap = *((uint16_t *)WedgeSysStateGet(WEDGE_GEOFENCE_BIT_MAP));

    APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s] WEDGE GEOFENCES Cfg Chg Index%d"
                                        , FmtTimeShow(), CfgChg - GEOFENCES1_CFG_CHG + 1);
    if (GFNC.index == 0)
    {
        GeofenceDefinedBitMap &= (~(0x01 << (CfgChg - GEOFENCES1_CFG_CHG)));
    }
    else
    {
        GeofenceDefinedBitMap |= (0x01 << (CfgChg - GEOFENCES1_CFG_CHG));
    }

    WedgeSysStateSet(WEDGE_GEOFENCE_BIT_MAP, &GeofenceDefinedBitMap);
}

static void WedgeRELAYCfgChg(void)
{
    RELAYTypeDef RELAY = *((RELAYTypeDef *)WedgeCfgGet(WEDGE_CFG_RELAY));

    APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s] WEDGE RELAY Cfg Chg", FmtTimeShow());
    if (RELAY.state == 0)
    {
        WRITE_IO(GPIOA, PA15_MCU_RELAY_Pin, GPIO_PIN_RESET);
    }
    else
    {
        WRITE_IO(GPIOA, PA15_MCU_RELAY_Pin, GPIO_PIN_SET);
    }
}

static void WedgePLSRLYCfgChg(void)
{





    APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s] WEDGE PLSRLY Cfg Chg Reserved", FmtTimeShow());
}

static void WedgeOSPDCfgChg(void)
{
    APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s] WEDGE OSPD Cfg Chg", FmtTimeShow());
    WedgeOverSpeedAlertReset();
}

static void WedgeGPSDIAGCfgChg(void)
{
    APP_PRINT(DbgCtl.WedgeAppLogInfoEn, "\r\n[%s] WEDGE GPSDIAG Cfg Chg", FmtTimeShow());
}

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2018. All rights reserved
                                End Of The File
*******************************************************************************/
