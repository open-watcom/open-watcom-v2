/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2008 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  This file contains the functional tests for std::list.
*
****************************************************************************/

// Reverse alphabetical order. It should be possible to include headers in any order.
#include <list>
#include <iostream>
#include <cstdlib>
#include <algorithm>

#include "itcat.h"
#include "sanity.cpp"
#include "allocxtr.hpp"


bool construct_test( )
{
    using namespace std;
    int i;

    // Test ctor( size_t, value )
    list< char > lst1( 99, 'd' );    
    if( INSANE( lst1 ) || lst1.size( ) != 99 || lst1.empty( ) ) FAIL
    for( i = 0; i < 99; i++ ) {
        if( lst1.back( ) != 'd' ) FAIL
        lst1.pop_back( );
    }
    if( INSANE( lst1 ) || lst1.size( ) || !lst1.empty( ) ) FAIL
    
    // Test type dependent ctor
    // ... first when the template parameter is an iterator.
    int x[] = { 1, 2, 3, 4, 5, 6 };
    InpIt< int > p1( x );
    InpIt< int > p2( x + 6 );
    list< int > lst2( p1, p2 );
    if( INSANE( lst2 ) || lst2.size( ) != 6 || lst2.empty( ) ) FAIL
    for( i = 0; i < 6; i++ ) {
        if( lst2.front( ) != i + 1 ) FAIL
        lst2.pop_front( );
    }
    if( INSANE( lst2 ) || lst2.size( ) || !lst2.empty( ) ) FAIL
    
    // Now when the template parameter is an integer. Try sending an allocator instance also.
    list< int > lst3( 77, 88, allocator< int >( ) );
    if( INSANE( lst3 ) || lst3.size( ) != 77 || lst3.empty( ) ) FAIL
    for( i = 0; i < 77; i++ ) {
        if( lst3.front( ) != 88 ) FAIL
        lst3.pop_front( );
    }
    if( INSANE( lst3 ) || lst1.size( ) || !lst3.empty( ) ) FAIL
    
    return( true );
}


bool access_test( )
{
    typedef std::list< int > list_t;
    list_t lst;
    list_t::iterator it;
    int i;

    // Test insert.
    for( i = 0; i < 10; i++ ) {
        lst.insert( lst.begin( ), i );
    }
    for( it = lst.begin( ), i = 9; i >= 0; --i, ++it ) {
        if( INSANE( lst ) || *it != i ) FAIL
    }

    // Test push_front.
    for( i = 10; i < 20 ; i++ ) {
        lst.push_front( i );
    }
    for( it = lst.begin( ), i = 19; i >= 0; --i, ++it ) {
        if( INSANE( lst ) || *it != i ) FAIL
    }

    //Test push_back.
    for( i = -1; i > -11; i-- ) {
        lst.push_back( i );
    }
    for( it = lst.begin( ), i = 19; i >= -10; --i, ++it ) {
        if( INSANE( lst ) || *it != i ) FAIL
    }

    // Test erase.
    it = lst.begin( );
    for( i = 0; i < 30; i += 2 ) {
        lst.erase( it++ );
        ++it;
    }
    for( it = lst.begin( ), i = 18; i >= -10; i -= 2, ++it ) {
        if( INSANE( lst ) || *it != i ) FAIL
    }

    // Test pop_front.
    for( i = 18; i > 8; i -= 2 ) {
        if( lst.front( ) != i ) FAIL
        lst.pop_front( );
    }
    for( it = lst.begin( ), i = 8; i >= -10; i -= 2, ++it ) {
        if( INSANE( lst ) || *it != i ) FAIL
    }

    // Test pop_back.
    for( i = -10; i < 4; i += 2 ) {
        if( lst.back( ) != i ) FAIL
        lst.pop_back( );
    }
    for( it = lst.begin( ), i = 8; i >= 4; i -= 2, ++it ) {
        if( INSANE( lst ) || *it != i ) FAIL
    }
    lst.front( ) = 5;
    lst.back( ) = 7;
    for( it = lst.begin( ), i = 5; i <= 7; ++i, ++it ) {
        if( INSANE( lst ) || *it != i ) FAIL
    }

    // Insert( it, size_type, value )
    lst.clear( );
    lst.insert( lst.begin( ), (list_t::size_type)4, (list_t::value_type)9 );
    for( it = lst.begin( ), i = 0; it != lst.end( ); ++it ) {
        if( INSANE( lst ) || *it != 9 ) FAIL
        i++;
    }
    if( INSANE( lst ) || i != 4 ) FAIL

    // Template insert, int parameters.
    lst.insert( lst.begin( ), 5, 8 ); // Now contains 4 9s and 5 8s.
    for( it = lst.begin( ), i = 0; it != lst.end( ); ++it ){
        i += *it;
    }
    if( INSANE( lst ) || i != 4*9+5*8 ) FAIL
    
    // Template insert, iterator parameters.
    int v[] = { 0, 1, 4, 9, 16 };
    InpIt< int > p1( v );
    InpIt< int > p2( v + sizeof( v ) / sizeof( v[0] ) );
    lst.clear( );
    lst.insert( lst.begin( ), p1, p2 );
    if( INSANE( lst ) || lst.size( ) != 5 ) FAIL
    for( it = lst.begin( ), i = 0; it != lst.end( ); ++it, ++i ){
        if( INSANE( lst ) || *it != i * i ) FAIL
    }
    
    return( true );
}


bool assign_test( )
{
    std::list< int > lst1, lst2;
    
    // Check operator=
    for( int i =  1; i <= 10; ++i ) lst1.push_back( i );
    for( int i = 11; i <= 15; ++i ) lst2.push_back( i );
    lst1 = lst2;
    if( INSANE( lst1 ) || INSANE( lst2 ) ) FAIL
    if( lst1.size( ) != 5 || lst2.size( ) != 5 ) FAIL
    int i = 11;
    std::list< int >::iterator it = lst1.begin( );
    while( it != lst1.end( ) ) {
        if( *it != i ) FAIL
        ++i; ++it;
    }

    // Check assign method (cast types so matches fn, not template fn).
    lst1.assign( (std::list<int>::size_type)10, -1 );
    if( INSANE( lst1 ) || lst1.size( ) != 10 ) FAIL
    for( it = lst1.begin( ); it != lst1.end( ); ++it ) {
        if( *it != -1 ) FAIL
    }
    
    // Template assign, non integer types.
    int x[] = { 50, 51, 52, 53, 54 };
    InpIt< int > p1( x );
    InpIt< int > p2( x + 5 );
    lst2.assign( p1, p2 );
    if( INSANE( lst2 ) || lst2.size( ) != 5 ) FAIL
    for( i = 0; i < 5; i++) {
        if( lst2.front( ) != i + 50 ) FAIL
        lst2.pop_front( );
    }
    if( INSANE( lst2 ) || !lst2.empty( ) ) FAIL
    
    // Template assign, integer types.
    lst2.assign( 20, 50 );
    if( INSANE( lst2 ) || lst2.size( ) != 20 ) FAIL
    for( i = 0; i < 20; i++) {
        if( lst2.front( ) != 50 ) FAIL
        lst2.pop_front( );
    }
    if( INSANE( lst2 ) || !lst2.empty( ) ) FAIL
    
    return( true );
}


bool clear_test()
{
    std::list< int > lst1, lst2, lst3;
    typedef std::list< int > list_t;

    list_t *lstp = new list_t;
    
    for( int i = 0; i < 10; i++ ) {
        lst1.push_front( i );
        lst2.push_front( i );
        lst3.push_front( i );
        lstp->push_front( i );
        if( INSANE( lst1 )  || lst1.front()  != i || lst1.size()  != i + 1 ) FAIL
        if( INSANE( lst2 )  || lst2.front()  != i || lst2.size()  != i + 1 ) FAIL
        if( INSANE( lst3 )  || lst3.front()  != i || lst3.size()  != i + 1 ) FAIL
        if( INSANE( *lstp ) || lstp->front() != i || lstp->size() != i + 1 ) FAIL
    }

    // Test clear
    lst1.clear();
    if( INSANE( lst1 ) || lst1.size() || !lst1.empty() ) FAIL
    lst1.clear();
    if( INSANE( lst1 ) || lst1.size() || !lst1.empty() ) FAIL

    // Test removal
    for( int i = 0; i < 10; i++ ) lst2.pop_front( );
    if( INSANE( lst2 ) || lst2.size( ) || !lst2.empty( ) ) FAIL

    // Test destructor call
    delete lstp;

    // lst3 is cleared automatically 
    // (leak detect will fire if destructor is wrong)
    return( true );
}


bool erase_test( )
{
    using namespace std;  // Try this for something different.

    // Build a list.
    list< int > lst;
    for( int i = 1; i <= 20; ++i ) lst.push_back( i );
    list< int >::iterator it = find( lst.begin( ), lst.end( ), 11 );

    // Do the deed.
    lst.erase( lst.begin( ), it );
    if( INSANE( lst ) || lst.size( ) != 10 ) FAIL
    it = lst.begin( );
    for( int i = 1; i <= 10; ++i ) {
        if( *it != i + 10 ) FAIL
        ++it;
    }

    // Erase everything.
    lst.erase( lst.begin( ), lst.end( ) );
    if( INSANE( lst ) || lst.size( ) != 0 ) FAIL
    return( true );
}


bool swap_test( )
{
    std::list< int > lst_1, lst_2;
    lst_1.push_back( 1 );

    // Try a swap and then check the result.
    lst_1.swap( lst_2 );
    if( INSANE( lst_1 )    || INSANE( lst_2 ) ||
        lst_1.size( ) != 0 || lst_2.size( ) != 1) FAIL
    if( lst_2.front( ) != 1 ) FAIL

    // Add some things to lst_1 and swap again.
    lst_1.push_back( 10 );
    lst_1.push_back( 11 );
    lst_1.swap( lst_2 );
    if( INSANE( lst_1 )    || INSANE( lst_2 ) ||
        lst_1.size( ) != 1 || lst_2.size( ) != 2) FAIL
    if( lst_1.front( ) != 1 ) FAIL
    if( lst_2.front( ) != 10 ) FAIL
    lst_2.pop_front( );
    if( lst_2.front( ) != 11 ) FAIL
    return( true );
}


bool remove_test( )
{
    typedef std::list< int > list_t;
    list_t l;

    // Prepare the list.
    l.push_back( 0 );
    for( int i = 1; i <= 10; i++ ) {
        l.push_back( i );
    }
    l.push_back( 0 );
    l.push_back( 0 );
    for( int i = 11; i <= 20; i++ ) {
        l.push_back( i );
    }
    l.push_back( 0 );

    // Do the deed.
    l.remove( 0 );

    // Did it work?
    if( INSANE( l ) || l.size( ) != 20 ) FAIL
    list_t::iterator it( l.begin( ) );
    for( int i = 1; i <= 20; i++ ) {
        if( *it != i ) FAIL
        ++it;
    }
    return( true );
}


bool iterator_test( )
{
    typedef std::list< int > list_t;
    list_t l;
    list_t::iterator it;
    list_t::const_iterator cit;
    for( int i = 0; i < 20; i++ ) {
        l.push_back( i );
    }

    // Test increment and dereferencing
    it = l.begin( );
    int ans = *it;
    for( int i = 0; i < 20 ; i++ ) {
        if( INSANE( l ) || ans != i || *it != i ) FAIL
        if( i % 2 ) ans = (*(it++)) + 1;
        else ans = *(++it);
    }

    // ...and again with const iterator
    cit = l.begin( );
    ans = *cit;
    for( int i = 0; i < 20 ; i++ ) {
        if( INSANE( l ) || ans != i || *cit != i ) FAIL
        if( i % 2 ) ans = *(cit++) + 1;
        else ans = *(++cit);
    }

    // Test decrement.
    it = l.end( );
    for( int i = 19; i > 0 ; i-- ) {
        int ans;
        if( i % 2 ) ans = *(--it);
        else ans = *(it--) - 1;
        if( INSANE( l ) || ans != i || *it != i ) FAIL
    }

    // ...and again with const iterator
    cit = l.end( );
    for( int i = 19; i > 0 ; i-- ) {
        int ans;
        if( i % 2 ) ans = *(--cit);
        else ans = *(cit--) - 1;
        if( INSANE( l ) || ans != i || *cit != i ) FAIL
    }
    
    return( true );
}


bool reverse_iterator_test( )
{
    std::list< int > lst;
    for( int i = 1; i <= 10; ++i ) lst.push_back( i );

    std::list< int >::reverse_iterator rit = lst.rbegin( );
    for( int i = 10; i >= 1; --i ) {
      if( *rit != i ) FAIL
      ++rit;
    }
    if( rit != lst.rend( ) ) FAIL
    return( true );
}


bool copy_test()
{
    std::list< int > lst1;
    for( int i = 0; i < 20; i++ ) {
        lst1.push_front( -i );
    }
    std::list< int > lst2(lst1);
    if( INSANE( lst1 ) || lst1.size() != 20 ) FAIL
    if( INSANE( lst2 ) || lst2.size() != 20 ) FAIL
    for( int i = 0; i < 20; i++ ) {
        if( lst2.back() != -i ) FAIL
        lst2.pop_back();
    }
    return( true );
}


bool splice_test( )
{
    std::list< int > lst1, lst2;

    // Two trivial (empty) lists
    lst1.splice( lst1.begin( ), lst2 );
    if( INSANE( lst1 )    || INSANE( lst2 )    ||
        lst1.size( ) != 0 || lst2.size( ) != 0 ) FAIL

    // Non-trival list spliced into empty list.
    for( int i = 1; i <= 10; ++i ) lst2.push_back( i );
    lst1.splice( lst1.begin( ), lst2 );
    if( INSANE( lst1 )     || INSANE( lst2 )    ||
        lst1.size( ) != 10 || lst2.size( ) != 0 ) FAIL

    // Two non-trivial lists
    for( int i = 11; i <= 20; ++i ) lst2.push_back( i );
    std::list< int >::iterator it = lst1.begin( );
    ++it; ++it; ++it; ++it; ++it;
    lst1.splice( it, lst2 );
    if( INSANE( lst1 )     || INSANE( lst2 )    ||
        lst1.size( ) != 20 || lst2.size( ) != 0 ) FAIL

    // Check final list contents
    it = lst1.begin( );
    for( int i = 1; i <= 5; ++i ) {
        if( *it != i ) FAIL
        ++it;
    }
    for( int i = 11; i <= 20; ++i ) {
        if( *it != i ) FAIL
        ++it;
    }
    for( int i = 6; i <= 10; ++i ) {
        if( *it != i ) FAIL
        ++it;
    }

    std::list< int > lst3, lst4;
    for( int i = 1; i <= 5; ++i ) lst4.push_back( i );
    it = lst4.begin( ); ++it; ++it;
    lst3.splice( lst3.begin( ), lst4, it );
    if( INSANE( lst3 )    || INSANE( lst4 )    ||
        lst3.size( ) != 1 || lst4.size( ) != 4 ) FAIL
    lst3.splice( lst3.end( ), lst4, lst4.begin( ) );
    if( INSANE( lst3 )    || INSANE( lst4 )    ||
        lst3.size( ) != 2 || lst4.size( ) != 3 ) FAIL
    it = lst4.end( ); --it;
    lst3.splice( ++lst3.begin( ), lst4, it );
    if( INSANE( lst3 )    || INSANE( lst4 )    ||
        lst3.size( ) != 3 || lst4.size( ) != 2 ) FAIL
    lst3.splice( lst3.begin( ), lst3, --lst3.end( ) );
    if( INSANE( lst3 ) || lst3.size( ) != 3 ) FAIL

    // Check final list contents
    it = lst3.begin( );
    if( *it != 1 ) FAIL; ++it;
    if( *it != 3 ) FAIL; ++it;
    if( *it != 5 ) FAIL;

    std::list< int > lst5, lst6;
    for( int i = 1; i <= 10; ++i ) lst6.push_back( i );
    lst5.splice( lst5.begin( ), lst6, lst6.begin( ), lst6.end( ) );
    if( INSANE( lst5 )     || INSANE( lst6 )    ||
        lst5.size( ) != 10 || lst6.size( ) != 0 ) FAIL

    // Check final list contents.
    it = lst5.begin( );
    for( int i = 1; i <= 10; ++i ) {
        if( *it != i ) FAIL
        ++it;
    }

    return( true );
}


bool reverse_test( )
{
    std::list< int > lst;

    // Try a zero sized list.
    lst.reverse( );
    if( INSANE( lst ) || lst.size( ) != 0 ) FAIL

    // Try a non-trivial list.
    for( int i = 1; i <= 10; ++i ) lst.push_back( i );
    lst.reverse( );
    if( INSANE( lst ) || lst.size( ) != 10 ) FAIL

    // Check final list contents
    std::list< int >::iterator it( lst.begin( ) );
    for( int i = 10; i >= 1; --i ) {
        if( *it != i ) FAIL
        ++it;
    }

    // Let's try a reverse iteration too to check out the reverse links.
    std::list< int >::reverse_iterator rit( lst.rbegin( ) );
    for( int i = 1; i <= 10; ++i ) {
        if( *rit != i ) FAIL
        ++rit;
    }
    return( true );
}


struct merge_data {
  int A[11];
  int B[11];
  int R[11];
};
struct merge_data merge_tests[] = {
  { { 0, 2, 4, -1 },
    { 1, 3, 5, -1 },
    { 0, 1, 2, 3, 4, 5, -1 } },

  { { 0, 2, 4, -1 },
    { -1 },
    { 0, 2, 4, -1 } },

  { { -1 },
    { 0, 2, 4, -1 },
    { 0, 2, 4, -1 } },

  { { 1, 3, 5, -1 },
    { 1, 2, -1 },
    { 1, 1, 2, 3, 5, -1 } },

  { { 1, 2, -1 },
    { 1, 3, 5, -1 },
    { 1, 1, 2, 3, 5, -1 } },

  { { 1, 2, -1 },
    { 3, 4, 5, -1 },
    { 1, 2, 3, 4, 5, -1 } },

  { { 3, 4, 5, -1 },
    { 1, 2, -1 },
    { 1, 2, 3, 4, 5, -1 } }
};
const int merge_test_count = sizeof(merge_tests)/sizeof(merge_data);

bool merge_test( )
{
  int *p;

  for( int test_no = 0; test_no < merge_test_count; ++test_no ) {
    std::list< int > lst_1, lst_2;

    // Prepare the two lists and merge them.
    p = merge_tests[test_no].A;
    while( *p != -1 ) { lst_1.push_back( *p ); ++p; }
    p = merge_tests[test_no].B;
    while( *p != -1 ) { lst_2.push_back( *p ); ++p; }
    lst_1.merge( lst_2 );

    // Did it work?
    if( INSANE( lst_1 ) || INSANE( lst_2 ) ) FAIL;
    p = merge_tests[test_no].R;
    while( *p != -1 ) {
      if( lst_1.front( ) != *p ) FAIL;
      lst_1.pop_front( );
      ++p;
    }
  }

  return( true );
}


bool allocator_test( )
{
    typedef std::list< int, LowMemAllocator<int> > list_t;
    LowMemAllocator< int > mem( 100 );
    mem.SetTripOnAlloc( );
    list_t lst( mem );
    bool thrown = false;
    
    // LowMemAllocator is set to trip after 100 allocations
    try {
        for( int i = 0; i<101; i++ ) {
            lst.push_front( i );
        }
    } catch( std::bad_alloc const & ) {
        mem = lst.get_allocator( );
        if( mem.GetNumAllocs( ) != 101 ) FAIL          // Should fail on 101st.
        if( INSANE( lst ) || lst.size( ) != 99 ) FAIL  // One alloc for sentinel
        thrown = true;
    }
    if( !thrown ) FAIL  // Exception should have been thrown
    
    lst.clear();
    mem.Reset( 100 );
    mem.SetTripOnConstruct( );
    thrown = false;

    // LowMemAllocator is set to trip after 100 allocations.
    try {
        for( int i = 0; i < 101; i++ ) {
            lst.push_back( i );
        }
    } catch( std::bad_alloc const & ) {
        mem = lst.get_allocator( );
        if( mem.GetNumConstructs( ) != 101 ) FAIL

        // Should have cleaned up last one and left only 100 allocated items.
        if( mem.GetNumAllocs( ) != 101 || mem.GetNumDeallocs( ) != 1 ) FAIL    
        if( INSANE( lst ) || lst.size( ) != 100 ) FAIL
        thrown = true;
    }
    if( !thrown ) FAIL  // Exception should have been thrown

    // If container didn't deal with the exception and clean up the allocated memory then the
    // leak detector will also trip later.
    
    lst.clear( );
    mem.Reset( 100 );
    thrown = false;
    for( int i = 0; i < 70; i++ ) {
        lst.push_back( i );
    }

    // Now reset the allocator so it trips at a lower threshold and test the copy mechanism
    // works right.
    mem.Reset( 50 );
    mem.SetTripOnAlloc( );
    try {
        list_t lst2( lst );
    } catch( std::bad_alloc ) {
        if( mem.GetNumConstructs( ) != 49 ) FAIL  // Sentinel not constructed
        if( mem.GetNumAllocs( )     != 51 ) FAIL
        if( mem.GetNumDestroys( )   != 49 ) FAIL  // Sentinel not destroyed
        if( mem.GetNumDeallocs( )   != 50 ) FAIL
        if( INSANE( lst ) || lst.size() != 70 ) FAIL
        thrown = true;
    }
    if( !thrown ) FAIL
    
    return( true );
}

// Helper for sort tests check ordered with operator<
template< class T >
bool chk_sorted( T const &lst )
{
    T::const_iterator it, nit;
    it = lst.begin( );
    nit = it;
    ++nit;
    for( ; nit != lst.end( ); ++it, ++nit ) {
        if( *nit < *it ) return( false );
    }
    return( true );
}

// Quick and nasty helper class for sort test used for checking sort is stable
struct MyPair{
    int x;
    int y;
    MyPair( int xx, int yy ) : x( xx ), y( yy ) { }
    MyPair( const MyPair &o ) : x( o.x ), y( o.y ) { }
    bool operator==( const MyPair &o ) { return( x == o.x ); }
};
bool operator<( const MyPair &t, const MyPair &o ) { return( t.x < o.x ); }


bool sort_test( )
{
    typedef class std::list< int > list_t;
    list_t lst;
    
    // Check some special cases
    
    // Empty
    lst.sort( );
    if( INSANE( lst ) || !lst.empty( ) ) FAIL
    
    // Single element
    lst.push_back( 99 );
    lst.sort( );
    if( INSANE( lst ) || lst.size() != 1 || lst.front() != 99 ) FAIL
    lst.clear( );
    
    // try all combinations of 3 numbers
    // 1
    lst.push_back( 1 );
    lst.push_back( 2 );
    lst.push_back( 3 );
    lst.sort( );
    if( INSANE( lst ) || lst.size( ) != 3 || !chk_sorted( lst ) ) FAIL
    lst.clear( );
    // 2
    lst.push_back( 1 ) ;
    lst.push_back( 3 ) ;
    lst.push_back( 2 ) ;
    lst.sort( );
    if( INSANE( lst ) || lst.size( ) != 3 || !chk_sorted( lst ) ) FAIL
    lst.clear( );
    // 3
    lst.push_back( 2 ) ;
    lst.push_back( 1 ) ;
    lst.push_back( 3 ) ;
    lst.sort();
    if( INSANE( lst ) || lst.size( ) != 3 || !chk_sorted( lst ) ) FAIL
    lst.clear();
    // 4
    lst.push_back( 2 ) ;
    lst.push_back( 3 ) ;
    lst.push_back( 1 ) ;
    lst.sort( );
    if( INSANE( lst ) || lst.size( ) != 3 || !chk_sorted( lst ) ) FAIL
    lst.clear( );
    // 5
    lst.push_back( 3 ) ;
    lst.push_back( 1 ) ;
    lst.push_back( 2 ) ;
    lst.sort( );
    if( INSANE( lst ) || lst.size( ) != 3 || !chk_sorted( lst ) ) FAIL
    lst.clear( );
    // 6
    lst.push_back( 3 ) ;
    lst.push_back( 2 ) ;
    lst.push_back( 1 ) ;
    lst.sort( );
    if( INSANE( lst ) || lst.size( ) != 3 || !chk_sorted( lst ) ) FAIL
    lst.clear( );

    // Hit it with a load of random numbers.
    int const rand_size = 16383;
    for( int i = 0; i < rand_size; i++ ) {
        lst.push_back( std::rand( ) );
    }
    lst.sort( );
    if( INSANE( lst ) || lst.size( ) != rand_size || !chk_sorted( lst ) ) FAIL
    lst.clear( );

    // test the version that takes a functor
    for( int i = 0; i < rand_size; i++ ) {
        lst.push_back( std::rand( ) );
    }
    lst.sort( std::greater<int>( ) );
    lst.reverse( );
    if( INSANE( lst ) || lst.size() != rand_size || !chk_sorted( lst ) ) FAIL
    lst.clear( );
    
    // Test sort is stable
    std::list< MyPair > l2;
    int c;
    // build array of random numbers
    for( c = 0; c < 10001; ) {
        int r = std::rand( ) % 15;
        if( r == 7 ){
            // 7s are special, second value ascends and 
            // should stay in that order after sort
            l2.push_back( MyPair( 7, c ) ); 
            c++;
        } else {
            l2.push_back( MyPair( r, 0 ) );
        }
    }
    l2.sort( );
    if( INSANE( l2 ) ) FAIL
    c = 0;
    int last = l2.front( ).x;
    while( !l2.empty( ) ) {

        // Check in sorted order
        if( l2.front( ).x < last ) FAIL 
        last = l2.front( ).x;

        // Check 7s still in order
        if( l2.front( ).x == 7 ){
            if( l2.front( ).y != c ) FAIL
            c++;
        }
        l2.pop_front( );
    }
    
    return( true );
}


bool relational_test( )
{
    // Make these static just to see if it works.
    static int a1[] = { 0 };
    static int a2[] = { 1, 2, 3, 4 };
    static int a3[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    static int a4[] = { 0, 2, 3, 4, 5, 6, 7, 8 };
    static int a5[] = { 1, 0, 3, 4 };

    // Initialize some test lists using the data above.
    std::list< int > x1( a1, a1 + 1 );
    std::list< int > x2( a2, a2 + 4 );
    std::list< int > x3( a3, a3 + 8 );
    std::list< int > x4( a4, a4 + 8 );
    std::list< int > x5( a5, a5 + 4 );

    if( !( x1 == x1 ) ) FAIL;
    if(  ( x1 == x2 ) ) FAIL;
    if(  ( x2 == x3 ) ) FAIL;
    if( !( x3 == x3 ) ) FAIL;

    if(  ( x1 < x1 ) ) FAIL;
    if(  ( x2 < x2 ) ) FAIL;
    if( !( x1 < x2 ) ) FAIL;
    if(  ( x2 < x1 ) ) FAIL;
    if( !( x2 < x3 ) ) FAIL;
    if(  ( x3 < x2 ) ) FAIL;
    if(  ( x3 < x4 ) ) FAIL;
    if( !( x4 < x3 ) ) FAIL;
    if( !( x4 < x5 ) ) FAIL;
    if(  ( x5 < x4 ) ) FAIL;

    // Make sure there are no instantiation errors on the other relational operators.
    if(  ( x1 != x1 ) ) FAIL;
    if(  ( x2 >  x3 ) ) FAIL;
    if(  ( x2 >= x3 ) ) FAIL;
    if(  ( x3 <= x2 ) ) FAIL;
    return( true );
}


int main( )
{
    int rc = 0;
    int original_count = heap_count( );

    try {
        if( !construct_test( )        || !heap_ok( "t01" ) ) rc = 1;
        if( !access_test( )           || !heap_ok( "t02" ) ) rc = 1;
        //if( !string_test( )           || !heap_ok( "t03" ) ) rc = 1;
        //if( !torture_test( )          || !heap_ok( "t04" ) ) rc = 1;
        if( !assign_test( )           || !heap_ok( "t05" ) ) rc = 1;
        if( !clear_test( )            || !heap_ok( "t06" ) ) rc = 1;
        if( !erase_test( )            || !heap_ok( "t07" ) ) rc = 1;
        if( !swap_test( )             || !heap_ok( "t08" ) ) rc = 1;
        if( !remove_test( )           || !heap_ok( "t09" ) ) rc = 1;
        if( !iterator_test( )         || !heap_ok( "t10" ) ) rc = 1;
        if( !reverse_iterator_test( ) || !heap_ok( "t11" ) ) rc = 1;
        if( !copy_test( )             || !heap_ok( "t12" ) ) rc = 1;
        if( !splice_test( )           || !heap_ok( "t13" ) ) rc = 1;
        if( !reverse_test( )          || !heap_ok( "t14" ) ) rc = 1;
        if( !merge_test( )            || !heap_ok( "t15" ) ) rc = 1;
        if( !allocator_test( )        || !heap_ok( "t16" ) ) rc = 1;
        if( !sort_test( )             || !heap_ok( "t17" ) ) rc = 1;
        if( !relational_test( )       || !heap_ok( "t18" ) ) rc = 1;
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
