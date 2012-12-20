
/********************************************************************/
/* Copyright (C) SSE-USTC, 2010                                     */
/*                                                                  */
/*  FILE NAME             :  event.h                                */
/*  PRINCIPAL AUTHOR      :  Mengning                               */
/*  SUBSYSTEM NAME        :  ChatSys                                */
/*  MODULE NAME           :  ChatSys                                */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  ANY                                    */
/*  DATE OF FIRST RELEASE :  2010/12/03                             */
/*  DESCRIPTION           :  interface of Event ¡¤¡¤	                */
/********************************************************************/

/*
 * Revision log:
 *
 * Created by Mengning,2010/12/03
 *
 */

#ifndef _EVENT_H_
#define _EVENT_H_

#include<semaphore.h>

/* Event typedefine */
typedef sem_t tEvent;

/*
 * Initialize an Event
 * input	: tEvent *pEvent, Memory allocate outside
 * input	: Value (0)
 * return	: SUCCESS(0)/FAILURE(-1)
 *
 */
int EventInit(tEvent *pEvent,int Value);
/*
 * Shutdown an Event
 * input	: ...
 *
 */
int EventShut(tEvent *pEvent);
/*
 * ....
 *
 */
int SentEvent(tEvent *pEvent);
/*
 * ...
 * return	: SUCCESS(0)/FAILURE(-1)
 *
 */
int WaitEvent(tEvent *pEvent);

#endif /* _EVENT_H_ */


