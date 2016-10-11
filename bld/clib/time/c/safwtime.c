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
* Description:  Non-exhaustive test of the Safer C library wide time functions.
*
******************************************************************************/


#define __STDC_WANT_LIB_EXT1__ 1       // Enable Safer C interfaces

#include <wchar.h>
#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <env.h>



errno_t _wasctime_s( wchar_t *s, rsize_t maxsize, const struct tm *timeptr );

errno_t _wctime_s( wchar_t *s, rsize_t maxsize, const time_t *timer );

#ifdef __SW_BW
    #include <wdefwin.h>
#endif

#define VERIFY( exp )   if( !(exp) ) {                                      \
                            printf( "%s: ***FAILURE*** at line %d of %s.\n",\
                                    ProgramName, __LINE__, myfile );        \
                            NumErrors++;                                    \
                            exit( -1 );                                     \
                        }


char        ProgramName[128];           /* executable filename */
int         NumErrors = 0;              /* number of errors */
char        myfile[ sizeof __FILE__ ];
int         NumViolations = 0;          /* runtime-constraint violation counter */

/***************************************************************************/
/*  Test Safer C Library time routines                                     */
/***************************************************************************/

/* Runtime-constraint handler for tests; doesn't abort program. */
void my_constraint_handler( const char *msg, void *ptr, errno_t error )
{
#if DEBUG_MSG
    fprintf( stderr, "Runtime-constraint in %s", msg );
#endif
    ++NumViolations;
}


void testtime_s( void )
{
    int                 violations = NumViolations;
    time_t              tt1;
    time_t              tt2;
    struct tm           tm1;
    struct tm           tm2;
    wchar_t const * const  datestr = L"Wed Aug 14 17:23:31 2002\n";
    wchar_t             buf[64];

    /*** Test various functions ***/

    tm1.tm_sec   = 31;
    tm1.tm_min   = 23;
    tm1.tm_hour  = 17;
    tm1.tm_mday  = 14;
    tm1.tm_mon   = 7;
    tm1.tm_year  = 102;
    tm1.tm_isdst = -1;
    tt1 = mktime( &tm1 );

    tm1.tm_sec++;
    tt2 = mktime( &tm1 );

    /* Test localtime_s() and _wasctime_s() */
    VERIFY( localtime_s( &tt1, &tm2 ) == &tm2 );

    VERIFY( _wasctime_s( buf, sizeof(buf), &tm2 ) == 0 );
    VERIFY( NumViolations == violations );
    VERIFY( wcscmp( buf, datestr ) == 0 );

    VERIFY( _wasctime_s( NULL, sizeof(buf), &tm2 ) != 0 );
    VERIFY( NumViolations == ++violations );

    buf[0] = L'X';
    VERIFY( _wasctime_s( buf, 25, &tm2 ) != 0 );
    VERIFY( NumViolations == ++violations );
    VERIFY( L'\0' == buf[0] );

#if RSIZE_MAX != SIZE_MAX
    buf[0] = L'X';
    VERIFY( _wasctime_s( buf, ~0, &tm2 ) != 0 );
    VERIFY( NumViolations == ++violations );
    VERIFY( L'X' == buf[0] );
#endif

    tm2.tm_year = -1;
    buf[0] = L'X';
    VERIFY( _wasctime_s( buf, sizeof(buf), &tm2 ) != 0 );
    VERIFY( NumViolations == ++violations );
    VERIFY( L'\0' == buf[0] );

    tm2.tm_year = 9999+1;
    buf[0] = L'X';
    VERIFY( _wasctime_s( buf, sizeof(buf), &tm2 ) != 0 );
    VERIFY( NumViolations == ++violations );
    VERIFY( L'\0' == buf[0] );

    /*  _wctime_s()                                       */
    VERIFY( _wctime_s( buf, sizeof(buf), &tt1 ) == 0 );
    VERIFY( NumViolations == violations );
    VERIFY( wcscmp( buf, datestr ) == 0 );

    VERIFY( _wctime_s( NULL, sizeof(buf), &tt1 ) != 0 );
    VERIFY( NumViolations == ++violations );

    buf[0] = L'X';
    VERIFY( _wctime_s( buf, 25, &tt1 ) != 0 );
    VERIFY( NumViolations == ++violations );
    VERIFY( L'\0' == buf[0] );

#if RSIZE_MAX != SIZE_MAX
    buf[0] = L'X';
    VERIFY( _wctime_s( buf, ~0, &tt1 ) != 0 );
    VERIFY( NumViolations == ++violations );
    VERIFY( L'X' == buf[0] );
#endif


}


/****
***** Program entry point.
****/

int main( int argc, char * const argv[] )
{
#ifdef __SW_BW
    FILE    *my_stdout;

    my_stdout = freopen( "tmp.log", "a", stdout );
    if( my_stdout == NULL ) {
        fprintf( stderr, "Unable to redirect stdout\n" );
        exit( EXIT_FAILURE );
    }
#endif
    ( void )argc;                       /* Unused */
    /*** Initialize ***/
    strcpy( ProgramName, argv[0] );     /* store filename */
    strlwr( ProgramName );              /* and lower case it */
    strcpy( myfile, __FILE__ );
    strlwr( myfile );

    /***************************************************************************/
    /*  Test Safer C Library time routines                                     */
    /***************************************************************************/

    /***********************************************************************/
    /*  set constraint-handler                                             */
    /***********************************************************************/

    set_constraint_handler_s( my_constraint_handler );

    testtime_s();

    /*** Print a pass/fail message and quit ***/
    if( NumErrors != 0 ) {
        printf( "%s: FAILURE (%d errors).\n", ProgramName, NumErrors );
        return( EXIT_FAILURE );
    }
    printf( "Tests completed (%s).\n", ProgramName );
#ifdef __SW_BW
    fprintf( stderr, "Tests completed (%s).\n", ProgramName );
    fclose( my_stdout );
    _dwShutDown();
#endif
    return( EXIT_SUCCESS );
}
