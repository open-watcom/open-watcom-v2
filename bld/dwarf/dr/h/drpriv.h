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


/*
 * definitions used in the dwarf reading library that are private to the
 * library
*/

#include "dr.h"
#include "drrtns.h"
#include "virtmem.h"

#ifndef FALSE
#define FALSE (1==0)
#define TRUE  (1==1)
#endif

enum {
    TAB_IDX_FNAME,
    TAB_IDX_PATH
};

typedef struct {
    unsigned_16 fnameidx;
    unsigned_16 pathidx;
} fileidx_entry;

typedef union {
    fileidx_entry       idx;
    char *              name;
} filetab_entry;

typedef struct {
    unsigned            len;
    filetab_entry *     tab;
} file_table;

typedef struct {
    file_table  fnametab;
    file_table  pathtab;
} file_info;

typedef struct {
    unsigned_32 size;
    dr_handle   base;
} sect_info;

typedef struct COMPUNIT_INFO {
    struct COMPUNIT_INFO *      next;
    dr_handle                   start;
    file_table                  filetab;
} compunit_info;

#if 0
typedef struct {
} cache_info;
#endif

struct dr_dbg_info {
    struct dr_dbg_info *next;
    void *              file;   // task's file information.
    sect_info           sections[DR_DEBUG_NUM_SECTS];
    compunit_info       compunit;
//  cache_info          cache;
    unsigned            numabbrevs;
    dr_handle          *abbrevs;     // variable length array.
    compunit_info       *last_ccu;
    unsigned_8          addr_size;
    unsigned_8          old_version;
};

extern struct dr_dbg_info * DWRCurrNode;
#define DWARF_VERSION 2

#define ABBREV_TABLE_GUESS 500
#define ABBREV_TABLE_INCREMENT 100

