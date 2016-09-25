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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "wlib.h"

#include "clibext.h"


static libfile fileList;

void InitLibIo( void )
{
    fileList = NULL;
}

void FiniLibIo( void )
{
    libfile lio;

    while( (lio = fileList) != NULL ) {
        fileList = lio->next;
        fclose( lio->io );
        MemFreeGlobal( lio->name );
        MemFreeGlobal( lio );
    }
}

libfile LibOpen( const char *name, bool write_to )
{
    FILE    *io;
    libfile lio;

    if( write_to ) {
        io = fopen( name, "wb" );
    } else {
        io = fopen( name, "rb" );
    }
    if( io == NULL && errno == EMFILE ) {
        CloseOneInputLib();
        if( write_to ) {
            io = fopen( name, "wb" );
        } else {
            io = fopen( name, "rb" );
        }
    }
    if( io == NULL ) {
        FatalError( ERR_CANT_OPEN, name, strerror( errno ) );
    }
    if( write_to ) {
        lio = MemAllocGlobal( sizeof( *lio ) + WRITE_FILE_BUFFER_SIZE - 1);
    } else {
        lio = MemAllocGlobal( sizeof( *lio ) + READ_FILE_BUFFER_SIZE - 1);
    }
    lio->next = fileList;
    lio->prev = NULL;
    if( fileList != NULL ) {
        fileList->prev = lio;
    }
    fileList = lio;
    lio->write_to = write_to;
    lio->io = io;
    lio->name = DupStrGlobal( name );
    lio->buf_size = 0;
    lio->buf_pos = 0;
    fseek( io, 0, SEEK_END );
    lio->endpos = ftell( io );
    fseek( io, 0, SEEK_SET );
    return( lio );
}

void LibReadError( libfile lio )
{
    FatalError( ERR_CANT_READ, lio->name, strerror( errno ) );
}

void LibWriteError( libfile lio )
{
    FatalError( ERR_CANT_WRITE, lio->name, strerror( errno ) );
}

void BadLibrary( const char *name )
/*********************************/
{
    FatalError( ERR_BAD_LIBRARY, name );
}

static void LibFlush( libfile lio )
{
    if( lio->buf_size ) {
        if( fwrite( lio->buffer, 1, lio->buf_size, lio->io ) != lio->buf_size ) {
            LibWriteError( lio );
        }
        lio->buf_size = 0;
    }
}

size_t LibRead( libfile lio, void *buff, size_t len )
{
    size_t      ret;
    size_t      b_read;

    if( len > READ_FILE_BUFFER_SIZE ) {
        b_read = lio->buf_size - lio->buf_pos;
        if( b_read ) {
            memcpy( buff, lio->buffer + lio->buf_pos, b_read );
        }
        ret = fread( (char *)buff + b_read, 1, len - b_read, lio->io );
        if( ferror( lio->io ) ) {
            LibReadError( lio );
        }
        b_read += ret;
        lio->buf_size = 0;
        lio->buf_pos = 0;
        return( b_read );
    }
    b_read = lio->buf_size - lio->buf_pos;
    if( b_read > len )
        b_read = len;
    if( b_read ) {
        memcpy( buff, lio->buffer + lio->buf_pos, b_read );
        lio->buf_pos += b_read;
    }
    if( lio->buf_pos == lio->buf_size ) {
        ret = fread( lio->buffer, 1, READ_FILE_BUFFER_SIZE, lio->io );
        if( ferror( lio->io ) ) {
            LibReadError( lio );
        }
        lio->buf_size = ret;
        lio->buf_pos = 0;
        if( ret == 0 ) {
            return( b_read );
        }
        if( b_read < len ) {
            if( ret > len - b_read )
                ret = len - b_read;
            memcpy( (char *)buff + b_read, lio->buffer, ret );
            b_read += ret;
            lio->buf_pos += ret;
        }
    }
    return( b_read );
}

void LibWrite( libfile lio, const void *buff, size_t len )
{
    size_t  num;

    if( len > WRITE_FILE_BUFFER_SIZE ) {
        LibFlush( lio );
        if( fwrite( buff, 1, len, lio->io ) != len ) {
            LibWriteError( lio );
        }
        return;
    }
    num = WRITE_FILE_BUFFER_SIZE - lio->buf_size;
    if( num > len )
        num = len;
    memcpy( lio->buffer + lio->buf_size, buff, num );
    len -= num;
    lio->buf_size += num;
    if( len ) {
        LibFlush( lio );
        memcpy( lio->buffer, (char *)buff + num, len );
        lio->buf_size = len;
    }
}

void LibClose( libfile lio )
{
    if( lio->write_to ) {
        LibFlush( lio );
    }
    if( fclose( lio->io ) != 0 ) {
        LibWriteError( lio );
    }
    if( fileList == lio ) {
        fileList = fileList->next;
    }
    if( lio->next != NULL ) {
        lio->next->prev = lio->prev;
    }
    if( lio->prev != NULL ) {
        lio->prev->next = lio->next;
    }
    MemFreeGlobal( lio->name );
    MemFreeGlobal( lio );
}

void LibSeek( libfile lio, long where, int whence )
{
    if( lio->write_to ) {
        LibFlush( lio );
    } else {
        if( whence == SEEK_END ) {
            where += lio->endpos - LibTell( lio );
            whence = SEEK_CUR;
        } else if( whence == SEEK_SET ) {
            where -= LibTell( lio );
            whence = SEEK_CUR;
        }
        if( ( lio->buf_pos >= -where ) && ( lio->buf_pos + where < lio->buf_size ) ) {
            lio->buf_pos += where;
            return;
        }
        where -= lio->buf_size - lio->buf_pos;
    }
    fseek( lio->io, where, whence );
    lio->buf_size = 0;
    lio->buf_pos = 0;
}

long LibTell( libfile lio )
{
    if( lio->write_to ) {
        return( ftell( lio->io ) + lio->buf_size );
    } else {
        return( ftell( lio->io ) - lio->buf_size + lio->buf_pos );
    }
}
