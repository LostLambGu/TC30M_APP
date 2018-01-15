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
