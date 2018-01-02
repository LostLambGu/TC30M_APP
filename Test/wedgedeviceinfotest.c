#include "wedgetest.h"

#if WEDGE_DEVICE_INFO_TEST
void WedgeDeviceInfoTest(void)
{
    uint8_t DeviceInfo[32] = {0};
    uint32_t i = 0, count = 0;
    
    // Spi Flash Init
	SerialFlashInit();

    while (1)
    {
        if (0 == WedgeDeviceInfoRead(DeviceInfo, sizeof(DeviceInfo)))
        {
            DebugLog("WedgeDeviceInfoRead ok");
        }

        DebugPrintf(1, "\r\n");
        for (i = 0; i < sizeof(DeviceInfo); i++)
        {
            DebugPrintf(1, "%x", DeviceInfo[i]);
        }
        DebugPrintf(1, "\r\n");

        if (0 == WedgeDeviceInfoWrite(DeviceInfo, sizeof(DeviceInfo)))
        {
            DebugLog("WedgeDeviceInfoWrite ok");
        }

        HAL_Delay(1000);

        count++;

        if (count == 5)
        {
            if (0 == WedgeFlashEraseSector(WEDGE_DEVICE_INFO_START_ADDR + WEDGE_DEVICE_INFO_CELL_SIZE_BYTES * 4))
            {
                DebugLog("WedgeFlashEraseSector OK");
            }

            if (0 == WedgeFlashWriteData(WEDGE_DEVICE_INFO_START_ADDR + WEDGE_DEVICE_INFO_CELL_SIZE_BYTES * 4, (uint8_t *)"12345678", 8))
            {
                DebugLog("WedgeFlashWriteData OK");
            }
        }
    }
    

}

#endif
