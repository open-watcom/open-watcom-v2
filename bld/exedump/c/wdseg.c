/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023-2026 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Watcom debug information segment processing.
*
****************************************************************************/


#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>
#include "wio.h"
#include "wdglb.h"
#include "wdfunc.h"

/*
 * Dump Segment flags word
 */
static void dmp_seg_flags( unsigned_16 flags )
/********************************************/
{
    char            buffer[128];
    char            *p;

    p = buffer;
    if( flags & SEG_DATA ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "DATA" );
    } else if( Options_dmp & ZERO_BITS ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "CODE" );
    }
    if( flags & SEG_FLAG_1 ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "BIT1" );
    }
    if( flags & SEG_FLAG_2 ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "BIT2" );
    }
    if( flags & SEG_ITERATED ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "ITER" );
    }
    if( flags & SEG_MOVABLE ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "MOVABLE" );
    } else if( Options_dmp & ZERO_BITS ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "FIXED" );
    }
    if( flags & SEG_PURE ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "SHARE" );
    } else if( Options_dmp & ZERO_BITS ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "NOSHARE" );
    }
    if( flags & SEG_PRELOAD ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "PRELOAD" );
    } else if( Options_dmp & ZERO_BITS ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "LOADONCALL" );
    }
    if( flags & SEG_DATA ) {
        if( flags & SEG_READ_ONLY ) {
            p += sprintf( p, GET_OR_FMT( p == buffer ), "READONLY" );
        } else if( Options_dmp & ZERO_BITS ) {
            p += sprintf( p, GET_OR_FMT( p == buffer ), "READWRITE" );
        }
    } else {
        if( flags & SEG_READ_ONLY ) {
            p += sprintf( p, GET_OR_FMT( p == buffer ), "EXECONLY" );
        } else if( Options_dmp & ZERO_BITS ) {
            p += sprintf( p, GET_OR_FMT( p == buffer ), "EXECREAD" );
        }
    }
    if( flags & SEG_RELOC ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "RELOCS" );
    }
    if( flags & SEG_CONFORMING ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "DEBUG" );
    }
    *p = '\0';
    Wdputs( buffer );
}

/*
 * Dump Segment Table Entry
 */
static void dmp_seg_ent( segment_record *seg_ent )
/************************************************/
{
    unsigned_16     flags;

    Puthex( (unsigned_32)seg_ent->address << Os2_head.align, 8 );
    Wdputc( ' ' );
    Puthex( seg_ent->size, 4 );
    Wdputc( ' ' );
    Puthex( seg_ent->min, 4 );
    Wdputs( "    " );
    flags = seg_ent->info;
    Putdecl( GET_SEG_DISCARD_PRIORITY( flags ), 2 );
    Wdputs( "    " );
    Puthex( GET_SEG_PMODE_DPL( flags ), 2 );
    Wdputc( ' ' );
    Puthex( flags, 4 );
    Wdputs( "  " );
    dmp_seg_flags( flags );
}

/*
 * Dump Segment Table
 */
void Dmp_seg_tab( void )
/**********************/
{
    unsigned_16     num_segs;
    segment_record  *segtab;
    unsigned_16     segtabsize;
    unsigned_16     segnum;

    Banner( "Segment Table" );
    num_segs = Os2_head.segments;
    if( num_segs == 0 ) {
        return;
    }
    Wlseek( New_exe_off + Os2_head.segment_off );
    segtabsize = sizeof( segment_record ) * num_segs;
    Int_seg_tab = Wmalloc( segtabsize );
    segtab = Int_seg_tab;
    Wread( segtab, segtabsize );
    ++num_segs;
    Wdputslc( "seg  fileoff  len  alloc prior priv flags\n" );
    Wdputslc( "==== ======== ==== ====  ====  ==== ====\n" );
    for( segnum = 1; segnum != num_segs; segnum++ ) {
        Puthex( segnum, 4 );
        Wdputc( ' ' );
        dmp_seg_ent( segtab++ );
        Wdputslc( "\n" );
    }
    Wdputslc( "\n" );
}
