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
* Description:  DOS implementation of directory functions.
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include <stdlib.h>
#include <string.h>
#include <mbstring.h>
#include <direct.h>
#include <dos.h>
#include "strdup.h"
#include "liballoc.h"
#include "tinyio.h"
#include "seterrno.h"
#include "msdos.h"
#include "_direct.h"
#include "rtdata.h"

#define SEEK_ATTRIB (TIO_HIDDEN | TIO_SYSTEM | TIO_SUBDIRECTORY)


static int is_directory( const CHAR_TYPE *name )
/**********************************************/
{
    UINT_WC_TYPE    curr_ch;
    UINT_WC_TYPE    prev_ch;

    curr_ch = NULLCHAR;
    for(;;) {
        prev_ch = curr_ch;
#if defined( __WIDECHAR__ ) || defined( __UNIX__ )
        curr_ch = *name;
#else
        curr_ch = _mbsnextc( name );
#endif
        if( curr_ch == NULLCHAR )
            break;
        if( prev_ch == '*' )
            break;
        if( prev_ch == '?' )
            break;
#if defined( __WIDECHAR__ ) || defined( __UNIX__ )
        ++name;
#else
        name = _mbsinc( name );
#endif
    }
    if( curr_ch == NULLCHAR ) {
        if( prev_ch == '\\' || prev_ch == '/' || prev_ch == '.' ) {
            return( 1 );
        }
    }
    return( 0 );
}


#ifdef __WIDECHAR__
static void filenameToWide( DIR_TYPE *dir )
/*****************************************/
{
    wchar_t             wcs[ _MAX_PATH ];

    /* convert string */
    mbstowcs( wcs, (char*)dir->d_name, sizeof( wcs ) / sizeof( wcs[0] ) );
    /* copy string */
    wcscpy( dir->d_name, wcs );
}
#endif


DIR_TYPE *__F_NAME(__opendir,__wopendir)( const CHAR_TYPE *dirname,
                                            unsigned attr, DIR_TYPE *dirp )
/**************************************************************************/
{
    DIR_TYPE        tmp;
    int             i;
    CHAR_TYPE       pathname[ _MAX_PATH + 6 ];
    const CHAR_TYPE *p;
    UINT_WC_TYPE    curr_ch;
    UINT_WC_TYPE    prev_ch;
    int             flag_opendir = ( dirp == NULL );
    int             opened;

    /*** Convert a wide char string to a multibyte string ***/
#ifdef __WIDECHAR__
    char            mbcsName[ MB_CUR_MAX * _MAX_PATH ];

    if( wcstombs( mbcsName, dirname, sizeof( mbcsName ) ) == (size_t)-1 ) {
        return( NULL );
    }
#endif

    tmp.d_attr = _A_SUBDIR;
    opened = 0;
    if( !is_directory( dirname ) ) {
        if( _dos_findfirst( __F_NAME(dirname,mbcsName), attr, (struct _find_t *)tmp.d_dta ) ) {
            return( NULL );
        }
        opened = 1;
    }
    if( tmp.d_attr & _A_SUBDIR ) {
        prev_ch = NULLCHAR;
        p = dirname;
        for( i = 0; i < _MAX_PATH; i++ ) {
            pathname[i] = *p;
#if defined( __WIDECHAR__ ) || defined( __UNIX__ )
            curr_ch = *p;
#else
            curr_ch = _mbsnextc( p );
            if( curr_ch > 256 ) {
                ++i;
                ++p;
                pathname[i] = *p;       /* copy second byte */
            }
#endif
            if( curr_ch == NULLCHAR ) {
                if( i != 0  &&  prev_ch != '\\' && prev_ch != '/' ) {
                    pathname[i++] = '\\';
                }
                __F_NAME(strcpy,wcscpy)( &pathname[i], STRING( "*.*" ) );
#ifdef __WIDECHAR__
                if( wcstombs( mbcsName, pathname, sizeof( mbcsName ) ) == (size_t)-1 )
                    return( NULL );
#endif
                if( opened ) {
                    _dos_findclose( (struct _find_t *)tmp.d_dta );
                }
                if( _dos_findfirst( __F_NAME(pathname,mbcsName), attr, (struct _find_t *)tmp.d_dta ) ) {
                    return( NULL );
                }
                opened = 1;
                break;
            }
            if( curr_ch == '*' )
                break;
            if( curr_ch == '?' )
                break;
            ++p;
            prev_ch = curr_ch;
        }
    }
    if( flag_opendir ) {
        dirp = lib_malloc( sizeof( *dirp ) );
        if( dirp == NULL ) {
            if( opened ) {
                _dos_findclose( (struct _find_t *)tmp.d_dta );
            }
            __set_errno_dos( E_nomem );
            return( NULL );
        }
        tmp.d_openpath = __F_NAME(__clib_strdup,__clib_wcsdup)( dirname );
    } else {
        _dos_findclose( (struct _find_t *)dirp->d_dta );
        tmp.d_openpath = dirp->d_openpath;
    }
    tmp.d_first = _DIR_ISFIRST;
    *dirp = tmp;
    return( dirp );
}


_WCRTLINK DIR_TYPE *__F_NAME(opendir,_wopendir)( const CHAR_TYPE *dirname )
{
    return( __F_NAME(__opendir,__wopendir)( dirname, SEEK_ATTRIB, NULL ) );
}


_WCRTLINK DIR_TYPE *__F_NAME(readdir,_wreaddir)( DIR_TYPE *dirp )
{
    if( dirp == NULL || dirp->d_first >= _DIR_INVALID )
        return( NULL );
    if( dirp->d_first == _DIR_ISFIRST ) {
        dirp->d_first = _DIR_NOTFIRST;
    } else {
        if( _dos_findnext( (struct _find_t *)dirp->d_dta ) ) {
            return( NULL );
        }
    }
#ifdef __WIDECHAR__
    filenameToWide( dirp );
#endif
    return( dirp );
}


_WCRTLINK int __F_NAME(closedir,_wclosedir)( DIR_TYPE *dirp )
{
    unsigned    rc;

    if( dirp == NULL || dirp->d_first == _DIR_CLOSED ) {
        return( __set_errno_dos( E_ihandle ) );
    }
    rc = _dos_findclose( (struct _find_t *)dirp->d_dta );
    if( rc ) {
        return( rc );
    }
    dirp->d_first = _DIR_CLOSED;
    if( dirp->d_openpath != NULL )
        free( dirp->d_openpath );
    lib_free( dirp );
    return( 0 );
}


_WCRTLINK void __F_NAME(rewinddir,_wrewinddir)( DIR_TYPE *dirp )
{
    if( dirp == NULL || dirp->d_openpath == NULL )
        return;
    if( __F_NAME(__opendir,__wopendir)( dirp->d_openpath, SEEK_ATTRIB, dirp ) == NULL ) {
        dirp->d_first = _DIR_INVALID;    /* so reads won't work any more */
    }
}
