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
* Description:  DOS implementation of open() and sopen().
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <share.h>
#include <dos.h>
#include <dosfunc.h>
#include "rtdata.h"
#include "tinyio.h"
#include "iomode.h"
#include "seterrno.h"
#include "defwin.h"
#include "extender.h"
#include "msdos.h"

/* file attributes */

#define _A_RDONLY       0x01

extern unsigned __NFiles;

static int __F_NAME(__sopen,__wsopen)( const CHAR_TYPE *name, int mode,
                                       int shflag, va_list args )
{
    int         rwmode;
    int         handle;
    int         attr;
    int         permission;
    unsigned    iomode_flags;
    tiny_ret_t  rc;
    char        dummy;
#ifdef __WIDECHAR__
    char        mbName[MB_CUR_MAX * _MAX_PATH];     /* single-byte char */
#endif

    handle = -1;
    rc = 0;
    while( *name == STRING( ' ' ) )
        ++name;
#ifdef __WIDECHAR__
    /*** If necessary, convert the wide filename to multibyte form ***/
    if( wcstombs( mbName, name, sizeof( mbName ) ) == -1 ) {
        mbName[0] = '\0';
    }
#endif
    rwmode = mode & ( O_RDONLY | O_WRONLY | O_RDWR | O_NOINHERIT );
    if( _dos_open( __F_NAME(name,mbName), rwmode | shflag, &handle ) == 0 ) {
        if( handle >= __NFiles ) {
            TinyClose( handle );
            __set_errno( EMFILE );
            return( -1 );
        }
    }
                                        /* 17-apr-90   05-sep-91 */
    if( (mode & (O_RDONLY | O_WRONLY | O_RDWR)) != O_RDONLY ) {
        if( handle != -1 ) {
            if( ! isatty( handle ) ) {      /* if not a device */
#if 0
                rc = TinyAccess( name, 0 ); /* check for existence */
                if( TINY_ERROR( rc ) ) {    /* file does not exist */
                    TinyClose( handle );    /* close whatever file we got */
                    handle = -1;
                } else if( mode & O_EXCL ) {    /* must not exist */
#else
    /*
    Don't need to do the access check, since the file was opened
    and therefore must exist (TinyOpen can't create a file).
    We don't want to do the check because there are classes of items
    in the file system namespace that are not devices, but the TinyAccess
    will fail on (e.g. named pipes).
    */
                /* must not exist if O_CREAT specified */
                if( (mode & O_EXCL) && (mode & O_CREAT) ) {
#endif
                    TinyClose( handle );
                    __set_errno( EEXIST );
                    return( -1 );
                } else if( mode & O_TRUNC ) {   /* truncate file */
                    rc = TinyWrite( handle, &dummy, 0 );
                    if( TINY_ERROR( rc ) ) {
                        TinyClose( handle );
                        return( __set_errno_dos( TINY_INFO( rc ) ) );
                    }
                }
            }
        }
    }
    if( handle == -1 ) {                    /* could not open */
        if( (mode & O_CREAT) == 0 || _RWD_doserrno != E_nofile ) {
            return( -1 );
        }
        /* creating the file */
        permission = va_arg( args, int );
        va_end( args );
        if( permission == 0 )
            permission = S_IWRITE | S_IREAD;
        permission &= ~_RWD_umaskval;               /* 05-jan-95 */
        attr = 0;
        if(( permission & S_IWRITE) == 0 )
            attr = _A_RDONLY;
        #if 0
            /* remove this support because it is not consistently available */
            if( _RWD_osmajor >= 5
                #ifdef __DOS_EXT__
                    && !_IsFlashTek()
                    && !_IsRational()
                #endif
                ) {
                /* this function is only available in version DOS 5 and up */
                /* this new way was added to handle the case of creating a */
                /* new file with read-only access, but with a writeable */
                /* file handle */
                #ifdef __WIDECHAR__
                    rc = TinyCreateEx( mbName, rwmode|shflag, attr, TIO_OPEN );
                #else
                    rc = TinyCreateEx( name, rwmode|shflag, attr, TIO_OPEN );
                #endif
                if( TINY_ERROR( rc ) ) {
                    return( __set_errno_dos( TINY_INFO( rc ) ) );
                }
                handle = TINY_INFO( rc );
            } else
        #endif
        {
            /* do it the old way */
            if( _dos_creat( __F_NAME(name,mbName), attr, &handle ) ) {
                return( -1 );
            }
            if( handle >= __NFiles ) {
                TinyClose( handle );
                __set_errno( EMFILE );
                return( -1 );
            }

            /* 21-nov-90 AFS: the file is created so now the file must be */
            /*                    opened with the correct share permissions */
            if( shflag != 0 ) {
                rc = TinyClose( handle );
                if( TINY_ERROR( rc ) ) {
                    return( __set_errno_dos( TINY_INFO( rc ) ) );
                }
                if( _dos_open( __F_NAME(name,mbName), rwmode | shflag, &handle ) ) {
                    return( -1 );
                }
                /* handle does not equal -1 now */
            }
        }
    }
    iomode_flags = __GetIOMode( handle );
    iomode_flags &= ~(_READ|_WRITE|_APPEND|_BINARY);     /* 11-aug-88 */
    if( isatty( handle ) )              iomode_flags |= _ISTTY;
    rwmode &= ~O_NOINHERIT;
    if( rwmode == O_RDWR )              iomode_flags |= _READ | _WRITE;
    if( rwmode == O_RDONLY)             iomode_flags |= _READ;
    if( rwmode == O_WRONLY)             iomode_flags |= _WRITE;
    if( mode & O_APPEND )               iomode_flags |= _APPEND;
    if( mode & (O_BINARY|O_TEXT) ) {
        if( mode & O_BINARY )           iomode_flags |= _BINARY;
    } else {
        if( _RWD_fmode == O_BINARY )    iomode_flags |= _BINARY;
    }
    __SetIOMode( handle, iomode_flags );
#ifdef DEFAULT_WINDOWING
    if( _WindowsNewWindow != 0 ) {
        if( !__F_NAME(stricmp,wcscmp)( name, STRING( "con" ) ) ) {
            _WindowsNewWindow( NULL, handle, -1 );
        }
    }
#endif
    return( handle );
}


#if 0 /* couldn't find any user; please re-enable if it's necessary */
#ifndef __WIDECHAR__                    /* compile one version only */
int __set_binary( int handle )
{
    unsigned        iomode_flags;

    __ChkTTYIOMode( handle );
    iomode_flags = __GetIOMode( handle );
    iomode_flags |= _BINARY;
    __SetIOMode( handle, iomode_flags );
    if( iomode_flags & _ISTTY ) {
        tiny_ret_t rc;

        rc = TinyGetDeviceInfo( handle );
        if( TINY_ERROR( rc ) ) {
            return( __set_errno_dos( TINY_INFO( rc ) ) );
        }
        rc = TinySetDeviceInfo( handle, TINY_INFO(rc) | TIO_CTL_RAW );
        if( TINY_ERROR( rc ) ) {
            return( __set_errno_dos( TINY_INFO( rc ) ) );
        }
    }
    return( 0 );
}
#endif
#endif


_WCRTLINK int __F_NAME(open,_wopen)( const CHAR_TYPE *name, int mode, ... )
{
    int         permission;
    va_list     args;

    va_start( args, mode );
    permission = va_arg( args, int );
    va_end( args );
    return( __F_NAME(sopen,_wsopen)( name, mode, 0, permission ) );
}


_WCRTLINK int __F_NAME(sopen,_wsopen)( const CHAR_TYPE *name, int mode, int shflag, ... )
{
    va_list     args;

    va_start( args, shflag );
    return( __F_NAME(__sopen,__wsopen)( name, mode, shflag, args ) );
}
