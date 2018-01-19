/*******************************************************************************
* File Name          : wedgecommonapi.h
* Author             : Yangjie Gu
* Description        : This file provides all the wedgecommonapi functions.

* History:
*  10/28/2017 : wedgecommonapi V1.00
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef WEDGE_COMMON_API_H__
#define WEDGE_COMMON_API_H__

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

extern void WedgeIsPowerLostSet(uint8_t Status);
extern uint8_t WedgeIsPowerLostGet(void);

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

extern void WedgeCfgInit(WEDGECfgTypeDef *pWEDGECfg);
extern void WedgeCfgGetTotal(uint8_t *pBuf, uint32_t *pSize);
extern void *WedgeCfgGet(WEDGECfgOperateTypeDef CfgGet);
extern void WedgeCfgSet(WEDGECfgOperateTypeDef CfgSet, void *pvData);

typedef enum
{
    SMS_ADDR_CFG_CHG = 0,
    SVRCFG_CFG_CHG_ALL,
    SVRCFG_CFG_CHG_1,
    SVRCFG_CFG_CHG_2,
    SVRCFG_CFG_CHG_3,
    SVRCFG_CFG_CHG_4,
    SVRCFG_CFG_CHG_5,
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
    OPSOD_CFG_CHG,/* Over Speed */
    CFG_CHG_INVALIAD_MAX
} WEDGECfgChangeTypeDef;

typedef struct
{
    uint8_t CfgChgNum;
    uint8_t CfgChgState[CFG_CHG_INVALIAD_MAX];
} WEDGECfgChgStateTypedef;

extern void WedgeCfgChgStateResetAll(void);
extern void WedgeCfgChgStateSet(WEDGECfgChangeTypeDef CfgChg, uint8_t State);
extern uint8_t WedgeCfgChgStateIsChanged(void);
extern uint8_t WedgeCfgChgStateGet(WEDGECfgChangeTypeDef CfgChg);
extern void WedgeUpdateBinaryMsgGpsRecord(void);

extern void SmsReceivedHandle(void *MsgBufferP, uint32_t size);
extern void UdpReceivedHandle(void *MsgBufferP, uint32_t size);
extern void WedgeResponseUdpBinaryInit(void);
extern void WedgeResponseUdpBinary(WEDGEPYLDTypeDef PYLDType, WEDGEEVIDTypeDef EvID);
extern void WedgeResponseUdpAsciiInit(void);
extern void WedgeResponseUdpAscii(WEDGEPYLDTypeDef PYLDType, void *MsgBufferP, uint32_t size);
extern void WedgeResponseSms(WEDGEPYLDTypeDef PYLDType, void *MsgBufferP, uint32_t size);

extern uint8_t WedgeFlashChipErase(void);
extern uint8_t WedgeFlashEraseSector(uint32_t address);
extern uint8_t WedgeFlashReadData(uint32_t address, uint8_t *pDataBuf, uint32_t datalen);
extern uint8_t WedgeFlashWriteData(uint32_t address, uint8_t *pDataBuf, uint32_t datalen);

#define WEDGE_BUF_POOL_NUM_MAX (12)
#define WEDGE_BUF_CELL_LEN_MAX (256) // sizeof(WEDGEMsgQueCellTypeDef), WEDGE_MSG_QUE_DATA_LEN_MAX + head + size
typedef struct
{
	uint8_t UsedFlag[WEDGE_BUF_POOL_NUM_MAX];
	uint8_t BufPool[WEDGE_BUF_POOL_NUM_MAX][WEDGE_BUF_CELL_LEN_MAX];
} WedgeBufPoolTypedef;

void *WedgeBufPoolCalloc(uint16_t len);
void WedgeBufPoolFree(void *pBuf);

#define WEDGE_UDP_SEND_QUEUE_LENGHT_MAX (6)
typedef struct
{
    uint16_t datalen;
    uint8_t *buf;
} WedgeUdpSendUintTypedef;

typedef struct
{
    uint8_t putindex;
    uint8_t getindex;
    uint8_t numinqueue;
    WedgeUdpSendUintTypedef WedgeUDPIpSendUint[WEDGE_UDP_SEND_QUEUE_LENGHT_MAX];
} WedgeUdpSendQueueTypedef;

extern void WedgeUdpSendUnitIn(WedgeUdpSendQueueTypedef *pWedgeUdpSendQueue, WedgeUdpSendUintTypedef *pWedgeUDPIpSendUint);

extern void WedgeUdpSendUintOut(WedgeUdpSendQueueTypedef *pWedgeUdpSendQueue, WedgeUdpSendUintTypedef *pWedgeUDPIpSendUint);

typedef enum  
{
    WEDGE_UDP_DISCONNECTED_STAT         = 0,
    WEDGE_UDP_FREE_IDLE_STAT 			= 1,
    WEDGE_UDP_NEED_OPEN_STAT 			= 2,
    WEDGE_UDP_WAIT_OPEN_STAT		    = 3,
    WEDGE_UDP_OPENED_STAT               = 4,
    WEDGE_UDP_CLOSE_STAT                = 5,
    WEDGE_WAIT_UDP_DISCONNECTED_STAT    = 6,
    
    WEDGE_UDP_MAXIMUM_STAT,
} WedgeUdpSocketManageStatT;

typedef struct
{
    uint8_t newdatacome;
    WedgeUdpSocketManageStatT UdpSocketManageStat;
} WedgeUdpSocketManageTypeDef;

extern void *WedgeBufPoolCalloc(uint16_t len);
extern void WedgeBufPoolFree(void *pBuf);
extern void WedgeUdpSendUnitIn(WedgeUdpSendQueueTypedef *pWedgeUdpSendQueue, WedgeUdpSendUintTypedef *pWedgeUDPIpSendUint);
extern void WedgeUdpSendUintOut(WedgeUdpSendQueueTypedef *pWedgeUdpSendQueue, WedgeUdpSendUintTypedef *pWedgeUDPIpSendUint);
extern void WedgeUdpSocketManageDataComeSet(uint8_t Status);
extern void WedgeUdpSocketManageStatSet(WedgeUdpSocketManageStatT Stat);
extern void WedgeUdpSocketManageProcess(void);


#ifdef __cplusplus
}
#endif

#endif /* WEDGE_COMMON_API_H__ */

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2018. All rights reserved
                                End Of The File
*******************************************************************************/
