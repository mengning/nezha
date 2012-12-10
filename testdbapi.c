/********************************************************************/
/* Copyright (C) MC2Lab-USTC, 2012                                  */
/*                                                                  */
/*  FILE NAME             :  testdbapi.c                            */
/*  PRINCIPAL AUTHOR      :  Mengning                               */
/*  SUBSYSTEM NAME        :  DB                                     */
/*  MODULE NAME           :  main                                   */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  Linux                                  */
/*  DATE OF FIRST RELEASE :  2012/11/27                             */
/*  DESCRIPTION           :  The test code of dbapi.c/dbapi.h(TC)   */
/********************************************************************/

/*
 * Revision log:
 * Test dbapi,it can get/set KV data.Created by Mengning ,2012/11/27
 *
 */

#include "dbapi.h"
#include <stdio.h>
#include <string.h>

#define MAX_BUF_LEN 1024

int main(int argc, char **argv)
{
    tKey key = 0;
    char buf[MAX_BUF_LEN] = "\0";
    tValue setvalue;
    tValue getvalue;
    printf(">>>>>   Test dbapi:Basic Use Case  <<<<<\n");
    tDatabase db = DBCreate("nezha.hdb");
    key = 1;
    setvalue.str = "hello world!";
    setvalue.len = strlen(setvalue.str);
    /*
     *
     */
    int ret = DBSetKeyValue(db,key,setvalue);
    if(ret == FAILURE)
    {
        fprintf(stderr, "set error: %d:%s\n",key,setvalue.str);
    }
    printf("set %d:\"%s\" successfully!\n",key,setvalue.str);
    getvalue.str = buf;
    getvalue.len = MAX_BUF_LEN;
    ret = DBGetKeyValue(db,key,&getvalue);
    if(ret == FAILURE)
    {
        fprintf(stderr, "get error: %d\n",key);
    } 
    printf("get %d:\"%s\" successfully!\n",key,getvalue.str); 
    ret = memcmp(getvalue.str,setvalue.str,getvalue.len);
    if(ret == 0 && getvalue.len == setvalue.len)
    {
        printf(">>>>>   Test Result:Pass  <<<<<\n");
    }
    else
    {
        printf(">>>>>   Test Result:Failure  <<<<<\n");
    }
    ret = DBDelKeyValue(db,key);
    if(ret == FAILURE)
    {
        fprintf(stderr, "delete error: %d\n",key);
    }
    DBDelete(db);
    return;
}
