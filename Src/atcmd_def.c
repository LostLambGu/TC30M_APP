/*******************************************************************************
* File Name          : atcmd_def.c
* Author             : Yangjie Gu
* Description        : This file provides all the atcmd_def functions.

* History:
*  09/28/2017 : atcmd_def V1.00
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdlib.h>

#include "adc.h"
#include "iwdg.h"
#include "atcmd.h"
#include "initialization.h"
#include "rtcclock.h"
#include "ublox_driver.h"
#include "lis3dh_driver.h"
#include "pwmaudio.h"
#include "iocontrol.h"
#include "deepsleep.h"
#include "flash.h"

/* Private define ------------------------------------------------------------*/
#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define ENTER_MODEM_BYPASS_INFO "\r\nIn this mode, you can use fully modem command.\r\n"
/* Variables -----------------------------------------------------------------*/

extern const ATCompareTable_t ATCmdTypeTable[AT_CMD_DEF_LAST];
extern PswGpsNmeaStreamMsgT MsgBuffer;

__IO uint8_t factorymodeindicatefalg = FALSE;

extern char ICCIDBuf[32];
extern char IMEIBuf[32];

extern void RefreshImei(void);

/* Static function declarations ----------------------------------------------*/
static void ATCmdDefGPSFactoryTest(void);
static void ATCmdModemFactoryTest(void);
static void ATCmdGsensorFactoryTest(void);
static void ATCmdDefAdcFactoryTest(void);
static void ATCmdDefFlashFactoryTest(void);
static void ATCmdDefRtcFactoryTest(void);
static void ATCmdIoFactoryTest(void);

static void ATCmdDefPrototype(void);
static void ATCmdDefReset(void);
static void ATCmdDefSleep(uint8_t Len, int32_t Param);
static void ATCmdDefTransfer(uint8_t Len, int32_t Param, uint8_t *dataBuf);
static void ATCmdDefPwrCtl(uint8_t Len, int32_t Param, uint8_t *dataBuf);
static void ATCmdDefLed(uint8_t Len, int32_t Param, uint8_t *dataBuf);
static void ATCmdDefPwm(uint8_t Len, int Param);
static void ATCmdDefGPS(uint8_t Len, int Param);
static void ATCmdGsensorTest(uint8_t Len, int Param);
static void ATCmdDefGPIOREAD(uint8_t Len, int Param, uint8_t *dataBuf);
static void ATCmdDefGPIOWRITE(uint8_t Len, int32_t Param, uint8_t *dataBuf);
static void ATCmdDefWatchDog(uint8_t Len, int32_t Param, uint8_t *dataBuf);
static void ATCmdDefDbgCtl(uint8_t Len, int32_t Param, uint8_t *dataBuf);
static void ATCmdDefReadIMEI(uint8_t Len, int32_t Param);

static void ATCmdDefDefault(void);

/* Function definitions ------------------------------------------------------*/
void ATCmdProcessing(uint8_t Type, uint8_t FactoryMode, uint8_t Len, int32_t Param, uint8_t *dataBuf)
{
	if (dataBuf == NULL)
		ATDbgPrintf(DbgCtl.ATCmdDbgEn, "[%s] AT Process: Type(%d) Len(%d) Param(%d)",
					FmtTimeShow(), Type, Len, Param);
	else
		ATDbgPrintf(DbgCtl.ATCmdDbgEn, "[%s] AT Process: Type(%d) Len(%d) Param(%d) Buf(%s)",
					FmtTimeShow(), Type, Len, Param, dataBuf);
	if (Type == AT_CMD_DEF_NULL)
	{
		ATCmdPrintf(DbgCtl.ATCmdInfoEn, "ERROR");
		ATDbgPrintf(DbgCtl.ATCmdDbgEn, "[%s] ERROR", FmtTimeShow());
		return;
	}

	if (FactoryMode == TRUE)
	{
		switch (Type)
		{
		case AT_CMD_DEF_GPS:
			ATCmdDefGPSFactoryTest();
			break;

		case AT_CMD_DEF_MODEM:
			ATCmdModemFactoryTest();
			break;

		case AT_CMD_DEF_GSENSOR:
			ATCmdGsensorFactoryTest();
			break;

		case AT_CMD_DEF_ADC:
			ATCmdDefAdcFactoryTest();
			break;

		case AT_CMD_DEF_FLASH:
			ATCmdDefFlashFactoryTest();
			break;

		case AT_CMD_DEF_RTC:
			ATCmdDefRtcFactoryTest();
			break;

		case AT_CMD_DEF_IOINFO:
			ATCmdIoFactoryTest();
			break;

		default:
			ATCmdDefDefault();
			break;
		}
	}
	else
	{
		switch (Type)
		{
		case AT_CMD_DEF_INITIAL:
			ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nOK\r\n");
			break;

		case AT_CMD_DEF_PROTOTYPE:
			ATCmdDefPrototype();
			break;

		case AT_CMD_DEF_RESET:
			ATCmdDefReset();
			break;

		case AT_CMD_DEF_SLEEP:
			ATCmdDefSleep(Len, Param);
			break;

		case AT_CMD_DEF_TRANSFER:
			ATCmdDefTransfer(Len, Param, dataBuf);
			break;

		case AT_CMD_DEF_PWRCTL:
			ATCmdDefPwrCtl(Len, Param, dataBuf);
			break;

		case AT_CMD_DEF_LED:
			ATCmdDefLed(Len, Param, dataBuf);
			break;

		case AT_CMD_DEF_PWM:
			ATCmdDefPwm(Len, Param);
			break;

		case AT_CMD_DEF_GPS:
			ATCmdDefGPS(Len, Param);
			break;

		case AT_CMD_DEF_GSENSOR:
			ATCmdGsensorTest(Len, Param);
			break;

		case AT_CMD_DEF_GPIOREAD:
			ATCmdDefGPIOREAD(Len, Param, dataBuf);
			break;

		case AT_CMD_DEF_GPIOWRITE:
			ATCmdDefGPIOWRITE(Len, Param, dataBuf);
			break;

		case AT_CMD_DEF_WATCHDOG:
			ATCmdDefWatchDog(Len, Param, dataBuf);
			break;

		case AT_CMD_DEF_DBGCTL:
			ATCmdDefDbgCtl(Len, Param, dataBuf);
			break;

		case AT_CMD_DEF_READ_IMEI:
			ATCmdDefReadIMEI(Len, Param);
			break;

		default:
			ATCmdDefDefault();
			break;
		}
	}
}

static void ATCmdDefGPSFactoryTest(void)
{
	const uint8_t temp = DbgCtl.UbloxDbgInfoEn;

	UbloxPowerEnControl(ENABLE);
	DbgCtl.UbloxDbgInfoEn = TRUE;
	factorymodeindicatefalg = TRUE;

	ATCmdPrintf(TRUE, "\r\nGps fatctory test mode\r\n");

	HAL_Delay(1000);
	while (TRUE == factorymodeindicatefalg)
	{
		ReadUbloxData();
		HAL_Delay(CHECK_UBLOX_STAT_TIMEOUT);
	}

	DbgCtl.UbloxDbgInfoEn = temp;
	UbloxPowerEnControl(DISABLE);
}

static void ATCmdModemFactoryTest(void)
{
	uint8_t DataLen = 0;
	uint8_t UartData[UART_BUF_MAX_LENGTH] = {'\0'};

	ModemPowerEnControl(DISABLE);
	ModemRTSEnControl(DISABLE);
	HAL_Delay(1000);
	factorymodeindicatefalg = TRUE;
	
	ModemPowerEnControl(ENABLE);
	ModemRTSEnControl(ENABLE);

	HAL_Delay(3000);
	HAL_UART_Transmit(&huart1, ENTER_MODEM_BYPASS_INFO, strlen(ENTER_MODEM_BYPASS_INFO), UART_SEND_DATA_TIMEOUT);

	while (factorymodeindicatefalg == TRUE)
	{
		if (Uart1RxCount & UART_FINISHED_RECV)
		{
			// Get Data
			SystemDisableAllInterrupt(); // Disable Interrupt
			DataLen = Uart1RxCount & UART_BUF_MAX_LENGTH;
			// Copy Data
			if (*Uart1RxBuffer == 0xff)
			{
				memcpy((char *)UartData, (char *)(Uart1RxBuffer + 1), DataLen);
			}
			else
				memcpy((char *)UartData, (char *)Uart1RxBuffer, DataLen);
			UartData[DataLen++] = '\r';
			UartData[DataLen++] = '\n';
			UartData[DataLen] = '\0';
			// Clear Data
			memset(Uart1RxBuffer, 0, sizeof(Uart1RxBuffer));
			Uart1RxCount = 0;
			SystemEnableAllInterrupt(); // Enable Interrupt
			// HAL_UART_Transmit(&huart1, UartData, strlen((const char *)UartData), UART_SEND_DATA_TIMEOUT);
			HAL_UART_Transmit(&huart3, UartData, DataLen, UART_SEND_DATA_TIMEOUT);
			DataLen = 0;
			memset(UartData, 0, sizeof(UartData));
		}
		HAL_Delay(50);
		if (Uart3RxCount != 0)
		{
			HAL_UART_Transmit(&huart1, Uart3RxBuffer, Uart3RxCount, UART_SEND_DATA_TIMEOUT);
			memset(Uart3RxBuffer, 0, sizeof(Uart3RxBuffer));
			Uart3RxCount = 0;
		}
	}

	ModemRTSEnControl(DISABLE);
	ModemPowerEnControl(DISABLE);
}

static void ATCmdGsensorFactoryTest(void)
{
	status_t status;
	status = LIS3DH_GetWHO_AM_I();

	if (status == MEMS_SUCCESS)
	{
		ATCmdPrintf(TRUE, "\r\nOK\r\n");
	}
	else
	{
		ATCmdPrintf(TRUE, "\r\nERROR\r\n");
	}
}

static void ATCmdDefAdcFactoryTest(void)
{

	uint16_t uhADCxConvertedValue[2] = {0};
	uint16_t ADC1ConvertedVoltage[2] = {0};

	// Srart Conversion
	ADCDMAConversion(uhADCxConvertedValue, ADC1ConvertedVoltage);

	// Show Result
	ATCmdPrintf(TRUE, "\r\nADC: AdcCh10 VAL(%04d) VOL(%d mV)", uhADCxConvertedValue[0], ADC1ConvertedVoltage[0]);
	ATCmdPrintf(TRUE, "\r\nADC: AdcCh11 VAL(%04d) VOL(%d mV)", uhADCxConvertedValue[1], ADC1ConvertedVoltage[1]);
	ATCmdPrintf(TRUE, "\r\nOK\r\n");
}

static void ATCmdDefFlashFactoryTest(void)
{
	uint16_t i, j, sectorsize = 4096, pagesize = 256;
	uint8_t readbuf[256] = {0}, writebuf[256] = {0};

	ATCmdPrintf(TRUE, "\r\nSFlash factory test start!");
	SerialFlashInit();
	for (i = 0; i < 5; i++)
	{

		if (SerialFlashErase(FLASH_ERASE_04KB, i) != FLASH_STAT_OK)
		{
			ATCmdPrintf(TRUE, "\r\n[%s] SFlash sector erase error!", FmtTimeShow());
			return;
		}

		for (j = 0; j < pagesize; j++)
		{
			writebuf[j] = j + i;
		}

		if (SerialFlashWrite(writebuf, sectorsize * i, pagesize) != FLASH_STAT_OK)
		{
			ATCmdPrintf(TRUE, "\r\n[%s] SFlash page write error!", FmtTimeShow());
			return;
		}

		memset(readbuf, 0, sizeof(readbuf));

		if (SerialFlashRead(readbuf, sectorsize * i, pagesize) != FLASH_STAT_OK)
		{
			ATCmdPrintf(TRUE, "\r\n[%s] SFlash page write error!", FmtTimeShow());
			return;
		}

		if (strncmp((const char *)writebuf, (const char *)readbuf, pagesize) == 0)
		{
			ATCmdPrintf(TRUE, "\r\n[%s] SFlash %dth page test pass!", FmtTimeShow(), i);
		}
		else
		{
			ATCmdPrintf(TRUE, "\r\n[%s] SFlash %dth page test fail!", FmtTimeShow(), i);
			return;
		}
	}

	ATCmdPrintf(TRUE, "\r\nOK\r\n");
}

static void ATCmdDefRtcFactoryTest(void)
{
	TimeTableT timeTable = GetRTCDatetime();
	ATCmdPrintf(TRUE, "\r\nRTC:%04d-%02d-%02d %02d:%02d:%02d",
				timeTable.year, timeTable.month, timeTable.day,
				timeTable.hour, timeTable.minute, timeTable.second);
	ATCmdPrintf(TRUE, "\r\nOK\r\n");
}

static void ATCmdIoFactoryTest(void)
{
	// char *str[2] = {"LOW", "HIGH"};
	// ATCmdPrintf(TRUE, "\r\nIO:PWR_DET, %s", str[READ_IO(PC14_CHG_END_GPIO_Port, PC14_CHG_END_Pin)]);
	ATCmdPrintf(TRUE, "\r\nOK\r\n");
}

static void ATCmdDefDefault(void)
{
	ATCmdPrintf(DbgCtl.ATCmdInfoEn, "ERROR");
	ATCmdPrintf(DbgCtl.ATCmdInfoEn, "[%s] ERROR", FmtTimeShow());
}

static void ATCmdDefReset(void)
{
	// Print Out
	ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\n*RST:Done\r\n");
	ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nOK\r\n");
	// Jump to bootloader
	MCUReset();
}

static void ATCmdDefSleep(uint8_t Len, int32_t Param)
{
	if (Len == 0)
	{
		ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nERROR\r\n");
		return;
	}
	else
	{
		if (Param > 0)
		{
			ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\n*SLEEP:Done\r\n");
			ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nOK\r\n");
			// Set Alarm Time
			MCUDeepSleep(Param);
		}
	}
	ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\n*WAKE UP OK\r\n");
}

static void ATCmdDefTransfer(uint8_t Len, int32_t Param, uint8_t *dataBuf)
{
	if (Len == 0)
	{
		ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nERROR\r\n");
		return;
	}
	else
	{
		if (dataBuf != NULL)
		{
			char Sendbuf[TBUFFER_MAX + 4] = {'\0'};

			//Enable UART3 RTS
			ModemRTSEnControl(ENABLE);
			
			// AT TO LTE
			sprintf((char *)Sendbuf, "%s\r\n", dataBuf);

			PutStrToUart3Modem(Sendbuf, strlen((char *)Sendbuf));

			// if(GetModemATPortStat() == FALSE)
			// {
			// 	ATOutPrintf(DbgCtl.ATCmdInfoEn,"\r\nToModem: Not Ready, Retry!");
			// }

			HAL_Delay(10);
			//Disable UART3 RTS
			// ModemRTSEnControl(DISABLE);
			return;
		}
	}
	ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nERROR\r\n");
}

static void ATCmdDefPwrCtl(uint8_t Len, int32_t Param, uint8_t *dataBuf)
{
	uint8_t Result = 0;
	if (Len == 0)
	{
		ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\n*PWR: (%s)", "DEBUG_PWR_INFO");
	}
	else
	{
		switch (Param)
		{
		case 0:
		{
			Result = 0;
			ModemPowerEnControl(DISABLE);
			ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\n*PWR: MODEM POWER OFF\r\n");
		}
		break;

		case 1:
		{
			Result = 0;
			ModemPowerEnControl(ENABLE);
			ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\n*PWR: MODEM POWER ON\r\n");
		}
		break;

		case 2:
		{
			Result = 0;
			UbloxPowerEnControl(DISABLE);
			ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\n*PWR: GPS POWER OFF\r\n");
		}
		break;

		case 3:
		{
			Result = 0;
			UbloxPowerEnControl(ENABLE);
			ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\n*PWR: GPS POWER ON\r\n");
		}
		break;

		default:
			ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\n*PWR: (%s)", "DEBUG_PWR_INFO");
			break;
		}
	}
	if (Result != 1)
		ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nOK\r\n");
	else
		ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nERROR\r\n");
}

static void ATCmdDefLed(uint8_t Len, int32_t Param, uint8_t *dataBuf)
{
	if (Len == 0)
	{
		ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nLED: (%s)", "DEBUG_LED_INFO");
		return;
	}

	switch (Param)
	{
	case 0:
		ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nLED: Red LEDs Off");
		HAL_GPIO_WritePin(PC9_LED_R_GPIO_Port, PC9_LED_R_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(PC8_LED_G_GPIO_Port, PC8_LED_G_Pin, GPIO_PIN_SET);
		break;

	case 1:
		ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nLED: Red LEDs On");
		HAL_GPIO_WritePin(PC9_LED_R_GPIO_Port, PC9_LED_R_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(PC8_LED_G_GPIO_Port, PC8_LED_G_Pin, GPIO_PIN_RESET);
		break;

		// case 0:
		// 	gRedLEDFlashingFlag = FALSE;
		// 	// Print Out
		// 	ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nLED: Red LED Flashing OFF");
		// 	gGreenLEDFlashingFlag = FALSE;
		// 	// Print Out
		// 	ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nLED: Green LED Flashing OFF");
		// 	break;

		// case 1:
		// 	gRedLEDFlashingFlag = TRUE;
		// 	// Print Out
		// 	ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nLED: Red LED Flashing ON");
		// 	gGreenLEDFlashingFlag = TRUE;
		// 	// Print Out
		// 	ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nLED: Green LED Flashing ON");
		// 	break;

		// case 2:
		// 	gRedLEDFlashingFlag = TRUE;
		// 	// Print Out
		// 	ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nLED: Red LED Flashing ON");
		// 	break;

		// case 3:
		// 	gRedLEDFlashingFlag = FALSE;
		// 	// Print Out
		// 	ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nLED: Red LED Flashing OFF");
		// 	break;

		// case 4:
		// 	gGreenLEDFlashingFlag = TRUE;
		// 	// Print Out
		// 	ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nLED: Green LED Flashing ON");
		// 	break;

		// case 5:
		// 	gGreenLEDFlashingFlag = FALSE;
		// 	// Print Out
		// 	ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nLED: Green LED Flashing OFF");
		// 	break;

	default:
		ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nLED: (%s)", "DEBUG_LED_INFO");
		break;
	}
	ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nOK\r\n");
}

static void ATCmdDefPwm(uint8_t Len, int Param)
{
	if (Len == 0)
	{
		ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\n[%s] Error ", FmtTimeShow());
		return;
	}

	switch (Param)
	{
	case 0:
		StopPWMAudio();
		// Print Out
		ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nPWM: STOP");
		break;

	case 1:
		StartPWMAudio(PERIOD_0050Hz, 10);
		// Print Out
		ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nPWM: START 50Hz");
		break;

	case 2:
		StartPWMAudio(PERIOD_0500Hz, 10);
		// Print Out
		ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nPWM: START 500Hz");
		break;

	case 3:
		StartPWMAudio(PERIOD_1000Hz, 10);
		// Print Out
		ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nPWM: START 1KHz");
		break;

	case 4:
		StartPWMAudio(PERIOD_2000Hz, 10);
		// Print Out
		ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nPWM: START 2KHz");
		break;

	case 5:
		StartPWMAudio(PERIOD_4000Hz, 10);
		// Print Out
		ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nPWM: START 4KHz");
		break;

	case 6:
		StartPWMAudio(PERIOD_12000HZ, 10);
		// Print Out
		ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nPWM: START 12KHz");
		break;

	default:
		break;
	}
	ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nOK\r\n");
}

static void ATCmdDefGPS(uint8_t Len, int Param)
{
	if (Len == 0)
	{
		ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nERROR\r\n");
		return;
	}
	else
	{
		switch (Param)
		{
		case 0:
		{
			ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\n[%s] *GPS: GPS Stop", FmtTimeShow());
			UbloxGPSStop();
		}
		break;

		case 1:
		{
			ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\n[%s] *GPS: GPS Start", FmtTimeShow());
			// Print Out
			ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\n[%s] *GPS: Timeout Value(%d)", FmtTimeShow(), UbloxCheckStatTimeout);
			UbloxGPSStart();
		}
		break;

		default:
			break;
		}
	}
	ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nOK\r\n");
}

static void ATCmdDefPrototype(void)
{
	uint8_t Count = 0;

	ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\n==SUPPORT AT AS FOLLOW==\r\n");
	for (Count = 0; Count < (sizeof(ATCmdTypeTable) / sizeof(ATCmdTypeTable[0])); Count++)
	{
		ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\n    %s", ATCmdTypeTable[Count].cmdStr);
	}
	ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\n==END OF THE AT LIST==");
}

static void ATCmdGsensorTest(uint8_t Len, int Param)
{
	//uint8_t response;

	if (Len == 0)
	{
		ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nERROR\r\n");
		return;
	}
	else
	{
		// switch (Param)
		// {
		// case 0:
		// {
		// 	GsensorStop();
		// }
		// break;

		// case 1:
		// {
		// 	GsensorStart();
		// }
		// break;

		// case 2:
		// {
		// 	LIS2DH_GetWHO_AM_I();
		// }
		// break;

		// case 3:
		// {
		// 	GsensorGetRawAxes();
		// }
		// break;
		// }
	}
	ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nOK\r\n");
}

static void ATCmdDefGPIOREAD(uint8_t Len, int Param, uint8_t *dataBuf)
{
	bool val;

	if (Len == 0)
	{
		ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\n[%s] Error ", FmtTimeShow());
		return;
	}

	// val = IO_Read(Param);
	ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nval %d \r\n", val);
	ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nOK\r\n");
}

static void ATCmdDefGPIOWRITE(uint8_t Len, int32_t Param, uint8_t *dataBuf)
{
	if (Len == 0)
	{
		ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\n[%s] Error ", FmtTimeShow());
		return;
	}

	// if (dataBuf != NULL)
	// {
	// 	if (dataBuf[0] == '0')
	// 		IO_Write(Param, FALSE);
	// 	if (dataBuf[0] == '1')
	// 		IO_Write(Param, TRUE);
	// }

	ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nOK\r\n");
}

static void ATCmdDefWatchDog(uint8_t Len, int32_t Param, uint8_t *dataBuf)
{
	// uint32_t milliseconds;

	// if (Len == 0)
	// {
	// 	ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nERROR\r\n");
	// 	return;
	// }
	// else
	// {
	// 	switch (Param)
	// 	{
	// 	case 0:
	// 	{
	// 		WatchdogTick();
	// 		// Print Out
	// 		ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\n[%s] *WDT: Tick", FmtTimeShow());
	// 	}
	// 	break;

	// 	case 1:
	// 	{
	// 		milliseconds = atoi((const char *)dataBuf);

	// 		WatchdogEnable(milliseconds);
	// 		// Print Out
	// 		ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\n[%s] *WDT: Start with %d ms", FmtTimeShow(), milliseconds);
	// 	}
	// 	break;

	// 	default:
	// 		break;
	// 	}
	// }
	ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nOK\r\n");
}

static void ATCmdDefDbgCtl(uint8_t Len, int32_t Param, uint8_t *dataBuf)
{
	if (Len == 0)
	{
		ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nERROR\r\n");
		return;
	}
	else
	{
		uint8_t *pdbgctl = (uint8_t *)&DbgCtl;

		if (Param < (sizeof(DbgCtl)))
		{
			if (*dataBuf == '0')
			{
				pdbgctl[Param] = FALSE;
			}
			else
			{
				pdbgctl[Param] = TRUE;
			}
		}
		else
		{
			ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nParam Error\r\n");
			return;
		}
	}
	ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nOK\r\n");
}

static void ATCmdDefReadIMEI(uint8_t Len, int32_t Param)
{
	if (Len == 0)
	{
		ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nERROR\r\n");
		return;
	}
	else
	{
		switch (Param)
		{
			case 0:
				ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nIMEI(Getted In Modem Init):(%s)", IMEIBuf);
			break;

			case 1:
			{
				//Get IMEI
				RefreshImei();
				DelayMsTime(2000);
				ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nIMEI(Read From Modem):(%s)", IMEIBuf);
			}
			break;

			default:
			break;
		}
	}
	ATCmdPrintf(DbgCtl.ATCmdInfoEn, "\r\nOK\r\n");
}

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2018. All rights reserved
                                End Of The File
*******************************************************************************/
