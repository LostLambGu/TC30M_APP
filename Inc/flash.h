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

/* Wedge Spi Flash Defines -------------------------------------------------- */
#define WEDGE_STORAGE_SECTOR_SIZE (4096)
#define WEDGE_STORAGE_SECTOR_ALIGN_MASK (WEDGE_STORAGE_SECTOR_SIZE - 1)
#define WEDGE_STORAGE_BASE_ADDR (0x00000000)

#define WEDGE_POWER_LOST_INDICATE_ON 'O'
#define WEDGE_POWER_LOST_INDICATE_OFF 'F'
#define WEDGE_POWER_LOST_INDICATE_OFFSET (0)
#define WEDGE_POWER_LOST_INDICATE_START_ADDR (WEDGE_STORAGE_BASE_ADDR + WEDGE_POWER_LOST_INDICATE_OFFSET)
#define WEDGE_POWER_LOST_INDICATE_TOTAL_SIZE (WEDGE_STORAGE_SECTOR_SIZE * 16)
#define WEDGE_POWER_LOST_INDICATE_END_ADDR (WEDGE_POWER_LOST_INDICATE_START_ADDR + WEDGE_POWER_LOST_INDICATE_TOTAL_SIZE - 1)

#define WEDGE_MSG_QUE_OFFSET (WEDGE_STORAGE_SECTOR_SIZE * 256)
#define WEDGE_MSG_QUE_START_ADDR (WEDGE_STORAGE_BASE_ADDR + WEDGE_MSG_QUE_OFFSET)
#define WEDGE_MSG_QUE_TOTAL_SIZE (WEDGE_STORAGE_SECTOR_SIZE * 256)
// #define WEDGE_MSG_QUE_TOTAL_SIZE (WEDGE_STORAGE_SECTOR_SIZE * 8) /*Just for test!*/
#define WEDGE_MSG_QUE_TOTAL_NUM (WEDGE_MSG_QUE_TOTAL_SIZE / sizeof(WEDGEMsgQueCellTypeDef))
#define WEDGE_MSG_QUE_END_ADDR (WEDGE_MSG_QUE_START_ADDR + WEDGE_MSG_QUE_TOTAL_SIZE - 1)

#endif  /* _SERIAL_FLAH__H */

/*******************************************************************************
                        Copyrights (C) Asiatelco Technologies Co., 2003-2016. All rights reserved
                                                             End Of The File
*******************************************************************************/

