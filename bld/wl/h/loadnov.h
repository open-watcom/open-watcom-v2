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


// linker specific structures for novell load files

#pragma pack(push,1);

#ifdef _EXENOV_H
// this is the portion of the nlm_header which is of a fixed size.
typedef struct fixed_header {
    char            signature[ sizeof( NLM_SIGNATURE ) - 1 ];
    unsigned_32     version;
    char            moduleName[14];     // 1st character is a length byte.
    unsigned_32     codeImageOffset;
    unsigned_32     codeImageSize;
    unsigned_32     dataImageOffset;
    unsigned_32     dataImageSize;
    unsigned_32     uninitializedDataSize;
    unsigned_32     customDataOffset;
    unsigned_32     customDataSize;
    unsigned_32     moduleDependencyOffset;      // temporarily stackSize for the C Lib */
    unsigned_32     numberOfModuleDependencies;
    unsigned_32     relocationFixupOffset;
    unsigned_32     numberOfRelocationFixups;
    unsigned_32     externalReferencesOffset;
    unsigned_32     numberOfExternalReferences;
    unsigned_32     publicsOffset;
    unsigned_32     numberOfPublics;
    unsigned_32     debugInfoOffset;
    unsigned_32     numberOfDebugRecords;
    unsigned_32     codeStartOffset;
    unsigned_32     exitProcedureOffset;
    unsigned_32     checkUnloadProcedureOffset;
    unsigned_32     moduleType;
    unsigned_32     flags;
    unsigned_8      descriptionLength;
} fixed_header;

/* the second "fixed chunk" of the header */

typedef struct {
    char            versionSignature[ VERSION_SIGNATURE_LENGTH ];
    unsigned_32     majorVersion;
    unsigned_32     minorVersion;
    unsigned_32     revision;
    unsigned_32     year;
    unsigned_32     month;      /* starts at 1 */
    unsigned_32     day;
} fixed_hdr_2;

typedef struct {
    char            copyrightSignature[ COPYRIGHT_SIGNATURE_LENGTH ];
} fixed_hdr_3;

#endif

// NOTE: This assumes sizeof( virt_mem ) == sizeof( unsigned_32 ) !!!!!!!

#define MAX_IMP_INTERNAL  10  // maximum # of import relocs stored internally
                              // before allocating virtual memory.
#define MAX_IMP_VIRT 9       // maximum # of virt blocks before allocating more.
#define IMP_VIRT_ALLOC_SIZE 512     // size of virtual mem block to allocate
#define IMP_NUM_VIRT (IMP_VIRT_ALLOC_SIZE / sizeof( unsigned_32 ) )

typedef struct nov_import {
    byte        contents;       // # of relocs || # of virtmem blocks + 10
    unsigned_32 num_relocs;     // number of relocs allocated.
    virt_mem    addr[1];        // variable # of pointers virtual mem. hunks
} nov_import;

// values for upper bits of the relocation offsets.
#define NOV_IMP_NONRELATIVE  0x80000000
#define NOV_IMP_ISCODE       0x40000000

#define NOV_EXP_ISCODE      0x80000000

#define DO_NOV_EXPORTS          0x80
#define DO_WATCOM_EXPORTS       0x40
#define DO_NOV_REF_ONLY         0x20
#define NOV_EXE_FLAG_MASK       0x1F


extern void     NovDBIAddGlobal( symbol * );
extern void     NovDBIAddrStart( void );
extern void     NovDBIGenGlobal( symbol * );
extern void     FiniNovellLoadFile( void );
extern void     AddNovImpReloc( symbol *, unsigned_32, bool, bool );
extern void     FindExportedSyms( void );

#pragma pack(pop);

