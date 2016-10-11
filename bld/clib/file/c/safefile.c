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
* Description:  Simple test of clib file I/O routines.
*
****************************************************************************/


#define __STDC_WANT_LIB_EXT1__   1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utime.h>
#include <errno.h>
#include <time.h>

#define NUM_FILES       50
#ifdef __SW_BW
    #include <wdefwin.h>
    #define VERIFY( expr ) if( !(expr) ) {                                  \
                              printf( "***FAILURE*** Condition failed:" );  \
                              printf( " %s, line %u.\n", #expr, __LINE__ ); \
                              printf( "Abnormal termination.\n" );          \
                              exit( -1 );                                   \
                           }
#else
    #define VERIFY( expr ) if( !(expr) ) {                                  \
                              printf( "***FAILURE*** Condition failed:" );  \
                              printf( " %s, line %u.\n", #expr, __LINE__ ); \
                              exit( -1 );                                   \
                           }
#endif
#define EXPECT( expr )  if( !(expr) ) {                                     \
                          printf( "*WARNING* Condition failed:" );          \
                          printf( " %s, line %u.\n", #expr, __LINE__ );     \
                          ++warnings;                                       \
                        }

#define VERIFYS( exp )   if( !(exp) ) {                                      \
                            printf( "%s: ***FAILURE*** at line %d of %s.\n",\
                                    ProgramName, __LINE__,                  \
                                    strlwr(__FILE__) );                     \
                            NumErrors++;                                    \
                            exit( -1 );                                     \
                        }


char    ProgramName[128];   /* executable filename */
int     NumErrors = 0;      /* number of errors */
int     NumViolations = 0;  /* runtime-constraint violation counter */


/* Runtime-constraint handler for tests; doesn't abort program. */
void my_constraint_handler( const char *msg, void *ptr, errno_t error )
{
#ifdef DEBUG_MSG
    fprintf( stderr, "Runtime-constraint in %s", msg );
#endif
    ++NumViolations;
}

unsigned warnings = 0;

void main( int argc, char *argv[] )
{
    char filename[NUM_FILES][L_tmpnam_s];
    FILE *fp;
    int ctr, ctr2;
    struct stat info;
    errno_t rc;

    int     violations = NumViolations;

    #ifdef __SW_BW
        FILE *my_stdout;
        my_stdout = freopen( "tmp.log", "a", stdout );
        if( my_stdout == NULL ) {
            fprintf( stderr, "Unable to redirect stdout\n" );
            exit( -1 );
        }
    #endif
    argc=argc;

    /*** Initialize ***/
    strcpy( ProgramName, strlwr( argv[0] ) );   /* store filename */

    /***********************************************************************/
    /*  set constraint-handler                                             */
    /***********************************************************************/

    set_constraint_handler_s( my_constraint_handler );


    /* test runtime constraints */
    rc = tmpnam_s( NULL, sizeof( filename[0] ) );
    VERIFYS( rc != 0 );
    VERIFYS( NumViolations == ++violations );

    filename[0][0] = 'x';
    rc = tmpnam_s( filename[0], 0 );
    VERIFYS( rc != 0 );
    VERIFYS( filename[0][0] == 'x' );
    VERIFYS( NumViolations == ++violations );

    rc = tmpnam_s( filename[0], 4 ); /* only space for .aaa */
    VERIFYS( rc != 0 );
    VERIFYS( filename[0][0] == '\0' );
    VERIFYS( NumViolations == ++violations );


    for( ctr = 0; ctr < NUM_FILES; ++ctr ) {
        rc = tmpnam_s( filename[ctr], sizeof( filename[0] ) );
        VERIFYS( rc == 0 );
        VERIFY( remove( filename[ctr] ) != 0 ); // remove should fail
        VERIFY( errno == ENOENT );
        for( ctr2 = 0; ctr2 < ctr; ++ctr2 ) {
            VERIFY( strcmp( filename[ctr], filename[ctr2] ) != 0 );
            // Make sure all tmpfiles are unique
        }
    }
    VERIFY( utime( filename[0], NULL ) == -1 );
    EXPECT( errno == ENOENT );
    for( ctr = 0; ctr < 2; ++ctr ) {
        if( ( fp = fopen( filename[ctr], "w" ) ) == NULL ) {
            fprintf( stderr, "Internal: fopen() failed\n" );
            perror( filename[ctr] );
            exit( -1 );
        }
        fclose( fp );
    }
#ifndef __UNIX__    // rename is allowed
    VERIFY( rename( filename[0], filename[1] ) != 0 );  // rename should fail
    EXPECT( (errno == EACCES)||(errno == EEXIST) );
#endif
    VERIFY( rename( filename[0], filename[2] ) == 0 );
    VERIFY( remove( filename[0] ) != 0 );   // filename[0] was renamed -> DNE
    EXPECT( errno == ENOENT );
    VERIFY( remove( filename[2] ) == 0 );
    VERIFY( access( filename[2], F_OK ) == -1 );// filename[2] was removed
    EXPECT( errno == ENOENT );
    VERIFY( access( filename[1], F_OK ) == 0 ); // filename[1] is still here
    VERIFY( access( filename[1], R_OK ) == 0 ); // read permission
    VERIFY( access( filename[1], W_OK ) == 0 ); // write permission
    VERIFY( access( argv[0], X_OK ) == 0 );     // execute permission
    VERIFY( chmod( filename[1], S_IRUSR | S_IRGRP ) == 0 );
    // filename[1] is now read-only
    VERIFY( access( filename[1], W_OK ) == -1 );
    EXPECT( errno == EACCES );
#if !defined( __UNIX__ ) && !defined( __RDOS__ )    // remove would succeed
    VERIFY( remove( filename[1] ) != 0 );
    EXPECT( errno == EACCES );
#endif
    VERIFY( chmod( filename[1], S_IRWXU | S_IRWXG ) == 0 );
    VERIFY( stat( filename[1], &info ) == 0 );
#ifndef __UNIX__
    EXPECT( 0 <= info.st_dev && info.st_dev < 26 );
#endif
    VERIFY( utime( filename[1], NULL ) == 0 );
    VERIFY( remove( filename[1] ) == 0 );
    printf( "Tests completed (%s).\n", strlwr( argv[0] ) );
    if( warnings ) {
        printf( "Total number of warning(s) = %d.\n", warnings );
        exit( -1 );
    }
    #ifdef __SW_BW
    {
        fprintf( stderr, "Tests completed (%s).\n", strlwr( argv[0] ) );
        fclose( my_stdout );
        _dwShutDown();
    }
    #endif
    exit( 0 );
}
