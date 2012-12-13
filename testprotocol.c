
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
    char *str = "hello world!";
    FormatData(Buf,MAX_BUF_LEN,CLOSE_CMD);
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
    FormatData1(Buf,MAX_BUF_LEN,OPEN_CMD,str,strlen(str));
    ParseData(Buf,MAX_BUF_LEN,&cmd,&DataNum,Data1,&Data1Size,Data2,&Data2Size);
    if(cmd == OPEN_CMD && DataNum == 1
        && Data1Size == strlen(str)
        && strcmp(Data1,str) == 0)
    {
        printf("%s\n",Data1);
        printf("Test One Data Result: Pass\n");
    }
    FormatData2(Buf,MAX_BUF_LEN,SET_CMD,str,strlen(str),str,strlen(str));
    ParseData(Buf,MAX_BUF_LEN,&cmd,&DataNum,Data1,&Data1Size,Data2,&Data2Size);
    if(cmd == SET_CMD && DataNum == 2
        && Data1Size == strlen(str) && Data2Size == strlen(str)
        && strcmp(Data1,str) == 0 && strcmp(Data2,str) == 0)
    {
        printf("%s\n",Data1);
        printf("%s\n",Data2);
        printf("Test Two Data Result: Pass\n");
    }    
    return 0;   
}



