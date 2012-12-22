
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
 *
 */

#include "dbapi.h"
#include <tcutil.h>
#include <tchdb.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

TCMDB * allOpenedDB = NULL;

typedef struct OpenedDB
{
    TCHDB *hdb;
    int counter;
}tOpenedDB;
/*
 * Create an Database
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
    }
    else
    {
        int vsize = -1;
        tOpenedDB *opendb = (tOpenedDB*)tcmdbget(allOpenedDB,(void*)filename,strlen(filename),&vsize);
        if(opendb != NULL)
        {
            hdb = opendb->hdb;
            opendb->counter ++ ;
            tcmdbput(allOpenedDB,(void*)filename,strlen(filename),(void*)opendb,vsize);
            free(opendb);
            return (tDatabase)hdb;
        }
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
        tOpenedDB *opendb = (tOpenedDB*)tcmdbget(allOpenedDB,(void*)filename,ksize,&vsize);
        if(opendb != NULL && opendb->hdb == hdb)
        {
            opendb->counter -- ;
            if(opendb->counter <= 0)
            {
                /* remove this record */
                tcmdbout(allOpenedDB,(void*)filename,ksize);
                free(filename);
                free(opendb);
                break;
            }
            /* update this record counter -- */
            tcmdbput(allOpenedDB,(void*)filename,ksize,(void*)opendb,vsize);
            free(filename);
            free(opendb);            
            return 0;
        } 
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
