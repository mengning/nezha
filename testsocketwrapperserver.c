/********************************************************************/
/* Copyright (C) SSE-USTC, 2012                                     */
/*                                                                  */
/*  FILE NAME             :  server.c                               */
/*  PRINCIPAL AUTHOR      :  Mengning                               */
/*  SUBSYSTEM NAME        :  server                                 */
/*  MODULE NAME           :  server                                 */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  Any                                    */
/*  DATE OF FIRST RELEASE :  2012/11/22                             */
/*  DESCRIPTION           :  Reply Service Provider                 */
/********************************************************************/

/*
 * Revision log:
 *
 * Created by Mengning,2012/11/22
 *
 */
 
#include"socketwrapper.h"
#include<stdio.h>
#include<stdlib.h> 
#include<string.h>

#define PORT                5001
#define IP_ADDR             "127.0.0.1"
#define MAX_BUF_LEN         1024

int main()
{
    char szBuf[MAX_BUF_LEN] = "\0";
    int size = MAX_BUF_LEN;
    char szReplyMsg[MAX_BUF_LEN] = "hi\0";
    tServiceHandler h = -1;
    InitializeNetService(IP_ADDR,PORT);
    while(1)
    {
        h = ServiceStart();
        printf("return h = %d\n",h);
        size = MAX_BUF_LEN;
        int ret = RecvData(h,szBuf,&size);
        if(ret == 0)
            continue;
        printf("server recv:%d,%s,%d,%d\n",h,szBuf,ret,size); 
        SendData(h,szReplyMsg,strlen(szReplyMsg));
        printf("server send:%d,%s\n",h,szReplyMsg);  
        ServiceStop(h); 
    }
    ShutdownNetService();
    return 0;
}