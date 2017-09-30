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
* Description:  This file tests sorting and searching algorithms.
****************************************************************************/

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "itcat.h"
#include "sanity.cpp"

//
// Sort test cases can contain no more than this many elements.
//
const int MAX_SIZE = 32;

//
// The following structure holds information about a single test case.
//
struct test_case {
    char *title;              // Something to print when reporting results.
    int   input[MAX_SIZE];    // The array to sort.
    int   expected[MAX_SIZE]; // The expected result.
    int   size;               // The number of elements in input I care about.
};

// The test cases. The current version of std::sort uses QuickSort but falls over to
// InsertionSort on subsequences of length <= 10. Thus it is important to use test cases that
// are longer than 10 to properly exercise both algorithms.
//
// Note that the 'title' member is no longer used but it is retained for documentation and
// possible future use.
//
const struct test_case tests[] = {

    // The first few test cases are short. They only exercise the InsertionSort subset of
    // std::sort's behavior.

    { "SHORT: An empty sequence",
        { 0 },        // Open Watcom v1.5 doesn't accept { } as an initializer.
        { 0 }, 0 },

    { "SHORT: One element", 
        { 1 },
        { 1 }, 1 },

    { "SHORT: Two elements",
        { 2, 1 },
        { 1, 2 }, 2 },

    { "SHORT: Three elements",
        { 2, 3, 1 },
        { 1, 2, 3 }, 3 },

    { "SHORT: Random elements, size 2^n",
        { 4, 2, 8, 5, 3, 7, 6, 1 },
        { 1, 2, 3, 4, 5, 6, 7, 8 }, 8 },

    { "SHORT: Random elements, size 2^n + 1",
        { 1, 8, 3, 2, 4, 9, 5, 7, 6 },
        { 1, 2, 3, 4, 5, 6, 7, 8, 9 }, 9 },

    { "SHORT: Random elements, size 2^n - 1",
        { 4, 2, 5, 3, 1, 7, 6 },
        { 1, 2, 3, 4, 5, 6, 7 }, 7 },

    { "SHORT: Already sorted",
        { 1, 2, 3, 4, 5 },
        { 1, 2, 3, 4, 5 }, 5 },

    { "SHORT: Already reverse sorted",
        { 5, 4, 3, 2, 1 },
        { 1, 2, 3, 4, 5 }, 5 },

    { "SHORT: One pair of duplicate elements",
        { 1, 2, 4, 2, 3 },
        { 1, 2, 2, 3, 4 }, 5 },

    { "SHORT: All duplicate elements",
        { 1, 1, 1, 1, 1 },
        { 1, 1, 1, 1, 1 }, 5 },

    // The following tests are all length 11. This is the first length at which a partitioning
    // is done.

    { "LONG: Random elements; median3 in middle",
        {  3,  4,  1,  9, 10,  5,  7, 11,  2,  6,  8 },
        {  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11 }, 11 },

    { "LONG: Random elements; median3 on left",
        {  5,  4,  1,  9, 10,  8,  7, 11,  2,  6,  3 },
        {  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11 }, 11 },

    { "LONG: Random elements; median3 on right",
        {  3,  4,  1,  9, 10,  8,  7, 11,  2,  6,  5 },
        {  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11 }, 11 },

    { "LONG: Already sorted",
        {  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11 },
        {  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11 }, 11 },

    { "LONG: Already reversed sorted",
        { 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1 },
        {  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11 }, 11 },

    { "LONG: All duplicate elements",
        {  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1 },
        {  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1 }, 11 },

    // The following tests are all long enough to cause multiple recursions.

    { "VERY LONG: Random elements",
        { 4, 18, 5, 1, 9, 7, 12, 20, 6, 13, 2, 3, 14, 8, 10, 15, 11, 19, 17, 16, 21 },
        { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21 }, 21 }
};
const int number_cases = sizeof(tests)/sizeof(test_case);


// The following test cases exercise reverse sorts. They allow the sorting algorithm that takes
// a comparison object to be exercised.
//
const struct test_case reverse_tests[] = {

    // The first few test cases are short. They only exercise the InsertionSort subset of
    // std::sort's behavior.

    { "SHORT: An empty sequence",
        { 0 },        // Open Watcom v1.5 doesn't accept { } as an initializer.
        { 0 }, 0 },

    { "SHORT: One element", 
        { 1 },
        { 1 }, 1 },

    { "SHORT: Two elements",
        { 1, 2 },
        { 2, 1 }, 2 },

    { "SHORT: Three elements",
        { 2, 3, 1 },
        { 3, 2, 1 }, 3 },

    { "SHORT: Random elements, size 2^n",
        { 4, 2, 8, 5, 3, 7, 6, 1 },
        { 8, 7, 6, 5, 4, 3, 2, 1 }, 8 },

    { "SHORT: Random elements, size 2^n + 1",
        { 1, 8, 3, 2, 4, 9, 5, 7, 6 },
        { 9, 8, 7, 6, 5, 4, 3, 2, 1 }, 9 },

    { "SHORT: Random elements, size 2^n - 1",
        { 4, 2, 5, 3, 1, 7, 6 },
        { 7, 6, 5, 4, 3, 2, 1 }, 7 },

    { "SHORT: Already sorted",
        { 5, 4, 3, 2, 1 },
        { 5, 4, 3, 2, 1 }, 5 },

    { "SHORT: Already reverse sorted",
        { 1, 2, 3, 4, 5 },
        { 5, 4, 3, 2, 1 }, 5 },

    { "SHORT: One pair of duplicate elements",
        { 1, 2, 4, 2, 3 },
        { 4, 3, 2, 2, 1 }, 5 },

    { "SHORT: All duplicate elements",
        { 1, 1, 1, 1, 1 },
        { 1, 1, 1, 1, 1 }, 5 },

    // The following tests are all length 11. This is the first length at which a partitioning
    // is done.

    { "LONG: Random elements; median3 in middle",
        {  3,  4,  1,  9, 10,  5,  7, 11,  2,  6,  8 },
        { 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1 }, 11 },

    { "LONG: Random elements; median3 on left",
        {  5,  4,  1,  9, 10,  8,  7, 11,  2,  6,  3 },
        { 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1 }, 11 },

    { "LONG: Random elements; median3 on right",
        {  3,  4,  1,  9, 10,  8,  7, 11,  2,  6,  5 },
        { 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1 }, 11 },

    { "LONG: Already sorted",
        { 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1 },
        { 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1 }, 11 },

    { "LONG: Already reversed sorted",
        {  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11 },
        { 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1 }, 11 },

    { "LONG: All duplicate elements",
        {  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1 },
        {  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1 }, 11 },

    // The following tests are all long enough to cause multiple recursions.

    { "VERY LONG: Random elements",
        {  4, 18,  5,  1,  9,  7, 12, 20,  6, 13,  2,  3, 14, 8, 10, 15, 11, 19, 17, 16, 21 },
        { 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10,  9, 8,  7,  6,  5,  4,  3,  2,  1 }, 21 }
};
const int number_reverse_cases = sizeof(reverse_tests)/sizeof(test_case);

bool heap_test( )
{
    // Since sorting a heap gives most of the other heap related code a good workout, I will
    // "borrow" the sort test cases to do a quick and dirty heap test. At some point it might be
    // nice to build a more heap- specific test.

    // First I must copy the test cases to avoid leaving them sorted.
    test_case *tc = new test_case[number_cases];
    for( int i = 0; i < number_cases; ++i ) tc[i] = tests[i];

    // For each test...
    for( int i = 0; i < number_cases; ++i ) {

        // Do the test.
        std::make_heap( &tc[i].input[0], &tc[i].input[tc[i].size] );
        std::sort_heap( &tc[i].input[0], &tc[i].input[tc[i].size] );

        // Did it work?
        bool worked = true;
        for( int j = 0; j < tc[i].size; ++j ) {
            if ( tc[i].input[j] != tc[i].expected[j] ) worked = false;
        }
        if ( !worked ) FAIL;
    }

    delete [] tc;
    return( true );
}

bool reverse_heap_test( )
{
    // First I must copy the test cases to avoid leaving them sorted.
    test_case *tc = new test_case[number_reverse_cases];
    for( int i = 0; i < number_reverse_cases; ++i ) tc[i] = reverse_tests[i];

    // For each test...
    for( int i = 0; i < number_reverse_cases; ++i ) {

        // Do the test.
        std::make_heap( &tc[i].input[0], &tc[i].input[tc[i].size], std::greater< int >( ) );
        std::sort_heap( &tc[i].input[0], &tc[i].input[tc[i].size], std::greater< int >( ) );

        // Did it work?
        bool worked = true;
        for( int j = 0; j < tc[i].size; ++j ) {
            if ( tc[i].input[j] != tc[i].expected[j] ) worked = false;
        }
        if ( !worked ) FAIL;
    }

    delete [] tc;
    return( true );
}

bool sort_test( )
{
    // First I must copy the test cases to avoid leaving them sorted.
    test_case *tc = new test_case[number_cases];
    for( int i = 0; i < number_cases; ++i ) tc[i] = tests[i];

    // For each test...
    for( int i = 0; i < number_cases; ++i ) {

        // Do the test.
        std::sort( &tc[i].input[0], &tc[i].input[tc[i].size] );

        // Did it work?
        bool worked = true;
        for( int j = 0; j < tc[i].size; ++j ) {
            if ( tc[i].input[j] != tc[i].expected[j] ) worked = false;
        }
        if ( !worked ) FAIL;
    }

    delete [] tc;
    return( true );
}

bool reverse_sort_test( )
{
    // First I must copy the test cases to avoid leaving them sorted.
    test_case *tc = new test_case[number_reverse_cases];
    for( int i = 0; i < number_reverse_cases; ++i ) tc[i] = reverse_tests[i];

    // For each test...
    for( int i = 0; i < number_reverse_cases; ++i ) {

        // Do the test.
        std::sort( &tc[i].input[0], &tc[i].input[tc[i].size], std::greater< int >( ) );

        // Did it work?
        bool worked = true;
        for( int j = 0; j < tc[i].size; ++j ) {
            if ( tc[i].input[j] != tc[i].expected[j] ) worked = false;
        }
        if ( !worked ) FAIL;
    }

    delete [] tc;
    return( true );
}

bool bsearch_test( )
{
    using std::binary_search;
    
    static const int data[] = { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18 };
    
    if(  binary_search( data, data +  0,  2 ) ) FAIL;
    if( !binary_search( data, data +  1,  0 ) ) FAIL;
    if(  binary_search( data, data +  1,  2 ) ) FAIL;
    if( !binary_search( data, data +  2,  0 ) ) FAIL;
    if( !binary_search( data, data +  2,  2 ) ) FAIL;
    if(  binary_search( data, data +  2,  4 ) ) FAIL;
    if( !binary_search( data, data +  3,  0 ) ) FAIL;
    if( !binary_search( data, data +  3,  2 ) ) FAIL;
    if( !binary_search( data, data +  3,  4 ) ) FAIL;
    if(  binary_search( data, data +  3,  6 ) ) FAIL;
    if( !binary_search( data, data + 10,  0 ) ) FAIL;
    if( !binary_search( data, data + 10, 18 ) ) FAIL;
    if( !binary_search( data, data + 10,  8 ) ) FAIL;
    if( !binary_search( data, data + 10, 10 ) ) FAIL;
    if( !binary_search( data, data + 10,  4 ) ) FAIL;
    if( !binary_search( data, data + 10, 12 ) ) FAIL;
    if(  binary_search( data, data + 10,  1 ) ) FAIL;
    if(  binary_search( data, data + 10, -1 ) ) FAIL;
    if(  binary_search( data, data + 10, 17 ) ) FAIL;
    if(  binary_search( data, data + 10, 19 ) ) FAIL;
    if(  binary_search( data, data + 10,  9 ) ) FAIL;
    return( true );
}


bool lexicographical_test( )
{
    using namespace std;

    int a1[] = { 0 };
    int a2[] = { 1, 2, 3, 4 };
    int a3[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    int a4[] = { 0, 2, 3, 4, 5, 6, 7, 8 };
    int a5[] = { 1, 0, 3, 4 };

    if(  lexicographical_compare( InpIt<int>( a1 ), InpIt<int>( a1 + 0 ),
                                  InpIt<int>( a1 ), InpIt<int>( a1 + 0 ) ) ) FAIL;
    if(  lexicographical_compare( InpIt<int>( a2 ), InpIt<int>( a2 + 4 ),
                                  InpIt<int>( a2 ), InpIt<int>( a2 + 4 ) ) ) FAIL;
    if( !lexicographical_compare( InpIt<int>( a1 ), InpIt<int>( a1 + 0 ),
                                  InpIt<int>( a2 ), InpIt<int>( a2 + 4 ) ) ) FAIL;
    if(  lexicographical_compare( InpIt<int>( a2 ), InpIt<int>( a2 + 4 ),
                                  InpIt<int>( a1 ), InpIt<int>( a1 + 0 ) ) ) FAIL;
    if( !lexicographical_compare( InpIt<int>( a2 ), InpIt<int>( a2 + 4 ),
                                  InpIt<int>( a3 ), InpIt<int>( a3 + 8 ) ) ) FAIL;
    if(  lexicographical_compare( InpIt<int>( a3 ), InpIt<int>( a3 + 8 ),
                                  InpIt<int>( a2 ), InpIt<int>( a2 + 4 ) ) ) FAIL;
    if(  lexicographical_compare( InpIt<int>( a3 ), InpIt<int>( a3 + 8 ),
                                  InpIt<int>( a4 ), InpIt<int>( a4 + 8 ) ) ) FAIL;
    if( !lexicographical_compare( InpIt<int>( a4 ), InpIt<int>( a4 + 8 ),
                                  InpIt<int>( a3 ), InpIt<int>( a3 + 8 ) ) ) FAIL;
    if( !lexicographical_compare( InpIt<int>( a4 ), InpIt<int>( a4 + 8 ),
                                  InpIt<int>( a5 ), InpIt<int>( a5 + 4 ) ) ) FAIL;
    if(  lexicographical_compare( InpIt<int>( a5 ), InpIt<int>( a5 + 4 ),
                                  InpIt<int>( a4 ), InpIt<int>( a4 + 8 ) ) ) FAIL;

    // Sequences in different types of containers.
    vector< int > v1( a3, a3 + 8 );
    if(  !lexicographical_compare( a2, a2 + 4, v1.begin( ), v1.end( ) ) ) FAIL;
    if(   lexicographical_compare( v1.begin( ), v1.end( ), a2, a2 + 4 ) ) FAIL;

    return( true );
}


bool permutation_test( )
{
    bool permutation_result;
    std::string permutations[] = {
        "abcd", "abdc", "acbd", "acdb", "adbc", "adcb",
        "bacd", "badc", "bcad", "bcda", "bdac", "bdca",
        "cabd", "cadb", "cbad", "cbda", "cdab", "cdba",
        "dabc", "dacb", "dbac", "dbca", "dcab", "dcba"
    };

    // First try going forward through all possible permutations.
    std::string workspace( permutations[0] );
    for( int i = 1; i < 24; ++i ) {
        permutation_result =
            std::next_permutation( workspace.begin( ), workspace.end( ) );
        if( !permutation_result ) FAIL;
        if( workspace != permutations[i] ) FAIL;
    }
    // Check wrap-around behavior.
    permutation_result =
        std::next_permutation( workspace.begin( ), workspace.end( ) );
    if( permutation_result ) FAIL;
    if( workspace != permutations[0] ) FAIL;

    // Now try going backward though those same permutations.
    workspace = permutations[23];
    for( int i = 22; i >= 0; --i ) {
        permutation_result =
            std::prev_permutation( workspace.begin( ), workspace.end( ) );
        if( !permutation_result ) FAIL;
        if( workspace != permutations[i] ) FAIL;
    }
    // Check wrap-around behavior.
    permutation_result =
        std::prev_permutation( workspace.begin( ), workspace.end( ) );
    if( permutation_result ) FAIL;
    if( workspace != permutations[23] ) FAIL;
    return( true );
}


int main( )
{
    int rc = 0;
    int original_count = heap_count( );

    try {
        if( !heap_test( )            || !heap_ok( "t01" ) ) rc = 1;
        if( !reverse_heap_test( )    || !heap_ok( "t02" ) ) rc = 1;
        if( !sort_test( )            || !heap_ok( "t03" ) ) rc = 1;
        if( !reverse_sort_test( )    || !heap_ok( "t04" ) ) rc = 1;
        if( !bsearch_test( )         || !heap_ok( "t05" ) ) rc = 1;
        if( !lexicographical_test( ) || !heap_ok( "t06" ) ) rc = 1;
        if( !permutation_test( )     || !heap_ok( "t07" ) ) rc = 1;
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
