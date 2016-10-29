/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  Trap/Server request strings
*
****************************************************************************/

#include "watcom.h"

char *DBG_ReqStrings[] = {
    "REQ_CONNECT",                        /* 00 */
    "REQ_DISCONNECT",                     /* 01 */
    "REQ_SUSPEND",                        /* 02 */
    "REQ_RESUME",                         /* 03 */
    "REQ_GET_SUPPLEMENTARY_SERVICE",      /* 04 */
    "REQ_PERFORM_SUPPLEMENTARY_SERVICE",  /* 05 */
    "REQ_GET_SYS_CONFIG",                 /* 06 */
    "REQ_MAP_ADDR",                       /* 07 */
    "REQ_CHECKSUM_MEM",                   /* 08 */
    "REQ_READ_MEM",                       /* 09 */
    "REQ_WRITE_MEM",                      /* 10 */
    "REQ_READ_IO",                        /* 11 */
    "REQ_WRITE_IO",                       /* 12 */
    "REQ_PROG_GO",                        /* 13 */
    "REQ_PROG_STEP",                      /* 14 */
    "REQ_PROG_LOAD",                      /* 15 */
    "REQ_PROG_KILL",                      /* 16 */
    "REQ_SET_WATCH",                      /* 17 */
    "REQ_CLEAR_WATCH",                    /* 18 */
    "REQ_SET_BREAK",                      /* 19 */
    "REQ_CLEAR_BREAK",                    /* 20 */
    "REQ_GET_NEXT_ALIAS",                 /* 21 */
    "REQ_SET_USER_SCREEN",                /* 22 */
    "REQ_SET_DEBUG_SCREEN",               /* 23 */
    "REQ_READ_USER_KEYBOARD",             /* 24 */
    "REQ_GET_LIB_NAME",                   /* 25 */
    "REQ_GET_ERR_TEXT",                   /* 26 */
    "REQ_GET_MESSAGE_TEXT",               /* 27 */
    "REQ_REDIRECT_STDIN",                 /* 28 */
    "REQ_REDIRECT_STDOUT",                /* 29 */
    "REQ_SPLIT_CMD",                      /* 30 */
    "REQ_READ_REGS",                      /* 31 */
    "REQ_WRITE_REGS",                     /* 32 */
    "REQ_MACHINE_DATA"                    /* 33 */
};

uint_8                   DBG_Indent;
uint_8                   DBG_Lines;
