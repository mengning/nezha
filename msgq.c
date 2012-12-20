
/********************************************************************/
/* Copyright (C) SSE-USTC, 2010                                     */
/*                                                                  */
/*  FILE NAME             :  msgq.c                                 */
/*  PRINCIPAL AUTHOR      :  Mengning                               */
/*  SUBSYSTEM NAME        :  ChatSys                                */
/*  MODULE NAME           :  ChatSys                                */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  ANY                                    */
/*  DATE OF FIRST RELEASE :  2010/12/04                             */
/*  DESCRIPTION           :  implement a Queue	                    */
/********************************************************************/

/*
 * Revision log:
 *
 * Created by Mengning,2010/12/04
 *
 */


#include"msgq.h"
#include<stdio.h>
#include<stdlib.h>

#define SUCCESS 0
#define FAILURE (-1)

/*
 * Create a Queue
 */
int QueueCreate(tQueue * pQ)
{
	if(pQ == NULL)
		return FAILURE;
	pQ->pHead = NULL;
	pQ->pTail = NULL;
	pQ->SumOfNode = 0;
	return SUCCESS;
}
/*
 * Delete a Queue
 */
int QueueDelete(tQueue *pQ)
{
	if(pQ == NULL)
		return FAILURE;	
	while(pQ->pHead != NULL)
	{
		tQueueNode * p = pQ->pHead;
		pQ->pHead = pQ->pHead->pNext;
		pQ->SumOfNode -= 1 ;
		free(p);
	}
	pQ->pHead = NULL;
	pQ->pTail = NULL;
	pQ->SumOfNode = 0;
	return SUCCESS;		
}
/*
 * Put in a Queue Msg to Q
 */
int QueueInMsg(tQueue *pQ,tQueueNode * pMsg)
{
	if(pQ == NULL || pMsg == NULL)
		return FAILURE;	
	pMsg->pNext = NULL;
	if(pQ->pHead == NULL)
		pQ->pHead = pMsg;
	if(pQ->pTail == NULL)
		pQ->pTail = pMsg;
	else
	{
		pQ->pTail->pNext = pMsg;
		pQ->pTail = pMsg;
	}
	pQ->SumOfNode += 1 ;
	return SUCCESS;		
}
/*
 * Get out a Queue Msg from Q
 */
int QueueOutMsg(tQueue *pQ,tQueueNode ** ppMsg)
{
	if(pQ == NULL || ppMsg == NULL)
		return FAILURE;	
	if(pQ->pHead != NULL)
	{
		*ppMsg = pQ->pHead;
		pQ->pHead = pQ->pHead->pNext;
		pQ->SumOfNode -= 1 ;
		if(pQ->SumOfNode == 0)
		{
			pQ->pTail = NULL;	
		}	
		return SUCCESS;	
	}
	return FAILURE;		
}



