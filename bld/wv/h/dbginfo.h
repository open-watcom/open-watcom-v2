/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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


#ifndef __DBGINFO_INCLUDED
#define __DBGINFO_INCLUDED

#include "dbgmem.h"
#include "dipwv.h"


typedef enum inspect_type {
    INSP_CODE,
    INSP_DATA,
    INSP_RAW_DATA,
} inspect_type;

enum ll_flags {
    LLF_CONST = 0x01,
    LLF_REG_FLAGS_SHIFT = 1
};

struct location_context {
    address             frame;
    address             stack;
    address             execution;
    location_list       object;
    machine_state       *regs;
    type_handle         *th;
    sym_handle          *sh;
    unsigned            use;
    boolbit             have_frame              : 1;
    boolbit             have_stack              : 1;
    boolbit             have_object             : 1;
    boolbit             maybe_have_frame        : 1;
    boolbit             maybe_have_object       : 1;
    boolbit             up_stack_level          : 1;
};

typedef struct sym_list {
    struct sym_list     *next;
    location_list       loc;
    byte                h[1]; /* variable size */
} sym_list;

typedef struct map_entry        map_entry;
typedef struct image_entry      image_entry;

struct image_entry {
    image_entry         *link;
    map_entry           *map_list;
    char                *symfile_name;
    unsigned long       system_handle;
    mod_handle          dip_handle;
    void                (*mapper)( image_entry *, addr_ptr *, addr_off *, addr_off * );
    address             def_addr_space;
    boolbit             nofree           : 1;
    boolbit             deferred_symbols : 1;
    char                image_name[1];          /* variable sized */
};

struct map_entry {
    map_entry           *link;
    addr_off            map_valid_lo;
    addr_off            map_valid_hi;
    addr_ptr            map_addr;
    addr_ptr            real_addr;
    bool                pre_map;
};

typedef struct {
    address             addr;
    char                *image_name;
} mappable_addr;

typedef enum {
    REMAP_REMAPPED,
    REMAP_ERROR,
    REMAP_WRONG_IMAGE,
} remap_return;

#endif
