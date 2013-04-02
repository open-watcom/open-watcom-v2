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

// handle definitions

typedef uint_8          elf_file_flags;
typedef uint_32         elf_file_index;
typedef uint_32         elf_headers_size;

typedef uint_32         elf_sec_size;
typedef uint_32         elf_sec_flags;
typedef uint_32         elf_sec_offset;

typedef uint_16         elf_reloc_type;

typedef int_32          elf_quantity;

typedef struct elf_handle_struct elf_handle_struct;
typedef elf_handle_struct * elf_handle;

typedef struct elf_file_handle_struct elf_file_handle_struct;
typedef elf_file_handle_struct * elf_file_handle;

typedef struct elf_sec_handle_struct elf_sec_handle_struct;
typedef elf_sec_handle_struct * elf_sec_handle;

typedef struct elf_symbol_handle_struct elf_symbol_handle_struct;
typedef elf_symbol_handle_struct * elf_symbol_handle;

struct elf_handle_struct {
    orl_funcs *         funcs;
    elf_file_handle     first_file_hnd;
};

struct elf_file_handle_struct {
    elf_handle          elf_hnd;
    elf_file_handle     next;
    elf_sec_handle      *elf_sec_hnd;
    elf_sec_handle      *orig_sec_hnd;
    void                *file;
    unsigned char       *contents_buffer1;
    unsigned char       *contents_buffer2;
    uint16_t            shentsize;
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
    orl_reloc *         relocs;
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

struct elf_sec_handle_struct {
    orl_file_format     file_format;
    elf_file_handle     elf_file_hnd;
    elf_sec_handle      next;
    char                *name;
    orl_sec_size        size;
    orl_file_offset     offset;
    orl_sec_type        type;
    orl_sec_flags       flags;
    orl_sec_alignment   alignment;
    unsigned char       *contents;
    orl_sec_offset      base;
    orl_table_index     index;
//    elf_quantity        index;
    orl_sec_size        entsize;
    // assoc - things associated with the section
    union {
        struct elf_normal_assoc_struct  normal;
        struct elf_reloc_assoc_struct   reloc;
        struct elf_sym_assoc_struct     sym;
        struct elf_import_assoc_struct  import;
        struct elf_export_assoc_struct  export;
    } assoc;
};

struct elf_symbol_handle_struct {
    orl_file_format     file_format;
    elf_file_handle     elf_file_hnd;
    orl_symbol_binding  binding;
    orl_symbol_type     type;
    char                *name;
    orl_symbol_value    value;
    uint16_t            shndx;
    uint8_t             info;
};

#endif
