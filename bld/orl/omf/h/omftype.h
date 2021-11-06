/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Structures, defines and enums for OMF files.
*
****************************************************************************/


#ifndef OMF_TYPE_INCLUDED
#define OMF_TYPE_INCLUDED

#include "pcobj.h"

#define OMFENTRY

/* status flags
 */
#define OMF_STATUS_FILE_LOADED          0x00000001
#define OMF_STATUS_ARCH_SET             0x00000002
#define OMF_STATUS_ADD_LIDATA           0x00000008
#define OMF_STATUS_EASY_OMF             0x00000010
#define OMF_STATUS_ADD_BAKPAT           0x00000020
#define OMF_STATUS_WATCOM               0x00000040

#define OMF_STATUS_ADD_MASK             (OMF_STATUS_ADD_LIDATA | OMF_STATUS_ADD_BAKPAT)

/* section flags
 */
#define OMF_SEG_IS_32                   0x00000001
#define OMF_SEG_IS_BIG                  0x00000002
#define OMF_SEG_IS_ABS                  0x00000004

/* symbol flags
 */
#define OMF_SYM_FLAGS_LOCAL             0x00000001
#define OMF_SYM_FLAGS_COMDEF            0x00000002
#define OMF_SYM_FLAGS_GRPDEF            0x00000004

enum{
    OMF_SEC_NEXT_AVAILABLE = -1,
    OMF_SEC_LNAME_INDEX,
    OMF_SEC_IMPORT_INDEX,
    OMF_SEC_RELOC_INDEX,
    OMF_SEC_SYM_TABLE_INDEX,
    OMF_SEC_COMMENT_INDEX,
    OMF_SEC_DATA_CODE_START
};

/* Debug info styles we recognize */
enum {
    OMF_DBG_STYLE_UNKNOWN = -1,
    OMF_DBG_STYLE_CODEVIEW,
    OMF_DBG_STYLE_HLL
};

TYPEDEF_LOCAL_TYPE( omf_handle );
TYPEDEF_LOCAL_TYPE( omf_file_handle );
TYPEDEF_LOCAL_TYPE( omf_sec_handle );
TYPEDEF_LOCAL_TYPE( omf_symbol_handle );

TYPEDEF_LOCAL_TYPE( omf_grp_handle );

TYPEDEF_LOCAL_TYPE( omf_tmp_lidata );
TYPEDEF_LOCAL_TYPE( omf_tmp_fixup );
TYPEDEF_LOCAL_TYPE( omf_tmp_bakpat );
TYPEDEF_LOCAL_TYPE( omf_tmp_bkfix );

// handle definitions

typedef unsigned_8                      omf_file_flags;
typedef unsigned_32                     omf_file_index;
typedef unsigned_32                     omf_headers_size;

typedef unsigned_32                     omf_sec_flags;
typedef unsigned_32                     omf_sec_size;
typedef unsigned_32                     omf_sec_offset;
typedef signed_32                       omf_sec_addend;
typedef unsigned_32                     omf_sec_base;

typedef unsigned_16                     omf_reloc_type;

typedef unsigned_32                     omf_symbol_flags;
typedef unsigned_32                     omf_symbol_value;

typedef unsigned_16                     omf_idx;
typedef unsigned_16                     omf_frame;

typedef signed_32                       omf_rec_size;
typedef unsigned_8                      omf_string_len;

typedef signed_32                       omf_quantity;

typedef unsigned_8                      *omf_bytes;
typedef unsigned_8                      omf_rectyp;
typedef signed_8                        omf_dbg_style;

TYPEDEF_LOCAL_TYPE( omf_thread_fixup );

ORL_STRUCT( omf_thread_fixup ) {
    omf_idx             idx;
    unsigned char       method;
};

ORL_STRUCT( omf_tmp_lidata ) {
    omf_rec_size        size;
    omf_rec_size        used;
    unsigned_32         offset;
    bool                is32;
    omf_tmp_fixup       first_fixup;
    omf_tmp_fixup       last_fixup;
    omf_tmp_fixup       new_fixup;
};


ORL_STRUCT( omf_tmp_fixup ) {
    omf_tmp_fixup       next;
    bool                is32;
    bool                mode;
    omf_fix_loc         fix_loc;
    omf_sec_offset      offset;
    ORL_STRUCT( omf_thread_fixup ) fthread;
    ORL_STRUCT( omf_thread_fixup ) tthread;
    omf_sec_addend      disp;
};

ORL_STRUCT( omf_tmp_bakpat ) {
    omf_tmp_bkfix       first_fixup;
    omf_tmp_bkfix       last_fixup;
};

ORL_STRUCT( omf_tmp_bkfix ) {
    omf_tmp_bkfix       next;
    orl_reloc_type      reltype;
    omf_idx             segidx;
    omf_idx             symidx;
    omf_sec_offset      offset;
    omf_sec_addend      disp;
};

ORL_STRUCT( omf_handle ) {
    orl_funcs           *funcs;
    omf_file_handle     first_file_hnd;
};

ORL_STRUCT( omf_file_handle ) {
    omf_handle          omf_hnd;
    omf_file_handle     next;
    FILE                *fp;
    unsigned char       *parsebuf;
    unsigned short      parselen;
    long                status;

    omf_sec_handle      lnames;
    omf_sec_handle      extdefs;
    omf_sec_handle      relocs;
    omf_sec_handle      comments;

    ORL_STRUCT( omf_thread_fixup )   frame_thread[4];
    ORL_STRUCT( omf_thread_fixup )   target_thread[4];

    omf_sec_handle      first_sec;
    omf_sec_handle      last_sec;
    omf_quantity        next_idx;

    omf_sec_handle      *segs;
    omf_idx             num_segs;

    omf_sec_handle      *comdats;
    omf_idx             num_comdats;

    omf_grp_handle      *groups;
    omf_idx             num_groups;

    orl_machine_type    machine_type;
    orl_file_type       type;
    orl_file_size       size;
    orl_file_flags      flags;
    omf_quantity        num_sections;
    omf_sec_handle      work_sec;

    omf_tmp_lidata      lidata;
    omf_tmp_bakpat      bakpat;

    omf_dbg_style       debug_style;

    omf_sec_handle      symbol_table;
    omf_rectyp          last_rec;
};

#define SEC_NAME_HASH_TABLE_SIZE        53
#define STRING_HASH_TABLE_SIZE          53
#define STD_HASH_TABLE_SIZE             257

#define MAX_LNAMES                      256

typedef struct omf_comdat_struct {
    omf_sec_handle      assoc_seg;
    omf_grp_handle      group;
    omf_frame           frame;
} omf_comdat_struct;

struct omf_seg_assoc_struct {
    omf_idx             name;
    omf_idx             class;
    orl_sec_alignment   alignment;
    orl_sec_combine     combine;
    omf_frame           frame;
    unsigned_32         seg_flags;
    unsigned_32         cur_size;
    omf_sec_offset      cur_offset;
    omf_idx             seg_id;
    omf_symbol_handle   sym;
    omf_comdat_struct   comdat;
    omf_grp_handle      group;
    orl_linnum          lines;
    orl_table_index     num_lines;
};

struct omf_reloc_assoc_struct {
    omf_quantity        num;
    orl_reloc           *relocs;
};

struct omf_sym_assoc_struct {
    omf_quantity        num;
    omf_symbol_handle   *syms;
    orl_hash_table      hash_tab;
};

TYPEDEF_LOCAL_TYPE( omf_string );

ORL_STRUCT( omf_string ) {
    omf_string_len      len;
    char                string[1];
};

struct omf_string_assoc_struct {
    omf_idx             num;
    omf_string          *strings;
};

TYPEDEF_LOCAL_TYPE( omf_comment );

ORL_STRUCT( omf_comment ) {
    unsigned_8          class;
    unsigned_8          flags;
    omf_rec_size        len;
    unsigned char       data[1];
};

struct omf_comment_assoc_struct {
    omf_quantity        num;
    omf_comment         *comments;
};

ORL_STRUCT( omf_sec_handle ) {
    orl_file_format     file_format;
    omf_file_handle     omf_file_hnd;
    omf_sec_handle      next;
    omf_sec_size        size;
    orl_sec_type        type;
    orl_sec_flags       flags;
    omf_bytes           contents;
    orl_table_index     index;
    // assoc - things associated with the section
    union {
        struct omf_seg_assoc_struct     seg;
        struct omf_reloc_assoc_struct   reloc;
        struct omf_string_assoc_struct  string;
        struct omf_comment_assoc_struct comment;
        struct omf_sym_assoc_struct     sym;
    } assoc;
};

ORL_STRUCT( omf_symbol_handle ) {
    orl_file_format     file_format;
    omf_file_handle     omf_file_hnd;
    orl_symbol_type     typ;
    int                 rec_typ;
    omf_idx             idx;
    omf_idx             seg;
    omf_frame           frame;
    omf_sec_offset      offset;
    omf_symbol_flags    flags;
    omf_sec_handle      section;
    orl_symbol_binding          binding;
    ORL_STRUCT( omf_string )    name;
};

ORL_STRUCT( omf_grp_handle ) {
    orl_file_format     file_format;
    omf_file_handle     omf_file_hnd;
    omf_idx             name;
    omf_idx             idx;
    orl_table_index     num_segs;
    omf_idx             *segs;
    omf_symbol_handle   sym;
};

TYPEDEF_LOCAL_TYPE( omf_scan_tab );

ORL_STRUCT( omf_scan_tab ) {
    omf_idx             seg;
    omf_idx             lname;
    orl_sec_offset      start;
    orl_sec_offset      end;
};

#endif
