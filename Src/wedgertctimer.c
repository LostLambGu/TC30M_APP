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
#include "initialization.h"
#include "ublox_driver.h"
#include "wedgeeventalertflow.h"
#include "wedgecommonapi.h"

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
static uint8_t WedgeRtcTimerPeriodOverProcess(RTCTimerListCellTypeDef RTCTimerListCell, uint32_t Period);
static RTCTimerListCellTypeDef WedgeRTCGetCurrentInstance(void);

extern void WedgeDeviceInfoSave(void);
extern void MCUReset(void);

void WedgeRTCTimerEventProcess(void)
{
    RTCTimerListCellTypeDef currentinstance;
    uint8_t ret = 0;
    uint32_t Period = 0;
    RPTINTVLTypeDef RPTINTVL;
    char *WedgeRTCTimerEventProcessStr = " WEDGE RTC Timer ";

    if (WedgeGetRTCAlarmStatus() == FALSE)
    {
        return;
    }

    WedgeSetRTCAlarmStatus(FALSE);
    memset(&currentinstance, 0, sizeof(currentinstance));
    currentinstance = WedgeRTCGetCurrentInstance();

    switch (currentinstance.RTCTimerInstance)
    {
    case Periodic_Moving_Event:
    {
        double speedkm = 0.0;
        RPTINTVL = *((RPTINTVLTypeDef *)WedgeCfgGet(WEDGE_CFG_RPTINTVL));
        Period = RPTINTVL.perint * WEDGE_MINUTE_TO_SECOND_FACTOR;
        
        speedkm = UbloxSpeedKM();
        if (UbloxFixStateGet() == TRUE)
        {
            if (speedkm > (WEDGE_PERIOD_MOVING_MILES_PERHOUR * MILE_TO_KM_FACTOR))
            {
                WEDGE_RTC_TIMER_PRINT(DbgCtl.WedgeRtcTimerInfoEn, "\r\n[%s]%sPeriodic_Moving_Event-->(UdpBinary)",
                              FmtTimeShow(), WedgeRTCTimerEventProcessStr);
                WedgeResponseUdpBinary(WEDGEPYLD_STATUS, Drive_Report_Location);
            }
            else
            {
                WEDGE_RTC_TIMER_PRINT(DbgCtl.WedgeRtcTimerInfoEn, "\r\n[%s]%sPeriodic_Moving_Event Speed < %dmile/hour",
                              FmtTimeShow(), WedgeRTCTimerEventProcessStr, WEDGE_PERIOD_MOVING_MILES_PERHOUR);
            }
        }
        else
        {
            WEDGE_RTC_TIMER_PRINT(DbgCtl.WedgeRtcTimerInfoEn, "\r\n[%s]%sPeriodic_Moving_Event Gps Not fix",
                              FmtTimeShow(), WedgeRTCTimerEventProcessStr);
        }
    }
    break;

    case Periodic_OFF_Event:
    {
        RPTINTVL = *((RPTINTVLTypeDef *)WedgeCfgGet(WEDGE_CFG_RPTINTVL));
        Period = RPTINTVL.ioffint * WEDGE_MINUTE_TO_SECOND_FACTOR;
        WEDGE_RTC_TIMER_PRINT(DbgCtl.WedgeRtcTimerInfoEn, "\r\n[%s]%sPeriodic_OFF_Event-->(UdpBinary)",
                              FmtTimeShow(), WedgeRTCTimerEventProcessStr);
        WedgeResponseUdpBinary(WEDGEPYLD_STATUS, Periodic_Report_with_Ignition_OFF);
    }
    break;

    case Periodic_Health_Event:
    {
        RPTINTVL = *((RPTINTVLTypeDef *)WedgeCfgGet(WEDGE_CFG_RPTINTVL));
        Period = RPTINTVL.perint * WEDGE_MINUTE_TO_SECOND_FACTOR;
        WEDGE_RTC_TIMER_PRINT(DbgCtl.WedgeRtcTimerInfoEn, "\r\n[%s]%sPeriodic_Health_Event-->(UdpBinary)",
                              FmtTimeShow(), WedgeRTCTimerEventProcessStr);
        WedgeResponseUdpBinary(WEDGEPYLD_STATUS, Health_Message);
    }
    break;

    case Stop_Report_Onetime_Event:
    {
        WEDGE_RTC_TIMER_PRINT(DbgCtl.WedgeRtcTimerInfoEn, "\r\n[%s]%sStop_Report_Onetime_Event-->(UdpBinary)",
                              FmtTimeShow(), WedgeRTCTimerEventProcessStr);
        WedgeResponseUdpBinary(WEDGEPYLD_STATUS, Stop_Report);
    }
    break;

    case Periodic_Hardware_Reset_Onetime:
    {
        uint32_t LastHWRSTRTCTime = 0;
        WEDGE_RTC_TIMER_PRINT(DbgCtl.WedgeRtcTimerInfoEn, "\r\n[%s]%sPeriodic_Hardware_Reset_Onetime",
                              FmtTimeShow(), WedgeRTCTimerEventProcessStr);
        
        UbloxGPSStop();
        ModemPowerEnControl(DISABLE);

        LastHWRSTRTCTime = WedgeRtcCurrentSeconds();
        WedgeSysStateSet(WEDGE_LAST_HWRST_RTC_TIME, &LastHWRSTRTCTime);
        WedgeDeviceInfoSave();
        // Message not sent out save.
        // Reserved
        WedgeSetPowerLostBeforeReset();
        WEDGE_RTC_TIMER_PRINT(DbgCtl.WedgeRtcTimerInfoEn, "\r\n[%s]%sPeriodic_Hardware_Reset_Onetime MCUReset",
                              FmtTimeShow(), WedgeRTCTimerEventProcessStr);
        MCUReset();
    }
    break;

    default:
        WEDGE_RTC_TIMER_PRINT(DbgCtl.WedgeRtcTimerInfoEn, "\r\n[%s]%sProcess err",
                              FmtTimeShow(), WedgeRTCTimerEventProcessStr);
        return;
    }

    ret = WedgeRtcTimerPeriodOverProcess(currentinstance, Period);
    if (0 != ret)
    {
        WEDGE_RTC_TIMER_PRINT(DbgCtl.WedgeRtcTimerInfoEn, "\r\n[%s]%s PeriodOver err%d",
                              FmtTimeShow(), WedgeRTCTimerEventProcessStr, ret);
        return;
    }
    WEDGE_RTC_TIMER_PRINT(DbgCtl.WedgeRtcTimerInfoEn, "\r\n[%s]%sProcess Ok",
                          FmtTimeShow(), WedgeRTCTimerEventProcessStr, ret);
}

void WedgeSetRTCAlarmStatus(uint8_t Status)
{
    WedgeRtcAlarmHappen = Status;
}

uint8_t WedgeGetRTCAlarmStatus(void)
{
    return WedgeRtcAlarmHappen;
}

uint32_t WedgeRtcCurrentSeconds(void)
{
    TimeTableT timetable = {0};
    uint32_t currenttimeinseconds = {0};

    timetable = GetRTCDatetime();
    currenttimeinseconds = TimeTableToSeconds(timetable);

    return currenttimeinseconds;
}

uint8_t WedgeRtcTimerInit(RTCTimerListTypeDef *pRTCTimerList)
{
    // Wedge Power/Start up time compensation
    #if TC30M_TEST_CONFIG_OFF
    #define WEDGE_HWREST_TIME_SECONDS (2)
    #else
    #define WEDGE_HWREST_TIME_SECONDS (1)
    #endif /* TC30M_TEST_CONFIG_OFF */
    uint32_t LastHWRSTRTCTime = 0, RTCTimeBase = 0;
    TimeTableT timetable = {0};

    memset(&RTCTimerList, 0, sizeof(RTCTimerList));

    if (pRTCTimerList == NULL)
    {

    }
    else
    {
        RTCTimerList = *pRTCTimerList;
        LastHWRSTRTCTime = *((uint32_t *)WedgeSysStateGet(WEDGE_LAST_HWRST_RTC_TIME));
        if (LastHWRSTRTCTime != 0)
        {
            // RTC Timer Compensation
            WedgeRtcTimerModifySettime(WEDGE_HWREST_TIME_SECONDS, WEDGE_RTC_TIMER_MODIFY_INCREASE);
            timetable = SecondsToTimeTable(LastHWRSTRTCTime + WEDGE_HWREST_TIME_SECONDS);
            SetRTCDatetime(&timetable);
        }
    }

    // The RTC set should before lte time get
    RTCTimeBase = WedgeRtcCurrentSeconds();
    WedgeSysStateSet(WEDGE_BASE_RTC_TIME, &RTCTimeBase);

    WEDGE_RTC_TIMER_PRINT(TRUE, "\r\n[%s] WEDGE RTCTimerInit RTCTimeBase(%u)", FmtTimeShow(), RTCTimeBase);

    return 0;
}

void WedgeRTCTimerListGet(uint8_t *pBuf, uint32_t *pSize)
{
    if ((pBuf == NULL) || (pSize == NULL))
    {
        return;
    }

    memcpy(pBuf, &RTCTimerList, sizeof(RTCTimerList));
    *pSize = sizeof(RTCTimerList);
}

uint8_t WedgeRtcTimerInstanceAdd(RTCTimerListCellTypeDef Instance)
{
    uint8_t Index = 0, ret = 0;
    RTCTimerListCellTypeDef tmpinstance;
    char *WedgeRtcTimerInstanceAddErrStr = "WEDGE RTC Timer Add";

    // WedgeRtcTimerInstanceInsert has Param check
    ret = WedgeRtcTimerInstanceInsert(Instance);
    if (ret)
    {
        WEDGE_RTC_TIMER_PRINT(DbgCtl.WedgeRtcTimerInfoEn, "\r\n[%s] %s Insert err%d",
				FmtTimeShow(), WedgeRtcTimerInstanceAddErrStr, ret);
        return 1;
    }

    ret = WedgeRtcTimerFetchMinInArray(&tmpinstance, &Index);
    if (ret)
    {
        WEDGE_RTC_TIMER_PRINT(DbgCtl.WedgeRtcTimerInfoEn, "\r\n[%s] %s Fetch err%d",
				FmtTimeShow(), WedgeRtcTimerInstanceAddErrStr, ret);

        if (ret != 3)
        {
            return 2;
        }
    }
    
    if (ret == 3)
    {

    }
    else
    {
        // Need this condition?
        if ((RTCTimerList.currentinstance.RTCTimerType == WEDGE_RTC_TIMER_INVALID) || (RTCTimerList.currentinstance.RTCTimerType >= WEDGE_RTC_TIMER_MAX))
        {
            RTCTimerList.currentinstance = tmpinstance;
        }
        else if (tmpinstance.settime < (RTCTimerList.currentinstance).settime)
        {
            RTCTimerList.instancearray[Index] = RTCTimerList.currentinstance;
            RTCTimerList.currentinstance = tmpinstance;
        }
    }
    
    ret = WedgeRtcTimerInstanceAlarmRefresh();
    if (ret)
    {
        WEDGE_RTC_TIMER_PRINT(DbgCtl.WedgeRtcTimerInfoEn, "\r\n[%s] %s Refresh err%d",
				FmtTimeShow(), WedgeRtcTimerInstanceAddErrStr, ret);
        return 3;
    }

    return 0;
}

uint8_t WedgeRtcTimerInstanceDel(WEDGERTCTimerInstanceTypeDef InstanceType)
{
    uint8_t i = 0, Index = 0, ret = 0;
    RTCTimerListCellTypeDef tmpinstance;
    char *WedgeRtcTimerInstanceDelErrStr = "WEDGE RTC Timer Del";

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
                return 0;
            }
        }
    }

    if ((RTCTimerList.currentinstance).RTCTimerInstance == InstanceType)
    {
        ret = WedgeRtcTimerFetchMinInArray(&tmpinstance, &Index);
        if (ret)
        {
            WEDGE_RTC_TIMER_PRINT(DbgCtl.WedgeRtcTimerInfoEn, "\r\n[%s] %s Fetch err%d",
                                      FmtTimeShow(), WedgeRtcTimerInstanceDelErrStr, ret);
            return 3;
        }

        RTCTimerList.currentinstance = tmpinstance;
        (RTCTimerList.instancearray[Index]).RTCTimerType = WEDGE_RTC_TIMER_INVALID;
        RTCTimerList.instancenum--;

        ret = WedgeRtcTimerInstanceAlarmRefresh();
        if (ret)
        {
            WEDGE_RTC_TIMER_PRINT(DbgCtl.WedgeRtcTimerInfoEn, "\r\n[%s] %s Refresh err%d",
                                      FmtTimeShow(), WedgeRtcTimerInstanceDelErrStr, ret);
            return 4;
        }
        return 0;
    }

    WEDGE_RTC_TIMER_PRINT(DbgCtl.WedgeRtcTimerInfoEn, "\r\n[%s] %s No Timer Instance:%d",
                                      FmtTimeShow(), WedgeRtcTimerInstanceDelErrStr, InstanceType);
    return 5;
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

        if (RTCTimerList.instancenum == 0)
        {
            RTCTimerList.instancenum++;
        }
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


    // If there is no instance in RTC Timer list , there is no instance type check, this may cause bugs(Luckyly this is at least on instance.)
    if ((RTCTimerList.currentinstance).settime <= curseconds)
    {
        // This may cause a difference between atctual and wanted
        setseconds = curseconds - RTCTimerList.currentinstance.settime;
        WedgeRtcTimerModifySettime((setseconds < 1) ? 1 : setseconds, WEDGE_RTC_TIMER_MODIFY_INCREASE);
        // return 4;
    }

    setseconds = (RTCTimerList.currentinstance).settime - curseconds;

    return SetRTCAlarmTime(setseconds, DbgCtl.WedgeRtcTimerInfoEn);
}

static uint8_t WedgeRtcTimerPeriodOverProcess(RTCTimerListCellTypeDef RTCTimerListCell, uint32_t Period)
{
    if (RTCTimerListCell.RTCTimerType == WEDGE_RTC_TIMER_ONETIME)
    {
        if (0 != WedgeRtcTimerInstanceDel(RTCTimerListCell.RTCTimerInstance))
        {
            return 1;
        }
        return 0;
    }
    else if (RTCTimerListCell.RTCTimerType == WEDGE_RTC_TIMER_PERIODIC)
    {
        if (Period == 0)
        {
            return 2;
        }

        RTCTimerListCell.settime += Period;
       if (0 != WedgeRtcTimerInstanceAdd(RTCTimerListCell))
       {
           return 3;
       }

       return 0;
    }
    else
    {
        return 4;
    }
}

static RTCTimerListCellTypeDef WedgeRTCGetCurrentInstance(void)
{
    return RTCTimerList.currentinstance;
}

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2018. All rights reserved
                                End Of The File
*******************************************************************************/
