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
* Description:  Non-exhaustive test of Safer C Library wide formatted I/O.
*
****************************************************************************/


/* The wide-character tests are separate from test of regular routines
 * for two reasons:
 *
 * - Doing byte output on a stream sets its orientation, preventing later
 *   wide-character output.
 *
 * - Lumping everything into one executable causes problems with code
 *   segment size, especially for compact memory models. Changing the
 *   memory model is obviously not a solution.
 *
 */

#define __STDC_WANT_LIB_EXT1__  1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <float.h>
#include <stdarg.h>
#include <wchar.h>

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

char    ProgramName[FILENAME_MAX];  /* executable filename */
int     NumErrors = 0;              /* number of errors */
int     NumViolations = 0;          /* runtime-constraint violation counter */


/* Runtime-constraint handler for tests; doesn't abort program. */
void my_constraint_handler( const char *msg, void *ptr, errno_t error )
{
#ifdef DEBUG_MSG
    fprintf( stderr, "Runtime-constraint message: %s", msg );
#endif
    ++NumViolations;
}

/**********************/
/* Main test routines */
/**********************/


static int my_wscanf_s( const wchar_t *fmt, ... )
{
    va_list     arg;
    int         rc;

    va_start( arg, fmt );
    rc = vwscanf_s( fmt, arg );
    va_end( arg );
    return( rc );
}

static int my_fwscanf_s( FILE * stream, const wchar_t *fmt, ... )
{
    va_list     arg;
    int         rc;

    va_start( arg, fmt );
    rc = vfwscanf_s( stream, fmt, arg );
    va_end( arg );
    return( rc );
}

static int my_swscanf_s( const wchar_t *s, const wchar_t *fmt, ... )
{
    va_list     arg;
    int         rc;

    va_start( arg, fmt );
    rc = vswscanf_s( s, fmt, arg );
    va_end( arg );
    return( rc );
}


int Test_scan_wide( void )
/************************/
{
    int         number;
    int         violations = NumViolations;
    wchar_t     buf[64];

    VERIFY( swscanf_s( L"123", L"%d", &number ) == 1 );
    VERIFY( number == 123 );

    VERIFY( swscanf_s( NULL, L"%d", &number ) == WEOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( swscanf_s( L"123", NULL, &number ) == WEOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( swscanf_s( L"123", L"%d", NULL ) == WEOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( swscanf_s( L"123", L"%*d", NULL ) == 0 );

    VERIFY( swscanf_s( L"123", L"%s", NULL ) == WEOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( swscanf_s( L"123", L"%c", NULL ) == WEOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( swscanf_s( L"123", L"%f", NULL ) == WEOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( swscanf_s( L"123", L"%n", NULL ) == WEOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( swscanf_s( L"123", L"%*d%n%n", &number, NULL ) == WEOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( swscanf_s( L" 123", L"%s%n", &buf, sizeof( buf ), &number ) == 1 );
    VERIFY( number == 4 && !wcscmp( L"123", buf ) );

    VERIFY( swscanf_s( L" 123", L"%s%n", &buf, 3 ) == 0 );

    VERIFY( swscanf_s( L"aaa", L"%[a]%n", &buf, 4, &number ) == 1 );
    VERIFY( number == 3 && !wcscmp( L"aaa", buf ) );

    VERIFY( swscanf_s( L"aaaa", L"%[a]", &buf, 4 ) == 0 );

    wmemset( buf, 'Q', 6 );
    VERIFY( swscanf_s( L"aaaaa", L"%5c%n", &buf, 5, &number ) == 1 );
    VERIFY( number == 5 && !wmemcmp( L"aaaaaQ", buf, 6 ) );

    VERIFY( swscanf_s( L"aaa", L"%3c%n", &buf, 2 ) == 0 );


    VERIFY( my_swscanf_s( L"123", L"%d", &number ) == 1 );
    VERIFY( number == 123 );

    VERIFY( my_swscanf_s( NULL, L"%d", &number ) == WEOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( my_swscanf_s( L"123", NULL, &number ) == WEOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( my_swscanf_s( L"123", L"%d", NULL ) == WEOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( my_swscanf_s( L"123", L"%*d", NULL ) == 0 );

    VERIFY( my_swscanf_s( L"123", L"%s", NULL ) == WEOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( my_swscanf_s( L"123", L"%c", NULL ) == WEOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( my_swscanf_s( L"123", L"%f", NULL ) == WEOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( my_swscanf_s( L"123", L"%n", NULL ) == WEOF );
    VERIFY( ++violations == NumViolations );


    VERIFY( wscanf_s( NULL ) == WEOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( wscanf_s( L"%n", NULL ) == WEOF );
    VERIFY( ++violations == NumViolations );


    VERIFY( my_wscanf_s( NULL ) == WEOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( my_wscanf_s( L"%n", NULL ) == WEOF );
    VERIFY( ++violations == NumViolations );


    VERIFY( fwscanf_s( NULL, L"%n", &number ) == WEOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( fwscanf_s( stdin, NULL ) == WEOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( fwscanf_s( stdin, L"%n", NULL ) == WEOF );
    VERIFY( ++violations == NumViolations );


    VERIFY( my_fwscanf_s( NULL, L"%d", &number ) == WEOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( my_fwscanf_s( stdin, NULL ) == WEOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( my_fwscanf_s( stdin, L"%n", NULL ) == WEOF );
    VERIFY( ++violations == NumViolations );

    return( 1 );
}


static int my_wprintf_s( const wchar_t *fmt, ... )
{
    va_list     arg;
    int         rc;

    va_start( arg, fmt );
    rc = vwprintf_s( fmt, arg );
    va_end( arg );
    return( rc );
}

static int my_fwprintf_s( FILE *fp, const wchar_t *fmt, ... )
{
    va_list     arg;
    int         rc;

    va_start( arg, fmt );
    rc = vfwprintf_s( fp, fmt, arg );
    va_end( arg );
    return( rc );
}


static int my_swprintf_s( wchar_t *buf, rsize_t n, const wchar_t *fmt, ... )
{
    va_list     arg;
    int         rc;

    va_start( arg, fmt );
    rc = vswprintf_s( buf, n, fmt, arg );
    va_end( arg );
    return( rc );
}

static int my_snwprintf_s( wchar_t *buf, rsize_t n, const wchar_t *fmt, ... )
{
    va_list     arg;
    int         rc;

    va_start( arg, fmt );
    rc = vsnwprintf_s( buf, n, fmt, arg );
    va_end( arg );
    return( rc );
}


int Test_print_wide( void )
/*************************/
{
    wchar_t     buf[128];
    int         n;
    int         violations = NumViolations;


    VERIFY( wprintf_s( NULL ) < 0 );
    VERIFY( ++violations == NumViolations );

    VERIFY( wprintf_s( L"%n", &n ) < 0 );
    VERIFY( ++violations == NumViolations );

    VERIFY( wprintf_s( L"%s", NULL ) < 0 );
    VERIFY( ++violations == NumViolations );


    VERIFY( fwprintf_s( NULL, L"hello" ) < 0 );
    VERIFY( ++violations == NumViolations );

    VERIFY( fwprintf_s( stdout, NULL, 3 ) < 0 );
    VERIFY( ++violations == NumViolations );

    VERIFY( fwprintf_s( stdout, L"%hhn", &n ) < 0 );
    VERIFY( ++violations == NumViolations );


    VERIFY( my_wprintf_s( NULL, 3, 6 ) < 0 );
    VERIFY( ++violations == NumViolations );

    VERIFY( my_fwprintf_s( NULL, L"hi" ) < 0 );
    VERIFY( ++violations == NumViolations );

    VERIFY( my_fwprintf_s( stdout, NULL ) < 0 );
    VERIFY( ++violations == NumViolations );


    VERIFY( swprintf_s( buf, 2, L"hi" ) < 0 );
    VERIFY( ++violations == NumViolations );
    VERIFY( *buf == '\0' );

    VERIFY( swprintf_s( buf, sizeof( buf ), L"%%n%d", 1 ) == 3 );
    VERIFY( !wcscmp( buf, L"%n1" ) );

    *buf = 'z';
    VERIFY( swprintf_s( buf, sizeof( buf ), NULL ) == 0 );
    VERIFY( ++violations == NumViolations );
    VERIFY( *buf == '\0' );

    *buf = 'z'; /* preinit output buffer ... */
    VERIFY( swprintf_s( NULL, sizeof( buf ), L"hi" ) == 0 );
    VERIFY( ++violations == NumViolations );

    VERIFY( swprintf_s( buf, 0, L"hi" ) == 0 );
    VERIFY( ++violations == NumViolations );

#if RSIZE_MAX != SIZE_MAX
    VERIFY( swprintf_s( buf, ~0, L"hi" ) == 0 );
    VERIFY( ++violations == NumViolations );
#endif


    VERIFY( my_swprintf_s( NULL, sizeof( buf ), L"hi" ) == 0 );
    VERIFY( ++violations == NumViolations );

    VERIFY( my_swprintf_s( buf, 0, L"hi" ) == 0 );
    VERIFY( ++violations == NumViolations );

#if RSIZE_MAX != SIZE_MAX
    VERIFY( my_swprintf_s( buf, ~0, L"hi" ) == 0 );
    VERIFY( ++violations == NumViolations );
#endif

    VERIFY( *buf == 'z' );  /* ... and make sure no one touched it */

    VERIFY( my_swprintf_s( buf, 5, NULL, 'z', "oo", 36 ) == 0 );
    VERIFY( ++violations == NumViolations );
    VERIFY( *buf == '\0' );

    VERIFY( my_swprintf_s( buf, 6, L"%c%s%d", 'z', L"oo", 36 ) == 5 );
    VERIFY( !wcscmp( buf, L"zoo36" ) );

    VERIFY( my_swprintf_s( buf, 5, L"%c%s%d", 'z', L"oo", 36 ) < 0 );
    VERIFY( ++violations == NumViolations );
    VERIFY( *buf == '\0' );

    VERIFY( my_swprintf_s( buf, sizeof( buf ), L"hi%#6.3n", &n ) == 0 );
    VERIFY( ++violations == NumViolations );

    VERIFY( my_swprintf_s( buf, sizeof( buf ), L"%%n%s", L"Y" ) == 3 );
    VERIFY( !wcscmp( buf, L"%nY" ) );

    VERIFY( my_swprintf_s( buf, sizeof( buf ), L"hi%s", NULL ) == 0 );
    VERIFY( ++violations == NumViolations );
    VERIFY( *buf == '\0' );


    VERIFY( my_snwprintf_s( buf, sizeof( buf ), L"hi%#6.3n", &n ) < 0 );
    VERIFY( ++violations == NumViolations );
    VERIFY( *buf == '\0' );

    VERIFY( my_snwprintf_s( buf, 1, L"hi%s", NULL ) < 0 );
    VERIFY( ++violations == NumViolations );
    VERIFY( *buf == '\0' );

    wmemset( buf, 'Q', 10 );
    VERIFY( my_snwprintf_s( buf, 4, L"%c%s%d", 'z', L"oo", 3676 ) == 7 );
    VERIFY( !wmemcmp( buf, L"zoo\0QQQQQQ", 10 ) );

    VERIFY( my_snwprintf_s( buf, sizeof( buf ), L"%c%s%d", 'z', L"oo", 36 ) == 5 );
    VERIFY( !wcscmp( buf, L"zoo36" ) );

    VERIFY( my_snwprintf_s( NULL, sizeof( buf ), L"hi" ) < 0 );
    VERIFY( ++violations == NumViolations );

    VERIFY( my_snwprintf_s( buf, 0, L"hi" ) < 0 );
    VERIFY( ++violations == NumViolations );

#if RSIZE_MAX != SIZE_MAX
    VERIFY( my_snwprintf_s( buf, ~0, L"hi" ) < 0 );
    VERIFY( ++violations == NumViolations );
#endif


    VERIFY( snwprintf_s( NULL, sizeof( buf ), L"hi" ) < 0 );
    VERIFY( ++violations == NumViolations );

    VERIFY( snwprintf_s( buf, 0, L"hi" ) < 0 );
    VERIFY( ++violations == NumViolations );

#if RSIZE_MAX != SIZE_MAX
    VERIFY( snwprintf_s( buf, ~0, L"hi" ) < 0 );
    VERIFY( ++violations == NumViolations );
#endif

    VERIFY( *buf == 'z' );

    VERIFY( snwprintf_s( buf, sizeof( buf ), NULL, 3, 5 ) < 0 );
    VERIFY( ++violations == NumViolations );
    VERIFY( *buf == '\0' );

    VERIFY( snwprintf_s( buf, sizeof( buf ), L"hi%#3.55n", &n ) < 0 );
    VERIFY( ++violations == NumViolations );
    VERIFY( *buf == '\0' );

    VERIFY( snwprintf_s( buf, sizeof( buf ), L"hi%s", NULL ) < 0 );
    VERIFY( ++violations == NumViolations );
    VERIFY( *buf == '\0' );

    return( 1 );
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

    /****************************/
    /*  set constraint-handler  */
    /****************************/

    set_constraint_handler_s( my_constraint_handler );

    /******************/
    /* Start of tests */
    /******************/

    Test_scan_wide();
    Test_print_wide();

    /****************/
    /* End of tests */
    /****************/

    /*** Print a pass/fail message and quit ***/
    if( NumErrors != 0 ) {
        printf( "%s: FAILURE (%d errors).\n", ProgramName, NumErrors );
        return( EXIT_FAILURE );
    }
    wprintf( L"Tests completed (%hs).\n", strlwr( argv[0] ) );
#ifdef __SW_BW
    fprintf( stderr, "Tests completed (%s).\n", strlwr( argv[0] ) );
    fclose( my_stdout );
    _dwShutDown();
#endif

    return( 0 );
}
