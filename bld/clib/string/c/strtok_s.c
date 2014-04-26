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
* Description:  Implementation of strtok_s() - bounds-checking strtok().
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include "saferlib.h"
#include <string.h>
#include <wchar.h>
#include "setbits.h"


_WCRTLINK CHAR_TYPE *__F_NAME(strtok_s,wcstok_s)( CHAR_TYPE * __restrict s1,
                rsize_t * __restrict s1max, const CHAR_TYPE * __restrict s2,
                                                 CHAR_TYPE ** __restrict ptr )
/****************************************************************************/
{
#ifdef __WIDECHAR__
    const CHAR_TYPE   *p2;
    CHAR_TYPE         tc2;
#else
    unsigned char     vector[ CHARVECTOR_SIZE ];
#endif
    char              *msg = NULL;
    CHAR_TYPE         *p1 = s1;
    CHAR_TYPE         *str;
    rsize_t           m;
    CHAR_TYPE         tc1;

    // Verify runtime-constraints
    // s1max not NULL
    // s2    not NULL
    // ptr   not NULL
    // *s1max <= RSIZE_MAX
    // if s1 == NULL then *ptr != NULL
    if( __check_constraint_nullptr_msg( msg, s1max ) &&
        __check_constraint_nullptr_msg( msg, s2 ) &&
        __check_constraint_nullptr_msg( msg, ptr ) &&
        __check_constraint_maxsize_msg( msg, *s1max ) &&
       ((s1 != NULL) || __check_constraint_nullptr_msg( msg, *ptr )) ) {

        /* if necessary, continue from where we left off */
        if( s1 == NULL ) {
            p1 = *ptr;                                  /* use previous value */
        }
#ifndef __WIDECHAR__
        __setbits( vector, s2 );
#endif
        m = *s1max;
        for( ; tc1 = *p1; ++p1 ) {
            if( ! ((m == 0) ? ( msg = "no start of token found" ), 0 : 1 ) ) {
               break;                                   /* limit reached, quit */
            }

#ifdef __WIDECHAR__
            for( p2 = s2; tc2 = *p2; p2++ ) {
                if( tc1 == tc2 ) break;
            }
            if( tc2 == NULLCHAR ) break;
#else
            /* quit if we find any char not in charset */
            if( GETCHARBIT( vector, tc1 ) == 0 )  break;
#endif
            --m;
        }

        if( msg == NULL ) {                             /* no rt-constraint violated */
            if( tc1 == NULLCHAR ) return( NULL );       /* no (more) tokens */
        } else {
            /* Now call the handler */
            __rtct_fail( __func__, msg, NULL );
            return( NULL );
        }

        str = p1++;                                     /* start of token */

        for( ; tc1 = *p1; p1++ ) {
            if( ! ((m == 0) ? ( msg = "no closing token delimiter found" ), 0 : 1 ) ) {
               break;                                   /* limit reached, quit */
            }

            --m;

            /* skip characters until we reach one in delimiterset */
#ifdef __WIDECHAR__
            for( p2 = s2; tc2 = *p2; p2++ ) {
                if( tc1 == tc2 ) break;
            }
            if( tc2 != NULLCHAR ) {
#else
            if( GETCHARBIT( vector, tc1 ) != 0 ) {
#endif
                *p1 = NULLCHAR;                         /* terminate the token  */
                p1++;                                   /* start of next token  */
                *ptr = p1;
                *s1max = m - 1;                      /* adjust remaining length */
                return( str );
            }
        }
    }
    if( msg != NULL ) {                                 /* rt-constraint violated */
        /* Now call the handler */
        __rtct_fail( __func__, msg, NULL );
        return( NULL );
    } else {                            /* last token reached */
        *ptr = p1;                      /* point to end of last token */
        *s1max = 0;                     /* remaining length 0 */
    }
    return( str );
}
