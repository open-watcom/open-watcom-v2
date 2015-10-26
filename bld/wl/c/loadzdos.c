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
* Description:  LOADZDOS : routines for creating ZDOS load files.
*
****************************************************************************/

#include <string.h>
#include "linkstd.h"
#include "exezdos.h"
#include "loadzdos.h"
#include "alloc.h"
#include "msg.h"
#include "reloc.h"
#include "wlnkmsg.h"
#include "proc386.h"
#include "virtmem.h"
#include "objcalc.h"
#include "dbgall.h"
#include "fileio.h"
#include "loadfile.h"

static unsigned_32  WriteZdosData( unsigned file_pos );
static unsigned_32  WriteZdosRelocs( void );

extern void FiniZdosLoadFile( void )
/*************************************/
{
    zdos_exe_header header;
    unsigned_32     position;
    unsigned_32     size;
	unsigned_32     checksum;

    for( position = 0; position < 4; position ++ )
        header.reserved[position] = 0;
	checksum = ZDOS_SIGNATURE;
    _HostU32toTarg( ZDOS_SIGNATURE, header.signature );
    _HostU32toTarg( StartInfo.addr.off, header.EIP );
	checksum += StackAddr.off;
    _HostU32toTarg( StackAddr.off, header.ESP );
	checksum += StackAddr.off;
    _HostU32toTarg( sizeof( zdos_exe_header ), header.hdr_size );
	checksum += sizeof ( zdos_exe_header );
    _HostU32toTarg( Root->relocs, header.num_relocs );
	checksum += Root->relocs;
    _HostU32toTarg( sizeof( zdos_exe_header ), header.reloc_offset );
	checksum += sizeof ( zdos_exe_header );
	_HostU32toTarg( FmtData.base, header.reloc_base );
	checksum += FmtData.base;
	SeekLoad( sizeof (zdos_exe_header) );
	position = sizeof ( zdos_exe_header ) + WriteZdosRelocs();
    _HostU32toTarg( position, header.image_offset );
	checksum += position;
	size = WriteZdosData( position );
    _HostU32toTarg( size, header.image_size );
	checksum += size;
	position += size;
    _HostU32toTarg( position, header.debug_offset );
	checksum += position;
    size = ( StackAddr.off - FmtData.base ) - size;
	_HostU32toTarg( size, header.extra_size );
	checksum += size;
	_HostU32toTarg( size, header.chk_sum );
    DBIWrite();
    SeekLoad( 0 );
    WriteLoad( &header, sizeof ( zdos_exe_header ) );
}

static unsigned_32 WriteZdosData( unsigned file_pos )
/***************************************************/
{
    group_entry         *group;
    outfilelist *       fnode;
    bool                repos;

    DEBUG(( DBG_BASE, "Writing data" ));
    OrderGroups( CompareOffsets );
    CurrSect = Root;            // needed for WriteInfo.
    fnode = Root->outfile;
    fnode->file_loc = file_pos;
    Root->u.file_loc = file_pos;
    Root->sect_addr = Groups->grp_addr;
    for( group = Groups; group != NULL; group = group->next_group ) {
        repos = WriteDOSGroup( group );
        if( repos ) {
            SeekLoad( fnode->file_loc );
        }
    }
    return( fnode->file_loc - file_pos );
}

static unsigned_32 WriteZdosRelocs( void )
/****************************************/
{
    RELOC_INFO  *temp;

    temp = Root->reloclist;                 // don't want to modify original
    return DumpMaxRelocList( &temp, 0 );    // write the relocations.
}
