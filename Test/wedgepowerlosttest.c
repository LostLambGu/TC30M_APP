#include "wedgetest.h"

#if WEDGE_POWER_LOST_TEST

void WedgePowerLostTest(void)
{
    uint8_t ret = 0;
    uint8_t buf[1024] = {0};
    int32_t i = 0, k = 0;
    uint32_t address = 0;
    // Spi Flash Init
	SerialFlashInit();

    // if (0 == WedgeFlashEraseSector(WEDGE_POWER_LOST_INDICATE_START_ADDR))
    // {
    //     DebugLog("Power lost EraseSector OK");
    // }

    // if (0 == WedgeFlashWriteData(WEDGE_POWER_LOST_INDICATE_START_ADDR, (uint8_t *)"12345678", 8))
    // {
    //     DebugLog("Power lost WriteData OK");
    // }
    // while (1)
    //     ;

    // for (i = 0; i < sizeof(buf); i++)
    // {
    //     if (i % 2 == 0)
    //     {
    //         buf[i] = 'O';
    //     }
    //     else
    //     {
    //         buf[i] = 'F';
    //     }
    // }

    // for (k = 0; k < (WEDGE_POWER_LOST_INDICATE_TOTAL_SIZE / WEDGE_STORAGE_SECTOR_SIZE); k++)
    // {
    //     address = (WEDGE_POWER_LOST_INDICATE_START_ADDR + k * WEDGE_STORAGE_SECTOR_SIZE) & (~WEDGE_STORAGE_SECTOR_ALIGN_MASK);
    //     if (0 != WedgeFlashEraseSector(address))
    //     {

    //         DebugLog("Power lost EraseSector err");
    //     }
    // }

    // // for (k = 0; k < (WEDGE_POWER_LOST_INDICATE_TOTAL_SIZE / sizeof(buf)); k++)
    // // {
    // //     address = (WEDGE_POWER_LOST_INDICATE_START_ADDR + k * sizeof(buf));
    // //     if (0 != WedgeFlashWriteData(address, buf, sizeof(buf)))
    // //     {

    // //         DebugLog("Power lost WriteData err");
    // //     }
    // // }

    // for (k = 0; k < (WEDGE_POWER_LOST_INDICATE_TOTAL_SIZE / sizeof(buf)) - 1; k++)
    // {
    //     address = (WEDGE_POWER_LOST_INDICATE_START_ADDR + k * sizeof(buf));
    //     if (0 != WedgeFlashWriteData(address, buf, sizeof(buf)))
    //     {

    //         DebugLog("Power lost WriteData err");
    //     }
    // }

    // buf[sizeof(buf) - 1] = 0xff;
    // address = (WEDGE_POWER_LOST_INDICATE_START_ADDR + k * sizeof(buf));
    // if (0 != WedgeFlashWriteData(address, buf, sizeof(buf)))
    // {

    //     DebugLog("Power lost WriteData err");
    // }

    // while (1)
    //     ;

    ret = WedgeIsStartFromPowerLost();
    if (0 == ret )
    {
        DebugLog("------------------->>>>>>>No power lost");
    }
    else if (1 == ret)
    {
        DebugLog("------------------->>>>>>>Power lost");
    }
    else
    {
        DebugLog("Power lost err");
    }

    HAL_Delay(10000);
    DebugLog("Power lost set Before reset");

    if (0 == WedgeSetPowerLostBeforeReset())
    {
        DebugLog("Power lost set ok");
    }
    else
    {
        DebugLog("Power lost set err");
    }

    DebugLog("MCUReset");
    MCUReset();
}

#endif
