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
* Description:  Non-exhaustive test of the C library environment functions.
*
****************************************************************************/


#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#ifdef __SW_BW
    #include <wdefwin.h>
#endif


#ifndef RSIZE_MAX
    #error RSIZE_MAX must be defined
#endif

#define VERIFY( exp )                                   \
    if( !(exp) ) {                                      \
        printf( "%s: ***FAILURE*** at line %d of %s.\n",\
                ProgramName, __LINE__,                  \
                strlwr(__FILE__) );                     \
        NumErrors++;                                    \
        exit(-1);                                       \
    }

char    ProgramName[128];               /* executable filename */
int     NumErrors = 0;                  /* number of errors */
int     NumViolations;                  /* runtime-constraint violation counter */

#ifdef __SW_BW
    FILE *my_stdout;
#endif


/* Runtime-constraint handler for tests; doesn't abort program. */
void my_constraint_handler( const char *msg, void *ptr, errno_t error )
{
#ifdef DEBUG_MSG
    fprintf( stderr, msg );
#endif
    ++NumViolations;
}


void test_getenv_s( void )
{
    char    buf[128];
    size_t  len = 0;
    int     violations = NumViolations;

    VERIFY( putenv( "NAME=VALUE" ) == 0 );

    set_constraint_handler_s( my_constraint_handler );

    /* Test the "good" case */
    VERIFY( getenv_s( NULL, buf, sizeof( buf ), "NAME" ) == 0 );
    VERIFY( getenv_s( &len, buf, sizeof( buf ), "NAME" ) == 0 );
    VERIFY( len == strlen( "VALUE" ) );
    VERIFY( strcmp( buf, "VALUE" ) == 0 );
    VERIFY( NumViolations == violations );

    /* Test various failing cases */
    VERIFY( getenv_s( &len, buf, sizeof( buf ), "R$#X8ZY" ) != 0 );
    VERIFY( getenv_s( NULL, buf, sizeof( buf ), "R$#X8ZY" ) != 0 );
    VERIFY( getenv_s( NULL, NULL, 0, "R$#X8ZY" ) != 0 );
    VERIFY( len == 0 );
    VERIFY( buf[0] == '\0' );

    VERIFY( getenv_s( &len, buf, 4, "NAME" ) != 0 );
    VERIFY( len == strlen( "VALUE" ) );
    VERIFY( NumViolations == violations );

    VERIFY( getenv_s( &len, NULL, 0, "NAME" ) != 0 );
    VERIFY( len == strlen( "VALUE" ) );
    VERIFY( NumViolations == violations );

    /* Test runtime-constraint violations */
    VERIFY( getenv_s( &len, NULL, sizeof( buf ), "NAME" ) != 0 );
    VERIFY( NumViolations == ++violations );

    VERIFY( getenv_s( &len, buf, sizeof( buf ), NULL ) != 0 );
    VERIFY( NumViolations == ++violations );

#if RSIZE_MAX != SIZE_MAX
    VERIFY( getenv_s( &len, buf, ~0, "NAME" ) != 0 );
    VERIFY( NumViolations == ++violations );
#endif
}

int main( int argc, char **argv )
{
#ifdef __SW_BW
    my_stdout = freopen( "tmp.log", "a", stdout );
    if( my_stdout == NULL ) {
        fprintf( stderr, "Unable to redirect stdout\n" );
        exit( EXIT_FAILURE );
    }
#endif

    /*** Initialize ***/
    strcpy( ProgramName, strlwr(argv[0]) );     /* store filename */

    /*** Test bounds-checking getenv ***/
    test_getenv_s();

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
