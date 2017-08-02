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
* Description:  This file contains the functional tests for the algorthms
*               in _algmut.h.
*
****************************************************************************/

#include <cstring>
#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include "itcat.h"
#include "sanity.cpp"

// "Little" functions used in some of the tests.
// ---------------------------------------------

// Make some internally linked to see if that causes any problems.
static bool is_odd( int num )
{
    return( static_cast<bool>( num % 2 ) );
}

bool both_oddeven( int num1, int num2 )
{
    if( is_odd( num1 ) && is_odd( num2 ) ) return true;
    if( !is_odd( num1 ) && !is_odd( num2 ) ) return true;
    return false;
}

static int square( int num )
{
    return( num * num );
}

std::string join_strings( const std::string &left,
                          const std::string &right )
{
    return( left + right );
}

static int powers_of_two( )
{
    static int current = 1;

    int temp = current;
    current *= 2;
    return( temp );
}


bool copy_test( )
{
    int array1[] = { 0, 1, 2, 3, 4 };
    int array2[] = { 0, 0, 0, 0, 0 };
    int array3[] = { 0, 0, 0, 0, 0 };
    int array4[] = { 1, 2, 3, 4, 4 };

    std::copy( InpIt<int>(array1),
               InpIt<int>(array1 + 5),
               OutIt<int>(array2) );
    if( std::memcmp( array1, array2, 5 * sizeof( int ) ) != 0 ) FAIL;

    OutIt<int> p = std::copy( InpIt<int>(array1),
                              InpIt<int>(array1 + 3),
                              OutIt<int>(array3) );
    std::copy( InpIt<int>(array1 + 3), InpIt<int>(array1 + 5), p );
    if( std::memcmp( array1, array3, 5 * sizeof( int ) ) != 0 ) FAIL;

    std::copy( InpIt<int>(array1 + 1),
               InpIt<int>(array1 + 5),
               OutIt<int>(array1) );
    if( std::memcmp( array1, array4, 5 * sizeof( int ) ) != 0 ) FAIL;

    int array5[] = { 0, 1, 2, 3, 4 };
    int array6[] = { 0, 0, 1, 2, 3 };

    std::copy_backward( BidIt<int>(array5),
                        BidIt<int>(array5 + 4),
                        BidIt<int>(array5 + 5) );
    if( std::memcmp( array5, array6, 5 * sizeof( int ) ) != 0 ) FAIL;

    return( true );
}


bool swap_test( )
{
    int array1[] = { 0, 1, 2, 3, 4 };
    int array2[] = { 4, 3, 2, 1, 0 };

    int result_array1[] = { 4, 3, 2, 1, 0 };
    int result_array2[] = { 0, 1, 2, 3, 4 };

    std::swap_ranges( FwdIt<int>(array1),
                      FwdIt<int>(array1 + 5),
                      FwdIt<int>(array2) );
    if( std::memcmp( array1, result_array1, 5 * sizeof( int ) ) != 0 ) FAIL;
    if( std::memcmp( array2, result_array2, 5 * sizeof( int ) ) != 0 ) FAIL;

    std::string sarray1[] = { "abc", "xyz", "123" };
    std::string sarray2[] = { "123", "xyz", "abc" };

    std::swap_ranges( FwdIt<std::string>(sarray1),
                      FwdIt<std::string>(sarray1 + 3),
                      FwdIt<std::string>(sarray2) );
    if( sarray1[0] != "123" || sarray1[1] != "xyz" || sarray1[2] != "abc" ) FAIL;
    if( sarray2[0] != "abc" || sarray2[1] != "xyz" || sarray2[2] != "123" ) FAIL;

    return( true );
}


bool transform_test( )
{
    int array1[]  = { 0, 1, 2, 3,  4 };
    int array2[]  = { 0, 0, 0, 0,  0 };
    int result1[] = { 0, 1, 4, 9, 16 };
    int result2[] = { 0, 2, 4, 6,  8 };

    std::transform( InpIt<int>(array1),
                    InpIt<int>(array1 + 5),
                    OutIt<int>(array2),
                    square );
    if( std::memcmp( array2, result1, 5 * sizeof( int ) ) != 0 ) FAIL;

    std::transform( InpIt<int>(array1),
                    InpIt<int>(array1 + 5),
                    OutIt<int>(array2),
                    std::bind1st( std::multiplies< int >( ), 2 ) );
    if( std::memcmp( array2, result2, 5 * sizeof( int ) ) != 0 ) FAIL;

    std::string sarray1[] = { "hello", "fizzle" };
    std::string sarray2[] = { "world", "sticks" };
    std::string sarray3[2];
    std::string sresult[] = { "helloworld", "fizzlesticks" };

    std::transform( InpIt<std::string>(sarray1),
                    InpIt<std::string>(sarray1 + 2),
                    InpIt<std::string>(sarray2),
                    OutIt<std::string>(sarray3),
                    join_strings );
    if( sarray3[0] != sresult[0] || sarray3[1] != sresult[1] ) FAIL;

    return( true );
}


bool replace_test( )
{
    int array1[] = { 0, 1,  2, 3, 4 };
    int array2[] = { 0, 1, -1, 3, 4 };

    std::replace( FwdIt<int>(array1), FwdIt<int>(array1 + 5), 2, -1 );
    if( std::memcmp( array1, array2, 5 * sizeof( int ) ) != 0 ) FAIL;

    int array3[] = {  0, 1,  2, 3,  4 };
    int array4[] = { -1, 1, -1, 3, -1 };

    std::replace_if( FwdIt<int>(array3),
                     FwdIt<int>(array3 + 5),
                     std::not1( std::ptr_fun( is_odd ) ),
                     -1 );
    if( std::memcmp( array3, array4, 5 * sizeof( int ) ) != 0 ) FAIL;

    int array5[] = { 0, 1, 2,  3, 4 };
    int array6[5];
    int array7[] = { 0, 1, 2, -1, 4 };

    std::replace_copy( InpIt<int>(array5),
                       InpIt<int>(array5 + 5),
                       OutIt<int>(array6), 3,
                       -1 );
    if( std::memcmp( array6, array7, 5 * sizeof( int ) ) != 0 ) FAIL;

    int array8[] = {  0,  1,  2, 3, 4 };
    int array9[5];
    int arrayA[] = { -1, -1, -1, 3, 4 };

    std::replace_copy_if( InpIt<int>(array8),
                          InpIt<int>(array8 + 5),
                          OutIt<int>(array9),
                          std::bind2nd( std::less_equal< int >( ), 2 ),
                          -1 );
    if( std::memcmp( array9, arrayA, 5 * sizeof( int ) ) != 0 ) FAIL;

    return( true );
}


bool fill_test( )
{
    int array1[5];
    int array2[] = { -2, -2, -2, -2, -2 };
    int array3[] = {  1,  1,  1, -2, -2 };

    std::fill( FwdIt<int>(array1), FwdIt<int>(array1 + 5), -2 );
    if( std::memcmp( array1, array2, 5 * sizeof( int ) ) != 0 ) FAIL;

    std::fill_n( OutIt<int>(array1), 3, 1 );
    if( std::memcmp( array1, array3, 5 * sizeof( int ) ) != 0 ) FAIL;

    return( true );
}


bool generate_test( )
{
    int array1[5];
    int array2[] = {  1,  2, 4, 8, 16 };
    int array3[] = { 32, 64, 4, 8, 16 };

    std::generate( FwdIt<int>(array1), FwdIt<int>(array1 + 5), powers_of_two );
    if( std::memcmp( array1, array2, 5 * sizeof( int ) ) != 0 ) FAIL;

    std::generate_n( OutIt<int>(array1), 2, powers_of_two );
    if( std::memcmp( array1, array3, 5 * sizeof( int ) ) != 0 ) FAIL;

    return( true );
}

bool remove_test( )
{
    FwdIt<int> o;
    int ref[] = { 1,2,3,4,3,2,1 };
    int a1[] = { 1,2,3,4,3,2,1 };
    //---- remove ----
    //don't remove anything
    o = std::remove( FwdIt<int>(a1), FwdIt<int>(a1 + 7), 0 );
    if( std::memcmp( a1, ref, sizeof( ref ) ) || (o.get() != a1 + 7) ) FAIL
    //remove 2s
    o = std::remove( FwdIt<int>(a1), FwdIt<int>(a1 + 7), 2 );
    int ref2[] = { 1,3,4,3,1 };
    if( std::memcmp( a1, ref2, sizeof( ref2 ) ) || (o.get() != a1 + 5) ) FAIL
    //remove 1s
    o = std::remove( FwdIt<int>(a1), FwdIt<int>(a1 + 5), 1 );
    int ref3[] = { 3,4,3 };
    if( std::memcmp( a1, ref3, sizeof( ref3 ) ) || (o.get() != a1 + 3) ) FAIL
    //remove 4
    o = std::remove( FwdIt<int>(a1), FwdIt<int>(a1 + 3), 4 );
    int ref4[] = { 3,3 };
    if( std::memcmp( a1, ref4, sizeof( ref4 ) ) || (o.get() != a1 + 2) ) FAIL
    //remove 3
    o = std::remove( FwdIt<int>(a1), FwdIt<int>(a1 + 2), 3 );
    if( o.get() != a1 ) FAIL
  
    //---- remove_copy_if ----
    int out[9];
    int a2[] = { 1,2,3,4,5,4,3,2,1 };
    int ref5[] = { 2,4,4,2 };
    OutIt<int> o2 = std::remove_copy_if( InpIt<int>(a2),
                                         InpIt<int>(a2 + 9),
                                         OutIt<int>(out),
                                         is_odd ); //removes if !false
    if( std::memcmp( out, ref5, sizeof( ref5 ) ) ) FAIL
  
    //---- remove_if ----
    //just a quick test because it is really the same as remove_copy_if
    o = std::remove_if( FwdIt<int>(a2), FwdIt<int>(a2 + 9), is_odd ); //removes if !false
    if( std::memcmp( out, ref5, sizeof( ref5 ) ) || (o.get() != a2 + 4 ) ) FAIL
  
    //---- remove_copy ----
    //just a quick test because it is really the same as remove
    int a3[] = { 9, 1, 9, 2, 9, 3, 9, 4, 9, 5, 9, 6, 9, 7, 9 };
    int ref6[] = { 1, 2, 3, 4, 5, 6, 7 };
    o2 = std::remove_copy( InpIt<int>(a3),
                           InpIt<int>(a3+15),
                           OutIt<int>(out),
                           9 );
    if( std::memcmp( out, ref6, sizeof( ref6 ) ) ) FAIL
  
    return( true );
}


bool unique_test( )
{
    int s1[] = { 0, 1, 2, 3 };
    int s2[] = { 0, 0, 0, 0 };
    int s3[] = { 1, 2, 2, 3, 3, 3, 4, 5, 5 };
    int s3_result[] = { 1, 2, 3, 4, 5 };

    FwdIt<int> p;
    p = std::unique( FwdIt<int>(s1), FwdIt<int>(s1 + 4) );
    if( p.get() != s1 + 4 ) FAIL;

    p = std::unique( FwdIt<int>(s2), FwdIt<int>(s2 + 4) );
    if( p.get() != s2 + 1 || *p.get() != 0 ) FAIL;

    p = std::unique( FwdIt<int>(s3), FwdIt<int>(s3 + 9) );
    if( p.get() != s3 + 5 ) FAIL;
    for( int i = 0; i < 5; ++i ) {
        if( s3[i] != s3_result[i] ) FAIL;
    }

    int s4[] = { 1, 3, 5, 2, 4, 6 };
    int s4_result[] = { 1, 2 };

    p = std::unique( FwdIt<int>(s4), FwdIt<int>(s4 + 6), both_oddeven );
    if( p.get() != s4 + 2 ) FAIL;
    for( int i = 0; i < 2; ++i ) {
        if( s4[i] != s4_result[i] ) FAIL;
    }

    int c1[] = { 0, 1, 2, 3 };
    int c2[] = { 0, 0, 0, 0 };
    int c3[] = { 1, 2, 2, 3, 3, 3, 4, 5, 5 };
    int c_result[ 16 ];

    int c1_result[] = { 0, 1, 2, 3 };
    int c2_result[] = { 0 };
    int c3_result[] = { 1, 2, 3, 4, 5 };

    OutIt<int> p1 = std::unique_copy( InpIt<int>(c1),
                                      InpIt<int>(c1 + 4),
                                      OutIt<int>(c_result) );
    if( p1.get() != c_result + 4 ) FAIL;
    if( std::memcmp( c_result, c1_result, 4 * sizeof( int ) ) != 0 ) FAIL;

    OutIt<int> p2 = std::unique_copy( InpIt<int>(c2),
                                      InpIt<int>(c2 + 4),
                                      OutIt<int>(c_result) );
    if( p2.get() != c_result + 1 ) FAIL;
    if( std::memcmp( c_result, c2_result, 1 * sizeof( int ) ) != 0 ) FAIL;

    OutIt<int> p3 = std::unique_copy( InpIt<int>(c3),
                                      InpIt<int>(c3 + 9),
                                      OutIt<int>(c_result) );
    if( p3.get() != c_result + 5 ) FAIL;
    if( std::memcmp( c_result, c3_result, 5 * sizeof( int ) ) != 0 ) FAIL;

    int c4[] = { 1, 3, 5, 2, 4, 6 };
    int c4_result[] = { 1, 2 };

    OutIt<int> p4 = std::unique_copy( InpIt<int>(c4),
                                      InpIt<int>(c4 + 6),
                                      OutIt<int>(c_result),
                                      both_oddeven );
    if( p4.get() != c_result + 2 ) FAIL;
    if( std::memcmp( c_result, c4_result, 2 * sizeof( int ) ) != 0 ) FAIL;

    return( true );
}


bool reverse_test( )
{
    int array1[] = { 0, 1, 2, 3, 4 };
    int array2[] = { 4, 3, 2, 1, 0 };

    std::reverse( BidIt<int>(array1), BidIt<int>(array1 + 5) );
    if( std::memcmp( array1, array2, 5 * sizeof( int ) ) != 0 ) FAIL;

    int array3[] = { 0, 1, 2, 3 };
    int array4[] = { 3, 2, 1, 0 };

    std::reverse( BidIt<int>(array3), BidIt<int>(array3 + 4) );
    if( std::memcmp( array3, array4, 4 * sizeof( int ) ) != 0 ) FAIL;

    int array5[] = { 0, 1, 2, 3, 4 };
    int array6[5];
    int array7[] = { 4, 3, 2, 1, 0 };

    std::reverse_copy( BidIt<int>(array5),
                       BidIt<int>(array5 + 5),
                       OutIt<int>(array6) );
    if( std::memcmp( array6, array7, 5 * sizeof( int ) ) != 0 ) FAIL;

    int array8[] = { 0, 1, 2, 3 };
    int array9[4];
    int arrayA[] = { 3, 2, 1, 0 };

    std::reverse_copy( BidIt<int>(array8),
                       BidIt<int>(array8 + 4),
                       OutIt<int>(array9) );
    if( std::memcmp( array9, arrayA, 4 * sizeof( int ) ) != 0 ) FAIL;

    return( true );
}

// This function verifies that each value from 0..size is in the given
// array once and only once. This function is used to check the sanity
// of the output of random_shuffle.
//
static bool check_values( int *array, int size )
{
    bool all_good = true;

    // Let's use good old malloc here... just to see if it works for us.
    int *counters = static_cast< int * >( std::malloc( size*sizeof( int ) ) );
    for( int i = 0; i < size; ++i ) counters[i] = 0;
    for( int i = 0; i < size; ++i ) counters[ array[i] ]++;
    for( int i = 0; i < size; ++i )
        if( counters[i] != 1 ) all_good = false;

    std::free( counters );
    return( all_good );
}

// This function is a simple random number generator given to the three
// argument form of random_shuffle. It is intentionally not very random
// so that we can force certain effects.
//
static int simple_generator( int )
{
    return 0;
}

bool random_shuffle_test( )
{
    int array[ ] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    // Let's just try a few shuffles... er... at random.

    std::srand( 0 );
    std::random_shuffle( array, array + 10 );
    if( !check_values( array, 10 ) ) FAIL;

    std::srand( 10 );
    std::random_shuffle( array, array + 10 );
    if( !check_values( array, 10 ) ) FAIL;

    std::srand( 100 );
    std::random_shuffle( array, array + 10 );
    if( !check_values( array, 10 ) ) FAIL;

    std::random_shuffle( array, array + 10, simple_generator );
    if( !check_values( array, 10 ) ) FAIL; 

    return( true );
}


int main( )
{
  int rc = 0;
  int original_count = heap_count( );

  try {
    if( !copy_test( )           || !heap_ok( "t01" ) ) rc = 1;
    if( !swap_test( )           || !heap_ok( "t02" ) ) rc = 1;
    if( !transform_test( )      || !heap_ok( "t03" ) ) rc = 1;
    if( !replace_test( )        || !heap_ok( "t04" ) ) rc = 1;
    if( !fill_test( )           || !heap_ok( "t05" ) ) rc = 1;
    if( !generate_test( )       || !heap_ok( "t06" ) ) rc = 1;
    if( !remove_test( )         || !heap_ok( "t07" ) ) rc = 1;
    if( !unique_test( )         || !heap_ok( "t08" ) ) rc = 1;
    if( !reverse_test( )        || !heap_ok( "t09" ) ) rc = 1;
    if( !random_shuffle_test( ) || !heap_ok( "t10" ) ) rc = 1;
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
