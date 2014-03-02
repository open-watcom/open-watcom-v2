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
* Description:  Win32 implementation of directory functions.
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
#include <windows.h>
#include "strdup.h"
#include "libwin32.h"
#include "rtdata.h"
#include "ntex.h"
#include "seterrno.h"
#include "_direct.h"
#include "_dtaxxx.h"
#include "liballoc.h"

#define SEEK_ATTRIB (~_A_VOLID)

#ifdef __WIDECHAR__
    #define FIND_FIRST              __lib_FindFirstFileW
    #define FIND_NEXT               __lib_FindNextFileW
    #define CHECK_FIND_NEXT_ATTR    __wNTFindNextFileWithAttr
    #define GET_DIR_INFO            __wGetNTDirInfo
#else
    #define FIND_FIRST              FindFirstFileA
    #define FIND_NEXT               FindNextFileA
    #define CHECK_FIND_NEXT_ATTR    __NTFindNextFileWithAttr
    #define GET_DIR_INFO            __GetNTDirInfo
#endif


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
            if( prev_ch == '\\' || prev_ch == '/' || prev_ch == ':' || prev_ch == '.' ){
                /* directory, need add wildcard */
                return( 1 );
            }
            /* without wildcard maybe file or directory, need next check */
            return( 0 );
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
    /* with wildcard must be file */
    return( -1 );
}

static BOOL __find_close( DIR_TYPE *dirp )
/****************************************/
{
    if( DIR_HANDLE_OF( dirp ) )
        return( FindClose( DIR_HANDLE_OF( dirp ) ) );
    return( TRUE );
}


static DIR_TYPE *__F_NAME(___opendir,___wopendir)( const CHAR_TYPE *dirname,
                                               unsigned attr, DIR_TYPE *dirp )
/****************************************************************************/
{
    WIN32_FIND_DATA     ffb;
    HANDLE              h;

    __find_close( dirp );
    dirp->d_first = _DIR_CLOSED;
    h = FIND_FIRST( dirname, &ffb );
    if( h == (HANDLE)-1 ) {
        __set_errno_nt();
        return( NULL );
    }
    if( !CHECK_FIND_NEXT_ATTR( h, attr, &ffb ) ) {
        __set_errno_dos( ERROR_FILE_NOT_FOUND );
        return( NULL );
    }
    DIR_HANDLE_OF( dirp ) = h;
    DIR_ATTR_OF( dirp ) = attr;
    GET_DIR_INFO( dirp, &ffb );
    dirp->d_first = _DIR_ISFIRST;
    return( dirp );
}

static DIR_TYPE *__F_NAME(__opendir,__wopendir)( const CHAR_TYPE *dirname, unsigned attr )
/****************************************************************************************/
{

    DIR_TYPE        tmp;
    DIR_TYPE        *dirp;
    int             i;
    CHAR_TYPE       pathname[MAX_PATH+6];
    const CHAR_TYPE *p;
    UINT_WC_TYPE    curr_ch;
    UINT_WC_TYPE    prev_ch;

    DIR_HANDLE_OF( &tmp ) = 0;
    tmp.d_attr = _A_SUBDIR;               /* assume sub-directory */
    tmp.d_first = _DIR_CLOSED;
    dirp = NULL;
    i = is_directory( dirname );
    if( i <= 0 ) {
        if( (dirp = __F_NAME(___opendir,___wopendir)( dirname, attr, &tmp )) == NULL ) {
            return( NULL );
        }
    }
    if( i >= 0 && (tmp.d_attr & _A_SUBDIR) ) {
        /* directory, add wildcard */
        prev_ch = NULLCHAR;
        dirp = NULL;
        p = dirname;
        for( i = 0; i < MAX_PATH; i++ ) {
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
    WIN32_FIND_DATA     ffd;
    DWORD               err;

    if( dirp == NULL || dirp->d_first == _DIR_INVALID || dirp->d_first == _DIR_CLOSED )
        return( NULL );
    if( dirp->d_first == _DIR_ISFIRST ) {
        dirp->d_first = _DIR_NOTFIRST;
    } else {
        if( !FIND_NEXT( DIR_HANDLE_OF( dirp ), &ffd ) ) {
            err = GetLastError();
            if( err != ERROR_NO_MORE_FILES ) {
                __set_errno_dos( err );
            }
            return( NULL );
        }
        if( !CHECK_FIND_NEXT_ATTR( DIR_HANDLE_OF( dirp ), DIR_ATTR_OF( dirp ), &ffd ) ) {
            __set_errno_dos( ERROR_NO_MORE_FILES );
            return( NULL );
        }
        GET_DIR_INFO( dirp, &ffd );
    }
    return( dirp );

}

_WCRTLINK int __F_NAME(closedir,_wclosedir)( DIR_TYPE *dirp )
/***********************************************************/
{

    if( dirp == NULL || dirp->d_first == _DIR_CLOSED ) {
        return( __set_errno_dos( ERROR_INVALID_HANDLE ) );
    }
    if( __find_close( dirp ) == FALSE ) {
        return( __set_errno_nt() );
    }
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
