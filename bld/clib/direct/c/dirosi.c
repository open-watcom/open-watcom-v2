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
#include "liballoc.h"
#include "tinyio.h"
#include "seterrno.h"

#define SEEK_ATTRIB (TIO_HIDDEN | TIO_SYSTEM | TIO_SUBDIRECTORY)

static  int     is_directory( const char *name ) {
/**************************************************/
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
}

_WCRTLINK DIR_TYPE *_opendir( const char *name, char attr )
{
    DIR_TYPE    *parent;
    int         i;
    tiny_ret_t  rc;
    auto char   pathname[_MAX_PATH+6];

    parent = lib_malloc( sizeof( *parent ) );
    if( parent == NULL ) {
        return( parent );
    }
    if( ! is_directory( name ) ) {
        rc = TinyFindFirstDTA( name, attr, &parent->d_dta );
        if( TINY_ERROR( rc ) ) {
            __set_errno_dos( TINY_INFO( rc ) );
            lib_free( parent );
            return( NULL );
        }
    } else {
        parent->d_attr = _A_SUBDIR;
    }
    if( parent->d_attr & _A_SUBDIR ) {                  /* 05-apr-91 */
        for( i = 0; i < _MAX_PATH; i++ ) {
            pathname[i] = *name;
            if( *name == '\0' ) {
                if( i != 0  &&  pathname[i-1] != '\\' && pathname[i-1] != '/' ){
                    pathname[i++] = '\\';
                }
                strcpy( &pathname[i], "*.*" );
                rc = TinyFindFirstDTA( pathname, attr, &parent->d_dta );
                if( TINY_ERROR( rc ) ) {
                    __set_errno_dos( TINY_INFO( rc ) );
                    lib_free( parent );
                    return( NULL );
                }
                break;
            }
            if( *name == '*' ) break;
            if( *name == '?' ) break;
            ++name;
        }
    }
    parent->d_first = 1;
    return( parent );
}


_WCRTLINK DIR_TYPE *opendir( const char *name )
{
    return( _opendir( name, SEEK_ATTRIB ) );
}


_WCRTLINK DIR_TYPE *readdir( DIR_TYPE *parent )
{
    tiny_ret_t rc;

    if( parent == NULL ) {
        return( NULL );
    }
    if( parent->d_first ) {
        parent->d_first = 0;
        return( parent );
    }
    rc = TinyFindNextDTA( &parent->d_dta );
    if( TINY_ERROR( rc ) == 18 ) { // E_nomore files
        return( NULL );
    } else if( TINY_ERROR( rc ) ) {
        __set_errno_dos( TINY_INFO( rc ) );
        return( NULL );
    }
    return( parent );
}


_WCRTLINK int closedir( DIR_TYPE *dirp )
{
    if( dirp == NULL ) {
        return( 1 );
    }
    if( dirp->d_first > 1 ) {
        return( 1 );
    }
    TinyFindCloseDTA( &dirp->d_dta );
    dirp->d_first = 2;
    lib_free( dirp );
    return( 0 );
}
