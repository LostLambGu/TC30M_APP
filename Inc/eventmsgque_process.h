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
    WEDGE_CFG_SVRCFG,
    WEDGE_CFG_FTPCFG,
    WEDGE_CFG_APNCFG,

    WEDGE_CFG_OPERATE_INVALID_MAX
} WEDGECfgOperateTypeDef;

extern uint8_t WedgeCfgInit(void);
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

#define WEDGE_CFG_STATE_NUM_MAX (39)
typedef struct
{
    uint8_t CfgChgNum;
    uint8_t CfgChgState[WEDGE_CFG_STATE_NUM_MAX];
} WEDGECfgChgStateTypedef;

extern void WedgeCfgChgStateInit(void);
extern void WedgeCfgChgStateSet(WEDGECfgChangeTypeDef CfgChg, uint8_t State);
extern uint8_t WedgeCfgChgStateIsChanged(void);
extern uint8_t WedgeCfgChgStateGet(void);
extern void WedgeCfgChgStateProcess(void);

extern uint8_t WedgeFlashChipErase(void);
extern uint8_t WedgeFlashEraseSector(uint32_t address);
extern uint8_t WedgeFlashReadData(uint32_t address, const uint8_t * const pDataBuf, const uint32_t datalen);
extern uint8_t WedgeFlashWriteData(uint32_t address, uint8_t * const pDataBuf, const uint32_t datalen);

enum
{
    WEDGE_MSG_QUE_SENT = 0xBB,
    WEDGE_MSG_QUE_UNSENT = 0xFF
};

enum
{
    WEDGE_MSG_QUE_UDP_TYPE = 0xCC,
    WEDGE_MSG_QUE_SMS_TYPE = 0x33
};

#define WEDGE_MSG_QUE_DATA_LEN_MAX (254)

typedef struct
{
    uint8_t sentstate;
    uint8_t type;
    uint8_t data[WEDGE_MSG_QUE_DATA_LEN_MAX];
} WEDGEMsgQueCellTypeDef;

#define WEDGE_STORAGE_SECTOR_SIZE (4096)
#define WEDGE_STORAGE_BASE_ADDR (0x00000000)

#define WEDGE_MSG_QUE_OFFSET (WEDGE_STORAGE_SECTOR_SIZE * 256)
#define WEDGE_MSG_QUE_START_ADDR (WEDGE_STORAGE_BASE_ADDR + WEDGE_MSG_QUE_OFFSET)
#define WEDGE_MSG_QUE_TOTAL_SIZE (WEDGE_STORAGE_SECTOR_SIZE * 256)
#define WEDGE_MSG_QUE_TOTAL_NUM (WEDGE_MSG_QUE_TOTAL_SIZE / sizeof(WEDGEMsgQueCellTypeDef))
#define WEDGE_MSG_QUE_END_ADDR (WEDGE_MSG_QUE_START_ADDR + WEDGE_MSG_QUE_TOTAL_SIZE - 1)

extern uint8_t WedgeMsgQueInit(void);
extern uint8_t WedgeMsgQueInWrite(const WEDGEMsgQueCellTypeDef * const pQueCell, const uint32_t index);
extern uint8_t WedgeMsgQueOutRead(WEDGEMsgQueCellTypeDef * const pQueCell, const uint32_t index);

typedef enum
{
    WEDGE_RTC_TIMER_INVALID = 0,
    WEDGE_RTC_TIMER_PERIODIC,
    WEDGE_RTC_TIMER_ONETIME,

    WEDGE_RTC_TIMER_MAX
} WEDGERTCTimerTypeDef;

typedef enum
{
    Periodic_Moving_Event = 0,
    Periodic_OFF_Event,
    Periodic_Health_Event,
    Stop_Report_Onetime_Event,
    Periodic_Hardware_Reset_Onetime,

    WEDGE_RTC_TIMER_INSTANCE_INVALID_MAX
} WEDGERTCTimerInstanceTypeDef;

typedef struct
{
    WEDGERTCTimerTypeDef RTCTimerType;
    WEDGERTCTimerInstanceTypeDef RTCTimerInstance;
    uint32_t settime;
} RTCTimerListCellTypeDef;

#define WEDGE_RTC_TIMER_INSTANCE_MAX (16)
typedef struct
{
    uint8_t instancenum;
    RTCTimerListCellTypeDef currentinstance;
    RTCTimerListCellTypeDef instancearray[WEDGE_RTC_TIMER_INSTANCE_MAX];
} RTCTimerListTypeDef;

typedef enum
{
    WEDGE_RTC_TIMER_MODIFY_INCREASE = 0,
    WEDGE_RTC_TIMER_MODIFY_DECREASE,

    WEDGE_RTC_TIMER_MODIFY_INVALID_MAX
} WEDGERTCTimerSettimeTypeDef;

extern uint32_t WedgeRtcCurrentSeconds(void);
extern uint8_t WedgeRtcTimerInit(void);
extern uint8_t WedgeRtcHwrstPowerLostJudge(void);
extern uint8_t WedgeRtcTimerInstanceAdd(RTCTimerListCellTypeDef Instance);
extern uint8_t WedgeRtcTimerInstanceDel(WEDGERTCTimerInstanceTypeDef InstanceType);
extern uint8_t WedgeRtcTimerModifySettime(uint32_t Delta, WEDGERTCTimerSettimeTypeDef ModifyType);

#ifdef __cplusplus
}
#endif

#endif /* __EVENT_MSG_QUEUE_PROCESS_H__ */

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2017. All rights reserved
                                End Of The File
*******************************************************************************/
