/*******************************************************************************
* File Name          : wedgedeviceinfo.c
* Author             : Yangjie Gu
* Description        : This file provides all the wedgedeviceinfo functions.

* History:
*  01/02/2018 : wedgedeviceinfo V1.00
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "wedgedeviceinfo.h"
#include "eventmsgque_process.h"
#include "wedgertctimer.h"

/* Defines ------------------------------------------------------------------*/
#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define WEDGE_DEV_INFO_PRINT DebugPrintf

static uint32_t WedgePowerLostAddr = 0;
static void WedgePowerLostAddrSet(uint32_t address)
{
    WedgePowerLostAddr = address;
}

static uint32_t WedgePowerLostAddrGet(void)
{
    return WedgePowerLostAddr;
}

static uint8_t WedgePowerLostRecordErase(void)
{
    int32_t k = 0;
    uint32_t address = 0;
    char *pon = "O";

    for (k = 0; k < (WEDGE_POWER_LOST_INDICATE_TOTAL_SIZE / WEDGE_STORAGE_SECTOR_SIZE); k++)
    {
        address = (WEDGE_POWER_LOST_INDICATE_START_ADDR + k * WEDGE_STORAGE_SECTOR_SIZE) & (~WEDGE_STORAGE_SECTOR_ALIGN_MASK);
        if (0 != WedgeFlashEraseSector(address))
        {

            return 1;
        }
    }

    if (0 != WedgeFlashWriteData(WEDGE_POWER_LOST_INDICATE_START_ADDR, (uint8_t *)pon, 1))
    {
        return 2;
    }

    WedgePowerLostAddrSet(WEDGE_POWER_LOST_INDICATE_START_ADDR + 1);

    return 0;
}

uint8_t WedgeSetPowerLostBeforeReset(void)
{
    uint32_t address = WedgePowerLostAddrGet();
    char *poff = "F";

    if (0 != WedgeFlashWriteData(address, (uint8_t *)poff, 1))
    {
        return 1;
    }

    return 0;
}

uint8_t WedgeIsStartFromPowerLost(void)
{
    #define WEDGE_IS_POWER_LOST_READ_BUF_SIZE_BYTES (256)
    uint32_t oncount = 0, offcount = 0, address = 0;
    int32_t i = 0, j = 0, loops = 0;
    uint8_t readbuf[WEDGE_IS_POWER_LOST_READ_BUF_SIZE_BYTES] = {0}, ret = 0, breakflag = 0;
    char *WedgeIsStartFromPowerLostErrStr = "WEDGE Is Power Lost", *pon = "O";

    loops = WEDGE_POWER_LOST_INDICATE_TOTAL_SIZE / WEDGE_IS_POWER_LOST_READ_BUF_SIZE_BYTES;
    for (i = 0; i < loops; i++)
    {
        memset(readbuf, 0, sizeof(readbuf));
        if (0 != WedgeFlashReadData(WEDGE_POWER_LOST_INDICATE_START_ADDR + (i * sizeof(readbuf)), (uint8_t *)readbuf, sizeof(readbuf)))
        {
            WEDGE_DEV_INFO_PRINT(DbgCtl.WedgeDeviceInfoEn, "\r\n[%s] %s Err2",
                                      FmtTimeShow(), WedgeIsStartFromPowerLostErrStr);
            return 2;
        }

        for (j = 0; j < sizeof(readbuf); j++)
        {
            if (readbuf[j] == WEDGE_POWER_LOST_INDICATE_ON)
            {
                oncount++;
            }
            else if (readbuf[j] == WEDGE_POWER_LOST_INDICATE_OFF)
            {
                offcount++;
            }
            else if (readbuf[j] == 0xFF)
            {
                breakflag = 1;
                break;
            }
            else
            {
                WEDGE_DEV_INFO_PRINT(DbgCtl.WedgeDeviceInfoEn, "\r\n[%s] %s Err3",
                                   FmtTimeShow(), WedgeIsStartFromPowerLostErrStr);

                if (0 != WedgePowerLostRecordErase())
                {
                    WEDGE_DEV_INFO_PRINT(DbgCtl.WedgeDeviceInfoEn, "\r\n[%s] %s Erase Err1",
                                   FmtTimeShow(), WedgeIsStartFromPowerLostErrStr);
                }

                return 3;
            }
        }

        address = WEDGE_POWER_LOST_INDICATE_START_ADDR + (i * sizeof(readbuf)) + j;
        if (breakflag == 1)
        {
            break;
        }
    }

    if (oncount != offcount)
    {
        ret = 1;
    }

    if (breakflag == 1)
    {
        if (ret == 0)
        {
            if (0 != WedgeFlashWriteData(address, (uint8_t *)pon, 1))
            {
                WEDGE_DEV_INFO_PRINT(DbgCtl.WedgeDeviceInfoEn, "\r\n[%s] %s Err4",
                                   FmtTimeShow(), WedgeIsStartFromPowerLostErrStr);

                return 4;
            }

            WedgePowerLostAddrSet(address + 1);
        }
        else
        {
            WedgePowerLostAddrSet(address);
        }
    }
    
    if ((breakflag == 0) || (address >= WEDGE_POWER_LOST_INDICATE_END_ADDR))
    {
        if (0 != WedgePowerLostRecordErase())
        {
            WEDGE_DEV_INFO_PRINT(DbgCtl.WedgeDeviceInfoEn, "\r\n[%s] %s Erase Err2",
                               FmtTimeShow(), WedgeIsStartFromPowerLostErrStr);
        }
    }

    return ret;
}

static uint32_t WedgeDeviceInfoAddr = 0;
static uint32_t WedgeDeviceInfoAddrGet(void)
{
    return WedgeDeviceInfoAddr;
}

static void WedgeDeviceInfoAddrSet(uint32_t address)
{
    WedgeDeviceInfoAddr = address;
}

static uint8_t WedgeDeviceInfoEraseTotal(void)
{
    int32_t k = 0;
    uint32_t address = 0;

    for (k = 0; k < (WEDGE_DEVICE_INFO_TOTAL_SIZE / WEDGE_STORAGE_SECTOR_SIZE); k++)
    {
        address = (WEDGE_DEVICE_INFO_START_ADDR + k * WEDGE_STORAGE_SECTOR_SIZE) & (~WEDGE_STORAGE_SECTOR_ALIGN_MASK);
        if (0 != WedgeFlashEraseSector(address))
        {
            return 1;
        }
    }

    WedgeDeviceInfoAddrSet(WEDGE_DEVICE_INFO_START_ADDR);

    return 0;
}

uint8_t WedgeDeviceInfoWrite(uint8_t *pDeviceInfo, uint32_t infosize)
{
    #define WEDGE_DEVICE_INFO_VERIFY_DEPART_BYTES (2)
    uint32_t address = 0;
    WEDGEDeviceInfoHeadTypedef writebuf = {0};
    char *WedgeDeviceInfoWriteErrStr = "WEDGE Device Info Write";

    if ((pDeviceInfo == NULL) || (infosize == 0))
    {
        WEDGE_DEV_INFO_PRINT(DbgCtl.WedgeDeviceInfoEn, "\r\n[%s] %s Param Err",
                             FmtTimeShow(), WedgeDeviceInfoWriteErrStr);
        return 0xFE;
    }

    address = WedgeDeviceInfoAddrGet();
    writebuf.timestamp = WedgeRtcCurrentSeconds();
    writebuf.verifydata = WEDGE_DEVICE_INFO_VERIFY;

    if ((address % WEDGE_STORAGE_SECTOR_SIZE) == 0)
    {
        if (0 != WedgeFlashEraseSector(address))
        {
            WEDGE_DEV_INFO_PRINT(DbgCtl.WedgeDeviceInfoEn, "\r\n[%s] %s Err1",
                             FmtTimeShow(), WedgeDeviceInfoWriteErrStr);
            return 1;
        }
    }

    if (0 != WedgeFlashWriteData(address, (uint8_t *)&writebuf , 
    (sizeof(WEDGEDeviceInfoHeadTypedef) - WEDGE_DEVICE_INFO_VERIFY_DEPART_BYTES)))
    {
        WEDGE_DEV_INFO_PRINT(DbgCtl.WedgeDeviceInfoEn, "\r\n[%s] %s Err2",
                             FmtTimeShow(), WedgeDeviceInfoWriteErrStr);
        return 2;
    }

    if (0 != WedgeFlashWriteData(address + sizeof(WEDGEDeviceInfoHeadTypedef), pDeviceInfo, infosize))
    {
        WEDGE_DEV_INFO_PRINT(DbgCtl.WedgeDeviceInfoEn, "\r\n[%s] %s Err3",
                             FmtTimeShow(), WedgeDeviceInfoWriteErrStr);
        return 3;
    }

    if (0 != WedgeFlashWriteData(address, (uint8_t *)&writebuf, sizeof(WEDGEDeviceInfoHeadTypedef)))
    {
        WEDGE_DEV_INFO_PRINT(DbgCtl.WedgeDeviceInfoEn, "\r\n[%s] %s Err4",
                             FmtTimeShow(), WedgeDeviceInfoWriteErrStr);
        return 4;
    }

    return 0;
}

uint8_t WedgeDeviceInfoRead(uint8_t *pDeviceInfo, uint32_t infosize)
{
    uint32_t address = 0;
    int32_t i = 0, loops = 0;
    WEDGEDeviceInfoHeadTypedef readbuf = {0};
    WEDGEDeviceInfoHeadTypedef readbuftmp = {0};
    char *WedgeDeviceInfoReadErrStr = "WEDGE Device Info Read";

    if ((pDeviceInfo == NULL) || (infosize == 0))
    {
        WEDGE_DEV_INFO_PRINT(DbgCtl.WedgeDeviceInfoEn, "\r\n[%s] %s Param Err",
                             FmtTimeShow(), WedgeDeviceInfoReadErrStr);
        return 0xFE;
    }

    loops = WEDGE_DEVICE_INFO_TOTAL_SIZE / WEDGE_DEVICE_INFO_CELL_SIZE_BYTES;

    memset(&readbuftmp, 0, sizeof(readbuftmp));
    if (0 != WedgeFlashReadData(WEDGE_DEVICE_INFO_START_ADDR + (i * WEDGE_DEVICE_INFO_CELL_SIZE_BYTES)
        , (uint8_t *)&readbuftmp, sizeof(readbuftmp)))
    {
        WEDGE_DEV_INFO_PRINT(DbgCtl.WedgeDeviceInfoEn, "\r\n[%s] %s Err1",
                             FmtTimeShow(), WedgeDeviceInfoReadErrStr);
        return 1;
    }
    address = WEDGE_DEVICE_INFO_START_ADDR + (i * WEDGE_DEVICE_INFO_CELL_SIZE_BYTES);
    if ((readbuftmp.verifydata == WEDGE_DEVICE_INFO_DEFAULT) && (readbuftmp.timestamp == WEDGE_DEVICE_INFO_DEFAULT))
    {
        WEDGE_DEV_INFO_PRINT(DbgCtl.WedgeDeviceInfoEn, "\r\n[%s] %s No Info",
                             FmtTimeShow(), WedgeDeviceInfoReadErrStr);
        WedgeDeviceInfoAddrSet(WEDGE_DEVICE_INFO_START_ADDR);
        return 0xFF;
    }

    if ((readbuftmp.verifydata != WEDGE_DEVICE_INFO_VERIFY) && (readbuftmp.verifydata != WEDGE_DEVICE_INFO_DEFAULT))
    {
        if (0 != WedgeDeviceInfoEraseTotal())
        {
            WEDGE_DEV_INFO_PRINT(DbgCtl.WedgeDeviceInfoEn, "\r\n[%s] %s Device Info Section Init Err",
                                 FmtTimeShow(), WedgeDeviceInfoReadErrStr);
            return 0xFD;
        }
    }

    for (i = 1; i < loops; i++)
    {
        memset(&readbuf, 0, sizeof(readbuf));
        if (0 != WedgeFlashReadData(WEDGE_DEVICE_INFO_START_ADDR + (i * WEDGE_DEVICE_INFO_CELL_SIZE_BYTES)
            , (uint8_t *)&readbuf, sizeof(readbuf)))
        {
            WEDGE_DEV_INFO_PRINT(DbgCtl.WedgeDeviceInfoEn, "\r\n[%s] %s Err2",
                                      FmtTimeShow(), WedgeDeviceInfoReadErrStr);
            return 2;
        }

        if ((readbuf.verifydata == WEDGE_DEVICE_INFO_DEFAULT) && (readbuf.timestamp == WEDGE_DEVICE_INFO_DEFAULT))
        {
            break;
        }
        else if (readbuf.verifydata == WEDGE_DEVICE_INFO_VERIFY)
        {
            if (readbuftmp.timestamp <= readbuf.timestamp)
            {
                readbuftmp = readbuf;
                address = WEDGE_DEVICE_INFO_START_ADDR + (i * WEDGE_DEVICE_INFO_CELL_SIZE_BYTES);
                continue;
            }
            else
            {
                break;
            }
        }
        else
        {
            WEDGE_DEV_INFO_PRINT(DbgCtl.WedgeDeviceInfoEn, "\r\n[%s] %s Err3",
                                   FmtTimeShow(), WedgeDeviceInfoReadErrStr);
            if (0 != WedgeDeviceInfoEraseTotal())
            {
                WEDGE_DEV_INFO_PRINT(DbgCtl.WedgeDeviceInfoEn, "\r\n[%s] %s Err4",
                                   FmtTimeShow(), WedgeDeviceInfoReadErrStr);
                return 4;
            }
            return 3;
        }
    }

    address += WEDGE_DEVICE_INFO_CELL_SIZE_BYTES;
    
    if (address >= WEDGE_DEVICE_INFO_END_ADDR)
    {
        address = WEDGE_DEVICE_INFO_START_ADDR;
    }

    WedgeDeviceInfoAddrSet(address);

    if (0 != WedgeFlashReadData(address + sizeof(WEDGEDeviceInfoHeadTypedef), (uint8_t *)pDeviceInfo, infosize))
    {
        WEDGE_DEV_INFO_PRINT(DbgCtl.WedgeDeviceInfoEn, "\r\n[%s] %s Err5",
                             FmtTimeShow(), WedgeDeviceInfoReadErrStr);
        return 5;
    }

    return 0;
}


/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2017. All rights reserved
                                End Of The File
*******************************************************************************/
