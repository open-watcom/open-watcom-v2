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
* Description:  Tests for std::multimap
*
****************************************************************************/

//#include <cstdlib>
#include <iostream>
#include <string>
#include "sanity.cpp"
#include "allocxtr.hpp"
//#include <_rbtree.h>
#include <set>

/* ------------------------------------------------------------------
 * construct_test( )
 * Construct different maps in different ways.
 */
bool construct_test( )
{
    typedef std::multiset< int > msii_t;
    msii_t s1;
    int i;
    for( i = 0;  i < 100;  i++ ){
        s1.insert( 1 );
        s1.insert( 2 );
    }
    return( true );
}
/* ------------------------------------------------------------------
 * access_test( )
 * insert, find, erase, count
 */
bool access_test( )
{
    typedef std::multiset< int > msii_t;
    typedef msii_t::iterator msiter_t;
    msii_t s1;
    
    int num[] = {11,11,11,10,4,8,8,2,1,12,13,19,7,18,4,2,2,17,11};
    int notnum[] = {3,5,6,9,14,15,16};
    int const num_size = sizeof(num) / sizeof(int);
    int const notnum_size = sizeof(notnum) / sizeof(int);
        
    //insert
    for( int i = 0; i < num_size; i++ ){
        msiter_t it;
        it = s1.insert( num[i] );
        if( INSANE( s1 ) || s1.size() != (i+1) || s1.empty() ||
            it == s1.end() || *it != num[i] ) FAIL
    }
    //find inserted
    for( int i = 0; i < num_size; i++ ){
        msiter_t it = s1.find( num[i] );
        if( INSANE( s1 ) || s1.size() != num_size || s1.empty() ||
            it == s1.end() || *it != num[i] ) FAIL
    }
    //find not inserted
    for( int i = 0; i < notnum_size; i++ ){
        msiter_t it = s1.find( notnum[i] );
        if( INSANE( s1 ) || s1.size() != num_size || s1.empty() ||
            it != s1.end() ) FAIL
    }
    //insert again
    for( int i = 0; i < num_size; i++ ){
        msiter_t it;
        it = s1.insert( num[i] );
        if( INSANE( s1 ) || s1.size() != (num_size+i+1) ||
            s1.empty() || it == s1.end() || *it != num[i] ) FAIL
    }
    //spot checks on contents
    if( s1.count( 11 ) != 8 ) FAIL      //count the number of 11's
    msiter_t it = s1.find( 11 );
    for( int i = 0; i < 4; i++ ){
        if( INSANE( s1 ) || s1.size() != num_size*2 ||
            s1.empty() || it == s1.end() || *it != 11 ) FAIL
        ++it;
    }
    //erase tests
    //erase first of multiple keys
    s1.erase( s1.find( 11 ) );
    if( INSANE( s1 ) || s1.size() != num_size*2-1 || s1.count( 11 ) != 7 ) FAIL
    //erase last of multiple keys
    s1.erase( --( s1.find( 12 ) ) );
    if( INSANE( s1 ) || s1.size() != num_size*2-2 || s1.count( 11 ) != 6 ) FAIL
    //erase middle of multiple keys
    s1.erase( --(--(--( s1.find( 12 ) ))) );
    if( INSANE( s1 ) || s1.size() != num_size*2-3 || s1.count( 11 ) != 5 ) FAIL
    //erase all keys
    s1.erase( s1.find(12) );
    s1.erase( s1.find(12) );
    if( INSANE( s1 ) || s1.size() != num_size*2-5 || s1.count( 12 ) != 0 ) FAIL
    //erase all keys other method
    if( s1.erase( 2 ) != 6 ) FAIL;
    if( INSANE( s1 ) || s1.size() != num_size*2-11 || s1.count( 2 ) != 0 ) FAIL
    
    return( true );
}
/* ------------------------------------------------------------------
 * iterator_test( )
 * Test the iterator functionality
 */
bool iterator_test( )
{
    typedef std::multiset< int > s_t;
    s_t s1;
    s_t::iterator it;
    s_t::const_iterator cit;
    //use nums[] so numbers are not inserted in simple order
    int nums[] = { 2,1,5,6,7,8,4,9,3,0 };   
    for( int i = 0; i < 10; i++ ){
        for( int j = 0; j < 5; j++ ){
            s1.insert( nums[i] );
        }
    }
    //test increment and dereferencing ( will be sorted by std::less<int> )
    it = s1.begin( );
    int ans = *it;
    for( int i = 0; i < 10 ; i++ ){
        for( int j = 0; j < 5; j++ ){
            if( INSANE( s1 ) || ans != i || *it != i ) FAIL
            if( i%2 ) ans = *(it++) + ( (j == 4) ? 1 : 0 );
            else ans = *(++it);
        }
    }
    //and again with const iterator
    cit = s1.begin( );
    ans = *cit;
    for( int i = 0; i < 10 ; i++ ){
        for( int j = 0; j < 5; j++ ){
            if( INSANE( s1 ) || ans != i || *cit != i ) FAIL
            if( i%2 ) ans = *(cit++) + ( (j == 4) ? 1 : 0 );
            else ans = *(++cit);
        }
    }
    //test decrement ( will be sorted by std::less<int> )
    it = s1.end( );
    for( int i = 9; i > 0 ; i-- ){
        for( int j = 0; j < 5; j++ ){
            if( i%2 ) ans = *(--it);
            else ans = *(it--) - ( (j == 0) ? 1 : 0 );
            if( INSANE( s1 ) || ans != i || *it != i ) FAIL
        }
    }
    //and again with const iterator
    cit = s1.end( );
    for( int i = 9; i > 0 ; i-- ){
        for( int j = 0; j < 5; j++ ){
            if( i%2 ) ans = *(--cit);
            else ans = *(cit--) - ( (j == 0) ? 1 : 0 );
            if( INSANE( s1 ) || ans != i || *cit != i ) FAIL
        }
    }
    return( true );
}
/* ------------------------------------------------------------------
 * copy_test( )
 * Test all things plagiarised
 */
bool copy_test( )
{
    typedef std::multiset< int > s_t;
    s_t s1;
    int i, j;
    for( i = 0; i < 10; i++ ){
        for( j = 0; j < 10; j++ ){
            s1.insert( i );
        }
    }
    s_t s1cpy( s1 );
    if( INSANE( s1cpy ) || s1cpy.size() != 100 ) FAIL
    //check it copied to new one ok
    for( i = 0; i < 10; i++ ){
        s_t::iterator it = s1cpy.find( i );
        for( j = 0; j < 10; j++, ++it ){
            if( *it != i ) FAIL
        }
    }
    s1cpy.erase( 1 );
    if( INSANE( s1cpy ) || s1cpy.size() != 90 ) FAIL
    //check it is a copy and old not effecting new one
    if( INSANE( s1 ) || s1.size() != 100 ) FAIL
    for( i = 0; i < 10; i++ ){
        s_t::iterator it = s1.find( i );
        for( j = 0; j < 10; j++, ++it ){
            if( *it != i ) FAIL
        }
    }
    //assignment style construct
    s_t s1cpy2 = s1;
    if( INSANE( s1cpy2 ) || s1cpy2.size() != 100 ) FAIL
    for( i = 0; i < 10; i++ ){
        s_t::iterator it = s1cpy2.find( i );
        for( j = 0; j < 10; j++, ++it ){
            if( *it != i ) FAIL
        }
    }
    //check assignment
    s1cpy.clear();
    s1.erase( 5 );
    s1.insert( 11 );
    s1 = s1cpy = s1cpy2;
    if( INSANE( s1cpy ) || s1cpy.size() != 100 ) FAIL
    if( INSANE( s1 )    || s1.size()    != 100 ) FAIL
    for( i = 0; i < 10; i++ ){
        s_t::iterator it = s1.find( i );
        s_t::iterator it2 = s1cpy.find( i );
        for( j = 0; j < 10; j++, ++it ){
            if( *it != i ) FAIL
            if( *it != i ) FAIL
        }
    }
    
    return( true );
}
/* ------------------------------------------------------------------
 * allocator_test
 * test stateful allocators and exception handling
 */
bool allocator_test( )
{
    typedef std::multiset< int, std::less<int>, LowMemAllocator<int> > set_t;
    LowMemAllocator<int> mem(200);
    mem.SetTripOnAlloc();
    set_t s( set_t::key_compare(), mem );
    bool thrown = false;

    //LowMemAllocator is set to trip after 200 allocations or 100 inserts
    //(each unique key gets 2 allocations in multimap/set)
    try{
        for( int i=0; i<101; i++ ){
            s.insert( i );
        }
    }catch( std::bad_alloc const & ){
        mem = s.get_allocator();
        if( mem.GetNumAllocs() != 201 ) FAIL    //should have failed on 201st
        if( INSANE(s) || s.size() != 100 ) FAIL
        thrown = true;
    }
    if( !thrown ) FAIL  //exception should have been thrown

    s.clear();
    mem.Reset(200);
    mem.SetTripOnConstruct();
    thrown = false;
    //LowMemAllocator is set to trip after 100 allocations
    try{
        for( int i=0; i<101; i++ ){
            s.insert( i );
        }
    }catch( std::bad_alloc const & ){
        mem = s.get_allocator();
        if( mem.GetNumConstructs() != 201 ) FAIL
        //should have cleaned up last extra alloc
        if( mem.GetNumAllocs() != 201 || mem.GetNumDeallocs() != 1 ) FAIL
        if( INSANE(s) || s.size() != 100 ) FAIL
        thrown = true;
    }
    if( !thrown ) FAIL  //exception should have been thrown
    //if container didn't deal with the exception and clean up the allocated 
    //memory then the leak detector will also trip later
    
    s.clear();
    mem.Reset(140);
    mem.SetTripOnAlloc();
    thrown = false;
    for( int i = 0; i < 70; i++ ){
        s.insert( i );
    }
    //now reset the allocator so it trips at a lower threshold
    //and test the copy mechanism works right
    mem.Reset( 50 );
    mem.SetTripOnAlloc();
    try{
        set_t s2( s );
    }catch( std::bad_alloc ){
        if( mem.GetNumConstructs() != 50 ) FAIL
        if( mem.GetNumAllocs()     != 51 ) FAIL
        if( mem.GetNumDestroys()   != 50 ) FAIL
        if( mem.GetNumDeallocs()   != 50 ) FAIL
        if( INSANE( s ) || s.size() != 70 ) FAIL
        thrown = true;
    }
    if( !thrown ) FAIL
    
    return( true );
}
/* ------------------------------------------------------------------
 * bounds_test( )
 */
bool bounds_test( )
{
    typedef std::multiset<int> s_t;
    s_t s;
    int i, j;
    
    for( i = 0; i < 10; i++ ){
        for( j = 0; j < 10; j++ ){
            s.insert( i );
        }
    }
    s.erase( 5 );
    
    s_t::iterator it;
    
    if( s.lower_bound( -1 ) != s.begin() )      FAIL
    if( s.lower_bound( 0 )  != s.begin() )      FAIL
    it = s.lower_bound( 2 );
    for( j = 0; j < 10; j++ ) if( *it++ != 2 )  FAIL
    it = s.lower_bound( 4 );
    for( j = 0; j < 10; j++ ) if( *it++ != 4 )  FAIL
    it = s.lower_bound( 5 );
    for( j = 0; j < 10; j++ ) if( *it++ != 6 )  FAIL
    it = s.lower_bound( 6 );
    for( j = 0; j < 10; j++ ) if( *it++ != 6 )  FAIL
    it = s.lower_bound( 9 );
    for( j = 0; j < 10; j++ ) if( *it++ != 9 )  FAIL
    if( s.lower_bound( 10 ) != s.end() )        FAIL
    
    if( s.upper_bound( -1 ) != s.begin() )      FAIL
    it = s.upper_bound( 0 );
    for( j = 0; j < 10; j++ ) if( *it++ != 1 )  FAIL
    it = s.upper_bound( 2 );
    for( j = 0; j < 10; j++ ) if( *--it != 2 )  FAIL
    it = s.upper_bound( 4 );
    for( j = 0; j < 10; j++ ) if( *--it != 4 )  FAIL
    it = s.upper_bound( 5 );
    for( j = 0; j < 10; j++ ) if( *it++ != 6 )  FAIL
    it = s.upper_bound( 6 );
    for( j = 0; j < 10; j++ ) if( *it++ != 7 )  FAIL
    it = s.upper_bound( 8 );
    for( j = 0; j < 10; j++ ) if( *it++ != 9 )  FAIL
    if( s.upper_bound( 9 )  != s.end() )        FAIL
    if( s.upper_bound( 10 ) != s.end() )        FAIL
    
    std::pair< s_t::iterator, s_t::iterator> pit;
    pit = s.equal_range( 4 );
    for( i = 0; pit.first != pit.second; ++i ){
        if( *pit.first != 4 ) FAIL
        ++pit.first;
    }
    if( i != 10 ) FAIL
    
    s_t const sc( s );
    s_t::const_iterator cit;
    
    if( sc.lower_bound( -1 ) != sc.begin() )    FAIL
    if( sc.lower_bound( 0 )  != sc.begin() )    FAIL
    cit = sc.lower_bound( 2 );
    for( j = 0; j < 10; j++ ) if( *cit++ != 2 ) FAIL
    cit = sc.lower_bound( 4 );
    for( j = 0; j < 10; j++ ) if( *cit++ != 4 ) FAIL
    cit = sc.lower_bound( 5 );
    for( j = 0; j < 10; j++ ) if( *cit++ != 6 ) FAIL
    cit = sc.lower_bound( 6 );
    for( j = 0; j < 10; j++ ) if( *cit++ != 6 ) FAIL
    cit = sc.lower_bound( 9 );
    for( j = 0; j < 10; j++ ) if( *cit++ != 9 ) FAIL
    if( sc.lower_bound( 10 ) != sc.end() )      FAIL
    
    if( sc.upper_bound( -1 ) != sc.begin() )    FAIL
    cit = sc.upper_bound( 0 );
    for( j = 0; j < 10; j++ ) if( *cit++ != 1 ) FAIL
    cit = sc.upper_bound( 2 );
    for( j = 0; j < 10; j++ ) if( *--cit != 2 ) FAIL
    cit = sc.upper_bound( 4 );
    for( j = 0; j < 10; j++ ) if( *--cit != 4 ) FAIL
    cit = sc.upper_bound( 5 );
    for( j = 0; j < 10; j++ ) if( *cit++ != 6 ) FAIL
    cit = sc.upper_bound( 6 );
    for( j = 0; j < 10; j++ ) if( *cit++ != 7 ) FAIL
    cit = sc.upper_bound( 8 );
    for( j = 0; j < 10; j++ ) if( *cit++ != 9 ) FAIL
    if( sc.upper_bound( 9 )  != sc.end() )      FAIL
    if( sc.upper_bound( 10 ) != sc.end() )      FAIL
    //it = sc.upper_bound( 3 );       //illegal
    
    std::pair< s_t::const_iterator, s_t::const_iterator> pcit;
    pcit = sc.equal_range( 4 );
    for( i = 0; pcit.first != pcit.second; ++i ){
        if( *pcit.first != 4 ) FAIL
        ++pcit.first;
    }
    if( i != 10 ) FAIL
    
    return( true );
}

/* ------------------------------------------------------------------
 * hint_ins_test( )
 * insert with hints
 */
bool hint_ins_test( )
{
    typedef std::multiset< int > msii_t;
    typedef msii_t::iterator msiter_t;
    msii_t s1;
    msiter_t it;
    
    //hint insert tests
    s1.clear();
    s1.insert( s1.end(),     4 );
    s1.insert( s1.end(),     7 );
    s1.insert( --s1.end(),   6 );
    s1.insert( s1.end(),     8 );
    s1.insert( s1.begin(),   2 );
    s1.insert( ++s1.begin(), 3 );
    s1.insert( s1.find(6),   5 );
    s1.insert( s1.end(),     0 ); //invalid hint
    s1.insert( s1.find(0),   1 ); //invalid hint
    s1.insert( ++s1.begin(), 9 ); //invalid hint
    for( int i = 0; i < 10; i++){
        it = s1.find( i );
        if( INSANE( s1 ) || s1.size() != 10 || s1.empty() ||
            it == s1.end() || *it != i ) FAIL
    }
    s1.clear();
    msiter_t it2;
    s1.insert( s1.end(),     0 );
    s1.insert( s1.end(),     0 );
    it  = s1.insert( s1.end(), 1 );
    it2 = s1.insert( s1.find(1), 1 );
    s1.insert( s1.end(),     1 );
    s1.insert( s1.end(),     2 );
    s1.insert( it2,          0 );
    s1.insert( --s1.end(),   2 );
    s1.insert( ++(++it),     2 );
    it = s1.insert( s1.begin(), 4 ); //invalid hint, insert closest
    s1.insert( s1.find(1),   4 ); //invalid hint, insert closest
    s1.insert( it2,          4 ); //invalid hint, insert closest
    s1.insert( ++s1.find(4), 3 ); //invalid hint, insert closest
    s1.insert( it,           3 ); //invalid hint, insert closest
    s1.insert( ++s1.find(4), 3 ); //invalid hint, insert closest
    
    it = s1.begin();
    for( int i = 0; i < 5; i++){
        for( int j = 0; j < 3; j++){
            if( INSANE( s1 ) || s1.size() != 15 || s1.empty() ||
                it == s1.end() || *it != i ) FAIL
            ++it;
        }
    }
    return( true );
}

int main( )
{
    int rc = 0;
    //heap_dump();
    int original_count = heap_count( );
    
    try {
        if( !construct_test( )  || !heap_ok( "t1" ) ) rc = 1;
        if( !access_test( )     || !heap_ok( "t2" ) ) rc = 1;
        //if( !string_test( )     || !heap_ok( "t3" ) ) rc = 1;
        //if( !torture_test( )    || !heap_ok( "t4" ) ) rc = 1;
        //if( !clear_test( )      || !heap_ok( "t5" ) ) rc = 1;
        if( !iterator_test( )   || !heap_ok( "t6" ) ) rc = 1;
        if( !copy_test( )       || !heap_ok( "t7" ) ) rc = 1;
        if( !allocator_test( )  || !heap_ok( "t8" ) ) rc = 1;
        if( !bounds_test( )     || !heap_ok( "t9" ) ) rc = 1;
        if( !hint_ins_test( )   || !heap_ok( "tA" ) ) rc = 1;
    }
    catch( ... ) {
        std::cout << "Unexpected exception of unexpected type.\n";
        rc = 1;
    }
    int heap_diff = heap_count( ) - original_count;
    if( heap_diff ) {
        heap_dump();
        std::cout << "Possible memory leak! " << heap_diff << " " << original_count << "\n";
        rc = 1;
    }
    
    return( rc );
}
