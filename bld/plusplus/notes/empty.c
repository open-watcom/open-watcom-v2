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


#include <stdio.h>
#include <stddef.h>

struct E {
    E();
};
E::E() {
}
struct DE1 : E {
};
struct DE2 : E {
};
struct EE {
    EE();
};
EE::EE() {
}

struct D1 : DE1, DE2 {
    char x,y;
};

struct D2 : DE1, DE2 {
    DE1 x;
    DE2 y;
};

struct D3 : DE1, EE {
    char x,y;
};
struct X32 {
    char a[32];
};

struct D4 : EE, DE1, X32, DE2 {
    DE1 x;
    DE2 y;
};

#define oo( QQ, mm ) \
    printf( "offsetof(" #QQ "," #mm ")= %u\n", offsetof( QQ, mm ) );

#define bo( QQ, bb ) \
    { QQ v; \
    printf( "base offset of " #bb " in " #QQ " = %u\n", ((char*)(bb*)&v) - ((char*)&v) ); \
    }

main() {
    oo( D1, x );
    oo( D1, y );
    bo( D1, DE1 );
    bo( D1, DE2 );
    oo( D2, x );
    oo( D2, y );
    bo( D2, DE1 );
    bo( D2, DE2 );
    oo( D3, x );
    oo( D3, y );
    bo( D3, DE1 );
    bo( D3, EE );
    oo( D4, x );
    oo( D4, y );
    bo( D4, EE );
    bo( D4, DE1 );
    bo( D4, DE2 );
}
