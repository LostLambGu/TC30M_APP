/*******************************************************************************
* File Name          : atcmd.c
* Author             : Yangjie Gu
* Description        : This file provides all the atcmd functions.

* History:
*  09/28/2017 : atcmd V1.00
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "atcmd.h"
#include "rtcclock.h"

/* Private variables ---------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* External variables --------------------------------------------------------*/
extern DebugCtlPrarm DbgCtl;
/* Private function prototypes -----------------------------------------------*/
/* Private Const prototypes -----------------------------------------------*/
const ATCompareTable_t ATCmdTypeTable[AT_CMD_DEF_LAST] =
	{
		{"AT", AT_CMD_DEF_INITIAL, TRUE},
		{"AT*ATELLIST", AT_CMD_DEF_PROTOTYPE, TRUE},
		{"AT*ATELRESET", AT_CMD_DEF_RESET, TRUE},
		{"AT*ATELSLEEP", AT_CMD_DEF_SLEEP, TRUE},
		{"AT*ATELTO", AT_CMD_DEF_TRANSFER, TRUE},
		{"AT*ATELPWR", AT_CMD_DEF_PWRCTL, TRUE},
		{"AT*ATELLED", AT_CMD_DEF_LED, TRUE},
		{"AT*ATELPWM", AT_CMD_DEF_PWM, TRUE},
		{"AT*ATELFLASH", AT_CMD_DEF_FLASH, TRUE},
		{"AT*ATELFLASHCE", AT_CMD_DEF_FLASH_CHIP_ERASE, TRUE},
		{"AT*ATELGPS", AT_CMD_DEF_GPS, TRUE},
		{"AT*ATELGSENSOR", AT_CMD_DEF_GSENSOR, TRUE},
		{"AT*ATELADC", AT_CMD_DEF_ADC, TRUE},
		{"AT*ATELRTC", AT_CMD_DEF_RTC, TRUE},
		{"AT*ATELMODEM", AT_CMD_DEF_MODEM, TRUE},
		{"AT*ATELIOINFO", AT_CMD_DEF_IOINFO, TRUE},
		{"AT*ATELGPIOREAD", AT_CMD_DEF_GPIOREAD, TRUE},
		{"AT*ATELGPIOWRITE", AT_CMD_DEF_GPIOWRITE, TRUE},
		{"AT*ATELWDT", AT_CMD_DEF_WATCHDOG, TRUE},
		{"AT*ATELDBGCTL", AT_CMD_DEF_DBGCTL, TRUE},
		{"AT*ATELRIMEI", AT_CMD_DEF_READ_IMEI, TRUE},
};

/* Static function declarations ----------------------------------------------*/
static uint8_t ATCmdGetType(ATCompareTable_t *checkTable, uint8_t tableSize, uint8_t *dataBuf, uint16_t dataLen, uint8_t *idxSearch);
static uint8_t ATGetCmdType(uint8_t *dataBuf, uint8_t dataLen, uint8_t *idxSearch);

/* Public functions ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static uint8_t ATCmdGetType(ATCompareTable_t *checkTable, uint8_t tableSize, uint8_t *dataBuf, uint16_t dataLen, uint8_t *idxSearch)
{
	uint8_t idx;
	uint8_t cmdLen;
	ATCompareTable_t *pTable;

	ATDbgPrintf(DbgCtl.ATCmdDbgEn, "\r\n[%s] ATGetType: TableSize(%d) DataLen(%d)", FmtTimeShow(), tableSize, dataLen);
	if (dataLen == 0 || dataBuf == NULL || idxSearch == NULL || checkTable == NULL)
		return AT_CMD_DEF_NULL;

	for (idx = 0; idx < tableSize; idx++)
	{
		pTable = &checkTable[idx];
		cmdLen = strlen(pTable->cmdStr);
		if (strncmp((char *)dataBuf, pTable->cmdStr, cmdLen) == 0)
		{
			if (pTable->isMatchAll)
			{
				if (dataLen == cmdLen /* || !IS_ALPHABET_OR_NUMBER(dataBuf[cmdLen]) */)
				{
					*idxSearch += cmdLen;
					ATDbgPrintf(DbgCtl.ATCmdDbgEn, "\r\n[%s] ATGetType: Match(%d) CmdLen(%d) CmdType(%d)", FmtTimeShow(), pTable->isMatchAll, cmdLen, pTable->cmdType);
					return pTable->cmdType;
				}
			}
			else
			{
				*idxSearch += cmdLen;
				ATDbgPrintf(DbgCtl.ATCmdDbgEn, "\r\n[%s] ATGetType: Match(%d) CmdLen(%d) CmdType(%d)", FmtTimeShow(), pTable->isMatchAll, cmdLen, pTable->cmdType);
				return pTable->cmdType;
			}
		}
	}
	ATDbgPrintf(DbgCtl.ATCmdDbgEn, "\r\n[%s] ATGetType: Can not find Match Command!", FmtTimeShow());
	return AT_CMD_DEF_NULL;
}

static uint8_t ATGetCmdType(uint8_t *dataBuf, uint8_t dataLen, uint8_t *idxSearch)
{
	return ATCmdGetType((ATCompareTable_t *)ATCmdTypeTable, (sizeof(ATCmdTypeTable) / sizeof(ATCmdTypeTable[0])), dataBuf, dataLen, idxSearch);
}

static void ParseATcmdContect(uint8_t *dataBuf, uint8_t dataLen)
{
	uint8_t idx = 0;
	uint8_t idxTemp = 0;
	uint8_t matchCmdType = AT_CMD_DEF_NULL;
	uint8_t ATCmd[TBUFFER_MAX] = {'\0'};
	uint8_t ATCmdLen = 0;
	uint8_t Param2Len = 0;
	int32_t ATParamInt = 0;
	uint8_t *ATParamP = NULL;
	uint8_t isFactoryMode = FALSE;
	uint8_t isStartToGetParameter = FALSE;
	uint8_t isHaveParameter2 = FALSE;

#ifndef AT_COMMAND_USE_LOWERCASE
	// To lower all alphabet
	for (idx = 0; idx < dataLen; idx++)
	{
		if (dataBuf[idx] >= 'a' && dataBuf[idx] <= 'z')
		{
			dataBuf[idx] = (dataBuf[idx] - 'a' + 'A');
		}
		/*if( dataBuf[idx] >= 'A' && dataBuf[idx] <= 'Z' )
		{
			dataBuf[idx] = ( dataBuf[idx] - 'A' + 'a' );
		}*/
		else
		{
			dataBuf[idx] = dataBuf[idx];
		}
	}
	dataBuf[idx] = '\0';
	ATDbgPrintf(DbgCtl.ATCmdDbgEn, "\r\n[%s] UartDBG:To lower  len(%d) buf(%s)", FmtTimeShow(), dataLen, dataBuf);
#endif
	// Parse AT Command
	if (dataBuf != NULL && dataLen > 0)
	{
		memset(ATCmd, 0, sizeof(ATCmd));
		ATCmdLen = 0;
		for (idx = 0; idx < dataLen && ATCmdLen < TBUFFER_MAX; idx++)
		{
			if (dataBuf[idx] == '=' || dataBuf[idx] == '$' || dataBuf[idx] == ',' || dataBuf[idx] == '\"' || dataBuf[idx] == '\0')
			{
				break;
			}
			else
			{
				if (IS_ALPHABET_NUM_POUND(dataBuf[idx]))
				{
					ATCmd[ATCmdLen++] = dataBuf[idx];
				}
				else
				{
					break;
				}
			}
		}
		ATDbgPrintf(DbgCtl.ATCmdDbgEn, "\r\n[%s] AT: Len (%d) AT(%s)", FmtTimeShow(), ATCmdLen, ATCmd);
		if (ATCmdLen < 2)
		{
			ATCmdPrintf(DbgCtl.ATCmdInfoEn, "ERROR");
			ATCmdPrintf(DbgCtl.ATCmdInfoEn, "[%s] ERROR", FmtTimeShow());
			return;
		}
	}
	// Get AT Command Type
	matchCmdType = ATGetCmdType(ATCmd, ATCmdLen, &idxTemp);
	ATParamP = dataBuf + ATCmdLen;
	ATDbgPrintf(DbgCtl.ATCmdDbgEn, "\r\n[%s] AT: Type (%d) AT(%s)", FmtTimeShow(), matchCmdType, ATParamP);
	memset((char *)ATCmd, 0, sizeof(ATCmd));
	ATCmdLen = 0;
	ATParamInt = 0;
	// Check Special AT Type
	if (dataBuf[dataLen - 1] == '?')
	{
		isFactoryMode = TRUE;
	}
	else
	{
		// Parse AT Param 1
		if (ATParamP != NULL && dataLen > 0 && matchCmdType != AT_CMD_DEF_NULL)
		{
			isStartToGetParameter = FALSE;
			for (idx = 0; idx < dataLen && ATCmdLen < TBUFFER_MAX; idx++)
			{
				if (ATParamP[idx] == ',')
				{
					isHaveParameter2 = TRUE;
					break;
				}
				else if (ATParamP[idx] == '\"' || ATParamP[idx] == '\0')
				{
					break;
				}
				else if (ATParamP[idx] == '=')
				{
					isStartToGetParameter = TRUE;
				}
				else if (isStartToGetParameter == TRUE)
				{
					if (IS_NUMBER_ONLY(ATParamP[idx]))
					{
						ATCmd[ATCmdLen++] = ATParamP[idx];
					}
					else
					{
						break;
					}
				}
			}

			if (ATCmdLen > 0 && ATCmdLen < 11)
			{
				StringToInt(ATCmd, ATCmdLen, NULL, &ATParamInt);
				ATDbgPrintf(DbgCtl.ATCmdDbgEn, "\r\n[%s] AT: Len(%d) Parm1 (%d) Stat(%d)", FmtTimeShow(), ATCmdLen, ATParamInt, isHaveParameter2);
			}
		}
		// Parse AT Param 2
		if (isHaveParameter2 == TRUE)
		{
			ATParamP += (ATCmdLen + 1);
			ATDbgPrintf(DbgCtl.ATCmdDbgEn, "\r\n[%s] AT: Check Parm2 AT(%s)", FmtTimeShow(), ATParamP);
			Param2Len = 0;
			memset((char *)ATCmd, 0, sizeof(ATCmd));
			if (ATParamP != NULL && dataLen > 0 && matchCmdType != AT_CMD_DEF_NULL)
			{
				isStartToGetParameter = FALSE;
				for (idx = 0; idx < dataLen && Param2Len < TBUFFER_MAX; idx++)
				{
					if (ATParamP[idx] == '\"' || ATParamP[idx] == '\0')
					{
						break;
					}
					else if (ATParamP[idx] == ',')
					{
						isStartToGetParameter = TRUE;
					}
					else if (isStartToGetParameter == TRUE)
					{
						if (IS_ALPHABET_OR_NUMBER(ATParamP[idx]))
						{
							ATCmd[Param2Len++] = ATParamP[idx];
						}
						else
						{
							break;
						}
					}
				}
				ATDbgPrintf(DbgCtl.ATCmdDbgEn, "\r\n[%s] AT: Len(%d) Parm2(%s)", FmtTimeShow(), Param2Len, ATCmd);
			}
		}
	}
	// AT Process
	if (Param2Len == 0)
		ATCmdProcessing(matchCmdType, isFactoryMode, ATCmdLen, ATParamInt, NULL);
	else
		ATCmdProcessing(matchCmdType, isFactoryMode, ATCmdLen, ATParamInt, ATCmd);
}

/*****************************************************************************
******************************* AT COMMAND START ******************************
*****************************************************************************/
void ATCmdDetection(void)
{
	if (Uart1RxCount & UART_FINISHED_RECV)
	{
		uint8_t DataLen = 0;
		uint8_t Tmp = 0;
		uint8_t UartData[UART_BUF_MAX_LENGTH] = {'\0'};
		// Get Data
		SystemDisableAllInterrupt(); // Disable Interrupt
		DataLen = Uart1RxCount & UART_BUF_MAX_LENGTH;
		// Copy Data
		while ((Uart1RxBuffer[Tmp] != 'a') && (Uart1RxBuffer[Tmp] != 'A') && (DataLen > 0))
		{
			DataLen--;
			Tmp++;
		}
		memcpy((char *)UartData, (char *)(Uart1RxBuffer + Tmp), DataLen);
		UartData[DataLen] = '\0';
		// Clear Data
		memset(Uart1RxBuffer, 0, sizeof(Uart1RxBuffer));
		Uart1RxCount = 0;
		SystemEnableAllInterrupt(); // Enable Interrupt

		// Print Out
		ATDbgPrintf(DbgCtl.ATCmdDbgEn, "\r\n[%s] UartDBG: len(%d) buf(%s)",
					FmtTimeShow(), DataLen, UartData);

		if (DataLen >= 4)
		{
			if ((strstr((char *)UartData, "AT*ATEL") == NULL) && (strstr((char *)UartData, "at*atel") == NULL))
			{
				extern void AppUartAtProcess(uint8_t * UartData, uint8_t DataLen);
				AppUartAtProcess(UartData, DataLen);
				return;
			}
		}
		// Parse AT Cmd
		ParseATcmdContect(UartData, DataLen);
	}
}
/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2018. All rights reserved
                                End Of The File
*******************************************************************************/
