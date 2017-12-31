/*******************************************************************************
* File Name          : eventmsgque_process.c
* Author             : Yangjie Gu
* Description        : This file provides all the eventmsgque_process functions.

* History:
*  10/28/2017 : eventmsgque_process V1.00
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "eventmsgque_process.h"
#include "rtcclock.h"
#include "usrtimer.h"
#include "network.h"
#include "ublox_driver.h"
#include "flash.h"
#include "eventalertflow.h"

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
static RTCTimerListTypeDef RTCTimerList = {0};

/* Function definition -------------------------------------------------------*/

void SmsReceivedHandle(void *MsgBufferP, uint32_t size)
{
    
}

void UdpReceivedHandle(void *MsgBufferP, uint32_t size)
{
    
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











}

void WedgeCfgChgStateSet(WEDGECfgChangeTypeDef CfgChg, uint8_t State)
{
    if (CfgChg >= CFG_CHG_INVALIAD_MAX)
    {
        EVENTMSGQUE_PROCESS_LOG("WEDGE CFG CHG SET: Param err");
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

uint8_t WedgeCfgChgStateGet(void)
{










	return 0;
}

void WedgeCfgChgStateProcess(void)
{
    if (WEDGECfgState.CfgChgNum == 0)
    {
        return;
    }
    
    // case SMS_ADDR_CFG_CHG:
    //     break;
    // case SVRCFG_CFG_CHG:
    //     break;
    // case FTPCFG_CFG_CHG:
    //     break;
    // case APNCFG_CFG_CHG:
    //     break;
    // case HWRST_CFG_CHG:
    //     break;
    // case PWRMGT_CFG_CHG:
    //     break;
    // case USRDAT_CFG_CHG:
    //     break;
    // case CFGALL_CFG_CHG:
    //     break;
    // case RESET_DEFAULT_CFG_CHG:
    //     break;
    // case IGNTYP_CFG_CHG:
    //     break;
    // case RPTINTVL_CFG_CHG:
    //     break;
    // case ALARM1_CFG_CHG:
    //     break;
    // case ALARM2_CFG_CHG:
    //     break;
    // case LVA_CFG_CHG:
    //     break;
    // case IDLE_CFG_CHG:
    //     break;
    // case SODO_CFG_CHG:
    //     break;
    // case DIRCHG_CFG_CHG:
    //     break;
    // case TOW_CFG_CHG:
    //     break;
    // case STPINTVL_CFG_CHG:
    //     break;
    // case VODO_CFG_CHG:
    //     break;
    // case GEOFENCES1_CFG_CHG:
    // case GEOFENCES2_CFG_CHG:
    // case GEOFENCES3_CFG_CHG:
    // case GEOFENCES4_CFG_CHG:
    // case GEOFENCES5_CFG_CHG:
    // case GEOFENCES6_CFG_CHG:
    // case GEOFENCES7_CFG_CHG:
    // case GEOFENCES8_CFG_CHG:
    // case GEOFENCES9_CFG_CHG:
    // case GEOFENCES10_CFG_CHG:
    //     break;
    // case RELAY_CFG_CHG:
    //     break;
    // case PLSRLY_CFG_CHG:
    //     break;
    // case OPSOD_CFG_CHG:
    //     break;
    // default:
    //     EVENTMSGQUE_PROCESS_LOG("WEDGE CFG CHG SET: Param err");
    //     break;
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

static uint8_t WedgeRtcTimerInstanceInsert(RTCTimerListCellTypeDef Instance)
{
    uint8_t i = 0;

    if (RTCTimerList.instancenum >= WEDGE_RTC_TIMER_INSTANCE_MAX)
    {
        return 1;
    }

    if ((Instance.RTCTimerType >= WEDGE_RTC_TIMER_MAX) || (Instance.RTCTimerType == WEDGE_RTC_TIMER_INVALID))
    {
        return 2;
    }

    for (i = 0; i < WEDGE_RTC_TIMER_INSTANCE_MAX; i++)
    {
        if ((RTCTimerList.instancearray[i]).RTCTimerType == WEDGE_RTC_TIMER_INVALID)
        {

        }
        else if ((RTCTimerList.instancearray[i]).RTCTimerType < WEDGE_RTC_TIMER_MAX)
        {
            if ((RTCTimerList.instancearray[i]).RTCTimerInstance == Instance.RTCTimerInstance)
            {
                RTCTimerList.instancearray[i] = Instance;
                return 0;
            }
        }
        else
        {

        }
    }

    if ((RTCTimerList.currentinstance).RTCTimerInstance == Instance.RTCTimerInstance)
    {
        RTCTimerList.currentinstance = Instance;
        return 0;
    }

    for (i = 0; i < WEDGE_RTC_TIMER_INSTANCE_MAX; i++)
    {
        if ((RTCTimerList.instancearray[i]).RTCTimerType == WEDGE_RTC_TIMER_INVALID)
        {
            break;
        }
    }

    if (i >= WEDGE_RTC_TIMER_INSTANCE_MAX)
    {
        return 3;
    }

    RTCTimerList.instancearray[i] = Instance;
    RTCTimerList.instancenum++;

    return 0;
}

static uint8_t WedgeRtcTimerFetchMinInArray(RTCTimerListCellTypeDef *pInstance, uint8_t *pIndex)
{
    RTCTimerListCellTypeDef mintmpinstance;
    uint8_t i = 0, Index;

    if ((pInstance == NULL) || (pIndex == NULL))
    {
        return 1;
    }

    if (RTCTimerList.instancenum == 0)
    {
        return 2;
    }

    mintmpinstance.settime = 0xFFFFFFFF;
    Index = WEDGE_RTC_TIMER_INSTANCE_MAX;

    for (i = 0; i < WEDGE_RTC_TIMER_INSTANCE_MAX; i++)
    {
        if (((RTCTimerList.instancearray[i]).RTCTimerType != WEDGE_RTC_TIMER_INVALID)
            && ((RTCTimerList.instancearray[i]).RTCTimerType < WEDGE_RTC_TIMER_MAX))
        {
            if ((RTCTimerList.instancearray[i]).settime < mintmpinstance.settime)
            {
                mintmpinstance = RTCTimerList.instancearray[i];
                Index = i;
            }
        }
    }

    if (Index == WEDGE_RTC_TIMER_INSTANCE_MAX)
    {
        return 3;
    }

    *pInstance = mintmpinstance;
    *pIndex = Index;
    return 0;
}

uint32_t WedgeRtcCurrentSeconds(void)
{
    TimeTableT timetable = {0};
    uint32_t currenttimeinseconds = {0};

    timetable = GetRTCDatetime();
    currenttimeinseconds = TimeTableToSeconds(timetable);

    return currenttimeinseconds;
}

static uint8_t WedgeRtcTimerInstanceAlarmRefresh(void)
{
    uint32_t curseconds = WedgeRtcCurrentSeconds();
    uint32_t setseconds = 0;

    if ((RTCTimerList.currentinstance).settime <= curseconds)
    {
        return 4;
    }

    setseconds = (RTCTimerList.currentinstance).settime - curseconds;

    return SetRTCAlarmTime(setseconds, DbgCtl.WedgeEvtMsgQueInfoEn);
}

uint8_t WedgeRtcTimerInit(void)
{












	return 1;
}

uint8_t WedgeRtcHwrstPowerLostJudge(void)
{
    return 0; // HWRST
}

uint8_t WedgeRtcTimerInstanceAdd(RTCTimerListCellTypeDef Instance)
{
    uint8_t Index = 0, ret = 0;
    RTCTimerListCellTypeDef tmpinstance;

    ret = WedgeRtcTimerInstanceInsert(Instance);
    if (ret)
    {
        EVENTMSGQUE_PROCESS_PRINT(DbgCtl.WedgeEvtMsgQueInfoEn, "\r\n[%s] WEDGE RTC Timer Add Insert err: %d",
				FmtTimeShow(), ret);
        return 1;
    }

    ret = WedgeRtcTimerFetchMinInArray(&tmpinstance, &Index);
    if (ret)
    {
        EVENTMSGQUE_PROCESS_PRINT(DbgCtl.WedgeEvtMsgQueInfoEn, "\r\n[%s] WEDGE RTC Timer Add Fetch err: %d",
				FmtTimeShow(), ret);
        return 2;
    }
    
    if (tmpinstance.settime < (RTCTimerList.currentinstance).settime)
    {
        RTCTimerList.instancearray[Index] = RTCTimerList.currentinstance;
        RTCTimerList.currentinstance = tmpinstance;
    }

    ret = WedgeRtcTimerInstanceAlarmRefresh();
    if (ret)
    {
        EVENTMSGQUE_PROCESS_PRINT(DbgCtl.WedgeEvtMsgQueInfoEn, "\r\n[%s] WEDGE RTC Timer Add Refresh err: %d",
				FmtTimeShow(), ret);
        return 3;
    }

    return 0;
}

uint8_t WedgeRtcTimerInstanceDel(WEDGERTCTimerInstanceTypeDef InstanceType)
{
    uint8_t i = 0, Index = 0, ret = 0;
    RTCTimerListCellTypeDef tmpinstance;

    if (InstanceType >= WEDGE_RTC_TIMER_INSTANCE_INVALID_MAX)
    {
        return 1;
    }

    if (RTCTimerList.instancenum == 0)
    {
        return 2;
    }

    for (i = 0; i < WEDGE_RTC_TIMER_INSTANCE_MAX; i++)
    {
        if (((RTCTimerList.instancearray[i]).RTCTimerType > WEDGE_RTC_TIMER_INVALID)
        &&  ((RTCTimerList.instancearray[i]).RTCTimerType < WEDGE_RTC_TIMER_MAX))
        {
            if ((RTCTimerList.instancearray[i]).RTCTimerInstance == InstanceType)
            {
                (RTCTimerList.instancearray[i]).RTCTimerType = WEDGE_RTC_TIMER_INVALID;
                RTCTimerList.instancenum--;
            }
        }
    }

    if ((RTCTimerList.currentinstance).RTCTimerInstance == InstanceType)
    {
        ret = WedgeRtcTimerFetchMinInArray(&tmpinstance, &Index);
        if (ret)
        {
            EVENTMSGQUE_PROCESS_PRINT(DbgCtl.WedgeEvtMsgQueInfoEn, "\r\n[%s] WEDGE RTC Timer Del Fetch err: %d",
                                      FmtTimeShow(), ret);
            return 3;
        }

        RTCTimerList.currentinstance = tmpinstance;
        (RTCTimerList.instancearray[Index]).RTCTimerType = WEDGE_RTC_TIMER_INVALID;

        ret = WedgeRtcTimerInstanceAlarmRefresh();
        if (ret)
        {
            EVENTMSGQUE_PROCESS_PRINT(DbgCtl.WedgeEvtMsgQueInfoEn, "\r\n[%s] WEDGE RTC Timer Del Refresh err: %d",
                                      FmtTimeShow(), ret);
            return 4;
        }
    }

    return 0;
    
}

uint8_t WedgeRtcTimerModifySettime(uint32_t Delta, WEDGERTCTimerSettimeTypeDef ModifyType)
{
    uint8_t i = 0, ret = 0;

    if (ModifyType >= WEDGE_RTC_TIMER_MODIFY_INVALID_MAX)
    {
        return 1;
    }
    
    if (Delta == 0)
    {
        return 0;
    }

    for (i = 0; i < WEDGE_RTC_TIMER_INSTANCE_MAX; i++)
    {
        if (((RTCTimerList.instancearray[i]).RTCTimerType > WEDGE_RTC_TIMER_INVALID)
        &&  ((RTCTimerList.instancearray[i]).RTCTimerType < WEDGE_RTC_TIMER_MAX))
        {
            if (ModifyType == WEDGE_RTC_TIMER_MODIFY_INCREASE)
            {
                (RTCTimerList.instancearray[i]).settime += Delta;
            }
            else
            {
                (RTCTimerList.instancearray[i]).settime -= Delta;
            }
        }
    }

    if (ModifyType == WEDGE_RTC_TIMER_MODIFY_INCREASE)
    {
        (RTCTimerList.currentinstance).settime += Delta;
    }
    else
    {
        (RTCTimerList.currentinstance).settime -= Delta;
    }

    ret = WedgeRtcTimerInstanceAlarmRefresh();
	if (ret)
    {
        EVENTMSGQUE_PROCESS_PRINT(DbgCtl.WedgeEvtMsgQueInfoEn, "\r\n[%s] WEDGE RTC Timer Modify Refresh err: %d",
                                  FmtTimeShow(), ret);
        return 2;
    }

    return 0;
}

static uint8_t WedgeRtcAlarmHappen = 0;
void WedgeSetRTCAlarmStatus(uint8_t Status)
{
    WedgeRtcAlarmHappen = Status;
}

uint8_t WedgeGetRTCAlarmStatus(void)
{
    return WedgeRtcAlarmHappen;
}

static RTCTimerListCellTypeDef WedgeRTCGetCurrentInstance(void)
{
    return RTCTimerList.currentinstance;
}

void WedgeRTCTimerEventProcess(void)
{
    RTCTimerListCellTypeDef currentinstance;
    uint8_t ret = 0;

    if (WedgeGetRTCAlarmStatus() == FALSE)
    {
        return;
    }

    memset(&currentinstance, 0, sizeof(currentinstance));
    currentinstance = WedgeRTCGetCurrentInstance();
    WedgeSetRTCAlarmStatus(FALSE);

    ret = WedgeRtcTimerInstanceAlarmRefresh();
    if (ret == 0)
    {
        return;
    }

    switch (currentinstance.RTCTimerInstance)
    {
    case Periodic_Moving_Event:
    {
        EVENTMSGQUE_PROCESS_PRINT(DbgCtl.WedgeEvtMsgQueInfoEn, "\r\n[%s] WEDGE RTC Timer Process: %d",
                                  FmtTimeShow(), Periodic_Moving_Event);
    }
        break;

    case Periodic_OFF_Event:
        break;

    case Periodic_Health_Event:
        break;

    case Stop_Report_Onetime_Event:
        break;

    case Periodic_Hardware_Reset_Onetime:
        break;

    default:
        break;
    }
}

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2017. All rights reserved
                                End Of The File
*******************************************************************************/
