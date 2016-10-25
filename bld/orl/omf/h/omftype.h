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
* Description:  Structures, defines and enums for OMF files.
*
****************************************************************************/


#ifndef OMF_TYPE_INCLUDED
#define OMF_TYPE_INCLUDED

#define OMFENTRY

/* status flags
 */
#define OMF_STATUS_FILE_LOADED          0x00000001
#define OMF_STATUS_ARCH_SET             0x00000002
#define OMF_STATUS_ADD_LIDATA           0x00000008
#define OMF_STATUS_EASY_OMF             0x00000010
#define OMF_STATUS_ADD_BAKPAT           0x00000020

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

// handle definitions

typedef uint_8                          omf_file_flags;
typedef uint_32                         omf_file_index;
typedef uint_32                         omf_headers_size;

typedef uint_32                         omf_sec_size;
typedef uint_32                         omf_sec_flags;
typedef uint_32                         omf_sec_offset;

typedef uint_32                         omf_sym_flags;

typedef uint_16                         omf_reloc_type;
typedef uint_16                         omf_idx;
typedef uint_16                         omf_frame;

typedef int_32                          omf_rec_size;
typedef uint_8                          omf_string_len;

typedef int_32                          omf_quantity;

typedef uint_8                          *omf_bytes;
typedef uint_8                          omf_rectyp;
typedef int_8                           omf_dbg_style;

typedef struct omf_handle_struct        omf_handle_struct;
typedef omf_handle_struct               *omf_handle;

typedef struct omf_file_handle_struct   omf_file_handle_struct;
typedef omf_file_handle_struct          *omf_file_handle;

typedef struct omf_sec_handle_struct    omf_sec_handle_struct;
typedef omf_sec_handle_struct           *omf_sec_handle;

typedef struct omf_symbol_handle_struct omf_symbol_handle_struct;
typedef omf_symbol_handle_struct        *omf_symbol_handle;

typedef struct omf_grp_handle_struct    omf_grp_handle_struct;
typedef omf_grp_handle_struct           *omf_grp_handle;

typedef orl_reloc                       omf_reloc_handle_struct;
typedef omf_reloc_handle_struct         *omf_reloc_handle;

typedef struct omf_tmp_lidata_struct    omf_tmp_lidata_struct;
typedef omf_tmp_lidata_struct           *omf_tmp_lidata;

typedef struct omf_tmp_fixup_struct     omf_tmp_fixup_struct;
typedef omf_tmp_fixup_struct            *omf_tmp_fixup;

typedef struct omf_tmp_bakpat_struct    omf_tmp_bakpat_struct;
typedef omf_tmp_bakpat_struct           *omf_tmp_bakpat;

typedef struct omf_tmp_bkfix_struct    omf_tmp_bkfix_struct;
typedef omf_tmp_bkfix_struct           *omf_tmp_bkfix;

typedef struct omf_thred_fixup_struct   omf_thred_fixup;

struct omf_tmp_lidata_struct {
    omf_rec_size        size;
    omf_rec_size        used;
    uint_32             offset;
    int                 is32;
    omf_tmp_fixup       first_fixup;
    omf_tmp_fixup       last_fixup;
    omf_tmp_fixup       new_fixup;
};


struct omf_tmp_fixup_struct {
    omf_tmp_fixup       next;
    int                 is32;
    int                 mode;
    int                 location;
    orl_sec_offset      offset;
    int                 fmethod;
    omf_idx             fidx;
    int                 tmethod;
    omf_idx             tidx;
    orl_sec_offset      disp;
};

struct omf_tmp_bakpat_struct {
    omf_tmp_bkfix       first_fixup;
    omf_tmp_bkfix       last_fixup;
};

struct omf_tmp_bkfix_struct {
    omf_tmp_bkfix       next;
    orl_reloc_type      reltype;
    omf_idx             segidx;
    omf_idx             symidx;
    orl_sec_offset      offset;
    orl_sec_offset      disp;
};

struct omf_handle_struct {
    orl_funcs           *funcs;
    omf_file_handle     first_file_hnd;
};

struct omf_thred_fixup_struct {
    omf_idx             idx;
    unsigned char       method;
};

struct omf_file_handle_struct {
    omf_handle          omf_hnd;
    omf_file_handle     next;
    void                *file;
    unsigned char       *parsebuf;
    unsigned short      parselen;
    long                status;

    omf_sec_handle      lnames;
    omf_sec_handle      extdefs;
    omf_sec_handle      relocs;
    omf_sec_handle      comments;

    omf_thred_fixup     frame_thred[4];
    omf_thred_fixup     target_thred[4];

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
    uint_32             frame;
} omf_comdat_struct;

struct omf_seg_assoc_struct {
    omf_idx             name;
    omf_idx             class;
    orl_sec_alignment   alignment;
    orl_sec_combine     combine;
    orl_sec_frame       frame;
    uint_32             seg_flags;
    uint_32             cur_size;
    uint_32             cur_offset;
    omf_idx             seg_id;
    omf_symbol_handle   sym;
    omf_comdat_struct   comdat;
    omf_grp_handle      group;
    orl_linnum          *lines;
    orl_table_index     num_lines;
};

struct omf_reloc_assoc_struct {
    omf_quantity        num;
    omf_reloc_handle    *relocs;
};

struct omf_sym_assoc_struct {
    omf_quantity        num;
    omf_symbol_handle   *syms;
    orl_hash_table      hash_tab;
};

typedef struct omf_string_struct {
    omf_string_len      len;
    char                string[1];
} omf_string_struct;

struct omf_string_assoc_struct {
    omf_idx             num;
    omf_string_struct   **strings;
};

typedef struct omf_comment_struct {
    uint_8              class;
    uint_8              flags;
    omf_rec_size        len;
    unsigned char       data[1];
} omf_comment_struct;

struct omf_comment_assoc_struct {
    omf_quantity        num;
    omf_comment_struct  **comments;
};

struct omf_sec_handle_struct {
    orl_file_format     file_format;
    omf_file_handle     omf_file_hnd;
    omf_sec_handle      next;
    orl_sec_size        size;
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

struct omf_symbol_handle_struct {
    orl_file_format     file_format;
    omf_file_handle     omf_file_hnd;
    orl_symbol_type     typ;
    int                 rec_typ;
    omf_idx             idx;
    omf_idx             seg;
    omf_frame           frame;
    omf_sec_offset      offset;
    omf_sym_flags       flags;
    omf_sec_handle      section;
    orl_symbol_binding  binding;
    omf_string_struct   name;
};

struct omf_grp_handle_struct {
    orl_file_format     file_format;
    omf_file_handle     omf_file_hnd;
    omf_idx             name;
    omf_idx             id;
    orl_table_index     size;
    omf_idx             *segs;
    omf_symbol_handle   sym;
};

struct omf_reloc_handle_struct {
    omf_sec_handle      section;
    orl_sec_offset      locale;
    omf_symbol_handle   symbol;
    orl_reloc_type      type;
    omf_sec_offset      offset;
};

typedef struct omf_scan_tab_struct {
    omf_idx             seg;
    omf_idx             lname;
    orl_sec_offset      start;
    orl_sec_offset      end;
} omf_scan_tab_struct;

#endif
