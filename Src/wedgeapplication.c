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

static uint8_t WedgeIsPowerLost = FALSE;
static void WedgeInit(void);
static void WedgeUdpInit(void);
static void WedgeIgnitionStateProcess(void);
static void WedgeCfgChgStateProcess(void);
static void WedgeMsgQueProcess(void);
static void WedgeSMSAddrCfgChg(void);
static void WedgeSVRCFGCfgChg(WEDGECfgChangeTypeDef CfgChg);
static void WedgeAPNCfgChg(void);
static void WedgeHWRRSTCfgChg(void);
static void WedgePWRMGTCfgChg(void);
static void WedgeUSRDATCfgChg(void);
static void WedgeCFGALLCfgChg(void);
static void WedgeResetDefaultCfgChg(void);
static void WedgeIGNTYPCfgChg(void);

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
    case WEDGE_IGN_IGNORE_STATE:
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
    }
    break;

    default:
        APP_LOG("WEDGE Ign Stat Err");
        break;
    }

    return;
}

void WedgeIsPowerLostSet(uint8_t Status)
{
    WedgeIsPowerLost = Status;
}

uint8_t WedgeIsPowerLostGet(void)
{
    return WedgeIsPowerLost;
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

static void WedgeSMSAddrCfgChg(void)
{

}

static void WedgeSVRCFGCfgChg(WEDGECfgChangeTypeDef CfgChg)
{

}

static void WedgeAPNCfgChg(void)
{

}

static void WedgeHWRRSTCfgChg(void)
{

}

static void WedgePWRMGTCfgChg(void)
{

}

static void WedgeUSRDATCfgChg(void)
{

}
static void WedgeCFGALLCfgChg(void)
{

}

static void WedgeResetDefaultCfgChg(void)
{

}

static void WedgeIGNTYPCfgChg(void)
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

                default:
                    APP_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s]%sDefault",
                                      FmtTimeShow(), WedgeCfgChgStateProcessStr);
                    break;
            }
        }
    }

    // Wedge Device Info Save




}

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2018. All rights reserved
                                End Of The File
*******************************************************************************/
