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
#include "network.h"

/* Variables -----------------------------------------------------------------*/
BinaryMsgFormatTypeDef BinaryMsgRec = {0};
BinaryMsgFormatTypeDef BinaryMsgRecord = {0};
AsciiMsgFormatTypedDef AsciiMsgRecord = {0};
WEDGECfgTypeDef WEDGECfgRecord = {0};
WEDGECfgStateTypedef WEDGECfgState = {0};

/* Function definition -------------------------------------------------------*/

void SmsReceivedHandle(void *MsgBufferP, uint32_t size)
{
    
}

void UdpReceivedHandle(void *MsgBufferP, uint32_t size)
{
    
}

void WedgeCfgStateSet(WEDGECfgChangeTypeDef CfgChg, uint8_t State)
{

}

uint8_t WedgeCfgStateGet(void)
{
	return 0;
}

void WedgeCfgStateProcess(void)
{
    
}

void WedgeResponseUdpBinary(WEDGEPYLDTypeDef PYLDType, WEDGEEVIDTypeDef EvID)
{

}

void WedgeResponseUdpAscii(WEDGEPYLDTypeDef PYLDType, void *MsgBufferP, uint32_t size)
{

}

void WedgeResponseSms(WEDGEPYLDTypeDef PYLDType, void *MsgBufferP, uint32_t size)
{

}

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2017. All rights reserved
                                End Of The File
*******************************************************************************/
