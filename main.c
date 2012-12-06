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
/*  DESCRIPTION           :  The cmdline of Nezha KV Database system*/
/********************************************************************/
/********************************************************************/
/*              Software Architecture of Nezha                      */
/********************************************************************
                    *****************
                    *    cmdline    *
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
 * Nezha 0.1.0,it can get/set KV data.Created by Mengning,2012/11/27
 * Nezha 0.2.0,it add cmdline interface.Created by Mengning,2012/12/06
 *
 */

#include "dbapi.h"
#include <stdio.h>
#include <string.h>

#define CMD_NUM     5
#define MAX_STR_LEN 1024
 
const char * CmdPattern[CMD_NUM] = 
{
/* 0 */    "open",
/* 1 */    "close",
/* 2 */    "set( *|\\t)\\d*( *|\\t).*",
/* 3 */    "get( *|\\t)\\d*",
/* 4 */    "delete( *|\\t)\\d*"
};
char cmdbuf[MAX_STR_LEN];
char prompt[MAX_STR_LEN] = "Nezha>";

int GetCmd(char * cmdbuf,int size);
int ExecCmd(char * cmdbuf);

int main(int argc, char **argv)
{
    /*start cmdline here*/
    while(1)
    {
        printf("%s ",prompt);
        GetCmd(cmdbuf,MAX_STR_LEN);
        ExecCmd(cmdbuf);
    }
    return;
}

int GetCmd(char * cmdbuf,int size)
{
    size;
    gets(cmdbuf);
    return 0;
}

int ExecCmd(char * cmdbuf)
{
    printf("%s\n",cmdbuf);
    return 0;
}