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
* Description:  DOS/16M executable dumping routines.
*
****************************************************************************/


#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>
#include "wio.h"
#include "wdglb.h"
#include "wdfunc.h"

static  char    *dos16m_exe_msg[] = {
    "2length of load module mod 200H                       = ",
    "2number of 200H pages in load module                  = ",
    "2reserved1                                            = ",
    "2reserved2                                            = ",
    "2minimum memory allocation in KB                      = ",
    "2maximum memory allocation in KB                      = ",
    "2initial value of SS                                  = ",
    "2initial value of SP                                  = ",
    "2selector for start of huge reloc list                = ",
    "2initial value of IP                                  = ",
    "2initial value of CS                                  = ",
    "2runtime GDT size in bytes less one                   = ",
    "2MAKEPM/GLU version * 100                             = ",
    "4offset of possible next spliced .exp                 = ",
    "4offset of debug info                                 = ",
    "2last selector value used                             = ",
    "2private memory allocation in KB                      = ",
    "2allocation increment (ExtReserve) in KB              = ",
    "4reserved3                                            = ",
    "2reserved4                                            = ",
    "2runtime options bit mask                             = ",
    "2selector of transparent stack                        = ",
    "2module flags                                         = ",
    "2program size in paras                                = ",
    "2file size of GDT in bytes less one                   = ",
    "2first user selector (0 -> default of 0x80)           = ",
    "1default mem strategy                                 = ",
    "1reserved5                                            = ",
    "2transfer buffer size in bytes (0 -> default of 8192) = ",
    NULL
};

#if 0
static  char    *dos16m_gdt_msg[] = {
    "2gdtlen      = ",
    "2gdtaddr     = ",
    "1gdtaddr_hi  = ",
    "1gdtaccess   = ",
    "2gdtreserved = ",
    NULL
};
#endif

typedef struct seg_data_info {
    unsigned_32 size;
    unsigned_32 file_off;
} seg_data_info;

typedef struct {
    unsigned_16 off;
    unsigned_16 sel;
} reloc;

/*
 * dump the DOS/16M GDT file data
 */
static void dmp_dos16m_gdt( dos16m_exe_header *d16m_head, int sel_count,
                                               seg_data_info *segs_info )
/***********************************************************************/
{
    int                             i;
    gdt_info                        gdt;
    unsigned_32                     selector;
    unsigned_32                     file_size;
    unsigned_32                     mem_size;
    unsigned_32                     segdata_offset;
    unsigned                        access;

    Wdputslc( "GDT selectors:\n" );
    Wlseek( New_exe_off + sizeof( dos16m_exe_header ) );
    selector = d16m_head->first_selector;
    /* Calculate offset of the first segment image in file */
    segdata_offset = New_exe_off + sizeof( dos16m_exe_header ) + 1
                   + d16m_head->gdtimage_size - 16 * sizeof( gdt_info );

    Wdputslc( "                                 Size in    Size in\n" );
    Wdputslc( "Sel #    Access    File offset   File       Memory     DPL    Present    Flags\n" );
    Wdputslc( "-----    ------    -----------   -------    -------    ---    -------    -----\n" );
    for( i = 0; i < sel_count; ++i ) {
        Wread( &gdt, sizeof( gdt_info ) );
//        Dump_header( &gdt, dos16m_gdt_msg );
        Puthex( selector, 4 );
        selector += 8;
        Wdputs( "      " );
        access = gdt.gdtaccess & 0x0F;
        if( access & 0x08 ) {   /* code seg */
            Wdputs( (access & 0x02) ? "ER" : "EO" );
        } else {                /* data seg */
            Wdputs( (access & 0x02) ? "RW" : "RO" );
        }
        Wdputs( "         " );
        Puthex( segdata_offset, 6 );
        Wdputs( "       " );
        file_size = gdt.gdtlen ? gdt.gdtlen + 1 : 0;
        Puthex( file_size, 5 );
        segs_info[ i ].file_off = segdata_offset;
        segs_info[ i ].size = file_size;
        segdata_offset += file_size;
        Wdputs( "      " );
        mem_size = ((unsigned_32)gdt.gdtreserved & 0x1FFF) << 4;
        if( mem_size == 0 ) mem_size = file_size;
        Puthex( mem_size, 5 );
        Wdputs( "     " );
        Putdec( (gdt.gdtaccess >> 5) & 0x03 );
        Wdputs( "         " );
        Putdec( (gdt.gdtaccess & 0x80) != 0 );
        Wdputs( "       " );
        if( gdt.gdtreserved & 0x2000 ) Wdputs( "Z" );
        Wdputslc( "\n" );
    }
    Wdputslc( "\n" );
}

static int  get_selector_count( dos16m_exe_header *d16m_head )
/************************************************************/
{
    /* Calculate the number of selector entries (GDT) in file. Guesswork! */
    if( d16m_head->first_selector == 0 )
        d16m_head->first_selector = D16M_USER_SEL;
    if( d16m_head->last_sel_used ) {
        return( (d16m_head->last_sel_used - d16m_head->first_selector) / sizeof( gdt_info ) + 1 );
    } else {
        return( (d16m_head->gdtimage_size + 1) / sizeof( gdt_info ) - 17 );
    }
}

static unsigned_16 put_reloc( reloc *r, unsigned_16 idx )
{
    Puthex( r->sel, 4 );
    Wdputslc( ":" );
    Puthex( r->off, 4 );
    Wdputslc( " " );
    ++idx;
    if( idx % 4 == 0 ) {
        Wdputslc( "\n" );
    }
    return( idx );
}

/*
 * dump the DOS/16M header information
 */
static void dmp_dos16m_head_info( dos16m_exe_header *d16m_head )
/**************************************************************/
{
    unsigned_32     load_len;
    unsigned_16     i;
    unsigned_16     ver;
    unsigned_16     sel;
    unsigned_16     last_sel;
    unsigned_32     load_pos_sel;
    unsigned_32     load_pos_off;
    unsigned_16     sel_count;
    seg_data_info   *segs_info;
    unsigned_32     size;
    unsigned_16     offs_cnt;
    reloc           r;
    bool            last_reloc;

    Banner( "DOS/16M EXE Header" );
    Dump_header( &d16m_head->last_page_bytes, dos16m_exe_msg );
    load_len = (unsigned_32)d16m_head->pages_in_file * 0x200 + d16m_head->last_page_bytes;
    Wdputs( "\nload module length                                   = " );
    Puthex( load_len, 8 );
    Wdputslc( "H\n" );
    ver = d16m_head->MAKEPM_version;
    if( ver > 10 * 100 ) {
        Wdputs( "GLU version                                          = " );
        Putdec( ver / 1000 );
        Wdputs( "." );
        Putdecl( ver % 1000, 3 );
    } else {
        Wdputs( "MAKEPM version                                       = " );
        Putdec( ver / 100 );
        Wdputs( "." );
        Putdecl( ver % 100, 2 );
    }
    Wdputslc( "\n" );
    Wdputs( "original name: " );
    Wdputs( d16m_head->EXP_path );
    Wdputslc( "\n" );
    Wdputslc( "\n" );

    sel_count = get_selector_count( d16m_head );

    segs_info = Wmalloc( sel_count * sizeof( seg_data_info ) );

    dmp_dos16m_gdt( d16m_head, sel_count, segs_info );

    if( Options_dmp & FIX_DMP ) {
        Wdputslc( "Relocations selector:offset\n\n" );
        i = 0;
        if( (d16m_head->options & OPT_AUTO) == 0 ) {
            //  no reloc info
        } else if( d16m_head->first_reloc_sel == 0 ) {
            //  RSI-1 reloc format
            sel = ( d16m_head->last_sel_used - d16m_head->first_selector ) / sizeof( gdt_info );
            size = segs_info[ sel ].size;
            load_pos_sel = segs_info[ sel - 1 ].file_off;
            load_pos_off = segs_info[ sel ].file_off;
            while( size ) {
                Wlseek( load_pos_sel );
                Wread( &r.sel, sizeof( r.sel ) );
                load_pos_sel += sizeof( r.sel );
                Wlseek( load_pos_off );
                Wread( &r.off, sizeof( r.off ) );
                load_pos_off += sizeof( r.off );
                if( (r.sel | r.off) == 0 )
                    break;
                size -= sizeof( r.sel );
                i = put_reloc( &r, i );
            }
        } else {
            //  RSI-2 reloc format
            sel = ( d16m_head->first_reloc_sel - d16m_head->first_selector ) / sizeof( gdt_info );
            for( ; sel < sel_count; ++sel ) {
                last_reloc = FALSE;
                size = segs_info[ sel ].size;
                Wlseek( segs_info[ sel ].file_off );
                while( size ) {
                    Wread( &r.sel, sizeof( r.sel ) );
                    size -= sizeof( r.sel );
                    if( r.sel & 0x02 ) {        // last selector in list
                        r.sel &= ~0x02;
                        last_reloc = TRUE;
                    }
                    Wread( &offs_cnt, sizeof( offs_cnt ) );
                    size -= sizeof( offs_cnt ) + offs_cnt * sizeof( r.off );
                    while( offs_cnt-- ) {
                        Wread( &r.off, sizeof( r.off ) );
                        i = put_reloc( &r, i );
                    }
                    if( last_reloc )
                        break;
                }
            }
        }
        if( i % 4 ) {
            Wdputslc( "\n" );
        }
        Wdputslc( "\n" );
    }
    if( Options_dmp & DOS_SEG_DMP ) {
        if( (d16m_head->options & OPT_AUTO) == 0 ) {
            // no reloc info
            last_sel = sel_count;
        } else if( d16m_head->first_reloc_sel == 0 ) {
            // RSI-1 reloc info
            last_sel = sel_count - 2;
        } else {
            // RSI-2 reloc info
            last_sel = ( d16m_head->first_reloc_sel - d16m_head->first_selector ) / sizeof( gdt_info );
        }
        sel = d16m_head->first_selector;
        for( i = 0; i < last_sel; ++i ) {
            Wdputslc( "Load selector = " );
            Puthex( sel, 4 );
            Wdputslc( "\n\n" );
            Dmp_seg_data( segs_info[ i ].file_off, segs_info[ i ].size );
            sel += 8;
        }
    }
    free( segs_info );
}

/*
 * Dump the DOS/16M Executable Header, if any.
 */
bool Dmp_d16m_head( void )
/************************/
{
    dos16m_exe_header   dos16m_head;
    bool                retval;

    retval = 0;
    Wlseek( New_exe_off );
    Wread( &dos16m_head, sizeof( dos16m_head ) );
    for( ; dos16m_head.signature == DOS16M_SIGNATURE; ) {
        retval = 1;
        Banner( "DOS/16M EXE Header - BW" );
        Wdputs( "file offset = " );
        Puthex( New_exe_off, 8 );
        Wdputslc( "H\n" );
        Wdputslc( "\n" );
        dmp_dos16m_head_info( &dos16m_head );
        New_exe_off = dos16m_head.next_header_pos;
        Wlseek( New_exe_off );
        if( Weof() )
            break;
        Wread( &dos16m_head, sizeof( dos16m_head ) );
    }
    return( retval );
}
