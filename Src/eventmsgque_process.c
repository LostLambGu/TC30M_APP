/*******************************************************************************
* File Name          : eventmsgque_process.c
* Author             : Yangjie Gu
* Description        : This file provides all the eventmsgque_process functions.

* History:
*  10/28/2017 : eventmsgque_process V1.00
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "eventmsgque_process.h"
#include "rtcclock.h"
#include "usrtimer.h"

/* Variables -----------------------------------------------------------------*/

// UdpRecUintTypedef UdpRecUint = {0};

// uint8_t UdpRecBuf[UART3_RX_BUFFER_SIZE] = {0};

extern uint16_t ADC_ReadRaw(uint16_t adc);
extern void ATCmdDetection(void);

/* Function definition -------------------------------------------------------*/
void OemMsgQueSmsSend(void)
{
    if (GetNetworkMachineStatus() == NET_CONNECTED_STAT)
    {
        // Reset Network Timer
        SoftwareTimerReset(&NetworkCheckTimer, CheckNetlorkTimerCallback, 10);
        SoftwareTimerStart(&NetworkCheckTimer);
    }
}

void OemMsgQueSmsDeleteAll(void)
{
    SendATCmd(GSM_CMD_CMGD, GSM_CMD_TYPE_EVALUATE , "1,4");
}

void OemMsgSmsReceived(void *MsgBufferP, uint32_t size)
{
    OemMsgHandle(OEM_SMS_REV_MSG, MsgBufferP, size);
}

void OemMsgQueUdpConnectStatus(uint8_t socketnum, uint8_t status)
{
    uint8_t data[2] = {socketnum, status};
    OemMsgHandle(OEM_PPP_UDP_STATUS_MSG, data, sizeof(data));
}


void OemMsgQueUdpRec(void *MsgBufferP, uint32_t size)
{
    OemMsgHandle(OEM_UDP_RECEIVE_DATA, MsgBufferP, size);
}




/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2017. All rights reserved
                                End Of The File
*******************************************************************************/
