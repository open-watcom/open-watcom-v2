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


#include <windows.h>
#include <ctype.h>
#include <string.h>
#include <mbstring.h>
#include "watcom.h"
#include "wrglbl.h"
#include "wrstrdup.h"

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

char *WRStrDup( const char *src )
{
    char *dest;

    if( src != NULL ) {
        if( (dest = WRMemAlloc( strlen( src ) + 1 )) != NULL ) {
            strcpy( dest, src );
        }
    } else {
        return( NULL );
    }

    return( dest );
}

char *WRCopyString( char *mem, char *str, int len )
{
    memcpy( mem, str, len );
    return( mem + len );
}

BOOL WRIsStrSpace( char *_s )
{
    unsigned char   *s = (unsigned char *)_s;

    if( s != NULL ) {
        for( ; *s != '\0'; s = _mbsinc( s ) ) {
            if( _mbclen( s ) != 1 || (_mbclen( s ) == 1 && !isspace( *s )) ) {
                return( FALSE );
            }
        }
    }
    return( TRUE );
}

BOOL WRStrHasSpace( char *_s )
{
    unsigned char   *s = (unsigned char *)_s;

    if( s != NULL ) {
        for( ; *s != '\0'; s = _mbsinc( s ) ) {
            if( _mbclen( s ) == 1 && isspace( *s ) ) {
                return( TRUE );
            }
        }
    }
    return( FALSE );
}

void WRStripStr( char *_s )
{
    unsigned char   *last_space;
    unsigned char   *last_nonspace;
    unsigned char   *p;
    unsigned char   *s = (unsigned char *)_s;

    if( s != NULL ) {
        last_space = NULL;
        last_nonspace = NULL;
        for( p = s; *p != '\0'; p = _mbsinc( p ) ) {
            if( _mbclen( p ) == 1 && isspace( *p ) ) {
                if( last_space == NULL ||
                    (last_nonspace != NULL && (last_nonspace > last_space) ) ) {
                    last_space = p;
                }
            } else {
                last_nonspace = p;
            }
        }

        // string has no spaces
        if( last_space == NULL ) {
            return;
        }

        // string has is all spaces
        if( last_space == NULL ) {
            s[0] = '\0';
            return;
        }

        if( last_space > last_nonspace ) {
            last_space[0] = '\0';
        }

        // find first non-whitespace char
        for( p = s; *p != '\0'; p = _mbsinc( p ) ) {
            if( _mbclen( p ) != 1 || (_mbclen( p ) == 1 && !isspace( *p )) ) {
                break;
            }
        }
        if( *p != '\0' && p != s ) {
            memmove( s, p, strlen( (char *)p ) + 1 );
        }
    }
}
