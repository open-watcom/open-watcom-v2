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
* Description:  Tests for std::set
*
****************************************************************************/

#include <iostream>
#include <set>
#include <string>
#include "sanity.cpp"
#include <cstdlib>
#include "allocxtr.hpp"

bool construct_test( )
{
    int i;
    typedef std::set< int > seti_t;
    seti_t s1;
    std::set< char > s2;                // to do: construct with different alloc/comp
    std::set< char* > s3;
    seti_t *s4 = new seti_t();
    
    if( INSANE(s1) || s1.size() || !s1.empty() ) FAIL
    if( INSANE(s2) || s2.size() || !s2.empty() ) FAIL
    if( INSANE(s3) || s3.size() || !s3.empty() ) FAIL
    if( INSANE(*s4) || s4->size() || !s4->empty() ) FAIL
    delete s4;
    
    // test template constructor, note: no default args yet
    int init[] = { 1, 2, 3, 4 };
    int initsize = sizeof(init)/sizeof(int);
    
    seti_t s5( init, init + initsize, seti_t::key_compare(), 
               seti_t::allocator_type() );
    
    if( INSANE(s5) || s5.size() != initsize ) FAIL
    for( i = 0; i < s5.size(); i++ ){
        if( s5.find( init[i] ) == s5.end() ) FAIL
    }
    
    return( true );
}

bool access_test( )
{
    // todo... choose an array of numbers that excercise all the different
    // interals of insert and delete (eg different tree transforms)
    int num[] = {10,5,3,4,6,2,7,8,11,12,14,13,19,17,16}; 
    int notnum[] = {0,1,9,15,18}; //numbers not in num[]
    int delnum[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
    int totsize = (sizeof(num) + sizeof(notnum)) / sizeof(int);
    int size;
    std::set< int > s1;
    
    //insert
    for( int i = 0; i < (sizeof( num ) / sizeof( int )); i++ ){
        std::pair< std::set< int >::iterator, bool > ans;
        ans = s1.insert( num[i] );
        if( INSANE( s1 ) || s1.size() != (i+1) || s1.empty() || !ans.second ) FAIL
    }
    //find inserted
    for( int i = 0; i < (sizeof(num) / sizeof(int)); i++){
        std::set< int >::iterator it = s1.find( num[i] );
        if( INSANE( s1 ) || s1.size() != (sizeof( num ) / sizeof( int )) ||
            s1.empty() || it == s1.end() ) FAIL
    }
    //find not inserted
    for( int i = 0; i < (sizeof(notnum) / sizeof(int)); i++){
        std::set< int >::iterator it = s1.find( notnum[i] );
        if( INSANE( s1 ) || s1.size() != (sizeof( num ) / sizeof( int )) ||
            s1.empty() || it != s1.end() ) FAIL
    }
    //insert again
    for( int i = 0; i < (sizeof(num) / sizeof(int)); i++){
        std::pair< std::set< int >::iterator, bool > ans;
        ans = s1.insert( num[i] );
        if( INSANE( s1 ) || s1.size() != (sizeof(num) / sizeof(int)) ||
            s1.empty() || ans.second ) FAIL
    }
    //insert all
    size = sizeof( num ) / sizeof( int );
    for( int i = 0; i < totsize; i++){
        std::pair< std::set< int >::iterator, bool > ans;
        ans = s1.insert( i );
        bool should_have_inserted = false;
        for( int j = 0; j < (sizeof( notnum ) / sizeof( int )); j++)
            if( notnum[j] == i ) should_have_inserted = true;
        if( should_have_inserted ){
            ++size;
            if( !ans.second ) FAIL
        }else{ // shouldn't have inserted
            if( ans.second ) FAIL
        }
        if( INSANE( s1 ) || s1.size() != size || s1.empty() ) FAIL
    }
    //delete the elements
    size = totsize;
    for( int i = 0; i < (sizeof( delnum ) / sizeof( int )); i++){
        s1.erase( delnum[i] );
        --size;
        if( INSANE( s1 ) || s1.size() != size ) FAIL
    }
    if( !s1.empty() ) FAIL
    //try finding the elements now they are deleted
    size = totsize;
    for( int i = 0; i < totsize; i++){
        std::set< int >::iterator it = s1.find( i );
        if( INSANE( s1 ) || s1.size() || !s1.empty() || it != s1.end() ) FAIL
    }
    //--------------------------------
    //test the templated insert method
    //first put a couple of values in to make sure they doesn't get messed up
    s1.insert( 4 );         //this one is in the init vector as well
    s1.insert( 29 );
    int init[] = { 1, 2, 4, 7, 11, 16, 22 };
    s1.insert( init, init+7 );
    for( int j = 1, i = 1; i < s1.size()+1; i++ ){
        std::set< int >::iterator it = s1.find( j );
        if( INSANE( s1 ) || s1.size()!= 8 || s1.empty() || it == s1.end() || *it != j ) FAIL
        j += i;
    }
    
    return( true );
}

/* ------------------------------------------------------------------
 * some quick tests with a more complicated type
 */
bool string_test( )
{
    using namespace std;
    typedef set< string > s_t;
    string eejit = "Dan";
    string place = "here";
    s_t &s1 = *(new s_t);

    if( INSANE(s1) || !s1.empty() ) FAIL
    s1.insert( eejit );
    if( INSANE(s1) || s1.empty() || s1.size() != 1) FAIL
    s1.insert( "here" );
    if( INSANE(s1) || s1.empty() || s1.size() != 2) FAIL
    s1.insert( place );
    if( INSANE(s1) || s1.empty() || s1.size() != 2) FAIL
    
    if( s1.find( "Dan" ) == s1.end() || s1.find( "here" ) == s1.end() ||
        *s1.find( eejit ) != "Dan" || *s1.find( place ) != place ||
        INSANE(s1) || s1.empty() || s1.size() != 2 ) FAIL
    s1.erase( s1.find("Dan") );
    s1.erase( "was" );
    s1.erase( "here" );
    if( INSANE(s1) || !s1.empty() || s1.size() ) FAIL
    delete &s1;
    return( true );
}

/* ------------------------------------------------------------------
 * Run lots of inserts/deletes so statistically good chance of 
 * finding a problem if haven't covered all possibilities in access test
 */
bool torturer( int maxsize )
{
    using namespace std;
    typedef set< int > m_t;
    m_t s;
    bool growing = true;
    int p = 0.9 * RAND_MAX;
    int size = 0;
    int c = 0;
    
    while( growing || s.size() > 0 ){
        if( s.size() > maxsize ){
            growing = false;
            p = 0.1 * RAND_MAX;
        }
        if( (rand() < p) || (size == 0) ){
            int v = rand() + RAND_MAX * rand();
            while( !s.insert( v ).second ) v++;
            ++size;
        }else{
            int n = (double)rand() / (double)RAND_MAX * (double)(s.size() - 1);
            m_t::iterator it = s.begin();
            if ( n >= s.size() ) { cout<< "internal err 1 "<<n<<" "<<maxsize<<" "<<c<<"\n"; return ( false ); }
            advance(it, n);
            s.erase( it );
            --size;
        }
        if( !heap_ok( "x" ) ) { cout<<"heap err\n"; return false;}
        if( INSANE(s) || s.size() != size ){
            std::cout << "torture failure 0001 count " << c << 
                         " maxsize " << maxsize << " error " << s.mError <<"\n";
            FAIL
        }
        ++c;
    }
    if( INSANE(s) || s.size() || !s.empty() ) FAIL
    
    return( true );
}
/* ------------------------------------------------------------------
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
    // no point in this test really as the base RedBlackTree is 
    // tested under map
    int const biggest = 256; 
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
    typedef std::set< int > s_t;
    s_t s1;
    int i;
    for( i = 0; i < 2 ; i++ ){
        s1.insert( i );
    }
    s1.clear( );
    if( INSANE(s1) || s1.size() || !s1.empty() ) FAIL
    //check clearing a empty container
    s1.clear( );
    if( INSANE(s1) || s1.size() || !s1.empty() ) FAIL
    
    s_t s2;
    for( i = 0; i < 2 ; i++ ){
        s2.insert( i );
    }// let the destructor clear this one, and trip the leak detector if problem
    return true;
}


/* ------------------------------------------------------------------
 * iterator_test( )
 * Test the iterator functionality
 */
bool iterator_test( )
{
    typedef std::set< int > s_t;
    s_t s1;
    s_t::iterator it;
    s_t::const_iterator cit;
    //use nums[] so numbers are not inserted in simple order
    int nums[] = { 2,1,5,6,7,8,4,9,3,0 };   
    const int nums_len = sizeof( nums ) / sizeof( int );
    for( int i = 0; i < nums_len; i++ ){
        s1.insert( nums[i] );
    }
    //test increment and dereferencing ( will be sorted by std::less<int> )
    it = s1.begin();
    int ans = *it;
    for( int i = 0; i < nums_len ; i++ ){
        if( INSANE( s1 ) || ans != i || *it != i ) FAIL
        if( i%2 ) ans = *(it++) + 1;
        else ans = *(++it);
    }
    //and again with const iterator
    cit = s1.begin();
    ans = *cit;
    for( int i = 0; i < nums_len ; i++ ){
        if( INSANE( s1 ) || ans != i || *cit != i ) FAIL
        if( i%2 ) ans = *(cit++) + 1;
        else ans = *(++cit);
    }
    //test decrement ( will be sorted by std::less<int> )
    it = s1.end();
    for( int i = 9; i > 0 ; i-- ){
        int ans;
        if( i%2 ) ans = *(--it);
        else ans = *(it--) - 1;
        if( INSANE( s1 ) || ans != i || *it != i ) FAIL
    }
    //and again with const iterator
    cit = s1.end( );
    for( int i = 9; i > 0 ; i-- ){
        int ans;
        if( i%2 ) ans = *(--cit);
        else ans = *(cit--) - 1;
        if( INSANE( s1 ) || ans != i || *cit != i ) FAIL
    }
    return( true );
}
/* ------------------------------------------------------------------
 * copy_test( )
 * Test all things plagiarised
 */
bool copy_test( )
{
    typedef std::set< int > s_t;
    s_t s1;
    int i;
    for( i=0; i<10; i++ ){
        s1.insert( i );
    }
    s_t s1cpy( s1 );
    if( INSANE( s1cpy ) || s1cpy.size( ) != 10 ) FAIL
    //check it copied to new one ok
    for( i = 0; i < 10; i++ ){  
        if( s1cpy.find( i ) == s1cpy.end() ) FAIL
    }
    s1cpy.erase( 1 );
    s1cpy.erase( 0 );
    s1cpy.insert( 1979 );
    if( INSANE( s1cpy ) || s1cpy.size( ) != 9 ) FAIL
    //check it is a copy and old not effecting new one
    if( INSANE( s1 ) || s1.size( ) != 10 ) FAIL
    for( i = 0; i < 10; i++ ){
        if( s1.find( i ) == s1.end() ) FAIL
    }
    //assignment style
    s_t s1cpy2 = s1;
    if(  INSANE( s1cpy2 ) || s1cpy2.size( ) != 10 ) FAIL
    for( i = 0; i < 10; i++ ){
        if( s1cpy2.find( i ) == s1cpy2.end() ) FAIL
    }
    //check assignment
    s1.erase( 5 );
    s1.insert( 11 );
    s1 = s1cpy = s1cpy2;
    if( INSANE( s1cpy ) || s1cpy.size( ) != 10 ) FAIL
    if( INSANE( s1 ) || s1.size( ) != 10 ) FAIL
    for( i = 0; i < 10; i++ ){
        if( s1cpy.find( i ) == s1cpy.end() ) FAIL
        if( s1.find( i ) == s1.end() ) FAIL
    }
    
    return( true );
}
/* ------------------------------------------------------------------
 * allocator_test
 * test stateful allocators and exception handling
 */
bool allocator_test( )
{
    typedef std::set< int, std::less<int>, LowMemAllocator<int> > set_t;
    LowMemAllocator<int> mem(100);
    set_t s( set_t::key_compare(), mem );

    mem.SetTripOnAlloc();
    bool thrown = false;
    //LowMemAllocator is set to trip after 100 allocations
    try{
        for( int i=0; i<101; i++ ){
            s.insert(i);
        }
    }catch( std::bad_alloc const & ){
        mem = s.get_allocator();
        if( mem.GetNumAllocs() != 101 ) FAIL    //should have failed on 101st
        if( INSANE(s) || s.size() != 100 ) FAIL
        thrown = true;
    }
    if( !thrown ) FAIL  //exception should have been thrown
    
    s.clear();
    mem.Reset(100);
    mem.SetTripOnConstruct();
    thrown = false;
    //LowMemAllocator is set to trip after 100 constructs
    try{
        for( int i=0; i<101; i++ ){
            s.insert(i);
        }
    }catch( std::bad_alloc const & ){
        mem = s.get_allocator();
        if( mem.GetNumConstructs() != 101 ) FAIL
        //should have cleaned up last one and left only 100 allocated items
        if( mem.GetNumAllocs() != 101 && mem.GetNumDeallocs() != 1 ) FAIL
        if( INSANE(s) || s.size() != 100 ) FAIL
        thrown = true;
    }
    if( !thrown ) FAIL  //exception should have been thrown
    //if container didn't deal with the exception and clean up the allocated 
    //memory then the leak detector will also trip later
    
    s.clear();
    mem.Reset( 100 );
    thrown = false;
    for( int i = 0; i < 70; i++ ){
        s.insert(i);
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
    typedef std::set<int> s_t;
    s_t s;
    int i;
    
    for( i = 0; i < 10; i++ ){
        s.insert( i );
    }
    s.erase( 5 );
    
    if( s.lower_bound( -1 ) != s.begin() ) FAIL
    if( s.lower_bound( 0 )  != s.begin() ) FAIL
    if( *s.lower_bound( 2 ) != 2         ) FAIL
    if( *s.lower_bound( 4 ) != 4         ) FAIL
    if( *s.lower_bound( 5 ) != 6         ) FAIL
    if( *s.lower_bound( 6 ) != 6         ) FAIL
    if( *s.lower_bound( 9 ) != 9         ) FAIL
    if( s.lower_bound( 10 ) != s.end()   ) FAIL
    
    if( s.upper_bound( -1 ) != s.begin() ) FAIL
    if( *s.upper_bound( 0 ) != 1         ) FAIL
    if( *s.upper_bound( 2 ) != 3         ) FAIL
    if( *s.upper_bound( 4 ) != 6         ) FAIL
    if( *s.upper_bound( 5 ) != 6         ) FAIL
    if( *s.upper_bound( 6 ) != 7         ) FAIL
    if( *s.upper_bound( 8 ) != 9         ) FAIL
    if( s.upper_bound( 9 )  != s.end()   ) FAIL
    if( s.upper_bound( 10 ) != s.end()   ) FAIL
    
    if( s.equal_range( 1 ).first   != ++s.begin() ) FAIL
    if( s.equal_range( 1 ).second  != ++(++s.begin()) ) FAIL
    if( *s.equal_range( 4 ).first  != 4 ) FAIL
    if( *s.equal_range( 4 ).second != 6 ) FAIL
    if( *s.equal_range( 5 ).first  != 6 ) FAIL
    if( *s.equal_range( 5 ).second != 6 ) FAIL
    
    s_t const sc( s );
    if( sc.lower_bound( -1 ) != sc.begin() ) FAIL
    if( sc.lower_bound( 0 )  != sc.begin() ) FAIL
    if( *sc.lower_bound( 2 ) != 2          ) FAIL
    if( *sc.lower_bound( 4 ) != 4          ) FAIL
    if( *sc.lower_bound( 5 ) != 6          ) FAIL
    if( *sc.lower_bound( 6 ) != 6          ) FAIL
    if( *sc.lower_bound( 9 ) != 9          ) FAIL
    if( sc.lower_bound( 10 ) != sc.end()   ) FAIL
    
    if( sc.upper_bound( -1 ) != sc.begin() ) FAIL
    if( *sc.upper_bound( 0 ) != 1          ) FAIL
    if( *sc.upper_bound( 2 ) != 3          ) FAIL
    if( *sc.upper_bound( 4 ) != 6          ) FAIL
    if( *sc.upper_bound( 5 ) != 6          ) FAIL
    if( *sc.upper_bound( 6 ) != 7          ) FAIL
    if( *sc.upper_bound( 8 ) != 9          ) FAIL
    if( sc.upper_bound( 9 )  != sc.end()   ) FAIL
    if( sc.upper_bound( 10 ) != sc.end()   ) FAIL
    //m_t::iterator it = sc.upper_bound( 3 );       //illegal
    
    if( sc.equal_range( 1 ).first   != ++sc.begin() ) FAIL
    if( sc.equal_range( 1 ).second  != ++(++sc.begin()) ) FAIL
    if( *sc.equal_range( 4 ).first  != 4 ) FAIL
    if( *sc.equal_range( 4 ).second != 6 ) FAIL
    if( *sc.equal_range( 5 ).first  != 6 ) FAIL
    if( *sc.equal_range( 5 ).second != 6 ) FAIL
    
    
    return( true );
}
/* ------------------------------------------------------------------
 * hint_ins_test( )
 * insert, find, erase, count
 */
bool hint_ins_test( )
{
    typedef std::multiset< int > s_t;
    typedef s_t::iterator siter_t;
    s_t s1;
    siter_t it;
    
    //hint insert tests
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
    
    return( true );
}

int main( )
{
    int rc = 0;
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

    if( heap_count( ) != original_count ) {
        std::cout << "Possible memory leak!\n";
        rc = 1;
    }
    return( rc );
}
