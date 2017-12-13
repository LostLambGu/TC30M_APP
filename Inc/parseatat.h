/*******************************************************************************
* File Name          : parseatat.h
* Author               : Jevon
* Description        : This file provides all the parseatat functions.

* History:
*  06/02/2015 : parseatat V1.00
*******************************************************************************/
#ifndef	__RECEATCMD__H__
#define	__RECEATCMD__H__
 /* Includes */
#include "include.h"
#include "sendatcmd.h"

/* Defines */
#define GSM_MAX_PARAM			13
#define GSM_MAX_PARAM_LEN		32
#define IPRECV_PARSE_LENGTH		50

#define HEAD_LENGTH 				2
#define TAIL_LENGTH 				2
#define ATDHTBL_NUM_ACT 			1
#define TAIL_LENGTH_QCMIPGETP 	1
#define AT_MAX_AT_CMD_LEN 		(128)//UART3_RX_BUFFER_SIZE

#define SQNSI_PARSE_LENGTH		64

enum 
{
  ATDH_IDLE_ST,

  ATDH_CHECK_HEAD_ST,
  ATDH_CHECK_TAIL_ST,
  ATDH_WFR_CMD_BODY_ST,
  ATDH_PARSE_EXECUTE_CMD_ST,

  ATDHTBL_STATE_MAX
};

enum
{
  ATDH_CheckCmdHead_AC = 1,
  ATDH_CheckCmdTail_AC,
  ATDH_ReenterCmdFound_AC,
  ATDH_ParseExeCmd_AC,

  ATDH_NextCmd_AC,
  ATDHTBL_ACTION_MAX
};

/* DH Events */
enum
{
  ATDH_DPD_TxReq,
  ATDH_set,
  ATDH_clr,
  NUM_ATDHEvents
};

typedef enum
{
	NUM_TYPE,					// Number
	STR_TYPE,					// String
	STR_WITH_QUOTE_TYPE,		// "String"
	TYPE_NUM					// No Use
}ATRspParmTypeT;

typedef struct
{
	uint8 ParmNum;
	ATRspParmTypeT ParmType[GSM_MAX_PARAM];
}ATRspParmFmtT;

typedef struct
{
  UINT8 *buf;
  UINT16 len;
} AtBuf;

typedef struct 
{
	const UINT8 *strp;
	UINT8 CheckIndex;
} CheckStr;

typedef struct
{
	/* Local storage */
	CheckStr Head;
	CheckStr Tail;
	UINT8 *datap;
	UINT16 datalen;
	AtBuf process;
} ATDHParms;

typedef struct
{
	u16 Num;
	char Str[GSM_MAX_PARAM_LEN];
}ATRspParmT;

// Variable Declared 
extern u8 resetFSM;
extern AtBuf AtBuffer;
extern const ARCA_ATCMDStruct gsm_rsp_cmd[GSM_FB_LAST_GSM_FB];
extern const ATRspParmFmtT ATParmFmt[GSM_FB_LAST_GSM_FB];

extern char ICCIDBuf[32];
extern char IMEIBuf[32];

//Function Declare
extern void RefreshImei(void);
extern u8 CheckIpAddress(char* p);
extern GSM_FB_CMD ValATRspParse(uint8 *DataP, uint16 DataLen, ATRspParmT* pRspData);
extern void ValATRspProcess(GSM_FB_CMD RspCmdType, ATRspParmT* pRspData, char *DataP);
extern void ReceiveDataProcess(UINT8 *data, UINT16 length);
extern void atDhParmInit(void);
extern void parseAtDHInit (void);
extern void atDHProcessEvent (AtDHEvent event, void* parms, UINT8 parmSize);
#endif

/******************************************************************************
* End of File
******************************************************************************/
