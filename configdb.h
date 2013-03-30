/********************************************************************/
/* Copyright (C) MC2Lab-USTC, 2012-2013                             */
/*                                                                  */
/*  FILE NAME             :  configdb.h                             */
/*  PRINCIPAL AUTHOR      :  Mengning                               */
/*  SUBSYSTEM NAME        :  DB                                     */
/*  MODULE NAME           :  dbapi                                  */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  ANY                                    */
/*  DATE OF FIRST RELEASE :  2012/1/14                              */
/*  DESCRIPTION           :  Abstract Interface of ConfigDB API     */
/********************************************************************/

/*
 * Revision log:
 *
 * Created by Mengning,2012/1/14
 *
 */

#ifndef _CONFIG_DB_H_
#define _CONFIG_DB_H_

#define HUAWEI_TEST 0


#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>

/********************************************************/
/*        Abstract Interface of ConfigDB API            */
/********************************************************/
#define SUCCESS             0
#define FAILURE             (-1)
/* mode */
#define LOCAL_MODE          1
#define GRID_MODE           2
#define MASTER_MODE         4
// Master info
//#define PORT                5001
//#define ADDR                "127.0.0.1"

#define NAME_STR_LEN        128

typedef struct
{
    int     mode;
    char    filename[NAME_STR_LEN];
    char    addr[NAME_STR_LEN];
    int     port;
    void  * db;
    void  * cache;
    void  * cluster;
    pthread_t engine;
    
    
}tConfigDB;

/**********************************************/
/* Database Operations
/**********************************************/
/*
 * Initialize ConfigDB
 * input	: mode,addr,port,filename
 * output	: None
 * in/out	: None
 * return	: if SUCCESS return (tConfigDB *)Database handler
 *          : if FAILURE return NULL
 */
tConfigDB*  ConfigInitialize(int mode,char * addr,int port,char * filename);
	

/*
 * Destroy the ConfigDB
 * input	: tConfigDB db
 * output	: None
 * in/out	: None
 * return	: SUCCESS(0)/FAILURE(-1)
 */
int ConfigDestroy(tConfigDB* db);


/*
 * Put key/value
 * input	: key,value - one key/value
 * output	: None
 * in/out	: None
 * return	: SUCCESS(0)/FAILURE(-1)
 */	
int ConfigPut(tConfigDB* db,const void* pKey,int KeySize,const void* pValue,int ValueSize);

/*
 * Get key/value
 * input	: key
 * output	: value
 * in/out	: pValueSize
 * return	: SUCCESS(0)/FAILURE(-1)
 */
int ConfigGet(tConfigDB* db,const void* pKey,int KeySize,void* pValue,int *pValueSize);

/*
 * delete key/value
 * input	: key
 * output	: None
 * in/out	: None
 * return	: SUCCESS(0)/FAILURE(-1)
 */
int ConfigDel(tConfigDB* db,const void* pKey,int KeySize);



#ifdef __cplusplus
}
#endif
#endif /* _CONFIG_DB_H_ */


