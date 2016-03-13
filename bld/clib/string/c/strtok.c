/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. 
*    Portions Copyright (c) 2015 Open Watcom contributors.
*    All Rights Reserved.
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
* Description:  Implementation of strtok(), strtok_r(), and wcstok().
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdio.h>
#include <string.h>
#include "rtdata.h"
#ifdef __WIDECHAR__
    #include "nextwtok.h"
#else
    #include "nexttok.h"
#endif
#include "setbits.h"
#include "thread.h"


#ifdef __WIDECHAR__
_WCRTLINK wchar_t *_ustrtok( wchar_t *str, const wchar_t *charset )
{
    return( wcstok( str, charset, NULL ) );
}

_WCRTLINK wchar_t *wcstok( wchar_t *str, const wchar_t *charset, wchar_t **ptr )    
{
    CHAR_TYPE           *p1;
    const CHAR_TYPE     *p2;
    CHAR_TYPE           tc1;
    CHAR_TYPE           tc2;

    /* if necessary, continue from where we left off */
    if( str == NULL ) {
        if( ptr == NULL ) {
            str = _RWD_nextwtok;
        } else {
            str = *ptr;                 /* use previous value */
        }
        if( str == NULL )
            return( NULL );
    }

    /* skip characters until we reach one not in charset */
    for( ; tc1 = *str; str++ ) {
        for( p2 = charset; tc2 = *p2; p2++ ) {
            if( tc1 == tc2 )
                break;
        }
        if( tc2 == NULLCHAR )
            break;
    }
    if( tc1 == NULLCHAR )
        return( NULL );
    /* skip characters until we reach one in charset */
    for( p1 = str; tc1 = *p1; p1++ ) {
        for( p2 = charset; tc2 = *p2; p2++ ) {
            if( tc1 == tc2 )
                break;
        }
        if( tc2 != NULLCHAR ){
            *p1 = NULLCHAR;         /* terminate the token  */
            p1++;                   /* start of next token  */
            if( ptr == NULL ) {
                _RWD_nextwtok = p1;
            } else {
                *ptr = p1;
            }
            return( str );
        }
    }
    if( ptr == NULL ) {
        _RWD_nextwtok = NULL;
    } else {
        *ptr = NULL;
    }
    
    return( str );
}

#else

_WCRTLINK char *strtok_r( char *str, const char *charset, char **ptr )
{
    char            tc;
    unsigned char   vector[ CHARVECTOR_SIZE ];
    char            *p1;

    /* if necessary, continue from where we left off */
    if( str == NULL ) {
        str = *ptr;            /* use previous value */
        if( str == NULL )
            return( NULL );
    }

    __setbits( vector, charset );
    for( ; (tc = *str) != NULLCHAR; ++str ) {
        /* quit if we find any char not in charset */
        if( GETCHARBIT( vector, tc ) == 0 ) {
            break;
        }
    }
    if( tc == NULLCHAR )
        return( NULL );
    p1 = str;
    for( ; (tc = *p1) != NULLCHAR; ++p1 ) {
        /* quit when we find any char in charset */
        if( GETCHARBIT( vector, tc ) != 0 ) {
            *p1 = NULLCHAR;         /* terminate the token  */
            p1++;                   /* start of next token  */
            *ptr = p1;
            return( str );
        }
    }
    *ptr = NULL;

    return( str );
}

_WCRTLINK char *strtok( char *str, const char *charset )
{
    _INITNEXTTOK
    return strtok_r( str, charset, &(_RWD_nexttok));
}

#endif


