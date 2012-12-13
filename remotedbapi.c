
/********************************************************************/
/* Copyright (C) MC2Lab-USTC, 2012                                  */
/*                                                                  */
/*  FILE NAME             :  remotedbapi.c                          */
/*  PRINCIPAL AUTHOR      :  Mengning                               */
/*  SUBSYSTEM NAME        :  network                                */
/*  MODULE NAME           :  remotedbapi                            */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  Linux                                  */
/*  DATE OF FIRST RELEASE :  2012/12/18                             */
/*  DESCRIPTION           :  Impement of Cliet Side Interface       */
/*                           to access database                     */
/********************************************************************/

/*
 * Revision log:
 *
 * Created by Mengning ,2012/5/30
 * Modify for Nezha project,by Mengning,2012/11/27
 * Modify for Cliet Side of Nezha project,by Mengning,2012/12/18
 *
 */

#include "dbapi.h"
#include "socketwrapper.h"
#include <tcutil.h>
#include <tchdb.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define PORT                5001
#define IP_ADDR             "127.0.0.1"

tServiceHandler h = -1;

/*
 * Create an Database
 */
tDatabase  DBCreate(const char * filename)
{
    /* connect server */
    h = OpenRemoteService(IP_ADDR,PORT);
    if(h == -1)
    {
        return -1;   
    }
    /* open database file */
    
    return (tDatabase)&h;

}

/*
 * Delete the Database
 */
int DBDelete(tDatabase db)
{
    /* close database file */
    
    /* close connection */
    if(CloseRemoteService(*db) == -1)
    {
        return -1; 
    }
    return 0;
}


/*
 * Set key/value
 */
int DBSetKeyValue(tDatabase db,tKey key,tValue value)
{

    return 0;
}

/*
 * get key/value
 */
int DBGetKeyValue(tDatabase db,tKey key,tValue *pvalue)
{
    
    return 0;
}

/*
 * delete key/value
 */
int DBDelKeyValue(tDatabase db,tKey key)
{

    return 0;
}
