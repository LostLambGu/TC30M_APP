/*******************************************************************************
* File Name          : IqMgr.h
* Author               : Taotao Yan
* Description        : This file provides all the IqMgr functions.

* History:
*  12/20/2014 : IqMgr V1.00
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _IQMGR_H
#define _IQMGR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
#include "SendATCmd.h"

/* Exported types ------------------------------------------------------------*/
typedef struct
{
  /* Static queue descriptors */
  uint8_t *q;
  uint16_t numRecords;
  uint16_t recordSize;

  /* Dynamic queue descriptors */
  uint16_t putIndex;
  uint16_t getIndex;
  uint16_t numInQueue;

  uint16_t scanIndex;
  uint16_t numToScan;
} IQDesc;

// Variable Declared

//Function Declare
extern void IqmgrMutexInit(void);
extern void IqmgrMutexDestroy(void);
extern uint16_t min16(uint16_t a, uint16_t b);
extern uint8_t iqPut(IQDesc *qd, void *data);
extern uint16_t iqBlockPut(IQDesc *qd, void *data, uint16_t recordsToPut);
extern uint8_t iqGet(IQDesc *qd, void *data);
extern uint8_t iqDelGet(IQDesc *qd, void *data);
extern uint16_t iqBlockGet(IQDesc *qd, void *data, uint16_t recordsToGet);
extern void *iqGetRef(IQDesc *qd);
extern uint8_t iqDelete(IQDesc *qd);
extern uint16_t iqNum(IQDesc *qd);
extern uint16_t iqRoom(IQDesc *qd);
extern void iqFlush(IQDesc *qd);
extern void iqResetScan(IQDesc *qd, uint16_t start);
extern void *iqScan(IQDesc *qd);
extern void iqInit(IQDesc *qd, void *q, uint16_t numRecords, uint16_t recordSize);

#ifdef __cplusplus
}
#endif

#endif

/*******************************************************************************
                        Copyrights (C) Asiatelco Technologies Co., 2003-2014. All rights reserved
                                                             End Of The File
*******************************************************************************/
