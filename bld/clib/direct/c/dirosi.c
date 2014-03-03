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
#define __OS2__
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <dos.h>
#include "liballoc.h"
#include "tinyio.h"
#include "seterrno.h"
#include "msdos.h"
#include "_direct.h"
#include "strdup.h"

#define SEEK_ATTRIB (TIO_HIDDEN | TIO_SYSTEM | TIO_SUBDIRECTORY)

static  int     is_directory( const char *name )
/**********************************************/
{
    if( name[0] == '\0' )
        return( 0 );
    while( name[1] != '\0' ){
        if( name[0] == '*' || name[0] == '?' ) {
            /* with wildcard must be file */
            return( -1 );
        }
        ++name;
    }
    if( name[0] == '\\' || name[0] == '/' || name[0] == '.' || name[0] == ':' ){
        /* directory, need add wildcard */
        return( 1 );
    }
    /* without wildcard maybe file or directory, need next check */
    return( 0 );
}

static DIR_TYPE *___opendir( const char *dirname, unsigned attr, DIR_TYPE *dirp )
/*******************************************************************************/
{
    if( dirp->d_first != _DIR_CLOSED ) {
        _dos_findclose( (struct _find_t *)dirp->d_dta );
        dirp->d_first = _DIR_CLOSED;
    }
    if( _dos_findfirst( dirname, attr, (struct _find_t *)dirp->d_dta ) ) {
        return( NULL );
    }
    dirp->d_first = _DIR_ISFIRST;
    return( dirp );
}

static DIR_TYPE *__opendir( const char *dirname, unsigned attr )
/**************************************************************/
{
    DIR_TYPE    tmp;
    DIR_TYPE    *dirp;
    int         i;
    char        pathname[_MAX_PATH+6];
    char        *p;
    char        pchar;
    char        cchar;

    tmp.d_attr = _A_SUBDIR;
    tmp.d_first = _DIR_CLOSED;
    dirp = NULL;
    i = is_directory( dirname );
    if( i <= 0 ) {
        if( (dirp = ___opendir( dirname, attr, &tmp )) == NULL ) {
            return( NULL );
        }
    }
    if( i >= 0 && (tmp.d_attr & _A_SUBDIR) ) {
        /* directory, add wildcard */
        dirp = NULL;
        p = pathname;
        pchar = NULLCHAR;
        for( i = 0; i < _MAX_PATH; i++ ) {
            cchar = dirname[i];
            if( cchar == '\0' ) {
                if( i != 0  &&  pchar != '\\' && pchar != '/' && pchar != ':' ){
                    *p++ = '\\';
                }
                strcpy( p, "*.*" );
                if( (dirp = ___opendir( pathname, attr, &tmp )) == NULL ) {
                    return( NULL );
                }
                dirname = pathname;
                break;
            }
            if( cchar == '*' )
                break;
            if( cchar == '?' ) {
                break;
            }
            *p++ = cchar;
            pchar = cchar;
        }
    }
    if( dirp == NULL ) {
        __set_errno_dos( E_nopath );
        return( NULL );
    }
    dirp = lib_malloc( sizeof( DIR_TYPE ) );
    if( dirp == NULL ) {
        _dos_findclose( (struct _find_t *)tmp.d_dta );
        __set_errno_dos( E_nomem );
        return( NULL );
    }
    tmp.d_openpath = __clib_strdup( dirname );
    *dirp = tmp;
    return( dirp );
}


_WCRTLINK DIR_TYPE *opendir( const char *dirname )
{
    return( __opendir( dirname, SEEK_ATTRIB ) );
}


_WCRTLINK DIR_TYPE *readdir( DIR_TYPE *dirp )
{
    if( dirp == NULL || dirp->d_first == _DIR_INVALID || dirp->d_first == _DIR_CLOSED )
        return( NULL );
    if( dirp->d_first == _DIR_ISFIRST ) {
        dirp->d_first = _DIR_NOTFIRST;
    } else {
        if( _dos_findnext( (struct _find_t *)dirp->d_dta ) ) {
            return( NULL );
        }
    }
    return( dirp );
}


_WCRTLINK int closedir( DIR_TYPE *dirp )
{
    if( dirp == NULL || dirp->d_first == _DIR_CLOSED ) {
        return( __set_errno_dos( E_ihandle ) );
    }
    _dos_findclose( (struct _find_t *)dirp->d_dta );
    dirp->d_first = _DIR_CLOSED;
    if( dirp->d_openpath != NULL )
        free( dirp->d_openpath );
    lib_free( dirp );
    return( 0 );
}


_WCRTLINK void rewinddir( DIR_TYPE *dirp )
{
    if( dirp == NULL || dirp->d_openpath == NULL )
        return;
    if( ___opendir( dirp->d_openpath, SEEK_ATTRIB, dirp ) == NULL ) {
        dirp->d_first = _DIR_INVALID;    /* so reads won't work any more */
    }
}
