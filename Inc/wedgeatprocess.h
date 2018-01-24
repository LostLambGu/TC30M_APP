/******************************************************************************
*
* File Name          : wedgeatprocess.h
* Author             : 
* Description        : This file provides all the wedgeatprocess functions.

* History:
*  12/12/2017 : wedgeatprocess V1.00
*******************************************************************************/


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TC30M_APPATPROCESS_H__
#define __TC30M_APPATPROCESS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes 
------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

#include "include.h"

#define APP_AT_LOG DebugLog
#define APP_AT_PRINT DebugPrintf

extern uint8_t *AppAtProcess(uint8_t * Data, uint8_t DataLen);
extern void AppUartAtProcess(uint8_t * UartData, uint8_t DataLen);
extern void AppUdpAtProcess(uint8_t * UdpData, uint8_t DataLen);
extern void AppSmsAtProcess(uint8_t * SmsData, uint8_t DataLen);
typedef struct
{
  const char*	cmdStr;
  u8			cmdType;
  u8			isMatchAll;
}ATCompareTable1_t;
typedef enum
{
	AT_CMD_DEF_INITIAL,
	
	AT_CMD_ADMIN_SMS,
	AT_CMD_ADMIN_SVRCFG,
	AT_CMD_ADMIN_FTPCFG,
	AT_CMD_ADMIN_APNCFG,
	AT_CMD_ADMIN_HWRST,
	AT_CMD_ADMIN_PWRMGT,
	AT_CMD_ADMIN_USRDAT,
	AT_CMD_ADMIN_CFGALL,
	AT_CMD_ADMIN_F,

	AT_CMD_OPERA_IGNTYP,
	AT_CMD_OPERA_RPTINTVL,
	AT_CMD_OPERA_IOALM1,
	AT_CMD_OPERA_IOALM2,
	AT_CMD_OPERA_LVA,
	AT_CMD_OPERA_IDLE,
	AT_CMD_OPERA_SODO,
	AT_CMD_OPERA_DIRCHG,
	AT_CMD_OPERA_TOW,
	AT_CMD_OPERA_STPINTVL,
	AT_CMD_OPERA_VODO,
	AT_CMD_OPERA_GFNC,
	AT_CMD_OPERA_CURGEO,
	AT_CMD_OPERA_LOC,
	AT_CMD_OPERA_RELAY,
	AT_CMD_OPERA_PLSRLY,
	AT_CMD_OPERA_OSPD,
	AT_CMD_OPERA_FTPGET,
	
	AT_CMD_DIAGN_VER,
	AT_CMD_DIAGN_RST,
	AT_CMD_DIAGN_MQSTAT,
	AT_CMD_DIAGN_MQCLR,
	AT_CMD_DIAGN_GPSDIAG,
	AT_CMD_DIAGN_REG,
	AT_CMD_DIAGN_GSMDIAG,
  AT_CMD_DIAGN_UNKNOW,
	AT_CMD_DEF_NULL = 0xFF
}ATCmdType_t;
typedef enum
{
	ToUDP,
	ToUART,
	ToSMS
}ATCmdSource_t;

typedef struct  {
int len;
char *s;
} PvtString; 

#ifdef __cplusplus
}
#endif

#endif /* __TC30M_APPATPROCESS_H__ */

/******************************************************************************
*
    Copyrights (C) Asiatelco Technologies Co., 2003-2018. All rights reserved
                                End Of The File
*******************************************************************************/
