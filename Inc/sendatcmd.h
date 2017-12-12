/*******************************************************************************
* File Name          : sedatcmd.h
* Author               : Jevon
* Description        : This file provides all the sedatcmd functions.

* History:
*  06/02/2015 : sedatcmd V1.00
*******************************************************************************/
#ifndef	__SENDATCMD__H__
#define	__SENDATCMD__H__
 /* Includes */
#include "stm32f0xx_hal.h"
#include "include.h"

typedef enum
{
	// LTE
	GSM_CMD_AT,
	GSM_CMD_Z,
	GSM_CMD_I0,
	GSM_CMD_I1,
	GSM_CMD_ATE0,
	GSM_CMD_ATE1,
	GSM_CMD_CMEE,
	GSM_CMD_IMEI,
	GSM_CMD_IMSI,
	GSM_CMD_CEREG,
	GSM_CMD_CCLK,
	GSM_CMD_CTZR,
	GSM_CMD_CRSM,
	GSM_CMD_CSQ,
	GSM_CMD_CESQ,
	GSM_CMD_CPIN,
	GSM_CMD_CGACT,
	GSM_CMD_IMSSTATEGET,
	GSM_CMD_SQNSI,
	GSM_CMD_SQNSS,
	GSM_CMD_SQNSCFG,
	GSM_CMD_SQNSCFGEXT,
	GSM_CMD_SQNSDIAL,
	GSM_CMD_SQNSLUDP,
	GSM_CMD_SQNSSEND,
	GSM_CMD_SQNHEXDAT,
	GSM_CMD_SQNSRECV,
	GSM_CMD_SQNSH,
	GSM_CMD_CFUN,
	GSM_CMD_CMGF,
	GSM_CMD_CNMI,
	GSM_CMD_CNUM,
	GSM_CMD_CMGS,
	GSM_CMD_CMGR,
	GSM_CMD_CMGD,
	GSM_CMD_SQNHTTPCFG,
	GSM_CMD_SQNHTTPQRY,
	GSM_CMD_SQNHTTPRCV,	
	// CBP82
	GSM_CMD_GPSPDE,
	GSM_CMD_TFTPSTART,
	GSM_CMD_TFTPSTOP,
	GSM_CMD_VLOCINFO,
	GSM_CMD_BARCODE,
        
	GSM_CMD_LAST
}GSM_ATCMDINDEX;

typedef enum
{
	GSM_CMD_TYPE_SPECIAL,		// UDP HEX
	GSM_CMD_TYPE_EXECUTE,		// ""
	GSM_CMD_TYPE_EVALUATE,	// "="
	GSM_CMD_TYPE_QUERY,		// "?"
	GSM_CMD_TYPE_QUERYALL,	// "=?"
	GSM_CMD_TYPE_COMMA,		// ","
	GSM_CMD_TYPE_SPEQUAL,		// "="

	GSM_CMD_TYPE_INVALID
}ARCA_GSM_COMAND_TYPE;

typedef enum
{
	// LTE
	GSM_FB_SYSSTART,
	GSM_FB_CEREG,
	GSM_FB_CSQ,
	GSM_FB_CESQ,
	GSM_FB_OK,
	GSM_FB_ERROR,
	GSM_FB_CIMI,
	GSM_FB_CNUM,
	GSM_FB_CGSN,
	GSM_FB_CCLK,
	GSM_FB_CTZR,	
	GSM_FB_COPN,
	GSM_FB_CRSM,		
	GSM_FB_CMGS,		
	GSM_FB_CMTI,
	GSM_FB_CMGR,		
	GSM_FB_CPIN,
	GSM_FB_SQNSI,
	GSM_FB_SQNSS,
	GSM_FB_SQNSRECV,
	GSM_FB_SQNSRING,
	GSM_FB_SQNHTTPRING,
	GSM_FB_CMSERROR,
	GSM_FB_CMEERROR,
	GSM_FB_SQNVERS1,
	GSM_FB_SQNVERS2,
	// CBP82
	GSM_FB_HCMGSS,
	GSM_FB_PPPCONIP,
	GSM_FB_SHORTSMS,
	GSM_FB_TFTPSTATUS,
	GSM_FB_DNLD,
	GSM_FB_VLOCINFO,
	GSM_FB_BARCODE,

	GSM_FB_UNDEFINITION_FB,
	GSM_FB_LAST_GSM_FB
}GSM_FB_CMD;

typedef struct
{
	int	nIndex;
	const char*	strCmdString;
}ARCA_ATCMDStruct;

// Variable declared
extern GSM_ATCMDINDEX cmdIndex;

// Function declaration 
extern void aiDFdhInit(void);
extern u8 GetFwpIsTimerStatus(void);
extern void SetFwpIsTimerStatus(u8 Status);
extern u8 GetFwpCurrentIndex(void);
extern void SetFwpCurrentIndex(u8 index);
extern void DeleteQueueElement(void);
extern void ATCommandSending(void);
extern void AtSendtoUart3TimerCallback(u8 Status);
extern GSM_ATCMDINDEX GetATIndex(char* fwp_cmd_buffer);
extern void SendATCmd(GSM_ATCMDINDEX command, ARCA_GSM_COMAND_TYPE cmd_type,uint8* ext_buf);

extern const ARCA_ATCMDStruct gsm_at_cmd[GSM_CMD_LAST];
extern const ARCA_ATCMDStruct gsm_at_cmd_type[GSM_CMD_TYPE_INVALID];
#endif

/******************************************************************************
* End of File
******************************************************************************/
