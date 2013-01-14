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
#include "configdb.h"


/**********************************************/
/* Database Operations
/**********************************************/
/*
 * Initialize ConfigDB
 */
tConfigDB*  ConfigInitialize()
{
    tConfigDB* db = (tConfigDB*)malloc(sizeof(tConfigDB));
    return db;
}
	

/*
 * Destroy the ConfigDB
 */
int ConfigDestroy(tConfigDB* db)
{
    free(db);
    return SUCCESS;
}

/*
 * Put key/value
 */	
int ConfigPut(tConfigDB* db,const void* pKey,int KeySize,const void* pValue,int ValueSize)
{
    return SUCCESS;
}
/*
 * Get key/value
 */
int ConfigGet(tConfigDB* db,const void* pKey,int KeySize,void* pValue,int *pValueSize)
{
    return SUCCESS;
}
/*
 * delete key/value
 */
int ConfigDel(tConfigDB* db,const void* pKey,int KeySize)
{
    return SUCCESS;
}


/**********************************************/
/* Internal Operations
/**********************************************/


