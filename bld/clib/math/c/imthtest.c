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
* Description:  Non-exhaustive test of C library integer math.
*
****************************************************************************/


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __SW_BW
    #include <wdefwin.h>
#endif

#define VERIFY( exp )   if( !(exp) ) {                                      \
                            printf( "%s: ***FAILURE*** at line %d of %s.\n",\
                                    ProgramName, __LINE__,                  \
                                    strlwr(__FILE__) );                     \
                            printf( "%s\n", strerror(errno) );              \
                            NumErrors++;                                    \
                            exit( -1 );                                     \
                        }


char    ProgramName[128];                       /* executable filename */
int     NumErrors = 0;                          /* number of errors */



void TestIntMath( void )
{
    div_t   div_result;
    ldiv_t  ldiv_result;
    int     num[20], ctr;

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

void TestLLIntMath( void )
{
    lldiv_t     lldiv_result;

    VERIFY( llabs( -1 ) == 1 );
    VERIFY( llabs( 0 ) == 0 );
    VERIFY( llabs( 1 ) == 1 );
    VERIFY( llabs( 1234567890 ) == 1234567890 );
    VERIFY( llabs( -1234567890 ) == 1234567890 );
    lldiv_result = lldiv( 0, 100 );
    VERIFY( lldiv_result.quot == 0 && lldiv_result.rem == 0 );
    lldiv_result = lldiv( 100000010, 100000000 );
}


int main( int argc, char *argv[] )
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
    strcpy( ProgramName, strlwr( argv[0] ) );   /* store executable filename */

    /*** Test various functions ***/
    TestIntMath();                              /* integer math */

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
