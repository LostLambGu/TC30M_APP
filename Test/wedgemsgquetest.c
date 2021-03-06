#include "wedgetest.h"
#include "wedgemsgque.h"

#if WEDGE_MSG_QUE_TEST

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
    for (i = 0; i < WEDGE_MSG_QUE_TOTAL_NUM; i++)
    {
        if (0 == WedgeMsgQueInWrite(&QueCell))
        {
            // DebugLog("WedgeMsgQueInWrite OK");
        }

        // MQSTAT = *((MQSTATTypeDef *)WedgeSysStateGet(WEDGE_MQSTAT));
        // DebugLog("i: %d, MQSTAT.unsent: %d, MQSTAT.sent :%d, MQSTAT.queinindex: %d, MQSTAT.queoutindex: %d"
        // , i, MQSTAT.unsent, MQSTAT.sent, MQSTAT.queinindex, MQSTAT.queoutindex);
    }

#if 1
    for (i = 0; i < 64; i++)
    {
        if (0 == WedgeMsgQueOutRead(&QueCell))
        {
            DebugLog("WedgeMsgQueOutRead OK");
        }

        MQSTAT = *((MQSTATTypeDef *)WedgeSysStateGet(WEDGE_MQSTAT));
        DebugLog("i: %d, MQSTAT.unsent: %d, MQSTAT.sent :%d, MQSTAT.queinindex: %d, MQSTAT.queoutindex: %d"
        , i, MQSTAT.unsent, MQSTAT.sent, MQSTAT.queinindex, MQSTAT.queoutindex);
    }
#else
    MQSTAT.unsent = 0;
    MQSTAT.sent = 0;
    MQSTAT.queinindex = 0;
    MQSTAT.queoutindex = 0;
    WedgeSysStateSet(WEDGE_MQSTAT, &MQSTAT);
    QueCell.sentstate = WEDGE_MSG_QUE_SENT;
    QueCell.type = WEDGE_MSG_QUE_UDP_TYPE;

    for (i = 0; i < 64; i++)
    {
        if (0 == WedgeMsgQueInWrite(&QueCell))
        {
            // DebugLog("WedgeMsgQueInWrite OK");
        }

        // MQSTAT = *((MQSTATTypeDef *)WedgeSysStateGet(WEDGE_MQSTAT));
        // DebugLog("i: %d, MQSTAT.unsent: %d, MQSTAT.sent :%d, MQSTAT.queinindex: %d, MQSTAT.queoutindex: %d"
        // , i, MQSTAT.unsent, MQSTAT.sent, MQSTAT.queinindex, MQSTAT.queoutindex);
    }
#endif

    MQSTAT.unsent = 0;
    MQSTAT.sent = 0;
    MQSTAT.queinindex = 0;
    MQSTAT.queoutindex = 0;
    WedgeSysStateSet(WEDGE_MQSTAT, &MQSTAT);
    QueCell.sentstate = WEDGE_MSG_QUE_UNSENT;
    QueCell.type = WEDGE_MSG_QUE_UDP_TYPE;
    for (i = 0; i < 10; i++)
    {
        if (0 == WedgeMsgQueInWrite(&QueCell))
        {
            // DebugLog("WedgeMsgQueInWrite OK");
        }

        // MQSTAT = *((MQSTATTypeDef *)WedgeSysStateGet(WEDGE_MQSTAT));
        // DebugLog("i: %d, MQSTAT.unsent: %d, MQSTAT.sent :%d, MQSTAT.queinindex: %d, MQSTAT.queoutindex: %d"
        // , i, MQSTAT.unsent, MQSTAT.sent, MQSTAT.queinindex, MQSTAT.queoutindex);
    }

    // QueCell.sentstate = WEDGE_MSG_QUE_SENT;
    // QueCell.type = WEDGE_MSG_QUE_UDP_TYPE;
    // for (i = 0; i < 64; i++)
    // {
    //     if (0 == WedgeMsgQueInWrite(&QueCell))
    //     {
    //         DebugLog("WedgeMsgQueInWrite OK");
    //     }

    //     MQSTAT = *((MQSTATTypeDef *)WedgeSysStateGet(WEDGE_MQSTAT));
    //     DebugLog("i: %d, MQSTAT.unsent: %d, MQSTAT.sent :%d, MQSTAT.queinindex: %d, MQSTAT.queoutindex: %d"
    //     , i, MQSTAT.unsent, MQSTAT.sent, MQSTAT.queinindex, MQSTAT.queoutindex);
    // }
    

    for (i = 0, j = 0, k = 0; i < 10; i++)
    {
        MQSTAT.unsent = i;
        MQSTAT.sent = 0;
        j += 8;
        MQSTAT.queinindex = j % 128;
        k += 6;
        MQSTAT.queoutindex = k++;
        WedgeSysStateSet(WEDGE_MQSTAT, &MQSTAT);

        WedgeMsgQueInit();

        MQSTAT = *((MQSTATTypeDef *)WedgeSysStateGet(WEDGE_MQSTAT));
        DebugLog("i: %d, MQSTAT.unsent: %d, MQSTAT.sent :%d, MQSTAT.queinindex: %d, MQSTAT.queoutindex: %d"
        , i, MQSTAT.unsent, MQSTAT.sent, MQSTAT.queinindex, MQSTAT.queoutindex);
    }
}

#endif
