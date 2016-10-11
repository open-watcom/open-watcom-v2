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
* Description:  Non-exhaustive test of the Safer C library time functions.
*
****************************************************************************/


#define __STDC_WANT_LIB_EXT1__ 1       // Enable Safer C interfaces

#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <env.h>


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
    fprintf( stderr, "Runtime-constraint in %s", msg );
    ++NumViolations;
}


void testtime_s( void )
{
    int                 violations = NumViolations;
    clock_t const       clocktime = clock();
    time_t              tt1;
    time_t              tt2;
    time_t              tt3;
    struct tm           tm1;
    struct tm           tm2;
    struct tm           gmt;
    char const * const  datestr = "Wed Aug 14 17:23:31 2002\n";
    char                buf[64];

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

    /* Test localtime_s() and asctime_s() */
    VERIFY( localtime_s( &tt1, &tm2 ) == &tm2 );

    VERIFY( asctime_s( buf, sizeof(buf), &tm2 ) == 0 );
    VERIFY( NumViolations == violations );
    VERIFY( strcmp( buf, datestr ) == 0 );

    VERIFY( localtime_s( NULL, &tm2 ) == NULL );
    VERIFY( NumViolations == ++violations );

    VERIFY( localtime_s( &tt1, NULL ) == NULL );
    VERIFY( NumViolations == ++violations );

    VERIFY( asctime_s( NULL, sizeof(buf), &tm2 ) != 0 );
    VERIFY( NumViolations == ++violations );

    buf[0] = 'X';
    VERIFY( asctime_s( buf, 25, &tm2 ) != 0 );
    VERIFY( NumViolations == ++violations );
    VERIFY( '\0' == buf[0] );

#if RSIZE_MAX != SIZE_MAX
    buf[0] = 'X';
    VERIFY( asctime_s( buf, ~0, &tm2 ) != 0 );
    VERIFY( NumViolations == ++violations );
    VERIFY( 'X' == buf[0] );
#endif

    tm2.tm_year = -1;
    buf[0] = 'X';
    VERIFY( asctime_s( buf, sizeof(buf), &tm2 ) != 0 );
    VERIFY( NumViolations == ++violations );
    VERIFY( '\0' == buf[0] );

    tm2.tm_year = 9999+1;
    buf[0] = 'X';
    VERIFY( asctime_s( buf, sizeof(buf), &tm2 ) != 0 );
    VERIFY( NumViolations == ++violations );
    VERIFY( '\0' == buf[0] );

    /*  ctime_s()                                       */
    VERIFY( ctime_s( buf, sizeof(buf), &tt1 ) == 0 );
    VERIFY( NumViolations == violations );
    VERIFY( strcmp( buf, datestr ) == 0 );

    VERIFY( ctime_s( NULL, sizeof(buf), &tt1 ) != 0 );
    VERIFY( NumViolations == ++violations );

    buf[0] = 'X';
    VERIFY( ctime_s( buf, 25, &tt1 ) != 0 );
    VERIFY( NumViolations == ++violations );
    VERIFY( '\0' == buf[0] );

#if RSIZE_MAX != SIZE_MAX
    buf[0] = 'X';
    VERIFY( ctime_s( buf, ~0, &tt1 ) != 0 );
    VERIFY( NumViolations == ++violations );
    VERIFY( 'X' == buf[0] );
#endif


    /* Test gmtime() and strtime() */
    VERIFY( gmtime_s( NULL, &tm2 ) == NULL);
    VERIFY( NumViolations == ++violations );

    VERIFY( gmtime_s( &tt2, NULL ) == NULL);
    VERIFY( NumViolations == ++violations );


    VERIFY( gmtime_s( &tt2 , &gmt ) != NULL );
    VERIFY( strftime( buf, sizeof( buf ), "%Y", &gmt ) == 4 );
    VERIFY( strcmp( buf, "2002" ) == 0 );
    VERIFY( strftime( buf, sizeof( buf ), "%m", &gmt ) == 2 );
    VERIFY( strcmp( buf, "08" ) == 0 );

    /* Make sure clock() isn't going backwards */
    VERIFY( clocktime <= clock() );

    /* Set TZ to UTC; this is so that mktime() doesn't use any offsets
     * and we can test the boundary values of time_t in the tests below.
     */
    setenv( "TZ", "GMT0", 1 );

    /* This time is the lowest to overflow in UNIX - has value 0x80000000 */
    tm2.tm_sec   = 8;
    tm2.tm_min   = 14;
    tm2.tm_hour  = 3;
    tm2.tm_mday  = 19;
    tm2.tm_mon   = 0;
    tm2.tm_year  = 138;
    tm2.tm_isdst = -1;
    tt3 = mktime( &tm2 );
    if( tt3 != (time_t) -1 ) {
        VERIFY( gmtime_s( &tt3, &gmt ) == &gmt );
        VERIFY( tm2.tm_sec  == gmt.tm_sec );
        VERIFY( tm2.tm_min  == gmt.tm_min );
        VERIFY( tm2.tm_hour == gmt.tm_hour );
        VERIFY( tm2.tm_mday == gmt.tm_mday );
        VERIFY( tm2.tm_mon  == gmt.tm_mon );
        VERIFY( tm2.tm_year == gmt.tm_year );
    }

    /* This time is the greatest to work */
    tm2.tm_sec   -= 1;
    tt3 = mktime( &tm2 );
    VERIFY( tt3 != (time_t) -1 );
    VERIFY( gmtime_s( &tt3, &gmt ) == &gmt );
    VERIFY( tm2.tm_sec  == gmt.tm_sec );
    VERIFY( tm2.tm_min  == gmt.tm_min );
    VERIFY( tm2.tm_hour == gmt.tm_hour );
    VERIFY( tm2.tm_mday == gmt.tm_mday );
    VERIFY( tm2.tm_mon  == gmt.tm_mon );
    VERIFY( tm2.tm_year == gmt.tm_year );
#ifndef __UNIX__ /* time_t is signed */
    /* This time is the lowest to overflow - has value 0 */
    tm2.tm_sec   = 16;
    tm2.tm_min   = 28;
    tm2.tm_hour  = 6;
    tm2.tm_mday  = 7;
    tm2.tm_mon   = 1;
    tm2.tm_year  = 206;
    tm2.tm_isdst = -1;
    tt3 = mktime( &tm2 );
    if( tt3 != (time_t) -1 ) {
        VERIFY( gmtime_s( &tt3, &gmt ) == &gmt );
        VERIFY( tm2.tm_sec  == gmt.tm_sec );
        VERIFY( tm2.tm_min  == gmt.tm_min );
        VERIFY( tm2.tm_hour == gmt.tm_hour );
        VERIFY( tm2.tm_mday == gmt.tm_mday );
        VERIFY( tm2.tm_mon  == gmt.tm_mon );
        VERIFY( tm2.tm_year == gmt.tm_year );
    }

    /* This time is the greatest to work */
    tm2.tm_sec   -= 2;
    tt3 = mktime( &tm2 );
    VERIFY( tt3 != (time_t) -1 );
    VERIFY( gmtime_s( &tt3, &gmt ) == &gmt );
    VERIFY( tm2.tm_sec  == gmt.tm_sec );
    VERIFY( tm2.tm_min  == gmt.tm_min );
    VERIFY( tm2.tm_hour == gmt.tm_hour );
    VERIFY( tm2.tm_mday == gmt.tm_mday );
    VERIFY( tm2.tm_mon  == gmt.tm_mon );
    VERIFY( tm2.tm_year == gmt.tm_year );
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
        exit( -1 );
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
    return( 0 );
}
