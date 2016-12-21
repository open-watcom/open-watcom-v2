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
* Description:  Routines for creation of RDOS device driver files
*
****************************************************************************/

#include <string.h>
#include "linkstd.h"
#include <exerdos.h>
#include "command.h"
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

static unsigned_32  CodeSize = 0;
static unsigned_32  DataSize = 0;
static unsigned_32  HeaderSize = 0;


#define MB_BASE 0x120000

struct mb_header
{
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
/**********************************************************/
/* copy code from extra memory to loadfile */
{
    group_entry         *group;
    SECTION             *sect;

    DEBUG(( DBG_BASE, "Writing data" ));
    OrderGroups( CompareDosSegments );
    CurrSect = Root;        // needed for WriteInfo.

    Root->outfile->file_loc = Root->u.file_loc;
    Root->sect_addr = Groups->grp_addr;

/* write groups and relocations */
    for( group = Groups; group != NULL; ) {
        sect = group->section;
        CurrSect = sect;

        if( group->totalsize ) {
            WriteDOSGroup( group );
            CodeSize += group->totalsize;
        }
        group = group->next_group;
    }
}

static void WriteRDOSCode( void )
/**********************************************************/
{
    group_entry         *group;
    SECTION             *sect;
    struct seg_leader   *leader;
    SEGDATA             *piece;
    int                 iscode;

    DEBUG(( DBG_BASE, "Writing code" ));
    OrderGroups( CompareDosSegments );
    CurrSect = Root;        // needed for WriteInfo.

    Root->outfile->file_loc = Root->u.file_loc;
    Root->sect_addr = Groups->grp_addr;
    leader = 0;

/* write groups and relocations */
    iscode = 0;
    for( group = Groups; group != NULL; ) {
        if( leader != group->leaders ) {
            iscode = 0;
            leader = group->leaders;
            if( leader && leader->size && Extension == E_RDV ) {
                piece = leader->pieces; 
                if( piece ) {
                    if( piece->iscode && ( leader->seg_addr.seg == FmtData.u.rdos.code_seg ) ) {
                        iscode = 1;
                    }
                }
            }
        }
        sect = group->section;
        CurrSect = sect;

        if( iscode ) {
            sect->u.file_loc = HeaderSize + CodeSize;
            WriteDOSGroup( group );
            if( group->totalsize > group->size )
                PadLoad( group->totalsize - group->size );

            CodeSize += group->totalsize;
        }
        group = group->next_group;
    }
}

static void WriteRDOSData( void )
/**********************************************************/
/* copy code from extra memory to loadfile */
{
    group_entry         *group;
    SECTION             *sect;
    struct seg_leader   *leader;
    SEGDATA             *piece;
    int                 isdata;

    DEBUG(( DBG_BASE, "Writing data" ));

/* write groups and relocations */
    leader = NULL;
        isdata = 0;
    for( group = Groups; group != NULL; ) {
        if( leader != group->leaders ) {
            isdata = 0;
            leader = group->leaders;
            if( leader && leader->size && Extension == E_RDV ) {
                piece = leader->pieces; 
                if( piece ) {
                    if( ( piece->isidata || piece->isuninit ) && ( leader->seg_addr.seg == FmtData.u.rdos.data_seg ) ) {
                        isdata = 1;
                    }
                }
            }
        }
        sect = group->section;
        CurrSect = sect;

        if( isdata ) {
            sect->u.file_loc = HeaderSize + CodeSize + DataSize;

            if( StackSegPtr != NULL ) {
                if( group->totalsize - group->size < StackSize ) {
                    StackSize = group->totalsize - group->size;
                    group->totalsize = group->size;
                } else {
                    group->totalsize -= StackSize;
                }
            }

            WriteDOSGroup( group );
            if( group->totalsize > group->size )
                PadLoad( group->totalsize - group->size );
            DataSize += group->totalsize;
        }
        group = group->next_group;
    }
}

void GetRdosSegs( void )
/* resolve RDOS code & data segments */
{
    group_entry         *group;
    struct seg_leader   *leader;
    SEGDATA             *piece;

    leader = 0;

    for( group = Groups; group != NULL; ) {
        if( leader != group->leaders ) {
            leader = group->leaders;
            if( leader && leader->size ) {
                piece = leader->pieces; 
                if( piece ) {
                    if( piece->iscode )
                        FmtData.u.rdos.code_seg = leader->seg_addr.seg;
                    if( piece->isidata || piece->isuninit )
                        FmtData.u.rdos.data_seg = leader->seg_addr.seg;
                }
            }
        }
        group = group->next_group;
    }
}

static void WriteHeader16( void )
/* write 16-bit device header */
{
    rdos_dev16_header   exe_head;
    unsigned_16         temp16;

    SeekLoad( 0 );
    _HostU16toTarg( RDOS_SIGNATURE_16, exe_head.signature );
    _HostU16toTarg( StartInfo.addr.off, exe_head.IP );
    temp16 = (unsigned_16)CodeSize;
    _HostU16toTarg( temp16, exe_head.code_size );
    temp16 = (unsigned_16)FmtData.u.rdos.code_sel;
    _HostU16toTarg( temp16, exe_head.code_sel );
    temp16 = (unsigned_16)DataSize;
    _HostU16toTarg( temp16, exe_head.data_size );
    temp16 = (unsigned_16)FmtData.u.rdos.data_sel;
    _HostU16toTarg( temp16, exe_head.data_sel );
    WriteLoad( &exe_head, sizeof( rdos_dev16_header ) );
}

static void WriteHeader32( void )
/* write 32-bit device header */
{
    rdos_dev32_header   exe_head;
    unsigned_16         temp16;
    unsigned_32         temp32;

    SeekLoad( 0 );
    _HostU16toTarg( RDOS_SIGNATURE_32, exe_head.signature );
    _HostU32toTarg( StartInfo.addr.off, exe_head.EIP );
    temp32 = (unsigned_32)CodeSize;
    _HostU32toTarg( temp32, exe_head.code_size );
    temp16 = (unsigned_16)FmtData.u.rdos.code_sel;
    _HostU16toTarg( temp16, exe_head.code_sel );
    temp32 = (unsigned_32)DataSize;
    _HostU32toTarg( temp32, exe_head.data_size );
    temp16 = (unsigned_16)FmtData.u.rdos.data_sel;
    _HostU16toTarg( temp16, exe_head.data_sel );
    WriteLoad( &exe_head, sizeof( rdos_dev32_header ) );
}

static void WriteMbootHeader( void )
/* write multiboot header */
{
    struct mb_header   mb_head;
    unsigned_32        temp32;
    unsigned_32        linear = MB_BASE;

    SeekLoad( 0 );
    _HostU32toTarg(0x1BADB002, mb_head.mb_magic );
    _HostU32toTarg(0x00010003, mb_head.mb_flags );
    _HostU32toTarg(0xE4514FFB, mb_head.mb_checksum );
    _HostU32toTarg(linear, mb_head.mb_header_addr );
    _HostU32toTarg(linear, mb_head.mb_load_addr );
    linear += CodeSize + sizeof( struct mb_header );
    _HostU32toTarg(linear, mb_head.mb_load_end_addr );
    _HostU32toTarg(linear, mb_head.mb_bss_end_addr );
    temp32 = MB_BASE + StartInfo.addr.off + sizeof( struct mb_header );
    _HostU32toTarg(temp32, mb_head.mb_entry_addr );
    WriteLoad( &mb_head, sizeof( struct mb_header ) );
}

static void FiniRdosLoadFile16( void )
/* terminate writing of load file */
{
    if( Extension == E_RDV ) {
        HeaderSize = sizeof( rdos_dev16_header );
        SeekLoad( HeaderSize );
        Root->u.file_loc = HeaderSize;
        WriteRDOSCode();
        WriteRDOSData();
        DBIWrite();
        WriteHeader16();
    } else {
        if( FmtData.u.rdos.mboot ) {
            HeaderSize = sizeof( struct mb_header );
            SeekLoad( HeaderSize );
            Root->u.file_loc = HeaderSize;
        } else {
            Root->u.file_loc = 0;
            SeekLoad( 0 );
        }
        WriteBinData();
        DBIWrite();
        if( FmtData.u.rdos.mboot )
            WriteMbootHeader(); 
    }
}

static void FiniRdosLoadFile32( void )
/* terminate writing of load file */
{
    if( Extension == E_RDV ) {
        HeaderSize = sizeof( rdos_dev32_header );
        SeekLoad( HeaderSize );
        Root->u.file_loc = HeaderSize;
        WriteRDOSCode();
        WriteRDOSData();
        DBIWrite();
        CodeSize += 0x10;           // this is a fix to make offsets into data segment correct
        WriteHeader32();
    } else {
        SeekLoad( 0 );
        Root->u.file_loc = 0;
        WriteBinData();
        DBIWrite();
    }
}

void FiniRdosLoadFile( void )
/* terminate writing of load file */
{
    if( FmtData.u.rdos.bitness == 16 ) FiniRdosLoadFile16();
    if( FmtData.u.rdos.bitness == 32 ) FiniRdosLoadFile32();
}
