/*******************************************************************************
* File Name          : Ublox_driver.c
* Author             : Lq Zhang
* Description        : This file provides all the Ublox_driver functions.

* History:
*  1/17/2016 : Ublox_driver V1.00
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "ublox_driver.h"
#include "i2c.h"
#include "iocontrol.h"
#include "wedgeeventalertflow.h"
#include "wedgecommonapi.h"

#define UbloxPrintf DebugPrintf

extern I2C_HandleTypeDef hi2c2;
/* Private variables ---------------------------------------------------------*/
GPIO_InitTypeDef UBLOXGPIO;
u16 UbloxCheckStatTimeout = CHECK_UBLOX_STAT_TIMEOUT;
PswGpsNmeaStreamMsgT MsgBuffer;
ValGpsInfo GpsInfo;
ValNMEADataT gLocMsg;
u8 ATUbloxTestFlag = FALSE;

/* Public functions ----------------------------------------------------------*/
void UbloxGPSStart(void)
{
	UbloxPowerEnControl(ENABLE);
	ATUbloxTestFlag = TRUE;
	SoftwareTimerReset(&RegularTimer, CheckRegularTimerCallback, UbloxCheckStatTimeout);
	SoftwareTimerStart(&RegularTimer);
}

void UbloxGPSStop(void)
{
	ATUbloxTestFlag = FALSE;
	SoftwareTimerStop(&RegularTimer);
	UbloxPowerEnControl(DISABLE);
}

void UbloxPowerEnControl(FunctionalState Status)
{
	// Active high
	if (Status == ENABLE)
	{
		HAL_GPIO_WritePin(UBLOX_POWER_PORT, UBLOX_POWER_PIN, GPIO_PIN_SET);
	}
	else if (Status == DISABLE)
	{
		HAL_GPIO_WritePin(UBLOX_POWER_PORT, UBLOX_POWER_PIN, GPIO_PIN_RESET);
	}
}

void UbloxI2cInit(void)
{
	// Power Enable
	UBLOX_SET_POWER_OUT();
	//UbloxPowerEnControl(ENABLE);

	// Print Out
	UbloxPrintf(DbgCtl.UbloxDbgInfoEn, "\r\n[%s] Ublox: Hardware Init", FmtTimeShow());
}

void ReadUbloxData(void)
{
	u8 LoopCount = 0;
	u8 I2CRXBuffer[4] = {'\0'};

	// Read NMEA Data Length
	if (HAL_I2C_Mem_Read(&hi2c2, (u16)UBLOX_I2C_DRV_ADDRESS, (u16)UBLOX_LEN_START_ADDR, I2C_MEMADD_SIZE_8BIT, (u8 *)I2CRXBuffer, 2, UBLOX_OPERATE_TIMEOUT) != HAL_OK)
	{
		UbloxPrintf(DbgCtl.UbloxDbgInfoEn, "\r\n[%s] Ublox: RD Len First Fail", FmtTimeShow());
	}
	else
	{
		memset((void *)&MsgBuffer, 0, sizeof(MsgBuffer));
		MsgBuffer.DataLen = I2CRXBuffer[1] + (I2CRXBuffer[0] << 8);
		// Check Ublox Data Length
		for (LoopCount = 0; LoopCount < LENGTH_LOOP_TIME; LoopCount++)
		{
			if (MsgBuffer.DataLen > 0)
			{
				break;
			}
			else
			{
				HAL_Delay(50);
				if (HAL_I2C_Mem_Read(&hi2c2, (u16)UBLOX_I2C_DRV_ADDRESS, (u16)UBLOX_LEN_START_ADDR, I2C_MEMADD_SIZE_8BIT, (u8 *)I2CRXBuffer, 2, UBLOX_OPERATE_TIMEOUT) == HAL_OK)
				{
					MsgBuffer.DataLen = I2CRXBuffer[1] + (I2CRXBuffer[0] << 8);
				}
				else
				{
					UbloxPrintf(DbgCtl.UbloxDbgInfoEn, "\r\n[%s] Ublox: RD Len Go on Fail", FmtTimeShow());
				}
			}
		}
		// Print Out
		//UbloxPrintf(DbgCtl.UbloxDbgInfoEn," \r\n[%s] Ublox: Len[%d] Loop[%d]",FmtTimeShow(), MsgBuffer.DataLen, LoopCount);
		// Read NMEA Data
		if (MsgBuffer.DataLen > 0 && MsgBuffer.DataLen < DATA_SIZE_MAX + 1)
		{
			// Get Data
			if (HAL_I2C_Master_Receive(&hi2c2, (u16)UBLOX_I2C_DRV_ADDRESS, (u8 *)MsgBuffer.Data, MsgBuffer.DataLen, UBLOX_OPERATE_TIMEOUT) == HAL_OK)
			{
				//UbloxPrintf(DbgCtl.ATCmdInfoEn, "\r\n[%s] Ublox: NMEA DATA:\r\n[\r\n%s]",FmtTimeShow(),MsgBuffer.Data);
				// UbloxPrintf(DbgCtl.ATCmdInfoEn, "\r\n%s", MsgBuffer.Data);
				if (DbgCtl.UbloxDbgInfoEn)
				{
					UART1PrintMassData(MsgBuffer.Data, strlen((char *)MsgBuffer.Data));
				}
			}
		}
		else
		{
			// Check Ublox Data Length
			for (LoopCount = 0; LoopCount < LENGTH_LOOP_TIME; LoopCount++)
			{
				if (MsgBuffer.DataLen >= DATA_SIZE_MAX)
				{
					MsgBuffer.DataLen = DATA_SIZE_MAX;
				}
				//if length is still above max size, it means the data includes 2 packet data at least.
				if (HAL_I2C_Master_Receive(&hi2c2, (u16)UBLOX_I2C_DRV_ADDRESS, (u8 *)MsgBuffer.Data, MsgBuffer.DataLen, UBLOX_OPERATE_TIMEOUT) == HAL_OK)
				{
					UbloxPrintf(DbgCtl.UbloxDbgInfoEn, "\r\n[%s] Ublox: abnormal data[%d]", FmtTimeShow(), MsgBuffer.DataLen);
				}
				else
				{
					UbloxPrintf(DbgCtl.UbloxDbgInfoEn, "\r\n[%s] Ublox: abnormal break[%d]", FmtTimeShow(), MsgBuffer.DataLen);
				}
				HAL_Delay(50);
				// Recheck Data Length
				if (HAL_I2C_Mem_Read(&hi2c2, (u16)UBLOX_I2C_DRV_ADDRESS, (u16)UBLOX_LEN_START_ADDR, I2C_MEMADD_SIZE_8BIT, (u8 *)I2CRXBuffer, 2, UBLOX_OPERATE_TIMEOUT) == HAL_OK)
				{
					MsgBuffer.DataLen = I2CRXBuffer[1] + (I2CRXBuffer[0] << 8);
					if (MsgBuffer.DataLen == 0)
					{
						break;
					}
				}
				else
				{
					UbloxPrintf(DbgCtl.UbloxDbgInfoEn, "\r\n[%s] Ublox: abnormal Recheck", FmtTimeShow());
				}
			}
			//if clear buffer,parsefun will display 0
			//memset((void*)&MsgBuffer, 0, sizeof(MsgBuffer));
		}
	}
}

double ChangetoRadian(int value)
{
	int tmp = value;

	tmp = (tmp / 10000000) * 10000000;
	value = value - tmp;

	return (value / 60.0 + tmp) / 10000000.0;
}

bool NmeaGPGGAParseFun(void *pdata, int *fixmode)
{
	uint32 uIdx = 0;
	uint8 buffer[518];
	uint8 ubufLen = 0;
	uint8 uDataLen;
	uint8 udataId = 0;
	uint8 *pGGAData = (uint8 *)pdata;
	//	MonPrintf("NmeaGPGGAParseFun" );
	if (NULL == pGGAData)
	{
		return FALSE;
	}
	uDataLen = strlen((const char *)pGGAData);
	/*	{
		char buffer[1024 + 10];
		memset((void*)buffer,0,sizeof(buffer));
		sprintf(buffer,"\r\nNmea: ubufLen %d: %s\r\n",uDataLen,pGGAData);
		ValSendDataToUart(buffer, strlen(buffer));
	}*/
	memset((void *)buffer, 0, sizeof(buffer));
	for (uIdx = GPSNMEA_CMD_NAME_LEN + 1; uIdx < uDataLen - 1; uIdx++)
	{
		if (pGGAData[uIdx] == ',' || pGGAData[uIdx] == '*' || pGGAData[uIdx] == 0x0d)
		{
			udataId++;
			//			MonPrintf("\r\nNmeaGPGGAParseFun udataId %d ubufLen %d\r\n",udataId,ubufLen);
			switch (udataId)
			{
#if 0
				case GPGGA_UTC_Time:
					{
						uint32 time = 0;
						if(ubufLen == 0)
						{
//							gLocMsg.UTCTime.Hour = 0;
//							gLocMsg.UTCTime.Minute = 0;
//							gLocMsg.UTCTime.Second = 0;
//							gLocMsg.UTCTime.Milliseconds = 0;
						}
						else
						{
							memset(temp,0,sizeof(temp));
							memcpy(temp,buffer,6);
							time = atoi(temp);
							gLocMsg.UTCTime.Hour = time/10000;
							gLocMsg.UTCTime.Minute = (time%10000)/100;
							gLocMsg.UTCTime.Second = time%100;
							memset(temp,0,sizeof(temp));
							memcpy(temp,buffer+7,3);
							gLocMsg.UTCTime.Milliseconds = atoi(temp);
							gLocMsg.bNorth = 0;
						}
						//printgpstestresult();
					}
					break;
#endif
			case GPGGA_Latitude:
			{
				if (ubufLen == 0)
				{
					//							gLocMsg.Latitude= 0;
				}
				else
				{
					gLocMsg.Latitude = 100000 * strtod((const char *)buffer, (char **)NULL);
				}
			}
			break;
			case GPGGA_North:
			{
				if (ubufLen != 0 && buffer[0] == 'N')
				{
					//gLocMsg.bNorth = TRUE;
					if (gLocMsg.Latitude < 0)
						gLocMsg.Latitude = 0 - gLocMsg.Latitude;
				}
				else
				{
					//gLocMsg.bNorth = FALSE;
					if (gLocMsg.Latitude > 0)
						gLocMsg.Latitude = 0 - gLocMsg.Latitude;
				}
			}
			break;
			case GPGGA_Longitude:
			{
				if (ubufLen == 0)
				{
					//							gLocMsg.Longitude= 0;
				}
				else
				{
					gLocMsg.Longitude = 100000 * strtod((const char *)buffer, (char **)NULL);
				}
			}
			break;
			case GPGGA_East:
			{
				if (ubufLen != 0 && buffer[0] == 'E')
				{
					//gLocMsg.bEast= TRUE;
					if (gLocMsg.Longitude < 0)
						gLocMsg.Longitude = 0 - gLocMsg.Longitude;
				}
				else
				{
					//gLocMsg.bEast = FALSE;
					if (gLocMsg.Longitude > 0)
						gLocMsg.Longitude = 0 - gLocMsg.Longitude;
				}
			}
			break;
			case GPGGA_Quality:
			{
				if (ubufLen != 0 && 0 != atoi((const char *)buffer))
				{
					*fixmode = 1;
				}
				else
				{
					*fixmode = 0;
					return FALSE;
				}
			}
			break;
			case GPGGA_SatelliteCount:
			{
				if (ubufLen == 0)
				{
					gLocMsg.SatelliteCount = 0;
				}
				else
				{
					gLocMsg.SatelliteCount = atoi((const char *)buffer);
				}
			}
			break;
			case GPGGA_HorizontalDilutionOfPrecision:
			{
				if (ubufLen == 0)
				{
					gLocMsg.HorizontalDilutionOfPrecision = 0;
				}
				else
				{
					gLocMsg.HorizontalDilutionOfPrecision = strtod((const char *)buffer, (char **)NULL);
				}
			}
			break;
			case GPGGA_AltitudeWRTSeaLevel:
			{
				if (ubufLen == 0)
				{
					gLocMsg.AltitudeWRTSeaLevel = 0;
				}
				else
				{
					gLocMsg.AltitudeWRTSeaLevel = strtod((const char *)buffer, (char **)NULL);
				}
			}
			break;
			case GPGGA_UnitsOfAntennaAltitude:
				break;
			case GPGGA_AltitudeWRTEllipsoid:
			{
				if (ubufLen == 0)
				{
					gLocMsg.AltitudeWRTEllipsoid = 0;
				}
				else
				{
					gLocMsg.AltitudeWRTEllipsoid = strtod((const char *)buffer, (char **)NULL);
				}
			}
			break;
			case GPGGA_UnitsOfGeoidalSeparation:
				break;
			case GPGGA_RTCMTime:
			{
				/*						if(ubufLen == 0)
						{
							gLocMsg.RTCMTime = 0;
						}
						else
						{
							gLocMsg.RTCMTime = atoi(buffer);
						}*/
			}
			break;
			case GPGGA_StationID:
			{
				/*						if(ubufLen == 0)
						{
							gLocMsg.stationid = 0;
						}
						else
						{
							gLocMsg.stationid = atoi(buffer);
						}*/
			}
			break;
			case GPGGA_Checksum:
				break;
			default:
				break;
			}
			ubufLen = 0;
			memset((void *)buffer, 0, sizeof(buffer));
		}
		else
		{
			buffer[ubufLen++] = pGGAData[uIdx];
		}
	}
	return TRUE;
}
bool NmeaGPGSAParseFun(void *pdata, int *fixmode)
{
	uint32 uIdx = 0;
	char buffer[518];
	uint8 ubufLen = 0;
	uint8 uDataLen;
	uint8 udataId = 0;
	uint8 *pGSAData = (uint8 *)pdata;
	//	MonPrintf("NmeaGPGSAParseFun" );
	if (NULL == pGSAData)
	{
		return TRUE;
	}
	uDataLen = strlen((const char *)pGSAData);
	/*	{
		char buffer[1024 + 10];
		memset((void*)buffer,0,sizeof(buffer));
		sprintf(buffer,"\r\nNmea: ubufLen %d: %s\r\n",uDataLen,pGSAData);
		ValSendDataToUart(buffer, strlen((const char*)buffer));
	}*/
	memset((void *)buffer, 0, sizeof(buffer));
	for (uIdx = GPSNMEA_CMD_NAME_LEN + 1; uIdx < uDataLen - 1; uIdx++)
	{
		if (pGSAData[uIdx] == ',' || pGSAData[uIdx] == '*' || pGSAData[uIdx] == 0x0d)
		{
			udataId++;
			//			MonPrintf("\r\nNmeaGPGSAParseFun udataId %d ubufLen %d\r\n",udataId,ubufLen);
			switch (udataId)
			{
			case GPGSA_Selection_Mode:
			{
				if (ubufLen == 1 && buffer[0] == 'M')
				{
					gLocMsg.SelectionType = GPS_FIX_SELECTION_MANUAL;
				}
				else if (ubufLen == 1 && buffer[0] == 'A')
				{
					gLocMsg.SelectionType = GPS_FIX_SELECTION_AUTO;
				}
				else
				{
					gLocMsg.SelectionType = GPS_FIX_SELECTION_UNKNOWN;
				}
			}
			break;
			case GPGSA_Mode:
			{
				if (ubufLen == 0)
				{
					gLocMsg.FixType = VAL_GPS_FIX_UNKNOWN;
				}
				else
				{
					gLocMsg.FixType = (ValGpsFixTypeT)(atoi((const char *)buffer) - 1);
				}
			}
			break;
			case GPGSA_1st_Satellite:
			case GPGSA_2nd_Satellite:
			case GPGSA_3rd_Satellite:
			case GPGSA_4th_Satellite:
			case GPGSA_5th_Satellite:
			case GPGSA_6th_Satellite:
			case GPGSA_7th_Satellite:
			case GPGSA_8th_Satellite:
			case GPGSA_9th_Satellite:
			case GPGSA_10th_Satellite:
			case GPGSA_11th_Satellite:
			case GPGSA_12th_Satellite:
			{

				if (ubufLen == 0)
				{
					gLocMsg.SatellitesUsedPRNs[udataId - GPGSA_1st_Satellite] = 0;
				}
				else
				{
					gLocMsg.SatellitesUsedPRNs[udataId - GPGSA_1st_Satellite] = atoi((const char *)buffer);
				}
			}
			break;
			case GPGSA_PDOP:
			{

				if (ubufLen == 0)
				{
					gLocMsg.PositionDilutionOfPrecision = 0;
				}
				else
				{
					gLocMsg.PositionDilutionOfPrecision = strtod((const char *)buffer, (char **)NULL);
				}
			}
			break;
			case GPGSA_HDOP:
			{

				if (ubufLen == 0)
				{
					gLocMsg.HorizontalDilutionOfPrecision = 0;
				}
				else
				{
					gLocMsg.HorizontalDilutionOfPrecision = strtod((const char *)buffer, (char **)NULL);
				}
			}
			break;
			case GPGSA_VDOP:
			{

				if (ubufLen == 0)
				{
					gLocMsg.VerticalDilutionOfPrecision = 0;
				}
				else
				{
					gLocMsg.VerticalDilutionOfPrecision = strtod((const char *)buffer, (char **)NULL);
				}
			}
			break;
			case GPGSA_Checksum:
				break;
			default:
				break;
			}
			ubufLen = 0;
			memset((void *)buffer, 0, sizeof(buffer));
		}
		else
		{
			buffer[ubufLen++] = pGSAData[uIdx];
		}
	}
	return TRUE;
}
bool NmeaGPGSVParseFun(void *pdata, int *fixmode)
{
	uint32 uIdx = 0;
	uint8 buffer[518];
	uint8 ubufLen = 0;
	uint8 uDataLen;
	uint8 udataId = 0;
	uint8 *pGSVData = (uint8 *)pdata;
	uint8 uSatellitesId = 0;
	//	MonPrintf("NmeaGPGSVParseFun" );
	if (NULL == pGSVData)
	{
		return TRUE;
	}
	uDataLen = strlen((const char *)pGSVData);
	/*	{
		char buffer[1024 + 10];
		memset((void*)buffer,0,sizeof(buffer));
		sprintf(buffer,"\r\nNmea: ubufLen %d: %s\r\n",uDataLen,pGSVData);
		ValSendDataToUart(buffer, strlen(buffer));
	}*/
	memset((void *)buffer, 0, sizeof(buffer));
	for (uIdx = GPSNMEA_CMD_NAME_LEN + 1; uIdx < uDataLen - 1; uIdx++)
	{
		if (pGSVData[uIdx] == ',' || pGSVData[uIdx] == '*' || pGSVData[uIdx] == 0x0d)
		{
			udataId++;
			//			MonPrintf("\r\NmeaGPGSVParseFun udataId %d ubufLen %d uSatellitesId %d\r\n",udataId,ubufLen,uSatellitesId);
			switch (udataId)
			{
			case GPGSV_Total_Msg_Num:
			{
				if (ubufLen == 0)
				{
					return TRUE;
				}
				else
				{
					gLocMsg.totalGsvNum = atoi((const char *)buffer);
				}
			}
			break;
			case GPGSV_Msg_Num:
			{
				if (ubufLen == 0)
				{
					return TRUE;
				}
				else
				{
					gLocMsg.seqGsvNum = atoi((const char *)buffer);
					uSatellitesId = (gLocMsg.seqGsvNum - 1) * 4;
				}
			}
			break;
			case GPGSV_Satellites_In_View:
			{
				if (ubufLen == 0)
				{
					return TRUE;
				}
				else
				{
					gLocMsg.SatellitesInView = atoi((const char *)buffer);
				}
			}
			break;
			case GPGSV_SatelliteInfos_PRN_1:
			case GPGSV_SatelliteInfos_PRN_2:
			case GPGSV_SatelliteInfos_PRN_3:
			case GPGSV_SatelliteInfos_PRN_4:
			{
				if (ubufLen == 0)
				{
					return TRUE;
				}
				else
				{
					gLocMsg.SatellitesInViewPRNs[uSatellitesId] = atoi((const char *)buffer);
				}
			}
			break;
			case GPGSV_SatelliteInfos_Elevation_1:
			case GPGSV_SatelliteInfos_Elevation_2:
			case GPGSV_SatelliteInfos_Elevation_3:
			case GPGSV_SatelliteInfos_Elevation_4:
			{
				if (ubufLen == 0)
				{
					return TRUE;
				}
				else
				{
					gLocMsg.SatellitesInViewElevation[uSatellitesId] = atoi((const char *)buffer);
				}
			}
			break;
			case GPGSV_SatelliteInfos_Azimuth_1:
			case GPGSV_SatelliteInfos_Azimuth_2:
			case GPGSV_SatelliteInfos_Azimuth_3:
			case GPGSV_SatelliteInfos_Azimuth_4:
			{
				if (ubufLen == 0)
				{
					return TRUE;
				}
				else
				{
					gLocMsg.SatellitesInViewAzimuth[uSatellitesId] = atoi((const char *)buffer);
				}
			}
			break;
			case GPGSV_SatelliteInfos_SNR_1:
			case GPGSV_SatelliteInfos_SNR_2:
			case GPGSV_SatelliteInfos_SNR_3:
			case GPGSV_SatelliteInfos_SNR_4:
			{
				if (ubufLen == 0)
				{
					gLocMsg.SatellitesInViewSNR[uSatellitesId] = 0;
				}
				else
				{
					gLocMsg.SatellitesInViewSNR[uSatellitesId] = atoi((const char *)buffer);
				}
				uSatellitesId++;
			}
			break;

			default:
				break;
			}
			ubufLen = 0;
			memset((void *)buffer, 0, sizeof(buffer));
		}
		else
		{
			buffer[ubufLen++] = pGSVData[uIdx];
		}
		if (pGSVData[uIdx] == '*')
		{
			return TRUE;
		}
	}
	return TRUE;
}
bool NmeaGPRMCParseFun(void *pdata, int *fixmode)
{
	uint32 uIdx = 0;
	uint8 buffer[518];
	uint8 temp[518];
	uint8 ubufLen = 0;
	uint8 uDataLen;
	uint8 udataId = 0;
	uint8 *pRMCData = (uint8 *)pdata;
	//	MonPrintf("NmeaGPRMCParseFun" );
	if (NULL == pRMCData)
	{
		return FALSE;
	}
	uDataLen = strlen((const char *)pRMCData);
	/*	{
		char buffer[1024 + 10];
		memset((void*)buffer,0,sizeof(buffer));
		sprintf(buffer,"\r\nNmea: ubufLen %d: %s\r\n",uDataLen,pRMCData);
		ValSendDataToUart(buffer, strlen(buffer));
	}*/
	memset(buffer, 0, sizeof(buffer));
	for (uIdx = GPSNMEA_CMD_NAME_LEN + 1; uIdx < uDataLen - 1; uIdx++)
	{
		if (pRMCData[uIdx] == ',' || pRMCData[uIdx] == '*' || pRMCData[uIdx] == 0x0d)
		{
			udataId++;
			//			UbloxPrintf("\r\nNmeaGPRMCParseFun udataId %d ubufLen %d\r\n",udataId,ubufLen);
			switch (udataId)
			{
			case GPRMC_UTC_Time:
			{
				uint32 time = 0;
				if (ubufLen == 0)
				{
					//							gLocMsg.UTCTime.Hour = 0;
					//							gLocMsg.UTCTime.Minute = 0;
					//							gLocMsg.UTCTime.Second = 0;
					//							gLocMsg.UTCTime.Milliseconds = 0;
				}
				else
				{
					memset((void *)temp, 0, sizeof(temp));
					memcpy((void *)temp, (void *)buffer, 6);
					time = atoi((const char *)temp);
					gLocMsg.UTCTime.Hour = time / 10000;
					gLocMsg.UTCTime.Minute = (time % 10000) / 100;
					gLocMsg.UTCTime.Second = time % 100;
					memset((void *)temp, 0, sizeof(temp));
					memcpy((void *)temp, (void *)(buffer + 7), 3);
					gLocMsg.UTCTime.Milliseconds = atoi((const char *)temp);
				}
			}
			break;
			case GPRMC_Status:
			{
				if (ubufLen != 0 && buffer[0] == 'A')
				{
					//							gLocMsg.bGpsLocked = TRUE;
				}
				else
				{
					return FALSE;
					//							gLocMsg.bGpsLocked = FALSE;
				}
			}
			break;
#if 0
				case GPRMC_Latitude:
					{
						if(ubufLen == 0)
						{
//							gLocMsg.Latitude= 0;
						}
						else
						{
							gLocMsg.Latitude = 100000*strtod(buffer,(char **)NULL);
						}
					}
					break;
				case GPRMC_North:
					{
						if(ubufLen != 0 && buffer[0]== 'N')
						{
							gLocMsg.bNorth = TRUE;
						}
						else
						{
							gLocMsg.bNorth = FALSE;
						}
					}
					break;
				case GPRMC_Longitude:
					{
						if(ubufLen == 0)
						{
//							gLocMsg.Longitude= 0;
						}
						else
						{
							gLocMsg.Longitude = 100000*strtod(buffer,(char **)NULL);
						}
					}
					break;
				case GPRMC_East:
					{
						if(ubufLen != 0 && buffer[0]== 'E')
						{
							gLocMsg.bEast= TRUE;
						}
						else
						{
							gLocMsg.bEast = FALSE;
						}
					}
					break;
#endif
			case GPRMC_Speed:
			{
				if (ubufLen == 0)
				{
					gLocMsg.Speed = 0;
				}
				else
				{
					gLocMsg.Speed = strtod((const char *)buffer, (char **)NULL);
				}
			}
			break;
			case GPRMC_head:
			{
				if (ubufLen == 0)
				{
					gLocMsg.Heading = 0;
				}
				else
				{
					gLocMsg.Heading = strtod((const char *)buffer, (char **)NULL);
				}
			}
			break;
			case GPRMC_UTC_Date:
			{
				uint32 udate = 0;
				if (ubufLen == 0)
				{
					//							gLocMsg.UTCTime.Day= 0;
					//							gLocMsg.UTCTime.Month= 0;
					//							gLocMsg.UTCTime.Year= 0;
				}
				else
				{
					memset((void *)temp, 0, sizeof(temp));
					memcpy((void *)temp, (void *)buffer, 6);
					udate = atoi((const char *)temp);
					gLocMsg.UTCTime.Day = udate / 10000;
					gLocMsg.UTCTime.Month = (udate % 10000) / 100;
					gLocMsg.UTCTime.Year = udate % 100;
				}
			}
			break;
#if 0
				case GPRMC_MagneticVariation:
					{
						if(ubufLen == 0)
						{
							gLocMsg.MagneticVariation= 0;
						}
						else
						{
							gLocMsg.MagneticVariation = strtod(buffer,(char **)NULL);
						}
					}
					break;
				case GPRMC_Declination:
					break;
				//case GPRMC_Mode_Indicator:
				//	{
				//	}
				//	break;
				case GPGGA_Checksum:
					break;
#endif
			default:
				break;
			}
			ubufLen = 0;
			memset((void *)buffer, 0, sizeof(buffer));
		}
		else
		{
			buffer[ubufLen++] = pRMCData[uIdx];
		}
	}
	return TRUE;
}
bool NmeaGPVTGParseFun(void *pdata, int *fixmode)
{
	if (NULL == pdata)
	{
		return TRUE;
	}
	/*	if(1)
	{
		char buffer[1024 + 10];
		memset((void*)buffer,0,sizeof(buffer));
		sprintf(buffer,"\r\nNmea: uTempLen %d: %s\r\n",strlen(pdata),pdata);
		ValSendDataToUart(buffer, strlen(buffer));
	}*/
	return TRUE;
}
bool NmeaGPZDAParseFun(void *pdata, int *fixmode)
{
	if (NULL == pdata)
	{
		return TRUE;
	}
	/*	if(1)
	{
		char buffer[1024 + 10];
		memset((void*)buffer,0,sizeof(buffer));
		sprintf(buffer,"\r\nNmea: uTempLen %d: %s\r\n",strlen(pdata),pdata);
		ValSendDataToUart(buffer, strlen(buffer));
	}*/
	return TRUE;
}
bool NmeaGPGLLParseFun(void *pdata, int *fixmode)
{
	if (NULL == pdata)
	{
		return TRUE;
	}
	/*	if(1)
	{
		char buffer[1024 + 10];
		memset((void*)buffer,0,sizeof(buffer));
		sprintf(buffer,"\r\nNmeaParseFun uTempLen %d: %s\r\n",strlen(pdata),pdata);
		ValSendDataToUart(buffer, strlen(buffer));
	}*/
	return TRUE;
}

static uint8 ToUper(uint8 ch)
{
	if (ch >= 'a' && ch <= 'z')
		return (uint8)(ch + ('A' - 'a'));
	return ch;
}

uint8 stricmp(const char *Str1, const char *Str2)
{
	uint16 nLen1, nLen2;
	uint16 i = 0;
	if (!Str1 || !Str2)
		return 1;
	nLen1 = strlen((char *)Str1);
	nLen2 = strlen((char *)Str2);

	if (nLen1 > nLen2)
		return 1;
	else if (nLen1 < nLen2)
		return 1;
	while (i < nLen1)
	{
		if (ToUper((uint8)Str1[i]) != ToUper((uint8)Str2[i]))
			return 1;
		i++;
	}
	return 0;
}

bool NmeaParseFun(PswGpsNmeaStreamMsgT *pNmeaMsg, ValGpsInfo *GpsInfop)
{
	uint32 uidx = 0;
	uint32 uTempLen = 0;
	uint32 uTempIdx = 0;
	uint8 uTableIdx = 0;
	uint8 Temp[GPSNMEA_CMD_INFO_LEN];
	GpsParseStruct GpsNmeaTable[] =
		{
			{"GPGGA", NmeaGPGGAParseFun},
			{"GPGSA", NmeaGPGSAParseFun},
			{"GPGSV", NmeaGPGSVParseFun},
			{"GPRMC", NmeaGPRMCParseFun},
			{"GPVTG", NmeaGPVTGParseFun},
			{"GPGLL", NmeaGPGLLParseFun},
			{"GPZDA", NmeaGPZDAParseFun}};
	uint32 GpsCmdNum = sizeof(GpsNmeaTable) / sizeof(GpsParseStruct);
	//	bool           GpsLocked = TRUE;
	int fixmode = 0;

	if (NULL == pNmeaMsg || 0 == pNmeaMsg->DataLen)
	{
		if (GpsInfop)
		{
			GpsInfop->bGpsLock = FALSE;
			GpsInfop->validFix = 0;
			GpsInfop->WeakSignalvalidFix = 0;
			GpsInfop->Gps2Dfix_Blocked = 0;
		}
		return FALSE;
	}
	//	memset((void*)&gLocMsg,0x00,sizeof(gLocMsg));
	memset((void *)GpsInfop, 0x00, sizeof(ValGpsInfo));
	gLocMsg.Latitude = 0;
	gLocMsg.Longitude = 0;
	for (uidx = 0; uidx < pNmeaMsg->DataLen - 1; uidx++)

	{
		if (pNmeaMsg->Data[uidx] == '$')
		{
			memset((void *)Temp, 0x00, sizeof(Temp));
			uTempLen = 0;
		}
		else if (pNmeaMsg->Data[uidx] == 0x0d && pNmeaMsg->Data[uidx + 1] == 0x0a)
		{
			char name[GPSNMEA_CMD_NAME_LEN];
			uidx++;
			if (uTempLen < GPSNMEA_CMD_NAME_LEN)
			{
				continue;
			}
			uTempIdx = 0;
			memset((void *)name, 0x00, sizeof(name));
			while ((Temp[uTempIdx] != ',') && uTempIdx < uTempLen)
			{
				name[uTempIdx] = Temp[uTempIdx];
				uTempIdx++;
				if (uTempIdx > GPSNMEA_CMD_NAME_LEN)
				{
					break;
				}
			}
			// ValSendDataToUart(name, strlen(name));
			if (uTempIdx == GPSNMEA_CMD_NAME_LEN)
			{
				for (uTableIdx = 0; uTableIdx < GpsCmdNum; uTableIdx++)
				{
					if (stricmp((const char *)GpsNmeaTable[uTableIdx].Name, (const char *)name) == 0)
					{
						GpsNmeaTable[uTableIdx].GpsProcessFunc(&Temp, &fixmode);
						break;
					}
				}
			}
		}
		else
		{
			Temp[uTempLen++] = pNmeaMsg->Data[uidx];
		}
	}
	if (fixmode && gLocMsg.Latitude != 0 && gLocMsg.Longitude != 0)
	{
		GpsInfop->Altitude = gLocMsg.AltitudeWRTSeaLevel;
		GpsInfop->Heading = gLocMsg.Heading;
		GpsInfop->Latitude = ChangetoRadian(gLocMsg.Latitude);
		GpsInfop->Longitude = ChangetoRadian(gLocMsg.Longitude);
		GpsInfop->SatelliteCount = gLocMsg.SatelliteCount;
		GpsInfop->Velocity = gLocMsg.Speed;
		GpsInfop->bGpsLock = TRUE;

		GpsInfop->Year = gLocMsg.UTCTime.Year + 2000;
		GpsInfop->Month = gLocMsg.UTCTime.Month;
		//               GpsInfop->DayOfWeek = gLocMsg.UTCTime.DayOfWeek;
		GpsInfop->Day = gLocMsg.UTCTime.Day;
		GpsInfop->Hour = gLocMsg.UTCTime.Hour;
		GpsInfop->Minute = gLocMsg.UTCTime.Minute;
		GpsInfop->Second = gLocMsg.UTCTime.Second;
		GpsInfop->Milliseconds = gLocMsg.UTCTime.Milliseconds;
		//               GpsInfop->GPSweek = gLocMsg.UTCTime.GPSweek;
		//               GpsInfop->GPSTimeOfWeek = gLocMsg.UTCTime.GPSTimeOfWeek;
		GpsInfop->validFix = fixmode;
		// GpsInfop->hdop = gLocMsg.HorizontalDilutionOfPrecision;
		GpsInfop->pdop = gLocMsg.PositionDilutionOfPrecision;
		//               UbloxPrintf(DbgCtl.UbloxDbgInfoEn,"\r\n====111111111 year : %d  month: %d day : %d  hour: %d minute : %d  second: %d DayOfWeek: %d Milliseconds: %d GPSweek: %d GPSTimeOfWeek: %d=====\r\n",GpsInfo.Year,
		//				GpsInfo.Month,GpsInfo.Day,GpsInfo.Hour,GpsInfo.Minute,GpsInfo.Second,GpsInfo.DayOfWeek,GpsInfo.Milliseconds,GpsInfo.GPSweek,GpsInfo.GPSTimeOfWeek);
		//		UbloxPrintf("\nUblox GPS Lock time %d:%d:%d\n", GpsInfop->Hour, GpsInfop->Minute , GpsInfop->Second);
		//	        OemMsgHandle(OEM_GPS_LOC_INF_MSG, GpsInfop, sizeof(ValGpsInfo));
	}
	else
	{
		GpsInfop->bGpsLock = FALSE;
		GpsInfop->validFix = 0;
		GpsInfop->WeakSignalvalidFix = 0;
		GpsInfop->Gps2Dfix_Blocked = 0;
		fixmode = 0;
		//               OemMsgHandle(OEM_GPS_LOC_INF_MSG, GpsInfop, sizeof(ValGpsInfo));
	}
	return fixmode;
}

void ParseUbloxData(int length, uint8 *data)
{
	int i, dataleft, msgl;
	uint8 *p = data;

	dataleft = length;
	while (dataleft >= 8)
	{
		if (p[0] == UBX_SYNC_1 && p[1] == UBX_SYNC_2)
		{
			msgl = p[4] + p[5] * 256;
			//MonTrace(1, 6, 111222, p[2], p[3], msgl, p[6], p[7]);
			//printf("\nUBlox Msg (%02x), id (%02x): lenght (%d)\n Data: ", p[2], p[3], msgl);
			//UbloxPrintf(DbgCtl.UbloxDbgInfoEn," \r\n[%s] UBlox Msg (%02x), id (%02x): lenght (%d)\n Data",FmtTimeShow(),p[2], p[3], msgl);
			if (msgl + 8 <= dataleft)
			{
				for (i = 0; i < msgl; i++)
				{
					//UbloxPrintf("%02x ", p[i+6]);
				}
				//UbloxPrintf("\n");
				//MonTrace(1, 2, 111333, (msgl+8) );
				//OemMsgHandle(OEM_UBLOX_MSG, (void *)p, (msgl+8));
				p += (msgl + 8);
			}
			else
			{
				//UbloxPrintf("Corrupted \n");
				//MonTrace(1, 2, 111444, dataleft );
				//OemMsgHandle(OEM_UBLOX_MSG, p, dataleft);
				return;
			}
		}
		else
		{
			dataleft--;
			p++;
		}
	}
}

void UbloxGPSTimerProcess(void)
{
	static uint8_t firstfix = FALSE;
	//ParseUbloxData(Datalen, p);
	if (MsgBuffer.DataLen != 0)
	{
		NmeaParseFun(&MsgBuffer, &GpsInfo);
		/* UbloxPrintf(DbgCtl.UbloxDbgInfoEn,"\r\n[%s] UBlox:Hour %d Minute %d Second %d Milliseconds %d ",FmtTimeShow(),GpsInfo.Hour,GpsInfo.Minute,GpsInfo.Second,GpsInfo.Second);
	        UbloxPrintf(DbgCtl.UbloxDbgInfoEn,"\r\n[%s] UBlox:Long[%f] Lat[%f] Heading[%f] Velocity[%f] Altitude[%f] hdop[%f] Satellite[%d] bGpsLock[%d]",FmtTimeShow(),GpsInfo.Longitude, GpsInfo.Latitude,
												GpsInfo.Heading,  GpsInfo.Velocity, GpsInfo.Altitude, GpsInfo.hdop,GpsInfo.SatelliteCount,GpsInfo.bGpsLock );
	        UbloxPrintf(DbgCtl.UbloxDbgInfoEn,"\r\n[%s] UBlox:FixType[%d]SelectionType[%d]",FmtTimeShow(),gLocMsg.FixType ,gLocMsg.SelectionType); */
	}

	if (FALSE == GpsInfo.validFix)
	{
		WedgeGpsRedLedControl(FALSE);
	}
	else
	{
		WedgeGpsRedLedControl(TRUE);
	}

	WedgeSysStateSet(WEDGE_GPS_FIX_STATE, &(GpsInfo.validFix));
	if (firstfix == FALSE)
	{
		if (GpsInfo.validFix)
		{
			uint8_t poweron = FALSE;
			
			WedgeSysStateSet(WEDGE_GPS_LASTFIX_STATE, &(GpsInfo.validFix));
			WedgeUpdateBinaryMsgGpsRecord();

			firstfix = TRUE;

			poweron = *((uint8_t *)WedgeSysStateGet(WEDGE_POWER_ON_OFF_STATE));
			if (FALSE == poweron)
			{
				// TRUE means wedge is power on without power lost.
				poweron = TRUE;
				WedgeSysStateSet(WEDGE_POWER_ON_OFF_STATE, &poweron);

				WedgeResponseUdpBinary(WEDGEPYLD_STATUS, Modem_powered_up);
			}
		}
	}
}

#define KN_TO_KM_FACTOR (1.852)

double UbloxSpeedKM(void)
{
    double speedkm = 0.0;

    speedkm = GpsInfo.Velocity * KN_TO_KM_FACTOR;

    return speedkm;
}

uint8_t UbloxFixStateGet(void)
{
	return GpsInfo.validFix;
}

void UBloxGetGpsPoint(double *pLatitude, double *pLongitude)
{
	*pLatitude = GpsInfo.Latitude;
	*pLongitude = GpsInfo.Longitude;
}

double UbloxGetHeading(void)
{
	return GpsInfo.Heading;
}

#include <math.h>
float UBloxGpsPointDistance(double long Latitude1, double long Longitude1, double long Latitude2, double long Longitude2)
{
	#define MUL_NUM				1000.0
	#define PI					3.141592653589793238
	#define EARTH_ROUND			6371.001
	double long TempDistance;
	// Calculate Distance
	Latitude1 *= (PI * MUL_NUM) /180.0/MUL_NUM;
	Longitude1 *= (PI * MUL_NUM)/180.0/MUL_NUM;
	Latitude2 *= ( PI* MUL_NUM) /180.0/MUL_NUM;
	Longitude2 *= (PI * MUL_NUM)/180.0/MUL_NUM;
	TempDistance = acos(sin(Latitude1) * sin(Latitude2) + \
		cos(Latitude1) * cos(Latitude2) * cos(fabs(Longitude1- Longitude2)));
	TempDistance = TempDistance * 1000.0;
	TempDistance = TempDistance * EARTH_ROUND;
	// Print Out
	// Print Out
	UbloxPrintf(DbgCtl.UbloxDbgInfoEn,"\r\n[%s] Geo-fence: Diff(%f meters)", \
		FmtTimeShow(), \
		TempDistance);
	// Return Value
	return TempDistance;
}

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2018. All rights reserved
                                End Of The File
*******************************************************************************/
