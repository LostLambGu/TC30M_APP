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
extern void OemNmeaDataProcess(uint8_t *MsgBufferP, uint32_t size);

#define UDPIP_SOCKET_MIN_NUM (1)
#define UDPIP_SOCKET_MAX_NUM (5)
#define UDP_SEND_QUEUE_LENGHT_MAX (8)
#define SMS_SEND_QUEUE_LENGHT_MAX (8)

typedef enum
{
    SOCKETCLOSE = 0,
    SOCKETOPEN = 1,
    SOCKETOPEN_REPORTONCE = 2,
} SocketOperationTypedef;

typedef enum
{
    SOCKET_CLOSE = 0,
} SocketStatusTypedef;

typedef struct
{
  uint8_t operation;
  uint8_t status;
  uint16_t LocalPort;
  uint16_t PortNum;
  char DestAddrP[MAX_IP_ADDR_LEN];
} UDPIPSocketTypedef;

extern uint8_t UdpSocketOpenIndicateFlag;
extern uint8_t UdpSocketCloseIndicateFlag;
extern uint8_t UdpSocketListenIndicateFlag;
extern UDPIPSocketTypedef UDPIPSocket[UDPIP_SOCKET_MAX_NUM];

typedef struct
{
    uint16_t datalen;
    uint8_t socketnum;
    char *buf;
} UdpSendUintTypedef;

typedef struct
{
    uint8_t putindex;
    uint8_t getindex;
    uint8_t numinqueue;
    UdpSendUintTypedef UDPIpSendUint[UDP_SEND_QUEUE_LENGHT_MAX];
} UdpSendQueueTypedef;

extern UdpSendQueueTypedef UdpSendQueue;

// typedef struct
// {
//     uint16_t datalen;
//     uint8_t socketnum;
// } UdpRecUintTypedef;

// extern UdpRecUintTypedef UdpRecUint;

// // extern uint8_t UdpRecBuf[UART3_RX_BUFFER_SIZE];

extern void UdpSendUnitIn(UdpSendQueueTypedef *pUdpSendQueue, UdpSendUintTypedef *pUDPIpSendUint);

extern void UdpSendUintOut(UdpSendQueueTypedef *pUdpSendQueue, UdpSendUintTypedef *pUDPIpSendUint);

typedef struct
{
    char *number;
    char *buf;
} SmsSendUintTypedef;

typedef struct
{
    uint8_t putindex;
    uint8_t getindex;
    uint8_t numinqueue;
    SmsSendUintTypedef SMSSendUint[SMS_SEND_QUEUE_LENGHT_MAX];
} SmsSendQueueTypedef;

extern SmsSendQueueTypedef SmsSendQueue;

extern void SmsSendUnitIn(SmsSendQueueTypedef *pSmsSendQueue, SmsSendUintTypedef *pSMSSendUint);

extern void SmsSendUintOut(SmsSendQueueTypedef *pSmsSendQueue, SmsSendUintTypedef *pSMSSendUint);

#define SMS_RECEIVE_SMS_STAT_MAX_LEN (32)
#define SMS_RECEIVE_NUMBER_MAX_LEN (32)
#define SMS_RECEIVE_DATE_TIME_MAX_LEN (32)
#define SMS_RECEIVE_TEXT_DATA_MAX_LEN (128)
typedef struct
{
    char smsstat[SMS_RECEIVE_SMS_STAT_MAX_LEN];
    char smsnumber[SMS_RECEIVE_NUMBER_MAX_LEN];
    char smsdatetime[SMS_RECEIVE_DATE_TIME_MAX_LEN];
    char smstextdata[SMS_RECEIVE_TEXT_DATA_MAX_LEN];
} SmsReceiveBufTypedef;

extern SmsReceiveBufTypedef SmsReceiveBuf;

extern uint8_t SmsRecFlag;

#ifdef __cplusplus
}
#endif

#endif /* __EVENT_MSG_QUEUE_PROCESS_H__ */

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2017. All rights reserved
                                End Of The File
*******************************************************************************/
