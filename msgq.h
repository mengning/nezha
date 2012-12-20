
/********************************************************************/
/* Copyright (C) SSE-USTC, 2010                                     */
/*                                                                  */
/*  FILE NAME             :  msgq.h                                 */
/*  PRINCIPAL AUTHOR      :  Mengning                               */
/*  SUBSYSTEM NAME        :  ChatSys                                */
/*  MODULE NAME           :  ChatSys                                */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  ANY                                    */
/*  DATE OF FIRST RELEASE :  2010/12/03                             */
/*  DESCRIPTION           :  interface of msg Queue	                */
/********************************************************************/

/*
 * Revision log:
 *
 * Created by Mengning,2010/12/03
 *
 */

#ifndef _MSG_QUEUE_H_
#define _MSG_QUEUE_H_


/*
 * Queue Node Type
 */
typedef struct QueueNode
{
	struct QueueNode * pNext;
}tQueueNode;

/*
 * Queue Type
 */
typedef struct Queue
{
	tQueueNode *pHead;
	tQueueNode *pTail;
	int			SumOfNode;
	
}tQueue;

/*
 * Create a Queue
 */
int QueueCreate(tQueue *pQ);
/*
 * Delete a Queue
 */
int QueueDelete(tQueue *pQ);
/*
 * Put in a Queue Msg to Q
 */
int QueueInMsg(tQueue *pQ,tQueueNode * pMsg);
/*
 * Get out a Queue Msg from Q
 */
int QueueOutMsg(tQueue *pQ,tQueueNode ** ppMsg);

#endif /* _MSG_QUEUE_H_ */


