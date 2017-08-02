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
* Description: This file contains new functional tests for std::deque.
*              The design changed and these tests were written as the 
*              updated version was checked.  The old tests are kept as
*              they should pass as well.
*
****************************************************************************/

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <deque>
#include <cstdlib>

#include "sanity.cpp"

bool construction_test( )
{
    std::deque< int > d1;
    int i;
    if( INSANE( d1 ) || !d1.empty() ) FAIL
    
    for( i = 0; i < 100; i++ ) d1.push_back( i );
    if( INSANE( d1 ) || d1.size() != 100 ) FAIL
    
    for( i = 0; i < 100; i++ ){
        if( d1[i] != i ) FAIL
    }
    
    
    // copy ctor
    std::deque< int > d2( d1 );
    if( INSANE(d1) || INSANE(d2) || d1.size() != 100 || d2.size() != 100  ) FAIL

    for( i = 0; i < 100; i++ ){
        if( d1[i] != i ) FAIL
        if( d2.front() != i ) FAIL
        d2.pop_front();
    }
    if( INSANE(d2) || INSANE(d1) || !d2.empty() || d1.size() != 100 ) FAIL
    
    return( true );
}

bool push_test()
{
    std::deque< int > d1;
    int i, size;
    size = 330;
    
    for( i = 0; i < size; i++ ){
        //std::cout<<i<<", ";
        d1.push_front( i*2 );
    }

    if( INSANE(d1) || d1.size() != 330 ) FAIL
    for( i = 0; i < size; i++ ){
        if( d1[i] != (size-i-1)*2 ) FAIL
    }
    if( INSANE(d1) || d1.size() != 330 ) FAIL
    d1.clear();
    if( INSANE(d1) || d1.size() != 0 ) FAIL
    
    return( true );
}

bool torture_test( int size )
{
    using namespace std;
    deque< int > d1;
    srand( size );
    // favour adding stuff until full
    while( d1.size() < size ){
        if( d1.empty() || rand()%100 > 10 ){
            //grow
            switch( rand() %3 ){
                case 0 : d1.push_front( rand() ); break;
                case 1 : d1.push_back( rand() ); break;
                default : 
                    int n = d1.empty() ? 0 : rand() % d1.size();
                    d1.insert( d1.begin() + n, rand() ); 
                    break;
            }
        }else{
            //strink
            switch( rand() %3 ){
                case 0 : d1.pop_front( ); break;
                case 1 : d1.pop_back( ); break;
                default : 
                    int n = rand() % d1.size();
                    d1.erase( d1.begin() + n ); 
                    break;
            }
        }
        if( INSANE( d1 ) ) FAIL
    };
    // favour removing until empty
    while( !d1.empty() ){
        if( rand()%100 > 90 ){
            //grow
            switch( rand() %3 ){
                case 0 : d1.push_front( rand() ); break;
                case 1 : d1.push_back( rand() ); break;
                default : 
                    int n = d1.empty() ? 0 : rand() % d1.size();
                    d1.insert( d1.begin() + n, rand() ); 
                    break;
            }
        }else{
            //strink
            switch( rand() %3 ){
                case 0 : d1.pop_front( ); break;
                case 1 : d1.pop_back( ); break;
                default : 
                    int n = rand() % d1.size();
                    d1.erase( d1.begin() + n ); 
                    break;
            }
        }
        if( INSANE( d1 ) ) FAIL
    };
    
    
    return( true );
}

bool iterator_test()
{
    std::deque<int> d;
    std::deque<int>::iterator it, it2;
    int i,j, size=1333;
    
    for( i = 0; i < size; i++ ){
        d.push_back(i);
        for( j = 0; j <= i; j++ ){
            it = d.begin();
            it += j;
            if( *it != j ) FAIL
        }
    }
    if( INSANE( d ) ) FAIL

    // post inc
    for( i = 0, it = d.begin(); it != d.end(); i++ ){
        //std::cout<<*it++<<", ";
        if( INSANE( d ) || *it++ != i ) FAIL
    }
    
    // rotate elements and test iterators
    for( i = size-1; i >= 0; i-- ){
        d.push_front( d.back() );
        d.pop_back();
        for( j = 0, it = d.begin(); j < size; ++j, ++it ){
            //std::cout<< *it<<", "<<(j+i)%size<<"\n" ;
            if( *it != (j+i)%size ) FAIL
            if( it > d.end() ) FAIL
        }
        if( it != d.end() ) FAIL
    }
    
    // test some ordering ops
    it = d.begin() + size/2;
    if( it < d.begin() || it > d.end() || d.begin() >= it || d.end() <= it ) FAIL
    for( i = 0, it2 = d.begin(); i < size/2; ++i, ++it2);
    if( it != it2 ) FAIL
    --it;
    if( it == it2 || it > it2 || it >= it2 ) FAIL
    if( it2 < it || it2 <= it ) FAIL
    
    //pre inc
    for( i = 0, it = d.begin(); it != d.end(); ++it, ++i ){
        //std::cout<<*it<<", ";
        if( INSANE( d ) || *it != i ) FAIL
        *it = 999;
    }
    
    //pre dec
    for( it = --d.end(); ; --it ){
        //std::cout<<*it<<", ";
        if( INSANE( d ) || *it != 999 ) FAIL
        if( it == d.begin() ) break;
    }
    
    return( true );
}

bool reserve_test()
{
    using namespace std;
    deque<int> d;
    
    d.reserve( 1024 );
    if( INSANE( d ) ) FAIL
    
    // use special allocator and insert to test no alloc takes place
    // to do
    
    return( true );
}


bool erase_test()
{
    using namespace std;
    deque<int> d;
    std::deque<int>::iterator it;
    int i,j, s = 1001;
    
    for( i = 0; i < s; i++ ) d.push_back(i);
    
    for( i = 0; i < s-1; i++ ){
        d.erase( ++d.begin() );
        for( j = 1; j < s-i-1; j++ ){
            //cout<<i+j+1<<", "<<d[j]<<"\n";
            if( INSANE(d) || i+j+1 != d[j] ) FAIL
        }
    }
    if( d.size() != 1 || d.front() != 0 ) FAIL
    
    
    return( true );
}

bool insert_single_test()
{
    using namespace std;
    deque<int> d;
    deque<int>::iterator it;
    
    int i;
    
    d.insert( d.begin(),        2 );    // front
    d.insert( d.begin(),        0 );    // front
    d.insert( d.end(),          5 );    // end
    d.insert( ++d.begin(),      1 );    // middle
    it = d.insert( --d.end(),   4 );    // middle
    d.insert( it,               3 );    // middle
    
    
    if( INSANE( d ) || d.size() != 6 ) FAIL
    
    for( i = 0; i < 6; i++ ){
        //cout<< d[i]<<"\n";
        if( d[i] != i ) FAIL
    }
        
    return( true );
}

bool insert_multiple_test( )
{
    return( true );
}

bool clear_test()
{
    using namespace std;
    deque<int> d;
    int i;
    
    if( INSANE( d ) || d.size() != 0 ) FAIL
    for( i = 0; i < 128; i++ ){
        d.clear();
        if( INSANE( d ) || d.size() != 0 ) FAIL
        d.push_back( 1 );
        if( INSANE( d ) || d.size() != 1 ) FAIL
        d.clear();
        if( INSANE( d ) || d.size() != 0 ) FAIL
        d.clear();
        if( INSANE( d ) || d.size() != 0 ) FAIL
        d.push_back( 1 );
        if( INSANE( d ) || d.size() != 1 ) FAIL
        d.push_back( 1 );
        if( INSANE( d ) || d.size() != 2 ) FAIL
        d.clear();
        if( INSANE( d ) || d.size() != 0 ) FAIL
        d.push_back(1);
        if( INSANE( d ) || d.size() != 1 ) FAIL
    }
    
    return( true );
}

int main( )
{
    int rc = 0;
    int original_count = heap_count( );
    
    try {
        if( !construction_test()    || !heap_ok( "t01" ) ) rc = 1;
        if( !push_test()            || !heap_ok( "t02" ) ) rc = 1;
        if( !torture_test(12345)    || !heap_ok( "t03" ) ) rc = 1;
        if( !torture_test(16*512)   || !heap_ok( "t04" ) ) rc = 1;
        if( !torture_test(16*256-1) || !heap_ok( "t05" ) ) rc = 1;
        if( !torture_test(16*128+1) || !heap_ok( "t06" ) ) rc = 1;
        
        //if( !access_test( )          || !heap_ok( "t02" ) ) rc = 1;
        // if( !assign_test( )          || !heap_ok( "t03" ) ) rc = 1;
        //if( !pushfront_test( int_check, int_check_size ) ||          !heap_ok( "t4" ) ) rc = 1;
        //if( !pushfront_test( string_check, string_check_size ) ||          !heap_ok( "t5" ) ) rc = 1;
        //if( !pushback_test( int_check, int_check_size ) ||          !heap_ok( "t6" ) ) rc = 1;
        //if( !pushback_test( string_check, string_check_size ) ||          !heap_ok( "t7" ) ) rc = 1;
        if( !iterator_test( )        || !heap_ok( "t08" ) ) rc = 1;
        if( !reserve_test( )         || !heap_ok( "t08" ) ) rc = 1;
        if( !erase_test( )           || !heap_ok( "t08" ) ) rc = 1;
        if( !insert_single_test( )   || !heap_ok( "t09" ) ) rc = 1;
        // if( !insert_multiple_test( ) || !heap_ok( "t10" ) ) rc = 1;
        // if( !erase_test( )           || !heap_ok( "t11" ) ) rc = 1;
        // if( !relational_test( )      || !heap_ok( "t12" ) ) rc = 1;
        // if( !swap_test( )            || !heap_ok( "t13" ) ) rc = 1;
        if( !clear_test( )           || !heap_ok( "t14" ) ) rc = 1;
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
