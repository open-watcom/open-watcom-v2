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


// developed by:
//      Erwin Unruh     erwin.unruh@mch.sni.de
//
template <int i>
    struct D {
        D( void * );
        operator int();
    };

template <int p, int i>
    struct is_prime {
        enum {
            prim = (p%i) && is_prime< (i>2?p:0), i-1 >::prim
        };
    };

template <int i>
    struct Prime_print {
        Prime_print<i-1> a;
        enum {
            prim = is_prime< i, i-1 >::prim
        };
        void f() {
            D<i> d = prim;
        }
    };

struct is_prime<0,0> {
    enum {
        prim = 1
    };
};
struct is_prime<0,1> {
    enum {
        prim = 1
    };
};
struct Prime_print<2> {
    enum {
        prim = 1
    };
    void f() {
        D<2> d = prim;
    }
};

void gen_primes()
{
    Prime_print<30> a;
}
