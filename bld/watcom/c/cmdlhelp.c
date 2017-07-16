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


bool BuildQuotedItem( char *dst, size_t maxlen, const char *path, const char *filename, const char *quote_char )
/**************************************************************************************************************/
{
    bool    has_space;

    /* unused parameters */ (void)maxlen;

    has_space = false;
    if( strchr( path, ' ' ) != NULL )
        has_space = true;
    if( strchr( filename, ' ' ) != NULL )
        has_space = true;

    strcpy( dst, has_space ? quote_char : "" );
    strcat( dst, path );
    strcat( dst, filename );
    strcat( dst, has_space ? quote_char : "" );

    return( has_space );
}

bool UnquoteItem( char *dst, size_t maxlen, const char *src, bool (*chk_sep)(char) )
/***********************************************************************************
 * Removes doublequote characters from filename and copies other content
 * from src to dst. Only maxlen number of characters are copied to dst
 * including terminating NUL character. Returns value 1 when quotes was
 * removed from orginal filename, 0 otherwise.
 */
{
    size_t  pos;
    char    c;
    bool    string_open;
    bool    un_quoted;

    assert( maxlen );

    // leave space for NUL terminator
    maxlen--;
    pos = 0;
    string_open = false;
    un_quoted = false;
    while( pos < maxlen && (c = *src++) != '\0' ) {
        if( c == '\"' ) {
            string_open = !string_open;
            un_quoted = true;
            continue;
        }
        if( c == '\\' ) {
            if( string_open && *src != '\0' ) {
                c = *src++;
                if( c == '\"' ) {
                    un_quoted = true;
                }
            }
        } else if( !string_open && chk_sep( c ) ) {
            break;
        }
        *dst++ = c;
        pos++;
    }
    *dst = '\0';

    return( un_quoted );
}

char *FindNextSep( const char *str, bool (*chk_sep)(char) )
/*********************************************************
 * Finds next free white space character, allowing doublequotes to
 * be used to specify strings with white spaces.
 */
{
    bool        string_open;
    char        c;

    string_open = false;
    while( (c = *str) != '\0' ) {
        if( c == '\"' ) {
            string_open = !string_open;
        } else if( c == '\\' ) {
            if( string_open && str[1] != '\0' ) {
                ++str;
            }
        } else if( !string_open && chk_sep( c ) ) {
            break;
        }
        ++str;
    }

    return( (char *)str );
}
