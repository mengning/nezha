
/********************************************************************/
/* Copyright (C) SSE-USTC, 2012                                     */
/*                                                                  */
/*  FILE NAME             :  protocol.c                             */
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
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

#define debug               

typedef struct DataFormat
{
    int cmd;
    int num;
    int len;/* len of data-1 */
            /* data-1 */
            /* ... */    
}tDataFormat;

/*
 * FormatData - with 0-2 data
 * input	: cmd,data and data size
 * in/out	: pBuf - to store formated packet 
 * input	: BufSize - size of the memory pointed by pBuf
 * return	: if SUCCESS return 0
 *          : if FAILURE return (-1)
 */
int FormatData(char * pBuf,int *pBufSize,int cmd)
{
    if(*pBufSize < sizeof(tDataFormat) || pBuf == NULL)
    {
        fprintf(stderr,"FormatData Error,%s:%d\n", __FILE__,__LINE__);
        return -1;
    }
    tDataFormat *pData = (tDataFormat *)pBuf;
    pData->cmd = htonl(cmd);
    pData->num = htonl(0);
    pData->len = htonl(0);
    *pBufSize = sizeof(tDataFormat);
    return 0;
}
int FormatData1(char * pBuf,int *pBufSize,int cmd,char* pData1,int Data1Size)
{
    if((*pBufSize < sizeof(tDataFormat) + Data1Size)
        || pBuf == NULL
        || pData1 == NULL)
    {
        fprintf(stderr,"FormatData1 Error,%s:%d\n", __FILE__,__LINE__);
        return -1;
    }
    tDataFormat *pData = (tDataFormat *)pBuf;
    pData->cmd = htonl(cmd);
    pData->num = htonl(1);
    pData->len = htonl(Data1Size);
    memcpy(pBuf+sizeof(tDataFormat),pData1,Data1Size);
    *pBufSize = sizeof(tDataFormat) + Data1Size;
    return 0;   
}
int FormatData2(char * pBuf,int *pBufSize,int cmd,char* pData1,int Data1Size,char* pData2,int Data2Size)
{
    //printf("FormatData2:%s\n",pData1);
    //printf("FormatData2:%s\n",pData2);     
    if((*pBufSize < sizeof(tDataFormat) + Data1Size + sizeof(int) + Data2Size)
        || pBuf == NULL
        || pData1 == NULL
        || pData2 == NULL)
    {
        fprintf(stderr,"FormatData2 Error,%s:%d\n", __FILE__,__LINE__);
        return -1;
    }
    tDataFormat *pData = (tDataFormat *)pBuf;
    pData->cmd = htonl(cmd);
    pData->num = htonl(2);
    pData->len = htonl(Data1Size);
    memcpy(pBuf + sizeof(tDataFormat),pData1,Data1Size);
    int *pData2Len = (int *)(pBuf + sizeof(tDataFormat) + Data1Size);
    *pData2Len = htonl(Data2Size);
    debug("FormatData2:%d -> %s\n",Data2Size,pData2);
    memcpy(pBuf + sizeof(tDataFormat) + Data1Size + sizeof(int),pData2,Data2Size);
    *pBufSize = sizeof(tDataFormat) + Data1Size + sizeof(int) + Data2Size;     
    return 0;      
}
int FormatDataN(char *pBuf,int *pBufSize,int cmd,char ppData[MAX_DATA_NUM][MAX_DATA_LEN],int DataNum)
{
    int i = 0;
    int DataSize = 0;
    for(i=0;i<DataNum;i++)
    {
        DataSize += strlen(ppData[i]);
    }
    if((*pBufSize < sizeof(tDataFormat) + DataSize + sizeof(int)*DataNum)
        || pBuf == NULL
        || ppData == NULL)
    {
        fprintf(stderr,"FormatDataN Error,%s:%d\n", __FILE__,__LINE__);
        return -1;
    }
    tDataFormat *pData = (tDataFormat *)pBuf;
    pData->cmd = htonl(cmd);
    pData->num = htonl(DataNum);
    pData->len = htonl(0);
    char * p = pBuf + sizeof(tDataFormat) - sizeof(int); /* point to pData->len */
    for(i=0;i<DataNum;i++)
    {
        *(int*)p = strlen(ppData[i]);
        p += sizeof(int);
        debug("FormatDataN:%d,%s\n",strlen(ppData[i]),ppData[i]);
        memcpy(p,ppData[i],strlen(ppData[i]));
        p += strlen(ppData[i]);
    }     
    return 0;     
}
/*
 * ParseData - with 0-2 data
 * input	: pBuf - point to recved packet 
 * input	: BufSize - size of packet pointed by pBuf
 * output   : pcmd,pDataNum
 * output   : pData1,pData1Size,pData2,pData2Size,if no data,*pDataX == '\0',*pData1Size == 0
 * return	: if SUCCESS return 0
 *          : if FAILURE return (-1)
 */
int ParseCmd(char * pBuf,int BufSize,int *pcmd,int *pDataNum)
{
    if(BufSize < sizeof(tDataFormat)
        || pBuf == NULL || pcmd == NULL || pDataNum == NULL)
    {
        fprintf(stderr,"ParseCmd Error,%s:%d\n", __FILE__,__LINE__);
        return -1;
    }
    tDataFormat * pData = (tDataFormat *)pBuf;
    *pcmd = ntohl(pData->cmd);
    *pDataNum = ntohl(pData->num);
    return 0;  
} 
int ParseData(char * pBuf,int BufSize,int *pcmd,int *pDataNum,char* pData1,int *pData1Size,char* pData2,int *pData2Size)
{
    if(BufSize < sizeof(tDataFormat)
        || pBuf == NULL || pcmd == NULL || pDataNum == NULL
        || pData1 == NULL || pData1Size == NULL || *pData1Size <= 0
        || pData2 == NULL || pData2Size == NULL || *pData2Size <= 0)
    {
        fprintf(stderr,"ParseData Error,%s:%d\n", __FILE__,__LINE__);
        return -1;
    }
    tDataFormat * pData = (tDataFormat *)pBuf;
    *pcmd = ntohl(pData->cmd);
    *pDataNum = ntohl(pData->num);
    int len1 = 0;
    int len2 = 0;
    if(*pDataNum > 0)
    {
        len1 = ntohl(pData->len);
        debug("ParseData:%d    %d\n",pData->len,len1);
        if(BufSize < (sizeof(tDataFormat) + len1)
            || len1 > *pData1Size)
        {
            fprintf(stderr,"Unknown Format,%s:%d\n", __FILE__,__LINE__);
            return -1;            
        }
        *pData1Size = len1;
        memcpy(pData1,pBuf + sizeof(tDataFormat),*pData1Size);
    }
    if(*pDataNum > 1)
    {
        int *pData2Len = (int*)(pBuf + sizeof(tDataFormat) + *pData1Size);
        int len2 = ntohl(*pData2Len);
        debug("ParseData:%d    %d\n",*pData1Size,len1);
        debug("ParseData:%d    %d\n",*pData2Len,len2);
        if( BufSize < (sizeof(tDataFormat) + *pData1Size + sizeof(int) + len2)
            || len2 > *pData2Size)
        {
            fprintf(stderr,"Unknown Format,%s:%d\n", __FILE__,__LINE__);
            return -1;           
        }
        *pData2Size = len2;
        debug("ParseData:%s\n",pBuf + sizeof(tDataFormat) + *pData1Size + sizeof(int));
        memcpy(pData2,pBuf + sizeof(tDataFormat) + *pData1Size + sizeof(int),*pData2Size);
        debug("ParseData:%s    %d\n",pData2,len2);
    }
    if(*pDataNum > 2)
    {
        fprintf(stderr,"Unknown Format,%s:%d\n", __FILE__,__LINE__);
        return -1;
    }
    return 0;  
}
        
int ParseDataN(char *pBuf,int BufSize,int *pcmd,int *pDataNum,char ppData[MAX_DATA_NUM][MAX_DATA_LEN])
{
    if(BufSize < sizeof(tDataFormat)
        || pBuf == NULL || pcmd == NULL || pDataNum == NULL
        || ppData == NULL)
    {
        fprintf(stderr,"ParseDataN Error,%s:%d\n", __FILE__,__LINE__);
        return -1;
    }
    tDataFormat * pData = (tDataFormat *)pBuf;
    *pcmd = ntohl(pData->cmd);
    *pDataNum = ntohl(pData->num);
    debug("ParseDataN CMD:%d,DataNum:%d\n",*pcmd,*pDataNum);
    if(*pDataNum > MAX_DATA_NUM)
    {
        fprintf(stderr,"ParseDataN Error:*pDataNum > MAX_DATA_NUM,%s:%d\n", __FILE__,__LINE__);
        return -1;        
    }
    int DataSize = 0;
    char * p = pBuf + sizeof(tDataFormat) - sizeof(int);
    int i = 0;
    for(i=0;i<(*pDataNum);i++)
    {
        DataSize = *(int*)p;
        debug("ParseDataN:%d,%s\n",DataSize,p + sizeof(int));
        if(DataSize > MAX_DATA_LEN)
        {
            fprintf(stderr,"ParseDataN Error:DataSize > MAX_DATA_LEN,%s:%d\n", __FILE__,__LINE__);
            return -1;        
        }        
        p += sizeof(int);
        memcpy(ppData[i],p,DataSize);
        ppData[i][DataSize] = '\0';
        debug("ParseDataN:%d,%s\n",DataSize,ppData[i]);
        p += DataSize;
    }
    return 0;     
}


