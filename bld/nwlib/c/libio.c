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


#include <wlib.h>

static libfile fileList;

void InitLibIo()
{
    fileList = NULL;
}

void ResetLibIo()
{
    libfile lio;

    while (fileList) {
        lio = fileList->next;
        close(fileList->io);
        MemFreeGlobal(fileList->name);
        MemFreeGlobal(fileList);
        fileList = lio;
    }
}

libfile LibOpen( char *name, int access )
{
    int io;
    libfile lio;
    if( access & O_CREAT ){
        io = open( name, access, S_IREAD | S_IWRITE | S_IEXEC);
    } else {
        io = open( name, access );
    }


    if( io == -1 && errno == EMFILE ) {
        CloseOneInputLib();
        if( access & O_CREAT ){
            io = open( name, access, S_IREAD | S_IWRITE | S_IEXEC);
        } else {
            io = open( name, access );
        }
    }
    if( io == -1 ) {
        FatalError( ERR_CANT_OPEN, name, strerror( errno ) );
    }
    if( access & O_WRONLY ) {
        lio = MemAllocGlobal( sizeof( *lio ) + WRITE_FILE_BUFFER_SIZE - 1);
    } else {
        lio = MemAllocGlobal( sizeof( *lio ) + READ_FILE_BUFFER_SIZE - 1);
    }
    lio->next = fileList;
    lio->prev = NULL;
    if( fileList ) {
        fileList->prev = lio;
    }
    fileList = lio;
    lio->access = access;
    lio->io = io;
    lio->name = DupStrGlobal( name );
    lio->buf_size = 0;
    lio->buf_pos = 0;
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

static void LibFlush( libfile lio )
{
    if( lio->buf_size ) {
        if( write( lio->io, lio->buffer, lio->buf_size ) != lio->buf_size ) {
            LibWriteError( lio );
        }
        lio->buf_size = 0;
    }
}

file_offset LibRead( libfile lio, void *buff, file_offset len )
{
    signed_32 ret;
    file_offset b_read;

    if( len > READ_FILE_BUFFER_SIZE ) {
        b_read = lio->buf_size - lio->buf_pos;
        if( b_read ) {
            memcpy( buff, lio->buffer + lio->buf_pos, b_read );
        }
        ret = read( lio->io, (char *)buff + b_read, len - b_read );
        if( ret < 0 ) {
            LibReadError( lio );
        }
        b_read += ret;
        lio->buf_size = 0;
        lio->buf_pos = 0;
        return( b_read );
    }
    b_read = min( len, lio->buf_size - lio->buf_pos );
    if( b_read ) {
        memcpy( buff, lio->buffer + lio->buf_pos, b_read );
        lio->buf_pos += b_read;
    }
    if( (lio->buf_pos == lio->buf_size) ) {
        ret = read( lio->io, lio->buffer, READ_FILE_BUFFER_SIZE );
        if( ret < 0 ) {
            LibReadError( lio );
        }
        lio->buf_size = ret;
        lio->buf_pos = 0;
        if( ret == 0 ) {
            return( b_read );
        }
        if( b_read < len ) {
            ret = min( ret, len - b_read );
            memcpy( ( (char  *)buff ) + b_read, lio->buffer, ret );
            b_read += ret;
            lio->buf_pos += ret;
        }
    }
    return( b_read );
}

void LibWrite( libfile lio, void *buff, file_offset len )
{
    file_offset num;

    if( len > WRITE_FILE_BUFFER_SIZE ) {
        LibFlush( lio );
        if( write( lio->io, buff, len ) != len ) {
            LibWriteError( lio );
        }
        return;
    }
    num = min( WRITE_FILE_BUFFER_SIZE - lio->buf_size, len );
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
    if( lio->access & O_WRONLY ) {
        LibFlush( lio );
    }
    if( close( lio->io ) != 0 ) {
        LibWriteError( lio );
    }
    if( fileList == lio ) {
        fileList = fileList->next;
    }
    if( lio->next ) {
        lio->next->prev = lio->prev;
    }
    if( lio->prev ) {
        lio->prev->next = lio->next;
    }
    MemFreeGlobal( lio->name );
    MemFreeGlobal( lio );
}

void LibSeek( libfile lio, long where, int whence )
{
    if( lio->access & O_WRONLY ) {
        LibFlush( lio );
    } else {
        if( whence == SEEK_END ) {
            where += filelength( lio->io ) - LibTell( lio );
            whence = SEEK_CUR;
        } else if( whence == SEEK_SET ) {
            where -= LibTell( lio );
            whence = SEEK_CUR;
        }
        if( ( lio->buf_pos >= -where ) && ( (lio->buf_pos + where) < lio->buf_size ) ) {
            lio->buf_pos += where;
            return;
        }
        where -= lio->buf_size - lio->buf_pos;
    }
    lseek( lio->io, where, whence );
    lio->buf_size = 0;
    lio->buf_pos = 0;
}

file_offset LibTell( libfile lio )
{
    if( lio->access & O_WRONLY ) {
        return( tell( lio->io ) + lio->buf_size );
    } else {
        return( tell( lio->io ) - lio->buf_size + lio->buf_pos );
    }
}
