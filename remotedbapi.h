/********************************************************************/
/* Copyright (C) MC2Lab-USTC, 2012                                  */
/*                                                                  */
/*  FILE NAME             :  remotedbapi.h                          */
/*  PRINCIPAL AUTHOR      :  Mengning                               */
/*  SUBSYSTEM NAME        :  DB                                     */
/*  MODULE NAME           :  remotedbapi                            */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  ANY                                    */
/*  DATE OF FIRST RELEASE :  2012/2/28                              */
/*  DESCRIPTION           :  Remote Interface of NoSQL DB API       */
/********************************************************************/

/*
 * Revision log:
 *
 * Created by Mengning,2012/2/28
 * add some input/output args comments by Mengning,2012/2/29
 * Modify for Nezha project,by Mengning,2012/11/27
 * Support Memory Database(no file) Operations,by Mengning,2012/12/18
 * copy from dbapi.h,and change api names for inserting cloud module,
 * so that provide the same api for higher layer,by Mengning,2012/12/28
 *
 */

#ifndef _REMOTE_DB_API_H_
#define _REMOTE_DB_API_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "dbapi.h" /* use data types */

/**********************************************/
/* Database(have a file) Operations
/**********************************************/
/*
 * Create an Database
 * input	: filename,addr,port
 * output	: None
 * in/out	: None
 * return	: if SUCCESS return (tDatabase *)Database handler
 *          : if FAILURE exit(-1)
 */
tDatabase  RemoteDBCreate(const char * filename,char * addr,int port);
	

/*
 * Delete the Database
 * input	: tDatabase db
 * output	: None
 * in/out	: None
 * return	: SUCCESS(0)/exit(-1)
 */
int RemoteDBDelete(tDatabase db);


/*
 * Set key/value
 * input	: tKey key,tValue value - one key/value
 * output	: None
 * in/out	: None
 * return	: SUCCESS(0)/FAILURE(-1)
 */	
int RemoteDBSetKeyValue(tDatabase db,tKey key,tValue value);

/*
 * get key/value
 * input	: tKey key
 * output	: None
 * in/out	: tValue *pvalue MUST BE initialized,it means pvalue->str is malloced,
            : and pvalue->len is the length of pvalue->str 
            : if return SUCCESS(0),value will stored in pvalue(str,len).
 * return	: SUCCESS(0)/FAILURE(-1)
 */
int RemoteDBGetKeyValue(tDatabase db,tKey key,tValue *pvalue);

/*
 * delete key/value
 * input	: tKey key
 * output	: None
 * in/out	: None
 * return	: SUCCESS(0)/FAILURE(-1)
 */
int RemoteDBDelKeyValue(tDatabase db,tKey key);



#ifdef __cplusplus
}
#endif
#endif /* _REMOTE_DB_API_H_ */


