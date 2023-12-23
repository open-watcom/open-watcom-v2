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
#include "tokenize.h"


#if defined( __WATCOMC__ ) && defined( _M_IX86 )
extern char toUpper( char );
#pragma aux toUpper = \
        "cmp  al,61h"   \
        "jl short L1"   \
        "cmp  al,7ah"   \
        "jg short L1"   \
        "sub  al,20h"   \
    "L1:"               \
    __parm      [__al] \
    __value     [__al] \
    __modify    []

extern char toLower( char );
#pragma aux toLower = \
        "cmp  al,41h"   \
        "jl short L1"   \
        "cmp  al,5ah"   \
        "jg short L1"   \
        "add  al,20h"   \
    "L1:"               \
    __parm      [__al] \
    __value     [__al] \
    __modify    []
#else
#define toUpper( x )    toupper( x )
#define toLower( x )    tolower( x )
#endif

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
        SKIP_TOEND( t );
        i++;
    }
    return( TOK_INVALID );

} /* Tokenize */
