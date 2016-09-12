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
* Description:  QNX executable dumping routines.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>

#include "wdglb.h"
#include "wdfunc.h"


static  const_string_table qnx_def_msg[] = {
    "2version number                              = ",
    "2cflags                                      = ",
    "2cpu (86, 186, 286, 386, 486)                = ",
    "2fpu (0, 87, 287, 287)                       = ",
    "2selector of code start address              = ",
    "2selector where to put the stack             = ",
    "2selector to put heap                        = ",
    "2selector where to put argv and environment  = ",
    "2zero                                        = ",
    "2zero                                        = ",
    "2zero                                        = ",
    "2zero                                        = ",
    "4code offset                                 = ",
    "4stack size in bytes                         = ",
    "4initial near heap size in bytes             = ",
    "4image base                                  = ",
    "4zero                                        = ",
    "4zero                                        = ",
    NULL
};

static  const_string_table qnx_data_msg[] = {
    "2segment index          = ",
    "4offset                 = ",
    NULL
};

/*
 * Dump the flags.
 */
static void dmp_flag( unsigned_16 flag )
/**************************************/
{
    if( !flag ) {
        return;
    }
    Wdputs( "flags                                       =" );
    if( flag & _TCF_LONG_LIVED ) {
        Wdputs( " LONG_LIVED" );
    }
    if( flag & _TCF_32BIT ) {
        Wdputs( " 32BIT" );
    }
    if( flag & _TCF_PRIV_MASK ) {
        Wdputs( " PRIV_MASK" );
    }
    if( flag & _TCF_FLAT ) {
        Wdputs( " FLAT" );
    }
    Wdputslc( "\n" );
}

/*
 * Dump segments.
 */
static void dmp_seg( unsigned_16 size )
/*************************************/
{
    unsigned_16     i;
    unsigned_32     seg;

    if( !size ) {
        return;
    }
    Wdputslc( "segments = type:size\n" );
    Wdputslc( "type  0==read/write, 1==read-only, 2==execute/read, 3==execute-only\n" );
    for( i = 0; i < size; i++ ) {
        if( i != 0 ) {
            if( i % 6 == 0 ) {
                Wdputslc( "\n" );
            } else {
                Wdputs( "    " );
            }
        }
        Wread( &seg, sizeof( unsigned_32 ) );
        Puthex( seg >> 28, 1 );
        Wdputc( ':' );
        Puthex( seg, 7 );
    }
    Wdputslc( "\n" );
}

/*
 * Dump rw end.
 */
static void dmp_rw_end( void )
/****************************/
{
    lmf_rw_end      rw_end;

    Wdputslc( "\n" );
    Banner( "Read/Write end" );
    Wread( &rw_end, sizeof( lmf_rw_end ) );
    Wdputs( "verify = " );
    Puthex( rw_end.verify, 4 );
    Wdputs( "H     signature = " );
    Puthex( rw_end.signature, 8 );
    Wdputslc( "H\n" );
}

/*
 * Dump resources.
 */
static void dmp_resrc( unsigned_16 size, unsigned_32 offset )
/***********************************************************/
{
    unsigned_16     i;
    lmf_resource    resrc;

    Wdputslc( "\n" );
    Banner( "Resource Table" );
    Wread( &resrc, sizeof( lmf_resource ) );
    Wdputs( "resource type (0==usage) = " );
    Puthex( resrc.res_type, 4 );
    Wdputslc( "H\n" );
    Wdputs( "zeros                    = " );
    for( i = 0; i < 3; i++ ) {              // 3 spares (zeros)
        Puthex( resrc.spare[i], 4 );
        Wdputs( "H   " );
    }
    Wdputslc( "\n" );
    size -= sizeof( lmf_resource );
    Wdputs( "size                     = " );
    Puthex( size, 4 );
    Wdputslc( "H\n" );
    offset += sizeof( lmf_resource );
    if( Options_dmp & RESRC_DMP ) {
        Dmp_seg_data( offset, size );
    }
}

/*
 * Dump fixups.
 */
static void dmp_fixup( unsigned_16 size, bool float_pt )
/******************************************************/
{
    lmf_data        fixup;
    unsigned_16     i;

    Wdputslc( "\n" );
    if( float_pt ) {
        Banner( "Floating Point Fixup Table" );
    } else {
        Banner( "Fixup Table" );
    }
    Wdputs( "size = " );
    Puthex( size, 4 );
    if( Options_dmp & FIX_DMP ) {
        if( float_pt ) {
            Wdputslc( "      segment : type:offset\n" );
        } else {
            Wdputslc( "      segment : offset\n" );
        }
        for( i = 0; i < size / sizeof( lmf_data ); i++ ) {
            Wread( &fixup, sizeof( lmf_data ) );
            if( i != 0 ) {
                if( i % 4 == 0 ) {
                    Wdputslc( "\n" );
                } else {
                    Wdputs( "     " );
                }
            }
            Puthex( fixup.segment, 4 );
            Wdputs( " : " );
            if( float_pt ) {
                Puthex( fixup.offset >> 28, 1 );
                Wdputc( ':' );
                Puthex( fixup.offset, 7 );
            } else {
                Puthex( fixup.offset, 8 );
            }
        }
    }
    Wdputslc( "\n" );
}

static void dmp_banner( void )
/****************************/
{
    char        buff[80];

    sprintf( buff, "Data Table %u", Data_count );
    Banner( buff );
}

/*
 * Dump data.
 */
static void dmp_data( unsigned_16 size, unsigned_32 offset )
/**********************************************************/
{
    lmf_data        data;

    Wdputslc( "\n" );
    Data_count++;
    dmp_banner();
    Wread( &data, sizeof( lmf_data ) );
    Dump_header( (char *)&data.segment, qnx_data_msg );
    size -= sizeof( lmf_data );
    Wdputs( "size                   =     " );
    Puthex( size, 4 );
    Wdputslc( "H\n" );
    offset += sizeof( lmf_data );
    if( Options_dmp & (DOS_SEG_DMP | OS2_SEG_DMP) ) {
        if( Segspec == 0 || Segspec == Data_count ) {
            Dmp_seg_data( offset, size );
        }
    }
}

/*
 * Dump linear fixups.
 */
static void dmp_lin_fix( unsigned_16 size )
/*****************************************/
{
    unsigned_16     seg_indx, i;
    unsigned_32     offset;

    Wdputslc( "\n" );
    Banner( "Linear Fixup Table" );
    Wread( &seg_indx, sizeof( unsigned_16 ) );
    Wdputs( "fixup segment index = " );
    Puthex( seg_indx, 4 );
    size -= sizeof( unsigned_16 );
    Wdputs( "     size = " );
    Puthex( size, 4 );
    if( Options_dmp & FIX_DMP ) {
        Wdputslc( "\n" );
        Wdputslc( "80000000H = target is in code segment\n" );
        for( i = 0; i < size / sizeof( unsigned_32 ); i++ ) {
            Wread( &offset, sizeof( unsigned_32 ) );
            if( i != 0 ) {
                if( i % 6 == 0 ) {
                    Wdputslc( "\n" );
                } else {
                    Wdputs( "     " );
                }
            }
            Puthex( offset, 8 );
        }
    }
    Wdputslc( "\n" );
}

/*
 * get type of next record.
 */
static void get_rec_type( unsigned_8 rtype, unsigned_16 size, unsigned_32 offset )
/********************************************************************************/
{
    switch( rtype ) {
    case LMF_COMMENT_REC:
        Wdputslc( "\n" );
        Banner( "Comments" );
        Dmp_seg_data( offset, size );
        break;
    case LMF_LOAD_REC:
        dmp_data( size, offset );
        break;
    case LMF_FIXUP_REC:
        dmp_fixup( size, false );
        break;
    case LMF_8087_FIXUP_REC:
        dmp_fixup( size, true );
        break;
    case LMF_RESOURCE_REC:
        dmp_resrc( size, offset );
        break;
    case LMF_RW_END_REC:
        dmp_rw_end();
        break;
    case LMF_LINEAR_FIXUP_REC:
        dmp_lin_fix( size );
        break;
    default:
        Wdputs( "unknown record type: " );
        Putdec( rtype );
        Wdputslc( "\n" );
        break;
    }
}

/*
 * Dump the QNX header, if any.
 */
bool Dmp_qnx_head( void )
/***********************/
{
    lmf_record      qnx_rec;
    unsigned_16     size;
    unsigned_32     offset;

    Wlseek( 0 );
    Wread( &qnx_rec, sizeof( lmf_record ) );
    if( qnx_rec.rec_type == LMF_HEADER_REC ) {
        Wread( &Qnx_head, sizeof( lmf_header ) );
        if( Qnx_head.version != QNX_VERSION ) {
            return( false );
        }
        Banner( "QNX EXE Header" );
        Dump_header( (char *)&Qnx_head.version, qnx_def_msg );
        dmp_flag( Qnx_head.cflags );
        size = ( qnx_rec.data_nbytes - sizeof( lmf_header ) ) / 4;
        dmp_seg( size );
        offset = qnx_rec.data_nbytes + sizeof( lmf_record );
        for( ;; ) {
            Wlseek( offset );
            Wread( &qnx_rec, sizeof( lmf_record ) );
            if( qnx_rec.rec_type == LMF_IMAGE_END_REC ) {
                break;
            }
            offset += sizeof( lmf_record );
            get_rec_type( qnx_rec.rec_type, qnx_rec.data_nbytes, offset );
            offset += qnx_rec.data_nbytes;
        }
        return( true );
    }
    return( false );
}
