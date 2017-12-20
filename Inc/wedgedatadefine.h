/*******************************************************************************
* File Name          : wedgedatadefine.h
* Author             : Yangjie Gu
* Description        : This file provides all the wedgedatadefine functions.

* History:
*  12/14/2017 : wedgedatadefine V1.00
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _WEDGE_DATA_DEFINE_H_
#define _WEDGE_DATA_DEFINE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

/* Defines -------------------------------------------------------------------*/
typedef enum
{
    WEDGEPYLD_STATUS = 0,
    WEDGEPYLD_COMMAND,
    WEDGEPYLD_COMMAND_RESPONSE,
    WEDGEPYLD_DIAGNOSTIC_RESPONSE,
    WEDGEPYLD_DIAGNOSTIC_REQUEST,
    WEDGEPYLD_INVALID_MAX
} WEDGEPYLDTypeDef;

typedef enum
{
    /* Events and Alarms table */
    Location_request = 500,
    Drive_Report_Location,
    IDLE_Detect,
    Service_Alert,
    Starter_Disabled = 505,
    Starter_Enabled = 506,
    Door_unlock = 511,
    Location_of_Disabled_Vehicle = 515,
    Periodic_Report_with_Ignition_OFF = 521,
    Alarm1_input_INACTIVE = 524,
    Alarm1_input_ACTIVE,
    Alarm2_input_INACTIVE,
    Alarm2_input_ACTIVE,
    Modem_powered_up = 530,
    Ignition_OFF,
    Ignition_ON,
    Low_Battery_Alert = 536,
    Heading_Change_Detect = 559,
    Over_Speed_Threshold_Detect,
    Health_Message = 580,
    Stop_Report = 583,

    /* Configuration Acknowledgement */
    Configure_Ignition_Type_Acknowledge = 600,
    Configure_Reporting_Intervals_Acknowledge,
    Configure_Low_Battery_Voltage_Level_Acknowledge = 603,
    Configure_IDLE_Detect_Acknowledge,
    Configure_Service_Odometer_Acknowledge,
    Configure_Heading_Change_Acknowledge,
    Configure_Tow_Alert_Acknowledge,
    Configure_Stop_Report_Acknowledge,
    Configure_Virtual_Odometer_Acknowledge,
    Configure_Geofence_Acknowledge,
    Configure_Setting_Cur_Loc_as_Geofence_SUCCEED_Acknowledge,
    Configure_Setting_Cur_Loc_as_Geofence_FAIL_Acknowledge,
    Configure_Over_Speed_Threshold_Acknowledge = 613,
    Configure_Alarm1_Input_Acknowledge = 615,
    Configure_Alarm2_Input_Acknowledge,
    Unsupported_Unknown_command = 699,

    /* Geofence Alerts */
    Geofence1_entered = 700,
    Geofence2_entered,
    Geofence3_entered,
    Geofence4_entered,
    Geofence5_entered,
    Geofence6_entered,
    Custom_Geofence7_entered,
    Custom_Geofence8_entered,
    Custom_Geofence9_entered,
    Custom_Geofence10_entered,
    Geofence1_exited,
    Geofence2_exited,
    Geofence3_exited,
    Geofence4_exited,
    Geofence5_exited,
    Geofence6_exited,
    Custom_Geofence7_exited,
    Custom_Geofence8_exited,
    Custom_Geofence9_exited,
    Custom_Geofence10_exited,
    Tow_Alert_Exited = 720,

    WEDGEEVID_ID_INVALID = 0xFFFF
} WEDGEEVIDTypeDef;

#define MSG_FMT_DEFAULT_1_BYTES (1)
#define MSG_FMT_IDENT_15_BYTES (15)
#define MSG_FMT_EVID_2_BYTES (2)
#define MSG_FMT_RTC_TS_6_BYTES (6)
#define MSG_FMT_IO_STAT_3_BYTES (3)
#define MSG_FMT_GPS_DT_3_BYTES (3)
#define MSG_FMT_GPS_TM_3_BYTES (3)
#define MSG_FMT_POS_LAT_4_BYTES (4)
#define MSG_FMT_POS_LON_4_BYTES (4)
#define MSG_FMT_POS_ALT_3_BYTES (3)
#define MSG_FMT_POS_SPD_2_BYTES (2)
#define MSG_FMT_POS_HDG_2_BYTES (2)
#define MSG_FMT_POS_QUAL_2_BYTES (2)
#define MSG_FMT_V_ODOM_4_BYTES (4)
#define MSG_FMT_USRDAT_10_BYTES (10)
#define MSG_FMT_FW_VER_10_BYTES (10)
#define MSG_FMT_SPARE_2_BYTES (2)

typedef struct
{
    uint8_t EQ_VEN[MSG_FMT_DEFAULT_1_BYTES];      /* OEM = 0x01 */
    uint8_t PROT[MSG_FMT_DEFAULT_1_BYTES];        /* Binary = 0x00; ASCII = 0x01 */
    uint8_t PVER[MSG_FMT_DEFAULT_1_BYTES];        /* Protocol Version */
    uint8_t PYLD[MSG_FMT_DEFAULT_1_BYTES];        /* Status = 0x00; **Command = 0x01; (reserved)
                                                    Command Response = 0x02
                                                    Diagnostic Response = 0x03
                                                    **Diagnostic Request = 0x04 (reserved) */
    uint8_t MCNT[MSG_FMT_DEFAULT_1_BYTES];        /* Number of times this message was sent(applicable with ACK type)
                                                    Count = 0x00 for no ACK required
                                                    Count > 0x00 for number of times message sent, ACK required */
    uint8_t IDENT[MSG_FMT_IDENT_15_BYTES];      /* IMEI of device, 15 bytes in ASCII format */
    uint8_t SEQ[MSG_FMT_DEFAULT_1_BYTES];         /* Sequence ID of messages */
    uint8_t EVID[MSG_FMT_EVID_2_BYTES];        /* Event Identification */
    uint8_t RTC_TS[MSG_FMT_RTC_TS_6_BYTES];      /* Real Time Clock Timestamp YYMMDDHHMMSS in UTC */
    uint8_t IO_STAT[MSG_FMT_IO_STAT_3_BYTES];     /* Byte 1 : GPIO state; Byte 2 : GPIO configuration; Byte 3 : ADC (Input power) */
    uint8_t GSM_SS[MSG_FMT_DEFAULT_1_BYTES];      /* GSM Signal Strength (AT+CSQ) */
    uint8_t GSM_RS[MSG_FMT_DEFAULT_1_BYTES];      /* GSM Registration Status */
    uint8_t GPRS_RS[MSG_FMT_DEFAULT_1_BYTES];     /* GPRS Registration Status */
    uint8_t GPS_DT[MSG_FMT_GPS_DT_3_BYTES];      /* GPS Date (YYMMDD) */
    uint8_t GPS_TM[MSG_FMT_GPS_TM_3_BYTES];      /* GPS Fix Time (HHMMSS) */
    uint8_t POS_LAT[MSG_FMT_POS_LAT_4_BYTES];     /* * Latitude 4-byte floating point */
    uint8_t POS_LON[MSG_FMT_POS_LON_4_BYTES];     /* * Longitude 4-byte floating point */
    uint8_t POS_ALT[MSG_FMT_POS_ALT_3_BYTES];     /* Altitude in meters (integer) */
    uint8_t POS_SPD[MSG_FMT_POS_SPD_2_BYTES];     /* Speed in kilometers/hour */
    uint8_t POS_HDG[MSG_FMT_POS_HDG_2_BYTES];     /* Heading in 10ths of degrees (integer) */
    uint8_t POS_VAL[MSG_FMT_DEFAULT_1_BYTES];     /* Status of GPS fix
                                                    0x00 = INVALID FIX
                                                    0x01 = VALID FIX
                                                    0x0A = OLD GPS FIX
                                                    0xFF = Malfunction (no GPS data) */
    uint8_t POS_SATS[MSG_FMT_DEFAULT_1_BYTES];    /* Number of satellites in view */
    uint8_t POS_QUAL[MSG_FMT_POS_QUAL_2_BYTES];    /* Position Dilution of Precision (PDOP) (value is in 10th’s of PDOP value) */
    uint8_t V_ODOM[MSG_FMT_V_ODOM_4_BYTES];      /* Virtual Odometer in meters (integer) */
    uint8_t USRDAT[MSG_FMT_USRDAT_10_BYTES];     /* User Defined Data */
    uint8_t FW_VER[MSG_FMT_FW_VER_10_BYTES];     /* Firmware Version */
    uint8_t SPARE[MSG_FMT_SPARE_2_BYTES];       /* Spare data bytes */
} BinaryMsgFormatTypeDef;

#define MSG_FMT_ASCII_DATA_250_BYTES (250)
typedef struct
{
    uint8_t EQ_VEN[MSG_FMT_DEFAULT_1_BYTES];      /* OEM = 0x01 */
    uint8_t PROT[MSG_FMT_DEFAULT_1_BYTES];        /* Binary = 0x00; ASCII = 0x01 */
    uint8_t PVER[MSG_FMT_DEFAULT_1_BYTES];        /* Protocol Version */
    uint8_t PYLD[MSG_FMT_DEFAULT_1_BYTES];        /* Status = 0x00; **Command = 0x01; (reserved)
                                                    Command Response = 0x02
                                                    Diagnostic Response = 0x03
                                                    **Diagnostic Request = 0x04 (reserved) */
    uint8_t ASCII_DATA[MSG_FMT_ASCII_DATA_250_BYTES];
} AsciiMsgFormatTypedDef;

enum
{
    No_Ignition_detect = 0,
    Virtual_Ignition_Battery_Voltage,
    Virtual_Ignition_GPS_velocity,
    Wired_Ignition
};

typedef struct
{
    uint8_t itype;  /* <itype> is: 0 – No Ignition detect
                    1 – Virtual Ignition – Battery Voltage
                    2 – Virtual Ignition – GPS velocity
                    3 – Wired Ignition */
    uint16_t dbnc;  /* <dbnc>:0 to 1024 seconds */
    union
    {
        float voff;     /* <voff>,<von> is: Voltage level for detecting Ignition OFF and ON */
        int32_t spdoff; /* <spdoff>,<spdon>: The speed in meters per second for Ignition OFF and ON */
    } offthreshold;
    union
    {
        float von;
        int32_t spdon;
    } onthreshold;
} IGNTYPETypeDef;

typedef struct
{
    uint16_t ionint;    /* <ionint> - is the ignition ON interval in minutes (default: 0 or disabled)
                        (valid range: 0 – 1024) */
    uint16_t ioffint;   /* <ioffint> - is the ignition OFF interval in minutes (default: 0 or disabled)
                        (valid range: 0 – 1024) */
    uint16_t perint;    /* <perint> - is the periodic interval regardless of ignition state, in minutes
                        (default: 1500 minutes)
                        (valid range: 0 – 65535 minutes; 0 is disabled) */
} RPTINTVLTypeDef;

typedef struct
{
    uint8_t ioen;       /* <ioen> defines whether ALARM1 signals are processed
                        (Default: 0 – not enabled)
                        (range: 0 – 1; 1 is enabled) */
    uint8_t iodbnc;     /* <iodbnc> defines the debounce value in seconds
                        (Default: 3 seconds)
                        (Range: 0 – 255) */
} IOALRMTypeDef;

typedef struct
{
    float battlvl;      /* <battlvl> is the battery voltage threshold in deci-volts
                        (default: 11.5 )
                        (range: 5.0 – 20.0 : in 0.1 increments) */
} LVATypeDef;

typedef struct
{
    uint16_t duration;  /* <duration> is the threshold in seconds when the device is not moving where an “idle”
                        message is sent to the server.
                        (default: 0 ; range: 10 – 65535 seconds) */
} IDLETypeDef;

#define SODO_METERS_RANGE_MIN (10000)
#define SODO_METERS_RANGE_MAX (20000000)
typedef struct
{
    uint32_t meters;    /* <meters> is the service odometer setting in meters
                        (default: 0 - disabled)
                        (range: 10,000 – 20,000,000 meters) */
} SODOTypeDef;

#define VODO_METERS_RANGE_MAX (500000000)
typedef struct
{
    uint32_t meters;    /* <meters> is the virtual odometer setting in meters
                        (default: 0)
                        (range: 0 – 500,000,000 meters) */
} VODOTypeDef;

typedef struct
{
    uint8_t deg;        /* <deg> is the degree change threshold
                        (default: 0 - disabled)
                        (range: 10 to 180 degrees in 5 degree increments) */
} DIRCHGTypeDef;

typedef struct
{
    uint8_t enable;     /* <enable> is 0 for disabled and 1 for enabled
                        (default: 0) */
    uint16_t radius;    /* <radius> is the radius in meters at the current point
                        (default: 1000; range: 10 – 10000) */
} TOWTypeDef;

typedef struct
{
    uint16_t interval;  /* <interval> is the duration of time in minutes after the ignition is turned off that the
                        Stop Report event is triggered.
                        (default: 0 or disabled)
                        (range: 1 – 16535) */
} STPINTVLTypeDef;

typedef struct
{
    uint8_t index;      /* <index> is the index of the geofence (1 – 10) */
    uint8_t type;       /* <type> is the type of geofence: 0 – rectangle; 1 – circular; 2 – polygonal */
    uint8_t dir;        /* <dir> is whether an alert will be generated on:
                        0 – enter or exit
                        1 – exit
                        2 - enter */
    float lat_ctr;      /* <lat_xxx> is the latitude in degrees (+/- 90.00000) */
    float lon_ctr;      /* <lon_xxx> is the longitude in degrees (+/- 180.00000) */
    uint32_t radius;    /* <radius> is the radius of a circular geofence in meters: 0 – 999999) */
} GFNCTypeDef;

typedef struct
{
    uint8_t state;      /* Set/Reset RELAY output */
} RELAYTypeDef;

typedef struct
{
    uint8_t units;      /* <units> is the speed measurement units
                        0 = kilometers/hour (future)
                        1 = miles / hour
                        2 = knots (future)
                        3 = meters / second (future) */
    uint16_t speed;     /* <speed> is the threshold to detect */
    uint8_t debounce;   /* <debounce> is the consecutive seconds before event is raised
                        (range: 0 – 255) */
} OSPDTypeDef;

typedef struct
{
    uint16_t pw;        /* <pw> is the pulse width in 100’s of milliseconds
                        (range: 5 – 65535) */
    uint16_t ps;        /* <ps> is the pulse spacing in 100’s of milliseconds between pulses
                        (range: 5 – 65535) */
    uint16_t count;     /* <count> is the number of pulses */
    uint16_t evid;      /* <evid> is the event ID (EVID) for the message response
                        (range: 0 – 9999) */
} PLSRLYTypeDef;

typedef struct
{
    /* No Power Mgt: AT*PWRMGT=<mode=0>
    Timer method: AT*PWRMGT=<mode=1>,<ontime>,<sleeptime>
    Vib detect method: AT*PWRMGT=<mode=2,3, or 4>,<debounce>,<ontime>
    Mode 0 : Always in “RUN” mode (default state)
    Mode 1 : Timer Method
    Mode 2 : Vibration detect, start of vibration (after debounce) for <ontime>
    Mode 3 : Vibration detect, stop vibration (after debounce) for <ontime>
    Mode 4 : Vibration detect, start vibration / stop vibration (after debounce) for <ontime> for
    each transition detected.
    Where: <ontime> is the number of seconds the device is in Run mode
    <sleeptime> is the number of seconds the device is “sleeping”
    <debounce> is the number of seconds of bouncing (or no bouncing) of the
    vibration sensor.
    NOTE: “Sleeping” is defined as the GSM and GPS are turned off however the internal
    microcontroller. */
    uint8_t mode;
    uint32_t ontime;
    union
    {
        uint32_t sleeptime;
        uint32_t debounce;
    } dbcslp;
} PWRMGTTypeDef;

typedef struct
{
    uint32_t interval;  /* <interval> is the periodic hardware reset interval in minutes. */
} HWRSTTypeDef;

#define USRDAT_LEN_10_BYTES (10)
typedef struct
{
    uint8_t data[USRDAT_LEN_10_BYTES];  /* <data> is zero (0) to ten (10) ASCII alphanumeric character field */
} USRDATTypeDef;

#define SMS_ADDRESS_LEN_MAX_24_BYTES (24)
typedef struct
{
    uint8_t sms_1[SMS_ADDRESS_LEN_MAX_24_BYTES];    /* <sms_x> is a valid phone number or short code */
    uint8_t sms_2[SMS_ADDRESS_LEN_MAX_24_BYTES];
    uint8_t sms_3[SMS_ADDRESS_LEN_MAX_24_BYTES];
} SMSTypeDef;

#define SRVR_ADDRESS_LEN_MAX_24_BYTES (24)
typedef struct
{
    uint16_t port;      /* <port> is a valid port: 0 – 65535 */
    uint8_t prot;       /* <prot> is the protocol: 0 = SMS (Future)
                        1 = UDP
                        2 = UDP w/ACK (Future)
                        3 = TCP (Future) */
    uint8_t reg;        /* <reg> is registration: 0 = reserved
                        1 = No PDP context unless data to send
                        2 = Persistent PDP context */
    uint8_t srvr1[SRVR_ADDRESS_LEN_MAX_24_BYTES];   /* <srvr_x> is a valid IP address: “0.0.0.0” to “255.255.255.255” */
    uint8_t srvr2[SRVR_ADDRESS_LEN_MAX_24_BYTES];
    uint8_t srvr3[SRVR_ADDRESS_LEN_MAX_24_BYTES];
    uint8_t srvr4[SRVR_ADDRESS_LEN_MAX_24_BYTES];
    uint8_t srvr5[SRVR_ADDRESS_LEN_MAX_24_BYTES];
} SVRCFGTypeDef;

typedef struct
{
    uint8_t reserved;
} FTPCFGTypeDef;

#define APNCFG_APN_LEN_MAX_64_BYTES (64)
#define APNCFG_USR_LEN_MAX_32_BYTES (32)
#define APNCFG_PWD_LEN_MAX_32_BYTES (32)
typedef struct
{
    uint8_t apn[APNCFG_APN_LEN_MAX_64_BYTES];
    uint8_t usr[APNCFG_USR_LEN_MAX_32_BYTES];
    uint8_t pwd[APNCFG_PWD_LEN_MAX_32_BYTES];
} APNCFGTypeDef;

#define WEDGE_GEOFENCES_NUM_MAX (10)
typedef struct
{
    IGNTYPETypeDef IGNTYPE;
    RPTINTVLTypeDef RPTINTVL;
    IOALRMTypeDef IOALRM1;
    IOALRMTypeDef IOALRM2;
    LVATypeDef LVA;
    IDLETypeDef IDLE;
    SODOTypeDef SODO;
    VODOTypeDef VODO;
    DIRCHGTypeDef DIRCHG;
    TOWTypeDef TOW;
    STPINTVLTypeDef STPINTVL;
    GFNCTypeDef GFNC[WEDGE_GEOFENCES_NUM_MAX];
    RELAYTypeDef RELAY;
    OSPDTypeDef OSPD;
    PLSRLYTypeDef PLSRLY;
    PWRMGTTypeDef PWRMGT;
    HWRSTTypeDef HWRST;
    USRDATTypeDef USRDAT;
    SMSTypeDef SMS;
    SVRCFGTypeDef SVRCFG;
    FTPCFGTypeDef FTPCFG;
    APNCFGTypeDef APNCFG;
} WEDGECfgTypeDef;

typedef struct
{
    uint16_t unsent;
    uint16_t sent;
    uint16_t queinindex;
    uint16_t queoutindex;
} MQSTATTypeDef;

#ifdef __cplusplus
}
#endif

#endif /* _WEDGE_DATA_DEFINE_H_ */

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2017. All rights reserved
                                End Of The File
*******************************************************************************/
