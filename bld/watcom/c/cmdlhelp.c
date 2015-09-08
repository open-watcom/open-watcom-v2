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
* Description:  Command line argument handling helpers.
*
****************************************************************************/

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "cmdlhelp.h"

#include "clibext.h"


int BuildQuotedFName( char *dst, size_t maxlen, const char *path, const char *filename, const char *quote_char )
/*****************************************************************************************************************/
{
    int has_space = 0;

    maxlen = maxlen;
    if( strchr( path, ' ' ) != NULL )
        has_space = 1;
    if( strchr( filename, ' ' ) != NULL )
        has_space = 1;

    strcpy( dst, has_space ? quote_char : "" );
    strcat( dst, path );
    strcat( dst, filename );
    strcat( dst, has_space ? quote_char : "" );

    return( has_space );
}

int UnquoteFName( char *dst, size_t maxlen, const char *src )
/***********************************************************************
 * Removes doublequote characters from filename and copies other content
 * from src to dst. Only maxlen number of characters are copied to dst
 * including terminating NUL character. Returns value 1 when quotes was
 * removed from orginal filename, 0 otherwise.
 */
{
    char    string_open = 0;
    size_t  pos = 0;
    char    t;
    int     un_quoted = 0;

    assert( maxlen );

    // leave space for NUL terminator
    maxlen--;

    while( pos < maxlen ) {
        t = *src++;

        if( t == '\0' ) break;

        if( t == '\\' ) {
            t = *src++;

            if( t == '\"' ) {
                *dst++ = '\"';
                pos++;
                un_quoted = 1;
            } else {
                *dst++ = '\\';
                pos++;

                if( pos < maxlen ) {
                    *dst++ = t;
                    pos++;
                }
            }
        } else {
            if( t == '\"' ) {
                string_open = !string_open;
                un_quoted = 1;
            } else {
                if( string_open ) {
                    *dst++ = t;
                    pos++;
                } else if( t == ' ' || t == '\t' ) {
                    break;
                } else {
                    *dst++ = t;
                    pos++;
                }
            }
        }
    }

    *dst = '\0';

    return( un_quoted );
}

char *FindNextWS( char *str )
/***********************************
 * Finds next free white space character, allowing doublequotes to
 * be used to specify strings with white spaces.
 */
{
    char    string_open = 0;

    while( *str != '\0' ) {
        if( *str == '\\' ) {
            str++;
            if( *str != '\0' ) {
                if( !string_open && ( *str == ' ' || *str == '\t' ) ) {
                    break;
                }
                str++;
            }
        } else {
            if( *str == '\"' ) {
                string_open = !string_open;
                str++;
            } else {
                if( !string_open && ( *str == ' ' || *str == '\t' ) ) {
                    break;
                }
                str++;
            }
        }
    }

    return( str );
}

