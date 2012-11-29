/********************************************************************/
/* Copyright (C) MC2Lab-USTC, 2012                                  */
/*                                                                  */
/*  FILE NAME             :  main.c                                 */
/*  PRINCIPAL AUTHOR      :  Mengning                               */
/*  SUBSYSTEM NAME        :  DB                                     */
/*  MODULE NAME           :  main                                   */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  Linux                                  */
/*  DATE OF FIRST RELEASE :  2012/11/27                             */
/*  DESCRIPTION           :  The main of Nezha KV Database system   */
/********************************************************************/
/********************************************************************/
/*              Software Architecture of Nezha                      */
/********************************************************************
                    *****************
                    *      main     *
                    *****************
                            |
                            |
                    *****************
                    *     dbapi     *
                    *****************
                            |
                            |
                    *****************
                    * Tokyo Cabinet *
                    *****************
**********************************************************************/
/*
 * Revision log:
 * Nezha 0.1.0,it can get/set KV data.Created by Mengning ,2012/11/27
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
    tDatabase db = DBCreate();
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
    DBDelete(db);
    return;
}
