/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Platform independent fopen() implementation.
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include "seterrno.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#ifdef __WIDECHAR__
    #include <wctype.h>
#endif
#include <fcntl.h>
#include <sys/stat.h>
#if defined( __NT__ )
    #include <windows.h>
#elif defined( __OS2__ )
    #include <wos2.h>
#elif defined( __NETWARE__ )
    #include "nw_lib.h"
#endif
#include "rtdata.h"
#include "fileacc.h"
#include "commode.h"
#include "openmode.h"
#include "defwin.h"
#include "streamio.h"
#include "thread.h"
#include "openflag.h"


#ifdef __UNIX__
    #define PMODE   (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)
#else
    #define PMODE   (S_IREAD | S_IWRITE)
#endif

unsigned _WCNEAR __F_NAME(__open_flags,__wopen_flags)( const CHAR_TYPE *modestr, int *extflags )
{
    unsigned            flags;
    bool                alive;
    bool                gotplus;
    bool                gottextbin;
#ifndef __NETWARE__
    bool                gotcommit;
#endif

    flags = 0;
    if( extflags != NULL ) {
#ifdef __NETWARE__
        *extflags = 0;
#else
        if( _RWD_commode == _COMMIT ) {
            *extflags = _COMMIT;
        } else {
            *extflags = 0;
        }
#endif
    }

    /*
     * The first character in modestr must be 'r', 'w', or 'a'.
     */
    switch( *modestr ) {
    case STRING( 'r' ):
        flags |= _READ;
        break;
    case STRING( 'w' ):
        flags |= _WRITE;
        break;
    case STRING( 'a' ):
        flags |= _WRITE | _APPEND;
        break;
    default:
        lib_set_errno( EINVAL );
        return( 0 );
    }
    modestr++;

    /*
     * Next we might have, in any order, some additional mode modifier
     * characters:
     *      1.  A '+' character.
     *      2.  Either a 't' or a 'b'.
     *      3.  Either a 'c' or a 'n'.  (Not available for Netware.)
     * For MS compatability, scanning stops when any of the three groups
     * is encountered twice; e.g., "wct+b$&!" is valid and will result in
     * a text, not binary, stream.  Also for MS compatability, scanning
     * stops at any unrecognized character, without causing failure.
     */
    alive = true;
    gottextbin = false;
    gotplus = false;
#ifndef __NETWARE__
    gotcommit = false;
#endif
    while( (*modestr != NULLCHAR) && alive ) {
        switch( *modestr ) {
        case STRING( '+' ):
            if( gotplus ) {
                alive = false;
            } else {
                flags |= _READ | _WRITE;
                gotplus = true;
            }
            break;
        case STRING( 't' ):
            if( gottextbin ) {
                alive = false;
            } else {
                gottextbin = true;
            }
            break;
        case STRING( 'b' ):
            if( gottextbin ) {
                alive = false;
            } else {
#ifndef __UNIX__
                flags |= _BINARY;
#endif
                gottextbin = true;
            }
            break;
#ifndef __NETWARE__
        case STRING( 'c' ):
            if( gotcommit ) {
                alive = false;
            } else {
                *extflags |= _COMMIT;
                gotcommit = true;
            }
            break;
        case STRING( 'n' ):
            if( gotcommit ) {
                alive = false;
            } else {
                *extflags &= ~_COMMIT;
                gotcommit = true;
            }
            break;
#endif
        default:
            break;
        }
        modestr++;
    }

    /*
     * Handle defaults for any unspecified options.
     */
#ifndef __UNIX__
    if( !gottextbin ) {
        if( _RWD_fmode == O_BINARY ) {
            flags |= _BINARY;
        }
    }
#endif

    return( flags );
}


static FILE * _WCNEAR __F_NAME(__doopen,__wdoopen)( const CHAR_TYPE *name,
                       CHAR_TYPE    mode,
                       unsigned     file_flags,
                       int          extflags,
                       int          shflag,     /* sharing flag */
                       FILE *       fp )
{
    int open_mode;
    int p_mode;

#ifdef __NETWARE__

    /* unused parameters */ (void)extflags;

#endif

    SetupTGCSandNCS( RETURN_ARG( FILE *, 0 ) );     /* for NW386 */

    /* we need the mode character to indicate if the original */
    /* intention is to open for read or for write */
    mode = __F_NAME(tolower,towlower)( (UCHAR_TYPE)mode );
    if( mode == STRING( 'r' ) ) {
        open_mode = O_RDONLY;
        if( file_flags & _WRITE ) {         /* if "r+" mode */
            open_mode = O_RDWR;
        }
#if defined( __NETWARE__ )
        open_mode |= O_BINARY;
#elif defined( __UNIX__ )
#else
        if( file_flags & _BINARY ) {
            open_mode |= O_BINARY;
        } else {
            open_mode |= O_TEXT;
        }
#endif
        p_mode = 0;
    } else {        /* mode == 'w' || mode == 'a' */
        if( file_flags & _READ ) {          /* if "a+" or "w+" mode */
            open_mode = O_RDWR | O_CREAT;
        } else {
            open_mode = O_WRONLY | O_CREAT;
        }
        if( file_flags & _APPEND ) {
            open_mode |= O_APPEND;
        } else {                    /* mode == 'w' */
            open_mode |= O_TRUNC;
        }
#if defined( __NETWARE__ )
        open_mode |= O_BINARY;
#elif defined( __UNIX__ )
#else
        if( file_flags & _BINARY ) {
            open_mode |= O_BINARY;
        } else {
            open_mode |= O_TEXT;
        }
#endif
        p_mode = PMODE;
    }
    fp->_handle = __F_NAME(_sopen,_wsopen)( name, open_mode, shflag, p_mode );
    if( fp->_handle == -1 ) {
        // since we couldn't open the file, release the FILE struct
        __freefp( fp );
        return( NULL );
    }
    fp->_flag |= file_flags;
    fp->_cnt = 0;
    fp->_bufsize = 0;                       /* was BUFSIZ */
#ifndef __NETWARE__
    _FP_ORIENTATION( fp ) = _NOT_ORIENTED;  /* initial orientation */
    _FP_EXTFLAGS( fp ) = extflags;
#endif
#if defined( __NT__ ) || defined( __OS2__ ) || defined( __UNIX__ )
    _FP_PIPEDATA( fp ).isPipe = 0;          /* not a pipe */
#endif
    _FP_BASE( fp ) = NULL;
    if( file_flags & _APPEND ) {
        fseek( fp, 0L, SEEK_END );
    }
    __chktty( fp );
    return( fp );
}


_WCRTLINK FILE *__F_NAME(_fsopen,_wfsopen)( const CHAR_TYPE *name,
                                const CHAR_TYPE *access_mode, int shflag )
{
    FILE *          fp;
    unsigned        file_flags;
    int             extflags;

    /* validate access_mode */
    file_flags = __F_NAME(__open_flags,__wopen_flags)( access_mode, &extflags );
    if( file_flags == 0 ) {
        return( NULL );
    }

    fp = __allocfp();
    if( fp != NULL ) {
        fp = __F_NAME(__doopen,__wdoopen)( name, *access_mode, file_flags, extflags, shflag, fp );
    }
    return( fp );
}


_WCRTLINK FILE *__F_NAME(fopen,_wfopen)( const CHAR_TYPE *name, const CHAR_TYPE *access_mode )
{
    return( __F_NAME(_fsopen,_wfsopen)( name, access_mode, OPENMODE_DENY_COMPAT ) );
}

static FILE * _WCNEAR close_file( FILE *fp )
{
    __stream_link * link;
    __stream_link **owner;

    _AccessIOB();
    /* See if the file pointer is a currently open file. */
    for( link = _RWD_ostream; link != NULL; link = link->next ) {
        if( link->stream == fp ) {
            __doclose( fp, 1 );
            _ReleaseIOB();
            return( fp );
        }
    }
    /*
       It's not on the list of open files, so check the list of
       recently closed ones.
    */
    for( owner = &_RWD_cstream; (link = *owner) != NULL; owner = &link->next ) {
        if( link->stream == fp ) {
            /* remove from closed list and put on open list */
            *owner = link->next;
            link->next = _RWD_ostream;
            _RWD_ostream = link;
            _ReleaseIOB();
            return( fp );
        }
    }
    /* We ain't seen that file pointer ever. Leave things be. */
    lib_set_errno( EBADF );
    _ReleaseIOB();
    return( NULL );
}


_WCRTLINK FILE *__F_NAME(freopen,_wfreopen)( const CHAR_TYPE *name,
                                const CHAR_TYPE *access_mode, FILE *fp )
{
    int             hdl;
    unsigned        file_flags;
    int             extflags;

    _ValidFile( fp, 0 );

    /* validate access_mode */
    file_flags = __F_NAME(__open_flags,__wopen_flags)( access_mode, &extflags );
    if( file_flags == 0 ) {
        return( NULL );
    }

    hdl = fileno( fp );
    _AccessFileH( hdl );

#ifdef DEFAULT_WINDOWING
    if( _WindowsRemoveWindowedHandle != NULL ) {
        _WindowsRemoveWindowedHandle( hdl );
    }
#endif
    fp = close_file( fp );
    if( fp != NULL ) {
        fp = __F_NAME(__doopen,__wdoopen)( name, *access_mode, file_flags, extflags, 0, fp );
    }
    _ReleaseFileH( hdl );
    return( fp );
}
