/*******************************************************************************
* File Name          : wedgeapplication.c
* Author             : Yangjie Gu
* Description        : This file provides all the wedgeapplication functions.

* History:
*  12/12/2017 : wedgeapplication V1.00
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "wedgeapplication.h"

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

static void WedgeInit(void);
static void WedgeUdpInit(void);
static void WedgeIgnitionStateProcess(void);
static void WedgeIGNOnStateReset(void);
static void WedgeIGNOffStateReset(void);
static void WedgeMsgQueProcess(void);

static void WedgeCfgChgStateProcess(void);
static void WedgeDeviceInfoSave(void);
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

void ApplicationProcess(void)
{
    WedgeInit();

    while (1)
    {
        // Debug Info
        ATCmdDetection();
        // LTE Info
        LteCmdDetection();
        // Soft Timer
        SoftwareCheckTimerStatus();
        // Gsensor Interupt
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
        // Wedge Rtc Timer Event Process
        WedgeRTCTimerEventProcess();
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
        if (IGNTYPE.itype == Wired_Ignition)
        {
            WedgeLocationOfDisabledVehicleOnToOff();

            // When Ignition on, WEDGESysState.StopReportOnetimeRtcTimerAdded need to be reset!
            WedgeStopReportOnToOff();
        }

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

    ret = WedgeIsStartFromPowerLost();
    if (ret != FALSE)
    {
        APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s]%sFrom Power Lost",
                                      FmtTimeShow(), WedgeInitStr);
        WedgeIsPowerLostSet(TRUE);
    }
    else
    {
        WedgeIsPowerLostSet(FALSE);
        APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s]%s",
                                      FmtTimeShow(), WedgeInitStr);
    }

    memset(&WEDGEDeviceInfo, 0, sizeof(WEDGEDeviceInfo));
    if (0 == WedgeDeviceInfoRead((uint8_t *)&WEDGEDeviceInfo, sizeof(WEDGEDeviceInfo)))
    {
        // Normal
        APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s]%sDevice Info Read",
                                      FmtTimeShow(), WedgeInitStr);

        WedgeCfgInit(&(WEDGEDeviceInfo.WEDGECfg));

        WedgeSysStateInit(&(WEDGEDeviceInfo.WEDGESysState));

        WedgeRtcTimerInit(&(WEDGEDeviceInfo.RTCTimerList));
    }
    else
    {
        // Abnormal
        APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s]%sDevice Info Not Read",
                                      FmtTimeShow(), WedgeInitStr);

        WedgeCfgInit(NULL);

        WedgeSysStateInit(NULL);

        WedgeRtcTimerInit(NULL);
    }

    WedgeCfgChgStateInit();

    WedgeMsgQueInit();

    WedgeUdpInit();

    WedgePeriodicHardwareResetInit();

    WedgePeriodicMovingEventInit();
}

static void WedgeOpenUdpSocket(SVRCFGTypeDef *pSVRCFG, uint8_t socketnum)
{
    if (pSVRCFG == NULL)
    {
        APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] WEDGE Udp Open Parm Err1",
                  FmtTimeShow());
    }

    switch(socketnum)
    {
    case 1:
        if (strlen((const char *)pSVRCFG->srvr1) > strlen("\"\""))
        {
            UdpIpSocketOpen(1, 1111, (char *)pSVRCFG->srvr1, pSVRCFG->port);
        }
        break;

    case 2:
        if (strlen((const char *)pSVRCFG->srvr2) > strlen("\"\""))
        {
            UdpIpSocketOpen(2, 2222, (char *)pSVRCFG->srvr2, pSVRCFG->port);
        }
        break;

    case 3:
        if (strlen((const char *)pSVRCFG->srvr3) > strlen("\"\""))
        {
            UdpIpSocketOpen(3, 3333, (char *)pSVRCFG->srvr3, pSVRCFG->port);
        }
        break;

    case 4:
        if (strlen((const char *)pSVRCFG->srvr4) > strlen("\"\""))
        {
            UdpIpSocketOpen(4, 4444, (char *)pSVRCFG->srvr4, pSVRCFG->port);
        }
        break;

    case 5:
        if (strlen((const char *)pSVRCFG->srvr5) > strlen("\"\""))
        {
            UdpIpSocketOpen(5, 5555, (char *)pSVRCFG->srvr5, pSVRCFG->port);
        }
        break;

    default:
        APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] WEDGE Udp Open Parm Err2",
                  FmtTimeShow());
        break;
    }
}

static void WedgeUdpInit(void)
{
    #define SVRCFG_UDP (1)
    #define SVRCFG_MAX_UDP_NUM (5)
    SVRCFGTypeDef SVRCFG;
    uint8_t i = 0;

    memset(&SVRCFG, 0, sizeof(SVRCFG));

    SVRCFG = *((SVRCFGTypeDef *)WedgeCfgGet(WEDGE_CFG_SVRCFG));

    if (SVRCFG.prot != SVRCFG_UDP)
    {
        APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] WEDGE Udp Init Not Udp",
                                      FmtTimeShow());
        return;
    }

    for (i = 0; i < SVRCFG_MAX_UDP_NUM; i++)
    {
        WedgeOpenUdpSocket(&SVRCFG, i + 1);
    }
}

static void WedgeMsgQueProcess(void)
{








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
                    APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s]%sFTPCFG Cfg Chg Reserved",
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
                    APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s]%sAlarm Cfg Chg Reserved",
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

                default:
                    APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s]%sDefault",
                                      FmtTimeShow(), WedgeCfgChgStateProcessStr);
                    break;
            }
        }
    }

    // Wedge Device Info Save
    WedgeDeviceInfoSave();
}

static void WedgeDeviceInfoSave(void)
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
        APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s]%sErr%d",
                    FmtTimeShow(), WedgeDeviceInfoSaveStr, ret);
    }
    else
    {
        APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s]%sOk",
                    FmtTimeShow(), WedgeDeviceInfoSaveStr);
    }
}

static void WedgeSMSAddrCfgChg(void)
{




    APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] WEDGE SMS Addr Cfg Chg Reserved", FmtTimeShow());
}

static void WedgeSVRCFGCfgChg(WEDGECfgChangeTypeDef CfgChg)
{



    APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] WEDGE SVRCFG Cfg Chg Reserved", FmtTimeShow());
}

static void WedgeAPNCfgChg(void)
{




    APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] WEDGE APN Cfg Chg Reserved", FmtTimeShow());
}

static void WedgeHWRRSTCfgChg(void)
{
    APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] WEDGE HWRRST Cfg Chg", FmtTimeShow());
    WedgePeriodicHardwareResetReinit();
}

static void WedgePWRMGTCfgChg(void)
{



    APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] WEDGE PWRMGT Cfg Chg Reserved", FmtTimeShow());
}

static void WedgeUSRDATCfgChg(void)
{




    APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] WEDGE USRDAT Cfg Chg Reserved", FmtTimeShow());
}
static void WedgeCFGALLCfgChg(void)
{




    APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] WEDGE CFGALL Cfg Chg Reserved", FmtTimeShow());
}

static void WedgeResetDefaultCfgChg(void)
{




    APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] WEDGE Reset Default Cfg Chg Reserved", FmtTimeShow());
}

static void WedgeIGNTYPCfgChg(void)
{
    char *WedgeIGNTYPCfgChgStr= " WEDGE IGNTYP Cfg Chg Stat";
    IGNTYPETypeDef IGNTYPE = *((IGNTYPETypeDef *)WedgeCfgGet(WEDGE_CFG_IGNTYPE));

    if (IGNTYPE.itype == No_Ignition_detect)
    {
        APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s]%s1", FmtTimeShow(), WedgeIGNTYPCfgChgStr);
        WedgeIgnitionStateSet(WEDGE_IGN_TO_IGNORE_STATE);
    }
    else if (IGNTYPE.itype == Virtual_Ignition_Battery_Voltage)
    {



        APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s]%s2", FmtTimeShow(), WedgeIGNTYPCfgChgStr);
    }
    else if (IGNTYPE.itype == Virtual_Ignition_GPS_velocity)
    {



        APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s]%s3", FmtTimeShow(), WedgeIGNTYPCfgChgStr);
    }
    else if (IGNTYPE.itype == Wired_Ignition)
    {
        if (GPIO_PIN_RESET == READ_IO(PC10_MCU_IGN_GPIO_Port, PC10_MCU_IGN_Pin))
        {
            APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s]%s4", FmtTimeShow(), WedgeIGNTYPCfgChgStr);
            WedgeIgnitionStateSet(WEDGE_IGN_ON_TO_OFF_STATE);
        }
        else
        {
            APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s]%s5", FmtTimeShow(), WedgeIGNTYPCfgChgStr);
            WedgeIgnitionStateSet(WEDGE_IGN_OFF_TO_ON_STATE);
        }
    }
    else
    {
        APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s]%s6", FmtTimeShow(), WedgeIGNTYPCfgChgStr);
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
        APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s]%sRet%d", FmtTimeShow(), WedgeRPTINTVLCfgChgStr, ret);
    }
    else
    {
        APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s]%sPeriodic Moving Event Reinit"
                                , FmtTimeShow(), WedgeRPTINTVLCfgChgStr);
        WedgePeriodicMovingEventInit();
    }

    WedgePeriodicOffEventReset();
    WedgePeriodicHealthEventReset();
}

static void WedgeLVACfgChg(void)
{
    APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] WEDGE LVA Cfg Chg", FmtTimeShow());
}

static void WedgeIDLECfgChg(void)
{
    APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] WEDGE IDLE Cfg Chg", FmtTimeShow());
    WedgeIDLEDetectAlertReset();
}

static void WedgeSODOCfgChg(void)
{
    APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] WEDGE SODO Cfg Chg", FmtTimeShow());
}

static void WedgeDIRCHGCfgChg(void)
{
    APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] WEDGE DIRCHG Cfg Chg", FmtTimeShow());
}

static void WedgeTOWCfgChg(void)
{
    APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] WEDGE TOW Cfg Chg", FmtTimeShow());
    WedgeTowAlertReset();
}

static void WedgeSTPINTVLCfgChg(void)
{
    WEDGEIgnitionStateTypeDef IgnitionState = WedgeIgnitionStateGet();
    IGNTYPETypeDef IGNTYPE = *((IGNTYPETypeDef *)WedgeCfgGet(WEDGE_CFG_IGNTYPE));

    APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] WEDGE TOW Cfg Chg", FmtTimeShow());
    if (IgnitionState == WEDGE_IGN_OFF_STATE)
    {
        if (IGNTYPE.itype == Wired_Ignition)
        {
            // If at first the stop report had been sent, this will cause another report.
            WedgeStopReportOnToOffDisable();
            WedgeStopReportOnToOff();
        }
    }
}

static void WedgeVODOCfgChg(void)
{
    APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] WEDGE VODO Cfg Chg", FmtTimeShow());
}

static void WedgeGEOFENCESCfgChg(WEDGECfgChangeTypeDef CfgChg)
{
    GFNCTypeDef GFNC = *((GFNCTypeDef *)WedgeCfgGet((WEDGECfgOperateTypeDef)(WEDGE_CFG_GFNC1 
                                                            + CfgChg - GEOFENCES1_CFG_CHG)));
    uint16_t GeofenceDefinedBitMap = *((uint16_t *)WedgeSysStateGet(WEDGE_GEOFENCE_BIT_MAP));

    APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] WEDGE GEOFENCES Cfg Chg Index%d"
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

    APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] WEDGE RELAY Cfg Chg", FmtTimeShow());
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





    APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] WEDGE PLSRLY Cfg Chg Reserved", FmtTimeShow());
}

static void WedgeOSPDCfgChg(void)
{
    APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] WEDGE OSPD Cfg Chg", FmtTimeShow());
    WedgeOverSpeedAlertReset();
}

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2018. All rights reserved
                                End Of The File
*******************************************************************************/
