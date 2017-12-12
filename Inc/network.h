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
#include "ltecatm.h"
#include "parseatat.h"

/* Defines  ------------------------------------------------------------------*/
#define DEFAULT_RSSI_VALUE			0
#define MAX_IP_ADDR_LEN 				32
#define MAX_FILE_PATH_LEN 			64
#define NETWORK_LISTEN_PORT 		5555
#define CHECK_NETWORK_TIMEOUT 		2000
#define OTA_REPROGRAM_ACT_TIMEOUT	(10*1000)

#define MAX_VERSION_LEN                  			(22)
#define BARCODE_LEN_MAX          			(32)

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

typedef struct 
{
    u8 	SysInitializeStat;
    u8  	ModemATPorStatus;
    u8  	NetworkReadyStat;
    u8 	RssiValue;
    s8		RssiDbm;
    char   	SourceAddr[MAX_IP_ADDR_LEN];
    char 	DestinationAddr[MAX_IP_ADDR_LEN];
    u16 	DestinationPort;
    u8 	Version[MAX_VERSION_LEN+2];
    u8 	BleVersion[8+2];
    u8 	BleMacAddr[8+2];
    u32 	PRevInUse;
    u32 	MCC;
    u32 	IMSI_11_12;
    u32 	sid;
    u32 	nid;
    u32 	baseId;
    u32 	baseLat;
    u32 	baseLong;
    char 	barcode[BARCODE_LEN_MAX];
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
extern uint8 GetNetworkRssiValue(void);
extern void SetNetworkRssiValue(uint8 value);
extern ServiceStatus GetNetServiceStatus(void);
extern u8 GetSysInitializeStat(void);
extern NetworkStatT GetNetworkMachineStatus(void);
extern void SetNetworkMachineStatus(NetworkStatT status);
extern void CheckNetlorkTimerCallback(u8 Status);

#ifdef __cplusplus
}
#endif

#endif /* __NETWORK_H__ */

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2017. All rights reserved
                                End Of The File
*******************************************************************************/
