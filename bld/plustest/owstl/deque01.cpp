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
* Description: This file contains the functional tests for std::deque.
*
****************************************************************************/

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <deque>

#include "sanity.cpp"

bool construction_test( )
{
  bool rc = true;
  std::deque< int > d1;
  std::deque< int > d2(10);
  std::deque< int > d3(d1);
  std::deque< int > d4(d2);
  std::deque< int > d5(10, 1);

  if( d1.size( ) !=  0 || !d1.empty( ) || INSANE( d1 ) ) FAIL
  if( d2.size( ) != 10 ||  d2.empty( ) || INSANE( d2 ) ) FAIL
  if( d3.size( ) !=  0 || !d3.empty( ) || INSANE( d3 ) ) FAIL
  if( d4.size( ) != 10 ||  d4.empty( ) || INSANE( d4 ) ) FAIL
  if( d5.size( ) != 10 ||  d5.empty( ) || INSANE( d5 ) ) FAIL

  // Use checked access so that we get an exception if something's wrong.
  for( std::deque< int >::size_type i = 0; i < d5.size( ); ++i ) {
    if( d5.at( i ) != 1 ) FAIL
  }

  // Try a non-contiguous copy construction. The test deque has size 10
  // which is less than the size where a reallocation is done (15).
  //
  std::deque< int > d6;
  for( std::deque< int >::size_type i = 6; i <= 10; ++i ) {
    d6.push_back( static_cast< int >( i ) );
  }
  for( std::deque< int >::size_type i = 5; i >= 1; --i ) {
    d6.push_front( static_cast< int >( i ) );
  }
  std::deque< int > d7( d6 );
  if( d7.size( ) != 10 || INSANE( d7 ) ) FAIL;
  for( std::deque< int >::size_type i = 0; i < 10; ++i ) {
    if( d6[i] != i + 1 ) FAIL;
  }

  return( rc );
}

bool access_test( )
{
  using std::out_of_range;

  bool rc = true;
  std::deque< int > d(10);

  for (int i = 0; i < 10; ++i )     d[i]  = i;
  for (int i = 0; i < 10; ++i ) if( d[i] != i ) FAIL
  for (int i = 0; i < 10; ++i )     d.at( i )  = i;
  for (int i = 0; i < 10; ++i ) if( d.at( i ) != i ) FAIL

  try {
    d.at( 10 ) = 0;
    FAIL
  }
  catch( out_of_range ) {
    // Okay
  }

  return( rc );
}

#ifdef __NEVER

bool assign_test( )
{
  bool rc = true;
  std::deque< int > d1(10, 1);
  std::deque< int > d2( 5, 2);
  std::deque< int > d3(15, 3);

  d1 = d2;
  if( d1.size( ) != 5  || INSANE( d1 ) ) FAIL;

  for( std::deque< int >::size_type i = 0; i < d1.size( ); ++i ) {
    if( d1.at( i ) != 2 ) FAIL;
  }

  d1 = d3;
  if( d1.size( ) != 15 || INSANE( d1 ) ) FAIL;

  for( std::deque< int >::size_type i = 0; i < d1.size( ); ++i ) {
    if( d1.at( i ) != 3 ) FAIL;
  }

  d1.assign( 10, 4 );
  if( d1.size( ) != 10 || INSANE( d1 ) ) FAIL;

  for( std::deque< int >::size_type i = 0; i < d1.size( ); ++i ) {
    if( d1.at( i ) != 4 ) FAIL;
  }

  d1.assign( 20, 5 );
  if( d1.size( ) != 20 || INSANE( d1 ) ) FAIL;

  for( std::deque< int >::size_type i = 0; i < d1.size( ); ++i ) {
    if( d1.at( i ) != 5 ) FAIL;
  }

  return( rc );
}

#endif

template< class Type >
bool pushfront_test(const Type *check, std::size_t check_size )
{
  using std::deque;  // Make sure this works.

  bool rc = true;
  deque< Type > deq;

  for( typename deque< Type >::size_type i = 0; i < check_size; ++i ) {
    deq.push_front( check[i] );
    if( deq.back( )  != check[0] ) FAIL;
    if( deq.front( ) != check[i] ) FAIL;
  }
  if( deq.size( ) != check_size || INSANE( deq ) ) FAIL;

  std::reverse( deq.begin( ), deq.end( ) );
  for( typename deque< Type >::size_type i = 0; i < check_size; ++i ) {
    if( deq.back( )  != check[check_size - 1 - i] ) FAIL;
    if( deq.front( ) != check[0] ) FAIL;
    deq.pop_back( );
  }
  if( deq.size( ) != 0 || INSANE( deq ) ) FAIL;

  return( rc );
}

template< class Type >
bool pushback_test(const Type *check, std::size_t check_size )
{
  using std::deque;  // Make sure this works.

  bool rc = true;
  deque< Type > deq;

  for( typename deque< Type >::size_type i = 0; i < check_size; ++i ) {
    deq.push_back( check[i] );
    if( deq.back( )  != check[i] ) FAIL;
    if( deq.front( ) != check[0] ) FAIL;
  }
  if( deq.size( ) != check_size || INSANE( deq ) ) FAIL;

  std::reverse( deq.begin( ), deq.end( ) );
  for( typename deque< Type >::size_type i = 0; i < check_size; ++i ) {
    if( deq.back( )  != check[i] ) FAIL;
    if( deq.front( ) != check[check_size - 1] ) FAIL;
    deq.pop_back( );
  }
  if( deq.size( ) != 0 || INSANE( deq ) ) FAIL;

  return( rc );
}

static bool midscan_iterator_comp(
  std::deque< int >::iterator p, std::deque< int > &deq )
{
  if(    p <  deq.begin( )   ) FAIL;
  if( !( p >= deq.begin( ) ) ) FAIL;
  if(    p >  deq.end( )   ) FAIL;
  if(    p >= deq.end( )   ) FAIL;
  if( !( p <= deq.end( ) ) ) FAIL;
  if( !( p <  deq.end( ) ) ) FAIL;
  if(    deq.end( ) <  p   ) FAIL;
  if(    deq.end( ) <= p   ) FAIL;
  if( !( deq.end( ) >= p ) ) FAIL;
  if( !( deq.end( ) >  p ) ) FAIL;
  return true;
}

static bool endscan_iterator_comp(
  std::deque< int >::iterator p, std::deque< int > &deq )
{
  if(    p == deq.begin( )   ) FAIL;
  if(    p <  deq.begin( )   ) FAIL;
  if(    p <= deq.begin( )   ) FAIL;
  if( !( p >  deq.begin( ) ) ) FAIL;
  if( !( p >= deq.begin( ) ) ) FAIL;
  if(    p <  deq.end( )   ) FAIL;
  if(    p >  deq.end( )   ) FAIL;
  if( !( p == deq.end( ) ) ) FAIL;
  return true;
}

bool iterator_test( )
{
  // These tests consider that the initial size of a deque is 16.

  bool rc = true;
  int  counter;
  std::deque< int > deq1(15);  // Choose maximum size with no reallocation.
  std::deque< int > deq2;
  std::deque< int >::iterator p;

  // Set up deq1 as a contiguous block.
  for( std::deque< int >::size_type i = 0; i < 15; ++i ) {
    deq1[i] = i;
  }
  // Set up deq2 as non-contiguous with one wrap-around value.
  for( std::deque< int >::size_type i = 1; i <= 14; ++i ) {
    deq2.push_back( static_cast< int >( i ) );
  }
  deq2.push_front( 0 );

  // Use ++p
  counter = 0;
  for( p = deq1.begin( ); p != deq1.end( ); ++p ) {
    if( *p != counter ) FAIL;
    if( !midscan_iterator_comp( p, deq1 ) ) return false;
    ++counter;
  }
  if( !endscan_iterator_comp( p, deq1 ) ) return false;

  counter = 0;
  for( p = deq2.begin( ); p != deq2.end( ); ++p ) {
    if( *p != counter ) FAIL;
    if( !midscan_iterator_comp( p, deq2 ) ) return false;
    ++counter;
  }
  if( !endscan_iterator_comp( p, deq2 ) ) return false;

  // Use p++
  counter = 0;
  for( p = deq1.begin( ); p != deq1.end( ); p++ ) {
    if( *p != counter ) FAIL;
    if( !midscan_iterator_comp( p, deq1 ) ) return false;
    ++counter;
  }
  if( !endscan_iterator_comp( p, deq1 ) ) return false;

  counter = 0;
  for( p = deq2.begin( ); p != deq2.end( ); p++ ) {
    if( *p != counter ) FAIL;
    if( !midscan_iterator_comp( p, deq2 ) ) return false;
    ++counter;
  }
  if( !endscan_iterator_comp( p, deq2 ) ) return false;

  // Build a non-contiguous deque (should have a function for this).
  std::deque< int > deq3;
  for( std::deque< int >::size_type i = 6; i <= 10; ++i ) {
    deq3.push_back( static_cast< int >( i ) );
  }
  for( std::deque< int >::size_type i = 5; i >= 1; --i ) {
    deq3.push_front( static_cast< int >( i ) );
  }
  p = deq3.begin( );
  p += 4; if( *p !=  5 ) FAIL;
  p -= 4; if( *p !=  1 ) FAIL;
  p += 5; if( *p !=  6 ) FAIL;
  p += 4; if( *p != 10 ) FAIL;
  p -= 9; if( *p !=  1 ) FAIL;

  p = deq3.begin( );
  if( *( p + 4 ) !=  5 ) FAIL;
  if( *( 5 + p ) !=  6 ) FAIL;
  if( *( p + 9 ) != 10 ) FAIL;
  p = p + 9;
  if( *( p - 4 ) !=  6 ) FAIL;
  if( *( p - 5 ) !=  5 ) FAIL;
  if( *( p - 9 ) !=  1 ) FAIL;

  return( rc );
}

#ifdef __NEVER

bool insert_single_test( )
{
  bool rc = true;
  typedef std::deque< int >::size_type size_type;
  typedef std::deque< int >::iterator iterator;

  // Try inserting repeatedly at the beginning.
  std::deque< int > v1;
  size_type cap = v1.capacity( );
  int       lim = 4 * cap;
  iterator   it = v1.end( );

  for( int i = 0; i < lim; ++i ) {
    it = v1.insert( it, i );
    if( INSANE( v1 ) ) {
      std::cout << "insert_single FAIL 0001\n"; rc = false;
    }
  }
  int expected_value = lim - 1;
  for( size_type i = 0; i < lim; ++i ) {
    if( v1[i] != expected_value ) {
      std::cout << "insert_single FAIL 0002\n"; rc = false;
    }
    --expected_value;
  }

  // Try inserting repeatedly at the end.
  std::deque< int > v2;
  cap = v2.capacity( );
  lim = 4 * cap;
  it  = v2.end( );

  for( int i = 0; i < lim; ++i ) {
    it = v2.insert( it, i );
    ++it;
    if( INSANE( v2 ) ) {
      std::cout << "insert_single FAIL 0003\n"; rc = false;
    }
  }
  expected_value = 0;
  for( size_type i = 0; i < lim; ++i ) {
    if( v2[i] != expected_value ) {
      std::cout << "insert_single FAIL 0004\n"; rc = false;
    }
    ++expected_value;
  }

  // Add test case for repeated insertions in the middle.

  return( rc );
}

bool insert_multiple_test( )
{
  // This test considers deque's initial capacity of 16.
  bool rc = true;
  typedef std::deque< int >::size_type size_type;
  typedef std::deque< int >::iterator iterator;

  // Small insertion, no reallocation.
  std::deque< int > v1;
  v1.push_back( 1 ); v1.push_back( 2 ); v1.push_back( 3 ); v1.push_back( 4 );
  v1.insert( v1.begin( ), 1, -1 );
  if( v1.size( ) != 5 || INSANE( v1 ) ) {
    std::cout << "insert_multiple FAIL 0001\n"; rc = false;
  }
  for( size_type i = 0; i < v1.size( ); ++i ) {
    if( i == 0 && v1[i] != -1 ) {
      std::cout << "insert_multiple FAIL 0002\n"; rc = false;
    }
    if( i != 0 && v1[i] != i ) {
      std::cout << "insert_multiple FAIL 0003\n"; rc = false;
    }
  }

  // Large insertion, no reallocation.
  std::deque< int > v2;
  v2.push_back( 1 ); v2.push_back( 2 ); v2.push_back( 3 ); v2.push_back( 4 );
  v2.insert( v2.begin( ), 4, -1 );
  if( v2.size( ) != 8 || INSANE( v2 ) ) {
    std::cout << "insert_multiple FAIL 0004\n"; rc = false;
  }
  for( size_type i = 0; i < v2.size( ); ++i ) {
    if( i < 4 && v2[i] != -1 ) {
      std::cout << "insert_multiple FAIL 0005\n"; rc = false;
    }
    if( i >= 4 && v2[i] != i - 3 ) {
      std::cout << "insert_multiple FAIL 0006\n"; rc = false;
    }
  }

  // Very large insertion, no reallocation.
  std::deque< int > v3;
  v3.push_back( 1 ); v3.push_back( 2 ); v3.push_back( 3 ); v3.push_back( 4 );
  v3.insert( v3.begin( ), 8, -1 );
  if( v3.size( ) != 12 || INSANE( v3 ) ) {
    std::cout << "insert_multiple FAIL 0007\n"; rc = false;
  }
  for( size_type i = 0; i < v3.size( ); ++i ) {
    if( i < 8 && v3[i] != -1 ) {
      std::cout << "insert_multiple FAIL 0008\n"; rc = false;
    }
    if( i >= 8 && v3[i] != i - 7 ) {
      std::cout << "insert_multiple FAIL 0009\n"; rc = false;
    }
  }

  // Insertion at the end, no reallocation.
  std::deque< int > v4;
  v4.push_back( 1 ); v4.push_back( 2 ); v4.push_back( 3 ); v4.push_back( 4 );
  v4.insert( v4.end( ), 8, -1 );
  if( v4.size( ) != 12 || INSANE( v4 ) ) {
    std::cout << "insert_multiple FAIL 0010\n"; rc = false;
  }
  for( size_type i = 0; i < v4.size( ); ++i ) {
    if( i < 4 && v4[i] != i + 1 ) {
      std::cout << "insert_multiple FAIL 0011\n"; rc = false;
    }
    if( i >= 4 && v4[i] != -1 ) {
      std::cout << "insert_multiple FAIL 0012\n"; rc = false;
    }
  }

  // Do a no-reallocation insertion in the middle as well?

  // Insertion at beginning, reallocation necessary.
  std::deque< int > v5;
  v5.push_back( 1 ); v5.push_back( 2 ); v5.push_back( 3 ); v5.push_back( 4 );
  v5.insert( v5.begin( ), 32, -1 );
  if( v5.size( ) != 36 || INSANE( v5 ) ) {
    std::cout << "insert_multiple FAIL 0013\n"; rc = false;
  }
  for( size_type i = 0; i < v5.size( ); ++i ) {
    if( i < 32 && v5[i] != -1 ) {
      std::cout << "insert_multiple FAIL 0014\n"; rc = false;
    }
    if( i >= 32 && v5[i] != i - 31 ) {
      std::cout << "insert_multiple FAIL 0015\n"; rc = false;
    }
  }

  // Insertion at end, reallocation necessary.
  std::deque< int > v6;
  v6.push_back( 1 ); v6.push_back( 2 ); v6.push_back( 3 ); v6.push_back( 4 );
  v6.insert( v6.end( ), 32, -1 );
  if( v6.size( ) != 36 || INSANE( v6 ) ) {
    std::cout << "insert_multiple FAIL 0016\n"; rc = false;
  }
  for( size_type i = 0; i < v6.size( ); ++i ) {
    if( i < 4 && v6[i] != i + 1 ) {
      std::cout << "insert_multiple FAIL 0017\n"; rc = false;
    }
    if( i >= 4 && v6[i] != -1 ) {
      std::cout << "insert_multiple FAIL 0018\n"; rc = false;
    }
  }

  // Insertion in the middle, reallocation necessary.
  std::deque< int > v7;
  v7.push_back( 1 ); v7.push_back( 2 ); v7.push_back( 3 ); v7.push_back( 4 );
  v7.insert( v7.begin( ) + 2, 32, -1 );
  if( v7.size( ) != 36 || INSANE( v7 ) ) {
    std::cout << "insert_multiple FAIL 0019\n"; rc = false;
  }
  for( size_type i = 0; i < v7.size( ); ++i ) {
    if( i < 2 && v7[i] != i + 1 ) {
      std::cout << "insert_multiple FAIL 0020\n"; rc = false;
    }
    if( i >= 2 && i < 34 && v7[i] != -1 ) {
      std::cout << "insert_multiple FAIL 0021\n"; rc = false;
    }
    if( i >= 34 && v7[i] != i - 31 ) {
      std::cout << "insert_multiple FAIL 0022\n"; rc = false;
    }
  }

  return( rc );
}

bool erase_test( )
{
  bool rc = true;

  std::deque< int > v1;
 
  v1.push_back( 5 );
  v1.erase( v1.begin( ) );
  if( v1.size( ) != 0 || INSANE( v1 ) ) {
    std::cout << "erase FAIL 0001\n"; rc = false;
  }

  v1.push_back( 6 ); v1.push_back( 7 );
  v1.erase( v1.end( ) - 1 );
  if( v1.size( ) != 1 || v1[0] != 6 || INSANE( v1 ) ) {
    std::cout << "erase FAIL 0002\n"; rc = false;
  }

  v1.push_back( 8 );
  v1.erase( v1.begin( ) );
  if( v1.size( ) != 1 || v1[0] != 8 || INSANE( v1 ) ) {
    std::cout << "erase FAIL 0003\n"; rc = false;
  }

  std::deque< int > v2;
  v2.push_back( 1 ); v2.push_back( 2 ); v2.push_back( 3 );
  v2.erase( v2.begin( ), v2.begin( ) + 2 );
  if( v2.size( ) != 1 || v2[0] != 3 || INSANE( v2 ) ) {
    std::cout << "erase FAIL 0004\n"; rc = false;
  }

  v2.push_back( 2 ); v2.push_back( 1 );
  v2.erase( v2.begin( ) + 1, v2.begin( ) + 3 );
  if( v2.size( ) != 1 || v2[0] != 3 || INSANE( v2 ) ) {
    std::cout << "erase FAIL 0005\n"; rc = false;
  }
  return( rc );
}

bool relational_test( )
{
  bool rc = true;

  // These initializations are crude, but good enough for now.
  std::deque< char > s1;
  s1.push_back( 'a' ); s1.push_back( 'b' );
  s1.push_back( 'c' ); s1.push_back( 'd' );

  std::deque< char > s2;
  s2.push_back( 'a' ); s2.push_back( 'b' );
  s2.push_back( 'c' ); s2.push_back( 'd' );

  std::deque< char > s3;
  s3.push_back( 'a' ); s3.push_back( 'b' );
  s3.push_back( 'c' ); s3.push_back( 'c' );

  std::deque< char > s4;
  s4.push_back( 'a' ); s4.push_back( 'b' );
  s4.push_back( 'c' ); s4.push_back( 'e' );

  std::deque< char > s5;
  s5.push_back( 'a' ); s5.push_back( 'b' );
  s5.push_back( 'c' );

  std::deque< char > s6;
  s6.push_back( 'a' ); s6.push_back( 'b' );
  s6.push_back( 'c' ); s6.push_back( 'd' );
  s6.push_back( 'e' );

  // Operator==
  if( !( s1 == s2 ) ) {
    std::cout << "relational FAIL 0001\n"; rc = false;
  }
  if(  ( s1 == s5 ) ) {
    std::cout << "relational FAIL 0002\n"; rc = false;
  }

  // Operator !=
  if(  ( s1 != s2 ) ) {
    std::cout << "relational FAIL 0003\n"; rc = false;
  }
  if( !( s5 != s1 ) ) {
    std::cout << "relational FAIL 0004\n"; rc = false;
  }

  // Operator<
  if(  ( s1 < s2 ) ) {
    std::cout << "relational FAIL 0005\n"; rc = false;
  }
  if( !( s3 < s1 ) ) {
    std::cout << "relational FAIL 0006\n"; rc = false;
  }
  if(  ( s4 < s1 ) ) {
    std::cout << "relational FAIL 0007\n"; rc = false;
  }
  if( !( s5 < s1 ) ) {
    std::cout << "relational FAIL 0008\n"; rc = false;
  }
  if(  ( s4 < s6 ) ) {
    std::cout << "relational FAIL 0009\n"; rc = false;
  }

  // Operator>
  if( !( s4 > s3 ) ) {
    std::cout << "relational FAIL 0010\n"; rc = false;
  }
  if(  ( s1 > s6 ) ) {
    std::cout << "relational FAIL 0011\n"; rc = false;
  }
  
  // Operator <=
  if( !( s1 <= s2 && s3 <= s2 ) ) {
    std::cout << "relational FAIL 0012\n"; rc = false;
  }
  if(  ( s2 <= s3 || s6 <= s5 ) ) {
    std::cout << "relational FAIL 0013\n"; rc = false;
  }

  // Operator>=
  if( !( s2 >= s1 && s6 >= s5 ) ) {
    std::cout << "relational FAIL 0014\n"; rc = false;
  }
  if(  ( s3 >= s4 || s5 >= s4 ) ) {
    std::cout << "relatioanl FAIL 0015\n"; rc = false;
  }

  return( rc );
}

bool swap_test( )
{
  bool rc = true;

  std::deque< int > v1, v2;
  v1.push_back(  1 ); v1.push_back(  2 );
  v2.push_back( 10 ); v2.push_back( 20 );

  v1.swap( v2 );
  if( v1[0] != 10 || v1[1] != 20 || INSANE( v1 ) ) {
    std::cout << "swap FAIL 0001\n"; rc = false;
  }
  if( v2[0] !=  1 || v2[1] !=  2 || INSANE( v2 ) ) {
    std::cout << "swap FAIL 0002\n"; rc = false;
  }

  #ifdef __NEVER
  std::swap( v1, v2 );
  if( v1[0] !=  1 || v1[1] !=  2 || INSANE( v1 ) ) {
    std::cout << "swap FAIL 0003\n"; rc = false;
  }
  if( v2[0] != 10 || v2[1] != 20 || INSANE( v2 ) ) {
    std::cout << "swap FAIL 0004\n"; rc = false;
  }
  #endif

  return( rc );
}

bool clear_test( )
{
  bool rc = true;

  std::deque< int > vec;
  vec.push_back( 1 ); vec.push_back( 2 ); vec.push_back( 3 );

  vec.clear( );
  if( vec.size( ) != 0 || INSANE( vec ) ) {
    std::cout << "clear FAIL 0001\n"; rc = false;
  }

  return( rc );
}

#endif

int main( )
{
  // Probably the entire test suite should be a template so that all
  // tests can be run using both structured and unstructed types. For
  // now this is better than nothing. Note that these array sizes are
  // large enough to force reallocation during repeated push_* (> 16).
  //
  static int int_check[] = {
     0,  2,  4,  6,  8, 10, 12, 14, 16, 18,
    20, 22, 24, 26, 28, 30, 32, 34, 36, 38 };
  const std::size_t int_check_size = sizeof( int_check )/sizeof( int );

  static std::string string_check[] = {
    "abc", "def", "ghi", "jkl", "mno", "pqr", "stu", "vwx", "yza", "bcd",
    "efg", "hij", "klm", "nop", "qrs", "tuv", "wxy", "zab", "cde", "fgh" };
  const std::size_t string_check_size = sizeof( string_check )/sizeof( std::string );

  int rc = 0;
  try {
    if( !construction_test( )    || !heap_ok( "t01" ) ) rc = 1;
    if( !access_test( )          || !heap_ok( "t02" ) ) rc = 1;
    // if( !assign_test( )          || !heap_ok( "t03" ) ) rc = 1;
    if( !pushfront_test( int_check, int_check_size ) ||
          !heap_ok( "t4" ) ) rc = 1;
    if( !pushfront_test( string_check, string_check_size ) ||
          !heap_ok( "t5" ) ) rc = 1;
    if( !pushback_test( int_check, int_check_size ) ||
          !heap_ok( "t6" ) ) rc = 1;
    if( !pushback_test( string_check, string_check_size ) ||
          !heap_ok( "t7" ) ) rc = 1;
    if( !iterator_test( )        || !heap_ok( "t08" ) ) rc = 1;
    // if( !insert_single_test( )   || !heap_ok( "t09" ) ) rc = 1;
    // if( !insert_multiple_test( ) || !heap_ok( "t10" ) ) rc = 1;
    // if( !erase_test( )           || !heap_ok( "t11" ) ) rc = 1;
    // if( !relational_test( )      || !heap_ok( "t12" ) ) rc = 1;
    // if( !swap_test( )            || !heap_ok( "t13" ) ) rc = 1;
    // if( !clear_test( )           || !heap_ok( "t14" ) ) rc = 1;
  }
  catch( ... ) {
    std::cout << "Unexpected exception of unexpected type.\n";
    rc = 1;
  }

  return( rc );
}
