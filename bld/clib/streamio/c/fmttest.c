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
* Description:  Non-exhaustive test of C library formatted I/O functions.
*               Tests floating-point conversions.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <float.h>
#include <math.h>

#ifdef __SW_BW
    #include <wdefwin.h>
#endif

/* Test macros */

#define VERIFY( exp )   if( !(exp) ) {                                      \
                           printf( "%s: ***FAILURE*** at line %d of %s.\n", \
                                   ProgramName, __LINE__,                   \
                                   strlwr( __FILE__ ) );                    \
                           NumErrors++;                                     \
                           exit( -1 );                                      \
                       }

char    ProgramName[FILENAME_MAX];      /* executable filename */
int     NumErrors = 0;                  /* number of errors */


/**********************/
/* Main test routines */
/**********************/

int Test_scan( void )
/*******************/
{
    float   f, g;


    VERIFY( sscanf( "-82.25 31.6e-1", "%f%f", &f, &g ) == 2 );
    VERIFY( f == -82.25f );
    VERIFY( g == 3.16f );

    return( 1 );
}


int Test_string_to_float( void )
/******************************/
{
    double  x, y;
    char    *eptr;


    /* This one needs full 17 decimal places */
    x = strtod( "1.1920928955078125E-007", NULL );
    y = strtod( "0x1P-23", NULL );
    VERIFY( x == y );

    x = strtod( "-1.0e-0a", &eptr );
    VERIFY( x == -1.0 );
    VERIFY( *eptr == 'a' );

    x = strtod( ".", &eptr );
    VERIFY( x == 0.0 );
    VERIFY( *eptr == '.' );

    x = strtod( "+3.14159265", &eptr );
    VERIFY( x == 3.14159265 );
    VERIFY( *eptr == '\0' );

    x = strtod( "-0", NULL );
    VERIFY( x == 0 );   // Should that be true??

    x = strtod( "0x1g", &eptr );
    VERIFY( x == 1.0 );
    VERIFY( *eptr == 'g' );

    x = strtod( "0x2", &eptr );
    VERIFY( x == 2.0 );

    x = strtod( "0x4", &eptr );
    VERIFY( x == 0x4 );

    x = strtod( "0x8", NULL );
    VERIFY( x == 8.0 );

    x = strtod( "0x10", &eptr );
    VERIFY( x == 16.0 );

    x = strtod( "-0x10.0p-0z", &eptr );
    VERIFY( x == -16 );
    VERIFY( *eptr == 'z' );

    x = strtod( "-0x100.0P-8P", &eptr );
    VERIFY( x == -1 );
    VERIFY( *eptr == 'P' );

    x = strtod( "0x.cq", &eptr );
    VERIFY( x == 12.0 / 16 );
    VERIFY( *eptr == 'q' );

    x = strtod( "0x.0000Ap20", &eptr );
    VERIFY( x == 10 );

    x = strtod( "0xF.a", &eptr );
    VERIFY( x == 250.0 / 16 );

    x = strtod( "0xfAp-04", &eptr );
    VERIFY( x == 250.0 / 16 );

    x = strtod( "0x1..0p3", &eptr );
    VERIFY( x == 1 );
    VERIFY( !strcmp( eptr, ".0p3" ) );

    x = strtod( "0x0000000000000000000000000000000000000000000000001234567890123j", &eptr );
    VERIFY( x == 0x1234567890123 );
    VERIFY( *eptr == 'j' );

    x = strtod( "-0x1p-1a", &eptr );
    VERIFY( x == -.5 );
    VERIFY( *eptr == 'a' );

    x = strtod( "NaNumber", &eptr );
    VERIFY( isnan(x) && !signbit( x ) );
    VERIFY( *eptr == 'u' );

    x = strtod( "NAN()", &eptr );
    VERIFY( isnan(x) && !signbit( x ) );
    VERIFY( *eptr == '\0' );

    x = strtod( "-nan(non_sense_354)", &eptr );
    VERIFY( isnan( x ) && signbit( x ) );
    VERIFY( *eptr == '\0' );

    x = strtod( "Infiniti", &eptr );
    VERIFY( isinf( x ) && !signbit( x ) );
    VERIFY( !strcmp( eptr, "initi" ) );

    x = strtod( "InFiNiTy01", &eptr );
    VERIFY( isinf( x ) && !signbit( x ) );
    VERIFY( *eptr == '0' );

    x = strtod( "-INF", NULL );
    VERIFY( isinf( x ) && signbit( x ) );

    x = strtod( "1.0e99999", NULL );
    VERIFY( isinf( x ) && !signbit( x ) );

    x = strtod( "-1.0e99999", NULL );
    VERIFY( isinf( x ) && signbit( x ) );

    return( 1 );
}


int Test_scan_std_xmp( void )
/***************************/
{
    int     i;
    float   x;
    char    name[50];
    int     d1, d2, n1, n2;

    /* Examples from ISO C99 standard */

    VERIFY( sscanf( "25 54.32E-1 thompson", "%d%f%s", &i, &x, name ) == 3 );
    VERIFY( i == 25 );
    VERIFY( x == 5.432f );
    VERIFY( !strcmp( name, "thompson" ) );

    VERIFY( sscanf( "56789 0123 56a72", "%2d%f%*d %[0123456789]", &i, &x, name ) == 3 );
    VERIFY( i == 56 );
    VERIFY( x == 789.0f );
    VERIFY( !strcmp( name, "56" ) );

    d2 = 34;
    VERIFY( sscanf( "123", "%d%n%n%d", &d1, &n1, &n2, &d2 ) == 1 );
    VERIFY( d1 == 123 );
    VERIFY( d2 == 34 );
    VERIFY( n1 == 3 );
    VERIFY( n2 == 3 );

    return( 1 );
}


int Test_print_float( void )
/**************************/
{
    char        buf[128];
    long double lval;
    double      dval;
    float       fval;

    VERIFY( sprintf( buf, "%+#22.14e", 123456789.0 ) == 22 );
    VERIFY( !strcmp( buf, " +1.23456789000000e+08" ) );

    fval = 4.5;
    VERIFY( sprintf( buf, "%2.4f", fval ) == 6 );
    VERIFY( !strcmp( buf, "4.5000" ) );

    VERIFY( sprintf( buf, "%05.2f", 4.5f ) == 5 );
    VERIFY( !strcmp( buf, "04.50" ) );

    dval = 0.8;
    VERIFY( sprintf( buf, "%0f", dval ) == 8 );
    VERIFY( !strcmp( buf, "0.800000" ) );

    VERIFY( sprintf( buf, "%.0f", dval ) == 1 );
    VERIFY( !strcmp( buf, "1" ) );

    lval = 4.5123;
    VERIFY( sprintf( buf, "%2.4Lf", lval ) == 6 );
    VERIFY( !strcmp( buf, "4.5123" ) );

    VERIFY( sprintf( buf, "%05.2Lf", 4.5123L ) == 5 );
    VERIFY( !strcmp( buf, "04.51" ) );

    VERIFY( sprintf( buf, "%2.4e", 4.5 ) == 10 );
    VERIFY( !strcmp( buf, "4.5000e+00" ) );

    VERIFY( sprintf( buf, "%2.4g", 4.5 ) == 3 );
    VERIFY( !strcmp( buf, "4.5" ) );

    VERIFY( sprintf( buf, "%2.4Lg", 4.5l ) == 3 );
    VERIFY( !strcmp( buf, "4.5" ) );

    /* Test rounding */
    VERIFY( sprintf( buf, "%0.9e", 99999999.99 ) == 15 );
    VERIFY( !strcmp( buf, "9.999999999e+07" ) );

    VERIFY( sprintf( buf, "%0.9e", 999999999.99 ) == 15 );
    VERIFY( !strcmp( buf, "1.000000000e+09" ) );

    VERIFY( sprintf( buf, "%0.9e", 9999999999.99 ) == 15 );
    VERIFY( !strcmp( buf, "1.000000000e+10" ) );

    VERIFY( sprintf( buf, "%0.9e", 99999999999.99 ) == 15 );
    VERIFY( !strcmp( buf, "1.000000000e+11" ) );

#if defined( _NAN ) && defined( _INF )
    /* Test NaN/inf formatting */
    VERIFY( sprintf( buf, "%e", _INF ) == 3 );
    VERIFY( !strcmp( buf, "inf" ) );

    VERIFY( sprintf( buf, "%f", _INF ) == 3 );
    VERIFY( !strcmp( buf, "inf" ) );

    VERIFY( sprintf( buf, "%g", _INF ) == 3 );
    VERIFY( !strcmp( buf, "inf" ) );

    VERIFY( sprintf( buf, "%E", _INF ) == 3 );
    VERIFY( !strcmp( buf, "INF" ) );

    VERIFY( sprintf( buf, "%G", _INF ) == 3 );
    VERIFY( !strcmp( buf, "INF" ) );

    VERIFY( sprintf( buf, "%e", _NAN ) == 3 );
    VERIFY( !strcmp( buf, "nan" ) );

    VERIFY( sprintf( buf, "%f", _NAN ) == 3 );
    VERIFY( !strcmp( buf, "nan" ) );

    VERIFY( sprintf( buf, "%g", _NAN ) == 3 );
    VERIFY( !strcmp( buf, "nan" ) );

    VERIFY( sprintf( buf, "%E", _NAN ) == 3 );
    VERIFY( !strcmp( buf, "NAN" ) );

    VERIFY( sprintf( buf, "%G", _NAN ) == 3 );
    VERIFY( !strcmp( buf, "NAN" ) );

    VERIFY( sprintf( buf, "%5g", _INF ) == 5 );
    VERIFY( !strcmp( buf, "  inf" ) );

    VERIFY( sprintf( buf, "%05.2E", _NAN ) == 5 );
    VERIFY( !strcmp( buf, "  NAN" ) );

    /* Currently %F is a far pointer modified in some libs, to
     * be changed later. Most libs are ISO C compliant in this regard.
     */
#if !defined( __DOS__ ) && !defined( _M_IX86 )
    VERIFY( sprintf( buf, "%F", _INF ) == 3 );
    VERIFY( !strcmp( buf, "INF" ) );

    VERIFY( sprintf( buf, "%F", _NAN ) == 3 );
    VERIFY( !strcmp( buf, "NAN" ) );
#endif
#endif

    return( 0 );
}


int Test_print_char_str( void )
/*****************************/
{
    char    buf[128];

    VERIFY( sprintf( buf, "a%ce", 'x' ) == 3 );
    VERIFY( !strcmp( buf, "axe" ) );

    VERIFY( sprintf( buf, "%3c", 'x' ) == 3 );
    VERIFY( !strcmp( buf, "  x" ) );

    VERIFY( sprintf( buf, "%-3c", 'x' ) == 3 );
    VERIFY( !strcmp( buf, "x  " ) );

    VERIFY( sprintf( buf, "%4s", "zz" ) == 4 );
    VERIFY( !strcmp( buf, "  zz" ) );

    VERIFY( sprintf( buf, "%-4s", "zz" ) == 4 );
    VERIFY( !strcmp( buf, "zz  " ) );

    /* NB: The following tests exercise behaviour that is undefined
     * by ISO C. Nevertheless, defining the results is not unreasonable
     * and helps compatibility (intended to be MSVC comaptible).
     */
    VERIFY( sprintf( buf, "%03c", 'x' ) == 3 );
    VERIFY( !strcmp( buf, "00x" ) );

    VERIFY( sprintf( buf, "%-03c", 'x' ) == 3 );
    VERIFY( !strcmp( buf, "x  " ) );

    VERIFY( sprintf( buf, "%04s", "zz" ) == 4 );
    VERIFY( !strcmp( buf, "00zz" ) );

    VERIFY( sprintf( buf, "%-04s", "zz" ) == 4 );
    VERIFY( !strcmp( buf, "zz  " ) );

    return( 0 );
}

int main( int argc, char *argv[] )
/********************************/
{
#ifdef __SW_BW
    FILE    *my_stdout;

    my_stdout = freopen( "tmp.log", "a", stdout );
    if( my_stdout == NULL ) {
        fprintf( stderr, "Unable to redirect stdout\n" );
        exit( -1 );
    }
#endif
    /*** Initialize ***/
    strcpy( ProgramName, strlwr( argv[0] ) );   /* store filename */

    /******************/
    /* Start of tests */
    /******************/

    Test_scan();
    Test_scan_std_xmp();
    Test_string_to_float();
    Test_print_float();
    Test_print_char_str();

    /****************/
    /* End of tests */
    /****************/

    /*** Print a pass/fail message and quit ***/
    if( NumErrors != 0 ) {
        printf( "%s: FAILURE (%d errors).\n", ProgramName, NumErrors );
        return( EXIT_FAILURE );
    }
    printf( "Tests completed (%s).\n", strlwr( argv[0] ) );
#ifdef __SW_BW
    fprintf( stderr, "Tests completed (%s).\n", strlwr( argv[0] ) );
    fclose( my_stdout );
    _dwShutDown();
#endif

    return( 0 );
}
