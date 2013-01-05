
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
 *
 */
#ifndef _NODES_H_
#define _NODES_H_

#include "linktable.h"
#include "protocol.h"

#define PORT                5001
#define IP_ADDR             "127.0.0.1\0"

#define ADDR_STR_LEN        128              

typedef struct CloudNode
{
    tLinkTableNode * pNext;
    char addr[ADDR_STR_LEN];
    int  port;
    int  fd;
}tCloudNode;

/* Init cloud nodes info */
tLinkTable *  InitCloudNodes(char * addr,int port);
/* Register And Load cloud nodes info */
tLinkTable *  RegisterAndLoadCloudNodes(char * addr,int port);
/* add recved buf(nodes info) to linktable */
int AddCloudNodes(tLinkTable * nodes,char ppData[MAX_DATA_NUM][MAX_DATA_LEN],int DataNum);
/* Linktable(Nodes info) to ppData array */
int CloudNodesInfo(tLinkTable * nodes,char ppData[MAX_DATA_NUM][MAX_DATA_LEN],int *NodeNum);


#endif /* _NODES_H_ */
