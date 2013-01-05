
/********************************************************************/
/* Copyright (C) SSE-USTC, 2012-2013                                */
/*                                                                  */
/*  FILE NAME             :  nodes.c                                */
/*  PRINCIPAL AUTHOR      :  Mengning                               */
/*  SUBSYSTEM NAME        :  cloud                                  */
/*  MODULE NAME           :  nodes                                  */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  Linux                                  */
/*  DATE OF FIRST RELEASE :  2013/1/4                               */
/*  DESCRIPTION           :  server nodes admin module              */
/********************************************************************/

/*
 * Revision log:
 *
 * Created by Mengning,2013/1/4
 *
 */
#include "nodes.h"
#include "socketwrapper.h"
#include "protocol.h"
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

#define MAX_BUF_LEN     1024

#define debug               

/* Init cloud nodes info */
tLinkTable *  InitCloudNodes(char * addr,int port)
{

    tLinkTable * nodes = CreateLinkTable();
    tCloudNode* pNode = (tCloudNode*)malloc(sizeof(tCloudNode));
    if(addr != NULL)
    {
        memcpy(pNode->addr,addr,strlen(addr));
        pNode->addr[strlen(addr)+1] = '\0';
        pNode->port = port;
        pNode->fd = -1;
        AddLinkTableNode(nodes,(tLinkTableNode *)pNode);
    }
    return nodes;
}

/* Register And Load cloud nodes info */
tLinkTable *  RegisterAndLoadCloudNodes(char * addr,int port)
{
    int i;
    int DataNum = -1;
    char Buf[MAX_BUF_LEN] = "\0";
    int BufSize = MAX_BUF_LEN;
    char szMsg[MAX_BUF_LEN] = "\0";
    char ppData[MAX_DATA_NUM][MAX_DATA_LEN] = {0};
    tServiceHandler h = -1;
    h = OpenRemoteService(IP_ADDR,PORT);
    if(addr == NULL)
    {
        DataNum = 0;
    }
    else
    {
        DataNum = 1;
        sprintf(ppData[0],"%s %d\0",addr,port);
        debug("pasrer sprintf :%s\n",ppData[0]);     
    }
    FormatDataN(Buf,&BufSize,CTRL_REG_CMD,ppData,DataNum);
    SendData(h,Buf,BufSize);
    RecvData(h,Buf,&BufSize);
    int cmd = -1;
    ParseDataN(Buf,BufSize,&cmd,&DataNum,ppData);
    if(cmd != CTRL_REG_RSP || DataNum > MAX_DATA_NUM || DataNum < 2)
    {
        fprintf(stderr,"AddCloudNodes Error,%s:%d\n",__FILE__,__LINE__);
        return NULL;         
    }
    tLinkTable * nodes = CreateLinkTable();
    AddCloudNodes(nodes,ppData,DataNum);    
    CloseRemoteService(h);
    return nodes;    
}
/* add recved buf(nodes info) to linktable */
int AddCloudNodes(tLinkTable * nodes,char ppData[MAX_DATA_NUM][MAX_DATA_LEN],int DataNum)
{
    if(nodes == NULL || ppData == NULL)
    {
        fprintf(stderr,"AddCloudNodes Error,%s:%d\n",__FILE__,__LINE__);
        return -1;         
    }
    if(DataNum == 0)
    {
        return 0;
    }
    int i;
    for(i=0;i<DataNum;i++)
    {
        tCloudNode* pNode = (tCloudNode*)malloc(sizeof(tCloudNode));
        char addr[MAX_DATA_LEN];
        int port;
        sscanf(ppData[i],"%s%d",addr,&port);
        debug("pasrer %s:%d\n",addr,port);
        memcpy(pNode->addr,addr,strlen(ppData[i]));
        pNode->addr[strlen(ppData[i])+1] = '\0';
        pNode->port = port;
        pNode->fd = -1;
        AddLinkTableNode(nodes,(tLinkTableNode *)pNode);
    }
    return 0;
}
/* Linktable(Nodes info) to ppData array */
int CloudNodesInfo(tLinkTable * nodes,char ppData[MAX_DATA_NUM][MAX_DATA_LEN],int *NodeNum)
{
    if(*NodeNum < nodes->SumOfNode)
    {
        fprintf(stderr,"CloudNodesInfo Error,%s:%d\n",__FILE__,__LINE__);
        return -1;
    }
    *NodeNum = nodes->SumOfNode;
    int i = 0;
    tCloudNode * pNode = (tCloudNode*)GetLinkTableHead(nodes);
    while(pNode != NULL)
    {
        sprintf(ppData[i],"%s %d\0",pNode->addr,pNode->port);
        debug("pasrer sprintf :%s\n",ppData[i]);
        pNode = (tCloudNode*)GetNextLinkTableNode(nodes,(tLinkTableNode *)pNode);
        i++;
    }
    return 0;  
}

