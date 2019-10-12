/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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


#ifndef COFF_TYPE_INCLUDED
#define COFF_TYPE_INCLUDED

#include <coff.h>

#define COFFENTRY

TYPEDEF_LOCAL_TYPE( coff_handle );
TYPEDEF_LOCAL_TYPE( coff_file_handle );
TYPEDEF_LOCAL_TYPE( coff_sec_handle );
TYPEDEF_LOCAL_TYPE( coff_symbol_handle );

// handle definitions

typedef unsigned_8                          coff_file_flags;
typedef unsigned_32                         coff_file_index;
typedef unsigned_32                         coff_headers_size;

typedef unsigned_32                         coff_sec_flags;
typedef unsigned_32                         coff_sec_size;
typedef unsigned_32                         coff_sec_offset;

typedef unsigned_16                         coff_reloc_type;

typedef unsigned_32                         coff_symbol_table_index;
typedef unsigned_32                         coff_symbol_type;
typedef unsigned_32                         coff_symbol_value;
typedef unsigned_32                         coff_symbol_storage_class;
typedef unsigned_32                         coff_function_size;
typedef unsigned_32                         coff_symbol_characteristics;
typedef unsigned_32                         coff_comdat_checksum;
typedef unsigned_8                          coff_comdat_selection;

typedef unsigned_32                         coff_quantity;

typedef unsigned_32                         coff_rva;
typedef unsigned_32                         coff_sec_base;

ORL_STRUCT( coff_handle ) {
    orl_funcs           *funcs;
    coff_file_handle    first_file_hnd;
};

ORL_STRUCT( coff_file_handle ) {
    coff_handle         coff_hnd;
    coff_file_handle    next;
    coff_sec_handle     *coff_sec_hnd;
    coff_sec_handle     *orig_sec_hnd;
    FILE                *fp;
    coff_quantity       initial_size;
    coff_file_header    *f_hdr_buffer;
    char                *s_hdr_table_buffer;
    unsigned char       *rest_of_file_buffer;
    orl_machine_type    machine_type;
    orl_file_type       type;
    orl_file_size       size;
    orl_file_flags      flags;
    coff_quantity       num_sections;
    coff_quantity       num_symbols;
    coff_symbol_handle  symbol_handles;
    coff_sec_handle     symbol_table;
    coff_sec_handle     string_table;
    orl_hash_table      sec_name_hash_table;
    char                *implib_data;
    coff_rva            export_table_rva;
    bool                pe64;
};

#define SEC_NAME_HASH_TABLE_SIZE 53

struct coff_normal_assoc_struct {
    coff_sec_handle     reloc_sec;
};

struct coff_reloc_assoc_struct {
    coff_sec_handle     orig_sec;
    unsigned            num_relocs;
    orl_reloc           relocs;
};

ORL_STRUCT( coff_sec_handle ) {
    orl_file_format     file_format;
    coff_file_handle    coff_file_hnd;
    coff_sec_handle     next;
    char                *name;
    coff_sec_size       size;
    coff_sec_offset     offset;
    orl_sec_type        type;
    orl_sec_flags       flags;
    orl_sec_alignment   align;
    unsigned char       *contents;
    coff_section_header *hdr;
    coff_sec_base       base;
    union {
        struct coff_normal_assoc_struct     normal;
        struct coff_reloc_assoc_struct      reloc;
    } assoc;
    boolbit             name_alloced    : 1;
    boolbit             relocs_done     : 1;
};

ORL_STRUCT( coff_symbol_handle ) {
    orl_file_format     file_format;
    coff_file_handle    coff_file_hnd;
    orl_symbol_binding  binding;
    orl_symbol_type     type;
    coff_symbol ORLUNALIGNED *symbol;
    char                *name;
    boolbit             name_alloced    : 1;
    boolbit             has_bf          : 1;
};

typedef struct pe_header_struct pe_header;

struct pe_header_struct {
    char        MZ[2];
    char        space[0x3a];
    short       offset;
};

typedef union pe_opt_hdr_struct pe_opt_hdr;

union pe_opt_hdr_struct {
    unsigned_16     magic;
    struct {
        unsigned_16 magic;
        char        space[94];
        unsigned_32 export_table_rva;
    } pe32;
    struct {
        unsigned_16 magic;
        char        space[110];
        unsigned_32 export_table_rva;
    } pe64;
};

/*
// these may be useless, or they might come in handy sometime

typedef enum {
    COFF_AUX_SYMBOL_FUNCDEF,
    COFF_AUX_SYMBOL_BFEF,
    COFF_AUX_SYMBOL_WEAK_EXTERN,
    COFF_AUX_SYMBOL_FILE,
    COFF_AUX_SYMBOL_SECDEF
} coff_aux_symbol_type;

struct coff_aux_symbol_struct {
    coff_aux_symbol_type                                type;
    struct coff_aux_symbol_struct                       *next;
    union {
        struct coff_aux_symbol_func_def_struct          *func_def;
        struct coff_aux_symbol_bfef_struct              *bfef;
        struct coff_aux_symbol_weak_extern_struct       *weak_extern;
        struct coff_aux_symbol_file_struct              *file;
        struct coff_aux_symbol_sec_def_struct           *sec_def;
    };
};

struct coff_aux_symbol_func_def_struct {
    coff_symbol_table_index     bf_symbol;
    coff_function_size          function_size;
    coff_symbol_table_index     next_function;
};

struct coff_aux_symbol_bfef_struct {
    coff_line_number            line_number;
    coff_symbol_table_index     next_function;
};

struct coff_aux_symbol_weak_extern_struct {
    coff_symbol_table_index     symbol_2;
    coff_symbol_characteristics characteristics;
};

struct coff_aux_symbol_file_struct {
    char        filename[18];
};

struct coff_aux_symbol_sec_def_struct {
    coff_sec_size               size;
    coff_quantity               num_relocs;
    coff_quantity               num_line_numbers;
    coff_comdat_checksum        checksum;
    coff_sec_handle             coff_sec_hnd;
    coff_comdat_selection       selection;
};
*/

#endif
