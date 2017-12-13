/*******************************************************************************
* File Name          : spi_driver.h
* Author               : Jevon
* Description        : This file provides all the spi_driver functions.

* History:
*  05/15/2015 : spi_driver V1.00
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _SPI_SERIAL_FLASH_DRV_H
#define _SPI_SERIAL_FLASH_DRV_H
/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
#include "include.h"
#define SPIPrintf DebugPrintf

// PA4- SPI FLASH CS
#define SPI_FLASH_CS_PORT  		PA4_FLASH_CS_N_GPIO_Port
#define SPI_FLASH_CS_PIN  	 	PA4_FLASH_CS_N_Pin

// PA5- SPI FLASH SCK
#define SPI_FLASH_SCK_PORT  	PA5_FLASH_SCK_GPIO_Port
#define SPI_FLASH_SCK_PIN  	 	PA5_FLASH_SCK_Pin

// PA6- SPI FLASH MISO
#define SPI_FLASH_MISO_PORT  	PA6_FLASH_MISO_GPIO_Port
#define SPI_FLASH_MISO_PIN  	PA6_FLASH_MISO_Pin

// PA7- SPI FLASH MOSI
#define SPI_FLASH_MOSI_PORT  	PA7_FLASH_MOSI_GPIO_Port
#define SPI_FLASH_MOSI_PIN  	PA7_FLASH_MOSI_Pin

// PA8- SPI FLASH WP
#define SPI_FLASH_WP_PORT  		PA8_FLASH_WP_N_GPIO_Port
#define SPI_FLASH_WP_PIN  	 	PA8_FLASH_WP_N_Pin

// CS 	- PA4
// SCK 	- PA5
// MISO - PA6
// MOSI - PA7
// WPEN - PA8
#define SPI_CS_LOW()			HAL_GPIO_WritePin(SPI_FLASH_CS_PORT, SPI_FLASH_CS_PIN, GPIO_PIN_RESET)
#define SPI_CS_HIGH()			HAL_GPIO_WritePin(SPI_FLASH_CS_PORT, SPI_FLASH_CS_PIN, GPIO_PIN_SET)
#define SPI_SCK_LOW()		HAL_GPIO_WritePin(SPI_FLASH_SCK_PORT, SPI_FLASH_SCK_PIN, GPIO_PIN_RESET)
#define SPI_SCK_HIGH()		HAL_GPIO_WritePin(SPI_FLASH_SCK_PORT, SPI_FLASH_SCK_PIN, GPIO_PIN_SET)
#define SPI_MOSI_LOW()		HAL_GPIO_WritePin(SPI_FLASH_MOSI_PORT, SPI_FLASH_MOSI_PIN, GPIO_PIN_RESET)
#define SPI_MOSI_HIGH()		HAL_GPIO_WritePin(SPI_FLASH_MOSI_PORT, SPI_FLASH_MOSI_PIN, GPIO_PIN_SET)
#define SPI_WPEN_LOW()		HAL_GPIO_WritePin(SPI_FLASH_WP_PORT, SPI_FLASH_WP_PIN, GPIO_PIN_RESET)
#define SPI_WPEN_HIGH()		HAL_GPIO_WritePin(SPI_FLASH_WP_PORT, SPI_FLASH_WP_PIN, GPIO_PIN_SET)
// Read
#define SPI_MISO_READ()		HAL_GPIO_ReadPin(SPI_FLASH_MISO_PORT,SPI_FLASH_MISO_PIN)
// GPIO CFG
#define SPI_SET_CS_OUT()		  SPIGPIO.Pin = SPI_FLASH_CS_PIN;	\
							  SPIGPIO.Mode = GPIO_MODE_OUTPUT_PP;	\
							  SPIGPIO.Pull = GPIO_NOPULL;	\
							  SPIGPIO.Speed = GPIO_SPEED_FREQ_HIGH;	\
							  HAL_GPIO_Init(SPI_FLASH_CS_PORT, &SPIGPIO)

#define SPI_SET_SCK_OUT()		  SPIGPIO.Pin = SPI_FLASH_SCK_PIN;	\
							  SPIGPIO.Mode = GPIO_MODE_OUTPUT_PP;	\
							  SPIGPIO.Pull = GPIO_NOPULL;	\
							  SPIGPIO.Speed = GPIO_SPEED_FREQ_HIGH;	\
							  HAL_GPIO_Init(SPI_FLASH_SCK_PORT, &SPIGPIO)

#define SPI_SET_MISO_IN()		  SPIGPIO.Pin = SPI_FLASH_MISO_PIN;	\
							  SPIGPIO.Mode = GPIO_MODE_INPUT;	\
							  SPIGPIO.Pull = GPIO_NOPULL;	\
							  SPIGPIO.Speed = GPIO_SPEED_FREQ_HIGH;	\
							  HAL_GPIO_Init(SPI_FLASH_MISO_PORT, &SPIGPIO)
							  
#define SPI_SET_MOSI_OUT()	  SPIGPIO.Pin = SPI_FLASH_MOSI_PIN;	\
							  SPIGPIO.Mode = GPIO_MODE_OUTPUT_PP;	\
							  SPIGPIO.Pull = GPIO_NOPULL;	\
							  SPIGPIO.Speed = GPIO_SPEED_FREQ_HIGH;	\
							  HAL_GPIO_Init(SPI_FLASH_MOSI_PORT, &SPIGPIO)

#define SPI_SET_WPEN_OUT()	  SPIGPIO.Pin = SPI_FLASH_WP_PIN;	\
							  SPIGPIO.Mode = GPIO_MODE_OUTPUT_PP;	\
							  SPIGPIO.Pull = GPIO_NOPULL;	\
							  SPIGPIO.Speed = GPIO_SPEED_FREQ_HIGH;	\
							  HAL_GPIO_Init(SPI_FLASH_WP_PORT, &SPIGPIO)
							  
#define SPI_FIFO_SIZE 			16
//#define READ_BY_PAGE

#define SPIC_READ_BYTES 		(1<<0)
#define SPIC_WRITE_BYTES 		(1<<1)
#define SPIC_USER_MODE 		(1<<2)
#define SPIC_4B_ADDR 			(1<<3)

#if defined(RD_MODE_QOR) || defined(RD_MODE_QIOR)
#define RD_MODE_QUAD
#endif

#define    FlashID 			0xc22012
#define    ElectronicID 		0x11
#define    RESID0 			0xc211
#define    RESID1 			0x11c2
#define    FlashSize 			0x40000        // 256 KB
#define    CE_period 		2083334        // tCE /  ( CLK_PERIOD * Min_Cycle_Per_Inst *One_Loop_Inst)
#define    tW 				100000000      // N*15ms, N is a multiple of 10K cycle
#define    tDP 				10000          // 10us
#define    tPP 				5000000        // 5ms
#define    tSE 				60000000       // 60ms
#define    tBE 				2000000000     // 2s
#define    tPUW 			10000000       // 10ms

/* SPI FLASH elementray definition and function */
#define FLASH_PAGESIZE		256

//#define NO_4B_ADDRESS_SUPPORT

#ifndef NO_4B_ADDRESS_SUPPORT
#define ADDRESS_4B_MODE
#endif

/* Flash opcodes. */
#define OPCODE_WREN				6	/* Write enable */
#define OPCODE_WRDI				4	/* Write disable */
#define OPCODE_RDSR				5	/* Read status register */
#define OPCODE_WRSR				1	/* Write status register */
#define OPCODE_READ				3	/* Read data bytes */
#define OPCODE_PP				2	/* Page program */
#define OPCODE_SE				0xD8	/* Sector erase (64K)*/
#define OPCODE_RES				0xAB	/* Read Electronic Signature */
#define OPCODE_RDID				0x9F	/* Read JEDEC ID */

#define OPCODE_FAST_READ		0x0B	/* Fast Read */
#define OPCODE_DOR				0x3B	/* Dual Output Read */
#define OPCODE_QOR				0x6B	/* Quad Output Read */
#define OPCODE_DIOR				0xBB	/* Dual IO High Performance Read */
#define OPCODE_QIOR				0xEB	/* Quad IO High Performance Read */
#define OPCODE_READ_ID			0x90	/* Read Manufacturer and Device ID */

#define OPCODE_P4E				0x20	/* 4KB Parameter Sectore Erase */
#define OPCODE_P8E				0x40	/* 8KB Parameter Sectore Erase */
#define OPCODE_BE				0x60	/* Bulk Erase */
#define OPCODE_BE1				0xC7	/* Bulk Erase */
#define OPCODE_QPP				0x32	/* Quad Page Programing */

#define OPCODE_CLSR				0x30
#define OPCODE_RCR				0x35	/* Read Configuration Register */

#define OPCODE_BRRD				0x16
#define OPCODE_BRWR				0x17

/* Status Register bits. */
#define SR_WIP					1	/* Write in progress */
#define SR_WEL					2	/* Write enable latch */
#define SR_BP0					4	/* Block protect 0 */
#define SR_BP1					8	/* Block protect 1 */
#define SR_BP2					0x10	/* Block protect 2 */
#define SR_EPE					0x20	/* Erase/Program error */
#define SR_SRWD					0x80	/* SR write protect */

typedef struct
{
	char		*name;
	u8		id;
	u32		jedec_id;
	u32		sector_size;
	uint		n_sectors;
	char		addr4b;
}chip_info;

// Variable Declared 
extern chip_info *spi_chip_info;

//Function Declare
extern int spic_init(u8 EnableDelay);
extern u32 raspi_init(u8 PrintEn);
extern int raspi_erase(u8 opcode, uint offs, u8 PrintEn);
extern int raspi_read(u8 *buf, uint from, int len, u8 PrintEn);
extern int raspi_write(u8 *buf, uint to, int len, u8 PrintEn);
#ifdef SUPPORT_ERASE_WRITE
extern int raspi_erase_write(u8 *buf, uint offs, int count, u8 PrintEn);
#endif
#endif  /* _SPI_SERIAL_FLASH_DRV_H */

/*******************************************************************************
End Of The File
*******************************************************************************/

