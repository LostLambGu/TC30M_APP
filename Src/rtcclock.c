/*******************************************************************************
* File Name          : rtcclock.c
* Author               : Jevon
* Description        : This file provides all the rtcclock functions.

* History:
*  04/01/2015 : rtcclock V1.00
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "rtc.h"
#include "rtcclock.h"
#include "uart_api.h"

/* Private typedef -----------------------------------------------------------*/
#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define RTCTimPrintf DebugPrintf

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const char *weekstring[]={"Sun","Mon","Tue","Wed","Thu","Fri","Sat","Sun"};
static const int8_t norm_month_table[] = {31,28,31,30,31,30,31,31,30,31,30,31};
static char SysDate[25] = {'\0'};
static uint8_t RTCAlarmHappenFlag = FALSE;
/* Public variables ---------------------------------------------------------*/

/* Public function prototypes -----------------------------------------------*/
extern RTC_HandleTypeDef hrtc;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*
const char *wday[]={"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
const char *month[] = {"Err", 
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
};

static uint8_t isleapyear(int32_t year)
{
	return (!(year % 4) && (year % 100)) || !(year % 400);
}

uint8_t bcd2bin(uint8_t val)
{
	return (val & 0x0f) + (val >> 4) * 10;
}

uint8_t bin2bcd(uint8_t val)
{
	return ((val / 10) << 4) + val % 10;
}
*/

void SetRTCAlarmStatus(uint8_t Status)
{
	RTCAlarmHappenFlag = Status;
}

uint8_t GetRTCAlarmStatus(void)
{
	return RTCAlarmHappenFlag;
}

int32_t datetoweek(int32_t yr, int32_t mn,int32_t day)
{
	int32_t i;
	int32_t days=0;
	int32_t s;
	int32_t week;
	int32_t mont[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
	
	if((0==yr%4 && 0!=yr%100)||0==yr%400)
		mont[2]=29;
	else
		mont[2]=28;
	for (i=0;i<mn;i++)
	days+=mont[i];
	days+=day;
	s=yr-1+(int32_t)((yr-1)/4)-(int32_t)((yr-1)/100)+(int32_t)((yr-1)/400)+days;
	week=s%7;
	//if(gDeviceConfig.DbgCtl.RTCDebugInfoEn == TRUE)
	//RTCTimPrintf(DbgCtl.RTCDebugInfoEn,"%d-%d-%d is [%d].\n",yr,mn,day,week);
	return week;
}


void DectoBCD(int32_t Dec, uint8_t *Bcd, int32_t length)
{
	int32_t i;
	int32_t temp;
	for(i=length-1; i>=0; i--)
	{
		temp = Dec%100;
		Bcd[i] = ((temp/10)<<4) + ((temp%10) & 0x0F);
		Dec /= 100;
	}
}

uint32_t Divide(uint32_t Second, uint32_t value, uint32_t *returnValue)
{
  *returnValue = Second%value;
  return Second/value;
}

int8_t GetMonthFromDays(uint32_t days,uint32_t year, uint32_t *returnDays)
{
   int32_t i =0;
   uint32_t totalday = 0;
   for(i =0; i < 12 ; i++) {
     if(days < totalday) {
       break;
     }
     if(((year&0x3) == 0)&&(i == 1))
       totalday += 29;
     else 
       totalday += norm_month_table[i];
   }
   if(((year&0x3) == 0)&&(i == 2))
      *returnDays = days - totalday + 29 + 1;
   else 
      *returnDays = days - totalday + norm_month_table[i - 1] + 1;
   return i;
}

TimeTableT SecondsToTimeTable(uint32_t seconds)
{
  TimeTableT timeTable;
  uint32_t days;
  
  seconds+= BASE_OFFSET;

  //get seconds, minute and hour;
  seconds = Divide(seconds, 60, &timeTable.second);
  seconds = Divide(seconds, 60, &timeTable.minute);
  seconds = Divide(seconds, 24, &timeTable.hour);

  //count how many leap_loop be included;
  uint32_t leap = Divide(seconds, LEAP_LOOP, &days);
  timeTable.year = BASE_YEAR + 4*leap;

  //get surplus days to determine the appointed year;
  if(days < 366) {
  }
  else if(days < (366 + 365)) {
    timeTable.year+=1;
    days -= (366);
  }
  else if(days < (366 + 365*2)) {
    timeTable.year+=2; 
    days -= (366 + 365);
  }
  else if(days < (366 + 365*3)) {
    timeTable.year+=3;
    days -= (366 + 365*2);
  }
  
  timeTable.month= GetMonthFromDays(days,timeTable.year,&timeTable.day);
  return timeTable;
}

uint32_t TimeTableToSeconds(TimeTableT timeTable)
{
  uint32_t  seconds =0;  
  int32_t i;

  //get total days from 1980 not included nowyear
  for(i = BASE_YEAR;i < timeTable.year; i++) {

    //if %4 != 0;
    if((i&0x3)!= 0) {
      seconds += NORM_YEAR;
    }
    else {
      seconds += LEAP_YEAR;
    }
  }
  //get nowyears total days not included this month;
  for(i = 1; i < timeTable.month; i++) {
    //if leap year and 2th month;
    if(((timeTable.year&0x3) == 0)&&(i == 2))
       seconds += 29;
    else 
       seconds += norm_month_table[i - 1];
  }
  //get this month's days;
  seconds += timeTable.day - 1;
  seconds = seconds * 24 + timeTable.hour;
  seconds = seconds * 60 + timeTable.minute;
  seconds = seconds * 60 + timeTable.second;
  
  seconds -= BASE_OFFSET;
  return seconds;   
}

char *FmtTimeShow(void)
{
	//int32_t week = 0;
	char *tstr = SysDate;
	TimeTableT timeTable = GetRTCDatetime();
	/* Display time Format : hh:mm:ss */
	//week = datetoweek(timeTable.year, timeTable.month, timeTable.day);
	/* Show Time */
	sprintf(tstr,"%02d/%02d/%02d %02d:%02d:%02d:%03d", \
		timeTable.month, \
		timeTable.day, \
		(timeTable.year - 2000), \
		/*week, */\
		timeTable.hour, \
		timeTable.minute, \
		timeTable.second, \
		timeTable.subses);
	return tstr;
}

TimeTableT GetRTCDatetime(void)
{
  TimeTableT timeTable;
  RTC_DateTypeDef sdatestructureget = {0};
  RTC_TimeTypeDef stimestructureget = {0};
  
  /* Get the RTC current Time */
  HAL_RTC_GetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&hrtc, &sdatestructureget, RTC_FORMAT_BIN);
  /* Display time Format : hh:mm:ss */

  timeTable.year = sdatestructureget.Year+2000;
  timeTable.month = sdatestructureget.Month;
  timeTable.day = sdatestructureget.Date;
  timeTable.hour = stimestructureget.Hours;
  timeTable.minute = stimestructureget.Minutes;
  timeTable.second = stimestructureget.Seconds;
  timeTable.subses = stimestructureget.SubSeconds;

  return timeTable;
}

int32_t SetRTCDatetime(TimeTableT *tm)
{
  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  // Data
  if(tm->year < 1900)
  	DectoBCD(tm->year+2000, &sDate.Year, 1);
  else
  	DectoBCD(tm->year, &sDate.Year, 1);
  DectoBCD(tm->month, &sDate.Month, 1);
  DectoBCD(tm->day, &sDate.Date, 1);
  // Time
  DectoBCD(tm->hour, &sTime.Hours, 1);
  DectoBCD(tm->minute, &sTime.Minutes, 1);
  DectoBCD(tm->second, &sTime.Seconds, 1);
  // Week
  sDate.WeekDay =  datetoweek(tm->year, tm->month, tm->day);
  if(sDate.WeekDay == 0)
  	sDate.WeekDay = 7;
//if(gDeviceConfig.DbgCtl.RTCDebugInfoEn == TRUE)
	RTCTimPrintf(DbgCtl.RTCDebugInfoEn,"\r\n UpdateRtcTime: %X/%X/%X %X:%X:%X Week[%X]", \
	sDate.Month,sDate.Date,sDate.Year, \
	sTime.Hours,sTime.Minutes,sTime.Seconds, \
	sDate.WeekDay);
  //Set Time: Hour:Minure:Second
  //sTime.Hours = 0x10;
  //sTime.Minutes = 0x10;
  //sTime.Seconds = 0x00;
  sTime.SubSeconds = 0;
  #ifdef RTC_TIME_12HOUR_FORMAT
  sTime.TimeFormat = RTC_HOURFORMAT12_AM;
  #endif
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
  /* Set Date: Week Month Day Year */
  //sDate.WeekDay = RTC_WEEKDAY_THURSDAY;
  //sDate.Month = RTC_MONTH_MARCH;
  //sDate.Date = 0x26;
  //sDate.Year = 0x15;
  HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
  
  return 0;
}

uint8_t SetRTCAlarmTime(uint32_t seconds, uint8_t Status)
{
	uint8_t ErrCode = 0;
	uint8_t BCDday;
	uint8_t BCDhour;
	uint8_t BCDmin;
	uint8_t BCDSec;
	uint32_t  CurrentSecs =0;  
	TimeTableT    Timetable;
	RTC_AlarmTypeDef sAlarm;
	// Get seconds of current time
	CurrentSecs = TimeTableToSeconds(GetRTCDatetime());
	CurrentSecs += seconds;
	Timetable =  SecondsToTimeTable(CurrentSecs);
  	// Converted to BCD Code
	DectoBCD(Timetable.day, &BCDday, 1);
	DectoBCD(Timetable.hour, &BCDhour, 1);
	DectoBCD(Timetable.minute, &BCDmin, 1);
	DectoBCD(Timetable.second, &BCDSec, 1);
	// Set Alarm Values
	sAlarm.AlarmTime.Hours = BCDhour;
	sAlarm.AlarmTime.Minutes = BCDmin;
	sAlarm.AlarmTime.Seconds = BCDSec;
	sAlarm.AlarmTime.SubSeconds = 0x00;
	#ifdef RTC_TIME_12HOUR_FORMAT
	sAlarm.AlarmTime.TimeFormat = RTC_HOURFORMAT12_AM;
	#endif
	sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
	sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
	sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
	sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
	sAlarm.AlarmDateWeekDay = BCDday;
	sAlarm.Alarm = RTC_ALARM_A;
	ErrCode = (uint8_t)HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD);
	// Print Out
	RTCTimPrintf(DbgCtl.RTCDebugInfoEn,"\r\n[%s] RTC: Almset(%d)Next(%02X %02X:%02X:%02X)Err(%d)", \
		FmtTimeShow(), \
		seconds, \
		BCDday, \
		BCDhour, \
		BCDmin, \
		BCDSec, \
		ErrCode);
	return ErrCode;
}

/*******************************************************************************
End Of The File
*******************************************************************************/

