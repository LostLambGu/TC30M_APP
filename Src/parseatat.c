/*******************************************************************************
* File Name          : parseatat.c
* Author               : Jevon
* Description        : This file provides all the parseatat functions.

* History:
*  06/02/2015 : parseatat V1.00
*******************************************************************************/

#include "parseatat.h"
#include "ltecatm.h"
#include "network.h"
#include "iocontrol.h"
#include "wedgecommonapi.h"
#include "wedgertctimer.h"
#include "wedgeeventalertflow.h"

#include "rtc.h"

#define ParseatPrint DebugPrintf

#undef NRCMD
#define NRCMD (1)

extern DebugCtlPrarm DbgCtl;

u8 resetFSM = FALSE;
AtBuf AtBuffer;

static FSMDescriptor fsm_desc;
static ATDHParms ATDH_parms;

const UINT8 StrHead[HEAD_LENGTH] 	= {0x0D, 0x0A};
const UINT8 StrTail[TAIL_LENGTH] 	= {0x0D, 0x0A};

uint16_t smsindex = 0;

uint16 smsLength = 0;
char StrBody[AT_MAX_AT_CMD_LEN];

char ICCIDBuf[32] = {0};
char IMEIBuf[32] = {0};
uint16_t ModemtimeZoneGet = 0;
uint32_t ModemTimeInSeconds = 0;
TimeTableT ModemTimetalbeGet = {0};

const ARCA_ATCMDStruct  gsm_rsp_cmd[GSM_FB_LAST_GSM_FB] = 
{
	// LTE
	{GSM_FB_SYSSTART,			"+SYSSTART"},
	{GSM_FB_CEREG,				"+CEREG:"},
	{GSM_FB_CSQ,				"+CSQ:"},
	{GSM_FB_CESQ,				"+CESQ:"},
	{GSM_FB_OK,					"OK"},
	{GSM_FB_ERROR,				"ERROR"},
	{GSM_FB_CIMI,      				"+CIMI:"},
	{GSM_FB_CNUM,      			"+CNUM:"},
	{GSM_FB_CGSN,      			"+CGSN:"},
	{GSM_FB_CCLK,      				"+CCLK:"},
	{GSM_FB_CTZR,      				"+CTZR:"},
	{GSM_FB_COPN,      			"+COPN:"},
	{GSM_FB_CGDCONT,            "+CGDCONT:"},
	{GSM_FB_CGCONTRDP,          "+CGCONTRDP:"},
	{GSM_FB_CRSM,				"+CRSM:"},
	{GSM_FB_CMGS,				"+CMGS:"},		// +CMGS: <mr>[,<scts>]
	{GSM_FB_CMTI,				"+CMTI:"},		// +CMTI:"RAM",0
	{GSM_FB_CMGR,				"+CMGR:"},
	{GSM_FB_CPIN,				"+CPIN:"},
	{GSM_FB_CGACT,				"+CGACT:"},
	{GSM_FB_SQNSI,				"+SQNSI:"},		// Socket Information
	{GSM_FB_SQNSS,				"+SQNSS:"},		// Socket Status
	{GSM_FB_SQNSH,				"+SQNSH:"},		// Socket Status
	{GSM_FB_SQNSRECV,			"+SQNSRECV:"},
	{GSM_FB_SQNSRING,			"+SQNSRING:"},
	{GSM_FB_SQNHTTPRING,			"+SQNHTTPRING:"},
	{GSM_FB_CMSERROR,     			"+CMS ERROR:"},	// +CMS ERROR:513 (lower layer error)
	{GSM_FB_CMEERROR,			"+CME ERROR:"},
	{GSM_FB_SQNVERS1,     			"UE"},			// UE5.0.0.0c
	{GSM_FB_SQNVERS2,     			"LR"},			// LR5.1.1.0-32741
	// CBP82
	{GSM_FB_HCMGSS,     			"^HCMGSS:"},
	{GSM_FB_PPPCONIP,			"^PPPCONIP:"},
	{GSM_FB_SHORTSMS,			"+SMS:"},
	{GSM_FB_TFTPSTATUS,			"+TFTPSTATUS:"},
	{GSM_FB_DNLD,				"+MCU:"},
	{GSM_FB_VLOCINFO,			"+VLOCINFO:"},
	{GSM_FB_BARCODE,			"+BARCODE:"},

	{GSM_FB_UNDEFINITION_FB,		"XXXXXXXX"}
};

const ATRspParmFmtT ATParmFmt[GSM_FB_LAST_GSM_FB] = 
{
	// LTE
	/*GSM_FB_SYSSTART,*/			{0, 		{NUM_TYPE}},
	/*GSM_FB_CEREG*/			{0, 		{NUM_TYPE}},
	/*GSM_FB_CSQ,*/				{2, 		{NUM_TYPE,NUM_TYPE}},
	/*GSM_FB_CESQ,*/			{6,		{NUM_TYPE,NUM_TYPE,NUM_TYPE,NUM_TYPE,NUM_TYPE,NUM_TYPE}},
	/*GSM_FB_OK,*/				{0, 		{NUM_TYPE}},
	/*GSM_FB_ERROR,*/			{0, 		{NUM_TYPE}},
	/*GSM_FB_CIMI,*/				{0, 		{NUM_TYPE}},
	/*GSM_FB_CNUM,*/			{3, 		{STR_WITH_QUOTE_TYPE,STR_WITH_QUOTE_TYPE,NUM_TYPE}},
	/*GSM_FB_CGSN,*/			{1,		{STR_WITH_QUOTE_TYPE}},
	/*GSM_FB_CCLK,*/				{6, 		{NUM_TYPE,NUM_TYPE,NUM_TYPE,NUM_TYPE,NUM_TYPE,NUM_TYPE}},
	/*GSM_FB_CTZR*/				{2, 		{NUM_TYPE}},
	/*GSM_FB_COPN*/				{2,		{STR_WITH_QUOTE_TYPE,STR_TYPE}},
	/*GSM_FB_CGDCONT*/          {11,		{NUM_TYPE, STR_WITH_QUOTE_TYPE, STR_WITH_QUOTE_TYPE, TYPE_NUM, TYPE_NUM, TYPE_NUM, NUM_TYPE, NUM_TYPE, NUM_TYPE, NUM_TYPE, NUM_TYPE}},
	/*GSM_FB_CGCONTRDP*/		{9, 		{NUM_TYPE, NUM_TYPE, STR_WITH_QUOTE_TYPE, STR_WITH_QUOTE_TYPE, STR_WITH_QUOTE_TYPE, STR_WITH_QUOTE_TYPE, STR_WITH_QUOTE_TYPE, STR_WITH_QUOTE_TYPE, STR_WITH_QUOTE_TYPE}},
	/*GSM_FB_CRSM*/				{3,		{NUM_TYPE,NUM_TYPE,STR_TYPE}},
	/*GSM_FB_CMGS*/			{1,		{NUM_TYPE}},		
	/*GSM_FB_CMTI,*/				{2, 		{STR_WITH_QUOTE_TYPE,NUM_TYPE}},
	/*GSM_FB_CMGR*/			{4,		{STR_WITH_QUOTE_TYPE,STR_WITH_QUOTE_TYPE,STR_WITH_QUOTE_TYPE,STR_WITH_QUOTE_TYPE}},				
	/*GSM_FB_CPIN*/				{1,		{STR_TYPE}},	
	/*GSM_FB_CGACT*/				{2,		{NUM_TYPE,NUM_TYPE}},
	/*GSM_FB_SQNSI*/			{6,		{NUM_TYPE,NUM_TYPE,STR_WITH_QUOTE_TYPE,NUM_TYPE,STR_WITH_QUOTE_TYPE,NUM_TYPE}},	
	/*GSM_FB_SQNSS*/			{6,		{NUM_TYPE,NUM_TYPE,STR_TYPE,NUM_TYPE,STR_TYPE,NUM_TYPE}},
	/*GSM_FB_SQNSH*/			{1, 		{NUM_TYPE}},
	/*GSM_FB_SQNSRECV*/			{2,		{NUM_TYPE,NUM_TYPE}},	
	/*GSM_FB_SQNSRING*/			{0, 		{NUM_TYPE}},	
	/*GSM_FB_SQNHTTPRING*/		{4,		{NUM_TYPE,NUM_TYPE,STR_WITH_QUOTE_TYPE,NUM_TYPE}},
	/*GSM_FB_CMSERROR,*/		{1, 		{NUM_TYPE}},
	/*GSM_FB_CMEERROR,*/		{1, 		{NUM_TYPE}},
	/*GSM_FB_SQNVERS1,*/		{1, 		{STR_TYPE}},
	/*GSM_FB_SQNVERS2,*/		{1, 		{STR_TYPE}},
	// CBP82
	/*GSM_FB_HCMGSS,*/			{1, 		{NUM_TYPE}},
	/*GSM_FB_PPPCONIP,*/			{0, 		{NUM_TYPE}},
	/*GSM_FB_SHORTSMS,*/		{0, 		{NUM_TYPE}},
	/*GSM_FB_TFTPSTATUS,*/		{1, 		{NUM_TYPE}},
	/*GSM_FB_DNLD,*/			{1, 		{STR_TYPE}},
	/*GSM_FB_VLOCINFO,*/			{8, 		{NUM_TYPE,NUM_TYPE,NUM_TYPE,NUM_TYPE,NUM_TYPE,NUM_TYPE,NUM_TYPE,NUM_TYPE}},
	/*GSM_FB_BARCODE,*/			{1, 		{STR_TYPE}},

	/*GSM_FB_UNDEFINITION_FB,*/	{0, 		{NUM_TYPE}}
};

MAKEFSMTable(ATDH_fsm_tbl, ATDHTBL_STATE_MAX, NUM_ATDHEvents, ATDHTBL_NUM_ACT) =
{
  /*   State is IDLE [DH0]  
   *   -------------------
   *   Next State            Action List              Event */
  {
    {ATDH_CHECK_HEAD_ST,              			/* DPD_TxReq */
     ATDH_CheckCmdHead_AC},
    {ATDH_IDLE_ST,             /* set */
     NO_ACTION},
    {ATDH_IDLE_ST,                                   /* clr */
     NO_ACTION},
  },

  /*   State is CHECK_HEAD [CSA]
   *   -----------------------------------
   *   Next State            Action List              Event */
  {
    {ATDH_CHECK_HEAD_ST,                /* DPD_TxReq */
     NO_ACTION},
    {ATDH_CHECK_TAIL_ST,                 	/* set */
     ATDH_CheckCmdTail_AC},
    {ATDH_IDLE_ST,                   	/* clr */
     NO_ACTION},
  },

  /*   State is CHECK_TAIL [CSA]
   *   -----------------------------------
   *   Next State            Action List              Event */
  {
    {ATDH_CHECK_TAIL_ST,                /* DPD_TxReq */
     NO_ACTION},
    {ATDH_PARSE_EXECUTE_CMD_ST,                 /* set */
     ATDH_ParseExeCmd_AC},
    {ATDH_WFR_CMD_BODY_ST,                   	/* clr */
     NO_ACTION},
  },

  /*   State is WFR_CMD_BODY [CSC]
   *   ---------------------------------
   *   Next State            Action List              Event */
  {
    {ATDH_CHECK_TAIL_ST,                          /* DPD_TxReq */ 
     ATDH_ReenterCmdFound_AC},
    {ATDH_WFR_CMD_BODY_ST,                         /* set */
     NO_ACTION},
    {ATDH_WFR_CMD_BODY_ST,                          /* clr */
     NO_ACTION},
  },

  /*   State is PARSE_EXECUTE_CMD [CSD]
   *   ---------------------------------
   *   Next State            Action List              Event */
  {
    {ATDH_PARSE_EXECUTE_CMD_ST,                          /* DPD_TxReq */ 
     NO_ACTION},
    {ATDH_CHECK_HEAD_ST,                     		/* set */
     ATDH_NextCmd_AC},
    {ATDH_IDLE_ST,                         		/* clr */
     NO_ACTION},
  },
};

/*========================================
              Function Delcare
   =======================================*/
static u8 check_cmd_head (void);
static u8 check_cmd_tail(void);
static u8 parse_exe_cmd(void);
static u8 perform_action (FSMPtr fsm, FSMAction action);

/*========================================
              Command  Processing  routine statement
   =======================================*/
// LTE
static void MmiSYSSTART (ATRspParmT* MsgDataP);
static void MmiCEREG(char *MsgDataP);
static void MmiCSQ (ATRspParmT* MsgDataP);
static void MmiCESQ (ATRspParmT* MsgDataP);
static void MmiOK (ATRspParmT* MsgDataP);
static void MmiERROR (ATRspParmT* MsgDataP);
static void MmiCIMI (char *MsgDataP);
// static void MmiCNUM (ATRspParmT* MsgDataP);
static void MmiCGSN (ATRspParmT* MsgDataP);
static void MmiCCLK (ATRspParmT* MsgDataP);
static void MmiCTZR(ATRspParmT* MsgDataP);
static void MmiCOPN(ATRspParmT* MsgDataP);
static void MmiCGDCONT(ATRspParmT* MsgDataP);
static void MmiCGCONTRDP(ATRspParmT* MsgDataP);
static void MmiCRSM(ATRspParmT* MsgDataP);
static void MmiCMGS(ATRspParmT* MsgDataP);
static void MmiCMTI(ATRspParmT* MsgDataP);
static void MmiCMGR(ATRspParmT* MsgDataP);
static void MmiCPIN(ATRspParmT* MsgDataP);
static void MmiCGACT(ATRspParmT* MsgDataP);
// static void MmiSQNSI(char* MsgDataP);
static void MmiSQNSS(ATRspParmT* MsgDataP);
static void MmiSQNSH(ATRspParmT* MsgDataP);
// static void MmiSQNSRECV(ATRspParmT* MsgDataP);
static void MmiSQNSRING(char* MsgDataP);
static void MmiSQNHTTPRING(ATRspParmT* MsgDataP);
static void MmiCMSERROR(ATRspParmT* MsgDataP);
static void MmiCMEERROR (ATRspParmT* MsgDataP);
static void MmiSQNVERSTR1(ATRspParmT* MsgDataP);
static void MmiSQNVERSTR2(ATRspParmT* MsgDataP);
// // CBP82
// static void MmiHCMGSS(ATRspParmT* MsgDataP);
// static void MmiPPPCONIP(char* MsgDataP);
// static void MmiSHORTSMS(char* MsgDataP);
// static void MmiTFTPSTATUS(ATRspParmT* MsgDataP);
// static void MmiDNLD(ATRspParmT* MsgDataP);
// static void MmiVLOCINFO(ATRspParmT* MsgDataP);
// static void MmiBARCODE(ATRspParmT* MsgDataP);
static void MmiATDefault (GSM_FB_CMD FB_CMD,char* MsgDataP);

void RefreshImei(void)
{
	SendATCmd(GSM_CMD_IMEI, GSM_CMD_TYPE_EVALUATE, "1");
}

u8 CheckIpAddress(char* p)
{
	int a,b,c,d;
	if (4==sscanf(p,"%d.%d.%d.%d",&a,&b,&c,&d)) 
	{
		if (0<=a && a<=255
			&& 0<=b && b<=255
			&& 0<=c && c<=255
			&& 0<=d && d<=255) 
		{
			// #ifdef ATRSP_RESULT
			// if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
			// 	ParseatPrint(NRCMD,"\r\n[%s] PAT:Valid IPv4",FmtTimeShow());
			// #endif
			return TRUE;
		} 
		else 
		{
			// #ifdef ATRSP_RESULT
			// if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
			// 	ParseatPrint(NRCMD,"\r\n[%s] PAT:Iinvalid IPv4",FmtTimeShow());
			// #endif
			return FALSE;
		}
	} 
	else 
	{
		// #ifdef ATRSP_RESULT
		// if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
		// 	ParseatPrint(NRCMD,"\r\n[%s] PAT:Iinvalid IPv4",FmtTimeShow());
		// #endif
		return FALSE;
	}
}

static u8 check_cmd_head (void)
{
/*	UINT8 txCh = 0;
	
	ATDH_parms.Head.strp = StrHead;
	while (ATDH_parms.process.len)
	{
		txCh = *(ATDH_parms.process.buf++);
		ATDH_parms.process.len--;
		if (txCh == ATDH_parms.Head.strp[ATDH_parms.Head.CheckIndex])
		{
			ATDH_parms.Head.CheckIndex++;
			if (ATDH_parms.Head.CheckIndex >= HEAD_LENGTH)
			{
				ATDH_parms.Head.CheckIndex = 0;
				return TRUE;
			}
		}
		else if (ATDH_parms.Head.CheckIndex > 0)
		{
			ATDH_parms.Head.CheckIndex = 0;
			if (txCh == ATDH_parms.Head.strp[ATDH_parms.Head.CheckIndex])
			{
				ATDH_parms.Head.CheckIndex++;
			}
		}
	}
	return FALSE;*/
	if(ATDH_parms.process.len >= 2 && \
		ATDH_parms.process.buf[0] == StrHead[0] && \
		ATDH_parms.process.buf[1] == StrHead[1])
	{
		ATDH_parms.process.buf++;
		ATDH_parms.process.buf++;
		ATDH_parms.process.len--;
		ATDH_parms.process.len--;
	}
	return TRUE;
}

static u8 check_cmd_tail(void)
{
	UINT8 txCh = 0;
	// Check Tail
	ATDH_parms.Tail.strp = StrTail;
	while (ATDH_parms.process.len)
	{
		txCh = *(ATDH_parms.process.buf++);
		ATDH_parms.process.len--;
		*(ATDH_parms.datap++) = txCh;		
		ATDH_parms.datalen++;
		if (ATDH_parms.datalen >= AT_MAX_AT_CMD_LEN)
		{
			ATDH_parms.Tail.CheckIndex = 0;
			return TRUE;
		}
		else if (txCh == ATDH_parms.Tail.strp[ATDH_parms.Tail.CheckIndex])
		{
			ATDH_parms.Tail.CheckIndex++;
			if (ATDH_parms.Tail.CheckIndex >= TAIL_LENGTH)
			{
				ATDH_parms.Tail.CheckIndex = 0;
				return TRUE;
			}
		}
		else if (ATDH_parms.Tail.CheckIndex > 0)
		{
			ATDH_parms.Tail.CheckIndex = 0;
			if (txCh == ATDH_parms.Tail.strp[ATDH_parms.Tail.CheckIndex])
			{
				ATDH_parms.Tail.CheckIndex++;
			}
		}
	}
	return FALSE;
}

GSM_FB_CMD ValATRspParse(uint8 *DataP, uint16 DataLen, ATRspParmT* pRspData)
{
	GSM_FB_CMD ATRspType = GSM_FB_LAST_GSM_FB;
	uint8 *ptr = DataP;
	uint8 *qtr = NULL;
	uint16 i = 0;
	uint8 ParmNum = 0, j = 0, k = 0;
	char ParmTmp[GSM_MAX_PARAM_LEN];

	// #ifdef PARSEAT_AT
	// if(gDeviceConfig.DbgCtl.ParseatCmdEn == TRUE)
	// 	ParseatPrint(NRCMD,"\r\n[%s] PAT:Recv Cmd(%s)", FmtTimeShow(),ptr);
	// #endif
	memset((void*)ParmTmp, 0, sizeof(ParmTmp));
	while (*ptr == '\r' || *ptr == '\n' )
	{
		ptr++;
	}
	// Lookup Table
	for (i = 0; i < GSM_FB_LAST_GSM_FB; i++)
	{
		if (!strncmp((const char*)ptr, gsm_rsp_cmd[i].strCmdString, strlen((char*)gsm_rsp_cmd[i].strCmdString)))
		{
			ATRspType = (GSM_FB_CMD)gsm_rsp_cmd[i].nIndex;
			break;
		}
	}
	// Show
	// #ifdef PARSEAT_AT
	// if(gDeviceConfig.DbgCtl.ParseatCmdEn == TRUE)
	// 	ParseatPrint(NRCMD,"\r\n[%s] PAT:Type(%d) P(%s) P(%x %x %x %x %d)", \
	// 		FmtTimeShow(),ATRspType,(char*)ptr,ptr[0],ptr[1],ptr[2],ptr[3],strlen((char*)ptr));
	// #endif
	// Special Handling
	if(ATRspType == GSM_FB_CEREG || \
		ATRspType == GSM_FB_PPPCONIP || \
		ATRspType == GSM_FB_SQNSRING || \
		ATRspType == GSM_FB_SHORTSMS)
	{		
		return ATRspType;
	}
	// Get Each Param
	if(ATRspType == GSM_FB_LAST_GSM_FB)
	{
		/*ParseatPrint(NRCMD,"\r\n[%s] PAT: command(%d %s 0x%x 0x%x)", \
			FmtTimeShow(),DataLen, ptr, ptr[15], ptr[16]);*/
		if(DataLen == 17 && ptr[15] == 0x0D && ptr[16] == 0x0A)
		{
			u8 index;
			for(index = 0; index < 15; index++)
			{
				if(IS_NUMBER_ONLY(ptr[index]) == FALSE)
					return GSM_FB_LAST_GSM_FB;
			}
			return GSM_FB_CIMI;
		}
	}
	else if (ATRspType < GSM_FB_LAST_GSM_FB)
	{
		memset((void*)pRspData, 0, sizeof(ATRspParmT));
		ptr += strlen((char*)gsm_rsp_cmd[i].strCmdString);
		ParmNum = ATParmFmt[i].ParmNum;  //get the corresponding respone type
		// #ifdef PARSEAT_AT
		// if(gDeviceConfig.DbgCtl.ParseatCmdEn == TRUE)
		// 	ParseatPrint(NRCMD,"\r\n[%s] PAT:Pnum(%d) P(%s)", FmtTimeShow(),ParmNum,(char*)ptr);
		// #endif
		for (j = 0; j < ParmNum; j++)
		{
			while (*ptr == ' ')
				ptr++;
			qtr = ptr;
			memset((void*)ParmTmp, 0, sizeof(ParmTmp));
			// #ifdef PARSEAT_AT
			// if(gDeviceConfig.DbgCtl.ParseatCmdEn == TRUE)
			// 	ParseatPrint(NRCMD,"\r\n[%s] PAT:Q(%s)", FmtTimeShow(),(char*)qtr);
			// #endif
			if(ATRspType == GSM_FB_CCLK)
			{
				//"10/04/26,21:50:19","15301606355"
				// +CCLK:2015/7/14,14:25:54+32
				//+CCLK: "17/10/26,23:20:23-16"
				if(*qtr == '\"')
					qtr++;
				for (k = 0; ((*qtr != ' ') && (*qtr != '\"') && (*qtr != ',') && (*qtr != '/') && (*qtr != ':') && (*qtr != '-') && (*qtr != '+') &&  (*qtr != '\r') && (*qtr != '\n') && (k < (GSM_MAX_PARAM_LEN - 1))); qtr++, k++)
				{
					ParmTmp[k] = *qtr;
				}
				while (*qtr != '\"' && *qtr != ',' && *qtr != '/' && *qtr != ':'&& *qtr != '-' && *qtr != '+' && *qtr != '\r' && *qtr != '\n' )
				{
					qtr++;
				}
			}
			else
			{
				if(*qtr == '\"')
				{
					for (k = 0; ((*qtr != '\"') || (*(qtr+1) != ',')) && (*qtr != '\r') && (*qtr != '\n') && (k < (GSM_MAX_PARAM_LEN - 1)); qtr++, k++)
					{
						ParmTmp[k] = *qtr;
					}
					if(*qtr == '\"')
					{
						ParmTmp[k] = '\"';	
					}
				}
				else
				{
					for (k = 0; (*qtr != ',') && (*qtr != '\r') && (*qtr != '\n') && (k < (GSM_MAX_PARAM_LEN - 1)); qtr++, k++)
					{
						ParmTmp[k] = *qtr;
					}
				}
				while (*qtr != ',' && *qtr != '\r' && *qtr != '\n' )
				{
					qtr++;
				}
			}
			// #ifdef PARSEAT_AT
			// if(gDeviceConfig.DbgCtl.ParseatCmdEn == TRUE)
			// 	ParseatPrint(NRCMD,"\r\n[%s] PAT:T(%d)ij(%d %d) T(%d) String(%s) Len(%d)", \
			// 		FmtTimeShow(),ATRspType,i,j,ATParmFmt[i].ParmType[j],\
			// 		ParmTmp,strlen((char*)ParmTmp));
			// #endif
			switch (ATParmFmt[i].ParmType[j])
			{
				case NUM_TYPE:
					pRspData[j].Num = atoi((const char*)ParmTmp);
					break;
				case STR_TYPE:
					memset((void*)pRspData[j].Str, 0, sizeof(pRspData[j].Str));
					strcpy(pRspData[j].Str, ParmTmp);
					break;
				case STR_WITH_QUOTE_TYPE:
					if(strlen((char*)ParmTmp) >= 2)
					{
						memset((void*)pRspData[j].Str, 0, sizeof(pRspData[j].Str));
						strncpy(pRspData[j].Str, (ParmTmp+1), (strlen((char*)ParmTmp) - 2));
					}
					break;
				default:
					break;
			}
			ptr = qtr + 1;
		}	
	}

	return ATRspType;
}

void ValATRspProcess(GSM_FB_CMD RspCmdType, ATRspParmT* pRspData, char *pOriginalData)
{
	u16 OrgDatalength = strlen((char*)pOriginalData);
	// #ifdef PARSEAT_AT
	// if(gDeviceConfig.DbgCtl.ParseatCmdEn == TRUE)
	// 	ParseatPrint(NRCMD,"\r\n[%s] PAT:RspCmdType(%d) cmdIndex(%d) len(%d)", \
	// 		FmtTimeShow(),RspCmdType,cmdIndex, OrgDatalength);
	// #endif
	// remove 0d 0a
	if(((OrgDatalength - 2) > 0) && \
		(OrgDatalength < (AT_MAX_AT_CMD_LEN - 2)))
	{
		pOriginalData[OrgDatalength -2] = '\0';
	}
	// check type
	switch(RspCmdType)
	{
		// LTE
		case GSM_FB_SYSSTART:
			MmiSYSSTART(pRspData);
			break;
		case GSM_FB_CEREG:
			MmiCEREG(pOriginalData);
			break;
		case GSM_FB_CSQ:
			MmiCSQ(pRspData);
			break;
		case GSM_FB_CESQ:
			MmiCESQ(pRspData);
			break;
		case GSM_FB_OK:
			MmiOK(pRspData);
			break;
		case GSM_FB_ERROR:
			MmiERROR(pRspData);
			break;
		case GSM_FB_CIMI:
			MmiCIMI(pOriginalData);
			break;
		// case GSM_FB_CNUM:
		// 	MmiCNUM(pRspData);
		//	break;
		case GSM_FB_CGSN:
			MmiCGSN(pRspData);
			break;
		case GSM_FB_CCLK:
			MmiCCLK(pRspData);
			break;
		case GSM_FB_CTZR:
			MmiCTZR(pRspData);
			break;
		case GSM_FB_COPN:
			MmiCOPN(pRspData);
			break;
		case GSM_FB_CGDCONT:
			MmiCGDCONT(pRspData);
			break;
		case GSM_FB_CGCONTRDP:
			MmiCGCONTRDP(pRspData);
			break;
		case GSM_FB_CRSM:
			MmiCRSM(pRspData);
			break;				
		case GSM_FB_CMGS:
			MmiCMGS(pRspData);
			break;				
		case GSM_FB_CMTI:
			MmiCMTI(pRspData);
			break;
		case GSM_FB_CMGR:
			MmiCMGR(pRspData);
			break;
		case GSM_FB_CPIN:
			MmiCPIN(pRspData);
			break;
		case GSM_FB_CGACT:
			MmiCGACT(pRspData);
			break;
		// case GSM_FB_SQNSI:
		// 	MmiSQNSI(pOriginalData);
		// 	break;
		case GSM_FB_SQNSS:
			MmiSQNSS(pRspData);
			break;
		case GSM_FB_SQNSH:
			MmiSQNSH(pRspData);
			break;
		// case GSM_FB_SQNSRECV:
		// 	MmiSQNSRECV(pRspData);
		// 	break;
		case GSM_FB_SQNSRING:
			MmiSQNSRING(pOriginalData);
			break;
		case GSM_FB_SQNHTTPRING:
			MmiSQNHTTPRING(pRspData);
			break;
		case GSM_FB_CMSERROR:
			MmiCMSERROR(pRspData);
			break;															
		case GSM_FB_CMEERROR:
			MmiCMEERROR(pRspData);
			break;
		case GSM_FB_SQNVERS1:
			MmiSQNVERSTR1(pRspData);
			break;
		case GSM_FB_SQNVERS2:
			MmiSQNVERSTR2(pRspData);
			break;
		// CBP82
		// case GSM_FB_HCMGSS:
		// 	MmiHCMGSS(pRspData);
		// 	break;
		// case GSM_FB_PPPCONIP:
		// 	MmiPPPCONIP(pOriginalData);
		// 	break;
		// case GSM_FB_SHORTSMS:
		// 	MmiSHORTSMS(pOriginalData);
		// 	break;
		// case GSM_FB_TFTPSTATUS:
		// 	MmiTFTPSTATUS(pRspData);
		// 	break;
		// case GSM_FB_DNLD:
		// 	MmiDNLD(pRspData);			
		// 	break;
		// case GSM_FB_VLOCINFO:
		// 	MmiVLOCINFO(pRspData);			
		// 	break;
		// case GSM_FB_BARCODE:
		// 	MmiBARCODE(pRspData);			
		// 	break;

		default:
			MmiATDefault(RspCmdType,pOriginalData);
			break;
	}
}

void ReceiveDataProcess(UINT8 *data, UINT16 length)
{
	GSM_FB_CMD RspType;
	ATRspParmT ATParameter[GSM_MAX_PARAM];
	/*parse and execute cmd*/
	// #ifdef PARSEAT_AT
	// if(gDeviceConfig.DbgCtl.ParseatCmdEn == TRUE)
	// 	ParseatPrint(NRCMD,"\r\n[%s] PAT:length(%d) data(%s)", FmtTimeShow(),length,data);
	// #endif

	RspType = ValATRspParse(data,length,ATParameter);
	ValATRspProcess(RspType, ATParameter, (char*)data);
}

static u8 parse_exe_cmd(void)
{
	/*parse and execute cmd*/
	// #ifdef PARSEAT_AT
	// if(gDeviceConfig.DbgCtl.ParseatCmdEn == TRUE)
	// 	ParseatPrint(NRCMD,"\r\n[%s] PAT:len(%d) Str(%x %x %x %x) len(%d)", \
	// 		FmtTimeShow(), \
	// 		ATDH_parms.datalen, \
	// 		StrBody[0],StrBody[1],StrBody[2],StrBody[3], \
	// 		ATDH_parms.process.len);
	// #endif
	
	ReceiveDataProcess((UINT8 *)StrBody,strlen((char*)StrBody));

	/*after parse and execute cmd*/
	ATDH_parms.datap = (UINT8*)StrBody;
	ATDH_parms.datalen = 0;
	memset((void*)StrBody, 0, sizeof(StrBody));
	if (ATDH_parms.process.len)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

static u8 perform_action (FSMPtr fsm, FSMAction action)
{
	// #ifdef PARSEAT_AT
	// if(gDeviceConfig.DbgCtl.ParseatCmdEn == TRUE)
	// 	ParseatPrint(NRCMD,"\r\n[%s] PAT:action(%d)", FmtTimeShow(),action);
	// #endif
	switch (action)
	{
		case ATDH_CheckCmdHead_AC:
			ATDH_parms.process = *(AtBuf*)(getEventParameters(fsm));

		case ATDH_NextCmd_AC:
			processEventWithNoParms(fsm, (FSMEvent)((check_cmd_head() ? (UINT8)ATDH_set : (UINT8)ATDH_clr)));
			break;
	
		case ATDH_ReenterCmdFound_AC:
			ATDH_parms.process = *(AtBuf*)(getEventParameters(fsm));

		case ATDH_CheckCmdTail_AC:
			processEventWithNoParms(fsm, (FSMEvent)((check_cmd_tail() ? (UINT8)ATDH_set : (UINT8)ATDH_clr)));
			break;
	
		case ATDH_ParseExeCmd_AC:
			processEventWithNoParms(fsm, (FSMEvent)((parse_exe_cmd() ? (UINT8)ATDH_set : (UINT8)ATDH_clr)));
			break;

		case NO_ACTION:
		case NO_EVENT:
		case UNEXPECTED_EVENT_AC:
			break;
		default:
			// #ifdef PARSEAT_AT
			// if(gDeviceConfig.DbgCtl.ParseatCmdEn == TRUE)
			// 	ParseatPrint(NRCMD,"\r\n[%s] PAT:Err action(%d)", FmtTimeShow(),action);
			// #endif
//			assert(0);
			break;
	}

	return TRUE;
}

void atDhParmInit(void)
{
	ATDH_parms.Head.strp = StrHead;
	ATDH_parms.Head.CheckIndex = 0;
	ATDH_parms.Tail.strp = StrTail;
	ATDH_parms.Tail.CheckIndex = 0;
	ATDH_parms.datap = (UINT8*)StrBody;
	ATDH_parms.datalen = 0;
	ATDH_parms.process.buf = NULL;
	ATDH_parms.process.len = 0;
	memset((void*)StrBody, 0, sizeof(StrBody));
}

void parseAtDHInit (void)
{
	// #ifdef PARSEAT_AT
	// if(gDeviceConfig.DbgCtl.ParseatCmdEn == TRUE)
	// 	ParseatPrint(NRCMD,"\r\n[%s] PAT:Init",FmtTimeShow());
	// #endif
	atDhParmInit();

	initializeFSM(&fsm_desc, FSMTable(ATDH_fsm_tbl), ATDHTBL_STATE_MAX,
	            NUM_ATDHEvents, ATDHTBL_NUM_ACT, ATDH_IDLE_ST, perform_action,
	            NULL);
}

void atDHProcessEvent (AtDHEvent event, void* parms, UINT8 parmSize)
{
	// #ifdef PARSEAT_AT
	// if(gDeviceConfig.DbgCtl.ParseatCmdEn == TRUE)
	// 	ParseatPrint(NRCMD,"\r\n[%s] PAT:event(%d) parmSize(%d)", FmtTimeShow(),event,parmSize);
	// #endif
	processEventWithParmStruct(&fsm_desc, event, parms, parmSize);
}

static void MmiSYSSTART (ATRspParmT* MsgDataP)
{
	// Module powered on indicator
	// +IMS: START
	// #ifdef ATRSP_RESULT
	// if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
		ParseatPrint(DbgCtl.ParseatCmdEn,"\r\n[%s] PAT:+SYSSTART", FmtTimeShow());
	// #endif
	// Set Sleep Flag
	// SetSystemEventMode(SYS_EVENT_ATCOMMAND_TO_MODEM_MSK);
	// Change Network Status
	SetNetworkReadyStat(FALSE);
	SetSimCardReadyStat(FALSE);
	SetNetworkRssiValue(DEFAULT_RSSI_VALUE);
	SetNetworkMachineStatus(NET_FREE_IDLE_STAT);
	// Change AT Port Status
	SetModemATPortStat(TRUE);
	// Set startup flag
	SetModemNeedInitStat(TRUE);
	// Reset AT Init Timer
	SoftwareTimerReset(&ModemATInitTimer,ModemATInitTimerCallback,MODEM_AT_INIT_DELAY_TIMEOUT);
	SoftwareTimerStart(&ModemATInitTimer);
	//Reset Check Modem Timeout Time
	// ResetCheckModemTimeoutTim();
} 

static void MmiCEREG(char *MsgDataP)
{
	//AT+CEREG=<n> +CME ERROR: <err>
	//AT+CEREG? +CEREG: <n>,<stat>[,[<tac>],[<ci>],[<AcT>[,<cause_type>,<reject_cause>]]]
	//AT+CEREG=? +CEREG: (list of supported <n>s))

	// <n> n = 0,1,2,3 Integer. Indicates the EPS registration status.
	// 0 disable network registration unsolicited result code
	// 1 enable network registration unsolicited result code +CEREG: <stat>
	// 2 enable network registration and location information unsolicited result code +CEREG: <stat>[,[<tac>],[<ci>],[<AcT>]]
	// 3 enable network registration, location information and EMM cause value information unsolicited result code +CEREG:
	// <stat>[,[<tac>],[<ci>],[<AcT>][,<cause_type>,<reject_cause>]]

	// <stat> stat = 0,1,2,3,4,5,6,7,8,9,10 
	// NOTE 2:3GPP TS 24.008 [8] and 3GPP TS 24.301 [83] specify the condition
	// when the MS is considered as attached for emergency bearer services.
	// 0 not registered, MT is not currently searching an operator to register to
	// 1 registered, home network 		(*****)
	// 2 not registered, but MT is currently trying to attach or searching an operator to register to
	// 3 registration denied
	// 4 unknown (e.g. out of E-UTRAN coverage)
	// 5 registered, roaming			(*****)
	// 6 registered for "SMS only", home network (not applicable)
	// 7 registered for "SMS only", roaming (not applicable)
	// 8 attached for emergency bearer services only (See NOTE 2)
	// 9 registered for "CSFB not preferred", home network (not applicable)
	// 10 registered for "CSFB not preferred", roaming (not applicable)

	//tac: String type; two byte tracking area code in hexadecimal format (e.g. "00C3"equals 195 in decimal).
	//ci: String type; four byte E-UTRAN cell ID in hexadecimal format
	//act: Integer. The parameter sets/shows the access technology of the serving cell
	//<stat>[,[<tac>],[<ci>],[<AcT>]]    eg: +CEREG: 1,28FF,0FA0CC01,7
	//+CEREG: <stat> 	eg: +CEREG: 0
	//+CEREG: <n>,<stat>	eg: +CEREG: 1,2
	char *NumP;
	char *CmdP;
	u8 RegisterStatus;

	// #ifdef ATRSP_RESULT
	// if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
	ParseatPrint(DbgCtl.ParseatCmdEn, "\r\n[%s] PAT:%s",
				 FmtTimeShow(), MsgDataP);
	// #endif

	NumP = strstr((char *)MsgDataP, (char *)"+CEREG:");
	if (NumP != NULL)
	{
		NumP += 7;
		CmdP = strstr((char *)MsgDataP, (char *)",\"");
		if (CmdP == NULL)
		{
			CmdP = strstr((char *)MsgDataP, (char *)",");
		}
		if (CmdP != NULL)
		{
			// +CREG:1,5
			if (CmdP[1] == '\"')
			{
				CmdP -= 1;
			}
			else
			{
				CmdP += 1;
			}

			if (CmdP[0] == ' ')
				RegisterStatus = CmdP[1];
			else
				RegisterStatus = CmdP[0];

			gModemParam.stat = RegisterStatus - '0';

			WedgeLteGreenLedBlink(FALSE);
			if ((RegisterStatus == '1') || (RegisterStatus == '5'))
			{
				SendATCmd(GSM_CMD_CGACT, GSM_CMD_TYPE_QUERY, NULL);
				SetNetworkReadyStat(TRUE);
				WedgeLteGreenLedControl(TRUE);
				NumP = strstr((char *)MsgDataP, (char *)"\",");
				if (NumP != NULL)
				{
					NumP += 2;
					NumP = strstr((char *)NumP, (char *)"\",");
					if (NumP != NULL)
					{
						NumP += 2;
						gModemParam.act = NumP[0] - '0';
						ParseatPrint(DbgCtl.ParseatCmdEn, "\r\n[%s] PAT:+CEREG check(stat 0x%X act 0x%X)",
									 FmtTimeShow(), RegisterStatus, NumP[0]);
					}
				}
			}
			else
			{
				SetNetworkReadyStat(FALSE);
				if (RegisterStatus == 0)
				{
					WedgeLteGreenLedControl(FALSE);
				}
				else
				{
					WedgeLteGreenLedBlink(TRUE);
				}
			}

			ParseatPrint(DbgCtl.ParseatCmdEn, "\r\n[%s] PAT:+CEREG check(stat 0x%X)",
						 FmtTimeShow(), RegisterStatus);
		}
		else
		{
			if (NumP[0] == ' ')
				RegisterStatus = NumP[1];
			else
				RegisterStatus = NumP[0];
			// #ifdef ATRSP_RESULT
			// if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
			// {
			ParseatPrint(DbgCtl.ParseatCmdEn, "\r\n[%s] PAT:+CREG unsolicited(%s 0x%X)",
						 FmtTimeShow(), NumP, RegisterStatus);
			// }
			// #endif
			// +CREG:1
			gModemParam.stat = RegisterStatus - '0';

			WedgeLteGreenLedBlink(FALSE);
			if ((RegisterStatus == '1') || (RegisterStatus == '5'))
			{
				SendATCmd(GSM_CMD_CGACT, GSM_CMD_TYPE_QUERY, NULL);
				SetNetworkReadyStat(TRUE);
				WedgeLteGreenLedControl(TRUE);
			}
			else
			{
				SetNetworkReadyStat(FALSE);
				if (RegisterStatus == 0)
				{
					WedgeLteGreenLedControl(FALSE);
				}
				else
				{
					WedgeLteGreenLedBlink(TRUE);
				}
			}
		}
	}
	//Reset Check Modem Timeout Time
	// ResetCheckModemTimeoutTim();
}

static void MmiCSQ (ATRspParmT* MsgDataP)
{
	static u8 CsqShakeProtectCount = 0;
	// #ifdef ATRSP_RESULT
	// if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
		ParseatPrint(DbgCtl.ParseatCmdEn,"\r\n[%s] PAT:+CSQ(%d %d) Count(%d+)", \
			FmtTimeShow(), \
			MsgDataP[0].Num, \
			MsgDataP[1].Num, \
			CsqShakeProtectCount);
	// #endif
	// Set AT Port Ready
	SetModemATPortStat(TRUE);
	// Csq Protect Process
	if(MsgDataP[0].Num == 0)
	{
		CsqShakeProtectCount++;
		// #ifdef ATRSP_RESULT
		// if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
			ParseatPrint(DbgCtl.ParseatCmdEn,"\r\n[%s] PAT:+CSQ Zero Protect(%d)", \
			FmtTimeShow(), \
			CsqShakeProtectCount);
		// #endif
		if(CsqShakeProtectCount >= 3)
		{
			CsqShakeProtectCount = 0;
			SetNetworkRssiValue((u8)MsgDataP[0].Num);
		}
	}
	else
	{
		CsqShakeProtectCount = 0;
		SetNetworkRssiValue((u8)MsgDataP[0].Num);
	}
	//Reset Check Modem Timeout Time
	// ResetCheckModemTimeoutTim();
}

static void MmiCESQ (ATRspParmT* MsgDataP)
{
	// +CESQ: 99,99,255,255,255,255
	// #ifdef ATRSP_RESULT
	// if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
		ParseatPrint(DbgCtl.ParseatCmdEn,"\r\n[%s] PAT:+CESQ(%d %d %d %d %d %d)", \
			FmtTimeShow(), \
			MsgDataP[0].Num,MsgDataP[1].Num, \
			MsgDataP[2].Num,MsgDataP[3].Num, \
			MsgDataP[4].Num,MsgDataP[5].Num);
	// #endif
}

static void MmiOK (ATRspParmT* MsgDataP)
{
	// #ifdef ATRSP_RESULT
	// if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
	// 	ParseatPrint(NRCMD,"\r\n[%s] PAT:FB OK", FmtTimeShow());
	// #endif

	ParseatPrint(NRCMD,"\r\n[%s] PAT:FB OK", FmtTimeShow());
	// Set AT Port Ready
	SetModemATPortStat(TRUE);
	// Cmd sending
	ATCommandSending();
	//Reset Check Modem Timeout Time
	// ResetCheckModemTimeoutTim();
	// Check data send status
	if(GetFwpCurrentIndex() == GSM_CMD_SQNHEXDAT)
	{
		// #ifdef ATRSP_RESULT
		// if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
		// {
		// 	ParseatPrint(NRCMD,"\r\n[%s] PAT: OK Type(%d) MSA(%d) Retry(%d %d %d %d) F(%d)", \
		// 		FmtTimeShow(), \
		// 		GetNetQueueElementMsgType(0), \
		// 		gGpsInfo.isGPSInMSALrunningMode, \
		// 		gRTCParam.MsgRetryTim.benable, \
		// 		UDPPacketInTransmittingFlag, \
		// 		UDPIssueNeedRebootModemFlag, \
		// 		UDPPacketSendOutRetryTimes, \
		// 		UDPIsNeedConfirmWithServerFlag);
		// }
		// #endif
		// Check Queue Status
		// if(GetNetQueueElementMsgType(0) == MSG_TYPE_UDP_END && \
		// 	gGpsInfo.isGPSInMSALrunningMode == FALSE)
		// {
		// 	// Stop Send End Session
		// 	StopSmsControlSokcetTimer(1);
		// 	// Set Status
		// 	SetNetworkMachineStatus(NET_SOCKET_DISCONNECT_STAT);
		// }
		// Clear Sleep Flag
		// if(GetNetQueueNum() == 0)
		// {
		// 	// ClearSystemEventMode(SYS_EVENT_UDP_SESSION_PROCESS_MSK);
		// }
		// else
		// {
		// 	#ifdef UDP_RPT_NEED_SERVER_CONFIRM
		// 	// Check Status
		// 	if(UDPIsNeedConfirmWithServerFlag == FALSE)
		// 	#endif
		// 	{
		// 		// Remove Item
		// 		NetQueueRemove(0);
		// 		// Close UDP Msg Retry Mechanism
		// 		CloseMsgRetryMechanism();
		// 	}
		// }
		// Check PPP Establish Report Flag
		// if(gDeviceConfig.PPPEstablishReportFlag == TRUE)
		// {
		// 	gDeviceConfig.PPPEstablishReportFlag = FALSE;
		// 	// Save Param
		// 	SaveParamToFlash();
		// 	#ifdef ATRSP_RESULT
		// 	if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
		// 		ParseatPrint(NRCMD,"\r\n[%s] PAT:PPP Establish Fail Report Sendout Successfull!", \
		// 			FmtTimeShow());
		// 	#endif
		// }
		// #ifdef ATRSP_RESULT
		// if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
		// 	ParseatPrint(NRCMD,"\r\n[%s] PAT:Sending successful(%d) Evt(0x%X 0x%X)", \
		// 		FmtTimeShow(), \
		// 		GetNetQueueNum(), \
		// 		GetSystemEventMode(), \
		// 		GetBle40EventMode());
		// #endif
	}
}

static void MmiERROR (ATRspParmT* MsgDataP)
{
	ParseatPrint(NRCMD,"\r\n[%s] PAT:FB ERROR CurrentIndex(%d)", FmtTimeShow(), GetFwpCurrentIndex());

	if (GetFwpCurrentIndex() == GSM_CMD_CMGS)
	{
		SetSMSNeedReleaseStat(TRUE);
		ParseatPrint(NRCMD,"\r\n[%s] PAT:FB ERROR GSM_CMD_CMGS", FmtTimeShow());
	}

	SetModemATPortStat(TRUE);
	// Cmd sending
	ATCommandSending();
	//Reset Check Modem Timeout Time
	// ResetCheckModemTimeoutTim();
}

static void MmiCIMI (char *MsgDataP)
{
	// u8 length = strlen(MsgDataP);
	// // IMSI replace MEID
	// #ifdef ATRSP_RESULT
	// if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
	// 	ParseatPrint(NRCMD,"\r\n[%s] PAT:+CIMI(%d|%s)", \
	// 		FmtTimeShow(), length, MsgDataP);
	// #endif
	// // IMSI - International Mobile Subscriber Identity
	// if(length == MAX_MEID_LEN)
	// {
	// 	u8 index;
	// 	u8 *imsiptr = gDeviceConfig.DeviceCfgInfo.Meid;
	// 	u8 *mlpwdptr = gDeviceConfig.DeviceCfgInfo.MLPWD;
	// 	// Get Meid
	// 	memset((char *)imsiptr, 0, sizeof(gDeviceConfig.DeviceCfgInfo.Meid));
	// 	// To lower all alphabet 
	// 	for( index = 0; index < (MAX_MEID_LEN); index ++ )
	// 	{
	// 		if( imsiptr[index] >= 'a' && imsiptr[index] <= 'z' )
	// 		{
	// 			imsiptr[index] = ( imsiptr[index] - 'a' + 'A' );
	// 		}
	// 	}
	// 	strcpy( (char *)imsiptr, MsgDataP);
	// 	// Set flag
	// 	gDevReadyRpt.MEIDReadyFlag = TRUE;
	// 	// Set AT Port Ready
	// 	SetModemATPortStat(TRUE);
	// 	// Calculate the password 
	// 	memset((char *)mlpwdptr, 0, MAX_MPLW_LEN);
	// 	Ds600_GetMLPW(mlpwdptr,MAX_MPLW_LEN);
	// 	#ifdef ATRSP_RESULT
	// 	if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
	// 		ParseatPrint(NRCMD,"\r\n[%s] PAT:IMSI(%s) MLPWD(%s)", \
	// 			FmtTimeShow(), imsiptr, mlpwdptr);
	// 	#endif
	// }
	// else
	// {
	// 	#ifdef ATRSP_RESULT
	// 	if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
	// 		ParseatPrint(NRCMD,"\r\n[%s] PAT:IMSI len error",FmtTimeShow());
	// 	#endif
	// }
}

// static void MmiCNUM (ATRspParmT* MsgDataP)
// {
// 	u8 length = strlen(MsgDataP[1].Str);
// 	u8 *mdnptr = gDeviceConfig.DeviceCfgInfo.MDN;
// 	// Subscriber Number (phone number)
// 	// +CNUM: "","+11000000102",145
// 	#ifdef ATRSP_RESULT
// 	if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
// 		ParseatPrint(NRCMD,"\r\n[%s] PAT:+CNUM(%d|%s %s %d)", \
// 			FmtTimeShow(), length, MsgDataP[0].Str, MsgDataP[1].Str, MsgDataP[2].Num);
// 	#endif

// 	if(length <= MAX_MDN_LEN)
// 	{
// 		// memset(mdnptr, 0, sizeof(gDeviceConfig.DeviceCfgInfo.MDN));
// 		// strcpy((char *)mdnptr, MsgDataP[0].Str);
// 		// #ifdef ATRSP_RESULT
// 		// if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
// 		// 	ParseatPrint(NRCMD,"\r\n[%s] PAT:mdn(%s)", \
// 		// 		FmtTimeShow(), mdnptr);
// 		// #endif
// 		// // Set flag
// 		// gDevReadyRpt.MDNReadyFlag = TRUE;
// 		// // Set AT Port Ready
// 		// SetModemATPortStat(TRUE);
// 		// // Reset Network Timer
// 		// SoftwareTimerReset(&NetworkCheckTimer,CheckNetlorkTimerCallback,CHECK_NETWORK_TIMEOUT);
// 		// SoftwareTimerStart(&NetworkCheckTimer);
// 	}
// 	else
// 	{
// 		#ifdef ATRSP_RESULT
// 		if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
// 			ParseatPrint(NRCMD,"\r\n[%s] PAT:MDN len error",FmtTimeShow());
// 		#endif
// 	}
// }

static void MmiCGSN (ATRspParmT* MsgDataP)
{
	u8 length = strlen(MsgDataP[0].Str);

	ParseatPrint(DbgCtl.ParseatCmdEn, "\r\n[%s] PAT:+CGSN(%d|%s)",
				 FmtTimeShow(), length, MsgDataP[0].Str);

	memset(IMEIBuf, 0, sizeof(IMEIBuf));
	memcpy(IMEIBuf, MsgDataP[0].Str, length);
}

// static void MmiCGSN (ATRspParmT* MsgDataP)
// {
// 	u8 length = strlen(MsgDataP[0].Str);
// 	u8 *msidptr = gDeviceConfig.DeviceCfgInfo.Msid;
// 	u8 *mlpwdptr = gDeviceConfig.DeviceCfgInfo.MLPWD;
// 	// GSN replace MSID
// 	#ifdef ATRSP_RESULT
// 	if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
// 		ParseatPrint(NRCMD,"\r\n[%s] PAT:+CGSN(%d|%s)", \
// 			FmtTimeShow(), length, MsgDataP[0].Str);
// 	#endif
	
// 	if(length <= MAX_MSID_LEN)
// 	{
// 		// Get msid
// 		memset( (char *)msidptr, 0, sizeof(gDeviceConfig.DeviceCfgInfo.Msid));
// 		strcpy((char *)msidptr, (char *)MsgDataP[0].Str);
// 		// Set flag
// 		gDevReadyRpt.MSIDReadyFlag = TRUE;
// 		// Set AT Port Ready
// 		SetModemATPortStat(TRUE);
// 		// Calculate the password 
// 		memset((char *)mlpwdptr, 0, MAX_MPLW_LEN);
// 		Ds600_GetMLPW(mlpwdptr,MAX_MPLW_LEN);
// 		#ifdef ATRSP_RESULT
// 		if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
// 			ParseatPrint(NRCMD,"\r\n[%s] PAT:MSID(%s) MLPWD(%s)", \
// 				FmtTimeShow(), msidptr, mlpwdptr);
// 		#endif
// 	}
// 	else
// 	{
// 		#ifdef ATRSP_RESULT
// 		if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
// 			ParseatPrint(NRCMD,"\r\n[%s] PAT:MSID len error",FmtTimeShow());
// 		#endif
// 	}
// }

static void MmiCCLK (ATRspParmT* MsgDataP)
{
	// CBP82
	// +CCLK:2015/7/14,14:25:54+32

	// LTE
	//+CCLK: "70/01/01,00:00:51+00"
	//+CCLK: "70/01/01,00:00:09+00"
	//+CCLK: "17/10/26,23:20:23-16"
	//E.g. 6th of May 1994, 22:10:00 GMT+2 hours equals to "94/05/06,22:10:00+08"
	TimeTableT timeTable;
	// #ifdef ATRSP_RESULT
	// if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
		ParseatPrint(DbgCtl.ParseatCmdEn,"\r\n[%s] PAT:+CCLK(%d-%d-%d %d:%d:%d)", \
			FmtTimeShow(), \
			MsgDataP[0].Num,MsgDataP[1].Num,MsgDataP[2].Num, \
			MsgDataP[3].Num,MsgDataP[4].Num,MsgDataP[5].Num);
	// #endif
	// Set AT Port Ready
	SetModemATPortStat(TRUE);
	if(MsgDataP[0].Num < 100)
		MsgDataP[0].Num+=2000;
	if((MsgDataP[0].Num > 2015 && MsgDataP[0].Num < 2099) && \
		(MsgDataP[1].Num >= 1 && MsgDataP[1].Num <= 12) && \
		(MsgDataP[2].Num >= 1 && MsgDataP[2].Num <= 31) && \
		(MsgDataP[3].Num <= 24) && \
		(MsgDataP[4].Num <= 59) && \
		(MsgDataP[5].Num <= 59))
	{
		uint32_t RTCTimeBase = *((uint32_t *)WedgeSysStateGet(WEDGE_BASE_RTC_TIME));
		// Init RTC Time
		timeTable.year 	= MsgDataP[0].Num;
		timeTable.month 	= MsgDataP[1].Num;
		timeTable.day 	= MsgDataP[2].Num;
		timeTable.hour 	= MsgDataP[3].Num;
		timeTable.minute 	= MsgDataP[4].Num;
		timeTable.second 	= MsgDataP[5].Num;

		if (HAL_OK != HAL_RTC_DeactivateAlarm(&hrtc, RTC_ALARM_A))
		{
			ParseatPrint(TRUE,"\r\n--->>PAT:+CCLK HAL_RTC_DeactivateAlarm Fail");
		}
		else
		{
			// HAL_RTC_DeInit(&hrtc);
			// MX_RTC_Init();
			SetRTCDatetime(&timeTable);
			ModemTimetalbeGet = timeTable;
			ModemTimeInSeconds = WedgeRtcCurrentSeconds();

			if (RTCTimeBase < ModemTimeInSeconds)
			{
				WedgeRtcTimerModifySettime((ModemTimeInSeconds - RTCTimeBase), WEDGE_RTC_TIMER_MODIFY_INCREASE);
				WedgeRtcTimerInstanceAlarmRefresh();
			}
			else
			{
				WedgeRtcTimerModifySettime((RTCTimeBase - ModemTimeInSeconds), WEDGE_RTC_TIMER_MODIFY_DECREASE);
				WedgeRtcTimerInstanceAlarmRefresh();
			}

			WedgeSysStateSet(WEDGE_BASE_RTC_TIME, &ModemTimeInSeconds);
		}
	}
	else
	{
		// #ifdef ATRSP_RESULT
		// if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
			ParseatPrint(DbgCtl.ParseatCmdEn,"\r\n[%s] PAT:Time Format Error", FmtTimeShow());
		// #endif
	}
	//Reset Check Modem Timeout Time
	// ResetCheckModemTimeoutTim();
}

static void MmiCTZR(ATRspParmT* MsgDataP)
{	
	// #ifdef ATRSP_RESULT
	// if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
		ParseatPrint(DbgCtl.ParseatCmdEn,"\r\n[%s] PAT:time zone(%d)",\
			FmtTimeShow(),MsgDataP[0].Num);
	// #endif

	ModemtimeZoneGet = MsgDataP[0].Num;
}

static void MmiCOPN(ATRspParmT* MsgDataP)
{
	// #ifdef ATRSP_RESULT
	// if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
		ParseatPrint(DbgCtl.ParseatCmdEn,"\r\n[%s] PAT:operator name (%s) (%s)",\
			FmtTimeShow( ),MsgDataP[0].Str,MsgDataP[1].Str);
	// #endif
}

static void MmiCGDCONT(ATRspParmT* MsgDataP)
{
	ParseatPrint(DbgCtl.ParseatCmdEn,"\r\n[%s] PAT:PDP context parameter values (%d) (%s) (%s)",\
			FmtTimeShow( ), MsgDataP[0].Num, MsgDataP[1].Str, MsgDataP[2].Str);
	if (MsgDataP[0].Num == TC30M_DEFAULT_CID)
	{
		memset(gModemParam.pdpType, 0, sizeof(gModemParam.pdpType));
		memset(gModemParam.apn_attach, 0, sizeof(gModemParam.apn_attach));
		memcpy(&(gModemParam.pdpType), MsgDataP[1].Str, strlen(MsgDataP[1].Str));
		memcpy(&(gModemParam.apn_attach), MsgDataP[2].Str, strlen(MsgDataP[2].Str));
	}
}

static void MmiCGCONTRDP(ATRspParmT* MsgDataP)
{
	ParseatPrint(DbgCtl.ParseatCmdEn,"\r\n[%s] PAT:CGCONTRDP cid(%d) local_addr.subnet_mask(%s)", FmtTimeShow( ), MsgDataP[0].Num, MsgDataP[3].Str);

	if (MsgDataP[0].Num == GetNetworkCidInUse())
	{
		uint8_t dotcount = 0, i = 0, j = 0;
		char *ptmp = MsgDataP[3].Str;
		uint8_t ipstrlen = strlen(MsgDataP[3].Str);

		for (j = 0; j < ipstrlen; j++)
		{
			if (*ptmp == '.')
			{
				dotcount++;
				if (dotcount > 7)
				{
					// Not ipv4
					return;
				}
			}
			ptmp++;
		}

		dotcount = 0;
		ptmp = MsgDataP[3].Str;
		memset(gModemParam.defaultcidipstr, 0, TC30M_DEFAULT_IPV4_MAX_LEN);
		memset(gModemParam.dns1str, 0, TC30M_DEFAULT_IPV4_MAX_LEN);
		memset(gModemParam.dns2str, 0, TC30M_DEFAULT_IPV4_MAX_LEN);
		gModemParam.defaultcidipgetflag = FALSE;
		gModemParam.defaultcidipnum = 0xfffffffful;
		for (i = 0; i < TC30M_DEFAULT_IPV4_MAX_LEN; i++)
		{
			if (*ptmp == '.')
			{
				dotcount++;
				if (dotcount == 4)
				{
					break;
				}
			}
			gModemParam.defaultcidipstr[i] = *ptmp;
			ptmp++;
		}
		gModemParam.defaultcidipnum = ipaddr_addr((char *)gModemParam.defaultcidipstr);
		memcpy(gModemParam.dns1str, MsgDataP[5].Str, strlen(MsgDataP[5].Str));
		memcpy(gModemParam.dns2str, MsgDataP[6].Str, strlen(MsgDataP[6].Str));
		gModemParam.defaultcidipgetflag = TRUE;
		ParseatPrint(1, "IP(%s)(0x%x) DNS1(%s) DNS2(%s)", gModemParam.defaultcidipstr, gModemParam.defaultcidipnum,
		gModemParam.dns1str, gModemParam.dns2str);
		ParseatPrint(DbgCtl.ParseatCmdEn,"\r\n[%s] PAT:CGCONTRDP CidInuse(0x%X)", FmtTimeShow( ), GetNetworkCidInUse());
	}
}


static void MmiCRSM(ATRspParmT* MsgDataP)
{	
	char *pstr = MsgDataP[2].Str;
	u8 index = 0;
	u8 temp;

	// #ifdef ATRSP_RESULT
	// if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
	// {
		ParseatPrint(DbgCtl.ParseatCmdEn,"\r\n[%s] PAT:sw1(%d),sw2(%d)",\
			FmtTimeShow(),MsgDataP[0].Num,MsgDataP[1].Num);
		ParseatPrint(DbgCtl.ParseatCmdEn,"\r\n[%s] PAT:resp(%s)",\
			FmtTimeShow(),MsgDataP[2].Str);
	// }
	// #endif
	
	//eg: 89148 00000 17036 82921,20 bytes
	while(index < 20)
	{
		temp = pstr[index];
		pstr[index] = pstr[index + 1];
		pstr[index + 1] = temp;
		index += 2;
	}
	pstr[20] = '\0';	

	// #ifdef ATRSP_RESULT
	// if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
		ParseatPrint(NRCMD,"\r\n[%s] PAT:ICCID(%s)",FmtTimeShow(),pstr);
	// #endif

	memset(ICCIDBuf, 0, sizeof(ICCIDBuf));
	memcpy(ICCIDBuf,pstr,21);
}

static void MmiCMGS(ATRspParmT *MsgDataP)
{
	ParseatPrint(DbgCtl.ParseatCmdEn, "\r\n[%s] PAT:+CMGS(%d) send sms result",
				 FmtTimeShow(), MsgDataP[0].Num);
	SetSMSNeedResponseStat(FALSE);
}

static void MmiCMTI(ATRspParmT* MsgDataP)
{
	u8 IdxSting[8];
	// Unsolicited Result Code
	// +CMTI:"RAM",0
	memset(IdxSting,0, sizeof(IdxSting));
	sprintf((char *)IdxSting, "%d",MsgDataP[1].Num);
	smsindex = 0;
	smsindex = MsgDataP[1].Num;

	// #ifdef ATRSP_RESULT
	// if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
		ParseatPrint(DbgCtl.ParseatCmdEn,"\r\n[%s] PAT:+CMTI(%s %d) idx(%s)", \
			FmtTimeShow(),MsgDataP[0].Str,MsgDataP[1].Num, IdxSting);
	// #endif

	// Read new message with index
	// SendATCmd(GSM_CMD_CMGR, GSM_CMD_TYPE_EVALUATE , "1");
	SendATCmd(GSM_CMD_CMGR, GSM_CMD_TYPE_EVALUATE , IdxSting);
}

static void MmiCMGR(ATRspParmT* MsgDataP)
{
	SmsRecFlag = 1;
	memset(&SmsReceiveBuf, 0, sizeof(SmsReceiveBuf));

	SmsReceiveBuf.smsindex = smsindex;
	strcpy(SmsReceiveBuf.smsstat, (char *)MsgDataP[0].Str);
	strcpy(SmsReceiveBuf.smsnumber, (char *)MsgDataP[1].Str);
	strcpy(SmsReceiveBuf.smsdatetime, (char *)MsgDataP[3].Str);

	// char *numberp = gDeviceConfig.SmsNum.Number;
	
	// +CMGR: "REC UNREAD","11000000102",,"17/10/26,05:05:48+08"
	// Short message content
	// OK
	
	// Clear old number
	// gDeviceConfig.SmsNum.Numlen = 0;
	// memset( (char *)numberp, 0, sizeof(gDeviceConfig.SmsNum.Number) );
	// strcpy((char *)numberp, (char *)MsgDataP[1].Str);
	// Set flag
	// IsCmgrSmsCommingType = TRUE;
	// #ifdef ATRSP_RESULT
	// if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
		// ParseatPrint(NRCMD,"\r\n[%s] PAT:+CMGR(%s %s %s %s) Num(%s)read sms",\
		// 	FmtTimeShow( ), \
		// 	MsgDataP[0].Str,MsgDataP[1].Str, \
		// 	MsgDataP[2].Str,MsgDataP[3].Str, numberp);
	// #endif
}

static void MmiCPIN(ATRspParmT* MsgDataP)
{
	// #ifdef ATRSP_RESULT
	// if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
		ParseatPrint(NRCMD,"\r\n[%s] PAT:+CPIN(%s)",\
			FmtTimeShow( ),MsgDataP[0].Str);
	// #endif

	StringToUper(MsgDataP[0].Str);
	
	if(strcmp("READY", (char*)MsgDataP[0].Str) == 0)
	{
		SetSimCardReadyStat(TRUE);
	}
}

static void MmiCGACT(ATRspParmT* MsgDataP)
{
	ParseatPrint(NRCMD,"\r\n[%s] PAT:+CGACT Cid(%d) State(%d)",\
			FmtTimeShow(), MsgDataP[0].Num, MsgDataP[1].Num);

	if (MsgDataP[1].Num != FALSE)
	{
		char strbuf[16];
		memset(strbuf, 0, sizeof(strbuf));

		SetNetworkCidInUse(MsgDataP[0].Num);

		sprintf(strbuf, "%d", GetNetworkCidInUse());
		SendATCmd(GSM_CMD_CGCONTRDP, GSM_CMD_TYPE_EVALUATE, (uint8_t *)strbuf);
	}
}

// static void MmiSQNSI(char* MsgDataP)
// {
// 	// u16	idx = 0;
// 	// u16	param = 0;
// 	// char	 *tmpP;
// 	// u16 tmpLen = 0;
// 	// char	tmpBuf[MAX_IP_ADDR_LEN];
// 	// uint8 Socket_Num = 0;
// 	// uint16 sent = 0;
// 	// uint16 received = 0;
// 	// uint16 buff_in = 0;
// 	// uint16 ack_waiting = 0;

// 	// /* +SQNSI:<connId>,<sent>,<received>,<buff_in>,<ack_waiting>
// 	// 	connId
// 	// 		Integer in range [1-6]. Socket connection identifier.
// 	// 	sent
// 	// 		Total amount (in bytes) of sent data since the last time the socket connection
// 	// 		identified by <connId> has been opened.
// 	// 	received
// 	// 		Total amount (in bytes) of received data since the last time the socket
// 	// 		connection identified by <connId> has been opened.
// 	// 	buff_in
// 	// 		Total amount (in bytes) of data just arrived through the socket connection
// 	// 		identified by <connId> and currently buffered, not yet read.
// 	// 	ack_waiting
// 	// 		Total amount (in bytes) of sent and not yet acknowledged data since the
// 	// 		last time the socket connection identified by <connId> has been opened.*/
// 	// /* Example:
// 	// +SQNSI: 1,0,0,0,0
// 	// +SQNSI: 2,0,0,0,0
// 	// +SQNSI: 3,0,0,0,0
// 	// +SQNSI: 4,0,0,0,0
// 	// +SQNSI: 5,0,0,0,0
// 	// +SQNSI: 6,0,0,0,0*/
	
// 	// /*ParseatPrint(NRCMD,"\r\n[%s] PAT:+SQNSI(%s)",\
// 	// 	FmtTimeShow( ),MsgDataP);*/

// 	// // Parsing Data
// 	// tmpP = strstr((char*)MsgDataP,(char*)"+SQNSI:");
// 	// if( tmpP != NULL)
// 	// {
// 	// 	tmpP += 7;
// 	// 	param = 0;
// 	// 	tmpLen = 0;
// 	// 	memset( (char *)tmpBuf, 0, sizeof(tmpBuf));
// 	// 	for( idx = 0; idx < SQNSI_PARSE_LENGTH; idx++ )
// 	// 	{
// 	// 		if( tmpP[idx] == 0x0D || tmpP[idx] == 0x0A )
// 	// 		{
// 	// 			break;
// 	// 		}
// 	// 		else if( tmpP[idx] == ',' || tmpP[idx] == '\0')
// 	// 		{
// 	// 			param++;
// 	// 			if(param == 1)
// 	// 			{
// 	// 				if(tmpLen > 0)// <Socket ID>
// 	// 				{
// 	// 					Socket_Num = atoi((const char*)tmpBuf);
// 	// 				}
// 	// 			}
// 	// 			else if(param == 2)// <sent>
// 	// 			{
// 	// 				if(tmpLen > 0)
// 	// 				{
// 	// 					sent = atoi((const char*)tmpBuf);
// 	// 				}
// 	// 			}
// 	// 			else if(param == 3)// <received>
// 	// 			{
// 	// 				if(tmpLen > 0)
// 	// 				{
// 	// 					received = atoi((const char*)tmpBuf);
// 	// 				}
// 	// 			}
// 	// 			else if(param == 4)	// <buff_in>
// 	// 			{
// 	// 				if(tmpLen > 0)
// 	// 				{
// 	// 					buff_in = atoi((const char*)tmpBuf);
// 	// 				}
// 	// 			}
// 	// 			else if(param == 5)// <ack_waiting>
// 	// 			{
// 	// 				if(tmpLen > 0)
// 	// 				{
// 	// 					ack_waiting = atoi((const char*)tmpBuf);
// 	// 				}
// 	// 			}
// 	// 			// Check End
// 	// 			if( tmpP[idx] == '\0')
// 	// 			{
// 	// 				break;
// 	// 			}
				
// 	// 			tmpLen = 0;
// 	// 			memset( (char *)tmpBuf, 0, sizeof(tmpBuf));
// 	// 		}
// 	// 		else if( tmpP[idx] == ' ') 
// 	// 		{
// 	// 			// Skip Spaces
// 	// 		}
// 	// 		else
// 	// 		{
// 	// 			if(param == 0) // <Socket ID>
// 	// 			{
// 	// 				if( IS_NUMBER_ONLY(tmpP[idx]) )
// 	// 				{
// 	// 					tmpBuf[tmpLen++] = tmpP[idx];
// 	// 				}
// 	// 			}
// 	// 			else if(param == 1)	// <sent>
// 	// 			{
// 	// 				if( IS_NUMBER_ONLY(tmpP[idx]) )
// 	// 				{
// 	// 					tmpBuf[tmpLen++] = tmpP[idx];
// 	// 				}
// 	// 			}
// 	// 			else if(param == 2)// <received>
// 	// 			{
// 	// 				if( IS_NUMBER_ONLY(tmpP[idx]) )
// 	// 				{
// 	// 					tmpBuf[tmpLen++] = tmpP[idx];
// 	// 				}
// 	// 			}
// 	// 			else if(param == 3)	// <buff_in>
// 	// 			{
// 	// 				if( IS_IP_ADRESS(tmpP[idx]) )
// 	// 				{
// 	// 					tmpBuf[tmpLen++] = tmpP[idx];
// 	// 				}
// 	// 			}
// 	// 			else if(param == 4)	// <ack_waiting>
// 	// 			{
// 	// 				if( IS_NUMBER_ONLY(tmpP[idx]) )
// 	// 				{
// 	// 					tmpBuf[tmpLen++] = tmpP[idx];
// 	// 				}
// 	// 			}
// 	// 		}
// 	// 	}
// 	// 	// Socket Status
// 	// 	//atel_tcpip_socketAmount_Handle(Socket_Num, sent, received, buff_in, ack_waiting);

// 	// 	// #ifdef ATRSP_RESULT
// 	// 	// if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
// 	// 	// 	ParseatPrint(NRCMD,"\r\n[%s] APP: SOCK AMOUNT: ID(%d) SEND(%d) REVC(%d) BUFFIN(%d) ACKWAIT(%d)",\
// 	// 	// 		FmtTimeShow( ),Socket_Num,sent,received,buff_in,ack_waiting);
// 	// 	// #endif
// 	// }
// }

static void MmiSQNSS(ATRspParmT* MsgDataP)
{
	/*+SQNSS:<connId>, <state>, <locIP>, <locPort>, <remIP>, <remPort>
	connId  	->Integer in range [1-6]. Socket connection identifier.
	state    	->Integer in range [0-6]. Current state of the socket.
	locIP    	->IP address associated by the context activation to the socket.
	locPort 	->One of the following:
			     The listening port if the socket is in listen mode.
		 	     The local port for the connection if the socket is connected to a remotemachine.
	remIP   	->Remote IP address when the device is connected to a remote machine.
	remPort  	->Connected port on the remote machine.*/

	/* Example:
	+SQNSS: 1,0
	+SQNSS: 2,2,"192.168.6.2",5555,"180.166.175.194",30004
	+SQNSS: 3,2,"192.168.6.2",5555,"192.168.6.1",9999
	+SQNSS: 4,0
	+SQNSS: 5,0
	+SQNSS: 6,0*/
	
	ParseatPrint(DbgCtl.ParseatCmdEn,"\r\n[%s] PAT:+SQNSS(%d %d %s %d %s %d)",\
		FmtTimeShow( ), \
		MsgDataP[0].Num,MsgDataP[1].Num,MsgDataP[2].Str,\
		MsgDataP[3].Num,MsgDataP[4].Str,MsgDataP[5].Num);


	if((0x01 << (MsgDataP[0].Num - 1)) & UdpSocketListenIndicateFlag)
	{
		if(MsgDataP[1].Num == 0)
		{
			UDPIPSocket[MsgDataP[0].Num - 1].status = 0;
		}
		else
		{
			UDPIPSocket[MsgDataP[0].Num - 1].status = MsgDataP[1].Num;
		}

		if (UDPIPSocket[MsgDataP[0].Num - 1].operation == SOCKETOPEN_REPORTONCE)
		{
			UDPIPSocket[MsgDataP[0].Num - 1].operation = SOCKETOPEN;
		}
				
		// Reset Network Timer
		SoftwareTimerReset(&NetworkCheckTimer,CheckNetlorkTimerCallback,10);
		SoftwareTimerStart(&NetworkCheckTimer);
	}

}

static void MmiSQNSH(ATRspParmT* MsgDataP)
{
	ParseatPrint(DbgCtl.ParseatCmdEn,"\r\n[%s] PAT:+SQNSH(%d)",\
		FmtTimeShow( ), MsgDataP[0].Num);


	if((0x01 << (MsgDataP[0].Num - 1)) & UdpSocketListenIndicateFlag)
	{
		UDPIPSocket[MsgDataP[0].Num - 1].status = 0;
	}
}

// static void MmiSQNSRECV(ATRspParmT* MsgDataP)
// {
// 	// #ifdef ATRSP_RESULT
// 	// if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
// 		ParseatPrint(DbgCtl.ParseatCmdEn,"\r\n[%s] PAT:+SQNSRECV(%d %d)",\
// 			FmtTimeShow( ),MsgDataP[0].Num,MsgDataP[2].Num);
// 	// #endif
// }

static void MmiSQNSRING(char* MsgDataP)
{
	char *tmp;

	ParseatPrint(DbgCtl.ParseatCmdEn, "\r\n[%s] PAT:%s UdpSocketListenIndicateFlag(0x%X)", FmtTimeShow(), MsgDataP, UdpSocketListenIndicateFlag);

	// +SQNSRING: 3,12
	tmp = strstr((char*)MsgDataP,(char*)"+SQNSRING:");
	if( tmp != NULL)
	{
		uint8_t socketnum = 0;
		uint16_t socketlen = 0;
		uint8_t lenstr[4] = {0};
		uint8_t i = 0, j = 0;
		tmp += 10;

		tmp = strstr((const char *)tmp, ",");
		socketnum = *(tmp - 1) - '0';

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
		tmp--;

		for (i = 0; i < j; i++)
		{
			socketlen = socketlen * 10 + lenstr[i];
		}

		// Check Socket
		if((0x01 << (socketnum - 1)) & UdpSocketListenIndicateFlag)
		{
			char rec[32];

			ParseatPrint(DbgCtl.ParseatCmdEn,"\r\nSQNSRING TO SQNSRECV: ID(%d) Len(%d)", socketnum, socketlen);

			memset(rec, 0, sizeof(rec));
			sprintf((char *)rec, "%d,%d", socketnum, socketlen);

			SendATCmd(GSM_CMD_SQNSRECV, GSM_CMD_TYPE_EVALUATE, (uint8_t *)rec);
		}
	}
}

// static void MmiSQNSRING(char* MsgDataP)
// {	
// 	u8	idx = 0;
// 	u8	param = 0;
// 	char	 *tmpP;
// 	u16 tmpLen = 0;
// 	char	tmpBuf[20];
// 	u16 SockID = 0;
// 	u16 SockLen = 0;
// 	//if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
// 		ParseatPrint(NRCMD,"\r\n[%s] PAT:%s", FmtTimeShow(),MsgDataP);
// 	// +SQNSRING: 3,12,111111111111
// 	tmpP = strstr((char*)MsgDataP,(char*)"+SQNSRING:");
// 	if( tmpP != NULL)
// 	{
// 		tmpP += 10;
// 		param = 0;
// 		tmpLen = 0;
// 		memset( (char *)tmpBuf, 0, sizeof(tmpBuf));
// 		for( idx = 0; idx < IPRECV_PARSE_LENGTH; idx++ )
// 		{
// 			if( tmpP[idx] == '\0' || tmpP[idx] == ';' || tmpP[idx] == '}' )
// 			{
// 				break;
// 			}
// 			else if( tmpP[idx] == ',' )
// 			{
// 				param++;
// 				if(param == 1) // <Socket ID>
// 				{
// 					if(tmpLen > 0)
// 					{
// 						SockID = atoi((const char*)tmpBuf);
// 					}
// 				}
// 				else if(param == 2) // <Length>
// 				{
// 					if(tmpLen > 0)
// 					{
// 						SockLen = atoi((const char*)tmpBuf);
// 					}
// 					break;
// 				}
// 				tmpLen = 0;
// 				memset( (char *)tmpBuf, 0, sizeof(tmpBuf));
// 			}
// 			else if( tmpP[idx] == ' ') 
// 			{
// 				// Skip Spaces
// 			}
// 			else
// 			{
// 				if(param == 0) // <Socket ID>
// 				{
// 					if( IS_NUMBER_ONLY(tmpP[idx]) )
// 					{
// 						tmpBuf[tmpLen++] = tmpP[idx];
// 					}
// 				}
// 				else if(param == 1) // <Length>
// 				{
// 					if( IS_NUMBER_ONLY(tmpP[idx]) )
// 					{
// 						tmpBuf[tmpLen++] = tmpP[idx];
// 					}
// 				}
// 			}
// 		}
// 		tmpP += idx;
// 		tmpP++;// just for ','
// 		// if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
// 		// {
// 			ParseatPrint(NRCMD,"\r\n[%s] PAT:+SQNSRING:ID(%d) Len(%d) idx(%d)", \
// 				FmtTimeShow(),SockID,SockLen,idx);
// 			ParseatPrint(NRCMD,"\r\n[%s] PAT:+SQNSRING:Buf(%s)", FmtTimeShow(),tmpP);
// 		// }
// 		// Check Socket
// 		if((0x01 << (SockID - 1)) & UdpSocketListenIndicateFlag)
// 		{
// 			char rec[32];

// 			ParseatPrint(NRCMD,"\r\nSQNSRING Rec");

// 			memset(rec, 0, sizeof(rec));
// 			sprintf((char *)rec, "%d,%d", SockID, SockLen);

// 			SendATCmd(GSM_CMD_SQNSRECV, GSM_CMD_TYPE_EVALUATE, rec);
// 		}
// 	}
// 	// Check status
// 	// if(GetLogSavingUsableStat() == TRUE)
// 	// {
// 	// 	// Log saving
// 	// 	SaveSystemLogToMemory(LOG_SAVE_UDP_IN, (u8*)MsgDataP);
// 	// }
// }

static void MmiSQNHTTPRING(ATRspParmT* MsgDataP)
{
	//+SQNHTTPRING: 1,200,"text/html; charset=iso-8859-1",5223
	// #ifdef ATRSP_RESULT
	// if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
		ParseatPrint(DbgCtl.ParseatCmdEn,"\r\n[%s] PAT:+SQNHTTPRING(%d %d %s %d)",\
			FmtTimeShow( ),MsgDataP[0].Num,MsgDataP[1].Num,MsgDataP[2].Str,MsgDataP[3].Num);
	// #endif		

	// http_status_code = (HttpStatusCode)MsgDataP[1].Num;
}

static void MmiCMSERROR(ATRspParmT* MsgDataP)
{
	// +CMS ERROR:513
	// lower layer error
	ParseatPrint(NRCMD, "\r\n[%s] PAT:+CMS ERROR(%d)",
				 FmtTimeShow(), MsgDataP[0].Num);
	if (MsgDataP[0].Num == 513)
	{
		ParseatPrint(NRCMD, "\r\n[%s] PAT:SMS lower layer error",
					 FmtTimeShow());
	}
	SetSMSNeedResponseStat(FALSE);
}

static void MmiCMEERROR (ATRspParmT* MsgDataP)
{
	// #ifdef ATRSP_RESULT
	// if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
	// 	ParseatPrint(NRCMD,"\r\n[%s] PAT:+CME ERR(%d) NetCtl(%d %d)", \
	// 		FmtTimeShow(),\
	// 		MsgDataP[0].Num, \
	// 		NetworkSession.benable, \
	// 		gDeviceConfig.NetCarrierCtl.KeepNetAliveForever);
	// #endif
	// Set AT Port Ready
	SetModemATPortStat(TRUE);
	switch(MsgDataP[0].Num)
	{
		case CME_NET_ENOTCONN:	// 815
		// #ifdef ATRSP_RESULT
		// if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
		// 	ParseatPrint(NRCMD,"\r\n[%s] PAT:Socket not connected", FmtTimeShow());
		// #endif
		// if((NetworkSession.benable == TRUE) || \
		// 	(gDeviceConfig.NetCarrierCtl.KeepNetAliveForever == TRUE))
		// {
		// 	SetNetworkMachineStatus(NET_SOCKET_DISCONNECT_STAT);
		// 	// Reset Network Timer
		// 	SoftwareTimerReset(&NetworkCheckTimer,CheckNetlorkTimerCallback,10);
		// 	SoftwareTimerStart(&NetworkCheckTimer);
		// }
		break;

		case CME_NET_EISCONN:	// 813
		// #ifdef ATRSP_RESULT
		// if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
		// 	ParseatPrint(NRCMD,"\r\n[%s] PAT:Connection already established",FmtTimeShow());
		// #endif
		// if((NetworkSession.benable == TRUE) || \
		// 	(gDeviceConfig.NetCarrierCtl.KeepNetAliveForever == TRUE))
		// {
		// 	// Get PPP Connect Status
		// 	SendATCmd(GSM_CMD_SQNSS, GSM_CMD_TYPE_EXECUTE, NULL);
		// 	// Reset Network Timer
		// 	SoftwareTimerReset(&NetworkCheckTimer,CheckNetlorkTimerCallback,10);
		// 	SoftwareTimerStart(&NetworkCheckTimer);
		// }
		break;

		case CME_NET_EADDRREQ:	// 811
		// #ifdef ATRSP_RESULT
		// if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
		// 	ParseatPrint(NRCMD,"\r\n[%s] PAT:Destination address required", FmtTimeShow());
		// #endif
		{
			/* Disable Modem Power Supply */
			//LTEPowerEnControl(DISABLE, 26);

			//HAL_Delay(100);
			
		 	// Modem Power Supply
			//LTEPowerEnControl(ENABLE, 27);
		}
		break;

		case CME_DNS_LOOKUP:	// 3
		// #ifdef ATRSP_RESULT
		// if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
		// 	ParseatPrint(NRCMD,"\r\n[%s] PAT:DNS Lookup Error", FmtTimeShow());
		// #endif
		{

		}
		break;

		case CME_DNS_NOTSUPPORT:
		{
			if(GetFwpCurrentIndex() == GSM_CMD_SQNSDIAL)
			{
				// #ifdef ATRSP_RESULT
				// if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
				// 	ParseatPrint(NRCMD,"\r\n[%s] PAT: Cmd not support, send next", FmtTimeShow());
				// #endif
				// Cmd sending
				ATCommandSending();
			}
		}
		break;

		// case AT_AUDIO_FAIL_ERR:	// 75
		// case AT_AUDIO_WRONG_FORMAT_ERR:
		// case AT_AUDIO_NOT_SUPPORTED_ERR:
		// case AT_AUDIO_CONFLICT_ERR:
		// case AT_AUDIO_NOT_ACTIVE:
		// break;

		case CME_NET_ECONNABORTED:
		break;

		default:
		break;
	}

	if((GetUDPDataCanSendStat() == FALSE) /*&& (GetFwpCurrentIndex() == GSM_CMD_CMGS || GetFwpCurrentIndex() == GSM_CMD_SQNSSEND)*/)
	{
		// #ifdef ATRSP_RESULT
		// if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
		// 	ParseatPrint(NRCMD,"\r\n[%s] PAT:Err delete queue element %d", \
		// 		FmtTimeShow(),GetFwpCurrentIndex());
		// #endif
		// Delete queue element
		DeleteQueueElement();
	}
	// Set AT Port Ready
	SetModemATPortStat(TRUE);
} 

static void MmiSQNVERSTR1(ATRspParmT* MsgDataP)
{
	// #ifdef ATRSP_RESULT
	// if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
		ParseatPrint(DbgCtl.ParseatCmdEn,"\r\n[%s] PAT:Version UE(%s)", \
			FmtTimeShow(),MsgDataP[0].Str);
	// #endif
}

static void MmiSQNVERSTR2(ATRspParmT* MsgDataP)
{
	u8 length;
	u8 *VerisonP = gModemParam.Version;
	// #ifdef ATRSP_RESULT
	// if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
		ParseatPrint(DbgCtl.ParseatCmdEn,"\r\n[%s] PAT:Version LR(%s)", \
			FmtTimeShow(),MsgDataP[0].Str);
	// #endif
	length = 0;
	memset( (char *)VerisonP, 0, sizeof(VerisonP) );
	// Fill head
	sprintf((char *)&VerisonP[length], "LR");
	length= strlen((char *)VerisonP );
	// Fill version
	if(strlen((char *)MsgDataP[0].Str) < MAX_VERSION_LEN)
	{
		sprintf((char *)&VerisonP[length], (char *)MsgDataP[0].Str);
		length= strlen((char *)VerisonP );
	}
	// #ifdef ATRSP_RESULT
	// if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
		ParseatPrint(DbgCtl.ParseatCmdEn,"\r\n[%s] PAT:Version(%s)",FmtTimeShow(),VerisonP);
	// #endif
}

// static void MmiHCMGSS(ATRspParmT* MsgDataP)
// {
// 	// ^HCMGSS: 1
// 	// Short SMS Send Successfull
// 	#ifdef ATRSP_RESULT
// 	if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
// 		ParseatPrint(NRCMD,"\r\n[%s] PAT:^HCMGSS(%d)", \
// 			FmtTimeShow(),MsgDataP[0].Num);
// 	#endif
// 	if(MsgDataP[0].Num == 1)
// 	{
// 		#ifdef ATRSP_RESULT
// 		if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
// 			ParseatPrint(NRCMD,"\r\n[%s] PAT:Send SMS Successfull", \
// 				FmtTimeShow());
// 		#endif
// 	}
// }

// static void MmiPPPCONIP(char* MsgDataP)
// {
// 	char	 *tmpP;
// 	// Succ:	^PPPCONIP:<IP address>
// 	#ifdef ATRSP_RESULT
// 	if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
// 		ParseatPrint(NRCMD,"\r\n[%s] PAT:^PPPCONIP(%s)", \
// 			FmtTimeShow(),MsgDataP);
// 	#endif
// 	tmpP = strstr((char*)MsgDataP,(char*)"^PPPCONIP:");
// 	if( tmpP != NULL)
// 	{
// 		tmpP += 10;
// 		if(strlen(tmpP) > 3 && CheckIpAddress(tmpP) == TRUE)
// 		{
// 			memset( (char *)gModemParam.SourceAddr, 0, sizeof(gModemParam.SourceAddr) );
// 			strcpy( (char *)gModemParam.SourceAddr, tmpP );
// 			#ifdef ATRSP_RESULT
// 			if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
// 				ParseatPrint(NRCMD,"\r\n[%s] PAT:Addr(%s) NetCtl(%d %d)", \
// 					FmtTimeShow(),gModemParam.SourceAddr,\
// 					NetworkSession.benable, \
// 					gDeviceConfig.NetCarrierCtl.KeepNetAliveForever);
// 			#endif
// 			if((NetworkSession.benable == TRUE) || \
// 				(gDeviceConfig.NetCarrierCtl.KeepNetAliveForever == TRUE))
// 			{
// 				//SetNetworkMachineStatus(NET_PPP_CONNECTED_STAT);
// 				// Reset Network Timer
// 				// SoftwareTimerReset(&NetworkCheckTimer,CheckNetlorkTimerCallback,10);
// 				// SoftwareTimerStart(&NetworkCheckTimer);
// 			}
// 		}
// 	}
// }

// static void MmiSHORTSMS(char* MsgDataP)
// {
// 	char	 *NumP;
// 	char	 *CmdP;
// 	u8 idx = 0;
// 	u8 Numlen = 0;
// 	// +SMS:18918088477,test message
// 	#ifdef ATRSP_RESULT
// 	if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
// 		ParseatPrint(NRCMD,"\r\n[%s] PAT:+SMS Parsing", FmtTimeShow());
// 	#endif
// 	NumP = strstr((char*)MsgDataP,(char*)"+SMS:");
// 	if( NumP != NULL)
// 	{
// 		NumP += 5;
// 		CmdP = strstr((char*)MsgDataP,(char*)",");
// 		if( CmdP != NULL)
// 		{
// 			Numlen = CmdP - NumP;
// 			CmdP += 1;
// 			if(Numlen >= 3)
// 			{
// 				// Clear old number
// 				gDeviceConfig.SmsNum.Numlen = 0;
// 				memset( (char *)gDeviceConfig.SmsNum.Number, 0, sizeof(gDeviceConfig.SmsNum.Number) );
// 				// Get new number
// 				for( idx = 0; idx < Numlen && idx < MAX_SMS_NUMBER_LEN; idx++ )
// 				{
// 					if( NumP[idx] == ',' ||NumP[idx] == '\0')
// 					{
// 						break;
// 					}
// 					else
// 					{
// 						if( IS_SMSNUMBER_OR_PLUS(NumP[idx]) )
// 						{
// 							gDeviceConfig.SmsNum.Number[gDeviceConfig.SmsNum.Numlen++] = NumP[idx];
// 						}
// 						else
// 						{
// 							break;
// 						}
// 					}
// 				}
// 				Ds600_ServiceStrHandle(CmdP);
// 			}
// 		}
// 	}

// }

// static void MmiTFTPSTATUS(ATRspParmT* MsgDataP)
// {
// 	/*+TFTPSTATUS:0		+TFTPSTATUS��status
// 	typedef enum {
// 	TFTP_SUCCESS = 0,
// 	// BOD Note: we do not detect this error and instead we timeout
// 	TFTP_ERR_FILE_NOT_FOUND = 1,  
// 	// BOD Note: there is currently no check on write
// 	TFTP_ERR_DISK_FULL = 3,             
// 	TFTP_ERR_PACKET_DATA = 4,
// 	// BOD Note: not reported
// 	TFTP_ERR_UNKNOWN_TID = 5,      
// 	// BOD Note: do we get this error when the file already exist?
// 	TFTP_ERR_CREATE_FILE = 6,         
// 	// BOD Note: not detected. at_startTftp() 
// 	// returns but nothing is reported
// 	TFTP_ERR_BUSY = 8,                     
// 	TFTP_ERR_PPP_CONNECT = 11,
// 	TFTP_ERR_SOCKET_CREATE = 11,
// 	TFTP_ERR_PACKET_TIMEOUT = 12,
// 	}tftp_download_result;*/
// 	// Print Out
// 	#ifdef ATRSP_RESULT
// 	if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
// 		ParseatPrint(NRCMD,"\r\n[%s] PAT:+TFTPSTATUS(%d)", \
// 			FmtTimeShow(),MsgDataP[0].Num);
// 	#endif
// 	// Fill TFTF Status
// 	gOTAReprogram.StartReprogram = FALSE;
// 	gOTAReprogram.FinishedReprogram = TRUE;
// 	switch((TFTFActResT)MsgDataP[0].Num)
// 	{
// 		case TFTP_SUCCESS:
// 		{
// 			// DS600OTAReprogramReport(OTA_REPG_WRITING_FLASH);
// 		    //     // Start Process
// 		    //     if(gDeviceConfig.Acktype == SER_ACK_UDPIP_MODE)
// 		    //     {
// 		    //     	// Set Sleep Flag
// 			// 	SetSystemEventMode(SYS_EVENT_DEBUG_MODE_WAKEUP_MSK);
// 			// 	// Start Timer
// 			// 	SoftwareTimerReset(&StartDnldProcessTimer,StartDnldProcessTimerCallback,OTA_REPROGRAM_ACT_TIMEOUT);
// 			// 	SoftwareTimerStart(&StartDnldProcessTimer);
// 		    //     }
// 		    //     else
// 		    //     {
// 		    //     	// Set Sleep Flag
// 			// 	SetSystemEventMode(SYS_EVENT_DEBUG_MODE_WAKEUP_MSK);
// 			// 	// Start Timer
// 			// 	SoftwareTimerReset(&StartDnldProcessTimer,StartDnldProcessTimerCallback,(OTA_REPROGRAM_ACT_TIMEOUT/10));
// 			// 	SoftwareTimerStart(&StartDnldProcessTimer);
// 		    //     }
// 		}
// 		break;

// 		case TFTP_ERR_FILE_NOT_FOUND:
// 		{
// 			DS600OTAReprogramReport(OTA_REPG_FILE_NOT_FOUND);
// 		}
// 		break;
		
// 		case TFTP_ERR_DISK_FULL:
// 		{
// 			DS600OTAReprogramReport(OTA_REPG_DISK_FULL);
// 		}
// 		break;
		
// 		case TFTP_ERR_PACKET_DATA:
// 		{
// 			DS600OTAReprogramReport(OTA_REPG_BAD_IMAGE);
// 		}
// 		break;
		
// 		case TFTP_ERR_UNKNOWN_TID:
// 		{
// 			DS600OTAReprogramReport(OTA_REPG_NOT_REPORTED);
// 		}
// 		break;
		
// 		case TFTP_ERR_CREATE_FILE:
// 		{
// 			DS600OTAReprogramReport(OTA_REPG_CREATE_FILE);
// 		}
// 		break;
		
// 		case TFTP_ERR_BUSY:
// 		{
// 			DS600OTAReprogramReport(OTA_REPG_SYSTEM_BUSY);
// 		}
// 		break;
		
// 		case TFTP_ERR_PPP_CONNECT:
// 		{
// 			DS600OTAReprogramReport(OTA_REPG_PPP_CONNECT);
// 		}
// 		break;

// 		case TFTP_ERR_SOCKET_CREATE:
// 		{
// 			DS600OTAReprogramReport(OTA_REPG_SOCKET_CREATE);
// 		}
// 		break;

// 		case TFTP_ERR_PACKET_TIMEOUT:
// 		{
// 			DS600OTAReprogramReport(OTA_REPG_PACKET_TIMEOUT);
// 		}
// 		break;		
		
// 		default:
// 		break;
// 	}
// }

// static void MmiDNLD(ATRspParmT* MsgDataP)
// {
// 	/*+MCU:OK*/
// 	// Print Out
// 	#ifdef ATRSP_RESULT
// 	if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
// 	{
// 		ParseatPrint(NRCMD,"\r\n[%s] PAT:+MCU(%s)", \
// 			FmtTimeShow(),MsgDataP[0].Str);
// 	}
// 	#endif
// 	// Flash OP
// 	WriteIAPFlagToFlashMemory(IAP_BOOT_CDMA);
// 	// Refresh IWDG: reload counter
// 	TickHardwareWatchdog(TRUE);
// 	// Jumping
// 	SoftwareJumping(JUMP_TO_BOOTLOADER_ADDRESS);
// }

// static void MmiVLOCINFO(ATRspParmT* MsgDataP)
// {
// 	// +VLOCINFO:6,460,3,13840,15,8958,449104,1750744
// 	gModemParam.PRevInUse = MsgDataP[0].Num;
// 	gModemParam.MCC = MsgDataP[1].Num;
// 	gModemParam.IMSI_11_12 = MsgDataP[2].Num;
// 	gModemParam.sid = MsgDataP[3].Num;
// 	gModemParam.nid = MsgDataP[4].Num;
// 	gModemParam.baseId = MsgDataP[5].Num;
// 	gModemParam.baseLat = MsgDataP[6].Num;
// 	gModemParam.baseLong = MsgDataP[7].Num;

// 	#ifdef ATRSP_RESULT
// 	if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
// 		ParseatPrint(NRCMD,"\r\n[%s] PAT:+VLOCINFO(%d %d %d %d %d %d %d %d)", \
// 			FmtTimeShow(), \
//  			gModemParam.PRevInUse, \
//  			gModemParam.MCC,\
//  			gModemParam.IMSI_11_12,\
//  			gModemParam.sid,\
//  			gModemParam.nid,\
//  			gModemParam.baseId,\
//  			gModemParam.baseLat,\
//  			gModemParam.baseLong);
// 	#endif
// }

// static void MmiBARCODE(ATRspParmT* MsgDataP)
// {
// 	/*+BARCODE:455555555*/
// 	memset(gModemParam.barcode, 0, sizeof(gModemParam.barcode));
// 	strcpy((char *)gModemParam.barcode, MsgDataP[0].Str);

// 	// Print Out
// 	#ifdef ATRSP_RESULT
// 	if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
// 		ParseatPrint(NRCMD,"\r\n[%s] PAT:+BARCODE(%s)", \
// 			FmtTimeShow(),gModemParam.barcode);
// 	#endif
// }

static void MmiATDefault (GSM_FB_CMD FB_CMD,char* MsgDataP)
{
	// u16 Strlen = 0;
	// Strlen = strlen((char*)MsgDataP);
	ParseatPrint(DbgCtl.ParseatCmdEn,"\r\n[%s] MmiATDefault , SmsRecFlag(%d)",FmtTimeShow(), SmsRecFlag);
	
	if (SmsRecFlag)
	{
		ParseatPrint(DbgCtl.ParseatCmdEn,"\r\n[%s] MmiATDefault , SmsRec(%s)",FmtTimeShow(), MsgDataP);
		strcpy(SmsReceiveBuf.smstextdata, MsgDataP);
		SmsReceivedHandle(&SmsReceiveBuf, sizeof(SmsReceiveBuf));

		SmsRecFlag = 0;
	}
  	//if the prase is abnormity,reset the gsm at parse status
  	// if(IsCmgrSmsCommingType == TRUE)
  	// {
  	//       IsCmgrSmsCommingType = FALSE;

	// 	ParseatPrint(NRCMD,"\r\n[%s] PAT:SMS comming",FmtTimeShow());
	// 	// Analysis data
	// 	// Ds600_ServiceStrHandle(MsgDataP);
	// 	// Check status
	// 	// if(GetLogSavingUsableStat() == TRUE)
	// 	// {
	// 	// 	// Log saving
	// 	// 	SaveSystemLogToMemory(LOG_SAVE_SMS_IN, (u8*)MsgDataP);
	// 	// }
  	// }
	// else
	// {
	// 	if(FB_CMD == GSM_FB_LAST_GSM_FB && (Strlen == 0 || Strlen == 2))  //data is only ODOA, no content.
	// 	{
	// 		resetFSM = TRUE;
	// 		ParseatPrint(NRCMD,"\r\n[%s] PAT:Status matine is wrong",FmtTimeShow());
	// 	}
		//Other commands are not in the list, just print in here.
		// #ifdef ATRSP_RESULT
		// if(gDeviceConfig.DbgCtl.ParseatResultEn == TRUE)
		// {
		// 	ParseatPrint(NRCMD,"\r\n[%s] PAT:Default(%d %d %d) Info(%d %s)", \
		// 		FmtTimeShow(), \
		// 		resetFSM, \
		// 		FB_CMD, \
		// 		GSM_FB_LAST_GSM_FB, \
		// 		Strlen, \
		// 		MsgDataP);
		// }
		// #endif
	// }
}

/******************************************************************************
* End of File
******************************************************************************/

