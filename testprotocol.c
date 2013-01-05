
/********************************************************************/
/* Copyright (C) SSE-USTC, 2012                                     */
/*                                                                  */
/*  FILE NAME             :  testprotocol.c                         */
/*  PRINCIPAL AUTHOR      :  Mengning                               */
/*  SUBSYSTEM NAME        :  network                                */
/*  MODULE NAME           :  protocol                               */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  Linux                                  */
/*  DATE OF FIRST RELEASE :  2012/12/18                             */
/*  DESCRIPTION           :  Protocol between Nezha Sever and Client*/
/********************************************************************/

/*
 * Revision log:
 *
 * Created by Mengning,2012/12/18
 *
 */
#include "protocol.h"
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

#define MAX_BUF_LEN         1024

int main()
{
    char Buf[MAX_BUF_LEN] = "\0";
    int BufSize = MAX_BUF_LEN;
    char *str = "hello world!";
    BufSize = MAX_BUF_LEN;
    FormatData(Buf,&BufSize,CLOSE_CMD);
    int cmd = -1;
    int DataNum = -1;
    char Data1[MAX_BUF_LEN] = "\0";
    int Data1Size = MAX_BUF_LEN;
    char Data2[MAX_BUF_LEN] = "\0";
    int Data2Size = MAX_BUF_LEN;
    ParseData(Buf,MAX_BUF_LEN,&cmd,&DataNum,Data1,&Data1Size,Data2,&Data2Size);
    if(cmd == CLOSE_CMD && DataNum == 0)
    {
        printf("Test Zero Data Result: Pass\n");
    }
    BufSize = MAX_BUF_LEN;
    FormatData1(Buf,&BufSize,OPEN_CMD,str,strlen(str));
    ParseData(Buf,MAX_BUF_LEN,&cmd,&DataNum,Data1,&Data1Size,Data2,&Data2Size);
    if(cmd == OPEN_CMD && DataNum == 1
        && Data1Size == strlen(str)
        && strcmp(Data1,str) == 0)
    {
        printf("%s\n",Data1);
        printf("Test One Data Result: Pass\n");
    }
    BufSize = MAX_BUF_LEN;
    FormatData2(Buf,&BufSize,SET_CMD,str,strlen(str),str,strlen(str));
    ParseData(Buf,MAX_BUF_LEN,&cmd,&DataNum,Data1,&Data1Size,Data2,&Data2Size);
    if(cmd == SET_CMD && DataNum == 2
        && Data1Size == strlen(str) && Data2Size == strlen(str)
        && strcmp(Data1,str) == 0 && strcmp(Data2,str) == 0)
    {
        printf("%s\n",Data1);
        printf("%s\n",Data2);
        printf("Test Two Data Result: Pass\n");
    }
    char ppData[10][MAX_DATA_LEN] = 
    {
        {"127.0.0.1:5001"},
        {"127.0.0.2:5002"},
        {"127.0.0.3:5003"},
        {"127.0.0.4:5004"},
        {"127.0.0.5:5005"},
        {"127.0.0.6:5006"},
        {"127.0.0.7:5007"},
        {"127.0.0.8:5008"},
        {"127.0.0.9:5009"},
        {"127.0.0.10:5010"}
    };
    char ppData1[10][MAX_DATA_LEN] = {0};
    BufSize = MAX_BUF_LEN;
    FormatDataN(Buf,&BufSize,CTRL_REG_RSP,ppData,10);
    DataNum = 0;
    ParseDataN(Buf,MAX_BUF_LEN,&cmd,&DataNum,ppData1);
    printf("CMD:%d,DataNum:%d\n",cmd,DataNum);
    int i = 0;
    for(i=0;i<10;i++)
    {
        printf("%s\n",ppData1[i]);
    }

    return 0;   
}



