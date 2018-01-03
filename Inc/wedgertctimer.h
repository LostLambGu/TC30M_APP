/*******************************************************************************
* File Name          : wedgertctimer.h
* Author             : Yangjie Gu
* Description        : This file provides all the wedgertctimer functions.

* History:
*  12/31/2017 : wedgertctimer V1.00
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef WEDGE_RTC_TIMER_H__
#define WEDGE_RTC_TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "uart_api.h"

#include "wedgedatadefine.h"

#define WEDGE_RTC_TIMER_INSTANCE_MAX (8)

typedef enum
{
    WEDGE_RTC_TIMER_INVALID = 0,
    WEDGE_RTC_TIMER_PERIODIC,
    WEDGE_RTC_TIMER_ONETIME,

    WEDGE_RTC_TIMER_MAX
} WEDGERTCTimerTypeDef;

typedef enum
{
    Periodic_Moving_Event = 0,
    Periodic_OFF_Event,
    Periodic_Health_Event,
    Stop_Report_Onetime_Event,
    Periodic_Hardware_Reset_Onetime,

    WEDGE_RTC_TIMER_INSTANCE_INVALID_MAX
} WEDGERTCTimerInstanceTypeDef;

typedef struct
{
    WEDGERTCTimerTypeDef RTCTimerType;
    WEDGERTCTimerInstanceTypeDef RTCTimerInstance;
    uint32_t settime;
} RTCTimerListCellTypeDef;

typedef struct
{
    uint8_t instancenum;
    RTCTimerListCellTypeDef currentinstance;
    RTCTimerListCellTypeDef instancearray[WEDGE_RTC_TIMER_INSTANCE_MAX];
} RTCTimerListTypeDef;

typedef enum
{
    WEDGE_RTC_TIMER_MODIFY_INCREASE = 0,
    WEDGE_RTC_TIMER_MODIFY_DECREASE,

    WEDGE_RTC_TIMER_MODIFY_INVALID_MAX
} WEDGERTCTimerSettimeTypeDef;

extern void WedgeSetRTCAlarmStatus(uint8_t Status);
extern uint8_t WedgeGetRTCAlarmStatus(void);
extern uint32_t WedgeRtcCurrentSeconds(void);
extern uint8_t WedgeRtcTimerInit(void *pRTCTimerList);
extern void WedgeRTCTimerListGet(uint8_t *pBuf, uint32_t *pSize);
extern uint8_t WedgeRtcTimerInstanceAdd(RTCTimerListCellTypeDef Instance);
extern uint8_t WedgeRtcTimerInstanceDel(WEDGERTCTimerInstanceTypeDef InstanceType);
extern uint8_t WedgeRtcTimerModifySettime(uint32_t Delta, WEDGERTCTimerSettimeTypeDef ModifyType);

#ifdef __cplusplus
}
#endif

#endif /* WEDGE_RTC_TIMER_H__ */

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2018. All rights reserved
                                End Of The File
*******************************************************************************/
