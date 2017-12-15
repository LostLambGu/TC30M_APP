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
typedef struct
{
    uint8_t EQ_VEN[1];      /* OEM = 0x01 */
    uint8_t PROT[1];        /* Binary = 0x00; ASCII = 0x01 */
    uint8_t PVER[1];        /* Protocol Version */
    uint8_t PYLD[1];        /* Status = 0x00; **Command = 0x01; (reserved)
                            Command Response = 0x02
                            Diagnostic Response = 0x03
                            **Diagnostic Request = 0x04 (reserved) */
    uint8_t MCNT[1];        /* Number of times this message was sent(applicable with ACK type)
                            Count = 0x00 for no ACK required
                            Count > 0x00 for number of times message sent, ACK required */
    uint8_t IDENT[15];      /* IMEI of device, 15 bytes in ASCII format */
    uint8_t SEQ[1];         /* Sequence ID of messages */
    uint8_t EVID[2];        /* Event Identification */
    uint8_t RTC_TS[6];      /* Real Time Clock Timestamp YYMMDDHHMMSS in UTC */
    uint8_t IO_STAT[3];     /* Byte 1 : GPIO state; Byte 2 : GPIO configuration; Byte 3 : ADC (Input power) */
    uint8_t GSM_SS[1];      /* GSM Signal Strength (AT+CSQ) */
    uint8_t GSM_RS[1];      /* GSM Registration Status */
    uint8_t GPRS_RS[1];     /* GPRS Registration Status */
    uint8_t GPS_DT[3];      /* GPS Date (YYMMDD) */
    uint8_t GPS_TM[3];      /* GPS Fix Time (HHMMSS) */
    uint8_t POS_LAT[4];     /* * Latitude 4-byte floating point */
    uint8_t POS_LON[4];     /* * Longitude 4-byte floating point */
    uint8_t POS_ALT[3];     /* Altitude in meters (integer) */
    uint8_t POS_SPD[2];     /* Speed in kilometers/hour */
    uint8_t POS_HDG[2];     /* Heading in 10ths of degrees (integer) */
    uint8_t POS_VAL[1];     /* Status of GPS fix
                            0x00 = INVALID FIX
                            0x01 = VALID FIX
                            0x0A = OLD GPS FIX
                            0xFF = Malfunction (no GPS data) */
    uint8_t POS_SATS[1];    /* Number of satellites in view */
    uint8_t POS_QUAL[2];    /* Position Dilution of Precision (PDOP) (value is in 10th’s of PDOP value) */
    uint8_t V_ODOM[4];      /* Virtual Odometer in meters (integer) */
    uint8_t USRDAT[10];     /* User Defined Data */
    uint8_t FW_VER[10];     /* Firmware Version */
    uint8_t SPARE[2];       /* Spare data bytes */
} BinaryMsgFormatTypeDef;

extern BinaryMsgFormatTypeDef BinaryMsgRec;

#ifdef __cplusplus
}
#endif

#endif /* _WEDGE_DATA_DEFINE_H_ */

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2017. All rights reserved
                                End Of The File
*******************************************************************************/
