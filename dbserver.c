
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

#define PORT                5001
#define IP_ADDR             "127.0.0.1"
#define MAX_BUF_LEN         1024

tDatabase  db = NULL;

int HandleRequests(tServiceHandler h);

int main()
{
    /* Server Engine for Client's Connections */
    tServiceHandler h = -1;
    InitializeNetService(IP_ADDR,PORT);
    while(1)
    {
        h = ServiceStart();
        HandleRequests(h);  
        ServiceStop(h); 
    }
    ShutdownNetService();
    return 0;
}

int HandleRequests(tServiceHandler h)
{
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
            BufSize = MAX_BUF_LEN;
            FormatData(Buf,&BufSize,ERROR_RSP);
            SendData(h,Buf,BufSize);            
            continue;
        }
        if(cmd == OPEN_CMD && DataNum == 1)
        {
            printf("OPEN_CMD\n");
            db = DBCreate(Data1);
            BufSize = MAX_BUF_LEN;
            FormatData(Buf,&BufSize,OPEN_RSP);
            SendData(h,Buf,BufSize);            
        }
        else if(cmd == CLOSE_CMD && DataNum == 0)
        {
            printf("CLOSE_CMD\n");
            DBDelete(db);
            BufSize = MAX_BUF_LEN;
            FormatData(Buf,&BufSize,CLOSE_RSP);
            SendData(h,Buf,BufSize);            
            return 0;
        }
        else if(cmd == GET_CMD && DataNum == 1)
        {
            printf("GET_CMD\n");
            
        }
        else if(cmd == SET_CMD && DataNum == 2)
        {
            printf("SET_CMD\n");
            if(Data1Size != sizeof(tKey))
            {
                fprintf(stderr,"Data Format Error,%s:%d\n",__FILE__,__LINE__);
                continue;            
            }
            tKey key = *(tKey*)Data1;
            tValue value;
            value.len = Data2Size;
            value.str = Data2;
            DBSetKeyValue(db,key,value);
            BufSize = MAX_BUF_LEN;
            FormatData(Buf,&BufSize,SET_RSP);
            SendData(h,Buf,BufSize);                             
        }
        else if(cmd == DELETE_CMD && DataNum == 1)
        {
            printf("DELETE_CMD\n");
        }
        else
        {
            printf("Unknow Request!\n");
        }                                   
    }    
}
