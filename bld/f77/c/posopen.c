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
#include "sopen.h"
#include "watcom.h"

#if defined( __RT__ )
#include "rmemmgr.h"

#define MEM_ALLOC       RMemAlloc
#define MEM_FREE        RMemFree

#else
#include "fmemmgr.h"

#define MEM_ALLOC       FMemAlloc
#define MEM_FREE        FMemFree

#endif

/* Forward declarations */
#if defined( __RT__ )
static  void    ChkRedirection( b_file *fp );
#endif

static  int     IOBufferSize = { IO_BUFFER };

void    InitStd( void )
{
// Initialize standard i/o.
#if !defined( __UNIX__ ) && defined( __WATCOMC__ )
    // don't call setmode() since we don't want to affect higher level
    // i/o so that if C function gets called, printf() works ok
// There is no __GetIOMode in the C runtime!
#define __GetIOMode  __IOMode

    extern      unsigned        __GetIOMode(int);
    extern      void            __SetIOMode(int,unsigned);

    __SetIOMode( STDIN_FILENO, __GetIOMode( STDIN_FILENO ) | _BINARY );
    __SetIOMode( STDOUT_FILENO, __GetIOMode( STDOUT_FILENO ) | _BINARY );
    __SetIOMode( STDERR_FILENO, __GetIOMode( STDERR_FILENO ) | _BINARY );
#endif
#if defined( __RT__ )
    ChkRedirection( FStdIn );
    ChkRedirection( FStdOut );
    ChkRedirection( FStdErr );
    if( __DevicesCC() ) {
        FStdOut->attrs |= CC_NOLF;
    }
#endif
}

#if defined( __RT__ )

static  void    ChkRedirection( b_file *fp ) {
// Check for redirection of standard i/o devices.
    struct stat         info;

    if( fstat( fp->handle, &info ) == -1 ) return;
    if( !S_ISCHR( info.st_mode ) ) {
        fp->attrs |= BUFFERED;
    }
}

#endif

void    SetIOBufferSize( uint buff_size ) {
    if( buff_size < MIN_BUFFER ) {
        buff_size = MIN_BUFFER;
    }
    IOBufferSize = buff_size;
}

b_file  *_AllocFile( int h, f_attrs attrs, long int fpos ) {
// Allocate file structure.
    b_file      *io;
    struct stat info;
    int         buff_size;

    if( fstat( h, &info ) == -1 ) {
        FSetSysErr( NULL );
        return( NULL );
    }
    attrs &= ~CREATION_MASK;
    if( S_ISCHR( info.st_mode ) ) {
        io = MEM_ALLOC( sizeof( a_file ) );
        // Turn off truncate just in case we turned it on by accident due to
        // a buggy NT dos box.  We NEVER want to truncate a device.
        attrs &= ~TRUNC_ON_WRITE;
        attrs |= CHAR_DEVICE;
    } else {
        attrs |= BUFFERED;
        buff_size = IOBufferSize;
        io = MEM_ALLOC( sizeof( b_file ) + IOBufferSize - MIN_BUFFER );
        if( ( io == NULL ) && ( IOBufferSize > MIN_BUFFER ) ) {
            // buffer is too big (low on memory) so use small buffer
            buff_size = MIN_BUFFER;
            io = MEM_ALLOC( sizeof( b_file ) );
        }
    }
    if( io == NULL ) {
        close( h );
        FSetErr( IO_NO_MEM, NULL );
    } else {
        if( attrs & CARRIAGE_CONTROL ) {
            attrs |= CC_NOLF;
        }
        io->attrs = attrs;
        io->handle = h;
        if( attrs & BUFFERED ) {
            io->b_curs = 0;
            io->read_len = 0;
            io->buff_size = buff_size;
            io->high_water = 0;
        }
        io->phys_offset = fpos;
        IOOk( io );
    }
    return( io );
}

b_file  *Openf( char *f, f_attrs attrs ) {
// Open a file.
    int         retc;
    long int    fpos;
#if defined( __WATCOMC__ ) || !defined( __UNIX__ )
    int         share;

    share = SH_COMPAT;
    if( attrs & S_DENYRW ) {
        share = SH_DENYRW;
    } else if( attrs & S_DENYWR ) {
        share = SH_DENYWR;
    } else if( attrs & S_DENYRD ) {
        share = SH_DENYRD;
    } else if( attrs & S_DENYNO ) {
        share = SH_DENYNO;
    }
#endif
    if( attrs & WRONLY ) {
        attrs |= WRITE_ONLY;
        if( attrs & APPEND ) {
            retc = sopen4( f, O_WRONLY | O_BINARY | O_CREAT, share, PMODE_RW );
        } else {
            retc = sopen4( f, O_WRONLY | O_BINARY | O_CREAT | O_TRUNC, share, PMODE_RW );
        }
    } else if( attrs & RDONLY ) {
        retc = sopen3( f, O_RDONLY | O_BINARY, share );
    } else { // if( attrs & RDWR ) {
        retc = sopen4( f, O_RDWR | O_BINARY | O_CREAT, share, PMODE_RW );
    }
    if( retc < 0 ) {
        FSetSysErr( NULL );
        return( NULL );
    }
    fpos = 0;
    if( attrs & APPEND ) {
        fpos = lseek( retc, 0, SEEK_END );
        if( fpos < 0 ) {
            FSetSysErr( NULL );
            close( retc );
            return( NULL );
        }
    }
    return( _AllocFile( retc, attrs, fpos ) );
}

void    Closef( b_file *io ) {
// Close a file.
    uint        cc_len;
    char        *cc;

    if( io->attrs & CARRIAGE_CONTROL ) {
        cc_len = FSetCC( io, ' ', &cc );
        if( SysWrite( io, cc, cc_len ) == -1 ) return;
    }
    if( FlushBuffer( io ) < 0 ) return;
    if( close( io->handle ) < 0 ) {
        FSetSysErr( io );
        return;
    }
    MEM_FREE( io );
    IOOk( NULL );
}
