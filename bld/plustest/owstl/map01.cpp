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
* Description:  Tests for std::map
*
****************************************************************************/

#include <iostream>
#include <map>
#include <string>
#include "sanity.cpp"
#include <cstdlib>
#include "allocxtr.hpp"


/* ------------------------------------------------------------------
 * To do:
 *      * user comparator
 *      * other functions not yet implemented in map
 */


/* ------------------------------------------------------------------
 * construct_test( )
 * Construct different maps in different ways.
 */
bool construct_test( )
{
    int i;
    typedef std::map< int, int > mapii_t;
    mapii_t m1;
    std::map< char, char const * > m2;
    std::map< char*, double > m3;
    mapii_t *m4 = new mapii_t();
    
    if( INSANE(m1) || m1.size() || !m1.empty() ) FAIL
    if( INSANE(m2) || m2.size() || !m2.empty() ) FAIL
    if( INSANE(m3) || m3.size() || !m3.empty() ) FAIL
    if( INSANE(*m4) || m4->size() || !m4->empty() ) FAIL
    delete m4;
    
    // test template constructor, note: no default args yet
    typedef mapii_t::value_type v_t;
    v_t init[] = { v_t(0,0), v_t(1,1), v_t(2,4), v_t(3,9) };
    int initsize = sizeof(init)/sizeof(v_t);
    
    mapii_t m5( init, init + initsize, mapii_t::key_compare(), 
                mapii_t::allocator_type() );
    
    if( INSANE(m5) || m5.size() != initsize ) FAIL
    for( i = 0; i< m5.size(); i++ ){
        if( i*i != m5[i] ) FAIL
    }
    
    return( true );
}

/* ------------------------------------------------------------------
 * access_test( )
 * check the tree is working with different inserts / finds / deletes /
 * repeat inserts
 */
bool access_test( )
{
    // todo... choose an array of numbers that excercise all the different
    // interals of insert and delete (eg different tree transforms)
    int num[] = {10,5,3,4,6,2,7,8,11,12,14,13,19,17,16}; 
    int notnum[] = {0,1,9,15,18}; //numbers not in num[]
    int delnum[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
    int totsize = (sizeof(num) + sizeof(notnum)) / sizeof(int);
    int size;
    std::map< int, int > m1;
    
    //insert
    for( int i = 0; i < (sizeof(num) / sizeof(int)); i++){
        std::pair< std::map< int, int >::iterator, bool > ans;
        ans = m1.insert( std::map< int, int >::value_type( num[i], num[i]*num[i] ) );
        if( INSANE( m1 ) || m1.size() != (i+1) || m1.empty() || !ans.second) FAIL
    }
    //find inserted
    for( int i = 0; i < (sizeof(num) / sizeof(int)); i++){
        std::map< int, int >::iterator it = m1.find( num[i] );
        if( INSANE( m1 ) || m1.size() != (sizeof(num) / sizeof(int)) ||
            m1.empty() || it == m1.end() ) FAIL
    }
    //find not inserted
    for( int i = 0; i < (sizeof(notnum) / sizeof(int)); i++){
        std::map< int, int >::iterator it = m1.find( notnum[i] );
        if( INSANE( m1 ) || m1.size() != (sizeof(num) / sizeof(int)) ||
            m1.empty() || it != m1.end() ) FAIL
    }
    //insert again
    for( int i = 0; i < (sizeof(num) / sizeof(int)); i++){
        std::pair< std::map< int, int >::iterator, bool > ans;
        ans = m1.insert( std::map< int, int >::value_type( num[i], -1 ) );
        if( INSANE( m1 ) || m1.size() != (sizeof(num) / sizeof(int)) ||
            m1.empty() || ans.second ) FAIL
    }
    //use subscript to find inserted again: shouldn't have been over-written
    for( int i = 0; i < (sizeof(num) / sizeof(int)); i++){
        if( INSANE( m1 ) || m1.size() != (sizeof(num) / sizeof(int)) ||
            m1.empty() || m1[num[i]] != num[i]*num[i] ) FAIL
    }
    //use subscript to insert
    size = sizeof(num) / sizeof(int);
    for( int i = 0; i < totsize; i++){
        m1[i] = -i;
        for( int j = 0; j < (sizeof(notnum) / sizeof(int)); j++)
            if( notnum[j] == i ) ++size;
        if( INSANE( m1 ) || m1.size() != size || m1.empty() ) FAIL
    }
    //use subscript to find: should have been over-written this time
    for( int i = 0; i < totsize; i++){
        if( INSANE( m1 ) || m1.size() != totsize || m1.empty() ||
            m1[i] != -i ) FAIL
    }
    //delete the elements
    size = totsize;
    for( int i = 0; i < (sizeof(delnum) / sizeof(int)); i++){
        m1.erase( delnum[i] );
        --size;
        if( INSANE( m1 ) || m1.size() != size ) FAIL
    }
    if( !m1.empty() ) FAIL
    //try finding the elements now they are deleted
    size = totsize;
    for( int i = 0; i < totsize; i++){
        std::map< int, int >::iterator it = m1.find( i );
        if( INSANE( m1 ) || m1.size() || !m1.empty() || it != m1.end() ) FAIL
    }
    //--------------------------------
    //test the templated insert method
    //first put a couple of values in to make sure they doesn't get messed up
    typedef std::map<int,int>::value_type v_t;
    m1.insert( v_t(2,4) ); //this one is in the init vector as well
    m1.insert( v_t(4,16) );
    v_t init[] = { v_t(0,0), v_t(1,1), v_t(2,4), v_t(3,9), v_t(5,25) };
    int initsize = sizeof(init)/sizeof(v_t);
    m1.insert( init, init+initsize );
    for( int i = 0; i < m1.size(); i++ ){
        if( INSANE( m1 ) || m1.size()!= 6 || m1.empty() || m1[i] != i*i ) FAIL
    }
    
    return( true );
}
/* ------------------------------------------------------------------
 * string_test( )
 * some quick tests with a more complicated type
 */
bool string_test( )
{
    using namespace std;
    typedef map< string, __int64 > m_t;
    string eejit = "Dan";
    string place = "here";
    m_t &m1 = *(new m_t);
    if( INSANE(m1) || !m1.empty() ) FAIL
    
    m1.insert( m_t::value_type(eejit,0x123456789abcdefUI64) );
    if( INSANE(m1) || m1.empty() || m1.size() != 1) FAIL
    
    m1.insert( m_t::value_type("was",0x123456789abcdef0UI64) );
    if( INSANE(m1) || m1.empty() || m1.size() != 2) FAIL
    
    m1.insert( m_t::value_type(place,0x5a5a5a5a5a5a5a5aUI64) );
    if( INSANE(m1) || m1.empty() || m1.size() != 3) FAIL
    
    __int64 sum = (*m1.find( "Dan" )).second +
                  (*m1.find( "was" )).second + m1["here"];
    if( INSANE(m1) || m1.empty() || m1.size() != 3 ||
        sum != 0x6db1f63a7ec30739ui64) FAIL
    m1.erase( m1.find("Dan") );
    m1.erase( m1.find("was") );
    m1.erase( m1.find("here") );
    if( INSANE(m1) || !m1.empty() || m1.size() ) FAIL
    delete &m1 ;
    return( true );
}

/* ------------------------------------------------------------------
 * torturer( int )
 * Run lots of inserts/deletes so statistically good chance of 
 * finding a problem if haven't covered all possibilities in access test
 */
bool torturer( int maxsize )
{
    using namespace std;
    typedef map< int, int > m_t;
    m_t m;
    bool growing = true;
    int p = 0.9 * RAND_MAX;
    int size = 0;
    int c = 0;
    
    while( growing || m.size() > 0 ){
        if( m.size() > maxsize ){
            growing = false;
            p = 0.1 * RAND_MAX;
        }
        if( (rand() < p) || (size == 0) ){
            int v = rand() + RAND_MAX * rand();
            while( !m.insert( m_t::value_type(v, v) ).second ) v++;
            ++size;
        }else{
            int n = (double)rand() / (double)RAND_MAX * (double)(m.size() - 1);
            m_t::iterator it = m.begin();
            if ( n >= m.size() ) { cout<< "internal err 1"<<n<<" "<<maxsize<<" "<<c<<"\n"; return ( false ); }
            advance(it, n);
            m.erase( it );
            --size;
        }
        if( !heap_ok( "x" ) ) { cout<<"heap err\n"; return false;}
        if( INSANE(m) || m.size() != size ){
            std::cout << "torture count " << c << " maxsize " << maxsize <<
                         " error " << m.mError <<"\n";
            FAIL
        }
        ++c;
    }
    if( INSANE(m) || m.size() || !m.empty() ) FAIL
    return( true );
}
/* ------------------------------------------------------------------
 * torture_test( )
 * _very_ rough timings just to give an idea of how long you have
 * to wait for the test to complete before suspecting it has bust
 * P4 3GHz debug build
 * biggest = 2048   3 sec
 * biggest = 4096   9 sec
 * biggest = 8192   37 sec
 * biggest = 16384  164 sec
 * ~~x^2 time because we run _Sane at every element as it is inserted and 
 * deleted and _Sane runs through every element currently in the tree.
 * Some actual benchmarks should probably be created in the appropriate place.
 */
bool torture_test( )
{
    int const biggest = 2048; //65536
    for( int i = 1; i <= biggest; i *= 2 ){
        if( !torturer( i ) ) return false;
    }
    return( true );
}

/* ------------------------------------------------------------------
 * clear_test( )
 * this was triggering the memory leak detection system due to a 
 * silly mistake added to rbtree when moving from delete to allocator
 */
bool clear_test( )
{
    typedef std::map< int, int > m_t;
    m_t m1;
    int i;
    for( i = 0; i < 2 ; i++ ){
        m1.insert( m_t::value_type( i, i*i ) );
    }
    m1.clear( );
    if( INSANE(m1) || m1.size() || !m1.empty() ) FAIL
        
    m_t m2;
    for( i = 0; i < 2 ; i++ ){
        m2.insert( m_t::value_type( i, i*i ) );
    }// let the destructor clear this one, and trip the leak detector if problem
    return true;
}
/* ------------------------------------------------------------------
 * iterator_test( )
 * Test the iterator functionality
 */
bool iterator_test( )
{
    typedef std::map< int, int > m_t;
    m_t m1;
    m_t::iterator it;
    m_t::const_iterator cit;
    //use nums[] so numbers are not inserted in simple order
    int nums[] = { 2,1,5,6,7,8,4,9,3,0 };   
    const int nums_len = sizeof( nums ) / sizeof( int );
    for( int i = 0; i < nums_len; i++ ){
        m1[nums[i]] = nums[i]*nums[i];
    }
    //test increment and dereferencing ( will be sorted by std::less<int> )
    it = m1.begin( );
    int ans = (*it).first;
    for( int i = 0; i < nums_len ; i++ ){
        if( INSANE( m1 ) || ans != i || it->second != i*i ) FAIL
        it->second = -i;
        if( i%2 ) ans = (*(it++)).first + 1;
        else ans = (*(++it)).first;
    }
    //and again with const iterator
    cit = m1.begin( );
    ans = (*cit).first;
    for( int i = 0; i < nums_len ; i++ ){
        if( INSANE( m1 ) || ans != i || cit->second != -i ) FAIL
        if( i%2 ) ans = (*(cit++)).first + 1;
        else ans = (*(++cit)).first;
    }
    //test decrement ( will be sorted by std::less<int> )
    it = m1.end( );
    for( int i = 9; i > 0 ; i-- ){
        int ans;
        if( i%2 ) ans = (*(--it)).first;
        else ans = (*(it--)).first - 1;
        if( INSANE( m1 ) || ans != i || it->second != -i ) FAIL
        (*it).second = i*i;
    }
    //and again with const iterator
    cit = m1.end( );
    for( int i = 9; i > 0 ; i-- ){
        int ans;
        if( i%2 ) ans = (*(--cit)).first;
        else ans = (*(cit--)).first - 1;
        if( INSANE( m1 ) || ans != i || cit->second != i*i ) FAIL
    }
    
    return( true );
}
/* ------------------------------------------------------------------
 * copy_test( )
 * Test all things plagiarised
 */
bool copy_test( )
{
    typedef std::map< int, int > m_t;
    m_t m1;
    int i;
    for( i=0; i<10; i++ ){
        m1.insert( m_t::value_type( i, -i ) );
    }
    m_t m1cpy( m1 );
    if(  INSANE( m1cpy ) || m1cpy.size( ) != 10 ) FAIL
    //check it copied to new one ok
    for( i = 0; i < 10; i++ ){  
        if( m1cpy[i] != -i ) FAIL
        m1cpy[i] = i*2;
    }
    m1cpy.erase( 1 );
    if(  INSANE( m1cpy ) || m1cpy.size( ) != 9 ) FAIL
    //check it is a copy and old not effecting new one
    if(  INSANE( m1 ) || m1.size( ) != 10 ) FAIL
    for( i = 0; i < 10; i++ ){
        if( m1[i] != -i ) FAIL
    }
    if(  INSANE( m1 ) || m1.size( ) != 10 ) FAIL
    
    m_t m1cpy2 = m1;    //assignment style construct
    if(  INSANE( m1cpy2 ) || m1cpy2.size( ) != 10 ) FAIL
    for( i = 0; i < 10; i++ ){
        if( m1cpy2[i] != -i ) FAIL
    }
    //check assignment
    m1.erase( 5 );
    m1.insert( m_t::value_type( 11, -11 ));
    m1 = m1cpy = m1cpy2;
    if(  INSANE( m1cpy ) || m1cpy.size( ) != 10 ) FAIL
    if(  INSANE( m1 ) || m1.size( ) != 10 ) FAIL
    for( i = 0; i < 10; i++ ){
        if( m1cpy[i] != -i ) FAIL
        if( m1[i] != -i ) FAIL
    }
    
    return( true );
}
/* ------------------------------------------------------------------
 * allocator_test
 * test stateful allocators and exception handling
 */
bool allocator_test( )
{
    typedef std::map< int, int, std::less<int>, LowMemAllocator<int> > map_t;
    LowMemAllocator<int> mem(100);
    mem.SetTripOnAlloc();
    map_t m( map_t::key_compare(), mem );
    bool thrown = false;
    
    //LowMemAllocator is set to trip after 100 allocations
    try{
        for( int i=0; i<101; i++ ){
            m[i];
        }
    }catch( std::bad_alloc const & ){
        mem = m.get_allocator();
        if( mem.GetNumAllocs() != 101 ) FAIL    //should have failed on 101st
        if( INSANE(m) || m.size() != 100 ) FAIL
        thrown = true;
    }
    if( !thrown ) FAIL  //exception should have been thrown
    
    m.clear();
    mem.Reset(100);
    mem.SetTripOnConstruct();
    thrown = false;
    //LowMemAllocator is set to trip after 100 allocations
    try{
        for( int i=0; i<101; i++ ){
            m[i];
        }
    }catch( std::bad_alloc const & ){
        mem = m.get_allocator();
        if( mem.GetNumConstructs() != 101 ) FAIL
        //should have cleaned up last one and left only 100 allocated items
        if( mem.GetNumAllocs() != 101 && mem.GetNumDeallocs() != 1 ) FAIL
        if( INSANE(m) || m.size() != 100 ) FAIL
        thrown = true;
    }
    if( !thrown ) FAIL  //exception should have been thrown
    //if container didn't deal with the exception and clean up the allocated 
    //memory then the leak detector will also trip later

    m.clear();
    mem.Reset(100);
    thrown = false;
    for( int i = 0; i < 70; i++ ){
        m[i];
    }
    //now reset the allocator so it trips at a lower threshold
    //and test the copy mechanism works right
    mem.Reset( 50 );
    mem.SetTripOnAlloc();
    try{
        map_t m2(m);
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
    typedef std::map<int, int> m_t;
    m_t m;
    int i;
    
    for( i = 0; i < 10; i++ ){
        m.insert( m_t::value_type(i, i) );
    }
    m.erase( 5 );
    
    if( m.lower_bound( -1 ) != m.begin() ) FAIL
    if( m.lower_bound( 0 )  != m.begin() ) FAIL
    if( m.lower_bound( 2 )->first  != 2  ) FAIL
    if( m.lower_bound( 4 )->first  != 4  ) FAIL
    if( m.lower_bound( 5 )->first  != 6  ) FAIL
    if( m.lower_bound( 6 )->first  != 6  ) FAIL
    if( m.lower_bound( 9 )->first  != 9  ) FAIL
    if( m.lower_bound( 10 ) != m.end()   ) FAIL
    
    if( m.upper_bound( -1 ) != m.begin() ) FAIL
    if( m.upper_bound( 0 )->first  != 1  ) FAIL
    if( m.upper_bound( 2 )->first  != 3  ) FAIL
    if( m.upper_bound( 4 )->first  != 6  ) FAIL
    if( m.upper_bound( 5 )->first  != 6  ) FAIL
    if( m.upper_bound( 6 )->first  != 7  ) FAIL
    if( m.upper_bound( 8 )->first  != 9  ) FAIL
    if( m.upper_bound( 9 )  != m.end()   ) FAIL
    if( m.upper_bound( 10 ) != m.end()   ) FAIL
        
    if( m.equal_range( 1 ).first   != ++m.begin() ) FAIL
    if( m.equal_range( 1 ).second  != ++(++m.begin()) ) FAIL
    if( m.equal_range( 4 ).first->second  != 4 ) FAIL
    if( m.equal_range( 4 ).second->second != 6 ) FAIL
    if( m.equal_range( 5 ).first->second  != 6 ) FAIL
    if( m.equal_range( 5 ).second->second != 6 ) FAIL
    
    m_t const mc( m );
    if( mc.lower_bound( -1 ) != mc.begin() ) FAIL
    if( mc.lower_bound( 0 )  != mc.begin() ) FAIL
    if( mc.lower_bound( 2 )->first  != 2   ) FAIL
    if( mc.lower_bound( 4 )->first  != 4   ) FAIL
    if( mc.lower_bound( 5 )->first  != 6   ) FAIL
    if( mc.lower_bound( 6 )->first  != 6   ) FAIL
    if( mc.lower_bound( 9 )->first  != 9   ) FAIL
    if( mc.lower_bound( 10 ) != mc.end()   ) FAIL
    
    if( mc.upper_bound( -1 ) != mc.begin() ) FAIL
    if( mc.upper_bound( 0 )->first  != 1   ) FAIL
    if( mc.upper_bound( 2 )->first  != 3   ) FAIL
    if( mc.upper_bound( 4 )->first  != 6   ) FAIL
    if( mc.upper_bound( 5 )->first  != 6   ) FAIL
    if( mc.upper_bound( 6 )->first  != 7   ) FAIL
    if( mc.upper_bound( 8 )->first  != 9   ) FAIL
    if( mc.upper_bound( 9 )  != mc.end()   ) FAIL
    if( mc.upper_bound( 10 ) != mc.end()   ) FAIL
    //m_t::iterator it = mc.upper_bound( 3 );       //illegal
    
    if( mc.equal_range( 1 ).first   != ++mc.begin() ) FAIL
    if( mc.equal_range( 1 ).second  != ++(++mc.begin()) ) FAIL
    if( mc.equal_range( 4 ).first->second  != 4 ) FAIL
    if( mc.equal_range( 4 ).second->second != 6 ) FAIL
    if( mc.equal_range( 5 ).first->second  != 6 ) FAIL
    if( mc.equal_range( 5 ).second->second != 6 ) FAIL
    
    return( true );
}
/* ------------------------------------------------------------------
 * hint_ins_test( )
 * insert, find, erase, count
 */
bool hint_ins_test( )
{
    typedef std::map< int, int > mii_t;
    typedef mii_t::iterator miter_t;
    mii_t m1;
    miter_t it;
    
    //hint insert tests
    typedef std::map< int, int > m_t;
    m1.insert( m1.end(),     m_t::value_type(4,4) );
    m1.insert( m1.end(),     m_t::value_type(7,7) );
    m1.insert( --m1.end(),   m_t::value_type(6,6) );
    m1.insert( m1.end(),     m_t::value_type(8,8) );
    m1.insert( m1.begin(),   m_t::value_type(2,2) );
    m1.insert( ++m1.begin(), m_t::value_type(3,3) );
    m1.insert( m1.find(6),   m_t::value_type(5,5) );
    m1.insert( m1.end(),     m_t::value_type(0,0) ); //invalid hint
    m1.insert( m1.find(0),   m_t::value_type(1,1) ); //invalid hint
    m1.insert( ++m1.begin(), m_t::value_type(9,9) ); //invalid hint
    for( int i = 0; i < 10; i++){
        it = m1.find( i );
        if( INSANE( m1 ) || m1.size() != 10 || m1.empty() ||
            it == m1.end() || it->first != i || it->second != i) FAIL
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
        if( !string_test( )     || !heap_ok( "t3" ) ) rc = 1;
        if( !torture_test( )    || !heap_ok( "t4" ) ) rc = 1;
        if( !clear_test( )      || !heap_ok( "t5" ) ) rc = 1;
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
