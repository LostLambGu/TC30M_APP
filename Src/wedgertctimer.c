/*******************************************************************************
* File Name          : wedgertctimer.c
* Author             : Yangjie Gu
* Description        : This file provides all the wedgertctimer functions.

* History:
*  12/31/2017 : wedgertctimer V1.00
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "wedgertctimer.h"
#include "rtcclock.h"
#include "eventalertflow.h"
#include "eventmsgque_process.h"

/* Private define ------------------------------------------------------------*/
#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define WEDGE_RTC_TIMER_PRINT DebugPrintf

/* Variables -----------------------------------------------------------------*/
static RTCTimerListTypeDef RTCTimerList = {0};
static uint8_t WedgeRtcAlarmHappen = 0;

/* Function definition -------------------------------------------------------*/
static uint8_t WedgeRtcTimerInstanceInsert(RTCTimerListCellTypeDef Instance);
static uint8_t WedgeRtcTimerFetchMinInArray(RTCTimerListCellTypeDef *pInstance, uint8_t *pIndex);
static uint8_t WedgeRtcTimerInstanceAlarmRefresh(void);

void WedgeSetRTCAlarmStatus(uint8_t Status)
{
    WedgeRtcAlarmHappen = Status;
}

uint8_t WedgeGetRTCAlarmStatus(void)
{
    return WedgeRtcAlarmHappen;
}

static RTCTimerListCellTypeDef WedgeRTCGetCurrentInstance(void)
{
    return RTCTimerList.currentinstance;
}

uint32_t WedgeRtcCurrentSeconds(void)
{
    TimeTableT timetable = {0};
    uint32_t currenttimeinseconds = {0};

    timetable = GetRTCDatetime();
    currenttimeinseconds = TimeTableToSeconds(timetable);

    return currenttimeinseconds;
}

uint8_t WedgeRtcTimerInit(void *pRTCTimerList)
{












	return 1;
}

uint8_t WedgeRtcHwrstPowerLostJudge(void)
{









    return 0; // HWRST
}

uint8_t WedgeRtcTimerInstanceAdd(RTCTimerListCellTypeDef Instance)
{
    uint8_t Index = 0, ret = 0;
    RTCTimerListCellTypeDef tmpinstance;
    char *WedgeRtcTimerInstanceAddErrStr = "WEDGE RTC Timer Add Insert err";

    // WedgeRtcTimerInstanceInsert has Param check
    ret = WedgeRtcTimerInstanceInsert(Instance);
    if (ret)
    {
        WEDGE_RTC_TIMER_PRINT(DbgCtl.WedgeRtcTimerInfoEn, "\r\n[%s] %s%d",
				FmtTimeShow(), WedgeRtcTimerInstanceAddErrStr, ret);
        return 1;
    }

    ret = WedgeRtcTimerFetchMinInArray(&tmpinstance, &Index);
    if (ret)
    {
        WEDGE_RTC_TIMER_PRINT(DbgCtl.WedgeRtcTimerInfoEn, "\r\n[%s] %s%d",
				FmtTimeShow(), WedgeRtcTimerInstanceAddErrStr, ret);
        return 2;
    }
    
    if (tmpinstance.settime < (RTCTimerList.currentinstance).settime)
    {
        RTCTimerList.instancearray[Index] = RTCTimerList.currentinstance;
        RTCTimerList.currentinstance = tmpinstance;
    }

    ret = WedgeRtcTimerInstanceAlarmRefresh();
    if (ret)
    {
        WEDGE_RTC_TIMER_PRINT(DbgCtl.WedgeRtcTimerInfoEn, "\r\n[%s] %s%d",
				FmtTimeShow(), WedgeRtcTimerInstanceAddErrStr, ret);
        return 3;
    }

    return 0;
}

uint8_t WedgeRtcTimerInstanceDel(WEDGERTCTimerInstanceTypeDef InstanceType)
{
    uint8_t i = 0, Index = 0, ret = 0;
    RTCTimerListCellTypeDef tmpinstance;

    if (InstanceType >= WEDGE_RTC_TIMER_INSTANCE_INVALID_MAX)
    {
        return 1;
    }

    if (RTCTimerList.instancenum == 0)
    {
        return 2;
    }

    for (i = 0; i < WEDGE_RTC_TIMER_INSTANCE_MAX; i++)
    {
        if (((RTCTimerList.instancearray[i]).RTCTimerType > WEDGE_RTC_TIMER_INVALID)
        &&  ((RTCTimerList.instancearray[i]).RTCTimerType < WEDGE_RTC_TIMER_MAX))
        {
            if ((RTCTimerList.instancearray[i]).RTCTimerInstance == InstanceType)
            {
                (RTCTimerList.instancearray[i]).RTCTimerType = WEDGE_RTC_TIMER_INVALID;
                RTCTimerList.instancenum--;
            }
        }
    }

    if ((RTCTimerList.currentinstance).RTCTimerInstance == InstanceType)
    {
        ret = WedgeRtcTimerFetchMinInArray(&tmpinstance, &Index);
        if (ret)
        {
            WEDGE_RTC_TIMER_PRINT(DbgCtl.WedgeRtcTimerInfoEn, "\r\n[%s] WEDGE RTC Timer Del Fetch err%d",
                                      FmtTimeShow(), ret);
            return 3;
        }

        RTCTimerList.currentinstance = tmpinstance;
        (RTCTimerList.instancearray[Index]).RTCTimerType = WEDGE_RTC_TIMER_INVALID;

        ret = WedgeRtcTimerInstanceAlarmRefresh();
        if (ret)
        {
            WEDGE_RTC_TIMER_PRINT(DbgCtl.WedgeRtcTimerInfoEn, "\r\n[%s] WEDGE RTC Timer Del Refresh err%d",
                                      FmtTimeShow(), ret);
            return 4;
        }
    }

    return 0;
    
}

uint8_t WedgeRtcTimerModifySettime(uint32_t Delta, WEDGERTCTimerSettimeTypeDef ModifyType)
{
    uint8_t i = 0, ret = 0;

    if (ModifyType >= WEDGE_RTC_TIMER_MODIFY_INVALID_MAX)
    {
        return 1;
    }
    
    if (Delta == 0)
    {
        return 0;
    }

    for (i = 0; i < WEDGE_RTC_TIMER_INSTANCE_MAX; i++)
    {
        if (((RTCTimerList.instancearray[i]).RTCTimerType > WEDGE_RTC_TIMER_INVALID)
        &&  ((RTCTimerList.instancearray[i]).RTCTimerType < WEDGE_RTC_TIMER_MAX))
        {
            if (ModifyType == WEDGE_RTC_TIMER_MODIFY_INCREASE)
            {
                (RTCTimerList.instancearray[i]).settime += Delta;
            }
            else
            {
                (RTCTimerList.instancearray[i]).settime -= Delta;
            }
        }
    }

    if (ModifyType == WEDGE_RTC_TIMER_MODIFY_INCREASE)
    {
        (RTCTimerList.currentinstance).settime += Delta;
    }
    else
    {
        (RTCTimerList.currentinstance).settime -= Delta;
    }

    ret = WedgeRtcTimerInstanceAlarmRefresh();
	if (ret)
    {
        WEDGE_RTC_TIMER_PRINT(DbgCtl.WedgeRtcTimerInfoEn, "\r\n[%s] WEDGE RTC Timer Modify Refresh err: %d",
                                  FmtTimeShow(), ret);
        return 2;
    }

    return 0;
}

void WedgeRTCTimerEventProcess(void)
{
    RTCTimerListCellTypeDef currentinstance;
    uint8_t ret = 0;

    if (WedgeGetRTCAlarmStatus() == FALSE)
    {
        return;
    }

    memset(&currentinstance, 0, sizeof(currentinstance));
    currentinstance = WedgeRTCGetCurrentInstance();
    WedgeSetRTCAlarmStatus(FALSE);

    ret = WedgeRtcTimerInstanceAlarmRefresh();
    if (ret == 0)
    {
        return;
    }

    switch (currentinstance.RTCTimerInstance)
    {
    case Periodic_Moving_Event:
    {
        WEDGE_RTC_TIMER_PRINT(DbgCtl.WedgeRtcTimerInfoEn, "\r\n[%s] WEDGE RTC Timer Process: %d",
                                  FmtTimeShow(), Periodic_Moving_Event);
    }
        break;

    case Periodic_OFF_Event:
        break;

    case Periodic_Health_Event:
        break;

    case Stop_Report_Onetime_Event:
        break;

    case Periodic_Hardware_Reset_Onetime:
        break;

    default:
        break;
    }
}

static uint8_t WedgeRtcTimerInstanceInsert(RTCTimerListCellTypeDef Instance)
{
    uint8_t i = 0;

    if (RTCTimerList.instancenum >= WEDGE_RTC_TIMER_INSTANCE_MAX)
    {
        return 1;
    }

    if ((Instance.RTCTimerType >= WEDGE_RTC_TIMER_MAX) || (Instance.RTCTimerType == WEDGE_RTC_TIMER_INVALID))
    {
        return 2;
    }

    for (i = 0; i < WEDGE_RTC_TIMER_INSTANCE_MAX; i++)
    {
        if ((RTCTimerList.instancearray[i]).RTCTimerType == WEDGE_RTC_TIMER_INVALID)
        {

        }
        else if ((RTCTimerList.instancearray[i]).RTCTimerType < WEDGE_RTC_TIMER_MAX)
        {
            if ((RTCTimerList.instancearray[i]).RTCTimerInstance == Instance.RTCTimerInstance)
            {
                RTCTimerList.instancearray[i] = Instance;
                return 0;
            }
        }
        else
        {

        }
    }

    if ((RTCTimerList.currentinstance).RTCTimerInstance == Instance.RTCTimerInstance)
    {
        RTCTimerList.currentinstance = Instance;
        return 0;
    }

    for (i = 0; i < WEDGE_RTC_TIMER_INSTANCE_MAX; i++)
    {
        if ((RTCTimerList.instancearray[i]).RTCTimerType == WEDGE_RTC_TIMER_INVALID)
        {
            break;
        }
    }

    if (i >= WEDGE_RTC_TIMER_INSTANCE_MAX)
    {
        return 3;
    }

    RTCTimerList.instancearray[i] = Instance;
    RTCTimerList.instancenum++;

    return 0;
}

static uint8_t WedgeRtcTimerFetchMinInArray(RTCTimerListCellTypeDef *pInstance, uint8_t *pIndex)
{
    RTCTimerListCellTypeDef mintmpinstance;
    uint8_t i = 0, Index;

    if ((pInstance == NULL) || (pIndex == NULL))
    {
        return 1;
    }

    if (RTCTimerList.instancenum == 0)
    {
        return 2;
    }

    mintmpinstance.settime = 0xFFFFFFFF;
    Index = WEDGE_RTC_TIMER_INSTANCE_MAX;

    for (i = 0; i < WEDGE_RTC_TIMER_INSTANCE_MAX; i++)
    {
        if (((RTCTimerList.instancearray[i]).RTCTimerType != WEDGE_RTC_TIMER_INVALID)
            && ((RTCTimerList.instancearray[i]).RTCTimerType < WEDGE_RTC_TIMER_MAX))
        {
            if ((RTCTimerList.instancearray[i]).settime < mintmpinstance.settime)
            {
                mintmpinstance = RTCTimerList.instancearray[i];
                Index = i;
            }
        }
    }

    if (Index == WEDGE_RTC_TIMER_INSTANCE_MAX)
    {
        //No valid rtc timer in list
        return 3;
    }

    *pInstance = mintmpinstance;
    *pIndex = Index;
    return 0;
}

static uint8_t WedgeRtcTimerInstanceAlarmRefresh(void)
{
    uint32_t curseconds = WedgeRtcCurrentSeconds();
    uint32_t setseconds = 0;

    if ((RTCTimerList.currentinstance).settime <= curseconds)
    {







        return 4;
    }

    setseconds = (RTCTimerList.currentinstance).settime - curseconds;

    return SetRTCAlarmTime(setseconds, DbgCtl.WedgeRtcTimerInfoEn);
}

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2017. All rights reserved
                                End Of The File
*******************************************************************************/
