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
* Description:  Non-exhaustive test of the C library time functions.
*
****************************************************************************/


#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
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


char ProgramName[128];                          /* executable filename */
int NumErrors = 0;                              /* number of errors */


/****
***** Program entry point.
****/

int main( int argc, char * const argv[] )
{
    clock_t const       clocktime = clock();
    time_t              tt1;
    time_t              tt2;
    time_t              tt3;
    double              dtime;
    struct tm           tm1;
    struct tm           tm2;
    struct tm const     *gmt;
    char const * const  datestr = "Wed Aug 14 17:23:31 2002\n";
    char                buf[64];
    char                myfile[ sizeof __FILE__ ];

#ifdef __SW_BW
    FILE                *my_stdout;

    my_stdout = freopen( "tmp.log", "a", stdout );
    if( my_stdout == NULL ) {
        fprintf( stderr, "Unable to redirect stdout\n" );
        exit( -1 );
    }
#endif
    ( void ) argc;                      /* Unused */
    /*** Initialize ***/
    strcpy( ProgramName, argv[0] );     /* store filename */
    strlwr( ProgramName );              /* and lower case it */
    strcpy( myfile, __FILE__ );
    strlwr( myfile );

    /*** Test various functions ***/
    tt1 = time( &tt2 );
    /* Unfortunately we have no good way to verify that the current time
     * is being returned correctly. So let's just see if time() behaves
     * consistently.
     */
    VERIFY( tt1 == tt2 );

    tm1.tm_sec   = 31;
    tm1.tm_min   = 23;
    tm1.tm_hour  = 17;
    tm1.tm_mday  = 14;
    tm1.tm_mon   = 7;
    tm1.tm_year  = 102;
    tm1.tm_isdst = -1;
    tt1 = mktime( &tm1 );
    /* See if mktime() works properly */
    VERIFY( tm1.tm_wday == 3 );
    VERIFY( tm1.tm_yday == 225 );

    tm1.tm_sec++;
    tt2 = mktime( &tm1 );
    /* Test difftime() */
    dtime = difftime( tt1, tt2 );
    VERIFY( dtime == -1.0 );
    /* Test localtime() and asctime() */
    VERIFY( strcmp( asctime( localtime( &tt1 ) ), datestr ) == 0 );

    /* Test gmtime() and strtime() */
    gmt = gmtime( &tt2 );
    VERIFY( strftime( buf, sizeof( buf ), "%Y", gmt ) == 4 );
    VERIFY( strcmp( buf, "2002" ) == 0 );
    VERIFY( strftime( buf, sizeof( buf ), "%m", gmt ) == 2 );
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
        gmt = gmtime( &tt3 );
        VERIFY( tm2.tm_sec  == gmt->tm_sec );
        VERIFY( tm2.tm_min  == gmt->tm_min );
        VERIFY( tm2.tm_hour == gmt->tm_hour );
        VERIFY( tm2.tm_mday == gmt->tm_mday );
        VERIFY( tm2.tm_mon  == gmt->tm_mon );
        VERIFY( tm2.tm_year == gmt->tm_year );
    }

    /* This time is the greatest to work */
    tm2.tm_sec   -= 1;
    tt3 = mktime( &tm2 );
    VERIFY( tt3 != (time_t) -1 );
    gmt = gmtime( &tt3 );
    VERIFY( tm2.tm_sec  == gmt->tm_sec );
    VERIFY( tm2.tm_min  == gmt->tm_min );
    VERIFY( tm2.tm_hour == gmt->tm_hour );
    VERIFY( tm2.tm_mday == gmt->tm_mday );
    VERIFY( tm2.tm_mon  == gmt->tm_mon );
    VERIFY( tm2.tm_year == gmt->tm_year );
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
        gmt = gmtime( &tt3 );
        VERIFY( tm2.tm_sec  == gmt->tm_sec );
        VERIFY( tm2.tm_min  == gmt->tm_min );
        VERIFY( tm2.tm_hour == gmt->tm_hour );
        VERIFY( tm2.tm_mday == gmt->tm_mday );
        VERIFY( tm2.tm_mon  == gmt->tm_mon );
        VERIFY( tm2.tm_year == gmt->tm_year );
    }

    /* This time is the greatest to work */
    tm2.tm_sec   -= 2;
    tt3 = mktime( &tm2 );
    VERIFY( tt3 != (time_t) -1 );
    gmt = gmtime( &tt3 );
    VERIFY( tm2.tm_sec  == gmt->tm_sec );
    VERIFY( tm2.tm_min  == gmt->tm_min );
    VERIFY( tm2.tm_hour == gmt->tm_hour );
    VERIFY( tm2.tm_mday == gmt->tm_mday );
    VERIFY( tm2.tm_mon  == gmt->tm_mon );
    VERIFY( tm2.tm_year == gmt->tm_year );
#endif
    /* test strftime format codes */
    tm1.tm_sec   = 57;
    tm1.tm_min   = 19;
    tm1.tm_hour  = 14;
    tm1.tm_mday  = 04;
    tm1.tm_mon   = 5;
    tm1.tm_year  = 106;
    tm1.tm_isdst = -1;
    tt1 = mktime( &tm1 );

    strftime( buf, sizeof( buf ), "%a %A %b %B %c %C", &tm1 );
    VERIFY( 0 == strcmp( buf, "Sun Sunday Jun June Sun Jun 04 14:19:57 2006 20" ) );

    strftime( buf, sizeof( buf ), "%d %D %e %E %f %F", &tm1 );
    VERIFY( 0 == strcmp( buf, "04 06/04/06  4  f 2006-06-04" ) );

    strftime( buf, sizeof( buf ), "%g %G %h %H %i %I", &tm1 );
    VERIFY( 0 == strcmp( buf, "06 2006 Jun 14 i 02" ) );

    strftime( buf, sizeof( buf ), "%j %J %k %K %l %L", &tm1 );
    VERIFY( 0 == strcmp( buf, "155 J k K l L" ) );

    strftime( buf, sizeof( buf ), "%m %M %n %N %o %O", &tm1 );
    VERIFY( 0 == strcmp( buf, "06 19 \n N o " ) );

    strftime( buf, sizeof( buf ), "%p %P %q %Q %r %R", &tm1 );
    VERIFY( 0 == strcmp( buf, "PM P q Q 02:19:57 PM 14:19" ) );

    strftime( buf, sizeof( buf ), "%s %S %t %T %u %U", &tm1 );
    VERIFY( 0 == strcmp( buf, "s 57 \t 14:19:57 7 23" ) );

    strftime( buf, sizeof( buf ), "%v %V %w %W %x %X", &tm1 );
    VERIFY( 0 == strcmp( buf, "v 22 0 22 Sun Jun 04, 2006 14:19:57" ) );

/*  %z %Z timezone dependant codes omitted */
    strftime( buf, sizeof( buf ), "%y %Y %%", &tm1 );
    VERIFY( 0 == strcmp( buf, "06 2006 %" ) );

    strftime( buf, sizeof( buf ), "%Ec %Ex %EY", &tm1 );
    VERIFY( 0 == strcmp( buf, "Sun Jun 04 14:19:57 2006 Sun Jun 04, 2006 2006" ) );

    strftime( buf, sizeof( buf ), "%Od %Oe %OH %OI %OM %OS", &tm1 );
    VERIFY( 0 == strcmp( buf, "04  4 14 02 19 57" ) );

    strftime( buf, sizeof( buf ), "%Ou %OU %OV %Ow %OW %Oy", &tm1 );
    VERIFY( 0 == strcmp( buf, "7 23 22 0 22 06" ) );

    /* test some TZ ENV changes first set OW format */
    setenv( "TZ", "CET-1CED-2,M3.5.0/2:0:0,M10.5.0/3:0:0", 1 );

/*  test now %z %Z timezone dependant codes */
    strftime( buf, sizeof( buf ), "%z %Z", &tm1 );
    VERIFY( 0 == strcmp( buf, "+0100 CET" ) );

    VERIFY( daylight != 0 );
    VERIFY( -3600 == timezone );
    VERIFY( 0 == strcmp( tzname[0], "CET" ) );
    VERIFY( 0 == strcmp( tzname[1], "CED" ) );

    tm1.tm_sec   = 57;
    tm1.tm_min   = 46;
    tm1.tm_hour  = 11;
    tm1.tm_mday  = 16;
    tm1.tm_mon   = 8;
    tm1.tm_year  = 109;
    tm1.tm_isdst = -1;
    tt1 = mktime( &tm1 );

    strftime( buf, sizeof( buf ), "%a %A %b %B %c %C", &tm1 );
    VERIFY( 0 == strcmp( buf,
                 "Wed Wednesday Sep September Wed Sep 16 11:46:57 2009 20" ) );

    VERIFY( 1 == tm1.tm_isdst );

    tm1.tm_sec   = 0;                   /*  2009-10-25 02:00:00 end of dst */
    tm1.tm_min   = 0;
    tm1.tm_hour  = 2;
    tm1.tm_mday  = 25;
    tm1.tm_mon   = 9;
    tm1.tm_year  = 109;
    tm1.tm_isdst = -1;
    tt1 = mktime( &tm1 );

    VERIFY( 0 == tm1.tm_isdst );

    /* test some TZ ENV changes now set OS/2 format */
    setenv( "TZ", "CET-1CED,3,-1,0,7200,10,-1,0,10800,3600", 1 );

/*  test now %z %Z timezone dependant codes */
    strftime( buf, sizeof( buf ), "%z %Z", &tm1 );
    VERIFY( 0 == strcmp( buf, "+0100 CET" ) );

    VERIFY( daylight != 0 );
    VERIFY( -3600 == timezone );
    VERIFY( 0 == strcmp( tzname[0], "CET" ) );
    VERIFY( 0 == strcmp( tzname[1], "CED" ) );

    tm1.tm_sec   = 57;
    tm1.tm_min   = 46;
    tm1.tm_hour  = 11;
    tm1.tm_mday  = 16;
    tm1.tm_mon   = 8;
    tm1.tm_year  = 109;
    tm1.tm_isdst = -1;
    tt1 = mktime( &tm1 );

    strftime( buf, sizeof( buf ), "%a %A %b %B %c %C", &tm1 );
    VERIFY( 0 == strcmp( buf,
                 "Wed Wednesday Sep September Wed Sep 16 11:46:57 2009 20" ) );

    VERIFY( 1 == tm1.tm_isdst );

    tm1.tm_sec   = 0;                   /*  2009-10-25 02:00:00 end of dst */
    tm1.tm_min   = 0;
    tm1.tm_hour  = 2;
    tm1.tm_mday  = 25;
    tm1.tm_mon   = 9;
    tm1.tm_year  = 109;
    tm1.tm_isdst = -1;
    tt1 = mktime( &tm1 );

    VERIFY( 0 == tm1.tm_isdst );
    VERIFY( daylight != 0 );
    VERIFY( -3600 == timezone );
    VERIFY( 0 == strcmp( tzname[0], "CET" ) );
    VERIFY( 0 == strcmp( tzname[1], "CED" ) );

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
