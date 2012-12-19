
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
#include <stdbool.h>
#include <stdint.h>


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
    hdb = tchdbnew();
    /* open the database */
    if(!tchdbopen(hdb, filename, HDBOWRITER | HDBOCREAT))
    {
       	ecode = tchdbecode(hdb);
       	fprintf(stderr, "open error: %s\n", tchdberrmsg(ecode));
       	exit(-1);
    }

    return (tDatabase)hdb;

}

/*
 * Delete the Database
 */
int DBDelete(tDatabase db)
{
    int ecode;

    if(!tchdbclose(db))
    {
       	ecode = tchdbecode(db);
       	fprintf(stderr, "close error: %s\n", tchdberrmsg(ecode));
       	exit(-1);
    }

    tchdbdel(db);

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
