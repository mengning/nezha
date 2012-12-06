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
    StartCmdLincEngine();
    return;
}
