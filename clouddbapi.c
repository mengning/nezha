
/********************************************************************/
/* Copyright (C) MC2Lab-USTC, 2012                                  */
/*                                                                  */
/*  FILE NAME             :  clouddbapi.c                           */
/*  PRINCIPAL AUTHOR      :  Mengning                               */
/*  SUBSYSTEM NAME        :  cloud                                  */
/*  MODULE NAME           :  clouddbapi                             */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  Linux                                  */
/*  DATE OF FIRST RELEASE :  2012/12/28                             */
/*  DESCRIPTION           :  Impement of Client Side Interface      */
/*                           to access cloud database(multi-servers)*/
/*                           distribute strategy inside here        */
/********************************************************************/

/*
 * Revision log:
 *
 * Created by Mengning ,2012/5/30
 * Modify for Nezha project,by Mengning,2012/11/27
 * Modify for Client Side of Nezha project,by Mengning,2012/12/18
 * Copy from remotedbapi.c,provide dbapi.h interface,by Mengning,2012/12/28
 *
 */

#include "dbapi.h"
#include "remotedbapi.h"
#include "nodes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


#define MAX_NODE_NUM        nodes->SumOfNode
#define debug               printf
              


/*
 * Create an Database
 */
tDatabase  DBCreate(const char * filename)
{
    tLinkTable * nodes = RegisterAndLoadCloudNodes(NULL,0);
    tCloudNode * pNode = (tCloudNode*)GetLinkTableHead(nodes);
    while(pNode != NULL)
    {
        debug("DBCreate %s:%d\n",pNode->addr,pNode->port);
        pNode->fd = RemoteDBCreate(filename,pNode->addr,pNode->port);
        if(pNode->fd == -1)
        {
            return NULL;
        }
        pNode = (tCloudNode*)GetNextLinkTableNode(nodes,(tLinkTableNode *)pNode);
    }    
    return (tDatabase)nodes;
}

/*
 * Delete the Database
 */
int DBDelete(tDatabase db)
{
    tLinkTable* nodes = (tLinkTable*)db;
    tCloudNode * pNode = (tCloudNode*)GetLinkTableHead(nodes);
    while(pNode != NULL)
    {
        if(pNode->fd != -1)
        {
            RemoteDBDelete(pNode->fd);
        }
        pNode = (tCloudNode*)GetNextLinkTableNode(nodes,(tLinkTableNode *)pNode);
    } 
    return 0;
}
/* find the node in linktable */
static tCloudNode * FindNode(tLinkTable* nodes,int index)
{
    int i = 0;
    tCloudNode * pNode = (tCloudNode*)GetLinkTableHead(nodes);
    while(pNode != NULL)
    {
        if(i == index)
        {
            return pNode;;
        }
        pNode = (tCloudNode*)GetNextLinkTableNode(nodes,(tLinkTableNode *)pNode);
        i++;
    }
    return NULL; 
}
/*
 * Set key/value
 */
int DBSetKeyValue(tDatabase db,tKey key,tValue value)
{
    tLinkTable* nodes = (tLinkTable*)db;
    int nodeindex = key%MAX_NODE_NUM;/* distribute strategy */
    tCloudNode * pNode = FindNode(nodes,nodeindex);
    debug("key=%d,nodeindex=%d\n",key,nodeindex);
    return RemoteDBSetKeyValue(pNode->fd,key,value);
}

/*
 * get key/value
 */
int DBGetKeyValue(tDatabase db,tKey key,tValue *pvalue)
{
    tLinkTable* nodes = (tLinkTable*)db;
    int nodeindex = key%MAX_NODE_NUM;/* distribute strategy */
    tCloudNode * pNode = FindNode(nodes,nodeindex);
    debug("key=%d,nodeindex=%d\n",key,nodeindex);  
    return RemoteDBGetKeyValue(pNode->fd,key,pvalue);
}

/*
 * delete key/value
 */
int DBDelKeyValue(tDatabase db,tKey key)
{
    tLinkTable* nodes = (tLinkTable*)db;
    int nodeindex = key%MAX_NODE_NUM;/* distribute strategy */
    tCloudNode * pNode = FindNode(nodes,nodeindex); 
    debug("key=%d,nodeindex=%d\n",key,nodeindex);   
    return RemoteDBDelKeyValue(pNode->fd,key);
}
