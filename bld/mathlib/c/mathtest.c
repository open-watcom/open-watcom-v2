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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifdef __FPI__
 #include <float.h>
 #include <signal.h>
#endif

#ifndef TRUE
 #define TRUE           1
 #define FALSE          0
#endif
#ifndef PI
 #define PI             3.14159265358979323846
#endif
#ifndef E
 #define E              2.718281828459045
#endif
#define VERIFY(expr)    if(!(expr)) \
                            printf( "FAIL: %s, line %d\n",#expr,__LINE__ )
#define MYABS( a )      ((a) < 0 ? -(a) : (a) )

#ifdef __FPI__
volatile int sig_count = 0;
double a = 1.0;
double b = 3.0;
double q;

void my_handler( int sig, int fpe ) {
    if( sig == SIGFPE ) {
        if( fpe == _FPE_INEXACT ) {
            sig_count++;
        }
    } else {
        abort();
    }
    signal( SIGFPE, (__sig_func)my_handler );
}

#else

volatile int my_matherrno;

int matherr( struct exception *err )
{
    if( strcmp( err->name, "sqrt" ) == 0 ) {
        if( err->type == DOMAIN ) {
            my_matherrno = DOMAIN;
            err->retval = sqrt( -(err->arg1) );
            return( 1 );
        } else {
            return( 0 );
        }
    }
    return( 0 );
}
#endif

void test_integer_math( void )
{
    div_t div_result;
    ldiv_t ldiv_result;
    int num[20], ctr;

    printf( "Testing integer functions...\n" );
    VERIFY( abs( -1 ) == 1 );
    VERIFY( abs( 0 ) == 0 );
    VERIFY( abs( 1 ) == 1 );
    VERIFY( labs( -999999L ) == 999999L );
    VERIFY( labs( 0L ) == 0L );
    VERIFY( labs( 999999L ) == 999999L );
    div_result = div( 100, 100 );
    VERIFY( div_result.quot == 1 && div_result.rem == 0 );
    div_result = div( 0, 100 );
    VERIFY( div_result.quot == 0 && div_result.rem == 0 );
    div_result = div( 101, 100 );
    VERIFY( div_result.quot == 1 && div_result.rem == 1 );
    ldiv_result = ldiv( 100000L, 100000L );
    VERIFY( ldiv_result.quot == 1L && ldiv_result.rem == 0L );
    ldiv_result = ldiv( 0L, 100000L );
    VERIFY( ldiv_result.quot == 0L && ldiv_result.rem == 0L );
    ldiv_result = ldiv( 101000L, 100000L );
    VERIFY( ldiv_result.quot == 1L && ldiv_result.rem == 1000L );
    for( ctr = 0; ctr < 20; ++ctr ) {
        num[ctr] = rand();
        VERIFY( num[ctr] >= 0 );
    }
    srand( 1 );
    for( ctr = 0; ctr < 20; ++ctr ) {
        num[ctr] -= rand();
        VERIFY( num[ctr] == 0 );
    }
}

void test_complex_math( void )
{
    struct complex c = { 5.0, -12.0 };

    printf( "Testing complex functions...\n" );
    VERIFY( CompDbl( cabs( c ), 13.0 ) );
}

void test_trig( void )
{
    //double num;
    printf( "Testing trigonometric functions...\n" );
    VERIFY( CompDbl( sin( PI ), 0.0 ) );
    VERIFY( CompDbl( sin( 0.0 ), 0.0 ) );
    VERIFY( CompDbl( sin( -PI ), 0.0 ) );
    VERIFY( CompDbl( sin( PI / 2 ), 1.0 ) );
    VERIFY( CompDbl( sin(- PI / 2 ), -1.0 ) );
    VERIFY( CompDbl( cos( PI ), -1.0 ) );
    VERIFY( CompDbl( cos( 0.0 ), 1.0 ) );
    VERIFY( CompDbl( cos( -PI ), -1.0 ) );
    VERIFY( CompDbl( cos( PI / 2 ), 0.0 ) );
    VERIFY( CompDbl( cos(- PI / 2 ), 0.0 ) );
    VERIFY( CompDbl( tan( PI ), 0.0 ) );
    VERIFY( CompDbl( tan( 0.0 ), 0.0 ) );
    VERIFY( CompDbl( tan( -PI ), 0.0 ) );
    VERIFY( CompDbl( tan( PI / 4 ), 1.0 ) );
    VERIFY( CompDbl( asin( 1.0 ), PI / 2 ) );
    VERIFY( CompDbl( asin( 0.0 ), 0.0 ) );
    VERIFY( CompDbl( asin( -1.0 ), -PI / 2 ) );
    VERIFY( CompDbl( acos( 1.0 ), 0.0 ) );
    VERIFY( CompDbl( acos( 0.0 ), PI / 2 ) );
    VERIFY( CompDbl( acos( -1.0 ), PI ) );
    VERIFY( CompDbl( atan( 1.0 ), PI / 4 ) );
    VERIFY( CompDbl( atan( 0.0 ), 0.0 ) );
    VERIFY( CompDbl( atan( -1.0 ), -PI / 4 ) );
    VERIFY( CompDbl( atan( DBL_MAX ), PI / 2 ) );
    VERIFY( CompDbl( atan( -DBL_MAX ), -PI / 2 ) );
    VERIFY( CompDbl( atan2( 2.0, 2.0 ), PI / 4 ) );
    VERIFY( CompDbl( atan2( 1.0, -1.0 ), 3 * PI / 4 ) );
    VERIFY( CompDbl( atan2( -1.0, -1.0 ), -3 * PI / 4 ) );
    VERIFY( CompDbl( atan2( -3.0, 3.0 ), -PI / 4 ) );
    VERIFY( CompDbl( atan2( 0.0, 1.0 ), 0.0 ) );
    VERIFY( CompDbl( atan2( 1.0, 0.0 ), PI / 2 ) );
    VERIFY( CompDbl( atan2( -DBL_MAX, 1.0 ), -PI / 2 ) );
    VERIFY( CompDbl( atan2( DBL_MAX, 1.0 ), PI / 2 ) );
    VERIFY( CompDbl( atan2( 1.0, DBL_MAX ), 0.0 ) );
    VERIFY( CompDbl( atan2( -1.0, DBL_MAX ), 0.0 ) );
    VERIFY( CompDbl( hypot( 3.0, 4.0 ), 5.0 ) );
    VERIFY( CompDbl( sinh( 1.0 ), 1.1752011936 ) );
    VERIFY( CompDbl( sinh( 0.0 ), 0.0 ) );
    VERIFY( CompDbl( cosh( 1.0 ), 1.5430806348 ) );
    VERIFY( CompDbl( cosh( 0.0 ), 1.0 ) );
    VERIFY( CompDbl( tanh( 1.0 ), 0.7615941560 ) );
    VERIFY( CompDbl( tanh( 0.0 ), 0.0 ) );
    VERIFY( CompDbl( asinh( 1.1752011936 ), 1.0 ) );
    VERIFY( CompDbl( asinh( 0.0 ), 0.0 ) );
    VERIFY( CompDbl( acosh( 1.5430806348 ), 1.0 ) );
    VERIFY( CompDbl( acosh( 1.0 ), 0.0 ) );
    VERIFY( CompDbl( atanh( 0.7615941560 ), 1.0 ) );
    VERIFY( CompDbl( atanh( 0.0 ), 0.0 ) );
    /*
        Note:
        sinh(1) = 1.175201193643802
        sinh(0) = 0.000000000000000
        cosh(1) = 1.543080634815244
        cosh(0) = 1.000000000000000
        tanh(1) = 0.761594155955765
        tanh(0) = 0.000000000000000
    */
}

void test_fp_and_80x87_math( void )
{
    double      dnum;
    int         inum;
    #ifdef __FPI__
    unsigned    fp_status, fp_control, fp_mask, origbits, bits;
    #endif

    printf( "Testing other floating point " );
    #ifdef __FPI__
    printf( "and 80x87 specific " );
    #endif
    printf( "functions...\n" );
    VERIFY( CompDbl( j0( 2.387 ), 0.009288 ) );
    VERIFY( CompDbl( j1( 2.387 ), 0.522941 ) );
    VERIFY( CompDbl( jn( 2, 2.387 ), 0.428870 ) );
    VERIFY( CompDbl( y0( 2.387 ), 0.511681 ) );
    VERIFY( CompDbl( y1( 2.387 ), 0.094374 ) );
    VERIFY( CompDbl( yn( 2, 2.387 ), -0.432608 ) );
    VERIFY( ceil( 1.0001 ) == 2.0 );
    VERIFY( ceil( -1.0001 ) == -1.0 );
    VERIFY( floor( 1.0001 ) == 1.0 );
    VERIFY( floor( -1.0001 ) == -2.0 );
    VERIFY( CompDbl( exp( 1.0 ), E ) );
    VERIFY( CompDbl( exp( 3.0 ), E * E * E ) );
    VERIFY( exp( 0.0 ) == 1.0 );
    VERIFY( fabs( -1.5 ) == 1.5 );
    VERIFY( fabs( 2.5 ) == 2.5 );
    VERIFY( fabs( 0.0 ) == 0.0 );
    VERIFY( fmod( 2.5, 0.5 ) == 0.0 );
    VERIFY( fmod( 4.5, 2.0 ) == 0.5 );
    VERIFY( fmod( -4.5, 2.0 ) == -0.5 );
    VERIFY( fmod( -7.5, -2.0 ) == -1.5 );
    dnum = frexp( 65535.0, &inum );
    VERIFY( inum == 16 );
    VERIFY( CompDbl( ldexp( dnum, inum ), 65535.0 ) );
    while( inum-- > 0 ) dnum *= 2;
    VERIFY( CompDbl( dnum, 65535.0 ) );
    VERIFY( CompDbl( log( E ), 1.0 ) );
    VERIFY( CompDbl( log( 1.0 ), 0.0 ) );
    VERIFY( CompDbl( log( 1.0 / E ), -1.0 ) );
    VERIFY( CompDbl( log10( 10.0 ), 1.0 ) );
    VERIFY( CompDbl( log10( 1 ), 0.0 ) );
    VERIFY( CompDbl( log10( 0.1 ), -1.0 ) );
    VERIFY( CompDbl( log2( 65536.0 ), 16.0 ) );
    VERIFY( CompDbl( log2( 1 ), 0.0 ) );
    VERIFY( CompDbl( log2( 0.25 ), -2.0 ) );
    VERIFY( CompDbl( sqrt( 99980001.0 ), 9999.0 ) );
    dnum = 1.0/DBL_MIN;
    VERIFY( CompDbl( 1.0 / dnum, DBL_MIN, __LINE__ ) );
    VERIFY( CompDbl( modf( PI, &dnum ), PI - 3.0 ) );
    VERIFY( dnum == 3.0 );
    VERIFY( CompDbl( modf( -PI, &dnum ) , - PI + 3.0 ) );
    VERIFY( dnum == -3.0 );
    VERIFY( pow( 1.0, 123456789.0 ) == 1.0 );
    VERIFY( pow( 2.0, 16.0 ) == 65536.0 );
    VERIFY( CompDbl( pow( E, log(1234.0) ), 1234.0 ) );
    #ifndef __FPI__
    VERIFY( sqrt( -1 ) == 1 );
    // Now my_matherrno should == DOMAIN after calling sqrt( -1 )
    // If not, matherr() fails
    VERIFY( my_matherrno == DOMAIN );
    my_matherrno = 0;   // reset
    VERIFY( sqrt( 0 ) == 0 );
    VERIFY( my_matherrno == 0 );
    _fpreset();
    fp_status = _clear87();
    VERIFY( fp_status == 0 );
    //
    fp_status = _status87();
    VERIFY( fp_status == 0 );
    //
    _fpreset();
    fp_control = _control87( 0, 0 );
    bits = fp_control & MCW_IC;
    bits = (bits == IC_AFFINE) ? IC_PROJECTIVE : IC_AFFINE;
    fp_control = _control87( bits, MCW_IC );
    VERIFY( (fp_control & MCW_IC) ==  bits );
    bits = (bits == IC_AFFINE) ? IC_PROJECTIVE : IC_AFFINE;
    fp_control = _control87( bits, MCW_IC );
    VERIFY( (fp_control & MCW_IC) ==  bits );
    //
    fp_control = _control87( 0, 0 );
    origbits = fp_control & MCW_RC;
    bits = (origbits == RC_NEAR) ? RC_CHOP : RC_NEAR;
    fp_control = _control87( bits, MCW_RC );
    VERIFY( (fp_control & MCW_RC) ==  bits );
    fp_control = _control87( origbits, MCW_RC );
    VERIFY( (fp_control & MCW_RC) ==  origbits );
    //
    _fpreset();
    fp_control = 0;
    fp_mask = EM_PRECISION;
    signal( SIGFPE, (__sig_func)my_handler );
    (void)_control87( fp_control, fp_mask );
    q = a / b;
    VERIFY( sig_count > 0 );
    fp_control = EM_PRECISION;
    (void)_control87( fp_control, fp_mask );
    sig_count = 0;
    q = a / b;
    VERIFY( sig_count == 0 );
    signal( SIGFPE, SIG_DFL );
    #endif
}

int CompDbl( double n1, double n2 )
{
    double num;

    if( MYABS( n1 ) < 0.000001 && MYABS( n2 ) < 0.000001 ) return( TRUE );
    if( n1 == 0.0 || n2 == 0.0 ) {
        return( FALSE );
    } else {
        num = ( n1 - n2 ) / ( n1 < n2 ? n1 : n2 );
        if( MYABS( num ) > 0.0001 ) return( FALSE );
    }
    return( TRUE );
}


void main( void )
{
    test_integer_math();
    test_complex_math();
    test_trig();
    test_fp_and_80x87_math();
    printf( "Tests completed.\n" );
}
