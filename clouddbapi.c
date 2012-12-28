
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
#include "socketwrapper.h"
#include "protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define PORT                5001
#define IP_ADDR             "127.0.0.1"
#define MAX_BUF_LEN         1024
#define ADDR_STR_LEN        128
#define MAX_NODE_NUM        3
#define debug               printf
              
typedef struct CloudNode
{
    char addr[ADDR_STR_LEN];
    int  port;
    int  db;
}tCloudNode;
/* Initialize servers info */
tCloudNode nodes[MAX_NODE_NUM] = 
{
    {IP_ADDR,5001,-1},
    {IP_ADDR,5002,-1},
    {IP_ADDR,5003,-1}    
};

/*
 * Create an Database
 */
tDatabase  DBCreate(const char * filename)
{
    int i;
    for(i = 0; i < MAX_NODE_NUM; i++)
    {
        nodes[i].db = RemoteDBCreate(filename,nodes[i].addr,nodes[i].port);
        if(nodes[i].db == -1)
        {
            return NULL;
        }
    }      
    return (tDatabase)nodes;
}

/*
 * Delete the Database
 */
int DBDelete(tDatabase db)
{
    tCloudNode *pnodes = (tCloudNode*)db;
    int i;
    for(i = 0; i < MAX_NODE_NUM; i++)
    {    
        RemoteDBDelete(pnodes[i].db);
    }
    return 0;
}


/*
 * Set key/value
 */
int DBSetKeyValue(tDatabase db,tKey key,tValue value)
{
    tCloudNode *pnodes = (tCloudNode*)db;
    int nodeindex = key%MAX_NODE_NUM;/* distribute strategy */
    debug("key=%d,nodeindex=%d\n",key,nodeindex);
    return RemoteDBSetKeyValue(pnodes[nodeindex].db,key,value);
}

/*
 * get key/value
 */
int DBGetKeyValue(tDatabase db,tKey key,tValue *pvalue)
{
    tCloudNode *pnodes = (tCloudNode*)db;
    int nodeindex = key%MAX_NODE_NUM;/* distribute strategy */ 
    debug("key=%d,nodeindex=%d\n",key,nodeindex);  
    return RemoteDBGetKeyValue(pnodes[nodeindex].db,key,pvalue);
}

/*
 * delete key/value
 */
int DBDelKeyValue(tDatabase db,tKey key)
{
    tCloudNode *pnodes = (tCloudNode*)db;
    int nodeindex = key%MAX_NODE_NUM;/* distribute strategy */ 
    debug("key=%d,nodeindex=%d\n",key,nodeindex);   
    return RemoteDBDelKeyValue(pnodes[nodeindex].db,key);
}
