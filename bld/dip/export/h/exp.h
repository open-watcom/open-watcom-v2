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


#ifndef DIP_EXP

#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "dip.h"
#include "dipimp.h"


/*
    An imp_mod_handle is defined as an unsigned_16. The value zero is
    reserved to indicate "no module".
*/
#define MY_MOD_ID       ((imp_mod_handle)1)

typedef struct exp_sym          exp_sym;
typedef struct exp_block        exp_block;
typedef struct exp_hunk         exp_hunk;

struct imp_sym_handle {
    /* any stuff for getting information on symbols */
    exp_sym             *p;
};

struct imp_type_handle {
    /* any stuff for getting information on types */
    unsigned_8          code;
};

struct imp_cue_handle {
    /* any stuff for getting information on source line cues */
    void                *p;
};


struct exp_sym {
    exp_sym     *next;
    addr_ptr    addr;
    unsigned_16 len;
    char        name[1];
};

struct exp_block {
    exp_block   *next;
    addr_ptr    start;
    unsigned_32 len;
    unsigned_8  code;
};

#define HUNK_SIZE       1024

struct exp_hunk {
    exp_hunk    *next;
    unsigned    left;
    unsigned_8  data[HUNK_SIZE];
};

struct imp_image_handle {
    exp_sym     *gbl;
    exp_block   *addr;
    char        *name;
    unsigned    len;
    exp_hunk    *hunks;
};

extern address  NilAddr;

extern exp_block        *FindAddrBlock( imp_image_handle *, addr_ptr );

#define SameAddrSpace( a, b )   ((a).segment == (b).segment)

#define DIP_EXP
#endif
