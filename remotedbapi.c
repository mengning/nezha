
/********************************************************************/
/* Copyright (C) MC2Lab-USTC, 2012                                  */
/*                                                                  */
/*  FILE NAME             :  remotedbapi.c                          */
/*  PRINCIPAL AUTHOR      :  Mengning                               */
/*  SUBSYSTEM NAME        :  network                                */
/*  MODULE NAME           :  remotedbapi                            */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  Linux                                  */
/*  DATE OF FIRST RELEASE :  2012/12/18                             */
/*  DESCRIPTION           :  Impement of Cliet Side Interface       */
/*                           to access database                     */
/********************************************************************/

/*
 * Revision log:
 *
 * Created by Mengning ,2012/5/30
 * Modify for Nezha project,by Mengning,2012/11/27
 * Modify for Cliet Side of Nezha project,by Mengning,2012/12/18
 *
 */

#include "dbapi.h"
#include "socketwrapper.h"
#include "protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define PORT                5001
#define IP_ADDR             "127.0.0.1"
#define MAX_BUF_LEN         1024

tServiceHandler h = -1;

/*
 * Create an Database
 */
tDatabase  DBCreate(const char * filename)
{
    if(filename == NULL)
    {
        fprintf(stderr,"filename Error,%s:%d\n", __FILE__,__LINE__);
        filename = "nezha.hdb";
    }        
    /* connect server */
    h = OpenRemoteService(IP_ADDR,PORT);
    if(h == -1)
    {
        exit(-1);   
    }
    /* open database file */
    char Buf[MAX_BUF_LEN] = "\0";
    int BufSize = MAX_BUF_LEN;
    FormatData1(Buf,&BufSize,OPEN_CMD,(char*)filename,strlen(filename));
    SendData(h,Buf,BufSize);
    BufSize = MAX_BUF_LEN;
    RecvData(h,Buf,&BufSize);
    int cmd = -1;
    int DataNum = -1;
    char Data1[MAX_BUF_LEN] = "\0";
    int Data1Size = MAX_BUF_LEN;
    char Data2[MAX_BUF_LEN] = "\0";
    int Data2Size = MAX_BUF_LEN; 
    ParseData(Buf,MAX_BUF_LEN,&cmd,&DataNum,Data1,&Data1Size,Data2,&Data2Size);
    if(cmd != OPEN_RSP || DataNum != 0)
    {
        fprintf(stderr,"Remote DBCreate Error,%s:%d\n", __FILE__,__LINE__);
    }        
    return (tDatabase)&h;
}

/*
 * Delete the Database
 */
int DBDelete(tDatabase db)
{
    /* close database file */
    char Buf[MAX_BUF_LEN] = "\0";
    int BufSize = MAX_BUF_LEN;
    FormatData(Buf,&BufSize,CLOSE_CMD);
    SendData(*(tServiceHandler*)db,Buf,BufSize);
    BufSize = MAX_BUF_LEN;
    if(RecvData(*(tServiceHandler*)db,Buf,&BufSize) == 0)
    {
        fprintf(stderr,"Connection Error,%s:%d\n",__FILE__,__LINE__);
        return -1;            
    }    
    int cmd = -1;
    int DataNum = -1;
    char Data1[MAX_BUF_LEN] = "\0";
    int Data1Size = MAX_BUF_LEN;
    char Data2[MAX_BUF_LEN] = "\0";
    int Data2Size = MAX_BUF_LEN; 
    ParseData(Buf,MAX_BUF_LEN,&cmd,&DataNum,Data1,&Data1Size,Data2,&Data2Size);
    if(cmd != CLOSE_RSP || DataNum != 0)
    {
        fprintf(stderr,"Remote DBDelete Error,%s:%d\n", __FILE__,__LINE__);
        return -1;
    }    
    /* close connection */
    if(CloseRemoteService(*(tServiceHandler*)db) == -1)
    {
        return -1; 
    }
    return 0;
}


/*
 * Set key/value
 */
int DBSetKeyValue(tDatabase db,tKey key,tValue value)
{
    char Buf[MAX_BUF_LEN] = "\0";
    int BufSize = MAX_BUF_LEN;
    int ret = FormatData2(Buf,&BufSize,SET_CMD,(char*)&key,sizeof(tKey),value.str,value.len);
    if(ret == -1)
    {
        return -1;
    }
    SendData(*(tServiceHandler*)db,Buf,BufSize);
    BufSize = MAX_BUF_LEN;
    if(RecvData(*(tServiceHandler*)db,Buf,&BufSize) == 0)
    {
        fprintf(stderr,"Connection Error,%s:%d\n",__FILE__,__LINE__);
        return -1;            
    }    
    int cmd = -1;
    int DataNum = -1;
    char Data1[MAX_BUF_LEN] = "\0";
    int Data1Size = MAX_BUF_LEN;
    char Data2[MAX_BUF_LEN] = "\0";
    int Data2Size = MAX_BUF_LEN; 
    ParseData(Buf,MAX_BUF_LEN,&cmd,&DataNum,Data1,&Data1Size,Data2,&Data2Size);
    if(cmd != SET_RSP || DataNum != 0)
    {
        fprintf(stderr,"Remote DBSetKeyValue Error,%s:%d\n", __FILE__,__LINE__);
        return -1;
    }
    return 0;
}

/*
 * get key/value
 */
int DBGetKeyValue(tDatabase db,tKey key,tValue *pvalue)
{
    
    return 0;
}

/*
 * delete key/value
 */
int DBDelKeyValue(tDatabase db,tKey key)
{

    return 0;
}
