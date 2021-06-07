/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
#include "posio.h"
#include "sysbuff.h"
#include "poscc.h"
#include "posput.h"
#include "posseek.h"
#include "poserr.h"
#include "posflush.h"
#include "posdat.h"


/* forward declarations */
static  void    PutTextRec( b_file *io, const char *b, size_t len );
static  void    PutVariableRec( b_file *io, const char *b, size_t len );
static  void    PutFixedRec( b_file *io, const char *b, size_t len );

void    FPutRec( b_file *io, const char *b, size_t len )
//======================================================
// Put a record to a file.
{
    FSetIOOk( io );
    if( io->attrs & REC_TEXT ) {
        PutTextRec( io, b, len );
    } else if( io->attrs & REC_VARIABLE ) {
        PutVariableRec( io, b, len );
    } else {
        PutFixedRec( io, b, len );
    }
    if( io->attrs & TRUNC_ON_WRITE ) {
        ChopFile( io );
    }
}


void    ChopFile( b_file *io )
{
    long    offset;

    offset = CurrFileOffset( io );
    if( io->attrs & BUFFERED ) {
        if( FlushBuffer( io ) < 0 ) {
            return;
        }
    }
    // We have to call lseek here, not SysSeek to ensure that the real
    // file offset is actually where we want it.
    if( lseek( io->handle, offset, SEEK_SET ) == -1L )
        return;
    io->phys_offset = offset;
    if( chsize( io->handle, offset ) < 0 ) {
        FSetSysErr( io );
    }
}

#if 0
void    PutRec( const char *b, size_t len )
// Put a record to standard output device.
{
    FPutRec( FStdOut, b, len );
}
#endif

static  void    PutTextRec( b_file *io, const char *b, size_t len )
//=================================================================
// Put a record to a file with "text" records.
{
    int         cc_len;
    const char  *cc;
    char        tag[2];

    cc_len = 0;
    if( io->attrs & CARRIAGE_CONTROL ) {
        cc_len = FSetCC( io, *b, &cc );
        b++;    // skip carriage control character
        len--;  // ...
    }
    if( io->attrs & CARRIAGE_CONTROL ) {
        if( SysWrite( io, cc, cc_len ) == -1 ) {
            return;
        }
    }
    if( SysWrite( io, b, len ) == -1 )
        return;
    if( ( io->attrs & CC_NOCR ) == 0 ) {
#if defined( __UNIX__ )
        tag[0] = CHAR_LF;
        len = 1;
#else
        tag[0] = CHAR_CR;
        len = 1;
        if( ( io->attrs & CC_NOLF ) == 0 ) {
            tag[1] = CHAR_LF;
            ++len;
        }
#endif
        io->attrs &= ~CC_NOLF;
        if( SysWrite( io, tag, len ) == -1 ) {
            return;
        }
    }
}


static  void    PutVariableRec( b_file *io, const char *b, size_t len )
//=====================================================================
// Put a record to a file with "variable" records.
{
    unsigned_32 tag;

    tag = len;
    if( io->attrs & LOGICAL_RECORD ) {
        tag |= 0x80000000;
    }
    if( SysWrite( io, (char *)(&tag), sizeof( tag ) ) == -1 )
        return;
    if( SysWrite( io, b, len ) == -1 )
        return;
    if( SysWrite( io, (char *)(&tag), sizeof( tag ) ) == -1 ) {
        return;
    }
}


static  void    PutFixedRec( b_file *io, const char *b, size_t len )
//==================================================================
// Put a record to a file with "fixed" records.
{
    if( SysWrite( io, b, len ) == -1 ) {
        return;
    }
}


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
        written = write( io->handle, buff, amt );
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
