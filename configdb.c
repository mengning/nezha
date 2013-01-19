/********************************************************************/
/* Copyright (C) MC2Lab-USTC, 2012-2013                             */
/*                                                                  */
/*  FILE NAME             :  configdb.c                             */
/*  PRINCIPAL AUTHOR      :  Mengning                               */
/*  SUBSYSTEM NAME        :  DB                                     */
/*  MODULE NAME           :  dbapi                                  */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  linux                                  */
/*  DATE OF FIRST RELEASE :  2012/1/14                              */
/*  DESCRIPTION           :  Abstract Interface of ConfigDB API     */
/********************************************************************/

/*
 * Revision log:
 *
 * Created by Mengning,2012/1/14
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "configdb.h"
#include "nodes.h"
#include "engine.h"
#include "dbapi.h"

#define debug  printf

/**********************************************/
/* Database Operations
/**********************************************/
/*
 * Initialize ConfigDB
 */
tConfigDB*  ConfigInitialize(int mode,char * addr,int port,char * filename)
{
    debug("start ConfigInitialize...\n");
    if(strlen(addr)>NAME_STR_LEN || strlen(filename)>NAME_STR_LEN)
    {
        fprintf(stderr,"Args Error,%s:%d\n",__FILE__,__LINE__);
        return NULL;
    }
    tConfigDB* db = (tConfigDB*)malloc(sizeof(tConfigDB));
    strncpy(db->filename,filename,NAME_STR_LEN);
    strncpy(db->addr,addr,NAME_STR_LEN);
    db->port = port;
    if(mode & LOCAL_MODE)
    {
        /* open config database file */
        
    }
    else if(mode & GRID_MODE)
    {   
        /* start as Master */
        if(strcmp(addr,IP_ADDR) == 0 && PORT == port)
        {
            debug("GRID_MODE:Master.\n");
            mode |= MASTER_MODE;
            db->cluster = (void*)InitCluster();
            AddNode((tCluster*)db->cluster,addr,port);
        }
        else /* start as Data Node */
        { 
            debug("GRID_MODE:Data Node.\n");           
            db->cluster = (void*)RegisterAndLoadClusterNodes(addr,port);
        }   
    }
    else
    {
        goto ERROR;
    }
    /* create local database */
    db->db = DBCreate(filename);
    /* start Service Engine */
    debug("start Service Engine.\n");
    if(pthread_create(&(db->engine),NULL,(void*)ServiceEngine,(void*)db) != 0)
    {
        fprintf(stderr,"Service Engine pthread_create Error,%s:%d\n",__FILE__,__LINE__);
        goto ERROR;
    }
    if(ConnectDataNode((tCluster*)db->cluster,addr,port,filename) != 0)
    {
        goto ERROR;
    }
    //BroadcastMyself((tCluster*)db->cluster,addr,port);
    return db;
ERROR:
    ConfigDestroy(db);
    return NULL;
}
	

/*
 * Destroy the ConfigDB
 */
int ConfigDestroy(tConfigDB* db)
{
    DisconnectDataNode((tCluster*)db->cluster);
    pthread_join(db->engine,NULL);
    DBDelete(db->db);
    free(db);
    return SUCCESS;
}

/*
 * Put key/value
 */	
int ConfigPut(tConfigDB* db,const void* pKey,int KeySize,const void* pValue,int ValueSize)
{
    tCluster * cluster = (tCluster*)db->cluster;
    tKey key = *(tKey*)pKey;
    tValue value;
    value.str = (char*)pValue;
    value.len = ValueSize;
    int hash = key%MAX_NODE_NUM;/* distribute strategy */
    tNode* pNode = (tNode*)GetNode(cluster,hash);
    debug("key=%d,hash=%d\n",key,hash);
    if(pNode->fd == 0)
    {
        return DBSetKeyValue(db->db,key,value);
    }
    else
    {
        return RemoteDBSetKeyValue(pNode->fd,key,value);
    }
}
/*
 * Get key/value
 */
int ConfigGet(tConfigDB* db,const void* pKey,int KeySize,void* pValue,int *pValueSize)
{
    tCluster * cluster = (tCluster*)db->cluster;
    tKey key = *(tKey*)pKey;
    tValue value;
    value.str = pValue;
    value.len = *pValueSize;
    int hash = key%MAX_NODE_NUM;/* distribute strategy */
    tNode* pNode = (tNode*)GetNode(cluster,hash);
    debug("key=%d,hash=%d\n",key,hash);
    if(pNode->fd == 0)
    {
        DBGetKeyValue(db->db,key,&value);
    }
    else
    {
        RemoteDBGetKeyValue(pNode->fd,key,&value);
    }
    *pValueSize = value.len;
    return SUCCESS;
}
/*
 * delete key/value
 */
int ConfigDel(tConfigDB* db,const void* pKey,int KeySize)
{
    tCluster * cluster = (tCluster*)db->cluster;
    tKey key = *(tKey*)pKey;
    int hash = key%MAX_NODE_NUM;/* distribute strategy */
    tNode* pNode = (tNode*)GetNode(cluster,hash);
    debug("key=%d,hash=%d\n",key,hash);
    if(pNode->fd == 0)
    {
        return DBDelKeyValue(db->db,key);
    }
    else
    {
        return RemoteDBDelKeyValue(pNode->fd,key);
    }
}


/**********************************************/
/* Internal Operations
/**********************************************/
