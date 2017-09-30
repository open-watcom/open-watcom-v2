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
* Description:  This file contains the functional test from the complex
*               template. These tests do not exercise the precision of
*               complex calculations. The checks against expected values
*               are done to a limited number of significant figures. They
*               are only intended to verify that the correct algorithm
*               is being used in the computation.
****************************************************************************/

#include <cmath>
#include <complex>
#include <iostream>

// Really should check all three floating point types.
typedef std::complex<double> complex;

#define CLOSE_ENOUGH(x, y) (std::abs(((x)/(y)) - 1.0) <= 1.0E-3)

static bool close_enough( const complex &x, const complex &y )
{
  if (CLOSE_ENOUGH( x.real( ), y.real( ) ) &&
      CLOSE_ENOUGH( x.imag( ), y.imag( ) ) )
    return( true );
  return( false );
}


bool construct_test( )
{
  bool rc = true;

  complex z1;
  complex z2( 1.0 );
  complex z3( 1.0, -1.0 );
  complex z4( z3 );

  // Don't use the close_enough() function until basic construction checks.

  if( z1.real( ) != 0.0 || z1.imag( ) != 0.0  ) {
    std::cout << "basic FAIL 0001\n"; rc = false;
  }

  if( z2.real( ) != 1.0 || z2.imag( ) != 0.0 ) {
    std::cout << "basic FAIL 0002\n"; rc = false;
  }

  if( z3.real( ) !=  1.0 || z3.imag( ) != -1.0 ) {
    std::cout << "basic FAIL 0003\n"; rc = false;
  }

  if( z4.real( ) !=  1.0 || z4.imag( ) != -1.0 ) {
    std::cout << "basic FAIL 0004\n"; rc = false;
  }

  return( rc );
}


bool add_test( )
{
  bool rc = true;

  complex z( 1.0, 1.0 );
  complex x;

  x = z + complex( 1.0, 1.0 );
  if( !close_enough( x, complex( 2.0, 2.0 ) ) ) {
    std::cout << "add FAIL 0001\n"; rc = false;
  }

  x = z + complex( 1.0, -2.0 );
  if( !close_enough( x, complex( 2.0, -1.0 ) ) ) {
    std::cout << "add FAIL 0002\n"; rc = false;
  }

  x = z + complex( -2.0, 1.0 );
  if( !close_enough( x, complex( -1.0, 2.0 ) ) ) {
    std::cout << "add FAIL 0003\n"; rc = false;
  }

  x = z + complex( -2.0, -2.0 );
  if( !close_enough( x, complex( -1.0, -1.0 ) ) ) {
    std::cout << "add FAIL 0004\n"; rc = false;
  }

  return( rc );
}


bool subtract_test( )
{
  bool rc = true;

  complex z( 1.0, 1.0 );
  complex x;

  x = z - complex( 0.5, 0.5 );
  if( !close_enough( x, complex( 0.5, 0.5 ) ) ) {
    std::cout << "subtract FAIL 0001\n"; rc = false;
  }

  x = z - complex( 2.0, -0.5 );
  if( !close_enough( x, complex( -1.0, 1.5 ) ) ) {
    std::cout << "subtract FAIL 0002\n"; rc = false;
  }

  x = z - complex( 2.0, 2.0 );
  if( !close_enough( x, complex( -1.0, -1.0 ) ) ) {
    std::cout << "subtract FAIL 0003\n"; rc = false;
  }

  x = z - complex( -0.5, 2.0 );
  if( !close_enough( x, complex( 1.5, -1.0 ) ) ) {
    std::cout << "subtract FAIL 0004\n"; rc = false;
  }

  return( rc );
}


bool multiply_test( )
{
  bool rc = true;

  complex z( 2.0, 2.0 );
  complex x;

  x = z * complex( 1.0, 1.5 );
  if( !close_enough( x, complex( -1.0, 5.0 ) ) ) {
    std::cout << "multiply FAIL 0001\n"; rc = false;
  }

  x = z * complex( 1.0, 0.1 );
  if( !close_enough( x, complex( 1.8, 2.2 ) ) ) {
    std::cout << "multiply FAIL 0002\n"; rc = false;
  }

  x = z * complex( -0.175, -0.425 );
  if( !close_enough( x, complex( 0.5, -1.2 ) ) ) {
    std::cout << "multiply FAIL 0003\n"; rc = false;
  }

  x = z * complex( -0.25, 0.0 );
  if( !close_enough( x, complex( -0.5, -0.5 ) ) ) {
    std::cout << "multiply FAIL 0004\n"; rc = false;
  }

  return( rc );
}


bool divide_test( )
{
  bool rc = true;

  complex z( 2.0, 2.0 );
  complex x;

  x = z / complex( 1.0, 1.5 );
  if( !close_enough( x, complex( 1.5385, -0.3077 ) ) ) {
    std::cout << "divide FAIL 0001\n"; rc = false;
  }

  x = z / complex( 0.9, 0.1 );
  if( !close_enough( x, complex( 2.4390, 1.9512 ) ) ) {
    std::cout << "divide FAIL 0002\n"; rc = false;
  }

  x = z / complex( -1.2235, 0.0941 );
  if( !close_enough( x, complex( -1.5, -1.75 ) ) ) {
    std::cout << "divide FAIL 0003\n"; rc = false;
  }

  x = z / complex( 0.0941, -1.2235 );
  if( !close_enough( x, complex( -1.5, 1.75 ) ) ) {
    std::cout << "divide FAIL 0004\n"; rc = false;
  }

  return( rc );
}

using std::abs;

bool abs_test( )
{
  // using std::abs;
  bool rc = true;

  double result = abs( complex( 1.0, 1.0 ) );
  if( !CLOSE_ENOUGH( result, 1.4142 ) ) {
    std::cout << "abs FAIL 0001\n"; rc = false;
  }

  result = abs( complex( -1.0, 0.0 ) );
  if( !CLOSE_ENOUGH( result, 1.0 ) ) {
    std::cout << "abs FAIL 0002\n"; rc = false;
  }

  result = abs ( complex( 0.0, 0.0 ) );
  if( abs( result ) >= 1.0E-3 ) {
    std::cout << "abs FAIL 0003\n"; rc = false;
  }

  return( rc );
}

using std::arg;

bool arg_test( )
{
  // using std::arg;
  bool rc = true;

  double result = arg( complex( 1.0, 1.0 ) );
  if( !CLOSE_ENOUGH( result, 7.854E-1 ) ) {
    std::cout << "arg FAIL 0001\n"; rc = false;
  }

  result = arg( complex( 1.0, -0.1 ) );
  if( !CLOSE_ENOUGH( result, -9.967E-2 ) ) {
    std::cout << "arg FAIL 0002\n"; rc = false;
  }

  result = arg( complex( -0.1, -1.0 ) );
  if( !CLOSE_ENOUGH( result, -1.670 ) ) {
    std::cout << "arg FAIL 0003\n"; rc = false;
  }

  result = arg( complex( -1.0, 2.0 ) );
  if( !CLOSE_ENOUGH( result, 2.034 ) ) {
    std::cout << "arg FAIL 0004\n"; rc = false;
  }
  return( rc );
}

using std::polar;

bool polar_test( )
{
  // using std::polar;
  bool rc = true;

  complex z = polar( 1.0, 1.0 );
  if( !close_enough( z, complex( 0.5403, 0.8415 ) ) ) {
    std::cout << "polar FAIL 0001\n"; rc = false;
  }

  z = polar( 0.0, 1.0 );
  if( z.real( ) >= 1.0E-3 || z.imag( ) >= 1.0E-3 ) {
    std::cout << "polar FAIL 0002\n"; rc = false;
  }

  z = polar( 1.0, 1.57080 );
  if( z.real( ) >= 1.0E-3 || !CLOSE_ENOUGH( z.imag( ), 1.0 ) ) {
    std::cout << "polar FAIL 0003\n"; rc = false;
  }

  z = polar( 1.0, 3.14159 );
  if( !CLOSE_ENOUGH( z.real( ), -1.0 ) || z.imag( ) >= 1.0E-3 ) {
    std::cout << "polar FAIL 0004\n"; rc = false;
  }

  z = polar( 1.0, -3.14159 );
  if( !CLOSE_ENOUGH( z.real( ), -1.0 ) || z.imag( ) >= 1.0E-3 ) {
    std::cout << "polar FAIL 0005\n"; rc = false;
  }

  z = polar( 1.0, 10.0 );
  if( !close_enough( z, complex( -0.8391, -0.5440 ) ) ) {
    std::cout << "polar FAIL 0006\n"; rc = false;
  }
  return( rc );
}

using std::cos;

bool cos_test( )
{
  // using std::cos;
  bool rc = true;

  complex i_values[] = {
    complex(  1.0,  1.0 ), complex( -2.0,  1.0 ),
    complex( -1.0, -1.0 ), complex(  1.0, -2.0 ) };

  complex o_values[] = {
    complex( 8.337E-1, -9.889E-1 ), complex( -6.421E-1, 1.069E+0 ),
    complex( 8.337E-1, -9.889E-1 ), complex(  2.033E+0, 3.052E+0 ) };

  for( int i = 0; i < 4; ++i ) {
    if( !close_enough( cos( i_values[i] ), o_values[i] ) ) {
      std::cout << "cos FAIL 0001\n"; rc = false;
    }
  }
  return( rc );
}

using std::cosh;

bool cosh_test( )
{
  // using std::cosh;
  bool rc = true;

  complex i_values[] = {
    complex(  1.0,  1.0 ), complex( -2.0,  1.0 ),
    complex( -1.0, -1.0 ), complex(  1.0, -2.0 ) };

  complex o_values[] = {
    complex( 8.337E-1, 9.889E-1 ), complex(  2.033E+0, -3.052E+0 ),
    complex( 8.337E-1, 9.889E-1 ), complex( -6.421E-1, -1.069E+0 ) };

  for( int i = 0; i < 4; ++i ) {
    if( !close_enough( cosh( i_values[i] ), o_values[i] ) ) {
      std::cout << "cosh FAIL 0001\n"; rc = false;
    }
  }
  return( rc );
}

using std::exp;

bool exp_test( )
{
  // using std::exp;
  bool rc = true;

  complex i_values[] = {
    complex(  1.0,  1.0 ), complex( -2.0,  1.0 ),
    complex( -1.0, -1.0 ), complex(  1.0, -2.0 ) };

  complex o_values[] = {
    complex( 1.469E+0,  2.287E+0 ), complex(  7.312E-2,  1.139E-1 ),
    complex( 1.988E-1, -3.096E-1 ), complex( -1.131E+0, -2.472E+0 ) };

  for( int i = 0; i < 4; ++i ) {
    if( !close_enough( exp( i_values[i] ), o_values[i] ) ) {
      std::cout << "exp FAIL 0001\n"; rc = false;
    }
  }
  return( rc );
}

using std::log;

bool log_test( )
{
  // using std::log;
  bool rc = true;

  complex i_values[] = {
    complex(  1.0,  1.0 ), complex( -2.0,  1.0 ),
    complex( -1.0, -1.0 ), complex(  1.0, -2.0 ),

    // Exercise mag_rp > 32 * mag_ip, etc. Should explore all four quads.
    complex( 100.0, 1.0 ), complex( 1.0, 100.0 ) };

  complex o_values[] = {
    complex( 3.466E-1,  7.854E-1 ), complex(  8.047E-1,  2.678E+0 ),
    complex( 3.466E-1, -2.356E+0 ), complex(  8.047E-1, -1.107E+0 ),
    complex( 4.605E+0,  1.000E-2 ), complex(  4.605E+0,  1.561E+0 ) };

  for( int i = 0; i < 6; ++i ) {
    if( !close_enough( log( i_values[i] ), o_values[i] ) ) {
      std::cout << "log FAIL 0001\n"; rc = false;
    }
  }
  return( rc );
}

using std::log10;

bool log10_test( )
{
  // using std::log10;
  bool rc = true;

  complex i_values[] = {
    complex(  1.0,  1.0 ), complex( -2.0,  1.0 ),
    complex( -1.0, -1.0 ), complex(  1.0, -2.0 ),

    // Exercise mag_rp > 32 * mag_ip, etc. Should explore all four quads.
    complex( 100.0, 1.0 ), complex( 1.0, 100.0 ) };

  complex o_values[] = {
    complex( 1.505E-1,  3.411E-1 ), complex(  3.495E-1,  1.163E+0 ),
    complex( 1.505E-1, -1.023E+0 ), complex(  3.495E-1, -4.808E-1 ),
    complex( 2.000E+0,  4.343E-3 ), complex(  2.000E+0,  6.778E-1 ) };

  for( int i = 0; i < 6; ++i ) {
    if( !close_enough( log10( i_values[i] ), o_values[i] ) ) {
      std::cout << "log10 FAIL 0001\n"; rc = false;
    }
  }
  return( rc );
}

using std::pow;

bool pow_test( )
{
  // using std::pow;
  bool rc = true;

  // This testing is minimal. At least it instantiates all three templates.

  complex z = pow( complex( -1.0, 1.0 ), 3 );
  if( !close_enough( complex( 2.0, 2.0 ), z ) ) {
    std::cout << "pow FAIL 0001\n"; rc = false;
  }

  z = pow( complex( -2.0, -1.0 ), 2.5 );
  if( !close_enough( complex( 6.852E+0, -2.992E+0 ), z ) ) {
    std::cout << "pow FAIL 0002\n"; rc = false;
  }

  z = pow( complex( 1.0, -2.0 ), complex( 3.0, -0.5 ) );
  if( !close_enough( complex( -5.369E+0, 3.534E+0 ), z ) ) {
    std::cout << "pow FAIL 0003\n"; rc = false;
  }

  z = pow( 4.2, complex( 1.5, 2.0 ) );
  if( !close_enough( complex( -8.292E+0, 2.308E+0 ), z ) ) {
    std::cout << "pow FAIL 0004\n"; rc = false;
  }
  return( rc );
}

using std::sin;

bool sin_test( )
{
  // using std::sin;
  bool rc = true;

  complex i_values[] = {
    complex(  1.0,  1.0 ), complex( -2.0,  1.0 ),
    complex( -1.0, -1.0 ), complex(  1.0, -2.0 ) };

  complex o_values[] = {
    complex(  1.298E+0,  6.350E-1 ), complex( -1.403E+0, -4.891E-1 ),
    complex( -1.298E+0, -6.350E-1 ), complex(  3.166E+0, -1.960E+0 ) };

  for( int i = 0; i < 4; ++i ) {
    if( !close_enough( sin( i_values[i] ), o_values[i] ) ) {
      std::cout << "sin FAIL 0001\n"; rc = false;
    }
  }
  return( rc );
}

using std::sinh;

bool sinh_test( )
{
  // using std::sinh;
  bool rc = true;

  complex i_values[] = {
    complex(  1.0,  1.0 ), complex( -2.0,  1.0 ),
    complex( -1.0, -1.0 ), complex(  1.0, -2.0 ) };

  complex o_values[] = {
    complex(  6.350E-1,  1.298E+0 ), complex( -1.960E+0,  3.166E+0 ),
    complex( -6.350E-1, -1.298E+0 ), complex( -4.891E-1, -1.403E+0 ) };

  for( int i = 0; i < 4; ++i ) {
    if( !close_enough( sinh( i_values[i] ), o_values[i] ) ) {
      std::cout << "sinh FAIL 0001\n"; rc = false;
    }
  }
  return( rc );
}

using std::sqrt;

bool sqrt_test( )
{
  // using std::sqrt;
  bool rc = true;

  complex i_values[] = {
    complex(  1.0,  1.0 ), complex( -2.0,  1.0 ),
    complex( -1.0, -1.0 ), complex(  1.0, -2.0 ) };

  complex o_values[] = {
    complex(  1.099E-0,  4.551E-1 ), complex( 3.436E-1,  1.455E+0 ),
    complex(  4.551E-1, -1.099E+0 ), complex( 1.272E+0, -7.862E-1 ) };

  for( int i = 0; i < 4; ++i ) {
    if( !close_enough( sqrt( i_values[i] ), o_values[i] ) ) {
      std::cout << "sqrt FAIL 0001\n"; rc = false;
    }
  }
  return( rc );
}

using std::tan;

bool tan_test( )
{
  // using std::tan;
  bool rc = true;

  complex i_values[] = {
    complex(  1.0,  1.0 ), complex( -2.0,  1.0 ),
    complex( -1.0, -1.0 ), complex(  1.0, -2.0 ) };

  complex o_values[] = {
    complex(  2.718E-1,  1.084E+0 ), complex( 2.435E-1,  1.167E+0 ),
    complex( -2.718E-1, -1.084E+0 ), complex( 3.381E-2, -1.015E+0 ) };

  for( int i = 0; i < 4; ++i ) {
    if( !close_enough( tan( i_values[i] ), o_values[i] ) ) {
      std::cout << "tan FAIL 0001\n"; rc = false;
    }
  }
  return( rc );
}

using std::tanh;

bool tanh_test( )
{
  // using std::tanh;
  bool rc = true;

  complex i_values[] = {
    complex(  1.0,  1.0 ), complex( -2.0,  1.0 ),
    complex( -1.0, -1.0 ), complex(  1.0, -2.0 ) };

  complex o_values[] = {
    complex(  1.084E+0,  2.718E-1 ), complex( -1.015E+0,  3.381E-2 ),
    complex( -1.084E+0, -2.718E-1 ), complex(  1.167E+0,  2.435E-1 ) };

  for( int i = 0; i < 4; ++i ) {
    if( !close_enough( tanh( i_values[i] ), o_values[i] ) ) {
      std::cout << "tanh FAIL 0001\n"; rc = false;
    }
  }
  return( rc );
}

int main( )
{
  int rc = 0;
  try {
    if( !construct_test( )   ) rc = 1;
    if( !add_test( )         ) rc = 1;
    if( !subtract_test( )    ) rc = 1;
    if( !multiply_test( )    ) rc = 1;
    if( !divide_test( )      ) rc = 1;
    if( !abs_test( )         ) rc = 1;
    if( !arg_test( )         ) rc = 1;
    if( !polar_test( )       ) rc = 1;
    if( !cos_test( )         ) rc = 1;
    if( !cosh_test( )        ) rc = 1;
    if( !exp_test( )         ) rc = 1;
    if( !log10_test( )       ) rc = 1;
    if( !pow_test( )         ) rc = 1;
    if( !sin_test( )         ) rc = 1;
    if( !sinh_test( )        ) rc = 1;
    if( !sqrt_test( )        ) rc = 1;
    if( !tan_test( )         ) rc = 1;
    if( !tanh_test( )        ) rc = 1;
  }
  catch( ... ) {
    std::cout << "Unexpected exception of unexpected type.\n";
    rc = 1;
  }

  return( rc );
}
