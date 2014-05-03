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
* Description:  MAPSYM DIP internals.
*
****************************************************************************/


#ifndef DIP_MSYM

#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "dip.h"
#include "dipimp.h"
#include "mapsym.h"

/*
    An imp_mod_handle is defined as an unsigned_16. The value zero is
    reserved to indicate "no module".
*/
#define MY_MOD_ID       ((imp_mod_handle)1)

typedef struct msym_sym         msym_sym;
typedef struct msym_block       msym_block;
typedef struct msym_hunk        msym_hunk;

struct imp_sym_handle {
    /* any stuff for getting information on symbols */
    msym_sym            *p;
};

struct imp_type_handle {
    /* any stuff for getting information on types */
    unsigned_8          code;
};

struct imp_cue_handle {
    /* any stuff for getting information on source line cues */
    void                *p;
};


struct msym_sym {
    msym_sym    *next;
    addr_ptr    addr;
    unsigned_16 len;
    char        name[1];
};

struct msym_block {
    msym_block  *next;
    addr_ptr    start;
    unsigned_32 len;
    unsigned_8  code;
};

#define HUNK_SIZE       1024

struct msym_hunk {
    msym_hunk   *next;
    unsigned    left;
    unsigned_8  data[HUNK_SIZE];
};

struct imp_image_handle {
    msym_sym    *gbl;
    msym_block   *addr;
    char        *name;
    unsigned    len;
    msym_hunk   *hunks;
};

extern address  NilAddr;

extern msym_block       *FindAddrBlock( imp_image_handle *, addr_ptr );

#define SameAddrSpace( a, b )   ((a).segment == (b).segment)

#define DIP_MSYM
#endif
