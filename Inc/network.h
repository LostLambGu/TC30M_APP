/*******************************************************************************
* File Name          : network.h
* Author             : Yangjie Gu
* Description        : This file provides all the network functions.

* History:
*  11/10/2017 : network V1.00
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __NETWORK_H__
#define __NETWORK_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "include.h"
#include "sendatcmd.h"

/* Defines  ------------------------------------------------------------------*/
#define DEFAULT_RSSI_VALUE			0
#define MAX_IP_ADDR_LEN 				32
#define MAX_FILE_PATH_LEN 			64
#define NETWORK_LISTEN_PORT 		5555
#define CHECK_NETWORK_TIMEOUT 		2000
#define OTA_REPROGRAM_ACT_TIMEOUT	(10*1000)

#define MAX_VERSION_LEN             (22)

#define TC30M_UDP_SEND_DATA_MAX_LEN (1500)

typedef enum  
{
    NET_FREE_IDLE_STAT 				= 0,
    NET_IDLE_AIRPLANE_STAT 			= 1,
    NET_CHECK_NETWORK_STATUS		= 2,
    NET_CONNECTED_STAT              = 3,
    NET_DISCONNECT_STAT             = 4,
    NET_WAITT_DISCONNECTED_STAT     = 5,
    NET_DISCONNECTED_STAT           = 6,

    NET_MAXIMUM_STAT,
} NetworkStatT;

// typedef enum  
// {
//     NET_FREE_IDLE_STAT 				= 0,
//     NET_IDLE_AIRPLANE_STAT 			= 1,
//     NET_CHECK_NETWORK_STATUS		= 2,
//     NET_CREATE_SOCKET_STAT			= 3,
//     NET_WAIT_SOCKET_CONNECT_STAT	= 4,
//     NET_SOCKET_CONNECTED_STAT		= 5,
//     NET_SOCKET_DISCONNECT_STAT		= 6,
//     NET_WAITT_SOCKET_DISCONNECT		= 7,
//     NET_SOCKET_DISCONNECTED_STAT	= 8,

//     NET_MAXIMUM_STAT,
// } NetworkStatT;

typedef enum 
{
    CME_DNS_LOOKUP = 3,			/*03 DNS Lookup fail*/
    CME_DNS_NOTSUPPORT = 4,		/*03 operation not supported*/
    CME_NET_EEOF = 800,             		/*00 End of file*/
    CME_NET_EBADF,                  		/*01 Invalid socked descriptor*/
    CME_NET_EFAULT,                 		/*02 Invalid buffer or argument*/
    CME_NET_EWOULDBLOCK,            	/*03 Operation would block*/
    CME_NET_EAFNOSUPPORT,           	/*04 Address family not supported*/
    CME_NET_EPROTOTYPE,             	/*05 Wrong protocol for socket type*/
    CME_NET_ESOCKNOSUPPOR,          	/*06 Socket parameter not supported*/
    CME_NET_EPROTONOSUPPO,          	/*07 Protocol not supported*/
    CME_NET_EMFILE,               		/*08 No more socket available for opening*/
    CME_NET_EOPNOTSUPP,             	/*09 Operation not supported*/
    CME_NET_EADDRINUSE,             	/*10 Address already in use*/
    CME_NET_EADDRREQ,               		/*11 Destination address required*/
    CME_NET_EINPROGRESS,           	/*12 Connection establishment in progress*/
    CME_NET_EISCONN,                		/*13 Connection already established*/
    CME_NET_EIPADDRCHANGE,         	/*14 IP address changed,causing TCP reset*/
    CME_NET_ENOTCONN,               		/*15 Socket not connected*/
    CME_NET_ECONNREFUSED,           	/*16 Connection attempt refused*/
    CME_NET_ETIMEDOUT,              		/*17 Connection attempt timed out*/
    CME_NET_ECONNRESET,             	/*18 Connection reset*/
    CME_NET_ECONNABORTED,          	/*19 Connection aborted*/
    CME_NET_EPIPE,                 		/*20 Broken pipe*/
    CME_NET_ENETDOWN,              		/*21 Network subsystem unavailable*/
    CME_NET_EMAPP,                  		/*22 No more application available*/
    CME_NET_EBADAPP,                		/*23 Invalid application ID*/
    CME_NET_SOCKEXIST,              		/*24 There are exising sockets*/
    CME_NET_EINVAL,                 		/*25 Invalid operation*/
    CME_NET_EMSGSIZE,               		/*26 Message too long*/
    CME_NET_EBADOPTNAME,           	/*27 Bad operation name*/
    CME_NET_EBADOPTLEN,             	/*28 Bad operation len*/
    CME_NET_EBADOPTVAL,             	/*29 Bad operation val*/
    CME_NET_ENOMEM,                 		/*30 Out of memory*/
    CME_NET_ESHUTDOWN,              	/*31 Connection shutdown*/
    CME_NET_EURGENTFAILED,          	/*32 Urgent sendto failed*/
    CME_NET_ENOPROTOOPT            	/*33 the option is unknown at the level indiciated*/
}TcpIpCmeError;

#define TC30M_DEFAULT_CID 3
#define _TC30M_DEFAULT_CID_STR(x) #x
#define TC30M_DEFAULT_CID_STR(x) _TC30M_DEFAULT_CID_STR(x)
#define TC30M_DEFAULT_IPV4_MAX_LEN (16)
typedef struct 
{
    u8 	SysInitializeStat;
    u8  	ModemATPorStatus;
    u8  	NetworkReadyStat;
    u8 	RssiValue;
    s8		RssiDbm;
    u8 	Version[MAX_VERSION_LEN+2];
    u8      stat; //  Indicates the EPS registration status. 1 registered, home network, 5 registered, roaming
    u8      act; // access technology(ServiceType)
    u32 	sid; // tac area code 
    u32 	nid; // ci cell ID 
    u8  pdpType[8];
    u8  apn_attach[FDH_MAX_CMD_LEN - 14];
    u8 defaultcid;
    u8 defaultcidipgetflag;
    char defaultcidipstr[TC30M_DEFAULT_IPV4_MAX_LEN];
    u32 defaultcidipnum;
}ServerConfigParam;

typedef enum
{
   MSG_TYPE_NORMAL 		= 0,
   MSG_TYPE_UDP_END 	= 1,
   
   MSG_TYPE_ACK_UNKNOWN
}MsgTypeT;

// Variable declared
extern ServerConfigParam gModemParam;

/* Functions ------------------------------------------------------------------*/
extern void SetSysInitializeStat(u8 Status);
extern u8 GetSysInitializeStat(void);
extern void SetModemATPortStat(u8 Status);
extern u8 GetModemATPortStat(void);
extern void SetNetworkReadyStat(u8 Status);
extern u8 GetNetworkReadyStat(void);
extern uint8 GetNetworkRssiValue(void);
extern void SetNetworkRssiValue(uint8 value);
extern u8 GetNetworkRegistrationStat(void);
extern ServiceStatus GetNetServiceStatus(void);
extern NetworkStatT GetNetworkMachineStatus(void);
extern void SetNetworkMachineStatus(NetworkStatT status);
extern void CheckNetlorkTimerCallback(u8 Status);


#define UDPIP_SOCKET_MIN_NUM (1)
#define UDPIP_SOCKET_MAX_NUM (5)
#define UDP_SEND_QUEUE_LENGHT_MAX (6)
#define SMS_SEND_QUEUE_LENGHT_MAX (6)

typedef enum
{
    SOCKETCLOSE = 0,
    SOCKETOPEN = 1,
    SOCKETOPEN_REPORTONCE = 2,
} SocketOperationTypedef;

typedef enum
{
    SOCKET_CLOSE = 0,
} SocketStatusTypedef;

typedef struct
{
  uint8_t operation;
  uint8_t status;
  uint16_t LocalPort;
  uint16_t PortNum;
  char DestAddrP[MAX_IP_ADDR_LEN];
} UDPIPSocketTypedef;

extern uint8_t UdpSocketOpenIndicateFlag;
extern uint8_t UdpSocketCloseIndicateFlag;
extern uint8_t UdpSocketListenIndicateFlag;
extern UDPIPSocketTypedef UDPIPSocket[UDPIP_SOCKET_MAX_NUM];

typedef struct
{
    uint16_t datalen;
    uint8_t socketnum;
    char *buf;
} UdpSendUintTypedef;

typedef struct
{
    uint8_t putindex;
    uint8_t getindex;
    uint8_t numinqueue;
    UdpSendUintTypedef UDPIpSendUint[UDP_SEND_QUEUE_LENGHT_MAX];
} UdpSendQueueTypedef;

extern UdpSendQueueTypedef UdpSendQueue;

extern void UdpSendUnitIn(UdpSendQueueTypedef *pUdpSendQueue, UdpSendUintTypedef *pUDPIpSendUint);

extern void UdpSendUintOut(UdpSendQueueTypedef *pUdpSendQueue, UdpSendUintTypedef *pUDPIpSendUint);

typedef struct
{
    char *number;
    char *buf;
} SmsSendUintTypedef;

typedef struct
{
    uint8_t putindex;
    uint8_t getindex;
    uint8_t numinqueue;
    SmsSendUintTypedef SMSSendUint[SMS_SEND_QUEUE_LENGHT_MAX];
} SmsSendQueueTypedef;

extern SmsSendQueueTypedef SmsSendQueue;

extern void SmsSendUnitIn(SmsSendQueueTypedef *pSmsSendQueue, SmsSendUintTypedef *pSMSSendUint);

extern void SmsSendUintOut(SmsSendQueueTypedef *pSmsSendQueue, SmsSendUintTypedef *pSMSSendUint);


#define SMS_RECEIVE_SMS_STAT_MAX_LEN (32)
#define SMS_RECEIVE_NUMBER_MAX_LEN (32)
#define SMS_RECEIVE_DATE_TIME_MAX_LEN (32)
#define SMS_RECEIVE_TEXT_DATA_MAX_LEN (250 - SMS_RECEIVE_NUMBER_MAX_LEN) // WEDGE_MSG_QUE_DATA_LEN_MAX
typedef struct
{
    char smsnumber[SMS_RECEIVE_NUMBER_MAX_LEN];
    char smstextdata[SMS_RECEIVE_TEXT_DATA_MAX_LEN];
} SmsReceiveBufProcessTypedef;

typedef struct
{
    uint16_t smsindex;
    char smsstat[SMS_RECEIVE_SMS_STAT_MAX_LEN];
    char smsdatetime[SMS_RECEIVE_DATE_TIME_MAX_LEN];
    char smsnumber[SMS_RECEIVE_NUMBER_MAX_LEN];
    char smstextdata[SMS_RECEIVE_TEXT_DATA_MAX_LEN];
} SmsReceiveBufTypedef;

extern SmsReceiveBufTypedef SmsReceiveBuf;

extern uint8_t SmsRecFlag;

extern void UdpIpSocketOpen(uint8_t Socket_Num, uint16_t LocalPort, char *DestAddrP, uint16_t PortNum);

extern void UdpIpSocketClose(uint8_t Socket_Num);

extern void UdpIpSocketSendData(char *buffer, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* __NETWORK_H__ */

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2018. All rights reserved
                                End Of The File
*******************************************************************************/
