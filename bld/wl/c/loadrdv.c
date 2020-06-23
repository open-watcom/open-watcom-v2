/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Routines for creation of RDOS device driver files
*
****************************************************************************/

#include <string.h>
#include "linkstd.h"
#include <exerdos.h>
#include "pcobj.h"
#include "newmem.h"
#include "msg.h"
#include "alloc.h"
#include "reloc.h"
#include "wlnkmsg.h"
#include "virtmem.h"
#include "fileio.h"
#include "overlays.h"
#include "loadfile.h"
#include "objcalc.h"
#include "ring.h"
#include "dbgall.h"
#include "loadrdv.h"


#ifdef _RDOS

#define MB_BASE     0x120000

static unsigned_32  CodeSize = 0;
static unsigned_32  DataSize = 0;
static unsigned_32  HeaderSize = 0;

struct mb_header {
    unsigned_32 mb_magic;
    unsigned_32 mb_flags;
    unsigned_32 mb_checksum;
    unsigned_32 mb_header_addr;
    unsigned_32 mb_load_addr;
    unsigned_32 mb_load_end_addr;
    unsigned_32 mb_bss_end_addr;
    unsigned_32 mb_entry_addr;
};

static void WriteBinData( void )
/*******************************
 * copy code from extra memory to loadfile
 */
{
    group_entry         *group;

    DEBUG(( DBG_BASE, "Writing data" ));
    OrderGroups( CompareDosSegments );

    Root->outfile->file_loc = Root->u.file_loc;
    Root->sect_addr = Groups->grp_addr;

    /* write groups and relocations */
    for( group = Groups; group != NULL; group = group->next_group ) {
        CurrSect = group->section;  // needed for WriteInfo.
        if( group->totalsize ) {
            WriteGroup( group );
            CodeSize += group->totalsize;
        }
    }
}

static void WriteRDOSCode( void )
/********************************
 * copy driver code from extra memory
 * to loadfile
 */
{
    group_entry         *group;
    struct seg_leader   *leader;
    SEGDATA             *piece;
    bool                iscode;

    DEBUG(( DBG_BASE, "Writing code" ));
    OrderGroups( CompareDosSegments );

    Root->outfile->file_loc = Root->u.file_loc;
    Root->sect_addr = Groups->grp_addr;
    leader = NULL;

    /* write groups and relocations */
    iscode = false;
    for( group = Groups; group != NULL; group = group->next_group ) {
        if( leader != group->leaders ) {
            iscode = false;
            leader = group->leaders;
            if( leader != NULL && leader->size ) {
                piece = leader->pieces;
                if( piece != NULL ) {
                    if( piece->iscode && ( leader->seg_addr.seg == FmtData.u.rdos.code_seg ) ) {
                        iscode = true;
                    }
                }
            }
        }
        CurrSect = group->section;  // needed for WriteInfo.
        if( iscode ) {
            CurrSect->u.file_loc = HeaderSize + CodeSize;
            WriteGroup( group );
            if( group->totalsize > group->size ) {
                PadLoad( group->totalsize - group->size );
            }
            CodeSize += group->totalsize;
        }
    }
}

static void WriteRDOSData( void )
/********************************
 * copy driver data from extra memory
 * to loadfile
 */
{
    group_entry         *group;
    struct seg_leader   *leader;
    SEGDATA             *piece;
    bool                isdata;

    DEBUG(( DBG_BASE, "Writing data" ));

    /* write groups and relocations */
    leader = NULL;
    isdata = false;
    for( group = Groups; group != NULL; group = group->next_group ) {
        if( leader != group->leaders ) {
            isdata = false;
            leader = group->leaders;
            if( leader != NULL && leader->size ) {
                piece = leader->pieces;
                if( piece != NULL ) {
                    if( ( piece->isidata || piece->isuninit ) && ( leader->seg_addr.seg == FmtData.u.rdos.data_seg ) ) {
                        isdata = true;
                    }
                }
            }
        }
        CurrSect = group->section;  // needed for WriteInfo.
        if( isdata ) {
            CurrSect->u.file_loc = HeaderSize + CodeSize + DataSize;
            if( StackSegPtr != NULL ) {
                if( group->totalsize - group->size < StackSize ) {
                    StackSize = group->totalsize - group->size;
                    group->totalsize = group->size;
                } else {
                    group->totalsize -= StackSize;
                }
            }
            WriteGroup( group );
            if( group->totalsize > group->size ) {
                PadLoad( group->totalsize - group->size );
            }
            DataSize += group->totalsize;
        }
    }
}

void GetRdosSegs( void )
/***********************
 * resolve RDOS code & data segments
 */
{
    group_entry         *group;
    struct seg_leader   *leader;
    SEGDATA             *piece;

    leader = NULL;
    for( group = Groups; group != NULL; group = group->next_group ) {
        if( leader != group->leaders ) {
            leader = group->leaders;
            if( leader != NULL && leader->size ) {
                piece = leader->pieces;
                if( piece != NULL ) {
                    if( piece->iscode )
                        FmtData.u.rdos.code_seg = leader->seg_addr.seg;
                    if( piece->isidata || piece->isuninit ) {
                        FmtData.u.rdos.data_seg = leader->seg_addr.seg;
                    }
                }
            }
        }
    }
}

static void WriteHeader16( void )
/********************************
 * write 16-bit device header
 */
{
    rdos_dev16_header   exe_head;

    SeekLoad( 0 );
    _HostU16toTarg( RDOS_SIGNATURE_16, exe_head.signature );
    _HostU16toTarg( StartInfo.addr.off, exe_head.IP );
    _HostU16toTarg( CodeSize, exe_head.code_size );
    _HostU16toTarg( FmtData.u.rdos.code_sel, exe_head.code_sel );
    _HostU16toTarg( DataSize, exe_head.data_size );
    _HostU16toTarg( FmtData.u.rdos.data_sel, exe_head.data_sel );
    WriteLoad( &exe_head, sizeof( rdos_dev16_header ) );
}

static void WriteHeader32( void )
/********************************
 * write 32-bit device header
 */
{
    rdos_dev32_header   exe_head;

    SeekLoad( 0 );
    _HostU16toTarg( RDOS_SIGNATURE_32, exe_head.signature );
    _HostU32toTarg( StartInfo.addr.off, exe_head.EIP );
    _HostU32toTarg( CodeSize, exe_head.code_size );
    _HostU16toTarg( FmtData.u.rdos.code_sel, exe_head.code_sel );
    _HostU32toTarg( DataSize, exe_head.data_size );
    _HostU16toTarg( FmtData.u.rdos.data_sel, exe_head.data_sel );
    WriteLoad( &exe_head, sizeof( rdos_dev32_header ) );
}

static void WriteMbootHeader( void )
/***********************************
 * write multiboot header
 */
{
    struct mb_header   mb_head;
    unsigned_32        temp32;
    unsigned_32        linear;

    SeekLoad( 0 );
    _HostU32toTarg( 0x1BADB002, mb_head.mb_magic );
    _HostU32toTarg( 0x00010003, mb_head.mb_flags );
    _HostU32toTarg( 0xE4514FFB, mb_head.mb_checksum );
    _HostU32toTarg( MB_BASE, mb_head.mb_header_addr );
    _HostU32toTarg( MB_BASE, mb_head.mb_load_addr );
    linear = MB_BASE + CodeSize + sizeof( struct mb_header );
    _HostU32toTarg( linear, mb_head.mb_load_end_addr );
    _HostU32toTarg( linear, mb_head.mb_bss_end_addr );
    temp32 = MB_BASE + StartInfo.addr.off + sizeof( struct mb_header );
    _HostU32toTarg( temp32, mb_head.mb_entry_addr );
    WriteLoad( &mb_head, sizeof( struct mb_header ) );
}

static size_t getHeaderSize( void )
/**********************************
 * get header size for appropriate
 * RDOS format
 */
{
    size_t  size;

    if( FmtData.u.rdos.mboot ) {
        size = sizeof( struct mb_header );
    } else if( FmtData.u.rdos.driver ) {
        if( LinkState & LS_FMT_SEEN_32BIT ) {
            size = sizeof( rdos_dev32_header );
        } else {
            size = sizeof( rdos_dev16_header );
        }
    } else {
        size = 0;
    }
    return( size );
}

static void writeHeader( void )
/**********************************
 * if required then write header for
 * appropriate RDOS format
 */
{
    if( FmtData.u.rdos.mboot ) {
        WriteMbootHeader();
    } else if( FmtData.u.rdos.driver ) {
        if( LinkState & LS_FMT_SEEN_32BIT ) {
            CodeSize += 0x10;   // this is a fix to make offsets into data segment correct
            WriteHeader32();
        } else {
            WriteHeader16();
        }
    } else {
        /* nothing to write */
    }
}

void FiniRdosLoadFile( void )
/****************************
 * finish writing of load file
 */
{
    if( (FmtData.type & MK_RDOS_16) && (LinkState & LS_FMT_SEEN_32BIT) ) {
        LnkMsg( WRN+LOC+MSG_CANNOT_HAVE_16_AND_32, NULL );
    }
    HeaderSize = getHeaderSize();
    SeekLoad( HeaderSize );
    Root->u.file_loc = HeaderSize;
    if( FmtData.u.rdos.driver ) {
        WriteRDOSCode();
        WriteRDOSData();
    } else {
        WriteBinData();
    }
    DBIWrite();
    writeHeader();
}

#endif
