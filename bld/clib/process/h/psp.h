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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#ifndef _psp_h
#define _psp_h

#include "variety.h"
#include "fcb.h"

typedef unsigned char   _byte;

#pragma pack(__push,1);
typedef struct a_psp {  /* DOS 3.X Program Segment Prefix */
    _byte       exit[2];                /* Contains INT 20h */
    unsigned    maxpara;                /* Top of memory */
    _byte       res1[1];
    _byte       opcode;                 /* Long call to DOS */
    unsigned    avail;                  /* # bytes in segment */
    unsigned    segment;
    void        _WCFAR *terminate;      /* Terminate address */
    void        _WCFAR *ctrl_break;     /* Ctrl-break exit address */
    void        _WCFAR *crit_error;     /* Critical error exit address */
    unsigned    parent_psp;             /* undocumented */
    _byte       sft_indices[20];        /* undocumented */
    unsigned    envp;                   /* Segment address of environment */
    void        _WCFAR *savstk;         /* undocumented */
    unsigned    num_handles;            /* undocumented */
    _byte       _WCFAR *handle_table;   /* undocumented */
    _byte       res3[8];
    _byte       doscall[2];             /* DOS call */
    _byte       res4[10];
    an_fcb      fcb1;                   /* unopened standard FCB1 */
    an_fcb      fcb2;                   /* unopened standard FCB2 */
    _byte       fcbx[4];
    char        cmdline[128];           /* Command parameters */
} a_psp;
#pragma pack(__pop);

#endif
