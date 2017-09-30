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
* Description:  Functional tests for the bitset template.
*
****************************************************************************/

#include <bitset>
#include <iostream>

#include "sanity.cpp"

bool various_tests( )
{
    std::bitset<10> b1( 255 );
    std::bitset<10> b2( 254 );

    if( b1.to_ulong( ) != 255 ) FAIL;
    if( b2.to_ulong( ) != 254 ) FAIL;
    if( b1.count( ) != 8 ) FAIL;
    if( b2.count( ) != 7 ) FAIL;

    b1.flip( 0 );
    if( b1.to_ulong( ) != 254 ) FAIL;
    if( b2.to_ulong( ) != 254 ) FAIL;

    try {
      b1.set( 11 );
      FAIL;
    }
    catch( std::out_of_range ) { }
    catch( ... ) {
      FAIL;
    }

    if( b1 != b2 ) FAIL;
    if( !( b1 == b2 ) ) FAIL;
    if( !b1.any( )  ) FAIL;
    if(  b1.none( ) ) FAIL;

    b1.reset( );
    if(  b1.any( )  ) FAIL;
    if( !b1.none( ) ) FAIL;
    if( !b2.any( )  ) FAIL;
    if(  b2.none( ) ) FAIL;

    b2 &= b1;
    if(  b1.any( )  ) FAIL;
    if( !b1.none( ) ) FAIL;
    if(  b2.any( )  ) FAIL;
    if( !b2.none( ) ) FAIL;

    b1.set( 5 );
    b2.set( 5, 0 );
    if( !b1.test( 5 ) ) FAIL;
    if(  b2.test( 5 ) ) FAIL;
    if(  b1.count( ) != 1 ) FAIL;
    if(  b2.count( ) != 0 ) FAIL;

    b1 <<= 1;
    if( !b1.test( 6 ) ) FAIL;
    if(  b1.test( 5 ) ) FAIL;

    b2 = ~b1;
    if(  b2.test( 6 ) ) FAIL;
    if( !b2.test( 5 ) ) FAIL;

    b1.reset( );
    if(  b1.test( 7 ) ) FAIL;

    b1.set( 7 );
    if (!b1.test( 7 ) ) FAIL;

    b1 >>= 1;
    if(  b1.test( 7 ) ) FAIL;
    if( !b1.test( 6 ) ) FAIL;

    b1 >>= -1;
    if( b1.count( ) != 0 ) FAIL;

    b1.set( );
    b1 <<= -1;
    if( b1.count() != 0 ) FAIL;

    b1.set( );
    b1 >>= -1;
    if( b1.count( ) != 0 ) FAIL;

    return true;
}

bool reference_test( )
{
  std::bitset< 12 > bits;

  for( std::size_t i = 0; i < 12; ++i ) {
    if( bits[i] != 0 ) FAIL;
  }
  bits[3] = 1;
  for( std::size_t i = 0; i < 12; ++i ) {
    if( i != 3 && bits[i] != 0 ) FAIL;
    if( i == 3 && bits[i] != 1 ) FAIL;
  }

  return true;
}

int main( )
{
  int rc = 0;
  int original_count = heap_count( );

  try {
    if( !various_tests( )    || !heap_ok( "t01" ) ) rc = 1;
    if( !reference_test( )   || !heap_ok( "t02" ) ) rc = 1;
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
