/********************************************************************/
/* Copyright (C) MC2Lab-USTC, 2012                                  */
/*                                                                  */
/*  FILE NAME             :  dbapi.h                                */
/*  PRINCIPAL AUTHOR      :  Mengning                               */
/*  SUBSYSTEM NAME        :  DB                                     */
/*  MODULE NAME           :  dbapi                                  */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  ANY                                    */
/*  DATE OF FIRST RELEASE :  2012/2/28                              */
/*  DESCRIPTION           :  Abstract Interface of NoSQL DB API     */
/********************************************************************/

/*
 * Revision log:
 *
 * Created by Mengning,2012/2/28
 * add some input/output args comments by Mengning,2012/2/29
 * Modify for Nezha project,by Mengning,2012/11/27
 *
 */

#ifndef _DB_API_H_
#define _DB_API_H_


#ifdef __cplusplus
extern "C" {
#endif

/********************************************************/
/*        Abstract Interface of NoSQL DB API            */
/********************************************************/
#define SUCCESS         0
#define FAILURE         (-1)

typedef void*           tDatabase;
typedef unsigned int    tKey;

typedef struct Value{
    int     len;    /* Length of string */
    char    *str;   /* Used for Value Content */
}tValue;

/*
 * Create an Database
 * input	: None
 * output	: None
 * in/out	: None
 * return	: if SUCCESS return (tDatabase *)Database handler
 *          : if FAILURE exit(-1)
 */
tDatabase  DBCreate(void);
	

/*
 * Delete the Database
 * input	: tDatabase db
 * output	: None
 * in/out	: None
 * return	: SUCCESS(0)/exit(-1)
 */
int DBDelete(tDatabase db);


/*
 * Set key/value
 * input	: tKey key,tValue value - one key/value
 * output	: None
 * in/out	: None
 * return	: SUCCESS(0)/FAILURE(-1)
 */	
int DBSetKeyValue(tDatabase db,tKey key,tValue value);

/*
 * get key/value
 * input	: tKey key
 * output	: None
 * in/out	: tValue *pvalue MUST BE initialized,it means pvalue->str is malloced,
            : and pvalue->len is the length of pvalue->str 
            : if return SUCCESS(0),value will stored in pvalue(str,len).
 * return	: SUCCESS(0)/FAILURE(-1)
 */
int DBGetKeyValue(tDatabase db,tKey key,tValue *pvalue);

/*
 * delete key/value
 * input	: tKey key
 * output	: None
 * in/out	: None
 * return	: SUCCESS(0)/FAILURE(-1)
 */
int DBDelKeyValue(tDatabase db,tKey key);

#ifdef __cplusplus
}
#endif
#endif /* _DB_API_H_ */


