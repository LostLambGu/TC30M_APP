/*******************************************************************************
* File Name          : spi_driver.c
* Author               : Jevon
* Description        : This file provides all the spi_driver functions.

* History:
*  05/15/2015 : spi_driver V1.00
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "spi_driver.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
GPIO_InitTypeDef SPIGPIO;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

chip_info *spi_chip_info = NULL;
static const chip_info chips_data [] = {
	/* REVISIT: fill in JEDEC ids, for parts that have them */
	{ "UNKNOWN",							0, 					0, 								 		0, 					0,  	0 },
	//{ "AT25DF321",			0x1f, 0x47000000, 64 * 1024, 64,  	0 },
	//{ "AT26DF161",			0x1f, 0x46000000, 64 * 1024, 32,  	0 },
	//{ "FL016AIF",					0x01, 0x02140000, 64 * 1024, 32,  	0 },
	//{ "FL064AIF",					0x01, 0x02160000, 64 * 1024, 128, 	0 },
	// { "MX25L2006D",		0xc2, 0x2012c220, 	 4 * 1024, 64,  	0 },
	//{ "MX25L1605D",		0xc2, 0x2015c220, 64 * 1024, 32,  	0 },
	//{ "MX25L3205D",		0xc2, 0x2016c220, 64 * 1024, 64,  	0 },
	// { "MX25L6405D",		0xc2, 0x2017c220, 64 * 1024, 128, 	0 },
	{ "MX25L6406E",		0xc2, 0x2017c216, 64 * 1024, 128, 	0 },
	//{ "MX25L12805D",	0xc2, 0x2018c220, 64 * 1024, 256, 	0 },
	//#ifndef NO_4B_ADDRESS_SUPPORT
	//{ "MX25L25635E",	0xc2, 0x2019c220, 64 * 1024, 512, 	1 },
	//{ "S25FL256S",			0x01, 0x02194D01, 64 * 1024, 512, 	1 },
	//#endif
	//{ "S25FL128P",			0x01, 0x20180301, 64 * 1024, 256, 	0 },
	//{ "S25FL129P",			0x01, 0x20184D01, 64 * 1024, 256, 	0 },
	//{ "S25FL032P",			0x01, 0x02154D00, 64 * 1024, 64,  	0 },
	//{ "S25FL064P",			0x01, 0x02164D00, 64 * 1024, 128, 	0 },
	//{ "F25L64QA",       	0x8c, 0x41170000, 64 * 1024, 128, 	0 }, //ESMT
	//{ "F25L128QA",     	0x8c, 0x41180000, 64 * 1024, 256, 	0 }, //F25L128QA
	//{ "EN25F16",					0x1c, 0x31151c31, 64 * 1024, 32,  	0 },
	//{ "EN25Q32B",				0x1c, 0x30161c30, 64 * 1024, 64,  	0 },
	//{ "EN25F32",					0x1c, 0x31161c31, 64 * 1024, 64,  	0 },
	//{ "EN25F64",					0x1c, 0x20171c20, 64 * 1024, 128,	0 }, //EN25P64
	//{ "EN25Q64",         0x1c, 0x30171c30, 64 * 1024, 128,	0 },
	//{ "EN25Q128",       0x1c, 0x30181c30, 64 * 1024, 256,	0 }, //EN25Q128
	//{ "W25Q32BV",			0xef, 0x40160000, 64 * 1024, 64,  	0 }, //S25FL032K
	//{ "W25Q64BV",			0xef, 0x40170000, 64 * 1024, 128,	0 }, //S25FL064K
	//{ "W25Q128BV",		0xef, 0x40180000, 64 * 1024, 256,	0 }, //S25FL064K
	//{ "W25Q128FW",		0xef, 0x60180000, 64 * 1024, 256,	0 }, //W25Q128FW
};

static int raspi_wait_ready(int sleep_ms, u8 PrintEn);

static void udelay(u32 time)	//delay 5+2*time (machine time)
{
	u32 Count;
	Count = time;
	while(--Count);
	while(--time);
}

static void SendByte(uint8 byte_value)
{
	uint8 i;
	uint8 cycle_cnt;

	cycle_cnt = 8;
	for( i= 0; i < cycle_cnt; i++ )
	{
	    if ( (byte_value & 0x80) == 0x80 )
	        SPI_MOSI_HIGH();	//SI = 1;
	    else
	        SPI_MOSI_LOW();	//SI = 0;
	    SPI_SCK_LOW(); 			//SCLK = 0;
	    byte_value = byte_value << 1;
	    SPI_SCK_HIGH();			//SCLK = 1;
	}
}

static uint8 GetByte(void)
{
	uint8 i;
	uint8 data_buf;
	data_buf = 0;

	// Set VIP 8051 GPIO as input ( need pull to high )
	SPI_MOSI_HIGH(); //SO = 1;
	// End VIP 8051 GPIO

	for( i= 0; i < 8; i++ )
	{
	    SPI_SCK_LOW(); //SCLK = 0;
	    if ( SPI_MISO_READ() == GPIO_PIN_SET )
	    {
	        data_buf = (data_buf | (0x80 >> i));
	    }
	    SPI_SCK_HIGH(); //SCLK = 1;
	}
	   
	return data_buf;
}
static int spic_transfer(const u8 *cmd, int n_cmd, u8 *buf, int n_buf, int flag)
{
	int retval = -1;
	/*SPIPrintf(PrintEn,"\r\n[%s] SPI:cmd(%x): %02x %02x %02x %02x, buf:%02x len:%04d, flag:%s", \
		FmtTimeShow(),
		n_cmd, cmd[0], cmd[1], cmd[2], cmd[3],
		(buf)? (*buf) : 0, n_buf,
		(flag == SPIC_READ_BYTES)? "read" : "write");*/

	// assert CS and we are already CLK normal high
	SPI_WPEN_HIGH();
	SPI_CS_LOW();
	
	// write command
	for (retval = 0; retval < n_cmd; retval++) 
	{
		SendByte(cmd[retval]);
	}

	// read / write  data
	if (flag & SPIC_READ_BYTES) 
	{
		for (retval = 0; retval < n_buf; retval++) 
		{
			buf[retval] = GetByte();
		}
	}
	else if (flag & SPIC_WRITE_BYTES) 
	{
		for (retval = 0; retval < n_buf; retval++) 
		{
			SendByte(buf[retval]);
		}
	}

	SPI_CS_HIGH();
	SPI_WPEN_LOW();

	return retval;
}

static int spic_read(const u8 *cmd, size_t n_cmd, u8 *rxbuf, size_t n_rx)
{
	return spic_transfer(cmd, n_cmd, rxbuf, n_rx, SPIC_READ_BYTES);
}

static int spic_write(const u8 *cmd, size_t n_cmd, const u8 *txbuf, size_t n_tx)
{
	return spic_transfer(cmd, n_cmd, (u8 *)txbuf, n_tx, SPIC_WRITE_BYTES);
}

#if defined (RD_MODE_QUAD)
static int raspi_set_quad(u8 PrintEn)
{
	int retval = 0;

	// Atmel set quad is not tested yet,
	if (spi_chip_info->id == 0x1f) // Atmel, Write the 7th bit of Configuration register
	{
		u8 sr;
		retval = raspi_cmd(0x3f, 0, 0, &sr, 1, 0, SPIC_READ_BYTES);
		if (retval == -1)
			goto err_end;
		if ((sr & (1 << 7)) == 0)
		{
			sr |= (1 << 7);
			raspi_write_enable();
			retval = raspi_cmd(0x3e, 0, 0, &sr, 1, 0, SPIC_WRITE_BYTES);
		}
	}
	else if (spi_chip_info->id == 0xc2) //MXIC, 
	{
		u8 sr;
		retval = raspi_cmd(OPCODE_RDSR, 0, 0, &sr, 1, 0, SPIC_READ_BYTES);
		if (retval == -1)
			goto err_end;
		if ((sr & (1 << 6)) == 0)
		{
			sr |= (1 << 6);
			raspi_write_enable();
			retval = raspi_cmd(OPCODE_WRSR, 0, 0, &sr, 1, 0, SPIC_WRITE_BYTES);
		}
	}
	else if ((spi_chip_info->id == 0x01) || (spi_chip_info->id == 0xef)) // Spansion or WinBond
	{
		u8 sr[2];
		retval = raspi_cmd(OPCODE_RDSR, 0, 0, sr, 1, 0, SPIC_READ_BYTES);
		if (retval == -1)
			goto err_end;
		retval = raspi_cmd(0x35, 0, 0, &sr[1], 1, 0, SPIC_READ_BYTES);
		if (retval == -1)
			goto err_end;
		if ((sr[1] & (1 << 1)) == 0)
		{
			sr[1] |= (1 << 1);
			//raspi_write_enable();
			retval = raspi_cmd(OPCODE_WRSR, 0, 0, sr, 2, 0, SPIC_WRITE_BYTES);
		}
	}	

err_end:
	if (retval == -1)
		SPIPrintf(PrintEn,"\r\n[%s] SPI:raspi set quad error",FmtTimeShow());

	return retval;
}
#endif

/*
 * read SPI flash device ID
 */
static int raspi_read_devid(u8 *rxbuf, int n_rx, u8 PrintEn)
{
	u8 code = OPCODE_RDID;
	int retval;
	
	retval = spic_read(&code, 1, rxbuf, n_rx);
	if (retval != n_rx) 
	{
		SPIPrintf(PrintEn,"\r\n[%s] SPI:read devid ret(%x)", \
			FmtTimeShow(), retval);
		return retval;
	}
	return retval;
}



#ifndef NO_4B_ADDRESS_SUPPORT
static int raspi_read_rg(u8 *val, u8 opcode, u8 PrintEn)
{
	int retval;
	u8 code = opcode;
	
	if (!val)
		SPIPrintf(PrintEn,"\r\n[%s] SPI:null pointer",FmtTimeShow());

	retval = spic_read(&code, 1, val, 1);

	return retval;
}

static int raspi_write_rg(u8 *val, u8 opcode, u8 PrintEn)
{
	int retval;
	u8 code = opcode;
	//u32 dr;

	if (!val)
		SPIPrintf(PrintEn,"\r\n[%s] SPI:null pointer",FmtTimeShow());

	//dr = ra_inl(RT2880_SPI_DMA);
	//ra_outl(RT2880_SPI_DMA, 0); // Set TxBurstSize to 'b00: 1 transfer

	retval = spic_write(&code, 1, val, 1);

	//ra_outl(RT2880_SPI_DMA, dr);
	return retval;
}
#endif



/*
 * read status register
 */
static int raspi_read_sr(u8 *val, u8 PrintEn)
{
	int retval;
	u8 code = OPCODE_RDSR;

	retval = spic_read(&code, 1, val, 1);
	if (retval != 1) {
		SPIPrintf(PrintEn,"\r\n[%s] SPI:read sr ret(%x)",FmtTimeShow(), retval);
		return -1;
	}
	return 0;
}

/*
 * write status register
 */
static int raspi_write_sr(u8 *val, u8 PrintEn)
{
	int retval;
	u8 code = OPCODE_WRSR;

	retval = spic_write(&code, 1, val, 1);
	if (retval != 1) 
	{
		SPIPrintf(PrintEn,"\r\n[%s] SPI:write sr ret(%x)",FmtTimeShow(), retval);
		return -1;
	}
	return 0;
}

#ifndef NO_4B_ADDRESS_SUPPORT
#ifdef SPI_FLASH_DBG_CMD
static int raspi_read_scur(u8 *val, u8 PrintEn)
{
	int retval;
	u8 code = 0x2b;

	retval = spic_read(&code, 1, val, 1);
	if (retval != 1) {
		SPIPrintf(PrintEn,"\r\n[%s] SPI:read scur ret(%x)",FmtTimeShow(), retval);
		return -1;
	}
	return 0;
}
#endif

static int raspi_4byte_mode(int Enable, u8 PrintEn)
{
	if (spi_chip_info->id == 0x01) // Spansion
	{
		u8 br, br_cfn; // bank register

		raspi_wait_ready(1, PrintEn);

		if (Enable)
		{
			br = 0x81;
			//ra_or(RT2880_SPICFG_REG, SPICFG_ADDRMODE);
		}
		else
		{
			br = 0x0;
			//ra_and(RT2880_SPICFG_REG, ~(SPICFG_ADDRMODE));
		}
	
		raspi_write_rg(&br, OPCODE_BRWR, PrintEn);
		raspi_read_rg(&br_cfn, OPCODE_BRRD, PrintEn);
		if (br_cfn != br)
		{
			SPIPrintf(PrintEn,"\r\n[%s] SPI:4B mode switch failed",FmtTimeShow());
			return -1;
		}
	}
	else
	{
		int retval;
		u8 code;

		raspi_wait_ready(1, PrintEn);
	
		if (Enable)
		{
			code = 0xB7; /* EN4B, enter 4-byte mode */
			//ra_or(RT2880_SPICFG_REG, SPICFG_ADDRMODE);
		}
		else
		{
			code = 0xE9; /* EX4B, exit 4-byte mode */
			//ra_and(RT2880_SPICFG_REG, ~(SPICFG_ADDRMODE));
		}
		retval = spic_read(&code, 1, 0, 0);
		if (retval != 0) {
			SPIPrintf(PrintEn,"\r\n[%s] SPI:4byte mode ret(%x)",FmtTimeShow(), retval);
			return -1;
		}
	}
	return 0;
}
#endif



/*
 * Set write enable latch with Write Enable command.
 * Returns negative if error occurred.
 */
static int raspi_write_enable(void)
{
	u8 code = OPCODE_WREN;
	return spic_write(&code, 1, NULL, 0);
}

static int raspi_write_disable(void)
{
	u8 code = OPCODE_WRDI;
	return spic_write(&code, 1, NULL, 0);
}

/*
 * Set all sectors (global) unprotected if they are protected.
 * Returns negative if error occurred.
 */
static int raspi_unprotect(u8 PrintEn)
{
	u8 sr = 0;

	if (raspi_read_sr(&sr, PrintEn) < 0) 
	{
		SPIPrintf(PrintEn,"\r\n[%s] SPI:unprotect fail(%x)",FmtTimeShow(), sr);
		return -1;
	}

	if ((sr & (SR_BP0 | SR_BP1 | SR_BP2)) != 0) {
		sr = 0;
		raspi_write_sr(&sr, PrintEn);
	}
	return 0;
}

/*
 * Service routine to read status register until ready, or timeout occurs.
 * Returns non-zero if error.
 */
static int raspi_wait_ready(int sleep_ms, u8 PrintEn)
{
	int count;
	int sr = 0;

	//udelay(1000 * sleep_ms);

	/* one chip guarantees max 5 msec wait here after page writes,
	 * but potentially three seconds (!) after page erase.
	 */
	for (count = 0;  count < ((sleep_ms+1) *1000); count++) 
	{
		if ((raspi_read_sr((u8 *)&sr, PrintEn)) < 0)
			break;
		
		else if (!(sr & SR_WIP)) 
		{
			return 0;
		}
		udelay(500);
		/* REVISIT sometimes sleeping would be best */
	}
	SPIPrintf(PrintEn,"\r\n[%s] SPI:wait ready fail(%x)",FmtTimeShow(), sr);
	return -1;
}

/*
 * Erase one sector of flash memory at offset ``offset'' which is any
 * address within the sector which should be erased.
 *
 * Returns 0 if successful, non-zero otherwise.
 */
static int raspi_erase_sector(u8 opcode, u32 offset, u8 PrintEn)
{
	u8 buf[5];

	/* Wait until finished previous write command. */
	if (raspi_wait_ready(3, PrintEn))
		return -1;

	/* Send write enable, then erase commands. */
	raspi_write_enable();
	raspi_unprotect(PrintEn);

#ifdef ADDRESS_4B_MODE
	if (spi_chip_info->addr4b) {

		raspi_4byte_mode(1, PrintEn);
		raspi_write_enable();

		buf[0] = opcode;
		buf[1] = offset >> 24;
		buf[2] = offset >> 16;
		buf[3] = offset >> 8;
		buf[4] = offset;
		spic_write(buf, 5, 0 , 0);
		raspi_wait_ready(950, PrintEn);

		raspi_4byte_mode(0, PrintEn);
		return 0;
	}
#endif

	/* Set up command buffer. */
	buf[0] = opcode;
	buf[1] = offset >> 16;
	buf[2] = offset >> 8;
	buf[3] = offset;

	spic_write(buf, 4, 0 , 0);
	raspi_wait_ready(950, PrintEn);

	raspi_write_disable();

	return 0;
}

static chip_info *chip_prob(u8 PrintEn)
{
	chip_info *info;
	chip_info *match;
	int i =0;
	u32 jedec = 0;
	//u32 weight = 0;
	u8 buf[5] = {'\0'};

	raspi_read_devid(buf, 5, PrintEn);
	jedec = (u32)((u32)(buf[1] << 24) | ((u32)buf[2] << 16) | ((u32)buf[3] <<8) | (u32)buf[4]);

	SPIPrintf(PrintEn,"\r\n[%s] SPI:device id:%X %X %X %X %X (%X)", \
		FmtTimeShow(), buf[0], buf[1], buf[2], buf[3], buf[4], jedec);

	// FIXME, assign default as AT25D
	//weight = 0xffffffff;
	match =(chip_info *) (&chips_data[0]);
	for (i = 0; i < sizeof(chips_data)/sizeof(chips_data[0]); i++) 
	{
		info = (chip_info *)(&chips_data[i]);
		if (info->id == buf[0]) 
		{
			if (info->jedec_id == jedec) 
			{
				SPIPrintf(PrintEn,"\r\n[%s] SPI:find flash:%s",FmtTimeShow(), info->name);
				return info;
			}
			
			/*if (weight > (info->jedec_id ^ jedec)) 
			{
				weight = info->jedec_id ^ jedec;
				match = info;
			}*/
		}
	}
	SPIPrintf(PrintEn,"\r\n[%s] SPI:Un-recognized chip ID",FmtTimeShow());

	return match;
}

int spic_init(u8 EnableDelay)
{
	__HAL_RCC_GPIOC_CLK_ENABLE();
	SPI_SET_CS_OUT();
	SPI_SET_SCK_OUT();
	SPI_SET_MOSI_OUT();
	SPI_SET_WPEN_OUT();
	SPI_SET_MISO_IN();
	// Init Status
	SPI_CS_HIGH();
	SPI_WPEN_LOW();
	SPI_SCK_HIGH();
	SPI_MOSI_HIGH();

	if(EnableDelay == TRUE)
	{
		udelay(1);
	}

	return 0;
}

u32 raspi_init(u8 PrintEn)
{
	/*
	SPI:device id:C2 20 17 C2 20 (2017C220)
	SPI:find Flash:MX25L6405D
	SPI:sector size(65536) total sector(128)
	SPI:read from(0x21000) len(1800)
	*/
	spic_init(TRUE);
	spi_chip_info = chip_prob(PrintEn);
	SPIPrintf(PrintEn,"\r\n[%s] SPI:sector size(%d) total sector(%d)", \
		FmtTimeShow(), \
		spi_chip_info->sector_size,\
		spi_chip_info->n_sectors);
	return (spi_chip_info->sector_size * spi_chip_info->n_sectors);
}

int raspi_erase(u8 opcode, uint offs, u8 PrintEn)
{
	SPIPrintf(PrintEn,"\r\n[%s] SPI:erase op(0x%X) offs(0x%X)", \
		FmtTimeShow(), opcode, offs);

	return raspi_erase_sector(opcode, offs, PrintEn);
}

int raspi_read(u8 *buf, uint from, int len, u8 PrintEn)
{
	u8 cmd[5];
	int rdlen;
	//u32 start_time, end_time;

	SPIPrintf(PrintEn,"\r\n[%s] SPI:read from(0x%X) len(%d)", \
		FmtTimeShow(), from, len);

	/* sanity checks */
	if (len == 0)
		return 0;

	/* Wait till previous write/erase is done. */
	if (raspi_wait_ready(1, PrintEn)) 
	{
		/* REVISIT status return?? */
		return -1;
	}

	/* NOTE: OPCODE_FAST_READ (if available) is faster... */

	//start_time = get_timer(0);

	/* Set up the write data buffer. */
	cmd[0] = OPCODE_READ;

	#ifndef READ_BY_PAGE
	#ifdef ADDRESS_4B_MODE
	if (spi_chip_info->addr4b) {
		raspi_4byte_mode(1, PrintEn);
		cmd[1] = from >> 24;
		cmd[2] = from >> 16;
		cmd[3] = from >> 8;
		cmd[4] = from;
		rdlen = spic_read(cmd, 5, buf , len);
		raspi_4byte_mode(0, PrintEn);
	}
	else
	#endif
	/*current utility start*/
	{
		cmd[1] = from >> 16;
		cmd[2] = from >> 8;
		cmd[3] = from;
		rdlen = spic_read(cmd, 4, buf, len);
	}
	if(rdlen != len)
		SPIPrintf(PrintEn,"\r\n[%s] SPI:warning: rdlen != len",FmtTimeShow());
	/*current utility end*/
	#else
	#ifdef ADDRESS_4B_MODE
	if (spi_chip_info->addr4b) 
	{
		u32 page_size;
		
		rdlen = 0;
		raspi_4byte_mode(1, PrintEn);
		while (len > 0) {

			cmd[1] = from >> 24;
			cmd[2] = from >> 16;
			cmd[3] = from >> 8;
			cmd[4] = from;

			page_size = min(len, FLASH_PAGESIZE);
			rdlen += spic_read(cmd, 5, buf , page_size);
			buf += page_size;
			len -= page_size;
			from += page_size;
		}
		raspi_4byte_mode(0, PrintEn);
	}
	else
	#endif
	{
		u32 page_size;

		rdlen = 0;
		while (len > 0) {

			cmd[1] = from >> 16;
			cmd[2] = from >> 8;
			cmd[3] = from;

			page_size = min(len, FLASH_PAGESIZE);
			rdlen += spic_read(cmd, 4, buf, page_size);
			buf += page_size;
			len -= page_size;
			from += page_size;
		}
	}
#endif
	//end_time = get_timer(0);
	//SPIPrintf(PrintEn,"\r\n[%s] SPI:time spend: 0x%X",FmtTimeShow(), (end_time - start_time));

	return rdlen;
}

int raspi_write(u8 *buf, uint to, int len, u8 PrintEn)
{
	int rc = 0;
	int retlen = 0;
	u8 cmd[5];
	u32 page_offset = 0;
	u32 page_size = 0;

	SPIPrintf(PrintEn,"\r\n[%s] SPI:write to(0x%X) len(%d) addr4b(%d)", \
		FmtTimeShow(), to, len,spi_chip_info->addr4b);

	/* sanity checks */
	if(len == 0)
		return 0 ;
	
	if(to + len > (spi_chip_info->sector_size * spi_chip_info->n_sectors))
		return -1;

	/* Wait until finished previous write command. */
	if (raspi_wait_ready(2, PrintEn)) 
		return -1;

	/* Set up the opcode in the write buffer. */
	cmd[0] = OPCODE_PP;
	#ifdef ADDRESS_4B_MODE
	if (spi_chip_info->addr4b) 
	{
		cmd[1] = to >> 24;
		cmd[2] = to >> 16;
		cmd[3] = to >> 8;
		cmd[4] = to;
	}
	else
	#endif
	{
		cmd[1] = to >> 16;
		cmd[2] = to >> 8;
		cmd[3] = to;
	}

	/* what page do we start with? */
	page_offset = to % FLASH_PAGESIZE;

	#ifdef ADDRESS_4B_MODE
	if (spi_chip_info->addr4b) 
	{
		raspi_4byte_mode(1, PrintEn);
	}
	#endif

	/* write everything in PAGESIZE chunks */
	while (len > 0) 
	{
		page_size = min(len, FLASH_PAGESIZE-page_offset);
		page_offset = 0;
		/* write the next page to flash */
		#ifdef ADDRESS_4B_MODE
		if (spi_chip_info->addr4b) 
		{
			cmd[1] = to >> 24;
			cmd[2] = to >> 16;
			cmd[3] = to >> 8;
			cmd[4] = to;
		}
		else
		#endif
		{
			cmd[1] = to >> 16;
			cmd[2] = to >> 8;
			cmd[3] = to;
		}

		raspi_wait_ready(3, PrintEn);
		raspi_write_enable();
		raspi_unprotect(PrintEn);

		#ifdef ADDRESS_4B_MODE
		if (spi_chip_info->addr4b)
			rc = spic_write(cmd, 5, buf, page_size);
		else
		#endif
			rc = spic_write(cmd, 4, buf, page_size);

		//SPIPrintf(PrintEn,"\r\n[%s] SPI:read scur to:%x page_size:%x ret:%x",FmtTimeShow(), to, page_size, rc);
		//if ((retlen & 0xffff) == 0)
		//	SPIPrintf(PrintEn,"\r\n[%s] SPI:write len(%d)",FmtTimeShow(),retlen);

		if (rc > 0) 
		{
			retlen += rc;
			if (rc < page_size) 
			{
				SPIPrintf(PrintEn,"\r\n[%s] SPI:write rc(%x) page_size(%d)", \
					FmtTimeShow(), rc, page_size);
				raspi_write_disable();
				return retlen;
			}
		}

		len -= page_size;
		to += page_size;
		buf += page_size;
	}

	#ifdef ADDRESS_4B_MODE
	if (spi_chip_info->addr4b) 
	{
		raspi_4byte_mode(0, PrintEn);
	}
	#endif
	raspi_write_disable();
	
	return retlen;
}

#ifdef SUPPORT_ERASE_WRITE
int raspi_erase_write(u8 *buf, uint offs, int count, u8 PrintEn)
{
	int blocksize = spi_chip_info->sector_size;
	int blockmask = blocksize - 1;

	SPIPrintf(PrintEn,"\r\n[%s] SPI:weite offs(%d) Count(%d) Mask(%d)", \
		FmtTimeShow(), offs, count,blockmask);

	if (count > (spi_chip_info->sector_size * spi_chip_info->n_sectors)) 
	{
		SPIPrintf(PrintEn,"\r\n[%s] SPI:abort: image size larger than %d!", \
			FmtTimeShow(), (spi_chip_info->sector_size * spi_chip_info->n_sectors));
		return -1;
	}

	while (count > 0) 
	{
		if ((offs & blockmask) || (count < blocksize)) 
		{
			uint piece, blockaddr;
			int piece_size;
			u8 block[4096];
			#ifdef RALINK_SPI_UPGRADE_CHECK
			u8 temp[4096];
			#endif
		
			blockaddr = offs & ~blockmask;

			if (raspi_read(block, blockaddr, blocksize, PrintEn) != blocksize) 
			{
				return -2;
			}

			piece = offs & blockmask;
			piece_size = min(count, blocksize - piece);
			memcpy(block + piece, buf, piece_size);

			if (raspi_erase(ERASE_BLOCK_RULE, blockaddr, blocksize, PrintEn) != 0) 
			{
				return -3;
			}
			if (raspi_write(block, blockaddr, blocksize, PrintEn) != blocksize) 
			{
				return -4;
			}
			#ifdef RALINK_SPI_UPGRADE_CHECK
			if (raspi_read(temp, blockaddr, blocksize, PrintEn) != blocksize) 
			{
				return -2;
			}
			if(memcmp(block, temp, blocksize) == 0)
			{    
			   // SPIPrintf(NRCMD,"\r\n[%s] SPI:block write ok!",FmtTimeShow());
			}
			else
			{
				SPIPrintf(PrintEn,"\r\n[%s] SPI:block write incorrect!",FmtTimeShow());
				return -2;
			}
			#endif

			buf += piece_size;
			offs += piece_size;
			count -= piece_size;
		}
		else 
		{
			uint aligned_size = count & ~blockmask;
			#ifdef RALINK_SPI_UPGRADE_CHECK
			char temp[4096];
			int i;
			#endif

			if (raspi_erase(ERASE_BLOCK_RULE, offs, aligned_size, PrintEn) != 0)
			{
				return -1;
			}
			if (raspi_write(buf, offs, aligned_size, PrintEn) != aligned_size)
			{
				return -1;
			}

			#ifdef RALINK_SPI_UPGRADE_CHECK
			for( i=0; i< (aligned_size/blocksize); i++)
			{
				if (raspi_read(temp, offs+(i*blocksize), blocksize, PrintEn) != blocksize)
				{
					return -2;
				}
				if(memcmp(buf+(i*blocksize), temp, blocksize) == 0)
				{
				//	SPIPrintf(NRCMD,"\r\n[%s] SPI:blocksize write ok i=%d!",FmtTimeShow(), i);
				}
				else
				{
					SPIPrintf(PrintEn,"\r\n[%s] SPI:blocksize write incorrect block#=%d!",FmtTimeShow(),i);
					return -2;
				}
			}
			#endif
	
			buf += aligned_size;
			offs += aligned_size;
			count -= aligned_size;
		}
	}
	SPIPrintf(PrintEn,"\r\n[%s] SPI:done!",FmtTimeShow());
	return 0;
}
#endif

/*******************************************************************************
End Of The File
*******************************************************************************/

