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
* Description:  Non-exhaustive test of Safer C Library formatted I/O.
*
****************************************************************************/


#define __STDC_WANT_LIB_EXT1__  1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <float.h>
#include <stdarg.h>

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


static int my_scanf_s( const char *fmt, ... )
{
    va_list     arg;
    int         rc;

    va_start( arg, fmt );
    rc = vscanf_s( fmt, arg );
    va_end( arg );
    return( rc );
}

static int my_fscanf_s( FILE * stream, const char *fmt, ... )
{
    va_list     arg;
    int         rc;

    va_start( arg, fmt );
    rc = vfscanf_s( stream, fmt, arg );
    va_end( arg );
    return( rc );
}

static int my_sscanf_s( const char *s, const char *fmt, ... )
{
    va_list     arg;
    int         rc;

    va_start( arg, fmt );
    rc = vsscanf_s( s, fmt, arg );
    va_end( arg );
    return( rc );
}


int Test_scan( void )
/*******************/
{
    int     number;
    int     violations = NumViolations;
    char    buf[64];

    VERIFY( sscanf_s( "123", "%d", &number ) == 1 );
    VERIFY( number == 123 );

    VERIFY( sscanf_s( NULL, "%d", &number ) == EOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( sscanf_s( "123", NULL, &number ) == EOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( sscanf_s( "123", "%d", NULL ) == EOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( sscanf_s( "123", "%*d", NULL ) == 0 );

    VERIFY( sscanf_s( "123", "%s", NULL ) == EOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( sscanf_s( "123", "%c", NULL ) == EOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( sscanf_s( "123", "%f", NULL ) == EOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( sscanf_s( "123", "%n", NULL ) == EOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( sscanf_s( "123", "%*d%n%n", &number, NULL ) == EOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( sscanf_s( " 123", "%s%n", &buf, sizeof( buf ), &number ) == 1 );
    VERIFY( number == 4 && !strcmp( "123", buf ) );

    VERIFY( sscanf_s( " 123", "%s%n", &buf, 3 ) == 0 );

    VERIFY( sscanf_s( "aaa", "%[a]%n", &buf, 4, &number ) == 1 );
    VERIFY( number == 3 && !strcmp( "aaa", buf ) );

    VERIFY( sscanf_s( "aaaa", "%[a]", &buf, 4 ) == 0 );

    memset( buf, 'Q', 6 );
    VERIFY( sscanf_s( "aaa", "%3c%n", &buf, 3, &number ) == 1 );
    VERIFY( number == 3 && !memcmp( "aaaQQQ", buf, 6 ) );

    VERIFY( sscanf_s( "aaa", "%3c%n", &buf, 2 ) == 0 );

    VERIFY( my_sscanf_s( "123", "%d", &number ) == 1 );
    VERIFY( number == 123 );

    VERIFY( my_sscanf_s( NULL, "%d", &number ) == EOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( my_sscanf_s( "123", NULL, &number ) == EOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( my_sscanf_s( "123", "%d", NULL ) == EOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( my_sscanf_s( "123", "%*d", NULL ) == 0 );

    VERIFY( my_sscanf_s( "123", "%s", NULL ) == EOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( my_sscanf_s( "123", "%c", NULL ) == EOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( my_sscanf_s( "123", "%f", NULL ) == EOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( my_sscanf_s( "123", "%n", NULL ) == EOF );
    VERIFY( ++violations == NumViolations );


    VERIFY( scanf_s( NULL ) == EOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( scanf_s( "%n", NULL ) == EOF );
    VERIFY( ++violations == NumViolations );


    VERIFY( my_scanf_s( NULL ) == EOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( my_scanf_s( "%n", NULL ) == EOF );
    VERIFY( ++violations == NumViolations );


    VERIFY( fscanf_s( NULL, "%d", &number ) == EOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( fscanf_s( stdin, NULL ) == EOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( fscanf_s( stdin, "%n", NULL ) == EOF );
    VERIFY( ++violations == NumViolations );


    VERIFY( my_fscanf_s( NULL, "%d", &number ) == EOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( my_fscanf_s( stdin, NULL ) == EOF );
    VERIFY( ++violations == NumViolations );

    VERIFY( my_fscanf_s( stdin, "%n", NULL ) == EOF );
    VERIFY( ++violations == NumViolations );

    return( 1 );
}


static int my_printf_s( const char *fmt, ... )
{
    va_list     arg;
    int         rc;

    va_start( arg, fmt );
    rc = vprintf_s( fmt, arg );
    va_end( arg );
    return( rc );
}

static int my_fprintf_s( FILE *fp, const char *fmt, ... )
{
    va_list     arg;
    int         rc;

    va_start( arg, fmt );
    rc = vfprintf_s( fp, fmt, arg );
    va_end( arg );
    return( rc );
}

static int my_sprintf_s( char *buf, rsize_t n, const char *fmt, ... )
{
    va_list     arg;
    int         rc;

    va_start( arg, fmt );
    rc = vsprintf_s( buf, n, fmt, arg );
    va_end( arg );
    return( rc );
}

static int my_snprintf_s( char *buf, rsize_t n, const char *fmt, ... )
{
    va_list     arg;
    int         rc;

    va_start( arg, fmt );
    rc = vsnprintf_s( buf, n, fmt, arg );
    va_end( arg );
    return( rc );
}


int Test_print( void )
/********************/
{
    char    buf[128];
    int     n;
    int     violations = NumViolations;

    VERIFY( printf_s( NULL ) < 0 );
    VERIFY( ++violations == NumViolations );

    VERIFY( printf_s( "%n", &n ) < 0 );
    VERIFY( ++violations == NumViolations );

    VERIFY( printf_s( "%s", NULL ) < 0 );
    VERIFY( ++violations == NumViolations );


    VERIFY( fprintf_s( NULL, "hello" ) < 0 );
    VERIFY( ++violations == NumViolations );

    VERIFY( fprintf_s( stdout, NULL, 3 ) < 0 );
    VERIFY( ++violations == NumViolations );

    VERIFY( fprintf_s( stdout, "%hhn", &n ) < 0 );
    VERIFY( ++violations == NumViolations );


    VERIFY( my_printf_s( NULL, 3, 6 ) < 0 );
    VERIFY( ++violations == NumViolations );

    VERIFY( my_printf_s( "%jn", &n ) < 0 );
    VERIFY( ++violations == NumViolations );

    VERIFY( my_printf_s( "%S", NULL ) < 0 );
    VERIFY( ++violations == NumViolations );


    VERIFY( my_fprintf_s( NULL, "hi" ) < 0 );
    VERIFY( ++violations == NumViolations );

    VERIFY( my_fprintf_s( stdout, NULL ) < 0 );
    VERIFY( ++violations == NumViolations );

    VERIFY( my_fprintf_s( stdout, "%-8.3zn", &n ) < 0 );
    VERIFY( ++violations == NumViolations );

    VERIFY( my_fprintf_s( stdout, "%S", NULL ) < 0 );
    VERIFY( ++violations == NumViolations );


    VERIFY( sprintf_s( buf, 2, "hi" ) == 0 );
    VERIFY( ++violations == NumViolations );
    VERIFY( *buf == '\0' );

    VERIFY( sprintf_s( buf, sizeof( buf ), "%%n%d", 1 ) == 3 );
    VERIFY( !strcmp( buf, "%n1" ) );

    *buf = 'z';
    VERIFY( sprintf_s( buf, sizeof( buf ), NULL ) == 0 );
    VERIFY( ++violations == NumViolations );
    VERIFY( *buf == '\0' );

    *buf = 'z'; /* preinit output buffer ... */
    VERIFY( sprintf_s( NULL, sizeof( buf ), "hi" ) == 0 );
    VERIFY( ++violations == NumViolations );

    VERIFY( sprintf_s( buf, 0, "hi" ) == 0 );
    VERIFY( ++violations == NumViolations );

#if RSIZE_MAX != SIZE_MAX
    VERIFY( sprintf_s( buf, ~0, "hi" ) == 0 );
    VERIFY( ++violations == NumViolations );
#endif


    VERIFY( my_sprintf_s( NULL, sizeof( buf ), "hi" ) == 0 );
    VERIFY( ++violations == NumViolations );

    VERIFY( my_sprintf_s( buf, 0, "hi" ) == 0 );
    VERIFY( ++violations == NumViolations );

#if RSIZE_MAX != SIZE_MAX
    VERIFY( my_sprintf_s( buf, ~0, "hi" ) == 0 );
    VERIFY( ++violations == NumViolations );
#endif

    VERIFY( *buf == 'z' );  /* ... and make sure no one touched it */

    VERIFY( my_sprintf_s( buf, 5, NULL, 'z', "oo", 36 ) == 0 );
    VERIFY( ++violations == NumViolations );
    VERIFY( *buf == '\0' );

    VERIFY( my_sprintf_s( buf, 6, "%c%s%d", 'z', "oo", 36 ) == 5 );
    VERIFY( !strcmp( buf, "zoo36" ) );

    VERIFY( my_sprintf_s( buf, 5, "%c%s%d", 'z', "oo", 36 ) == 0 );
    VERIFY( ++violations == NumViolations );
    VERIFY( *buf == '\0' );

    VERIFY( my_sprintf_s( buf, sizeof( buf ), "hi%#6.3n", &n ) == 0 );
    VERIFY( ++violations == NumViolations );

    VERIFY( my_sprintf_s( buf, sizeof( buf ), "%%n%s", "Y" ) == 3 );
    VERIFY( !strcmp( buf, "%nY" ) );

    VERIFY( my_sprintf_s( buf, sizeof( buf ), "hi%s", NULL ) == 0 );
    VERIFY( ++violations == NumViolations );
    VERIFY( *buf == '\0' );


    VERIFY( my_snprintf_s( buf, sizeof( buf ), "hi%#6.3n", &n ) < 0 );
    VERIFY( ++violations == NumViolations );
    VERIFY( *buf == '\0' );

    VERIFY( my_snprintf_s( buf, 1, "hi%s", NULL ) < 0 );
    VERIFY( ++violations == NumViolations );
    VERIFY( *buf == '\0' );

    memset( buf, 'Q', 10 );
    VERIFY( my_snprintf_s( buf, 4, "%c%s%d", 'z', "oo", 3676 ) == 7 );
    VERIFY( !memcmp( buf, "zoo\0QQQQQQ", 10 ) );

    VERIFY( my_snprintf_s( buf, sizeof( buf ), "%c%s%d", 'z', "oo", 36 ) == 5 );
    VERIFY( !strcmp( buf, "zoo36" ) );

    VERIFY( my_snprintf_s( NULL, sizeof( buf ), "hi" ) < 0 );
    VERIFY( ++violations == NumViolations );

    VERIFY( my_snprintf_s( buf, 0, "hi" ) < 0 );
    VERIFY( ++violations == NumViolations );

#if RSIZE_MAX != SIZE_MAX
    VERIFY( my_snprintf_s( buf, ~0, "hi" ) < 0 );
    VERIFY( ++violations == NumViolations );
#endif


    VERIFY( snprintf_s( NULL, sizeof( buf ), "hi" ) < 0 );
    VERIFY( ++violations == NumViolations );

    VERIFY( snprintf_s( buf, 0, "hi" ) < 0 );
    VERIFY( ++violations == NumViolations );

#if RSIZE_MAX != SIZE_MAX
    VERIFY( snprintf_s( buf, ~0, "hi" ) < 0 );
    VERIFY( ++violations == NumViolations );
#endif

    VERIFY( *buf == 'z' );

    VERIFY( snprintf_s( buf, sizeof( buf ), NULL, 3, 5 ) < 0 );
    VERIFY( ++violations == NumViolations );
    VERIFY( *buf == '\0' );

    VERIFY( snprintf_s( buf, sizeof( buf ), "hi%#3.55n", &n ) < 0 );
    VERIFY( ++violations == NumViolations );
    VERIFY( *buf == '\0' );

    VERIFY( snprintf_s( buf, sizeof( buf ), "hi%s", NULL ) < 0 );
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

    Test_scan();
    Test_print();

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
