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


#include "vi.h"

static bool isIgnorable( char, char * );

/*
 * RemoveLeadingSpaces - remove leading spaces from a string
 */
void RemoveLeadingSpaces( char *buff )
{
    int k = 0;

    if( buff[0] == 0 ) {
        return;
    }
    while( isspace( buff[k] ) ) {
        k++;
    }
    if( k == 0 ) {
        return;
    }
    EliminateFirstN( buff, k );

} /* RemoveLeadingSpaces */

/*
 * TranslateTabs
 */
void TranslateTabs( char *buff )
{
    int k = 0, j;

    if( buff[0] == 0 ) {
        return;
    }
    while( buff[k] ) {
        if( buff[k] == '\\' && buff[k + 1] == 't') {
            buff[k] = '\t';
            for( j = k + 1; buff[j] != 0; j++ ) {
                buff[j] = buff[j + 1];
            }
        }
        k++;
    }

} /* TranslateTabs */

/*
 * GetStringWithPossibleQuote
 */
vi_rc GetStringWithPossibleQuote2( char *data, char *st, bool allow_slash )
{
    int     len;
    
    RemoveLeadingSpaces( data );
    if( allow_slash && data[0] == '/' ) {
        len = NextWord( data, st, SingleSlash );
        if( len >= 0 ) {
            EliminateFirstN( data, 1 );
        }
    } else if( data[0] == '"' ) {
        len = NextWord( data, st, "\"" );
        if( len >= 0 ) {
            EliminateFirstN( data, 1 );
        }
    } else {
        len = NextWord1( data, st );
    }
    if( len <= 0 ) {
        return( ERR_NO_STRING );
    }
    return( ERR_NO_ERR );

} /* GetStringWithPossibleQuote2 */

vi_rc GetStringWithPossibleQuote( char *data, char *st )
{
    return( GetStringWithPossibleQuote2( data, st, TRUE ) );

} /* GetStringWithPossibleQuote */

/*
 * NextWord1 - get next space delimited word in buff
 */
int NextWord1( char *buff, char *res )
{
    int         j, k = 0;
    char        c;

    while( isspace( buff[k] ) ) {
        k++;
    }
    if( buff[k] == 0 ) {
        res[0] = 0;
        return( -1 );
    }
    j = 0;

    /*
     * get word
     */
    while( TRUE ) {
        c = buff[k];
        if( isspace( c ) || c == 0 ) {
            res[j] = 0;
            EliminateFirstN( buff, k );
            return( j );
        } else {
            res[j++] = c;
            k++;
        }
    }

} /* NextWord1 */

/*
 * NextWordSlash - next slash delimited word
 */
int NextWordSlash( char *buff, char *res )
{
    return( NextWord( buff, res, SingleSlash ) );

} /* NextWordSlash */

/*
 * NextWord - get next word in buff
 */
int NextWord( char *buff, char *res, char *ign )
{
    int         j = 0, k = 0, sl;
    char        c;

    /*
     * past any leading ignorable chars (if ignore list has single
     * character, then only skip past FIRST ignorable char)
     */
    sl = strlen( ign );
    if( sl == 1 ) {
        if( isIgnorable( buff[0], ign ) ) {
            k = 1;
        }
    } else {
        while( isIgnorable( buff[k], ign ) ) {
            k++;
        }
    }
    if( buff[k] == 0 ) {
        res[0] = 0;
        return( -1 );
    }

    /*
     * get word
     */
    while( TRUE ) {
        c = buff[k];
        /*
         * look for escaped delimiters
         */
        if( c == '\\' && sl == 1 ) {
            if( buff[k + 1] == ign[0] ) {
                k++;
                res[j++] = buff[k];
                k++;
                continue;
            }
            if( buff[k + 1] == '\\' ) {
                k += 2;
                res[j++] = '\\';
                res[j++] = '\\';
                continue;
            }
        }
        if( isIgnorable( c, ign ) || c == 0 ) {
            res[j] = 0;
            EliminateFirstN( buff, k );
            return( j );
        } else {
            res[j++] = c;
            k++;
        }
    }

} /* NextWord */

/*
 * isIgnorable - test if a character is ignorable
 */
static bool isIgnorable( char c, char *ign )
{
    while( *ign != 0 ) {
        if( *ign == ' ' ) {
            if( isspace( c ) ) {
                return( TRUE );
            }
        } else if( c == *ign ) {
            return( TRUE );
        }
        ign++;
    }

    return( FALSE );

} /* isIgnorable */

/*
 * EliminateFirstN - eliminate first n chars from buff
 */
void EliminateFirstN( char *buff, int n )
{
    char        *buff2;

    buff2 = &buff[n];
    while( *buff2 != 0 ) {
        *buff++ = *buff2++;
    }
    *buff = 0;

} /* EliminateFirstN */

#if !defined( _M_IX86 ) || !defined( __WATCOMC__ )
#define toUpper( x )    toupper( x )
#define toLower( x )    tolower( x )
#else
extern char toUpper( char );
#pragma aux toUpper = \
        "cmp    al, 061h" \
        "jl     LL34" \
        "cmp    al, 07ah" \
        "jg     LL34" \
        "sub    al, 0020H" \
        "LL34:" \
    parm [al] value[al];

extern char toLower( char );
#pragma aux toLower = \
        "cmp    al, 041h" \
        "jl     LL35" \
        "cmp    al, 05ah" \
        "jg     LL35" \
        "add    al, 0020H" \
        "LL35:" \
    parm [al] value[al];
#endif

/*
 * Tokenize - convert character to a token
 */
int Tokenize( char *Tokens, const char *token, bool entireflag )
{
    int         i = 0;
    char        *t, c, tc;
    const char  *tkn;

    if( Tokens == NULL ) {
        return( -1 );
    }
    t = Tokens;
    while( 1 ) {

        if( *t == 0 ) {
            return( -1 );
        }
        tkn = token;
        while( 1 ) {
            c = *t;
            tc = *tkn;
            if( c == 0 && (tc != ' ' && tc != 0) ) {
                break;
            }
            if( isupper( c ) ) {
                if( c != toUpper( tc ) ) {
                    break;
                }
            } else if( entireflag ) {
                if( toUpper( c ) != toUpper( tc ) ) {
                    break;
                }
            } else {
                if( tc == 0 ) {
                    return( i );
                }
                if( isupper( tc ) ) {
                    if( c != (char) toLower( tc ) ) {
                        break;
                    }
                } else  {
                    if( c != tc ) {
                        break;
                    }
                }
            }
            if( c == 0 ) {
                return( i );
            }
            t++;
            tkn++;
        }
        while( *t != 0 ) {
            t++;
        }
        t++;
        i++;

    }

} /* Tokenize */

/*
 * GetNumberOfTokens - return number of tokens in a token string
 */
int GetNumberOfTokens( char *list )
{
    int         i = 0, off = 0, k;
    char        *t;

    while( TRUE ) {

        t = &list[off];
        if( *t == 0 ) {
            break;
        }
        k = strlen( t );
        off += k + 1;
        i++;

    }

    return( i );

} /* GetNumberOfTokens */

/*
 * GetLongestTokenLength - return length of longest token in token string
 */
int GetLongestTokenLength( char *list )
{
    int         i = 0, off = 0, l = 0, k;
    char        *t;

    while( TRUE ) {

        t = &list[off];
        if( *t == 0 ) {
            break;
        }
        k = strlen( t );
        if( k > l ) {
            l = k;
        }
        off += k + 1;
        i++;

    }

    return( l );

} /* GetLongestTokenLength */

#if 0
/*
 * BuildTokenList - build an array of tokens
 */
char **BuildTokenList( int num, char *list )
{
    char        **arr, *data, *t;
    int         k, i = 0, off = 0;

    arr = MemAlloc( num * sizeof( char * ) );
    while( TRUE ) {

        t = &list[off];
        if( *t == 0 ) {
            break;
        }
        k = strlen( t );
        data = MemAlloc( k + 1 );
        memcpy( data, t, k + 1 );
        arr[i] = data;
        off += k + 1;
        i++;

    }
    return( arr );

} /* BuildTokenList */
#endif

/*
 * GetTokenString - return token string
 */
char *GetTokenString( char *list, int num )
{
    int         off = 0, i = 0, k;
    char        *t;

    while( TRUE ) {

        t = &list[off];
        if( *t == 0 ) {
            return( NULL );
        }
        if( i == num ) {
            return( t );
        }
        k = strlen( t );
        off += k + 1;
        i++;

    }

} /* GetTokenString */

/*
 * ReplaceSubString - replace a sub-string with a different one
 */
int ReplaceSubString( char *data, int len, int s, int e, char *rep, int replen )
{
    int i, ln, delta, slen;

    slen = e - s + 1;
    delta = slen - replen;

    /*
     * make room
     */
    ln = len;
    len -= delta;
    if( delta < 0 ) {
        delta *= -1;
        for( i = ln; i > e; i-- ) {
            data[i + delta] = data[i];
        }
    } else if( delta > 0 ) {
        for(i = e + 1; i <= ln; i++ ) {
            data[i - delta] = data[i];
        }
    }

    /*
     * copy in new string
     */
    for( i = 0; i < replen; i++ ) {
        data[s + i] = rep[i];
    }
    return( len );

} /* ReplaceSubString */
