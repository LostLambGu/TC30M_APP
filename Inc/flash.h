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
#include "include.h"
#include "spi_driver.h"

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
	u8		bi_flashvalidflag;
	u32 		bi_flashstart;	/* start of FLASH memory */
	u32	 	bi_flashsize;	/* size  of FLASH memory */
	u32 		bi_flashoffset;	/* reserved area for startup monitor */
}flash_info;


//Function Declare
extern void SerialFlashInit(void);
extern FlashStatusT SerialFlashRead(u8 *buf, uint from, int len);
extern FlashStatusT SerialFlashWrite(u8 *buf, uint to, int len);
extern FlashStatusT SerialFlashErase(EraseTypeT type, uint index);
extern FlashStatusT SerialFlashBlocksErase(u32 start_block, u32 num_blocks);

#endif  /* _SERIAL_FLAH__H */

/*******************************************************************************
                        Copyrights (C) Asiatelco Technologies Co., 2003-2016. All rights reserved
                                                             End Of The File
*******************************************************************************/

