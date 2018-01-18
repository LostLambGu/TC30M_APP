/*******************************************************************************
* File Name          : ltecatm_arrow.c
* Author             : Yangjie Gu
* Description        : This file provides all the ltecatm_arrow functions.

* History:
*  11/10/2017 : ltecatm_arrow V1.00
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "ltecatm.h"
#include "parseatat.h"
#include "wedgecommonapi.h"

/* Private define ------------------------------------------------------------*/
#undef NRCMD
#define NRCMD (1)

#define CBPCmdPrintf DebugPrintf

/* Variables -----------------------------------------------------------------*/
extern DebugCtlPrarm DbgCtl;

extern uint8_t udpsendoverflag;
extern uint8_t smssendoverflag;

static u8 ModemNeedInitFlag = FALSE;
static u8 SimCardReadyFlag = FALSE;
static u8 UDPDataCanSendFlag = FALSE;
static u8 SMSDataCanSendFlag = FALSE;
static u8 SMSNeedResponseFlag = FALSE;
static u8 SMSNeedReleaseFlag = FALSE;
uint8_t ModemPowerOnFlag = FALSE;

extern void OemMsgQueUdpRec(void *MsgBufferP, uint32_t size);

/* Function definition -------------------------------------------------------*/
u8 GetModemPoweronStat(void)
{
	return ModemPowerOnFlag;
}

void SetModemNeedInitStat(u8 Status)
{
	ModemNeedInitFlag = Status;
}

u8 GetModemNeedInitStat(void)
{
	return ModemNeedInitFlag;
}

void SetSimCardReadyStat(u8 Status)
{
	SimCardReadyFlag = Status;
}

u8 GetSimCardReadyStat(void)
{
	return SimCardReadyFlag;
}

void SetUDPDataCanSendStat(u8 Status)
{
	UDPDataCanSendFlag = Status;
}

u8 GetUDPDataCanSendStat(void)
{
	return UDPDataCanSendFlag;
}

void SetSMSDataCanSendStat(u8 Status)
{
	SMSDataCanSendFlag = Status;
}

u8 GetSMSDataCanSendStat(void)
{
	return SMSDataCanSendFlag;
}

void SetSMSNeedResponseStat(u8 Status)
{
	SMSNeedResponseFlag = Status;
}

u8 GetSMSNeedResponseStat(void)
{
	return SMSNeedResponseFlag;
}

void SetSMSNeedReleaseStat(u8 Status)
{
	SMSNeedReleaseFlag = Status;
}

u8 GetSMSNeedReleaseStat(void)
{
	return SMSNeedReleaseFlag;
}

void LteCmdDetection(void)
{
	if(UART3_RX_NUM == TRUE)
	{
		uint8_t Uart3ParseBuffer[UART3_RX_BUFFER_SIZE] = {'\0'};
		u8 IsNeedSendNetDataOutFlag = FALSE;
		char *tmp = NULL;
		
		UART3_RX_NUM = FALSE;
		// Get Data
		SystemDisableAllInterrupt() ;// Disable Interrupt
		memcpy(Uart3ParseBuffer, Uart3RxBuffer, Uart3RxCount);
		AtBuffer.buf = (u8*)Uart3ParseBuffer;
		AtBuffer.len = Uart3RxCount;
		// Clear Buffer
		Uart3RxCount = 0;
		memset((char *)Uart3RxBuffer,0,sizeof(Uart3RxBuffer));
		SystemEnableAllInterrupt();	// Enable Interrupt
		// Print Out
		CBPCmdPrintf(NRCMD,"\r\n[%s] LTE: REV %d [%.80s]", \
			FmtTimeShow(), AtBuffer.len, AtBuffer.buf);

		if ((tmp = strstr((const char *)AtBuffer.buf,"+SQNSRECV:")) != NULL)
		{
			// uint8_t socketnum = 0;
			uint32_t socketlen = 0;
			uint8_t lenstr[4] = {0};
			uint8_t i = 0 , j = 0;

			tmp = strstr(tmp, ",");
			if (tmp == NULL)
			{
				CBPCmdPrintf(NRCMD,"LteCmdDetection +SQNSRECV: Err");
				AtBuffer.buf = "ERROR\r\n";
				AtBuffer.len = 7;
			}
			else
			{
				// socketnum = *(tmp - 1) - '0';

				for (i = 0; i < 4; i++)
				{
					if ((*(tmp + i + 1) >= '0') && (*(tmp + i + 1) <= '9'))
					{
						lenstr[i] = *(tmp + i + 1) - '0';
					}
					else
					{
						break;
					}
				}

				j = i;
				tmp += j;
				tmp++;
				if (*tmp == '\r')
				{
					tmp++;
				}

				if (*tmp == '\n')
				{
					tmp++;
				}

				for (i = 0; i < j; i++)
				{
					socketlen = socketlen * 10 + lenstr[i];
				}

				CBPCmdPrintf(DbgCtl.LteRecDbgInfoEn, "\r\n[%s] socket rec len: %d\r\n", FmtTimeShow(), socketlen);
				CBPCmdPrintf(DbgCtl.LteRecDbgInfoEn, "Pass to OemMsgQueUdpRec(max print 80) (%.80s)", tmp);
				UdpReceivedHandle(tmp, socketlen);

				AtBuffer.buf = "OK\r\n";
				AtBuffer.len = 4;
			}
			// memset((char *)Uart3ParseBuffer,0,sizeof(Uart3ParseBuffer));
		}

		//prevent can not parse
		if(strstr((const char *)AtBuffer.buf,"+IMSSTATE: SIMSTORE,READY\r\n"))
		{
			/*isSIMReady = TRUE;
			//get IMEI
			SendATCmd(GSM_CMD_IMSI, GSM_CMD_TYPE_EXECUTE , NULL);
			//get ICCID
			SendATCmd(GSM_CMD_CRSM,GSM_CMD_TYPE_EVALUATE,"176,12258,0,0,10");*/

			//start InitClockTimer
//			SoftwareTimerStart(&InitClockTimer);
			//start CheckRssiTimer
//			SoftwareTimerStart(&CheckRssiTimer);
			// Print Out
			CBPCmdPrintf(DbgCtl.LteRecDbgInfoEn,"\r\n[%s] LTE: +IMSSTATE: SIMSTORE,READY",FmtTimeShow());
		}

		// Parse Special AT Cmd
		if(AtBuffer.len > 4 &&\
			AtBuffer.buf[AtBuffer.len-4] == 0x0D && \
			AtBuffer.buf[AtBuffer.len-3] == 0x0A && \
			AtBuffer.buf[AtBuffer.len-2] == 0x3E && \
			AtBuffer.buf[AtBuffer.len-1] == 0x20)
		{
			// Clear Data
			AtBuffer.buf[AtBuffer.len-4] = '\0';
			AtBuffer.buf[AtBuffer.len-3] = '\0';
			AtBuffer.buf[AtBuffer.len-2] = '\0';
			AtBuffer.buf[AtBuffer.len-1] = '\0';
			// Reduce Length
			AtBuffer.len -= 4;
			CBPCmdPrintf(DbgCtl.LteRecDbgInfoEn,"\r\n[%s] UDP: > 1%d",FmtTimeShow(),AtBuffer.len);
			IsNeedSendNetDataOutFlag = TRUE;
		}
		else if(AtBuffer.len > 4 &&\
			AtBuffer.buf[0] == 0x0D && \
			AtBuffer.buf[1] == 0x0A && \
			AtBuffer.buf[2] == 0x3E && \
			AtBuffer.buf[3] == 0x20)
		{
			// Clear Data
			AtBuffer.buf +=4;
			// Reduce Length
			AtBuffer.len -= 4;
			CBPCmdPrintf(DbgCtl.LteRecDbgInfoEn,"\r\n[%s] UDP: > 2%d",FmtTimeShow(),AtBuffer.len);
			IsNeedSendNetDataOutFlag = TRUE;
		}
		else if(AtBuffer.len == 1 && \
			AtBuffer.buf[0] == 0x3E)
		{
			CBPCmdPrintf(DbgCtl.LteRecDbgInfoEn,"\r\n[%s] LTE: > 3",FmtTimeShow());
			IsNeedSendNetDataOutFlag = TRUE;
		}
		else if(AtBuffer.len == 2 && \
			AtBuffer.buf[0] == 0x0A && \
			AtBuffer.buf[1] == 0x3E)
		{
			CBPCmdPrintf(DbgCtl.LteRecDbgInfoEn,"\r\n[%s] LTE: > 4",FmtTimeShow());
			IsNeedSendNetDataOutFlag = TRUE;
		}
		else if(AtBuffer.len == 3 && \
			AtBuffer.buf[0] == 0x0D && \
			AtBuffer.buf[1] == 0x0A && \
			AtBuffer.buf[2] == 0x3E)
		{
			CBPCmdPrintf(DbgCtl.LteRecDbgInfoEn,"\r\n[%s] LTE: > 5",FmtTimeShow());
			IsNeedSendNetDataOutFlag = TRUE;
		}
		else if(AtBuffer.len == 4 && \
			AtBuffer.buf[0] == 0x0D && \
			AtBuffer.buf[1] == 0x0A && \
			AtBuffer.buf[2] == 0x3E && \
			AtBuffer.buf[3] == 0x20)
		{
			CBPCmdPrintf(DbgCtl.LteRecDbgInfoEn,"\r\n[%s] UDP: > 6",FmtTimeShow());
			IsNeedSendNetDataOutFlag = TRUE;
		}
		else if(AtBuffer.len > 0)
		{
			//CBPCmdPrintf(NRCMD,"\r\n[%s] LTE: ResetFSM(%d) AtBuffer.buf(80 max)(%.80s)",FmtTimeShow(),resetFSM, AtBuffer.buf);
			
			if(resetFSM)
			{
				CBPCmdPrintf(NRCMD,"\r\n[%s] LTE: ResetFSM(%d)",FmtTimeShow(),resetFSM);
				resetFSM = FALSE;
				parseAtDHInit();
			}
			atDHProcessEvent(ATDH_DPD_TxReq, &AtBuffer, (UINT8)sizeof(AtBuffer));
		}

		if(IsNeedSendNetDataOutFlag == TRUE/*&&  iqNum(&FwpCmdQueue) > 0*/)
		{
			if (0 == udpsendoverflag)
			{
				SetUDPDataCanSendStat(TRUE);
			}
			else if (0 == smssendoverflag)
			{
				SetSMSDataCanSendStat(TRUE);
			}
			else
			{

			}
			// Cmd sending
			// ATCommandSending();
		}
		// Clear Buffer
		memset((char *)Uart3ParseBuffer,0,sizeof(Uart3ParseBuffer));
		#ifdef MODEM_AT_FLOW_CONTROL
		// Clear Busy
		ModemRTSEnControl(ENABLE);
		#endif
	}
}

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2018. All rights reserved
                                End Of The File
*******************************************************************************/
