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
* Description:  Object Reader Library public definitions.
*
****************************************************************************/


#ifndef ORL_GLOBAL_INCLUDED
#define ORL_GLOBAL_INCLUDED

#include "watcom.h"

#define ORLENTRY

#define ORL_NULL_HANDLE             NULL

#define ORL_STRUCT(s)               struct s ## _struct
#define ORL_STRUCT_SIZEOF(s)        sizeof( struct s ## _struct )
#define TYPEDEF_ORLENTRY_TYPE(t)    typedef const struct t ## _struct * t

TYPEDEF_ORLENTRY_TYPE( orl_handle );
TYPEDEF_ORLENTRY_TYPE( orl_file_handle );
TYPEDEF_ORLENTRY_TYPE( orl_sec_handle );
TYPEDEF_ORLENTRY_TYPE( orl_symbol_handle );
TYPEDEF_ORLENTRY_TYPE( orl_group_handle );

typedef unsigned_32             orl_file_offset;
typedef unsigned_32             orl_file_size;

typedef unsigned_32             orl_sec_size;
typedef unsigned_32             orl_sec_offset;
typedef signed_32               orl_sec_addend;
typedef unsigned_32             orl_sec_alignment;
typedef unsigned_32             orl_sec_frame;

typedef unsigned_64             orl_symbol_value;
typedef unsigned_64             orl_sec_base;

typedef unsigned_32             orl_table_index;

typedef unsigned_32             orl_rva;

#define ORL_SEC_NO_ABS_FRAME    ((orl_sec_frame)-1)

typedef enum {
    ORL_OUT_OF_MEMORY,
    ORL_ERROR,
    ORL_OKAY,
    ORL_TRUE = ORL_OKAY,
    ORL_FALSE
} orl_return;

typedef enum {
    ORL_ELF,
    ORL_COFF,
    ORL_OMF,
    ORL_UNRECOGNIZED_FORMAT
} orl_file_format;

#include <pushpck1.h>
TYPEDEF_ORLENTRY_TYPE( orl_linnum );

ORL_STRUCT( orl_linnum ) {
    unsigned_16 linnum;
    unsigned_32 off;
};
#include <poppck.h>

typedef struct orl_funcs {
    void        *(*cli_read)( FILE *, size_t );
    int         (*cli_seek)( FILE *, long, int );
    void        *(*cli_alloc)( size_t );
    void        (*cli_free)( void * );
} orl_funcs;

#define ORLSetFuncs( __name, __read, __seek, __alloc, __free ) \
    orl_funcs __name = { __read, __seek, __alloc, __free }

typedef enum {
    ORL_MACHINE_TYPE_NONE,      // not machine specific.
    ORL_MACHINE_TYPE_WE_2100,
    ORL_MACHINE_TYPE_SPARC,
    ORL_MACHINE_TYPE_I386,
    ORL_MACHINE_TYPE_I860,
    ORL_MACHINE_TYPE_M68000,
    ORL_MACHINE_TYPE_M88000,
    ORL_MACHINE_TYPE_ALPHA,
    ORL_MACHINE_TYPE_R3000,
    ORL_MACHINE_TYPE_R4000,
    ORL_MACHINE_TYPE_PPC601,
    ORL_MACHINE_TYPE_I8086,
    ORL_MACHINE_TYPE_AMD64,
    ORL_MACHINE_TYPE_SPARCPLUS,
    ORL_MACHINE_TYPE_UNKNOWN
} orl_machine_type;

typedef enum {
    ORL_FILE_TYPE_NONE = 0,
    ORL_FILE_TYPE_OBJECT,
    ORL_FILE_TYPE_EXECUTABLE,
    ORL_FILE_TYPE_SHARED_OBJECT,
    ORL_FILE_TYPE_DLL,
    ORL_FILE_TYPE_IMPORT,
    ORL_FILE_TYPE_CORE
} orl_file_type;

typedef enum {
    ORL_FILE_FLAG_NONE                  = 0x0000,
    ORL_FILE_FLAG_LINE_NUMS_STRIPPED    = 0x0001,// line #s removed from file
    ORL_FILE_FLAG_RELOCS_STRIPPED       = 0x0002,// relocs removed from file
    ORL_FILE_FLAG_LOCAL_SYMS_STRIPPED   = 0x0004,// local syms removed
    ORL_FILE_FLAG_DEBUG_STRIPPED        = 0x0008,// debug info removed
    ORL_FILE_FLAG_16BIT_MACHINE         = 0x0010,// for 16-bit machine
    ORL_FILE_FLAG_32BIT_MACHINE         = 0x0020,// for 32-bit machine
    ORL_FILE_FLAG_64BIT_MACHINE         = 0x0040,// for 64-bit machine
    ORL_FILE_FLAG_LITTLE_ENDIAN         = 0x0100,// LSB precedes MSB in memory
    ORL_FILE_FLAG_BIG_ENDIAN            = 0x0200,// MSB precedes LSB in memory
    ORL_FILE_FLAG_SYSTEM                = 0x1000,// system file
    ORL_FILE_CHKSUM_ERROR               = 0x2000,// check sum error
} orl_file_flags;

typedef enum {
    ORL_SEC_TYPE_NONE = 0,
    ORL_SEC_TYPE_NO_BITS,               // section has no contents in file (.bss)
    ORL_SEC_TYPE_PROG_BITS,
    ORL_SEC_TYPE_SYM_TABLE,             // section is a symbol table
    ORL_SEC_TYPE_DYN_SYM_TABLE,         // dynamic symbol table (Elf)
    ORL_SEC_TYPE_STR_TABLE,             // section is a string table
    ORL_SEC_TYPE_RELOCS,                // section contains relocs
    ORL_SEC_TYPE_RELOCS_EXPADD,         // relocs with explicit addends (Elf)
    ORL_SEC_TYPE_HASH,                  // section contains a hash table (Elf)
    ORL_SEC_TYPE_DYNAMIC,               // section contains dynamic linking info
    ORL_SEC_TYPE_NOTE,                  // section contains a note
    ORL_SEC_TYPE_IMPORT,                // section contains import data.
    ORL_SEC_TYPE_EXPORT,                // section contains export data.
} orl_sec_type;

typedef enum {
    ORL_SEC_FLAG_NONE                   = 0x00000000,
    ORL_SEC_FLAG_EXEC                   = 0x00000001, // section contains executable code
    ORL_SEC_FLAG_INITIALIZED_DATA       = 0x00000002, // section contains initialized data
    ORL_SEC_FLAG_UNINITIALIZED_DATA     = 0x00000004, // section contains uninitialized data (.bss)
    ORL_SEC_FLAG_GROUPED                = 0x00000008, // section is grouped with other sections by name
    ORL_SEC_FLAG_NO_PADDING             = 0x00000010, // section should not be aligned
    ORL_SEC_FLAG_OVERLAY                = 0x00000020, // section contains an overlay
    ORL_SEC_FLAG_REMOVE                 = 0x00000040, // section does not become part of image
    ORL_SEC_FLAG_COMDAT                 = 0x00000080, // section contains communal data
    ORL_SEC_FLAG_DISCARDABLE            = 0x00000100, // section can be discarded as needed
    ORL_SEC_FLAG_NOT_CACHED             = 0x00000200, // section cannot be cached
    ORL_SEC_FLAG_NOT_PAGEABLE           = 0x00000400, // section is not pageable
    ORL_SEC_FLAG_SHARED                 = 0x00000800, // section can be shared in memory
    ORL_SEC_FLAG_EXECUTE_PERMISSION     = 0x00001000, // section can be executed as code
    ORL_SEC_FLAG_READ_PERMISSION        = 0x00002000, // section can be read
    ORL_SEC_FLAG_WRITE_PERMISSION       = 0x00004000, // section can be written to
    ORL_SEC_FLAG_USE_16                 = 0x00008000, // section's use-16 bit
    ORL_SEC_FLAG_USE_32                 = 0x00010000, // section's use-32 bit
    ORL_SEC_FLAG_USE_64                 = 0x00020000  // section's use-64 bit
} orl_sec_flags;

typedef enum {
    ORL_SEC_COMBINE_MASK                = 0x000f, // mask for seg combinations
    ORL_SEC_COMBINE_NONE                = 0x0000, // section is not combineable
    ORL_SEC_COMBINE_PRIVATE             = 0x0001, // section not combined
    ORL_SEC_COMBINE_PUBLIC              = 0x0002, // section combined at alignable offset
    ORL_SEC_COMBINE_STACK               = 0x0003, // section combine like public on byte alignment
    ORL_SEC_COMBINE_COMMON              = 0x0004, // section combine by overlaying
    ORL_SEC_COMBINE_COMDAT              = 0x0004, // section is really a comdat
    // Used for comdats
    ORL_SEC_COMBINE_COMDAT_ALLOC_MASK   = 0x00f0, // comdat pick mask
    ORL_SEC_COMBINE_COMDAT_ALLOC_EXPLIC = 0x0000, // comdat pick mask
    ORL_SEC_COMBINE_COMDAT_ALLOC_CODE16 = 0x0010, // only one comdat allowed
    ORL_SEC_COMBINE_COMDAT_ALLOC_DATA16 = 0x0020, // any comdat allowed
    ORL_SEC_COMBINE_COMDAT_ALLOC_CODE32 = 0x0030, // same size comdat allowed
    ORL_SEC_COMBINE_COMDAT_ALLOC_DATA32 = 0x0040, // exactly same comdat allowed
    ORL_SEC_COMBINE_COMDAT_PICK_MASK    = 0x0f00, // comdat pick mask
    ORL_SEC_COMBINE_COMDAT_PICK_NONE    = 0x0000, // only one comdat allowed
    ORL_SEC_COMBINE_COMDAT_PICK_ANY     = 0x0100, // any comdat allowed
    ORL_SEC_COMBINE_COMDAT_PICK_SAME    = 0x0200, // same size comdat allowed
    ORL_SEC_COMBINE_COMDAT_PICK_EXACT   = 0x0300, // exactly same comdat allowed
} orl_sec_combine;

typedef enum {
    ORL_SYM_BINDING_NONE,
    ORL_SYM_BINDING_LOCAL,      // local symbol
    ORL_SYM_BINDING_WEAK,       // weak reference (no library search)
    ORL_SYM_BINDING_GLOBAL,     // global symbol
    ORL_SYM_BINDING_LAZY,       // lazy reference (library search done )
    ORL_SYM_BINDING_ALIAS
} orl_symbol_binding;

typedef enum {
    // a symbol should have one of these:
    ORL_SYM_TYPE_NONE                   = 0x0000,
    ORL_SYM_TYPE_OBJECT                 = 0x0001, // symbol refers to a data object
    ORL_SYM_TYPE_FUNCTION               = 0x0002, // symbol refers to a function
    ORL_SYM_TYPE_SECTION                = 0x0004, // symbol refers to a section
    ORL_SYM_TYPE_FILE                   = 0x0008, // symbol gives info on a source file
    ORL_SYM_TYPE_FUNC_INFO              = 0x0010, // symbol gives additional info on a function
    ORL_SYM_TYPE_GROUP                  = 0x0020, // symbol refers to an OMF group
    ORL_SYM_TYPE_NOTYPE                 = 0x0040, // symbol without a type
    // and at most one of these:
    ORL_SYM_TYPE_ABSOLUTE               = 0x0100, // symbol has an absolute value
    ORL_SYM_TYPE_COMMON                 = 0x0200, // symbol labels a common unallocated block (Elf)
    ORL_SYM_TYPE_UNDEFINED              = 0x0400, // symbol not assigned a section
    ORL_SYM_TYPE_DEFINED                = 0x0800, // symbol is assigned a section
    ORL_SYM_TYPE_DEBUG                  = 0x1000,  // symbol provided debug info
    // and these are meaningful if it is a comdat
    ORL_SYM_CDAT_NODUPLICATES           = 0x2000,
    ORL_SYM_CDAT_ANY                    = 0x4000,
    ORL_SYM_CDAT_SAME_SIZE              = 0x6000,
    ORL_SYM_CDAT_EXACT                  = 0x8000,
    ORL_SYM_CDAT_ASSOCIATIVE            = 0xA000,
    ORL_SYM_CDAT_MASK                   = 0xE000,
    ORL_SYM_CDAT_SHIFT                  = 13
} orl_symbol_type;

// relocation types - add new types at end of orlreloc.h if possible
typedef enum {
    #define pick(enum,text) enum,
    #include "orlreloc.h"
    #undef pick
    ORL_RELOC_TYPE_MAX          // Must be last value in this enum
} orl_reloc_type;

#endif
