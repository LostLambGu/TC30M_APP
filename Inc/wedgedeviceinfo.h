/*******************************************************************************
* File Name          : wedgedeviceinfo.h
* Author             : Yangjie Gu
* Description        : This file provides all the wedgedeviceinfo functions.

* History:
*  01/02/2018 : wedgedeviceinfo V1.00
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef WEDGE_DEVICE_INFO_H_
#define WEDGE_DEVICE_INFO_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "uart_api.h"
#include "wedgedatadefine.h"

/* Defines ------------------------------------------------------------------*/

extern uint8_t WedgeSetPowerLostBeforeReset(void);
extern uint8_t WedgeIsStartFromPowerLost(void);

typedef struct
{
    uint32_t timestamp;
    uint32_t verifydata;
} WEDGEDeviceInfoHeadTypedef;

extern void WedgeDeviceInfoAddrSet(uint32_t address);
extern uint8_t WedgeDeviceInfoWrite(uint8_t *pDeviceInfo, uint32_t infosize);
extern uint8_t WedgeDeviceInfoRead(uint8_t *pDeviceInfo, uint32_t infosize);

#ifdef __cplusplus
}
#endif

#endif /* WEDGE_DEVICE_INFO_H_ */

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2018. All rights reserved
                                End Of The File
*******************************************************************************/
