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


#pragma aux foo_asm             \
    = "add eax,edx"             \
      "add eax,ebx"             \
    parm[ eax][ edx][ ebx ]     \
    value [eax]                 \
    ;

#pragma aux bar_asm             \
    = "add eax,edx"             \
    parm [eax] [edx ]           \
    value [ eax ]               \
    ;


struct S {
    inline int __pragma("foo_asm") foo( int, int );
    inline static int __pragma("bar_asm") bar( char, char );
    inline int goo( int, int );
    inline static int woo( char, char );
};

int S::goo( int a, int b )
{
    return (int)this + a + b;
}

int S::woo( char a, char b )
{
    return a + b;
}

int value1;
int value2;
int value3;
int value4;

void use_asm( S *p )
{
    value1 = p->foo( 1, 2 );
    value2 = p->bar( 'a', 'b' );
}

//
// following should generate equivalent code to above
//
void use_cpp( S *p )
{
    value3 = p->goo( 1, 2 );
    value4 = p->woo( 'a', 'b' );
}



//
// The following illustrates how to implement member functions with
// alternative names.
//

#pragma aux t_foo_asm "_FOO";
#pragma aux t_bar_asm "_BAR";
struct T {
    void __pragma("t_foo_asm") foo( int, int );
    static void __pragma("t_bar_asm") bar( char, char );
};

void use( T *p )
{
    p->foo( 1, 2 );
    p->bar( 'a', 'b' );
}
