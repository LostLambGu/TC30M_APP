/*******************************************************************************
* File Name          : wedgeatprocess.c
* Author             :
* Description        : This file provides all the wedgeatprocess functions.

* History:
*  12/12/2017 : wedgeatprocess V1.00
*******************************************************************************/


#include "wedgeatprocess.h"
#include "wedgecommonapi.h"
#include "deepsleep.h"
#include "stdarg.h"
#include "network.h"

SmsReceiveBufProcessTypedef *SMSMessage;
char temsmsnumber[SMS_RECEIVE_NUMBER_MAX_LEN]={'\0'};

SMSTypeDef *SMS,_SMS;
SVRCFGTypeDef *SVRCFG,_SVRCFG;
APNCFGTypeDef *APNCFG,_APNCFG;
HWRSTTypeDef *HWRST,_HWRST;
PWRMGTTypeDef *PWRMGT,_PWRMGT;
IGNTYPETypeDef *IGNTYPE,_IGNTYPE;
RPTINTVLTypeDef *RPTINTVL,_RPTINTVL;
IOALRMTypeDef *IOALRM1,_IOALRM1;
IOALRMTypeDef *IOALRM2,_IOALRM2;
LVATypeDef *LVA,_LVA;
IDLETypeDef *IDLE,_IDLE;
SODOTypeDef *SODO,_SODO;
VODOTypeDef *VODO,_VODO;
DIRCHGTypeDef *DIRCHG,_DIRCHG;
TOWTypeDef *TOW,_TOW;
STPINTVLTypeDef *STPINTVL,_STPINTVL;
GFNCTypeDef *GFNC1,_GFNC1;
GFNCTypeDef *GFNC2,_GFNC2;
GFNCTypeDef *GFNC3,_GFNC3;
GFNCTypeDef *GFNC4,_GFNC4;
GFNCTypeDef *GFNC5,_GFNC5;
GFNCTypeDef *GFNC6,_GFNC6;
GFNCTypeDef *GFNC7,_GFNC7;
GFNCTypeDef *GFNC8,_GFNC8;
GFNCTypeDef *GFNC9,_GFNC9;
GFNCTypeDef *GFNC10,_GFNC10;
RELAYTypeDef *RELAY,_RELAY;
OSPDTypeDef *OSPD,_OSPD;
PLSRLYTypeDef *PLSRLY,_PLSRLY;
USRDATTypeDef *USRDAT,_USRDAT;
FTPCFGTypeDef *FTPCFG,_FTPCFG;
u8 ATKEY[0xff]={'\0'};
u8 ATKEYLen = 0;
char * myStrtok;
char * mystrtok(char * s, const char * ct);
int Setpara(uint8_t * dataBuf1,char buffer[255][255],int length);
void AppAtMainProcess(uint8_t * dataBuf, uint8_t dataLen,uint8_t Fromtype);
static char imei[15]={'\0'};
//double pLatitude[];
//double pLongitude[];
u8 a[]={0x01,0x00,0x00,0x01,0x65,0x84,0x42,0X65,0X80,0X78,0X67,0X70,0X71,0X63};
uint8_t *AppAtProcess(uint8_t * Data, uint8_t DataLen)
{
    return NULL;
}

void AppUartAtProcess(uint8_t	 * UartData, uint8_t DataLen)
{
    AppAtMainProcess(UartData,DataLen,ToUART);
}

void AppUdpAtProcess(uint8_t * UdpData, uint8_t DataLen)
{
    AppAtMainProcess(UdpData,DataLen,ToUDP);
}

void AppSmsAtProcess(uint8_t * SmsData, uint8_t DataLen)
{
    SMSMessage=(SmsReceiveBufProcessTypedef*)SmsData;

    AppAtMainProcess((uint8_t *)(SMSMessage->smstextdata),DataLen,ToSMS);
}
u8 PYIDchange(u8 pyid)
{
    switch (pyid)
    {
    case WEDGEPYLD_COMMAND:
        return WEDGEPYLD_COMMAND_RESPONSE;

    case WEDGEPYLD_DIAGNOSTIC_REQUEST:
        return WEDGEPYLD_DIAGNOSTIC_RESPONSE;

    case 0x31:
        return 0x32;

    case 0x34:
        return 0x33;


    default:
        return 0;

    }
}
static const ATCompareTable1_t ATCmdTypeTable[AT_CMD_DEF_NULL] =
{
    { "AT",				AT_CMD_DEF_INITIAL,			TRUE },
    { "SMS",			 AT_CMD_ADMIN_SMS,			TRUE },          //SMS addresses
    { "SVRCFG",			AT_CMD_ADMIN_SVRCFG,			TRUE },    //Application Server details
    { "FTPCFG",			AT_CMD_ADMIN_FTPCFG,				TRUE },	 //FTP Server configuration
    { "APNCFG",		AT_CMD_ADMIN_APNCFG,			TRUE },      //APN / Carrier details
    { "HWRST",			AT_CMD_ADMIN_HWRST,				TRUE },      //Periodic Hardware Reset
    { "PWRMGT",			AT_CMD_ADMIN_PWRMGT,			TRUE },    //Configure Power Management
    { "USRDAT",			AT_CMD_ADMIN_USRDAT,				TRUE },  //Configure User Defined Data
    { "CFGALL",			AT_CMD_ADMIN_CFGALL,			TRUE },    //Configure ALL Thresholds
    { "F",		       AT_CMD_ADMIN_F,			TRUE },          //Reset to Factory Defaults

    { "IGNTYP",			AT_CMD_OPERA_IGNTYP,			TRUE },    //Set Ignition Type (IGNTYP)
    { "RPTINTVL",			AT_CMD_OPERA_RPTINTVL,		TRUE },  //Configure Reporting Intervals
    { "IOALM1",			AT_CMD_OPERA_IOALM1,			TRUE },    //ALARM1 Configuration
    { "IOALM2",			AT_CMD_OPERA_IOALM2,			TRUE },    //ALARM2 Configuration
    { "LVA",			AT_CMD_OPERA_LVA,		TRUE },              //Configure Low Battery Alert
    { "IDLE",		AT_CMD_OPERA_IDLE,			TRUE },          //Configure IDLE Detect
    { "SODO",		AT_CMD_OPERA_SODO,			TRUE },          //Set Service Odometer
    { "DIRCHG",		AT_CMD_OPERA_DIRCHG,			TRUE },      //Configure Heading Change Reporting
    { "TOW",		AT_CMD_OPERA_TOW,			TRUE },              //Configure Tow Alert
    { "STPINTVL",		AT_CMD_OPERA_STPINTVL,			TRUE },  //Configure Stop Reports
    { "VODO",		AT_CMD_OPERA_VODO,			TRUE },          //Set Virtual Odometer
    { "GFNC",		AT_CMD_OPERA_GFNC,			TRUE },          //Set Set / Clear Geofences
    { "CURGEO",  AT_CMD_OPERA_CURGEO,			TRUE },        //Set Current Location as Geofence
    { "LOC",  AT_CMD_OPERA_LOC,			TRUE },              //Request Current Location
    { "RELAY",  AT_CMD_OPERA_RELAY,			TRUE },          // Starter Enable/ Disable
    { "PLSRLY",  AT_CMD_OPERA_PLSRLY,			TRUE },        //Unlock Doors
    { "OSPD",  AT_CMD_OPERA_OSPD,			TRUE },            //Set Over-speed Detect
    { "FTPGET",  AT_CMD_OPERA_FTPGET,			TRUE },        //Initiate Firmware Upgrade

    { "VER",  AT_CMD_DIAGN_VER,			TRUE },              //Request Firmware versions
    { "RST",  AT_CMD_DIAGN_RST,			TRUE },              //Reset Hardware
    { "MQSTAT",  AT_CMD_DIAGN_MQSTAT,			TRUE },        //Read Message Queue Statistics
    { "MQCLR",  AT_CMD_DIAGN_MQCLR,			TRUE },          //Clear Message Queue
    { "GPSDIAG",  AT_CMD_DIAGN_GPSDIAG,			TRUE },      //Request GPS Data
    { "REG",  AT_CMD_DIAGN_REG,			TRUE },              //GSM/GPRS registration status
    { "GSMDIAG",  AT_CMD_DIAGN_GSMDIAG,			TRUE },      //GSM Serving Cell Information
    { "---",  AT_CMD_DIAGN_UNKNOW,			TRUE },            //Equipment Manufacturer

};
int smssend(u8 PYLDType, char *fmt, ...)
{
    int cnt;
    char string[MAX_PRINTF_STR_SIZE + 2] = {'\0'};
    va_list ap;
    va_start(ap,fmt);

    //cnt = vsprintf(string, fmt, 	ap);
    cnt = vsnprintf(string,MAX_PRINTF_STR_SIZE ,fmt, ap);
    if (cnt > 0)
    {
        //PutStrToUart1Dbg(string,strlen((char *)string));
        if (cnt < MAX_PRINTF_STR_SIZE)
        {

            memcpy(SMSMessage->smstextdata, string ,strlen(string));
					  memcpy(&(SMSMessage->smstextdata[2]),	imei,15);
            WedgeResponseSms(PYLDType,SMSMessage,cnt);
        }
        else
        {
            memcpy(SMSMessage->smstextdata, string ,MAX_PRINTF_STR_SIZE);
					  memcpy(&(SMSMessage->smstextdata[2]),	imei,15);
            WedgeResponseSms(PYLDType,SMSMessage,MAX_PRINTF_STR_SIZE);
        }
    }
     memset(imei,0,15);
    return (cnt);	
}
int SendRpt(u8 PYLDType, ATCmdSource_t type,char *fmt, ...)
{

    if (type==ToSMS)
    {
        int cnt;
        char string[MAX_PRINTF_STR_SIZE + 2] = {'\0'};
        va_list ap;
        va_start(ap,fmt);
        //cnt = vsprintf(string, fmt, 	ap);
        cnt = vsnprintf(string,MAX_PRINTF_STR_SIZE ,fmt, ap);
				
					 smssend(PYIDchange(PYLDType),"%c,000000000000000,%s",PYIDchange(PYLDType),string);
				
				
    }
    else if (type==ToUDP)
    {
        int cnt;
        char string[MAX_PRINTF_STR_SIZE + 2] = {'\0'};
        va_list ap;
        va_start(ap,fmt);
        //cnt = vsprintf(string, fmt, ap);
        cnt = vsnprintf(string,MAX_PRINTF_STR_SIZE ,fmt, ap);
        if (cnt > 0)
        {
            //PutStrToUart1Dbg(string,strlen((char *)string));
            if (cnt < MAX_PRINTF_STR_SIZE)
            {
                WedgeResponseUdpAscii(PYIDchange(PYLDType),string,cnt);
            }
            else
            {
                WedgeResponseUdpAscii(PYIDchange(PYLDType),string,MAX_PRINTF_STR_SIZE);
            }
        }
        va_end(ap);
        return (cnt);
    }
    else
    {
        int cnt;
        char string[MAX_PRINTF_STR_SIZE + 2] = {'\0'};
        va_list ap;
        va_start(ap,fmt);
        //cnt = vsprintf(string, fmt, ap);
        cnt = vsnprintf(string,MAX_PRINTF_STR_SIZE ,fmt, ap);
        if (cnt > 0)
        {
            //PutStrToUart1Dbg(string,strlen((char *)string));
            if (cnt < MAX_PRINTF_STR_SIZE)
            {
                PutStrToUart1Dbg(string,cnt);
            }
            else
            {
                PutStrToUart1Dbg(string,MAX_PRINTF_STR_SIZE);
            }
        }
        va_end(ap);
        return (cnt);
    }
    return(-1);

}
void RemoveAT(uint8_t * dataBuf,uint8_t  dataLen,uint8_t * dataBuf1,uint8_t dataLen1)
{
    u8 idx = 0;
	  u8 x=0;

    if ( dataBuf != NULL && dataLen > 0 )
    {
 
			while(x<=dataLen)
			{
				 if ((dataBuf[x]=='a'||dataBuf[x]=='A')&&(dataBuf[x+1]=='t'||dataBuf[x+1]=='T'))
        {
            for ( idx = 0; idx < dataLen-x-2 ; idx++ )
            { 
                if ( 1 )
                {
                     dataBuf1[dataLen1++] = dataBuf[idx+x+2];
                }
                else
                {
                  
                }

            }
						return;
        } 
        else
				{
					x++;
				}					
			}
       

    }
}

static u8 ATCmdGetType1( ATCompareTable1_t *checkTable, u8 tableSize, u8 *dataBuf, u16 dataLen, u8 *idxSearch )
{
    u8						idx;
    u8						cmdLen;
    ATCompareTable1_t			*pTable;
    int x=255;


    if ( dataLen == 0 || dataBuf == NULL || idxSearch == NULL || checkTable == NULL )
        return 0xFF;

    for ( idx = 0; idx < tableSize ; idx++)
    {
        pTable = &checkTable[idx];
        cmdLen = strlen( pTable->cmdStr);

        x=strncmp( (char *)dataBuf, pTable->cmdStr, cmdLen);
        if (  x== 0 )
        {

            if ( pTable->isMatchAll )
            {


                *idxSearch += cmdLen;

                return pTable->cmdType;

            }
            else
            {
                *idxSearch += cmdLen;

                return pTable->cmdType;
            }
        }
    }

    return 0xFF;
}
void AtstateGet(WEDGEPYLDTypeDef PYid,ATCmdSource_t type)
{
    SendRpt(PYid,type,"atok");
}

void AdmSMSGet(WEDGEPYLDTypeDef PYid,ATCmdSource_t type)
{
    SMS=WedgeCfgGet(WEDGE_CFG_SMS);
    SendRpt(PYid,type,"*SMS:\"%s\",\"%s\",\"%s\"\r\n",SMS->sms_1,SMS->sms_2,SMS->sms_3);
}
void AdmSMSSet(WEDGEPYLDTypeDef PYid,ATCmdSource_t type,u8 *buffer)
{
    char str[3][255];

    SMS=&_SMS;

    if (  Setpara(buffer,str,3)>0)
    {
        memcpy(SMS->sms_1, str[0], strlen(str[0]));

        memcpy(SMS->sms_2, str[1], strlen(str[1]));

        memcpy(SMS->sms_3, str[2], strlen(str[2]));
        WedgeCfgSet(WEDGE_CFG_SMS,SMS);
        WedgeCfgChgStateSet(SMS_ADDR_CFG_CHG,TRUE);
        //SendRpt(PYid,type,"*SMS:\"%s\",\"%s\",\"%s\"\r\n",SMS->sms_1,SMS->sms_2,SMS->sms_3);
        SendRpt(PYid,type,"OK\r\n");
    }
    else
    {
        SendRpt(PYid,type,"ERROR\r\n");
    }


}
void AdmSVRCFGGet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid)
{
    SVRCFG=WedgeCfgGet(WEDGE_CFG_SVRCFG);
    SendRpt(PYid,type,"*SVRCFG:%d,%d,%d,\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"\r\n",SVRCFG->port,SVRCFG->prot,SVRCFG->reg,SVRCFG->srvr1,SVRCFG->srvr2,SVRCFG->srvr3,SVRCFG->srvr4,SVRCFG->srvr5);
}
void AdmSVRCFGSet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid,u8 *buffer)
{
    char str[8][255];
    int cnt=Setpara(buffer,str,8);

    if (cnt>0)
    {
        SVRCFG=&_SVRCFG;
        if (strlen(str[0])!=0)
            SVRCFG->port=atoi(str[0]);
        if (strlen(str[1])!=0)
            SVRCFG->prot=atoi(str[1]);
        if (strlen(str[2])!=0)
            SVRCFG->reg=atoi(str[2]);
        WedgeCfgSet(WEDGE_CFG_SVRCFG,SVRCFG);
        WedgeCfgChgStateSet(SVRCFG_CFG_CHG_ALL,TRUE);

        if ((cnt-3)>0)
        {
            memcpy(SVRCFG->srvr1, str[3], strlen(str[3]));
            WedgeCfgSet(WEDGE_CFG_SVRCFG,SVRCFG);
            WedgeCfgChgStateSet(SVRCFG_CFG_CHG_1,TRUE);
        }

        if ((cnt-3)>1)
        {
            memcpy(SVRCFG->srvr2, str[4], strlen(str[4]));
            WedgeCfgSet(WEDGE_CFG_SVRCFG,SVRCFG);
            WedgeCfgChgStateSet(SVRCFG_CFG_CHG_2,TRUE);
        }

        if ((cnt-3)>2)
        {
            memcpy(SVRCFG->srvr3, str[5], strlen(str[5]));
            WedgeCfgSet(WEDGE_CFG_SVRCFG,SVRCFG);
            WedgeCfgChgStateSet(SVRCFG_CFG_CHG_3,TRUE);
        }
        if ((cnt-3)>3)
        {
            memcpy(SVRCFG->srvr4, str[6], strlen(str[6]));
            WedgeCfgSet(WEDGE_CFG_SVRCFG,SVRCFG);
            WedgeCfgChgStateSet(SVRCFG_CFG_CHG_4,TRUE);
        }
        if ((cnt-3)>4)
        {
            memcpy(SVRCFG->srvr5, str[7], strlen(str[7]));
            WedgeCfgSet(WEDGE_CFG_SVRCFG,SVRCFG);
            WedgeCfgChgStateSet(SVRCFG_CFG_CHG_5,TRUE);
        }
        SendRpt(PYid,type,"OK\r\n");

    } else
    {

        SendRpt(PYid,type,"ERROR\r\n");

    }



    //SendRpt(PYid,type,"*SVRCFG:\"%d\",\"%d\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"\r\n",SVRCFG->port,SVRCFG->prot,SVRCFG->reg,SVRCFG->srvr1,SVRCFG->srvr2,SVRCFG->srvr3,SVRCFG->srvr4,SVRCFG->srvr5);

}

void AdmFTPCFGGet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid)
{



}
void AdmAPNCFGGet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid)
{
    APNCFG=WedgeCfgGet(WEDGE_CFG_APNCFG);
    SendRpt(PYid,type,"*APNCFG:\"%s\",\"%s\",\"%s\"\r\n",APNCFG->apn,APNCFG->usr,APNCFG->pwd);


}
void AdmAPNCFGSet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid,u8 *buffer)
{
    APNCFG=&_APNCFG;
    char str[3][255];
    if (Setpara(buffer,str,3)>0)
    {
        if (1)
        {
            memcpy(APNCFG->apn, str[0], strlen(str[0]));
        }
        if (1)
        {
            memcpy(APNCFG->usr, str[1], strlen(str[1]));
        }
        if (1)
        {
            memcpy(APNCFG->pwd, str[2], strlen(str[2]));
        }
        WedgeCfgSet(WEDGE_CFG_APNCFG,APNCFG);
        WedgeCfgChgStateSet(APNCFG_CFG_CHG,TRUE);
        SendRpt(PYid,type,"OK\r\n");
    }
    else
    {
        SendRpt(PYid,type,"ERROR\r\n");
    }

    // SendRpt(PYid,type,"*APNCFG:\"%s\",\"%s\",\"%s\"\r\n",APNCFG->apn,APNCFG->usr,APNCFG->pwd);
}
void AdmHWRSTGet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid)
{
    HWRST=WedgeCfgGet(WEDGE_CFG_HWRST);
    SendRpt(PYid,type,"*HWRST:%d\r\n",HWRST->interval);



}
void AdmHWRSTSet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid,u8 *buffer)
{
    HWRST=&_HWRST;
    char str[1][255];
    if (Setpara(buffer,str,1)>0)
    {
        if (strlen(str[0])!=0)
        {
            HWRST->interval=atoi(str[0]);
        }
        WedgeCfgSet(WEDGE_CFG_HWRST,HWRST);
        WedgeCfgChgStateSet(HWRST_CFG_CHG,TRUE);
        SendRpt(PYid,type,"OK\r\n");
    }
    else
    {
        SendRpt(PYid,type,"ERROR\r\n");
    }


}

void AdmPWRMGTGet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid)
{
    PWRMGT=WedgeCfgGet(WEDGE_CFG_PWRMGT);
    if (PWRMGT->mode==0)
    {
        SendRpt(PYid,type,"*PWRMGT:%d\r\n",PWRMGT->mode);
    }
    if (PWRMGT->mode==1)
    {
        SendRpt(PYid,type,"*PWRMGT:%d,%d,%d\r\n",PWRMGT->mode,PWRMGT->ontime,PWRMGT->dbcslp.sleeptime);
    }
    if (PWRMGT->mode==2||PWRMGT->mode==3||PWRMGT->mode==4)
    {
        SendRpt(PYid,type,"*PWRMGT:%d,%d,%d\r\n",PWRMGT->mode,PWRMGT->dbcslp.debounce,PWRMGT->ontime);
    }




}
void AdmPWRMGTSet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid,u8 *buffer)
{
    PWRMGT=&_PWRMGT;
    char str[3][255];
    if (Setpara(buffer,str,3)>0)
    {
        if (atoi(str[0])==0)
        {
            PWRMGT->mode=0;
        }
        if (atoi(str[0])==1)
        {
            PWRMGT->mode=1;
            PWRMGT->ontime=atoi(str[1]);
            PWRMGT->dbcslp.sleeptime=atoi(str[2]);
        }
        if (atoi(str[0])==2||atoi(str[0])==3||atoi(str[0])==4)
        {
            PWRMGT->mode=atoi(str[0]);
            PWRMGT->ontime=atoi(str[2]);
            PWRMGT->dbcslp.debounce=atoi(str[1]);
        }
        WedgeCfgSet(WEDGE_CFG_PWRMGT,PWRMGT);
        WedgeCfgChgStateSet(PWRMGT_CFG_CHG,TRUE);
        SendRpt(PYid,type,"OK\r\n");
    }
    else
    {
        SendRpt(PYid,type,"ERROR\r\n");
    }


}
void AdmUSRDATGet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid)
{
    USRDAT=WedgeCfgGet(WEDGE_CFG_USRDAT);
    SendRpt(PYid,type,"*USRDAT:\"%s\"\r\n",USRDAT->data);


}
void AdmUSRDATSet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid,u8 *buffer)
{
    USRDAT=&_USRDAT;
    char str[1][255];
    if (Setpara(buffer,str,1)>0)
    {
        memcpy(USRDAT->data, str[0], strlen(str[0]));
        WedgeCfgSet(WEDGE_CFG_USRDAT,USRDAT);
        WedgeCfgChgStateSet(USRDAT_CFG_CHG,TRUE);
        SendRpt(PYid,type,"OK\r\n");
    }
    else
    {
        SendRpt(PYid,type,"ERROR\r\n");
    }


}
void AdmCFGALLGet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid)
{
    IGNTYPE=WedgeCfgGet(WEDGE_CFG_IGNTYPE);
    RPTINTVL=WedgeCfgGet(WEDGE_CFG_RPTINTVL);
    IOALRM1=WedgeCfgGet(WEDGE_CFG_IOALRM1);
    IOALRM2=WedgeCfgGet(WEDGE_CFG_IOALRM2);
    LVA=WedgeCfgGet(WEDGE_CFG_LVA);
    IDLE=WedgeCfgGet(WEDGE_CFG_IDLE);
    SODO=WedgeCfgGet(WEDGE_CFG_SODO);
    DIRCHG=WedgeCfgGet(WEDGE_CFG_DIRCHG);
    TOW=WedgeCfgGet(WEDGE_CFG_TOW);
    STPINTVL=WedgeCfgGet(WEDGE_CFG_STPINTVL);
    SendRpt(PYid,type,	"*CFGALL:%d,%d,%d,%d,%d,%d,%d,%d,%.1f,%d,%d,%d,%d,%d\r\n",
            IGNTYPE->itype,RPTINTVL->ionint,RPTINTVL->ioffint,RPTINTVL->perint,IOALRM1->ioen,IOALRM1->iodbnc,
            IOALRM2->ioen,IOALRM2->iodbnc,LVA->battlvl,IDLE->duration,SODO->meters,DIRCHG->deg,TOW->enable,STPINTVL->interval);
}
void AdmCFGALLSet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid,u8 *buffer)
{
    IGNTYPE=&_IGNTYPE;
    RPTINTVL=&_RPTINTVL;
    IOALRM1=&_IOALRM1;
    IOALRM2=&_IOALRM2;
    LVA=&_LVA;
    IDLE=&_IDLE;
    SODO=&_SODO;
    DIRCHG=&_DIRCHG;
    TOW=&_TOW;
    STPINTVL=&_STPINTVL;
    char str[14][255];
    if (Setpara(buffer,str,14)>0)
    {
        IGNTYPE->itype=atoi(str[0]);
        RPTINTVL->ionint=atoi(str[1]);
        RPTINTVL->ioffint=atoi(str[2]);
        RPTINTVL->perint=atoi(str[3]);
        IOALRM1->ioen=atoi(str[4]);
        IOALRM1->iodbnc=atoi(str[5]);
        IOALRM2->ioen=atoi(str[6]);
        IOALRM2->iodbnc=atoi(str[7]);
        LVA->battlvl=atof(str[8]);
        IDLE->duration=atoi(str[9]);
        SODO->meters=atoi(str[10]);
        DIRCHG->deg=atoi(str[11]);
        TOW->enable=atoi(str[12]);
        STPINTVL->interval=atoi(str[13]);
        WedgeCfgSet(WEDGE_CFG_IGNTYPE,IGNTYPE);
        WedgeCfgSet(WEDGE_CFG_RPTINTVL,RPTINTVL);
        WedgeCfgSet(WEDGE_CFG_IOALRM1,IOALRM1);
        WedgeCfgSet(WEDGE_CFG_IOALRM2,IOALRM2);
        WedgeCfgSet(WEDGE_CFG_LVA,LVA);
        WedgeCfgSet(WEDGE_CFG_IDLE,IDLE);
        WedgeCfgSet(WEDGE_CFG_SODO,SODO);
        WedgeCfgSet(WEDGE_CFG_DIRCHG,DIRCHG);
        WedgeCfgSet(WEDGE_CFG_TOW,TOW);
        WedgeCfgSet(WEDGE_CFG_STPINTVL,STPINTVL);
        WedgeCfgChgStateSet(IGNTYP_CFG_CHG,TRUE);
        WedgeCfgChgStateSet(RPTINTVL_CFG_CHG,TRUE);
        WedgeCfgChgStateSet(ALARM1_CFG_CHG,TRUE);
        WedgeCfgChgStateSet(ALARM2_CFG_CHG,TRUE);
        WedgeCfgChgStateSet(LVA_CFG_CHG,TRUE);
        WedgeCfgChgStateSet(IDLE_CFG_CHG,TRUE);
        WedgeCfgChgStateSet(SODO_CFG_CHG,TRUE);
        WedgeCfgChgStateSet(DIRCHG_CFG_CHG,TRUE);
        WedgeCfgChgStateSet(TOW_CFG_CHG,TRUE);
        WedgeCfgChgStateSet(STPINTVL_CFG_CHG,TRUE);
        SendRpt(PYid,type,"OK\r\n");
    }
    else
    {
        SendRpt(PYid,type,"ERROR\r\n");
    }



}
void AdminFactory(ATCmdSource_t type,WEDGEPYLDTypeDef PYid)
{
//    IGNTYPE=WedgeCfgGet(WEDGE_CFG_IGNTYPE);

//    SendRpt(PYid,type,"*PWRMGT:\"%d\"\r\n",HWRST->interval);


}

void OperaIGNTYPGet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid)
{
    IGNTYPE=WedgeCfgGet(WEDGE_CFG_IGNTYPE);
    switch (IGNTYPE->itype)
    {
    case 0:

        SendRpt(PYid,type,"*IGNTYP:\"itype =0\"\r\n");

        break;
    case 1:
        SendRpt(PYid,type,"*IGNTYP:\"itype=1\",%d,%.1f,%.1f\r\n",IGNTYPE->dbnc,IGNTYPE->offthreshold.voff,IGNTYPE->onthreshold.von);
        break;
    case 2:
        SendRpt(PYid,type,"*IGNTYP:\"itype=2\",%d,%d,%d\r\n",IGNTYPE->dbnc,IGNTYPE->offthreshold.spdoff,IGNTYPE->onthreshold.spdon);
        break;
    case 3:
        SendRpt(PYid,type,"*IGNTYP:\"itype=3\",%d\r\n",IGNTYPE->dbnc);
        break;
    }
    WedgeResponseUdpBinary(PYid,Configure_Ignition_Type_Acknowledge);
}
void OperaIGNTYPSet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid,u8 *buffer)
{
    IGNTYPE=&_IGNTYPE;
    char str[4][255];
    int cnt=Setpara(buffer,str,4);
    if (cnt>0)
    {

        if (atoi(str[0])==0)
        {
            IGNTYPE->itype=0;
            WedgeCfgSet(WEDGE_CFG_IGNTYPE,IGNTYPE);
            WedgeCfgChgStateSet(IGNTYP_CFG_CHG,TRUE);
            SendRpt(PYid,type,"OK\r\n");
        }
        else if (atoi(str[0])==1)
        {
            IGNTYPE->itype=1;
            IGNTYPE->dbnc=atoi(str[1]);
            IGNTYPE->offthreshold.voff=atof( str[2]);
            IGNTYPE->onthreshold.von=atof(str[3]);

            WedgeCfgSet(WEDGE_CFG_IGNTYPE,IGNTYPE);
            WedgeCfgChgStateSet(IGNTYP_CFG_CHG,TRUE);
            SendRpt(PYid,type,"OK\r\n");
        }
        else if (atoi(str[0])==2)
        {
            IGNTYPE->itype=2;
            IGNTYPE->dbnc=atoi(str[1]);
            IGNTYPE->offthreshold.spdoff=atoi( str[2]);
            IGNTYPE->onthreshold.spdon=atoi(str[3]);
            WedgeCfgSet(WEDGE_CFG_IGNTYPE,IGNTYPE);
            WedgeCfgChgStateSet(IGNTYP_CFG_CHG,TRUE);
            SendRpt(PYid,type,"OK\r\n");
        }
        else if (atoi(str[0])==3)
        {
            IGNTYPE->itype=3;
            IGNTYPE->dbnc=atoi(str[1]);

            WedgeCfgSet(WEDGE_CFG_IGNTYPE,IGNTYPE);
            WedgeCfgChgStateSet(IGNTYP_CFG_CHG,TRUE);
            SendRpt(PYid,type,"OK\r\n");
        }
        else
        {
            SendRpt(PYid,type,"ERROR\r\n");
        }





    } else
    {

        SendRpt(PYid,type,"ERROR\r\n");

    }
    WedgeResponseUdpBinary(PYid,Configure_Ignition_Type_Acknowledge);

}
void OperaRPTINTVLGet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid)
{
    RPTINTVL=WedgeCfgGet(WEDGE_CFG_RPTINTVL);
    SendRpt(PYid,type,"*RPTINTVL:%d,%d,%d\r\n",RPTINTVL->ionint,RPTINTVL->ioffint,RPTINTVL->perint);
    WedgeResponseUdpBinary(PYid,Configure_Reporting_Intervals_Acknowledge);
}
void OperaRPTINTVLSet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid,u8 *buffer)
{
    RPTINTVL=&_RPTINTVL;


    char str[3][255];
    if (Setpara(buffer,str,3)>0)
    {

        RPTINTVL->ionint=atoi(str[0]);
        RPTINTVL->ioffint=atoi(str[1]);
        RPTINTVL->perint=atoi(str[2]);

        WedgeCfgSet(WEDGE_CFG_RPTINTVL,RPTINTVL);
        WedgeCfgChgStateSet(RPTINTVL_CFG_CHG,TRUE);
        SendRpt(PYid,type,"OK\r\n");
    }
    else
    {
        SendRpt(PYid,type,"ERROR\r\n");
    }
    WedgeResponseUdpBinary(PYid,Configure_Reporting_Intervals_Acknowledge);
}
void OperaIOALRM1Set(ATCmdSource_t type,WEDGEPYLDTypeDef PYid,u8 *buffer)
{
    IOALRM1=&_IOALRM1;
    char str[2][255];
    if (Setpara(buffer,str,2)>0)
    {

        IOALRM1->ioen=atoi(str[0]);
        IOALRM1->iodbnc=atoi(str[1]);


        WedgeCfgSet(WEDGE_CFG_IOALRM1,IOALRM1);
        WedgeCfgChgStateSet(ALARM1_CFG_CHG,TRUE);
        SendRpt(PYid,type,"OK\r\n");
    }
    else
    {
        SendRpt(PYid,type,"ERROR\r\n");
    }
    WedgeResponseUdpBinary(PYid,Configure_Alarm1_Input_Acknowledge);
}
void OperaIOALRM1Get(ATCmdSource_t type,WEDGEPYLDTypeDef PYid)
{
    IOALRM1=WedgeCfgGet(WEDGE_CFG_IOALRM1);
    SendRpt(PYid,type,"*IOALM1:%d,%d\r\n",IOALRM1->ioen,IOALRM1->iodbnc);
    WedgeResponseUdpBinary(PYid,Configure_Alarm1_Input_Acknowledge);
}

void OperaIOALRM2Get(ATCmdSource_t type,WEDGEPYLDTypeDef PYid)
{
    IOALRM2=WedgeCfgGet(WEDGE_CFG_IOALRM2);
    SendRpt(PYid,type,"*IOALM2:%d,%d\r\n",IOALRM2->ioen,IOALRM2->iodbnc);
    WedgeResponseUdpBinary(PYid,Configure_Alarm2_Input_Acknowledge);
}
void OperaIOALRM2Set(ATCmdSource_t type,WEDGEPYLDTypeDef PYid,u8 *buffer)
{
    IOALRM2=&_IOALRM2;
    char str[2][255];
    if (Setpara(buffer,str,2)>0)
    {

        IOALRM2->ioen=atoi(str[0]);
        IOALRM2->iodbnc=atoi(str[1]);


        WedgeCfgSet(WEDGE_CFG_IOALRM2,IOALRM2);
        WedgeCfgChgStateSet(ALARM2_CFG_CHG,TRUE);
        SendRpt(PYid,type,"OK\r\n");
    }
    else
    {
        SendRpt(PYid,type,"ERROR\r\n");
    }
    WedgeResponseUdpBinary(PYid,Configure_Alarm2_Input_Acknowledge);
}

void OperaLVAGet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid)
{
    LVA=WedgeCfgGet(WEDGE_CFG_LVA);
    SendRpt(PYid,type,"*LVA:%.1f\r\n",LVA->battlvl);
    WedgeResponseUdpBinary(PYid,Configure_Low_Battery_Voltage_Level_Acknowledge);
}
void OperaLVASet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid,u8 *buffer)
{
    LVA=&_LVA;
    char str[1][255];
    if (Setpara(buffer,str,1)>0)
    {

        LVA->battlvl=atof(str[0]);



        WedgeCfgSet(WEDGE_CFG_LVA,LVA);
        WedgeCfgChgStateSet(LVA_CFG_CHG,TRUE);
        SendRpt(PYid,type,"OK\r\n");
    }
    else
    {
        SendRpt(PYid,type,"ERROR\r\n");
    }
    WedgeResponseUdpBinary(PYid,Configure_Low_Battery_Voltage_Level_Acknowledge);
}
void OperaIDLEGet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid)
{
    IDLE=WedgeCfgGet(WEDGE_CFG_IDLE);
    SendRpt(PYid,type,"*IDLE:%d\r\n",IDLE->duration);
    WedgeResponseUdpBinary(PYid,Configure_IDLE_Detect_Acknowledge);
}
void OperaIDLESet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid,u8 *buffer)
{
    IDLE=&_IDLE;
    char str[1][255];
    if (Setpara(buffer,str,1)>0)
    {

        IDLE->duration=atoi(str[0]);
        WedgeCfgSet(WEDGE_CFG_IDLE,IDLE);
        WedgeCfgChgStateSet(IDLE_CFG_CHG,TRUE);
        SendRpt(PYid,type,"OK\r\n");
    }
    else
    {
        SendRpt(PYid,type,"ERROR\r\n");
    }
    WedgeResponseUdpBinary(PYid,Configure_IDLE_Detect_Acknowledge);
}
void OperaSODOGet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid)
{
    SODO=WedgeCfgGet(WEDGE_CFG_SODO);
    SendRpt(PYid,type,"*SODO:%d\r\n",SODO->meters);
    WedgeResponseUdpBinary(PYid,Configure_Service_Odometer_Acknowledge);
}
void OperaSODOSet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid,u8 *buffer)
{
    SODO=&_SODO;
    char str[1][255];
    if (Setpara(buffer,str,1)>0)
    {

        SODO->meters=atoi(str[0]);
        WedgeCfgSet(WEDGE_CFG_SODO,SODO);
        WedgeCfgChgStateSet(SODO_CFG_CHG,TRUE);
        SendRpt(PYid,type,"OK\r\n");
    }
    else
    {
        SendRpt(PYid,type,"ERROR\r\n");
    }
    WedgeResponseUdpBinary(PYid,Configure_Service_Odometer_Acknowledge);
}
void OperaDIRCHGGet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid)
{
    DIRCHG=WedgeCfgGet(WEDGE_CFG_DIRCHG);
    SendRpt(PYid,type,"*DIRCHG:%d\r\n",DIRCHG->deg);
    WedgeResponseUdpBinary(PYid,Configure_Heading_Change_Acknowledge);
}
void OperaDIRCHGSet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid,u8 *buffer)
{
    DIRCHG=&_DIRCHG;
    char str[1][255];
    if (Setpara(buffer,str,1)>0)
    {

        DIRCHG->deg=atoi(str[0]);
        WedgeCfgSet(WEDGE_CFG_DIRCHG,DIRCHG);
        WedgeCfgChgStateSet(DIRCHG_CFG_CHG,TRUE);
        SendRpt(PYid,type,"OK\r\n");
    }
    else
    {
        SendRpt(PYid,type,"ERROR\r\n");
    }
    WedgeResponseUdpBinary(PYid,Configure_Heading_Change_Acknowledge);
}

void OperaTOWGet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid)
{
    TOW=WedgeCfgGet(WEDGE_CFG_TOW);
    SendRpt(PYid,type,"*TOW:%d,%d\r\n",TOW->enable,TOW->radius);
    WedgeResponseUdpBinary(PYid,Configure_Tow_Alert_Acknowledge);
}
void OperaTOWSet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid,u8 *buffer)
{
    TOW=&_TOW;
    char str[2][255];
    if (Setpara(buffer,str,2)>0)
    {

        TOW->enable=atoi(str[0]);
        TOW->radius=atoi(str[1]);
        WedgeCfgSet(WEDGE_CFG_TOW,TOW);
        WedgeCfgChgStateSet(TOW_CFG_CHG,TRUE);
        SendRpt(PYid,type,"OK\r\n");
    }
    else
    {
        SendRpt(PYid,type,"ERROR\r\n");
    }
    WedgeResponseUdpBinary(PYid,Configure_Tow_Alert_Acknowledge);
}

void OperaSTPINTVLGet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid)
{
    STPINTVL=WedgeCfgGet(WEDGE_CFG_STPINTVL);
    SendRpt(PYid,type,"*STPINTVL:%d\r\n",STPINTVL->interval);

}
void OperaSTPINTVLSet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid,u8 *buffer)
{
    STPINTVL=&_STPINTVL;
    char str[1][255];
    if (Setpara(buffer,str,1)>0)
    {

        STPINTVL->interval=atoi(str[0]);

        WedgeCfgSet(WEDGE_CFG_STPINTVL,STPINTVL);
        WedgeCfgChgStateSet(STPINTVL_CFG_CHG,TRUE);
        SendRpt(PYid,type,"OK\r\n");
    }
    else
    {
        SendRpt(PYid,type,"ERROR\r\n");
    }


}
void OperaVODOGet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid)
{
    VODO=WedgeCfgGet(WEDGE_CFG_VODO);
    SendRpt(PYid,type,"*VODO:%d\r\n",VODO->meters);

}
void OperaVODOSet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid,u8 *buffer)
{
    VODO=&_VODO;
    char str[1][255];
    if (Setpara(buffer,str,1)>0)
    {

        VODO->meters=atoi(str[0]);

        WedgeCfgSet(WEDGE_CFG_VODO,VODO);
        WedgeCfgChgStateSet(VODO_CFG_CHG,TRUE);
        SendRpt(PYid,type,"OK\r\n");
    }
    else
    {
        SendRpt(PYid,type,"ERROR\r\n");
    }

}
void OperaGFNCGet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid)
{
    GFNC1=WedgeCfgGet(WEDGE_CFG_GFNC1);
    SendRpt(PYid,type,"*GFNC:%d,%d,%d,%d,%d,%d\r\n",GFNC1->index,GFNC1->type,GFNC1->dir,GFNC1->lat_ctr,GFNC1->lon_ctr,GFNC1->radius);
    GFNC2=WedgeCfgGet(WEDGE_CFG_GFNC2);
    SendRpt(PYid,type,"*GFNC:%d,%d,%d,%d,%d,%d\r\n",GFNC2->index,GFNC2->type,GFNC2->dir,GFNC2->lat_ctr,GFNC2->lon_ctr,GFNC2->radius);
    GFNC3=WedgeCfgGet(WEDGE_CFG_GFNC3);
    SendRpt(PYid,type,"*GFNC:%d,%d,%d,%d,%d,%d\r\n",GFNC3->index,GFNC3->type,GFNC3->dir,GFNC3->lat_ctr,GFNC3->lon_ctr,GFNC3->radius);
    GFNC4=WedgeCfgGet(WEDGE_CFG_GFNC4);
    SendRpt(PYid,type,"*GFNC:%d,%d,%d,%d,%d,%d\r\n",GFNC4->index,GFNC4->type,GFNC4->dir,GFNC4->lat_ctr,GFNC4->lon_ctr,GFNC4->radius);
    GFNC5=WedgeCfgGet(WEDGE_CFG_GFNC5);
    SendRpt(PYid,type,"*GFNC:%d,%d,%d,%d,%d,%d\r\n",GFNC5->index,GFNC5->type,GFNC5->dir,GFNC5->lat_ctr,GFNC5->lon_ctr,GFNC5->radius);
    GFNC6=WedgeCfgGet(WEDGE_CFG_GFNC6);
    SendRpt(PYid,type,"*GFNC:%d,%d,%d,%d,%d,%d\r\n",GFNC6->index,GFNC6->type,GFNC6->dir,GFNC6->lat_ctr,GFNC6->lon_ctr,GFNC6->radius);
    GFNC7=WedgeCfgGet(WEDGE_CFG_GFNC7);
    SendRpt(PYid,type,"*GFNC:%d,%d,%d,%d,%d,%d\r\n",GFNC7->index,GFNC7->type,GFNC7->dir,GFNC7->lat_ctr,GFNC7->lon_ctr,GFNC7->radius);
    GFNC8=WedgeCfgGet(WEDGE_CFG_GFNC8);
    SendRpt(PYid,type,"*GFNC:%d,%d,%d,%d,%d,%d\r\n",GFNC8->index,GFNC8->type,GFNC8->dir,GFNC8->lat_ctr,GFNC8->lon_ctr,GFNC8->radius);
    GFNC9=WedgeCfgGet(WEDGE_CFG_GFNC9);
    SendRpt(PYid,type,"*GFNC:%d,%d,%d,%d,%d,%d\r\n",GFNC9->index,GFNC9->type,GFNC9->dir,GFNC9->lat_ctr,GFNC9->lon_ctr,GFNC9->radius);
    GFNC10=WedgeCfgGet(WEDGE_CFG_GFNC10);
    SendRpt(PYid,type,"*GFNC:%d,%d,%d,%d,%d,%d\r\n",GFNC10->index,GFNC10->type,GFNC10->dir,GFNC10->lat_ctr,GFNC10->lon_ctr,GFNC10->radius);
}
void OperaGFNCSet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid,u8 *buffer)
{
    GFNC1=&_GFNC1;
    GFNC2=&_GFNC2;
    GFNC3=&_GFNC3;
    GFNC4=&_GFNC4;
    GFNC5=&_GFNC5;
    GFNC6=&_GFNC6;
    GFNC7=&_GFNC7;
    GFNC8=&_GFNC8;
    GFNC9=&_GFNC9;
    GFNC10=&_GFNC10;
    char str[4][255];
    if (Setpara(buffer,str,1)>0)
    {

        switch (atoi(str[0]))
        {
        case 1:
        {

            GFNC1->index=atoi(str[0]);
            GFNC1->type=atoi(str[1]);
            GFNC1->dir=atoi(str[2]);
            GFNC1->dir=atoi(str[3]);
            WedgeCfgSet(WEDGE_CFG_GFNC1,GFNC1);
            WedgeCfgChgStateSet(GEOFENCES1_CFG_CHG,TRUE);
            SendRpt(PYid,type,"OK\r\n");
            break;
        }
        case 2:

        {

            GFNC2->index=atoi(str[0]);
            GFNC2->type=atoi(str[1]);
            GFNC2->dir=atoi(str[2]);
            GFNC2->dir=atoi(str[3]);
            WedgeCfgSet(WEDGE_CFG_GFNC2,GFNC2);
            WedgeCfgChgStateSet(GEOFENCES2_CFG_CHG,TRUE);
            SendRpt(PYid,type,"OK\r\n");
            break;
        }
        case 3:

        {

            GFNC3->index=atoi(str[0]);
            GFNC3->type=atoi(str[1]);
            GFNC3->dir=atoi(str[2]);
            GFNC3->dir=atoi(str[3]);
            WedgeCfgSet(WEDGE_CFG_GFNC3,GFNC3);
            WedgeCfgChgStateSet(GEOFENCES3_CFG_CHG,TRUE);
            SendRpt(PYid,type,"OK\r\n");
            break;
        }
        case 4:

        {

            GFNC4->index=atoi(str[0]);
            GFNC4->type=atoi(str[1]);
            GFNC4->dir=atoi(str[2]);
            GFNC4->dir=atoi(str[3]);
            WedgeCfgSet(WEDGE_CFG_GFNC4,GFNC4);
            WedgeCfgChgStateSet(GEOFENCES4_CFG_CHG,TRUE);
            SendRpt(PYid,type,"OK\r\n");
            break;
        }
        case 5:

        {

            GFNC5->index=atoi(str[0]);
            GFNC5->type=atoi(str[1]);
            GFNC5->dir=atoi(str[2]);
            GFNC5->dir=atoi(str[3]);
            WedgeCfgSet(WEDGE_CFG_GFNC5,GFNC5);
            WedgeCfgChgStateSet(GEOFENCES5_CFG_CHG,TRUE);
            SendRpt(PYid,type,"OK\r\n");
            break;
        }
        case 6:

        {

            GFNC6->index=atoi(str[0]);
            GFNC6->type=atoi(str[1]);
            GFNC6->dir=atoi(str[2]);
            GFNC6->dir=atoi(str[3]);
            WedgeCfgSet(WEDGE_CFG_GFNC6,GFNC6);
            WedgeCfgChgStateSet(GEOFENCES6_CFG_CHG,TRUE);
            SendRpt(PYid,type,"OK\r\n");
            break;
        }
        case 7:

        {

            GFNC7->index=atoi(str[0]);
            GFNC7->type=atoi(str[1]);
            GFNC7->dir=atoi(str[2]);
            GFNC7->dir=atoi(str[3]);
            WedgeCfgSet(WEDGE_CFG_GFNC7,GFNC7);
            WedgeCfgChgStateSet(GEOFENCES7_CFG_CHG,TRUE);
            SendRpt(PYid,type,"OK\r\n");
            break;
        }
        case 8:

        {

            GFNC8->index=atoi(str[0]);
            GFNC8->type=atoi(str[1]);
            GFNC8->dir=atoi(str[2]);
            GFNC8->dir=atoi(str[3]);
            WedgeCfgSet(WEDGE_CFG_GFNC8,GFNC8);
            WedgeCfgChgStateSet(GEOFENCES8_CFG_CHG,TRUE);
            SendRpt(PYid,type,"OK\r\n");
            break;
        }
        case 9:

        {

            GFNC9->index=atoi(str[0]);
            GFNC9->type=atoi(str[1]);
            GFNC9->dir=atoi(str[2]);
            GFNC9->dir=atoi(str[3]);
            WedgeCfgSet(WEDGE_CFG_GFNC9,GFNC9);
            WedgeCfgChgStateSet(GEOFENCES9_CFG_CHG,TRUE);
            SendRpt(PYid,type,"OK\r\n");
            break;
        }
        case 10:

        {

            GFNC10->index=atoi(str[0]);
            GFNC10->type=atoi(str[1]);
            GFNC10->dir=atoi(str[2]);
            GFNC10->dir=atoi(str[3]);
            WedgeCfgSet(WEDGE_CFG_GFNC10,GFNC10);
            WedgeCfgChgStateSet(GEOFENCES10_CFG_CHG,TRUE);
            SendRpt(PYid,type,"OK\r\n");
            break;
        }
        default:
            SendRpt(PYid,type,"ERROR\r\n");
            break;
        }


    }
    else
    {
        SendRpt(PYid,type,"ERROR\r\n");
    }
}

void OperaLOCGet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid)
{
    //gps
    // SendRpt(PYid,type,"*VODO:\"%d\"\r\n",VODO->meters);

}

void OperaRELAYGet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid)
{
    RELAY=WedgeCfgGet(WEDGE_CFG_RELAY);
    SendRpt(PYid,type,"*RELAY:%d\r\n",RELAY->state);
}
void OperaRELAYSet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid,u8 *buffer)
{
    RELAY=&_RELAY;
    char str[1][255];
    if (Setpara(buffer,str,1)>0)
    {

        RELAY->state=atoi(str[0]);

        WedgeCfgSet(WEDGE_CFG_RELAY, RELAY);
        WedgeCfgChgStateSet(RELAY_CFG_CHG,TRUE);
        SendRpt(PYid,type,"OK\r\n");
    }
    else
    {
        SendRpt(PYid,type,"ERROR\r\n");
    }
}
void OperaPLSRLYGet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid)
{
    PLSRLY=WedgeCfgGet(WEDGE_CFG_PLSRLY);
    SendRpt(PYid,type,"*PLSRLY:%d,%d,%d,%d\r\n",PLSRLY->pw,PLSRLY->ps,PLSRLY->count,PLSRLY->evid);
}
void OperaPLSRLYSet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid,u8 *buffer)
{
    PLSRLY=&_PLSRLY;
    char str[4][255];
    if (Setpara(buffer,str,4)>0)
    {

        PLSRLY->pw=atoi(str[0]);
        PLSRLY->ps=atoi(str[1]);
        PLSRLY->count=atoi(str[2]);
        PLSRLY->evid=atoi(str[3]);

        WedgeCfgSet(WEDGE_CFG_PLSRLY,PLSRLY);
        WedgeCfgChgStateSet(PLSRLY_CFG_CHG,TRUE);
        SendRpt(PYid,type,"OK\r\n");
    }
    else
    {
        SendRpt(PYid,type,"ERROR\r\n");
    }
}
void OperaOSPDGet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid)
{
    OSPD=WedgeCfgGet(WEDGE_CFG_OSPD);
    SendRpt(PYid,type,"*OSPD:%d\r\n",OSPD->speed);
}
void OperaOSPDSet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid,u8 *buffer)
{
    OSPD=&_OSPD;
    char str[3][255];
    if (Setpara(buffer,str,3)>0)
    {

        OSPD->units=atoi(str[0]);
        OSPD->speed=atoi(str[1]);
        OSPD->debounce=atoi(str[2]);
        WedgeCfgSet(WEDGE_CFG_OSPD,OSPD);
        WedgeCfgChgStateSet(OPSOD_CFG_CHG,TRUE);
        SendRpt(PYid,type,"OK\r\n");
    }
    else
    {
        SendRpt(PYid,type,"ERROR\r\n");
    }
}
void OperaFTPGETGet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid)
{
    FTPCFG=WedgeCfgGet(WEDGE_CFG_FTPCFG);

    // SendRpt(PYid,type,"*FTPCFG:%d\r\n");
}
void OperaFTPGETSet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid,u8 *buffer)
{
//    OSPD=WedgeCfgGet(WEDGE_CFG_OSPD);
//    SendRpt(PYid,type,"*OSPD:\"%d\"\r\n",OSPD->speed);

}

void DiagnVERGet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid)
{
//*VER: <application version>, <GSM version>, <GPS version><CRLF>OK
    SendRpt(PYid,type,"verGet:\r\n");
}
void DiagnRSTSet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid)
{
    SendRpt(PYid,type,"OK\r\n");
    SoftwareJumping(ROM_START_ADDRESS);

}
void DiagnMQSTATGet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid)
{
    SendRpt(PYid,type,"MQSTATGet:\r\n");
}
void DiagnMQCLRSet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid)
{
    SendRpt(PYid,type,"MQCLRSet:\r\n");
}
void DiagnGPSDIAGGet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid)
{
    SendRpt(PYid,type,"GPSDIAGGet:\r\n");
}
void DiagnGPSDIAGSet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid,u8 *buffer)
{
    SendRpt(PYid,type,"GPSDIAGSet:\r\n");
}
void DiagnREGGet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid)
{
    SendRpt(PYid,type,"REGGet:\r\n");
}
void DiagnGSMDIAGGet(ATCmdSource_t type,WEDGEPYLDTypeDef PYid)
{
    SendRpt(PYid,type,"GSMDIAGGet:\r\n");
}
void ATGetProcess(ATCmdType_t type,ATCmdSource_t DataSource,u8 PYid,char IMEI[15])
{
    switch (type)
    {
    case AT_CMD_DEF_INITIAL:
        AtstateGet(PYid,DataSource);
        break;
    case AT_CMD_ADMIN_SMS:
        AdmSMSGet(PYid,DataSource);
        break;
    case AT_CMD_ADMIN_SVRCFG:
        AdmSVRCFGGet(DataSource,PYid);
        break;
    case AT_CMD_ADMIN_FTPCFG:
        AdmFTPCFGGet(DataSource,PYid);
        break;
    case AT_CMD_ADMIN_APNCFG:
        AdmAPNCFGGet(DataSource,PYid);
        break;
    case AT_CMD_ADMIN_HWRST:
        AdmHWRSTGet(DataSource,PYid);
        break;
    case AT_CMD_ADMIN_PWRMGT:
        AdmPWRMGTGet(DataSource,PYid);
        break;
    case AT_CMD_ADMIN_USRDAT:
        AdmUSRDATGet(DataSource,PYid);
        break;
    case AT_CMD_ADMIN_CFGALL:
        AdmCFGALLGet(DataSource,PYid);
        break;
    case AT_CMD_ADMIN_F:
        break;
    case AT_CMD_OPERA_IGNTYP:
        OperaIGNTYPGet(DataSource,PYid);
        break;
    case AT_CMD_OPERA_RPTINTVL:
        OperaRPTINTVLGet(DataSource,PYid);
        break;
    case AT_CMD_OPERA_IOALM1:
        OperaIOALRM1Get(DataSource,PYid);
        break;
    case AT_CMD_OPERA_IOALM2:
        OperaIOALRM2Get(DataSource,PYid);
        break;
    case AT_CMD_OPERA_LVA:
        OperaLVAGet(DataSource,PYid);
        break;
    case AT_CMD_OPERA_IDLE:
        OperaIDLEGet(DataSource,PYid);
        break;
    case AT_CMD_OPERA_SODO:
        OperaSODOGet(DataSource,PYid);
        break;
    case AT_CMD_OPERA_DIRCHG:
        OperaDIRCHGGet(DataSource,PYid);
        break;
    case AT_CMD_OPERA_TOW:
        OperaTOWGet(DataSource,PYid);
        break;
    case AT_CMD_OPERA_STPINTVL:
        OperaSTPINTVLGet(DataSource,PYid);
        break;
    case AT_CMD_OPERA_VODO:
        OperaVODOGet(DataSource,PYid);
        break;
    case AT_CMD_OPERA_GFNC:
        OperaGFNCGet(DataSource,PYid);
        break;
    case AT_CMD_OPERA_CURGEO:
        //OperaCURGEOGet(DataSource,PYid);
        break;
    case AT_CMD_OPERA_LOC:
        //OperaLOCGet(DataSource,PYid);
        break;
    case AT_CMD_OPERA_RELAY:
        OperaRELAYGet(DataSource,PYid);
        break;
    case AT_CMD_OPERA_PLSRLY:
        OperaPLSRLYGet(DataSource,PYid);
        break;
    case AT_CMD_OPERA_OSPD:
        OperaOSPDGet(DataSource,PYid);
        break;
    case AT_CMD_OPERA_FTPGET:
        OperaFTPGETGet(DataSource,PYid);
        break;
    case AT_CMD_DIAGN_VER:
        DiagnVERGet(DataSource,PYid);
        break;
    case AT_CMD_DIAGN_RST:
        DiagnRSTSet(DataSource,PYid);
        break;
    case AT_CMD_DIAGN_MQSTAT:
        DiagnMQSTATGet(DataSource,PYid);
        break;
    case AT_CMD_DIAGN_MQCLR:
        DiagnMQCLRSet(DataSource,PYid);
        break;
    case AT_CMD_DIAGN_GPSDIAG:
        DiagnGPSDIAGGet(DataSource,PYid);
        break;
    case AT_CMD_DIAGN_REG:
        DiagnREGGet(DataSource,PYid);
        break;
    case AT_CMD_DIAGN_GSMDIAG:
        DiagnGSMDIAGGet(DataSource,PYid);
        break;
    case AT_CMD_DIAGN_UNKNOW:
        break;

    default:
        SendRpt(PYid,DataSource,"ERROR");
        break;
    }
}
u8* ATSetProcess(ATCmdType_t type,ATCmdSource_t DataSource,WEDGEPYLDTypeDef PYid,uint8_t * dataBuf,char IMEI[15])
{
    switch (type)
    {
    case AT_CMD_ADMIN_SMS:
        AdmSMSSet(PYid,DataSource,dataBuf);
        break;
    case AT_CMD_ADMIN_SVRCFG:
        AdmSVRCFGSet(DataSource,PYid,dataBuf);
        break;
    case AT_CMD_ADMIN_FTPCFG:
        // AdmFTPCFGSet(DataSource,PYid,dataBuf);
        break;
    case AT_CMD_ADMIN_APNCFG:
        AdmAPNCFGSet(DataSource,PYid,dataBuf);
        break;
    case AT_CMD_ADMIN_HWRST:
        AdmHWRSTSet(DataSource,PYid,dataBuf);
        break;
    case AT_CMD_ADMIN_PWRMGT:
        AdmPWRMGTSet(DataSource,PYid,dataBuf);
        break;
    case AT_CMD_ADMIN_USRDAT:
        AdmUSRDATSet(DataSource,PYid,dataBuf);
        break;
    case AT_CMD_ADMIN_CFGALL:
        AdmCFGALLSet(DataSource,PYid,dataBuf);
        break;
    case AT_CMD_ADMIN_F:
        AdminFactory(DataSource,PYid);
        break;
    case AT_CMD_OPERA_IGNTYP:
        OperaIGNTYPSet(DataSource,PYid,dataBuf);
        break;
    case AT_CMD_OPERA_RPTINTVL:
        OperaRPTINTVLSet(DataSource,PYid,dataBuf);
        break;
    case AT_CMD_OPERA_IOALM1:
        OperaIOALRM1Set(DataSource,PYid,dataBuf);
        break;
    case AT_CMD_OPERA_IOALM2:
        OperaIOALRM2Set(DataSource,PYid,dataBuf);
        break;
    case AT_CMD_OPERA_LVA:
        OperaLVASet(DataSource,PYid,dataBuf);
        break;
    case AT_CMD_OPERA_IDLE:
        OperaIDLESet(DataSource,PYid,dataBuf);
        break;
    case AT_CMD_OPERA_SODO:
        OperaSODOSet(DataSource,PYid,dataBuf);
        break;
    case AT_CMD_OPERA_DIRCHG:
        OperaDIRCHGSet(DataSource,PYid,dataBuf);
        break;
    case AT_CMD_OPERA_TOW:
        OperaTOWSet(DataSource,PYid,dataBuf);
        break;
    case AT_CMD_OPERA_STPINTVL:
        OperaSTPINTVLSet(DataSource,PYid,dataBuf);
        break;
    case AT_CMD_OPERA_VODO:
        OperaVODOSet(DataSource,PYid,dataBuf);
        break;
    case AT_CMD_OPERA_GFNC:
        OperaGFNCSet(DataSource,PYid,dataBuf);
        break;
    case AT_CMD_OPERA_CURGEO:
        OperaGFNCSet(DataSource,PYid,dataBuf);
        break;
    case AT_CMD_OPERA_LOC:

        break;
    case AT_CMD_OPERA_RELAY:
        OperaRELAYSet(DataSource,PYid,dataBuf);
        break;
    case AT_CMD_OPERA_PLSRLY:
        OperaPLSRLYSet(DataSource,PYid,dataBuf);
        break;
    case AT_CMD_OPERA_OSPD:
        OperaOSPDSet(DataSource,PYid,dataBuf);
        break;
    case AT_CMD_OPERA_FTPGET:
        // OperaFTPGETSet(DataSource,PYid,dataBuf);
        break;
    case AT_CMD_DIAGN_VER:
        break;
    case AT_CMD_DIAGN_RST:
        break;
    case AT_CMD_DIAGN_MQSTAT:
        break;
    case AT_CMD_DIAGN_MQCLR:
        break;
    case AT_CMD_DIAGN_GPSDIAG:
        break;
    case AT_CMD_DIAGN_REG:
        break;
    case AT_CMD_DIAGN_GSMDIAG:
        DiagnGPSDIAGSet(DataSource,PYid,dataBuf);
        break;
    case AT_CMD_DIAGN_UNKNOW:
        break;

    default:
        SendRpt(PYid,DataSource,"ERROR");
        break;
    }
}
void AppAtMainProcess(uint8_t * dataBuf, uint8_t dataLen,uint8_t Fromtype)
{
    u8 idx = 0;
    u8 idxTemp = 0;
    u8 ATCmd[0xFF]  = {'\0'};
    u8 ATCmdLen = 0;
    char IMEI[15]={'\0'};

    char *result = NULL;
    u8 CMD[0xFF]= {'\0'};
    char *p;
    u8 PYID=0x00;
    u8 matchCmdType = 0;

    if (Fromtype==ToUDP)
    {
        PYID=dataBuf[3];
    }
    else if (Fromtype==ToSMS)
    {
        PYID=dataBuf[0];
        for (idx=0; idx<15; idx++)
        {
            imei[idx]=dataBuf[idx+2];
        }
    }


#ifndef AT_COMMAND_USE_LOWERCASE
    // To lower all alphabet
    for ( idx = 0; idx < dataLen; idx ++ )
    {
        if ( dataBuf[idx] >= 'a' && dataBuf[idx] <= 'z' )
        {
            dataBuf[idx] = ( dataBuf[idx] - 'a' + 'A' );
        }
        /*if( dataBuf[idx] >= 'A' && dataBuf[idx] <= 'Z' )
        {
        	dataBuf[idx] = ( dataBuf[idx] - 'A' + 'a' );
        }*/
        else
        {
            dataBuf[idx] = dataBuf[idx];
        }
    }
    dataBuf[idx] = '\0';
//    APP_AT_PRINT(1,"\r\n[%s] UartDBG:To lower  len(%d) buf(%s)",FmtTimeShow(),dataLen,dataBuf);
#endif
		 RemoveAT(dataBuf,dataLen,ATCmd,ATCmdLen);
    // Parse AT Command
    if ( dataBuf != NULL && dataLen > 0 )
    {
        if (1)
        {           
            result = strtok( (char*)ATCmd, ";" );
            while ( result != NULL )
            {

                if (strchr(result, '='))
                {
                    if (1)
                    {

                        p=strstr(result,"*");
                        if (p && 1==sscanf(p,"*%255[^=]",CMD))
                        {
                            matchCmdType=ATCmdGetType1((ATCompareTable1_t *)ATCmdTypeTable, (sizeof(ATCmdTypeTable)/sizeof(ATCmdTypeTable[0])), CMD, strlen((char*)CMD), &idxTemp);
                            memset( (char *)CMD, 0, sizeof(CMD));
                            if (matchCmdType==0xff)
                            {
                                SendRpt(PYID,Fromtype,"ERROR1");
                                return;
                            }
                            ATSetProcess(matchCmdType, Fromtype, PYID, (uint8_t *)p,IMEI);
                        }
                    }
                }
                else
                {
                    p=strstr(result,"*");
                    if (p && 1==sscanf(p,"*%255[^?]",CMD))
                    {
                        matchCmdType=ATCmdGetType1((ATCompareTable1_t *)ATCmdTypeTable, (sizeof(ATCmdTypeTable)/sizeof(ATCmdTypeTable[0])), CMD, strlen((char*)CMD), &idxTemp);
                        memset( (char *)CMD, 0, sizeof(CMD));
                        if (matchCmdType==0xff)
                        {
                            SendRpt(PYID,Fromtype,"ERROR1");
                            return;
                        }

                        ATGetProcess((ATCmdType_t)matchCmdType,Fromtype, PYID,IMEI);

                    }


                }

                result = strtok( NULL, ";" );
            }
        }



    }
}
int Setpara(uint8_t * dataBuf1,char buffer[255][255],int length)
{

    int index = 0;
    int k=0;
    char t[255];
    char *result = NULL;

    memset(buffer, 0, 255*length);
    result = mystrtok((char*)dataBuf1, ",");
    while (result != NULL) {

        memset(t, 0, 255);
        if (strchr(result, '"')==NULL)
        {
            if (strchr(result, '='))
            {
                k = sscanf(result, "%*[^=]=%[^/n]/n", t);
                strcpy(buffer[index], t);

                index++;
            }
            else
            {
                strcpy(buffer[index], result);
                index++;
            }

        }
        else

        {
            if (result[0]!='"')
                k=sscanf(result, "%*[^\"]\"%[^\"]\"", t);
            else
                k = sscanf(result, "\"%[^\"]\"", t);

            if (k == -1)
            {
                return -1;
            }
            else
            {
                strcpy(buffer[index], t);

                index++;
            }
        }

        result = mystrtok(NULL, ",");
    }
    return index;
}
char * mystrtok(char * s, const char * ct)
{

    char *sbegin, *send;
    sbegin = s ? s : myStrtok;//
    if (!sbegin) {
        return NULL;//
    }
    sbegin += strspn(sbegin, ct);//
    if (*sbegin == '\0') {
        myStrtok = NULL;
        return (NULL);
    }
    send = strpbrk(sbegin, ct);
    if (send && *send != '\0')
        *send++ = '\0';
    myStrtok = send;
    return (sbegin);

}

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2017. All rights reserved
                                End Of The File
*******************************************************************************/


/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2018. All rights reserved
                                End Of The File
*******************************************************************************/
