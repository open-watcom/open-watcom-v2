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
* Description:  OS/2 implementation of open() and sopen().
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
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
#include "rtdata.h"
#include "rterrno.h"
#include "fileacc.h"
#include "openmode.h"
#include "iomode.h"
#include "seterrno.h"
#include "defwin.h"
#include "thread.h"
#include "os2fil64.h"


static int _set_binary( int handle )
{
    handle = handle;
    return( 0 );
}


static int __F_NAME(__sopen,__wsopen)( const CHAR_TYPE *name, unsigned mode, unsigned share, va_list args )
{
    OS_UINT     error, actiontaken;
    OS_UINT     fileattr, openflag, openmode;
    HFILE       handle;
    unsigned    rwmode;
    int         perm = S_IREAD | S_IWRITE;
    unsigned    iomode_flags;
#ifdef __WIDECHAR__
    char        mbName[MB_CUR_MAX * _MAX_PATH];     /* single-byte char */
#endif

    while( *name == STRING( ' ' ) )
        ++name;
#ifdef __WIDECHAR__
    /*** If necessary, convert the wide filename to multibyte form ***/
    if( wcstombs( mbName, name, sizeof( mbName ) ) == -1 ) {
        mbName[0] = '\0';
    }
#endif
    if( mode & O_CREAT ) {
        perm = va_arg( args, int );
        va_end( args );
        if( mode & O_EXCL ) {
            openflag = OPENFLAG_FAIL_IF_EXISTS | OPENFLAG_CREATE_IF_NOT_EXISTS;
        } else if( mode & O_TRUNC ) {
            openflag = OPENFLAG_REPLACE_IF_EXISTS | OPENFLAG_CREATE_IF_NOT_EXISTS;
        } else {
            openflag = OPENFLAG_OPEN_IF_EXISTS | OPENFLAG_CREATE_IF_NOT_EXISTS;
        }
    } else if( mode & O_TRUNC ) {
        openflag = OPENFLAG_REPLACE_IF_EXISTS;
    } else {
        openflag = OPENFLAG_OPEN_IF_EXISTS;
    }
    rwmode = mode & OPENMODE_ACCESS_MASK;
#ifdef _M_I86
    if( rwmode == OPENMODE_ACCESS_WRONLY && _RWD_osmode == DOS_MODE ) {
        /* Can't open WRONLY file in bound application under DOS */
        rwmode = OPENMODE_ACCESS_RDWR;
    }
#endif
    if( share == OPENMODE_DENY_COMPAT ) {
        share = OPENMODE_DENY_NONE;
    }
    openmode = share+rwmode;

    perm &= ~_RWD_umaskval;                 /* 05-jan-95 */
/*      if( ( perm & S_IREAD ) && !( perm & S_IWRITE ) )  */
    if( !(perm & S_IWRITE) ) {              /* 10-jan-95 */
        fileattr = _A_RDONLY;
    } else {
        fileattr = _A_NORMAL;
    }

#ifndef _M_I86
    if( __os2_DosOpenL != NULL ) {
        error = __os2_DosOpenL( (PSZ)__F_NAME(name,mbName), &handle,
                    &actiontaken, 0ULL, fileattr, openflag, openmode, 0ul );
    } else {
#endif
        error = DosOpen( (PSZ)__F_NAME(name,mbName), &handle,
                    &actiontaken, 0ul, fileattr, openflag, openmode, 0ul );
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

    if( iomode_flags & _BINARY )  _set_binary( handle );
    __SetIOMode( handle, iomode_flags );
#ifdef DEFAULT_WINDOWING
    if( _WindowsNewWindow != 0 ) {
        if( ( __F_NAME(stricmp,_wcsicmp)( name, STRING( "con" ) ) == 0 ) ||
            ( __F_NAME(stricmp,_wcsicmp)( name, STRING( "\\dev\\con" ) ) == 0 ) ) {
            _WindowsNewWindow( NULL, handle, -1 );
        }
    }
#endif
    return( handle );
}


_WCRTLINK int __F_NAME(open,_wopen)( const CHAR_TYPE *name, int mode, ... )
{
    int         permission;
    va_list     args;

    va_start( args, mode );
    permission = va_arg( args, int );
    va_end( args );
    return( __F_NAME(sopen,_wsopen)( name, mode, SH_COMPAT, permission ) );
}


_WCRTLINK int __F_NAME(sopen,_wsopen)( const CHAR_TYPE *name, int mode, int shflag, ... )
{
    va_list     args;

    va_start( args, shflag );
    return( __F_NAME(__sopen,__wsopen)( name, mode, shflag, args ) );
}
