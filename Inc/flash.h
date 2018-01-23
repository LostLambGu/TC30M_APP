/*******************************************************************************
* File Name          : flash.h
* Author               : Jevon
* Description        : This file provides all the flash functions.

* History:
*  11/08/2016 : version V1.00
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _SERIAL_FLAH__H
#define _SERIAL_FLAH__H
/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
#include "spi_driver.h"
#include "uart_api.h"

#define SERIAL_FLASH_START_ADDR			0x00000000
#define SERIAL_FLASH_END_ADDR			0x00400000

typedef enum 
{
  FLASH_STAT_OK       		= 0x00,
  FLASH_STAT_ERROR    	= 0x01,
  FLASH_STAT_LEN_ERR    	= 0x02,
  FLASH_STAT_OVERRUN  	= 0x03,
  FLASH_STAT_TIMEOUT  	= 0x04
} FlashStatusT;

typedef struct
{
	uint8_t		bi_flashvalidflag;
	uint32_t 		bi_flashstart;	/* start of FLASH memory */
	uint32_t	 	bi_flashsize;	/* size  of FLASH memory */
	uint32_t 		bi_flashoffset;	/* reserved area for startup monitor */
}flash_info;


//Function Declare
extern void SerialFlashInit(void);
extern FlashStatusT SerialFlashRead(uint8_t *buf, uint32_t from, int len);
extern FlashStatusT SerialFlashWrite(uint8_t *buf, uint32_t to, int len);
extern FlashStatusT SerialFlashErase(EraseTypeT type, uint32_t index);
extern FlashStatusT SerialFlashBlocksErase(uint32_t start_block, uint32_t num_blocks);


#define JUMP_TO_BOOTLOADER_ADDRESS (0x08000000)
#define JUMP_TO_APPLICATION_ADDRESS (0x08005000)

#define IAP_BOOT_FLASH (uint32_t)(0xAA) // 170
#define IAP_BOOT_UART (uint32_t)(0xBB) // 187
#define IAP_APP_FLASH (uint32_t)(0xCC)  // 204
#define IAP_APP_UART (uint32_t)(0xDD)  // 221
#define IAP_RESET (uint32_t)(0xFF)     // 255
#define IAP_FLAG (uint32_t)(JUMP_TO_APPLICATION_ADDRESS - 0x400)

#define ADDR_FLASH_PAGE_9 (9 * FLASH_PAGE_SIZE + 0x08000000)
#define ADDR_FLASH_PAGE_8 (8 * FLASH_PAGE_SIZE + 0x08000000)
#define FLASH_USER_END_ADDR (128 * FLASH_PAGE_SIZE + 0x08000000)

/* Get the number of Sector from where the user program will be loaded */
#define FLASH_PAGE_NUMBER (uint32_t)((JUMP_TO_APPLICATION_ADDRESS - 0x08000000) >> 12)

/* Compute the mask to test if the Flash memory, where the user program will be
   loaded, is write protected */
#define FLASH_PROTECTED_PAGES ((uint32_t) ~((1 << FLASH_PAGE_NUMBER) - 1))

#define USER_FLASH_END_ADDRESS FLASH_USER_END_ADDR

/* define the user application size */
#define USER_FLASH_SIZE (USER_FLASH_END_ADDRESS - JUMP_TO_APPLICATION_ADDRESS + 1)

#define FIRMWARE_VERSION_LEN_MAX (32)
typedef struct FirmwareInfo
{
	uint32_t IAPFlag;
	uint32_t FirmwareSize;
	char Version[FIRMWARE_VERSION_LEN_MAX];
} FirmwareInfoTypeDef;

#endif  /* _SERIAL_FLAH__H */

/*******************************************************************************
                        Copyrights (C) Asiatelco Technologies Co., 2003-2016. All rights reserved
                                                             End Of The File
*******************************************************************************/

