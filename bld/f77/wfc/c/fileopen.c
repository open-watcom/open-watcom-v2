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
* Description:  ISO C I/O support for open/close files
*
****************************************************************************/


#include "ftnstd.h"
#if defined( __NT__ )
    #include <windows.h>
#endif
#include "wio.h"
#include "fileio.h"
#include "fileopen.h"
#include "fileerr.h"
#include "fmemmgr.h"

#include "clibext.h"


file_handle     FStdOut = NULL;

static b_file   _FStdOut = {0};

static  int     IOBufferSize = { IO_BUFFER };

void    InitFileIO( uint buff_size )
{
    // Initialize stdout i/o.
    _FStdOut.attrs     = REC_TEXT | WRITE_ONLY;
    _FStdOut.fp        = stdout;
    _FStdOut.buff_size = MIN_BUFFER;
    FSetIOOk( &_FStdOut );
    FStdOut = &_FStdOut;
    // Initialize i/o buffer size.
    if( buff_size < MIN_BUFFER ) {
        buff_size = MIN_BUFFER;
    }
    IOBufferSize = buff_size;
}

b_file  *Openf( const char *f, const char *mode, f_attrs attrs )
// Open a file.
{
    FILE        *fp;
    b_file      *io;
    struct stat info;
    int         buff_size;

    fp = fopen( f, mode );
    if( fp == NULL ) {
        FSetSysErr( NULL );
        return( NULL );
    }
    if( stat( f, &info ) == -1 ) {
        fclose( fp );
        FSetSysErr( NULL );
        return( NULL );
    }
    if( S_ISCHR( info.st_mode ) ) {
        io = FMemAlloc( offsetof( b_file, read_len ) );
        // Turn off truncate just in case we turned it on by accident due to
        // a buggy NT dos box.  We NEVER want to truncate a device.
//        attrs &= ~TRUNC_ON_WRITE;
//        attrs |= CHAR_DEVICE;
    } else {
        attrs |= BUFFERED;
        buff_size = IOBufferSize;
        io = FMemAlloc( sizeof( b_file ) + IOBufferSize - MIN_BUFFER );
        if( ( io == NULL ) && ( IOBufferSize > MIN_BUFFER ) ) {
            // buffer is too big (low on memory) so use small buffer
            buff_size = MIN_BUFFER;
            io = FMemAlloc( sizeof( b_file ) );
        }
    }
    if( io == NULL ) {
        fclose( fp );
        FSetErr( FILEIO_NO_MEM, NULL );
    } else {
        if( mode[0] == 'w' )
            attrs |= WRITE_ONLY;
        io->attrs = attrs;
        io->fp = fp;
        io->phys_offset = 0;
        if( attrs & BUFFERED ) {
            io->b_curs = 0;
            io->read_len = 0;
            io->buff_size = buff_size;
            io->high_water = 0;
        }
        FSetIOOk( io );
    }
    return( io );
}

void    Closef( b_file *io )
// Close a file.
{
    if( fclose( io->fp ) ) {
        FSetSysErr( io );
        return;
    }
    FMemFree( io );
    FSetIOOk( NULL );
}
