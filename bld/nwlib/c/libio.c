/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023-2026 The Open Watcom Contributors. All Rights Reserved.
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
#include <errno.h>

#include "clibext.h"


static libfile fileList;

void InitLibIo( void )
{
    fileList = NULL;
}

void FiniLibIo( void )
{
    libfile io;

    while( (io = fileList) != NULL ) {
        fileList = io->next;
        fclose( io->fp );
        MemFree( io->name );
        MemFree( io );
    }
}

libfile LibOpen( const char *name, bool write_to )
{
    FILE    *fp;
    libfile io;

    if( write_to ) {
        fp = fopen( name, "wb" );
    } else {
        fp = fopen( name, "rb" );
    }
    if( fp == NULL && errno == EMFILE ) {
        CloseOneInputLib();
        if( write_to ) {
            fp = fopen( name, "wb" );
        } else {
            fp = fopen( name, "rb" );
        }
    }
    if( fp == NULL ) {
        FatalError( ERR_CANT_OPEN, name, strerror( errno ) );
    }
    if( write_to ) {
        io = MemAlloc( sizeof( *io ) + WRITE_FILE_BUFFER_SIZE - 1);
    } else {
        io = MemAlloc( sizeof( *io ) + READ_FILE_BUFFER_SIZE - 1);
    }
    io->next = fileList;
    io->prev = NULL;
    if( fileList != NULL ) {
        fileList->prev = io;
    }
    fileList = io;
    io->write_to = write_to;
    io->fp = fp;
    io->name = MemStrdup( name );
    io->buf_size = 0;
    io->buf_pos = 0;
    fseek( fp, 0, SEEK_END );
    io->endpos = ftell( fp );
    fseek( fp, 0, SEEK_SET );
    return( io );
}

void LibReadError( libfile io )
{
    FatalError( ERR_CANT_READ, io->name, strerror( errno ) );
}

void LibWriteError( libfile io )
{
    FatalError( ERR_CANT_WRITE, io->name, strerror( errno ) );
}

void BadLibrary( libfile io )
/***************************/
{
    FatalError( ERR_BAD_LIBRARY, io->name );
}

static void LibFlush( libfile io )
{
    if( io->buf_size ) {
        if( fwrite( io->buffer, 1, io->buf_size, io->fp ) != io->buf_size ) {
            LibWriteError( io );
        }
        io->buf_size = 0;
    }
}

size_t LibRead( libfile io, void *buff, size_t len )
{
    size_t      ret;
    size_t      b_read;

    if( len > READ_FILE_BUFFER_SIZE ) {
        b_read = io->buf_size - io->buf_pos;
        if( b_read ) {
            memcpy( buff, io->buffer + io->buf_pos, b_read );
        }
        ret = fread( (char *)buff + b_read, 1, len - b_read, io->fp );
        if( ferror( io->fp ) ) {
            LibReadError( io );
        }
        b_read += ret;
        io->buf_size = 0;
        io->buf_pos = 0;
        return( b_read );
    }
    b_read = io->buf_size - io->buf_pos;
    if( b_read > len )
        b_read = len;
    if( b_read ) {
        memcpy( buff, io->buffer + io->buf_pos, b_read );
        io->buf_pos += b_read;
    }
    if( io->buf_pos == io->buf_size ) {
        ret = fread( io->buffer, 1, READ_FILE_BUFFER_SIZE, io->fp );
        if( ferror( io->fp ) ) {
            LibReadError( io );
        }
        io->buf_size = ret;
        io->buf_pos = 0;
        if( ret == 0 ) {
            return( b_read );
        }
        if( b_read < len ) {
            if( ret > len - b_read )
                ret = len - b_read;
            memcpy( (char *)buff + b_read, io->buffer, ret );
            b_read += ret;
            io->buf_pos += ret;
        }
    }
    return( b_read );
}

void LibWrite( libfile io, const void *buff, size_t len )
{
    size_t  num;

    if( len == 0 )
        return;
    if( len > WRITE_FILE_BUFFER_SIZE ) {
        LibFlush( io );
        if( fwrite( buff, 1, len, io->fp ) != len ) {
            LibWriteError( io );
        }
        return;
    }
    num = WRITE_FILE_BUFFER_SIZE - io->buf_size;
    if( num > len )
        num = len;
    memcpy( io->buffer + io->buf_size, buff, num );
    len -= num;
    io->buf_size += num;
    if( len ) {
        LibFlush( io );
        memcpy( io->buffer, (char *)buff + num, len );
        io->buf_size = len;
    }
}

void LibWriteU8( libfile io, unsigned_8 value )
{
    LibWrite( io, &value, sizeof( value ) );
}

void LibWriteU16LE( libfile io, unsigned_16 value )
{
    CONV_LE_16( value );
    LibWrite( io, &value, sizeof( value ) );
}

void LibWriteU32BE( libfile io, unsigned_32 value )
{
    CONV_BE_32( value );
    LibWrite( io, &value, sizeof( value ) );
}

void LibWriteU32LE( libfile io, unsigned_32 value )
{
    CONV_LE_32( value );
    LibWrite( io, &value, sizeof( value ) );
}

void LibWriteNulls( libfile io, size_t len )
{
    size_t  num;

    if( len == 0 )
        return;
    if( len > WRITE_FILE_BUFFER_SIZE ) {
        LibWriteNulls( io, len - WRITE_FILE_BUFFER_SIZE );
        len = WRITE_FILE_BUFFER_SIZE;
    }
    num = WRITE_FILE_BUFFER_SIZE - io->buf_size;
    if( num > len )
        num = len;
    memset( io->buffer + io->buf_size, 0, num );
    len -= num;
    io->buf_size += num;
    if( len ) {
        LibFlush( io );
        memset( io->buffer, 0, len );
        io->buf_size = len;
    }
}

void LibClose( libfile io )
{
    if( io->write_to ) {
        LibFlush( io );
    }
    if( fclose( io->fp ) != 0 ) {
        LibWriteError( io );
    }
    if( fileList == io ) {
        fileList = fileList->next;
    }
    if( io->next != NULL ) {
        io->next->prev = io->prev;
    }
    if( io->prev != NULL ) {
        io->prev->next = io->next;
    }
    MemFree( io->name );
    MemFree( io );
}

void LibSeek( libfile io, long where, int whence )
{
    if( io->write_to ) {
        LibFlush( io );
    } else {
        if( whence == SEEK_END ) {
            where += io->endpos - LibTell( io );
            whence = SEEK_CUR;
        } else if( whence == SEEK_SET ) {
            where -= LibTell( io );
            whence = SEEK_CUR;
        }
        if( ( io->buf_pos >= -where ) && ( io->buf_pos + where < io->buf_size ) ) {
            io->buf_pos += where;
            return;
        }
        where -= (long)( io->buf_size - io->buf_pos );
    }
    fseek( io->fp, where, whence );
    io->buf_size = 0;
    io->buf_pos = 0;
}

long LibTell( libfile io )
{
    if( io->write_to ) {
        return( (long)( ftell( io->fp ) + io->buf_size ) );
    } else {
        return( (long)( ftell( io->fp ) - io->buf_size + io->buf_pos ) );
    }
}
