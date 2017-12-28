/*******************************************************************************
* File Name          : uart_api.c
* Author             : Yangjie Gu
* Description        : This file provides all the uart_api functions.

* History:
*  10/23/2017 : uart_api V1.00
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include "uart_api.h"
#include "rtcclock.h"

/* Private define ------------------------------------------------------------*/
#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* Variables -----------------------------------------------------------------*/
__IO uint8_t UART1_RX_NUM = 0;
uint16_t Uart1RxCount = 0;
uint8_t Uart1RxBuffer[UART_BUF_MAX_LENGTH] = {'\0'};
__IO uint8_t UART3_RX_NUM = 0;
uint16_t Uart3RxCount = 0;

uint8_t Uart3RxBuffer[UART3_RX_BUFFER_SIZE] = {'\0'};
// uint8_t Uart3ParseBuffer[UART3_RX_BUFFER_SIZE] = {'\0'};

DebugCtlPrarm DbgCtl =
	{
		.NormalInfoEn = TRUE,
		.ATCmdInfoEn = TRUE,
		.ATCmdDbgEn = TRUE,
		.AppATCmdDbgEn = TRUE,
		.Lis3dhDbgInfoEn = TRUE,
		.SerialFlashEn = TRUE,
		.RTCDebugInfoEn = FALSE,
		.UbloxDbgInfoEn = TRUE,
		.ParseatCmdEn = TRUE,
		.ParseatResultEn = TRUE,
		.SendToModemEn = FALSE,
		.AppDebugInfoEn = TRUE,
		.VersionDebugInfoEn = TRUE,
		.NetworkDbgInfoEn = TRUE,
		.LteRecDbgInfoEn = FALSE,
		.WedgeEvtMsgQueInfoEn = TRUE,
		.WedgeEvtAlrtFlwInfoEn = TRUE,
};

/* Function definition -------------------------------------------------------*/
void PutString(uint8_t *String)
{
	HAL_UART_Transmit(&huart1, String, strlen((char *)String), UART_SEND_DATA_TIMEOUT);
}

uint8_t StringToInt(uint8_t *dataBuf, uint16_t dataLen, uint16_t *idxSearch, int32_t *rtnInt)
{
	uint16_t idx;
	uint16_t numLen;
	uint8_t numString[12];

	// Check if buffer is Number first
	if (dataLen == 0 || !IS_NUMBER(dataBuf[0]))
		return FALSE;

	numLen = 0;
	for (idx = 0; idx < dataLen; idx++)
	{
		if (dataBuf[idx] == ' ')
		{
			continue;
		}
		else if (!IS_NUMBER(dataBuf[idx]))
		{
			break;
		}
		else if (numLen == 10)
		{
			return FALSE;
		}
		numString[numLen++] = dataBuf[idx];
	}
	numString[numLen] = 0x0;

	if (idxSearch)
		*idxSearch += idx;

	*rtnInt = atoi((const char *)numString);
	return TRUE;
}

void PutInt(uint32_t num)
{
	uint32_t temp = num;
	uint8_t carray[11];
	uint8_t i = 0, j = 0;
	carray[0] = '0';
	carray[1] = 'x';
	carray[10] = 0;
	for (i = 0; i < 8; i++)
	{
		j = (temp >> ((7 - i) * 4)) & 0x0ful;
		if (j > 9)
		{
			carray[2 + i] = j - 10 + 'a';
		}
		else
		{
			carray[2 + i] = j + '0';
		}
	}
	HAL_UART_Transmit(&huart1, carray, 11, UART_SEND_DATA_TIMEOUT);
}

void PutStrToUart1Dbg(char *str, uint16_t size)
{
	HAL_UART_Transmit(&huart1, (uint8_t *)str, size, UART_SEND_DATA_TIMEOUT);
}

int32_t SerialDbgPrintf(uint8_t type, char *fmt, ...)
{
	if (type == TRUE)
	{
		int32_t cnt;
		char string[MAX_PRINTF_STR_SIZE + 2] = {'\0'};
		va_list ap;
		va_start(ap, fmt);

		cnt = vsnprintf(string, MAX_PRINTF_STR_SIZE, fmt, ap);
		if (cnt > 0)
		{
			if (cnt < MAX_PRINTF_STR_SIZE)
			{
				PutStrToUart1Dbg(string, cnt);
			}
			else
			{
				PutStrToUart1Dbg(string, MAX_PRINTF_STR_SIZE);
			}
		}
		va_end(ap);
		return (cnt);
	}
	return -1;
}

int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, UART_SEND_DATA_TIMEOUT);

	while (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TXE) == 0)
		;

	return (ch);
}

// void UartBufferInit(void)
// {
// 	// Uart3 LTE
// 	Uart3RxCount = 0;
// 	memset((char *)Uart3RxBuffer, 0, sizeof(Uart3RxBuffer));
// 	memset((char *)Uart3ParseBuffer, 0, sizeof(Uart3ParseBuffer));
// }

static uint8_t gMCUFirmwareUpgrade = FALSE;

static int check_command(const char *command)
{
	int length;
	if (NULL != command)
	{
		length = strlen((char *)command);
		if (command[length - 1] == 0x1A)
		{
			return 1;
		}
		else if (length >= 3)
		{
			if ((('a' == command[0]) || ('A' == command[0])) && (('t' == command[1]) || ('T' == command[1])) && ((('\r' == command[length - 2]) && ('\n' == command[length - 1])) || ('\r' == command[length - 1])))
			{
				return 1;
			}
		}
	}
	return 0;
}

void PutStrToUart3Modem(char *string, uint16_t slen)
{
	// Do not allowed to send new instructions
	// when system go into firmware upgrade mode
	if (gMCUFirmwareUpgrade == TRUE)
	{
		return;
	}

	// HAL_UART_Transmit(&huart1, "\r\n--->>", 7, UART_SEND_DATA_TIMEOUT);
	// HAL_UART_Transmit(&huart1, (uint8_t *)string, slen, UART_SEND_DATA_TIMEOUT);
	HAL_UART_Transmit(&huart3, (uint8_t *)string, slen, UART_SEND_DATA_TIMEOUT);

	// Print Out
	DebugPrintf(DbgCtl.NormalInfoEn, "\r\n[%s] LTE: SEND%d %s",
				FmtTimeShow(), slen, string);
}

void SendATCmdToModemDirectly(char *string, uint8_t UpgradeMode)
{
	PutStrToUart3Modem(string, strlen(string));
	gMCUFirmwareUpgrade = UpgradeMode;
	// Print Out
	DebugPrintf(DbgCtl.NormalInfoEn, "\r\n[%s] LTE: SEND Directly Mod%d %s ",
				FmtTimeShow(), UpgradeMode, string);
}

int SendDatatoIop(char *at_commands, uint16_t slen)
{
	if ((NULL == at_commands))
		return STATUS_ERR;
	else if (!check_command(at_commands))
		return STATUS_ERR;
	else
	{
		// Sent
		PutStrToUart3Modem(at_commands, slen);
	}
	return STATUS_OK;
}

void UART1PrintMassData(uint8_t *string, uint16_t slen)
{
	uint16_t i = 0;
    for (i = 0; i < slen; i++)
    {
        while (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TXE) == 0)
            ;
        huart1.Instance->TDR = ((uint8_t)0x00FF & string[i]);
    }
}

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2017. All rights reserved
                                End Of The File
*******************************************************************************/
