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
* Description:  This program exercises the facilities in <memory>
*
****************************************************************************/

#include <iostream>
#include <memory>

#include "sanity.cpp"

typedef std::allocator< int > int_allocator;
typedef std::allocator< int >::rebind< long >::other lng_allocator;

bool rebind_test( )
{
  int_allocator ia;
  lng_allocator la;

  int_allocator::pointer p1 = ia.allocate( 1 );
  lng_allocator::pointer p2 = la.allocate( 1 );
  if( p1 == 0 ) FAIL
  if( p2 == 0 ) FAIL
  if( static_cast< void *>( p1 ) == static_cast< void* >( p2 ) ) FAIL
  ia.deallocate( p1, 1 );
  la.deallocate( p2, 1 );
  return( true );
}

struct Base {
  int b_member;
};
struct Derived : Base {
  int d_member;
};

template< typename T >
bool f( std::auto_ptr< T > arg )
{
  if( *arg != 24 ) FAIL
  return true;
}

template< typename T >
void g( const std::auto_ptr< T > &arg )
{
  *arg = 42;
}

bool auto_ptr_test( )
{
  bool rc = true;

  // Basic tests
  // -----------

  std::auto_ptr< int > p1( new int( -2 ) );
  if( *p1 != -2 ) FAIL

  std::auto_ptr< int > p2( p1 );
  if(  p1.get( ) !=  0 ) FAIL
  if( *p2        != -2 ) FAIL

  std::auto_ptr< int > p3;
  p3 = p2;
  if(  p2.get( ) !=  0 ) FAIL
  if( *p3        != -2 ) FAIL

  // Check base/derived conversions
  // ------------------------------

  std::auto_ptr< Derived > pder( new Derived );
  pder->b_member = -2;

  std::auto_ptr< Base > pbas( pder );
  if( pder.get( )    !=  0 ) FAIL
  if( pbas->b_member != -2 ) FAIL

  pder.reset( new Derived );
  pder->b_member = -3;
  pbas = pder;
  if( pder.get( )    !=  0 ) FAIL
  if( pbas->b_member != -3 ) FAIL

  // Check auto_ptr as function argument type or return type
  // -------------------------------------------------------

  std::auto_ptr< int > arg1( new int( 24 ) );
  f( arg1 );
  if( arg1.get( ) != 0 ) FAIL

  const std::auto_ptr< int > arg2( new int( 24 ) );
  g( arg2 );
  if( *arg2 != 42 ) FAIL

  return( rc );
}


int main( )
{
  int rc = 0;
  int original_count = heap_count( );

  try {
    if( !rebind_test( )   || !heap_ok( "t01" ) ) rc = 1;
    if( !auto_ptr_test( ) || !heap_ok( "t02" ) ) rc = 1;
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
