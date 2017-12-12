/*******************************************************************************
* File Name          : rtcclock.h
* Author             : Jevon
* Description        : This file provides all the rtcclock functions.

* History:
*  04/01/2015 : rtcclock V1.00
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _REALTIME_CLOCK_H
#define _REALTIME_CLOCK_H
/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

#define SYNCHRONIZATION_TIME_FROM_MODEM 1

#define LEAP_YEAR  			366
#define NORM_YEAR  			365
#define BASE_YEAR 			1980
#define LEAP_LOOP       			(366+(3*365))
#define BASE_OFFSET 			432000
#define SECONDS_PER_YEAR 	 (86400*365)
#define WAKE_S1_FOR_SECS	(1*60)
#define WAKE_S2_FOR_SECS	(4*60)
#define RTC_DEFAULT_TIME 		{2001,1,1,1,1,1,0}


#define DEFAULT_KISSOFF_TIME 			90
#define DEFAULT_KISSOFF_TIMS 			10
#define DEFAULT_TEMP_DETECT 				1800	// 30min * 60secs

typedef enum 
{
  ALM_DATETIME = 0,
  ALM_WEEKDAY,

  ALM_UNKNOW
} AlmModeT;

typedef struct {
  uint32_t year; // 2015
  uint32_t month;
  uint32_t day;
  uint32_t hour;
  uint32_t minute;
  uint32_t second;
  uint32_t subses;
}TimeTableT;

// Variable declared
//Function Declare
extern void SetRTCAlarmStatus(uint8_t Status);
extern uint8_t GetRTCAlarmStatus(void);
extern int32_t datetoweek(int32_t yr, int32_t mn,int32_t day);
extern void DectoBCD(int32_t Dec, uint8_t *Bcd, int32_t length);
extern uint32_t Divide(uint32_t Second, uint32_t value, uint32_t *returnValue);
extern int8_t GetMonthFromDays(uint32_t days,uint32_t year, uint32_t *returnDays);
extern TimeTableT SecondsToTimeTable(uint32_t seconds);
extern uint32_t TimeTableToSeconds(TimeTableT timeTable);
extern char *FmtTimeShow(void);
extern TimeTableT GetRTCDatetime(void);
extern int32_t SetRTCDatetime(TimeTableT *tm);
extern uint8_t SetRTCAlarmTime(uint32_t seconds, uint8_t Status);

#endif  /* _REALTIME_CLOCK_H */

/*******************************************************************************
End Of The File
*******************************************************************************/

