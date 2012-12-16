
/********************************************************************/
/* Copyright (C) MC2Lab-USTC, 2012                                  */
/*                                                                  */
/*  FILE NAME             :  dbserver.c                             */
/*  PRINCIPAL AUTHOR      :  Mengning                               */
/*  SUBSYSTEM NAME        :  network                                */
/*  MODULE NAME           :  dbserver                               */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  Linux                                  */
/*  DATE OF FIRST RELEASE :  2012/12/14                             */
/*  DESCRIPTION           :  Impement of Socket Server Engine       */
/*                           to handle clients requests.            */
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

#define PORT                5001
#define IP_ADDR             "127.0.0.1"
#define MAX_BUF_LEN         1024

#define debug               



int HandleRequests(tServiceHandler h);

int main()
{
    pthread_t thread_id;
    /* Server Engine for Client's Connections */
    tServiceHandler h = -1;
    InitializeNetService(IP_ADDR,PORT);
    while(1)
    {
        h = ServiceStart();
        if(pthread_create(&thread_id,NULL, (void*)HandleRequests,(void*)h) != 0)
        {
            fprintf(stderr,"pthread_create Error,%s:%d\n",__FILE__,__LINE__);
            ServiceStop(h);            
        }
        //HandleRequests(h);  
        //ServiceStop(h); 
    }
    ShutdownNetService();
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
    tDatabase  db = NULL;
    char Buf[MAX_BUF_LEN] = "\0";
    int BufSize = MAX_BUF_LEN;
    int cmd = -1;
    int DataNum = -1;
    char Data1[MAX_BUF_LEN] = "\0";
    int Data1Size = MAX_BUF_LEN;
    char Data2[MAX_BUF_LEN] = "\0";
    int Data2Size = MAX_BUF_LEN;    
    /* Handle Requests of One Client */
    while(1)
    {
        BufSize = MAX_BUF_LEN;
        if(RecvData(h,Buf,&BufSize) == 0)
        {
            fprintf(stderr,"Connection Error,%s:%d\n",__FILE__,__LINE__);
            ServiceStop(h);
            return -1;            
        }
        if(BufSize == 0)
        {
            continue;
        }
        Data1Size = MAX_BUF_LEN;
        Data2Size = MAX_BUF_LEN;
        int ret = ParseData(Buf,BufSize,&cmd,&DataNum,Data1,&Data1Size,Data2,&Data2Size);
        if(ret == -1)
        {
            ErrorResponse(h,"Data Format Error!\n");
            continue;
        }
        if(cmd == OPEN_CMD && DataNum == 1)
        {
            debug("OPEN_CMD\n");
            db = DBCreate(Data1);
            BufSize = MAX_BUF_LEN;
            FormatData(Buf,&BufSize,OPEN_RSP);
            SendData(h,Buf,BufSize);            
        }
        else if(cmd == CLOSE_CMD && DataNum == 0)
        {
            debug("CLOSE_CMD\n");
            DBDelete(db);
            BufSize = MAX_BUF_LEN;
            FormatData(Buf,&BufSize,CLOSE_RSP);
            SendData(h,Buf,BufSize); 
            ServiceStop(h);           
            return 0;
        }
        else if(cmd == GET_CMD && DataNum == 1)
        {
            debug("GET_CMD\n");
            if(Data1Size != sizeof(tKey))
            {
                fprintf(stderr,"Data Format Error,%s:%d\n",__FILE__,__LINE__);
                continue;            
            }
            tKey key = *(tKey*)Data1;
            tValue value;
            Data2Size = MAX_BUF_LEN;
            value.len = Data2Size;
            value.str = Data2;
            ret = DBGetKeyValue(db,key,&value);
            if(ret == FAILURE)
            {
                ErrorResponse(h,"The key NOT FOUND!\n");
                continue;
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
                continue;            
            }
            tKey key = *(tKey*)Data1;
            tValue value;
            value.len = Data2Size;
            value.str = Data2;
            debug("SET_CMD:%d -> %s\n",*(tKey*)(Buf + 12),(char*)(Buf + 20));
            debug("SET_CMD:%d -> %s\n",key,value.str);
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
                continue;            
            }
            tKey key = *(tKey*)Data1;           
            ret = DBDelKeyValue(db,key);
            if(ret == FAILURE)
            {
                ErrorResponse(h,"The key NOT FOUND!\n");
                continue;
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
}
