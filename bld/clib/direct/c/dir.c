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


#ifdef __OSI__
#define __OS2__
#endif
#include "variety.h"
#include "widechar.h"
#include <stdlib.h>
#include <string.h>
#include <mbstring.h>
#include <direct.h>
#include "strdup.h"
#include "liballoc.h"
#include "tinyio.h"
#include "seterrno.h"

#define SEEK_ATTRIB (TIO_HIDDEN | TIO_SYSTEM | TIO_SUBDIRECTORY)

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
    if( name[0] == NULLCHAR )  return( 0 );
    while( name[1] != NULLCHAR ) {
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


#ifdef __WIDECHAR__
static void filenameToWide( DIR_TYPE *dir )
/*****************************************/
{
    wchar_t             wcs[_MAX_PATH];

    mbstowcs( wcs, (char*)dir->d_name, _MAX_PATH );     /* convert string */
    wcscpy( dir->d_name, wcs );                         /* copy string */
}
#endif


_WCRTLINK DIR_TYPE *__F_NAME(_opendir,_w_opendir)( const CHAR_TYPE *name, char attr )
{
    #ifndef __OSI__
        DIR_TYPE        tmp;
    #endif
    DIR_TYPE    *parent;
    int         i;
    tiny_ret_t  rc;
    auto CHAR_TYPE  pathname[ _MAX_PATH + 6 ];
    const CHAR_TYPE *dirnameStart = name;
    #ifndef __WIDECHAR__
        unsigned    curr_ch;
        unsigned    prev_ch;
    #else
        CHAR_TYPE   curr_ch;
        CHAR_TYPE   prev_ch;
    #endif

    /*** Convert a wide char string to a multibyte string ***/
    #ifdef __WIDECHAR__
        char            mbcsName[MB_CUR_MAX*_MAX_PATH];

        if( wcstombs( mbcsName, name, MB_CUR_MAX*(wcslen(name)+1) ) == (size_t)-1 )
            return( NULL );
    #endif

    #ifdef __OSI__
        parent = lib_malloc( sizeof( *parent ) );
        if( parent == NULL ) {
            return( parent );
        }
    #else
        parent = &tmp;
        TinySetDTA( &(tmp.d_dta) );
    #endif
    if( ! is_directory( name ) ) {
        #ifdef __WIDECHAR__
            #ifdef __OSI__
                rc = TinyFindFirstDTA( mbcsName, attr, &parent->d_dta );
            #else
                rc = TinyFindFirst( mbcsName, attr );
            #endif
        #else
            #ifdef __OSI__
                rc = TinyFindFirstDTA( name, attr, &parent->d_dta );
            #else
                rc = TinyFindFirst( name, attr );
            #endif
        #endif
        if( TINY_ERROR( rc ) ) {
            __set_errno_dos( TINY_INFO( rc ) );
            #ifdef __OSI__
                lib_free( parent );
            #endif
            return( NULL );
        }
    } else {
        parent->d_attr = _A_SUBDIR;
    }
    if( parent->d_attr & _A_SUBDIR ) {                  /* 05-apr-91 */
        prev_ch = NULLCHAR;                             /* 28-oct-98 */
        for( i = 0; i < _MAX_PATH; i++ ) {
            #ifdef __WIDECHAR__
                curr_ch = *name;
            #else
                #ifdef __QNX__
                    curr_ch = *name;
                #else
                    curr_ch = _mbsnextc( name );
                #endif
            #endif
            pathname[i] = *name;
            #ifndef __WIDECHAR__
                if( curr_ch > 256 ) {
                    ++i;
                    ++name;
                    pathname[i] = *name;        /* copy double-byte */
                }
            #endif
            if( curr_ch == NULLCHAR ) {
                if( i != 0  &&  prev_ch != '\\' && prev_ch != '/' ){
                    pathname[i++] = '\\';
                }
                #ifndef __WIDECHAR__
                    strcpy( &pathname[i], "*.*" );
                    #ifdef __OSI__
                        rc = TinyFindFirstDTA( pathname, attr, &parent->d_dta );
                    #else
                        rc = TinyFindFirst( pathname, attr );
                    #endif
                #else
                    wcscpy( &pathname[i], L"*.*" );
                    if( wcstombs( mbcsName, pathname, MB_CUR_MAX*(wcslen(pathname)+1) ) == (size_t)-1 )
                        return( NULL );
                    #ifdef __OSI__
                        rc = TinyFindFirstDTA( mbcsName, attr, &parent->d_dta );
                    #else
                        rc = TinyFindFirst( mbcsName, attr );
                    #endif
                #endif
                if( TINY_ERROR( rc ) ) {
                    __set_errno_dos( TINY_INFO( rc ) );
                    #ifdef __OSI__
                        lib_free( parent );
                    #endif
                    return( NULL );
                }
                break;
            }
            if( *name == '*' ) break;
            if( *name == '?' ) break;
            ++name;
            prev_ch = curr_ch;
        }

/* old logic
        for( i = 0; i < _MAX_PATH; i++ ) {
            pathname[i] = *name;
            if( *name == '\0' ) {
                if( i != 0  &&  pathname[i-1] != '\\' && pathname[i-1] != '/' ){
                    pathname[i++] = '\\';
                }
                #ifndef __WIDECHAR__
                    strcpy( &pathname[i], "*.*" );
                    #ifdef __OSI__
                        rc = TinyFindFirstDTA( pathname, attr, &parent->d_dta );
                    #else
                        rc = TinyFindFirst( pathname, attr );
                    #endif
                #else
                    wcscpy( &pathname[i], L"*.*" );
                    if( wcstombs( mbcsName, pathname, MB_CUR_MAX*(wcslen(pathname)+1) ) == (size_t)-1 )
                        return( NULL );
                    #ifdef __OSI__
                        rc = TinyFindFirstDTA( mbcsName, attr, &parent->d_dta );
                    #else
                        rc = TinyFindFirst( mbcsName, attr );
                    #endif
                #endif
                if( TINY_ERROR( rc ) ) {
                    __set_errno_dos( TINY_INFO( rc ) );
                    #ifdef __OSI__
                        lib_free( parent );
                    #endif
                    return( NULL );
                }
                break;
            }
            if( *name == '*' ) break;
            if( *name == '?' ) break;
            ++name;
        }
*/
    }
    #ifndef __OSI__
        parent = lib_malloc( sizeof( *parent ) );
        if( parent == NULL ) {
            return( parent );
        }
        *parent = tmp;
    #endif
    parent->d_first = _DIR_ISFIRST;

    if( parent != NULL ) parent->d_openpath = __F_NAME(__clib_strdup,__clib_wcsdup)( dirnameStart );
    return( parent );
}


_WCRTLINK DIR_TYPE *__F_NAME(opendir,_wopendir)( const CHAR_TYPE *name )
{
    return( __F_NAME(_opendir,_w_opendir)( name, SEEK_ATTRIB ) );
}


_WCRTLINK DIR_TYPE *__F_NAME(readdir,_wreaddir)( DIR_TYPE *parent )
{
    tiny_ret_t rc;

    if( parent == NULL ) {
        return( NULL );
    }
    if( parent->d_first == _DIR_CLOSED )  return( NULL );
    if( parent->d_first == _DIR_ISFIRST ) {
        parent->d_first = _DIR_NOTFIRST;
        #ifdef __WIDECHAR__
            filenameToWide( parent );
        #endif
        return( parent );
    }
    #ifdef __OSI__
        rc = TinyFindNextDTA( &parent->d_dta );
    #else
        TinySetDTA( &(parent->d_dta) );
        rc = TinyFindNext();
    #endif
    if( TINY_ERROR( rc ) == 18 ) { // E_nomore files
        return( NULL );
    } else if( TINY_ERROR( rc ) ) {
        __set_errno_dos( TINY_INFO( rc ) );
        return( NULL );
    }

    #ifdef __WIDECHAR__
        filenameToWide( parent );
    #endif
    return( parent );
}


_WCRTLINK int __F_NAME(closedir,_wclosedir)( DIR_TYPE *dirp )
{
    if( dirp == NULL || dirp->d_first > _DIR_MAX_FOR_CLOSE_OK ) {
        return( 1 );
    }
    #ifdef __OSI__
        TinyFindCloseDTA( &dirp->d_dta );
    #endif
    dirp->d_first = _DIR_CLOSED;
    if( dirp->d_openpath != NULL )  free( dirp->d_openpath );
    lib_free( dirp );
    return( 0 );
}


_WCRTLINK void __F_NAME(rewinddir,_wrewinddir)( DIR_TYPE *dirp )
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
