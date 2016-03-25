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
* Description:  Data structures for Mach-O file format (Mac OS X).
*
****************************************************************************/


#include <stdint.h>
#include "watcom.h"

typedef int32_t     vm_prot_t;
typedef int32_t     cpu_type_t;
typedef int32_t     cpu_subtype_t;

/* CPU type definitions */
#define CPU_ARCH_MASK           0xff000000
#define CPU_ARCH_ABI64          0x01000000

#define CPU_TYPE_ANY            (-1)
#define CPU_TYPE_VAX            1
#define CPU_TYPE_MC680x0        6
#define CPU_TYPE_X86            7
#define CPU_TYPE_I386           CPU_TYPE_X86
#define CPU_TYPE_X86_64         (CPU_TYPE_X86 | CPU_ARCH_ABI64)
#define CPU_TYPE_POWERPC        18
#define CPU_TYPE_POWERPC64      (CPU_TYPE_POWERPC | CPU_ARCH_ABI64)

/* CPU subtype definitions */
#define CPU_SUBTYPE_MULTIPLE        (-1)
#define CPU_SUBTYPE_LITTLE_ENDIAN   0
#define CPU_SUBTYPE_BIG_ENDIAN      1

#define CPU_SUBTYPE_MC680x0_ALL     1
#define CPU_SUBTYPE_MC68030         1
#define CPU_SUBTYPE_MC68040         2
#define CPU_SUBTYPE_MC68030_ONLY    3

#define CPU_SUBTYPE_INTEL(f, m)     ((f) + ((m) << 4))

#define CPU_SUBTYPE_I386_ALL        CPU_SUBTYPE_INTEL(3, 0)
#define CPU_SUBTYPE_386             CPU_SUBTYPE_INTEL(3, 0)
#define CPU_SUBTYPE_486             CPU_SUBTYPE_INTEL(4, 0)
#define CPU_SUBTYPE_486SX           CPU_SUBTYPE_INTEL(4, 8)
#define CPU_SUBTYPE_586             CPU_SUBTYPE_INTEL(5, 0)
#define CPU_SUBTYPE_PENT            CPU_SUBTYPE_INTEL(5, 0)
#define CPU_SUBTYPE_PENTPRO         CPU_SUBTYPE_INTEL(6, 1)
#define CPU_SUBTYPE_PENTII_M3       CPU_SUBTYPE_INTEL(6, 3)
#define CPU_SUBTYPE_PENTII_M5       CPU_SUBTYPE_INTEL(6, 5)
#define CPU_SUBTYPE_CELERON         CPU_SUBTYPE_INTEL(7, 6)
#define CPU_SUBTYPE_CELERON_MOBILE  CPU_SUBTYPE_INTEL(7, 7)
#define CPU_SUBTYPE_PENTIUM_3       CPU_SUBTYPE_INTEL(8, 0)
#define CPU_SUBTYPE_PENTIUM_3_M     CPU_SUBTYPE_INTEL(8, 1)
#define CPU_SUBTYPE_PENTIUM_3_XEON  CPU_SUBTYPE_INTEL(8, 2)
#define CPU_SUBTYPE_PENTIUM_M       CPU_SUBTYPE_INTEL(9, 0)
#define CPU_SUBTYPE_PENTIUM_4       CPU_SUBTYPE_INTEL(10, 0)
#define CPU_SUBTYPE_PENTIUM_4_M     CPU_SUBTYPE_INTEL(10, 1)
#define CPU_SUBTYPE_ITANIUM         CPU_SUBTYPE_INTEL(11, 0)
#define CPU_SUBTYPE_ITANIUM_2       CPU_SUBTYPE_INTEL(11, 1)
#define CPU_SUBTYPE_XEON            CPU_SUBTYPE_INTEL(12, 0)
#define CPU_SUBTYPE_XEON_MP         CPU_SUBTYPE_INTEL(12, 1)

#define CPU_SUBTYPE_INTEL_FAMILY(x)     ((x) & 15)
#define CPU_SUBTYPE_INTEL_FAMILY_MAX    15

#define CPU_SUBTYPE_INTEL_MODEL(x)      ((x) >> 4)
#define CPU_SUBTYPE_INTEL_MODEL_ALL     0

#define CPU_SUBTYPE_X86_ALL         3
#define CPU_SUBTYPE_X86_64_ALL      3
#define CPU_SUBTYPE_X86_ARCH1       4

#define CPU_SUBTYPE_POWERPC_ALL     0
#define CPU_SUBTYPE_POWERPC_601     1
#define CPU_SUBTYPE_POWERPC_602     2
#define CPU_SUBTYPE_POWERPC_603     3
#define CPU_SUBTYPE_POWERPC_603e    4
#define CPU_SUBTYPE_POWERPC_603ev   5
#define CPU_SUBTYPE_POWERPC_604     6
#define CPU_SUBTYPE_POWERPC_604e    7
#define CPU_SUBTYPE_POWERPC_620     8
#define CPU_SUBTYPE_POWERPC_750     9
#define CPU_SUBTYPE_POWERPC_7400    10
#define CPU_SUBTYPE_POWERPC_7450    11
#define CPU_SUBTYPE_POWERPC_970     100


/* From mach-o/loader.h */

typedef struct mach_header {
    uint32_t        magic;
    cpu_type_t      cputype;
    cpu_subtype_t   cpusubtype;
    uint32_t        filetype;
    uint32_t        ncmds;
    uint32_t        sizeofcmds;
    uint32_t        flags;
} mach_header;

typedef struct mach_header_64 {
    uint32_t        magic;
    cpu_type_t      cputype;
    cpu_subtype_t   cpusubtype;
    uint32_t        filetype;
    uint32_t        ncmds;
    uint32_t        sizeofcmds;
    uint32_t        flags;
    uint32_t        reserved;
} mach_header_64;

/* Header magic */
#define MH_MAGIC        0xfeedface
#define MH_CIGAM        0xcefaedfe
#define MH_MAGIC64      0xfeedfacf
#define MH_CIGAM64      0xcffaedfe

/* File type constants */
#define MH_OBJECT       0x1
#define MH_EXECUTE      0x2
#define MH_FVMLIB       0x3
#define MH_CORE         0x4
#define MH_PRELOAD      0x5
#define MH_DYLIB        0x6
#define MH_DYLINKER     0x7
#define MH_BUNDLE       0x8
#define MH_DYLIB_STUB   0x9
#define MH_DSYM         0xa

/* Flag constants */
#define MH_NOUNDEFS         0x1
#define MH_INCRLINK         0x2
#define MH_DYLDLINK         0x4
#define MH_BINDATLOAD       0x8
#define MH_PREBOUND         0x10
#define MH_SPLIT_SEGS       0x20
#define MH_LAZY_INIT        0x40
#define MH_TWOLEVEL         0x80
#define MH_FORCE_FLAT       0x100
#define MH_NOMULTIDEFS      0x200
#define MH_NOFIXPREBINDING  0x400
#define MH_PREBINDABLE      0x800
#define MH_ALLMODSBOUND     0x1000
#define MH_SUBSECTIONS_VIA_SYMBOLS  0x2000
#define MH_CANONICAL        0x4000
#define MH_WEAK_DEFINES     0x8000
#define MH_BINDS_TO_WEAK    0x10000
#define MH_ALLOW_STACK_EXECUTION    0x20000

typedef struct load_command {
    uint32_t    cmd;
    uint32_t    cmdsize;
} load_command;

/* Load commands */
#define LC_REQ_DYLD             0x80000000

#define LC_SEGMENT              0x1
#define LC_SYMTAB               0x2
#define LC_SYMSEG               0x3
#define LC_THREAD               0x4
#define LC_UNIXTHREAD           0x5
#define LC_LOADFVMLIB           0x6
#define LC_IDFVMLIB             0x7
#define LC_IDENT                0x8
#define LC_FVMFILE              0x9
#define LC_PREPAGE              0xa
#define LC_DYSYMTAB             0xb
#define LC_LOAD_DYLIB           0xc
#define LC_ID_DYLIB             0xd
#define LC_LOAD_DYLINKER        0xe
#define LC_ID_DYLINKER          0xf
#define LC_PREBOUND_DYLIB       0x10
#define LC_ROUTINES             0x11
#define LC_SUB_FRAMEWORK        0x12
#define LC_SUB_UMBRELLA         0x13
#define LC_SUB_CLIENT           0x14
#define LC_SUB_LIBRARY          0x15
#define LC_TWOLEVEL_HINTS       0x16
#define LC_PREBIND_CKSUM        0x17
#define LC_LOAD_WEAK_DYLIB      (0x18 | LC_REQ_DYLD)
#define LC_SEGMENT_64           0x19
#define LC_ROUTINES_0x1a        0x1a
#define LC_UUID                 0x1b

typedef union lc_str {
    uint32_t    offset;
#ifndef __LP64__
    char        *ptr;
#endif
} lc_str;

typedef struct segment_command {
    uint32_t        cmd;
    uint32_t        cmdsize;
    char            segname[16];
    uint32_t        vmaddr;
    uint32_t        vmsize;
    uint32_t        fileoff;
    uint32_t        filesize;
    vm_prot_t       maxprot;
    vm_prot_t       initprot;
    uint32_t        nsects;
    uint32_t        flags;
} segment_command;

typedef struct segment_command_64 {
    uint32_t        cmd;
    uint32_t        cmdsize;
    char            segname[16];
    uint64_t        vmaddr;
    uint64_t        vmsize;
    uint64_t        fileoff;
    uint64_t        filesize;
    vm_prot_t       maxprot;
    vm_prot_t       initprot;
    uint32_t        nsects;
    uint32_t        flags;
} segment_command_64;

/* Segment command flags */
#define SG_HIGHVM               0x1
#define SG_FVMLIB               0x2
#define SG_NORELOC              0x4
#define SG_PROTECTED_VERSION_1  0x8

typedef struct section {
    char       sectname[16];
    char       segname[16];
    uint32_t   addr;
    uint32_t   size;
    uint32_t   offset;
    uint32_t   align;
    uint32_t   reloff;
    uint32_t   nreloc;
    uint32_t   flags;
    uint32_t   reserved1;
    uint32_t   reserved2;
} section;

typedef struct section_64 {
    char       sectname[16];
    char       segname[16];
    uint64_t   addr;
    uint64_t   size;
    uint32_t   offset;
    uint32_t   align;
    uint32_t   reloff;
    uint32_t   nreloc;
    uint32_t   flags;
    uint32_t   reserved1;
    uint32_t   reserved2;
    uint32_t   reserved3;
} section_64;

/* Section types */
#define SECTION_TYPE                    0x000000ff
#define SECTION_ATTRIBUTES              0xffffff00

#define S_REGULAR                       0x0
#define S_ZEROFILL                      0x1
#define S_CSTRING_LITERALS              0x2
#define S_4BYTE_LITERALS                0x3
#define S_8BYTE_LITERALS                0x4
#define S_LITERAL_POINTERS              0x5
#define S_NON_LAZY_SYMBOL_POINTERS      0x6
#define S_LAZY_SYMBOL_POINTERS          0x7
#define S_SYMBOL_STUBS                  0x8
#define S_MOD_INIT_FUNC_POINTERS        0x9
#define S_MOD_TERM_FUNC_POINTERS        0xa
#define S_COALESCED                     0xb
#define S_GB_ZEROFILL                   0xc
#define S_INTERPOSING                   0xd
#define S_16BYTE_LITERALS               0xe

#define SECTION_ATTRIBUTES_USR          0xff000000
#define S_ATTR_PURE_INSTRUCTIONS        0x80000000
#define S_ATTR_NO_TOC                   0x40000000
#define S_ATTR_STRIP_STATIC_SYMS        0x20000000
#define S_ATTR_NO_DEAD_STRIP            0x10000000
#define S_ATTR_LIVE_SUPPORT             0x08000000
#define S_ATTR_SELF_MODIFING_CODE       0x04000000
#define S_ATTR_DEBUG                    0x02000000

#define SECTION_ATTRIBUTES_SYS          0x00ffff00
#define S_ATTR_SOME_INSTRUCTIONS        0x00000400
#define S_ATTR_EXT_RELOC                0x00000200
#define S_ATTR_LOC_RELOC                0x00000100

/* "Known" segment and section names */
#define SEG_PAGEZERO                    "__PAGEZERO"
#define SEG_TEXT                        "__TEXT"
#define SECT_TEXT                       "__text"
#define SECT_FVMLIB_INIT0               "__fvmlib_init0"
#define SECT_FVMLIB_INIT1               "__fvmlib_init1"
#define MACH_SEG_DATA                   "__DATA"
#ifndef SEG_DATA        /* conflicts with exeos2.h */
#define SEG_DATA                        MACH_SEG_DATA
#endif
#define SECT_DATA                       "__data"
#define SECT_BSS                        "__bss"
#define SECT_COMMON                     "__common"
#define SEG_OBJC                        "__OBJC"
#define SECT_OBJC_SYMBOLS               "__symbol_table"
#define SECT_OBJC_MODULES               "__module_info"
#define SECT_OBJC_STRINGS               "__selector_strs"
#define SECT_OBJC_REFS                  "__selector_refs"
#define SEG_ICON                        "__ICON"
#define SECT_ICON_HEADER                "__header"
#define SECT_ICON_TIFF                  "__tiff"
#define SEG_LINKEDIT                    "__LINKEDIT"
#define SEG_UNIXSTACK                   "__UNIXSTACK"
#define SEG_IMPORT                      "__IMPORT"

struct fvmlib {
    union lc_str    name;
    uint32_t        minor_version;
    uint32_t        header_addr;
};

struct fvmlib_command {
    uint32_t        cmd;
    uint32_t        cmdsize;
    struct fvmlib   fvmlib;
};

struct dylib {
    union lc_str    name;
    uint32_t        timestamp;
    uint32_t        current_version;
    uint32_t        compatibility_version;
};

struct dylib_command {
    uint32_t        cmd;
    uint32_t        cmdsize;
    struct dylib    dylib;
};

struct sub_client_command {
    uint32_t        cmd;
    uint32_t        cmdsize;
    union lc_str    client;
};

struct sub_framework_command {
    uint32_t        cmd;
    uint32_t        cmdsize;
    union lc_str    umbrella;
};

struct sub_umbrella_command {
    uint32_t        cmd;
    uint32_t        cmdsize;
    union lc_str    sub_umbrella;
};

struct sub_library_command {
    uint32_t        cmd;
    uint32_t        cmdsize;
    union lc_str    sub_library;
};

struct prebound_dylib_command {
    uint32_t        cmd;
    uint32_t        cmdsize;
    union lc_str    name;
    uint32_t        nmodules;
    union lc_str    linked_modules;
};

struct dylinker_command {
    uint32_t        cmd;
    uint32_t        cmdsize;
    union lc_str    name;
};

struct thread_command {
    uint32_t        cmd;
    uint32_t        cmdsize;
};

struct routines_command {
    uint32_t        cmd;
    uint32_t        cmdsize;
    uint32_t        init_address;
    uint32_t        init_module;
    uint32_t        reserved1;
    uint32_t        reserved2;
    uint32_t        reserved3;
    uint32_t        reserved4;
    uint32_t        reserved5;
    uint32_t        reserved6;
};

struct routines_command_64 {
    uint32_t        cmd;
    uint32_t        cmdsize;
    uint64_t        init_address;
    uint64_t        init_module;
    uint64_t        reserved1;
    uint64_t        reserved2;
    uint64_t        reserved3;
    uint64_t        reserved4;
    uint64_t        reserved5;
    uint64_t        reserved6;
};

struct symtab_command {
    uint32_t        cmd;
    uint32_t        cmdsize;
    uint32_t        symoff;
    uint32_t        nsyms;
    uint32_t        stroff;
    uint32_t        strsize;
};

struct dysymtab_command {
    uint32_t        cmd;
    uint32_t        cmdsize;
    uint32_t        ilocalsym;
    uint32_t        nlocalsym;
    uint32_t        iextdefsym;
    uint32_t        nextdefsym;
    uint32_t        iundefsym;
    uint32_t        nundefsym;
    uint32_t        tocoff;
    uint32_t        ntoc;
    uint32_t        nmodtab;
    uint32_t        extrefsymoff;
    uint32_t        nextrefsyms;
    uint32_t        indirectsymoff;
    uint32_t        nindirectsyms;
    uint32_t        extreloff;
    uint32_t        nextrel;
    uint32_t        locreloff;
    uint32_t        nlocrel;
};

#define INDIRECT_SYMBOL_LOCAL   0x80000000
#define INDIRECT_SYMBOL_ABS     0x40000000

struct dylib_table_of_contents {
    uint32_t        symbol_index;
    uint32_t        module_index;
};

struct dylib_module {
    uint32_t        module_name;
    uint32_t        iextdefsym;
    uint32_t        nextdefsym;
    uint32_t        irefsym;
    uint32_t        nrefsym;
    uint32_t        ilocalsym;
    uint32_t        nlocalsym;
    uint32_t        iextrel;
    uint32_t        nextrel;
    uint32_t        iinit_term;
    uint32_t        ninit_term;
    uint32_t        objc_module_info_addr;
    uint32_t        objc_module_info_size;
};

struct dylib_module_64 {
    uint32_t        module_name;
    uint32_t        iextdefsym;
    uint32_t        nextdefsym;
    uint32_t        irefsym;
    uint32_t        nrefsym;
    uint32_t        ilocalsym;
    uint32_t        nlocalsym;
    uint32_t        iextrel;
    uint32_t        nextrel;
    uint32_t        iinit_term;
    uint32_t        ninit_term;
    uint32_t        objc_module_info_size;
    uint64_t        objc_module_info_addr;
};

struct dylib_reference {
    uint32_t        isym    : 24,
                    flags   : 8;
};

struct twolevel_hints_command {
    uint32_t        cmd;
    uint32_t        cmdsize;
    uint32_t        offset;
    uint32_t        nhints;
};

struct twolevel_hint {
    uint32_t        isub_image  : 8,
                    itoc        : 24;
};

struct prebind_cksum_command {
    uint32_t        cmd;
    uint32_t        cmdsize;
    uint32_t        cksum;
};

struct uuid_command {
    uint32_t        cmd;
    uint32_t        cmdsize;
    uint32_t        uuid[16];
};

struct symseg_command {
    uint32_t        cmd;
    uint32_t        cmdsize;
    uint32_t        offset;
    uint32_t        size;
};


struct ident_command {
    uint32_t        cmd;
    uint32_t        cmdsize;
};

struct fvmfile_command {
    uint32_t        cmd;
    uint32_t        cmdsize;
    union lc_str    name;
    uint32_t        header_addr;
};


/* From mach-o/reloc.h */

struct relocation_info {
    int32_t         r_address;
    uint32_t        r_symbolnum : 24,
                    r_pcrel     : 1,
                    r_length    : 2,
                    r_extern    : 1,
                    r_type      : 4;
};

#define R_ABS       0
#define R_SCATTERED 0x80000000

struct scattered_relocation_info {
#ifdef __BIG_ENDIAN__
    uint32_t        r_scattered : 1,
                    r_pcrel     : 1,
                    r_length    : 2,
                    r_type      : 4,
                    r_address   : 24;
    int32_t         r_value;
#else
    uint32_t        r_address   : 24,
                    r_type      : 4,
                    r_length    : 2,
                    r_pcrel     : 1,
                    r_scattered : 1;
    int32_t         r_value;
#endif
};

enum reloc_type_generic
{
    GENERIC_RELOC_VANILLA,
    GENERIC_RELOC_PAIR,
    GENERIC_RELOC_SECTDIFF,
    GENERIC_RELOC_PB_LA_PTR,
    GENERIC_RELOC_LOCAL_SECTDIFF
};


/* From mach-o/ranlib.h */

#define SYMDEF          "__.SYMDEF"
#define SYMDEF_SORTED   "__.SYMDEF SORTED"

struct ranlib {
    union {
        uint32_t    ran_strx;
#ifndef __LP64__
        char        *ran_name;
#endif
    } ran_un;
    uint32_t        ran_off;
};


/* From mach-o/fat.h */

#define FAT_MAGIC       0xcafebabe
#define FAT_CIGAM       0xbebafeca

struct fat_header {
    uint32_t        magic;
    uint32_t        nfat_arch;
};

struct fat_arch {
    cpu_type_t      cputype;
    cpu_subtype_t   cpusubtype;
    uint32_t        offset;
    uint32_t        size;
    uint32_t        align;
};
