#include "wedgetest.h"

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

#if WEDGE_DEVICE_INFO_TEST
		extern void WedgeDeviceInfoTest(void);
    WedgeDeviceInfoTest();
#endif
}
