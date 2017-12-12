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
TimeTableT heartbeattimeTable = {0};

uint8_t UdpSocketOpenIndicateFlag = 0;
uint8_t UdpSocketCloseIndicateFlag = 0;
uint8_t UdpSocketListenIndicateFlag = 0;
UDPIPSocketTypedef UDPIPSocket[UDPIP_SOCKET_MAX_NUM] = {0};

UdpSendQueueTypedef UdpSendQueue = {0};
SmsSendQueueTypedef SmsSendQueue = {0};
SmsReceiveBufTypedef SmsReceiveBuf = {0};
uint8_t SmsRecFlag = 0;

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

void OemMsgQueAdcRead(uint8_t channel)
{
    uint16_t adcrawvalue = 0;
    adcrawvalue = ADC_ReadRaw(channel);

    OemMsgHandle(OEM_ADC_READ_RSP, &adcrawvalue, sizeof(adcrawvalue));
}

void OemMsgQueUdpSocketOpen(uint8_t socketnum)
{
    UdpSocketOpenIndicateFlag |= (0x01 << (socketnum - 1));

    if (GetNetworkMachineStatus() == NET_CONNECTED_STAT)
    {
        // Reset Network Timer
        SoftwareTimerReset(&NetworkCheckTimer, CheckNetlorkTimerCallback, 10);
        SoftwareTimerStart(&NetworkCheckTimer);
    }
}

void OemMsgQueUdpClose(uint8_t socketnum)
{
    UdpSocketCloseIndicateFlag |= (0x01 << (socketnum - 1));
}

void OemMsgQueUdpConnectStatus(uint8_t socketnum, uint8_t status)
{
    uint8_t data[2] = {socketnum, status};
    OemMsgHandle(OEM_PPP_UDP_STATUS_MSG, data, sizeof(data));
}

void OemMsgQueUdpSend(void)
{
    if (GetNetworkMachineStatus() == NET_CONNECTED_STAT)
    {
        // Reset Network Timer
        SoftwareTimerReset(&NetworkCheckTimer, CheckNetlorkTimerCallback, 10);
        SoftwareTimerStart(&NetworkCheckTimer);
    }
}

void OemMsgQueUdpRec(void *MsgBufferP, uint32_t size)
{
    OemMsgHandle(OEM_UDP_RECEIVE_DATA, MsgBufferP, size);
}


void UdpSendUnitIn(UdpSendQueueTypedef *pUdpSendQueue, UdpSendUintTypedef *pUDPIpSendUint)
{
    if ((pUdpSendQueue == NULL) || (pUDPIpSendUint == NULL))
    {
        QUEUE_PROCESS_LOG("UdpSendUnitIn param err");
        return;
    }

    {
        taskENTER_CRITICAL();
        UdpSendUintTypedef *ptemp = &(pUdpSendQueue->UDPIpSendUint[pUdpSendQueue->putindex % UDP_SEND_QUEUE_LENGHT_MAX]);
        ptemp->datalen = pUDPIpSendUint->datalen;
        ptemp->socketnum = pUDPIpSendUint->socketnum;
        ptemp->buf = pUDPIpSendUint->buf;

        pUdpSendQueue->putindex++;
        pUdpSendQueue->numinqueue++;
        if (pUdpSendQueue->numinqueue > UDP_SEND_QUEUE_LENGHT_MAX)
        {
            pUdpSendQueue->numinqueue = UDP_SEND_QUEUE_LENGHT_MAX;
        }
        taskEXIT_CRITICAL();
    }
}

void UdpSendUintOut(UdpSendQueueTypedef *pUdpSendQueue, UdpSendUintTypedef *pUDPIpSendUint)
{
    if ((pUdpSendQueue == NULL) || (pUDPIpSendUint == NULL))
    {
        QUEUE_PROCESS_LOG("UdpSendUintOut param err");
        return;
    }

    if (pUdpSendQueue->numinqueue > 0)
    {
        taskENTER_CRITICAL();
        UdpSendUintTypedef *ptemp = &(pUdpSendQueue->UDPIpSendUint[pUdpSendQueue->getindex % UDP_SEND_QUEUE_LENGHT_MAX]);
        pUDPIpSendUint->datalen = ptemp->datalen;
        pUDPIpSendUint->socketnum = ptemp->socketnum;
        pUDPIpSendUint->buf = ptemp->buf;

        pUdpSendQueue->getindex++;
        pUdpSendQueue->numinqueue--;
        if (pUdpSendQueue->numinqueue > UDP_SEND_QUEUE_LENGHT_MAX)
        {
            pUdpSendQueue->numinqueue = 0;
        }
        taskEXIT_CRITICAL();
    }

}

void SmsSendUnitIn(SmsSendQueueTypedef *pSmsSendQueue, SmsSendUintTypedef *pSMSSendUint)
{
    if ((pSmsSendQueue == NULL) || (pSMSSendUint == NULL))
    {
        QUEUE_PROCESS_LOG("SmsSendUnitIn param err");
        return;
    }

    {
        taskENTER_CRITICAL();
        SmsSendUintTypedef *ptemp = &(pSmsSendQueue->SMSSendUint[pSmsSendQueue->putindex % SMS_SEND_QUEUE_LENGHT_MAX]);
        ptemp->number = pSMSSendUint->number;
        ptemp->buf = pSMSSendUint->buf;

        pSmsSendQueue->putindex++;
        pSmsSendQueue->numinqueue++;
        if (pSmsSendQueue->numinqueue > SMS_SEND_QUEUE_LENGHT_MAX)
        {
            pSmsSendQueue->numinqueue = SMS_SEND_QUEUE_LENGHT_MAX;
        }
        taskEXIT_CRITICAL();
    }
}

void SmsSendUintOut(SmsSendQueueTypedef *pSmsSendQueue, SmsSendUintTypedef *pSMSSendUint)
{
    if ((pSmsSendQueue == NULL) || (pSMSSendUint == NULL))
    {
        QUEUE_PROCESS_LOG("SmsSendUintOut param err");
        return;
    }

    if (pSmsSendQueue->numinqueue > 0)
    {
        taskENTER_CRITICAL();
        SmsSendUintTypedef *ptemp = &(pSmsSendQueue->SMSSendUint[pSmsSendQueue->getindex % SMS_SEND_QUEUE_LENGHT_MAX]);
        pSMSSendUint->number = ptemp->number;
        pSMSSendUint->buf = ptemp->buf;

        pSmsSendQueue->getindex++;
        pSmsSendQueue->numinqueue--;
        if (pSmsSendQueue->numinqueue > SMS_SEND_QUEUE_LENGHT_MAX)
        {
            pSmsSendQueue->numinqueue = 0;
        }
        taskEXIT_CRITICAL();
    }
}

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2017. All rights reserved
                                End Of The File
*******************************************************************************/
