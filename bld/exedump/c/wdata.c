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
* Description:  Raw segment data dump routines.
*
****************************************************************************/


#include <stdio.h>
#include <setjmp.h>
#include <string.h>
#include <ctype.h>
#include "walloca.h"
#include "wio.h"
#include "wdglb.h"
#include "wdfunc.h"


static  const_string_table reloc_addr_type[] = {
    "OFF",
    "BAS",
    "PTR",
    "?04",
    "LDR"
};


/*
 * Dump a segment
 */
static void dmp_segment( struct segment_record *seg )
/***************************************************/
{
    unsigned_32           file_off;
    unsigned_32           iter;
    unsigned_32           seg_len;

    if( seg->address == 0 ) {
        Wdputslc( "\n" );
        Wdputslc( "\n" );
        return;
    }
    if( seg->size == 0 ) {
        seg_len = 0x00010000;
    } else {
        seg_len = seg->size;
    }
    file_off = (unsigned_32)seg->address << Os2_head.align;
    Wlseek( file_off );
    if( seg->info & SEG_ITERATED ) {
        Wdputslc( "Segment is iterated: NYI because of lack of examples\n" );
        Wread( &iter, sizeof( unsigned_32 ) );
        Wdputs( "        number of iterations = " );
        Putdec( iter & 0xffff );
        Wdputs( ", byte length of data = " );
        Putdec( iter > 16 );
        Wdputslc( "\n" );
    }
    Wdputslc( "        segment data =\n" );
    Dmp_seg_data( file_off, seg_len );
    Wdputslc( "\n" );
}

/*
 * Dump a relocation item
 */
static void dmp_reloc_item( struct relocation_item *reloc )
/*********************************************************/
{
    unsigned_16             seg_num;
    unsigned_16             ord_off;
    unsigned_32             imp_off;
    unsigned_8              string_len;
    char                    *imp_nam;

    switch( reloc->reloc_type ) {
    case REL_INTERNAL_REFERENCE :
        seg_num = reloc->xtrnl.internal.seg_num;
        ord_off = reloc->xtrnl.internal.ord_off;
        if( seg_num != MOVABLE_ENTRY_PNT ) {
            Wdputs( "seg " );
            Puthex( seg_num, 4 );
            Wdputs( " offset " );
            Puthex( ord_off, 4 );
            if( seg_num > Os2_head.segments ) {
                Wdputs( " *** invalid segment" );
            } else if( ord_off > Int_seg_tab[ seg_num - 1 ].min ) {
                Wdputs( " *** invalid offset" );
            }
        } else {
            Wdputs( "movable " );
            Dmp_ordinal( ord_off );
        }
        break;
    case REL_IMPORTED_ORDINAL :
        Wdputs( "ord " );
        Wdputs( Int_mod_ref_tab[ reloc->xtrnl.ordinal.modref_idx - 1 ] );
        Wdputc( '.' );
        Putdec( reloc->xtrnl.ordinal.ord_num );
        break;
    case REL_IMPORTED_NAME :
        Wdputs( "nam " );
        Wdputs( Int_mod_ref_tab[ reloc->xtrnl.name.modref_idx - 1 ] );
        Wdputc( '.' );
        /* One of Os2_386_head.impproc_off and Os2_head.import_off will be set */
        imp_off = reloc->xtrnl.name.impnam_off + Os2_386_head.impproc_off
                + Os2_head.import_off + New_exe_off;
        Wlseek( imp_off );
        Wread( &string_len, sizeof( unsigned_8 ) );
        imp_nam = Wmalloc( string_len + 1 );
        Wread( imp_nam, string_len );
        imp_nam[ string_len ] = '\0';
        if( *imp_nam == '\0' ) {
            Wdputs( "*** import table error" );
        } else {
            Wdputs( imp_nam );
            free( imp_nam );
        }
        Wdputc( '(' );
        Puthex( reloc->xtrnl.name.impnam_off, 4 );
        Wdputc( ')' );
        break;
    default :
        Wdputs( "osfixup :" );
        Puthex( reloc->xtrnl.data, 8 );
        Wdputc( 'H' );
        break;
    }
}

/*
 * Dump a Segment's relocation info
 */
static void dmp_reloc_info( unsigned_32 reloc_off )
/*************************************************/
{
    unsigned_16                     num_relocs;
    struct relocation_item          *reloc_tab;
    struct relocation_item          *reloc;
    unsigned_16                     size_reloc_tab;

    Wlseek( reloc_off );
    Wread( &num_relocs, sizeof( unsigned_16 ) );
    size_reloc_tab = num_relocs * sizeof( struct relocation_item );
    Wdputslc( "\nNumber of relocations: " );
    Putdec( num_relocs );
    Wdputs( "     Total size of relocations: " );
    Putdec( size_reloc_tab );
    reloc_tab = Wmalloc( size_reloc_tab );
    Wread( reloc_tab, size_reloc_tab );
    Wdputslc( "\n  typ off  target" );
    Wdputslc( "\n  === ==== ====" );
    for( reloc = reloc_tab; num_relocs-- != 0; ++reloc ) {
        Wdputslc( "\n  " );
        if( (reloc->addr_type > REL_SEGMENT_OFFSET + 2)
            || reloc->addr_type == 0  ) {
            Wdputc( '?' );
            Puthex( reloc->addr_type, 2 );
        } else {
            Wdputs( reloc_addr_type[ reloc->addr_type -1 ] );
        }
        Wdputc( ' ' );
        Puthex( reloc->reloc_offset, 4 );
        Wdputc( ' ' );
        if( reloc->reloc_type & REL_ADDITIVE ) {
            Wdputs( "ADDITIVE " );
            reloc->reloc_type &= ~REL_ADDITIVE;
        }
        dmp_reloc_item( reloc );
    }
    Wdputslc( "\n" );
    free( reloc_tab );
}

/*
 * Dump the Segments
 */
void Dmp_relocs( void )
/*********************/
{
    struct segment_record   *seg;
    unsigned_16             segnum;
    unsigned_16             num_segs;
    unsigned_32             reloc_off;

    if( !(Options_dmp & FIX_DMP) ) {
        return;
    }
    Wdputslc( "\n" );
    Banner( "The Relocations" );
    seg = Int_seg_tab;
    num_segs = Os2_head.segments + 1;
    for( segnum = 1; segnum != num_segs; segnum++ ) {
        reloc_off = (unsigned_32)seg->address <<
                       Os2_head.align;
        reloc_off += seg->size;
        if( seg->info & SEG_RELOC ) {
            Wdputs( "segment # " );
            Putdec( segnum );
            dmp_reloc_info( reloc_off );
        }
    seg++;
    }
}

/*
 * Dump the Segments
 */
void Dmp_segments( void )
/***********************/
{
    struct segment_record   *seg;
    unsigned_16             segnum;
    unsigned_16             num_segs;

    Wdputslc( "\n" );
    Banner( "The Segments" );
    if( Segspec != 0 ) {
        if( Segspec > Os2_head.segments ) {
            Wdputslc( "segment specified was too large\n" );
            return;
        }
        Wdputs( "segment # " );
        Putdec( Segspec );
        seg = &Int_seg_tab[ Segspec - 1 ];
        dmp_segment( seg );
        return;
    }
    seg = Int_seg_tab;
    num_segs = Os2_head.segments + 1;
    for( segnum = 1; segnum != num_segs; segnum++ ) {
        Wdputs( "segment # " );
        Putdec( segnum );
        dmp_segment( seg++ );
    }
}

/*
 * Dump the Segments
 */
void Dmp_le_page_seg( void )
/**************************/
{
    unsigned_32     offset;

    if( Segspec == 0 || Segspec == Data_count ) {
        Wdputslc( "\n" );
        offset = ( Data_count - 1 ) * Os2_386_head.page_size;
        offset += Os2_386_head.page_off;
        Wdputslc( "\n" );
        Wdputs( "segment # " );
        if( Segspec == 0 ) {
            Putdec( Data_count );
        } else {
            Putdec( Segspec );
        }
        Wdputs( "   offset: " );
        Puthex( offset, 8 );
        Wdputslc( "\n" );
        Wdputslc( "===========\n" );
        if( Data_count == Os2_386_head.num_pages ) {
            Dmp_seg_data( offset, Os2_386_head.l.last_page );
        } else {
            Dmp_seg_data( offset, Os2_386_head.page_size );
        }
    }
}

/*
 * Dump the Segments
 */
void Dmp_lx_page_seg( map_entry map )
/***********************************/
{
    unsigned_32     offset;

    if( Segspec == 0 || Segspec == Data_count ) {
        Wdputslc( "\n" );
        offset = map.lx.page_offset << Os2_386_head.l.page_shift;
        offset += Os2_386_head.page_off;
        Wdputslc( "\n" );
        Wdputs( "segment # " );
        if( Segspec == 0 ) {
            Putdec( Data_count );
        } else {
            Putdec( Segspec );
        }
        Wdputs( "   offset: " );
        Puthex( offset, 8 );
        Wdputslc( "\n" );
        Wdputslc( "===========\n" );
        Dmp_seg_data( offset, map.lx.data_size );
    }
}


/*
 * Dump a segment's data
 */
void Dmp_seg_data( unsigned long seg_off, unsigned long seg_len )
/***************************************************************/
{
    unsigned_16             address, amount;
    char                    buf[ PERLINE + 1 ];

    Wlseek( seg_off );
    address = 0;
    amount = PERLINE;
    for( ; seg_len != 0UL; ) {
        if( seg_len < PERLINE ) {
            amount = seg_len;
        }
        Wread( buf, amount );
        dmp_data_line( buf, address, amount );
        address += amount;
        seg_len -= amount;
    }
    Wdputslc( "\n" );
}

extern void dmp_mult_data_line( char *buf, unsigned_16 address,
                                unsigned_16 amount )
/*************************************************************/
{
    unsigned chunk;

    while( amount > 0 ) {
        if( amount >= 16 ) {
            chunk = 16;
        } else {
            chunk = amount;
        }
        dmp_data_line( buf, address, chunk );
        buf += chunk;
        address += chunk;
        amount -= chunk;
    }
}

extern void dmp_data_line( char *data, unsigned_16 address, unsigned_16 amount )
/******************************************************************************/
{
    unsigned_16     idx;
    char            *buf;

    buf = alloca( amount + 1 );
    memcpy( buf, data, amount );
    Puthex( address, 4 );
    Wdputc( ':' );
    for( idx = 0; idx < amount; ++idx ) {
        Wdputc( ' ' );
        if( (idx & 0x0007) == 0 )  Wdputc( ' ' );
            Puthex( buf[ idx ], 2 );
        if( !isprint( buf[ idx ] ) )  buf[ idx ] = ' ';
    }
    buf[ idx ] = '\0';
    while( idx < 16 ) {
        Wdputs( "   " );
        if( (idx & 0x0007) == 0 )  Wdputc( ' ' );
        ++idx;
    }
    Wdputs( "    " );
    Wdputs( buf );
    Wdputslc( "\n" );
}
