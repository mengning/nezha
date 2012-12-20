
/********************************************************************/
/* Copyright (C) SSE-USTC, 2010                                     */
/*                                                                  */
/*  FILE NAME             :  event.c                                */
/*  PRINCIPAL AUTHOR      :  Mengning                               */
/*  SUBSYSTEM NAME        :  ChatSys                                */
/*  MODULE NAME           :  ChatSys                                */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  ANY                                    */
/*  DATE OF FIRST RELEASE :  2010/12/03                             */
/*  DESCRIPTION           :  implement of Event ¡¤¡¤	                */
/********************************************************************/

/*
 * Revision log:
 *
 * Created by Mengning,2010/12/03
 *
 */
 

#include"event.h"
#include<stdio.h>
#include<semaphore.h>

#define SUCCESS 0
#define FAILURE (-1)
/*
 * Initialize an Event
 * input	: tEvent *pEvent, Memory allocate outside
 * input	: Value (0)
 * return	: SUCCESS(0)/FAILURE(-1)
 *
 */
int EventInit(tEvent *pEvent,int Value)
{
	if(sem_init(pEvent,0,Value) != SUCCESS)
    {
    	printf("sem_init failure!\n");
        return FAILURE;
    }
    return SUCCESS;
}
/*
 * Shutdown an Event
 * input	: ...
 *
 */
int EventShut(tEvent *pEvent)
{
	sem_destroy(pEvent);
	return SUCCESS;
}
/*
 * ....
 *
 */
int SentEvent(tEvent *pEvent)
{
	return sem_post(pEvent);
}
/*
 * ...
 * return	: SUCCESS(0)/FAILURE(-1)
 *
 */
int WaitEvent(tEvent *pEvent)
{
	return sem_wait(pEvent);
}



