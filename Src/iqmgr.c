/*******************************************************************************
* File Name          : IqMgr.c
* Author             : Taotao Yan
* Description        : This file provides all the IqMgr functions.

* History:
*  12/20/2014 : IqMgr V1.00
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <string.h>

#include "stm32f0xx_hal.h"

#include "iqmgr.h"
#include "limifsm.h"

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif
/* Private variables ---------------------------------------------------------*/
static uint8_t Iqmgr_mutex;

/* Function definition -------------------------------------------------------*/
void IqmgrMutexInit(void)
{
  //pthread_mutex_init(&Iqmgr_mutex, NULL);
  //pthread_mutexattr_init(&Iqmgr_mutex);
  Iqmgr_mutex = FALSE;
}

uint8_t CompareAndExchange(uint8_t *ptr, uint8_t old, uint8_t new)
{
  uint8_t actual = *ptr;
  if (actual == old)
    *ptr = new;

  return actual;
}

void pthread_mutex_lock(uint8_t *Mutex)
{
  while (CompareAndExchange(Mutex, 0, 1) == 1)
    ; // spin
}

void pthread_mutex_unlock(uint8_t *Mutex)
{
  Iqmgr_mutex = FALSE;
}

uint16_t min16(uint16_t a, uint16_t b)
{
  return ((a <= b) ? a : b);
}

uint8_t iqPut(IQDesc *qd, void *data)
{
  /* Check for queue full */
  if ((qd->numInQueue) >= (qd->numRecords))
  {
    return FALSE;
  }

  pthread_mutex_lock(&Iqmgr_mutex);
  /* Copy user record into the queue */
  memcpy(&(qd->q[(qd->putIndex) * (qd->recordSize)]), data, (qd->recordSize));
  (qd->numInQueue)++;

  /* Update the insertion pointer */
  if (++(qd->putIndex) >= (qd->numRecords))
  {
    qd->putIndex = 0;
  }
  pthread_mutex_unlock(&Iqmgr_mutex);

  return TRUE;
}

uint16_t iqBlockPut(IQDesc *qd, void *data, uint16_t recordsToPut)
{
  uint16_t bytesInFirstSegment;
  uint16_t putIndex;
  uint16_t recordsInSegment;

  if ((recordsToPut =
           min16(recordsToPut, ((qd->numRecords) - (qd->numInQueue)))) == 0)
  {
    return 0;
  }

  IASSERT(((recordsToPut) * (qd->recordSize)) <= 65535UL);

  /* Segment the block, if required, to wrap around the end of the queue. */

  /* Put the first segment. */
  putIndex = qd->putIndex;
  recordsInSegment = min16(recordsToPut, ((qd->numRecords) - putIndex));
  bytesInFirstSegment = recordsInSegment * (qd->recordSize);
  memcpy(&(qd->q[putIndex]), data, bytesInFirstSegment);
  qd->putIndex += recordsInSegment;

  /* Put the second segment, if required.
    * Note: The second segment always begins at the start of the queue. */
  if ((recordsInSegment = recordsToPut - recordsInSegment) != 0)
  {
    data = &(((uint8_t *)data)[bytesInFirstSegment]);
    memcpy((qd->q), data, (recordsInSegment * (qd->recordSize)));
    qd->putIndex = recordsInSegment;
  }

  /* If required, wrap the put index.
    * This occurs only if the block was completely contained in the first
    * segment and was exactly aligned with the end of the queue. */
  else if ((qd->putIndex) >= (qd->numRecords))
  {
    qd->putIndex = 0;
  }

  /* Update numInQueue */
  qd->numInQueue += recordsToPut;
  return recordsToPut;
}

uint8_t iqGet(IQDesc *qd, void *data)
{
  /* Check for queue empty */
  if (qd->numInQueue == 0)
  {
    return FALSE;
  }

  pthread_mutex_lock(&Iqmgr_mutex);
  /* Copy record to user buffer */
  memcpy(data, &(qd->q[(qd->getIndex) * (qd->recordSize)]), (qd->recordSize));
  pthread_mutex_unlock(&Iqmgr_mutex);

  return TRUE;
}

uint8_t iqDelGet(IQDesc *qd, void *data)
{
  /* Check for queue empty */
  if (qd->numInQueue == 0)
  {
    return FALSE;
  }

  pthread_mutex_lock(&Iqmgr_mutex);
  /* Copy record to user buffer */
  memcpy(data, &(qd->q[(qd->getIndex) * (qd->recordSize)]), (qd->recordSize));
  qd->numInQueue--;

  /* Update the extraction pointer */
  if (++(qd->getIndex) >= (qd->numRecords))
  {
    qd->getIndex = 0;
  }
  pthread_mutex_unlock(&Iqmgr_mutex);

  return TRUE;
}

uint16_t iqBlockGet(IQDesc *qd, void *data, uint16_t recordsToGet)
{
  uint16_t bytesInFirstSegment;
  uint16_t recordsInSegment;
  uint16_t getIndex;

  if ((recordsToGet = min16(recordsToGet, (qd->numInQueue))) == 0)
  {
    return 0;
  }

  IASSERT(((recordsToGet) * (qd->recordSize)) <= 65535UL);

  /* Retrieve the block in 2 segments, if required, 
    * to accomodate buffer wrap. */

  /* Retrieve the first segment */
  getIndex = qd->getIndex;
  recordsInSegment = min16(recordsToGet, ((qd->numRecords) - getIndex));
  bytesInFirstSegment = recordsInSegment * (qd->recordSize);
  memcpy(data, &(qd->q[getIndex]), bytesInFirstSegment);
  qd->getIndex += recordsInSegment;

  /* Retrieve the second segment, if required.
    * Note: The second segment always begins at the start of the queue. */
  if ((recordsInSegment = recordsToGet - recordsInSegment) != 0)
  {
    data = &(((uint8_t *)data)[bytesInFirstSegment]);
    memcpy(data, (qd->q), (recordsInSegment * (qd->recordSize)));
    qd->getIndex = recordsInSegment;
  }

  /* If required, wrap the get index.
    * This occurs only if the block was completely contained in the first
    * segment and was exactly aligned with the end of the queue. */
  else if ((qd->getIndex) >= (qd->numRecords))
  {
    qd->getIndex = 0;
  }

  qd->numInQueue -= recordsToGet;
  return recordsToGet;
}

void *iqGetRef(IQDesc *qd)
{
  if ((qd->numInQueue) == 0)
  {
    return NULL;
  }

  return &(qd->q[(qd->getIndex) * (qd->recordSize)]);
}

uint8_t iqDelete(IQDesc *qd)
{
  if ((qd->numInQueue) == 0)
  {
    return FALSE;
  }

  qd->numInQueue--;

  /* Update the extraction pointer */
  if (++(qd->getIndex) >= (qd->numRecords))
  {
    qd->getIndex = 0;
  }
  return TRUE;
}

uint16_t iqNum(IQDesc *qd)
{
  return (qd->numInQueue);
}

uint16_t iqRoom(IQDesc *qd)
{
  return ((qd->numRecords) - (qd->numInQueue));
}

void iqFlush(IQDesc *qd)
{
  qd->putIndex = qd->getIndex = qd->numInQueue = 0;
}

void iqResetScan(IQDesc *qd, uint16_t start)
{
  /* Check for past end of queue */
  if (start >= (qd->numInQueue))
  {
    qd->scanIndex = (qd->putIndex);
    qd->numToScan = 0;
    return;
  }

  if ((qd->scanIndex = (qd->getIndex) + start) >= (qd->numRecords))
  {
    qd->scanIndex -= qd->numRecords;
  }

  qd->numToScan = qd->numInQueue - start;
}

void *iqScan(IQDesc *qd)
{
  uint16_t scanIndex = qd->scanIndex;
  uint16_t rtnIndex;

  if (!(qd->numToScan))
  {
    return NULL;
  }

  qd->numToScan--;

  rtnIndex = scanIndex;
  if (++scanIndex >= (qd->numRecords))
  {
    scanIndex = 0;
  }
  qd->scanIndex = scanIndex;

  return &(qd->q[rtnIndex * (qd->recordSize)]);
}

void iqInit(IQDesc *qd, void *q, uint16_t numRecords, uint16_t recordSize)
{
  IASSERT(q);
  IASSERT(recordSize);

  qd->q = (uint8_t *)q;
  qd->numRecords = numRecords;
  qd->recordSize = recordSize;
  iqFlush(qd);
}

/*******************************************************************************
     Copyrights (C) Asiatelco Technologies Co., 2003-2018. All rights reserved
                             End Of The File
*******************************************************************************/
