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


#include <stddef.h>
#include "variety.h"
#include "clibi64.h"

/*
        Be careful of overlap between input parameters and output results.
*/

_WCRTLINK void __U64Div( const UINT64_TYPE *a, const UINT64_TYPE *b,
                UINT64_TYPE *div, UINT64_TYPE *rem )
{
    UINT64_TYPE tmp_a;
    UINT64_TYPE tmp_b;
    int         count;
    unsigned_32 tmp;

    if( a->u._32[I64HI32] == 0 && b->u._32[I64HI32] == 0 ) {
        /* both fit in 32-bit, use hardware divide */
        tmp = a->u._32[I64LO32] / b->u._32[I64LO32];
        if( rem != NULL ) {
            rem->u._32[I64LO32] = a->u._32[I64LO32] % b->u._32[I64LO32];
            rem->u._32[I64HI32] = 0;
        }
        div->u._32[I64LO32] = tmp;
        div->u._32[I64HI32] = 0;
    } else {
        tmp_a = *a;
        tmp_b = *b;
        div->u._32[I64LO32] = 0;
        div->u._32[I64HI32] = 0;
        count = 0;
        for( ;; ) {
            if( tmp_b.u.sign.v ) break;
            if( _clib_U64Cmp( tmp_a, tmp_b ) <= 0 ) break;
            __U64Shift( &tmp_b, -1, &tmp_b );
            ++count;
        }
        while( count >= 0 ) {
            __U64Shift( div, -1, div );
            if( _clib_U64Cmp( tmp_a, tmp_b ) >= 0 ) {
                UINT64_TYPE     tmp_s;
                div->u._32[I64LO32] |= 1;
                _clib_U64Neg( tmp_b, tmp_s );
                __U64Add( &tmp_a, &tmp_s, &tmp_a );
            }
            __U64Shift( &tmp_b, 1, &tmp_b );
            --count;
        }
        if( rem != NULL ) *rem = tmp_a;
    }
}
