
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
/*  DESCRIPTION           :  Impement of Client Side Interface      */
/*                           to access database                     */
/********************************************************************/

/*
 * Revision log:
 *
 * Created by Mengning ,2012/5/30
 * Modify for Nezha project,by Mengning,2012/11/27
 * Modify for Client Side of Nezha project,by Mengning,2012/12/18
 * change api names for inserting cloud module,
 * so that provide the same api for higher layer,by Mengning,2012/12/28
 */

#include "remotedbapi.h"
#include "socketwrapper.h"
#include "protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


#define MAX_BUF_LEN         1024
#define debug               


/*
 * Create an Database
 */
int  RemoteDBCreate(const char * filename,char * addr,int port)
{
    if(filename == NULL)
    {
        fprintf(stderr,"filename Error,%s:%d\n", __FILE__,__LINE__);
        filename = "nezha.hdb";
    }        
    /* connect server */
    tServiceHandler dbhandler = OpenRemoteService(addr,port);
    if(dbhandler == -1)
    {
        return -1;   
    }
    /* open database file */
    char Buf[MAX_BUF_LEN] = "\0";
    int BufSize = MAX_BUF_LEN;
    FormatData1(Buf,&BufSize,OPEN_CMD,(char*)filename,strlen(filename));
    SendData(dbhandler,Buf,BufSize);
    BufSize = MAX_BUF_LEN;
    RecvData(dbhandler,Buf,&BufSize);
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
        return -1; 
    }        
    return dbhandler;
}

/*
 * Delete the Database
 */
int RemoteDBDelete(int db)
{
    /* close database file */
    char Buf[MAX_BUF_LEN] = "\0";
    int BufSize = MAX_BUF_LEN;
    FormatData(Buf,&BufSize,CLOSE_CMD);
    SendData(db,Buf,BufSize);
    BufSize = MAX_BUF_LEN;
    if(RecvData(db,Buf,&BufSize) == 0)
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
    if(CloseRemoteService(db) == -1)
    {
        return -1; 
    }
    return 0;
}


/*
 * Set key/value
 */
int RemoteDBSetKeyValue(int db,tKey key,tValue value)
{
    char Buf[MAX_BUF_LEN] = "\0";
    int BufSize = MAX_BUF_LEN;
    int ret = FormatData2(Buf,&BufSize,SET_CMD,(char*)key.str,key.len,value.str,value.len);
    if(ret == -1)
    {
        return -1;
    }
    SendData(db,Buf,BufSize);
    BufSize = MAX_BUF_LEN;
    if(RecvData(db,Buf,&BufSize) == 0)
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
    ParseData(Buf,BufSize,&cmd,&DataNum,Data1,&Data1Size,Data2,&Data2Size);
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
int RemoteDBGetKeyValue(int db,tKey key,tValue *pvalue)
{
    if(db == -1 || pvalue == NULL)
    {
        return -1;
    }
    char Buf[MAX_BUF_LEN] = "\0";
    int BufSize = MAX_BUF_LEN;
    int ret = FormatData1(Buf,&BufSize,GET_CMD,(char*)key.str,key.len);
    if(ret == -1)
    {
        return -1;
    }
    SendData(db,Buf,BufSize);
    BufSize = MAX_BUF_LEN;
    if(RecvData(db,Buf,&BufSize) == 0)
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
    ParseData(Buf,BufSize,&cmd,&DataNum,Data1,&Data1Size,Data2,&Data2Size);
    if(cmd == ERROR_RSP && DataNum == 1)
    {
        return -1;
    }    
    if(cmd != GET_RSP || DataNum != 2 )
    {
        fprintf(stderr,"Remote DBGetKeyValue Error,%s:%d\n", __FILE__,__LINE__);
        return -1;
    }
    pvalue->len = Data2Size;
    memcpy(pvalue->str,Data2,Data2Size);
    return 0;
}

/*
 * delete key/value
 */
int RemoteDBDelKeyValue(int db,tKey key)
{
    if(db == -1)
    {
        return -1;
    }
    char Buf[MAX_BUF_LEN] = "\0";
    int BufSize = MAX_BUF_LEN;
    int ret = FormatData1(Buf,&BufSize,DELETE_CMD,(char*)key.str,key.len);
    if(ret == -1)
    {
        return -1;
    }
    SendData(db,Buf,BufSize);
    BufSize = MAX_BUF_LEN;
    if(RecvData(db,Buf,&BufSize) == 0)
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
    ParseData(Buf,BufSize,&cmd,&DataNum,Data1,&Data1Size,Data2,&Data2Size);
    if(cmd == ERROR_RSP && DataNum == 1)
    {
        fprintf(stderr,"%s,%s:%d\n",Data1, __FILE__,__LINE__);
        return -1;
    }
    return 0;
}
