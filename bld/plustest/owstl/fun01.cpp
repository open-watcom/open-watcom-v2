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
* Description:  This file contains the functional tests for the functional
*               objects.
*
****************************************************************************/

#include <functional>
#include <iostream>

bool math_test( )
{
  bool rc = true;

  std::plus< int >       f1;
  std::minus< int >      f2;
  std::multiplies< int > f3;
  std::divides< int >    f4;
  std::modulus< int >    f5;
  std::negate< int >     f6;

  if( f1( 1, 3 ) != 4 ) {
    std::cout << "math FAIL 0001\n"; rc = false;
  }
  if( f2( 1, 3 ) != -2 ) {
    std::cout << "math FAIL 0002\n"; rc = false;
  }
  if( f3( 4, 3 ) != 12 ) {
    std::cout << "math FAIL 0003\n"; rc = false;
  }
  if( f4( 20, 5 ) != 4 ) {
    std::cout << "math FAIL 0004\n"; rc = false;
  }
  if( f5( 17, 4 ) != 1 ) {
    std::cout << "math FAIL 0005\n"; rc = false;
  }
  if( f6( 10 ) != -10 ) {
    std::cout << "math FAIL 0006\n"; rc = false;
  }

  return( rc );
}


bool relational_test( )
{
  // Do this a different way to exercise namespace/scope handling.
  using namespace std;

  bool rc = true;

  equal_to< int >      f1;
  not_equal_to< int >  f2;
  greater< int >       f3;
  less< int >          f4;
  greater_equal< int > f5;
  less_equal< int >    f6;

  if( f1( 1, 2 ) ) {
    std::cout << "relational FAIL 0001\n"; rc = false;
  }
  if( f2( 2, 2 ) ) {
    std::cout << "relational FAIL 0002\n"; rc = false;
  }
  if( f3( 3, 3 ) ) {
    std::cout << "relational FAIL 0003\n"; rc = false;
  }
  if( f4( 4, 3 ) ) {
    std::cout << "relational FAIL 0004\n"; rc = false;
  }
  if( f5( 3, 4 ) ) {
    std::cout << "relational FAIL 0005\n"; rc = false;
  }
  if( f6( 4, 3 ) ) {
    std::cout << "relational FAIL 0006\n"; rc = false;
  }

  return( rc );
}


bool logical_test( )
{
  bool rc = true;

  std::logical_and< bool > f1;
  std::logical_or< bool >  f2;
  std::logical_not< bool > f3;

  if( f1( true, false ) ) {
    std::cout << "logical FAIL 0001\n"; rc = false;
  }
  if( f2( false, false ) ) {
    std::cout << "logical FAIL 0002\n"; rc = false;
  }
  if( f3( true ) ) {
    std::cout << "logical FAIL 0003\n"; rc = false;
  }

  return( rc );
}


bool negators_test( )
{
  bool rc = true;

  std::logical_not< bool > f1;
  std::logical_and< bool > f2;

  if( std::not1( f1 )( false ) ) {
    std::cout << "negators FAIL 0001\n"; rc = false;
  }

  if( std::not2( f2 )( true, true ) ) {
    std::cout << "negators FAIL 0002\n"; rc = false;
  }

  return( rc );
}


bool binders_test( )
{
  bool rc = true;

  std::less< int > f1;

  if( std::bind1st( f1, 5 )( 3 ) ) {
    std::cout << "binders FAIL 0001\n"; rc = false;
  }
  if( std::bind2nd( f1, 5 )( 8 ) ) {
    std::cout << "binders FAIL 0002\n"; rc = false;
  }

  return( rc );
}


// Ordinary function used in some of the tests below.
int square( int x )
{
  return( x * x );
}

// Ordinary function used in some of the tests below.
int product( int x, int y )
{
  return( x * y );
}

// Ordinary class used in some of the tests below.
class Holder {
  int m;
public:
  Holder( int value ) : m( value ) { }
  int access( );
  int access_plus( int );
  int c_access( ) const;
  int c_access_plus( int ) const;
};

int Holder::access( )
  { return( m ); }

int Holder::c_access( ) const  // Make this a little different.
  { return( m + 1 ); }

int Holder::access_plus( int v )
  { return( m + v ); }

int Holder::c_access_plus( int v ) const  // Make this a little different.
  { return( m + v + 1 ); }


bool adaptors_test( )
{
  using namespace std;

  bool rc = true;
  Holder example( 2 );

  if( ptr_fun( square )( 3 ) != 9 ) {
    cout << "adaptors FAIL 0001\n"; rc = false;
  }
  if( ptr_fun( product )( 4, 5 ) != 20 ) {
    cout << "adaptors FAIL 0002\n"; rc = false;
  }
  if( bind1st( ptr_fun( product ), 3 )( 5 ) != 15 ) {
    cout << "adaptors FAIL 0003\n"; rc = false;
  }
  if( mem_fun( &Holder::access )( &example ) != 2 ) {
    cout << "adaptors FAIL 0004\n"; rc = false;
  }
  if( mem_fun( &Holder::access_plus )( &example, 3 ) != 5 ) {
    cout << "adaptors FAIL 0005\n"; rc = false;
  }
  if( bind1st( mem_fun( &Holder::access_plus ), &example )( 4 ) != 6 ) {
    cout << "adaptors FAIL 0006\n"; rc = false;
  }
  if( mem_fun_ref( &Holder::access )( example ) != 2 ) {
    cout << "adaptors FAIL 0007\n"; rc = false;
  }
  if( mem_fun_ref( &Holder::access_plus )( example, 3 ) != 5 ) {
    cout << "adaptors FAIL 0008\n"; rc = false;
  }
  if( mem_fun( &Holder::c_access )( &example ) != 3 ) {
    cout << "adaptors FAIL 0009\n"; rc = false;
  }
  if( mem_fun( &Holder::c_access_plus )( &example, 3 ) != 6 ) {
    cout << "adaptors FAIL 0010\n"; rc = false;
  }
  if( mem_fun_ref( &Holder::c_access )( example ) != 3 ) {
    cout << "adaptors FAIL 0011\n"; rc = false;
  }
  if( mem_fun_ref( &Holder::c_access_plus )( example, 3 ) != 6 ) {
    cout << "adaptors FAIL 0012\n"; rc = false;
  }

  return( rc );
}


int main()
{
  int rc = 0;
  try {
    if( !math_test( )       ) rc = 1;
    if( !relational_test( ) ) rc = 1;
    if( !logical_test( )    ) rc = 1;
    if( !negators_test( )   ) rc = 1;
    if( !binders_test( )    ) rc = 1;
    if( !adaptors_test( )   ) rc = 1;
  }
  catch( ... ) {
    std::cout << "Unexpected exception of unexpected type.\n";
    rc = 1;
  }

  return( rc );
}
