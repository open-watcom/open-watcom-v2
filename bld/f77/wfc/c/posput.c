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
#include <string.h>
#include <limits.h>
#include "fileio.h"
#include "sysbuff.h"
#include "posput.h"
#include "posseek.h"
#include "fileerr.h"
#include "posflush.h"

#include "clibext.h"


#if defined( _MSC_VER ) && defined( _WIN64 )
static ssize_t  posix_write( int fildes, void const *buffer, size_t nbyte )
{
    unsigned    write_len;
    unsigned    amount;
    size_t      size;

    amount = INT_MAX;
    size = 0;
    while( nbyte > 0 ) {
        if( amount > nbyte )
            amount = (unsigned)nbyte;
        write_len = _write( fildes, buffer, amount );
        if( write_len == (unsigned)-1 ) {
            return( (ssize_t)-1 );
        }
        size += write_len;
        if( write_len != amount ) {
            break;
        }
        buffer = (char *)buffer + amount;
        nbyte -= amount;
    }
    return( size );
}
#else
#define posix_write     write
#endif


size_t  writebytes( b_file *io, const char *buff, size_t len )
{
    size_t      written;
    size_t      total;
    size_t      amt;

    total = 0;
    amt = MAX_SYSIO_SIZE;
    while( len ) {
        if( amt > len )
            amt = len;
        written = posix_write( io->handle, buff, amt );
        if( written == (size_t)-1 ) {
            FSetSysErr( io );
            return( 0 );
        }
        io->attrs &= ~READ_AHEAD;
        io->phys_offset += written;
        total += written;
        buff += written;
        len -= written;
        if( written < amt ) {
            FSetErr( POSIO_DISK_FULL, io );
            return( 0 );
        }
    }
    return( total );
}


int SysWrite( b_file *io, const char *b, size_t len )
{
    size_t      amt;

    if( len == 0 )
        return( 0 );
    if( io->attrs & BUFFERED ) {
        // copy any amt that can fit into remaining portion of current buffer
        amt = io->buff_size - io->b_curs;
        if( amt > len ) {
            amt = len;
        }
        memcpy( &io->buffer[io->b_curs], b, amt );
        io->attrs |= DIRTY_BUFFER;
        io->b_curs += amt;
        if( io->b_curs > io->high_water ) {
            io->high_water = io->b_curs;
            if( ( io->attrs & READ_AHEAD ) == 0 ) {
                io->read_len = io->high_water;
            }
        }
        // now check if there was any left over
        len -= amt;
        if( len ) {
            // flush the buffer
            if( FlushBuffer( io ) < 0 )
                return( -1 );
            b += amt;
            if( len > io->buff_size ) {
                // write out a multiple of io->buff_size bytes
                amt = len - len % io->buff_size;
                writebytes( io, b, amt );
                if( io->stat != POSIO_OK )
                    return( -1 );
                b += amt;
                len -= amt;
            }
            // now whatever is left will fit in the buffer
            if( len ) {
                memcpy( io->buffer, b, len );
                io->attrs |= DIRTY_BUFFER;
                io->high_water = len;
                io->read_len = len;
                io->b_curs = len;
            }
        }
    } else {
        writebytes( io, b, len );
        if( io->stat != POSIO_OK ) {
            return( -1 );
        }
    }
    return( 0 );
}


void    FPutRecText( b_file *io, const char *b, size_t len, bool nolf )
//=====================================================================
// Put a record to a file with "text" records.
{
    char        tag[2];

    FSetIOOk( io );
    if( SysWrite( io, b, len ) == -1 )
        return;
#if defined( __UNIX__ )
    (void)nolf;

    tag[0] = CHAR_LF;
    len = 1;
#else
    tag[0] = CHAR_CR;
    len = 1;
    if( !nolf ) {
        tag[1] = CHAR_LF;
        ++len;
    }
#endif
    SysWrite( io, tag, len );
}

void    FPutRecFixed( b_file *io, const char *b, size_t len )
//===========================================================
// Put a record to a file with "fixed" records.
{
    FSetIOOk( io );
    SysWrite( io, b, len );
}
