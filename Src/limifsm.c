/*******************************************************************************
* File Name          : Limfsm.c
* Author             : Taotao Yan
* Description        : This file provides all the Limfsm functions.

* History:
*  12/20/2014 : Limfsm V1.00
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <string.h>

#include "stm32f0xx_hal.h"

#include "limifsm.h"

/* Private define ------------------------------------------------------------*/
#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define ACTION_LIST(list) &list[1] /* Macro - Returns ptr to action list */
#define NEXT_STATE(list) list[0]   /* Macro - Returns next state value   */
#define FSM_TRACE_BASE_ID 0x4B0    /* define the base ID for FSM trace   */

#define NOT_APPLICABLE 0xFF
#define UNCHANGED 0xFE
#define UNEXPECTED 0xFD

/* Private function prototypes -----------------------------------------------*/
static FSMState *getEventEntry(FSMPtr fsm);

/* Local Functions */

/****************************************************************************
  *
  * Name:         getEventEntry
  *
  * Scope:       static
  *
  * Description: Returns the next-state/action list entry for the current
  *              state and event.
  *              
  * Parameters:
  *     FSMPtr fsm - The FSM from which to retrieve the information.
  *
  * Returns:
  *    FSMState* 
  *
  * Notes:
  *
  ****************************************************************************/
static FSMState *getEventEntry(FSMPtr fsm)
{
  uint16_t state_offset = (fsm->state) * (fsm->state_size);
  uint16_t event_offset = fsm->eventQ[fsm->get_index].event * (fsm->event_size);

  return ((fsm->state_table) + state_offset + event_offset);
}

/****************************************************************************
  *
  * Name:        initializeFSM
  *
  * Scope:       public
  *
  * Description: 
  *              
  * Parameters:
  *   FSMPtr fsm                      - The FSM to be initialized.
  *   FSMState* table                 - Next state/action list table.
  *   UINT8 num_state                 - Number of states in the FSM.
  *   UINT8 num_event                 - Number of events defined for the FSM.
  *   UINT8 num_action                - Maximum number of actions per event.
  *   FSMState initial_state          - Initial state for the FSM.
  *   FSMActionFuncPtr action_handler - Action handler function.
  *   void* user_parms -                Pointer to custom parameters for the FSM
  *
  * Returns:
  *    void
  *
  * Notes:
  *
  ****************************************************************************/
void initializeFSM(FSMPtr fsm, FSMState *table,
                   uint8_t num_state, uint8_t num_event, uint8_t num_action,
                   FSMState initial_state, FSMActionFuncPtr action_handler,
                   void *user_parms)
{
  /*
   fsm			&fsm_desc, 
   table			FSMTable(ATDH_fsm_tbl), 
   num_state		ATDHTBL_STATE_MAX,
   num_event		NUM_ATDHEvents, 
   num_action		ATDHTBL_NUM_ACT, 
   initial_state		ATDH_IDLE_ST, 
   action_handler	perform_action,
   user_parms		NULL
*/

  /* Build up our FSM Descriptor */
  fsm->state = initial_state;

#ifdef FSM_COND_ST
  fsm->conditional_state = CONDITIONAL_STATE;
  fsm->change_flag = FALSE;
#endif

  fsm->num_state = num_state;
  fsm->num_event = num_event;
  fsm->num_action = num_action;

  fsm->state_table = table;
  fsm->event_size = sizeof(FSMState) + sizeof(FSMAction) * num_action;
  fsm->state_size = num_event * fsm->event_size;

  fsm->action_handler = action_handler;
  fsm->user_parms = user_parms;

  /* Initialize Event Queue */
  fsm->put_index = fsm->get_index = fsm->num_in_queue = 0;
}

/****************************************************************************
  *
  * Name:        processEventWithNoParms
  *
  * Scope:       public
  *
  * Description: Sends an event to the specified state machine.  There are 
  *              no parameters associated with the event.
  *              
  * Parameters:
  *   FSMPtr fsm     - FSM to receive the event.
  *   FSMEvent event - Event id.
  *
  * Returns:
  *    void
  *
  * Notes:
  *
  ****************************************************************************/
void processEventWithNoParms(FSMPtr fsm, FSMEvent event)
{
#ifdef Limifsm_Debug
  LimifsmPrintf(DbgCtl.LimifsmDebugInfoEn, "\r\n--ProcessNoParms--event=%d--", event);
#endif
  /* No associated parameters */
  processEventWithParmStruct(fsm, event, (void *)NULL, 0);
}

/****************************************************************************
  *
  * Name:        processEventWithParmStruct
  *
  * Scope:       public
  *
  * Description: Sends an event to the specified state machine.  There are 
  *              parameters associated with the event.
  *              
  * Parameters:
  *   FSMPtr fsm        - FSM to receive the event.
  *   FSMEvent event    - Event id.
  *   void* parm_struct - Parameters associated with the event
  *   UINT8 size        - Size of event parameters (in bytes).
  *
  * Returns:
  *    void
  *
  * Notes:
  *
  ****************************************************************************/
void processEventWithParmStruct(FSMPtr fsm, FSMEvent event, void *parm_struct,
                                uint8_t size)
{
  uint8_t put_index = (fsm->put_index);

  /* Verify and load up our new event and associated parm list */
  IASSERT(event <= (fsm->num_event));
  IASSERT(size <= MAX_PARM_STRUCT_SIZE);
  IASSERT((fsm->num_in_queue) < EVENTQ_SIZE);

  /* Store the new event in the eventQ */
  fsm->eventQ[put_index].event = event;
  memcpy((void *)(fsm->eventQ[put_index].plist), parm_struct, size);

  /* Update the put pointer */
  if (++(fsm->put_index) == EVENTQ_SIZE)
  {
    fsm->put_index = 0;
  }

  /* Check to see if we're busy... */
  if ((fsm->num_in_queue)++ != 0)
  {
    return;
  }

  do
  {
    FSMState *current_event_entry = getEventEntry(fsm);
    FSMAction *action = ACTION_LIST(current_event_entry);
    FSMState next_state;
    FSMAction i = 0;
    uint8_t change_state = TRUE;

/* Base + Engine-specific Offset + Module offset */
#ifdef Limifsm_Debug
    LimifsmPrintf(DbgCtl.LimifsmDebugInfoEn, "\r\n--P1--State=%d,NOT_APPLICABLE=%d,event=%d,action=%d",
                  fsm->state,
                  NOT_APPLICABLE,
                  fsm->eventQ[fsm->get_index].event,
                  action[i]);
#endif

    /* Do all actions in the associated action list */
    for (i = 0; ((action[i] != NO_ACTION) && (i < (fsm->num_action))); i++)
    {
      if (action[i] == UNEXPECTED_EVENT_AC)
      {
#ifdef Limifsm_Debug
        LimifsmPrintf(DbgCtl.LimifsmDebugInfoEn, "\r\n--P2--State=%d,UNEXPECTED=%d,event=%d\n,action[%d]=%d",
                      fsm->state,
                      UNEXPECTED,
                      fsm->eventQ[fsm->get_index].event,
                      i, action[i]);
#endif
        change_state = FALSE;
        break;
      }
#ifdef Limifsm_Debug
      LimifsmPrintf(DbgCtl.LimifsmDebugInfoEn, "\r\n--Loop--State=%d,UNEXPECTED=%d,event=%d\n,action[%d]=%d,num_action=%d",
                    fsm->state,
                    UNEXPECTED,
                    fsm->eventQ[fsm->get_index].event,
                    i, action[i], fsm->num_action);
#endif
      if (!(*(fsm->action_handler))(fsm, action[i]))
      {
#ifdef FSM_COND_ST
#ifdef Limifsm_Debug
        LimifsmPrintf(DbgCtl.LimifsmDebugInfoEn, "\r\n--P3--action_handler--fsm->change_flag=%d", fsm->change_flag);
#endif
        if (fsm->change_flag == FALSE)
          change_state = FALSE;
        break;
#else
        change_state = FALSE;
#endif
      }
    } /* end - for (actions) */

#ifdef Limifsm_Debug
    LimifsmPrintf(DbgCtl.LimifsmDebugInfoEn, "\r\n--P4--change_state=%d", change_state);
#endif

    /* Change state */
    if (change_state)
    {
#ifdef FSM_COND_ST
      if ((next_state = NEXT_STATE(current_event_entry)) == CONDITIONAL_STATE)
      {
#ifdef Limifsm_Debug
        LimifsmPrintf(DbgCtl.LimifsmDebugInfoEn, "\r\n--P5--next_state=%d,conditional_state=%d", next_state, fsm->conditional_state);
#endif
        next_state = fsm->conditional_state;
      }
#else
      next_state = NEXT_STATE(current_event_entry);
#endif

      IASSERT(next_state < (fsm->num_state));
    }
    else
    {
#ifdef Limifsm_Debug
      LimifsmPrintf(DbgCtl.LimifsmDebugInfoEn, "\r\n--P6--next_state=%d,state=%d", next_state, fsm->state);
#endif
      next_state = fsm->state;
    }

    if (next_state != (fsm->state))
    {
#ifdef Limifsm_Debug
      LimifsmPrintf(DbgCtl.LimifsmDebugInfoEn, "\r\n--P7--State=%d,next_state=%d,event=%d",
                    fsm->state,
                    next_state,
                    fsm->eventQ[fsm->get_index].event);
#endif
      fsm->state = next_state;
    }
    else
    {
#ifdef Limifsm_Debug
      LimifsmPrintf(DbgCtl.LimifsmDebugInfoEn, "\r\n--P8--State=%d,UNCHANGED=%d,get_index=%d,event=%d",
                    fsm->state,
                    UNCHANGED,
                    fsm->get_index,
                    fsm->eventQ[fsm->get_index].event);
#endif
    }

    if (++(fsm->get_index) == EVENTQ_SIZE)
    {
      fsm->get_index = 0;
    }
#ifdef Limifsm_Debug
    LimifsmPrintf(DbgCtl.LimifsmDebugInfoEn, "\r\n--P9--get_index=%d,num_in_queue=%d", fsm->get_index, fsm->num_in_queue);
#endif
  } while (--(fsm->num_in_queue) != 0);
}

#ifdef FSM_COND_ST
void setConditionalState(FSMPtr fsm, FSMState cond_state)
{
  fsm->conditional_state = cond_state;
}

void setChangeFlag(FSMPtr fsm, uint8_t flag)
{
  fsm->change_flag = flag;
}
#endif

/****************************************************************************
  *
  * Name:        getCurrentEvent 
  *
  * Scope:       public
  *
  * Description: Retrieves the current event for the specified FSM.
  *              
  * Parameters:
  *   FSMPtr fsm - FSM Descriptor.
  *
  * Returns:
  *   FSMEvent
  *
  * Notes:   This function is intended to be called from an action routine.
  *
  ****************************************************************************/
FSMEvent getCurrentEvent(FSMPtr fsm)
{
  return (fsm->eventQ[fsm->get_index].event);
}

/****************************************************************************
  *
  * Name:        getEventParameters
  *
  * Scope:       public
  *
  * Description: Returns the parameters associated with the event currently
  *              being processed by the specified FSM.
  *              
  * Parameters:
  *    FSMPtr fsm - FSM specifier.
  *
  * Returns:
  *   void
  *
  * Notes:
  *
  ****************************************************************************/
void *getEventParameters(FSMPtr fsm)
{
  return ((void *)(fsm->eventQ[fsm->get_index].plist));
}

/*******************************************************************************
    Copyrights (C) Asiatelco Technologies Co., 2003-2018. All rights reserved
                                 End Of The File
*******************************************************************************/
