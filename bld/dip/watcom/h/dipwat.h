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


#ifndef DIP_WATCOM
#define DIP_WATCOM

#include <string.h>
#include <stdlib.h>
#include "dip.h"
#include "dipimp.h"
#include "dbginfo.h"

#define INFO_MAX_BLOCK      0xff80

#define MAX_LINK_ENTRIES    (4 * 1024U) /* per table block */

#define IDX2IMH(x)          (x + IMH_BASE)
#define IMH2IDX(x)          (x - IMH_BASE)

struct type_pos {
    unsigned short      entry;
    unsigned short      offset;
};

struct imp_sym_handle {
    imp_mod_handle      im;
    union {
        /* for SH_GBL */
        void            *gbl;
        /* for SH_LCL */
        struct {
            unsigned short base;
            unsigned short offset;
            void           *gbl_link;
        }               lcl;
        /* for SH_MBR, SH_TYP, SH_CST */
        struct {
            struct type_pos     t;
            struct type_pos     h;
        }               typ;
    }                   u;
    byte                name_off;
    enum {
        SH_NONE,
        SH_GBL,
        SH_LCL,
        SH_MBR,
        SH_TYP,
        SH_CST
    }   type;
};

struct imp_type_handle {
    imp_mod_handle      im;
    struct type_pos     t;
    union {
        struct {
            unsigned            array_ss        : 4;
            unsigned            col_major       : 1;
            unsigned            gbl             : 1;
            unsigned            sclr            : 1;
            unsigned            chr             : 1;
        }               s;
        unsigned        all;
    }                   f;
};

struct subrange_info {
    long                lo_bound;
    long                hi_bound;
};

struct imp_cue_handle {
    imp_mod_handle      im;
    unsigned short      entry;
    unsigned short      seg_bias;
    unsigned short      info_bias;
};

typedef struct info_block {
    void                *link;
    struct info_block   *next;
    unsigned short      size;
    byte                info[1];        /* variable sized */
} info_block;

typedef struct section_info {
    struct imp_image_handle     *ctl;
    info_block                  *mod_info;
    info_block                  *addr_info;
    info_block                  *gbl;
    pointer_int                 **dmnd_link;
    word                        mod_base_idx;
    unsigned                    sect_id;
} section_info;

struct imp_image_handle {
    struct section_info         *sect;
    unsigned                    num_sects;
    dig_fhandle                 sym_file;
    unsigned                    num_segs;
    char                        *lang;
    addr_seg                    *map_segs;
    addr_ptr                    *real_segs;
    unsigned                    v2              : 1;
};

typedef enum {
    NEED_NOTHING        = 0x00,
    NEED_REGISTERS      = 0x01,
    NEED_BASE           = 0x02,
    EMPTY_EXPR          = 0x04
} location_info;

extern address          NilAddr;

typedef walk_result (INT_MOD_WKR)( imp_image_handle *, imp_mod_handle, void * );
walk_result     MyWalkModList( imp_image_handle *, INT_MOD_WKR *, void * );

extern dip_imp_routines ImpInterface;

#endif
