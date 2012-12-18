
/********************************************************************/
/* Copyright (C) SSE-USTC, 2012                                     */
/*                                                                  */
/*  FILE NAME             :  protocol.h                             */
/*  PRINCIPAL AUTHOR      :  Mengning                               */
/*  SUBSYSTEM NAME        :  network                                */
/*  MODULE NAME           :  protocol                               */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  Any                                    */
/*  DATE OF FIRST RELEASE :  2012/12/18                             */
/*  DESCRIPTION           :  Protocol between Nezha Sever and Client*/
/********************************************************************/

/*
 * Revision log:
 *
 * Created by Mengning,2012/12/18
 *
 */

#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_
/*
 * Data format
    0---------------------------------------------------31
    |         operation command(int-4bytes)             |
    -----------------------------------------------------
    |         number of data(int-4bytes) = 0..x         |
    -----------------------------------------------------
    |            data-1 length(int-4bytes)              |
    -----------------------------------------------------
    |                                                   |
    /                   data-1                          /
    |                                                   |
    -----------------------------------------------------
    |          data-2..x length(int-4bytes)             |
    -----------------------------------------------------
    |                                                   |
    /                   data-2..x                       /
    |                                                   |
    0---------------------------------------------------31
 */
/* operation commands */
#define OPEN_CMD        1 /* With one data - database file name */
#define CLOSE_CMD       2 /* With zero data */
#define GET_CMD         3 /* With one data - key */
#define SET_CMD         4 /* With two data - key and value */
#define DELETE_CMD      5 /* With one data - key */
#define ERROR_RSP       0 /* With one data - Error infomation */
#define OPEN_RSP        OPEN_CMD    /* if success,With zero data */
#define CLOSE_RSP       CLOSE_CMD   /* if success,With zero data */
#define GET_RSP         GET_CMD     /* if success,With two data - key and value */
#define SET_RSP         SET_CMD     /* if success,With zero data */
#define DELETE_RSP      DELETE_CMD  /* if success,With zero data */

/*
 * FormatData - with 0-2 data
 * input	: cmd,data and data size
 * in/out	: pBuf - to store formated packet 
 * input	: BufSize - size of the memory pointed by pBuf
 * return	: if SUCCESS return 0
 *          : if FAILURE return (-1)
 */
int FormatData(char * pBuf,BufSize,int cmd);
int FormatData1(char * pBuf,BufSize,int cmd,char* pData,int DataSize);
int FormatData2(char * pBuf,BufSize,int cmd,char* pData1,int Data1Size,char* pData2,int Data2Size);
/*
 * ParseData - with 0-2 data
 * input	: pBuf - point to recved packet 
 * input	: BufSize - size of packet pointed by pBuf
 * output   : pcmd,pDataNum
 * output   : pData1,pData1Size,pData2,pData2Size,if no data,*pDataX == '\0',*pData1Size == 0
 * return	: if SUCCESS return 0
 *          : if FAILURE return (-1)
 */
int ParseData(char * pBuf,BufSize,int *pcmd,int * pDataNum,char* pData1,int *pData1Size,char* pData2,int *pData2Size);
        
#endif /* _PROTOCOL_H_ */


