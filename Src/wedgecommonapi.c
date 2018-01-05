/*******************************************************************************
* File Name          : wedgecommonapi.c
* Author             : Yangjie Gu
* Description        : This file provides all the wedgecommonapi functions.

* History:
*  10/28/2017 : wedgecommonapi V1.00
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "wedgecommonapi.h"
#include "rtcclock.h"
#include "usrtimer.h"
#include "network.h"
#include "ublox_driver.h"
#include "flash.h"
#include "wedgeeventalertflow.h"

/* Private define ------------------------------------------------------------*/
#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* Variables -----------------------------------------------------------------*/
static BinaryMsgFormatTypeDef BinaryMsgRecord = {0};
static AsciiMsgFormatTypedDef AsciiMsgRecord = {0};
static WEDGECfgTypeDef WEDGECfgRecord = {0};
static WEDGECfgChgStateTypedef WEDGECfgState = {0};

const static WEDGECfgTypeDef WEDGECfgFactoryDefaultOnChip =
{
    .IGNTYPE = {.itype = 0,},
    .RPTINTVL = {.ionint = 0, .ioffint = 0, .perint = 10080},
    .IOALRM1 = {.ioen = 0, .iodbnc = 5},
    .IOALRM2 = {.ioen = 0, .iodbnc = 5},
    .LVA = {.battlvl = 11.5},
    .IDLE = {.duration = 0},
    .SODO = {.meters = 0},
    .VODO = {.meters = 0},
    .DIRCHG = {.deg = 0},
    .TOW = {.enable = 0, .radius = 0},
    .STPINTVL = {.interval = 0},
    .GFNC = {0},
    .RELAY = {.state = 0},
    .OSPD = {.units = 1, .speed = 0, .debounce = 0},
    .PLSRLY = {.pw = 0, .ps = 0, .count = 0, .evid = 0},
    .PWRMGT = {.mode = 0},
    .HWRST = {.interval = 480},
    .USRDAT = {.data = ""},
    .SMS = {.sms_1 = "42818", .sms_2 = "", .sms_3 = ""},
    .SVRCFG = {.port = 14180, .prot = 1, .reg = 1, .srvr1 = "192.168.10.67", .srvr2 = "", .srvr3 = "", .srvr4 = "", .srvr5 = ""},
    .FTPCFG = {.reserved = 0},
    .APNCFG = {.apn = "mobiledata.t-mobile.com", .usr = "", .pwd = ""}
};
/* Function definition -------------------------------------------------------*/

void SmsReceivedHandle(void *MsgBufferP, uint32_t size)
{
    
}

void UdpReceivedHandle(void *MsgBufferP, uint32_t size)
{
    
}

void WedgeCfgInit(WEDGECfgTypeDef *pWEDGECfg)
{
    memset((uint8_t *)&WEDGECfgRecord, 0, sizeof(WEDGECfgTypeDef));
    if (pWEDGECfg == NULL)
    {
        EVENTMSGQUE_PROCESS_LOG("WEDGE Cfg Init Default");
        WEDGECfgRecord = WEDGECfgFactoryDefaultOnChip;
    }
    else
    {
        EVENTMSGQUE_PROCESS_LOG("WEDGE Cfg Init");
        WEDGECfgRecord = *pWEDGECfg;
    }
}

void WedgeCfgGetTotal(uint8_t *pBuf, uint32_t *pSize)
{
    if ((pBuf == NULL) || (pSize == NULL))
    {
        return;
    }

    memcpy(pBuf, &WEDGECfgRecord, sizeof(WEDGECfgRecord));
    *pSize = sizeof(WEDGECfgRecord);
}

void WedgeCfGetDefaultOnChip(uint8_t *pBuf, uint32_t *pSize)
{
    if ((pBuf == NULL) || (pSize == NULL))
    {
        return;
    }

    memcpy(pBuf, &WEDGECfgFactoryDefaultOnChip, sizeof(WEDGECfgFactoryDefaultOnChip));
    *pSize = sizeof(WEDGECfgFactoryDefaultOnChip);
}

uint16_t WedgeGetGeofenceBitMap(void)
{
    uint8_t i = 0;
    uint16_t bitmap = 0;

    for (i = 0; i < GFNC_CONSECUTIVE_VIOLATION_TIMES_MAX; i++)
    {
        if ((WEDGECfgRecord.GFNC[i]).index != 0 
        && (WEDGECfgRecord.GFNC[i]).index <= GFNC_CONSECUTIVE_VIOLATION_TIMES_MAX)
        {
            bitmap |= (0x01 << i);
        }
    }

    return bitmap;
}

void *WedgeCfgGet(WEDGECfgOperateTypeDef CfgGet)
{
    switch (CfgGet)
    {
    case WEDGE_CFG_IGNTYPE:
        return &(WEDGECfgRecord.IGNTYPE);

    case WEDGE_CFG_RPTINTVL:
        return &(WEDGECfgRecord.RPTINTVL);

    case WEDGE_CFG_IOALRM1:
        return &(WEDGECfgRecord.IOALRM1);

    case WEDGE_CFG_IOALRM2:
        return &(WEDGECfgRecord.IOALRM2);

    case WEDGE_CFG_LVA:
        return &(WEDGECfgRecord.LVA);

    case WEDGE_CFG_IDLE:
        return &(WEDGECfgRecord.IDLE);

    case WEDGE_CFG_SODO:
        return &(WEDGECfgRecord.SODO);

    case WEDGE_CFG_VODO:
        return &(WEDGECfgRecord.VODO);

    case WEDGE_CFG_DIRCHG:
        return &(WEDGECfgRecord.DIRCHG);

    case WEDGE_CFG_TOW:
        return &(WEDGECfgRecord.TOW);

    case WEDGE_CFG_STPINTVL:
        return &(WEDGECfgRecord.STPINTVL);

    case WEDGE_CFG_GFNC1:
    case WEDGE_CFG_GFNC2:
    case WEDGE_CFG_GFNC3:
    case WEDGE_CFG_GFNC4:
    case WEDGE_CFG_GFNC5:
    case WEDGE_CFG_GFNC6:
    case WEDGE_CFG_GFNC7:
    case WEDGE_CFG_GFNC8:
    case WEDGE_CFG_GFNC9:
    case WEDGE_CFG_GFNC10:
        return &(WEDGECfgRecord.GFNC[CfgGet - WEDGE_CFG_GFNC1]);

    case WEDGE_CFG_RELAY:
        return &(WEDGECfgRecord.RELAY);

    case WEDGE_CFG_OSPD:
        return &(WEDGECfgRecord.OSPD);

    case WEDGE_CFG_PLSRLY:
        return &(WEDGECfgRecord.PLSRLY);

    case WEDGE_CFG_PWRMGT:
        return &(WEDGECfgRecord.PWRMGT);

    case WEDGE_CFG_HWRST:
        return &(WEDGECfgRecord.HWRST);

    case WEDGE_CFG_USRDAT:
        return &(WEDGECfgRecord.USRDAT);

    case WEDGE_CFG_SMS:
        return &(WEDGECfgRecord.SMS);

    case WEDGE_CFG_SVRCFG:
        return &(WEDGECfgRecord.SVRCFG);

    case WEDGE_CFG_FTPCFG:
        return &(WEDGECfgRecord.FTPCFG);

    case WEDGE_CFG_APNCFG:
        return &(WEDGECfgRecord.APNCFG);

    default:
        EVENTMSGQUE_PROCESS_LOG("WEDGE CFG GET: Param err");
        return NULL;
    }
}

void WedgeCfgSet(WEDGECfgOperateTypeDef CfgSet, void *pvData)
{
    switch (CfgSet)
    {
    case WEDGE_CFG_IGNTYPE:
        WEDGECfgRecord.IGNTYPE = *((IGNTYPETypeDef *)pvData);
        break;

    case WEDGE_CFG_RPTINTVL:
        WEDGECfgRecord.RPTINTVL = *((RPTINTVLTypeDef *)pvData);
        break;

    case WEDGE_CFG_IOALRM1:
        WEDGECfgRecord.IOALRM1 = *((IOALRMTypeDef *)pvData);
        break;

    case WEDGE_CFG_IOALRM2:
        WEDGECfgRecord.IOALRM2 = *((IOALRMTypeDef *)pvData);
        break;

    case WEDGE_CFG_LVA:
        WEDGECfgRecord.LVA = *((LVATypeDef *)pvData);
        break;

    case WEDGE_CFG_IDLE:
        WEDGECfgRecord.IDLE = *((IDLETypeDef *)pvData);
        break;

    case WEDGE_CFG_SODO:
        WEDGECfgRecord.SODO = *((SODOTypeDef *)pvData);
        break;

    case WEDGE_CFG_VODO:
        WEDGECfgRecord.VODO = *((VODOTypeDef *)pvData);
        break;

    case WEDGE_CFG_DIRCHG:
        WEDGECfgRecord.DIRCHG = *((DIRCHGTypeDef *)pvData);
        break;

    case WEDGE_CFG_TOW:
        WEDGECfgRecord.TOW = *((TOWTypeDef *)pvData);
        break;

    case WEDGE_CFG_STPINTVL:
        WEDGECfgRecord.STPINTVL = *((STPINTVLTypeDef *)pvData);
        break;

    case WEDGE_CFG_GFNC1:
    case WEDGE_CFG_GFNC2:
    case WEDGE_CFG_GFNC3:
    case WEDGE_CFG_GFNC4:
    case WEDGE_CFG_GFNC5:
    case WEDGE_CFG_GFNC6:
    case WEDGE_CFG_GFNC7:
    case WEDGE_CFG_GFNC8:
    case WEDGE_CFG_GFNC9:
    case WEDGE_CFG_GFNC10:
        WEDGECfgRecord.GFNC[CfgSet - WEDGE_CFG_GFNC1] = *((GFNCTypeDef *)pvData);
        break;

    case WEDGE_CFG_RELAY:
        WEDGECfgRecord.RELAY = *((RELAYTypeDef *)pvData);
        break;

    case WEDGE_CFG_OSPD:
        WEDGECfgRecord.OSPD = *((OSPDTypeDef *)pvData);
        break;

    case WEDGE_CFG_PLSRLY:
        WEDGECfgRecord.PLSRLY = *((PLSRLYTypeDef *)pvData);
        break;

    case WEDGE_CFG_PWRMGT:
        WEDGECfgRecord.PWRMGT = *((PWRMGTTypeDef *)pvData);
        break;

    case WEDGE_CFG_HWRST:
        WEDGECfgRecord.HWRST = *((HWRSTTypeDef *)pvData);
        break;

    case WEDGE_CFG_USRDAT:
        WEDGECfgRecord.USRDAT = *((USRDATTypeDef *)pvData);
        break;

    case WEDGE_CFG_SMS:
        WEDGECfgRecord.SMS = *((SMSTypeDef *)pvData);
        break;

    case WEDGE_CFG_SVRCFG:
        WEDGECfgRecord.SVRCFG = *((SVRCFGTypeDef *)pvData);
        break;

    case WEDGE_CFG_FTPCFG:
        WEDGECfgRecord.FTPCFG = *((FTPCFGTypeDef *)pvData);
        break;

    case WEDGE_CFG_APNCFG:
        WEDGECfgRecord.APNCFG = *((APNCFGTypeDef *)pvData);
        break;

    default:
        EVENTMSGQUE_PROCESS_LOG("WEDGE CFG SET: Param err");
        break;
    }

    return;
}

void WedgeCfgChgStateInit(void)
{
    memset(&WEDGECfgState, 0, sizeof(WEDGECfgState));
}

void WedgeCfgChgStateSet(WEDGECfgChangeTypeDef CfgChg, uint8_t State)
{
    if (CfgChg >= CFG_CHG_INVALIAD_MAX)
    {
        EVENTMSGQUE_PROCESS_LOG("WEDGE Cfg Chg Set Param Err");
    }

    if (State != FALSE)
    {
        WEDGECfgState.CfgChgNum++;
    }
    else
    {
        WEDGECfgState.CfgChgNum--;
    }

    if (State == FALSE)
    {
        WEDGECfgState.CfgChgState[CfgChg] = FALSE;
    }
    else
    {
        WEDGECfgState.CfgChgState[CfgChg] = TRUE;
    }
}

uint8_t WedgeCfgChgStateIsChanged(void)
{
    return WEDGECfgState.CfgChgNum;
}

uint8_t WedgeCfgChgStateGet(WEDGECfgChangeTypeDef CfgChg)
{
    if (CfgChg >= CFG_CHG_INVALIAD_MAX)
    {
        EVENTMSGQUE_PROCESS_LOG("WEDGE Cfg Chg Get Param Err");
    }

	return WEDGECfgState.CfgChgState[CfgChg];
}

static void BytesOrderSwap(uint8_t *pBuf, uint16_t num)
{
    uint16_t i = 0, count = num / 2;
    uint8_t tmp = 0;

    for (i = 0; i < count; i++)
    {
        tmp = pBuf[i];
        pBuf[i] = pBuf[num - i - 1];
        pBuf[num - i - 1] = tmp;
    }
}

#define WEDGE_GPS_HEADING_DEG_FATCTOR (10)
#define WEDGE_GPS_PDOP_FACTOR (10)

void WedgeUpdateBinaryMsgGpsRecord(void)
{
    float *tmpf = NULL;
    uint8_t Buf[4] = {0};
    int32_t *tmpi = NULL;
    uint32_t *tmpu = NULL;
    double speedkm = 0.0;
    double headingdeg = 0.0;
    double pdop = 0.0;

    BinaryMsgRecord.GPS_DT[0] = (uint8_t)GpsInfo.Year;
    BinaryMsgRecord.GPS_DT[1] = (uint8_t)GpsInfo.Month;
    BinaryMsgRecord.GPS_DT[2] = (uint8_t)GpsInfo.Day;
    BinaryMsgRecord.GPS_TM[0] = (uint8_t)GpsInfo.Hour;
    BinaryMsgRecord.GPS_TM[1] = (uint8_t)GpsInfo.Minute;
    BinaryMsgRecord.GPS_TM[2] = (uint8_t)GpsInfo.Second;

    tmpf = (float *)(&(BinaryMsgRecord.POS_LAT[0]));
    *tmpf = (float)GpsInfo.Latitude;

    tmpf = (float *)(&(BinaryMsgRecord.POS_LON[0]));
    *tmpf = (float)GpsInfo.Longitude;

    tmpi = (int32_t *)Buf;
    *tmpi = (int32_t)(GpsInfo.Altitude);
    BytesOrderSwap(Buf, 3);
    BinaryMsgRecord.POS_ALT[0] = Buf[0];
    BinaryMsgRecord.POS_ALT[1] = Buf[1];
    BinaryMsgRecord.POS_ALT[2] = Buf[2];

    speedkm = UbloxSpeedKM();
    tmpu = (uint32_t *)Buf;
    *tmpu = (uint32_t)(speedkm);
    BytesOrderSwap(Buf, 2);
    BinaryMsgRecord.POS_SPD[0] = Buf[0];
    BinaryMsgRecord.POS_SPD[1] = Buf[1];

    headingdeg = GpsInfo.Heading * WEDGE_GPS_HEADING_DEG_FATCTOR;
    tmpu = (uint32_t *)Buf;
    *tmpu = (uint32_t)(headingdeg);
    BytesOrderSwap(Buf, 2);
    BinaryMsgRecord.POS_HDG[0] = Buf[0];
    BinaryMsgRecord.POS_HDG[1] = Buf[1];

    BinaryMsgRecord.POS_SATS[0] = (uint8_t)GpsInfo.SatelliteCount;

    pdop = GpsInfo.pdop * WEDGE_GPS_PDOP_FACTOR;
    tmpu = (uint32_t *)Buf;
    *tmpu = (uint32_t)(pdop);
    BytesOrderSwap(Buf, 2);
    BinaryMsgRecord.POS_QUAL[0] = Buf[0];
    BinaryMsgRecord.POS_QUAL[1] = Buf[1];
}

void WedgeResponseUdpBinary(WEDGEPYLDTypeDef PYLDType, WEDGEEVIDTypeDef EvID)
{

}

void WedgeResponseUdpAscii(WEDGEPYLDTypeDef PYLDType, void *MsgBufferP, uint32_t size)
{

}

void WedgeResponseSms(WEDGEPYLDTypeDef PYLDType, void *MsgBufferP, uint32_t size)
{

}

uint8_t WedgeFlashChipErase(void)
{
    FlashStatusT ret;

    ret = SerialFlashErase(FLASH_ERASE_ALL, 0);
    if (ret != FLASH_STAT_OK)
    {
        EVENTMSGQUE_PROCESS_PRINT(DbgCtl.WedgeEvtMsgQueInfoEn, "\r\n[%s] WEDGE Flash Chip Erase err: %d",
				FmtTimeShow(), ret);
    }

    return ret;
}

uint8_t WedgeFlashEraseSector(uint32_t address)
{
    FlashStatusT ret;

    ret = SerialFlashErase(FLASH_ERASE_04KB, address / WEDGE_STORAGE_SECTOR_SIZE);
    
    if (ret != FLASH_STAT_OK)
    {
        EVENTMSGQUE_PROCESS_PRINT(DbgCtl.WedgeEvtMsgQueInfoEn, "\r\n[%s] WEDGE Flash Sector Erase err: %d",
				FmtTimeShow(), ret);
    }

    return ret;
}

uint8_t WedgeFlashReadData(uint32_t address, uint8_t *pDataBuf, uint32_t datalen)
{
    FlashStatusT ret;

    ret = SerialFlashRead(pDataBuf, address, datalen);

    if (ret != FLASH_STAT_OK)
    {
        EVENTMSGQUE_PROCESS_PRINT(DbgCtl.WedgeEvtMsgQueInfoEn, "\r\n[%s] WEDGE Flash Read err: %d",
				FmtTimeShow(), ret);
    }

    return ret;
}

uint8_t WedgeFlashWriteData(uint32_t address, uint8_t *pDataBuf, uint32_t datalen)
{
    FlashStatusT ret;

    ret = SerialFlashWrite(pDataBuf, address, datalen);

    if (ret != FLASH_STAT_OK)
    {
        EVENTMSGQUE_PROCESS_PRINT(DbgCtl.WedgeEvtMsgQueInfoEn, "\r\n[%s] WEDGE Flash Write err: %d",
				FmtTimeShow(), ret);
    }

    return ret;
}

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2018. All rights reserved
                                End Of The File
*******************************************************************************/
