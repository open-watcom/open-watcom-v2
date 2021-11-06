/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  OS/2 implementation of open() and _sopen().
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <share.h>
#include <dos.h>
#include <dosfunc.h>
#include <direct.h>
#include <string.h>
#define INCL_LONGLONG
#include <wos2.h>
#include "os2fil64.h"
#include "rtdata.h"
#include "rterrno.h"
#include "rtumask.h"
#include "fileacc.h"
#include "openmode.h"
#include "iomode.h"
#include "seterrno.h"
#include "defwin.h"
#include "thread.h"


static int __F_NAME(__sopen,__wsopen)( const CHAR_TYPE *name, unsigned mode, unsigned share, va_list args )
{
    OS_UINT     error, actiontaken;
    OS_UINT     fileattr, openflag, openmode;
    HFILE       handle;
    unsigned    rwmode;
    mode_t      perm = S_IREAD | S_IWRITE;
    unsigned    iomode_flags;
#ifdef __WIDECHAR__
    char        mbName[MB_CUR_MAX * _MAX_PATH];     /* single-byte char */
#endif

    while( *name == STRING( ' ' ) )
        ++name;
#ifdef __WIDECHAR__
    /*** If necessary, convert the wide filename to multibyte form ***/
    if( wcstombs( mbName, name, sizeof( mbName ) ) == (size_t)-1 ) {
        mbName[0] = '\0';
    }
#endif
    if( mode & O_CREAT ) {
        perm = va_arg( args, int );
        if( mode & O_EXCL ) {
            openflag = OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_FAIL_IF_EXISTS ;
        } else if( mode & O_TRUNC ) {
            openflag = OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS;
        } else {
            openflag = OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS;
        }
    } else if( mode & O_TRUNC ) {
        openflag = OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS ;
    } else {
        openflag = OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS;
    }
    rwmode = mode & OPENMODE_ACCESS_MASK;
#ifdef _M_I86
    if( rwmode == OPEN_ACCESS_WRITEONLY && _osmode_REALMODE() ) {
        /* Can't open WRONLY file in bound application under DOS */
        rwmode = OPEN_ACCESS_READWRITE;
    }
#endif
    if( share == OPENMODE_DENY_COMPAT ) {
        share = OPEN_SHARE_DENYNONE;
    }
    openmode = share | rwmode;

    perm &= ~_RWD_umaskval;
/*      if( ( perm & S_IREAD ) && !( perm & S_IWRITE ) )  */
    if( (perm & S_IWRITE) == 0 ) {
        fileattr = FILE_READONLY;
    } else {
        fileattr = FILE_NORMAL;
    }

#ifndef _M_I86
    if( __os2_DosOpenL != NULL ) {
        error = __os2_DosOpenL( (PSZ)__F_NAME(name,mbName), &handle,
                    &actiontaken, 0, fileattr, openflag, openmode, 0 );
    } else {
#endif
        error = DosOpen( (PSZ)__F_NAME(name,mbName), &handle,
                    &actiontaken, 0, fileattr, openflag, openmode, 0 );
#ifndef _M_I86
    }
#endif
    if( error ) {
        return( __set_errno_dos( error ) );
    }

    if( handle >= __NFiles ) {
        DosClose( handle );
        _RWD_errno = ENOMEM;
        return( -1 );
    }
    iomode_flags = 0;
    if( rwmode == O_RDWR )              iomode_flags  = _READ | _WRITE;
    if( rwmode == O_RDONLY)             iomode_flags  = _READ;
    if( rwmode == O_WRONLY)             iomode_flags  = _WRITE;
    if( mode & O_APPEND )               iomode_flags |= _APPEND;
    if( mode & (O_BINARY|O_TEXT) ) {
        if( mode & O_BINARY )           iomode_flags |= _BINARY;
    } else {
        if( _RWD_fmode == O_BINARY )    iomode_flags |= _BINARY;
    }
    if( isatty( handle ) )              iomode_flags |= _ISTTY;

    __SetIOMode( handle, iomode_flags );
#ifdef DEFAULT_WINDOWING
    if( _WindowsNewWindow != NULL ) {
        if( ( __F_NAME(_stricmp,_wcsicmp)( name, STRING( "con" ) ) == 0 ) ||
            ( __F_NAME(_stricmp,_wcsicmp)( name, STRING( "\\dev\\con" ) ) == 0 ) ) {
            _WindowsNewWindow( NULL, handle, -1 );
        }
    }
#endif
    return( handle );
}


_WCRTLINK int __F_NAME(open,_wopen)( const CHAR_TYPE *name, int mode, ... )
{
    mode_t      permission;
    va_list     args;

    va_start( args, mode );
    permission = va_arg( args, int );
    va_end( args );
    return( __F_NAME(_sopen,_wsopen)( name, mode, SH_COMPAT, permission ) );
}


_WCRTLINK int __F_NAME(_sopen,_wsopen)( const CHAR_TYPE *name, int mode, int shflag, ... )
{
    va_list     args;
    int         ret;

    va_start( args, shflag );
    ret = __F_NAME(__sopen,__wsopen)( name, mode, shflag, args );
    va_end( args );
    return( ret );
}
