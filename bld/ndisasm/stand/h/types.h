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


#ifndef WDIS_TYPES_INCLUDED
#define WDIS_TYPES_INCLUDED

#include <watcom.h>
#include "orl.h"

// label list

typedef uint_32                 label_number;
typedef uint_32                 label_id;
typedef uint_16                 list_size;
typedef uint_16                 num_errors;

typedef enum {
    LTYP_EXTERNAL_NAMED,
    LTYP_NAMED,
    LTYP_SECTION,
    LTYP_UNNAMED,
    LTYP_FUNC_INFO,
    LTYP_GROUP,
} label_type;

typedef enum {
    RFLAG_DEFAULT       = 0x0000,
    RFLAG_IS_IMMED      = 0x0001,
    RFLAG_NO_FRAME      = 0x0002
} ref_flags;

typedef struct label_entry_struct label_entry_struct;
typedef label_entry_struct * label_entry;

struct label_entry_struct {
    orl_sec_handle      shnd;
    orl_sec_offset      offset;
    label_type          type;
    orl_symbol_binding  binding;
    union {
        char *          name;
        label_number    number;
    } label;
    label_entry         next;
};

typedef struct label_list_struct label_list_struct;
typedef label_list_struct * label_list;

struct label_list_struct {
    label_entry         first;
    label_entry         last;
};

typedef struct label_list_ptr_struct label_list_ptr_struct;
typedef label_list_ptr_struct * label_list_ptr;

struct label_list_ptr_struct {
    label_list          list;
    label_list_ptr      next;
};

typedef struct publics_struct publics_struct;

struct publics_struct {
    label_list_ptr      label_lists;
    label_entry *       public_symbols;
    list_size           number;
};

// reference list

typedef struct reference_entry_struct ref_entry_struct;
typedef ref_entry_struct * ref_entry;

struct reference_entry_struct {
    label_entry         label;
    orl_sec_offset      offset;
    orl_reloc_type      type;
    orl_reloc_addend    addend;
    ref_entry           next;
    int                 no_val;
    char                *frame;
};

typedef struct ref_list_struct ref_list_struct;
typedef ref_list_struct * ref_list;

struct ref_list_struct {
    ref_entry           first;
    ref_entry           last;
    list_size           size;
};

typedef struct externs_struct externs_struct;
typedef struct externs_struct * externs;

struct externs_struct {
    ref_entry *         extern_refs;
    list_size           number;
};

// others

#define MAX_LINE_LEN 255
#define CMD_LINE_LEN 128
#define MAX_SYM_LEN  1024       // C++

#define TAB_WIDTH 8

typedef enum {
    OUT_OF_MEMORY,
    ERROR,
    OKAY
} return_val;

#include "bool.h"

typedef enum {
    SECTION_TYPE_TEXT = 0,
    SECTION_TYPE_DATA,
    SECTION_TYPE_BSS,
    SECTION_TYPE_PDATA,
    SECTION_TYPE_DRECTVE,
    SECTION_TYPE_SYM_TABLE,
    SECTION_TYPE_NUM_RECOGNIZED,
    SECTION_TYPE_LINES,
    SECTION_TYPE_UNKNOWN,
    SECTION_TYPE_RELOCS                 // used for OMF relocs section
} section_type;

typedef enum {
    NONE                = 0,
    FORM_ASM            = 0x01,
    PRINT_PUBLICS       = 0x01<<1,
    PRINT_EXTERNS       = 0x01<<2,
    NODEMANGLE_NAMES    = 0x01<<3,
    METAWARE_COMPATIBLE = 0x01<<4,
} wd_options;

typedef int_16 buffer_position;
typedef uint_16 tab_position;

typedef struct section_struct section_struct;
typedef section_struct * section_ptr;

typedef struct scantab_struct scantab_struct;
typedef scantab_struct * scantab_ptr;

struct section_struct {
    char *              name;
    orl_sec_handle      shnd;
    section_type        type;
    section_ptr         next;
    scantab_ptr         scan;
};

struct scantab_struct {
    scantab_ptr         next;
    orl_sec_offset      start;
    orl_sec_offset      end;
};

typedef struct section_list_struct section_list_struct;

struct section_list_struct {
    section_ptr         first;
    section_ptr         last;
};

typedef struct unnamed_label_return_struct unnamed_label_return_struct;
typedef unnamed_label_return_struct * unnamed_label_return;

struct unnamed_label_return_struct {
    label_entry         entry;
    return_val          error;
};

// hash table definitions
typedef uint_32         hash_value;
typedef uint_32         hash_table_size;
typedef uint_32         hash_data;

typedef enum {
    HASH_STRING,
    HASH_NUMBER
} hash_table_type;

typedef
    int         (*hash_table_comparison_func)( hash_value, hash_value );

struct hash_struct {
    hash_value                  key;
    hash_data                   data;
    struct hash_struct *        next;
};

typedef struct hash_struct hash_struct;

struct hash_table_struct {
    hash_table_size             size;
    hash_table_type             type;
    hash_table_comparison_func  compare;
    hash_struct **              table;
};

typedef struct hash_table_struct hash_table_struct;
typedef hash_table_struct * hash_table;

#endif
