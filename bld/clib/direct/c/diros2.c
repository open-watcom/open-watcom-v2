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
#define INCL_ERRORS
#include <wos2.h>
#include "tinyio.h"
#include "rtdata.h"
#include "strdup.h"
#include "seterrno.h"

#if defined(__WARP__)
  #define FF_LEVEL      1
  #define FF_BUFFER     FILEFINDBUF3
#else
  #define FF_LEVEL      0
  #define FF_BUFFER     FILEFINDBUF
#endif

#define _DIR_ISFIRST            0
#define _DIR_NOTFIRST           1
#define _DIR_MAX_FOR_CLOSE_OK   2       /* dummy value used by closedir */
#define _DIR_CLOSED             3

#define SEEK_ATTRIB (_A_SUBDIR | _A_HIDDEN | _A_SYSTEM | _A_RDONLY | _A_ARCH)

/* Since dptr->d_dta is not used under OS/2, we will store the handle for
 * this directory scan in it.  This macro simply does the appropriate casting
 * for us. DJG
 */
#define HANDLE_OF(dirp)         ( *( HDIR * )( &( dirp )->d_dta[ 0 ] ) )

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
    mbstowcs( wcs, (char*)dirp->d_name, _MAX_PATH );    /* convert string */
    wcscpy( dirp->d_name, wcs );                        /* copy string */
#endif
}


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

    USHORT          rc;
    FF_BUFFER       dir_buff;
    HFILE           handle;
    OS_UINT         searchcount;

    /*** Convert a wide char string to a multibyte string ***/
    #ifdef __WIDECHAR__
        char        mbcsName[MB_CUR_MAX*_MAX_PATH];

        if( wcstombs( mbcsName, dirname, MB_CUR_MAX*(wcslen(dirname)+1) ) == (size_t)-1 )
            return( NULL );
    #endif

#if defined(__OS2_286__)
    if( _RWD_osmode == OS2_MODE ) {
#endif
        handle = ~0;            /* we want our own handle */
        searchcount = 1;        /* only one at a time */
        #ifndef __WIDECHAR__
            rc = DosFindFirst( (PSZ)dirname, &handle, attr, (PVOID)&dir_buff,
                        sizeof( dir_buff ), &searchcount, FF_LEVEL );
        #else
            rc = DosFindFirst( (PSZ)mbcsName, &handle, attr, (PVOID)&dir_buff,
                        sizeof( dir_buff ), &searchcount, FF_LEVEL );
        #endif

        if( rc != 0 ) {
            __set_errno_dos( rc );
            return( NULL );
        }
        if( searchcount != 1 ) {
            DosFindClose( handle );
            __set_errno_dos( ERROR_NO_MORE_FILES );
            return( NULL );
        }
        HANDLE_OF( dirp ) = handle;     /* store our handle     */
        copydir( dirp, &dir_buff );     /* copy in other fields */

#if defined(__OS2_286__)
    } else {            /* real mode */
        DIR_TYPE            buf;

        TinySetDTA( buf.d_dta );    /* set our DTA */
        #ifndef __WIDECHAR__
            rc = TinyFindFirst( dirname, attr );
        #else
            rc = TinyFindFirst( mbcsName, attr );
        #endif
        if( rc > 0 ) {
            __set_errno_dos( rc );
            return( NULL );
        }
        *dirp = buf;                    /* copy to new memory */
    }
#endif

    dirp->d_first = _DIR_ISFIRST;       /* indicate we have 1st name */
    return( dirp );
}


_WCRTLINK DIR_TYPE *__F_NAME(_opendir,_w_opendir)( const CHAR_TYPE *dirname,
                                                    int attr )
/**************************************************************************/
{

    DIR_TYPE        *dirp;
    int             i;
    auto CHAR_TYPE  pathname[_MAX_PATH+6];
    const CHAR_TYPE *dirnameStart = dirname;
    #ifndef __WIDECHAR__
        unsigned    curr_ch;
        unsigned    prev_ch;
    #else
        CHAR_TYPE   curr_ch;
        CHAR_TYPE   prev_ch;
    #endif

    dirp = malloc( sizeof( DIR_TYPE ) );
    HANDLE_OF( dirp ) = 0;                  /* initialize handle    */
    if( dirp == NULL ) {
        __set_errno_dos( ERROR_NOT_ENOUGH_MEMORY );
        return( NULL );
    }
    if( ! is_directory( dirname ) ) {
        if( __F_NAME(__opendir,_w__opendir)( dirname, attr, dirp ) == NULL ) {
            free( dirp );
            return( NULL );
        }
    } else {
        dirp->d_attr = _A_SUBDIR;
    }
    if( dirp->d_attr & _A_SUBDIR ) {                    /* 05-apr-91 */
        prev_ch = NULLCHAR;                             /* 28-oct-98 */
        for( i = 0; i < _MAX_PATH; i++ ) {
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
                    DosFindClose( HANDLE_OF(dirp) );
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
        for( i = 0; i < _MAX_PATH; i++ ) {
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
                    DosFindClose( HANDLE_OF(dirp) );
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
    if( dirp != NULL ) dirp->d_openpath = __F_NAME(__clib_strdup,__clib_wcsdup)( dirnameStart );
    return( dirp );
}


_WCRTLINK DIR_TYPE *__F_NAME(opendir,_wopendir)( const CHAR_TYPE *dirname )
/*************************************************************************/
{
    return( __F_NAME(_opendir,_w_opendir)( dirname, SEEK_ATTRIB ) );
}


_WCRTLINK DIR_TYPE *__F_NAME(readdir,_wreaddir)( DIR_TYPE *dirp )
/***************************************************************/
{
    USHORT  rc;

    if( dirp == NULL ) {
        return( NULL );
    }
    if( dirp->d_first == _DIR_CLOSED )  return( NULL );
    if( dirp->d_first == _DIR_ISFIRST ) {       /* if we already have one */
        dirp->d_first = _DIR_NOTFIRST;
        return( dirp );
    }

#if defined(__OS2_286__)
    if( _RWD_osmode == OS2_MODE )          /* protected mode */
#endif
        {

        FF_BUFFER       dir_buff;
        OS_UINT         searchcount = 1;

        rc = DosFindNext( HANDLE_OF( dirp ), (PVOID)&dir_buff,
            sizeof( dir_buff ), &searchcount );
        if( rc == ERROR_NO_MORE_FILES ) {
            return( NULL );
        }
        if( rc != 0 ) {
            __set_errno_dos( rc );
            return( NULL );
        }

        if( searchcount != 1 ) {
            __set_errno_dos( ERROR_NO_MORE_FILES );
            return( NULL );
        }

        copydir( dirp, &dir_buff );

        }
#if defined(__OS2_286__)
    else {              /* real mode */
        TinySetDTA( dirp->d_dta );

        rc = TinyFindNext();
        if( TINY_ERROR( rc ) == 18 ) { // E_nomore files
            return( NULL );
        } else if( TINY_ERROR( rc ) ) {
            __set_errno_dos( TINY_INFO( rc ) );
            return( NULL );
        }
    }
#endif
    return( dirp );
}


_WCRTLINK int __F_NAME(closedir,_wclosedir)( DIR_TYPE *dirp )
/***********************************************************/
{

    if( dirp == NULL || dirp->d_first > _DIR_MAX_FOR_CLOSE_OK  ) {/* error */
        __set_errno_dos( ERROR_INVALID_HANDLE );
        return( -1 );
    }

#if defined(__OS2_286__)
    if( _RWD_osmode == OS2_MODE )
#endif
        DosFindClose( HANDLE_OF( dirp ) );

    dirp->d_first = _DIR_CLOSED;
    if( dirp->d_openpath != NULL )  free( dirp->d_openpath );
    free( dirp );

    return( 0 );                        /* ok */
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
