
/********************************************************************/
/* Copyright (C) MC2Lab-USTC, 2012                  */
/*                                  */
/*  FILE NAME         :  dbserver.c                 */
/*  PRINCIPAL AUTHOR      :  Mengning                   */
/*  SUBSYSTEM NAME    :  network                */
/*  MODULE NAME       :  dbserver                   */
/*  LANGUAGE          :  C                      */
/*  TARGET ENVIRONMENT    :  Linux                  */
/*  DATE OF FIRST RELEASE :  2012/12/14                 */
/*  DESCRIPTION       :  Impement of Socket Server Engine       */
/*               to handle clients requests.        */
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>

#define PORT        5001
#define IP_ADDR         "127.0.0.1"
#define MAX_BUF_LEN     1024

#define THREAD_NUM      0

#define debug                    

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

int HandleRequests(tServiceHandler h);
int HandleOneRequest(tServiceHandler h);

int main()
{
    pthread_t thread_id;
    InitCDManager();
    /* Server Engine for Client's Connections */
    tServiceHandler request = -1;
    InitializeNetService(IP_ADDR,PORT);
    while(1)
    {
    /* return the client fd who have real data request */
    request = ServiceStart();
#if 0    
    if(pthread_create(&thread_id,NULL, (void*)HandleRequests,(void*)request) != 0)
    {
        fprintf(stderr,"pthread_create Error,%s:%d\n",__FILE__,__LINE__);
        ServiceStop(h);        
    }
#endif
    HandleOneRequest(request);   
    }
    ShutdownNetService();
    ShutCDManager();
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
int HandleRequests(tServiceHandler h)
{
    /* Handle Requests */
    while(1)
    {
        HandleOneRequest(h);
    }
}
int HandleOneRequest(tServiceHandler h)
{
    char Buf[MAX_BUF_LEN] = "\0";
    int BufSize = MAX_BUF_LEN;
    int cmd = -1;
    int DataNum = -1;
    char Data1[MAX_BUF_LEN] = "\0";
    int Data1Size = MAX_BUF_LEN;
    char Data2[MAX_BUF_LEN] = "\0";
    int Data2Size = MAX_BUF_LEN;  
      
    /* Handle One Request */

    BufSize = MAX_BUF_LEN;
    if(RecvData(h,Buf,&BufSize) == 0)
    {
        fprintf(stderr,"Connection Error,%s:%d\n",__FILE__,__LINE__);
        ServiceStop(h);
        return -1;        
    }
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
        db = DBCreate(Data1);
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
        DBDelete(db);
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
        printf("Unknow Request!\n");
    }                      
}
