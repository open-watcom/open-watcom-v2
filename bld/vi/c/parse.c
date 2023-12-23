/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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


#include "vi.h"
#include "parse.h"


/*
 * isIgnorable - test if a character is ignorable
 */
static bool isIgnorable( char c, const char *ign )
{
    char    ci;

    while( (ci = *ign++) != '\0' ) {
        if( ci == c || ci == ' ' && isspace( c ) ) {
            return( true );
        }
    }

    return( false );

} /* isIgnorable */

#ifndef VICOMP
/*
 * GetNextWordOrString
 */
vi_rc GetNextWordOrString( const char **pbuff, char *st )
{
    const char  *buff = *pbuff;

    SKIP_SPACES( buff );
    if( *buff == '/' ) {
        buff = GetNextWord( buff, st, SingleSlash );
        if( *buff == '/' ) {
            SKIP_CHAR_SPACES( buff );
        }
    } else if( *buff == '"' ) {
        buff = GetNextWord( buff, st, SingleDQuote );
        if( *buff == '"' ) {
            SKIP_CHAR_SPACES( buff );
        }
    } else {
        buff = GetNextWord1( buff, st );
    }
    *pbuff = buff;
    if( *st == '\0' ) {
        return( ERR_NO_STRING );
    }
    return( ERR_NO_ERR );

} /* GetNextWordOrString */
#endif

/*
 * GetNextWord1 - get next space delimited word in buff
 */
const char *GetNextWord1( const char *buff, char *res )
{
    char    c;

    SKIP_SPACES( buff );
    /*
     * get word
     */
    for( ; (c = *buff) != '\0'; ++buff ) {
        if( isspace( c ) ) {
            SKIP_CHAR_SPACES( buff );
            break;
        }
        *res++ = c;
    }
    *res = '\0';
    return( (char *)buff );

} /* GetNextWord1 */

/*
 * GetNextWord2 - get next space or alternate character delimited word in buff
 */
const char *GetNextWord2( const char *buff, char *res, char alt_delim )
{
    char    c;

    SKIP_SPACES( buff );
    /*
     * get word
     */
    for( ; (c = *buff) != '\0'; ++buff ) {
        if( isspace( c ) ) {
            SKIP_CHAR_SPACES( buff );
            c = *buff;
            break;
        }
        if( c == alt_delim )
            break;
        *res++ = c;
    }
    *res = '\0';
    if( c == alt_delim ) {
        SKIP_CHAR_SPACES( buff );
    }
    return( (char *)buff );

} /* GetNextWord2 */

/*
 * GetNextWord - get next word in buff
 */
const char *GetNextWord( const char *buff, char *res, const char *ign )
{
    size_t      ign_len;
    char        c;

    /*
     * past any leading ignorable chars (if ignore list has single
     * character, then only skip past FIRST ignorable char)
     */
    ign_len = strlen( ign );
    if( ign_len == 1 ) {
        if( isIgnorable( *buff, ign ) ) {
            ++buff;
        }
    } else {
        while( isIgnorable( *buff, ign ) ) {
            ++buff;
        }
    }
    /*
     * get word
     */
    for( ; (c = *buff) != '\0'; ++buff ) {
        /*
         * look for escaped delimiters
         */
        if( c == '\\' && ign_len == 1 ) {
            if( buff[1] == ign[0] ) {
                ++buff;
                *res++ = *buff;
                continue;
            }
            if( buff[1] == '\\' ) {
                ++buff;
                *res++ = '\\';
                *res++ = '\\';
                continue;
            }
        }
        if( isIgnorable( c, ign ) ) {
            break;
        }
        *res++ = c;
    }
    *res = '\0';
    return( (char *)buff );

} /* GetNextWord */
