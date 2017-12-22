/*******************************************************************************
* File Name          : ublox_driver.h
* Author             : Lq Zhang
* Description        : This file provides all the Ublox functions.
* History:
*  01/07/2016 : Ublox V1.00
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _UBLOX_DRIVER_H
#define _UBLOX_DRIVER_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
#include "include.h"

/* Exported macro ------------------------------------------------------------*/
#define DATA_SIZE_MAX 1536
#define LENGTH_LOOP_TIME 5
//I2C Read & Write Addr
#define UBLOX_I2C_READ 0x01
#define UBLOX_I2C_WRITE 0xFE

#define UBX_SYNC_1 0xB5
#define UBX_SYNC_2 0x62

#define UBLOX_I2C_DRV_ADDRESS 0x84
#define UBLOX_LEN_START_ADDR 0xFD
#define UBLOX_OPERATE_TIMEOUT 10

#define UBLOX_SET_POWER_OUT()             \
    UBLOXGPIO.Pin = UBLOX_POWER_PIN;      \
    UBLOXGPIO.Mode = GPIO_MODE_OUTPUT_PP; \
    UBLOXGPIO.Pull = GPIO_NOPULL;         \
    UBLOXGPIO.Speed = GPIO_SPEED_FREQ_HIGH;    \
    HAL_GPIO_Init(UBLOX_POWER_PORT, &UBLOXGPIO)

#define UBLOX_SET_POWER() HAL_GPIO_WritePin(UBLOX_POWER_PORT, UBLOX_POWER_PIN, GPIO_PIN_SET)
#define UBLOX_POWER_PORT PB5_GPS_PWR_EN_GPIO_Port
#define UBLOX_POWER_PIN PB5_GPS_PWR_EN_Pin

/* Exported types ------------------------------------------------------------*/
typedef struct
{
    uint16 DataLen;
    uint8 Data[DATA_SIZE_MAX];
} PswGpsNmeaStreamMsgT;

typedef struct
{
    double Longitude;
    double Latitude;
    double Heading;
    double Velocity;
    double Altitude;
    // double hdop;
    double pdop;
    uint16 SatelliteCount;
    bool bGpsLock;

    uint16 Year;
    uint16 Month;
    uint16 DayOfWeek;
    uint16 Day;
    uint16 Hour;
    uint16 Minute;
    uint16 Second;
    uint16 Milliseconds;

    /*; GPS week as the number of whole weeks since GPS time zero*/
    uint32 GPSweek;
    /*; GPS time of week in milliseconds*/
    uint32 GPSTimeOfWeek;

    uint8 validFix;
    uint8 WeakSignalvalidFix;
    uint8 Gps2Dfix_Blocked;

} ValGpsInfo;

typedef struct
{
    char *Name;
    bool (*GpsProcessFunc)(void *pdata, int *fixmode);
} GpsParseStruct;
#define GPSNMEA_CMD_NAME_LEN (5)
#define GPSNMEA_CMD_INFO_LEN (1024)
typedef struct
{
    uint16 Year;
    uint16 Month;
    uint16 DayOfWeek;
    uint16 Day;
    uint16 Hour;
    uint16 Minute;
    uint16 Second;
    uint16 Milliseconds;
} ValGpsTimeT;
typedef enum {
    GPGGA_UTC_Time = 1,
    GPGGA_Latitude,
    GPGGA_North,
    GPGGA_Longitude,
    GPGGA_East,
    GPGGA_Quality,
    GPGGA_SatelliteCount,
    GPGGA_HorizontalDilutionOfPrecision,
    GPGGA_AltitudeWRTSeaLevel,
    GPGGA_UnitsOfAntennaAltitude,
    GPGGA_AltitudeWRTEllipsoid,
    GPGGA_UnitsOfGeoidalSeparation,
    GPGGA_RTCMTime,
    GPGGA_StationID,
    GPGGA_Checksum,
} GPGGAStatusT;
typedef enum {
    GPGSA_Selection_Mode = 1,
    GPGSA_Mode,
    GPGSA_1st_Satellite,
    GPGSA_2nd_Satellite,
    GPGSA_3rd_Satellite,
    GPGSA_4th_Satellite,
    GPGSA_5th_Satellite,
    GPGSA_6th_Satellite,
    GPGSA_7th_Satellite,
    GPGSA_8th_Satellite,
    GPGSA_9th_Satellite,
    GPGSA_10th_Satellite,
    GPGSA_11th_Satellite,
    GPGSA_12th_Satellite,
    GPGSA_PDOP,
    GPGSA_HDOP,
    GPGSA_VDOP,
    GPGSA_Checksum,
} GPGSAStatusT;
typedef enum {
    GPGSV_Total_Msg_Num = 1,
    GPGSV_Msg_Num,
    GPGSV_Satellites_In_View,
    GPGSV_SatelliteInfos_PRN_1,
    GPGSV_SatelliteInfos_Elevation_1,
    GPGSV_SatelliteInfos_Azimuth_1,
    GPGSV_SatelliteInfos_SNR_1,
    GPGSV_SatelliteInfos_PRN_2,
    GPGSV_SatelliteInfos_Elevation_2,
    GPGSV_SatelliteInfos_Azimuth_2,
    GPGSV_SatelliteInfos_SNR_2,
    GPGSV_SatelliteInfos_PRN_3,
    GPGSV_SatelliteInfos_Elevation_3,
    GPGSV_SatelliteInfos_Azimuth_3,
    GPGSV_SatelliteInfos_SNR_3,
    GPGSV_SatelliteInfos_PRN_4,
    GPGSV_SatelliteInfos_Elevation_4,
    GPGSV_SatelliteInfos_Azimuth_4,
    GPGSV_SatelliteInfos_SNR_4,
} GPGSVStatusT;
typedef enum {
    GPRMC_UTC_Time = 1,
    GPRMC_Status,
    GPRMC_Latitude,
    GPRMC_North,
    GPRMC_Longitude,
    GPRMC_East,
    GPRMC_Speed,
    GPRMC_head,
    GPRMC_UTC_Date,
    GPRMC_MagneticVariation,
    GPRMC_Declination,
    //   GPRMC_Mode_Indicator,
    GPRMC_Checksum,
} GPRMCStatusT;
typedef enum {
    GPS_FIX_QUALITY_UNKNOWN, /*Fix uses information from GPS satellites only.*/
    GPS_FIX_QUALITY_GPS,     /*Fix uses information from GPS satellites and also a differential GPS (DGPS) station. */
    GPS_FIX_QUALITY_DGPS
} ValGpsFixQualityT;

typedef enum {
    VAL_GPS_FIX_UNKNOWN,
    VAL_GPS_FIX_2D,
    VAL_GPS_FIX_3D
} ValGpsFixTypeT;

typedef enum {
    GPS_FIX_SELECTION_UNKNOWN,
    GPS_FIX_SELECTION_AUTO,
    GPS_FIX_SELECTION_MANUAL
} ValGpsSelectionTypeT;

typedef enum {
    CP_VAL_GPS_FIX_MODE_UNKNOWN,
    CP_VAL_GPS_FIX_MODE_MSA,
    CP_VAL_GPS_FIX_MODE_MSB,
    CP_VAL_GPS_FIX_MODE_MSS,
    CP_VAL_GPS_FIX_MODE_AFLT,
    CP_VAL_GPS_FIX_MODE_SPEED_OPTIMAL,
    CP_VAL_GPS_FIX_MODE_ACCURACY_OPTIMAL,
    CP_VAL_GPS_FIX_MODE_DATA_OPTIMAL,
    CP_VAL_GPS_FIX_MODE_CONTROL_PLANE,
    CP_VAL_GPS_FIX_MODE_COUNT /* must be last entry*/
} ValGpsFixModeT;

typedef struct
{
    uint16 Year;
    uint16 Month;
    uint16 DayOfWeek;
    uint16 Day;
    uint16 Hour;
    uint16 Minute;
    uint16 Second;
    uint16 Milliseconds;

    /*; GPS week as the number of whole weeks since GPS time zero*/
    uint32 GPSweek;
    /*; GPS time of week in milliseconds*/
    uint32 GPSTimeOfWeek;
} ValGpsSystemTimeT;

typedef struct
{
    /*; Horizontal position uncertainty in meters of axis aligned with the angle 
  ; specified in dWHorizontalErrorAngle of a two-dimension horizontal error 
  ; ellipse. The value dwHorizontalConfidence gives the percentage of positions 
  ; expected to fall within this ellipse, e.g. dwHorizontalConfidence = 39 
  ; indicates a 1-sigma error ellipse is given.*/
    uint32 HorizontalErrorAlong;
    uint32 HorizontalErrorAngle;
    uint32 HorizontalErrorPerp;
    uint32 VerticalError;
    uint32 HorizontalConfidence;

    /*; Horizontal velocity uncertainty in m/s*/
    uint32 HorizontalVelocityError;
    /*; Vertical velocity uncertainty in m/s*/
    uint32 VerticalVelocityError;
    /*; Horizontal heading uncertainty in degrees*/
    uint32 HorinzontalHeadingError;
    /*; Latitude uncertainty*/
    uint32 LatitudeUncertainty;
    /*; Longitude Uncertainty*/
    uint32 LongitudeUncertainty;
} VALGpsPositionErrorT;

typedef struct
{
    uint32 ValidityMask;
    ValGpsSystemTimeT UTCTime;

    bool bOldPosDataSaved;

    int32 Latitude;  /*in degrees, positive number indicate north latitude*/
    int32 Longitude; /*in degrees, positive number indicate east longitude*/
    double Speed;    /*in knots (nautical miles)*/
    double Heading;  /*in degrees, a heading of zero is true north*/

    double MagneticVariation;
    double AltitudeWRTSeaLevel;  /*in meters, with respect to sea level*/
    double AltitudeWRTEllipsoid; /*in meters, with respect to the WGS84 ellipsoid*/

    ValGpsFixQualityT FixQuality;
    ValGpsFixTypeT FixType;
    ValGpsSelectionTypeT SelectionType;

    double PositionDilutionOfPrecision;
    double HorizontalDilutionOfPrecision;
    double VerticalDilutionOfPrecision;
    uint32 SatelliteCount; /*number of satellites used to obtain the position*/

    uint32 SatellitesUsedPRNs[12];
    uint32 SatellitesInView;
    uint8 totalGsvNum;
    uint8 seqGsvNum;
    uint32 SatellitesInViewPRNs[12];
    uint32 SatellitesInViewElevation[12];
    uint32 SatellitesInViewAzimuth[12];
    uint32 SatellitesInViewSNR[12];

    VALGpsPositionErrorT GPSPositionError;

    ValGpsFixModeT FixMode;
} ValNMEADataT;

// Variable Declared
extern u16 UbloxCheckStatTimeout;
extern PswGpsNmeaStreamMsgT MsgBuffer;
extern ValGpsInfo GpsInfo;
extern ValNMEADataT gLocMsg;

//Function Declare
extern void UbloxGPSStart(void);
extern void UbloxGPSStop(void);
extern void UbloxPowerEnControl(FunctionalState Status);
extern void UbloxI2cInit(void);
extern void UbloxI2cDeInit(void);
extern void ReadUbloxData(void);
extern void UbloxGPSTimerProcess(void);
extern double UbloxSpeedKM(void);
extern uint8_t UbloxFixStateGet(void);

#endif

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2017. All rights reserved
                                End Of The File
*******************************************************************************/
