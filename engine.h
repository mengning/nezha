
/********************************************************************/
/* Copyright (C) SSE-USTC, 2012-2013                                */
/*                                                                  */
/*  FILE NAME             :  engine.h                               */
/*  PRINCIPAL AUTHOR      :  Mengning                               */
/*  SUBSYSTEM NAME        :  main                                   */
/*  MODULE NAME           :  engine                                 */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  ANY                                    */
/*  DATE OF FIRST RELEASE :  2013/01/02                             */
/*  DESCRIPTION           :  interface of Service Engine            */
/********************************************************************/

/*
 * Revision log:
 *
 * Created by Mengning,2013/01/02
 *
 */

#ifndef _ENGINE_H_
#define _ENGINE_H_


/*
 * Server Engine for Clients' Requests
 */
int ServiceEngine(tConfigDB* db);
/* create cluster in master */
int  CreateCluster(char * addr,int port);
/* loading cluster nodes in nodes */
int  LoadingClusterNodes(char * addr,int port);


#endif /* _ENGINE_H_ */


