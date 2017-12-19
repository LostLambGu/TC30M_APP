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
#include "uart_api.h"

#include "wedgedatadefine.h"

/* Defines ------------------------------------------------------------------*/
#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define EVENTMSGQUE_PROCESS_LOG DebugLog

#define EVENTMSGQUE_PROCESS_PRINT DebugPrintf

typedef enum
{
    SMS_ADDR_CFG_CHG = 0,
    SVRCFG_CFG_CHG,
    FTPCFG_CFG_CHG,
    APNCFG_CFG_CHG,
    HWRST_CFG_CHG,
    PWRMGT_CFG_CHG,
    USRDAT_CFG_CHG,
    CFGALL_CFG_CHG,
    RESET_DEFAULT_CFG_CHG,
    IGNTYP_CFG_CHG,
    RPTINTVL_CFG_CHG,
    ALARM1_CFG_CHG,
    ALARM2_CFG_CHG,
    LVA_CFG_CHG,
    IDLE_CFG_CHG,
    SODO_CFG_CHG,
    DIRCHG_CFG_CHG,
    TOW_CFG_CHG,
    STPINTVL_CFG_CHG,
    VODO_CFG_CHG,
    GEOFENCES1_CFG_CHG,
    GEOFENCES2_CFG_CHG,
    GEOFENCES3_CFG_CHG,
    GEOFENCES4_CFG_CHG,
    GEOFENCES5_CFG_CHG,
    GEOFENCES6_CFG_CHG,
    GEOFENCES7_CFG_CHG,
    GEOFENCES8_CFG_CHG,
    GEOFENCES9_CFG_CHG,
    GEOFENCES10_CFG_CHG,
    RELAY_CFG_CHG,
    PLSRLY_CFG_CHG,
    OPSOD_CFG_CHG,
    CFG_CHG_INVALIAD_MAX
} WEDGECfgChangeTypeDef;

#define WEDGE_CFG_STATE_NUM_MAX (39)
typedef struct
{
    uint8_t CfgChgNum;
    uint8_t CfgChgState[WEDGE_CFG_STATE_NUM_MAX];
} WEDGECfgStateTypedef;

extern BinaryMsgFormatTypeDef BinaryMsgRecord;
extern AsciiMsgFormatTypedDef AsciiMsgRecord;
extern WEDGECfgTypeDef WEDGECfgRecord;
extern WEDGECfgStateTypedef WEDGECfgState;

extern void SmsReceivedHandle(void *MsgBufferP, uint32_t size);
extern void UdpReceivedHandle(void *MsgBufferP, uint32_t size);

extern void WedgeCfgStateSet(WEDGECfgChangeTypeDef CfgChg, uint8_t State);
extern uint8_t WedgeCfgStateGet(void);
extern void WedgeCfgStateProcess(void);

extern void WedgeResponseUdpBinary(WEDGEPYLDTypeDef PYLDType, WEDGEEVIDTypeDef EvID);
extern void WedgeResponseAscii(WEDGEPYLDTypeDef PYLDType, void *MsgBufferP, uint32_t size);
extern void WedgeResponseSms(WEDGEPYLDTypeDef PYLDType, void *MsgBufferP, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif /* __EVENT_MSG_QUEUE_PROCESS_H__ */

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2017. All rights reserved
                                End Of The File
*******************************************************************************/
