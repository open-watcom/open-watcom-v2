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
* Description:  Profiler test app, Part VIII.
*
****************************************************************************/


#include <stdio.h>
#include <malloc.h>

#include "pt.h"

void f0( void )
{
    void *p;
    void __near *q;

    q = _nmalloc( 20 );
    p = malloc( 10 );
    free( p );
    _nfree( q );
}
void f1( void )
{
}
void f2( void )
{
}
void f3( void )
{
}
void f4( void )
{
}
void f5( void )
{
    void *p;
    void __near *q;

    q = _nmalloc( 20 );
    p = malloc( 10 );
    free( p );
    _nfree( q );
}
void f6( void )
{
}
void f7( void )
{
}
void f8( void )
{
}
void f9( void )
{
}

void show_count( unsigned num )
{
    float    wait;

    fputs( "\r\t\t\t\t\t\t\twait ", stdout );
    printf( "%4.1f seconds \r    ", (float)(num * 15.0) );
    num *= 5;
    num %= 20;
    ++num;
    while( num-- ) {
        printf( "\r ^^^^ \r" );
        for( wait = SIZE; wait > 0.0; wait = wait - 10 ) {
            f0(); f1(); f2(); f3(); f4(); f5(); f6(); f7(); f8(); f9();
            f0(); f1(); f2(); f3(); f4(); f5(); f6(); f7(); f8(); f9();
            f0(); f1(); f2(); f3(); f4(); f5(); f6(); f7(); f8(); f9();
            continue;
        }
        printf( "\r vvvv \r" );
    }
}
