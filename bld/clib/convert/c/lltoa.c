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


#include "variety.h"
#include "widechar.h"
#include "watcom.h"
#include "clibi64.h"
#include <stdlib.h>

static const char _WCI86FAR Alphabet[] = "0123456789abcdefghijklmnopqrstuvwxyz";


_WCRTLINK CHAR_TYPE *__F_NAME(__clib_ulltoa,__clib_wulltoa)(
        UINT64_TYPE *pv,
        CHAR_TYPE *buffer,
        unsigned radix )
    {
        CHAR_TYPE *p = buffer;
        char *q;
        UINT64_TYPE value;
        UINT64_TYPE divisor;
        UINT64_TYPE rem;
        unsigned index;
        auto char buf[34];      // only holds ASCII so 'char' is OK

        value = *pv;
        _clib_U32ToU64( radix, divisor );
        buf[0] = '\0';
        q = &buf[1];
        do {
            _clib_U64Div( value, divisor, value, rem );
            _clib_U64ToUInt( rem, index );
            *q = Alphabet[ index ];
            ++q;
        } while( !_clib_U64Zero(value) );
        while( *p++ = (CHAR_TYPE)*--q );
        return( buffer );
    }


_WCRTLINK CHAR_TYPE *__F_NAME(__clib_lltoa,__clib_wlltoa)(
        INT64_TYPE *pv,
        CHAR_TYPE *buffer,
        int radix )
    {
        register CHAR_TYPE *p = buffer;
        UINT64_TYPE value;

        value = *pv;
        if( radix == 10 ) {
            if( _clib_I64Negative(value) ) {
                *p++ = '-';
                _clib_U64Neg( value, value );
            }
        }
        __F_NAME(__clib_ulltoa,__clib_wulltoa)( &value, p, radix );
        return( buffer );
    }
