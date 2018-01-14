/*******************************************************************************
* File Name          : wedgecommonapi.c
* Author             : Yangjie Gu
* Description        : This file provides all the wedgecommonapi functions.

* History:
*  10/28/2017 : wedgecommonapi V1.00
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "wedgecommonapi.h"
#include "iocontrol.h"
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
static uint8_t WedgeIsPowerLost = FALSE;
static WEDGECfgTypeDef WEDGECfgRecord = {0};
static WEDGECfgChgStateTypedef WEDGECfgState = {0};
static BinaryMsgFormatTypeDef BinaryMsgRecord = {0};
static AsciiMsgFormatTypedDef AsciiMsgRecord = {0};

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
void WedgeIsPowerLostSet(uint8_t Status)
{
    WedgeIsPowerLost = Status;
}

uint8_t WedgeIsPowerLostGet(void)
{
    return WedgeIsPowerLost;
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

void WedgeCfgChgStateResetAll(void)
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

    if (FALSE == UbloxFixStateGet())
    {
        return;
    }

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


void SmsReceivedHandle(void *MsgBufferP, uint32_t size)
{
    AppSmsAtProcess(MsgBufferP, size);
}

void UdpReceivedHandle(void *MsgBufferP, uint32_t size)
{
    AppUdpAtProcess(MsgBufferP, size);
}

void WedgeResponseUdpBinaryInit(void)
{
    BinaryMsgRecord.EQ_VEN = 0x01; // Vendor is OEM
    BinaryMsgRecord.PROT = 0x00; // Binary = 0x00; ASCII = 0x01
    BinaryMsgRecord.PVER = 0x00; // Initial Protocol version
    BinaryMsgRecord.MCNT = 0x00; // Message try count
    BinaryMsgRecord.SEQ = 0x00; // Sequence ID, rolling message counter(message rolls from FF to 00)

}

extern char IMEIBuf[32];
void WedgeResponseUdpBinary(WEDGEPYLDTypeDef PYLDType, WEDGEEVIDTypeDef EvID)
{
    // Did not do Param check
    uint8_t Buf[4] = {0};
    uint32_t *pTmpu = NULL;
    TimeTableT = timeTable = {0};

    BinaryMsgRecord.PYLD = PYLDType;
    memcpy(&BinaryMsgRecord.IDENT, IMEIBuf, sizeof(BinaryMsgRecord.IDENT));
    BinaryMsgRecord.SEQ--;

    pTmpu = (uint32_t *)Buf;
    *pTmpu = (uint32_t)(EvID);
    BytesOrderSwap(Buf, 2);
    BinaryMsgRecord.POS_SPD[0] = EVID[0];
    BinaryMsgRecord.POS_SPD[1] = EVID[1];

    // Real-Time Clock timestamp UTC
    timeTable = GetRTCDatetime();
    BinaryMsgRecord.RTC_TS[0] = (uint8_t)timeTable.year;
    BinaryMsgRecord.RTC_TS[1] = (uint8_t)timeTable.month;
    BinaryMsgRecord.RTC_TS[2] = (uint8_t)timeTable.day;
    BinaryMsgRecord.RTC_TS[3] = (uint8_t)timeTable.hour;
    BinaryMsgRecord.RTC_TS[4] = (uint8_t)timeTable.minute;
    BinaryMsgRecord.RTC_TS[5] = (uint8_t)timeTable.second;

    BinaryMsgRecord.IO_STAT[0] = 0;
    BinaryMsgRecord.IO_STAT[0] |= ((WedgeLteGreenLedStateGet() & 0x01) << 0);
    BinaryMsgRecord.IO_STAT[0] |= ((WedgeGpsRedLedStateGet() & 0x01) << 1);
    BinaryMsgRecord.IO_STAT[0] |= ((0 & 0x01) << 2); //ALARM1 input state Reserved
    BinaryMsgRecord.IO_STAT[0] |= ((0 & 0x01) << 3); //ALARM2 input state Reserved
    BinaryMsgRecord.IO_STAT[0] |= ((WedgeRelayStateGet() & 0x01) << 4);
    BinaryMsgRecord.IO_STAT[0] |= ((0 & 0x01) << 5); //Primary power applied state (Battery B/U devices only) Reserved
    BinaryMsgRecord.IO_STAT[0] |= ((WedgeGpsPowerStateGet() & 0x01) << 6);
    BinaryMsgRecord.IO_STAT[0] |= ((WedgeIgnitionStateGet() & 0x01) << 7);
    BinaryMsgRecord.IO_STAT[1] = 0x2C;
    Buf[0] = (((uint16_t)((ADCGetVinVoltage() + 3.9) * 10)) < 239) ? (uint8_t)((ADCGetVinVoltage() + 3.9) * 10): 239;
    BinaryMsgRecord.IO_STAT[2] = Buf[0];

    BinaryMsgRecord.GSM_SS[0] = (GetNetworkRssiValue() <= 6) ? 0 : (GetNetworkRssiValue() - 6);
    BinaryMsgRecord.GSM_RS[0] = GetNetworkRegistrationStat();



    // Reserved
    BinaryMsgRecord.GPRS_RS[0] = 0;

    WedgeUpdateBinaryMsgGpsRecord();
    ///* Status of GPS fix: 0x00 = INVALID FIX, 0x01 = VALID FIX, 0x0A = OLD GPS FIX, 0xFF = Malfunction (no GPS data) */
    if (FALSE == *((uint8_t *)WedgeSysStateGet(WEDGE_POWER_ON_OFF_STATE)))
    {
        BinaryMsgRecord.POS_VAL[0] = 0xFF;
    }
    else
    {
        if (UbloxFixStateGet() == FALSE)
        {
            if (FALSE == *((uint8_t *)WedgeSysStateGet(WEDGE_GPS_LASTFIX_STATE)))
            {
                BinaryMsgRecord.POS_VAL[0] = 0x00;
            }
            else
            {
                BinaryMsgRecord.POS_VAL[0] = 0x0A;
            }
        }
        else
        {
            BinaryMsgRecord.POS_VAL[0] = 0x01; 
        }
    }





    // Not implement yet.
    BinaryMsgRecord.V_ODOM[0] = 0;      /* Virtual Odometer in meters (integer) */
    BinaryMsgRecord.USRDAT[0] = 0;     /* User Defined Data */
    BinaryMsgRecord.FW_VER[0] = 0;     /* Firmware Version */
    BinaryMsgRecord.SPARE[0] = 0;       /* Spare data bytes */
       
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

static WedgeBufPoolTypedef WedgeBufPool = {0};
void *WedgeBufPoolCalloc(uint16_t len)
{
	uint8_t i;

	if (len >= WEDGE_BUF_CELL_LEN_MAX)
	{
		return NULL;
	}

	for (i = 0; i < WEDGE_BUF_POOL_NUM_MAX; i++)
	{
		if (WedgeBufPool.UsedFlag[i] == 0)
		{
			break;
		}
	}

	if (i < WEDGE_BUF_POOL_NUM_MAX)
	{
		uint8_t *tmp = WedgeBufPool.BufPool[i];

		memset(tmp, 0, WEDGE_BUF_CELL_LEN_MAX);
		WedgeBufPool.UsedFlag[i] = 1;

		return tmp;
	}

	return NULL;
}

void WedgeBufPoolFree(void *pBuf)
{
	uint32_t offset = 0;
	uint8_t index = 0;

	if (pBuf == NULL)
	{
		return;
	}

	offset = ((uint32_t)pBuf) - ((uint32_t)WedgeBufPool.BufPool);
	index = offset / WEDGE_BUF_CELL_LEN_MAX;

	if (index >= WEDGE_BUF_POOL_NUM_MAX)
	{
		return;
	}
	
	WedgeBufPool.UsedFlag[index] = 0;
}

static WedgeUdpSendQueueTypedef WedgeUdpSendQueue = {0};

void WedgeUdpSendUnitIn(WedgeUdpSendQueueTypedef *pWedgeUdpSendQueue, WedgeUdpSendUintTypedef *pWedgeUDPIpSendUint)
{
    if ((pWedgeUdpSendQueue == NULL) || (pWedgeUDPIpSendUint == NULL))
    {
        EVENTMSGQUE_PROCESS_PRINT(DbgCtl.WedgeEvtMsgQueInfoEn, "\r\n[%s] WedgeUdpSendUnitIn param err", );
        return;
    }

    {
        SystemDisableAllInterrupt();
        WedgeUdpSendUintTypedef *ptemp = &(pWedgeUdpSendQueue->UDPIpSendUint[pWedgeUdpSendQueue->putindex % WEDGE_UDP_SEND_QUEUE_LENGHT_MAX]);
        ptemp->datalen = pWedgeUDPIpSendUint->datalen;
        ptemp->buf = pWedgeUDPIpSendUint->buf;

        pWedgeUdpSendQueue->putindex++;
        pWedgeUdpSendQueue->numinqueue++;
        if (pWedgeUdpSendQueue->numinqueue > WEDGE_UDP_SEND_QUEUE_LENGHT_MAX)
        {
            pWedgeUdpSendQueue->numinqueue = WEDGE_UDP_SEND_QUEUE_LENGHT_MAX;
        }
        SystemEnableAllInterrupt();
    }
}

void WedgeUdpSendUintOut(WedgeUdpSendQueueTypedef *pWedgeUdpSendQueue, WedgeUdpSendUintTypedef *pWedgeUDPIpSendUint)
{
    if ((pWedgeUdpSendQueue == NULL) || (pWedgeUDPIpSendUint == NULL))
    {
        NetworkPrintf(DbgCtl.NetworkDbgInfoEn, "WedgeUdpSendUintOut param err");
        return;
    }

    if (pWedgeUdpSendQueue->numinqueue > 0)
    {
        SystemDisableAllInterrupt();
        WedgeUdpSendUintTypedef *ptemp = &(pWedgeUdpSendQueue->UDPIpSendUint[pWedgeUdpSendQueue->getindex % WEDGE_UDP_SEND_QUEUE_LENGHT_MAX]);
        pWedgeUDPIpSendUint->datalen = ptemp->datalen;
        pWedgeUDPIpSendUint->buf = ptemp->buf;

        pWedgeUdpSendQueue->getindex++;
        pWedgeUdpSendQueue->numinqueue--;
        if (pWedgeUdpSendQueue->numinqueue > WEDGE_UDP_SEND_QUEUE_LENGHT_MAX)
        {
            pWedgeUdpSendQueue->numinqueue = 0;
        }
        SystemEnableAllInterrupt();
    }

}

static void WedgeOpenUdpSocket(SVRCFGTypeDef *pSVRCFG, uint8_t socketnum)
{
    if (pSVRCFG == NULL)
    {
        EVENTMSGQUE_PROCESS_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] WEDGE Udp Open Parm Err1",
                  FmtTimeShow());
    }

    switch(socketnum)
    {
    case 1:
        if (strlen((const char *)pSVRCFG->srvr1) > strlen("\"\""))
        {
            UdpIpSocketOpen(1, 1111, (char *)pSVRCFG->srvr1, pSVRCFG->port);
        }
        break;

    case 2:
        if (strlen((const char *)pSVRCFG->srvr2) > strlen("\"\""))
        {
            UdpIpSocketOpen(2, 2222, (char *)pSVRCFG->srvr2, pSVRCFG->port);
        }
        break;

    case 3:
        if (strlen((const char *)pSVRCFG->srvr3) > strlen("\"\""))
        {
            UdpIpSocketOpen(3, 3333, (char *)pSVRCFG->srvr3, pSVRCFG->port);
        }
        break;

    case 4:
        if (strlen((const char *)pSVRCFG->srvr4) > strlen("\"\""))
        {
            UdpIpSocketOpen(4, 4444, (char *)pSVRCFG->srvr4, pSVRCFG->port);
        }
        break;

    case 5:
        if (strlen((const char *)pSVRCFG->srvr5) > strlen("\"\""))
        {
            UdpIpSocketOpen(5, 5555, (char *)pSVRCFG->srvr5, pSVRCFG->port);
        }
        break;

    default:
        EVENTMSGQUE_PROCESS_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] WEDGE Udp Open Parm Err2",
                  FmtTimeShow());
        break;
    }
}

static void WedgeUdpInit(void)
{
    #define SVRCFG_UDP (1)
    #define SVRCFG_MAX_UDP_NUM (5)
    SVRCFGTypeDef SVRCFG;
    uint8_t i = 0;

    memset(&SVRCFG, 0, sizeof(SVRCFG));

    SVRCFG = *((SVRCFGTypeDef *)WedgeCfgGet(WEDGE_CFG_SVRCFG));

    if (SVRCFG.prot != SVRCFG_UDP)
    {
        EVENTMSGQUE_PROCESS_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] WEDGE Udp Init Not Udp",
                                      FmtTimeShow());
        return;
    }

    for (i = 0; i < SVRCFG_MAX_UDP_NUM; i++)
    {
        WedgeOpenUdpSocket(&SVRCFG, i + 1);
    }
}

static WedgeUdpSocketManageTypeDef WedgeUdpSocketManage = {0};

void WedgeUdpSocketManageDataComeSet(uint8_t Status)
{
    WedgeUdpSocketManage.newdatacome = Status;
}

void WedgeUdpSocketManageStatSet(WedgeUdpSocketManageStatT Stat)
{
    WedgeUdpSocketManage.UdpSocketManageStat = Stat;
}

void WedgeUdpSocketManageProcess(void)
{
    uint8_t newdatacome = WedgeUdpSocketManage.newdatacome;
    NetworkStatT networkstat = GetNetworkMachineStatus();

    static uint32_t lastProcessTime = 0, deltaProcessTime = 0;

    if (newdatacome == FALSE)
    {
        if ((HAL_GetTick() - lastProcessTime) < deltaProcessTime)
        {
            return;
        }
    }

    lastProcessTime = HAL_GetTick();

    switch (WedgeUdpSocketManage.UdpSocketManageStat)
    {
    case WEDGE_UDP_DISCONNECTED_STAT:
    {
        if (networkstat == NET_CONNECTED_STAT)
        {
            WedgeUdpSocketManageStatSet(WEDGE_UDP_FREE_IDLE_STAT);
            deltaProcessTime = 10;
            return;
        }
        else
        {
            if (newdatacome == TRUE)
            {
                // Put in Msg Que



                deltaProcessTime = 10;
            }
            else
            {
                deltaProcessTime = 5000;
            }
        }
    }
    break;

    case WEDGE_UDP_FREE_IDLE_STAT:
    {
        if (networkstat != NET_CONNECTED_STAT)
        {
            
        }
    }
        break;

    case WEDGE_UDP_NEED_OPEN_STAT:
        break;

    case WEDGE_UDP_WAIT_OPEN_STAT:
        break;

    case WEDGE_UDP_OPENED_STAT:
        break;

    case WEDGE_UDP_CLOSE_STAT:
        break;

    case WEDGE_WAIT_UDP_DISCONNECTED_STAT:
        break;

    default:
        EVENTMSGQUE_PROCESS_PRINT(DbgCtl.WedgeMsgQueInfoEn, "\r\n[%s] WEDGE Udp Socket Process Stat Err",
                                      FmtTimeShow());
        break;
    }

    WedgeUdpSocketManage.newdatacome = FALSE;
}

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2018. All rights reserved
                                End Of The File
*******************************************************************************/