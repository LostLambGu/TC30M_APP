#include "wedgetest.h"

#if WEDGE_RTC_TIMER_TEST

void WedgeRtcTimerTest(void)
{
    RTCTimerListCellTypeDef Cell1;
    RTCTimerListCellTypeDef Cell2;
    RTCTimerListCellTypeDef Cell3;
    RTCTimerListCellTypeDef Cell4;
    RPTINTVLTypeDef RPTINTVL;
    uint32_t curtime = WedgeRtcCurrentSeconds();
    uint32_t i = 0;

    Cell1.RTCTimerType = WEDGE_RTC_TIMER_PERIODIC;
    Cell1.RTCTimerInstance = Periodic_Moving_Event;
    Cell1.settime = curtime + 1;

    Cell2.RTCTimerType = WEDGE_RTC_TIMER_ONETIME;
    Cell2.RTCTimerInstance = Stop_Report_Onetime_Event;
    Cell2.settime = curtime + 5;

    Cell3.RTCTimerType = WEDGE_RTC_TIMER_PERIODIC;
    Cell3.RTCTimerInstance = Periodic_Health_Event;
    Cell3.settime = curtime + 2;

    Cell4.RTCTimerType = WEDGE_RTC_TIMER_PERIODIC;
    Cell4.RTCTimerInstance = Periodic_OFF_Event;
    Cell4.settime = curtime + 3;

    RPTINTVL.ionint = 10;
    RPTINTVL.ioffint = 10;
    RPTINTVL.perint = 20;
    WedgeCfgSet(WEDGE_CFG_RPTINTVL, &RPTINTVL);

    if (0 == WedgeRtcTimerInstanceAdd(Cell1))
    {
        DebugLog("WedgeRtcTimerInstanceAdd(Cell1) OK");
    }
    else
    {
        DebugLog("WedgeRtcTimerInstanceAdd(Cell1) err");
    }

    if (0 == WedgeRtcTimerInstanceAdd(Cell2))
    {
        DebugLog("WedgeRtcTimerInstanceAdd(Cell2) OK");
    }
    else
    {
        DebugLog("WedgeRtcTimerInstanceAdd(Cell2) err");
    }

    if (0 == WedgeRtcTimerInstanceAdd(Cell3))
    {
        DebugLog("WedgeRtcTimerInstanceAdd(Cell3) OK");
    }
    else
    {
        DebugLog("WedgeRtcTimerInstanceAdd(Cell3) err");
    }

    if (0 == WedgeRtcTimerInstanceAdd(Cell4))
    {
        DebugLog("WedgeRtcTimerInstanceAdd(Cell4) OK");
    }
    else
    {
        DebugLog("WedgeRtcTimerInstanceAdd(Cell4) err");
    }

    DebugLog("WedgeRtcTimerTest Start!");
    while (1)
    {
        WedgeRTCTimerEventProcess();
        i++;

        if (i % 0xffff == 0)
        {
            WedgeRtcTimerInstanceDel(Periodic_Health_Event);
            WedgeRtcTimerInstanceDel(Stop_Report_Onetime_Event);
        }
    }
}

#endif
