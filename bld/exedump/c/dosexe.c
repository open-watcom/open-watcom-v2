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
* Description:  DOS MZ executable dumping routines.
*
****************************************************************************/


#include <stdio.h>
#include <setjmp.h>
#include "wio.h"
#include "wdglb.h"
#include "wdfunc.h"

static  char    *dos_exe_msg[] = {
    "2length of load module mod 200H                       = ",
    "2number of 200H pages in load module                  = ",
    "2number of relocation items                           = ",
    "2size of header in paragraphs                         = ",
    "2minimum number of paragraphs required above load mod = ",
    "2maximum number of paragraphs required above load mod = ",
    "2offset of stack segment in load module               = ",
    "2initial value of SP                                  = ",
    "2checksum                                             = ",
    "2initial value of IP                                  = ",
    "2offset of code segment within load module (segment)  = ",
    "2file offset of first relocation item                 = ",
    "2overlay number                                       = ",
    NULL
};

/*
 * dump the dos header information
 */
static void dmp_dos_head_info( void )
/***********************************/
{
    unsigned_32                     load_start;
    unsigned_32                     load_end;
    unsigned_16                     i;
    struct { unsigned_16 offset;
             unsigned_16 segment; } reloc;

    Banner( "DOS EXE Header" );
    Dump_header( (char *)&Dos_head.mod_size, dos_exe_msg );
    Load_len = Dos_head.file_size * 0x200 + Dos_head.mod_size;
    if( Dos_head.mod_size != 0 ) {
        Load_len -= 0x200;
    }
    Wdputslc( "load module length                                   = " );
    Puthex( Load_len, 8 );
    Wdputslc( "H\n\n" );
    if( Options_dmp & FIX_DMP ) {
        if( Dos_head.num_relocs != 0 ) {
            Wlseek( Dos_head.reloc_offset );
            Wdputslc( "segment:offset\n  " );
            for( i = 0; i < Dos_head.num_relocs; i++ ) {
                Wread( &reloc, sizeof( reloc ) );
                Puthex( reloc.segment, 4 );
                Wdputc( ':' );
                Puthex( reloc.offset, 4 );
                if( (i+1) % 6 ) {
                    Wdputs( "   " );
                } else {
                    Wdputslc( "\n  " );
                }
            }
            Wdputslc( "\n" );
            Wdputslc( "\n" );
        }
    }
    if( Options_dmp & DOS_SEG_DMP ) {
        load_start = Dos_head.hdr_size << 4;
        load_end = Load_len - load_start;
        Wdputslc( "load module =\n" );
        Dmp_seg_data( load_start, load_end );
        Wdputslc( "\n" );
    }
}

/*
 * Dump the Dos Executable Header, if any.
 */
bool Dmp_dos_head( void )
/***********************/
{
    Wread( &Dos_head, sizeof( Dos_head ) );
    if( Dos_head.signature != DOS_SIGNATURE ) {
        return( 0 );
    }
    dmp_dos_head_info();
    /* OS/2 and Windows use a special method where the DOS EXE header is slghtly
     * tweaked to point to the new style (NE, LE, LX, PE) header. DOS/16M simply
     * pastes a DOS EXE and protected mode executable together without modifying
     * the DOS exe in any way. If the file is larger than what the DOS EXE header
     * suggests, we may have a DOS/16M executable.
     */
    if( Dos_head.reloc_offset != OS2_EXE_HEADER_FOLLOWS ) {
        if( Load_len !=  WFileSize() ) {
            Wdputslc( "Additional file data follows DOS executable.\n\n" );
            New_exe_off = Load_len;
            return( 3 );
        } else {
            Wdputslc( "No New Executable header.\n" );
            return( 1 );
        }
    }
    Wlseek( OS2_NE_OFFSET );
    Wread( &New_exe_off, sizeof( New_exe_off ) );
    return( 2 );
}
