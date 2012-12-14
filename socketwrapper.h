
/********************************************************************/
/* Copyright (C) SSE-USTC, 2012                                     */
/*                                                                  */
/*  FILE NAME             :  socketwraper.h                         */
/*  PRINCIPAL AUTHOR      :  Mengning                               */
/*  SUBSYSTEM NAME        :  network                                */
/*  MODULE NAME           :  socketwraper                           */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  Any                                    */
/*  DATE OF FIRST RELEASE :  2012/12/18                             */
/*  DESCRIPTION           :  the interface to Linux system(socket)  */
/********************************************************************/

/*
 * Revision log:
 *
 * Created by Mengning,2012/12/18
 *
 */

#ifndef _SOCKET_WRAPER_H_
#define _SOCKET_WRAPER_H_

#define MAX_CONNECT_QUEUE   128
typedef int                 tServiceHandler;
/*
 * InitServer
 * input	: addr - server ip address
 * input	: port - server port to open
 * output	: None
 * in/out	: None
 * return	: if SUCCESS return 0
 *          : if FAILURE return (-1)
 */
int InitializeNetService(char * addr,short int port);
int ShutdownNetService();
/*
 * OpenRemoteService - Only used in Client side,it connects Server.
 * input	: addr - server ip address
 * input	: port - server port to open
 * output	: None
 * in/out	: None
 * return	: if SUCCESS return tServiceHandler
 *          : if FAILURE return (-1)
 */
tServiceHandler OpenRemoteService(char * addr,short int port);
/*
 * CloseRemoteService - Only used in Client side,it terminate connection.
 * input	: h - it MUST BE what OpenRemoteService returns.
 * output	: None
 * in/out	: None
 * return	: if SUCCESS return 0
 *          : if FAILURE return (-1)
 */
int CloseRemoteService(tServiceHandler h);
/*
 * ServiceStart - Only used in Sever side,when client connects it return.
 * input	: None
 * output	: None
 * in/out	: None
 * return	: if SUCCESS return tServiceHandler
 *          : if FAILURE return (-1)
 */
tServiceHandler ServiceStart();
/*
 * ServiceStop - Only used in Sever side,when client connects it return.
 * input	: h - it MUST BE what ServiceStart OR  returns.
 * output	: None
 * in/out	: None
 * return	: if SUCCESS return 0
 *          : if FAILURE return (-1)
 */
int ServiceStop(tServiceHandler h);
/*
 * SendData - send data to peer side
 * input	: h - it MUST BE what ServiceStart or OpenRemoteService returns.
 * input	: pBuf - point to data
 * input	: BufSize - the length of data bytes
 * output	: None
 * in/out	: None
 * return	: if SUCCESS return BufSize sended
 *          : if FAILURE return (-1)
 *          : if Connection Terminals return 0
 */
int SendData(tServiceHandler h,char * pBuf,int BufSize);
/*
 * SendData - send data to peer side
 * input	: h - it MUST BE what ServiceStart or OpenRemoteService returns.
 * in/out	: pBuf - point to memory that will store recved data
 * in/out	: pBufSize - the max length of data to recv
 * return	: if SUCCESS return BufSize recved
 *          : if FAILURE return (-1)
 *          : if Connection Terminals return 0
 */
int RecvData(tServiceHandler h,char * pBuf,int *pBufSize);

        
#endif /* _SOCKET_WRAPER_H_ */


