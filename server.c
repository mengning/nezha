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
#include "cmdline.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <time.h>

#define PORT                5001
#define IP_ADDR             "127.0.0.1\0"


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
        CreateCluster(addr,port);
    }
    else
    {
        addr = argv[1];
        port = atoi(argv[2]);
        LoadingClusterNodes(addr,port);       
    }
    /* start command line console */
    pthread_t cmdline_id;
    if(pthread_create(&cmdline_id,NULL,(void*)cmdline,(void*)0) != 0)
    {
        fprintf(stderr,"cmdline pthread_create Error,%s:%d\n",__FILE__,__LINE__);
        exit(-1);
    } 
    ServiceEngine(addr,port);
    return 0;
}
