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
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <mbstring.h>
#include <sys\types.h>
#include <direct.h>
#include <dos.h>
#include <windows.h>
#include "strdup.h"
#include "libwin32.h"
#include "rtdata.h"
#include "ntex.h"
#include "seterrno.h"


#define SEEK_ATTRIB (~_A_VOLID)

#ifdef __WIDECHAR__
    #define FIND_NEXT           _w__NTFindNextFileWithAttr
    #define GET_DIR_INFO        _w__GetNTDirInfo
#else
    #define FIND_NEXT           __NTFindNextFileWithAttr
    #define GET_DIR_INFO        __GetNTDirInfo
#endif

#define _DIR_ISFIRST            0
#define _DIR_NOTFIRST           1
#define _DIR_MAX_FOR_CLOSE_OK   2       /* dummy value used by closedir */
#define _DIR_CLOSED             3


static int is_directory( const CHAR_TYPE *name )
/**********************************************/
{
    /* 28-oct-98 */
    #ifndef __WIDECHAR__
        unsigned    curr_ch;
        unsigned    prev_ch;
    #else
        CHAR_TYPE   curr_ch;
        CHAR_TYPE   prev_ch;
    #endif

    curr_ch = NULLCHAR;
    for(;;) {
        prev_ch = curr_ch;
        #ifdef __WIDECHAR__
            curr_ch = *name;
        #else
            #ifdef __QNX__
                curr_ch = *name;
            #else
                curr_ch = _mbsnextc( name );
            #endif
        #endif
        if( curr_ch == NULLCHAR ) break;
        if( prev_ch == '*' ) break;
        if( prev_ch == '?' ) break;
        #ifdef __WIDECHAR__
            ++name;
        #else
            #ifdef __QNX__
                name++;
            #else
                name = _mbsinc( name );
            #endif
        #endif
    }
    if( curr_ch == NULLCHAR ) {
        if( prev_ch == '\\' || prev_ch == '/' || prev_ch == '.' ){
            return( 1 );
        }
    }
    return( 0 );

/*  old logic
    if( name[0] == '\0' )return( 0 );
    while( name[1] != '\0' ){
        if( name[0] == '*' || name[0] == '?' ) {
            return( 0 );
        }
        ++name;
    }
    if( name[0] == '\\' || name[0] == '/' || name[0] == '.' ){
        return( 1 );
    }
    return( 0 );
*/
}

_WCRTLINK DIR_TYPE *__F_NAME(__opendir,_w__opendir)( const CHAR_TYPE *dirname,
                                                    int attr, DIR_TYPE *dirp )
/****************************************************************************/
{
    WIN32_FIND_DATA     ffb;
    HANDLE              h;

    h = __F_NAME(FindFirstFileA,__lib_FindFirstFileW)( dirname, &ffb );

    if( h == (HANDLE)-1 ) {
        __set_errno_nt();
        return( NULL );
    }
    if( !FIND_NEXT( h, attr, &ffb ) ) {
        __set_errno_dos( ERROR_FILE_NOT_FOUND );
        return( NULL );
    }
    HANDLE_OF( dirp ) = h;
    ATTR_OF( dirp ) = attr;
    GET_DIR_INFO( dirp, &ffb );
    dirp->d_first = _DIR_ISFIRST;

    return( dirp );
}

_WCRTLINK DIR_TYPE *__F_NAME(_opendir,__wopendir)( const CHAR_TYPE *dirname,
                                                    int attr )
/**************************************************************************/
{

    DIR_TYPE    *dirp;
    int         i;
    CHAR_TYPE   pathname[MAX_PATH+6];
    const CHAR_TYPE *dirnameStart = dirname;
    #ifndef __WIDECHAR__
        unsigned    curr_ch;
        unsigned    prev_ch;
    #else
        CHAR_TYPE   curr_ch;
        CHAR_TYPE   prev_ch;
    #endif

    dirp = malloc( sizeof( DIR_TYPE ) );
    HANDLE_OF( dirp ) = 0;
    if( dirp == NULL ) {
        __set_errno_dos( ERROR_NOT_ENOUGH_MEMORY );
        return( NULL );
    }
    dirp->d_attr = _A_SUBDIR;               /* assume sub-directory */
    if( !is_directory( dirname ) ) {
        if( __F_NAME(__opendir,_w__opendir)( dirname, attr, dirp ) == NULL ) {
            free( dirp );
            return( NULL );
        }
    } else {
        dirp->d_attr = _A_SUBDIR;
    }
    if( dirp->d_attr & _A_SUBDIR ) {                    /* 05-apr-91 */
        prev_ch = NULLCHAR;                             /* 28-oct-98 */
        for( i = 0; i < MAX_PATH; i++ ) {
            #ifdef __WIDECHAR__
                curr_ch = *dirname;
            #else
                #ifdef __QNX__
                    curr_ch = *dirname;
                #else
                    curr_ch = _mbsnextc( dirname );
                #endif
            #endif
            pathname[i] = *dirname;
            #ifndef __WIDECHAR__
                if( curr_ch > 256 ) {
                    ++i;
                    ++dirname;
                    pathname[i] = *dirname;     /* copy double-byte */
                }
            #endif
            if( curr_ch == NULLCHAR ) {
                if( i != 0  &&  prev_ch != '\\' && prev_ch != '/' ){
                    pathname[i++] = '\\';
                }
                #ifndef __WIDECHAR__
                    strcpy( &pathname[i], "*.*" );
                #else
                    wcscpy( &pathname[i], L"*.*" );
                #endif
                if( HANDLE_OF( dirp ) != 0 ) {
                    FindClose( HANDLE_OF( dirp ) );
                }
                if( __F_NAME(__opendir,_w__opendir)( pathname, attr, dirp ) == NULL ) {
                    free( dirp );
                    return( NULL );
                }
                break;
            }
            if( *dirname == '*' ) break;
            if( *dirname == '?' ) break;
            ++dirname;
            prev_ch = curr_ch;
        }

/* old logic
        for( i = 0; i < MAX_PATH; i++ ) {
            pathname[i] = *dirname;
            if( *dirname == '\0' ) {
                if( i != 0  &&   pathname[i-1] != '\\' && pathname[i-1] != '/' ){
                    pathname[i++] = '\\';
                }
                #ifndef __WIDECHAR__
                    strcpy( &pathname[i], "*.*" );
                #else
                    wcscpy( &pathname[i], L"*.*" );
                #endif
                if( HANDLE_OF( dirp ) != 0 ) {
                    FindClose( HANDLE_OF( dirp ) );
                }
                if( __F_NAME(__opendir,_w__opendir)( pathname, attr, dirp ) == NULL ) {
                    free( dirp );
                    return( NULL );
                }
                break;
            }
            if( *dirname == '*' ) break;
            if( *dirname == '?' ) break;
            ++dirname;
        }
*/
    }
    if( dirp != NULL )  dirp->d_openpath = __F_NAME(__clib_strdup,__clib_wcsdup)( dirnameStart );
    return( dirp );
}

_WCRTLINK DIR_TYPE *__F_NAME(opendir,_wopendir)( const CHAR_TYPE *dirname )
/*************************************************************************/
{
    return( __F_NAME(_opendir,__wopendir)( dirname, SEEK_ATTRIB ) );
}

_WCRTLINK DIR_TYPE *__F_NAME(readdir,_wreaddir)( DIR_TYPE *dirp )
/***************************************************************/
{
    WIN32_FIND_DATA     ffd;
    DWORD               err;

    if( dirp == NULL ) {
        return( NULL );
    }
    if( dirp->d_first == _DIR_CLOSED )  return( NULL );
    if( dirp->d_first == _DIR_ISFIRST ) {
        dirp->d_first = _DIR_NOTFIRST;
        return( dirp );
    }

    if( !__F_NAME(FindNextFileA,__lib_FindNextFileW)( HANDLE_OF( dirp ), &ffd ) ) {
        err = GetLastError();
        if( err != ERROR_NO_MORE_FILES ) {
            __set_errno_dos( err );
        }
        return( NULL );
    }
    if( !FIND_NEXT( HANDLE_OF( dirp ), ATTR_OF( dirp ), &ffd ) ) {
        __set_errno_dos( ERROR_NO_MORE_FILES );
        return( NULL );
    }
    GET_DIR_INFO( dirp, &ffd );
    return( dirp );

}

_WCRTLINK int __F_NAME(closedir,_wclosedir)( DIR_TYPE *dirp )
/***********************************************************/
{

    if( dirp == NULL || dirp->d_first > _DIR_MAX_FOR_CLOSE_OK ) {
        return( __set_errno_dos( ERROR_INVALID_HANDLE ) );
    }

    if( !FindClose( HANDLE_OF( dirp ) ) ) {
        return( __set_errno_nt() );
    }
    dirp->d_first = _DIR_CLOSED;
    if( dirp->d_openpath != NULL )  free( dirp->d_openpath );
    free( dirp );

    return( 0 );
}

_WCRTLINK void __F_NAME(rewinddir,_wrewinddir)( DIR_TYPE *dirp )
/**************************************************************/
{
    CHAR_TYPE *         openpath;
    DIR_TYPE *          newDirp;

    /*** Get the name of the directory before closing it ***/
    if( dirp->d_openpath == NULL )  return;     /* can't continue if NULL */
    openpath = __F_NAME(__clib_strdup,__clib_wcsdup)( dirp->d_openpath ); /* store path */
    if( openpath == NULL ) {
        dirp->d_first = _DIR_CLOSED;    /* so reads won't work any more */
        return;
    }

    /*** Reopen the directory ***/
    __F_NAME(closedir,_wclosedir)( dirp );              /* close directory */
    newDirp = __F_NAME(opendir,_wopendir)( openpath );  /* open it again */
    if( newDirp == NULL ) {
        dirp->d_first = _DIR_CLOSED;    /* so reads won't work any more */
        return;
    }

    /*** Clean up and go home ***/
    free( openpath );                       /* don't need this any more */
    memcpy( dirp, newDirp, sizeof(DIR_TYPE) );   /* copy into user buffer */
}
