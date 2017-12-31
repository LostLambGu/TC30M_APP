#include "wedgetest.h"

void WedgeRtcTimerTest(void)
{
    RTCTimerListCellTypeDef Cell1;
    RTCTimerListCellTypeDef Cell2;
    RTCTimerListCellTypeDef Cell3;
    RPTINTVLTypeDef RPTINTVL;
    uint32_t curtime = WedgeRtcCurrentSeconds();

    Cell1.RTCTimerType = WEDGE_RTC_TIMER_PERIODIC;
    Cell1.RTCTimerInstance = Periodic_Moving_Event;
    Cell1.settime = curtime + 1;

    Cell2.RTCTimerType = WEDGE_RTC_TIMER_ONETIME;
    Cell2.RTCTimerInstance = Stop_Report_Onetime_Event;
    Cell2.settime = curtime + 5;

    Cell3.RTCTimerType = WEDGE_RTC_TIMER_PERIODIC;
    Cell3.RTCTimerInstance = Periodic_Health_Event;
    Cell3.settime = curtime + 2;

    RPTINTVL.ionint = 1;
    RPTINTVL.ioffint = 1;
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

    DebugLog("WedgeRtcTimerTest Start!");
    while (1)
    {
        WedgeRTCTimerEventProcess();
    }
}