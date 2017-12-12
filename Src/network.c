/*******************************************************************************
* File Name          : network.c
* Author             : Yangjie Gu
* Description        : This file provides all the network functions.

* History:
*  10/18/2017 : network V1.00
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "include.h"
#include "eventmsgque_process.h"

/* Defines -------------------------------------------------------------------*/
#undef NRCMD
#define NRCMD (1)

#define NetworkPrintf DebugPrintf

#define UDP_HEX_SEND_NUM_MAX (64)

/* Variables -----------------------------------------------------------------*/
extern DebugCtlPrarm DbgCtl;

uint8_t udpsendoverflag = 1;
uint8_t smssendoverflag = 1;

static NetworkStatT NetStateMachine = NET_FREE_IDLE_STAT;
static u8 NetWorkServiceCount = 0;

ServerConfigParam gModemParam = 
{
	FALSE,
	FALSE,
	FALSE,
	DEFAULT_RSSI_VALUE,
	-113,
	"UNKNOWN",
};

static const s8 RssiDbmTable[32] = 
{
	-113,-111,-109,-107,-105,-103,-101, -99,	// 8
	-97,  -95,  -93,  -91,  -89,  -87,  -85,  -83,	// 16
	-81,  -79,  -77,  -75,  -73,  -71,  -69,  -67,	// 24
	-65,  -63,  -61,  -59,  -57,  -55,  -53,  -51	// 32
};

const char *socketnumstr[8] = {"1", "2", "3", "4", "5", "6", "7", "8"};

uint8_t UdpSocketOpenIndicateFlag = 0;
uint8_t UdpSocketCloseIndicateFlag = 0;
uint8_t UdpSocketListenIndicateFlag = 0;
UDPIPSocketTypedef UDPIPSocket[UDPIP_SOCKET_MAX_NUM] = {0};

UdpSendQueueTypedef UdpSendQueue = {0};
SmsSendQueueTypedef SmsSendQueue = {0};
SmsReceiveBufTypedef SmsReceiveBuf = {0};
uint8_t SmsRecFlag = 0;


/* Function definition -------------------------------------------------------*/
void SetSysInitializeStat(u8 Status)
{
	gModemParam.SysInitializeStat = Status;
}

u8 GetSysInitializeStat(void)
{
	return gModemParam.SysInitializeStat;
}

void SetModemATPortStat(u8 Status)
{
	gModemParam.ModemATPorStatus = Status;
}

u8 GetModemATPortStat(void)
{
	return gModemParam.ModemATPorStatus;
}

void SetNetworkReadyStat(u8 Status)
{
	gModemParam.NetworkReadyStat = Status;
}

uint8 GetNetworkRssiValue(void)
{
	return gModemParam.RssiValue;
}

void SetNetworkRssiValue(uint8 value)
{
	gModemParam.RssiValue = value;

	
	// Table - rssi
	// 0 -113 dBm or less
	// 1 -111 dBm
	// 2 -109 dBm
	// ... ...
	// 30 -53 dBm
	// 31 -51 dBm or greater
	// 99 not known or not detectable

	if(value == 99 || value >= 32)
		gModemParam.RssiDbm = -113;
	else
		gModemParam.RssiDbm = RssiDbmTable[value];

	// Print Out
	NetworkPrintf(DbgCtl.NetworkDbgInfoEn,"\r\n[%s] NET: Rssi dbm(%d)", \
		FmtTimeShow(), gModemParam.RssiDbm);
}

ServiceStatus GetNetServiceStatus(void)
{
	if(gModemParam.RssiValue > 5 && \
		gModemParam.RssiValue != 99 && \
	gModemParam.NetworkReadyStat == TRUE)
	{
		return IN_SERVICE;
	}
	else
	{
		return NO_SERVICE;
	}
}

NetworkStatT GetNetworkMachineStatus(void)
{
	return NetStateMachine;
}

void SetNetworkMachineStatus(NetworkStatT status)
{
	NetStateMachine = status;
}

void UdpSendUnitIn(UdpSendQueueTypedef *pUdpSendQueue, UdpSendUintTypedef *pUDPIpSendUint)
{
    if ((pUdpSendQueue == NULL) || (pUDPIpSendUint == NULL))
    {
        QUEUE_PROCESS_LOG("UdpSendUnitIn param err");
        return;
    }

    {
        SystemDisableAllInterrupt();
        UdpSendUintTypedef *ptemp = &(pUdpSendQueue->UDPIpSendUint[pUdpSendQueue->putindex % UDP_SEND_QUEUE_LENGHT_MAX]);
        ptemp->datalen = pUDPIpSendUint->datalen;
        ptemp->socketnum = pUDPIpSendUint->socketnum;
        ptemp->buf = pUDPIpSendUint->buf;

        pUdpSendQueue->putindex++;
        pUdpSendQueue->numinqueue++;
        if (pUdpSendQueue->numinqueue > UDP_SEND_QUEUE_LENGHT_MAX)
        {
            pUdpSendQueue->numinqueue = UDP_SEND_QUEUE_LENGHT_MAX;
        }
        SystemEnableAllInterrupt();
    }
}

void UdpSendUintOut(UdpSendQueueTypedef *pUdpSendQueue, UdpSendUintTypedef *pUDPIpSendUint)
{
    if ((pUdpSendQueue == NULL) || (pUDPIpSendUint == NULL))
    {
        QUEUE_PROCESS_LOG("UdpSendUintOut param err");
        return;
    }

    if (pUdpSendQueue->numinqueue > 0)
    {
        SystemDisableAllInterrupt();
        UdpSendUintTypedef *ptemp = &(pUdpSendQueue->UDPIpSendUint[pUdpSendQueue->getindex % UDP_SEND_QUEUE_LENGHT_MAX]);
        pUDPIpSendUint->datalen = ptemp->datalen;
        pUDPIpSendUint->socketnum = ptemp->socketnum;
        pUDPIpSendUint->buf = ptemp->buf;

        pUdpSendQueue->getindex++;
        pUdpSendQueue->numinqueue--;
        if (pUdpSendQueue->numinqueue > UDP_SEND_QUEUE_LENGHT_MAX)
        {
            pUdpSendQueue->numinqueue = 0;
        }
        SystemEnableAllInterrupt();
    }

}

void SmsSendUnitIn(SmsSendQueueTypedef *pSmsSendQueue, SmsSendUintTypedef *pSMSSendUint)
{
    if ((pSmsSendQueue == NULL) || (pSMSSendUint == NULL))
    {
        QUEUE_PROCESS_LOG("SmsSendUnitIn param err");
        return;
    }

    {
        SystemDisableAllInterrupt();
        SmsSendUintTypedef *ptemp = &(pSmsSendQueue->SMSSendUint[pSmsSendQueue->putindex % SMS_SEND_QUEUE_LENGHT_MAX]);
        ptemp->number = pSMSSendUint->number;
        ptemp->buf = pSMSSendUint->buf;

        pSmsSendQueue->putindex++;
        pSmsSendQueue->numinqueue++;
        if (pSmsSendQueue->numinqueue > SMS_SEND_QUEUE_LENGHT_MAX)
        {
            pSmsSendQueue->numinqueue = SMS_SEND_QUEUE_LENGHT_MAX;
        }
        SystemEnableAllInterrupt();
    }
}

void SmsSendUintOut(SmsSendQueueTypedef *pSmsSendQueue, SmsSendUintTypedef *pSMSSendUint)
{
    if ((pSmsSendQueue == NULL) || (pSMSSendUint == NULL))
    {
        QUEUE_PROCESS_LOG("SmsSendUintOut param err");
        return;
    }

    if (pSmsSendQueue->numinqueue > 0)
    {
        SystemDisableAllInterrupt();
        SmsSendUintTypedef *ptemp = &(pSmsSendQueue->SMSSendUint[pSmsSendQueue->getindex % SMS_SEND_QUEUE_LENGHT_MAX]);
        pSMSSendUint->number = ptemp->number;
        pSMSSendUint->buf = ptemp->buf;

        pSmsSendQueue->getindex++;
        pSmsSendQueue->numinqueue--;
        if (pSmsSendQueue->numinqueue > SMS_SEND_QUEUE_LENGHT_MAX)
        {
            pSmsSendQueue->numinqueue = 0;
        }
        SystemEnableAllInterrupt();
    }
}

// socket
void UdpIpSocketOpen(uint8_t Socket_Num, uint16_t LocalPort, char *DestAddrP, uint16_t PortNum)
{
    if ((Socket_Num >= UDPIP_SOCKET_MIN_NUM) && (Socket_Num <= UDPIP_SOCKET_MAX_NUM) && (DestAddrP != NULL))
    {
        // if (GetNetworkMachineStatus() != NET_CONNECTED_STAT)
        // {
        //     at_printfDebug("\r\nNetwork not ready", Socket_Num);
        //     return;
        // }

        if (UDPIPSocket[Socket_Num - 1].status != SOCKET_CLOSE)
        {
            NetworkPrintf(DbgCtl.NetworkDbgInfoEn, "\r\nUdpIpSocket %d not close", Socket_Num);
            return;
        }

        UDPIPSocket[Socket_Num - 1].operation = SOCKETOPEN_REPORTONCE;
        UDPIPSocket[Socket_Num - 1].LocalPort = LocalPort;
        UDPIPSocket[Socket_Num - 1].PortNum = PortNum;
        strncpy(UDPIPSocket[Socket_Num - 1].DestAddrP, DestAddrP, MAX_IP_ADDR_LEN);

		UdpSocketOpenIndicateFlag |= (0x01 << (Socket_Num - 1));

		if (GetNetworkMachineStatus() == NET_CONNECTED_STAT)
		{
			// Reset Network Timer
			SoftwareTimerReset(&NetworkCheckTimer, CheckNetlorkTimerCallback, 10);
			SoftwareTimerStart(&NetworkCheckTimer);
		}
	}
    else
    {
        NetworkPrintf(DbgCtl.NetworkDbgInfoEn, "\r\nUdpIpSocket open param err");
    }
}

void UdpIpSocketClose(uint8_t Socket_Num)
{
    if ((Socket_Num >= UDPIP_SOCKET_MIN_NUM) && (Socket_Num <= UDPIP_SOCKET_MAX_NUM))
    {
        if (UDPIPSocket[Socket_Num - 1].status == SOCKET_CLOSE)
        {
            NetworkPrintf(DbgCtl.NetworkDbgInfoEn, "\r\nUdpIpSocket %d already closed", Socket_Num);
            return;
        }

        UDPIPSocket[Socket_Num - 1].operation = SOCKETCLOSE;
        UDPIPSocket[Socket_Num - 1].status = SOCKET_CLOSE;
        
		UdpSocketCloseIndicateFlag |= (0x01 << (Socket_Num - 1));
    }
    else
    {
        NetworkPrintf(DbgCtl.NetworkDbgInfoEn, "\r\nUdpIpSocket close param err");
    }
}

void UdpIpSocketSendData(uint8 Socket_Num, char *buffer, uint16 len)
{
    if ((Socket_Num >= UDPIP_SOCKET_MIN_NUM) && (Socket_Num <= UDPIP_SOCKET_MAX_NUM)
     && (buffer != NULL) && (len > 0))
    {
        if (UDPIPSocket[Socket_Num - 1].status != SOCKET_CLOSE)
        {
            UdpSendUintTypedef UDPIpSendUint = {0};

            if (UdpSendQueue.numinqueue >= UDP_SEND_QUEUE_LENGHT_MAX)
            {
                NetworkPrintf(DbgCtl.NetworkDbgInfoEn, "\r\nUdpIpSocket send queue fulll");
                return;
            }

            char *tmp = pvPortMalloc(len);

            if (tmp == NULL)
            {
                NetworkPrintf(DbgCtl.NetworkDbgInfoEn, "\r\nUdpIpSocket send malloc fail");
                return;
            }
            else
            {
                memset(tmp, 0 , len);
                memcpy(tmp , buffer, len);
                UDPIpSendUint.datalen = len;
                UDPIpSendUint.socketnum = Socket_Num;
                UDPIpSendUint.buf = tmp;
                UdpSendUnitIn(&UdpSendQueue, &UDPIpSendUint);

				if (GetNetworkMachineStatus() == NET_CONNECTED_STAT)
				{
					// Reset Network Timer
					SoftwareTimerReset(&NetworkCheckTimer, CheckNetlorkTimerCallback, 10);
					SoftwareTimerStart(&NetworkCheckTimer);
				}
			}
        }
        else
        {
            NetworkPrintf(DbgCtl.NetworkDbgInfoEn, "\r\nUdpIpSocket send soket closed");
        }
    }
    else
    {
        NetworkPrintf(DbgCtl.NetworkDbgInfoEn, "\r\nUdpIpSocket send param err");
    }
}

void NetReadySocketProcess(uint32_t *pTimeout)
{
	static UdpSendUintTypedef UDPIpSendUint = {0};
	static SmsSendUintTypedef SMSSendUint = {0};
	static uint8_t OpenStatSocketNum = 0;
	static uint8_t SocketQue[UDPIP_SOCKET_MAX_NUM] = {0};

	if (UdpSocketOpenIndicateFlag)
	{
		uint8_t i = 0;
		for (i = 0; i < UDPIP_SOCKET_MAX_NUM; i++)
		{
			if (UdpSocketOpenIndicateFlag & (0x01 << i))
			{
				uint8_t ListenSting[128];

				memset(ListenSting, 0, sizeof(ListenSting));
				sprintf((char *)ListenSting, "%d,3,480,90,600,50", i + 1);
				// Create socket
				SendATCmd(GSM_CMD_SQNSCFG, GSM_CMD_TYPE_EVALUATE, ListenSting);

				memset(ListenSting, 0, sizeof(ListenSting));
				sprintf((char *)ListenSting, "%d,1,0,0", i + 1);
				SendATCmd(GSM_CMD_SQNSCFGEXT, GSM_CMD_TYPE_EVALUATE, ListenSting);

				memset(ListenSting, 0, sizeof(ListenSting));
				sprintf((char *)ListenSting, "%d,%d,%d,\"%s\",%d,%d,%d",
						i + 1,
						UDP,
						UDPIPSocket[i].PortNum,
						UDPIPSocket[i].DestAddrP,
						0,
						UDPIPSocket[i].LocalPort,
						1);
				SendATCmd(GSM_CMD_SQNSDIAL, GSM_CMD_TYPE_EVALUATE, ListenSting);

				UdpSocketOpenIndicateFlag &= ~(0x01 << i);
				UdpSocketListenIndicateFlag |= (0x01 << i);
				// Check socket status
				SendATCmd(GSM_CMD_SQNSS, GSM_CMD_TYPE_EXECUTE, NULL);
			}
		}
	}

	if (UdpSocketCloseIndicateFlag)
	{
		uint8_t j = 0;
		for (j = 0; j < UDPIP_SOCKET_MAX_NUM; j++)
		{
			if (UdpSocketCloseIndicateFlag & (0x01 << j))
			{
				uint8_t SocSting[16];
				memset(SocSting, 0, sizeof(SocSting));

				sprintf((char *)SocSting, "%d", j + 1);

				SendATCmd(GSM_CMD_SQNSH, GSM_CMD_TYPE_EVALUATE, SocSting);

				UdpSocketCloseIndicateFlag &= ~(0x01 << j);
				UdpSocketListenIndicateFlag &= ~(0x01 << j);
			}
		}
	}

	if ((UdpSendQueue.numinqueue != 0) && (udpsendoverflag == 1))
	{
		UdpSendUintOut(&UdpSendQueue, &UDPIpSendUint);

		if ((UDPIpSendUint.buf != NULL) && (UDPIpSendUint.datalen != 0))
		{
			if (UdpSocketListenIndicateFlag)
			{
				uint8_t k = 0;

				memset(SocketQue, 0, sizeof(SocketQue));

				for (k = UDPIP_SOCKET_MAX_NUM; k > 0; k--)
				{
					if (UdpSocketListenIndicateFlag & (0x01 << (k - 1)))
					{
						if (UDPIPSocket[k - 1].status != 0)
						{
							SocketQue[OpenStatSocketNum] = k;
							OpenStatSocketNum++;
						}
					}
				}
			}

			if (OpenStatSocketNum > 0)
			{
				SendATCmd(GSM_CMD_SQNSSEND, GSM_CMD_TYPE_EVALUATE, (uint8_t *)socketnumstr[SocketQue[OpenStatSocketNum] - 1]);
				OpenStatSocketNum--;
				udpsendoverflag = 0;
				*pTimeout = 10;
			}
		}
	}

	if (GetUDPDataCanSendStat() == TRUE)
	{
		uint8_t i = 0, count = 0, left = 0;
		char *tmp = UDPIpSendUint.buf;

		if (tmp != NULL)
		{
			count = UDPIpSendUint.datalen / UDP_HEX_SEND_NUM_MAX;
			left = UDPIpSendUint.datalen % UDP_HEX_SEND_NUM_MAX;

			for (i = 0; i < count; i++)
			{
				UART3SendHexData(tmp + (i * UDP_HEX_SEND_NUM_MAX), UDP_HEX_SEND_NUM_MAX);
			}

			if (left)
			{
				UART3SendHexData(tmp + (i * UDP_HEX_SEND_NUM_MAX), left);
			}
		}

		UART3SendHexData("\x1a", 1);

		if (UDPIpSendUint.buf != NULL)
			vPortFree(UDPIpSendUint.buf);
		udpsendoverflag = 1;
		SetUDPDataCanSendStat(FALSE);
	}

	if ((SmsSendQueue.numinqueue != 0) && (smssendoverflag == 1))
	{
		SmsSendUintOut(&SmsSendQueue, &SMSSendUint);

		if ((SMSSendUint.buf != NULL) && (SMSSendUint.number != NULL))
		{
			SendATCmd(GSM_CMD_CMGS, GSM_CMD_TYPE_EVALUATE, (uint8_t *)SMSSendUint.number);

			smssendoverflag = 0;

			*pTimeout = 10;
		}
	}

	if (GetSMSDataCanSendStat() == TRUE)
	{
		UART3SendHexData(SMSSendUint.buf, strlen(SMSSendUint.buf));

		UART3SendHexData("\x1a", 1);

		if (SMSSendUint.buf != NULL)
			vPortFree(SMSSendUint.buf);

		if (SMSSendUint.number != NULL)
			vPortFree(SMSSendUint.number);

		smssendoverflag = 1;
		SetSMSDataCanSendStat(FALSE);
	}
}

void CheckNetlorkTimerCallback(u8 Status)
{
	u32 Timeout = CHECK_NETWORK_TIMEOUT;
	// Check
	if(Status == 1)
	{
		// Check Status
		if(GetModemATPortStat() == TRUE)
		{
			switch(GetNetworkMachineStatus())
			{
				case NET_FREE_IDLE_STAT:
				{
					SetNetworkMachineStatus(NET_CHECK_NETWORK_STATUS);

					NetworkPrintf(DbgCtl.NetworkDbgInfoEn, "\r\nNET_FREE_IDLE_STAT");
					Timeout = 300;
					// Update Signal
					UpdateSystemSignalStatus(6);
					// Clear count
					NetWorkServiceCount = 0;
				}
				break;

				case NET_IDLE_AIRPLANE_STAT:
				{
					NetworkPrintf(DbgCtl.NetworkDbgInfoEn, "\r\nNET_IDLE_AIRPLANE_STAT");
					SetNetworkMachineStatus(NET_FREE_IDLE_STAT);
					Timeout = 10;
				}
				break;

				case NET_CHECK_NETWORK_STATUS:
				{
					NetworkPrintf(DbgCtl.NetworkDbgInfoEn, "\r\nNET_CHECK_NETWORK_STATUS");

					// SoftwareTimerReset(&CheckRssiTimer, CheckRssiTimerCallback, CHECK_RSSI_TIMEOUT);
					// SoftwareTimerStart(&CheckRssiTimer);

					if (GetNetServiceStatus() == IN_SERVICE)
					{
						NetworkPrintf(DbgCtl.NetworkDbgInfoEn, "\r\nIN_SERVICE");
						// Go next
						SetNetworkMachineStatus(NET_CONNECTED_STAT);
						Timeout = 10;
					}
					else
					{
						NetworkPrintf(DbgCtl.NetworkDbgInfoEn, "\r\nNO_SERVICE");
						// Update Signal
						UpdateSystemSignalStatus(7);
						Timeout = TimeMsec(5);
						// Check timeout
						NetWorkServiceCount++;
						if (NetWorkServiceCount >= 18) // 90 seconds
						{
							NetWorkServiceCount = 0;
							SetNetworkMachineStatus(NET_DISCONNECTED_STAT);
						}
					}
				}
				break;

				case NET_CONNECTED_STAT:
				{
					// MmiIPOPEN()
					// MmiIPCLOSE()
					// Timeout = TimeMsec(5);
					Timeout = 100;
						// NetworkPrintf(DbgCtl.NetworkDbgInfoEn, "\r\nNET_CONNECTED_STAT");
					// Check Status
					if(GetNetServiceStatus() == NO_SERVICE)
					{
						SetNetworkMachineStatus(NET_DISCONNECT_STAT);
					}
					else
					{
						NetReadySocketProcess(&Timeout);
					}
					
					// If network is conneted
					// Wait here ...............
				}
				break;

				case NET_DISCONNECT_STAT:
				{
					// SoftwareTimerStop(&CheckRssiTimer);

					NetworkPrintf(DbgCtl.NetworkDbgInfoEn, "\r\nNET_DISCONNECT_STAT");
					// Close UDP Sokcet
					if (UdpSocketListenIndicateFlag)
					{
						uint8_t j = 0;
						for (j = 0; j < UDPIP_SOCKET_MAX_NUM; j++)
						{
							if (UdpSocketListenIndicateFlag & (0x01 << j))
							{
								uint8_t SocSting[4];
								memset(SocSting, 0, sizeof(SocSting));

								sprintf((char *)SocSting, "%d", j + 1);

								SendATCmd(GSM_CMD_SQNSH, GSM_CMD_TYPE_EVALUATE, SocSting);

								UdpSocketCloseIndicateFlag &= ~(0x01 << j);
								UdpSocketListenIndicateFlag &= ~(0x01 << j);
							}
						}
					}

					// Go next
					SetNetworkMachineStatus(NET_WAITT_DISCONNECTED_STAT);
					SendATCmd(GSM_CMD_SQNSS, GSM_CMD_TYPE_EXECUTE, NULL);

					Timeout = 100;
				}
				break;

				case NET_WAITT_DISCONNECTED_STAT:
				{
					NetworkPrintf(DbgCtl.NetworkDbgInfoEn, "\r\nNET_WAITT_DISCONNECTED_STAT");
					// After timeout go to next status
					SetNetworkMachineStatus(NET_DISCONNECTED_STAT);
					// Set timeout
					Timeout = TimeMsec(10);
				}
				break;

				case NET_DISCONNECTED_STAT:
				{

					NetworkPrintf(DbgCtl.NetworkDbgInfoEn, "\r\nNET_DISCONNECTED_STAT");
					Timeout = TimeMsec(10 /*5*/);
				}
				break;

				default:
				break;
			}
		}
		else
		{
			Timeout = TimeMsec(5);

			NetworkPrintf(DbgCtl.NetworkDbgInfoEn,"\r\n[%s] NET: WATING(%d) Sys(%d) AT(%d)", \
			FmtTimeShow(), \
			Status, \
			GetSysInitializeStat(), \
			GetModemATPortStat());
		}
	}
	else
	{
		// Print Out
		NetworkPrintf(DbgCtl.NetworkDbgInfoEn,"\r\n[%s] NET: UNKNOWN ID(%d)", FmtTimeShow(),Status);
	}
	// Reset Network Timer
	SoftwareTimerReset(&NetworkCheckTimer,CheckNetlorkTimerCallback,Timeout);
	SoftwareTimerStart(&NetworkCheckTimer);
}

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2017. All rights reserved
                                End Of The File
*******************************************************************************/
