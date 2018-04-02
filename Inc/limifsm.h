/*******************************************************************************
 * File Name          : Limfsm.h
 * Author               : Taotao Yan
 * Description        : This file provides all the Limfsm functions.
 
 * History:
 *  12/20/2014 : Limfsm V1.00
 *******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LIMIFSM__H__
#define __LIMIFSM__H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "stm32f0xx_hal.h"

#define CONST const
#define IASSERT(expr)
#define ROM

/* Constants */
/* <TBD: Due to a GNU compiler-bug, the MAX_PARM_STRUCT_SIZE must be set
          to: ((a multiple of 4) - 1).> */
#define MAX_PARM_STRUCT_SIZE 511

#define EVENTQ_SIZE 1

#define UNEXPECTED_EVENT_AC 0xFE
#define NO_EVENT 0xFF
#define NO_ACTION 0

#ifndef FSM_COND_ST
#define FSM_COND_ST 1
#endif

#ifdef FSM_COND_ST
#define CONDITIONAL_STATE 0x7F
#define CHANGE_STATE_ALWAYS TRUE
#define ACTION_DRIVES_CHANGE FALSE
#endif

/* Define FSM Base Elements */
typedef uint8_t FSMState;
typedef uint8_t FSMEvent;
typedef uint8_t FSMAction;

/* <TBD: Due to a GNU compiler-bug, 'plist' must be the first entry in this
          structure.> */
typedef struct
{
  uint8_t plist[MAX_PARM_STRUCT_SIZE];
  FSMEvent event;
} FSMTBLEvent;

/* Define FSM Descriptor Structure */

struct FSMDescriptorStruct
{
  FSMTBLEvent eventQ[EVENTQ_SIZE]; /* Event Queue        */
  uint8_t put_index;               /* Put Index for Q    */
  uint8_t get_index;               /* Get Index for Q    */
  uint8_t num_in_queue;            /* Current number of entries in the queue */

  FSMState state; /* Current State      */

#ifdef FSM_COND_ST
  FSMState conditional_state; /* Conditional State  */
  uint8_t change_flag;        /* If TRUE, always change state */
#endif

  FSMState num_state;   /* Number of States   */
  FSMEvent num_event;   /* Number of Events   */
  FSMAction num_action; /* Number of Actions  */

  FSMState *state_table; /* State Table        */
  uint16_t state_size;   /* State Entry Size   */
  uint16_t event_size;   /* Event Entry Size   */

  /* Ptr to Action Handler Function */
  uint8_t (*action_handler)(struct FSMDescriptorStruct *fsm, FSMAction action);

  /* Instance Related User Parameters */
  void *user_parms;
};

/* Useful Typedefs */
typedef struct FSMDescriptorStruct FSMDescriptor;
typedef FSMDescriptor *FSMPtr;
typedef uint8_t (*FSMActionFuncPtr)(FSMPtr fsm, FSMAction action);

/* Generic FSM Processing Functions */
extern void initializeFSM(FSMPtr fsm, FSMState *table,
                          uint8_t num_state, uint8_t num_event, uint8_t num_action,
                          FSMState initial_state, FSMActionFuncPtr action_handler,
                          void *user_parms);

extern void processEventWithNoParms(FSMPtr fsm, FSMEvent event);
extern void processEventWithParmStruct(FSMPtr fsm, FSMEvent event, void *parm_struct, uint8_t size);

#ifdef FSM_COND_ST
extern void setConditionalState(FSMPtr fsm, FSMState cond_state);
extern void setChangeFlag(FSMPtr fsm, uint8_t change_always);
#endif

/* Event Parameter Query */
extern void *getEventParameters(FSMPtr fsm);
extern FSMEvent getCurrentEvent(FSMPtr fsm);

/* FSM Table Generation Macros */

/* The macro MAKEFSMTable creates a FSM table, just provide the name of the
  * table, the number of states, the number of events and the number of
  * actions that are to be executed to get to that state.
  */

/*
  * Example of make fsm table macro
  * Intialize table
  * MAKEFSMTable(callFSM,2,3,1) = {{1,b},{2,c},{3,d},{4,e},{5,f},{0,a}};
  *    where 0-5 represent the different states
  *    where a-e represent different actions
  * Note this assumes that the states and actions are the same size 
  * The + 1 is for the state entry
  */
#define MAKEFSMTable(tableName, numStates, numEvents, numActions) \
  static CONST uint8_t ROM tableName[numStates][numEvents][numActions + 1]

/* The macro FSMTable is used to Instantiate a FSMTable
  * It just does the required type casting
  *
  * Example
  * Instantiate a FSMTBL with FSMTable callFSM
  * FSMTBL callFSMTable(FSMTable(callFSM),2,3,1,0);
  */
#define FSMTable(tableName) (FSMState *)&tableName

#ifdef __cplusplus
}
#endif

#endif /* __FSM_H_ */

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2014. All rights reserved
                                End Of The File
*******************************************************************************/
