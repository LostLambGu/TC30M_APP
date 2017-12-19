/*******************************************************************************
* File Name          : initialization.c
* Author             : Yangjie Gu
* Description        : This file provides all the initialization functions.

* History:
*  10/18/2017 : initialization V1.00
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

#include "uart_api.h"
#include "initialization.h"
#include "network.h"
#include "sendatcmd.h"
#include "rtcclock.h"
#include "usrtimer.h"
#include "ublox_driver.h"
#include "parseatat.h"
#include "iqmgr.h"
#include "version.h"
#include "ltecatm.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define NRCMD (1)

#define InitPrintf DebugPrintf
#define InitLog DebugLog
/*----------------------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
uint8_t gRedLEDFlashingFlag = FALSE;
uint8_t gGreenLEDFlashingFlag = FALSE;
uint8_t Lis2dhMemsChipID = 0;

extern uint8_t ModemPowerOnFlag;

static uint8_t AccIntHappenStatus = FALSE;

/* Public functions ----------------------------------------------------------*/
// DelayUsTime
void DelayUsTime(uint16_t time) //delay 5+2*time (machine time)
{
	uint16_t Count;
	Count = time;
	while (--Count)
		;
	while (--time)
		;
}

// DelayMsTime
void DelayMsTime(uint16_t delay_time) //delay delay_time * 1ms
{
	uint16_t i;
	uint8_t j;

	if (delay_time == 0)
		return;

	for (i = 0; i < delay_time; i++)
	{
		for (j = 0; j < 10; j++)
		{
			DelayUsTime(155);
		}
	}
}

uint8_t ToUperChar(uint8_t ch)
{
   if( ch >='a' && ch <= 'z' )
      return (uint8_t)( ch + ('A' - 'a') );
   return ch;
}

void StringToUper(char* s)
{
   uint16_t len = strlen(s);
   uint16_t idx;

   for(idx = 0; idx < len; idx++)
	s[idx] = ToUperChar(s[idx]);
}

void SystemDisableAllInterrupt(void)
{
	// portDISABLE_INTERRUPTS();
	__asm volatile( "cpsid i" );
}

void SystemEnableAllInterrupt(void)
{
	// portENABLE_INTERRUPTS();
	__asm volatile( "cpsie i" );
}

void SetAccIntHappenStatus(uint8_t status)
{
	AccIntHappenStatus = status;
}

uint8_t GetAccIntHappenStatus(void)
{
	return AccIntHappenStatus;
}

void ModemPowerEnControl(FunStates Status)
{
	if (Status == ENABLE)
	{
		// Active
		HAL_GPIO_WritePin(LTE_PWR_PORT, LTE_PWR_PIN, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LTE_RST_PORT, LTE_RST_PIN, GPIO_PIN_SET);
		HAL_GPIO_WritePin(MODEM_PWR_ON_PORT, MODEM_PWR_ON_PIN, GPIO_PIN_SET);
		ModemPowerOnFlag = TRUE;
	}
	else if (Status == DISABLE)
	{
		// Inactivation
		HAL_GPIO_WritePin(LTE_PWR_PORT, LTE_PWR_PIN, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LTE_RST_PORT, LTE_RST_PIN, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MODEM_PWR_ON_PORT, MODEM_PWR_ON_PIN, GPIO_PIN_RESET);
		// Clear Flag
		ModemPowerOnFlag = FALSE;
		// Clear Status
		SetModemATPortStat(FALSE);
		SetNetworkRssiValue(DEFAULT_RSSI_VALUE);
		SetNetworkReadyStat(FALSE);
		//Reset Network State Machine
		SetNetworkMachineStatus(NET_FREE_IDLE_STAT);
		// Stop Network Timer
		SoftwareTimerReset(&NetworkCheckTimer, CheckNetlorkTimerCallback, CHECK_NETWORK_TIMEOUT);
		SoftwareTimerStop(&NetworkCheckTimer);
	}
}

void ModemRTSEnControl(FunStates Status)
{
	if (Status == ENABLE)
	{
		// Active
		HAL_GPIO_WritePin(PB14_MCU_RTS3_GPIO_Port, PB14_MCU_RTS3_Pin, GPIO_PIN_RESET);
	}
	else if (Status == DISABLE)
	{
		// Inactivation
		HAL_GPIO_WritePin(PB14_MCU_RTS3_GPIO_Port, PB14_MCU_RTS3_Pin, GPIO_PIN_SET);
	}
}

static const char *monthstr[] = {
	"Err",
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
};

static void CompileSetRTCTime(void)
{
	TimeTableT tm = {0};
	char time[16] = {0};
	char date[16] = {0};
	char *timetemp = time;
	char *datetemp = date;
	char month[4] = {0};
	uint8_t i = 0;

	memcpy(time, __TIME__, strlen(__TIME__));
	memcpy(date, __DATE__, strlen(__DATE__));

	tm.hour = (timetemp[0] - '0') * 10 + timetemp[1] - '0';
	tm.minute = (timetemp[3] - '0') * 10 + timetemp[4] - '0';
	tm.second = (timetemp[6] - '0') * 10 + timetemp[7] - '0';

	month[0] = *datetemp++;
	month[1] = *datetemp++;
	month[2] = *datetemp++;
	datetemp++;
	for (i = 0; i < (sizeof(monthstr) / sizeof(*monthstr)); i++)
	{
		if (strcmp(month, monthstr[i]) == 0)
		{
			break;
		}
	}
	tm.month = i;

	tm.day = ((datetemp[0] >= '0' && datetemp[0] <= '3') ? (datetemp[0] - '0') * 10 : 0) + datetemp[1] - '0';
	datetemp += 3;

	tm.year = (datetemp[0] - '0') * 1000 + (datetemp[1] - '0') * 100 + (datetemp[2] - '0') * 10 + datetemp[3] - '0';

	SetRTCDatetime(&tm);
}

void SystemInitialization(void)
{
	// Init LED
	HAL_GPIO_WritePin(PC9_LED_R_GPIO_Port, PC9_LED_R_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(PC8_LED_G_GPIO_Port, PC8_LED_G_Pin, GPIO_PIN_RESET);
	gRedLEDFlashingFlag = TRUE;
	gGreenLEDFlashingFlag = TRUE;

	// Show Version
	ShowSoftVersion();

	//Set RTC time using compile time
	CompileSetRTCTime();

	// Ublox Init
	UbloxI2cInit();

	// UartBufferInit();
	// Timer Init
	InitSoftwareTimers();
	aiDFdhInit();
	parseAtDHInit();
	IqmgrMutexInit();

	// Eanble MCU RTS
	ModemRTSEnControl(ENABLE);
	DelayMsTime(1000);
	// OPEN Modem
	ModemPowerEnControl(ENABLE);

	// Close GPS
	UbloxPowerEnControl(DISABLE);

	//Extend Output pin

	// Delay
	DelayMsTime(4);

	// Lis2dhMemsChipID = AccelReadRegister(LIS2DH_MEMS_I2C_ADDRESS, LIS2DH_WHO_AM_I);
	// if (Lis2dhMemsChipID == WHOAMI_LIS2DH_ACC) //LIS2DH_MEMS_I2C_ADDRESS
	// {
	// 	InitLog("\r\nInit: Gsensor ID,0x%X\r\n", Lis2dhMemsChipID);
	// 	// Gsensor settings
	// 	/*GSensorI2cInit();*/

	// 	/*comment to eliminate errors*/
	// 	/* comment atel_ related lines */
	// 	// atel_gsensor_stop();
	// 	LIS2DH_SetMode(LIS2DH_POWER_DOWN);
	// }
}

extern uint8_t ATUbloxTestFlag;
void CheckRegularTimerCallback(uint8_t Status)
{
	if (ATUbloxTestFlag == TRUE)
	{
		ReadUbloxData();
		UbloxGPSTimerProcess();

		// // Reset Timer
		SoftwareTimerReset(&RegularTimer, CheckRegularTimerCallback, UbloxCheckStatTimeout);
		SoftwareTimerStart(&RegularTimer);
	}
}

void CheckLEDFlashTimerCallback(uint8_t Status)
{
	if (gRedLEDFlashingFlag == TRUE)
	{
		HAL_GPIO_TogglePin(PC9_LED_R_GPIO_Port, PC9_LED_R_Pin);
	}

	if (gGreenLEDFlashingFlag == TRUE)
	{
		HAL_GPIO_TogglePin(PC8_LED_G_GPIO_Port, PC8_LED_G_Pin);
	}

	// Reset Timer
	SoftwareTimerReset(&LEDFlashTimer, CheckLEDFlashTimerCallback, CHECK_LED_FLASH_STAT_TIMEOUT);
	SoftwareTimerStart(&LEDFlashTimer);
}

void CheckUART3RevCDMATimerCallback(uint8_t Status)
{
	if (Uart3RxCount > 0)
	{
		UART3_RX_NUM = TRUE;
	}
}

void ModemATInitTimerCallback(uint8_t Status)
{
	static uint8_t CheckSimCardCount = 0;
	// Init once
	if(GetModemNeedInitStat() == TRUE)
	{
		SetModemNeedInitStat(FALSE);
		// Print Out
		InitPrintf(NRCMD,"\r\n[%s] INIT:lte init", FmtTimeShow());
		// Echo off
		SendATCmd(GSM_CMD_ATE0, GSM_CMD_TYPE_EXECUTE, NULL);	
		//Enable protocol  -- send out -- halt fault
		SendATCmd(GSM_CMD_CFUN, GSM_CMD_TYPE_EVALUATE , "1");
		// Reset AT Init Timer
		SoftwareTimerReset(&ModemATInitTimer,ModemATInitTimerCallback, MODEM_WAITING_DELAY_TIMEOUT);
		SoftwareTimerStart(&ModemATInitTimer);
	}
	else
	{
		// Check sim card
		if(GetSimCardReadyStat() == FALSE)
		{
			if(CheckSimCardCount < 3)
			{
				CheckSimCardCount++;
				// Get sim state
				SendATCmd(GSM_CMD_CPIN, GSM_CMD_TYPE_QUERY, NULL);	
				// Reset AT Init Timer
				SoftwareTimerReset(&ModemATInitTimer,ModemATInitTimerCallback,MODEM_AT_INIT_DELAY_TIMEOUT);
				SoftwareTimerStart(&ModemATInitTimer);
				// Print Out
				InitPrintf(NRCMD,"\r\n[%s] INIT:check sim time %d", FmtTimeShow(), CheckSimCardCount);
			}
		}
		else
		{
			// Print Out
			InitPrintf(NRCMD,"\r\n[%s] INIT:sim card ready!", FmtTimeShow());
			// Clear counter
			CheckSimCardCount = 0;
			// Enable network registration state unsolicited
			SendATCmd(GSM_CMD_CEREG, GSM_CMD_TYPE_EVALUATE, "1");
			// Message format -- (1 text mode)
			SendATCmd(GSM_CMD_CMGF, GSM_CMD_TYPE_EVALUATE , "1");
			// New message indications -- (3,1,0,0,1 --> 2,1,0,0,1)
			// SendATCmd(GSM_CMD_CNMI, GSM_CMD_TYPE_EVALUATE , "2,2,0,0,1");
			SendATCmd(GSM_CMD_CNMI, GSM_CMD_TYPE_EVALUATE , "2,1,0,0,1");
			
			// // 0 disable +CME ERROR: <err> result code and use ERROR instead
			// // 1 enable+CME ERROR: <err> result code and use numeric <err> values
			// // 2 enable +CME ERROR: <err> result code and use verbose<err> values
			// SendATCmd(GSM_CMD_CMEE, GSM_CMD_TYPE_EVALUATE , "1");
			//Get IMEI
			SendATCmd(GSM_CMD_IMEI, GSM_CMD_TYPE_EVALUATE, "1");
			// //Get IMSI
			// SendATCmd(GSM_CMD_IMSI, GSM_CMD_TYPE_EXECUTE, NULL);
			// Get the version number
			// SendATCmd(GSM_CMD_I1, GSM_CMD_TYPE_EXECUTE, NULL);	
			// Get network registration state
			// SendATCmd(GSM_CMD_CEREG, GSM_CMD_TYPE_QUERY, NULL);
			// Get the current system time
			//SendATCmd(GSM_CMD_CCLK, GSM_CMD_TYPE_QUERY, NULL);
			// Get time zone	
			// SendATCmd(GSM_CMD_CTZR, GSM_CMD_TYPE_QUERY, NULL);
			// Get the current system time
			// SendATCmd(GSM_CMD_CCLK, GSM_CMD_TYPE_QUERY, NULL);
			//get ICCID
			SendATCmd(GSM_CMD_CRSM,GSM_CMD_TYPE_EVALUATE,"176,12258,0,0,10");
			// Signal quality
			SendATCmd(GSM_CMD_CSQ, GSM_CMD_TYPE_EXECUTE, NULL);
			// Extended signal quality
			// SendATCmd(GSM_CMD_CESQ, GSM_CMD_TYPE_EXECUTE, NULL);

			// // Print Out
			// InitPrintf(NRCMD,"\r\n[%s] INIT: LTE INIT BT(%d) Qos(%d) PPP(%d %d)", \
			// 	FmtTimeShow(), \
			// 	gDeviceConfig.ExtendedBT.ConnectScanTim, \
			// 	gDeviceConfig.GpsQosCfgValue, \
			// 	gDeviceConfig.PPPEstablishCounter, \
			// 	gDeviceConfig.PPPEstablishReportFlag);

			// Reset Network Timer
			SoftwareTimerReset(&NetworkCheckTimer,CheckNetlorkTimerCallback,CHECK_NETWORK_TIMEOUT);
			SoftwareTimerStart(&NetworkCheckTimer);
		}
	}
}

void UpdateSystemSignalStatus(uint8_t Type)
{
	if(GetModemPoweronStat() == TRUE && \
		GetSimCardReadyStat() == TRUE)
	{
		// get network register state
		SendATCmd(GSM_CMD_CEREG, GSM_CMD_TYPE_QUERY, NULL);
	        // Check vser
//		SendATCmd(GSM_CMD_VSER, GSM_CMD_TYPE_QUERY, NULL);
		// Get the signal
		SendATCmd(GSM_CMD_CSQ, GSM_CMD_TYPE_EXECUTE, NULL);
		// Extended signal quality
//		SendATCmd(GSM_CMD_CESQ, GSM_CMD_TYPE_EXECUTE, NULL);
		// Print Out
		InitPrintf(NRCMD,"\r\n[%s] INIT: Update Net Service Valid(%d)", FmtTimeShow(),Type);
	}
	else
	{
		// Print Out
		InitPrintf(NRCMD,"\r\n[%s] INIT: Update Net Service Forbid(%d %d %d)", \
			FmtTimeShow(),Type, \
			GetModemPoweronStat(), \
			GetSimCardReadyStat());
	}
}

void CheckRssiTimerCallback(uint8_t status)
{
	UpdateSystemSignalStatus(1);

	SoftwareTimerReset(&CheckRssiTimer, CheckRssiTimerCallback, CHECK_RSSI_TIMEOUT);
	SoftwareTimerStart(&CheckRssiTimer);
}

void HardFault_Handler(void)
{
	uint32_t r_sp;
	r_sp = __get_PSP(); // Get SP Value

	/* __asm{
  	MOV v_r14, R14;
	  }*/

	DebugLog("\r\n[%s] Init: HardFault! SP(%lu)(%lx)", FmtTimeShow(), r_sp, r_sp);
	while (1)
		;
}

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2017. All rights reserved
                                End Of The File
*******************************************************************************/
