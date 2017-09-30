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
* Description:  This file contains the functional tests for the templates
*               in <numeric>.
*
****************************************************************************/

#include <iostream>
#include <functional>
#include <numeric>
#include <string>

using namespace std;

int acc_nums[] = { 1, 2, 3, 4, 5 };

bool accumulate_test( )
{
  bool rc = true;

  int sum = accumulate( acc_nums, acc_nums + 5, 1 );
  if( sum != 16 ) {
    cout << "accumulate FAIL 0001\n"; rc = false;
  }

  int product = accumulate( acc_nums, acc_nums + 5, 1, multiplies<int>() );
  if( product != 120 ) {
    cout << "accumulate FAIL 0002\n"; rc = false;
  }
  return( rc );
}

int ip_nums1[] = { 0, 2, 4, 8 };
int ip_nums2[] = { 1, 3, 5, 7 };

bool inner_product_test( )
{
  bool rc = true;

  int dot_product = inner_product( ip_nums1, ip_nums1 + 4, ip_nums2, 0 );
  if( dot_product != 82 ) {
    cout << "inner_product FAIL 0001\n"; rc = false;
  }

  int result = inner_product(
    ip_nums1, ip_nums1 + 4, ip_nums2, 1, multiplies<int>(), plus<int>() );
  if( result != 675 ) {
    cout << "inner_product FAIL 0002\n"; rc = false;
  }

  return( rc );
}

string ps1a[] = { "a", "b", "c", "d" };
string ps2a[] = { " ", " ", " ", " " };
int    ps1b[] = { 1, 2, 3, 4 };
int    ps2b[] = { 0, 0, 0, 0 };

bool partial_sum_test( )
{
  bool rc = true;

  partial_sum( ps1a, ps1a + 4, ps2a );
  for( int i = 0; i < 4; ++i ) {
    cout << ps2a[i].c_str( ) << "\n";
  }

  partial_sum( ps1b, ps1b + 4, ps2b, multiplies<int>() );
  for( int i = 0; i < 4; ++i ) {
    cout << ps2b[i] << "\n";
  }

  return( rc );
}

int ad1a[] = { 1, 2, 3, 12 };
int ad1b[] = { 0, 0, 0, 0 };

bool adjacent_difference_test( )
{
  bool rc = true;

  adjacent_difference( ad1a, ad1a + 4, ad1b );
  for( int i = 0; i < 4; ++i ) {
    cout << ad1b[i] << "\n";
  }

  adjacent_difference( ad1a, ad1a + 4, ad1b, divides<int>() );
  for( int i = 0; i < 4; ++i ) {
    cout << ad1b[i] << "\n";
  }
  return( rc );
}

int main( )
{
  int rc = 0;
  try {
    if( !accumulate_test( )          ) rc = 1;
    if( !inner_product_test( )       ) rc = 1;
    if( !partial_sum_test( )         ) rc = 1;
    if( !adjacent_difference_test( ) ) rc = 1;
  }
  catch( ... ) {
    std::cout << "Unexpected exception of unexpected type.\n";
    rc = 1;
  }

  return( rc );
}
