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
* Description:  File I/O "move back" routines
*
****************************************************************************/

#include "ftnstd.h"
#include "ftextfun.h"
#include "ftextvar.h"
#include "posio.h"


#define NO_CHAR         0xffff

static  uint    GetPrevChar( b_file *io ) {
//=========================================

// Get previous character in file.

    if( CurrFileOffset( io ) == 0 ) return( NO_CHAR );
    if( SysSeek( io, -1, SEEK_CUR ) < 0 ) {
        return( NO_CHAR );
    }
    if( ( io->attrs & READ_AHEAD ) && io->b_curs < io->read_len ) {
        return( io->buffer[ io->b_curs ] );
    } else if( io->b_curs < io->high_water ) {
        return( io->buffer[ io->b_curs ] );
    }
    return( NO_CHAR );
}


void    FBackspace( b_file *io, int rec_size ) {
//==============================================

// Backspace a file.

    uint        ch;
    unsigned_32 offset;
    bool        start_of_logical_record;

    IOOk( io );
    if( io->attrs & REC_VARIABLE ) {
        for(;;) {
            offset = sizeof( unsigned_32 );
            if( SysSeek( io, -offset, SEEK_CUR ) < 0 ) return;
            if( SysRead( io, (char *)(&offset), sizeof( unsigned_32 ) ) ==
                READ_ERROR ) return;
            if( offset & 0x80000000 ) {
                offset &= 0x7fffffff;
                start_of_logical_record = FALSE;
            } else {
                start_of_logical_record = TRUE;
            }
            SysSeek( io, -( offset + 2 * sizeof( unsigned_32 ) ), SEEK_CUR );
            if( start_of_logical_record ) break;
        }
    } else if( io->attrs & REC_TEXT ) {
        // skip first record separator
        if( GetPrevChar( io ) == NO_CHAR ) return;
        for(;;) {
            ch = GetPrevChar( io );
            if( ch == NO_CHAR ) return;
            if( ch == LF ) break;
        }
        // give back record separator
        SysSeek( io, +1, SEEK_CUR );
    } else {
        SysSeek( io, -rec_size, SEEK_CUR );
    }
}
