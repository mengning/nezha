
/********************************************************************/
/* Copyright (C) SSE-USTC, 2012                                     */
/*                                                                  */
/*  FILE NAME             :  socketwraper.c                         */
/*  PRINCIPAL AUTHOR      :  Mengning                               */
/*  SUBSYSTEM NAME        :  network                                */
/*  MODULE NAME           :  socketwraper                           */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  Linux                                  */
/*  DATE OF FIRST RELEASE :  2012/12/13                             */
/*  DESCRIPTION           :  the interface to Linux system(socket)  */
/********************************************************************/

/*
 * Revision log:
 *
 * Created by Mengning,2012/12/13
 * Support epoll,by Mengning,2012/12/18
 *
 */
#include"socketwrapper.h"
#include<stdio.h>
#include<stdlib.h> 
#include<arpa/inet.h> /* internet socket */
#include<string.h>
#include<sys/epoll.h>
#include<fcntl.h>

int sockfd = -1;
int epollfd = -1; 

/* private macro */
#define PrepareSocket(addr,port)                        \
        struct sockaddr_in serveraddr;                  \
        struct sockaddr_in clientaddr;                  \
        socklen_t addr_len = sizeof(struct sockaddr);   \
        serveraddr.sin_family = AF_INET;                \
        serveraddr.sin_port = htons(port);              \
        serveraddr.sin_addr.s_addr = inet_addr(addr);   \
        memset(&serveraddr.sin_zero, 0, 8);             \
        sockfd = socket(PF_INET,SOCK_STREAM,0);         \
        if(sockfd == -1)                                \
        {                                               \
            fprintf(stderr,"Socket Error,%s:%d\n",      \
                            __FILE__,__LINE__);         \
            close(sockfd);                              \
            return -1;                                  \
        } 
#define InitServer()                                    \
        int ret = bind( sockfd,                         \
                        (struct sockaddr *)&serveraddr, \
                        sizeof(struct sockaddr));       \
        if(ret == -1)                                   \
        {                                               \
            fprintf(stderr,"Bind Error,%s:%d\n",        \
                            __FILE__,__LINE__);         \
            close(sockfd);                              \
            exit(-1);                                   \
        }                                               \
        listen(sockfd,MAX_LISTEN_QUEUE); 

#define InitClient()                                    \
        int ret = connect(sockfd,                       \
            (struct sockaddr *)&serveraddr,             \
            sizeof(struct sockaddr));                   \
        if(ret == -1)                                   \
        {                                               \
            fprintf(stderr,"Connect Error,%s:%d\n",     \
                __FILE__,__LINE__);                     \
            exit(-1);                                   \
        } 
                                                                      
/*
 * InitServer
 * input	: addr - server ip address
 * input	: port - server port to open
 * output	: None
 * in/out	: None
 * return	: if SUCCESS return 0
 *          : if FAILURE return (-1)
 */
int InitializeNetService(char * addr,short int port)
{
    struct epoll_event event;
    PrepareSocket(addr,port);
    epollfd = epoll_create(MAX_CONNECT_FD);
    int opts;
    opts = fcntl(sockfd,F_GETFL);
    if(opts < 0)
    {
        fprintf(stderr,"fcntl(sock,GETFL) Error,%s:%d\n", __FILE__,__LINE__);
        exit(1);
    }
    /* set non blocking */
    opts = opts|O_NONBLOCK;
    if(fcntl(sockfd,F_SETFL,opts)<0)
    {
        fprintf(stderr,"fcntl(sock,SETFL,opts) Error,%s:%d\n", __FILE__,__LINE__);
        exit(1);
    }
    event.data.fd = sockfd;
    event.events = EPOLLIN;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,sockfd,&event);
    InitServer();
    return 0;    
}

int ShutdownNetService()
{
    if(sockfd == -1)
    {
        return -1;
    }
    close(sockfd);
    if(epollfd == -1)
    {
        return -1;
    }
    close(epollfd);
    return 0;       
}
/*
 * OpenRemoteService - Only used in Client side,it connects Server.
 * input	: addr - server ip address
 * input	: port - server port to open
 * output	: None
 * in/out	: None
 * return	: if SUCCESS return tServiceHandler
 *          : if FAILURE return (-1)
 */
tServiceHandler OpenRemoteService(char * addr,short int port)
{
    PrepareSocket(addr,port);
    InitClient();
    return (tServiceHandler)sockfd;
}
/*
 * CloseRemoteService - Only used in Client side,it terminate connection.
 * input	: h - it MUST BE what OpenRemoteService returns.
 * output	: None
 * in/out	: None
 * return	: if SUCCESS return 0
 *          : if FAILURE return (-1)
 */
int CloseRemoteService(tServiceHandler h)
{
    if(h <= 0)
    {
        return -1;
    }
    close(h);
    return 0;     
}
/*
 * ServiceStart - Only used in Sever side,when client requests.
 * hide client connectionn,JUST return client who have real data request
 * input	: None
 * output	: None
 * in/out	: None
 * return	: if SUCCESS return tServiceHandler
 *          : if FAILURE return (-1)
 */
tServiceHandler ServiceStart()
{
    while(1)
    {
        struct epoll_event event;
        int fdnum = epoll_wait(epollfd,&event,1,-1);
        if(event.data.fd == sockfd)
        {
            struct sockaddr_in clientaddr;
            socklen_t addr_len = sizeof(struct sockaddr);
            int newfd = accept( sockfd,(struct sockaddr *)&clientaddr,&addr_len);
            if(newfd == -1) 
            {
                fprintf(stderr,"Accept Error,%s:%d\n", __FILE__,__LINE__);
            }
            event.data.fd = newfd;
            event.events = EPOLLIN;
            epoll_ctl(epollfd,EPOLL_CTL_ADD,newfd,&event);
        }
        else
        {
            return (tServiceHandler)event.data.fd;
        }
    }   
}
/*
 * ServiceStop - Only used in Sever side,when client connects it return.
 * input	: h - it MUST BE what ServiceStart OR  returns.
 * output	: None
 * in/out	: None
 * return	: if SUCCESS return 0
 *          : if FAILURE return (-1)
 */
int ServiceStop(tServiceHandler h)
{
    if(h <= 0)
    {
        return -1;
    }
    close(h);
    return 0;     
}
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
int SendData(tServiceHandler h,char * pBuf,int BufSize)
{
    int ret = send(h,pBuf,BufSize,0);
    if(ret < 0 || ret != BufSize)
    {
        fprintf(stderr,"Send Error,%s:%d\n",__FILE__,__LINE__);
        return -1;
    }
    return ret;
}
/*
 * SendData - send data to peer side
 * input	: h - it MUST BE what ServiceStart or OpenRemoteService returns.
 * in/out	: pBuf - point to memory that will store recved data
 * in/out	: pBufSize - the max length of data to recv
 * return	: if SUCCESS return BufSize recved
 *          : if FAILURE return (-1)
 *          : if Connection Terminals return 0
 */
int RecvData(tServiceHandler h,char * pBuf,int *pBufSize)
{
    int ret = recv(h,pBuf,*pBufSize,0);
    if(ret < 0)
    {
        fprintf(stderr,"Recv Error,%s:%d\n",__FILE__,__LINE__);
        *pBufSize = 0;
        return -1;
    }
    *pBufSize = ret;
    return ret;
}

        


