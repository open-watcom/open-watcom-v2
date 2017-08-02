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
* Description:  This file contains the functional tests for std::stack.
*
****************************************************************************/

#include <iostream>
#include <stack>
#include <string>
#include "sanity.cpp"
#include "allocxtr.hpp"

/* ------------------------------------------------------------------
 * access_test
 * test push, pop, top
 */
bool access_test( )
{
    std::stack< int > s;
    int i;
    
    if( s.size() != 0 || !s.empty() ) FAIL
    for( i = 1; i < 101; i++ ){
        s.push( i );
        if( s.size() != i || s.empty() ) FAIL
        if( s.top() != i ) FAIL
    }
    for( i = 100; i > 0; i-- ){
        if( s.top() != i ) FAIL
        if( s.size() != i || s.empty() ) FAIL
        s.pop();
    }
    if( s.size() != 0 || !s.empty() ) FAIL
    return( true );
}
/* ------------------------------------------------------------------
 * copy_test
 * test copy constructor
 */
bool copy_test()
{
    std::stack< std::string > s;
    s.push( "The"  );
    s.push( "quick" );
    s.push( "brown" );
    s.push( "fox" );
    std::stack< std::string > s2( s );
    if( INSANE( s2 ) || s2.size() != 4 ) FAIL
    if( s2.top() != "fox" ) FAIL
    s2.pop();
    if( INSANE( s2 ) || s2.size() != 3 ) FAIL
    if( s2.top() != "brown" ) FAIL
    s2.pop();
    if( INSANE( s2 ) || s2.size() != 2 ) FAIL
    if( s2.top() != "quick" ) FAIL
    s2.pop();
    if( INSANE( s2 ) || s2.size() != 1 ) FAIL
    if( s2.top() != "The" ) FAIL
    s2.pop();
    if( INSANE( s2 ) || s2.size() != 0 || !s2.empty() ) FAIL
    if( INSANE( s ) || s.size() != 4 || s.top() != "fox" ) FAIL

    return ( true );
}

int main( )
{
    int rc = 0;
    int original_count = heap_count( );

    try {
        if( !access_test( ) || !heap_ok( "t01" ) ) rc = 1;
        if( !copy_test( )   || !heap_ok( "t02" ) ) rc = 1;
    }
    catch( ... ) {
        std::cout << "Unexpected exception of unexpected type.\n";
        rc = 1;
    }

    if( heap_count( ) != original_count ) {
        std::cout << "Possible memory leak!\n";
        heap_dump();
        rc = 1;
    }
    return( rc );
}
