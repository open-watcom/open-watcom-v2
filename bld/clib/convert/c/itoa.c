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
* Description:  Implementation of itoa()/utoa() - convert int to string.
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdlib.h>

extern const char __based(__segname("_CONST")) __Alphabet[];

typedef unsigned __based(__segname("_STACK")) *uint_stk_ptr;

#if defined(__WATCOMC__)
unsigned __udiv( unsigned, uint_stk_ptr );
#if defined(__386__) && defined(__SMALL_DATA__)
    #pragma aux __udiv = \
        "xor edx,edx" \
        "div dword ptr [ebx]" \
        "mov [ebx],eax" \
        parm caller [eax] [ebx] \
        modify exact [eax edx] \
        value [edx];
#elif defined( __386__ ) && defined(__BIG_DATA__)
    #pragma aux __udiv = \
        "xor edx,edx" \
        "div dword ptr ss:[ebx]" \
        "mov ss:[ebx],eax" \
        parm caller [eax] [ebx] \
        modify exact [eax edx] \
        value [edx];
#elif defined( _M_I86 ) && defined(__BIG_DATA__)
    #pragma aux __udiv = \
        "xor dx,dx" \
        "div word ptr ss:[bx]" \
        "mov ss:[bx],ax" \
        parm caller [ax] [bx] \
        modify exact [ax dx] \
        value [dx];
#elif defined( _M_I86 ) && defined(__SMALL_DATA__)
    #pragma aux __udiv = \
        "xor dx,dx" \
        "div word ptr [bx]" \
        "mov [bx],ax" \
        parm caller [ax] [bx] \
        modify exact [ax dx] \
        value [dx];
#endif
#endif /* __WATCOMC__ */

_WCRTLINK CHAR_TYPE *__F_NAME(utoa,_utow)( unsigned value, CHAR_TYPE *buffer, int radix )
{
    CHAR_TYPE   *p = buffer;
    char        *q;
    unsigned    rem;
    unsigned    quot;
    char        buf[34];    // only holds ASCII so 'char' is OK

    buf[0] = '\0';
    q = &buf[1];
    do {
#if defined(_M_IX86) && defined(__WATCOMC__)
        quot = radix;
        rem = __udiv( value, &quot );
#else
        rem = value % radix;
        quot = value / radix;
#endif
        *q = __Alphabet[rem];
        ++q;
        value = quot;
    } while( value != 0 );
    while( (*p++ = (CHAR_TYPE)*--q) )
        ;
    return( buffer );
}


_WCRTLINK CHAR_TYPE *__F_NAME(itoa,_itow)( int value, CHAR_TYPE *buffer, int radix )
{
    CHAR_TYPE   *p = buffer;

    if( radix == 10 ) {
        if( value < 0 ) {
            *p++ = '-';
            value = - value;
        }
    }
    __F_NAME(utoa,_utow)( value, p, radix );
    return( buffer );
}
