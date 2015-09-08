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
* Description:  DWARF DIP public types and definitions.
*
****************************************************************************/


#ifndef DIP_DFDIP
#define DIP_DFDIP

#include <string.h>
#include <stdlib.h>
#include "dip.h"
#include "dipimp.h"
#include "dr.h"


#define SEG_CODE  MAP_FLAT_CODE_SELECTOR
#define SEG_FLAT  MAP_FLAT_CODE_SELECTOR
#define SEG_DATA  MAP_FLAT_DATA_SELECTOR

#define IMX2IMH(imx)        (imx + IMH_BASE)
#define IMH2IMX(im)         (im - IMH_BASE)

#define IMX2MODI(ii,imx)    ((ii)->mod_map+(imx))
#define IMH2MODI(ii,im)     IMX2MODI(ii,IMH2IMX(im))

typedef imp_mod_handle  im_idx;

typedef enum {
    DF_NOT = 0,  /* handle info not set */
    DF_SET = 1,
} imp_state;

typedef enum {
    SYM_VAR,
    SYM_ENUM,
    SYM_MEM,
    SYM_MEMVAR,
    SYM_VIRTF,
    SYM_MEMF
} sym_sclass;

typedef struct {
    int sign :1;
    int size :7;
} enum_einfo;

typedef struct {
    dr_handle       root;    /* root class for member */
    dr_handle       inh;     /* inheritance handle for member */
} mem_minfo;

struct imp_sym_handle {
    /* any stuff for getting information on symbols */
    sym_sclass      sclass;
    imp_mod_handle  im;
    unsigned        size;
    dr_handle       sym;
    dr_tag_type     stype;
    dr_access       acc;
    imp_state       state;
    int             isdef        :1;
    int             isstatic     :1;
    int             isartificial :1;
    union {
        enum_einfo   einfo;
        mem_minfo    minfo;
    } f;
};

typedef struct {
    uint_16    dims;
    dr_handle  index;
    int_32     low;
    uint_32    num_elts;
    uint_32    base_stride;
    uint_8     column_major :1;
    uint_8     is_set       :1;
    uint_8     is_based     :1;
} imp_array;


struct imp_type_handle {
    /* any stuff for getting information on types */
    imp_mod_handle      im;
    dr_handle           type;
    dr_typeinfo         typeinfo;
    imp_array           array;
    imp_state           state;
    int                 sub_array :1;
};

struct imp_cue_handle {
    /* any stuff for getting information on source line cues */
    imp_mod_handle      im;
    address             a;
    uint_16             fno;
    uint_16             line;
    uint_16             col;
};

/* Remember that that imp_[sym/type/cue]_handle's may be freely copied and
   destroyed by clients. No pointers to allocated memory unless there's
   another pointer somewhere else to free that memory. Clients don't have
   tell you how long they're hanging on to a handle either. Nasty bunch. */


typedef struct dwarf_info  dwarf_info; // private type
typedef struct mod_info    mod_info;   // private type
typedef struct seg_cue     seg_cue;   // private type

#include "dfseglst.h"
#include "dfcuelst.h"
#include "dfscplst.h"

typedef struct { // value of an AddrMod
    addr_ptr            mach;
    dword               len;
    imp_mod_handle      im;
} addrmod;

struct imp_image_handle {
    imp_image_handle    *next;
    dig_fhandle         sym_file;
    dwarf_info          *dwarf;
    im_idx              mod_count;
    mod_info            *mod_map;
    void                *dcmap;
    void                *name_map;
    seg_list            addr_map[1];
    cue_list            cue_map[1];
    seg_list            addr_sym[1];
    scope_ctl           scope;
    addrmod             last;
    bool                has_pubnames;
    bool                is_byteswapped;
};

extern address  NilAddr;
#ifdef DEBUG
extern void myprintf( char *ctl, ... );
#endif

#endif
