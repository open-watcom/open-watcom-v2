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
#include <stdlib.h>

unsigned long __uldiv( unsigned long, unsigned _WCNEAR * );
#if defined(__386__)
    #pragma aux __uldiv = \
        "xor edx,edx" \
        "div dword ptr [ebx]" \
        "mov [ebx],edx" \
        parm caller [eax] [ebx] \
        modify exact [eax edx] \
        value [eax];
#elif defined(M_I86) && defined(__BIG_DATA__)
    #pragma aux __uldiv = \
        "xor cx,cx" \
        "cmp dx,ss:[bx]" \
        "jb short SMALL_ENOUGH" \
        "xchg ax,dx" \
        "xchg cx,dx" \
        "div word ptr ss:[bx]" \
        "xchg ax,cx" \
        "SMALL_ENOUGH:" \
        "div word ptr ss:[bx]" \
        "mov ss:[bx],dx" \
        "mov dx,cx" \
        parm caller [ax dx] [bx] \
        modify exact [ax cx dx] \
        value [ax dx];
#elif defined(M_I86) && defined(__SMALL_DATA__)
    #pragma aux __uldiv = \
        "xor cx,cx" \
        "cmp dx,[bx]" \
        "jb short SMALL_ENOUGH" \
        "xchg ax,dx" \
        "xchg cx,dx" \
        "div word ptr [bx]" \
        "xchg ax,cx" \
        "SMALL_ENOUGH:" \
        "div word ptr [bx]" \
        "mov [bx],dx" \
        "mov dx,cx" \
        parm caller [ax dx] [bx] \
        modify exact [ax cx dx] \
        value [ax dx];
#elif defined(__AXP__)
    // no pragma
#elif defined(__PPC__)
    // no pragma
#else
    #error missing __uldiv #pragma
#endif

static const char _WCI86FAR Alphabet[] = "0123456789abcdefghijklmnopqrstuvwxyz";


_WCRTLINK CHAR_TYPE *__F_NAME(ultoa,_ultow)( value, buffer, radix )
        unsigned long value;
        CHAR_TYPE *buffer;
        unsigned radix;
    {
        CHAR_TYPE *p = buffer;
        char *q;
        unsigned rem;
        auto char buf[34];      // only holds ASCII so 'char' is OK

        buf[0] = '\0';
        q = &buf[1];
        do {
            #if defined(__AXP__) || defined(__PPC__)
                rem = value % radix;
                value = value / radix;
            #else
                rem = radix;
                value = __uldiv( value, (unsigned _WCNEAR *) &rem );
            #endif
            *q = Alphabet[ rem ];
            ++q;
        } while( value != 0 );
        while( *p++ = (CHAR_TYPE)*--q );
        return( buffer );
    }


_WCRTLINK CHAR_TYPE *__F_NAME(ltoa,_ltow)( value, buffer, radix )
        long value;
        CHAR_TYPE *buffer;
        int radix;
    {
        register CHAR_TYPE *p = buffer;

        if( radix == 10 ) {
            if( value < 0 ) {
                *p++ = '-';
                value = - value;
            }
        }
        __F_NAME(ultoa,_ultow)( value, p, radix );
        return( buffer );
    }
