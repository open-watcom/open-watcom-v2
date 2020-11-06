/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  POSIX level i/o support
*
****************************************************************************/


#include "ftnstd.h"
#include "fileio.h"
#include "fileerr.h"
#include "posput.h"
#include "posflush.h"


int     FlushBuffer( b_file *io )
// Flush i/o buffer.
{
    size_t      amt;
    size_t      bytes_written;
    int         rc;

    if( ( io->attrs & BUFFERED ) == 0 )
        return( 0 );
    rc = 0;
    if( io->attrs & DIRTY_BUFFER ) {
        if( io->attrs & READ_AHEAD ) {
            if( fseek( io->fp, -(long)io->read_len, SEEK_CUR ) ) {
                return( -1 );
            }
            amt = io->high_water;
            if( amt < io->read_len ) {
                amt = io->read_len;
            }
            bytes_written = fwrite( io->buffer, 1, amt, io->fp );
            if( bytes_written != amt && ferror( io->fp ) ) {
                return( -1 );
            }
            io->phys_offset += bytes_written - io->read_len;
            if( bytes_written < amt ) {
                rc = -1;
            }
        } else {
            writebytes( io, io->buffer, io->high_water );
            if( !IOOk( io ) ) {
                rc = -1;
            }
        }
    }
    io->b_curs = 0;
    io->read_len = 0;
    io->high_water = 0;
    io->attrs &= ~(READ_AHEAD | DIRTY_BUFFER);
    return( rc );
}
