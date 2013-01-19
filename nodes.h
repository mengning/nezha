
/********************************************************************/
/* Copyright (C) SSE-USTC, 2012-2013                                */
/*                                                                  */
/*  FILE NAME             :  nodes.h                                */
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
#ifndef _NODES_H_
#define _NODES_H_


#include "protocol.h"

#define MAX_NODE_NUM        1024
#define PORT                5001
#define IP_ADDR             "127.0.0.1"

#define ADDR_STR_LEN        128              

typedef struct Node
{
    int hash;
    char addr[ADDR_STR_LEN];
    int  port;
    int  fd;
}tNode;

typedef struct Cluster
{
    int SumOfNodes;
    tNode * nodes[MAX_NODE_NUM]; /* array of node info */
}tCluster;


/*
 * Basic Operation
 */

/* Init cloud nodes info */
tCluster *  InitCluster();
/* Destroy Cluster */
int DestroyCluster(tCluster * cluster);
/* Get cloud nodes info */
tNode *  GetNode(tCluster * cluster,int hash);
/* Add cloud nodes info */
int  AddNode(tCluster * cluster,char * addr,int port);
/* Register cloud nodes info */
int  RemoveNode(tCluster * cluster,int hash,char * addr,int port,int fd);

/* Register And Load cloud nodes info */
tCluster *  RegisterAndLoadClusterNodes(char * addr,int port);
/* add recved buf(nodes info) to cluster */
int AddClusterNodes(tCluster * cluster,char ppData[MAX_DATA_NUM][MAX_DATA_LEN],int DataNum);
/* Cluseter(Nodes info) to ppData array */
int ClusterNodesInfo(tCluster * cluster,char ppData[MAX_DATA_NUM][MAX_DATA_LEN],int *NodeNum);

int ConnectDataNode(tCluster* cluster,char * addr,int port,char * filename);
int DisconnectDataNode(tCluster* cluster);
#endif /* _NODES_H_ */
