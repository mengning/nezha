
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



#define debug               printf
              


/*
 * Create an Database
 */
tDatabase  DBCreate(const char * filename)
{
    tCluster * cluster = RegisterAndLoadClusterNodes(NULL,0);
    int i = 0;    
    while(i < MAX_NODE_NUM)
    {
        tNode * pNode = (tNode*)GetNode(cluster,i);
        debug("DBCreate %s:%d\n",pNode->addr,pNode->port);
        pNode->fd = RemoteDBCreate(filename,pNode->addr,pNode->port);
        if(pNode->fd == -1)
        {
            return NULL;
        }
        i = pNode->hash;
    }    
    return (tDatabase)cluster;
}

/*
 * Delete the Database
 */
int DBDelete(tDatabase db)
{
    tCluster * cluster = (tCluster*)db;
    int i = 0;    
    while(i < MAX_NODE_NUM)
    {
        tNode * pNode = (tNode*)GetNode(cluster,i);
        if(pNode->fd != -1)
        {
            RemoteDBDelete(pNode->fd);
        }
        i = pNode->hash;
    } 
    return 0;
}
/*
 * Set key/value
 */
int DBSetKeyValue(tDatabase db,tKey key,tValue value)
{
    tCluster * cluster = (tCluster*)db;
    int hash = key%MAX_NODE_NUM;/* distribute strategy */
    tNode* pNode = (tNode*)GetNode(cluster,hash);
    debug("key=%d,hash=%d\n",key,hash);
    return RemoteDBSetKeyValue(pNode->fd,key,value);
}

/*
 * get key/value
 */
int DBGetKeyValue(tDatabase db,tKey key,tValue *pvalue)
{
    tCluster * cluster = (tCluster*)db;
    int hash = key%MAX_NODE_NUM;/* distribute strategy */
    tNode* pNode = (tNode*)GetNode(cluster,hash);
    debug("key=%d,hash=%d\n",key,hash); 
    return RemoteDBGetKeyValue(pNode->fd,key,pvalue);
}

/*
 * delete key/value
 */
int DBDelKeyValue(tDatabase db,tKey key)
{
    tCluster * cluster = (tCluster*)db;
    int hash = key%MAX_NODE_NUM;/* distribute strategy */
    tNode* pNode = (tNode*)GetNode(cluster,hash);
    debug("key=%d,hash=%d\n",key,hash);   
    return RemoteDBDelKeyValue(pNode->fd,key);
}
