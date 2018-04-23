/*******************************************************************************
* File Name          : uart_api.h
* Author             : Yangjie Gu
* Description        : This file provides all the uart_api functions.

* History:
*  10/23/2017 : uart_api V1.00
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UART_API_
#define __UART_API_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <string.h>

#include "stm32f0xx_hal.h"

#include "usart.h"

#define UART_SEND_DATA_TIMEOUT 50
#define MAX_PRINTF_STR_SIZE 256

#define IS_SMSNUMBER_OR_PLUS(x) (IS_NUMBER_ONLY(x) || x == '+')
#define IS_ALPHABET_OR_NUMBER(x) (IS_NUMBER(x) || IS_ALPHABET(x))
#define IS_ALPHABET_NUM_POUND(x) (IS_ALPHABET(x) || IS_NUMBER_ONLY(x) || IS_POUND_CHAR(x))
#define IS_ALPHABET(x) ((x >= 'a' && x <= 'z') || (x >= 'A' && x <= 'Z'))
#define IS_NUMBER(x) ((x >= '0' && x <= '9') || x == '+' || x == '-' || x == '/' || x == '=' || x == '?')
#define IS_NUMBER_ONLY(x) (x >= '0' && x <= '9')
#define IS_IP_ADRESS(x) ((x >= '0' && x <= '9') || x == '.')
#define IS_SPACE_CHAR(x) (x == ' ' || x == '\r' || x == '\n')
#define IS_POUND_CHAR(x) (x == '#' || x == '*' || x == '$')

#define UART1_END_CHAR_OD 0x0D
#define UART1_END_CHAR_OA 0x0A
#define UART_FINISHED_RECV 0x200
#define UART_FIRST_END_CHAR 0x100
#define UART_BUF_MAX_LENGTH 0xFF

extern __IO uint8_t UART1_RX_NUM;
extern uint16_t Uart1RxCount;
extern uint8_t Uart1RxBuffer[UART_BUF_MAX_LENGTH];

typedef struct
{
    uint8_t NormalInfoEn;
    uint8_t ATCmdInfoEn;
    uint8_t ATCmdDbgEn;
    uint8_t AppATCmdDbgEn;
    uint8_t Lis3dhDbgInfoEn;
    uint8_t SerialFlashEn;
    uint8_t RTCDebugInfoEn;
    uint8_t UbloxDbgInfoEn;
    uint8_t ParseatCmdEn;
    uint8_t ParseatResultEn;
    uint8_t SendToModemEn;
    uint8_t VersionDebugInfoEn;
    uint8_t NetworkDbgInfoEn;
    uint8_t LteRecDbgInfoEn;
    uint8_t WedgeAppLogInfoEn;
    uint8_t WedgeCommonLogInfo;
    uint8_t WedgeDeviceInfoEn;
    uint8_t WedgeEvtAlrtFlwInfoEn;
    uint8_t WedgeMsgQueInfoEn;
    uint8_t WedgeRtcTimerInfoEn;
} DebugCtlPrarm;

extern DebugCtlPrarm DbgCtl;

extern void PutString(uint8_t *String);
extern void PutInt(uint32_t num);
extern uint8_t StringToInt(uint8_t *dataBuf, uint16_t dataLen, uint16_t *idxSearch, int32_t *rtnInt);
extern void PutStrToUart1Dbg(char *str, uint16_t size);
extern int32_t SerialDbgPrintf(uint8_t type, char *fmt, ...);

#define UART3_RX_BUFFER_SIZE 512 // AT*TO return to long
extern __IO uint8_t UART3_RX_NUM;
extern uint16_t Uart3RxCount;
extern uint8_t Uart3RxBuffer[UART3_RX_BUFFER_SIZE];
// extern uint8_t Uart3ParseBuffer[UART3_RX_BUFFER_SIZE];
// extern void UartBufferInit(void);

#define AT_SENDTO_RETRY_TIMEOUT (5 * 1000)
#define AT_SENDTO_DELAYT_TIMEOUT (1000)
#define AT_SENTTO_QUICKLY_TIMEOUT (5)
#define AT_WAIT_RING_TIMEOUT (1 * 1000)

/* at handle return status */
#define STATUS_ERR -1
#define STATUS_OK 0
#define STATUS_BAD_ARGUMENTS 1
#define STATUS_TIME_OUT 2
#define STATUS_BAD_AT_CMD 3
#define STATUS_RELAYING 4

extern void PutStrToUart3Modem(char *string, uint16_t slen);
extern void SendATCmdToModemDirectly(char *string, uint8_t UpgradeMode);
extern int SendDatatoIop(char *at_commands, uint16_t slen);
extern void UART1PrintMassData(uint8_t *string, uint16_t slen);

extern char *FmtTimeShow(void);
#define DebugLog(format, ...) SerialDbgPrintf(1, "\r\n[%s] " format, FmtTimeShow(), ##__VA_ARGS__)
#define DebugPrintf(value, format, ...) SerialDbgPrintf(value, format, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif /* __UART_API_ */

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2018. All rights reserved
                                End Of The File
*******************************************************************************/
