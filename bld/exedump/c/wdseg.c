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
* Description:  Watcom debug information segment processing.
*
****************************************************************************/


#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>
#include <unistd.h>

#include "wdglb.h"
#include "wdfunc.h"

/*
 * Dump Segment Flag word
 */
static void dmp_seg_flag( unsigned_16 flag )
/******************************************/
{
    if( flag & SEG_DATA ) {
        Wdputs( "  DATA" );
    } else {
        Wdputs( "  CODE" );
    }
    if( flag & SEG_FLAG_1 ) {
        Wdputs( "|BIT1" );
    }
    if( flag & SEG_FLAG_2 ) {
        Wdputs( "|BIT2" );
    }
    if( flag & SEG_FLAG_1 ) {
        Wdputs( "|BIT3" );
    }
    if( flag & SEG_FLAG_2 ) {
        Wdputs( "|BIT4" );
    }
    if( flag & SEG_ITERATED ) {
        Wdputs( "|ITER" );
    }
    if( flag & SEG_MOVABLE ) {
        Wdputs( "|MOVABLE" );
    } else {
        Wdputs( "|FIXED" );
    }
    if( flag & SEG_PURE ) {
        Wdputs( "|SHARE" );
    } else {
        Wdputs( "|NOSHARE" );
    }
    if( flag & SEG_PRELOAD ) {
        Wdputs( "|PRELOAD" );
    } else {
        Wdputs( "|LOADONCALL" );
    }
    if( flag & SEG_DATA ) {
        if( flag & SEG_READ_ONLY ) {
            Wdputs( "|READONLY" );
        } else {
            Wdputs( "|READWRITE" );
        }
    } else {
        if( flag & SEG_READ_ONLY ) {
            Wdputs( "|EXECONLY" );
        } else {
            Wdputs( "|EXECREAD" );
        }
    }
    if( flag & SEG_RELOC ) {
        Wdputs( "|RELOCS" );
    }
    if( flag & SEG_CONFORMING ) {
        Wdputs( "|DEBUG" );
    }
    if( flag & SEG_DISCARD ) {
        Wdputs( "|DISCARDABLE" );
    }
    if( flag & SEG_32_BIT ) {
        Wdputs( "|32 BIT SEG" );
    }
    if( flag & SEG_HUGE ) {
        Wdputs( "|PART OF HUGE" );
    }
}

/*
 * Dump Segment Table Entry
 */
static void dmp_seg_ent( struct segment_record *seg_ent )
/*******************************************************/
{
    Wdputc( ' ' );
    Puthex( (unsigned_32)seg_ent->address
        << Os2_head.align, 8 );
    Wdputc( ' ' );
    Puthex( seg_ent->size, 4 );
    Wdputc( ' ' );
    Puthex( seg_ent->min, 4 );
    Wdputc( ' ' );
    Wdputc( ' ' );
    Puthex( seg_ent->info >> SEG_SHIFT_PRI_LVL , 4 );
    Wdputc( ' ' );
    Wdputc( ' ' );
    Puthex( seg_ent->info >> SEG_SHIFT_PMODE_LVL, 4 );
    Wdputc( ' ' );
    Puthex( seg_ent->info, 4 );
    Wdputslc( "\n" );
    dmp_seg_flag( seg_ent->info );
    Wdputslc( "\n" );
}

/*
 * Dump Segment Table
 */
void Dmp_seg_tab( void )
/**********************/
{
    unsigned_16                     num_segs;
    struct segment_record           *segtab;
    unsigned_16                     segtabsize;
    unsigned_16                     segnum;

    Banner( "Segment Table" );
    num_segs = Os2_head.segments;
    if( num_segs == 0 ) {
        return;
    }
    Wlseek( New_exe_off + Os2_head.segment_off );
    segtabsize = sizeof( struct segment_record ) * num_segs;
    segtab = Wmalloc( segtabsize );
    Wread( segtab, segtabsize );
    Int_seg_tab = segtab;
    ++num_segs;
    Wdputslc( "seg  fileoff  len  alloc prior priv flag\n" );
    Wdputslc( "==== ======== ==== ====  ====  ==== ====\n" );
    for( segnum = 1; segnum != num_segs; segnum++ ) {
        Puthex( segnum, 4 );
        dmp_seg_ent( segtab++ );
    }
    Wdputslc( "\n" );
}
