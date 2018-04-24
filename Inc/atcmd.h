/*******************************************************************************
* File Name          : atcmd.h
* Author             : Yangjie Gu
* Description        : This file provides all the atcmd functions.

* History:
*  09/28/2017 : atcmd V1.00
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _ATCMD_H
#define _ATCMD_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "uart_api.h"

/* Private define ------------------------------------------------------------*/
#define TBUFFER_MAX UART_BUF_MAX_LENGTH
#define ATDbgPrintf DebugPrintf
#define ATCmdPrintf DebugPrintf

/* Exported types ------------------------------------------------------------*/
typedef struct
{
  const char *cmdStr;
  uint8_t cmdType;
  uint8_t isMatchAll;
} ATCompareTable_t;

typedef enum {
  AT_CMD_DEF_INITIAL,
  AT_CMD_DEF_PROTOTYPE,
  AT_CMD_DEF_RESET,
  AT_CMD_DEF_SLEEP,
  AT_CMD_DEF_PWRCTL,
  AT_CMD_DEF_TRANSFER,
  AT_CMD_DEF_LED,
  AT_CMD_DEF_PWM,
  AT_CMD_DEF_FLASH,
  AT_CMD_DEF_FLASH_CHIP_ERASE,
  AT_CMD_DEF_GPS,
  AT_CMD_DEF_GSENSOR,
  AT_CMD_DEF_ADC,
  AT_CMD_DEF_RTC,
  AT_CMD_DEF_MODEM,
  AT_CMD_DEF_MODEMUPDATE,
  AT_CMD_DEF_IOINFO,
  AT_CMD_DEF_GPIOSETDIR,
  AT_CMD_DEF_GPIOREAD,
  AT_CMD_DEF_GPIOWRITE,
  AT_CMD_DEF_WATCHDOG,
  AT_CMD_DEF_DBGCTL,
  AT_CMD_DEF_READ_IMEI,
  AT_CMD_DEF_READ_IP_DNS,
#if (TC30M_TEST_CONFIG_OFF == 0)
  AT_CMD_DEF_IGNCHG,
#endif

  AT_CMD_DEF_LAST,
  AT_CMD_DEF_NULL = 0xFF
} ATCmdType_t;

/* Variables -----------------------------------------------------------------*/
// Variable Declared
extern DebugCtlPrarm DbgCtl;

//Function Declare
extern void ATCmdDetection(void);

extern void SystemDisableAllInterrupt(void);
extern void SystemEnableAllInterrupt(void);

extern void ATCmdProcessing(uint8_t Type, uint8_t FactoryMode, uint8_t Len, int32_t Param, uint8_t *dataBuf);

#ifdef __cplusplus
}
#endif
#endif

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2018. All rights reserved
                                End Of The File
*******************************************************************************/
