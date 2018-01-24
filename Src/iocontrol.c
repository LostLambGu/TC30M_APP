/*******************************************************************************
* File Name          : iocontrol.c
* Author             : Yangjie Gu
* Description        : This file provides all the iocontrol functions.

* History:
*  10/25/2017 : iocontrol V1.00
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "iocontrol.h"
#include "usrtimer.h"

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

const GpioInfoCellTypedef GpioInfoArray[WEDGE_IO_Last] = 
{
    [Vbat_On_Off_Pin] = {PB0_BAT_ROUTE_EN_GPIO_Port, PB0_BAT_ROUTE_EN_Pin},
    [MCU_PD2_IO_Pin] = {PD2_MCU_IO_GPIO_Port, PD2_MCU_IO_Pin},
    [MCU_PC11_OD_Pin] = {PC11_MCU_OD_GPIO_Port, PC11_MCU_OD_Pin},
    [MCU_PC12_Starter_Pin] = {PC12_MCU_STARTER_GPIO_Port, PC12_MCU_STARTER_Pin},
    [MCU_PA15_RELAY_Pin] = {PA15_MCU_RELAY_GPIO_Port, PA15_MCU_RELAY_Pin},
    [MCU_PC10_IGN] = {PC10_MCU_IGN_GPIO_Port, PC10_MCU_IGN_Pin},
};

uint8_t GpioOutputInputSet(WEDGEIoTypedef Io, uint8_t outin)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    if (Io >= WEDGE_IO_Last)
    {
        return 1;
    }

    if (outin == GPIO_OUT_STATUS)
    {
        GPIO_InitStruct.Pin = GpioInfoArray[Io].PINx;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(GpioInfoArray[Io].GPIOx, &GPIO_InitStruct);
        return 0;
    }
    else
    {
        GPIO_InitStruct.Pin = GpioInfoArray[Io].PINx;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GpioInfoArray[Io].GPIOx, &GPIO_InitStruct);
        return 0;
    }
}

void GpioOutputStateSet(WEDGEIoTypedef Io, GPIO_PinState state)
{
    if (Io >= WEDGE_IO_Last)
    {
        return;
    }

    WRITE_IO(GpioInfoArray[Io].GPIOx, GpioInfoArray[Io].PINx, state);
}

uint8_t GpioOutputStateGet(WEDGEIoTypedef Io)
{
    if (Io >= WEDGE_IO_Last)
    {
        return 2;
    }

    return READ_IO(GpioInfoArray[Io].GPIOx, GpioInfoArray[Io].PINx);
}

void WedgeGpsRedLedControl(uint8_t state)
{
    state++;
    state &= 0x01;
    WRITE_IO(GPIOC, PC9_LED_R_Pin, (GPIO_PinState)state);
}

uint8_t WedgeGpsRedLedStateGet(void)
{
    return READ_IO(GPIOC, PC9_LED_R_Pin);
}

void WedgeLteGreenLedControl(uint8_t state)
{
    state++;
    state &= 0x01;
    WRITE_IO(GPIOC, PC8_LED_G_Pin, (GPIO_PinState)state);
}

uint8_t WedgeLteGreenLedStateGet(void)
{
    return READ_IO(GPIOC, PC8_LED_G_Pin);
}

void WedgeLteGreenLedBlink(uint8_t state)
{
    if (state == TRUE)
    {
        SoftwareTimerStart(&LEDFlashTimer);
    }
    else
    {
        SoftwareTimerStop(&LEDFlashTimer);
    }
}

uint8_t WedgeRelayStateGet(void)
{
    return READ_IO(GPIOA, PA15_MCU_RELAY_Pin);
}

uint8_t WedgeGpsPowerStateGet(void)
{
    return READ_IO(GPIOB, PB5_GPS_PWR_EN_Pin);
}

uint8_t WedgeIgnitionPinStateGet(void)
{
    return READ_IO(GPIOC, PC10_MCU_IGN_Pin);
}

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2018. All rights reserved
                                End Of The File
*******************************************************************************/
