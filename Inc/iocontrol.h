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

#define GPIO_OUT_STATUS (1)
#define GPIO_IN_STATUS (0)

typedef struct
{
    GPIO_TypeDef  *GPIOx;
    uint16_t PINx;
} GpioInfoCellTypedef;

typedef enum
{
    Vbat_On_Off_Pin = 0,
    MCU_PD2_IO_Pin,
    MCU_PC11_OD_Pin,
    MCU_PC12_Starter_Pin,
    MCU_PA15_RELAY_Pin,
    MCU_PC10_IGN,

    WEDGE_IO_Last
} WEDGEIoTypedef;

extern uint8_t GpioOutputInputSet(WEDGEIoTypedef Io, uint8_t outin);
extern void GpioOutputStateSet(WEDGEIoTypedef Io, GPIO_PinState state);
extern uint8_t GpioStateGet(WEDGEIoTypedef Io);

extern void WedgeGpsRedLedControl(uint8_t state);
extern uint8_t WedgeGpsRedLedStateGet(void);
extern void WedgeLteGreenLedControl(uint8_t state);
extern uint8_t WedgeLteGreenLedStateGet(void);
extern void WedgeLteGreenLedBlink(uint8_t state);
extern uint8_t WedgeRelayStateGet(void);
extern uint8_t WedgeGpsPowerStateGet(void);
extern uint8_t WedgeIgnitionPinStateGet(void);

#endif /* __IOCONTROL_H__ */

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2018. All rights reserved
                                End Of The File
*******************************************************************************/
