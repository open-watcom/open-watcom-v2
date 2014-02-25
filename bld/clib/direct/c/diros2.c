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
* Description:  OS/2 implementation of directory functions.
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <mbstring.h>
#include <sys/types.h>
#include <direct.h>
#include <dos.h>
#define INCL_ERRORS
#include <wos2.h>
#include "tinyio.h"
#include "rtdata.h"
#include "strdup.h"
#include "seterrno.h"
#include "msdos.h"
#include "_direct.h"
#include "_dtaxxx.h"
#include "liballoc.h"

#ifdef _M_I86
  #define FF_LEVEL      0
  #define FF_BUFFER     FILEFINDBUF
#else
  #define FF_LEVEL      FIL_STANDARD
  #define FF_BUFFER     FILEFINDBUF3
#endif

#define SEEK_ATTRIB (_A_SUBDIR | _A_HIDDEN | _A_SYSTEM | _A_RDONLY | _A_ARCH)

/* we'll use this to copy from a FILEFINDBUF to a DIR in copydir() */
struct name {
    char buf[ NAME_MAX + 1 ];
};

static void copydir( DIR_TYPE *dirp, FF_BUFFER *dir_buff )
/********************************************************/
{

#ifdef __WIDECHAR__
    wchar_t             wcs[_MAX_PATH];
#endif

    /*** Copy the structure data ***/
    dirp->d_attr = dir_buff->attrFile;
    dirp->d_time = *(unsigned short *)&dir_buff->ftimeLastWrite;
    dirp->d_date = *(unsigned short *)&dir_buff->fdateLastWrite;
    dirp->d_size = dir_buff->cbFile;
    *(struct name *)dirp->d_name = *(struct name *)dir_buff->achName;

#ifdef __WIDECHAR__
    mbstowcs( wcs, (char*)dirp->d_name, sizeof( wcs ) / sizeof( wcs[0] ) );    /* convert string */
    wcscpy( dirp->d_name, wcs );                        /* copy string */
#endif
}

static APIRET __find_close( DIR_TYPE *dirp )
/******************************************/
{
#ifdef _M_I86
    if( _RWD_osmode == OS2_MODE && DIR_HANDLE_OF( dirp ) )
#else
    if( DIR_HANDLE_OF( dirp ) )
#endif
        return( DosFindClose( DIR_HANDLE_OF( dirp ) ) );
    return( 0 );
}

static int is_directory( const CHAR_TYPE *name )
/**********************************************/
{
    UINT_WC_TYPE    curr_ch;
    UINT_WC_TYPE    prev_ch;

    curr_ch = NULLCHAR;
    for(;;) {
        prev_ch = curr_ch;
#ifdef __WIDECHAR__
        curr_ch = *name;
#else
        curr_ch = _mbsnextc( name );
#endif
        if( curr_ch == NULLCHAR ) {
            if( prev_ch == '\\' || prev_ch == '/' || prev_ch == '.' || prev_ch == ':' ){
                return( 1 );
            }
            break;
        }
        if( prev_ch == '*' )
            break;
        if( prev_ch == '?' )
            break;
#ifdef __WIDECHAR__
        ++name;
#else
        name = _mbsinc( name );
#endif
    }
    return( 0 );
}


static DIR_TYPE *__F_NAME(___opendir,___wopendir)( const CHAR_TYPE *dirname,
                                               unsigned attr, DIR_TYPE *dirp )
/****************************************************************************/
{

    /*** Convert a wide char string to a multibyte string ***/
#ifdef __WIDECHAR__
    char            mbcsName[ MB_CUR_MAX * _MAX_PATH ];

    if( wcstombs( mbcsName, dirname, sizeof( mbcsName ) ) == (size_t)-1 ) {
        return( NULL );
    }
#endif

#ifdef _M_I86
    if( _RWD_osmode == OS2_MODE )       /* protected mode */
#endif
    {
        FF_BUFFER       dir_buff;
        HDIR            handle;
        APIRET          rc;
        OS_UINT         searchcount;

        handle = HDIR_CREATE;           /* we want our own handle */
        searchcount = 1;                /* only one at a time */
        if( dirp->d_first != _DIR_CLOSED ) {
            __find_close( dirp );
            dirp->d_first = _DIR_CLOSED;
        }
        rc = DosFindFirst( (PSZ)__F_NAME(dirname,mbcsName), &handle, attr, (PVOID)&dir_buff,
                    sizeof( dir_buff ), &searchcount, FF_LEVEL );
        if( rc != 0 ) {
            __set_errno_dos( rc );
            return( NULL );
        }
        if( searchcount != 1 ) {
            DosFindClose( handle );
            __set_errno_dos( ERROR_NO_MORE_FILES );
            return( NULL );
        }
        DIR_HANDLE_OF( dirp ) = handle;     /* store our handle     */
        copydir( dirp, &dir_buff );     /* copy in other fields */
#ifdef _M_I86
    } else {                            /* real mode */
        tiny_ret_t      rc;

        TinySetDTA( dirp->d_dta );        /* set our DTA */
        rc = TinyFindFirst( __F_NAME(dirname,mbcsName), attr );
        if( TINY_ERROR( rc ) ) {
            __set_errno_dos( TINY_INFO( rc ) );
            return( NULL );
        }
#endif
    }
    dirp->d_first = _DIR_ISFIRST;       /* indicate we have 1st name */
    return( dirp );
}


static DIR_TYPE *__F_NAME(__opendir,__wopendir)( const CHAR_TYPE *dirname, unsigned attr )
/****************************************************************************************/
{
    DIR_TYPE        tmp;
    DIR_TYPE        *dirp;
    int             i;
    auto CHAR_TYPE  pathname[_MAX_PATH+6];
    const CHAR_TYPE *p;
    UINT_WC_TYPE    curr_ch;
    UINT_WC_TYPE    prev_ch;

    DIR_HANDLE_OF( &tmp ) = 0;                  /* initialize handle    */
    tmp.d_attr = _A_SUBDIR;
    tmp.d_first = _DIR_CLOSED;
    dirp = NULL;
    if( !is_directory( dirname ) ) {
        if( (dirp = __F_NAME(___opendir,___wopendir)( dirname, attr, &tmp )) == NULL ) {
            return( NULL );
        }
    }
    if( tmp.d_attr & _A_SUBDIR ) {
        prev_ch = NULLCHAR;
        dirp = NULL;
        p = dirname;
        for( i = 0; i < _MAX_PATH; i++ ) {
            pathname[i] = *p;
#ifdef __WIDECHAR__
            curr_ch = *p;
#else
            curr_ch = _mbsnextc( p );
            if( curr_ch > 256 ) {
                ++i;
                ++p;
                pathname[i] = *p;     /* copy second byte */
            }
#endif
            if( curr_ch == NULLCHAR ) {
                if( i != 0  &&  prev_ch != '\\' && prev_ch != '/' && prev_ch != ':' ){
                    pathname[i++] = '\\';
                }
                __F_NAME(strcpy,wcscpy)( &pathname[i], STRING( "*.*" ) );
                if( (dirp = __F_NAME(___opendir,___wopendir)( pathname, attr, &tmp )) == NULL ) {
                    return( NULL );
                }
                dirname = pathname;
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
    if( dirp == NULL ) {
        if( tmp.d_first != _DIR_CLOSED ) {
            __find_close( &tmp );
        }
        __set_errno_dos( ERROR_PATH_NOT_FOUND );
        return( NULL );
    }
    dirp = lib_malloc( sizeof( DIR_TYPE ) );
    if( dirp == NULL ) {
        __find_close( &tmp );
        __set_errno_dos( ERROR_NOT_ENOUGH_MEMORY );
        return( NULL );
    }
    tmp.d_openpath = __F_NAME(__clib_strdup,__clib_wcsdup)( dirname );
    *dirp = tmp;
    return( dirp );
}


_WCRTLINK DIR_TYPE *__F_NAME(opendir,_wopendir)( const CHAR_TYPE *dirname )
/*************************************************************************/
{
    return( __F_NAME(__opendir,__wopendir)( dirname, SEEK_ATTRIB ) );
}


_WCRTLINK DIR_TYPE *__F_NAME(readdir,_wreaddir)( DIR_TYPE *dirp )
/***************************************************************/
{
    if( dirp == NULL || dirp->d_first == _DIR_INVALID || dirp->d_first == _DIR_CLOSED )
        return( NULL );
    if( dirp->d_first == _DIR_ISFIRST ) {       /* if we already have one */
        dirp->d_first = _DIR_NOTFIRST;
    } else {
#ifdef _M_I86
        if( _RWD_osmode == OS2_MODE )           /* protected mode */
#endif
        {
            APIRET          rc;
            FF_BUFFER       dir_buff;
            OS_UINT         searchcount = 1;

            rc = DosFindNext( DIR_HANDLE_OF( dirp ), (PVOID)&dir_buff,
                                sizeof( dir_buff ), &searchcount );
            if( rc != 0 ) {
                if( rc != ERROR_NO_MORE_FILES ) {
                    __set_errno_dos( rc );
                }
                dirp = NULL;
            } else if( searchcount != 1 ) {
                __set_errno_dos( ERROR_NO_MORE_FILES );
                dirp = NULL;
            } else {
                copydir( dirp, &dir_buff );
            }
#ifdef _M_I86
        } else {                                /* real mode */
            tiny_ret_t      rc;

            TinySetDTA( dirp->d_dta );
            rc = TinyFindNext();
            if( TINY_ERROR( rc ) ) {
                if( TINY_INFO( rc ) != E_nomore ) {
                    __set_errno_dos( TINY_INFO( rc ) );
                }
                dirp = NULL;
            }
#endif
        }
    }
    return( dirp );
}


_WCRTLINK int __F_NAME(closedir,_wclosedir)( DIR_TYPE *dirp )
/***********************************************************/
{
    APIRET      rc;

    if( dirp == NULL || dirp->d_first == _DIR_CLOSED  ) {
        return( __set_errno_dos( ERROR_INVALID_HANDLE ) );
    }
    rc = __find_close( dirp );
    if( rc != 0 )
        return( __set_errno_dos( rc ) );
    dirp->d_first = _DIR_CLOSED;
    if( dirp->d_openpath != NULL )
        free( dirp->d_openpath );
    lib_free( dirp );
    return( 0 );
}


_WCRTLINK void __F_NAME(rewinddir,_wrewinddir)( DIR_TYPE *dirp )
/**************************************************************/
{
    if( dirp == NULL || dirp->d_openpath == NULL )
        return;
    if( __F_NAME(___opendir,___wopendir)( dirp->d_openpath, SEEK_ATTRIB, dirp ) == NULL ) {
        dirp->d_first = _DIR_INVALID;    /* so reads won't work any more */
    }
}

