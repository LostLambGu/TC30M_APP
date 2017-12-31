#include "wedgetest.h"

#define WEDGE_MSG_QUE_TEST (1)

void WedgeTest(void)
{
#if WEDGE_MSG_QUE_TEST
    SFlashMsgQueInitTest();
#endif
}
