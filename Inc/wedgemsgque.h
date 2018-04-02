/*******************************************************************************
* File Name          : wedgemsgque.h
* Author             : Yangjie Gu
* Description        : This file provides all the wedgemsgque functions.

* History:
*  12/31/2017 : wedgemsgque V1.00
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef WEDGE_MSG_QUE_H_
#define WEDGE_MSG_QUE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "uart_api.h"
#include "wedgedatadefine.h"

/* Defines ------------------------------------------------------------------*/
enum
{
    WEDGE_MSG_QUE_SENT = 0xBB,
    WEDGE_MSG_QUE_UNSENT = 0xFF
};

enum
{
    WEDGE_MSG_QUE_UDP_TYPE = 0xCC,
    WEDGE_MSG_QUE_SMS_TYPE = 0x33,
    WEDGE_MSG_QUE_EMPTY_TYPE = 0xFF
};

typedef struct
{
    uint8_t sentstate;
    uint8_t type;
} WEDGEMsgQueCellHeadTypeDef;
typedef struct
{
    uint8_t sentstate;
    uint8_t type;
    uint16_t size;
    uint8_t data[WEDGE_MSG_QUE_DATA_LEN_MAX];
} WEDGEMsgQueCellTypeDef; //The total size should be n * 256

extern uint8_t WedgeMsgQueInit(void);
extern uint8_t WedgeMsgQueInWrite(WEDGEMsgQueCellTypeDef *pQueCell);
extern uint8_t WedgeMsgQueOutRead(WEDGEMsgQueCellTypeDef *pQueCell);
extern void WedgeMsgQueClearAll(void);

#ifdef __cplusplus
}
#endif

#endif /* WEDGE_MSG_QUE_H_ */

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2018. All rights reserved
                                End Of The File
*******************************************************************************/
