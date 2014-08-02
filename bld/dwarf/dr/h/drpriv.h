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
* Description:  Private DWARF reading library definitions.
*
****************************************************************************/


#include "dr.h"
#include "virtmem.h"
#include "drrtns.h"

#ifndef FALSE
#define FALSE (1==0)
#define TRUE  (1==1)
#endif

enum {
    TAB_IDX_FNAME,
    TAB_IDX_PATH
};

typedef unsigned_16     file_tab_idx;

typedef struct {
    file_tab_idx        fnameidx;
    file_tab_idx        pathidx;
} fileidx_entry;

typedef struct {
    union {
        fileidx_entry   idx;
        char            *name;
    } u;
} filetab_entry;

typedef struct {
    unsigned            len;
    filetab_entry       *tab;
} file_table;

typedef struct {
    file_table          fnametab;
    file_table          pathtab;
} file_info;

typedef struct {
    unsigned_32         size;
    dr_handle           base;
} sect_info;

typedef struct COMPUNIT_INFO {
    struct COMPUNIT_INFO        *next;
    dr_handle                   start;
    dr_handle                   end;
    file_table                  filetab;
    unsigned                    numabbrevs;
    dr_handle                   abbrev_start;   // offset into abbrev section
    dr_handle                   *abbrevs;       // variable length array
    unsigned                    *abbrev_refs;   // abbrevs reference counter
} compunit_info;

struct dr_dbg_info {
    struct dr_dbg_info *next;
    void *              file;   // task's file information.
    sect_info           sections[DR_DEBUG_NUM_SECTS];
    compunit_info       compunit;
    compunit_info       *last_ccu;
    unsigned_8          addr_size;
    unsigned_8          wat_version;// compatibility flag for non-standard data
    bool                byte_swap;
};

extern struct dr_dbg_info * DWRCurrNode;
#define DWARF_VERSION 2

#define ABBREV_TABLE_GUESS 500
#define ABBREV_TABLE_INCREMENT 100

#ifdef __BIG_ENDIAN__
    #define SWAP_16     CONV_LE_16
    #define SWAP_32     CONV_LE_32
    #define SWAP_64     CONV_LE_64
#else
    #define SWAP_16     CONV_BE_16
    #define SWAP_32     CONV_BE_32
    #define SWAP_64     CONV_BE_64
#endif
