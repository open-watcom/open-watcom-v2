/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2006 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  This file contains the functional tests for the generic
*               iterator support in the library.
*
****************************************************************************/

#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <string>

#include "itcat.h"
#include "sanity.cpp"

// Use a namespace to exercise that ability.
namespace xyz {
    int X, Y;

    // Simple structured iterator gives random access to X or Y.
    class RandomToggleIterator :
        public std::iterator< std::random_access_iterator_tag, int, int > {

    public:
        RandomToggleIterator( bool first_flag )
            { if( first_flag ) isX = true; else isX = false; }

        int &operator*( )
            { if( isX ) return X; else return Y; }

    private:
        bool isX;
    };
}

char raw_array[] = { 'a', 'b', 'c', 'd' };
const char const_raw_array[] = { 'w', 'x', 'y', 'z' };

template< class Iterator >
void exchange( Iterator x, Iterator y )
{
    // Check operation of iterator_traits.
    typename std::iterator_traits< Iterator >::value_type temp;
    temp = *x;
    *x   = *y;
    *y   = temp;
}

template< class Iterator >
bool read_element( Iterator first )
{
    // Used for exercising partial specialization for pointers to const.
    typename std::iterator_traits< Iterator >::value_type temp( *first );
    if( temp != 'w' ) FAIL
        return true;
}

bool traits_test( )
{
    xyz::RandomToggleIterator p1( true );
    xyz::RandomToggleIterator p2( false );

    xyz::X = 1;
    xyz::Y = 2;
    exchange( p1, p2 );
    if( xyz::X != 2 || xyz::Y != 1 ) FAIL;
    exchange( p1, p2 );
    if( xyz::X != 1 || xyz::Y != 2 ) FAIL;

    exchange( raw_array, raw_array + 3 );
    if( raw_array[0] != 'd' || raw_array[3] != 'a' ) FAIL;
    exchange( raw_array, raw_array + 3 );
    if( raw_array[0] != 'a' || raw_array[3] != 'd' ) FAIL;
    read_element( const_raw_array );
    return( true );
}

bool advance_test( )
{
    char raw_array[] = { 'a', 'b', 'c', 'd', 'e' };
    
    // Does it make sense to also check output iterators?
    InpIt< char > inp_it( raw_array );
    FwdIt< char > fwd_it( raw_array );
    BidIt< char > bid_it( raw_array );
    char         *rnd_it( raw_array );
    
    std::advance( inp_it, 1 );
    std::advance( fwd_it, 2 );
    std::advance( bid_it, 3 );
    std::advance( rnd_it, 4 );
    
    if( *inp_it != 'b' ) FAIL;
    if( *fwd_it != 'c' ) FAIL;
    if( *bid_it != 'd' ) FAIL;
    if( *rnd_it != 'e' ) FAIL;
    
    return( true );
}

bool distance_test( )
{
    std::ptrdiff_t d;
    
    d = std::distance( InpIt< char >( raw_array ), InpIt< char >( raw_array ));
    if( d != 0 ) FAIL;
    d = std::distance( InpIt< char >( raw_array ), InpIt< char >( raw_array + 1 ));
    if( d != 1 ) FAIL;    
    d = std::distance( InpIt< char >( raw_array ), InpIt< char >( raw_array + 2 ));
    if( d != 2 ) FAIL;
    
    d = std::distance( FwdIt< char >( raw_array ), FwdIt< char >( raw_array ));
    if( d != 0 ) FAIL;
    d = std::distance( FwdIt< char >( raw_array ), FwdIt< char >( raw_array + 1 ));
    if( d != 1 ) FAIL;
    d = std::distance( FwdIt< char >( raw_array ), FwdIt< char >( raw_array + 2 ));
    if( d != 2 ) FAIL;

    d = std::distance( BidIt< char >( raw_array ), BidIt< char >( raw_array ));
    if( d != 0 ) FAIL;
    d = std::distance( BidIt< char >( raw_array ), BidIt< char >( raw_array + 1 ));
    if( d != 1 ) FAIL;
    d = std::distance( BidIt< char >( raw_array ), BidIt< char >( raw_array + 2 ));
    if( d != 2 ) FAIL;
    
    d = std::distance( raw_array, raw_array );
    if( d != 0 ) FAIL;
    d = std::distance( raw_array, raw_array + 1 );
    if( d != 1 ) FAIL;
    d =  std::distance( const_raw_array, const_raw_array + 2 );
    if( d != 2 ) FAIL;

    return( true );
}


bool reverse_test1( )
{
    int reverse_array[] = { 0, 1, 2, 3 };
    std::reverse_iterator< int * > p1( reverse_array + 4 );
    std::reverse_iterator< int * > p2( reverse_array );
    
    int value = 3;
    while( p1 != p2 ) {
        if( p1 == p2 ) FAIL;
        if( *p1 != value ) FAIL;
        --value;
        ++p1;
    }
    if( value != -1 ) FAIL;
    if( !( p1 == p2 ) ) FAIL;
    
    // Now try it with a more interesting container.
    std::list< int > my_list;
    my_list.push_back( 0 );
    my_list.push_back( 1 );
    my_list.push_back( 2 );
    my_list.push_back( 3 );
    
    std::list< int >::reverse_iterator q1;  // Check default construction.
    std::list< int >::reverse_iterator q2;
    
    q1 = my_list.rbegin( );
    q2 = my_list.rend( );
    
    value = 3;
    while( q1 != q2 ) {
        if( q1 == q2 ) FAIL;
        if( *q1 != value ) FAIL;
        --value;
        ++q1;
    }
    if( value != -1 ) FAIL;
    if( !( q1 == q2 ) ) FAIL;
    
    // Now try using operator->()
    struct wrapper {
        int member;
    };
    wrapper initial[] = { { 0 }, { 1 } , { 2 } , { 3 } };
    std::list< wrapper > my_other_list;
    for( int i = 0; i < sizeof(initial)/sizeof(wrapper); ++i ) {
        my_other_list.push_back( initial[i] );
    }
    
    std::list< wrapper >::reverse_iterator r1;  // Check default construction.
    std::list< wrapper >::reverse_iterator r2;
    
    r1 = my_other_list.rbegin( );
    r2 = my_other_list.rend( );
    
    value = 3;
    while( r1 != r2 ) {
        if( r1 == r2 ) FAIL;
        if( r1->member != value ) FAIL;
        --value;
        ++r1;
    }
    if( value != -1 ) FAIL;
    if( !( r1 == r2 ) ) FAIL;

    return( true );
}

bool reverse_test2( )
{
    int reverse_array[] = { 0, 1, 2, 3 };
    
    std::reverse_iterator< int * > p1( reverse_array + 4 );
    if( p1[0] != 3 ) FAIL;
    if( p1[1] != 2 ) FAIL;
    if( p1[2] != 1 ) FAIL;
    if( p1[3] != 0 ) FAIL;
    
    std::reverse_iterator< int * > p2( p1 + 2 );
    if( p2 - p1 !=  2 ) FAIL;
    if( p1 - p2 != -2 ) FAIL;
    if( p1 >  p2 ) FAIL;
    if( p1 >= p2 ) FAIL;
    if( p2 <  p1 ) FAIL;
    if( p2 <= p1 ) FAIL;
    if( p2[ 0] != 1 ) FAIL;
    if( p2[ 1] != 0 ) FAIL;
    if( p2[-1] != 2 ) FAIL;
    if( p2[-2] != 3 ) FAIL;
    
    p2 = p2 - 2;
    if( p1 != p2 ) FAIL;
    
    p2 += 2;
    if( p2[ 0] != 1 ) FAIL;
    if( p2[ 1] != 0 ) FAIL;
    if( p2[-1] != 2 ) FAIL;
    if( p2[-2] != 3 ) FAIL;
    
    p2 -= 2;
    if( p1 != p2 ) FAIL;
    
    return( true );
}

bool back_inserter_test( )
{
    char raw[] = { 'a', 'b', 'c', 'd' };

    std::string s1( "xyz" );
    std::copy( raw, raw + 4, std::back_inserter( s1 ) );
    if( s1 != "xyzabcd" ) FAIL;
    return( true );
}

bool front_inserter_test( )
{
    int raw [] = { 1, 2, 3, 4 };
    
    std::list< int > my_list;
    std::copy( raw, raw + 4, std::front_inserter( my_list ) );
    if( my_list.size( ) != 4 ) FAIL;
    int value = 4;
    std::list< int >::iterator p = my_list.begin( );
    while( p != my_list.end( ) ) {
        if( *p != value ) FAIL;
        --value;
        ++p;
    }
    return( true );
}

bool inserter_test( )
{
    char raw[] = { 'a', 'b', 'c', 'd' };

    std::string s1( "xyz" );
    std::copy( raw, raw + 4, std::inserter( s1, s1.begin( ) + 1 ) );
    if( s1 != "xabcdyz" ) FAIL;
    std::copy( raw, raw + 4, std::inserter( s1, s1.begin( ) ) );
    if( s1 != "abcdxabcdyz" ) FAIL;
    std::copy( raw, raw + 4, std::inserter( s1, s1.end( ) ) );
    if( s1 != "abcdxabcdyzabcd" ) FAIL;
    return( true );
}

int main( )
{
    int rc = 0;
    int original_count = heap_count( );

    try {
        if( !traits_test( )         || !heap_ok( "t01" )  ) rc = 1;
        if( !advance_test( )        || !heap_ok( "t02" )  ) rc = 1;
        if( !distance_test( )       || !heap_ok( "t03" )  ) rc = 1;
        if( !reverse_test1( )       || !heap_ok( "t04" )  ) rc = 1;
        if( !reverse_test2( )       || !heap_ok( "t05" )  ) rc = 1;
        if( !back_inserter_test( )  || !heap_ok( "t06" )  ) rc = 1;
        if( !front_inserter_test( ) || !heap_ok( "t07" )  ) rc = 1;
        if( !inserter_test( )       || !heap_ok( "t08" )  ) rc = 1;
    }
    catch( ... ) {
        std::cout << "Unexpected exception of unexpected type.\n";
        rc = 1;
    }

    if( heap_count( ) != original_count ) {
        std::cout << "Possible memory leak!\n";
        rc = 1;
    }
    return( rc );
}

