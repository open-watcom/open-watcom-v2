/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Import library management routines.
*
****************************************************************************/


#define MAX_IMPORT_STRING      (_MAX_PATH + 512 + 80)

#define ELFBASEIMPORTSIZE 0x14A
#define ELFBASESTRTABSIZE 0x22

extern char ElfBase[];
extern char ElfOSInfo[];

#define ElfBase_SIZE            0x122
#define ElfOSInfo_SIZE          0x28

#define ELF_IMPORT_SYM_INFO             0x10
#define ELF_IMPORT_NAMED_SYM_INFO       0x15

#include "pushpck1.h"

typedef struct {
    unsigned_32         flags;
    unsigned_32         date;
    struct {
        unsigned_16     major;
        unsigned_16     minor;
    } version;
    unsigned_32         nameRVA;
    unsigned_32         ordBase;
    unsigned_32         numAddr;
    unsigned_32         numNamePointer;
    unsigned_32         AddrTableRVA;
    unsigned_32         NamePointerTableRVA;
    unsigned_32         OrdTableRVA;
} Coff32_Export;

typedef unsigned_32 Coff32_EName;
typedef unsigned_16 Coff32_EOrd;

#include "poppck.h"

extern bool     AddImport( arch_header *arch, libfile io );
extern void     OmfMKImport( arch_header *, importType, long, const char *, const char *, char *, processor_type );
extern void     CoffMKImport( arch_header *, importType, long, const char *, const char *, char *, processor_type );
extern void     ElfMKImport( arch_header *, importType, long, const char *, const char *, Elf32_Export *, Elf32_Sym *, processor_type );
extern size_t   CoffImportSize( import_sym * );
extern size_t   ElfImportSize( import_sym * );
extern void     CoffWriteImport( libfile, sym_file *, bool );
extern void     ElfWriteImport( libfile, sym_file * );
extern void     ProcessImport( char * );
