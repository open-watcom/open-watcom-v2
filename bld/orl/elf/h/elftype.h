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


#ifndef ELF_TYPE_INCLUDED
#define ELF_TYPE_INCLUDED

#include <elfspec.h>

#define ELFENTRY

TYPEDEF_LOCAL_TYPE( elf_handle );
TYPEDEF_LOCAL_TYPE( elf_file_handle );
TYPEDEF_LOCAL_TYPE( elf_sec_handle );
TYPEDEF_LOCAL_TYPE( elf_symbol_handle );

// handle definitions

typedef unsigned_8                      elf_file_flags;
typedef unsigned_32                     elf_file_index;
typedef unsigned_64                     elf_file_offset;
typedef unsigned_64                     elf_file_size;
typedef unsigned_32                     elf_headers_size;

typedef unsigned_32                     elf_sec_flags;
typedef unsigned_64                     elf_sec_base;
typedef unsigned_64                     elf_sec_size;
typedef unsigned_32                     elf_sec_offset;
typedef signed_64                       elf_sec_addend;

typedef unsigned_64                     elf_symbol_value;

typedef unsigned_16                     elf_reloc_type;

typedef unsigned_32                     elf_quantity;

typedef unsigned_16                     elf_half;
typedef unsigned_32                     elf_word;
typedef elf_word                        elf_index;

ORL_STRUCT( elf_handle ) {
    orl_funcs           *funcs;
    elf_file_handle     first_file_hnd;
};

ORL_STRUCT( elf_file_handle ) {
    elf_handle          elf_hnd;
    elf_file_handle     next;
    elf_sec_handle      *sec_handles;
    elf_sec_handle      *orig_sec_handles;
    FILE                *fp;
    unsigned_8          *contents_buffer1;
    unsigned_8          *contents_buffer2;
    elf_half            shentsize;
    orl_machine_type    machine_type;
    orl_file_type       type;
    orl_file_size       size;
    orl_file_flags      flags;
    elf_quantity        num_sections;
    elf_sec_handle      symbol_table;
    orl_hash_table      sec_name_hash_table;
};

#define SEC_NAME_HASH_TABLE_SIZE 53

struct elf_normal_assoc_struct {
    elf_sec_handle      reloc_sec;
    char                unused[8];
};

struct elf_reloc_assoc_struct {
    elf_sec_handle      orig_sec;
    elf_sec_handle      symbol_table;
    orl_reloc           relocs;
};

struct elf_sym_assoc_struct {
    elf_sec_handle      string_table;
    elf_symbol_handle   symbols;
    char                unused[4];
};

struct elf_import_assoc_struct {
    elf_sec_handle      string_table;
    char                unused[8];
};

struct elf_export_assoc_struct {
    elf_sec_handle      symbol_table;
    char                unused[8];
};

ORL_STRUCT( elf_sec_handle ) {
    orl_file_format     file_format;
    elf_file_handle     elf_file_hnd;
    elf_sec_handle      next;
    char                *name;
    elf_sec_size        size;
    elf_file_offset     file_offset;
    orl_sec_type        type;
    orl_sec_flags       flags;
    orl_sec_alignment   alignment;
    unsigned_8          *contents;
    elf_sec_base        base;
    orl_table_index     index;
//    elf_quantity        index;
    elf_sec_size        entsize;
    // assoc - things associated with the section
    union {
        struct elf_normal_assoc_struct  normal;
        struct elf_reloc_assoc_struct   reloc;
        struct elf_sym_assoc_struct     sym;
        struct elf_import_assoc_struct  import;
        struct elf_export_assoc_struct  export;
    } assoc;
};

ORL_STRUCT( elf_symbol_handle ) {
    orl_file_format     file_format;
    elf_file_handle     elf_file_hnd;
    orl_symbol_binding  binding;
    orl_symbol_type     type;
    char                *name;
    elf_symbol_value    value;
    elf_half            shndx;
    unsigned_8          info;
};

#endif
