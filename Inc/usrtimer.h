/*******************************************************************************
* File Name          : UserTimer.h
* Author             : Taotao Yan
* Description        : This file provides all the UserTimer functions.

* History:
*  1/13/2016 : UserTimer V1.00
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _USRTIM_H
#define _USRTIM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

/* Defines -------------------------------------------------------------------*/
#define TimeMsec(NumMsec) ((NumMsec)*1000)

#define CHECK_UBLOX_STAT_TIMEOUT 927 //set as 910ms still has divided packet
#define CHECK_LED_FLASH_STAT_TIMEOUT 200
#define CHECK_UART3RevCDMATimer_STAT_TIMEOUT 10
#define CHECK_UART1_REC_TIMEOUT (1 * 5000)

// define softtimer structure.
typedef struct _TIMER
{
	__IO uint32_t TimeOutVal; //time out value
	uint8_t TimeId;			  // time ID
	uint32_t RecTickVal;	  //softtimer setting value
	uint8_t IsTimeOverflow;   //time out flag
	uint8_t TimerStartCounter;
	void (*Routine)(uint8_t);
} TIMER;

// Variable Declared
extern TIMER RegularTimer;
extern TIMER LEDFlashTimer;
extern TIMER AtSentToUart3Timer;
extern TIMER UART3RevCDMATimer;
extern TIMER NetworkCheckTimer;
extern TIMER ModemATInitTimer;
extern TIMER CheckRssiTimer;
extern TIMER UARTRecTimer;
extern TIMER WedgeIDLETimer;

//Function Declare
extern void InitSoftwareTimers(void);
extern void SoftwareTimerCounter(void);
extern void SoftwareCheckTimerStatus(void);
extern void SoftwareTimerStart(TIMER *timer);
extern void SoftwareTimerStop(TIMER *timer);
extern void SoftwareTimerReset(TIMER *timer, void (*Routine)(uint8_t), uint32_t timeout);
extern void SoftwareTimerCreate(TIMER *timer, uint8_t TimeId, void (*Routine)(uint8_t), uint32_t timeout);
extern uint8_t IsSoftwareTimeOut(TIMER *timer);

#ifdef __cplusplus
}
#endif

#endif

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2014. All rights reserved
                                End Of The File
*******************************************************************************/
