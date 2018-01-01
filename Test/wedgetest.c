#include "wedgetest.h"

#define WEDGE_MSG_QUE_TEST (0)
#define WEDGE_RTC_TIMER_TEST (0)
#define WEDGE_POWER_LOST_TEST (1)

void WedgeTest(void)
{
#if WEDGE_MSG_QUE_TEST
    SFlashMsgQueInitTest();
#endif

#if WEDGE_RTC_TIMER_TEST
    WedgeRtcTimerTest();
#endif

#if WEDGE_POWER_LOST_TEST
    WedgePowerLostTest();
#endif
}
