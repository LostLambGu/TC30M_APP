#include "wedgetest.h"

void SFlashMsgQueInitTest(void)
{
    MQSTATTypeDef MQSTAT = {0};
    uint16_t i = 0, j = 0, k = 0;
    WEDGEMsgQueCellTypeDef QueCell;

    SerialFlashInit();

    // DebugLog("SFlashMsgQueInitTest");
    // if (SerialFlashErase(FLASH_ERASE_ALL, 0) == 0)
    // {
    //     DebugLog("SerialFlashErase OK");
    // }
    // else
    // {
    //     DebugLog("SerialFlashErase Err");
    // }

    
    QueCell.sentstate = WEDGE_MSG_QUE_UNSENT;
    QueCell.type = WEDGE_MSG_QUE_UDP_TYPE;

    for (i = 0; i < 18; i++)
    {
        if (0 == WedgeMsgQueInWrite(&QueCell))
        {
            DebugLog("WedgeMsgQueInWrite OK");
        }

        MQSTAT = *((MQSTATTypeDef *)WedgeSysStateGet(WEDGE_MQSTAT));
        DebugLog("i: %d, MQSTAT.unsent: %d, MQSTAT.sent :%d, MQSTAT.queinindex: %d, MQSTAT.queoutindex: %d"
        , i, MQSTAT.unsent, MQSTAT.sent, MQSTAT.queinindex, MQSTAT.queoutindex);
    }
    

    for (i = 0, j = 0, k = 0; i < 10; i++)
    {
        MQSTAT.unsent = i;
        MQSTAT.sent = 0;
        MQSTAT.queinindex = j++;
        MQSTAT.queoutindex = k++;
        WedgeSysStateSet(WEDGE_MQSTAT, &MQSTAT);

        WedgeMsgQueInit();

        MQSTAT = *((MQSTATTypeDef *)WedgeSysStateGet(WEDGE_MQSTAT));
        DebugLog("i: %d, MQSTAT.unsent: %d, MQSTAT.sent :%d, MQSTAT.queinindex: %d, MQSTAT.queoutindex: %d"
        , i, MQSTAT.unsent, MQSTAT.sent, MQSTAT.queinindex, MQSTAT.queoutindex);
    }
}

void WedgeTest(void)
{
    SFlashMsgQueInitTest();
}