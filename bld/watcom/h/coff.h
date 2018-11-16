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
* Description:  COFF definitions.
*
****************************************************************************/


#include "watcom.h"

#include "pushpck1.h"
typedef struct {
    uint_16     cpu_type;
    uint_16     num_sections;
    uint_32     time_stamp;
    uint_32     sym_table;
    uint_32     num_symbols;
    uint_16     opt_hdr_size;
    uint_16     flags;
} coff_file_header;

#define COFF_FILE_HEADER_SIZE sizeof(coff_file_header)

#define COFF_SEC_NAME_LEN 8

typedef struct {
    char        name[COFF_SEC_NAME_LEN];
    uint_32     virtsize;
    uint_32     offset;
    uint_32     size;
    uint_32     rawdata_ptr;
    uint_32     reloc_ptr;
    uint_32     lineno_ptr;
    uint_16     num_relocs;
    uint_16     num_lineno;
    uint_32     flags;
} coff_section_header;

#define COFF_SECTION_HEADER_SIZE sizeof(coff_section_header)

typedef struct {
    uint_32     offset;
    uint_32     sym_tab_index;
    uint_16     type;
} coff_reloc;

#define COFF_RELOC_SIZE sizeof(coff_reloc)

#define COFF_SYM_NAME_LEN 8

// typedef struct _IMAGE_SYMBOL in WINNT.H

typedef struct {
    union {
        char            name_string[COFF_SYM_NAME_LEN];
        struct {
            uint_32     zeros;
            uint_32     offset;
        } non_name;
    } name;
    uint_32             value;
    signed_16           sec_num;
    uint_16             type;
    uint_8              storage_class;
    uint_8              num_aux;
} coff_symbol;

#define COFF_SYM_SIZE sizeof(coff_symbol)

#define _CoffSymType( complex, simple )         ( ( (complex) << 4 ) | (simple) )
#define _CoffBaseType( sym_type )               ( (sym_type) & 0xf )
#define _CoffComplexType( sym_type )            ( ( (sym_type) >> 4 ) & 0xf )

typedef struct {
    uint_32     bf;
    uint_32     size;
    uint_32     linenum;
    uint_32     next_func;
    char        unused[3];
} coff_sym_func;

typedef struct {
    char        unused1[4];
    uint_16     linenum;
    char        unused2[6];
    uint_32     next_func;
    char        unused3[2];
} coff_sym_bfef;

typedef struct {
    uint_32     tag_index;
    uint_32     characteristics;
    char        unused1[10];
} coff_sym_weak;

#define COFF_FILE_NAME_LEN 18

typedef struct {
    char        filename[COFF_FILE_NAME_LEN];
} coff_sym_file;

typedef struct {
    uint_32     length;
    uint_16     num_relocs;
    uint_16     num_line_numbers;
    uint_32     checksum;
    uint_16     number;
    uint_8      selection;
    char        unused[3];
} coff_sym_section;

typedef struct {
    union {
        uint_32         symbol_table_index;
        uint_32         RVA;
    } ir;
    uint_16             line_number;
} coff_line_num;

/* from the COFF/PE docs */

// CPU types
enum {
    COFF_IMAGE_FILE_MACHINE_UNKNOWN          = 0,
    COFF_IMAGE_FILE_MACHINE_I386             = 0x014c, // Intel 386 (Sys V).
    COFF_IMAGE_FILE_MACHINE_I860             = 0x014d, // Intel 860.
    COFF_IMAGE_FILE_MACHINE_R3000            = 0x0162, // MIPS little-endian, 0x160 big-endian
    COFF_IMAGE_FILE_MACHINE_R4000            = 0x0166, // MIPS little-endian
    COFF_IMAGE_FILE_MACHINE_R10000           = 0x0168, // MIPS little-endian
    COFF_IMAGE_FILE_MACHINE_WCEMIPSV2        = 0x0169, // MIPS little-endian WCE v2
    COFF_IMAGE_FILE_MACHINE_I386A            = 0x0175, // Intel 386 (AIX).
    COFF_IMAGE_FILE_MACHINE_ALPHA            = 0x0184, // Alpha_AXP
    COFF_IMAGE_FILE_MACHINE_POWERPC          = 0x01F0, // IBM PowerPC Little-Endian
    COFF_IMAGE_FILE_MACHINE_SH3              = 0x01a2, // SH3 little-endian
    COFF_IMAGE_FILE_MACHINE_SH3E             = 0x01a4, // SH3E little-endian
    COFF_IMAGE_FILE_MACHINE_SH4              = 0x01a6, // SH4 little-endian
    COFF_IMAGE_FILE_MACHINE_ARM              = 0x01c0, // ARM Little-Endian
    COFF_IMAGE_FILE_MACHINE_THUMB            = 0x01c2,
    COFF_IMAGE_FILE_MACHINE_IA64             = 0x0200, // Intel 64
    COFF_IMAGE_FILE_MACHINE_MIPS16           = 0x0266, // MIPS
    COFF_IMAGE_FILE_MACHINE_MIPSFPU          = 0x0366, // MIPS
    COFF_IMAGE_FILE_MACHINE_MIPSFPU16        = 0x0466, // MIPS
    COFF_IMAGE_FILE_MACHINE_ALPHA64          = 0x0284, // ALPHA64
    COFF_IMAGE_FILE_MACHINE_AXP64            = COFF_IMAGE_FILE_MACHINE_ALPHA64,
    COFF_IMAGE_FILE_MACHINE_AMD64            = 0x8664  // AMD64 / Intel EM64T
};

// file flag values
enum {
    COFF_IMAGE_FILE_RELOCS_STRIPPED          = 0x0001, // Relocation info stripped from file.
    COFF_IMAGE_FILE_EXECUTABLE_IMAGE         = 0x0002, // File is executable  (i.e. no unresolved externel references).
    COFF_IMAGE_FILE_LINE_NUMS_STRIPPED       = 0x0004, // Line nunbers stripped from file.
    COFF_IMAGE_FILE_LOCAL_SYMS_STRIPPED      = 0x0008, // Local symbols stripped from file.
    COFF_IMAGE_FILE_AGGRESIVE_WS_TRIM        = 0x0010, // Agressively trim working set
    COFF_IMAGE_FILE_LARGE_ADDRESS_AWARE      = 0x0020, // App can handle >2gb addresses
    COFF_IMAGE_FILE_16BIT_MACHINE            = 0x0040,
    COFF_IMAGE_FILE_BYTES_REVERSED_LO        = 0x0080, // Bytes of machine word are reversed.
    COFF_IMAGE_FILE_32BIT_MACHINE            = 0x0100, // 32 bit word machine.
    COFF_IMAGE_FILE_DEBUG_STRIPPED           = 0x0200, // Debugging info stripped from file in .DBG file
    COFF_IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP  = 0x0400, // If Image is on removable media, copy and run from the swap file.
    COFF_IMAGE_FILE_NET_RUN_FROM_SWAP        = 0x0800, // If Image is on Net, copy and run from the swap file.
    COFF_IMAGE_FILE_SYSTEM                   = 0x1000, // System File.
    COFF_IMAGE_FILE_DLL                      = 0x2000, // File is a DLL.
    COFF_IMAGE_FILE_UP_SYSTEM_ONLY           = 0x4000, // File should only be run on a UP machine
    COFF_IMAGE_FILE_BYTES_REVERSED_HI        = 0x8000  // Bytes of machine word are reversed.
};

// section flag values
enum {
//  COFF_IMAGE_SCN_TYPE_REG                  = 0x00000000, // Reserved.
//  COFF_IMAGE_SCN_TYPE_DSECT                = 0x00000001, // Reserved.
//  COFF_IMAGE_SCN_TYPE_NOLOAD               = 0x00000002, // Reserved.
//  COFF_IMAGE_SCN_TYPE_GROUP                = 0x00000004, // Reserved.
    COFF_IMAGE_SCN_TYPE_NO_PAD               = 0x00000008, // Reserved.
//  COFF_IMAGE_SCN_TYPE_COPY                 = 0x00000010, // Reserved.

    COFF_IMAGE_SCN_CNT_CODE                  = 0x00000020, // Section contains code.
    COFF_IMAGE_SCN_CNT_INITIALIZED_DATA      = 0x00000040, // Section contains initialized data.
    COFF_IMAGE_SCN_CNT_UNINITIALIZED_DATA    = 0x00000080, // Section contains uninitialized data.

    COFF_IMAGE_SCN_LNK_OTHER                 = 0x00000100, // Reserved.
    COFF_IMAGE_SCN_LNK_INFO                  = 0x00000200, // Section contains comments or some other type of information.
//  COFF_IMAGE_SCN_TYPE_OVER                 = 0x00000400, // Reserved.
    COFF_IMAGE_SCN_LNK_REMOVE                = 0x00000800, // Section contents will not become part of image.
    COFF_IMAGE_SCN_LNK_COMDAT                = 0x00001000, // Section contents comdat.
//                                           = 0x00002000, // Reserved.
//  COFF_IMAGE_SCN_MEM_PROTECTED - Obsolete  = 0x00004000,
    COFF_IMAGE_SCN_NO_DEFER_SPEC_EXC         = 0x00004000, // Reset speculative exceptions handling bits in the TLB entries for this section.
    COFF_IMAGE_SCN_GPREL                     = 0x00008000, // Section content can be accessed relative to GP
    COFF_IMAGE_SCN_MEM_FARDATA               = 0x00008000,
//  COFF_IMAGE_SCN_MEM_SYSHEAP  - Obsolete   = 0x00010000,
    COFF_IMAGE_SCN_MEM_PURGEABLE             = 0x00020000,
    COFF_IMAGE_SCN_MEM_16BIT                 = 0x00020000,
    COFF_IMAGE_SCN_MEM_LOCKED                = 0x00040000,
    COFF_IMAGE_SCN_MEM_PRELOAD               = 0x00080000,

    COFF_IMAGE_SCN_ALIGN_1BYTES              = 0x00100000, //
    COFF_IMAGE_SCN_ALIGN_2BYTES              = 0x00200000, //
    COFF_IMAGE_SCN_ALIGN_4BYTES              = 0x00300000, //
    COFF_IMAGE_SCN_ALIGN_8BYTES              = 0x00400000, //
    COFF_IMAGE_SCN_ALIGN_16BYTES             = 0x00500000, // Default alignment if no others are specified.
    COFF_IMAGE_SCN_ALIGN_32BYTES             = 0x00600000, //
    COFF_IMAGE_SCN_ALIGN_64BYTES             = 0x00700000, //
    COFF_IMAGE_SCN_ALIGN_128BYTES            = 0x00800000, //
    COFF_IMAGE_SCN_ALIGN_256BYTES            = 0x00900000, //
    COFF_IMAGE_SCN_ALIGN_512BYTES            = 0x00A00000, //
    COFF_IMAGE_SCN_ALIGN_1024BYTES           = 0x00B00000, //
    COFF_IMAGE_SCN_ALIGN_2048BYTES           = 0x00C00000, //
    COFF_IMAGE_SCN_ALIGN_4096BYTES           = 0x00D00000, //
    COFF_IMAGE_SCN_ALIGN_8192BYTES           = 0x00E00000, //
    COFF_IMAGE_SCN_ALIGN_MASK                = 0x00F00000,

    COFF_IMAGE_SCN_LNK_NRELOC_OVFL           = 0x01000000, // Section contains extended relocations.
    COFF_IMAGE_SCN_MEM_DISCARDABLE           = 0x02000000, // Section can be discarded.
    COFF_IMAGE_SCN_MEM_NOT_CACHED            = 0x04000000, // Section is not cachable.
    COFF_IMAGE_SCN_MEM_NOT_PAGED             = 0x08000000, // Section is not pageable.
    COFF_IMAGE_SCN_MEM_SHARED                = 0x10000000, // Section is shareable.
    COFF_IMAGE_SCN_MEM_EXECUTE               = 0x20000000, // Section is executable.
    COFF_IMAGE_SCN_MEM_READ                  = 0x40000000, // Section is readable.
    COFF_IMAGE_SCN_MEM_WRITE                 = 0x80000000, // Section is writeable.

    COFF_IMAGE_SCN_ALIGN_SHIFT               = 20
};

//
// Section values.
//
// Symbols have a section number of the section in which they are
// defined. Otherwise, section numbers have the following meanings:
//
enum {
    COFF_IMAGE_SYM_UNDEFINED         = (signed_16)0,  // Symbol is undefined or is common.
    COFF_IMAGE_SYM_ABSOLUTE          = (signed_16)-1, // Symbol is an absolute value.
    COFF_IMAGE_SYM_DEBUG             = (signed_16)-2  // Symbol is a special debug item.
};

//
// Type (fundamental) values.
//
enum {
    COFF_IMAGE_SYM_TYPE_NULL         = 0x0000, // no type.
    COFF_IMAGE_SYM_TYPE_VOID         = 0x0001, //
    COFF_IMAGE_SYM_TYPE_CHAR         = 0x0002, // type character.
    COFF_IMAGE_SYM_TYPE_SHORT        = 0x0003, // type short integer.
    COFF_IMAGE_SYM_TYPE_INT          = 0x0004, //
    COFF_IMAGE_SYM_TYPE_LONG         = 0x0005, //
    COFF_IMAGE_SYM_TYPE_FLOAT        = 0x0006, //
    COFF_IMAGE_SYM_TYPE_DOUBLE       = 0x0007, //
    COFF_IMAGE_SYM_TYPE_STRUCT       = 0x0008, //
    COFF_IMAGE_SYM_TYPE_UNION        = 0x0009, //
    COFF_IMAGE_SYM_TYPE_ENUM         = 0x000A, // enumeration.
    COFF_IMAGE_SYM_TYPE_MOE          = 0x000B, // member of enumeration.
    COFF_IMAGE_SYM_TYPE_BYTE         = 0x000C, //
    COFF_IMAGE_SYM_TYPE_WORD         = 0x000D, //
    COFF_IMAGE_SYM_TYPE_UINT         = 0x000E, //
    COFF_IMAGE_SYM_TYPE_DWORD        = 0x000F, //
    COFF_IMAGE_SYM_TYPE_PCODE        = 0x8000  //
};

//
// Type (derived) values.
//
enum {
    COFF_IMAGE_SYM_DTYPE_NULL        = 0, // no derived type.
    COFF_IMAGE_SYM_DTYPE_POINTER     = 1, // pointer.
    COFF_IMAGE_SYM_DTYPE_FUNCTION    = 2, // function.
    COFF_IMAGE_SYM_DTYPE_ARRAY       = 3  // array.
};

//
// Storage classes.
//
typedef enum {
    COFF_IMAGE_SYM_CLASS_END_OF_FUNCTION     = -1,
    COFF_IMAGE_SYM_CLASS_NULL                = 0x0000,
    COFF_IMAGE_SYM_CLASS_AUTOMATIC           = 0x0001,
    COFF_IMAGE_SYM_CLASS_EXTERNAL            = 0x0002,
    COFF_IMAGE_SYM_CLASS_STATIC              = 0x0003,
    COFF_IMAGE_SYM_CLASS_REGISTER            = 0x0004,
    COFF_IMAGE_SYM_CLASS_EXTERNAL_DEF        = 0x0005,
    COFF_IMAGE_SYM_CLASS_LABEL               = 0x0006,
    COFF_IMAGE_SYM_CLASS_UNDEFINED_LABEL     = 0x0007,
    COFF_IMAGE_SYM_CLASS_MEMBER_OF_STRUCT    = 0x0008,
    COFF_IMAGE_SYM_CLASS_ARGUMENT            = 0x0009,
    COFF_IMAGE_SYM_CLASS_STRUCT_TAG          = 0x000A,
    COFF_IMAGE_SYM_CLASS_MEMBER_OF_UNION     = 0x000B,
    COFF_IMAGE_SYM_CLASS_UNION_TAG           = 0x000C,
    COFF_IMAGE_SYM_CLASS_TYPE_DEFINITION     = 0x000D,
    COFF_IMAGE_SYM_CLASS_UNDEFINED_STATIC    = 0x000E,
    COFF_IMAGE_SYM_CLASS_ENUM_TAG            = 0x000F,
    COFF_IMAGE_SYM_CLASS_MEMBER_OF_ENUM      = 0x0010,
    COFF_IMAGE_SYM_CLASS_REGISTER_PARAM      = 0x0011,
    COFF_IMAGE_SYM_CLASS_BIT_FIELD           = 0x0012,

    COFF_IMAGE_SYM_CLASS_FAR_EXTERNAL        = 0x0044, //

    COFF_IMAGE_SYM_CLASS_BLOCK               = 0x0064,
    COFF_IMAGE_SYM_CLASS_FUNCTION            = 0x0065,
    COFF_IMAGE_SYM_CLASS_END_OF_STRUCT       = 0x0066,
    COFF_IMAGE_SYM_CLASS_FILE                = 0x0067,
// new
    COFF_IMAGE_SYM_CLASS_SECTION             = 0x0068,
    COFF_IMAGE_SYM_CLASS_WEAK_EXTERNAL       = 0x0069
}image_sym_class;

//
// Communal selection types.
//

typedef enum {
    COFF_IMAGE_COMDAT_SELECT_UNKNOWN         = 0,
    COFF_IMAGE_COMDAT_SELECT_NODUPLICATES    = 1,
    COFF_IMAGE_COMDAT_SELECT_ANY             = 2,
    COFF_IMAGE_COMDAT_SELECT_SAME_SIZE       = 3,
    COFF_IMAGE_COMDAT_SELECT_EXACT_MATCH     = 4,
    COFF_IMAGE_COMDAT_SELECT_ASSOCIATIVE     = 5,
    COFF_IMAGE_COMDAT_SELECT_LARGEST         = 6,
    COFF_IMAGE_COMDAT_SELECT_NEWEST          = 7
}image_comdat_select;

// weak extern types

enum {
    COFF_IMAGE_WEAK_EXTERN_SEARCH_NOLIBRARY  = 1,
    COFF_IMAGE_WEAK_EXTERN_SEARCH_LIBRARY    = 2,
    COFF_IMAGE_WEAK_EXTERN_SEARCH_ALIAS      = 3
};

// Following comes from winnt.h

//
// I386 relocation types.
//

#define COFF_IMAGE_REL_I386_ABSOLUTE 0       // Reference is absolute, no relocation is necessary
#define COFF_IMAGE_REL_I386_DIR16    1       // Direct 16-bit reference to the symbols virtual address
#define COFF_IMAGE_REL_I386_REL16    2       // PC-relative 16-bit reference to the symbols virtual address
#define COFF_IMAGE_REL_I386_DIR32    6       // Direct 32-bit reference to the symbols virtual address
#define COFF_IMAGE_REL_I386_DIR32NB  7       // Direct 32-bit reference to the symbols virtual address, base not included
#define COFF_IMAGE_REL_I386_SEG12    9       // Direct 16-bit reference to the segment-selector bits of a 32-bit virtual address
#define COFF_IMAGE_REL_I386_SECTION  10
#define COFF_IMAGE_REL_I386_SECREL   11
#define COFF_IMAGE_REL_I386_REL32    20      // PC-relative 32-bit reference to the symbols virtual address

//
// MIPS relocation types.
//

#define COFF_IMAGE_REL_MIPS_ABSOLUTE         0x0000  // Reference is absolute, no relocation is necessary
#define COFF_IMAGE_REL_MIPS_REFHALF          0x0001
#define COFF_IMAGE_REL_MIPS_REFWORD          0x0002
#define COFF_IMAGE_REL_MIPS_JMPADDR          0x0003
#define COFF_IMAGE_REL_MIPS_REFHI            0x0004
#define COFF_IMAGE_REL_MIPS_REFLO            0x0005
#define COFF_IMAGE_REL_MIPS_GPREL            0x0006
#define COFF_IMAGE_REL_MIPS_LITERAL          0x0007
#define COFF_IMAGE_REL_MIPS_SECTION          0x000A
#define COFF_IMAGE_REL_MIPS_SECREL           0x000B
#define COFF_IMAGE_REL_MIPS_SECRELLO         0x000C  // Low 16-bit section relative referemce (used for >32k TLS)
#define COFF_IMAGE_REL_MIPS_SECRELHI         0x000D  // High 16-bit section relative reference (used for >32k TLS)
#define COFF_IMAGE_REL_MIPS_JMPADDR16        0x0010
#define COFF_IMAGE_REL_MIPS_REFWORDNB        0x0022
#define COFF_IMAGE_REL_MIPS_PAIR             0x0025

//
// Alpha Relocation types.
//

#define COFF_IMAGE_REL_ALPHA_ABSOLUTE        0x0000
#define COFF_IMAGE_REL_ALPHA_REFLONG         0x0001
#define COFF_IMAGE_REL_ALPHA_REFQUAD         0x0002
#define COFF_IMAGE_REL_ALPHA_GPREL32         0x0003
#define COFF_IMAGE_REL_ALPHA_LITERAL         0x0004
#define COFF_IMAGE_REL_ALPHA_LITUSE          0x0005
#define COFF_IMAGE_REL_ALPHA_GPDISP          0x0006
#define COFF_IMAGE_REL_ALPHA_BRADDR          0x0007
#define COFF_IMAGE_REL_ALPHA_HINT            0x0008
#define COFF_IMAGE_REL_ALPHA_INLINE_REFLONG  0x0009
#define COFF_IMAGE_REL_ALPHA_REFHI           0x000A
#define COFF_IMAGE_REL_ALPHA_REFLO           0x000B
#define COFF_IMAGE_REL_ALPHA_PAIR            0x000C
#define COFF_IMAGE_REL_ALPHA_MATCH           0x000D
#define COFF_IMAGE_REL_ALPHA_SECTION         0x000E
#define COFF_IMAGE_REL_ALPHA_SECREL          0x000F
#define COFF_IMAGE_REL_ALPHA_REFLONGNB       0x0010
#define COFF_IMAGE_REL_ALPHA_SECRELLO        0x0011  // Low 16-bit section relative reference
#define COFF_IMAGE_REL_ALPHA_SECRELHI        0x0012  // High 16-bit section relative reference
#define COFF_IMAGE_REL_ALPHA_REFQ3           0x0013  // High 16 bits of 48 bit reference
#define COFF_IMAGE_REL_ALPHA_REFQ2           0x0014  // Middle 16 bits of 48 bit reference
#define COFF_IMAGE_REL_ALPHA_REFQ1           0x0015  // Low 16 bits of 48 bit reference
#define COFF_IMAGE_REL_ALPHA_GPRELLO         0x0016  // Low 16-bit GP relative reference
#define COFF_IMAGE_REL_ALPHA_GPRELHI         0x0017  // High 16-bit GP relative reference

//
// PowerPC relocation types.
//

#define COFF_IMAGE_REL_PPC_ABSOLUTE          0x0000  // NOP
#define COFF_IMAGE_REL_PPC_ADDR64            0x0001  // 64-bit address
#define COFF_IMAGE_REL_PPC_ADDR32            0x0002  // 32-bit address
#define COFF_IMAGE_REL_PPC_ADDR24            0x0003  // 26-bit address, shifted left 2 (branch absolute)
#define COFF_IMAGE_REL_PPC_ADDR16            0x0004  // 16-bit address
#define COFF_IMAGE_REL_PPC_ADDR14            0x0005  // 16-bit address, shifted left 2 (load doubleword)
#define COFF_IMAGE_REL_PPC_REL24             0x0006  // 26-bit PC-relative offset, shifted left 2 (branch relative)
#define COFF_IMAGE_REL_PPC_REL14             0x0007  // 16-bit PC-relative offset, shifted left 2 (br cond relative)
#define COFF_IMAGE_REL_PPC_TOCREL16          0x0008  // 16-bit offset from TOC base
#define COFF_IMAGE_REL_PPC_TOCREL14          0x0009  // 16-bit offset from TOC base, shifted left 2 (load doubleword)

#define COFF_IMAGE_REL_PPC_ADDR32NB          0x000A  // 32-bit addr w/o image base
#define COFF_IMAGE_REL_PPC_SECREL            0x000B  // va of containing section (as in an image sectionhdr)
#define COFF_IMAGE_REL_PPC_SECTION           0x000C  // sectionheader number
#define COFF_IMAGE_REL_PPC_IFGLUE            0x000D  // substitute TOC restore instruction iff symbol is glue code
#define COFF_IMAGE_REL_PPC_IMGLUE            0x000E  // symbol is glue code; virtual address is TOC restore instruction
#define COFF_IMAGE_REL_PPC_SECREL16          0x000F  // va of containing section (limited to 16 bits)
#define COFF_IMAGE_REL_PPC_REFHI             0x0010
#define COFF_IMAGE_REL_PPC_REFLO             0x0011
#define COFF_IMAGE_REL_PPC_PAIR              0x0012
#define COFF_IMAGE_REL_PPC_SECRELLO          0x0013  // Low 16-bit section relative reference (used for >32k TLS)
#define COFF_IMAGE_REL_PPC_SECRELHI          0x0014  // High 16-bit section relative reference (used for >32k TLS)
#define COFF_IMAGE_REL_PPC_GPREL             0x0015

#define COFF_IMAGE_REL_PPC_TYPEMASK          0x00FF  // mask to isolate above values in IMAGE_RELOCATION.Type

// Flag bits in IMAGE_RELOCATION.TYPE

#define COFF_IMAGE_REL_PPC_NEG               0x0100  // subtract reloc value rather than adding it
#define COFF_IMAGE_REL_PPC_BRTAKEN           0x0200  // fix branch prediction bit to predict branch taken
#define COFF_IMAGE_REL_PPC_BRNTAKEN          0x0400  // fix branch prediction bit to predict branch not taken
#define COFF_IMAGE_REL_PPC_TOCDEFN           0x0800  // toc slot defined in file (or, data in toc)

//
// AMD64 (X86-64) relocations
//
#define COFF_IMAGE_REL_AMD64_ABSOLUTE        0x0000  // Reference is absolute, no relocation is necessary
#define COFF_IMAGE_REL_AMD64_ADDR64          0x0001  // 64-bit address
#define COFF_IMAGE_REL_AMD64_ADDR32          0x0002  // 32-bit address
#define COFF_IMAGE_REL_AMD64_ADDR32NB        0x0003  // 32-bit address reference to the virtual address, base not included
#define COFF_IMAGE_REL_AMD64_REL32           0x0004  // PC-relative 32-bit reference to the symbols virtual address (0 byte distance to target)
#define COFF_IMAGE_REL_AMD64_REL32_1         0x0005  // PC-relative 32-bit reference to the symbols virtual address (1 byte distance to target)
#define COFF_IMAGE_REL_AMD64_REL32_2         0x0006  // PC-relative 32-bit reference to the symbols virtual address (2 byte distance to target)
#define COFF_IMAGE_REL_AMD64_REL32_3         0x0007  // PC-relative 32-bit reference to the symbols virtual address (3 byte distance to target)
#define COFF_IMAGE_REL_AMD64_REL32_4         0x0008  // PC-relative 32-bit reference to the symbols virtual address (4 byte distance to target)
#define COFF_IMAGE_REL_AMD64_REL32_5         0x0009  // PC-relative 32-bit reference to the symbols virtual address (5 byte distance to target)
#define COFF_IMAGE_REL_AMD64_SECTION         0x000A  // va of containing section (size unknown yet; I think its 32-bit)
#define COFF_IMAGE_REL_AMD64_SECREL          0x000B  // 32-bit section relative reference
#define COFF_IMAGE_REL_AMD64_SECREL7         0x000C  // 7-bit section relative reference
//
// I think that I've figured out for what these REL32_x relocations are.
// following is a simple asm program to demonstate the behavoir:
//
// asdf:
// ; ex for COFF_IMAGE_REL_AMD64_REL32
// ;          vvvvvvvvvvv <- distance: 0 (to the end)
// ; 44 12 05 00 00 00 00
// adc r8b, byte ptr [asdf]
//
// ; ex for COFF_IMAGE_REL_AMD64_REL32_1
// ;       vvvvvvvvvvv <- distance: 1 (to the end)
// ; 83 15 00 00 00 00 12
// adc    dword ptr asdf, 12h
//
// ; ex for COFF_IMAGE_REL_AMD64_REL32_4
// ;          vvvvvvvvvvv  <- distance: 4 (to the end)
// ; 48 81 15 00 00 00 00 78 56 34 12
// adc qword ptr [asdf], 12345678h
//


typedef struct {
    uint_32     rva;
    uint_32     size;
} coff_image_data_directory;

typedef struct {
    uint_16     magic;                  //standard fields
    uint_8      l_major;
    uint_8      l_minor;
    uint_32     code_size;
    uint_32     init_data;
    uint_32     uninit_data;
    uint_32     entry_point;
    uint_32     base_code;
    uint_32     base_data;
    uint_32     image_base;             //nt specific fields
    uint_32     section_align;
    uint_32     file_align;
    uint_16     os_major;
    uint_16     os_minor;
    uint_16     user_major;
    uint_16     user_minor;
    uint_16     subsys_major;
    uint_16     subsys_minor;
    uint_32     reserved1;
    uint_32     image_size;
    uint_32     header_size;
    uint_32     file_checksum;
    uint_16     sub_system;
    uint_16     dll_flags;
    uint_32     stack_reserve;
    uint_32     stack_commit;
    uint_32     heap_reserve;
    uint_32     heap_commit;
    uint_32     loader_flags;
    uint_32     data_directories;
    coff_image_data_directory   export_table;
    coff_image_data_directory   import_table;
    coff_image_data_directory   resource_table;
    coff_image_data_directory   exception_table;
    coff_image_data_directory   security_table;
    coff_image_data_directory   base_reloc_table;
    coff_image_data_directory   debug;
    coff_image_data_directory   copyright;
    coff_image_data_directory   machine_value;
    coff_image_data_directory   tls_table;
    coff_image_data_directory   load_config_table;
    uint_8      reserved2[40];
} coff_opt_hdr;

#define COFF_OPT_HDR_SIZE sizeof( coff_opt_hdr )

typedef struct {
    uint_16     magic;                  //standard fields
    uint_8      l_major;
    uint_8      l_minor;
    uint_32     code_size;
    uint_32     init_data;
    uint_32     uninit_data;
    uint_32     entry_point;
    uint_32     base_code;
    unsigned_64 image_base;             //nt specific fields
    uint_32     section_align;
    uint_32     file_align;
    uint_16     os_major;
    uint_16     os_minor;
    uint_16     user_major;
    uint_16     user_minor;
    uint_16     subsys_major;
    uint_16     subsys_minor;
    uint_32     reserved1;
    uint_32     image_size;
    uint_32     header_size;
    uint_32     file_checksum;
    uint_16     sub_system;
    uint_16     dll_flags;
    unsigned_64 stack_reserve;
    unsigned_64 stack_commit;
    unsigned_64 heap_reserve;
    unsigned_64 heap_commit;
    uint_32     loader_flags;
    uint_32     data_directories;
    coff_image_data_directory   export_table;
    coff_image_data_directory   import_table;
    coff_image_data_directory   resource_table;
    coff_image_data_directory   exception_table;
    coff_image_data_directory   security_table;
    coff_image_data_directory   base_reloc_table;
    coff_image_data_directory   debug;
    coff_image_data_directory   copyright;
    coff_image_data_directory   machine_value;
    coff_image_data_directory   tls_table;
    coff_image_data_directory   load_config_table;
    uint_8      reserved2[40];
} coff_opt_hdr64;

// The following structure defines the new import object. Note the
// values of the first two fields, which must be set as stated in
// order to differentiate old and new import members. Following this
// structure, the linker emits two null-terminated strings used to
// recreate the import at the time of use. The first string is the
// import's name, the second is the dll's name.

#define COFF_IMPORT_OBJECT_HDR_SIG2  0xffff

typedef struct {
    uint_16 sig1;                       // Must be COFF_IMAGE_FILE_MACHINE_UNKNOWN
    uint_16 sig2;                       // Must be COFF_IMPORT_OBJECT_HDR_SIG2.
    uint_16 version;
    uint_16 machine;
    uint_32 time_date_stamp;            // Time/date stamp
    uint_32 size_of_data;               // particularly useful for incremental links

    union {
        uint_16 ordinal;                // if grf & COFF_IMPORT_OBJECT_ORDINAL
        uint_16 hint;
    } oh;

    uint_16 object_type : 2;            // import_object_type
    uint_16 name_type : 3;              // import_name_type
    uint_16 reserved : 11;              // Reserved. Must be zero.
} coff_import_object_header;

// Note that coff_import_object_header is the same size as coff_file_header

typedef enum
{
    COFF_IMPORT_OBJECT_CODE     = 0,
    COFF_IMPORT_OBJECT_DATA     = 1,
    COFF_IMPORT_OBJECT_CONST    = 2,
} coff_import_object_type;

typedef enum
{
    COFF_IMPORT_OBJECT_ORDINAL          = 0,    // Import by ordinal
    COFF_IMPORT_OBJECT_NAME             = 1,    // Import name == public symbol name.
    COFF_IMPORT_OBJECT_NAME_NO_PREFIX   = 2,    // Import name == public symbol name skipping leading ?, @, or optionally _.
    COFF_IMPORT_OBJECT_NAME_UNDECORATE  = 3,    // Import name == public symbol name skipping leading ?, @, or optionally _
                                                // and truncating at first @
} coff_import_object_name_type;
#include "poppck.h"
