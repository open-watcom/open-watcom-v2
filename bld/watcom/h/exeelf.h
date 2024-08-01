/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Executable and Linkable Format (ELF) definitions.
*
****************************************************************************/


#ifndef _EXEELF_H_INCLUDED_
#define _EXEELF_H_INCLUDED_

#include <stdint.h>
#include "watcom.h"

// ELF scalar data types

typedef unsigned_32     Elf32_Addr;
typedef unsigned_16     Elf32_Half;
typedef unsigned_32     Elf32_Off;
typedef signed_32       Elf32_Sword;
typedef unsigned_32     Elf32_Word;

typedef unsigned_64     Elf64_Addr;
typedef uint16_t        Elf64_Half;
typedef unsigned_64     Elf64_Off;
typedef int32_t         Elf64_Sword;
typedef signed_64       Elf64_Sxword;
typedef uint32_t        Elf64_Word;
typedef unsigned_64     Elf64_Xword;

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

// ELF-64 header: identical to Elf32_Ehdr up to and including e_version
typedef struct {
    unsigned_8  e_ident[EI_NIDENT];     // signature & ID info
    Elf64_Half  e_type;                 // file type (i.e. obj file, exe file)
    Elf64_Half  e_machine;              // required architecture
    Elf64_Word  e_version;              // version of the file
    Elf64_Addr  e_entry;                // program entry point
    Elf64_Off   e_phoff;                // program header offset
    Elf64_Off   e_shoff;                // section header offset
    Elf64_Word  e_flags;                // processor specific flags
    Elf64_Half  e_ehsize;               // elf header size
    Elf64_Half  e_phentsize;            // program header entry size
    Elf64_Half  e_phnum;                // number of program header entries
    Elf64_Half  e_shentsize;            // section header entry size
    Elf64_Half  e_shnum;                // number of section header entries
    Elf64_Half  e_shstrndx;             // section name string table index.
} Elf64_Ehdr;

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
#define EI_OSABI        7       // OS/ABI identification
#define EI_ABIVERSION   8       // ABI version
#define EI_PAD          9       // start of padding bytes

// contents of the EI_CLASS field index

#define ELFCLASSNONE    0       // invalid class
#define ELFCLASS32      1       // 32-bit objects
#define ELFCLASS64      2       // 64-bit objects

// contents of the EI_DATA field index

#define ELFDATANONE     0       // invalid data encoding
#define ELFDATA2LSB     1       // "little-endian"
#define ELFDATA2MSB     2       // "big-endian"

// the current elf version number (EI_VERSION)

#define EV_CURRENT      1


// contents of the EI_OSABI field index

#define ELFOSABI_NONE       0   // No extensions or unspecified
#define ELFOSABI_HPUX       1   // Hewlett-Packard HP-UX
#define ELFOSABI_NETBSD     2   // NetBSD
#define ELFOSABI_LINUX      3   // Linux
#define ELFOSABI_HURD       4   // GNU-Hurd
#define ELFOSABI_86OPEN     5   // 86Open common IA32 ABI
#define ELFOSABI_SOLARIS    6   // Sun Solaris
#define ELFOSABI_AIX        7   // IBM AIX
#define ELFOSABI_IRIX       8   // SGI IRIX
#define ELFOSABI_FREEBSD    9   // FreeBSD
#define ELFOSABI_TRU64      10  // Compaq TRU64 UNIX
#define ELFOSABI_MODESTO    11  // Novell Modesto
#define ELFOSABI_OPENBSD    12  // Open BSD
#define ELFOSABI_OPENVMS    13  // Open VMS
#define ELFOSABI_NSK        14  // Hewlett-Packard Non-Stop Kernel
#define ELFOSABI_AROS       15  // Amiga Research OS
#define ELFOSABI_ARM_AEABI  64  // ARM EABI
#define ELFOSABI_ARM        97  // ARM
#define ELFOSABI_STANDALONE 255 // Standalone (embedded) application


// elf object file types

#define ET_NONE         0       // no file type
#define ET_REL          1       // relocatable file
#define ET_EXEC         2       // executable file
#define ET_DYN          3       // shared object file
#define ET_CORE         4       // core file
#define ET_LOPROC       0xff00  // processor specific (start)
#define ET_HIPROC       0xffff  // processor specific (end)

// elf machine types

#define EM_NONE            0    // No machine
#define EM_M32             1    // AT&T WE 32100
#define EM_SPARC           2    // Sun SPARC
#define EM_386             3    // Intel 80386
#define EM_68K             4    // Motorola 68000
#define EM_88K             5    // Motorola 88000
// was  EM_486             6    // Reserved
#define EM_860             7    // Intel 80860
#define EM_MIPS            8    // MIPS I Architecture
#define EM_S370            9    // IBM System/370 Processor
#define EM_MIPS_RS3_LE    10    // MIPS R3000 Little-endian
//                        11-14 // Reserved
#define EM_PARISC         15    // Hewlett-Packard PA-RISC
//                        16    // Reserved
#define EM_VPP500         17    // Fujitsu VPP500
#define EM_SPARC32PLUS    18    // Enhanced instruction set SPARC
#define EM_960            19    // Intel 80960
#define EM_PPC            20    // PowerPC
#define EM_PPC64          21    // 64-bit PowerPC
#define EM_S390           22    // IBM System/390 Processor
//                        23-35 // Reserved
#define EM_V800           36    // NEC V800
#define EM_FR20           37    // Fujitsu FR20
#define EM_RH32           38    // TRW RH-32
#define EM_RCE            39    // Motorola RCE
#define EM_ARM            40    // Advanced RISC Machines ARM
#define EM_ALPHA          41    // Digital Alpha
#define EM_SH             42    // Hitachi SH
#define EM_SPARCV9        43    // SPARC Version 9
#define EM_TRICORE        44    // Siemens TriCore embedded processor
#define EM_ARC            45    // Argonaut RISC Core, Argonaut Technologies Inc.
#define EM_H8_300         46    // Hitachi H8/300
#define EM_H8_300H        47    // Hitachi H8/300H
#define EM_H8S            48    // Hitachi H8S
#define EM_H8_500         49    // Hitachi H8/500
#define EM_IA_64          50    // Intel IA-64 processor architecture
#define EM_MIPS_X         51    // Stanford MIPS-X
#define EM_COLDFIRE       52    // Motorola ColdFire
#define EM_68HC12         53    // Motorola M68HC12
#define EM_MMA            54    // Fujitsu MMA Multimedia Accelerator
#define EM_PCP            55    // Siemens PCP
#define EM_NCPU           56    // Sony nCPU embedded RISC processor
#define EM_NDR1           57    // Denso NDR1 microprocessor
#define EM_STARCORE       58    // Motorola Star*Core processor
#define EM_ME16           59    // Toyota ME16 processor
#define EM_ST100          60    // STMicroelectronics ST100 processor
#define EM_TINYJ          61    // Advanced Logic Corp. TinyJ embedded processor family
#define EM_X86_64         62    // AMD x86-64 architecture
#define EM_PDSP           63    // Sony DSP Processor
#define EM_PDP10          64    // Digital Equipment Corp. PDP-10
#define EM_PDP11          65    // Digital Equipment Corp. PDP-11
#define EM_FX66           66    // Siemens FX66 microcontroller
#define EM_ST9PLUS        67    // STMicroelectronics ST9+ 8/16 bit microcontroller
#define EM_ST7            68    // STMicroelectronics ST7 8-bit microcontroller
#define EM_68HC16         69    // Motorola MC68HC16 Microcontroller
#define EM_68HC11         70    // Motorola MC68HC11 Microcontroller
#define EM_68HC08         71    // Motorola MC68HC08 Microcontroller
#define EM_68HC05         72    // Motorola MC68HC05 Microcontroller
#define EM_SVX            73    // Silicon Graphics SVx
#define EM_ST19           74    // STMicroelectronics ST19 8-bit microcontroller
#define EM_VAX            75    // Digital VAX
#define EM_CRIS           76    // Axis Communications 32-bit embedded processor
#define EM_JAVELIN        77    // Infineon Technologies 32-bit embedded processor
#define EM_FIREPATH       78    // Element 14 64-bit DSP Processor
#define EM_ZSP            79    // LSI Logic 16-bit DSP Processor
#define EM_MMIX           80    // Donald Knuth's educational 64-bit processor
#define EM_HUANY          81    // Harvard University machine-independent object files
#define EM_PRISM          82    // SiTera Prism
#define EM_AVR            83    // Atmel AVR 8-bit microcontroller
#define EM_FR30           84    // Fujitsu FR30
#define EM_D10V           85    // Mitsubishi D10V
#define EM_D30V           86    // Mitsubishi D30V
#define EM_V850           87    // NEC v850
#define EM_M32R           88    // Mitsubishi M32R
#define EM_MN10300        89    // Matsushita MN10300
#define EM_MN10200        90    // Matsushita MN10200
#define EM_PJ             91    // picoJava
#define EM_OPENRISC       92    // OpenRISC 32-bit embedded processor
#define EM_ARC_A5         93    // ARC Cores Tangent-A5
#define EM_XTENSA         94    // Tensilica Xtensa Architecture
#define EM_VIDEOCORE      95    // Alphamosaic VideoCore processor
#define EM_TMM_GPP        96    // Thomson Multimedia General Purpose Processor
#define EM_NS32K          97    // National Semiconductor 32000 series
#define EM_TPC            98    // Tenor Network TPC processor
#define EM_SNP1K          99    // Trebia SNP 1000 processor
#define EM_ST200         100    // STMicroelectronics (www.st.com) ST200 microcontroller
#define EM_IP2K          101    // Ubicom IP2xxx microcontroller family
#define EM_MAX           102    // MAX Processor
#define EM_CR            103    // National Semiconductor CompactRISC microprocessor
#define EM_F2MC16        104    // Fujitsu F2MC16
#define EM_MSP430        105    // Texas Instruments embedded microcontroller msp430
#define EM_BLACKFIN      106    // Analog Devices Blackfin (DSP) processor
#define EM_SE_C33        107    // S1C33 Family of Seiko Epson processors
#define EM_SEP           108    // Sharp embedded microprocessor
#define EM_ARCA          109    // Arca RISC Microprocessor
#define EM_UNICORE       110    // Microprocessor series from PKU-Unity Ltd. and MPRC of Peking University
#define EM_EXCESS        111    // eXcess 16/32/64-bit configurable embedded CPU
#define EM_DXP           112    // Icera Semiconductor Inc. Deep Execution Processor
#define EM_ALTERA_NIOS2  113    // Altera Nios II soft-core processor
#define EM_CRX           114    // National Semiconductor CompactRISC CRX microprocessor
#define EM_XGATE         115    // Motorola XGATE embedded processor
#define EM_C166          116    // Infineon C16x/XC16x processor
#define EM_M16C          117    // Renesas M16C series microprocessors
#define EM_DSPIC30F      118    // Microchip Technology dsPIC30F Digital Signal Controller
#define EM_CE            119    // Freescale Communication Engine RISC core
#define EM_M32C          120    // Renesas M32C series microprocessors
#define EM_TSK3000       131    // Altium TSK3000 core
#define EM_RS08          132    // Freescale RS08 embedded processor
#define EM_SHARC         133    // Analog Devices SHARC family of 32-bit DSP processors
#define EM_ECOG2         134    // Cyan Technology eCOG2 microprocessor
#define EM_SCORE7        135    // Sunplus S+core7 RISC processor
#define EM_DSP24         136    // New Japan Radio (NJR) 24-bit DSP Processor
#define EM_VIDEOCORE3    137    // Broadcom VideoCore III processor
#define EM_LATTICEMICO32 138    // RISC processor for Lattice FPGA architecture
#define EM_SE_C17        139    // Seiko Epson C17 family
#define EM_TI_C6000      140    // The Texas Instruments TMS320C6000 DSP family
#define EM_TI_C2000      141    // The Texas Instruments TMS320C2000 DSP family
#define EM_TI_C5500      142    // The Texas Instruments TMS320C55x  DSP family
#define EM_TI_ARP32      143    // Texas Instruments Application Specific RISC Processor, 32bit fetch
#define EM_TI_PRU        144    // Texas Instruments Programmable Realtime Unit
#define EM_MMDSP_PLUS    160    // STMicroelectronics 64bit VLIW Data Signal Processor
#define EM_CYPRESS_M8C   161    // Cypress M8C microprocessor
#define EM_R32C          162    // Renesas R32C series microprocessors
#define EM_TRIMEDIA      163    // NXP Semiconductors TriMedia architecture family
#define EM_QDSP6         164    // QUALCOMM DSP6 Processor
#define EM_8051          165    // Intel 8051 and variants
#define EM_STXP7X        166    // STMicroelectronics ST*P7* family of configurable and extensible RISC processors
#define EM_NDS32         167    // Andes Technology compact code size embedded RISC processor family
#define EM_ECOG1         168    // Cyan Technology eCOG1X family
#define EM_ECOG1X        168    // Cyan Technology eCOG1X family
#define EM_MAXQ30        169    // Dallas Semiconductor MAXQ30 Core Micro-controllers
#define EM_XIMO16        170    // New Japan Radio (NJR) 16-bit DSP Processor
#define EM_MANIK         171    // M2000 Reconfigurable RISC Microprocessor
#define EM_CRAYNV2       172    // Cray Inc NV2 vector architecture
#define EM_RX            173    // Renesas RX family
#define EM_METAG         174    // Imagination Technologies META processor architecture
#define EM_MCST_ELBRUS   175    // MCST Elbrus general purpose hardware architecture
#define EM_ECOG16        176    // Cyan Technology eCOG16 family
#define EM_CR16          177    // National Semiconductor CompactRISC CR16 16-bit microprocessor
#define EM_ETPU          178    // Freescale Extended Time Processing Unit
#define EM_SLE9X         179    // Infineon Technologies SLE9X core
#define EM_L10M          180    // Intel L10M
#define EM_K10M          181    // Intel K10M
#define EM_AARCH64       183    // ARM 64-bit architecture (AARCH64)
#define EM_AVR32         185    // Atmel Corporation 32-bit microprocessor family
#define EM_STM8          186    // STMicroeletronics STM8 8-bit microcontroller
#define EM_TILE64        187    // Tilera TILE64 multicore architecture family
#define EM_TILEPRO       188    // Tilera TILEPro multicore architecture family
#define EM_MICROBLAZE    189    // Xilinx MicroBlaze 32-bit RISC soft processor core
#define EM_CUDA          190    // NVIDIA CUDA architecture
#define EM_TILEGX        191    // Tilera TILE-Gx multicore architecture family
#define EM_CLOUDSHIELD   192    // CloudShield architecture family
#define EM_COREA_1ST     193    // KIPO-KAIST Core-A 1st generation processor family
#define EM_COREA_2ND     194    // KIPO-KAIST Core-A 2nd generation processor family
#define EM_ARC_COMPACT2  195    // Synopsys ARCompact V2
#define EM_OPEN8         196    // Open8 8-bit RISC soft processor core
#define EM_RL78          197    // Renesas RL78 family
#define EM_VIDEOCORE5    198    // Broadcom VideoCore V processor
#define EM_78KOR         199    // Renesas 78KOR family
#define EM_56800EX       200    // Freescale 56800EX Digital Signal Controller
#define EM_BA1           201    // Beyond BA1 CPU architecture
#define EM_BA2           202    // Beyond BA2 CPU architecture
#define EM_XCORE         203    // XMOS xCORE processor family
#define EM_MCHP_PIC      204    // Microchip 8-bit PIC family
//                       205-209// Reserved by Intel
#define EM_KM32          210    // KM211 KM32 32-bit processor
#define EM_KMX32         211    // KM211 KMX32 32-bit processor
#define EM_KMX16         212    // KM211 KMX16 16-bit processor
#define EM_KMX8          213    // KM211 KMX8 8-bit processor
#define EM_KVARC         214    // KM211 KVARC processor
#define EM_CDP           215    // Paneve CDP architecture family
#define EM_COGE          216    // Cognitive Smart Memory Processor
#define EM_COOL          217    // Bluechip Systems CoolEngine
#define EM_NORC          218    // Nanoradio Optimized RISC
#define EM_CSR_KALIMBA   219    // CSR Kalimba architecture family
#define EM_Z80           220    // Zilog Z80
#define EM_VISIUM        221    // Controls and Data Services VISIUMcore processor
#define EM_FT32          222    // FTDI Chip FT32 high performance 32-bit RISC architecture
#define EM_MOXIE         223    // Moxie processor family
#define EM_AMDGPU        224    // AMD GPU architecture
#define EM_RISCV         243    // RISC-V


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
#define SHN_LOPROC      0xff00  // processor specific (start)
#define SHN_HIPROC      0xff1f  // processor specific (end)
#define SHN_LOOS        0xff20  // OS specific (start)
#define SHN_LOSUNW      0xff3f
#define SHN_SUNW_IGNORE 0xff3f
#define SHN_HIOS        0xff3f  // OS specific (end)
#define SHN_HISUNW      0xff3f
#define SHN_ABS         0xfff1  // references to this section are absolute
#define SHN_COMMON      0xfff2  // references to this section are common
#define SHN_MACHO_64    0xfffd  // mach-o64 direct string access
#define SHN_MACHO       0xfffd  // mach-o direct string access
#define SHN_XINDEX      0xffff  // extended section index
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

typedef struct {
    Elf64_Word  sh_name;        // name of the section
    Elf64_Word  sh_type;        // section type
    Elf64_Xword sh_flags;
    Elf64_Addr  sh_addr;        // starting address of section in image
    Elf64_Off   sh_offset;      // start of section in file
    Elf64_Xword sh_size;        // size of section in file.
    Elf64_Word  sh_link;        // multipurpose field   (based on type)
    Elf64_Word  sh_info;        // another multipurpose field (based on type)
    Elf64_Xword sh_addralign;   // address alignment
    Elf64_Xword sh_entsize;     // entry size for sects with fixed sized entries
} Elf64_Shdr;

// section types

#define SHT_NULL                     0  // inactive
#define SHT_PROGBITS                 1  // meaning defined by program
#define SHT_SYMTAB                   2  // symbol table
#define SHT_STRTAB                   3  // string table
#define SHT_RELA                     4  // reloc entries with explicit addends
#define SHT_HASH                     5  // symbol hash table
#define SHT_DYNAMIC                  6  // dynamic linking information
#define SHT_NOTE                     7  // comment information
#define SHT_NOBITS                   8  // like PROGBITS but no space in file.
#define SHT_REL                      9  // as RELA but no explicit addends
#define SHT_SHLIB                   10  // reserved but evil
#define SHT_DYNSYM                  11  // dynamic link symbol table
#define SHT_INIT_ARRAY              14  // array of constructors
#define SHT_FINI_ARRAY              15  // array of destructors
#define SHT_PREINIT_ARRAY           16  // array of pre-constructors
#define SHT_GROUP                   17  // section group
#define SHT_SYMTAB_SHNDX            18  // extended section indicies
#define SHT_NUM                     19  // number of defined types
#define SHT_LOOS            0x60000000  // OS specific (start)
#define SHT_OS              0x60000001  // info to identify target OS
#define SHT_IMPORTS         0x60000002  // info on refs to external symbols
#define SHT_EXPORTS         0x60000003  // info on symbols exported by ordinal
#define SHT_RES             0x60000004  // read-only resource data.
#define SHT_PROGFRAGS       0x60001001  // similar to SHT_PROGBITS
#define SHT_IDMDLL          0x60001002  // symbol name demangling information
#define SHT_DEFLIB          0x60001003  // default static libraries
#define SHT_LOSUNW          0x6ffffff1  // sun specific low bound
#define SHT_SUNW_symsort    0x6ffffff1
#define SHT_SUNW_tlssort    0x6ffffff2
#define SHT_SUNW_LDYNSYM    0x6ffffff3
#define SHT_SUNW_dof        0x6ffffff4
#define SHT_SUNW_cap        0x6ffffff5
#define SHT_GNU_ATTRIBUTES  0x6ffffff5  // object attributes
#define SHT_GNU_HASH        0x6ffffff6  // gnu style hash table
#define SHT_SUNW_SIGNATURE  0x6ffffff6
#define SHT_GNU_LIBLIST     0x6ffffff7  // prelink library list
#define SHT_SUNW_ANNOTATE   0x6ffffff7
#define SHT_SUNW_DEBUGSTR   0x6ffffff8
#define SHT_CHECKSUM        0x6ffffff8  // checksum for DSO content
#define SHT_SUNW_DEBUG      0x6ffffff9
#define SHT_SUNW_move       0x6ffffffa
#define SHT_SUNW_COMDAT     0x6ffffffb
#define SHT_SUNW_syminfo    0x6ffffffc
#define SHT_GNU_verdef      0x6ffffffd  // version definition section
#define SHT_GNU_verneed     0x6ffffffe  // version needs section
#define SHT_GNU_versym      0x6fffffff  // version symbol table
#define SHT_HISUNW          0x6fffffff  // sun specefic high bound
#define SHT_HIOS            0x6fffffff  // OS specific (end)
#define SHT_LOPROC          0x70000000  // processor specific (start)
#define SHT_X86_64_UNWIND   0x70000001  // contains entries for stack unwinding
#define SHT_HIPROC          0x7fffffff  // processor specific (end)
#define SHT_LOUSER          0x80000000  // user defined (start)
#define SHT_HIUSER          0xffffffff  // user defined (end)

// Old section types.  Readers should handle these, writers must use the above

#define SHT_OS_O        12              // info to identify target OS
#define SHT_IMPORTS_O   13              // info on refs to external symbols
#define SHT_EXPORTS_O   14              // info on symbols exported by ordinal
#define SHT_RES_O       15              // read-only resource data.

// sh_flags values

#define SHF_WRITE            0x00000001 // section writable during execution
#define SHF_ALLOC            0x00000002 // section occupies space during exec
#define SHF_EXECINSTR        0x00000004 // section contains code
#define SHF_MERGE            0x00000010 // might be merged
#define SHF_STRINGS          0x00000020 // contains 0 terminated strings
#define SHF_INFO_LINK        0x00000040 // sh_info contains SHT index
#define SHF_LINK_ORDER       0x00000080 // preserve order after combining
#define SHF_OS_NONCONFORMING 0x00000100 // non-std. os specific handling required
#define SHF_GROUP            0x00000200 // section is member of a group
#define SHF_TLS              0x00000400 // section hold thread specific data
#define SHF_COMPRESSED       0x00000800 // section with compressed data

#define SHF_MASKOS           0x0ff00000 // OS specific values
#define SHF_BEGIN            0x01000000 // section to be placed at the beginning
                                        // of like-named sections by static link
#define SHF_END              0x02000000 // same, end.

#define SHF_MASKPROC         0xf0000000 // processor specific flags
#define SHF_X86_64_LARGE     0x10000000 // section with more than 2GB
#define SHF_ALPHA_GPREL      0x10000000
#define SHF_ORDERED          0x40000000 // solaris: special ordering required
#define SHF_EXCLUDE          0x80000000 // solaris: section is excluded unless
                                        // referenced or allocated

// symbol table entry

typedef struct {
    Elf32_Word  st_name;        // symbol name index into string table
    Elf32_Addr  st_value;       // symbol "value"
    Elf32_Word  st_size;        // symbol size
    unsigned_8  st_info;        // symbol's type and binding attribs.
    unsigned_8  st_other;       // no meaning yet.
    Elf32_Half  st_shndx;       // section index
//  ToDo: Field is only available for arm and arm64 targets
//  uint32_t    st_arch_subinfo;// Needed for arm function info (mach-o)
} Elf32_Sym;

typedef struct {
    Elf64_Word  st_name;        // symbol name index into string table
    unsigned_8  st_info;        // symbol's type and binding attribs.
    unsigned_8  st_other;       // no meaning yet.
    Elf64_Half  st_shndx;       // section index
    Elf64_Addr  st_value;       // symbol "value"
    Elf64_Xword st_size;        // symbol size
//  ToDo: Field is only available for arm and arm64 targets
//  uint32_t    st_arch_subinfo;// Needed for arm function info (mach-o)
} Elf64_Sym;

// symbol info field contents

#define ELF32_ST_BIND(i)        ((i)>>4)                // get the "bind" subfield
#define ELF32_ST_TYPE(i)        ((i)&0x0f)              // get the type subfield
#define ELF32_ST_INFO(b,t)      (((b)<<4)+((t)&0x0f))   // make a new st_info

#define ELF64_ST_BIND(i)        ((i)>>4)                // get the "bind" subfield
#define ELF64_ST_TYPE(i)        ((i)&0x0f)              // get the type subfield
#define ELF64_ST_INFO(b,t)      (((b)<<4)+((t)&0x0f))   // make a new st_info

// bind subfield contents

#define STB_LOCAL       0       // symbol has local binding
#define STB_GLOBAL      1       // symbol has global binding
#define STB_WEAK        2       // symbol has weak binding
#define STB_LOOS        10      // OS specific semantics (start)
#define STB_HIOS        12      // OS specific semantics (end)
#define STB_LOPROC      13      // processor specific semantics (start)
#define STB_HIPROC      15      // processor specific semantics (end)

// type subfield contents

#define STT_NOTYPE      0       // not specified
#define STT_OBJECT      1       // symbol is a data object
#define STT_FUNC        2       // symbol is a code symbol
#define STT_SECTION     3       // symbol associated with a section
#define STT_FILE        4       // symbol gives name of the source file.
#define STT_COMMON      5       // symbol labels an uninitialized common block
#define STT_TLS         6       // symbol specifies a thread-local storage entity
#define STT_LOOS        10      // OS specific semantics (start)
#define STT_HIOS        12      // OS specific semantics (end)
#define STT_LOPROC      13      // processor specific semantics (start)
#define STT_HIPROC      15      // processor specific semantics (end)

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

typedef struct {
    Elf64_Addr  r_offset;       // place to apply reloc (from begin of section)
    Elf64_Xword r_info;         // symbol idx, and type of reloc
} Elf64_Rel;

typedef struct {
    Elf64_Addr  r_offset;       // place to apply reloc (from begin of section)
    Elf64_Xword r_info;         // symbol idx, and type of reloc
    Elf64_Sxword r_addend;      // value used as a basis for the reloc.
} Elf64_Rela;

// r_info field contents

#define ELF32_R_SYM(i)          ((i)>>8)                        // gets the symbol index
#define ELF32_R_TYPE(i)         ((unsigned_8)(i))               // gets the symbol type
#define ELF32_R_INFO(s,t)       (((s)<<8)+(unsigned_8)(t))      // make a new r_info

#define ELF64_R_SYM(i)          ((i)>>32)                       // gets the symbol index
#define ELF64_R_TYPE(i)         ((i)&0xffffffffL)               // gets the symbol type
#define ELF64_R_INFO(s,t)       (((s)<<32)+((t)&0xffffffffL))   // make a new r_info

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
#define R_386_32PLT             11

//X86_64
#define R_X86_64_NONE           0
#define R_X86_64_64             1
#define R_X86_64_PC32           2
#define R_X86_64_GOT32          3
#define R_X86_64_PLT32          4
#define R_X86_64_COPY           5
#define R_X86_64_GLOB_DAT       6
#define R_X86_64_JUMP_SLOT      7
#define R_X86_64_RELATIVE       8
#define R_X86_64_GOTPCREL       9
#define R_X86_64_32             10
#define R_X86_64_32S            11
#define R_X86_64_16             12
#define R_X86_64_PC16           13
#define R_X86_64_8              14
#define R_X86_64_PC8            15
#define R_X86_64_DPTMOD64       16
#define R_X86_64_DTPOFF64       17
#define R_X86_64_TPOFF64        18
#define R_X86_64_TLSGD          19
#define R_X86_64_TLSLD          20
#define R_X86_64_DTPOFF32       21
#define R_X86_64_GOTTPOFF       22
#define R_X86_64_TPOFF32        23
#define R_X86_64_PC64           24
#define R_X86_64_GOTOFF64       25
#define R_X86_64_GOTPC32        26
#define R_X86_64_SIZE32         32
#define R_X86_64_SIZE64         33

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
#define R_SPARC_64              32
#define R_SPARC_OLO10           33
#define R_SPARC_WDISP16         40
#define R_SPARC_WDISP19         41
#define R_SPARC_7               43
#define R_SPARC_5               44
#define R_SPARC_6               45
#define R_SPARC_DISP64              46
#define R_SPARC_PLT64               47
#define R_SPARC_HIX22               48
#define R_SPARC_LOX10               49
#define R_SPARC_H44                 50
#define R_SPARC_M44                 51
#define R_SPARC_L44                 52
#define R_SPARC_REGISTER            53
#define R_SPARC_UA64                54
#define R_SPARC_UA16                55
#define R_SPARC_GOTDATA_HIX22       80
#define R_SPARC_GOTDATA_LOX22       81
#define R_SPARC_GOTDATA_OP_HIX22    82
#define R_SPARC_GOTDATA_OP_LOX22    83
#define R_SPARC_GOTDATA_OP          84

// MIPS
#define R_MIPS_NONE             0
#define R_MIPS_16               1
#define R_MIPS_32               2
#define R_MIPS_REL32            3
#define R_MIPS_26               4
#define R_MIPS_HI16             5
#define R_MIPS_LO16             6
#define R_MIPS_GPREL16          7
#define R_MIPS_LITERAL          8
#define R_MIPS_GOT16            9
#define R_MIPS_PC16             10
#define R_MIPS_CALL16           11
#define R_MIPS_GPREL32          12
#define R_MIPS_GOTHI16          21
#define R_MIPS_GOTLO16          22
#define R_MIPS_CALLHI16         30
#define R_MIPS_CALLLO16         31

// Alpha
#define R_ALPHA_NONE            0
#define R_ALPHA_REFLONG         1
#define R_ALPHA_REFQUAD         2
#define R_ALPHA_GPREL32         3
#define R_ALPHA_LITERAL         4
#define R_ALPHA_LITUSE          5
#define R_ALPHA_GPDISP          6
#define R_ALPHA_BRADDR          7
#define R_ALPHA_HINT            8
#define R_ALPHA_SREL16          9
#define R_ALPHA_SREL32          10
#define R_ALPHA_SREL64          11
#define R_ALPHA_GPRELHIGH       17
#define R_ALPHA_GPRELLOW        18
#define R_ALPHA_GPREL16         19
#define R_ALPHA_COPY            24
#define R_ALPHA_GLOB_DAT        25
#define R_ALPHA_JMP_SLOT        26
#define R_ALPHA_RELATIVE        27
#define R_ALPHA_BRSGP           28
#define R_ALPHA_TLSGD           29
#define R_ALPHA_TLS_LDM         30
#define R_ALPHA_DTPMOD64        31
#define R_ALPHA_GOTDTPREL       32
#define R_ALPHA_DTPREL64        33
#define R_ALPHA_DTPRELHI        34
#define R_ALPHA_DTPRELLO        35
#define R_ALPHA_DTPREL16        36
#define R_ALPHA_GOTTPREL        37
#define R_ALPHA_TPREL64         38
#define R_ALPHA_TPRELHI         39
#define R_ALPHA_TPRELLO         40
#define R_ALPHA_TPREL16         41

// program header

typedef struct {
    Elf32_Word  p_type;         // type of segment
    Elf32_Off   p_offset;       // offset of segment from beginnning of file
    Elf32_Addr  p_vaddr;        // segment virtual address
    Elf32_Addr  p_paddr;        // segment physical address
    Elf32_Word  p_filesz;       // size of segment in file
    Elf32_Word  p_memsz;        // size of segment in memory
    Elf32_Word  p_flags;        // segment flags
    Elf32_Word  p_align;        // segment align value (in mem & file)
} Elf32_Phdr;

typedef struct {
    Elf64_Word  p_type;         // type of segment
    Elf64_Word  p_flags;        // segment flags
    Elf64_Off   p_offset;       // offset of segment from beginnning of file
    Elf64_Addr  p_vaddr;        // segment virtual address
    Elf64_Addr  p_paddr;        // segment physical address
    Elf64_Xword p_filesz;       // size of segment in file
    Elf64_Xword p_memsz;        // size of segment in memory
    Elf64_Xword p_align;        // segment align value (in mem & file)
} Elf64_Phdr;

// segment types

#define PT_NULL         0               // unused segment
#define PT_LOAD         1               // loadable segment
#define PT_DYNAMIC      2               // contains dynamic linking information
#define PT_INTERP       3               // reference to a program interpreter
#define PT_NOTE         4               // comments & auxiliary information
#define PT_SHLIB        5               // here be dragons
#define PT_PHDR         6               // address of prog. header in mem (for interp.)
#define PT_TLS          7               // thread local storage
#define PT_GNU_EH_FRAME 0x6474e550      // unwinding tables
#define PT_GNU_STACK    0x6474e551      // stack executability
#define PT_GNU_RELRO    0x6474e552      // readonly after reloc
#define PT_GNU_PROPERTY 0x6474e553      // GNU properties
#define PT_OS           0x60000001      // target os information
#define PT_RES          0x60000002      // read-only resource information
#define PT_LOPROC       0x70000000      // processor specific (start)
#define PT_HIPROC       0x7fffffff      // processor specific (end)

// Old segment types.  Readers should handle these, writers must use the above

#define PT_OS_O         7       // target os information
#define PT_RES_O        9       // read-only resource information

// elf segment flag bits

#define PF_X            0x1             // seg has execute permissions
#define PF_W            0x2             // seg has write permissions
#define PF_R            0x4             // seg has read permissions
#define PF_S            0x01000000      // segment is shared.
#define PF_MASKPROC     0xf0000000      // processor-specific flag mask

// note entry format

typedef struct {
    unsigned_32     n_namesz;   // length of name
    unsigned_32     n_descsz;   // length of descriptor
    unsigned_32     n_type;     // user defined "type" of the note
    //char            name[];   // variable length name
    //unsigned_32     desc[];   // descriptors go here
} Elf_Note;

// note types (used in core files)

#define NT_PRSTATUS     1       // process status
#define NT_FPREGSET     2       // floating point registers
#define NT_PRPSINFO     3       // process state info

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
#define DT_LOPROC       0x70000000      // processor specific (start)
#define DT_HIPROC       0x7FFFFFFF      // processor specific (end)

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

typedef unsigned long INITTERM( unsigned long modhandle, unsigned long flag );

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


// typedefs/structs/unions and macros to better manage Elf32 / Elf64 differences

typedef union {
    unsigned_8  e_ident[EI_NIDENT];
    Elf32_Ehdr  elf32;
    Elf64_Ehdr  elf64;
} Elfxx_Ehdr;

#define ELF32(x)    (x).elf32
#define ELF64(x)    (x).elf64

#define IS_ELF32(x) ((x).e_ident[EI_CLASS] == ELFCLASS32)
#define IS_ELF64(x) ((x).e_ident[EI_CLASS] == ELFCLASS64)

#endif // _EXEELF_H_INCLUDED_
