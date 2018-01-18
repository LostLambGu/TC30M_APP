/*******************************************************************************
* File Name          : network.c
* Author             : Yangjie Gu
* Description        : This file provides all the network functions.

* History:
*  10/18/2017 : network V1.00
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "include.h"
#include "wedgecommonapi.h"
#include "network.h"
#include "sendatcmd.h"
#include "ltecatm.h"

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
	0,0,0,0,
	"",
	"",
	TC30M_DEFAULT_CID,
	FALSE,
	"",
	0xfffffffful
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

uint8_t OpenStatSocketNum = 0;
uint8_t SocketQue[UDPIP_SOCKET_MAX_NUM] = {0};

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

u8 GetNetworkReadyStat(void)
{
	return gModemParam.NetworkReadyStat;
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

u8 GetNetworkRegistrationStat(void)
{
	return gModemParam.stat;
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
        NetworkPrintf(DbgCtl.NetworkDbgInfoEn, "UdpSendUnitIn param err");
        return;
    }

    {
        // SystemDisableAllInterrupt();
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
        // SystemEnableAllInterrupt();
    }
}

void UdpSendUintOut(UdpSendQueueTypedef *pUdpSendQueue, UdpSendUintTypedef *pUDPIpSendUint)
{
    if ((pUdpSendQueue == NULL) || (pUDPIpSendUint == NULL))
    {
        NetworkPrintf(DbgCtl.NetworkDbgInfoEn, "UdpSendUintOut param err");
        return;
    }

    if (pUdpSendQueue->numinqueue > 0)
    {
        // SystemDisableAllInterrupt();
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
        // SystemEnableAllInterrupt();
    }

}

void SmsSendUnitIn(SmsSendQueueTypedef *pSmsSendQueue, SmsSendUintTypedef *pSMSSendUint)
{
    if ((pSmsSendQueue == NULL) || (pSMSSendUint == NULL))
    {
        NetworkPrintf(DbgCtl.NetworkDbgInfoEn, "SmsSendUnitIn param err");
        return;
    }

    {
        // SystemDisableAllInterrupt();
        SmsSendUintTypedef *ptemp = &(pSmsSendQueue->SMSSendUint[pSmsSendQueue->putindex % SMS_SEND_QUEUE_LENGHT_MAX]);
        ptemp->number = pSMSSendUint->number;
        ptemp->buf = pSMSSendUint->buf;

        pSmsSendQueue->putindex++;
        pSmsSendQueue->numinqueue++;
        if (pSmsSendQueue->numinqueue > SMS_SEND_QUEUE_LENGHT_MAX)
        {
            pSmsSendQueue->numinqueue = SMS_SEND_QUEUE_LENGHT_MAX;
        }
        // SystemEnableAllInterrupt();
    }
}

void SmsSendUintOut(SmsSendQueueTypedef *pSmsSendQueue, SmsSendUintTypedef *pSMSSendUint)
{
    if ((pSmsSendQueue == NULL) || (pSMSSendUint == NULL))
    {
        NetworkPrintf(DbgCtl.NetworkDbgInfoEn, "SmsSendUintOut param err");
        return;
    }

    if (pSmsSendQueue->numinqueue > 0)
    {
        // SystemDisableAllInterrupt();
        SmsSendUintTypedef *ptemp = &(pSmsSendQueue->SMSSendUint[pSmsSendQueue->getindex % SMS_SEND_QUEUE_LENGHT_MAX]);
        pSMSSendUint->number = ptemp->number;
        pSMSSendUint->buf = ptemp->buf;

        pSmsSendQueue->getindex++;
        pSmsSendQueue->numinqueue--;
        if (pSmsSendQueue->numinqueue > SMS_SEND_QUEUE_LENGHT_MAX)
        {
            pSmsSendQueue->numinqueue = 0;
        }
        // SystemEnableAllInterrupt();
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
        UDPIPSocket[Socket_Num - 1].LocalPort = 0xffff;
        UDPIPSocket[Socket_Num - 1].PortNum = 0xffff;
        memset(&(UDPIPSocket[Socket_Num - 1].DestAddrP), 0, MAX_IP_ADDR_LEN);
        
		UdpSocketCloseIndicateFlag |= (0x01 << (Socket_Num - 1));

		if (GetNetworkMachineStatus() == NET_CONNECTED_STAT)
		{
			// Reset Network Timer
			SoftwareTimerReset(&NetworkCheckTimer, CheckNetlorkTimerCallback, 10);
			SoftwareTimerStart(&NetworkCheckTimer);
		}
    }
    else
    {
        NetworkPrintf(DbgCtl.NetworkDbgInfoEn, "\r\nUdpIpSocket close param err");
    }
}

void UdpIpSocketSendData(char *buffer, uint16_t len)
{
    if ((buffer != NULL) && (len > 0))
    {
        if (UdpSocketListenIndicateFlag)
        {
            UdpSendUintTypedef UDPIpSendUint = {0};

            if (UdpSendQueue.numinqueue >= UDP_SEND_QUEUE_LENGHT_MAX)
            {
                NetworkPrintf(DbgCtl.NetworkDbgInfoEn, "\r\nUdpIpSocket send queue fulll");
                return;
            }

			UDPIpSendUint.datalen = len;
			// Default send to the socketnum which is opend
			UDPIpSendUint.socketnum = 0;
			UDPIpSendUint.buf = buffer;
			UdpSendUnitIn(&UdpSendQueue, &UDPIpSendUint);

			if (GetNetworkMachineStatus() == NET_CONNECTED_STAT)
			{
				// Reset Network Timer
				SoftwareTimerReset(&NetworkCheckTimer, CheckNetlorkTimerCallback, 10);
				SoftwareTimerStart(&NetworkCheckTimer);
			}
		}
        else
        {
            NetworkPrintf(DbgCtl.NetworkDbgInfoEn, "\r\nUdpIpSocket send soket closed");
        }
    }
    else
    {
		if (buffer != NULL)
		{
			WedgeBufPoolFree(buffer);
		}

        NetworkPrintf(DbgCtl.NetworkDbgInfoEn, "\r\nUdpIpSocket send param err");
    }
}

// message
void SendMessage(char *Num, char *buf)
{
	if ((NULL != Num) && (NULL != buf))
	{
		uint16_t numlen = strlen(Num);
		uint16_t buflen = strlen(buf);
		SmsSendUintTypedef SmsSendUint = {0};

		if (SmsSendQueue.numinqueue >= SMS_SEND_QUEUE_LENGHT_MAX)
		{
			NetworkPrintf(DbgCtl.NetworkDbgInfoEn, "\r\nSMS send queue full");
			return;
		}

		if ((0 == numlen) || (0 == buflen))
		{
			NetworkPrintf(DbgCtl.NetworkDbgInfoEn, "\r\nSMS send len err");

			WedgeBufPoolFree(Num);
			WedgeBufPoolFree(buf)

			return;
		}

		SmsSendUint.number = Num;
		SmsSendUint.buf = buf;

		SmsSendUnitIn(&SmsSendQueue, &SmsSendUint);

		if (GetNetworkMachineStatus() == NET_CONNECTED_STAT)
		{
			// Reset Network Timer
			SoftwareTimerReset(&NetworkCheckTimer, CheckNetlorkTimerCallback, 10);
			SoftwareTimerStart(&NetworkCheckTimer);
		}
	}
	else
	{
		NetworkPrintf(DbgCtl.NetworkDbgInfoEn, "\r\nSMS send param err");

		if (Num != NULL)
		{
			WedgeBufPoolFree(Num);
		}

		if (buf != NULL)
		{
			WedgeBufPoolFree(buf);
		}

		return;
	}
}

void DeleteAllSMS(void)
{
	SendATCmd(GSM_CMD_CMGD, GSM_CMD_TYPE_EVALUATE , "1,4");
}

static void UART3SendHexData(char *string, uint16_t slen)
{
	// HAL_UART_Transmit(&huart1, "\r\n--->>", 7, UART_SEND_DATA_TIMEOUT);
	// HAL_UART_Transmit(&huart1, (uint8_t *)string, slen, UART_SEND_DATA_TIMEOUT);
	HAL_UART_Transmit(&huart3, (uint8_t *)string, slen, UART_SEND_DATA_TIMEOUT);
	DebugPrintf(DbgCtl.NetworkDbgInfoEn, "\r\nLTE: SEND Hex Data Len: (%d)", slen);
}

#define SMS_SEND_OVER_TIME_MS (10000)
#define UDP_SEND_OVER_TIME_MS (10000)
void NetReadySocketProcess(uint32_t *pTimeout)
{
	static UdpSendUintTypedef UDPIpSendUint = {0};
	static SmsSendUintTypedef SMSSendUint = {0};
	static uint32_t udpsendovertime = 0;
	static uint32_t smsdendovertime = 0;

	if (UdpSocketOpenIndicateFlag)
	{
		uint8_t i = 0;

		for (i = 0; i < UDPIP_SOCKET_MAX_NUM; i++)
		{
			if (UdpSocketOpenIndicateFlag & (0x01 << i))
			{
				uint8_t ListenSting[128];

				memset(ListenSting, 0, sizeof(ListenSting));
				sprintf((char *)ListenSting, "%d,%d,480,90,600,50", i + 1, TC30M_DEFAULT_CID);
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

		return;
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

		return;
	}

	if ((UdpSendQueue.numinqueue != 0) && (udpsendoverflag == 1))
	{
		memset(&UDPIpSendUint, 0, sizeof(UDPIpSendUint));
		UdpSendUintOut(&UdpSendQueue, &UDPIpSendUint);

		if ((UDPIpSendUint.buf != NULL) && (UDPIpSendUint.datalen != 0))
		{
			if (UdpSocketListenIndicateFlag)
			{
				int8_t k = 0;

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
				SendATCmd(GSM_CMD_SQNSSEND, GSM_CMD_TYPE_EVALUATE, (uint8_t *)socketnumstr[SocketQue[OpenStatSocketNum - 1] - 1]);
				OpenStatSocketNum--;
				udpsendoverflag = 0;
				udpsendovertime = HAL_GetTick();
				*pTimeout = 10;
			}
		}

		return;
	}

	if (udpsendoverflag == 0)
	{
		if (UDP_SEND_OVER_TIME_MS < (HAL_GetTick() - udpsendovertime))
		{
			DebugLog("--->>>Network: Udp send data overtime");
			udpsendoverflag = 1;

			if (UDPIpSendUint.buf != NULL)
				WedgeBufPoolFree(UDPIpSendUint.buf);
			*pTimeout = 10;
			return;
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

		if (OpenStatSocketNum > 0)
		{
			SendATCmd(GSM_CMD_SQNSSEND, GSM_CMD_TYPE_EVALUATE, (uint8_t *)socketnumstr[SocketQue[OpenStatSocketNum - 1] - 1]);
			OpenStatSocketNum--;
			*pTimeout = 10;
		}
		else
		{
			if (UDPIpSendUint.buf != NULL)
				WedgeBufPoolFree(UDPIpSendUint.buf);
			udpsendoverflag = 1;
			SetUDPDataCanSendStat(FALSE);
		}

		return;
	}

	if ((SmsSendQueue.numinqueue != 0) && (smssendoverflag == 1))
	{
		DebugLog("--->>> SmsSendQueue.numinqueue (%d)", SmsSendQueue.numinqueue);
		memset(&SMSSendUint, 0, sizeof(SMSSendUint));
		SmsSendUintOut(&SmsSendQueue, &SMSSendUint);

		if ((SMSSendUint.buf != NULL) && (SMSSendUint.number != NULL))
		{
			SendATCmd(GSM_CMD_CMGS, GSM_CMD_TYPE_EVALUATE, (uint8_t *)SMSSendUint.number);

			smssendoverflag = 0;
			smsdendovertime = HAL_GetTick();

			*pTimeout = 10;
		}

		return;
	}

	if (smssendoverflag == 0)
	{
		if (SMS_SEND_OVER_TIME_MS < (HAL_GetTick() - smsdendovertime))
		{
			DebugLog("--->>>Network: Sms send data overtime");
			smssendoverflag = 1;
			if (SMSSendUint.buf != NULL)
				WedgeBufPoolFree(SMSSendUint.buf);

			if (SMSSendUint.number != NULL)
				WedgeBufPoolFree(SMSSendUint.number);
			*pTimeout = 10;
			return;
		}
	}

	if (GetSMSDataCanSendStat() == TRUE)
	{
		UART3SendHexData(SMSSendUint.buf, strlen(SMSSendUint.buf));

		UART3SendHexData("\x1a", 1);

		if (SMSSendUint.buf != NULL)
			WedgeBufPoolFree(SMSSendUint.buf);

		if (SMSSendUint.number != NULL)
			WedgeBufPoolFree(SMSSendUint.number);

		smssendoverflag = 1;
		SetSMSDataCanSendStat(FALSE);

		return;
	}
}

void CheckNetlorkTimerCallback(u8 Status)
{
	u32 Timeout = CHECK_NETWORK_TIMEOUT;
	static u8 noservicecount = 0;
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
						noservicecount++;
						if (noservicecount >= 10)
						{
							SetNetworkMachineStatus(NET_DISCONNECT_STAT);
						}
						else
						{
							NetworkPrintf(DbgCtl.NetworkDbgInfoEn, "\r\nNET_CONNECTED_STAT Update System Signal");
							UpdateSystemSignalStatus(7);
							Timeout = TimeMsec(5);
						}
					}
					else
					{
						// NetworkPrintf(DbgCtl.NetworkDbgInfoEn, "\r\nNET_CONNECTED_STAT NetReadySocketProcess");
						noservicecount = 0;
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
								UDPIPSocket[j].status = SOCKET_CLOSE;
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

					if (GetNetworkReadyStat() != FALSE)
					{
						SetNetworkMachineStatus(NET_FREE_IDLE_STAT);
					}

					// After timeout go to next status
					SetNetworkMachineStatus(NET_DISCONNECTED_STAT);
					// Set timeout
					Timeout = TimeMsec(10);
				}
				break;

				case NET_DISCONNECTED_STAT:
				{
					if (GetNetworkReadyStat() != FALSE)
					{
						SetNetworkMachineStatus(NET_FREE_IDLE_STAT);
					}

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

			// If AT port is not ok, for a long while, should restart it

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
    Copyrights (C) Asiatelco Technologies Co., 2003-2018. All rights reserved
                                End Of The File
*******************************************************************************/
