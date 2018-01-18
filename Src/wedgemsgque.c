/*******************************************************************************
* File Name          : wedgemsgque.c
* Author             : Yangjie Gu
* Description        : This file provides all the wedgemsgque functions.

* History:
*  12/31/2017 : wedgemsgque V1.00
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "wedgemsgque.h"
#include "rtcclock.h"
#include "wedgeeventalertflow.h"
#include "wedgecommonapi.h"

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define WEDGE_MSGQUE_PRINT DebugPrintf

#define WEDGE_MSG_QUE_HEAD_SZIE_BYTES (sizeof(WEDGEMsgQueCellHeadTypeDef))

uint8_t WedgeMsgQueInit(void)
{
    MQSTATTypeDef MQSTAT = {0};
    uint16_t inindex = 0, outindex = 0, sent = 0, unsent = 0, delta = 0, numinsector = 0;
    uint32_t inaddr = 0, outaddr = 0, address = 0;
    WEDGEMsgQueCellTypeDef MsgQueCell = {0};
    uint16_t i = 0, empty = 0;
    char *WedgeMsgQueInitErrStr = "WEDGE MsgQueInit err";

    MQSTAT = *((MQSTATTypeDef *)WedgeSysStateGet(WEDGE_MQSTAT));
    inindex = MQSTAT.queinindex % WEDGE_MSG_QUE_TOTAL_NUM;
    outindex = MQSTAT.queoutindex % WEDGE_MSG_QUE_TOTAL_NUM;
    sent = MQSTAT.sent;
    unsent = MQSTAT.unsent;

    WEDGE_MSGQUE_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] WEDGE Msg Que Init Begin", FmtTimeShow());

    do
    {
        inaddr = WEDGE_MSG_QUE_START_ADDR + (inindex % WEDGE_MSG_QUE_TOTAL_NUM) * sizeof(WEDGEMsgQueCellTypeDef);

        if (0 != WedgeFlashReadData(inaddr, (uint8_t *)&MsgQueCell, WEDGE_MSG_QUE_HEAD_SZIE_BYTES))
        {
            WEDGE_MSGQUE_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] %s1",
                                      FmtTimeShow(), WedgeMsgQueInitErrStr);
            return 1;
        }

        if ((MsgQueCell.sentstate == WEDGE_MSG_QUE_UNSENT) && (MsgQueCell.type == WEDGE_MSG_QUE_EMPTY_TYPE))
        {
            delta =  (inaddr & WEDGE_STORAGE_SECTOR_ALIGN_MASK) / sizeof(WEDGEMsgQueCellTypeDef);

            for (i = 0; i < delta; i++)
            {
                address = WEDGE_MSG_QUE_START_ADDR + 
                (((inindex - 1) > WEDGE_MSG_QUE_TOTAL_NUM) ? WEDGE_MSG_QUE_TOTAL_NUM : (inindex - 1)) * sizeof(WEDGEMsgQueCellTypeDef);
                if (0 != WedgeFlashReadData(address, (uint8_t *)&MsgQueCell, WEDGE_MSG_QUE_HEAD_SZIE_BYTES))
                {
                    WEDGE_MSGQUE_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] %s2",
                                              FmtTimeShow(), WedgeMsgQueInitErrStr);
                    return 2;
                }

                if ((MsgQueCell.sentstate == WEDGE_MSG_QUE_UNSENT) && (MsgQueCell.type == WEDGE_MSG_QUE_EMPTY_TYPE))
                {
                    inindex--;
                }
                else
                {
                    break;
                }
            }
            
            break;
        }
        else if ((MsgQueCell.sentstate == WEDGE_MSG_QUE_UNSENT) && (MsgQueCell.type != WEDGE_MSG_QUE_EMPTY_TYPE))
        {
            inindex++;
            unsent++;

            if (unsent >= WEDGE_MSG_QUE_TOTAL_NUM)
            {
                MQSTAT.unsent = WEDGE_MSG_QUE_TOTAL_NUM;
                MQSTAT.sent = 0;
                MQSTAT.queinindex = (((inaddr + WEDGE_STORAGE_SECTOR_SIZE) & (~WEDGE_STORAGE_SECTOR_ALIGN_MASK)) / sizeof(WEDGEMsgQueCellTypeDef)) % WEDGE_MSG_QUE_TOTAL_NUM;
                MQSTAT.queoutindex = inindex + 1;
                WedgeSysStateSet(WEDGE_MQSTAT, &MQSTAT);

                return 0;
            }
        }
        else if (MsgQueCell.sentstate == WEDGE_MSG_QUE_SENT)
        {
            address = WEDGE_MSG_QUE_START_ADDR + 
            (((inindex - 1) > WEDGE_MSG_QUE_TOTAL_NUM) ? WEDGE_MSG_QUE_TOTAL_NUM : (inindex - 1)) * sizeof(WEDGEMsgQueCellTypeDef);
            if (0 != WedgeFlashReadData(address, (uint8_t *)&MsgQueCell, WEDGE_MSG_QUE_HEAD_SZIE_BYTES))
            {
                WEDGE_MSGQUE_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] %s3",
                                          FmtTimeShow(), WedgeMsgQueInitErrStr);
                return 3;
            }

            if ((MsgQueCell.sentstate == WEDGE_MSG_QUE_UNSENT) && (MsgQueCell.type != WEDGE_MSG_QUE_EMPTY_TYPE))
            {
                break;
            }
            else
            {
                inindex++;
                sent++;
            }

            if (sent >= WEDGE_MSG_QUE_TOTAL_NUM)
            {
                address &= (~WEDGE_STORAGE_SECTOR_ALIGN_MASK);
                inindex = (address - WEDGE_MSG_QUE_START_ADDR) / sizeof(WEDGEMsgQueCellTypeDef);
                MQSTAT.unsent = 0;
                MQSTAT.sent = WEDGE_MSG_QUE_TOTAL_NUM;
                MQSTAT.queinindex = inindex;
                MQSTAT.queoutindex = inindex;
                WedgeSysStateSet(WEDGE_MQSTAT, &MQSTAT);

                return 0;
            }
        }
        else
        {
            WEDGE_MSGQUE_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] %s4",
                                              FmtTimeShow(), WedgeMsgQueInitErrStr);
            return 4;
        }
    } while(1);

    inindex %= WEDGE_MSG_QUE_TOTAL_NUM;
    MQSTAT.queinindex = inindex;
    
    outindex = MQSTAT.queoutindex % WEDGE_MSG_QUE_TOTAL_NUM;
    sent = MQSTAT.sent;
    unsent = MQSTAT.unsent;

    do
    {
        outaddr = WEDGE_MSG_QUE_START_ADDR + (outindex % WEDGE_MSG_QUE_TOTAL_NUM) * sizeof(WEDGEMsgQueCellTypeDef);
        if (0 != WedgeFlashReadData(outaddr, (uint8_t *)&MsgQueCell, WEDGE_MSG_QUE_HEAD_SZIE_BYTES))
        {
            WEDGE_MSGQUE_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] %s5",
                                      FmtTimeShow(), WedgeMsgQueInitErrStr);
            return 5;
        }

        if ((MsgQueCell.sentstate == WEDGE_MSG_QUE_UNSENT) && (MsgQueCell.type == WEDGE_MSG_QUE_EMPTY_TYPE))
        {
            address = WEDGE_MSG_QUE_START_ADDR + 
            (((outindex - 1) > WEDGE_MSG_QUE_TOTAL_NUM) ? WEDGE_MSG_QUE_TOTAL_NUM : (outindex - 1)) * sizeof(WEDGEMsgQueCellTypeDef);
            if (0 != WedgeFlashReadData(address, (uint8_t *)&MsgQueCell, WEDGE_MSG_QUE_HEAD_SZIE_BYTES))
            {
                WEDGE_MSGQUE_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] %s6",
                                          FmtTimeShow(), WedgeMsgQueInitErrStr);
                return 6;
            }

            if (MsgQueCell.sentstate == WEDGE_MSG_QUE_SENT)
            {
                break;
            }
            else
            {
                empty++;
                outindex++;

                if (empty >= WEDGE_MSG_QUE_TOTAL_NUM)
                {
                    MQSTAT.unsent = 0;
                    MQSTAT.sent = 0;
                    MQSTAT.queoutindex = 0;
                    MQSTAT.queinindex = 0;

                    WedgeSysStateSet(WEDGE_MQSTAT, &MQSTAT);
                    return 0;
                }
            }
        }
        else if ((MsgQueCell.sentstate == WEDGE_MSG_QUE_UNSENT) && (MsgQueCell.type != WEDGE_MSG_QUE_EMPTY_TYPE))
        {
            address = WEDGE_MSG_QUE_START_ADDR + 
            (((outindex - 1) > WEDGE_MSG_QUE_TOTAL_NUM) ? WEDGE_MSG_QUE_TOTAL_NUM : (outindex - 1))  * sizeof(WEDGEMsgQueCellTypeDef);
            if (0 != WedgeFlashReadData(address, (uint8_t *)&MsgQueCell, WEDGE_MSG_QUE_HEAD_SZIE_BYTES))
            {
                WEDGE_MSGQUE_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] %s7",
                                          FmtTimeShow(), WedgeMsgQueInitErrStr);
                return 7;
            }

            if ((MsgQueCell.sentstate == WEDGE_MSG_QUE_UNSENT) && (MsgQueCell.type == WEDGE_MSG_QUE_EMPTY_TYPE))
            {
                break;
            }
            else
            {
                outindex++;
                unsent++;

                if (unsent >= WEDGE_MSG_QUE_TOTAL_NUM)
                {
                    outaddr &= (~WEDGE_STORAGE_SECTOR_ALIGN_MASK);
                    outindex = (outaddr - WEDGE_MSG_QUE_START_ADDR) / sizeof(WEDGEMsgQueCellTypeDef);
                    MQSTAT.unsent = WEDGE_MSG_QUE_TOTAL_NUM;
                    MQSTAT.sent = 0;
                    MQSTAT.queinindex = outindex;
                    MQSTAT.queoutindex = outindex;
                    WedgeSysStateSet(WEDGE_MQSTAT, &MQSTAT);

                    return 0;
                }
            }
        }
        else if (MsgQueCell.sentstate == WEDGE_MSG_QUE_SENT)
        {
            address = WEDGE_MSG_QUE_START_ADDR + ((outindex + 1) % WEDGE_MSG_QUE_TOTAL_NUM) * sizeof(WEDGEMsgQueCellTypeDef);
            if (0 != WedgeFlashReadData(address, (uint8_t *)&MsgQueCell, WEDGE_MSG_QUE_HEAD_SZIE_BYTES))
            {
                WEDGE_MSGQUE_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] %s8",
                                          FmtTimeShow(), WedgeMsgQueInitErrStr);
                return 8;
            }

            if ((MsgQueCell.sentstate == WEDGE_MSG_QUE_UNSENT) && (MsgQueCell.type != WEDGE_MSG_QUE_EMPTY_TYPE))
            {
                outindex++;
                break;
            }
            else
            {
                outindex++;
                sent++;
            }

            if (sent >= WEDGE_MSG_QUE_TOTAL_NUM)
            {
                outaddr &= (~WEDGE_STORAGE_SECTOR_ALIGN_MASK);
                outindex = (outaddr - WEDGE_MSG_QUE_START_ADDR) / sizeof(WEDGEMsgQueCellTypeDef);
                MQSTAT.unsent = 0;
                MQSTAT.sent = WEDGE_MSG_QUE_TOTAL_NUM;
                MQSTAT.queinindex = outindex;
                MQSTAT.queoutindex = outindex;
                WedgeSysStateSet(WEDGE_MQSTAT, &MQSTAT);

                return 0;
            }
        }
        else
        {
            WEDGE_MSGQUE_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] %s9",
                                              FmtTimeShow(), WedgeMsgQueInitErrStr);
            return 9;
        }

    } while(1);

    outindex %= WEDGE_MSG_QUE_TOTAL_NUM;
    MQSTAT.queoutindex = outindex;

    numinsector = (WEDGE_STORAGE_SECTOR_SIZE / sizeof(WEDGEMsgQueCellTypeDef));

    if (outindex <= inindex)
    {
        // this is not ok
        MQSTAT.unsent = inindex - outindex;
        MQSTAT.sent = outindex;

        WedgeSysStateSet(WEDGE_MQSTAT, &MQSTAT);
    }
    else
    {
        MQSTAT.sent = outindex - inindex - (numinsector - inindex % numinsector);
        MQSTAT.unsent = WEDGE_MSG_QUE_TOTAL_NUM - (outindex - inindex);

        WedgeSysStateSet(WEDGE_MQSTAT, &MQSTAT);
    }

    WEDGE_MSGQUE_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] WEDGE Msg Que Init End MQSTAT.sent(%d) MQSTAT.unsent(%d)", FmtTimeShow(), MQSTAT.sent, MQSTAT.unsent);


    return 0;
}

uint8_t WedgeMsgQueInWrite(WEDGEMsgQueCellTypeDef *pQueCell)
{
    MQSTATTypeDef MQSTAT = {0};
    uint16_t inindex = 0, outindex = 0, delta = 0, numinsector = 0;
    uint32_t address = 0;
    char *WedgeMsgQueInWriteErrStr = "WEDGE Flash QueIn err";

    if (pQueCell == NULL)
    {
        WEDGE_MSGQUE_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] %s Param",
                                  FmtTimeShow(), WedgeMsgQueInWriteErrStr);
        return 6;
    }

    MQSTAT = *((MQSTATTypeDef *)WedgeSysStateGet(WEDGE_MQSTAT));

    inindex = MQSTAT.queinindex % WEDGE_MSG_QUE_TOTAL_NUM;
    outindex = MQSTAT.queoutindex % WEDGE_MSG_QUE_TOTAL_NUM;
    address = WEDGE_MSG_QUE_START_ADDR + inindex * sizeof(*pQueCell);
    
    numinsector = (WEDGE_STORAGE_SECTOR_SIZE / sizeof(*pQueCell));

    if ((address % WEDGE_STORAGE_SECTOR_SIZE) == 0)
    {
        if (inindex <= outindex)
        {
            delta = outindex - inindex;
            if (delta < numinsector)
            {
                if (MQSTAT.unsent > numinsector)
                {
                    if (0 == WedgeFlashEraseSector(address))
                    {
                        MQSTAT.unsent -= (numinsector - delta);
                        MQSTAT.sent -= delta;
                        MQSTAT.queoutindex += (numinsector - delta);

                        WedgeSysStateSet(WEDGE_MQSTAT, &MQSTAT);
                    }
                    else
                    {
                        WEDGE_MSGQUE_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] %s1",
                                                  FmtTimeShow(), WedgeMsgQueInWriteErrStr);
                        return 1;
                    }
                }
                else
                {
                    if (0 == WedgeFlashEraseSector(address))
                    {
                        MQSTAT.unsent = 0;
                        MQSTAT.sent = 0;
                        MQSTAT.queoutindex = 0;
                        MQSTAT.queinindex = 0;

                        WedgeSysStateSet(WEDGE_MQSTAT, &MQSTAT);
                    }
                    else
                    {
                        WEDGE_MSGQUE_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] %s2",
                                                  FmtTimeShow(), WedgeMsgQueInWriteErrStr);
                        return 2;
                    }
                }
            }
            else
            {
                if (0 == WedgeFlashEraseSector(address))
                {
                    MQSTAT.sent -= numinsector;

                    WedgeSysStateSet(WEDGE_MQSTAT, &MQSTAT);
                }
                else
                {
                    WEDGE_MSGQUE_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] %s3",
                                              FmtTimeShow(), WedgeMsgQueInWriteErrStr);
                    return 3;
                }
            }
        }
        else
        {
            if (0 == WedgeFlashEraseSector(address))
            {
                if ((MQSTAT.sent + MQSTAT.unsent + numinsector - 1) >= WEDGE_MSG_QUE_TOTAL_NUM)
                {
                    MQSTAT.sent -= numinsector;
                }

                WedgeSysStateSet(WEDGE_MQSTAT, &MQSTAT);
            }
            else
            {
                WEDGE_MSGQUE_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] %s4",
                                          FmtTimeShow(), WedgeMsgQueInWriteErrStr);
                return 4;
            }
        }
    }

    if (0 != WedgeFlashWriteData(address, (uint8_t *)pQueCell, sizeof(*pQueCell)))
    {
        WEDGE_MSGQUE_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] %s5",
                                  FmtTimeShow(), WedgeMsgQueInWriteErrStr);
        return 5;
    }
    else
    {
        MQSTAT.unsent++;
        if (MQSTAT.unsent > WEDGE_MSG_QUE_TOTAL_NUM)
        {
            MQSTAT.unsent = WEDGE_MSG_QUE_TOTAL_NUM;
        }
        MQSTAT.queinindex++;
        MQSTAT.queinindex %= WEDGE_MSG_QUE_TOTAL_NUM;
        WedgeSysStateSet(WEDGE_MQSTAT, &MQSTAT);

        WEDGE_MSGQUE_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] WEDGE Flash QueIn ok",
                                  FmtTimeShow());
        return 0;
    }
}

uint8_t WedgeMsgQueOutRead(WEDGEMsgQueCellTypeDef *pQueCell)
{
    MQSTATTypeDef MQSTAT = {0};
    WEDGEMsgQueCellHeadTypeDef WEDGEMsgQueCellHead = {0};
    uint16_t outindex = 0, unsent = 0,sent = 0;
    uint32_t address = 0;
    char *WedgeMsgQueOutReadErrStr = "WEDGE MsgQueOut err";

    if (pQueCell == NULL)
    {
        WEDGE_MSGQUE_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] %s Param",
                                  FmtTimeShow(), WedgeMsgQueOutReadErrStr);
        return 1;
    }

    MQSTAT = *((MQSTATTypeDef *)WedgeSysStateGet(WEDGE_MQSTAT));
    unsent = MQSTAT.unsent;
    if (unsent == 0)
    {
        WEDGE_MSGQUE_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] WEDGE MsgQueOut unsent:0",
                                  FmtTimeShow());
        return 2;
    }
    
    sent = MQSTAT.sent;
    outindex = MQSTAT.queoutindex % WEDGE_MSG_QUE_TOTAL_NUM;
    address = WEDGE_MSG_QUE_START_ADDR + outindex * sizeof(WEDGEMsgQueCellTypeDef);

    if (0 != WedgeFlashReadData(address, (uint8_t *)pQueCell, sizeof(WEDGEMsgQueCellTypeDef)))
    {
        WEDGE_MSGQUE_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] %s3",
                                  FmtTimeShow(), WedgeMsgQueOutReadErrStr);
        return 3;
    }

    if ((pQueCell->type == WEDGE_MSG_QUE_UDP_TYPE) || (pQueCell->type == WEDGE_MSG_QUE_SMS_TYPE))
    {
        if (unsent != 0)
        {
            unsent--;
        }
        
        sent++;
        if (sent >= WEDGE_MSG_QUE_TOTAL_NUM)
        {
            sent = WEDGE_MSG_QUE_TOTAL_NUM;
        }

        outindex++;
        outindex %= WEDGE_MSG_QUE_TOTAL_NUM;

        MQSTAT.unsent = unsent;
        MQSTAT.sent = sent;
        MQSTAT.queoutindex = outindex;

        if (pQueCell->sentstate == WEDGE_MSG_QUE_UNSENT)
        {
            WEDGEMsgQueCellHead.sentstate = WEDGE_MSG_QUE_SENT;
            WEDGEMsgQueCellHead.type = pQueCell->type;

            if (0 != WedgeFlashWriteData(address, (uint8_t *)&WEDGEMsgQueCellHead, sizeof(WEDGEMsgQueCellHead.sentstate)))
            {
                WEDGE_MSGQUE_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] %s4",
                                          FmtTimeShow(), WedgeMsgQueOutReadErrStr);
                return 4;
            }

            WedgeSysStateSet(WEDGE_MQSTAT, &MQSTAT);

            WEDGE_MSGQUE_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] WEDGE MsgQueOut ok",
                                      FmtTimeShow());
            return 0;
        }
        else if (pQueCell->sentstate == WEDGE_MSG_QUE_SENT)
        {
            if (MQSTAT.sent >= WEDGE_MSG_QUE_TOTAL_NUM)
            {
                outindex--;
                MQSTAT.sent = WEDGE_MSG_QUE_TOTAL_NUM;
                MQSTAT.queoutindex = outindex % WEDGE_MSG_QUE_TOTAL_NUM;
            }
            else
            {
                MQSTAT.unsent++;
                if ((MQSTAT.unsent + MQSTAT.sent) >= WEDGE_MSG_QUE_TOTAL_NUM)
                {
                    MQSTAT.unsent = WEDGE_MSG_QUE_TOTAL_NUM - MQSTAT.sent;
                }
            }
            
            WedgeSysStateSet(WEDGE_MQSTAT, &MQSTAT);

            WEDGE_MSGQUE_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] %s5",
                                      FmtTimeShow(), WedgeMsgQueOutReadErrStr);
            return 5;
        }
        else
        {
            MQSTAT.sent--;
            MQSTAT.queoutindex = MQSTAT.queinindex % WEDGE_MSG_QUE_TOTAL_NUM;
            WedgeSysStateSet(WEDGE_MQSTAT, &MQSTAT);

            WEDGE_MSGQUE_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] %s6",
                                      FmtTimeShow(), WedgeMsgQueOutReadErrStr);
            return 6;
        }
    }
    else/* if ((pQueCell->sentstate == WEDGE_MSG_QUE_UNSENT) && (pQueCell->type == WEDGE_MSG_QUE_EMPTY_TYPE)) && else*/
    {
        if (unsent != 0)
        {
            unsent--;
        }

        MQSTAT.unsent = unsent;
        MQSTAT.queoutindex = MQSTAT.queinindex % WEDGE_MSG_QUE_TOTAL_NUM;
        WedgeSysStateSet(WEDGE_MQSTAT, &MQSTAT);

        WEDGE_MSGQUE_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] %s7",
                                      FmtTimeShow(), WedgeMsgQueOutReadErrStr);
        return 7;
    }
}

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2018. All rights reserved
                                End Of The File
*******************************************************************************/
