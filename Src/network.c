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
	"",
	"",
	0,
	"UNKNOWN",
	"UNKNOWN",
	"",
	0,0,0,0,0,0,0,0,
	"",
};

static const s8 RssiDbmTable[32] = 
{
	-113,-111,-109,-107,-105,-103,-101, -99,	// 8
	-97,  -95,  -93,  -91,  -89,  -87,  -85,  -83,	// 16
	-81,  -79,  -77,  -75,  -73,  -71,  -69,  -67,	// 24
	-65,  -63,  -61,  -59,  -57,  -55,  -53,  -51	// 32
};


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

void NetReadySocketProcess(uint32_t *pTimeout)
{
	static UdpSendUintTypedef UDPIpSendUint = {0};
	static SmsSendUintTypedef SMSSendUint = {0};

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
		char *socketnumstr[8] = {"1", "2", "3", "4", "5", "6", "7", "8"};

		UdpSendUintOut(&UdpSendQueue, &UDPIpSendUint);

		if ((UDPIpSendUint.buf != NULL) && (UDPIpSendUint.datalen != 0))
		{
			SendATCmd(GSM_CMD_SQNSSEND, GSM_CMD_TYPE_EVALUATE, (uint8_t *)socketnumstr[UDPIpSendUint.socketnum - 1]);

			udpsendoverflag = 0;

			*pTimeout = 10;
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
