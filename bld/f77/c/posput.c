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
* Description:  POSIX level i/o support
*
****************************************************************************/

#include "ftnstd.h"
#include "ftextfun.h"
#include "ftextvar.h"
#include "posio.h"
#include "sysbuff.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>

/* forward declarations */
static  void    PutTextRec( b_file *io, char *b, int len );
static  void    PutVariableRec( b_file *io, char *b, uint len );
static  void    PutFixedRec( b_file *io, char *b, uint len );
int             SysWrite( b_file *io, char *b, uint len );
void            ChopFile( b_file *io );

void    FPutRec( b_file *io, char *b, int len ) {
// Put a record to a file.

    IOOk( io );
    if( io->attrs & REC_TEXT ) {
        PutTextRec( io, b, len );
    } else if( io->attrs & REC_VARIABLE ) {
        PutVariableRec( io, b, len );
    } else {
        PutFixedRec( io, b, len );
    }
#if defined( __RT__ )
    if( io->attrs & TRUNC_ON_WRITE ) {
        ChopFile( io );
    }
#endif
}


#if defined( __RT__ )

void    ChopFile( b_file *io ) {
    long int    offset;

    offset = CurrFileOffset( io );
    if( io->attrs & BUFFERED ) {
        if( FlushBuffer( io ) < 0 ) return;
    }
    // We have to call lseek here, not SysSeek to ensure that the real
    // file offset is actually where we want it.
    if( lseek( io->handle, offset, SEEK_SET ) < 0 ) return;
    io->phys_offset = offset;
    if( chsize( io->handle, offset ) < 0 ) {
        FSetSysErr( io );
    }
}

#endif

#if 0
void    PutRec( char *b, int len ) {
// Put a record to standard output device.

    FPutRec( FStdOut, b, len );
}
#endif

static  void    PutTextRec( b_file *io, char *b, int len ) {
// Put a record to a file with "text" records.

    int         cc_len;
    char        *cc;
    char        tag[2];

    cc_len = 0;
    if( io->attrs & CARRIAGE_CONTROL ) {
        cc_len = FSetCC( (a_file *)io, *b, &cc );
        b++;    // skip carriage control character
        len--;  // ...
    }
    if( io->attrs & CARRIAGE_CONTROL ) {
        if( SysWrite( io, cc, cc_len ) == -1 ) return;
    }
    if( SysWrite( io, b, len ) == -1 ) return;
    if( ( io->attrs & CC_NOCR ) == 0 ) {
#if defined( __UNIX__ )
        tag[0] = LF;
        len = 1;
#else
        tag[0] = CR;
        len = 1;
        if( ( io->attrs & CC_NOLF ) == 0 ) {
            tag[1] = LF;
            ++len;
        }
#endif
        io->attrs &= ~CC_NOLF;
        if( SysWrite( io, tag, len ) == -1 ) return;
    }
}


static  void    PutVariableRec( b_file *io, char *b, uint len ) {
// Put a record to a file with "variable" records.

    unsigned_32 tag;

    tag = len;
    if( io->attrs & LOGICAL_RECORD ) {
        tag |= 0x80000000;
    }
    if( SysWrite( io, (char *)(&tag), sizeof( unsigned_32 ) ) == -1 ) return;
    if( SysWrite( io, b, len ) == -1 ) return;
    if( SysWrite( io, (char *)(&tag), sizeof( unsigned_32 ) ) == -1 ) return;
}


static  void    PutFixedRec( b_file *io, char *b, uint len ) {
// Put a record to a file with "fixed" records.

    if( SysWrite( io, b, len ) == -1 ) return;
}


uint    writebytes( b_file *io, char *buff, uint len ) {
    int         written;
    uint        total;
    uint        amt;

    total = 0;
    amt = MAX_SYSIO_SIZE;
    while( len != 0 ) {
        if( len < amt )
            amt = len;
        written = write( io->handle, buff, amt );
        if( written < 0 ) {
            FSetSysErr( io );
            return( 0 );
        }
        io->attrs &= ~READ_AHEAD;
        io->phys_offset += written;
        total += written;
        buff += written;
        len -= written;
        if( written < amt ) {
            FSetErr( IO_DISK_FULL, io );
            return( 0 );
        }
    }
    return( total );
}


int SysWrite( b_file *io, char *b, uint len ) {
    uint        amt;

    if( len == 0 ) return( 0 );
    if( io->attrs & BUFFERED ) {
        // copy any amt that can fit into remaining portion of current buffer
        amt = io->buff_size - io->b_curs;
        if( amt > len ) {
            amt = len;
        }
        memcpy( &io->buffer[ io->b_curs ], b, amt );
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
            if( FlushBuffer( io ) < 0 ) return( -1 );
            b += amt;
            if( len > io->buff_size ) {
                // write out a multiple of io->buff_size bytes
                amt = len - len % io->buff_size;
                writebytes( io, b, amt );
                if( io->stat != IO_OK ) return( -1 );
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
        if( io->stat != IO_OK ) return( -1 );
    }
    return( 0 );
}
