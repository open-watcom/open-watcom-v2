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
* Description:  Routines for creating DOS/16M load files.
*
****************************************************************************/


#include <string.h>
#include <stdlib.h>
#include "linkstd.h"
#include "alloc.h"
#include "msg.h"
#include "exedos.h"
#include "exe16m.h"
#include "load16m.h"
#include "pcobj.h"
#include "reloc.h"
#include "specials.h"
#include "wlnkmsg.h"
#include "virtmem.h"
#include "loadfile.h"
#include "fileio.h"
#include "objcalc.h"
#include "dbgall.h"
#include "ring.h"


/* Notes:
 *  - Currently we only support RSI-1 style relocations (2 segments, one containing
 * fixup segment values and the other offsets, presumably limited to 32K relocs).
 * Newer tools create RSI-2 style relocations in slightly more complex format - a word
 * containing segment value, a word containing count, and specified number of word-sized
 * offsets, repeated for each segment to be fixed up. RSI-2 style relocs may span multiple
 * segments (huge seg). Support for RSI-2 could be added relatively easily.
 *   - It might be good to default to 'AUTO' (ie. relocatable) modules, since
 * non-relocatable DOS/16M modules only run on VCPI or raw systems, not DPMI.
 */

typedef dos_addr reloc_addr;

typedef struct context {
    reloc_addr      *reloc_data;
} context;

static unsigned_32  LastSel = 0;
static unsigned     reloc_fmt = 0;

static void WriteGDT( unsigned_32 reloc_size )
/********************************************/
// write out the GDTs
{
    gdt_info        gdt;
    group_entry     *currgrp;

// write program gdt's.
    gdt.gdtaddr_hi = 0;
    for( currgrp = Groups; currgrp != NULL; currgrp = currgrp->next_group ) {
        if( currgrp->size > 0 ) {
            gdt.gdtlen = MAKE_PARA( currgrp->size ) - 1; // para align.
        } else {
            gdt.gdtlen = 0;
        }
        gdt.gdtaddr = 0; // currgrp->u.dos_segment;
        gdt.gdtreserved = MAKE_PARA( (unsigned_32)currgrp->totalsize ) >> 4;   // mem size in paras
        if( gdt.gdtreserved == 0 && currgrp->size == 0 )
            gdt.gdtreserved |= 0x2000;
        if( currgrp->segflags & SEG_DATA ) {
            gdt.gdtaccess = D16M_ACC_DATA;
            if( currgrp == DataGroup ) {
                gdt.gdtreserved = FmtData.u.d16m.datasize;
                if( FmtData.u.d16m.flags & TRANS_STACK ) {
                    gdt.gdtreserved |= TRANSPARENT;
                }
            }
            if( FmtData.u.d16m.flags & TRANS_DATA ) {
                gdt.gdtreserved |= TRANSPARENT;
            }
        } else {
            gdt.gdtaccess = D16M_ACC_CODE;
        }
        WriteLoad( &gdt, sizeof( gdt_info ) );
    }
// write out gdt for reloc info
    if( reloc_fmt == 1 ) {
        reloc_size /= 2;
        gdt.gdtreserved = 0;
        gdt.gdtaddr = 0;
        gdt.gdtaccess = D16M_ACC_DATA;
        gdt.gdtlen = reloc_size - 1;
        gdt.gdtreserved = reloc_size >> 4;   // mem size in paras
        WriteLoad( &gdt, sizeof( gdt_info ) );
        WriteLoad( &gdt, sizeof( gdt_info ) );
    } else if( reloc_fmt == 2 ) {
        gdt.gdtreserved = 0;
        gdt.gdtaddr = 0;
        gdt.gdtaccess = D16M_ACC_DATA;
        gdt.gdtlen = reloc_size - 1;
        gdt.gdtreserved = reloc_size >> 4;   // mem size in paras
        WriteLoad( &gdt, sizeof( gdt_info ) );
    }
}

static unsigned_32 Write16MData( unsigned hdr_size )
/**************************************************/
{
    group_entry     *group;

    DEBUG(( DBG_BASE, "Writing DOS/16M data" ));

    SeekLoad( hdr_size );
    for( group = Groups; group != NULL; group = group->next_group ) {
        WriteGroupLoad( group );
        NullAlign( 0x10 );          // paragraph alignment.
    }
    return( PosLoad() );
}

static bool RelocWalkFn( void *data, unsigned_32 size, void *ctx )
/****************************************************************/
{
    context     *info = ctx;

    memcpy( info->reloc_data, data, size );
    info->reloc_data += size / sizeof( reloc_addr );
    return( false );   /* don't stop walking */
}

static void write_sel_reloc( unsigned_16 sel, reloc_addr *block_start, reloc_addr *block_end )
/********************************************************************************************/
{
    unsigned_16     block_cnt;

    if( sel != 0 ) {
        WriteLoad( &sel, sizeof( sel ) );
        block_cnt = block_end - block_start;
        WriteLoad( &block_cnt, sizeof( block_cnt ) );
        for( ; block_start < block_end; ++block_start ) {
            WriteLoad( &block_start->off, sizeof( block_start->off ) );
        }
    }
}

static int cmp_reloc_entry( const void *a, const void *b )
/********************************************************/
{
    int retval;

    retval = ((reloc_addr *)a)->seg - ((reloc_addr *)b)->seg;
    if( retval == 0 ) {
        retval = ((reloc_addr *)a)->off - ((reloc_addr *)b)->off;
    }
    return( retval );
}

static unsigned GetRelocBlock( reloc_addr **reloc_data )
/******************************************************************/
{
    RELOC_INFO      *relocs;
    unsigned        num_relocs;
    context         info;

    *reloc_data = NULL;
    if( (LinkState & MAKE_RELOCS) && Root->relocs ) {
        relocs = Root->reloclist;
        num_relocs = Root->relocs;
        *reloc_data = ChkLAlloc( sizeof( reloc_addr ) * num_relocs );
        if( *reloc_data != NULL ) {
            info.reloc_data = *reloc_data;
            WalkRelocList( &relocs, RelocWalkFn, &info );
            if( reloc_fmt == 1 ) {
                return( 2 );
            } else {
                qsort( *reloc_data, num_relocs, sizeof( reloc_addr ), cmp_reloc_entry );
                // TODO: number of extra reloc segments should be
                // properly calculated, now only one is set
                // (max. relocs are nearly 2^15 items)
                return( 1 );
            }
        }
    }
    reloc_fmt = 0;
    return( 0 );
}

// There are two types of relocations for DOS/16M. Old style or RSI-1 relocs
// are stored internally in one virtual memory block, with the segment selectors
// being stored in the first half of the memory block, and the segment offsets
// being stored in the second half. These are limited to 32K relocs.
// Newer style or "huge" RSI-2 relocs are stored in one or more consecutive
// segments. First is the selector, then a count word, followed by offsets within
// the selector. This sequence is repeated for other selectors. The last selector
// in the list has bit 1 set (selector numbers are multiples of 8 so the
// low four bits are unused).

static unsigned_32 Write16MRelocs( reloc_addr *reloc_data )
/*********************************************************************/
{
    unsigned        num_relocs;
    unsigned_32     pos;
    int             i;
    unsigned_16     sel;
    unsigned        block_start;

    pos = PosLoad();
    num_relocs = Root->relocs;
    if( reloc_fmt == 1 ) {
        // RSI-1 reloc format
        for( i = 0; i < num_relocs; ++ i ) {
            WriteLoad( &reloc_data[i].seg, sizeof( reloc_data[i].seg ) );
        }
        NullAlign( 0x10 );
        for( i = 0; i < num_relocs; ++ i ) {
            WriteLoad( &reloc_data[i].off, sizeof( reloc_data[i].off ) );
        }
    } else if( reloc_fmt == 2 ) {
        // RSI-2 reloc format
        sel = 0;
        block_start = 0;
        for( i = 0; i < num_relocs; ++ i ) {
            if( sel != reloc_data[i].seg ) {
                write_sel_reloc( sel, reloc_data + block_start, reloc_data + i );
                block_start = i;
                sel = reloc_data[i].seg;
            }
        }
        write_sel_reloc( sel | 0x02, reloc_data + block_start, reloc_data + i );
    }
    return( NullAlign( 0x10 ) - pos );
}

static unsigned_32 WriteStubProg( void )
/**************************************/
{
    unsigned_32 size;
    f_handle    fhandle;
    char        *name;

    name = FmtData.u.d16m.stub;
    if( name == NULL ) {
        size = 0;
    } else {
        fhandle = FindPath( name );
        if( fhandle == NIL_FHANDLE ) {
            LnkMsg( WRN+MSG_CANT_OPEN_NO_REASON, "s", name );
            size = 0;
        } else {
            size = CopyToLoad( fhandle, name );
        }
    }
    SetOriginLoad( size );
    return( size );
}

extern void Fini16MLoadFile( void )
/*********************************/
{
    unsigned_32         hdr_size;
    unsigned_32         temp;
    unsigned_32         exe_size;
    dos16m_exe_header   exe_head;
    unsigned            extra_sels;
    unsigned_32         stub_size;
    unsigned_32         reloc_size;
    reloc_addr          *reloc_data;

    // TODO: add some parameter for reloc format switching
    // for now it is setup to RSI-2 reloc format
    reloc_fmt = 2;
    extra_sels = GetRelocBlock( &reloc_data );
    stub_size = WriteStubProg();
    memset( &exe_head, 0, sizeof( exe_head ) );
    hdr_size = sizeof( exe_head ) + (NumGroups + extra_sels) * sizeof( gdt_info );
    hdr_size = MAKE_PARA( hdr_size );
    reloc_size = 0;
    exe_size = Write16MData( hdr_size );
    if( extra_sels ) {
        reloc_size = Write16MRelocs( reloc_data );
        exe_size += reloc_size;
        LFree( reloc_data );
    }
    DBIWrite();

    SeekLoad( 0 );
    _HostU16toTarg( DOS16M_SIGNATURE, exe_head.signature );
    temp = hdr_size / 16U;
    _HostU16toTarg( exe_size % 512U, exe_head.last_page_bytes );
    temp = exe_size / 512U;
    _HostU16toTarg( temp, exe_head.pages_in_file );
    _HostU16toTarg( FmtData.u.d16m.extended, exe_head.max_alloc );
    _HostU16toTarg( 0, exe_head.min_alloc );
    _HostU16toTarg( StartInfo.addr.off, exe_head.init_ip );
    _HostU16toTarg( StartInfo.addr.seg, exe_head.code_seg );
    if( reloc_fmt == 2 ) {
        temp = FmtData.u.d16m.selstart + NumGroups * sizeof( gdt_info );
    } else {
        temp = 0;
    }
    _HostU16toTarg( temp, exe_head.first_reloc_sel );
    _HostU16toTarg( StackAddr.seg, exe_head.stack_seg );
    _HostU16toTarg( StackAddr.off, exe_head.stack_ptr );
    _HostU16toTarg( FmtData.u.d16m.gdtsize, exe_head.runtime_gdt_size );
    _HostU16toTarg( 2000, exe_head.MAKEPM_version );   // pretend GLU version 2.0
    _HostU32toTarg( exe_size + stub_size, exe_head.next_header_pos );
    _HostU32toTarg( 0, exe_head.cv_info_offset );
    if( extra_sels ) {
        _HostU16toTarg( ef_auto, exe_head.exp_flags );
    } else {
        FmtData.u.d16m.options &= ~OPT_AUTO;
    }
    _HostU16toTarg( FmtData.u.d16m.options, exe_head.options );
    temp = NUM_RESERVED_SELS + NumGroups + extra_sels;
    _HostU16toTarg( MAKE_PARA( temp * sizeof( gdt_info ) ) - 1, exe_head.gdtimage_size );
    exe_head.reserved5 = D16M_ACC_DATA;
    _HostU16toTarg( FmtData.u.d16m.selstart, exe_head.first_selector );
    _HostU16toTarg( FmtData.u.d16m.buffer, exe_head.transfer_buffer_size );
    _HostU16toTarg( FmtData.u.d16m.strategy, exe_head.default_mem_strategy );
    while( extra_sels-- ) {
        LastSel += sizeof( gdt_info );
        if( LastSel > 0xFFFF ) {
            LnkMsg( FTL + MSG_TOO_MANY_SELECTORS, NULL );
        }
        if( LastSel >= 0xA000 && (LastSel & 0x1FF) == 0 ) {   // check for reserved selector
            LastSel += sizeof( gdt_info );
        }
    }
    _HostU16toTarg( LastSel, exe_head.last_sel_used );
    WriteLoad( &exe_head, sizeof( exe_head ) );
    WriteGDT( reloc_size );
}

extern unsigned ToD16MSel( unsigned seg_num )
/*******************************************/
{
    unsigned_32    x;

    LastSel = FmtData.u.d16m.selstart + ( seg_num - 1 ) * sizeof( gdt_info );
    for( x = 0xA000; LastSel >= x; x += 0x200 ) {   // check for reserved selector
        LastSel += sizeof( gdt_info );
        if( LastSel > 0xFFFF ) {
            LnkMsg( FTL + MSG_TOO_MANY_SELECTORS, NULL );
        }
    }
    return( LastSel );
}

extern segment Find16MSeg( segment selector )
/*******************************************/
// this finds the dos_segment value which corresponds to the given selector.
{
    group_entry     *currgrp;
    segment         result;

    result = 0;
    for( currgrp = Groups; currgrp != NULL; currgrp = currgrp->next_group ) {
        if( currgrp->grp_addr.seg == selector ) {
            result = currgrp->u.dos_segment;
            break;
        }
    }
    return( result );
}

extern void CalcGrpSegs( void )
/*****************************/
// go through group list & calculate what the segment would be if we were
// running under DOS.
{
    group_entry     *currgrp;
    offset          addr;

    addr = 0;
    for( currgrp = Groups; currgrp != NULL; currgrp = currgrp->next_group ) {
        addr = MAKE_PARA( addr );       // addr is paragraph aligned.
        currgrp->u.dos_segment = addr >> 4;
        addr += currgrp->totalsize;
    }
}

static void SetSegDataAbs( void *_sdata )
/***************************************/
{
    segdata     *sdata = _sdata;

    if( !sdata->isdead ) {
        sdata->isabs = true;
        sdata->combine = COMBINE_INVALID;
        sdata->isuninit = true;
    }
}

static void RemapAliasSels( void *_leader )
/*****************************************/
{
    seg_leader  *leader = _leader;
    seg_leader  *seg;
    section     *sect;

    if( leader->info & SEG_ABSOLUTE ) {
        sect = Root;
        while( (seg = FindSegment( sect, leader->segname )) != NULL ) {
            sect = NULL;
            if( (seg->info & SEG_ABSOLUTE) == 0 ) {
                seg->info |= SEG_ABSOLUTE;
                seg->combine = COMBINE_INVALID;
                seg->seg_addr = leader->seg_addr;
                RingWalk( seg->pieces, SetSegDataAbs );
            }
        }
    }
}

void MakeDos16PM( void )
/**********************/
{
    class_entry *class;

    /*
     * remap dos16m kernel selectors reference
     */
    for( class = Root->classlist; class != NULL; class = class->next_class ) {
        if( ( class->flags & CLASS_DEBUG_INFO ) == 0 ) {
            RingWalk( class->segs, RemapAliasSels );
        }
    }
    /*
     * change startup point
     */
    if( FindISymbol( "_cstart_" ) != NULL ) {
        StartInfo.type = START_UNDEFED;
        SetStartSym( "_cstart_" );
    }
}
