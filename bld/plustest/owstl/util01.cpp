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
* Description: This file contains the functional tests for the utilites.
*
****************************************************************************/

#include <utility>
#include <iostream>
#include "sanity.cpp"

bool pair_test( )
{
  std::pair< int, char > object1( 1, 'H' );
  std::pair< int, char > object2( 0, 'H' );
  std::pair< int, char > object3( 1, 'G' );

  // Does std::make_pair do something reasonable?
  if( !( object1 == std::make_pair( 1, 'H' ) ) ) FAIL;

  // Verify std::pair's relational operators.
  if( !( object1 == object1 ) ) FAIL;
  if(  ( object1 == object2 ) ) FAIL;
  if(  ( object1 == object3 ) ) FAIL;
  if( !( object2 <  object1 ) ) FAIL;
  if( !( object3 <  object1 ) ) FAIL;
  if( !( object2 <  object3 ) ) FAIL;

  // Make sure the other relational operators can be instantiated properly.
  if( !( object1 != object2 ) ) FAIL;
  if( !( object1 >  object2 ) ) FAIL;
  if(  ( object1 <= object2 ) ) FAIL;
  if( !( object1 >= object2 ) ) FAIL;

  // This sort of thing comes up when map's value_type is used.
  std::pair< const int, int > object( std::make_pair( 1, 2 ) );
  if( !( object.first == 1 ) || !( object.second == 2 ) ) FAIL;

  return true;
}

int main()
{
  int rc = 0;
  try {
    if( !pair_test( ) ) rc = 1;
  }
  catch( ... ) {
    std::cout << "Unexpected exception of unexpected type.\n";
    rc = 1;
  }

  return rc;
}
