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
* Description:  This file contains the functional tests for the algorithms
*               in _algnmod.h.
*
* To-Do: The non-mutating algorithms should work on input sequences that
*        are constants. To test this the iteratory catagory templates
*        need to be smarter.
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

void advance_char( char &ch )
{
    if( ++ch == 'z' ) ch = 'a';
}

// Make some internally linked to see if that causes any problems.
static bool is_odd( int num )
{
    return( static_cast<bool>( num % 2 ) );
}

static bool is_divisible( int x, int y )
{
    return( !(x % y) );
}


bool all_of_test( )
{
    int a1[] = { 1, 2, 3, 4 };
    int a2[] = { 1, 3, 5, 7 };

    if(  std::all_of( InpIt<int>( a1 ), InpIt<int>( a1 + 4 ), is_odd ) ) FAIL;
    if( !std::all_of( InpIt<int>( a2 ), InpIt<int>( a2 + 4 ), is_odd ) ) FAIL;
    if( !std::all_of( InpIt<int>( a1 ), InpIt<int>( a1 + 0 ), is_odd ) ) FAIL;

    return( true );
}


bool any_of_test( )
{
    int a1[] = { 1, 2, 3, 4 };
    int a2[] = { 2, 4, 6, 8 };

    if( !std::any_of( InpIt<int>( a1 ), InpIt<int>( a1 + 4 ), is_odd ) ) FAIL;
    if(  std::any_of( InpIt<int>( a2 ), InpIt<int>( a2 + 4 ), is_odd ) ) FAIL;
    if(  std::any_of( InpIt<int>( a1 ), InpIt<int>( a1 + 0 ), is_odd ) ) FAIL;

    return( true );
}


bool none_of_test( )
{
    int a1[] = { 1, 2, 3, 4 };
    int a2[] = { 2, 4, 6, 8 };

    if(  std::none_of( InpIt<int>( a1 ), InpIt<int>( a1 + 4 ), is_odd ) ) FAIL;
    if( !std::none_of( InpIt<int>( a2 ), InpIt<int>( a2 + 4 ), is_odd ) ) FAIL;
    if( !std::none_of( InpIt<int>( a1 ), InpIt<int>( a1 + 0 ), is_odd ) ) FAIL;

    return( true );
}


bool for_each_test( )
{
    char str[] = { 'h', 'e', 'l', 'l', 'o', '\0' };

    // Use FwdIt instead of InpIt because operation modifies sequence.
    std::for_each( FwdIt<char>(str), FwdIt<char>(str), advance_char );
    if( std::strcmp( str, "hello" ) != 0 ) FAIL;

    std::for_each
      ( FwdIt<char>(str), FwdIt<char>(str + 5), advance_char);
    if( std::strcmp( str, "ifmmp" ) != 0 ) FAIL;

    return( true );
}


bool find_test( )
{
    int array[] = { 0, 1, 2, 3, 4 };

    InpIt<int> p1( std::find( InpIt<int>(array), InpIt<int>(array + 5), 2 ) );
    if( p1 == InpIt<int>(array + 5) || *p1 != 2 ) FAIL;

    InpIt<int> p2( std::find( InpIt<int>(array), InpIt<int>(array + 5), 5 ) );
    if( p2 != InpIt<int>(array + 5) ) FAIL;

    InpIt<int> p3( std::find( InpIt<int>(array), InpIt<int>(array), 1 ) );
    if( p3 != InpIt<int>(array) ) FAIL;

    InpIt<int> p4( std::find_if( InpIt<int>(array), InpIt<int>(array + 5),
                                 std::bind2nd( std::greater< int >( ), 3 ) ) );
    if( p4 == InpIt<int>(array + 5) || *p4 != 4 ) FAIL;

    InpIt<int> p5( std::find_if( InpIt<int>(array), InpIt<int>(array + 5),
                                 is_odd ) );
    if( p5 == InpIt<int>(array + 5) || *p5 != 1 ) FAIL;

    return( true );
}


bool find_end_test( )
{
    int a[] = { 1, 1, 1, 1, 1, 1 };
    int b[] = { 1, 1, 1 };
    int c[] = { 2, 4, 6, 8, 10, 12 };
    int d[] = { 6, 8 };
    int e[] = { 2, 4, 6, 8, 10, 12 };
    int f[] = { 3, 6, 6, 8, 6, 5, 8, 6, 8, 7 };    //10 elements
    int g[] = { 10 };

    //find last of multiple matches
    if( std::find_end( FwdIt<int>(a), FwdIt<int>(a+6),
                       FwdIt<int>(b), FwdIt<int>(b+3) ) != FwdIt<int>(a+3) ) FAIL
    //can't find because subsequence is longer than sequence
    if( std::find_end( FwdIt<int>(b), FwdIt<int>(b+3),
                       FwdIt<int>(a), FwdIt<int>(a+6) ) != FwdIt<int>(b+3) ) FAIL
    //can't find because no match
    if( std::find_end( FwdIt<int>(a), FwdIt<int>(a+6),
                       FwdIt<int>(d), FwdIt<int>(d+2) ) != FwdIt<int>(a+6) ) FAIL
    //no match, same size sequences
    if( std::find_end( FwdIt<int>(a), FwdIt<int>(a+6),
                       FwdIt<int>(c), FwdIt<int>(c+6) ) != FwdIt<int>(a+6) ) FAIL
    //exact matching sequence
    if( std::find_end( FwdIt<int>(c), FwdIt<int>(c+6),
                       FwdIt<int>(e), FwdIt<int>(e+6) ) != FwdIt<int>(c) ) FAIL
    //find in middle
    if( std::find_end( FwdIt<int>(c), FwdIt<int>(c+6),
                       FwdIt<int>(d), FwdIt<int>(d+2) ) != FwdIt<int>(c+2) ) FAIL
    //multi matches
    if( std::find_end( FwdIt<int>(f), FwdIt<int>(f+10),
                       FwdIt<int>(d), FwdIt<int>(d+2) ) != FwdIt<int>(f+7) ) FAIL
    //substring is only 1 long
    if( std::find_end( FwdIt<int>(c), FwdIt<int>(c+6),
                       FwdIt<int>(g), FwdIt<int>(g+1) ) != FwdIt<int>(c+4) ) FAIL

    //quick test with predictate, there should realy be more but the
    //predicate version of find_end is just a copy of the non-predicate
    //version, as so long as any updates are applied to both functions it
    //will be ok.
    int h[] = { 2,3 };
    if( std::find_end( FwdIt<int>(c), FwdIt<int>(c+6),
                       FwdIt<int>(h), FwdIt<int>(h+2), is_divisible ) !=
                                                         FwdIt<int>(c+4) ) FAIL

    return( true );
}


bool find_first_of_test( )
{
    using namespace std;
    string s("the cat sat\non the mat");
    char* s2 = "cmo";
    char* n1 = "xyz";
    string::iterator i, j, k;
    i = find_first_of( s.begin(), s.end(), s2, s2+3 );
    k = s.begin() + 4;
    if( *i != s[4] || i != k ) FAIL
    //don't find anything
    i = find_first_of( s.begin(), s.end(), n1, n1+3 );
    if( i != s.end() ) FAIL
  
    string s3("sat\non");
    string s4;
    string ws(" \n");
    //skip past 2 spaces
    i = find_first_of( s.begin(), s.end(), ws.begin(), ws.end() );
    ++i;
    i = find_first_of( i, s.end(), ws.begin(), ws.end() );
    ++i;
    //skip another 2 spaces
    j = find_first_of( i, s.end(), ws.begin(), ws.end() );
    ++j;
    j = find_first_of( j, s.end(), ws.begin(), ws.end() );
    while( i != j ) s4 += *i++;
    if( s3 != s4 ) FAIL
  
    // test binary predictate version:
    int x[] = { 3, 12, 17, 19 };
    int y[] = { 5, 4, 7, 11 };
    FwdIt<int> i2;
    //find a x that is evenly divisible by a y
    i2 = find_first_of( FwdIt<int>(x), FwdIt<int>(x+4),
                        FwdIt<int>(y), FwdIt<int>(y+4), is_divisible );
    if( i2 != FwdIt<int>(x+1) ) FAIL
    //don't find one
    i2 = find_first_of( FwdIt<int>(x), FwdIt<int>(x+4),
                        FwdIt<int>(y+2), FwdIt<int>(y+4), is_divisible );
    if( i2 != FwdIt<int>(x+4) ) FAIL
  
    return( true );
}


bool adjacent_find_test( )
{
    int a[] = { 1, 2, 3, 4 };
    int b[] = { 1, 1, 2, 3 };
    int c[] = { 1, 2, 3, 3 };
    int d[] = { 5, 9, 3, 2 };
    FwdIt<int> p;

    // Sequence of length 0.
    p = std::adjacent_find( FwdIt<int>(a), FwdIt<int>(a) );
    if( p != FwdIt<int>(a) ) FAIL;
    // Sequence of length 1.
    p = std::adjacent_find( FwdIt<int>(a), FwdIt<int>(a + 1) );
    if( p != FwdIt<int>(a + 1) ) FAIL;
    // Sequence of length 2, no match.
    p = std::adjacent_find( FwdIt<int>(a), FwdIt<int>(a + 2) );
    if( p != FwdIt<int>(a + 2) ) FAIL;
    // Long sequence, no match.
    p = std::adjacent_find( FwdIt<int>(a), FwdIt<int>(a + 4) );
    if( p != FwdIt<int>(a + 4) ) FAIL;
    // Sequence of length two, with match.
    p = std::adjacent_find( FwdIt<int>(b), FwdIt<int>(b + 2) );
    if( p != FwdIt<int>(b) ) FAIL;
    // Long sequence, match early.
    p = std::adjacent_find( FwdIt<int>(b), FwdIt<int>(b + 4) );
    if( p != FwdIt<int>(b) ) FAIL;
    // Long sequence, match late.
    p = std::adjacent_find( FwdIt<int>(c), FwdIt<int>(c + 4) );
    if( p != FwdIt<int>(c + 2) ) FAIL;
    // Predicate check.
    p = std::adjacent_find( FwdIt<int>(d), FwdIt<int>(d + 4), is_divisible );
    if( p != FwdIt<int>(d + 1) ) FAIL;

    return( true );
}


bool count_test( )
{
    char a[] = { 'a', 'b', 'b', 'd', 'e' };
    int n = -1;

    n = std::count( InpIt<char>(a), InpIt<char>(a + 5), 'b' );
    if( n != 2 ) FAIL;

    n = std::count( InpIt<char>(a), InpIt<char>(a + 5), 'c' );
    if( n != 0) FAIL;

    n = std::count( InpIt<char>(a), InpIt<char>(a), 'a' );
    if( n != 0 ) FAIL;

    n = std::count_if( InpIt<char>(a), InpIt<char>(a + 5), is_odd );
    if( n != 2 ) FAIL;

    n = std::count_if(InpIt<char>(a), InpIt<char>(a + 5),
                      std::bind1st( std::less< char >( ), 'b' ) );
    if( n != 2 ) FAIL;

    return( true );
}


bool mismatch_test( )
{
    using namespace std;

    // Does this local typedef cause a problem?
    typedef pair< InpIt<int>, InpIt<int> > result_type;

    int a[] = { 1, 2, 3, 4 };
    int b[] = { 0, 2, 3, 4 };
    int c[] = { 1, 2, 3, 5 };
    int d[] = { 5, 4, 5, 8 };

    result_type result;

    // First sequence of zero length.
    result = mismatch( InpIt<int>(a), InpIt<int>(a), InpIt<int>(a) );
    if( result.first != InpIt<int>(a) ) FAIL;

    // First sequence of length one, no mismatch.
    result = mismatch( InpIt<int>(a), InpIt<int>(a + 1), InpIt<int>(a) );
    if( result.first != InpIt<int>(a + 1) ) FAIL;
    // First sequence long, no mismatch.
    result = mismatch( InpIt<int>(a), InpIt<int>(a + 4), InpIt<int>(a) );
    if( result.first != InpIt<int>(a + 4) ) FAIL;
    // First sequence of length one, with mismatch.
    result = mismatch( InpIt<int>(b), InpIt<int>(b + 1), InpIt<int>(a) );
    if( result.first != InpIt<int>(b) && result.second != InpIt<int>(a) ) FAIL;
    // First sequence long, mismatch in first position.
    result = mismatch( InpIt<int>(b), InpIt<int>(b + 4), InpIt<int>(a) );
    if( result.first != InpIt<int>(b) && result.second != InpIt<int>(a) ) FAIL;
    // First sequence long, mismatch at the end.
    result = mismatch( InpIt<int>(c), InpIt<int>(c + 4), InpIt<int>(a) );
    if( result.first != InpIt<int>(c + 3) &&
        result.second != InpIt<int>(a + 3) ) FAIL;
    // Predicate check.
    result = mismatch( InpIt<int>(d), InpIt<int>(d + 4), InpIt<int>(a),
                       is_divisible );
    if( result.first != InpIt<int>(d + 2) &&
        result.second != InpIt<int>(a + 2) ) FAIL;

    return( true );
}


bool equal_test( )
{
    int a1[] = { 0,  1,  2,  3,  4 };
    int a2[] = { 0,  1,  2,  3,  4 };
    int a3[] = { 0,  1,  3,  3,  4 };
    int a4[] = { 0, -1, -2, -3, -4 };

    if( !std::equal( InpIt<int>(a1), InpIt<int>(a1 + 5),
                     InpIt<int>(a2) ) ) FAIL;
    if(  std::equal( InpIt<int>(a1), InpIt<int>(a1 + 3),
                     InpIt<int>(a3) ) ) FAIL;
    if( !std::equal( InpIt<int>(a1), InpIt<int>(a1),
                     InpIt<int>(a2) ) ) FAIL;
    if( !std::equal( InpIt<int>(a1), InpIt<int>(a1 + 5),
                     InpIt<int>(a4), std::greater_equal< int >( ) ) ) FAIL;

    return( true );
}


bool search_test( )
{
    using std::search;

    int  haystack[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    int  needle1[]  = { 7, 8,  9 };
    int  needle2[]  = { 7, 8, 10 };
    int *result;

    // Normal case; search string found.
    result = search( FwdIt<int>(haystack), FwdIt<int>(haystack + 10),
                     FwdIt<int>(needle1),  FwdIt<int>(needle1 + 3) ).get( );
    if( result != haystack + 7 ) FAIL;

    // Normal case; search string not found.
    result = search( FwdIt<int>(haystack), FwdIt<int>(haystack + 10),
                     FwdIt<int>(needle2),  FwdIt<int>(needle2 + 3) ).get( );
    if( result != haystack + 10 ) FAIL;

    // Search string only one item long.
    result = search( FwdIt<int>(haystack), FwdIt<int>(haystack + 10),
                     FwdIt<int>(needle1),  FwdIt<int>(needle1 + 1) ).get( );
    if( result != haystack + 7 ) FAIL;

    // Search string zero items long. Is the behavior actually correct? Standard unclear.
    result = search( FwdIt<int>(haystack), FwdIt<int>(haystack + 10),
                     FwdIt<int>(needle1),  FwdIt<int>(needle1) ).get( );
    if( result != haystack + 10 ) FAIL;

    // Target string one item long.
    result = search( FwdIt<int>(haystack), FwdIt<int>(haystack + 1),
                     FwdIt<int>(needle1),  FwdIt<int>(needle1 + 3) ).get( );
    if( result != haystack + 1 ) FAIL;

    // Target string zero items long.
    result = search( FwdIt<int>(haystack), FwdIt<int>(haystack),
                     FwdIt<int>(needle1),  FwdIt<int>(needle1 + 3) ).get( );
    if ( result != haystack ) FAIL;

    return( true );
}


bool search_n_test( )
{
    using std::search_n;

    int haystack[] = { 0, 1, 2, 3, 4, 5, 6, 7, 7, 7 };
    int *result;

    // Normal case; search string found.
    result = search_n( FwdIt<int>(haystack), FwdIt<int>(haystack + 10), 3, 7 ).get( );
    if( result != haystack + 7 ) FAIL;

    // Normal case; search string not found.
    result = search_n( FwdIt<int>(haystack), FwdIt<int>(haystack + 10), 4, 7 ).get( );
    if( result != haystack + 10 ) FAIL;

    // Search string only one item long.
    result = search_n( FwdIt<int>(haystack), FwdIt<int>(haystack + 10), 1, 7 ).get( );
    if( result != haystack + 7 ) FAIL;

    // Search string zero items long. Is the behavior actually correct? Standard unclear.
    result = search_n( FwdIt<int>(haystack), FwdIt<int>(haystack + 10), 0, 7 ).get( );
    if( result != haystack + 10 ) FAIL;

    // Target string one item long.
    result = search_n( FwdIt<int>(haystack), FwdIt<int>(haystack + 1), 3, 7 ).get( );
    if( result != haystack + 1 ) FAIL;

    // Target string zero items long.
    result = search_n( FwdIt<int>(haystack), FwdIt<int>(haystack), 3, 7 ).get( );
    if ( result != haystack ) FAIL;

    return( true );
}


int main( )
{
    int rc = 0;
    int original_count = heap_count( );

    try {
        if( !all_of_test( )         || !heap_ok( "t01" ) ) rc = 1;
        if( !any_of_test( )         || !heap_ok( "t02" ) ) rc = 1;
        if( !none_of_test( )        || !heap_ok( "t03" ) ) rc = 1;
        if( !for_each_test( )       || !heap_ok( "t04" ) ) rc = 1;
        if( !find_test( )           || !heap_ok( "t05" ) ) rc = 1;
        if( !find_end_test( )       || !heap_ok( "t06" ) ) rc = 1;
        if( !find_first_of_test( )  || !heap_ok( "t07" ) ) rc = 1;
        if( !adjacent_find_test( )  || !heap_ok( "t08" ) ) rc = 1;
        if( !count_test( )          || !heap_ok( "t09" ) ) rc = 1;
        if( !mismatch_test( )       || !heap_ok( "t10" ) ) rc = 1;
        if( !equal_test( )          || !heap_ok( "t11" ) ) rc = 1;
        if( !search_test( )         || !heap_ok( "t12" ) ) rc = 1;
        if( !search_n_test( )       || !heap_ok( "t13" ) ) rc = 1;
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
