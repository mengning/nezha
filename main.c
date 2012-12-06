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
#include <regex.h>

#define CMD_NUM     6
#define MAX_STR_LEN 1024

char cmdbuf[MAX_STR_LEN];
const char *prompt = "Nezha";
char dbname[MAX_STR_LEN] = "\0";
tDatabase  db = NULL;

int GetCmd(char * cmdbuf,int size);
int ExecCmd(char * cmdbuf);

int main(int argc, char **argv)
{
    /*start cmdline here*/
    while(1)
    {
        printf("%s::%s>>",prompt,dbname);
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
int CheckCmd(char * cmdbuf,char * pattern)
{
    regex_t reg;
    regmatch_t pmatch[1];
    const size_t nmatch = 1;
    regcomp(&reg,pattern,REG_EXTENDED | REG_ICASE | REG_NOSUB);
    if (regexec(&reg,cmdbuf,nmatch,pmatch,0) == 0)
    {
        regfree(&reg);
        return 0;
    }
        
}
int ExecCmd(char * cmdbuf)
{
    if(CheckCmd(cmdbuf,"help") == 0 )
    {
        printf("open filename - EX:open nezha.hdb\n"); 
        printf("set key value - EX:set 100 helloworld\n");
        printf("get key       - EX:get 100\n");
        printf("close         - leave nezha.hdb\n"); 
        printf("help          - list cmds info\n"); 
    }
    else if(CheckCmd(cmdbuf,"open( *|\\t).*\\.hdb") == 0)
    {
        if(strlen(dbname) > 0)
        {
            printf("Please close %s first.\n",dbname);
        }
        else
        {
            sscanf(cmdbuf,"open %s",dbname);
            db = DBCreate(dbname);
        }        
    }
    else if(CheckCmd(cmdbuf,"close") == 0)
    {
        DBDelete(db);
        dbname[0] = '\0';     
    }    
    else
    {
        printf("Unknow Cmd!\n");   
    }
    return 0;
}