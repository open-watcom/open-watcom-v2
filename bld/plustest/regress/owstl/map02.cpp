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

#include <iostream>
#include <string>
#include "sanity.cpp"
#include "allocxtr.hpp"
#include <map>

/* ------------------------------------------------------------------
 * construct_test( )
 * Construct different maps in different ways.
 */
bool construct_test( )
{
    typedef std::multimap< int, int > mmii_t;
    mmii_t m1;
    int i;
    for( i = 0;  i < 10;  i++ ){
        m1.insert( mmii_t::value_type(1, i) );
        m1.insert( mmii_t::value_type(2, 2*i) );
    }
    mmii_t::iterator it = m1.begin();
    for( i = 0; i < 20; i++, ++it){
        //std::cout<< (*it).first << " " << (*it).second << "\n";
    }
    //if(  ) FAIL
    return( true );
}
/* ------------------------------------------------------------------
 * access_test( )
 * insert, find, erase, count
 */
bool access_test( )
{
    typedef std::multimap< int, int > mmii_t;
    typedef mmii_t::iterator mmiter_t;
    mmii_t m1;
    
    int num[] = {11,11,11,10,4,8,8,2,1,12,13,19,7,18,4,2,2,17,11};
    int notnum[] = {3,5,6,9,14,15,16};
    int const num_size = sizeof(num) / sizeof(int);
    int const notnum_size = sizeof(notnum) / sizeof(int);
        
    //insert
    for( int i = 0; i < num_size; i++ ){
        mmiter_t ans;
        ans = m1.insert( mmii_t::value_type( num[i], num[i]*num[i] ) );
        if( INSANE( m1 ) || m1.size() != (i+1) || m1.empty() ) FAIL
    }
    //find inserted
    for( int i = 0; i < num_size; i++ ){
        mmiter_t it = m1.find( num[i] );
        if( INSANE( m1 ) || m1.size() != num_size || m1.empty() ||
            it == m1.end() || (*it).second != num[i]*num[i] ) FAIL
    }
    //find not inserted
    for( int i = 0; i < notnum_size; i++ ){
        mmiter_t it = m1.find( notnum[i] );
        if( INSANE( m1 ) || m1.size() != num_size || m1.empty() ||
            it != m1.end() ) FAIL
    }
    //insert again
    for( int i = 0; i < num_size; i++ ){
        mmiter_t ans;
        ans = m1.insert( mmii_t::value_type( num[i], -1 ) );
        if( INSANE( m1 ) || m1.size() != (num_size+i+1) ||
            m1.empty() ) FAIL
    }
    //couple of spot checks on contents
    if( m1.count( 11 ) != 8 ) FAIL      //count the number of 11's
    mmiter_t it = m1.find( 11 );
    for( int i = 0; i < 4; i++ ){       //first 11's should map to 11^2
        if( INSANE( m1 ) || m1.size() != num_size*2 ||
            m1.empty() || (*it).second != 11*11 ) FAIL
        ++it;
    }
    for( int i = 0; i < 4; i++ ){       //next 11's should map to -1
        if( INSANE( m1 ) || m1.size() != num_size*2 ||
            m1.empty() || (*it).second != -1 ) FAIL
        ++it;
    }
    //erase tests
    //erase first of multiple keys
    m1.erase( m1.find( 11 ) );
    if( INSANE( m1 ) || m1.size() != num_size*2-1 || m1.count( 11 ) != 7 ) FAIL
    //erase last of multiple keys
    m1.erase( --( m1.find( 12 ) ) );
    if( INSANE( m1 ) || m1.size() != num_size*2-2 || m1.count( 11 ) != 6 ) FAIL
    //erase middle of multiple keys
    m1.erase( --(--(--( m1.find( 12 ) ))) );
    if( INSANE( m1 ) || m1.size() != num_size*2-3 || m1.count( 11 ) != 5 ) FAIL
    //erase all keys
    m1.erase( m1.find(12) );
    m1.erase( m1.find(12) );
    if( INSANE( m1 ) || m1.size() != num_size*2-5 || m1.count( 12 ) != 0 ) FAIL
    //erase all keys other method
    if( m1.erase( 2 ) != 6 ) FAIL;
    if( INSANE( m1 ) || m1.size() != num_size*2-11 || m1.count( 2 ) != 0 ) FAIL
    
    return( true );
}
/* ------------------------------------------------------------------
 * iterator_test( )
 * Test the iterator functionality
 */
bool iterator_test( )
{
    typedef std::multimap< int, int > m_t;
    m_t m1;
    m_t::iterator it;
    m_t::const_iterator cit;
    //use nums[] so numbers are not inserted in simple order
    int nums[] = { 2,1,5,6,7,8,4,9,3,0 };   
    for( int i = 0; i < 10; i++ ){
        for( int j = 0; j < 5; j++ ){
            m1.insert( m_t::value_type( nums[i], nums[i]*nums[i] ) );
        }
    }
    //test increment and dereferencing ( will be sorted by std::less<int> )
    it = m1.begin( );
    int ans = (*it).first;
    for( int i = 0; i < 10 ; i++ ){
        for( int j = 0; j < 5; j++ ){
            if( INSANE( m1 ) || ans != i || it->second != i*i ) FAIL
            it->second = -i;
            if( i%2 ) ans = (*(it++)).first + ( (j == 4) ? 1 : 0 );
            else ans = (*(++it)).first;
        }
    }
    //and again with const iterator
    cit = m1.begin( );
    ans = (*cit).first;
    for( int i = 0; i < 10 ; i++ ){
        for( int j = 0; j < 5; j++ ){
            if( INSANE( m1 ) || ans != i || cit->second != -i ) FAIL
            if( i%2 ) ans = (*(cit++)).first + ( (j == 4) ? 1 : 0 );
            else ans = (*(++cit)).first;
        }
    }
    //test decrement ( will be sorted by std::less<int> )
    it = m1.end( );
    for( int i = 9; i > 0 ; i-- ){
        for( int j = 0; j < 5; j++ ){
            if( i%2 ) ans = (*(--it)).first;
            else ans = (*(it--)).first - ( (j == 0) ? 1 : 0 );
            if( INSANE( m1 ) || ans != i || it->second != -i ) FAIL
            (*it).second = i*i;
        }
    }
    //and again with const iterator
    cit = m1.end( );
    for( int i = 9; i > 0 ; i-- ){
        for( int j = 0; j < 5; j++ ){
            if( i%2 ) ans = (*(--cit)).first;
            else ans = (*(cit--)).first - ( (j == 0) ? 1 : 0 );
            if( INSANE( m1 ) || ans != i || cit->second != i*i ) FAIL
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
    typedef std::multimap< int, int > m_t;
    m_t m1;
    int i, j;
    for( i = 0; i < 10; i++ ){
        for( j = 0; j < 10; j++ ){
            m1.insert( m_t::value_type( i, j ) );
        }
    }
    m_t m1cpy( m1 );
    if( INSANE( m1cpy ) || m1cpy.size() != 100 ) FAIL
    //check it copied to new one ok
    for( i = 0; i < 10; i++ ){
        m_t::iterator it = m1cpy.find( i );
        for( j = 0; j < 10; j++, ++it ){
            if( it->first != i && it->second != j ) FAIL
            it->second = -j;
        }
    }
    m1cpy.erase( 1 );
     if( INSANE( m1cpy ) || m1cpy.size() != 90 ) FAIL
    //check it is a copy and old not effecting new one
    if( INSANE( m1 ) || m1.size() != 100 ) FAIL
    for( i = 0; i < 10; i++ ){
        m_t::iterator it = m1.find( i );
        for( j = 0; j < 10; j++, ++it ){
            if( it->first != i && it->second != j ) FAIL
        }
    }
    m_t m1cpy2 = m1;    //assignment style construct
    if( INSANE( m1cpy2 ) || m1cpy2.size() != 100 ) FAIL
    for( i = 0; i < 10; i++ ){
        m_t::iterator it = m1cpy2.find( i );
        for( j = 0; j < 10; j++, ++it ){
            if( it->first != i && it->second != j ) FAIL
        }
    }
    //check assignment
    m1.erase( 5 );
    m1cpy.clear();
    m1.insert( m_t::value_type( 11, -11 ));
    m1 = m1cpy = m1cpy2;
    if( INSANE( m1cpy ) || m1cpy.size() != 100 ) FAIL
    if( INSANE( m1 )    || m1.size()    != 100 ) FAIL
    for( i = 0; i < 10; i++ ){
        m_t::iterator it = m1.find( i );
        m_t::iterator it2 = m1cpy.find( i );
        for( j = 0; j < 10; j++, ++it ){
            if( it->first != i && it->second != j ) FAIL
            if( it->first != i && it->second != j ) FAIL
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
    typedef std::multimap< int, int, std::less<int>, LowMemAllocator<int> > map_t;
    LowMemAllocator<int> mem(200);
    mem.SetTripOnAlloc();
    map_t m( map_t::key_compare(), mem );
    bool thrown = false;

    //LowMemAllocator is set to trip after 200 allocations or 100 inserts
    //(each unique key gets 2 allocations in multimap/set)
    try{
        for( int i=0; i<101; i++ ){
            m.insert( map_t::value_type(i,-i) );
        }
    }catch( std::bad_alloc const & ){
        mem = m.get_allocator();
        if( mem.GetNumAllocs() != 201 ) FAIL    //should have failed on 201st
        if( INSANE(m) || m.size() != 100 ) FAIL
        thrown = true;
    }
    if( !thrown ) FAIL  //exception should have been thrown

    m.clear();
    mem.Reset(200);
    mem.SetTripOnConstruct();
    thrown = false;
    //LowMemAllocator is set to trip after 100 allocations
    try{
        for( int i=0; i<101; i++ ){
            m.insert( map_t::value_type(i,-i) );
        }
    }catch( std::bad_alloc const & ){
        mem = m.get_allocator();
        if( mem.GetNumConstructs() != 201 ) FAIL
        //should have cleaned up last extra alloc
        if( mem.GetNumAllocs() != 201 || mem.GetNumDeallocs() != 1 ) FAIL
        if( INSANE(m) || m.size() != 100 ) FAIL
        thrown = true;
    }
    if( !thrown ) FAIL  //exception should have been thrown
    //if container didn't deal with the exception and clean up the allocated 
    //memory then the leak detector will also trip later
    
    m.clear();
    mem.Reset(140);
    mem.SetTripOnAlloc();
    thrown = false;
    for( int i = 0; i < 70; i++ ){
        m.insert( map_t::value_type(i,-i) );
    }
    //now reset the allocator so it trips at a lower threshold
    //and test the copy mechanism works right
    mem.Reset( 50 );
    mem.SetTripOnAlloc();
    try{
        map_t m2( m );
    }catch( std::bad_alloc ){
        if( mem.GetNumConstructs() != 50 ) FAIL
        if( mem.GetNumAllocs()     != 51 ) FAIL
        if( mem.GetNumDestroys()   != 50 ) FAIL
        if( mem.GetNumDeallocs()   != 50 ) FAIL
        if( INSANE( m ) || m.size() != 70 ) FAIL
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
    typedef std::multimap<int, int> m_t;
    m_t m;
    int i, j;
    
    for( i = 0; i < 10; i++ ){
        for( j = 0; j < 10; j++ ){
            m.insert( m_t::value_type(i, j) );
        }
    }
    m.erase( 5 );
    
    if( m.lower_bound( -1 ) != m.begin() ) FAIL
    if( m.lower_bound( 0 )  != m.begin() ) FAIL
    if( m.lower_bound( 2 )->first  != 2 ||
        m.lower_bound( 2 )->second != 0  ) FAIL
    if( m.lower_bound( 4 )->first  != 4 ||
        m.lower_bound( 4 )->second != 0  ) FAIL
    if( m.lower_bound( 5 )->first  != 6 ||
        m.lower_bound( 5 )->second != 0  ) FAIL
    if( m.lower_bound( 6 )->first  != 6 ||
        m.lower_bound( 6 )->second != 0  ) FAIL
    if( m.lower_bound( 9 )->first  != 9 ||
        m.lower_bound( 9 )->second != 0  ) FAIL
    if( m.lower_bound( 10 ) != m.end()   ) FAIL
    
    if( m.upper_bound( -1 ) != m.begin() ) FAIL
    if( m.upper_bound( 0 )->first  != 1 ||
        m.upper_bound( 0 )->second != 0  ) FAIL
    if( m.upper_bound( 2 )->first  != 3 ||
        m.upper_bound( 2 )->second != 0  ) FAIL
    if( m.upper_bound( 4 )->first  != 6 ||
        m.upper_bound( 4 )->second != 0  ) FAIL
    if( m.upper_bound( 5 )->first  != 6 ||
        m.upper_bound( 5 )->second != 0  ) FAIL
    if( m.upper_bound( 6 )->first  != 7 ||
        m.upper_bound( 6 )->second != 0  ) FAIL
    if( m.upper_bound( 8 )->first  != 9 ||
        m.upper_bound( 8 )->second != 0  ) FAIL
    if( m.upper_bound( 9 )  != m.end()   ) FAIL
    if( m.upper_bound( 10 ) != m.end()   ) FAIL
    
    std::pair< m_t::iterator, m_t::iterator> pit;
    pit = m.equal_range( 4 );
    for( i = 0; pit.first != pit.second; ++i ){
        if( pit.first->first != 4 || pit.first->second != i ) FAIL
        ++pit.first;
    }
    if( i != 10 ) FAIL
    
    m_t const mc( m );
    if( mc.lower_bound( -1 ) != mc.begin() ) FAIL
    if( mc.lower_bound( 0 )  != mc.begin() ) FAIL
    if( mc.lower_bound( 2 )->first  != 2 ||
        mc.lower_bound( 2 )->second != 0   ) FAIL
    if( mc.lower_bound( 4 )->first  != 4 ||
        mc.lower_bound( 4 )->second != 0   ) FAIL
    if( mc.lower_bound( 5 )->first  != 6 ||
        mc.lower_bound( 5 )->second != 0   ) FAIL
    if( mc.lower_bound( 6 )->first  != 6  ||
        mc.lower_bound( 6 )->second != 0   ) FAIL
    if( mc.lower_bound( 9 )->first  != 9  ||
        mc.lower_bound( 9 )->second != 0   ) FAIL
    if( mc.lower_bound( 10 ) != mc.end()   ) FAIL
    
    if( mc.upper_bound( -1 ) != mc.begin() ) FAIL
    if( mc.upper_bound( 0 )->first  != 1 ||
        mc.upper_bound( 0 )->second != 0   ) FAIL
    if( mc.upper_bound( 2 )->first  != 3 ||
        mc.upper_bound( 2 )->second != 0   ) FAIL
    if( mc.upper_bound( 4 )->first  != 6 ||
        mc.upper_bound( 4 )->second != 0   ) FAIL
    if( mc.upper_bound( 5 )->first  != 6 ||
        mc.upper_bound( 5 )->second != 0   ) FAIL
    if( mc.upper_bound( 6 )->first  != 7 ||
        mc.upper_bound( 6 )->second != 0   ) FAIL
    if( mc.upper_bound( 8 )->first  != 9 ||
        mc.upper_bound( 8 )->second != 0   ) FAIL
    if( mc.upper_bound( 9 )  != mc.end()   ) FAIL
    if( mc.upper_bound( 10 ) != mc.end()   ) FAIL
    //m_t::iterator it = mc.upper_bound( 3 );       //illegal
    
    std::pair< m_t::const_iterator, m_t::const_iterator> pcit;
    pcit = mc.equal_range( 4 );
    for( i = 0; pcit.first != pcit.second; ++i ){
        if( pcit.first->first != 4 || pcit.first->second != i ) FAIL
        ++pcit.first;
    }
    if( i != 10 ) FAIL
    
    return( true );
}
/* ------------------------------------------------------------------
 * hint_ins_test( )
 * insert, find, erase, count
 */
bool hint_ins_test( )
{
    typedef std::multimap< int, int > mmii_t;
    typedef mmii_t::iterator mmiter_t;
    mmii_t m1;
    mmiter_t it;
    
    //hint insert tests
    m1.clear();
    m1.insert( m1.end(),     mmii_t::value_type(4,4) );
    m1.insert( m1.end(),     mmii_t::value_type(7,7) );
    m1.insert( --m1.end(),   mmii_t::value_type(6,6) );
    m1.insert( m1.end(),     mmii_t::value_type(8,8) );
    m1.insert( m1.begin(),   mmii_t::value_type(2,2) );
    m1.insert( ++m1.begin(), mmii_t::value_type(3,3) );
    m1.insert( m1.find(6),   mmii_t::value_type(5,5) );
    m1.insert( m1.end(),     mmii_t::value_type(0,0) ); //invalid hint
    m1.insert( m1.find(0),   mmii_t::value_type(1,1) ); //invalid hint
    m1.insert( ++m1.begin(), mmii_t::value_type(9,9) ); //invalid hint
    //mmiter_t itx =m1.begin();
    for( int i = 0; i < 10; i++){
        //std::cout<<i<<", "<<itx->first<<", "<<itx->second<<"\n"; ++itx;
        mmiter_t it = m1.find( i );
        if( INSANE( m1 ) || m1.size() != 10 || m1.empty() ||
            it == m1.end() || it->first != i || it->second != i) FAIL
    }
    //std::cout<<"xxx\n";
    m1.clear();
    mmiter_t it2;
    m1.insert( m1.end(),     mmii_t::value_type(0,0) );
    m1.insert( m1.end(),     mmii_t::value_type(0,1) );
    it  = m1.insert( m1.end(),     mmii_t::value_type(1,1) );
    it2 = m1.insert( m1.find(1),   mmii_t::value_type(1,0) );
    m1.insert( m1.end(),     mmii_t::value_type(1,2) );
    m1.insert( m1.end(),     mmii_t::value_type(2,2) );
    m1.insert( it2,          mmii_t::value_type(0,2) );
    m1.insert( --m1.end(),   mmii_t::value_type(2,1) );
    m1.insert( ++(++it),     mmii_t::value_type(2,0) );
    it = m1.insert( m1.begin(),   mmii_t::value_type(4,2) ); //invalid hint, insert closest
    m1.insert( m1.find(1),   mmii_t::value_type(4,1) ); //invalid hint, insert closest
    m1.insert( it2,          mmii_t::value_type(4,0) ); //invalid hint, insert closest
    m1.insert( ++m1.find(4), mmii_t::value_type(3,0) ); //invalid hint, insert closest
    m1.insert( it,           mmii_t::value_type(3,1) ); //invalid hint, insert closest
    m1.insert( ++m1.find(4), mmii_t::value_type(3,2) ); //invalid hint, insert closest
    
    it = m1.begin();
    for( int i = 0; i < 5; i++){
        for( int j = 0; j < 3; j++){
            //std::cout<<i<<", "<<it->first<<", "<<it->second<<","<<m1.size()<<"\n";
            if( INSANE( m1 ) || m1.size() != 15 || m1.empty() ||
                it == m1.end() || it->first != i || it->second != j) FAIL
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
        if( !allocator_test()   || !heap_ok( "t8" ) ) rc = 1;
        if( !bounds_test()      || !heap_ok( "t9" ) ) rc = 1;
        if( !hint_ins_test()    || !heap_ok( "tA" ) ) rc = 1;
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
