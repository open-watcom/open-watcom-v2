/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  This header define the structure of the Program Segment
*               Prefix, an area of memory that DOS fills after loading the
*               COM/EXE file.
*
****************************************************************************/


#ifndef _DOSPSP_H_INCLUDED
#define _DOSPSP_H_INCLUDED

#include "dosfcb.h"

typedef unsigned char   _WCFAR *handle_tab_ptr;

#pragma pack(__push,1);
typedef struct dospsp {                      /* DOS 3.X Program Segment Prefix */
    unsigned char   exit[2];                /* Contains INT 20h */
    unsigned        maxpara;                /* Top of memory */
    unsigned char   res1[1];
    unsigned char   opcode;                 /* Long call to DOS */
    unsigned        avail;                  /* # bytes in segment */
    unsigned        segment;
    void            _WCFAR *terminate;      /* Terminate address */
    void            _WCFAR *ctrl_break;     /* Ctrl-break exit address */
    void            _WCFAR *crit_error;     /* Critical error exit address */
    unsigned        parent_psp;             /* undocumented */
    unsigned char   sft_indices[20];        /* undocumented */
    unsigned        envp;                   /* Segment address of environment */
    void            _WCFAR *savstk;         /* undocumented */
    unsigned        num_handles;            /* undocumented */
    handle_tab_ptr  handle_table;           /* undocumented */
    unsigned char   res3[8];
    unsigned char   doscall[2];             /* DOS call */
    unsigned char   res4[10];
    dosfcb          fcb1;                   /* unopened standard FCB1 */
    dosfcb          fcb2;                   /* unopened standard FCB2 */
    unsigned char   fcbx[4];
    char            cmdline[128];           /* Command parameters */
} dospsp;
#pragma pack(__pop);

#endif
