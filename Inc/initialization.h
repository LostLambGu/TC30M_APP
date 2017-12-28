/*******************************************************************************
* File Name          : initialization.h
* Author             : Yangjie Gu
* Description        : This file provides all the initialization functions.

* History:
*  10/18/2017 : initialization V1.00
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _INITIALIZATION_SOFTWARE_H
#define _INITIALIZATION_SOFTWARE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
#include "usrtimer.h"

/* Defines -------------------------------------------------------------------*/
#define MODEM_AT_INIT_DELAY_TIMEOUT 	(5*1000)
#define MODEM_AT_INIT_LOOP_TIMEOUT (100)
#define MODEM_WAITING_DELAY_TIMEOUT 	(10*1000)
#define AIRPLANE_MODE_DELAY_TIMEOUT (15000)
#define STRAT_BLE_SCAN_TIMEOUT (5000)
#define INIT_CLOCK_TIMEOUT (30000)
#define CHECK_RSSI_TIMEOUT (30000)

#define min(X, Y) ((X) < (Y) ? (X) : (Y))
#define max(X, Y) ((X) > (Y) ? (X) : (Y))

#define FunStates FunctionalState

#define LTE_AT_PORT_READY()	(HAL_GPIO_ReadPin(PB13_MCU_CTS3_GPIO_Port, PB13_MCU_CTS3_Pin) == GPIO_PIN_RESET)

// Variable Declared
extern uint8_t Lis2dhMemsChipID;
extern uint8_t gRedLEDFlashingFlag;
extern uint8_t gGreenLEDFlashingFlag;

//Function Declare
extern void DelayMsTime(uint16_t delay_time); //delay delay_time * 1ms
extern void StringToUper(char* s);
extern void SystemDisableAllInterrupt(void);
extern void SystemEnableAllInterrupt(void);
extern void ModemPowerEnControl(FunStates Status);
extern void ModemRTSEnControl(FunStates Status);
extern void SystemInitialization(void);
extern void CheckRegularTimerCallback(uint8_t Status);
extern void CheckLEDFlashTimerCallback(uint8_t Status);
extern void CheckUART3RevCDMATimerCallback(uint8_t Status);
extern void ModemATInitTimerCallback(uint8_t Status);
extern void UpdateSystemSignalStatus(uint8_t Type);
extern void CheckRssiTimerCallback(uint8_t status);

#ifdef __cplusplus
}
#endif

#endif /* _INITIALIZATION_SOFTWARE_H */

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2017. All rights reserved
                                End Of The File
*******************************************************************************/
