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


#include "variety.h"
#include "widechar.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <share.h>
#include "dos.h"
#include "dosfunc.h"
#include <direct.h>
#include "fileacc.h"
#include <wos2.h>
#include <string.h>
#include "openmode.h"
#include "iomode.h"
#include "rtdata.h"
#include "seterrno.h"
#include "defwin.h"
#ifdef __WIDECHAR__
    #include <mbstring.h>
    #include "mbwcconv.h"
#endif

extern unsigned __NFiles;


static int _set_binary( int handle )
{
    handle = handle;
    return( 0 );
}


_WCRTLINK int __F_NAME(open,_wopen)( const CHAR_TYPE *name, int mode, ... )
{
    int permission;
    va_list args;

    va_start( args, mode );
    permission = va_arg( args, int );
    va_end( args );
    return( __F_NAME(sopen,_wsopen)( name, mode, SH_COMPAT, permission ) );
}


_WCRTLINK int __F_NAME(sopen,_wsopen)( const CHAR_TYPE *name, int mode, int shflag, ... )
{
    va_list             args;

    va_start( args, shflag );
    return( __F_NAME(_sopen,__wsopen)( name, mode, shflag, args ) );
}


static int __F_NAME(_sopen,__wsopen)( const CHAR_TYPE *name, int mode, int share, va_list args )
{
    OS_UINT     rwmode, error, actiontaken, fileattr, openflag, openmode;
    HFILE       handle;
    int         perm = S_IREAD | S_IWRITE;
    unsigned    iomode_flags;
#ifdef __WIDECHAR__
    char        mbName[MB_CUR_MAX*_MAX_PATH];   /* single-byte char */
#endif

    while( *name == ' ' ) ++name;

    /*** If necessary, convert the wide filename to multibyte form ***/
    #ifdef __WIDECHAR__
        __filename_from_wide( mbName, name );
    #endif
                                                    /* 05-sep-91 */
    if( mode & O_CREAT ) {
        perm = va_arg( args, int );
        va_end( args );
        if( mode & O_EXCL ) {
            openflag = OPENFLAG_FAIL_IF_EXISTS |
                       OPENFLAG_CREATE_IF_NOT_EXISTS;
        } else if( mode & O_TRUNC ) {
            openflag = OPENFLAG_REPLACE_IF_EXISTS |
                       OPENFLAG_CREATE_IF_NOT_EXISTS;
        } else {
            openflag = OPENFLAG_OPEN_IF_EXISTS |
                       OPENFLAG_CREATE_IF_NOT_EXISTS;
        }
    } else if( mode & O_TRUNC ) {
        openflag = OPENFLAG_REPLACE_IF_EXISTS;
    } else {
        openflag = OPENFLAG_OPEN_IF_EXISTS;
    }
    rwmode = mode & OPENMODE_ACCESS_MASK;
#if defined(M_I86)
    if( rwmode == OPENMODE_ACCESS_WRONLY && !_RWD_osmode ) {
        /* Can't open WRONLY file in bound application under DOS */
        rwmode = OPENMODE_ACCESS_RDWR;
    }
#endif
    if( share == SH_COMPAT ) {
        share = SH_DENYNO;
    }
    openmode = share+rwmode;

    perm &= ~_RWD_umaskval;                 /* 05-jan-95 */
/*      if( ( perm & S_IREAD ) && !( perm & S_IWRITE ) )  */
    if( !(perm & S_IWRITE) ) {              /* 10-jan-95 */
        fileattr = _A_RDONLY;
    } else {
        fileattr = _A_NORMAL;
    }

    #ifdef __WIDECHAR__
        error = DosOpen( (PSZ)mbName, &handle, &actiontaken, 0ul,
                            fileattr, openflag, openmode, 0ul );
    #else
        error = DosOpen( (PSZ)name, &handle, &actiontaken, 0ul,
                            fileattr, openflag, openmode, 0ul );
    #endif
    if( error ) {
        return( __set_errno_dos( error ) );
    }

    if (handle >= __NFiles)
    {
            DosClose( handle );
            __set_errno(ENOMEM);
            return -1;
    }

    if( rwmode == O_RDWR )  iomode_flags = _READ | _WRITE;
    if( rwmode == O_RDONLY) iomode_flags = _READ;
    if( rwmode == O_WRONLY) iomode_flags = _WRITE;
    if( mode & O_APPEND )   iomode_flags |= _APPEND;
    if( mode & (O_BINARY|O_TEXT) ) {
        if( mode & O_BINARY )         iomode_flags |= _BINARY;
    } else {
        if( _RWD_fmode == O_BINARY )  iomode_flags |= _BINARY;
    }
    if( iomode_flags & _BINARY )  _set_binary( handle );
    __SetIOMode( handle, iomode_flags );
    #ifdef DEFAULT_WINDOWING
        if( _WindowsNewWindow != 0 ) {
            #ifdef __WIDECHAR__
                if( ( _wcsicmp( name, L"con" ) == 0 ) ||
                    ( _wcsicmp( name, L"\\dev\\con" ) == 0 ) ) {
                    _WindowsNewWindow( NULL, handle, -1 );
                }
            #else
                if( ( stricmp( name, "con" ) == 0 ) ||
                    ( stricmp( name, "\\dev\\con" ) == 0 ) ) {
                    _WindowsNewWindow( NULL, handle, -1 );
                }
            #endif
        }
    #endif
    return( handle );
}
