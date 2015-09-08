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

    if( buff[0] == '\0' ) {
        return;
    }
    while( isspace( buff[k] ) ) {
        k++;
    }
    if( k ) {
        EliminateFirstN( buff, k );
    }

} /* RemoveLeadingSpaces */

/*
 * SkipLeadingSpaces - remove leading spaces from a string
 */
char *SkipLeadingSpaces( const char *buff )
{
    while( isspace( *buff ) ) {
        buff++;
    }
    return( (char *)buff );

} /* SkipLeadingSpaces */

/*
 * TranslateTabs
 */
void TranslateTabs( char *buff )
{
    int     k;
    int     j;

    for( k = 0; buff[k] != '\0'; ++k ) {
        if( buff[k] == '\\' && buff[k + 1] == 't') {
            buff[k] = '\t';
            for( j = k + 1; buff[j] != 0; j++ ) {
                buff[j] = buff[j + 1];
            }
        }
    }

} /* TranslateTabs */

/*
 * GetStringWithPossibleQuote2
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
        len = NextWord( data, st, SingleQuote );
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
    return( GetStringWithPossibleQuote2( data, st, true ) );

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
    if( buff[k] == '\0' ) {
        res[0] = '\0';
        return( -1 );
    }

    /*
     * get word
     */
    for( j = 0; (c = buff[k]) != '\0'; ++k ) {
        if( isspace( c ) )
            break;
        res[j++] = c;
    }
    res[j] = '\0';
    EliminateFirstN( buff, k );
    return( j );

} /* NextWord1 */

/*
 * GetNextWord1 - get next space delimited word in buff
 */
char *GetNextWord1( const char *buff, char *res )
{
    int         j;
    char        c;

    while( isspace( *buff ) ) {
        ++buff;
    }
    /*
     * get word
     */
    for( j = 0; (c = *buff) != '\0'; ++buff ) {
        if( isspace( c ) )
            break;
        res[j++] = c;
    }
    res[j] = '\0';
    return( (char *)buff );

} /* GetNextWord1 */

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
    if( buff[k] == '\0' ) {
        res[0] = '\0';
        return( -1 );
    }

    /*
     * get word
     */
    for( ; (c = buff[k]) != '\0'; ++k ) {
        /*
         * look for escaped delimiters
         */
        if( c == '\\' && sl == 1 ) {
            if( buff[k + 1] == ign[0] ) {
                ++k;
                res[j++] = buff[k];
                continue;
            }
            if( buff[k + 1] == '\\' ) {
                ++k;
                res[j++] = '\\';
                res[j++] = '\\';
                continue;
            }
        }
        if( isIgnorable( c, ign ) ) {
            break;
        }
        res[j++] = c;
    }
    res[j] = '\0';
    EliminateFirstN( buff, k );
    return( j );

} /* NextWord */

/*
 * isIgnorable - test if a character is ignorable
 */
static bool isIgnorable( char c, char *ign )
{
    while( *ign != 0 ) {
        if( *ign == ' ' ) {
            if( isspace( c ) ) {
                return( true );
            }
        } else if( c == *ign ) {
            return( true );
        }
        ign++;
    }

    return( false );

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

#if defined( __WATCOMC__ ) && defined( _M_IX86 )
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
#else
#define toUpper( x )    toupper( x )
#define toLower( x )    tolower( x )
#endif

#define BOTX(s) ( s[0] == '!' && s[1] != '\0' )

#define EOT(c)  ( c == '\0' || c == ' ' )
#define EOTX(s) ( s[0] == exclm && EOT( s[1] ) )

/*
 * Tokenize - convert character to a token
 */
int Tokenize( const char *Tokens, const char *token, bool entireflag )
{
    int         i = 0;
    const char  *t, *tkn;
    char        c, tc, exclm;

    if( Tokens == NULL || *token == '\0' ) {
        return( TOK_INVALID );
    }
    for( t = Tokens; *t != '\0'; ++t ) {
        tkn = token;
        exclm = '\0';
        if( BOTX( t ) ) {
            exclm = '!';
            ++t;
            ++i;
        }
        for( ;; ) {
            c = *t;
            tc = *tkn;
            if( c == 0 ) {
                if( EOT( tc ) ) {
                    return( i );
                }
                if( EOTX( tkn ) ) {
                    return( i - 1 );
                }
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
                if( EOT( tc ) ) {
                    return( i );
                }
                if( EOTX( tkn ) ) {
                    return( i - 1 );
                }
                if( isupper( tc ) ) {
                    if( c != (char)toLower( tc ) ) {
                        break;
                    }
                } else  {
                    if( c != tc ) {
                        break;
                    }
                }
            }
            t++;
            tkn++;
        }
        while( *t != 0 ) {
            t++;
        }
        i++;
    }
    return( TOK_INVALID );

} /* Tokenize */

/*
 * GetNumberOfTokens - return number of tokens in a token string
 */
int GetNumberOfTokens( const char *list )
{
    int         i;

    for( i = 0; *list != '\0'; ++i ) {
        if( BOTX( list ) ) {
            ++i;
        }
        list += strlen( list ) + 1;
    }
    return( i );

} /* GetNumberOfTokens */

/*
 * GetLongestTokenLength - return length of longest token in token string
 */
int GetLongestTokenLength( const char *list )
{
    int         max_len = 0, len;
    const char  *t;

    for( t = list; *t != '\0'; t += len + 1 ) {
        len = strlen( t );
        if( len > max_len ) {
            max_len = len;
        }
    }
    return( max_len );

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
    for( ;; ) {

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
char *GetTokenString( const char *list, int num )
{
    int         i = 0;
    const char  *t;

    for( t = list; *t != '\0'; ) {
        if( i == num ) {
            return( (char *)t );
        }
        if( BOTX( t ) ) {
            ++i;
            if( i == num ) {
                return( (char *)( t + 1 ) );
            }
        }
        t += strlen( t ) + 1;
        i++;
    }
    return( NULL );

} /* GetTokenString */

/*
 * GetTokenStringCVT - return token string
 */
char *GetTokenStringCVT( const char *list, int num, char *dst, bool lowercase )
{
    char        *new = dst;
    const char  *src;
    int         f;

    if( dst == NULL ) {
        return( NULL );
    }
    src = GetTokenString( list, num );
    if( src == NULL ) {
        *dst = '\0';
        return( NULL );
    }
    f = ( *src == '!' );
    if( f ) {
        src++;
    }
    if( lowercase ) {
        while( (*dst = tolower( *src )) != '\0' ) {
            ++src;
            ++dst;
        }
    } else {
        while( (*dst = *src) != '\0' ) {
            ++src;
            ++dst;
        }
    }
    if( f ) {
        *dst++ = '!';
        *dst = '\0';
    }
    return( new );

} /* GetTokenStringCVT */

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
