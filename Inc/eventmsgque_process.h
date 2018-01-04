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

extern void SmsReceivedHandle(void *MsgBufferP, uint32_t size);
extern void UdpReceivedHandle(void *MsgBufferP, uint32_t size);

extern void WedgeUpdateBinaryMsgGpsRecord(void);
extern void WedgeResponseUdpBinary(WEDGEPYLDTypeDef PYLDType, WEDGEEVIDTypeDef EvID);
extern void WedgeResponseUdpAscii(WEDGEPYLDTypeDef PYLDType, void *MsgBufferP, uint32_t size);
extern void WedgeResponseSms(WEDGEPYLDTypeDef PYLDType, void *MsgBufferP, uint32_t size);

typedef enum
{
    WEDGE_CFG_IGNTYPE = 0,
    WEDGE_CFG_RPTINTVL,
    WEDGE_CFG_IOALRM1,
    WEDGE_CFG_IOALRM2,
    WEDGE_CFG_LVA,
    WEDGE_CFG_IDLE,
    WEDGE_CFG_SODO,
    WEDGE_CFG_VODO,
    WEDGE_CFG_DIRCHG,
    WEDGE_CFG_TOW,
    WEDGE_CFG_STPINTVL,
    WEDGE_CFG_GFNC1,
    WEDGE_CFG_GFNC2,
    WEDGE_CFG_GFNC3,
    WEDGE_CFG_GFNC4,
    WEDGE_CFG_GFNC5,
    WEDGE_CFG_GFNC6,
    WEDGE_CFG_GFNC7,
    WEDGE_CFG_GFNC8,
    WEDGE_CFG_GFNC9,
    WEDGE_CFG_GFNC10,
    WEDGE_CFG_RELAY,
    WEDGE_CFG_OSPD,
    WEDGE_CFG_PLSRLY,
    WEDGE_CFG_PWRMGT,
    WEDGE_CFG_HWRST,
    WEDGE_CFG_USRDAT,
    WEDGE_CFG_SMS,
    WEDGE_CFG_SVRCFG_ALL,
    WEDGE_CFG_SVRCFG_1,
    WEDGE_CFG_SVRCFG_2,
    WEDGE_CFG_SVRCFG_3
    WEDGE_CFG_SVRCFG_4,
    WEDGE_CFG_SVRCFG_5,
    WEDGE_CFG_FTPCFG,
    WEDGE_CFG_APNCFG,

    WEDGE_CFG_OPERATE_INVALID_MAX
} WEDGECfgOperateTypeDef;

extern uint8_t WedgeCfgInit(WEDGECfgTypeDef *pWEDGECfg);
extern void WedgeCfgGetTotal(uint8_t *pBuf, uint32_t *pSize);
extern void *WedgeCfgGet(WEDGECfgOperateTypeDef CfgGet);
extern void WedgeCfgSet(WEDGECfgOperateTypeDef CfgSet, void *pvData);

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

typedef struct
{
    uint8_t CfgChgNum;
    uint8_t CfgChgState[CFG_CHG_INVALIAD_MAX];
} WEDGECfgChgStateTypedef;

extern void WedgeCfgChgStateInit(void);
extern void WedgeCfgChgStateSet(WEDGECfgChangeTypeDef CfgChg, uint8_t State);
extern uint8_t WedgeCfgChgStateIsChanged(void);
extern uint8_t WedgeCfgChgStateGet(WEDGECfgChangeTypeDef CfgChg);
extern void WedgeCfgChgStateProcess(void);

extern uint8_t WedgeFlashChipErase(void);
extern uint8_t WedgeFlashEraseSector(uint32_t address);
extern uint8_t WedgeFlashReadData(uint32_t address, uint8_t *pDataBuf, uint32_t datalen);
extern uint8_t WedgeFlashWriteData(uint32_t address, uint8_t *pDataBuf, uint32_t datalen);

#ifdef __cplusplus
}
#endif

#endif /* __EVENT_MSG_QUEUE_PROCESS_H__ */

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2018. All rights reserved
                                End Of The File
*******************************************************************************/
