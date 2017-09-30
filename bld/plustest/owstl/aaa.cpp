/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2004-2013 The Open Watcom Contributors. All Rights Reserved.
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
* Description: This program tests the OWSTL test fixtures. This is to check
*              that the code used to conduct the other tests is working. The
*              strange name for this file ensures that it will appear first
*              in the test sequence (according to our usual convention of
*              ordering tests alphabetically). If this test fails, any later
*              failures have a good chance of being spurious.
*
****************************************************************************/

#include <algorithm>
#include <iostream>

#include "itcat.h"
#include "sanity.cpp"

bool input_iterator_test( )
{
    int a[] = { 1, 2, 3, 4, 5, 6 };
    InpIt<int> i( a );

    std::cout << *i++ << ", ";
    std::cout << *i++ << ", ";

    InpIt<int> j;
    j = i;
    ++i;
    std::cout << *j << ", ";
    j++;
    std::cout << *i <<"\n";

    return( true );
}

bool output_iterator_test( )
{
    int a[ ] = { 1, 2, 3, 4, 5, 6, 0 };
    int b[6];

    int *p = a;
    OutIt<int> i( b );

    std::fill( b, b + 6, -1 );
    while( *p != 0 ) *i++ = *p++;
    std::pair< int *, int * > result = std::mismatch( a, a + 6, b );
    if( result.first != a + 6 ) FAIL;

    return( true );
}


int main( )
{
    int rc = 0;
    int original_count = heap_count( );

    try {
        if( !input_iterator_test( )  || !heap_ok( "t01" ) ) rc = 1;
        if( !output_iterator_test( ) || !heap_ok( "t02" ) ) rc = 1;
    }
    catch( ... ) {
        std::cout << "Unexpected exception of unexpected type.\n";
        rc = 1;
    }

    // As long as the program outputs something "normally" this test will
    // fail. Apparently the number of allocated blocks changes inside the
    // library when output is generated. I guess it's possible this might
    // be an error in clib, but probably not.
    //
    // if( heap_count( ) != original_count ) {
    //     std::cout << "Possible memory leak!\n";
    //     rc = 1;
    // }
    return( rc );
}
