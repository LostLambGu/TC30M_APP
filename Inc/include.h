#ifndef __INCLUDE_H__
#define __INCLUDE_H__

#include <string.h>
#include <stdlib.h>

#include "stm32f0xx_hal.h"

typedef enum{
    HOLD = 0,
    RELEASE = 1,
  }FunCtrl;
  
  typedef enum{
    NO_SERVICE= 0,
    IN_SERVICE = 1,
  }ServiceStatus;
  
  typedef enum{
    TCP = 0,
    UDP = 1,
  }NetType;
  
  typedef enum{
    PLUGIN     	= 0,
    PLUGOUT      = 1,
  }ChgPlugT;
  
  typedef enum{
    LOW 	= 0,
    HIGH  	= 1,
  }etLevel;
  
  typedef enum{
    ACK 	= 1,
    NACK 	= 2,
    NOACK 	= 3,
  }etAck;
  
  // Error codes
  typedef enum{
    NO_ERROR 		= 0x00,
    ACK_ERROR 		= 0x01,
    TIME_OUT_ERROR  = 0x02, // timeout error
    CHECKSUM_ERROR  = 0x04, // checksum mismatch error
    UNIT_ERROR 		= 0x08,
    PARM_ERROR     	= 0x80, // parameter out of range error
    //CHECKSUM_ERROR 	= 0x04,
    
  }etError;

  typedef enum 
  {
      KISSOFF_AC = 0,	//accelerometer
      KISSOFF_TI,		//tilt (accelerometer)
      KISSOFF_VI,		//vibration 
      KISSOFF_MO,		// MiniDIN 
      KISSOFF_TE,		//low and high temperature
      KISSOFF_TP,		//tamper
      KISSOFF_LB1,		// low battery 1
      KISSOFF_LB2,		// low battery 2
      KISSOFF_MAX
  }KissOffType;

typedef int32_t  		s32;
typedef int16_t 		s16;
typedef int8_t  		s8;

typedef uint32_t  		u32;
typedef uint16_t 		u16;
typedef uint8_t  		u8;

typedef uint8_t 		AtDHEvent;
typedef uint16_t 		UINT16;
typedef uint8_t 		UINT8;

typedef uint32_t		uint;
typedef uint32_t       	uint32;
typedef uint16_t      	uint16;
typedef uint8_t       		uint8;

typedef int32_t    		int32;
typedef int16_t   		int16;
typedef int8_t    		int8;

typedef uint8_t   		u8t;      ///< range: 0 .. 255
typedef int8_t    	 	i8t;      ///< range: -128 .. +127
																    	
typedef uint16_t  		u16t;     ///< range: 0 .. 65535
typedef int16_t    		i16t;     ///< range: -32768 .. +32767
																    	
typedef uint32_t   		u32t;     ///< range: 0 .. 4'294'967'295
typedef int32_t     		i32t;     ///< range: -2'147'483'648 .. +2'147'483'647

typedef uint8_t bool;

typedef unsigned char      u8_t;
typedef signed char      s8_t;
typedef unsigned short    u16_t;
typedef signed   short    s16_t;

typedef unsigned int    u32_t;
typedef signed   int    s32_t;

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef NRCMD
#define NRCMD (1)
#endif

#include "initialization.h"
#include "uart_api.h"
#include "limifsm.h"
#include "rtcclock.h"
#include "iqmgr.h"
#include "usrtimer.h"

// #include "sendatcmd.h"
// #include "ltecatm.h"
// #include "network.h"
// #include "parseatat.h"

#endif
