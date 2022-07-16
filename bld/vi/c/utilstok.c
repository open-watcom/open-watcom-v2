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
#include "utilstok.h"


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

#if 0
/*
 * BuildTokenList - build an array of tokens
 */
char **BuildTokenList( int num, char *list )
{
    char        **arr, *data, *t;
    int         k, i = 0, off = 0;

    arr = _MemAllocPtrArray( char, num );
    for( ;; ) {

        t = &list[off];
        if( *t == 0 ) {
            break;
        }
        k = strlen( t );
        data = _MemAllocArray( char, k + 1 );
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
const char *GetTokenString( const char *list, int num )
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
        while( (*dst = (char)tolower( *(unsigned char *)src )) != '\0' ) {
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
