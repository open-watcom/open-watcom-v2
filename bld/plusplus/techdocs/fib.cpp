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


#include "fail.h"

template <int i>
    struct Fib {
        enum {
            val = Fib<i-1>::val + Fib<i-2>::val
        };
    };

struct Fib<0> {
    enum { val = 1 };
};
struct Fib<1> {
    enum { val = 1 };
};

int main()
{
    Fib<2> x2;
    Fib<3> x3;
    Fib<4> x4;
    Fib<5> x5;
    Fib<6> x6;
    Fib<7> x7;
    Fib<8> x8;
    Fib<9> x9;
    Fib<10> x10;

    if( x2.val != 2 ) fail(__LINE__);
    if( x3.val != 3 ) fail(__LINE__);
    if( x4.val != 5 ) fail(__LINE__);
    if( x5.val != 8 ) fail(__LINE__);
    if( x6.val != 13 ) fail(__LINE__);
    if( x7.val != 21 ) fail(__LINE__);
    if( x8.val != 34 ) fail(__LINE__);
    if( x9.val != 55 ) fail(__LINE__);
    if( x10.val != 89 ) fail(__LINE__);
    _PASS;
}
