/*******************************************************************************
* File Name          : UserTimer.c
* Author             : Taotao Yan
* Description        : This file provides all the UserTimer functions.

* History:
*  1/13/2016 : UserTimer V1.00
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

#include "usrtimer.h"
#include "initialization.h"
#include "sendatcmd.h"
#include "parseatat.h"
#include "network.h"
#include "uart_api.h"

/*----------------------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define GET_SMS_CONTENT_TIMEOUT (1000)
/*----------------------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
TIMER RegularTimer;
TIMER LEDFlashTimer;
TIMER AtSentToUart3Timer;
TIMER UART3RevCDMATimer;
TIMER NetworkCheckTimer;
TIMER ModemATInitTimer;
TIMER CheckRssiTimer;
TIMER UARTRecTimer;

void CheckUARTRecTimerCallback(uint8_t Status)
{
}

void InitSoftwareTimers(void)
{
	// Regular Timer
	SoftwareTimerCreate(&RegularTimer, 1, CheckRegularTimerCallback, CHECK_UBLOX_STAT_TIMEOUT);
	// SoftwareTimerStart(&RegularTimer);

	// LED Flashing
	SoftwareTimerCreate(&LEDFlashTimer, 1, CheckLEDFlashTimerCallback, CHECK_LED_FLASH_STAT_TIMEOUT);
	SoftwareTimerStart(&LEDFlashTimer);

	// AT Sent To UART3
	SoftwareTimerCreate(&AtSentToUart3Timer, 1, AtSendtoUart3TimerCallback, AT_SENDTO_DELAYT_TIMEOUT);
	//SoftwareTimerStart(&AtSentToUart3Timer);

	// UART3 Rev CDMA
	SoftwareTimerCreate(&UART3RevCDMATimer, 1, CheckUART3RevCDMATimerCallback, CHECK_UART3RevCDMATimer_STAT_TIMEOUT);
	//SoftwareTimerStart(&UART3RevCDMATimer);

	// Check Network Status
	SoftwareTimerCreate(&NetworkCheckTimer, 1, CheckNetlorkTimerCallback, CHECK_NETWORK_TIMEOUT);
	//SoftwareTimerStart(&NetworkCheckTimer);

	// Modem AT Init Status
	SoftwareTimerCreate(&ModemATInitTimer, 1, ModemATInitTimerCallback, MODEM_AT_INIT_LOOP_TIMEOUT);
	//SoftwareTimerStart(&ModemATInitTimer);

	//Check RSSI
	SoftwareTimerCreate(&CheckRssiTimer, 1, CheckRssiTimerCallback, CHECK_RSSI_TIMEOUT);
	//SoftwareTimerStart(&CheckRssiTimer);

	// UARTRecTimer
	SoftwareTimerCreate(&UARTRecTimer, 1, CheckUARTRecTimerCallback, CHECK_UART1_REC_TIMEOUT);
	// SoftwareTimerStart(&UARTRecTimer);
}

void SoftwareTimerCounter(void)
{
	// Regular Timer
	if (RegularTimer.TimerStartCounter == TRUE)
		RegularTimer.TimeOutVal++;

	// LED Flashing
	if (LEDFlashTimer.TimerStartCounter == TRUE)
		LEDFlashTimer.TimeOutVal++;

	// AT Sent To UART3
	if (AtSentToUart3Timer.TimerStartCounter == TRUE)
		AtSentToUart3Timer.TimeOutVal++;

	// UART3 Rev CDMA
	if (UART3RevCDMATimer.TimerStartCounter == TRUE)
		UART3RevCDMATimer.TimeOutVal++;

	// Check Network Status
	if (NetworkCheckTimer.TimerStartCounter == TRUE)
		NetworkCheckTimer.TimeOutVal++;

	// Modem AT Init
	if (ModemATInitTimer.TimerStartCounter == TRUE)
		ModemATInitTimer.TimeOutVal++;

	//Check RSSI
	if (CheckRssiTimer.TimerStartCounter == TRUE)
		CheckRssiTimer.TimeOutVal++;

	// UARTRecTimer
	if (UARTRecTimer.TimerStartCounter == TRUE)
		UARTRecTimer.TimeOutVal++;
}

void SoftwareCheckTimerStatus(void)
{
	// Regular Timer
	if (IsSoftwareTimeOut(&RegularTimer) == TRUE)
	{
	}

	// LED Flashing
	if (IsSoftwareTimeOut(&LEDFlashTimer) == TRUE)
	{
	}

	// AT Sent To UART3
	if (IsSoftwareTimeOut(&AtSentToUart3Timer) == TRUE)
	{
	}

	// UART3 Rev CDMA
	if (IsSoftwareTimeOut(&UART3RevCDMATimer) == TRUE)
	{
	}

	// Check Network Status
	if (IsSoftwareTimeOut(&NetworkCheckTimer) == TRUE)
	{
	}

	// Modem AT Init
	if (IsSoftwareTimeOut(&ModemATInitTimer) == TRUE)
	{
	}

	//Check RSSI
	if (IsSoftwareTimeOut(&CheckRssiTimer) == TRUE)
	{
	}
}

void SoftwareTimerStart(TIMER *timer)
{
	timer->TimerStartCounter = TRUE;
}

void SoftwareTimerStop(TIMER *timer)
{
	timer->TimerStartCounter = FALSE;
}

void SoftwareTimerReset(TIMER *timer, void (*Routine)(uint8_t), uint32_t timeout)
{
	timer->TimerStartCounter = FALSE; //timer stop
	timer->RecTickVal = timeout;	  //softtimer setting value
	timer->TimeOutVal = 0;			  //time out value
	timer->IsTimeOverflow = FALSE;	//time out flag
	timer->Routine = Routine;
}

void SoftwareTimerCreate(TIMER *timer, uint8_t TimeId, void (*Routine)(uint8_t), uint32_t timeout)
{
	timer->TimeId = TimeId;
	timer->RecTickVal = timeout;	  //softtimer setting value
	timer->TimeOutVal = 0;			  //time out value
	timer->IsTimeOverflow = FALSE;	//time out flag
	timer->TimerStartCounter = FALSE; //timer stop
	timer->Routine = Routine;
}

uint8_t IsSoftwareTimeOut(TIMER *timer)
{
	//ET0 = 0;
	if (timer->IsTimeOverflow == FALSE)
	{
		//After gSysTick and timer->TimeOutVal overflow,
		//the software timer function can still work well
		//the next statement is equivalent to:
		//(gSysTick - timer->TimeOutVal) < 0x80000000
		if (timer->TimeOutVal > timer->RecTickVal)
		{
			timer->TimerStartCounter = FALSE;
			timer->IsTimeOverflow = TRUE;
			timer->Routine(timer->TimeId);
		}
	}
	//ET0 = 1;

	return timer->IsTimeOverflow;
}

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2017. All rights reserved
                                End Of The File
*******************************************************************************/
