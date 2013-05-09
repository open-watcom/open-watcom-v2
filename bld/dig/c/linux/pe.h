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
* Description:  Internal header file describing the PE File Format
*               structures.
*
****************************************************************************/

#ifndef __PE_H
#define __PE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>

/*---------------------- Macros and type definitions ----------------------*/

#include "pushpck1.h"

/* Macro to round a value to a 4Kb boundary */

#define ROUND_4K(s) (((u_long)(s) + 0xFFF) & ~0xFFF)

/* Don't include these definitions if we have already included
 * <windows.h>
 */
#ifndef IMAGE_FILE_MACHINE_UNKNOWN

/* Flags for the machine type in the COFF file header */

#define IMAGE_FILE_MACHINE_UNKNOWN  0       /* Machine type is unknown  */
#define IMAGE_FILE_MACHINE_I386     0x14c   /* Intel 386 or later       */
#define IMAGE_FILE_MACHINE_R3000    0x162   /* MIPS R3000 little endian */
#define IMAGE_FILE_MACHINE_R4000    0x166   /* MIPS R4000 little endian */
#define IMAGE_FILE_MACHINE_R10000   0x168   /* MIPS R10000 little endian*/
#define IMAGE_FILE_MACHINE_ALPHA    0x184   /* DEC Alpha AXP            */
#define IMAGE_FILE_MACHINE_M68K     0x268   /* Motorola 68000 series    */
#define IMAGE_FILE_MACHINE_POWERPC  0x1F0   /* Power PC, little endian  */
#define IMAGE_FILE_MACHINE_SH3      0x1a2   /* Hitachi SH3              */
#define IMAGE_FILE_MACHINE_SH4      0x1a6   /* Hitachi SH4              */
#define IMAGE_FILE_MACHINE_ARM      0x1c0   /* ARM Processor            */

/* Flags for the characteristic in the COFF file header */

#define IMAGE_FILE_RELOCS_STRIPPED          0x0001
#define IMAGE_FILE_EXECUTABLE_IMAGE         0x0002
#define IMAGE_FILE_LINE_NUMS_STRIPPED       0x0004
#define IMAGE_FILE_LOCAL_SYMS_STRIPPED      0x0008
#define IMAGE_FILE_AGGRESSIVE_WS_TRIM       0x0010
#define IMAGE_FILE_LARGE_ADDRESS_AWARE      0x0020
#define IMAGE_FILE_16BIT_MACHINE            0x0040
#define IMAGE_FILE_BYTES_REVERSED_LO        0x0080
#define IMAGE_FILE_32BIT_MACHINE            0x0100
#define IMAGE_FILE_DEBUG_STRIPPED           0x0200
#define IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP  0x0400
#define IMAGE_FILE_SYSTEM                   0x1000
#define IMAGE_FILE_DLL                      0x2000
#define IMAGE_FILE_UP_SYSTEM_ONLY           0x4000
#define IMAGE_FILE_BYTES_REVERSED_HI        0x8000

/* Definitions for the Characteristics for the section header */

#define IMAGE_SCN_TYPE_REG                  0x00000000
#define IMAGE_SCN_TYPE_DSECT                0x00000001
#define IMAGE_SCN_TYPE_NOLOAD               0x00000002
#define IMAGE_SCN_TYPE_GROUP                0x00000004
#define IMAGE_SCN_TYPE_NO_PAD               0x00000008
#define IMAGE_SCN_TYPE_COPY                 0x00000010
#define IMAGE_SCN_CNT_CODE                  0x00000020
#define IMAGE_SCN_CNT_INITIALIZED_DATA      0x00000040
#define IMAGE_SCN_CNT_UNINITIALIZED_DATA    0x00000080
#define IMAGE_SCN_LNK_OTHER                 0x00000100
#define IMAGE_SCN_LNK_INFO                  0x00000200
#define IMAGE_SCN_TYPE_OVER                 0x00000400
#define IMAGE_SCN_LNK_REMOVE                0x00000800
#define IMAGE_SCN_LNK_COMDAT                0x00001000
#define IMAGE_SCN_MEM_FARDATA               0x00008000
#define IMAGE_SCN_MEM_PURGEABLE             0x00020000
#define IMAGE_SCN_MEM_16BIT                 0x00020000
#define IMAGE_SCN_MEM_LOCKED                0x00040000
#define IMAGE_SCN_MEM_PRELOAD               0x00080000
#define IMAGE_SCN_ALIGN_1BYTES              0x00100000
#define IMAGE_SCN_ALIGN_2BYTES              0x00200000
#define IMAGE_SCN_ALIGN_4BYTES              0x00300000
#define IMAGE_SCN_ALIGN_8BYTES              0x00400000
#define IMAGE_SCN_ALIGN_16BYTES             0x00500000
#define IMAGE_SCN_ALIGN_32BYTES             0x00600000
#define IMAGE_SCN_ALIGN_64BYTES             0x00700000
#define IMAGE_SCN_LNK_OVFL                  0x01000000
#define IMAGE_SCN_MEM_DISCARDABLE           0x02000000
#define IMAGE_SCN_MEM_NOT_CACHED            0x04000000
#define IMAGE_SCN_MEM_NOT_PAGED             0x08000000
#define IMAGE_SCN_MEM_SHARED                0x10000000
#define IMAGE_SCN_MEM_EXECUTE               0x20000000
#define IMAGE_SCN_MEM_READ                  0x40000000
#define IMAGE_SCN_MEM_WRITE                 0x80000000

/* Defines for the type field of the RELOC structure (top 4 bits) */

#define IMAGE_REL_BASED_ABSOLUTE        0
#define IMAGE_REL_BASED_HIGH            1
#define IMAGE_REL_BASED_LOW             2
#define IMAGE_REL_BASED_HIGHLOW         3
#define IMAGE_REL_BASED_HIGHADJ         4
#define IMAGE_REL_BASED_MIPS_JMPADDR    5

/* Definitions for the SubSystem field of the optional header */

#define IMAGE_SUBSYSTEM_UNKNOWN         0   /* Unknown subsystem        */
#define IMAGE_SUBSYSTEM_NATIVE          1   /* Device drivers           */
#define IMAGE_SUBSYSTEM_WINDOWS_GUI     2   /* Windows GUI              */
#define IMAGE_SUBSYSTEM_WINDOWS_CUI     3   /* Windows Console          */
#define IMAGE_SUBSYSTEM_POSIX_CUI       7   /* POSIX Console            */
#define IMAGE_SUBSYSTEM_WINDOWS_CE_GUI  9   /* Windows CE GUI           */

/* Definitions for the DllCharacteristics field of the optional header */

#define IMAGE_DLLCHARACTERISTICS_WDM_DRIVER 0x2000  /* WDM device driver*/

/* Offsets of the data directories in optional header */

#define DIR_EXPORT          0
#define DIR_IMPORT          1
#define DIR_RESOURCE        2
#define DIR_EXCEPTION       3
#define DIR_CERTIFICATE     4
#define DIR_BASERELOC       5
#define DIR_DEBUG           6
#define DIR_ARCHITECTURE    7
#define DIR_GLOBALPTR       8
#define DIR_TLS             9
#define DIR_LOAD_CONFIG     10
#define DIR_BOUNDIMPORT     11
#define DIR_IAT             12
#define DIR_DELAYIMPORT     13

#endif

/* Structure defining the MS-DOS executeable file header */

typedef struct {
    u_short signature;
    u_short low;
    u_short high;
    u_short reloc;
    u_short hdr_para;
    } EXE_HDR;

/* Structure defining the COFF file header */

typedef struct {
    u_short Machine;
    u_short NumberOfSections;
    u_long  TimeDateStamp;
    u_long  PointerToSymbolTable;
    u_long  NumberOfSynbols;
    u_short SizeOfOptionalHeader;
    u_short Characteristics;
    } FILE_HDR;

/* Structure of an entry in the data directory */

typedef struct {
    u_long  RelVirtualAddress;
    u_long  Size;
    } DATA_DIRECTORY;

/* Structure defining the optional PE header for Win32 */

typedef struct {
    u_short         Magic;
    u_char          MajorLinkerVersion;
    u_char          MinorLinkerVersion;
    u_long          SizeOfCode;
    u_long          SizeOfInitializedData;
    u_long          SizeOfUninitializedData;
    u_long          AddressOfEntryPoint;
    u_long          BaseOfCode;
    u_long          BaseOfData;
    u_long          ImageBase;
    u_long          SectionAlignment;
    u_long          FileAlignment;
    u_short         MajorOperatingSystemVersion;
    u_short         MinorOperatingSystemVersion;
    u_short         MajorImageVersion;
    u_short         MinorImageVersion;
    u_short         MajorSubsystemVersion;
    u_short         MinorSubsystemVersion;
    u_long          Reserved1;
    u_long          SizeOfImage;
    u_long          SizeOfHeaders;
    u_long          CheckSum;
    u_short         Subsystem;
    u_short         DllCharacteristics;
    u_long          SizeOfStackReserve;
    u_long          SizeOfStackCommit;
    u_long          SizeOfHeapReserve;
    u_long          SizeOfHeapCommit;
    u_long          LoaderFlags;
    u_long          NumberOfRvaAndSizes;
    DATA_DIRECTORY  DataDirectory[16];
    } OPTIONAL_HDR;

typedef struct {
    char        Name[8];
    u_long      VirtualSize;
    u_long      VirtualAddress;
    u_long      SizeOfRawData;
    u_long      PointerToRawData;
    u_long      PointerToRelocations;
    u_long      PointerToLineNumbers;
    u_short     NumberOfRelocations;
    u_short     NumberOfLineNumbers;
    u_long      Characteristics;
    } SECTION_HDR;

/* Structure defining a fixup table entry block in the .reloc section */

typedef struct {
    u_long  PageRVA;
    u_long  BlockSize;
    } BASE_RELOCATION;

/* Structure defining the export directory table */

typedef struct {
    u_long  ExportFlags;
    u_long  TimeDateStamp;
    u_short MajorVersion;
    u_short MinorVersion;
    u_long  NameRVA;
    u_long  OrdinalBase;
    u_long  AddressTableEntries;
    u_long  NumberOfNamePointers;
    u_long  AddressTableRVA;
    u_long  NameTableRVA;
    u_long  OrdinalTableRVA;
    } EXPORT_DIRECTORY;

#include "poppck.h"

#endif  /* __PE_H */

