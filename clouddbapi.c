
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

#define debug               


/*
 * Create an Database
 */
tDatabase  DBCreate(const char * filename)
{       
    return RemoteDBCreate(filename,IP_ADDR,PORT);
}

/*
 * Delete the Database
 */
int DBDelete(tDatabase db)
{
    RemoteDBDelete(db);
    return 0;
}


/*
 * Set key/value
 */
int DBSetKeyValue(tDatabase db,tKey key,tValue value)
{
    RemoteDBSetKeyValue(db,key,value);
    return 0;
}

/*
 * get key/value
 */
int DBGetKeyValue(tDatabase db,tKey key,tValue *pvalue)
{
    RemoteDBGetKeyValue(db,key,pvalue);
    return 0;
}

/*
 * delete key/value
 */
int DBDelKeyValue(tDatabase db,tKey key)
{
    RemoteDBDelKeyValue(db,key);
    return 0;
}
