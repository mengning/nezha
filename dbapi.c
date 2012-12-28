
/********************************************************************/
/* Copyright (C) MC2Lab-USTC, 2012                                  */
/*                                                                  */
/*  FILE NAME             :  dbapi.c                                */
/*  PRINCIPAL AUTHOR      :  Mengning                               */
/*  SUBSYSTEM NAME        :  DB                                     */
/*  MODULE NAME           :  dbapi                                  */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  Linux                                  */
/*  DATE OF FIRST RELEASE :  2012/4/17                              */
/*  DESCRIPTION           :  Impement of Abstract Interface         */
/*                           for Tokyo Cabinet DB API               */
/********************************************************************/

/*
 * Revision log:
 *
 * Created by Mengning ,2012/5/30
 * Modify for Nezha project,by Mengning,2012/11/27
 * Support Memory Database(no file) Operations,by Mengning,2012/12/18
 *
 */

#include "dbapi.h"
#include <tcutil.h>
#include <tchdb.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>

TCMDB * allOpenedDB = NULL;
pthread_mutex_t dbmutex;

typedef struct OpenedDB
{
    TCHDB *hdb;
    int counter;
}tOpenedDB;
/**********************************************/
/* Database(have a file) Operations
/**********************************************/

/*
 * Create an Database
 * NOTICE:Avoid openning one database file twice,and support multi-thread operations
 */
tDatabase  DBCreate(const char * filename)
{
    int ecode;
    TCHDB *hdb;
    if (filename == NULL)
    {
        filename = "nezha.hdb";   
    }
    if(allOpenedDB == NULL)
    {
        allOpenedDB = tcmdbnew();
        pthread_mutex_init(&dbmutex, NULL);
    }
    else
    {
        int vsize = -1;
        pthread_mutex_lock(&dbmutex);
        tOpenedDB *opendb = (tOpenedDB*)tcmdbget(allOpenedDB,(void*)filename,strlen(filename),&vsize);
        if(opendb != NULL)
        {
            hdb = opendb->hdb;
            opendb->counter ++ ;
            //if (opendb->counter == 2)
            //    sleep(10);
            //printf("%s's counter before put: %d\n", filename, opendb->counter);
            tcmdbput(allOpenedDB,(void*)filename,strlen(filename),(void*)opendb,vsize);
            //printf("%s's counter after put: %d\n", filename, opendb->counter);
            pthread_mutex_unlock(&dbmutex);
            free(opendb);
            return (tDatabase)hdb;
        }
        pthread_mutex_unlock(&dbmutex);
    }
    hdb = tchdbnew();
    /* set mutual exclusion control of a hash database object for threading. */
    tchdbsetmutex(hdb);     
    /* open the database */
    if(!tchdbopen(hdb, filename, HDBOWRITER | HDBOCREAT))
    {
       	ecode = tchdbecode(hdb);
       	fprintf(stderr, "open error: %s\n", tchdberrmsg(ecode));
       	exit(-1);
    }
    tOpenedDB db;
    db.hdb = hdb;
    db.counter = 1;
    tcmdbput(allOpenedDB,(void*)filename,strlen(filename),(void*)&db,sizeof(tOpenedDB));
    return (tDatabase)hdb;

}

/*
 * Delete the Database
 */
int DBDelete(tDatabase db)
{
    TCHDB * hdb = (TCHDB*)db;
    int i;
    long sum = (long)tcmdbrnum(allOpenedDB);
    tcmdbiterinit(allOpenedDB);
    for(i = 0;i < sum;i++)
    {
        int ksize = -1;
        char * filename = (char*)tcmdbiternext(allOpenedDB,&ksize);
        if(filename == NULL)
        {
            fprintf(stderr,"DBDelete:Can't find the database,%s:%d\n",__FILE__,__LINE__);
            break;
        }
        int vsize = -1;
        pthread_mutex_lock(&dbmutex);
        tOpenedDB *opendb = (tOpenedDB*)tcmdbget(allOpenedDB,(void*)filename,ksize,&vsize);
        if(opendb != NULL && opendb->hdb == hdb)
        {
            opendb->counter -- ;
            if(opendb->counter <= 0)
            {
                /* remove this record */
                tcmdbout(allOpenedDB,(void*)filename,ksize);
                pthread_mutex_unlock(&dbmutex);
                free(filename);
                free(opendb);
                break;
            }
            /* update this record counter -- */
            tcmdbput(allOpenedDB,(void*)filename,ksize,(void*)opendb,vsize);
            pthread_mutex_unlock(&dbmutex);
            free(filename);
            free(opendb);            
            return 0;
        } 
        pthread_mutex_unlock(&dbmutex);
        free(filename);
        free(opendb);
    }
   
    int ecode;
    if(!tchdbclose(hdb))
    {
       	ecode = tchdbecode(hdb);
       	fprintf(stderr, "close error: %s\n", tchdberrmsg(ecode));
       	return (-1);
    }
    tchdbdel(hdb);
    return 0;
}


/*
 * Set key/value
 */
int DBSetKeyValue(tDatabase db,tKey key,tValue value)
{
    int ecode;

    if(!tchdbput(db,&key,sizeof(tKey),value.str,value.len))
    {
        ecode = tchdbecode(db);
        fprintf(stderr, "put error: %s\n", tchdberrmsg(ecode));
        return -1;
    }

    return 0;
}

/*
 * get key/value
 */
int DBGetKeyValue(tDatabase db,tKey key,tValue *pvalue)
{
    int ecode;
    int ret;

    ret = tchdbget3(db,&key,sizeof(tKey),pvalue->str,pvalue->len);
    if(ret == -1)
    {
        ecode = tchdbecode(db);
        fprintf(stderr, "get error: %s\n", tchdberrmsg(ecode));   
        return -1;        
    }	
    pvalue->str[ret] = '\0';
    pvalue->len=ret;
    return 0;
}

/*
 * delete key/value
 */
int DBDelKeyValue(tDatabase db,tKey key)
{
    int ecode;
    if(!tchdbout(db, &key, sizeof(tKey)))
    {
        ecode = tchdbecode(db);
        fprintf(stderr, "delete error: %s\n", tchdberrmsg(ecode));
        return -1;
    }
    return 0;
}

/**********************************************/
/* Memory Database(no file) Operations
/**********************************************/
/*
 * Create an Memory Database
 * input	: None
 * output	: None
 * in/out	: None
 * return	: if SUCCESS return (tDatabase *)Database handler
 *          : if FAILURE exit(-1)
 */
tDatabase  MDBCreate()
{
    TCMDB * mdb = tcmdbnew();
    return (tDatabase)mdb;
}
	
/*
 * Delete the Database
 * input	: tDatabase db
 * output	: None
 * in/out	: None
 * return	: SUCCESS(0)/exit(-1)
 */
int MDBDelete(tDatabase mdb)
{
    tcmdbdel((TCMDB*)mdb);
    return 0;
}


/*
 * Set key/value
 * input	: tKey key,tValue value - one key/value
 * output	: None
 * in/out	: None
 * return	: SUCCESS(0)/FAILURE(-1)
 */	
int MDBSetKeyValue(tDatabase mdb,tKey key,tValue value)
{
    tcmdbput((TCMDB*)mdb,(void*)&key,sizeof(tKey),value.str,value.len); 
    return 0;   
}

/*
 * get key/value
 * input	: tKey key
 * output	: None
 * in/out	: tValue *pvalue MUST BE initialized,it means pvalue->str is malloced,
            : and pvalue->len is the length of pvalue->str 
            : if return SUCCESS(0),value will stored in pvalue(str,len).
 * return	: SUCCESS(0)/FAILURE(-1)
 */
int MDBGetKeyValue(tDatabase mdb,tKey key,tValue *pvalue)
{
    int vsize = -1;
    char *v = tcmdbget((TCMDB*)mdb,(void*)&key,sizeof(tKey),&vsize);
    if(v != NULL && vsize > 0 && vsize <= pvalue->len)
    {
        memcpy(pvalue->str,v,vsize);
        pvalue->len = vsize;
        return 0;
    }
    return -1;
}

/*
 * delete key/value
 * input	: tKey key
 * output	: None
 * in/out	: None
 * return	: SUCCESS(0)/FAILURE(-1)
 */
int MDBDelKeyValue(tDatabase mdb,tKey key)
{
    tcmdbout((TCMDB*)mdb,(void*)&key,sizeof(tKey));
    return 0;
}

