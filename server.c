/********************************************************************/
/* Copyright (C) MC2Lab-USTC, 2012                                  */
/*                                                                  */
/*  FILE NAME               :  server.c                             */
/*  PRINCIPAL AUTHOR        :  Mengning                             */
/*  SUBSYSTEM NAME          :  main                                 */
/*  MODULE NAME             :  server                               */
/*  LANGUAGE                :  C                                    */
/*  TARGET ENVIRONMENT      :  Linux                                */
/*  DATE OF FIRST RELEASE   :  2013/01/02                           */
/*  DESCRIPTION             :  Server start here                    */
/********************************************************************/

/*
 * Revision log:
 *
 * Created by Mengning ,2013/01/02
 *
 */

#include "engine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <time.h>

#define PORT            5001
#define IP_ADDR         "127.0.0.1"
#define MAX_BUF_LEN     1024

#define debug           printf



int main(int argc, char *argv[])
{
    printf("Nezha Database System Server Engine Starts\n");
    char * addr;
    int    port;
    if (argc != 3) 
    {
        addr = IP_ADDR;
        port = PORT;
    }
    else
    {
        addr = argv[1];
        port = atoi(argv[2]);        
    }
    ServiceEngine(addr,port);
    return 0;
}
