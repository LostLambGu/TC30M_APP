/*******************************************************************************
* File Name          : flash.c
* Author               : Jevon
* Description        : This file provides all the flash functions.

* History:
*  01/29/2016 : version V1.00
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "flash.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef NRCMD
#define NRCMD (1)
#endif

#define SerialPrintf SerialDbgPrintf
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
flash_info flash_data = {FALSE,0,0,0};
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void SerialFlashInit(void)
{
	uint32_t size;
	flash_data.bi_flashstart = 0;
	flash_data.bi_flashoffset = 0;
	if ((size = raspi_init()) == 0) 
	{
		flash_data.bi_flashvalidflag = FALSE;
	}
	else
	{
		flash_data.bi_flashvalidflag = TRUE;
		flash_data.bi_flashsize = size;
	}
}

FlashStatusT SerialFlashRead(uint8_t *buf, uint32_t from, int len)
{
	int Result = -1;	

	// Check address
	if(from > SERIAL_FLASH_END_ADDR)
	{
		return FLASH_STAT_OVERRUN;
	}

	// Check Length
	if(len <= 0)
	{
		return FLASH_STAT_LEN_ERR;
	}
	
	// Read data from serial flash
	Result = raspi_read((char*)buf, from, len);

	// Check Length
	if(Result ==  -1)
	{
		return FLASH_STAT_TIMEOUT;
	}
	else if(Result ==  len)
	{
		return FLASH_STAT_OK;
	}
	else
	{
		return FLASH_STAT_LEN_ERR;
	}
}

FlashStatusT SerialFlashWrite(uint8_t *buf, uint32_t to, int len)
{
	int Result = -1;

	// Check address
	if(to > SERIAL_FLASH_END_ADDR)
	{
		return FLASH_STAT_OVERRUN;
	}

	// Check Length
	if(len <= 0)
	{
		return FLASH_STAT_LEN_ERR;
	}

	// Write data to serial flash
	Result = raspi_write((char *)buf, to, len);
	
	// Check Length
	if(Result ==  -1)
	{
		return FLASH_STAT_TIMEOUT;
	}
	else if(Result ==  len)
	{
		return FLASH_STAT_OK;
	}
	else
	{
		return FLASH_STAT_LEN_ERR;
	}
}

FlashStatusT SerialFlashErase(EraseTypeT type, uint32_t index)
{
	int Result = -1;

	if(type == FLASH_ERASE_04KB)
	{
		if(index > (SERIAL_FLASH_END_ADDR / (4 * 1024)))
		{
			return FLASH_STAT_OVERRUN;
		}
		else
		{
			// Erase data
			Result = raspi_erase(type, index*4096);
		}
	}
	else if(type == FLASH_ERASE_08KB)
	{
		if(index > (SERIAL_FLASH_END_ADDR / (8 * 1024)))
		{
			return FLASH_STAT_OVERRUN;
		}
		else
		{
			// Erase data
			Result = raspi_erase(type, index*8192);
		}
	}
	else if(type == FLASH_ERASE_32KB)
	{
		if(index > (SERIAL_FLASH_END_ADDR / (32 * 1024)))
		{
			return FLASH_STAT_OVERRUN;
		}
		else
		{
			// Erase data
			Result = raspi_erase(type, index*32768);
		}
	}
	else if(type == FLASH_ERASE_64KB)
	{
		if(index > (SERIAL_FLASH_END_ADDR / (64 * 1024)))
		{
			return FLASH_STAT_OVERRUN;
		}
		else
		{
			// Erase data
			Result = raspi_erase(type, index*65536);
		}
	}
	else if(type == FLASH_ERASE_ALL)
	{
		raspi_chip_erase(OPCODE_BE);
		Result = 0;
	}

	// Check Result
	if(Result == 0)
	{
		return FLASH_STAT_OK;
	}
	else
	{
		return FLASH_STAT_ERROR;
	}
}

FlashStatusT SerialFlashBlocksErase(uint32_t start_block, uint32_t num_blocks)
{
	uint32_t len = 0;
	uint32_t offs = 0;

	if(num_blocks > spi_chip_info->block_size)
	{
		return FLASH_STAT_ERROR;
	}

	len = num_blocks * spi_chip_info->block_size;
	offs = start_block * spi_chip_info->block_size;

	while (len > 0) 
	{
		// Print Out
		SerialPrintf(DbgCtl.SerialFlashEn,"\r\n[%s] Flash: Erase Blocks len(0x%X) offs(0x%X)", \
			FmtTimeShow(), offs, len);
		// Erase OPT
		if (raspi_erase(FLASH_ERASE_64KB, offs)) 
		{
			return FLASH_STAT_ERROR;
		}
		offs += spi_chip_info->block_size;
		len -= spi_chip_info->block_size;
	}

	return FLASH_STAT_OK;
}


/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2017. All rights reserved
                                End Of The File
*******************************************************************************/

