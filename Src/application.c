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

void ApplicationProcess(void)
{
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
    }
}

static void WedgeInit(void)
{
    
}

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2017. All rights reserved
                                End Of The File
*******************************************************************************/
