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


/*
 * Program which does a recursive directory descent.  Used to test the
 * open/read/closedir functions.
 *
 * 16-apr-90    D.J.Gaudet      Defined
 */

#include <direct.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


static char *stpcpy( char *d, const char *s )
/*******************************************/
{
    while( *d = *s ) {
        ++d;
        ++s;
    }
    return( d );
}


static int doADir( char *path, char *work )
/*****************************************/
{
    DIR     *parent;
    DIR     *entry;
    char    *p;
    char    attr;

    *work = 0;
    puts( path );

    strcpy( work, "*.*" );
    parent = opendir( path );
    if( parent == NULL ) {
        return( -1 );
    }

        /* note assignment in while(...) */
    while( entry = readdir( parent ) ) {
        attr = entry->d_attr;
        p = entry->d_name;
        if( p[0] == '.' && ( p[1] == '\0' ||
            ( p[1] == '.' && p[2] == '\0' ) ) ) continue;
        if( attr & _A_SUBDIR ) {
            p = stpcpy( work, p );
            *p++ = '\\';
            doADir( path, p );
        } else {
            strcpy( work, p );
            puts( path );
        }
    }
    closedir( parent );

    return( 0 );
}


void main( int argc, char **argv )
/********************************/
{
    char path[ _MAX_PATH ];
    char *p;

    if( argc == 1 ) {
        p = stpcpy( path, "\\" );
        doADir( path, p );
    } else {
        while( ++argv, --argc > 0 ) {
            p = stpcpy( path, *argv );
            if( p > path && p[-1] != '\\' && p[-1] != '/' && p[-1] != ':' ) {
                *p++ = '/';
            }

            doADir( path, p );
        }
    }
}
