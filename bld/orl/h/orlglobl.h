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
* Description:  Object Reader Library public definitions.
*
****************************************************************************/


#ifndef ORL_GLOBAL_INCLUDED
#define ORL_GLOBAL_INCLUDED

#include <watcom.h>

#define ORLENTRY

typedef uint_32                         orl_file_offset;
typedef uint_32                         orl_file_size;

typedef uint_32                         orl_sec_size;
typedef uint_32                         orl_sec_offset;
typedef uint_32                         orl_sec_alignment;
typedef long                            orl_sec_frame;

typedef uint_32                         orl_reloc_addend;

typedef uint_32                         orl_symbol_value;

typedef uint_32                         orl_table_index;

#define ORL_SEC_NO_ABS_FRAME            -1

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
typedef struct {
    unsigned_16 linnum;
    unsigned_32 off;
} orl_linnum;
#include <poppck.h>

typedef struct {
    void *      (*read)( void *, size_t );
    long int    (*seek)( void *, long int, int );
    void *      (*alloc)( size_t );
    void        (*free)( void * );
} orl_funcs;

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
    ORL_SEC_FLAG_NONE                   = 0x0000,
    ORL_SEC_FLAG_EXEC                   = 0x0001, // section contains executable code
    ORL_SEC_FLAG_INITIALIZED_DATA       = 0x0002, // section contains initialized data
    ORL_SEC_FLAG_UNINITIALIZED_DATA     = 0x0004, // section contains uninitialized data (.bss)
    ORL_SEC_FLAG_GROUPED                = 0x0008, // section is grouped with other sections by name
    ORL_SEC_FLAG_NO_PADDING             = 0x0010, // section should not be aligned
    ORL_SEC_FLAG_OVERLAY                = 0x0020, // section contains an overlay
    ORL_SEC_FLAG_REMOVE                 = 0x0040, // section does not become part of image
    ORL_SEC_FLAG_COMDAT                 = 0x0080, // section contains communal data
    ORL_SEC_FLAG_DISCARDABLE            = 0x0100, // section can be discarded as needed
    ORL_SEC_FLAG_NOT_CACHED             = 0x0200, // section cannot be cached
    ORL_SEC_FLAG_NOT_PAGEABLE           = 0x0400, // section is not pageable
    ORL_SEC_FLAG_SHARED                 = 0x0800, // section can be shared in memory
    ORL_SEC_FLAG_EXECUTE_PERMISSION     = 0x1000, // section can be executed as code
    ORL_SEC_FLAG_READ_PERMISSION        = 0x2000, // section can be read
    ORL_SEC_FLAG_WRITE_PERMISSION       = 0x4000, // section can be written to
    ORL_SEC_FLAG_USE_32                 = 0x8000  // omf section's use-32 bit
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

// relocation types - add new types at end if possible, also update objdump!
typedef enum {
    ORL_RELOC_TYPE_NONE,        // error type
    ORL_RELOC_TYPE_ABSOLUTE,    // ref to a 32-bit absolute address
    ORL_RELOC_TYPE_WORD_16,     // a direct ref to a 16-bit address
    ORL_RELOC_TYPE_WORD_32,     // a direct ref to a 32-bit address
    ORL_RELOC_TYPE_WORD_32_NB,  // a direct ref to a 32-bit address (no base added)
    ORL_RELOC_TYPE_HALF_HI,     // ref to high half of 32-bit address
    ORL_RELOC_TYPE_HALF_HA,     // ditto adjusted for signed low 16-bits.
    ORL_RELOC_TYPE_HALF_LO,     // ref to low half of 32-bit address
    ORL_RELOC_TYPE_PAIR,        // reloc connecting a HALF_HI and HALF_LO
    ORL_RELOC_TYPE_JUMP,        // ref to the part of a 32-bit address valid for jump
    ORL_RELOC_TYPE_SECTION,     // ref to an offset from a section address
    ORL_RELOC_TYPE_SEC_REL,     // direct ref to a 32-bit address relative to the image base
    ORL_RELOC_TYPE_REL_16,      // relative reference to 16-bit address
    ORL_RELOC_TYPE_REL_21_SH,   // relative ref. to a 21-bit address shifted 2
    ORL_RELOC_TYPE_WORD_64,     // NYI: direct ref to a 64-bit address
    ORL_RELOC_TYPE_SEGMENT,     // 16-bit segment relocation
    ORL_RELOC_TYPE_WORD_14,     // a direct ref to a 14-bit address shifted 2
    ORL_RELOC_TYPE_WORD_24,     // a direct ref to a 24-bit address shifted 2
    ORL_RELOC_TYPE_WORD_26,     // a direct ref to a 28-bit address shifted 2
    ORL_RELOC_TYPE_REL_14,      // relative ref to a 14-bit address shifted 2
    ORL_RELOC_TYPE_REL_24,      // relative ref to a 24-bit address shifted 2
    ORL_RELOC_TYPE_REL_32,      // relative ref to a 32-bit address
    ORL_RELOC_TYPE_REL_32_NOADJ,// as above, but doesn't need -4 adjustment

    ORL_RELOC_TYPE_TOCREL_16,   // relative ref to 16-bit offset from TOC base.
    ORL_RELOC_TYPE_TOCREL_14,   // rel. ref to 14-bit offset from TOC base shifted 2.
    ORL_RELOC_TYPE_TOCVREL_16,  // ditto, data explicitely defined in .tocd
    ORL_RELOC_TYPE_TOCVREL_14,  // ditto
    ORL_RELOC_TYPE_GOT_32,      // direct ref to 32-bit offset from GOT base.
    ORL_RELOC_TYPE_GOT_16,      // direct ref to 16-bit offset from GOT base.
    ORL_RELOC_TYPE_GOT_16_HI,   // direct ref to hi 16-bits of offset from GOT base.
    ORL_RELOC_TYPE_GOT_16_HA,   // ditto adjusted for signed low 16-bits
    ORL_RELOC_TYPE_GOT_16_LO,   // direct ref to lo 16-bits of offset from GOT base.
    ORL_RELOC_TYPE_PLTREL_24,   // rel. ref to 24-bit offset from PLT base.
    ORL_RELOC_TYPE_PLTREL_32,   // rel. ref to 32-bit offset from PLT base.
    ORL_RELOC_TYPE_PLT_32,      // direct ref to 32-bit offset from PLT base.
    ORL_RELOC_TYPE_PLT_16_HI,   // direct ref to hi 16-bits of offset from PLT base.
    ORL_RELOC_TYPE_PLT_16_HA,   // ditto adjusted for signed low 16-bits.
    ORL_RELOC_TYPE_PLT_16_LO,   // direct ref to lo 16-bits of offset from PLT base.
    ORL_RELOC_TYPE_IFGLUE,      // Substitute TOC restore instruction iff symbol is glue code
    ORL_RELOC_TYPE_IMGLUE,      // symbol is glue code; virtual address is TOC restore instruction

    // OMF specific relocations
    ORL_RELOC_TYPE_WORD_8,      // bottom byte of offset
    ORL_RELOC_TYPE_REL_8,       // relative reference bottom byte of offset
    ORL_RELOC_TYPE_WORD_16_SEG, // 16 bit offset and segment
    ORL_RELOC_TYPE_REL_16_SEG,  // relative reference 16 bit offset and segment
    ORL_RELOC_TYPE_WORD_HI_8,   // high byte of a 16 bit offset
    ORL_RELOC_TYPE_REL_HI_8,    // relative reference high byte of 16 bit offset
    ORL_RELOC_TYPE_WORD_32_SEG, // 32-bit offset and segment
    ORL_RELOC_TYPE_REL_32_SEG,  // relative reference 32 bit offset and segment

    // special relications for x64 coff files, there are only used in ndisasm yet!
    // for more information see watcom/h/coff.h in amd64 section
    ORL_RELOC_TYPE_REL_32_ADJ1, // relative ref to a 32-bit address, need special adjustment
    ORL_RELOC_TYPE_REL_32_ADJ2, // relative ref to a 32-bit address, need special adjustment
    ORL_RELOC_TYPE_REL_32_ADJ3, // relative ref to a 32-bit address, need special adjustment
    ORL_RELOC_TYPE_REL_32_ADJ4, // relative ref to a 32-bit address, need special adjustment
    ORL_RELOC_TYPE_REL_32_ADJ5, // relative ref to a 32-bit address, need special adjustment

    ORL_RELOC_TYPE_MAX          // Must be last value in this enum
} orl_reloc_type;

#endif
