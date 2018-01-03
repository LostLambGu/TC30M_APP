/*******************************************************************************
* File Name          : application.c
* Author             : Yangjie Gu
* Description        : This file provides all the application functions.

* History:
*  12/12/2017 : application V1.00
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "application.h"

static void WedgeInit(void);
static void WedgeIgnitionStateProcess(void);

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
        // Wedge Event and Alert Process
        WedgeIgnitionStateProcess();
        // Wedge Configuration Change Process
        WedgeCfgChgStateProcess();
    }
}

static void WedgeInit(void)
{

}

static void WedgeIgnitionStateProcess(void)
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
        APP_LOG("WEDGE Ign Stat Err");
        break;
    }

    return;
}

void WedgeCfgChgStateProcess(void)
{   
    // case SMS_ADDR_CFG_CHG:
    //     break;
    // case SVRCFG_CFG_CHG:
    //     break;
    // case FTPCFG_CFG_CHG:
    //     break;
    // case APNCFG_CFG_CHG:
    //     break;
    // case HWRST_CFG_CHG:
    //     break;
    // case PWRMGT_CFG_CHG:
    //     break;
    // case USRDAT_CFG_CHG:
    //     break;
    // case CFGALL_CFG_CHG:
    //     break;
    // case RESET_DEFAULT_CFG_CHG:
    //     break;
    // case IGNTYP_CFG_CHG:
    //     break;
    // case RPTINTVL_CFG_CHG:
    //     break;
    // case ALARM1_CFG_CHG:
    //     break;
    // case ALARM2_CFG_CHG:
    //     break;
    // case LVA_CFG_CHG:
    //     break;
    // case IDLE_CFG_CHG:
    //     break;
    // case SODO_CFG_CHG:
    //     break;
    // case DIRCHG_CFG_CHG:
    //     break;
    // case TOW_CFG_CHG:
    //     break;
    // case STPINTVL_CFG_CHG:
    //     break;
    // case VODO_CFG_CHG:
    //     break;
    // case GEOFENCES1_CFG_CHG:
    // case GEOFENCES2_CFG_CHG:
    // case GEOFENCES3_CFG_CHG:
    // case GEOFENCES4_CFG_CHG:
    // case GEOFENCES5_CFG_CHG:
    // case GEOFENCES6_CFG_CHG:
    // case GEOFENCES7_CFG_CHG:
    // case GEOFENCES8_CFG_CHG:
    // case GEOFENCES9_CFG_CHG:
    // case GEOFENCES10_CFG_CHG:
    //     break;
    // case RELAY_CFG_CHG:
    //     break;
    // case PLSRLY_CFG_CHG:
    //     break;
    // case OPSOD_CFG_CHG:
    //     break;
    // default:
    //     EVENTMSGQUE_PROCESS_LOG("WEDGE CFG CHG SET: Param err");
    //     break;
}

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2018. All rights reserved
                                End Of The File
*******************************************************************************/
