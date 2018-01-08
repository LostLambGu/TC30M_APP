/*******************************************************************************
* File Name          : iocontrol.h
* Author             : Yangjie Gu
* Description        : This file provides all the iocontrol functions.

* History:
*  10/25/2017 : iocontrol V1.00
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __IOCONTROL_H__
#define __IOCONTROL_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

#define READ_IO(port, pin) ((uint8_t)HAL_GPIO_ReadPin(port, pin))
#define WRITE_IO(port, pin, state) HAL_GPIO_WritePin(port, pin, state)

extern void WedgeGpsRedLedControl(uint8_t state);
extern uint8_t WedgeGpsRedLedStateGet(void);
extern void WedgeLteGreenLedControl(uint8_t state);
extern uint8_t WedgeLteGreenLedStateGet(void);
extern void WedgeLteGreenLedBlink(uint8_t state);
extern uint8_t WedgeRelayStateGet(void);
extern uint8_t WedgeGpsPowerStateGet(void);
extern uint8_t WedgeIgnitionStateGet(void);

#endif /* __IOCONTROL_H__ */

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2018. All rights reserved
                                End Of The File
*******************************************************************************/
