/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
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
* Description: Benchmark program that exercises std::sort and compares
*              its behavior with the C library's qsort().
*
****************************************************************************/

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>

using namespace std;

#define SORT       std::sort  // Name of the STL-like sorting function.
#define N          1000000    // Size of array to sort.
#define TEST_COUNT 20         // Number of times array is sorted.

// Make a pseudo-random array of integers.
void make_random( int *p1, int *p2 )
{
  srand( 0 );
  while( p1 != p2 ) {
    *p1++ = rand();
  }
}

// Make a sorted array of integers.
void make_sorted( int *p1, int *p2 )
{
  int value = 0;
  while( p1 != p2 ) {
    *p1++ = value++;
  }
}

// Make a pseudo-random array of std::strings.
void make_random_strings( std::string *p1, std::string *p2 )
{
  srand( 0 );
  while( p1 != p2 ) {
    for( int i = 0; i < 50; ++i ) {
      p1->push_back( static_cast<char>( 'a' + ( rand() % 26 ) ) );
    }
    ++p1;
  }
}

// Comparison function for qsort( ).
int int_compare( const void *left, const void *right )
{
  const int *x = ( const int * )left;
  const int *y = ( const int * )right;
  if( *x < *y ) return -1;
  if( *x > *y ) return +1;
  return 0;
}

// Use the C++ library std::sort (or something similar) to sort array of Ts.
template< class T >
void cpp_sort(T *working, T *holding, int size, char *caption)
{
  clock_t interval = clock();

  for(int i = 0; i < TEST_COUNT; ++i) {
    copy( holding, holding + size, working);
    SORT( working, working + size );
  }
  interval = clock() - interval;
  std::cout << "std::sort => " << caption << ": "
            << interval/static_cast<double>( CLOCKS_PER_SEC ) << "\n";
}

// Use C library qsort() to sort arrays of integers.
void c_sort(int *working, int *holding, int size, char *caption)
{
  clock_t interval = clock();

  for(int i = 0; i < TEST_COUNT; ++i) {
    memcpy( working, holding, size * sizeof(int) );
    qsort( working, size, sizeof(int), int_compare );
  }
  interval = clock( ) - interval;
  std::cout << "qsort     => " << caption << ": "
            << interval/static_cast<double>( CLOCKS_PER_SEC ) << "\n";
}

int main( )
{
  int *int_holding = new int[N];
  int *int_working = new int[N];
  std::string *string_holding = new std::string[N];
  std::string *string_working = new std::string[N];

  make_random( int_holding, int_holding + N );
  cpp_sort( int_working, int_holding, N, "random" );
  c_sort( int_working, int_holding, N, "random" );

  make_sorted( int_holding, int_holding + N );
  cpp_sort( int_working, int_holding, N, "sorted" );
  c_sort( int_working, int_holding, N, "sorted" );

  make_random_strings( string_holding, string_holding + N );
  cpp_sort( string_working, string_holding, N, "string" );

  delete [] int_working;
  delete [] int_holding;
  delete [] string_working;
  delete [] string_holding;
  return 0;
}

