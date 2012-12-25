/********************************************************************/
/* Copyright (C) MC2Lab-USTC, 2012                                  */
/*                                                                  */
/*  FILE NAME               :  dbserver.c                           */
/*  PRINCIPAL AUTHOR        :  Mengning                             */
/*  SUBSYSTEM NAME          :  network                              */
/*  MODULE NAME             :  dbserver                             */
/*  LANGUAGE                :  C                                    */
/*  TARGET ENVIRONMENT      :  Linux                                */
/*  DATE OF FIRST RELEASE   :  2012/12/14                           */
/*  DESCRIPTION             :  Impement of Socket Server Engine     */
/*                             to handle clients requests.          */
/********************************************************************/

/*
 * Revision log:
 *
 * Created by Mengning ,2012/12/14
 *
 */

#include "dbapi.h"
#include "socketwrapper.h"
#include "protocol.h"
#include "event.h"
#include "msgq.h"
#include "cmdline.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <time.h>

#define PORT            5001
#define IP_ADDR         "127.0.0.1"
#define MAX_BUF_LEN     1024

#define debug   

#define MAX_TASK_NUM      1
pthread_t thread_id[MAX_TASK_NUM];
tEvent event[MAX_TASK_NUM];
tQueue taskq[MAX_TASK_NUM];
pthread_mutex_t dbmutex;
typedef struct TaskNode
{
    tQueueNode next;
    tServiceHandler req;
    char Buf[MAX_BUF_LEN];
    int BufSize;
}tTaskNode;  

#define RANDOM(x)                           \
                  srand((unsigned)time(0)); \
                  i = random()%x;

/*
 * store cliend fd and database mapping table
 */
tDatabase  CDManager = NULL;
#define    InitCDManager()  CDManager = MDBCreate();
#define    ShutCDManager()  MDBDelete(CDManager);
#define    AttachCD(c,mdb)   \
        tValue v;v.str = (char*)&mdb;v.len = sizeof(tDatabase); \
        MDBSetKeyValue(CDManager,c,v);
#define    DettachCD(c)   \
        MDBDelKeyValue(CDManager,c);          
#define    GetMdb(c,mdb)   \
        tValue v;v.str = (char*)&mdb;v.len = sizeof(tDatabase); \
        MDBGetKeyValue(CDManager,c,&v);      

int HandleRequests(int tasknum);
int HandleOneRequest(tServiceHandler h,char *Buf,int BufSize);

int main()
{ 
    printf("Nezha Database System Server Engine Starts\n");    
    int i;
    if(MAX_TASK_NUM > 0)
    {
        for(i = 0;i < MAX_TASK_NUM;i++)
        {
            EventInit(&event[i],0);
            QueueCreate(&taskq[i]);
            int temp = i;
            if(pthread_create(&thread_id[i],NULL,(void*)HandleRequests,(void*)temp) != 0)
            {
                fprintf(stderr,"pthread_create Error,%s:%d\n",__FILE__,__LINE__);
                exit(-1);
            }            
        }           
    } 
    pthread_mutex_init(&dbmutex, NULL);
    /* start command line console */
    pthread_t cmdline_id;
    if(pthread_create(&cmdline_id,NULL,(void*)cmdline,(void*)0) != 0)
    {
        fprintf(stderr,"cmdline pthread_create Error,%s:%d\n",__FILE__,__LINE__);
        exit(-1);
    }       
    InitCDManager();
    /* Server Engine for Clients' Requests */
    tServiceHandler request = -1;
    InitializeNetService(IP_ADDR,PORT);
    while(1)
    {
        /* return the client fd who have real data request */
        request = ServiceStart();
        tTaskNode *pnode = malloc(sizeof(tTaskNode));
        pnode->BufSize = MAX_BUF_LEN;
        if(RecvData(request,pnode->Buf,&(pnode->BufSize)) == 0)
        {
            /* close by peer */
            ServiceStop(request);
            continue;        
        }        
        if(MAX_TASK_NUM > 0)
        {            
            pnode->req = request;
            RANDOM(MAX_TASK_NUM);//i = random(MAX_TASK_NUM)
            debug("fd %d send to task %d\n",request,i);
            QueueInMsg(&taskq[i],(tQueueNode*)pnode);
            SentEvent(&event[i]);
        }
        else
        {
            HandleOneRequest(request,pnode->Buf,pnode->BufSize);
            free(pnode);
        }  
    }
    ShutdownNetService();
    ShutCDManager();
    if(MAX_TASK_NUM > 0)
    {        
        for(i = 0;i < MAX_TASK_NUM;i++)
        {
            EventShut(&event[i]);
            QueueDelete(&taskq[i]);
        }    
    }    
    return 0;
}

int ErrorResponse(tServiceHandler h,char * errorinfo)
{
    char Buf[MAX_BUF_LEN] = "\0";
    int BufSize = MAX_BUF_LEN;
    FormatData1(Buf,&BufSize,ERROR_RSP,errorinfo,strlen(errorinfo));
    SendData(h,Buf,BufSize);
    return 0;    
}

int HandleRequests(int tasknum)
{
    debug("task %d starts\n",tasknum);
    /* Handle Requests */
    tServiceHandler h = -1;
    int i = tasknum;
    while(1)
    {
        WaitEvent(&event[i]);
        debug("task %d get a event\n",i);
        tTaskNode *pnode = NULL;
        QueueOutMsg(&taskq[i],(tQueueNode**)&pnode);
        h = pnode->req;              
        if(HandleOneRequest(h,pnode->Buf,pnode->BufSize) == -1)
        {
            continue;
        }
        free(pnode);
    }
}
int HandleOneRequest(tServiceHandler h,char *Buf,int BufSize)
{
    int cmd = -1;
    int DataNum = -1;
    char Data1[MAX_BUF_LEN] = "\0";
    int Data1Size = MAX_BUF_LEN;
    char Data2[MAX_BUF_LEN] = "\0";
    int Data2Size = MAX_BUF_LEN;  
      
    /* Handle One Request */
    if(BufSize == 0)
    {
        return -1; 
    }
    Data1Size = MAX_BUF_LEN;
    Data2Size = MAX_BUF_LEN;
    int ret = ParseData(Buf,BufSize,&cmd,&DataNum,Data1,&Data1Size,Data2,&Data2Size);
    if(ret == -1)
    {
        ErrorResponse(h,"Data Format Error!\n");
        return -1; 
    }
    if(cmd == OPEN_CMD && DataNum == 1)
    {
        debug("OPEN_CMD\n");
        tDatabase  db = NULL;
        pthread_mutex_lock(&dbmutex);
        db = DBCreate(Data1);
        pthread_mutex_unlock(&dbmutex);
        AttachCD(h,db);
        BufSize = MAX_BUF_LEN;
        FormatData(Buf,&BufSize,OPEN_RSP);
        SendData(h,Buf,BufSize);        
    }
    else if(cmd == CLOSE_CMD && DataNum == 0)
    {
        debug("CLOSE_CMD\n");
        tDatabase  db = NULL;
        GetMdb(h,db);
        pthread_mutex_lock(&dbmutex);
        DBDelete(db);
        pthread_mutex_unlock(&dbmutex);
        BufSize = MAX_BUF_LEN;
        FormatData(Buf,&BufSize,CLOSE_RSP);
        SendData(h,Buf,BufSize);
        DettachCD(h);
        ServiceStop(h);       
        return 0;
    }
    else if(cmd == GET_CMD && DataNum == 1)
    {
        debug("GET_CMD\n");
        if(Data1Size != sizeof(tKey))
        {
            fprintf(stderr,"Data Format Error,%s:%d\n",__FILE__,__LINE__);
            return -1;        
        }
        tDatabase  db = NULL;
        GetMdb(h,db);        
        tKey key = *(tKey*)Data1;
        tValue value;
        Data2Size = MAX_BUF_LEN;
        value.len = Data2Size;
        value.str = Data2;
        ret = DBGetKeyValue(db,key,&value);
        if(ret == FAILURE)
        {
            ErrorResponse(h,"The key NOT FOUND!\n");
            return -1; 
        }
        BufSize = MAX_BUF_LEN;
        FormatData2(Buf,&BufSize,GET_RSP,(char*)&key,sizeof(tKey),value.str,value.len);
        SendData(h,Buf,BufSize);         
    }
    else if(cmd == SET_CMD && DataNum == 2)
    {
        debug("SET_CMD\n");
        if(Data1Size != sizeof(tKey))
        {
            fprintf(stderr,"Data Format Error,%s:%d\n",__FILE__,__LINE__);
            return -1;        
        }
        tKey key = *(tKey*)Data1;
        tValue value;
        value.len = Data2Size;
        value.str = Data2;
        debug("SET_CMD:%d -> %s\n",*(tKey*)(Buf + 12),(char*)(Buf + 20));
        debug("SET_CMD:%d -> %s\n",key,value.str);
        tDatabase  db = NULL;
        GetMdb(h,db);        
        DBSetKeyValue(db,key,value);
        BufSize = MAX_BUF_LEN;
        FormatData(Buf,&BufSize,SET_RSP);
        SendData(h,Buf,BufSize);                 
    }
    else if(cmd == DELETE_CMD && DataNum == 1)
    {
        debug("DELETE_CMD\n");
        if(Data1Size != sizeof(tKey))
        {
            fprintf(stderr,"Data Format Error,%s:%d\n",__FILE__,__LINE__);
            return -1;         
        }
        tKey key = *(tKey*)Data1; 
        tDatabase  db = NULL;
        GetMdb(h,db);              
        ret = DBDelKeyValue(db,key);
        if(ret == FAILURE)
        {
            ErrorResponse(h,"The key NOT FOUND!\n");
            return -1; 
        }        
        BufSize = MAX_BUF_LEN;
        FormatData(Buf,&BufSize,DELETE_RSP);
        SendData(h,Buf,BufSize);         
    }
    else
    {
        ErrorResponse(h,"Unknow Request!\n");
        return -1;
    } 
    return 0;                     
}
