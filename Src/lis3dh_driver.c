/******************** (C) COPYRIGHT 2012 STMicroelectronics ********************
* File Name          : LIS3DH_driver.c
* Author             : MSH Application Team
* Author             : Fabio Tota
* Version            : $Revision:$
* Date               : $Date:$
* Description        : LIS3DH driver file
*                      
* HISTORY:
* Date               |	Modification                    |	Author
* 24/06/2011         |	Initial Revision                |	Fabio Tota
* 11/06/2012         |	Support for multiple drivers in the same program |	Abhishek Anand

********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* THIS SOFTWARE IS SPECIFICALLY DESIGNED FOR EXCLUSIVE USE WITH ST PARTS.
*
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "lis3dh_driver.h"
#include "include.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
LIS3DH_POSITION_6D_t gsensordirection = LIS3DH_UNKOWN;
LIS3DH_POSITION_6D_t old_gsensordirection = LIS3DH_UNKOWN;
/* Private variables ---------------------------------------------------------*/
static u8 Lis3dhAlarmHappenFlag = FALSE;
/* Private function prototypes -----------------------------------------------*/

void SetLis3dhAlarmStatus(u8 Status)
{
	Lis3dhAlarmHappenFlag = Status;
}

u8 GetLis3dhAlarmStatus(void)
{
	return Lis3dhAlarmHappenFlag;
}

void GSensorI2cInit(void)
{
	SerialDbgPrintf(DbgCtl.Lis3dhDbgInfoEn, "\r\n[%s] LIS3DH: Enable",FmtTimeShow());
	//Inizialize MEMS Sensor
	//set ODR (turn ON device)
        LIS3DH_SetODR(LIS3DH_ODR_100Hz);
        //set PowerMode 
	LIS3DH_SetMode(LIS3DH_NORMAL);
       //set Fullscale
       LIS3DH_SetFullScale(LIS3DH_FULLSCALE_2); //16
       //set axis Enable
       LIS3DH_SetAxis(LIS3DH_X_ENABLE | LIS3DH_Y_ENABLE | LIS3DH_Z_ENABLE);

       //LIS3DH_SetBDU(MEMS_ENABLE);
       //set Int1Pin
       LIS3DH_SetInt1Pin(LIS3DH_I1_INT1_ON_PIN_INT1_ENABLE);

       //LIS2DH_Int1LatchEnable(MEMS_ENABLE);
       LIS3DH_HPFAOI1Enable(MEMS_ENABLE);//MEMS_DISABLE
	LIS3DH_SetInt1Threshold(5);
       //LIS3DH_SetInt1Duration(5);
       //LIS3DH_ResetInt1Latch();	
       LIS3DH_SetIntConfiguration(LIS3DH_INT1_ZHIE_ENABLE | LIS3DH_INT1_ZLIE_ENABLE |
							 LIS3DH_INT1_YHIE_ENABLE | LIS3DH_INT1_YLIE_ENABLE |
							 LIS3DH_INT1_XHIE_ENABLE | LIS3DH_INT1_XLIE_ENABLE );
       LIS3DH_SetInt6D4DConfiguration(LIS3DH_INT1_6D_ENABLE);
       LIS3DH_SetIntMode(LIS3DH_INT_MODE_6D_MOVEMENT);
}


void AccelWriteRegister(uint8 address, uint8 reg,uint8 val)
{
	if(HAL_I2C_Mem_Write(GSENSOR_I2C_DEF,(u16)address,(u16)reg,I2C_MEMADD_SIZE_8BIT,(u8 *)&val,1,1000) != HAL_OK)
	{
		SerialDbgPrintf(DbgCtl.Lis3dhDbgInfoEn, "\r\n[%s] LIS3DH: Write Byte Fail",FmtTimeShow());
	}
}

u8 AccelReadRegister(uint8 address, uint8 reg)
{
	u8 rdata;
	if(HAL_I2C_Mem_Read(GSENSOR_I2C_DEF,(u16)address,(u16)reg,I2C_MEMADD_SIZE_8BIT,(u8 *)&rdata,1,1000) != HAL_OK)
	{
		SerialDbgPrintf(DbgCtl.Lis3dhDbgInfoEn, "\r\n[%s] LIS3DH: Read Byte Fail",FmtTimeShow());
		
	}
	return(rdata);
}

void AcceReadSerialData(u8 address, uint8 reg,u8 count,u8 * buff)
{
	if(HAL_I2C_Mem_Read(GSENSOR_I2C_DEF,(u16)address,(u16)reg,I2C_MEMADD_SIZE_8BIT,(u8 *)buff,count,1000) != HAL_OK)

	{
		SerialDbgPrintf(DbgCtl.Lis3dhDbgInfoEn, "\r\n[%s] LIS3DH: Read Serial Byte Fail",FmtTimeShow());
	}
}

/*******************************************************************************
* Function Name		: LIS3DH_ReadReg
* Description		: Generic Reading function. It must be fullfilled with either
*			: I2C or SPI reading functions					
* Input			: Register Address
* Output		: Data REad
* Return		: None
*******************************************************************************/
u8_t LIS3DH_ReadReg(u8_t Reg, u8_t* Data) {
  
  //To be completed with either I2c or SPI reading function
  //i.e. *Data = SPI_Mems_Read_Reg( Reg );  
  return 1;
}


/*******************************************************************************
* Function Name		: LIS3DH_WriteReg
* Description		: Generic Writing function. It must be fullfilled with either
*			: I2C or SPI writing function
* Input			: Register Address, Data to be written
* Output		: None
* Return		: None
*******************************************************************************/
u8_t LIS3DH_WriteReg(u8_t WriteAddr, u8_t Data) {
  
  //To be completed with either I2c or SPI writing function
  //i.e. SPI_Mems_Write_Reg(WriteAddr, Data);  
  return 1;
}


/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : LIS2DH_GetWHO_AM_I
* Description    : Read identification code by WHO_AM_I register
* Input          : Char to empty by Device identification Value
* Output         : None
* Return         : Status [value of FSS]
*******************************************************************************/
status_t LIS3DH_GetWHO_AM_I(void)
{
    u8 ChipID; 	
    /* Check Chip ID */	
    ChipID = AccelReadRegister(LIS3DH_MEMS_I2C_ADDRESS,LIS3DH_WHO_AM_I);
    if(ChipID == WHOAMI_LIS3DH_ACC) //LIS2DH_MEMS_I2C_ADDRESS
    {
    	SerialDbgPrintf(DbgCtl.Lis3dhDbgInfoEn,"\r\n[%s] LIS3DH: ID(0x%02X)",FmtTimeShow(),ChipID);
    	return MEMS_SUCCESS;
    }
    else
    {
    	SerialDbgPrintf(DbgCtl.Lis3dhDbgInfoEn,"\r\n[%s] LIS3DH:Unknown Chipset(0x%X)",FmtTimeShow(),ChipID);
    }
    return MEMS_ERROR;
}

/*******************************************************************************
* Function Name  : LIS3DH_SetODR
* Description    : Sets LIS3DH Output Data Rate
* Input          : Output Data Rate
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetODR(LIS3DH_ODR_t ov){
  u8_t value;
  
  value = AccelReadRegister(LIS3DH_MEMS_I2C_ADDRESS,LIS3DH_CTRL_REG1);;
  
  value &= 0x0f;
  value |= ov<<LIS3DH_ODR_BIT;
  
  AccelWriteRegister(LIS3DH_MEMS_I2C_ADDRESS,LIS3DH_CTRL_REG1,value);
  
  return MEMS_SUCCESS;
}

/*******************************************************************************
* Function Name  : LIS3DH_SetMode
* Description    : Sets LIS3DH Operating Mode
* Input          : Modality (LIS3DH_NORMAL, LIS3DH_LOW_POWER, LIS3DH_POWER_DOWN)
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetMode(LIS3DH_Mode_t md) {
  u8_t value;
  u8_t value2;
  static   u8_t ODR_old_value;
  
  value = AccelReadRegister(LIS3DH_MEMS_I2C_ADDRESS,LIS3DH_CTRL_REG1);
  
  value2 = AccelReadRegister(LIS3DH_MEMS_I2C_ADDRESS,LIS3DH_CTRL_REG4); 

  
  if((value & 0xF0)==0) 
    value = value | (ODR_old_value & 0xF0); //if it comes from POWERDOWN  
  
  switch(md) {
    
  case LIS3DH_POWER_DOWN:
    ODR_old_value = value;
    value &= 0x0F;
    break;
    
  case LIS3DH_NORMAL:
    value &= 0xF7;
    value |= (MEMS_RESET<<LIS3DH_LPEN);
    value2 &= 0xF7;
    value2 |= (MEMS_SET<<LIS3DH_HR);   //set HighResolution_BIT
    break;
    
  case LIS3DH_LOW_POWER:		
    value &= 0xF7;
    value |=  (MEMS_SET<<LIS3DH_LPEN);
    value2 &= 0xF7;
    value2 |= (MEMS_RESET<<LIS3DH_HR); //reset HighResolution_BIT
    break;
    
  default:
    return MEMS_ERROR;
  }
  
  AccelWriteRegister(LIS3DH_MEMS_I2C_ADDRESS,LIS3DH_CTRL_REG1,value);
  
  AccelWriteRegister(LIS3DH_MEMS_I2C_ADDRESS,LIS3DH_CTRL_REG4,value2);;  
  
	return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS3DH_SetAxis
* Description    : Enable/Disable LIS3DH Axis
* Input          : LIS3DH_X_ENABLE/DISABLE | LIS3DH_Y_ENABLE/DISABLE | LIS3DH_Z_ENABLE/DISABLE
* Output         : None
* Note           : You MUST use all input variable in the argument, as example
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetAxis(LIS3DH_Axis_t axis) {
  u8_t value;
  
  if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG1, &value) )
    return MEMS_ERROR;
  value &= 0xF8;
  value |= (0x07 & axis);
  
  if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG1, value) )
    return MEMS_ERROR;   
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS3DH_SetFullScale
* Description    : Sets the LIS3DH FullScale
* Input          : LIS3DH_FULLSCALE_2/LIS3DH_FULLSCALE_4/LIS3DH_FULLSCALE_8/LIS3DH_FULLSCALE_16
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetFullScale(LIS3DH_Fullscale_t fs) {
  u8_t value;
  
  value = AccelReadRegister(LIS3DH_MEMS_I2C_ADDRESS,LIS3DH_CTRL_REG4);
  
  value &= 0xCF;	
  value |= (fs<<LIS3DH_FS);
  
  AccelWriteRegister(LIS3DH_MEMS_I2C_ADDRESS,LIS3DH_CTRL_REG4,value);
  
  return MEMS_SUCCESS;
}

/*******************************************************************************
* Function Name  : LIS3DH_HPFAOI1
* Description    : Enable/Disable High Pass Filter for AOI on INT_1
* Input          : MEMS_ENABLE/MEMS_DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_HPFAOI1Enable(State_t hpfe) {
  u8_t value;
  
  value = AccelReadRegister(LIS3DH_MEMS_I2C_ADDRESS,LIS3DH_CTRL_REG2);
  
  value &= 0xFE;
  value |= (hpfe<<LIS3DH_HPIS1);
  
  AccelWriteRegister(LIS3DH_MEMS_I2C_ADDRESS,LIS3DH_CTRL_REG2,value);
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS3DH_HPFAOI2
* Description    : Enable/Disable High Pass Filter for AOI on INT_2
* Input          : MEMS_ENABLE/MEMS_DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_HPFAOI2Enable(State_t hpfe) {
  u8_t value;
  
  value = AccelReadRegister(LIS3DH_MEMS_I2C_ADDRESS,LIS3DH_CTRL_REG2);
  
  value &= 0xFD;
  value |= (hpfe<<LIS3DH_HPIS2);
  
  AccelWriteRegister(LIS3DH_MEMS_I2C_ADDRESS,LIS3DH_CTRL_REG2,value);
  
  return MEMS_SUCCESS;
}

/*******************************************************************************
* Function Name  : LIS3DH_SetInt1Pin
* Description    : Set Interrupt1 pin Function
* Input          :  LIS3DH_CLICK_ON_PIN_INT1_ENABLE/DISABLE    | LIS3DH_I1_INT1_ON_PIN_INT1_ENABLE/DISABLE |              
                    LIS3DH_I1_INT2_ON_PIN_INT1_ENABLE/DISABLE  | LIS3DH_I1_DRDY1_ON_INT1_ENABLE/DISABLE    |              
                    LIS3DH_I1_DRDY2_ON_INT1_ENABLE/DISABLE     | LIS3DH_WTM_ON_INT1_ENABLE/DISABLE         |           
                    LIS3DH_INT1_OVERRUN_ENABLE/DISABLE  
* example        : SetInt1Pin(LIS3DH_CLICK_ON_PIN_INT1_ENABLE | LIS3DH_I1_INT1_ON_PIN_INT1_ENABLE |              
                    LIS3DH_I1_INT2_ON_PIN_INT1_DISABLE | LIS3DH_I1_DRDY1_ON_INT1_ENABLE | LIS3DH_I1_DRDY2_ON_INT1_ENABLE |
                    LIS3DH_WTM_ON_INT1_DISABLE | LIS3DH_INT1_OVERRUN_DISABLE   ) 
* Note           : To enable Interrupt signals on INT1 Pad (You MUST use all input variable in the argument, as example)
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetInt1Pin(LIS3DH_IntPinConf_t pinConf) {
  u8_t value;
  
  value = AccelReadRegister(LIS3DH_MEMS_I2C_ADDRESS,LIS3DH_CTRL_REG3);
  
  value &= 0x00;
  value |= pinConf;
  
 AccelWriteRegister(LIS3DH_MEMS_I2C_ADDRESS,LIS3DH_CTRL_REG3,value);
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS3DH_SetInt2Pin
* Description    : Set Interrupt2 pin Function
* Input          : LIS3DH_CLICK_ON_PIN_INT2_ENABLE/DISABLE   | LIS3DH_I2_INT1_ON_PIN_INT2_ENABLE/DISABLE |               
                   LIS3DH_I2_INT2_ON_PIN_INT2_ENABLE/DISABLE | LIS3DH_I2_BOOT_ON_INT2_ENABLE/DISABLE |                   
                   LIS3DH_INT_ACTIVE_HIGH/LOW
* example        : LIS3DH_SetInt2Pin(LIS3DH_CLICK_ON_PIN_INT2_ENABLE/DISABLE | LIS3DH_I2_INT1_ON_PIN_INT2_ENABLE/DISABLE |               
                   LIS3DH_I2_INT2_ON_PIN_INT2_ENABLE/DISABLE | LIS3DH_I2_BOOT_ON_INT2_ENABLE/DISABLE |                   
                   LIS3DH_INT_ACTIVE_HIGH/LOW)
* Note           : To enable Interrupt signals on INT2 Pad (You MUST use all input variable in the argument, as example)
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetInt2Pin(LIS3DH_IntPinConf_t pinConf) {
  u8_t value;
  
  value = AccelReadRegister(LIS3DH_MEMS_I2C_ADDRESS,LIS3DH_CTRL_REG6);
  
  value &= 0x00;
  value |= pinConf;
  
  AccelWriteRegister(LIS3DH_MEMS_I2C_ADDRESS,LIS3DH_CTRL_REG6,value);
  
  return MEMS_SUCCESS;
}                       

/*******************************************************************************
* Function Name  : LIS3DH_Int1LatchEnable
* Description    : Enable Interrupt 1 Latching function
* Input          : ENABLE/DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_Int1LatchEnable(State_t latch) {
  u8_t value;
  
  value = AccelReadRegister(LIS3DH_MEMS_I2C_ADDRESS,LIS3DH_CTRL_REG5);
  
  value &= 0xF7;
  value |= latch<<LIS3DH_LIR_INT1;
  
  AccelWriteRegister(LIS3DH_MEMS_I2C_ADDRESS,LIS3DH_CTRL_REG5,value);
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS3DH_ResetInt1Latch
* Description    : Reset Interrupt 1 Latching function
* Input          : None
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_ResetInt1Latch(void) {
  //u8_t value;
  
 /*value =*/ AccelReadRegister(LIS3DH_MEMS_I2C_ADDRESS,LIS3DH_INT1_SRC);  
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS3DH_SetIntConfiguration
* Description    : Interrupt 1 Configuration (without LIS3DH_6D_INT)
* Input          : LIS3DH_INT1_AND/OR | LIS3DH_INT1_ZHIE_ENABLE/DISABLE | LIS3DH_INT1_ZLIE_ENABLE/DISABLE...
* Output         : None
* Note           : You MUST use all input variable in the argument, as example
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetIntConfiguration(LIS3DH_Int1Conf_t ic) {
  u8_t value;
  
  value = AccelReadRegister(LIS3DH_MEMS_I2C_ADDRESS,LIS3DH_INT1_CFG);
  
  value &= 0x40; 
  value |= ic;
  
  AccelWriteRegister(LIS3DH_MEMS_I2C_ADDRESS,LIS3DH_INT1_CFG,value);
  
  return MEMS_SUCCESS;
} 

     
/*******************************************************************************
* Function Name  : LIS3DH_SetIntMode
* Description    : Interrupt 1 Configuration mode (OR, 6D Movement, AND, 6D Position)
* Input          : LIS3DH_INT_MODE_OR, LIS3DH_INT_MODE_6D_MOVEMENT, LIS3DH_INT_MODE_AND, 
				   LIS3DH_INT_MODE_6D_POSITION
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetIntMode(LIS3DH_Int1Mode_t int_mode) {
  u8_t value;
  
  value = AccelReadRegister(LIS3DH_MEMS_I2C_ADDRESS,LIS3DH_INT1_CFG);
  value &= 0x3F; 
  value |= (int_mode<<LIS3DH_INT_6D);
  
  AccelWriteRegister(LIS3DH_MEMS_I2C_ADDRESS,LIS3DH_INT1_CFG,value);
  
  return MEMS_SUCCESS;
}

    
/*******************************************************************************
* Function Name  : LIS3DH_SetInt6D4DConfiguration
* Description    : 6D, 4D Interrupt Configuration
* Input          : LIS3DH_INT1_6D_ENABLE, LIS3DH_INT1_4D_ENABLE, LIS3DH_INT1_6D_4D_DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetInt6D4DConfiguration(LIS3DH_INT_6D_4D_t ic) {
  u8_t value;
  u8_t value2;
  
  value = AccelReadRegister(LIS3DH_MEMS_I2C_ADDRESS,LIS3DH_INT1_CFG);

  value2 = AccelReadRegister(LIS3DH_MEMS_I2C_ADDRESS,LIS3DH_CTRL_REG5);
  
  if(ic == LIS3DH_INT1_6D_ENABLE){
    value &= 0xBF; 
    value |= (MEMS_ENABLE<<LIS3DH_INT_6D);
    value2 &= 0xFB; 
    value2 |= (MEMS_DISABLE<<LIS3DH_D4D_INT1);
  }
  
  if(ic == LIS3DH_INT1_4D_ENABLE){
    value &= 0xBF; 
    value |= (MEMS_ENABLE<<LIS3DH_INT_6D);
    value2 &= 0xFB; 
    value2 |= (MEMS_ENABLE<<LIS3DH_D4D_INT1);
  }
  
  if(ic == LIS3DH_INT1_6D_4D_DISABLE){
    value &= 0xBF; 
    value |= (MEMS_DISABLE<<LIS3DH_INT_6D);
    value2 &= 0xFB; 
    value2 |= (MEMS_DISABLE<<LIS3DH_D4D_INT1);
  }
  
  AccelWriteRegister(LIS3DH_MEMS_I2C_ADDRESS,LIS3DH_INT1_CFG,value);

  AccelWriteRegister(LIS3DH_MEMS_I2C_ADDRESS,LIS3DH_CTRL_REG5,value2);
  
 
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS3DH_Get6DPosition
* Description    : 6D, 4D Interrupt Position Detect
* Input          : Byte to empty by POSITION_6D_t Typedef
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_Get6DPosition(u8_t* val){
  u8_t value;
  
  value = AccelReadRegister(LIS3DH_MEMS_I2C_ADDRESS,LIS3DH_INT1_SRC); 
  
  value &= 0x7F;
  
  switch (value){
  case LIS3DH_UP_SX:   
    *val = LIS3DH_UP_SX;    
    break;
  case LIS3DH_UP_DX:   
    *val = LIS3DH_UP_DX;    
    break;
  case LIS3DH_DW_SX:   
    *val = LIS3DH_DW_SX;    
    break;
  case LIS3DH_DW_DX:   
    *val = LIS3DH_DW_DX;    
    break;
  case LIS3DH_TOP:     
    *val = LIS3DH_TOP;      
    break;
  case LIS3DH_BOTTOM:  
    *val = LIS3DH_BOTTOM;   
    break;
  }
  
  return MEMS_SUCCESS;  
}


/*******************************************************************************
* Function Name  : LIS3DH_SetInt1Threshold
* Description    : Sets Interrupt 1 Threshold
* Input          : Threshold = [0,31]
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetInt1Threshold(u8_t ths) {
  u8 RdRegData;
  if (ths > 127)
    return MEMS_ERROR;
  
  RdRegData |= ths;
  
  AccelWriteRegister(LIS3DH_MEMS_I2C_ADDRESS,LIS3DH_INT1_THS,RdRegData); 
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS3DH_SetInt1Duration
* Description    : Sets Interrupt 1 Duration
* Input          : Duration value
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetInt1Duration(LIS3DH_Int1Conf_t id) { 
  u8 RdRegData;
  
  if (id > 127)
    return MEMS_ERROR;

  RdRegData |= id;  
  
  AccelWriteRegister(LIS3DH_MEMS_I2C_ADDRESS,LIS3DH_INT1_DURATION,RdRegData);
  
  return MEMS_SUCCESS;
}

/**
  * @brief Interrupt Position Detect
  * @param LIS2DH_POSITION_6D_t direction
			LIS2DH_UP_SX       =     0x44,                  
			LIS2DH_UP_DX       =     0x42,
			LIS2DH_DW_SX      =     0x41,
			LIS2DH_DW_DX      =     0x48,
			LIS2DH_TOP           =     0x60,
			LIS2DH_BOTTOM     =     0x50,
			LIS2DH_UNKOWN    =     0xFF
  * @retval none
*/
void atel_gsensor_6DPosition_Handle(LIS3DH_POSITION_6D_t direction)
{
	u8 response;
	response = LIS3DH_Get6DPosition(&gsensordirection);
	if((response == 1) && (old_gsensordirection!=gsensordirection))
	{
	   	switch (gsensordirection)
	    	{
		    case LIS3DH_UP_SX:
				//UploadStableIntervalFlag = TRUE;
				SerialDbgPrintf(DbgCtl.Lis3dhDbgInfoEn,"\r\n[%s] ACCEL: Position = UP_SX  [0x%x]",FmtTimeShow(),gsensordirection); 
		    		break;
		    case LIS3DH_UP_DX:
				//UploadStableIntervalFlag = TRUE;
				SerialDbgPrintf(DbgCtl.Lis3dhDbgInfoEn,"\r\n[%s] ACCEL: Position = UP_DX [0x%x]",FmtTimeShow(),gsensordirection);
		    		break;
		    case LIS3DH_DW_SX:
				//UploadStableIntervalFlag = TRUE;
				SerialDbgPrintf(DbgCtl.Lis3dhDbgInfoEn,"\r\n[%s] ACCEL: Position = DW_SX [0x%x]",FmtTimeShow(),gsensordirection);
		    		break;              
		    case LIS3DH_DW_DX:
				//UploadStableIntervalFlag = TRUE;
				SerialDbgPrintf(DbgCtl.Lis3dhDbgInfoEn,"\r\n[%s] ACCEL: Position = DW_DX [0x%x]",FmtTimeShow(),gsensordirection);
		    		break; 
		    case LIS3DH_TOP:
				//UploadStableIntervalFlag = TRUE;
				SerialDbgPrintf(DbgCtl.Lis3dhDbgInfoEn,"\r\n[%s] ACCEL: Position = TOP [0x%x]",FmtTimeShow(),gsensordirection);
		    		break; 
		    case LIS3DH_BOTTOM:
				//UploadStableIntervalFlag = TRUE;
				SerialDbgPrintf(DbgCtl.Lis3dhDbgInfoEn,"\r\n[%s] ACCEL: Position = BOTTOM [0x%x]",FmtTimeShow(),gsensordirection); 				
		    		break; 
		    default:       		
				SerialDbgPrintf(DbgCtl.Lis3dhDbgInfoEn,"\r\n[%s] APP: Position = unknown [0x%x]",FmtTimeShow(),gsensordirection);
		    		break;
	    	}
		old_gsensordirection = gsensordirection;
	}
	
	// Print Out
	//atel_debug_sprintf(DbgCtl.Lis3dhDbgInfoEn,"\r\n[%s] APP: position = [0x%x]",FmtTimeShow( ),direction);
	

	// Add Customer Data Start


	// Add Customer Data End
}

void GsensorInterruptRoutine(void)
{
	//u8 RdRegData;
	
	// atel_gsensor_6DPosition_Handle((LIS3DH_POSITION_6D_t)gsensordirection);
	
	//LIS3DH_GetInt1Src(&RdRegData);
	
	LIS3DH_ResetInt1Latch();

}

// void GsensorIntProcess(void)
// {
// 	//u8 count;
// 	if(GetLis3dhAlarmStatus() == TRUE)
// 	{
// 		// Refresh IWDG: reload counter 
// 		TickHardwareWatchdog();
// 		// Reset Lis3dh Alm Flag
// 		SetLis3dhAlarmStatus(FALSE);
// 		//UploadStableIntervalFlag = TRUE;
// 		GsensorInterruptRoutine();
// 		// Check alm
// 		if(SendRptParm.DeviveActive == TRUE && \
// 			SendRptParm.Gsensor.AlmAllow == TRUE && \
// 			SendRptParm.Gsensor.AlmOccur == FALSE)
// 		{
// 			SendRptParm.Gsensor.AlmOccur = TRUE;
// 			// Print out
// 			SerialDbgPrintf(DbgCtl.Lis3dhDbgInfoEn,"\r\n[%s] GSENSOR: Int Happen, Action!",FmtTimeShow()); 
// 			// Send report
// 			SendRptParamToModem(REPORT_TYPE_GSENSOR);
			
// 		}
// 		else
// 		{
// 			// Print out
// 			SerialDbgPrintf(DbgCtl.Lis3dhDbgInfoEn,"\r\n[%s] GSENSOR: Int Happen Active(%d) Flag(%d %d)", \
// 				FmtTimeShow(), \
// 				SendRptParm.DeviveActive, \
// 				SendRptParm.Gsensor.AlmAllow, \
// 				SendRptParm.Gsensor.AlmOccur); 
// 		}
// 	}
// }


void GsensorIntProcess(void)
{
  //u8 count;
	if(GetLis3dhAlarmStatus() == TRUE)
	{
		// Refresh IWDG: reload counter 
		// TickHardwareWatchdog();
		// Reset Lis3dh Alm Flag
		SetLis3dhAlarmStatus(FALSE);
		//UploadStableIntervalFlag = TRUE;
		GsensorInterruptRoutine();

    SerialDbgPrintf(DbgCtl.Lis3dhDbgInfoEn,"\r\n[%s] GSENSOR: Int Happen",FmtTimeShow());
	}
}

/******************* (C) COPYRIGHT 2012 STMicroelectronics *****END OF FILE****/
