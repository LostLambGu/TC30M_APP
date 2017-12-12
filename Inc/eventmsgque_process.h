/*******************************************************************************
* File Name          : eventmsgque_process.h
* Author             : Yangjie Gu
* Description        : This file provides all the eventmsgque_process functions.

* History:
*  10/28/2017 : eventmsgque_process V1.00
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EVENT_MSG_QUEUE_PROCESS_H__
#define __EVENT_MSG_QUEUE_PROCESS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
	
#include "uart_api.h"

#include "network.h"

/* Defines ------------------------------------------------------------------*/

#define OEMMSG_QUEUE_LENGTH (128)
#define OEMMSG_QUEUE_SIZE (2)

#define OEMMSG_QUEUE_SEND_WAIT_TIME (50)  // ms

#define QUEUE_PROCESS_LOG DebugLog

extern void OemMsgQueUdpRec(void *MsgBufferP, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif /* __EVENT_MSG_QUEUE_PROCESS_H__ */

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2017. All rights reserved
                                End Of The File
*******************************************************************************/
