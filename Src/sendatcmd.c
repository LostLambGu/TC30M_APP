/*******************************************************************************
* File Name          : sedatcmd.c
* Author               : Jevon
* Description        : This file provides all the sedatcmd functions.

* History:
*  06/02/2015 : sedatcmd V1.00
*******************************************************************************/
#include "stm32f0xx_hal.h"
#include "include.h"
#include "sendatcmd.h"
#include "network.h"
#include "ltecatm.h"

#include "initialization.h"

#undef NRCMD
#define NRCMD (1)

#define SendatPrintf DebugPrintf

#ifdef MODEM_DEEPSLEEP_MODE
#define MODEM_AT_DPSLEEP ModemEnterSleep
#define MODEM_AT_WAKEUP ModemWakeUp
#endif /* MODEM_DEEPSLEEP_MODE */

extern DebugCtlPrarm DbgCtl;

 typedef struct
{
	GSM_ATCMDINDEX CmdIndex;
	UINT16 len;
	UINT8 buf[FDH_MAX_CMD_LEN];
}DFdhCmdBuf;

GSM_ATCMDINDEX cmdIndex = GSM_CMD_LAST;
DFdhCmdBuf FwpCmdBufQ[FDH_MAX_CMD_NUM];
static IQDesc FwpCmdQueue;
static u8 SendTimerStart = FALSE;
static u8 FwpCurrentIndex;

const ARCA_ATCMDStruct gsm_at_cmd[GSM_CMD_LAST] = 
{
	// LTE
	{GSM_CMD_AT,			""},				// general at for test
	{GSM_CMD_Z,				"Z"},            		// restore the configuration
	{GSM_CMD_I0,			"I0"},			// ATI or ATI0 -- MT manufacturer, MT model and the UE software version.
	{GSM_CMD_I1,			"I1"},			// ATI1 -- the UE and LR software versions.
	{GSM_CMD_ATE0,        		"E0"},            		// Close Echo character
	{GSM_CMD_ATE1,        		"E1"},            		// Open Echo character
	{GSM_CMD_CMEE,        		"+CMEE"},            	// Report Mobile Termination Error
	{GSM_CMD_IMEI,			"+CGSN"},		// get IMEI
	{GSM_CMD_IMSI,			"+CIMI"},			// get IMSI
	{GSM_CMD_CEREG,			"+CEREG"},		// get network register state
	{GSM_CMD_CCLK,			"+CCLK"},		// Get Date: +CCLK:2015/7/13,16:49:58+32
	{GSM_CMD_CTZR,			"+CTZR"},		// Time Zone Reporting
	{GSM_CMD_CGDCONT,       "+CGDCONT"},     // The set command specifies PDP context parameter values for a PDP context
	{GSM_CMD_CGCONTRDP,     "+CGCONTRDP"},  // The execution command returns the relevant information for an active non secondary PDP context with the context identifier <cid>
	{GSM_CMD_CRSM,			"+CRSM"},		// get ICCID
	{GSM_CMD_CSQ,			"+CSQ"},			// Query Received Signal Quality
	{GSM_CMD_CESQ,			"+CESQ"},		// Extended Signal Quality
	{GSM_CMD_CPIN,			"+CPIN"},			// Pin Code
	{GSM_CMD_CGACT,			"+CGACT"},		// PDP Context Activate or Deactivate
	{GSM_CMD_IMSSTATEGET,	"+IMSSTATEGET"},	// Get IMS state information
	{GSM_CMS_SQNMONI, "+SQNMONI"},
	{GSM_CMD_SQNSI,			"+SQNSI"},		// Socket Information
	{GSM_CMD_SQNSS,			"+SQNSS"},		// Socket Status
	{GSM_CMD_SQNSCFG,		"+SQNSCFG"},		// Socket Configuration:
	{GSM_CMD_SQNSCFGEXT,	"+SQNSCFGEXT"},	// Socket Configuration Extended
	{GSM_CMD_SQNSDIAL,		"+SQNSD"},		// UDP Socket Create(Socket Dial)
	{GSM_CMD_SQNSLUDP,		"+SQNSLUDP"},	// UDP Listen
	{GSM_CMD_SQNSSEND,     	"+SQNSSEND"},	// Send Socket Data In Command Mode
	{GSM_CMD_SQNSSENDEXT,	"+SQNSSENDEXT"},// Send Socket Data with length
	{GSM_CMD_SQNHEXDAT,  	""},				// Send Socket Data
	{GSM_CMD_SQNSRECV,		"+SQNSRECV"},	// Receive Socket Data in Command Mode
	{GSM_CMD_SQNSH,			"+SQNSH"},		// Socket Shutdown
	{GSM_CMD_CFUN,			"+CFUN"},		// configure the full functionality level 
	{GSM_CMD_CMGF,			"+CMGF"},            	// message format
	{GSM_CMD_CNMI,			"+CNMI"},            	// New Message Indications to TE
	{GSM_CMD_CNUM,			"+CNUM"},            	// Subscriber Number(Local SMS number)
	// if text mode (+CMGF=1) and sending successful: +CMGS: <mr>[,<scts>]
	// If sending fails:+CMS ERROR:<err>
	{GSM_CMD_CMGS,			"+CMGS"},            	// Send Text Message
	{GSM_CMD_CMGR,			"+CMGR"},            	// read sms
	{GSM_CMD_CMGD,			"+CMGD"},            	// delete sms
	// HTTP Configure
	{GSM_CMD_SQNHTTPCFG,	"+SQNHTTPCFG"}, 	// sets the parameters needed to the HTTP connection
	{GSM_CMD_SQNHTTPQRY,	"+SQNHTTPQRY"}, 	// performs HTTP GET, HEAD or DELETE request to server
	{GSM_CMD_SQNHTTPRCV,	"+SQNHTTPRCV"}, 	// read the body of HTTP response
	{GSM_CMD_SQNSUPGRADE,	"+SQNSUPGRADE"}, 	//  device upgrade with a firmware located either in the device filesystem or fetched from anexternal server
	// CBP82
	{GSM_CMD_GPSPDE,     		"+GPSPDE"},		// Sent PDE Addr to Modem
	{GSM_CMD_TFTPSTART,     	"+TFTPSTART"},	// Start TFTP Process
	{GSM_CMD_TFTPSTOP,     	"+TFTPSTOP"},		// Stop TFTP Process
	{GSM_CMD_VLOCINFO,     	"+VLOCINFO"},		// Get base station information
	{GSM_CMD_BARCODE,     	"+BARCODE"},		// Get BARCODE information
};

const ARCA_ATCMDStruct gsm_at_cmd_type[GSM_CMD_TYPE_INVALID]=
{
	{GSM_CMD_TYPE_SPECIAL, 		""},
	{GSM_CMD_TYPE_EXECUTE, 		""},
	{GSM_CMD_TYPE_EVALUATE,		"="},
	{GSM_CMD_TYPE_QUERY,		"?"},
	{GSM_CMD_TYPE_QUERYALL,		"=?"},
	{GSM_CMD_TYPE_COMMA,		","},
	{GSM_CMD_TYPE_SPEQUAL,		"="},
};

/*========================================
              Function Delcare
   =======================================*/
static void FwpPutData(DFdhCmdBuf* bufp, GSM_ATCMDINDEX CmdIndex, u8 *datap, u16 slen);
static void FwpEnQueueMsg(DFdhCmdBuf *buf);
static void FwpSendDataInque(DFdhCmdBuf* CmdPtr);
static void FwpSendAtTimerOn(void);
static const char* GetATCmd(GSM_ATCMDINDEX index);
static const char* GetAtCommandType(ARCA_GSM_COMAND_TYPE type);


void aiDFdhInit(void)
{
	iqInit(&FwpCmdQueue, (void*)FwpCmdBufQ, FDH_MAX_CMD_NUM, sizeof(DFdhCmdBuf));
}

u8 GetFwpIsTimerStatus(void)
{
	return SendTimerStart;
}

void SetFwpIsTimerStatus(u8 Status)
{
	// SendatPrintf(DbgCtl.SendToModemEn,"\r\n[%s] SAT: SetFwpIsTimerStatus %d", FmtTimeShow(), Status);
	SendTimerStart = Status;
}

u8 GetFwpCurrentIndex(void)
{
	return FwpCurrentIndex;
}

void SetFwpCurrentIndex(u8 index)
{
	FwpCurrentIndex = index;
}

static void FwpPutData(DFdhCmdBuf* bufp, GSM_ATCMDINDEX CmdIndex, u8 *datap, u16 slen)
{
	memset((void*)bufp, 0, sizeof(DFdhCmdBuf));
	bufp->CmdIndex = CmdIndex;
	if(slen > FDH_MAX_CMD_LEN)
	{
		//#ifdef DEBUG_SENT_AT
		//if(gDeviceConfig.DbgCtl.SendToModemEn == TRUE)
			SendatPrintf(DbgCtl.SendToModemEn,"\r\n[%s] SAT: Put in Que overflow(%d > %d) idx(%d)", \
				FmtTimeShow(),slen,FDH_MAX_CMD_LEN,CmdIndex);
		//#endif
		bufp->len = FDH_MAX_CMD_LEN;
		memcpy((void*)bufp->buf, (void*)datap, FDH_MAX_CMD_LEN);
	}
	else
	{
		bufp->len = slen;
		memcpy((void*)bufp->buf, (void*)datap, strlen((char*)datap));
	}
}

static void FwpEnQueueMsg(DFdhCmdBuf *buf)
{
	iqPut(&FwpCmdQueue, (void *) buf);
}

static void FwpSendDataInque(DFdhCmdBuf* CmdPtr)
{
	// #ifdef DEBUG_SENT_AT
	// if(gDeviceConfig.DbgCtl.SendToModemEn == TRUE)
		SendatPrintf(DbgCtl.SendToModemEn,"\r\n[%s] SAT: Send Que D(%d)", \
			FmtTimeShow(),iqNum(&FwpCmdQueue));
	// #endif
	// Check buffer
	if(CmdPtr->buf[0] == 'A' && CmdPtr->buf[1] == 'T')
	{
		cmdIndex = GetATIndex((char *)CmdPtr->buf);
	}
	else
	{
	 	//#ifdef DEBUG_SENT_AT
		//if(gDeviceConfig.DbgCtl.SendToModemEn == TRUE)
			SendatPrintf(DbgCtl.SendToModemEn,"\r\n[%s] SAT: Special Cmd(%c%c%c%c%c%c)", \
				FmtTimeShow(), \
				CmdPtr->buf[0], \
				CmdPtr->buf[1], \
				CmdPtr->buf[2], \
				CmdPtr->buf[3], \
				CmdPtr->buf[4], \
				CmdPtr->buf[5]);
		//#endif
	}
	SendDatatoIop((char*)CmdPtr->buf,CmdPtr->len);
	// #ifdef DEBUG_SENT_AT
	// if(gDeviceConfig.DbgCtl.SendToModemEn == TRUE)
	// {
		SendatPrintf(DbgCtl.SendToModemEn,"\r\n[%s] SAT: Tidx(%d) Len(%d)(%s)", \
			FmtTimeShow(),cmdIndex,CmdPtr->len,(char *)CmdPtr->buf);
	// }
	// #endif
}

void DeleteQueueElement(void)
{
	iqDelete(&FwpCmdQueue);
}

void ATCommandSending(void)
{
 	// #ifdef DEBUG_SENT_AT
	// if(gDeviceConfig.DbgCtl.SendToModemEn == TRUE)
		SendatPrintf(DbgCtl.SendToModemEn,"\r\n[%s] SAT: CmdSending(%d|%d %d %d)", \
			FmtTimeShow(), \
			iqNum(&FwpCmdQueue), \
			GetModemPoweronStat(), \
			GetModemATPortStat(), \
			LTE_AT_PORT_READY());
	// #endif
	// Start timer
	FwpSendAtTimerOn();
	// Queue Status
	if ((iqNum(&FwpCmdQueue) > 0) && \
		(GetModemPoweronStat() == TRUE) && \
		(GetModemATPortStat() == TRUE) && \
		(LTE_AT_PORT_READY() == TRUE))
	{
		DFdhCmdBuf CmdQueDataBuf;
		memset((void*)&CmdQueDataBuf, 0, sizeof(CmdQueDataBuf));
		if (iqGet(&FwpCmdQueue, &CmdQueDataBuf) == TRUE)		
		{
			// Set index
			SetFwpCurrentIndex(CmdQueDataBuf.CmdIndex);
			// Check index
			if(CmdQueDataBuf.CmdIndex == GSM_CMD_SQNHEXDAT)
			{
				if(GetUDPDataCanSendStat() == TRUE)
				{
					SetUDPDataCanSendStat(FALSE);
  					FwpSendDataInque(&CmdQueDataBuf);
					iqDelete(&FwpCmdQueue);
				}
			}
			else
			{
				FwpSendDataInque(&CmdQueDataBuf);
				iqDelete(&FwpCmdQueue);
			}
		}
		else
		{
			//#ifdef DEBUG_SENT_AT
			//if(gDeviceConfig.DbgCtl.SendToModemEn == TRUE)
			SendatPrintf(DbgCtl.SendToModemEn,"\r\n[%s] SAT: Queue error...", FmtTimeShow());
			//#endif
			return;
		}
 	}
}

extern uint8_t resetFSM;
//DualFwpCmdSendTimerProcess
void AtSendtoUart3TimerCallback(u8 Status)
{
	SendatPrintf(NRCMD,"\r\n[%s] SAT:timerout", FmtTimeShow());
	if((GetUDPDataCanSendStat() == FALSE) /*&& (GetFwpCurrentIndex() == GSM_CMD_CMGS || GetFwpCurrentIndex() == GSM_CMD_SQNSSEND)*/)
	{
		// #ifdef ATRSP_RESULT
		//if(gDeviceConfig.DbgCtl.SendToModemEn == TRUE)
			SendatPrintf(NRCMD,"\r\n[%s] SAT:timerout delete queue element %d", \
				FmtTimeShow(),GetFwpCurrentIndex());
		// #endif
		// Delete queue element
		DeleteQueueElement();
	}
	// Check queue
	if (iqNum(&FwpCmdQueue) > 0)
	{
		// Cmd sending
		// ATCommandSending();
		resetFSM = TRUE;
		SendatPrintf(NRCMD,"\r\n[%s] SAT: atcmd timer callback set resetFSM TRUE", FmtTimeShow());
	}
	// Reset timer
	FwpSendAtTimerOn();
}

static void FwpSendAtTimerOn(void)
{
	SoftwareTimerStop(&AtSentToUart3Timer);
	// Check at port
	if(GetModemPoweronStat() == TRUE && \
		GetModemATPortStat() == FALSE)
	{
		// Wait Modem AT Port Ready
		// Get the current system time to test AT port
		//SendATCmdToModemDirectly(CheckATPortStatSting, FALSE);
	}
	// Check queue
	if (iqNum(&FwpCmdQueue) > 0)
	{
		// Set flag
		SetFwpIsTimerStatus(TRUE);
		// Reset timer
		SoftwareTimerReset(&AtSentToUart3Timer,AtSendtoUart3TimerCallback,AT_SENDTO_RETRY_TIMEOUT);
		SoftwareTimerStart(&AtSentToUart3Timer);
		#ifdef DEBUG_SENT_AT
		//if(gDeviceConfig.DbgCtl.SendToModemEn == TRUE)
			SendatPrintf(DbgCtl.SendToModemEn,"\r\n[%s] SAT: atcmd timer reset...", FmtTimeShow());
		#endif
 	}
	else
	{
		// Set flag
		SetFwpIsTimerStatus(FALSE);
		#ifdef MODEM_DEEPSLEEP_MODE
		// Modem Dpsleep
		MODEM_AT_DPSLEEP();
		#endif /* MODEM_DEEPSLEEP_MODE */

		SendatPrintf(NRCMD,"\r\n[%s] SAT: atcmd timer stop...", FmtTimeShow());
	}
}

static void FwpSendData(GSM_ATCMDINDEX CmdIndex, char* cmdsring, u16 slen)
{
	DFdhCmdBuf buf; 
	memset((void*)&buf, 0, sizeof(DFdhCmdBuf));
	// #ifdef DEBUG_SENT_AT
	// if(gDeviceConfig.DbgCtl.SendToModemEn == TRUE)
		// SendatPrintf(NRCMD,"\r\n[%s] SAT: F(%d %d %d) Q(%d %d) L(%d %d)", \
		// 	FmtTimeShow(), \
		// 	CmdIndex,
		// 	GetFwpIsTimerStatus(), \
		// 	GetModemATPortStat(), \
		// 	iqNum(&FwpCmdQueue), \
		// 	FDH_MAX_CMD_NUM, \
		// 	strlen((char*)cmdsring), \
		// 	FDH_MAX_CMD_LEN);
	// #endif
	#ifdef MODEM_DEEPSLEEP_MODE
	// Modem Wakeup
	MODEM_AT_WAKEUP();
	#endif
	// Check Queue
	if(iqNum(&FwpCmdQueue) == FDH_MAX_CMD_NUM )
	{
		if(iqDelete(&FwpCmdQueue) == FALSE || iqNum(&FwpCmdQueue) == FDH_MAX_CMD_NUM )
		{
			iqInit(&FwpCmdQueue, (void*)FwpCmdBufQ, FDH_MAX_CMD_NUM, sizeof(DFdhCmdBuf));
			//#ifdef DEBUG_SENT_AT
			//if(gDeviceConfig.DbgCtl.SendToModemEn == TRUE)
			SendatPrintf(DbgCtl.SendToModemEn,"\r\n[%s] SAT: Que Overflow, Reinit", FmtTimeShow());
			//#endif
		}
		else
		{
			//#ifdef DEBUG_SENT_AT
			//if(gDeviceConfig.DbgCtl.SendToModemEn == TRUE)
			SendatPrintf(DbgCtl.SendToModemEn,"\r\n[%s] SAT: Del First Record Successfull", FmtTimeShow());
			//#endif
		}
	}
	// Put AT Data In Queue
	// #ifdef DEBUG_SENT_AT
	// if(gDeviceConfig.DbgCtl.SendToModemEn == TRUE)
		SendatPrintf(NRCMD,"\r\n[%s: %s] SAT: Put AT Data in Que", FmtTimeShow(),cmdsring);
	// #endif
	// Go into queue
	FwpPutData(&buf, CmdIndex, (u8*)cmdsring, slen);
	FwpEnQueueMsg(&buf);
	// Check status
	if(GetFwpIsTimerStatus() == FALSE)
	{
		// SendatPrintf(NRCMD,"\r\n[%s] SAT: Fwp Timer Status == FALSE", FmtTimeShow());
		// Cmd sending
		ATCommandSending();
	}
}

GSM_ATCMDINDEX GetATIndex(char* cmdsring)
{
	uint16 i = 0;
	
	for(;i<GSM_CMD_LAST;i++)
	{
		if(strlen(gsm_at_cmd[i].strCmdString) == 0)
		{
			continue;
		}
	       if((strncmp((char *)(cmdsring),"AT+CPIN=",8)==0) //not return GSM_CMD_CPIN when validate pin/puk error. return GSM_CMD_CPIN only when send AT+CPIN?
		   ||(strncmp((char *)(cmdsring),"AT+MNAM?",8)==0)) //not return GSM_CMD_MNAM when Query error. return GSM_CMD_MNAM only when send AT+MNAM=
		{
			return GSM_CMD_LAST;
		}
	       else if(strncmp((char *)(cmdsring+2),gsm_at_cmd[i].strCmdString,strlen(gsm_at_cmd[i].strCmdString)) == 0)
		{
			return (GSM_ATCMDINDEX)gsm_at_cmd[i].nIndex;
		}
	}	
	return GSM_CMD_LAST;
}

static const char* GetATCmd(GSM_ATCMDINDEX index)
{
	uint16 i = 0;
	
	for(;i<GSM_CMD_LAST;i++)
	{
		if(gsm_at_cmd[i].nIndex == index)
		{
			//if(index != GSM_CMD_MIOC)
			return gsm_at_cmd[i].strCmdString;
		}
	}
	return NULL;
}

static const char* GetAtCommandType(ARCA_GSM_COMAND_TYPE type)
{
	uint8 i = 0;
	
	for(;i<GSM_CMD_TYPE_INVALID;i++)
	{
		if(gsm_at_cmd_type[i].nIndex == type)
		{
			return gsm_at_cmd_type[i].strCmdString;
		}
	}
	return NULL;
}

void SendATCmd(GSM_ATCMDINDEX CmdIndex, ARCA_GSM_COMAND_TYPE cmd_type,uint8* ext_buf)
{
	char    atstring[FDH_MAX_CMD_LEN];
	// Set Sleep Flag
	// SetSystemEventMode(SYS_EVENT_ATCOMMAND_TO_MODEM_MSK);
	// Init
	memset(atstring, 0, FDH_MAX_CMD_LEN);
	switch((uint8)cmd_type)
	{
		case GSM_CMD_TYPE_SPECIAL:
		if(ext_buf)
		{
			if(CmdIndex == GSM_CMD_SQNHEXDAT)
			{
				sprintf(atstring, "%s",(char*)ext_buf);
				atstring[strlen(atstring)] = 0x1A;
				atstring[strlen(atstring)+1] = '\0';
			}
			else
			{
				//sprintf(atstring, "AT%s%s%s\r",GetATCmd(CmdIndex),GetAtCommandType(cmd_type),(char*)ext_buf);
				sprintf(atstring, "%s%s%s",GetATCmd(CmdIndex),GetAtCommandType(cmd_type),(char*)ext_buf);
			}
			FwpSendData(CmdIndex, atstring, strlen(atstring));
		}
		break;
		
		case GSM_CMD_TYPE_EXECUTE:
		if(ext_buf)
			sprintf(atstring, "AT%s%s%s\r",GetATCmd(CmdIndex),GetAtCommandType(cmd_type),(char*)ext_buf);
		else
			sprintf(atstring, "AT%s%s\r",GetATCmd(CmdIndex),GetAtCommandType(cmd_type));
		FwpSendData(CmdIndex, atstring, strlen(atstring));
		break;	
		
		case GSM_CMD_TYPE_QUERY:
		case GSM_CMD_TYPE_QUERYALL:
		sprintf(atstring, "AT%s%s\r",GetATCmd(CmdIndex),GetAtCommandType(cmd_type));
		FwpSendData(CmdIndex, atstring, strlen(atstring));
		break;	
		
		case GSM_CMD_TYPE_EVALUATE:
		case GSM_CMD_TYPE_COMMA:
		if(ext_buf)
		{
			if(CmdIndex == GSM_CMD_CMGS)
				sprintf(atstring, "AT%s%s\"%s\"\r",GetATCmd(CmdIndex),GetAtCommandType(cmd_type),(char*)ext_buf);
			else if((CmdIndex == GSM_CMD_SQNSSEND) || (CmdIndex == GSM_CMD_SQNSSENDEXT))
				sprintf(atstring, "AT%s%s%s\r",GetATCmd(CmdIndex),GetAtCommandType(cmd_type),(char*)ext_buf);
			else
				sprintf(atstring, "AT%s%s%s\r",GetATCmd(CmdIndex),GetAtCommandType(cmd_type),(char*)ext_buf);
			FwpSendData(CmdIndex, atstring, strlen(atstring));
		}
		break;

		case GSM_CMD_TYPE_SPEQUAL:
		if(ext_buf)
		{
			sprintf(atstring, "AT%s%s\"%s\"\r",GetATCmd(CmdIndex),GetAtCommandType(cmd_type),(char*)ext_buf);
			FwpSendData(CmdIndex, atstring, strlen(atstring));
		}
		break;
		
		default:
		// #ifdef DEBUG_SENT_AT
		// if(gDeviceConfig.DbgCtl.SendToModemEn == TRUE)
			SendatPrintf(DbgCtl.SendToModemEn,"\r\n[%s] SAT: Out Of Range", FmtTimeShow());
		// #endif
		break;
	}
}

/******************************************************************************
* End of File
******************************************************************************/
