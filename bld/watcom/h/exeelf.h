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


#ifndef _EXEELF_H_INCLUDED_
#define _EXEELF_H_INCLUDED_
#ifdef __cplusplus
extern "C" {
#endif

#include <watcom.h>

#pragma pack(push,1);

// elf scalar data types

typedef unsigned_32     Elf32_Addr;
typedef unsigned_16     Elf32_Half;
typedef unsigned_32     Elf32_Off;
typedef signed_32       Elf32_Sword;
typedef unsigned_32     Elf32_Word;

// the main header

#define EI_NIDENT       16

typedef struct {
    unsigned_8  e_ident[EI_NIDENT];     // signature & ID info
    Elf32_Half  e_type;                 // file type (i.e. obj file, exe file)
    Elf32_Half  e_machine;              // required architecture
    Elf32_Word  e_version;              // version of the file
    Elf32_Addr  e_entry;                // program entry point
    Elf32_Off   e_phoff;                // program header offset
    Elf32_Off   e_shoff;                // section header offset
    Elf32_Word  e_flags;                // processor specific flags
    Elf32_Half  e_ehsize;               // elf header size
    Elf32_Half  e_phentsize;            // program header entry size
    Elf32_Half  e_phnum;                // number of program header entries
    Elf32_Half  e_shentsize;            // section header entry size
    Elf32_Half  e_shnum;                // number of section header entries
    Elf32_Half  e_shstrndx;             // section name string table index.
// Obselete
/*    Elf32_Half        e_shosndx;              // OS info section index
    Elf32_Half  e_pad;                  // to keep header size a word multiple*/
} Elf32_Ehdr;

// e_ident field indicies

#define ELF_SIGNATURE   "\177ELF"
#define ELF_SIGNATURE_LEN 4
#define ELFMAG0         '\177'  // bytes of signature
#define ELFMAG1         'E'     // bytes of signature
#define ELFMAG2         'L'     // bytes of signature
#define ELFMAG3         'F'     // bytes of signature
#define EI_MAG0         0       // signature bytes
#define EI_MAG1         1       // signature bytes
#define EI_MAG2         2       // signature bytes
#define EI_MAG3         3       // signature bytes
#define EI_CLASS        4       // "file class", i.e. 32-bit vs. 64-bit
#define EI_DATA         5       // data encoding (big-endian vs. little-endian)
#define EI_VERSION      6       // header version #
#define EI_PAD          7       // start of padding bytes

// contents of the EI_CLASS field index

#define ELFCLASSNONE    0       // invalid class
#define ELFCLASS32      1       // 32-bit objects
#define ELFCLASS64      2       // 64-bit objects

// contents of the EI_DATA field index

#define ELFDATANONE     0       // invalid data encoding
#define ELFDATA2LSB     1       // "little-endian"
#define ELFDATA2MSB     2       // "big-endian"

// the current elf version number

#define EV_CURRENT      1

// elf object file types

#define ET_NONE         0       // no file type
#define ET_REL          1       // relocatable file
#define ET_EXEC         2       // executable file
#define ET_DYN          3       // shared object file
#define ET_CORE         4       // core file
#define ET_LOPROC       0xff00  // processor specific file types
#define ET_HIPROC       0xffff

// elf machine types

#define EM_NONE         0
#define EM_M32          1
#define EM_SPARC        2
#define EM_386          3
#define EM_68K          4
#define EM_88K          5
#define EM_860          7
#define EM_MIPS         8
#define EM_ALPHA        10      // bogus - BBB
#define EM_PPC          20      // Note - this changed from previous spec.
#define EM_PPC_O        17      // remove when the rest of the world conforms

// version number info

#define EV_NONE         0
#define EV_CURRENT      1
#define EV_WPIBM        2       // version identifier for extended ELF

// flags (machine specific)
//EM_M32
#define EF_M32_MAU      0x1

// special section indicies

#define SHN_UNDEF       0
#define SHN_LORESERVE   0xff00
#define SHN_LOPROC      0xff00  // reserved for processor-specific semantics
#define SHN_HIPROC      0xff1f
#define SHN_ABS         0xfff1  // references to this section are absolute
#define SHN_COMMON      0xfff2  // references to this section are common.
#define SHN_HIRESERVE   0xffff

// section header

typedef struct {
    Elf32_Word  sh_name;        // name of the section
    Elf32_Word  sh_type;        // section type
    Elf32_Word  sh_flags;
    Elf32_Addr  sh_addr;        // starting address of section in image
    Elf32_Off   sh_offset;      // start of section in file
    Elf32_Word  sh_size;        // size of section in file.
    Elf32_Word  sh_link;        // multipurpose field   (based on type)
    Elf32_Word  sh_info;        // another multipurpose field (based on type)
    Elf32_Word  sh_addralign;   // address alignment
    Elf32_Word  sh_entsize;     // entry size for sects with fixed sized entries
} Elf32_Shdr;

// section types

#define SHT_NULL        0               // inactive
#define SHT_PROGBITS    1               // meaning defined by program
#define SHT_SYMTAB      2               // symbol table
#define SHT_STRTAB      3               // string table
#define SHT_RELA        4               // reloc entries with explicit addends
#define SHT_HASH        5               // symbol hash table
#define SHT_DYNAMIC     6               // dynamic linking information
#define SHT_NOTE        7               // comment information
#define SHT_NOBITS      8               // like PROGBITS but no space in file.
#define SHT_REL         9               // as RELA but no explicit addends
#define SHT_SHLIB       10              // reserved but evil
#define SHT_DYNSYM      11              // dynamic link symbol table
#define SHT_OS          0x60000001      // info to identify target OS
#define SHT_IMPORTS     0x60000002      // info on refs to external symbols
#define SHT_EXPORTS     0x60000003      // info on symbols exported by ordinal
#define SHT_RES         0x60000004      // read-only resource data.
#define SHT_PROGFRAGS   0x60001001      // similar to SHT_PROGBITS
#define SHT_IDMDLL      0x60001002      // symbol name demangling information
#define SHT_DEFLIB      0x60001003      // default static libraries
#define SHT_LOPROC      0x70000000      // processor specific
#define SHT_HIPROC      0x7fffffff
#define SHT_LOUSER      0x80000000      // user defined sections
#define SHT_HIUSER      0xffffffff

// Old section types.  Readers should handle these, writers must use the above

#define SHT_OS_O        12              // info to identify target OS
#define SHT_IMPORTS_O   13              // info on refs to external symbols
#define SHT_EXPORTS_O   14              // info on symbols exported by ordinal
#define SHT_RES_O       15              // read-only resource data.

// sh_flags values

#define SHF_WRITE       0x00000001      // section writable during execution
#define SHF_ALLOC       0x00000002      // section occupies space during exec.
#define SHF_EXECINSTR   0x00000004      // section contains code.
#define SHF_BEGIN       0x01000000      // section to be placed at the beginning
                                        // of like-named sections by static link
#define SHF_END         0x02000000      // same, end.
#define SHF_MASKPROC    0xf0000000      // processor specific flags

// symbol table entry

typedef struct {
    Elf32_Word  st_name;        // symbol name index into string table
    Elf32_Addr  st_value;       // symbol "value"
    Elf32_Word  st_size;        // symbol size
    unsigned_8  st_info;        // symbol's type and binding attribs.
    unsigned_8  st_other;       // no meaning yet.
    Elf32_Half  st_shndx;       // section index
} Elf32_Sym;

// symbol info field contents

#define ELF32_ST_BIND(i)        ((i)>>4)        // get the "bind" subfield
#define ELF32_ST_TYPE(i)        ((i)&0xf)       // get the type subfield
#define ELF32_ST_INFO(b,t)      (((b)<<4)+((t)&0xf)) // make a new st_info

// bind subfield contents

#define STB_LOCAL       0       // symbol has local binding
#define STB_GLOBAL      1       // symbol has global binding
#define STB_WEAK        2       // symbol has weak binding
#define STB_ENTRY       12      // symbol is entry-point for the load module
#define STB_LOPROC      13      // processor specific semantics
#define STB_HIPROC      15

// type subfield contents

#define STT_NOTYPE      0       // not specified
#define STT_OBJECT      1       // symbol is a data object
#define STT_FUNC        2       // symbol is a code symbol
#define STT_SECTION     3       // symbol associated with a section
#define STT_FILE        4       // symbol gives name of the source file.
#define STT_IMPORT      11      // reference to a symbol in another module
#define STT_LOPROC      13      // processor specific semantics
#define STT_HIPROC      15

// relocation entries

typedef struct {
    Elf32_Addr  r_offset;       // place to apply reloc (from begin of section)
    Elf32_Word  r_info;         // symbol idx, and type of reloc
} Elf32_Rel;

typedef struct {
    Elf32_Addr  r_offset;       // place to apply reloc (from begin of section)
    Elf32_Word  r_info;         // symbol idx, and type of reloc
    Elf32_Sword r_addend;       // value used as a basis for the reloc.
} Elf32_Rela;

// r_info field contents

#define ELF32_R_SYM(i)  ((i)>>8)                // gets the symbol index
#define ELF32_R_TYPE(i) ((unsigned_8)(i))       // gets the symbol type
#define ELF32_R_INFO(s,t) (((s)<<8)+(unsigned_8)(t))    // make a new r_info

// relocation types.
//386
#define R_386_NONE              0
#define R_386_32                1
#define R_386_PC32              2
#define R_386_GOT32             3
#define R_386_PLT32             4
#define R_386_COPY              5
#define R_386_GLOB_DAT          6
#define R_386_JMP_SLOT          7
#define R_386_RELATIVE          8
#define R_386_GOTOFF            9
#define R_386_GOTPC             10
//PPC
#define R_PPC_NONE              0
#define R_PPC_ADDR32            1
#define R_PPC_ADDR24            2
#define R_PPC_ADDR16            3
#define R_PPC_ADDR16_LO         4
#define R_PPC_ADDR16_HI         5
#define R_PPC_ADDR16_HA         6
#define R_PPC_ADDR14            7
#define R_PPC_ADDR14_BRTAKEN    8
#define R_PPC_ADDR14_BRNTAKEN   9
#define R_PPC_REL24             10
#define R_PPC_REL14             11
#define R_PPC_REL14_BRTAKEN     12
#define R_PPC_REL14_BRNTAKEN    13
#define R_PPC_GOT16             14
#define R_PPC_GOT16_LO          15
#define R_PPC_GOT16_HI          16
#define R_PPC_GOT16_HA          17
#define R_PPC_PLTREL24          18
#define R_PPC_COPY              19
#define R_PPC_GLOB_DAT          20
#define R_PPC_JMP_SLOT          21
#define R_PPC_RELATIVE          22
#define R_PPC_LOCAL24PC         23
#define R_PPC_UADDR32           24
#define R_PPC_UADDR16           25
#define R_PPC_REL32             26
#define R_PPC_PLT32             27
#define R_PPC_PLTREL32          28
#define R_PPC_PLT16_LO          29
#define R_PPC_PLT16_HI          30
#define R_PPC_PLT16_HA          31
#define R_PPC_SDAREL16          32

//M32
#define R_M32_NONE              0
#define R_M32_32                1
#define R_M32_S                 2
#define R_M32_PC32_S            3
#define R_M32_GOT32_S           4
#define R_M32_PLT32_S           5
#define R_M32_COPY              6
#define R_M32_GLOB_DAT          7
#define R_M32_JMP_SLOT          8
#define R_M32_RELATIVE          9
#define R_M32_RELATIVE_S        10

//SPARC
#define R_SPARC_NONE            0
#define R_SPARC_8               1
#define R_SPARC_16              2
#define R_SPARC_32              3
#define R_SPARC_DISP8           4
#define R_SPARC_DISP16          5
#define R_SPARC_DISP32          6
#define R_SPARC_WDISP30         7
#define R_SPARC_WDISP22         8
#define R_SPARC_HI22            9
#define R_SPARC_22              10
#define R_SPARC_13              11
#define R_SPARC_LO10            12
#define R_SPARC_GOT10           13
#define R_SPARC_GOT13           14
#define R_SPARC_GOT22           15
#define R_SPARC_PC10            16
#define R_SPARC_PC22            17
#define R_SPARC_WPLT30          18
#define R_SPARC_COPY            19
#define R_SPARC_GLOB_DAT        20
#define R_SPARC_JMP_SLOT        21
#define R_SPARC_RELATIVE        22
#define R_SPARC_UA32            23
#define R_SPARC_PLT32           24
#define R_SPARC_HIPLT22         25
#define R_SPARC_LOPLT10         26
#define R_SPARC_PCPLT32         27
#define R_SPARC_PCPLT22         28
#define R_SPARC_PCPLT10         29
#define R_SPARC_10              30
#define R_SPARC_11              31
#define R_SPARC_WDISP16         40
#define R_SPARC_WDISP19         41
#define R_SPARC_7               43
#define R_SPARC_5               44
#define R_SPARC_6               45

// program header

typedef struct {
    Elf32_Word  p_type;         // type of segment
    Elf32_Off   p_offset;       // offset of segment from beginnning of file
    Elf32_Addr  p_vaddr;        // segment virtual address
    Elf32_Addr  p_paddr;        // segment physical address
    Elf32_Word  p_filesz;       // size of segment in file
    Elf32_Word  p_memsz;        // size of segment in memory
    Elf32_Word  p_flags;
    Elf32_Word  p_align;        // segment align value (in mem & file)
} Elf32_Phdr;

// segment types

#define PT_NULL         0               // unused segment
#define PT_LOAD         1               // loadable segment
#define PT_DYNAMIC      2               // contains dynamic linking information
#define PT_INTERP       3               // reference to a program interpreter
#define PT_NOTE         4               // comments & auxiliary information
#define PT_SHLIB        5               // here be dragons
#define PT_PHDR         6               // address of prog. header in mem (for interp.)
#define PT_OS           0x60000001      // target os information
#define PT_RES          0x60000002      // read-only resource information
#define PT_LOPROC       0x70000000      // processor specific
#define PT_HIPROC       0x7fffffff

// Old segment types.  Readers should handle these, writers must use the above

#define PT_OS_O         7       // target os information
#define PT_RES_O        9       // read-only resource information

// note entry format

typedef struct {
    unsigned_32 namesz;         // size of name
    unsigned_32 descsz;         // size of descriptor
    unsigned_32 type;           // user defined "type" of the note
    char        name[1];        // variable length name.
    //unsigned_32 desc[];       // descriptors go here
} elf_note;

// dynamic segment entry information.

typedef struct {
    Elf32_Sword         d_tag;
    union {
        Elf32_Word      d_val;
        Elf32_Addr      d_ptr;
    } d_un;
} Elf32_Dyn;

// dynamic array tags

#define DT_NULL         0
#define DT_NEEDED       1               // name of a needed library
#define DT_PLTRELSZ     2               // size of reloc entries for PLT
#define DT_PLTGOT       3               // address with PLT or GOT
#define DT_HASH         4               // symbol hash table address
#define DT_STRTAB       5               // string table address
#define DT_SYMTAB       6               // symbol table address
#define DT_RELA         7               // address of reloc table with addends
#define DT_RELASZ       8               // size of the DT_RELA table
#define DT_RELAENT      9               // size of a DT_RELA entry
#define DT_STRSZ        10              // size of the string table
#define DT_SYMENT       11              // size of a symbol table entry
#define DT_SONAME       14              // shared object name
#define DT_REL          17              // address of reloc table without addends
#define DT_RELSZ        18              // size of the DT_REL table
#define DT_RELENT       19              // size of a DT_REL entry
#define DT_PLTREL       20              // type of reloc entry for PLT
#define DT_DEBUG        21              // for debugging information
#define DT_JMPREL       23              // reloc entries only with PLT
#define DT_EXPORT       0x60000001      // address of export table
#define DT_EXPORTSZ     0x60000002      // size of export table
#define DT_EXPENT       0x60000003      // size of export table entry
#define DT_IMPORT       0x60000004      // address of import table
#define DT_IMPORTSZ     0x60000005      // size of import table
#define DT_IMPENT       0x60000006      // size of import table entry
#define DT_IT           0x60000007      // init and term types for a DLL.
#define DT_ITPRTY       0x60000008      // relative priority of init and term to other functions
#define DT_INITTERM     0x60000009      // address of init and term function
#define DT_PPC_GOT      0x70000001      // address of Global Offset Table
#define DT_PPC_GOTSZ    0x70000002      // size of Global Offset Table
#define DT_PPC_PLTSZ    0x70000003      // size of Procedure Linkage Table
#define DT_LOPROC       0x70000000      // range of processor-defined tags
#define DT_HIPROC       0x7FFFFFFF

// Old dynamic tags.  Readers should handle these, writers must use the above

#define DT_INIT_O       12      // address of initialization function
#define DT_FINI_O       13      // address of finialization function
#define DT_RPATH_O      15      // library search path
#define DT_SYMBOLIC_O   16      // affects dyn. linker's sym. resolution
#define DT_TEXTREL_O    22      // signal we might mod. a non-writable segment
#define DT_IT_O         24      // init and term types for a DLL.
#define DT_EXPORT_O     25      // address of export table
#define DT_EXPORTSZ_O   26      // size of export table
#define DT_IMPORT_O     27      // address of import table
#define DT_IMPORTSZ_O   28      // size of import table
#define DT_GOT_O        29      // address of Global Offset Table
#define DT_GOTSZ_O      30      // size of Global Offset Table
#define DT_PLTSZ_O      32      // size of Procedure Linkage Table
#define DT_ITPRTY_O     33      // relative priority of init and term to other functions
#define DT_LOUSER_O     0x60000000      // range of user-definable tags. will not
#define DT_HIUSER_O     0x6FFFFFFF      // conflict with system-defined tags
                                        // Ha Ha Ha!

// description of DT_IT tag:
// Describe type for initalization and termination of DLL
// Required if DT_INIT and DT_FINI also specified

#define ELF_32_IT_INIT(it)      ((it) & 0x0f)
#define ELF_32_IT_TERM(it)      (((it) >> 4) & 0x0f)
#define ELF_32_IT_INFO(i,t)     (((i) & 0x0f)|(((t) & 0x0f) << 4))

#define IT_NONE         0       // no initialization or termination
#define IT_GLOBAL       1       // global init, term
#define IT_INSTANCE     2       // process init, term
#define IT_THREAD       3       // thread init, term

// DT_INITTERM function prototype

typedef unsigned long INITTERM ( unsigned long modhandle, unsigned long flag );

// elf segment flag bits

#define PF_X            0x1             // seg has execute permissions
#define PF_W            0x2             // seg has write permissions
#define PF_R            0x4             // seg has read permissions
#define PF_S            0x01000000      // segment is shared.
#define PF_MASKPROC     0xf0000000      // processor-specific flag mask

// operating system information

typedef struct {
    Elf32_Word  os_type;
    Elf32_Word  os_size;
} Elf32_Os;

#define EOS_NONE        0       // bad or unknown
#define EOS_PN          1       // IBM Microkernel personality neutral
#define EOS_SVR4        2       // UNIX System V Release 4
#define EOS_AIX         3       // IBM AIX
#define EOS_OS2         4       // IBM OS/2, 32 bit
#define EOS_NT          5       // Microsoft Windows NT, 32 bit
#define EOS_VMS         6       // DEC VMS/VAX
#define EOS_OS400       7       // IBM OS/400
#define EOS_NEXT        8       // NEXT
#define EOS_SYSTEM7     9       // Apple System 7

// OS/2-specific information

typedef struct {
    unsigned char       os2_sessiontype;
    unsigned char       os2_sessionflags;
    unsigned char       os2_reserved[14];
} Elf32_OS2Info;

// os2_sessiontype values

#define OS2_SES_NONE    0       // no session type.  Only valid for DLL's
#define OS2_SES_FS      1       // Full Screen session.
#define OS2_SES_PM      2       // Presentation Manager session.
#define OS2_SES_VIO     3       // Windowed (character-mode) session

// import table entry

typedef struct {
    Elf32_Word  imp_ordinal;
    Elf32_Word  imp_name;
    Elf32_Word  imp_info;
    Elf32_Word  imp_reserved;
} Elf32_Import;

#define ELF32_IMP_TYPE(i)       ((i) >> 24)
#define ELF32_IMP_DLL(i)        ((i) & 0x00FFFFFF)
#define ELF32_IMP_INFO(t,d)     (((t)<<24) | ((d) & 0x00FFFFFF)))

#define IMP_IGNORED     0       // This import entry to be ignored
#define IMP_STR_IDX     1       // Value is string table index to load module
#define IMP_DT_IDX      2       // Value is ref to DT_NEEDED in Dynamic Segment

// export table entry

typedef struct {
    Elf32_Word  exp_ordinal;
    Elf32_Word  exp_symbol;
    Elf32_Word  exp_name;
    Elf32_Word  exp_reserved;
} Elf32_Export;

// Resource header

#define RH_NIDENT       16

typedef struct {
    unsigned char       rh_ident[RH_NIDENT];
    Elf32_Off           rh_name;
    Elf32_Word          rh_itnum;
    Elf32_Word          rh_rhsize;
    Elf32_Word          rh_size;
    Elf32_Off           rh_strtab;
} Elf32_Rhdr;

// rh_ident field indices

#define ELFRESMAG0      '\002'  // bytes of signature
#define ELFRESMAG1      'R'     // bytes of signature
#define ELFRESMAG2      'E'     // bytes of signature
#define ELFRESMAG3      'S'     // bytes of signature
#define RH_MAG0         0       // signature bytes
#define RH_MAG1         1       // signature bytes
#define RH_MAG2         2       // signature bytes
#define RH_MAG3         3       // signature bytes
#define RH_CLASS        4       // class of resource collection
#define RH_DATA         5       // data encoding of resource collection
#define RH_VERSION      6       // version
#define RH_PAD          7       // start of padding bytes - set to 0

// contents of RH_CLASS field

#define RESCLASSNONE    0       // invalid class
#define RESCLASS32      2       // 32-bit architecture
#define RESCLASS64      3       // reserved for 64-bit architecture

// contents of RH_DATA field

#define RESDATANONE     0       // invalid data
#define RESDATA2LSB     1       // Little Endian data encoding
#define RESDATA2MSB     2       // Bit Endian data encoding

// contents of RH_VERSION field

#define RV_NONE         0       // invalid version
#define RV_CURRENT      1       // current version.  will change in future.

// resource item

#define RI_NIDENT       4

typedef struct {
    unsigned char       ri_ident[RI_NIDENT];
    Elf32_Word          ri_type;
    Elf32_Off           ri_typename;
    Elf32_Word          ri_ordinal;
    Elf32_Off           ri_ordname;
    Elf32_Off           ri_data;
    Elf32_Word          ri_flags;
    Elf32_Word          ri_size;
    Elf32_Word          ri_reserved;
} Elf32_Ritem;

// ri_ident field indices

#define RI_VERSION      0       // version
#define RI_PAD          1       // start of padding bytes - set to 0

// contents of RI_VERSION field

#define IV_NONE         0       // invalid version
#define IV_CURRENT      1       // current version.  will change in future

// demangle information structure

typedef struct {
    Elf32_Word  idm_dllname;
    Elf32_Word  idm_initparms;
} Elf32_Demangle;

// default library structure

typedef struct {
    Elf32_Word  lib_name;
} Elf32_Library;

#pragma pack(pop);

#ifdef __cplusplus
};
#endif
#endif // _EXEELF_H_INCLUDED_
