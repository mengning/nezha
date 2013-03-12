
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
 * Consistent-Hash support ,by Mengning,2013/1/9
 *
 */
#include "nodes.h"
#include "socketwrapper.h"
#include "protocol.h"
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include "common.h"

#define MAX_BUF_LEN         1024

/* tell other nodes who am i */
int BroadcastMyself(int fd,char * addr,int port);
             
/* Init cloud nodes info */
tCluster *  InitCluster()
{
    tCluster * cluster = (tCluster *)malloc(sizeof(tCluster));
    cluster->SumOfNodes = 0;
    int i; 
    for(i=0;i<MAX_NODE_NUM;i++)
    {
        cluster->nodes[i] = NULL; 
    }    
    return cluster;    
}
/* Destroy Cluster */
int DestroyCluster(tCluster * cluster)
{
    int i = 0;
    tNode* pNode = cluster->nodes[i];
    while(1)
    {
        i = pNode->hash;
        free(pNode);
        pNode = cluster->nodes[i];
        if(pNode->hash >= MAX_NODE_NUM)
        {
            break;
        }
    }
    free(cluster);
    return 0;
}
int GetHashValue(tCluster * cluster,int SumOfNodes,int * start)
{
    int i = 0;
    int max=0,prev=0,next=0;    
    while(i < MAX_NODE_NUM)
    {
        tNode* pNode = cluster->nodes[i];
        if(max < pNode->hash - i)
        {
            max = pNode->hash - i;
            prev = i;
            next = pNode->hash;
        }
        i = pNode->hash;
    }
    *start = prev;
    return (next - prev)%2 == 0 ? (next - prev)/2 + prev : (next - prev - 1)/2 + prev;
}
/* Get cloud nodes info */
tNode *  GetNode(tCluster * cluster,int hash)
{
    return cluster->nodes[hash];
}
/* Add cloud nodes info */
int  AddNode(tCluster * cluster,char * addr,int port)
{
    debug("AddNode %s:%d\n",addr,port);
    int i = 0;    
    if(cluster->SumOfNodes >= MAX_NODE_NUM)
    {
        return -1;
    }
    tNode* pNode = (tNode*)malloc(sizeof(tNode));
    memcpy(pNode->addr,addr,strlen(addr));
    pNode->addr[strlen(addr)] = '\0';
    pNode->port = port;
    pNode->fd = -1;
    if(cluster->SumOfNodes == 0)
    {
        pNode->hash = MAX_NODE_NUM;
        debug("AddNode start:%d     hash:%d\n",i,pNode->hash);
        for(i=0;i<=MAX_NODE_NUM;i++)
        {
            cluster->nodes[i] = pNode; 
        }        
    }
    else
    {
        int start;
        pNode->hash = GetHashValue(cluster,cluster->SumOfNodes,&start);
        debug("AddNode start:%d     hash:%d\n",start,pNode->hash);
        for(i=start;i<pNode->hash;i++)
        {
            cluster->nodes[i] = pNode; 
        } 
    }
    cluster->SumOfNodes ++;
    return 0; 
}
/* Register cloud nodes info */
int  RemoveNode(tCluster * cluster,int hash,char * addr,int port,int fd)
{
    /* add here in future */
    return -1;
}

/* Register And Load cloud nodes info */
tCluster *  RegisterAndLoadClusterNodes(char * addr,int port)
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
        debug("RegisterAndLoadClusterNodes :%s\n",ppData[0]);     
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
    tCluster * cluster = InitCluster();
    AddClusterNodes(cluster,ppData,DataNum);    
    CloseRemoteService(h);
    debug("RegisterAndLoadClusterNodes END!\n");
    return cluster;    
}
/* add recved buf(nodes info) to cluster */
int AddClusterNodes(tCluster * cluster,char ppData[MAX_DATA_NUM][MAX_DATA_LEN],int DataNum)
{
    if(cluster == NULL || ppData == NULL)
    {
        fprintf(stderr,"AddClusterNodes Error,%s:%d\n",__FILE__,__LINE__);
        return -1;         
    }
    if(DataNum == 0)
    {
        return 0;
    }
    int i;
    for(i=0;i<DataNum;i++)
    {
        char addr[MAX_DATA_LEN];
        int port;
        sscanf(ppData[DataNum-1-i],"%s%d",addr,&port);
        debug("pasrer %s:%d\n",addr,port);
        AddNode(cluster,addr,port);
    }
    return 0;
}
/* Cluseter(Nodes info) to ppData array */
int ClusterNodesInfo(tCluster * cluster,char ppData[MAX_DATA_NUM][MAX_DATA_LEN],int *NodeNum)
{
    if(*NodeNum < cluster->SumOfNodes)
    {
        fprintf(stderr,"CloudNodesInfo Error,%s:%d\n",__FILE__,__LINE__);
        return -1;
    }
    *NodeNum = cluster->SumOfNodes;
    int i = 0; 
    int hash = 0;   
    while(hash < MAX_NODE_NUM)
    {
        tNode* pNode = cluster->nodes[hash];
        sprintf(ppData[i],"%s %d\0",pNode->addr,pNode->port);
        debug("ClusterNodesInfo :%s\n",ppData[i]);
        hash = pNode->hash;
        i++;
    }
    return 0;  
}

int ConnectDataNode(tCluster* cluster,char * addr,int port,char * filename)
{
    int i = 0;    
    while(i < MAX_NODE_NUM)
    {
        tNode * pNode = (tNode*)GetNode(cluster,i);        
        if(strcmp(addr,pNode->addr) == 0 && pNode->port == port)
        {
            pNode->fd = 0;/* don't need socket */
        }
        else
        {
            debug("RemoteDBCreate %s:%d\n",pNode->addr,pNode->port);
            pNode->fd = RemoteDBCreate(filename,pNode->addr,pNode->port);
            if(pNode->fd == -1)
            {
                return -1;
            }
            if(strcmp(IP_ADDR,pNode->addr) == 0 && pNode->port == PORT)
            {
               ; 
            }
            else
            {
                BroadcastMyself(pNode->fd,addr,port);
            }
        }
        i = pNode->hash;
    } 
    return 0;
}
int DisconnectDataNode(tCluster* cluster)
{
    int i = 0;    
    while(i < MAX_NODE_NUM)
    {
        tNode * pNode = (tNode*)GetNode(cluster,i);
        if(pNode->fd > 0)
        {
            RemoteDBDelete(pNode->fd);
        }
        i = pNode->hash;
    } 
    return 0;
}

/* tell other nodes who am i */
int BroadcastMyself(int h,char * addr,int port)
{
    int DataNum = -1;
    char Buf[MAX_BUF_LEN] = "\0";
    int BufSize = MAX_BUF_LEN;
    char szMsg[MAX_BUF_LEN] = "\0";
    char ppData[MAX_DATA_NUM][MAX_DATA_LEN] = {0};
    DataNum = 1;
    sprintf(ppData[0],"%s %d\0",addr,port);
    FormatDataN(Buf,&BufSize,CTRL_REG_CMD,ppData,DataNum);
    SendData(h,Buf,BufSize);
    RecvData(h,Buf,&BufSize);
    int cmd = -1;
    ParseDataN(Buf,BufSize,&cmd,&DataNum,ppData);
    if(cmd != CTRL_REG_RSP || DataNum > MAX_DATA_NUM || DataNum < 2)
    {
        fprintf(stderr,"BroadcastMyself Error,%s:%d\n",__FILE__,__LINE__);
        return -1;         
    }
    return 0;    
}
