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
* Description:  Import library management routines.
*
****************************************************************************/


#define MAX_IMPORT_STRING      (_MAX_PATH + 512 + 80)

#define COFFBASEDESCRIPTORSIZE 573
#define COFFBASENULLDESCRIPTORSIZE 127
#define COFFBASENULLTHUNKSIZE 148
#define COFFBASEAXPIMPORTSIZE 385
#define COFFBASEPPCIMPORTSIZE 656
#define COFFBASEAXPNAMEDSIZE 484
#define COFFBASEPPCNAMEDSIZE 755

#define ELFBASEIMPORTSIZE 0x14A
#define ELFBASESTRTABSIZE 0x22

extern char CoffDescrip1[];
extern char CoffDescrip2[];
extern char CoffNullDescriptor[];
extern char CoffNullThunk[];
extern char CoffAXPOrdinal[];
extern char CoffPPCOrdinal[];
extern char CoffAXPNamed1[];
extern char CoffAXPNamed2[];
extern char CoffPPCNamed1[];
extern char CoffPPCNamed2[];

extern char ElfBase[];
extern char ElfOSInfo[];

#define CoffDescrip1_SIZE       0x174
#define CoffDescrip2_SIZE       0x7e
#define CoffNullDescriptor_SIZE 0x7d
#define CoffNullThunk_SIZE      0x7c
#define CoffAXPOrdinal_SIZE     0x15e
#define CoffPPCOrdinal_SIZE     0x26a
#define CoffAXPNamed1_SIZE      0xfa
#define CoffAXPNamed2_SIZE      0xc6
#define CoffPPCNamed1_SIZE      0x19a
#define CoffPPCNamed2_SIZE      0x132

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
