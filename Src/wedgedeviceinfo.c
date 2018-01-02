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
#include "flash.h"

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

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2017. All rights reserved
                                End Of The File
*******************************************************************************/
