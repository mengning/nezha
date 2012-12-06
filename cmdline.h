/********************************************************************/ 
/* Copyright (C) MC2Lab-USTC, 2012                                  */
/*                                                                  */
/*  FILE NAME             :  cmdline.h                              */
/*  PRINCIPAL AUTHOR      :  Mengning                               */
/*  SUBSYSTEM NAME        :  DB                                     */
/*  MODULE NAME           :  cmdline                                */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  ANY                                    */
/*  DATE OF FIRST RELEASE :  2012/2/28                              */
/*  DESCRIPTION           :  CmdLine Interface of Nezha database    */
/********************************************************************/

/*
 * Revision log:
 *
 * Created by Mengning,2012/12/06
 *
 */

#ifndef _CMD_LINE_H_
#define _CMD_LINE_H_


#ifdef __cplusplus
extern "C" {
#endif

/*
 * Start Cmdline Engine
 * input	: None
 * output	: None
 * in/out	: None
 * return	: if SUCCESS return 0
 *              : if FAILURE exit(-1)
 */
int StartCmdLincEngine(void);
	



#ifdef __cplusplus
}
#endif
#endif /* _CMD_LINE_H_ */


