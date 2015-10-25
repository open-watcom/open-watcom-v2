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
#include "posio.h"

void    FSeekRec( b_file *io, unsigned_32 rec, uint recsize ) {
// Seek to specified record in file.

    IOOk( io );
    if( io->attrs & SEEK ) {
        if( io->attrs & REC_TEXT ) {
#if defined( __UNIX__ )
            recsize += sizeof( char );     // compensate for LF
#else
            recsize += 2 * sizeof( char ); // compensate for CR/LF
#endif
        } else if( io->attrs & REC_VARIABLE ) {
            recsize += 2 * sizeof( unsigned_32 ); // compensate for length tags
        }
        SysSeek( io, rec * recsize, SEEK_SET );
    } else {
        FSetErr( IO_BAD_OPERATION, io );
    }
}

long int        CurrFileOffset( b_file *io ) {
    long int    offs;

    offs = io->phys_offset + io->b_curs;
    if( io->attrs & READ_AHEAD ) {
        offs -= io->read_len;
    }
    return( offs );
}


int     SysSeek( b_file *io, long int new_offset, int seek_mode ) {
    long int    curr_offset;
    long int    new_page;
    long int    page_offset;
    uint        bytes_read;

    curr_offset = CurrFileOffset( io );
    if( seek_mode == SEEK_CUR ) {
        new_offset += curr_offset;
    }
    if( io->attrs & WRITE_ONLY ) {
        if( curr_offset != new_offset ) {
            if( FlushBuffer( io ) < 0 ) {
                FSetSysErr( io );
                return( -1 );
            }
            if( lseek( io->handle, new_offset, SEEK_SET ) == -1 ) {
                FSetSysErr( io );
                return( -1 );
            }
            io->phys_offset = new_offset;
        }
        return( 0 );
    } else if( io->attrs & READ_AHEAD ) {
        page_offset = io->phys_offset - io->read_len;
        if( page_offset <= new_offset ) {
            if( (new_offset < io->phys_offset) ||
                ( (io->attrs & PAST_EOF) &&
                  (new_offset < page_offset + io->buff_size) ) ) {
                // we have the part of file in memory still, or we know we're
                // at the end of the file and the offset we want is on this
                // page as well
                io->b_curs = new_offset - io->phys_offset + io->read_len;
                return( 0 );
            }
        }
    } else if( io->phys_offset <= new_offset &&
                            new_offset < io->phys_offset + io->buff_size ) {
        io->b_curs = new_offset - io->phys_offset;
        if( ( io->attrs & PAST_EOF ) || io->b_curs < io->high_water ) {
            // We already know that the EOF is on this page so don't bother
            // seeking and reading.  Or we already have the part of the
            // file in the buffer.
            return( 0 );
        }
        // we have part of this page in memory already... so read the
        // rest of the page
        if( lseek( io->handle, io->high_water, SEEK_CUR ) < 0 ) {
            FSetSysErr( io );
            return( -1 );
        }
        io->phys_offset += io->high_water;
        bytes_read = readbytes( io, io->buffer + io->high_water,
                                            io->buff_size - io->high_water );
        if( bytes_read == READ_ERROR ) {
            if( io->stat != IO_EOF ) return( -1 );
            IOOk( io );
            io->phys_offset -= io->high_water;  // restore offset
            if( lseek( io->handle, -(long)io->high_water, SEEK_CUR ) < 0 ) {
                FSetSysErr( io );
                return( -1 );
            }
            io->attrs |= PAST_EOF;              // we now know the EOF is here
        } else {
            io->attrs |= READ_AHEAD;
            io->read_len = io->high_water + bytes_read;
            if( bytes_read < io->buff_size - io->high_water ) {
                io->attrs |= PAST_EOF;          // we now know the EOF is here
            }
        }
        return( 0 );
    }
    if( new_offset != curr_offset ) {
        if( FlushBuffer( io ) < 0 ) return( 0 );
        // round down to the nearest multiple of buff_size
        new_page = new_offset - new_offset % io->buff_size;
        if( lseek( io->handle, new_page, SEEK_SET ) < 0 ) {
            FSetSysErr( io );
            return( -1 );
        }
        io->phys_offset = new_page;
        io->b_curs = new_offset - new_page;
        bytes_read = readbytes( io, io->buffer, io->buff_size );
        if( bytes_read == READ_ERROR ) {
            if( io->stat != IO_EOF ) return( -1 );
            IOOk( io );
            io->attrs |= PAST_EOF;
        } else {
            if( bytes_read < io->buff_size ) {
                io->attrs |= PAST_EOF;
            } else {
                io->attrs &= ~PAST_EOF;
            }
            io->attrs |= READ_AHEAD;
            io->read_len = bytes_read;
        }
    }
    return( 0 );
}

#if 0
void    FSeekAbs( b_file *io, unsigned_32 offset ) {
    SysSeek( io, offset, SEEK_SET );
}
#endif

long int        FGetFilePos( b_file *io ) {
    return( CurrFileOffset( io ) );
}
