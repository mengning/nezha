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

#include "common.h"

int gethash(const char *kbuf, int ksiz);

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
    int ret = -1;
    tKey key;
    key.str = (char*)pKey;
    key.len = KeySize;
    tValue value;
    value.str = (char*)pValue;
    value.len = ValueSize;
    unsigned int hash = gethash(key.str,key.len);
    hash = hash%MAX_NODE_NUM;/* distribute strategy */
    tNode* pNode = (tNode*)GetNode(cluster,hash);
    /* Put Data */
    if(pNode->fd == 0)
    {
        ret = DBSetKeyValue(db->db,key,value);
    }
    else if(pNode->fd == -1)
    {
        pNode->fd = RemoteDBCreate(db->filename,pNode->addr,pNode->port);
        if(pNode->fd == -1)
        {
            return -1;
        }
        ret = RemoteDBSetKeyValue(pNode->fd,key,value);
    }
    else
    {
        ret = RemoteDBSetKeyValue(pNode->fd,key,value);
    }
    /* Put Master */
    if(pNode != (tNode*)GetNode(cluster,MAX_NODE_NUM))
    {
        pNode = (tNode*)GetNode(cluster,MAX_NODE_NUM);
        if(pNode->fd == 0)
        {
            ret = DBSetKeyValue(db->db,key,value);
        }
        else if(pNode->fd == -1)
        {
            pNode->fd = RemoteDBCreate(db->filename,pNode->addr,pNode->port);
            if(pNode->fd == -1)
            {
                return -1;
            }
            ret = RemoteDBSetKeyValue(pNode->fd,key,value);
        }
        else
        {
            ret = RemoteDBSetKeyValue(pNode->fd,key,value);
        }
    }
    return ret;
}
/*
 * Get key/value
 */
int ConfigGet(tConfigDB* db,const void* pKey,int KeySize,void* pValue,int *pValueSize)
{
    tCluster * cluster = (tCluster*)db->cluster;
    int ret = -1;
    tKey key;
    key.str = (char*)pKey;
    key.len = KeySize;
    tValue value;
    value.str = (char*)pValue;
    value.len = *pValueSize;
    unsigned int hash = gethash(key.str,key.len);
    hash = hash%MAX_NODE_NUM;/* distribute strategy */
    tNode* pNode = (tNode*)GetNode(cluster,hash);
    /* hit local */
    if(pNode->fd == 0)
    {
        ret = DBGetKeyValue(db->db,key,&value);
    }
    else if(pNode->fd > 0)
    {
        ret = RemoteDBGetKeyValue(pNode->fd,key,&value);
    }
    else
    {
        pNode->fd = RemoteDBCreate(db->filename,pNode->addr,pNode->port);
        if(pNode->fd == -1)
        {
            return -1;
        }
        ret = RemoteDBGetKeyValue(pNode->fd,key,&value);
    }
    /* fail to get,get from master */
    if(ret == -1)
    {
        if(pNode == (tNode*)GetNode(cluster,MAX_NODE_NUM))
        {
            return -1;
        }
        tNode* pNode = (tNode*)GetNode(cluster,MAX_NODE_NUM);
        if(pNode->fd == 0)
        {
            ret = DBGetKeyValue(db->db,key,&value);
        }
        else if(pNode->fd > 0)
        {
            ret = RemoteDBGetKeyValue(pNode->fd,key,&value);
        }
        else
        {
            pNode->fd = RemoteDBCreate(db->filename,pNode->addr,pNode->port);
            if(pNode->fd == -1)
            {
                return -1;
            }
            ret = RemoteDBGetKeyValue(pNode->fd,key,&value);
        }  
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
    tKey key;
    key.str = (char*)pKey;
    key.len = KeySize;
    unsigned int hash = gethash(key.str,key.len);
    hash = hash%MAX_NODE_NUM;/* distribute strategy */
    tNode* pNode = (tNode*)GetNode(cluster,hash);
    if(pNode->fd == 0)
    {
        return DBDelKeyValue(db->db,key);
    }
    else if(pNode->fd == -1)
    {
        pNode->fd = RemoteDBCreate(db->filename,pNode->addr,pNode->port);
        if(pNode->fd == -1)
        {
            return -1;
        }
        return RemoteDBDelKeyValue(pNode->fd,key);
    }
    else
    {
        return RemoteDBDelKeyValue(pNode->fd,key);
    }
}


/**********************************************/
/* Internal Operations
/**********************************************/
/*
 * hash
 * copy from tokyocabinet,I don't know why so. 
 */
int gethash(const char *kbuf, int ksiz)
{
#if HUAWEI_TEST
    unsigned short hash = *(unsigned short*)kbuf;
#else
    unsigned int idx = 19780211;
    unsigned int hash = 751;
    const char *rp = kbuf + ksiz;
    while(ksiz--)
    {
        idx = idx * 37 + *(unsigned char *)kbuf++;
        hash = (hash * 31) ^ *(unsigned char *)--rp;
    }
#endif
    return hash;
}