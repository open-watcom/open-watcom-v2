/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2008 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  This file contains the functional tests for _watcom::slist.
*
****************************************************************************/

#include <algorithm>
#include <iostream>
#include <slist>
#include "sanity.cpp"
#include "allocxtr.hpp"

/* ------------------------------------------------------------------
 * construct_test()
 * ctor, ctor(size), cpyctor, assignment op
 */
struct test_type{
    int i;
    test_type() : i(777) {}
};
bool operator<( test_type const &, test_type const & ){ return false; }
bool operator==( test_type const & t, test_type const & o ){ return o.i==t.i; }

bool construct_test( )
{

        
    using namespace _watcom;
    
    slist<int> sl;
    if( INSANE(sl) || !sl.empty() ) FAIL
    
    slist<test_type> sl2(40);
    if( INSANE(sl2) || sl2.size() != 40 ) FAIL
    
    slist<test_type> sl3(sl2);
    if( INSANE(sl2) || sl2.size() != 40 ) FAIL
    if( INSANE(sl3) || sl3.size() != 40 ) FAIL
    for( int i = 0; i < 40; i++ ){
        if( sl3.front().i != 777 ) FAIL
        sl3.pop_front();
    }
    
    // copy ctor
    slist<test_type> sl4(sl3);
    if( INSANE(sl3) || !sl3.empty() ) FAIL
    if( INSANE(sl4) || !sl4.empty() ) FAIL
        
    // copy ctor again
    sl.clear();
    for( int i = 0; i< 20; i++ ) sl.push_front( i );
    if( INSANE(sl) || sl.size() != 20 ) FAIL
    slist<int> sl5( sl );
    if( INSANE(sl) || sl.size() != 20 ) FAIL
    if( INSANE(sl5) || sl5.size() != 20 ) FAIL
    for( int i = 19; i >= 0; i-- ){
        //std::cout<< sl.front()<<", "<<sl5.front()<<"\n";
        if( sl5.front() != i ) FAIL;
        sl5.pop_front();
        //sl.pop_front();
    }
    if( INSANE(sl5) || !sl5.empty() ) FAIL
        
    // assignment
    slist<int> * psl = &sl;
    sl5 = sl;
    if( INSANE(sl) || sl.size() != 20 ) FAIL
    if( INSANE(sl5) || sl5.size() != 20 ) FAIL
    for( int i = 19; i >= 0; i-- ){
        if( sl5.front() != i ) FAIL;
        sl5.pop_front();
    }
    if( INSANE(sl) || sl.size() != 20 ) FAIL
    if( INSANE(sl5) || !sl5.empty() ) FAIL
    
    // check self assignment ok
    sl = *psl;
    if( INSANE(sl) || sl.size() != 20 ) FAIL
    for( int i = 19; i >= 0; i-- ){
        if( sl.front() != i ) FAIL;
        sl.pop_front();
    }
    if( INSANE(sl) || !sl.empty() ) FAIL
    
    // assign empty
    for( int i = 0; i< 20; i++ ) sl.push_front( i );
    sl = sl5;
    
    // construct from iterators (allocator param not yet implemented)
    std::string str("Daniel Cletheroe");
    slist<char> sl6( str.begin(), str.end() );
    if( INSANE(sl6) || sl6.size() != str.length() ) FAIL
    for( int i = 0; i < str.length(); i++ ){
        if( str[i] != sl6.front() ) FAIL
        sl6.pop_front();
    }
    
    
    return( true );
}

/* ------------------------------------------------------------------
 * access_test
 * test access and modifiers
 * push_front, front, insert, pop_front
 */
bool access_test( )
{
    using namespace _watcom;
    slist<int> sl;
    int i;
    static int const chk[] = {
        101,
        102, 102, 102, 102,
        101, 101, 101, 101, 101,
        0,
        80, 81, 29, 
        99,
            28, 27, 26, 25, 24, 23, 22, 21, 20,
        55, 18, 17, 16, 15, 14, 13, 12, 11, 10,
        -1,
        -2, -2, -2, -2, -2,
        -3, -3, -3
    };
    const int chk_len = sizeof(chk) / sizeof(int);
    
    for( i=10; i<20; i++ ){
        sl.push_front( i );
    }
    if( INSANE(sl) || sl.front() != 19 ) FAIL
    sl.front() = 55;
    for( i=20; i<30; i++ ){
        sl.push_front( i );
    }
    if( INSANE(sl) || sl.front() != 29 ) FAIL
    
    sl.insert( sl.begin(), 0 );
    sl.insert( sl.end(), -1 );
    sl.insert( ++( ++sl.begin() ), 99 );
    sl.insert_after( sl.begin(), 80 );
    sl.insert_after( ++sl.begin(), 81 );
    sl.insert( sl.end(), 5, -2 );
    sl.insert( sl.begin(), 6, 101 );        // will favour non-template?
    sl.insert( sl.end(), 0, 21293 );
    sl.insert_after( sl.begin(), 4, 102 );
    sl.insert_after( sl.previous( sl.end() ), 3, -3 );
    if( INSANE(sl) || sl.size() != chk_len ) FAIL
    
    for( i=0; sl.size(); i++ ){
        if( sl.front() != chk[i] ) FAIL
        sl.pop_front();
    }
    if( INSANE(sl) || !sl.empty() ) FAIL
    
    sl.insert( sl.cend(), 20, 100 );        // will call template version
    if( INSANE(sl) || sl.size() != 20 ) FAIL
    while( !sl.empty() ){
        if( sl.front() != 100 ) FAIL
        sl.pop_front();
    }
    
    slist<int> sl2;
    for( i = 10; i >= 1; i-- ) sl2.push_front( i );
    sl.clear();
    sl.push_front( 11 );
    sl.insert( sl.cbegin(), sl2.begin(), sl2.end() );
    if( INSANE(sl) || sl.size() != 11 ) FAIL
    for( i = 1; i <= 11; i++ ){
        if( sl.front() != i ) FAIL
        sl.pop_front();
    }
    sl.push_front( 0 );
    sl.insert( ++sl.begin(), sl2.begin(), sl2.end() );
    if( INSANE(sl) || sl.size() != 11 ) FAIL
    for( i = 0; i <= 10; i++ ){
        if( sl.front() != i ) FAIL
        sl.pop_front();
    }
    
    sl.push_front( 0 );
    sl.insert_after( sl.cbegin(), sl2.begin(), sl2.end() );
    if( INSANE(sl) || sl.size() != 11 ) FAIL
    for( i = 0; i <= 10; i++ ){
        if( sl.front() != i ) FAIL
        sl.pop_front();
    }
    
    return( true );
}

/* ------------------------------------------------------------------
 * assign_test
 * test list assignment methods
 */
bool assign_test( )
{
    using namespace _watcom;
    slist<char> sl;
    
    sl.assign( (size_t)999, 'a' );
    if( INSANE(sl) || sl.size() != 999 ) FAIL
    while( !sl.empty() ){
        if( sl.front() != 'a' ) FAIL
        sl.pop_front();
    }
    
    std::string src("dan woz ere");
    sl.assign( src.begin(), src.end() );
    if( INSANE(sl) || sl.size() != src.length() ) FAIL
    for( int i =0; i < src.length(); i++ ){
        if( sl.front() != src[i] ) FAIL
        sl.pop_front();
    }
    
    sl.assign( 999, 99 );
    if( INSANE(sl) || sl.size() != 999 ) FAIL
    while( !sl.empty() ){
        if( sl.front() != 99 ) FAIL
        sl.pop_front();
    }
    
    return( true );
}

/* ------------------------------------------------------------------
 * capacity_test
 * resizing
 */
bool capacity_test( )
{
    _watcom::slist<int> sl;
    _watcom::slist<int>::iterator it;
    int i;
    
    for( i = 0; i < 100; i++ ) sl.push_front( i );
    
    // strink list
    sl.resize( 50 );
    if( INSANE(sl) || sl.size() != 50 ) FAIL
    
    it = sl.begin();
    for( i = 99; i >= 50; i-- ){
        if( INSANE(sl) || *it++ != i ) FAIL
    }
    
    // grow list (default construct element)
    sl.resize( 100 );
    if( INSANE(sl) || sl.size() != 100 ) FAIL
    
    it = sl.begin();
    for( i = 99; i >= 50; i-- ){
        if( INSANE(sl) || *it++ != i ) FAIL
    }
    for( ; i >= 0; i-- ){
        if( INSANE(sl) || *it++ != 0 ) FAIL
    }
    
    // strink to nothing
    sl.resize( 0 );
    if( INSANE(sl) || sl.size() != 0 || !sl.empty() ) FAIL
    
    // grow empty list
    sl.resize( 75 );
    if( INSANE(sl) || sl.size() != 75 || sl.empty() ) FAIL
    it = sl.begin();
    for( i = 0; i < 75; i++ ){
        if( INSANE(sl) || *it++ != 0 ) FAIL
    }
    
    return( true );
}

/* ------------------------------------------------------------------
 * clear_test
 * clear and destructor
 */
bool clear_test()
{
    using namespace _watcom;
    slist<int> sl;
    int i;
    
    // clear empty container
    sl.clear();
    if( INSANE(sl) || sl.size() != 0 || !sl.empty() ) FAIL
    
    // clear container with contents
    for( i = 0; i < 10; i++ ) sl.push_front( i );
    sl.clear();
    if( INSANE(sl) || sl.size() != 0 || !sl.empty() ) FAIL
    
    for( i = 0; i < 1000; i++ ) sl.push_front( i );
    // destructor will remove elements and there will be no memory leak
    return( true );
}

/* ------------------------------------------------------------------
 * erase_test
 * erase(it) and erase(it,it)
 */
bool erase_test( )
{
    using namespace _watcom;
    slist<int> sl;
    int i;

    for( i = 9; i >= 0; i-- ){
        sl.push_front( i );
    }
    slist<int>::iterator it = sl.begin();
    for( i = 0; i < 5; i++){
        ++it;
        it = sl.erase( it );
    }
    for( i = 0; i < 5; i++){
        if( sl.front() != i*2 ) FAIL
        sl.erase( sl.begin() );
    }
    if( INSANE(sl) || !sl.empty() ) FAIL
    
    for( i = 9; i >= 0; i-- ){
        sl.push_front( i );
    }
    for( it = sl.begin(), i = 0; i < 9; i++, ++it );
    sl.erase( ++sl.begin(), it );
    if( INSANE(sl) || sl.size() != 2 ) FAIL
    if( sl.front() != 0 || *it != 9 ) FAIL
    sl.erase( sl.begin(), sl.end() );
    if( INSANE(sl) || !sl.empty() ) FAIL
    
    // erase_after
    for( i = 9; i >= 0; i-- ) sl.push_front( i );
    it = sl.begin();
    for( i = 0; i < 5; i++ ){
        it = sl.erase_after( it );
    }
    if( INSANE(sl) || sl.empty() || it != sl.end() ) FAIL
    for( i = 0; i < 5; i++ ){
        if( sl.front() != i*2 ) FAIL
        sl.pop_front();
    }
    
    for( i = 9; i >= 0; i-- ) sl.push_front( i );
    sl.erase( sl.begin(), sl.end() );
    if( INSANE(sl) || !sl.empty() ) FAIL
    
    
    return( true );
}

/* ------------------------------------------------------------------
 * swap_test
 * test list swapping method
 */
bool swap_test( )
{
    _watcom::slist<int> sl1;
    _watcom::slist<int> sl2;
    int i;
    
    for( i = 0; i < 30; i++ ) sl1.push_front( i );
    
    sl2.swap( sl1 );
    if( INSANE( sl1 ) || !sl1.empty() ) FAIL
    if( INSANE( sl2 ) || sl2.size() != 30 ) FAIL
    for( i = 29; i >= 30; i-- ){
        if( sl2.front() != i ) FAIL
        sl2.pop_front();
    }
    
    
    
    return( true );
}

/* ------------------------------------------------------------------
 * remove_test( )
 * test the remove methods
 */
template< class T >
struct is_odd{
    bool operator()( T v ){ return( v%2 == 1 ); }
};

bool remove_test( )
{
    int a[] = { 1,1,1,1,2,1,1,3,4,1,5,6,7,8,1,1,9,1,1,1,1,1 };
    int size_a = sizeof(a)/sizeof(int);
    int i;
    _watcom::slist<int> sl;
    _watcom::slist<int>::const_iterator cit;
        
    for( i = 0; i < size_a; i++ ) sl.push_front( a[i] );
    sl.remove( 1 );
    for( cit = sl.begin(),i = 9; i >= 2; i--, ++cit ){
        if( *cit != i ) FAIL
    }
    if( INSANE(sl) || sl.size() != 8 ) FAIL
    for( i = 2; i<=9; i++ ) sl.remove( i );
    if( INSANE(sl) || !sl.empty() ) FAIL
    
    // remove_if
    sl.assign( a, a+size_a );
    sl.remove_if( is_odd<int>() );
    if( INSANE(sl) || sl.size() != 4 ) FAIL
    while( !sl.empty() ){
        if( sl.front() % 2 ) FAIL
        sl.pop_front();
    };

    return( true );
}

/* ------------------------------------------------------------------
 * iterator_test( )
 * Test the iterator functionality
 */
bool iterator_test( )
{
    using namespace _watcom;
    slist< int > sl;
    typedef slist< int >::iterator sit_t;
    sit_t sit;
    int i;
    
    for( i=0; i<20; i++ ){
        sl.push_front( i );
    }
    // operator++ pre
    sit = sl.begin();
    for( i=19; i>=0; i-- ){
        if( INSANE(sl) || *sit != i ) FAIL
        ++sit;
    }
    if( INSANE(sl) || sit != sl.end() ) FAIL
    // operator++ post
    sit = sl.begin();
    for( i=19; i>=0; i-- ){
        if( INSANE(sl) || *sit++ != i ) FAIL
    }
    if( INSANE(sl) || sit != sl.end() ) FAIL
    
    // const_iterator
    typedef slist< int >::const_iterator scit_t;
    scit_t scit1 = sl.cbegin();
    scit_t scit2 = sit;
    
    // operator++ pre
    for( i = 19 ; scit1 != const_cast< slist<int> const & >(sl).end(); ++scit1, --i ){
        if( INSANE(sl) || *scit1 != i ) FAIL
    }
    // operator++ post
    for( i = 19, scit1 = sl.begin(); scit1 != scit2; i-- ){
        if( INSANE(sl) || *scit1++ != i ) FAIL
    }
    if( INSANE(sl) || scit1 != sl.end() || scit1 != sl.cend() ) FAIL
    
    // previous
    sit = sl.end();
    for( i = 0; i < 20; i++ ){
        sit = sl.previous( sit );
        if( INSANE(sl) || *sit != i ) FAIL
    }
    if( INSANE(sl) || sit != sl.begin() ) FAIL
    
    return( true );
}

/* ------------------------------------------------------------------
 * unique_test
 * test unique
 */
template< class T >
struct is_one_more{
    bool operator()( T b, T a ){ return( (a+1) == b); }
};

bool unique_test( )
{
    int const init[] = { 1,2,3,3,3,4,5,6,6,6,6,7,7,8,8,8 };
    int const init_len = sizeof(init)/sizeof(int);
    _watcom::slist< int > sl;
    int i;
    sl.assign( init, init+init_len );
    if( INSANE( sl ) || sl.size() != init_len ) FAIL
    sl.unique();
    if( INSANE( sl ) || sl.size() != 8 ) FAIL
    for( i = 1; i <= 8; i++ ){
        if( sl.front() != i ) FAIL
        sl.pop_front();
    }
    
    // predicate version
    // The logic is a bit confusing so hopefully this doing the right thing...
    int const init2[] = { 1,1,1,2,2,3,2,3,4,6 };
    int const res2[] = { 1,1,1,3,2,4,6};
    int const init2_len = sizeof(init2)/sizeof(int);
    int const res2_len = sizeof(res2)/sizeof(int);
    sl.assign( init2, init2+init2_len );
    if( INSANE( sl ) || sl.size() != init2_len ) FAIL
    sl.unique( is_one_more<int>() );
    if( INSANE( sl ) || sl.size() != res2_len ) FAIL
    for( i = 0; i<res2_len; i++ ){
        if( sl.front() != res2[i] ) FAIL
        sl.pop_front();
    }
    
    return( true );
}

/* ------------------------------------------------------------------
 * splice_test
 * test list splicing operations
 */
bool splice_test( )
{
    int const src[] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31 };
    int const src_len = sizeof( src ) / sizeof( int );
    _watcom::slist< int > sl1( src, src+src_len );
    _watcom::slist< int > sl2( 1, 1 );
    
    // simple splice
    sl1.splice( sl1.begin(), sl2 );
    if( INSANE(sl2) || !sl2.empty() ) FAIL
    if( INSANE(sl1) || sl1.size() != src_len+1 ) FAIL
    sl2.assign( 1, 37 );
    sl1.splice( sl1.end(), sl2 );
    if( INSANE(sl2) || !sl2.empty() ) FAIL
    if( INSANE(sl1) || sl1.size() != src_len+2 ) FAIL
    
    _watcom::slist< int >::iterator it;
    it = sl1.begin();
    if( *it++ != 1 ) FAIL
    for( int i = 0; i < src_len ; ++it, i++ ) if( *it != src[i] ) FAIL
    if( *it != 37 ) FAIL
    
    // single element splice
    sl2.assign( 1, 789 );
    sl1.splice( sl1.begin(), sl2, sl2.begin() );
    if( INSANE(sl2) || !sl2.empty() ) FAIL
    if( INSANE(sl1) || sl1.size() != src_len+3 ) FAIL
    sl2.assign( 1, 987 );
    sl1.splice( sl1.end(), sl2, sl2.begin() );
    if( INSANE(sl2) || !sl2.empty() ) FAIL
    if( INSANE(sl1) || sl1.size() != src_len+4 ) FAIL
    
    it = sl1.begin();
    if( *it++ != 789 ) FAIL
    if( *it++ != 1 ) FAIL
    for( int i = 0; i < src_len ; ++it, i++ ) if( *it != src[i] ) FAIL
    if( *it++ != 37 ) FAIL
    if( *it++ != 987 ) FAIL
    
    // single element splice_after
    sl2.assign( src, src+src_len );
    sl1.assign( src, src+src_len );
    sl1.splice_after( sl1.begin(), sl2, ++sl2.begin() );
    if( INSANE(sl2) || sl2.size() != src_len-1 ) FAIL
    if( INSANE(sl1) || sl1.size() != src_len+1 ) FAIL
    sl1.splice_after( sl1.previous( sl1.end() ), sl2, 
                      sl2.previous(sl2.previous( sl2.end() ) ) );
    if( INSANE(sl2) || sl2.size() != src_len-2 ) FAIL
    if( INSANE(sl1) || sl1.size() != src_len+2 ) FAIL
    
    it = sl1.begin();
    if( *it++ != 2 ) FAIL
    if( *it++ != 5 ) FAIL
    for( int i = 1; i < src_len ; ++it, i++ ) if( *it != src[i] ) FAIL
    if( *it++ != 31 ) FAIL
    
    // splice in same container
    sl1.assign( src, src+src_len );
    // noop
    sl1.splice( ++sl1.begin(), sl1, sl1.begin() );
    if( INSANE(sl1) || sl1.size() != src_len ) FAIL
    sl1.splice( ++sl1.begin(), sl1, ++sl1.begin() );
    if( INSANE(sl1) || sl1.size() != src_len ) FAIL
    // move
    sl1.splice( sl1.begin(), sl1, ++sl1.begin() );
    if( INSANE(sl1) || sl1.size() != src_len ) FAIL
    it = sl1.begin();
    if( *it++ != 3 ) FAIL
    if( *it++ != 2 ) FAIL
    if( *it++ != 5 ) FAIL
    for( int i = 3; i < src_len ; ++it, i++ ) if( *it != src[i] ) FAIL
    
    // single element splice_after within same container
    sl1.assign( src, src+src_len );
    // noop: splice before self
    sl1.splice_after( sl1.begin(), sl1, sl1.begin() );
    if( INSANE(sl1) || sl1.size() != src_len ) FAIL
    // noop: splice after self
    sl1.splice_after( ++sl1.begin(), sl1, sl1.begin() );
    if( INSANE(sl1) || sl1.size() != src_len ) FAIL
    // move element
    sl1.splice_after( ++(++sl1.begin()), sl1, sl1.begin() );
    if( INSANE(sl1) || sl1.size() != src_len ) FAIL
    it = sl1.begin();
    if( *it++ != 2 ) FAIL
    if( *it++ != 5 ) FAIL
    if( *it++ != 3 ) FAIL
    for( int i = 3; i < src_len ; ++it, i++ ) if( *it != src[i] ) FAIL
    
    // multi element splice_after, same container
    sl1.assign( src, src+src_len );
    it = sl1.begin();
    ++it;++it;++it;++it;
    sl1.splice_after( sl1.begin(), sl1, ++sl1.begin(), it );
    if( INSANE(sl1) || sl1.size() != src_len ) FAIL
    it = sl1.begin();
    if( *it++ != 2 ) FAIL
    if( *it++ != 5 ) FAIL
    if( *it++ != 7 ) FAIL
    if( *it++ != 11 ) FAIL
    if( *it++ != 3 ) FAIL
    for( int i = 5; i < src_len ; ++it, i++ ) if( *it != src[i] ) FAIL
    
    // multi element splice_after, diff container
    sl1.assign( src, src+src_len );
    sl2.assign( src, src+src_len );
    sl1.splice_after( sl1.begin(), sl2, sl2.begin(), sl2.previous(sl2.end()) );
    if( INSANE(sl1) || sl1.size() != src_len*2-1 ) FAIL
    if( INSANE(sl2) || sl2.size() != 1 ) FAIL
    it = sl1.begin();
    if( sl2.front() != 2 ) FAIL
    for( int i = 0; i < src_len ; ++it, i++ ) if( *it != src[i] ) FAIL
    for( int i = 1; i < src_len ; ++it, i++ ) if( *it != src[i] ) FAIL
    
    return( true );
}

/* ------------------------------------------------------------------
 * reverse_test
 * test list reverse method
 */
bool reverse_test( )
{
    _watcom::slist< int > sl;
    int i;
    
    for( i = 0; i < 20; i++ ) sl.push_front( i );
    
    sl.reverse();
    if( INSANE(sl) || sl.size() != 20 ) FAIL
    
    for( i = 0; i < 20; i++ ){
        if( sl.front() != i ) FAIL
        sl.pop_front();
    }
    if( INSANE(sl) || !sl.empty() ) FAIL
    
    return( true );
}


/* ------------------------------------------------------------------
 * sort_test
 */
// helper for sort tests
// check ordered with operator<
template< class T >
bool chk_sorted( T const & lst )
{
    T::const_iterator it,nit;
    it = lst.begin();
    nit = it;
    ++nit;
    for( ; nit != lst.end(); ++it, ++nit ){
        if( *nit < *it ) return( false );
    }
    return( true );
}
// quick and nasty helper class for sort test
// used for checking sort is stable
struct MyPair{
    int x;
    int y;
    MyPair() {}
    MyPair( int xx, int yy ) : x(xx), y(yy) {}
    MyPair( MyPair const & o ) : x(o.x), y(o.y) {}
};
bool operator<( MyPair const & t, MyPair const & o ) { return( t.x < o.x ); }
bool operator==( MyPair const & t, MyPair const & o ) { return( t.x == o.x ); }
//
bool sort_test( )
{
    _watcom::slist<int> lst;
    
    // hit it with a load of random numbers
    int rand_size = 16383;
    for( int i = 0; i < rand_size; i++ ){
        lst.push_front( std::rand() );
    }
    lst.sort();
    if( INSANE( lst ) || lst.size() != rand_size || !chk_sorted(lst) ) FAIL
    lst.clear();
    
    // test the version that takes a functor
    rand_size = 32769;
    for( int i = 0; i < rand_size; i++ ){
        lst.push_front( std::rand() );
    }
    lst.sort( std::greater<int>() );
    lst.reverse();
    if( INSANE( lst ) || lst.size() != rand_size || !chk_sorted(lst) ) FAIL
    lst.clear();
    
    // test sort is stable
    _watcom::slist< MyPair > l2;
    int c;
    // build array of random numbers
    for( c = 10001; c >= 0; ){
        int r = std::rand() % 15;
        if( r == 7 ){
            // 7s are special, second value ascends and 
            // should stay in that order after sort
            l2.push_front( MyPair( 7, c ) ); 
            c--;
        }else{
            l2.push_front( MyPair( r, 0 ) );
        }
    }
    l2.sort();
    if( INSANE( l2 ) ) FAIL
    c = 0;
    int last = l2.front().x;
    while( !l2.empty()){
        // check in sorted order
        if( l2.front().x < last ) FAIL 
        last = l2.front().x;
        // check 7s still in order
        if( l2.front().x == 7 ){
            if( l2.front().y != c ) FAIL
            c++;
        }
        l2.pop_front();
    }
    // end stable test
    
    return( true );
}

/* ------------------------------------------------------------------
 * merge_test
 * test list merging methods
 */
bool merge_test( )
{
    _watcom::slist<int> l1, l2;
    
    // merge two lists of sorted random numbers
    int rand_size = 16383;
    for( int i = 0; i < rand_size; i++ ){
        l1.push_front( std::rand() );
        l2.push_front( std::rand() );
    }
    l1.sort();
    l2.sort();
    l1.merge( l2 );
    if( INSANE( l1 ) || INSANE( l2 ) || !chk_sorted( l1 ) ||
        l1.size() != rand_size*2 || !l2.empty() ) FAIL
    
    // merge nothing
    l1.merge( l2 );
    if( INSANE( l1 ) || INSANE( l2 ) || !chk_sorted( l1 ) ||
        l1.size() != rand_size*2 || !l2.empty() ) FAIL
    
    // merge all
    l2.merge( l1 );
    if( INSANE( l1 ) || INSANE( l2 ) || !chk_sorted( l2 ) ||
        l2.size() != rand_size*2 || !l1.empty() ) FAIL
    
    
    return( true );
}

/* ------------------------------------------------------------------
 * comparisons
 */
bool comparison_test( )
{
    int a[]={1,2,3,4,5,6,7,8,9};
    int b[]={1,2,3,4,5,6,7,8};
    int c[]={2,2,3,4,5,6,7,8,9};
    _watcom::slist<int> sl1( a, a+9 );
    _watcom::slist<int> sl2( a, a+9 );
    
    if( !( sl1 == sl2 ) ) FAIL
    if( !( sl1 <= sl2 ) ) FAIL
    if( !( sl1 >= sl2 ) ) FAIL
    if(    sl1 != sl2   ) FAIL
    if(    sl1 <  sl2   ) FAIL
    if(    sl1 >  sl2   ) FAIL
    
    sl2.assign( b, b+8 );
    if( !( sl1 != sl2 ) ) FAIL
    if( !( sl1 >= sl2 ) ) FAIL
    if( !( sl1 >  sl2 ) ) FAIL
    if(    sl1 == sl2   ) FAIL
    if(    sl1 <  sl2   ) FAIL
    if(    sl1 <= sl2   ) FAIL
    
    sl2.assign( c, c+9 );
    if( !( sl1 != sl2 ) ) FAIL
    if( !( sl1 <= sl2 ) ) FAIL
    if( !( sl1 <  sl2 ) ) FAIL
    if(    sl1 == sl2   ) FAIL
    if(    sl1 >  sl2   ) FAIL
    if(    sl1 >= sl2   ) FAIL
    
    return( true );
}

/* ------------------------------------------------------------------
 * allocator_test
 * test stateful allocators and exception handling
 */
bool allocator_test( )
{
    return( true );
}

int main( )
{
    int rc = 0;
    int original_count = heap_count( );
    //heap_dump();

    try {
        if( !construct_test( )      || !heap_ok( "t01" ) ) rc = 1;
        if( !access_test( )         || !heap_ok( "t02" ) ) rc = 1;
        if( !assign_test( )         || !heap_ok( "t05" ) ) rc = 1;
        if( !clear_test( )          || !heap_ok( "t06" ) ) rc = 1;
        if( !erase_test( )          || !heap_ok( "t07" ) ) rc = 1;
        if( !swap_test( )           || !heap_ok( "t08" ) ) rc = 1;
        if( !remove_test( )         || !heap_ok( "t09" ) ) rc = 1;
        if( !iterator_test( )       || !heap_ok( "t10" ) ) rc = 1;
        if( !unique_test( )         || !heap_ok( "t11" ) ) rc = 1;
        if( !splice_test( )         || !heap_ok( "t12" ) ) rc = 1;
        if( !reverse_test( )        || !heap_ok( "t13" ) ) rc = 1;
        if( !allocator_test( )      || !heap_ok( "t14" ) ) rc = 1;
        if( !capacity_test( )       || !heap_ok( "t15" ) ) rc = 1;
        if( !sort_test( )           || !heap_ok( "t16" ) ) rc = 1;
        if( !merge_test( )          || !heap_ok( "t17" ) ) rc = 1;
        if( !comparison_test( )     || !heap_ok( "t18" ) ) rc = 1;
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
