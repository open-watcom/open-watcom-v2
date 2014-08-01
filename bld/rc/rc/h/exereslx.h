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
* Description:  LX resource manipulation routines interface.
*
****************************************************************************/


#ifndef EXERESLX_H_INCLUDED
#define EXERESLX_H_INCLUDED

#include "exeflat.h"
#include "wresall.h"
#include "rcstrblk.h"

typedef struct LXResEntry {
    flat_res_table  resource;
    WResDirWindow   wind;       /* window into the current WResDir */
    uint_16         mem_flags;
    uint_16         assigned;
} LXResEntry;

typedef struct LXResTable {
    LXResEntry      *resources;
    uint_32         res_count;
    uint_32         table_size;
    uint_32         num_objects;
    uint_32         num_pages;
} LXResTable;

struct ResFileInfo;     // ANSI/gcc
struct ExeFileInfo;

int BuildLXResourceObjects( struct ExeFileInfo *exeinfo,
                            struct ResFileInfo *resinfo,
                            object_record *res_obj, unsigned_32 rva,
                            unsigned_32 offset, int writebyfile );
int WriteLXResourceObjects( struct ExeFileInfo *exe,
                            struct ResFileInfo *info );
int RcBuildLXResourceObjects( void );
int RcWriteLXResourceObjects( void );

#endif
