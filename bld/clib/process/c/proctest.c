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
* Description:  Non-exhaustive test of the C library process functions.
*
****************************************************************************/


#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>
#include <signal.h>


char ProgramName[512];                          /* executable filename */
int NumErrors = 0;                              /* number of errors */


#ifdef __SW_BW

/* Dummy implementation for Win16 which does not support spawn functions */

#include <wdefwin.h>

int main( int argc, char **argv )
{
    FILE                *my_stdout;

    strcpy( ProgramName, strlwr( argv[0] ) );   /* store filename */
    my_stdout = freopen( "tmp.log", "a", stdout );
    if( my_stdout == NULL ) {
        fprintf( stderr, "Unable to redirect stdout\n" );
        exit( EXIT_FAILURE );
    }

    printf( "Tests completed (%s).\n", ProgramName );
    fprintf( stderr, "Tests completed (%s).\n", ProgramName );
    fclose( my_stdout );
    _dwShutDown();
    return( EXIT_SUCCESS );
}

#else

#define VERIFY( exp )   if( !(exp) ) {                                      \
                            printf( "%s: ***FAILURE*** at line %d of %s.\n",\
                                    ProgramName, __LINE__, myfile );        \
                            NumErrors++;                                    \
                            exit( -1 );                                     \
                        }


/*
 * This program has two modes of operation, parent and child. Child mode
 * kicks in if any argument was passed. During testing, the first instance
 * (ie. parent) will spawn itself in test mode. The child instance will run
 * a few checks and return error code indicating pass/fail. The parent then
 * prints failure or success message depending on the child's return code.
 * This method should be portable across all supported target platforms.
 */

/* Define data to be passed between parent and child processes */

#define ARG1            "Hello,"
#define ARG2            "my"
#define ARG3            "child!"

#define ARG_REDIR       "Redir"
#define REDIR_TEXT      "This text should not be written to stdout"

#define VAR_NAME        "CHILD_ENV_VAR"
#define VAR_TEXT        "Test v@ri@ble"

#define CHILD_RC        0x55

/* Define data needed to test signal handling */
sig_atomic_t signal_count;
sig_atomic_t signal_number;


void break_handler( int signo )
{
    signal_count++;
    signal_number = signo;
}


/****
***** Program entry point.
****/

int main( int argc, char * const argv[] )
{
    char                myfile[ sizeof __FILE__ ];
    int                 child = argc > 1;
    int                 handle;
    int                 status;
    int                 handle_out;
    long                size;


    /*** Initialize ***/
    strcpy( ProgramName, argv[0] );     /* store filename */
    strlwr( ProgramName );              /* and lower case it */
    strcpy( myfile, __FILE__ );
    strlwr( myfile );

    if( child ) {
        char    *env_var;

        if( argc == 4 ) {        
        /* Verify expected command line contents */
            VERIFY( !strcmp( argv[1], ARG1 ) );
            VERIFY( !strcmp( argv[2], ARG2 ) );
            VERIFY( !strcmp( argv[3], ARG3 ) );

        /* Verify expected environment contents */
            env_var = getenv( VAR_NAME );
            VERIFY( env_var );
            VERIFY( !strcmp( env_var, VAR_TEXT ) );

            if( NumErrors != 0 ) {
                return( EXIT_FAILURE );
            } else {
                return( CHILD_RC );
            }
        } else {
            if( argc == 2 ) {
            /* Verify expected command line contents */
                VERIFY( !strcmp( argv[1], ARG_REDIR ) );

            /* Write text to stdout */
                printf( REDIR_TEXT );

                if( NumErrors != 0 ) {
                    return( EXIT_FAILURE );
                } else {
                    return( CHILD_RC );
                }
            }
            else
                return( EXIT_FAILURE );
        }                   
    } else {
        int         rc;
        char        **env;
        const char  *path = "PATH=.";   /* Default PATH if none is found */
        const char  *child_args[] = { ProgramName, ARG1, ARG2, ARG3, NULL };
        const char  *child_envp[] = { NULL, VAR_NAME "=" VAR_TEXT, NULL };

        /* We need to pass PATH down to the child because DOS/4GW style stub
         * programs rely on it to function properly.
         */
        for( env = environ; *env; ++env )
            if( !strncmp( *env, "PATH=", 5 ) ) {
                path = *env;
                break;
            }

        child_envp[0] = path;

        /* Test spawn functions */
        rc = spawnle( P_WAIT, ProgramName, ProgramName, ARG1, ARG2, ARG3, NULL, child_envp );
        VERIFY( rc == CHILD_RC );

        rc = spawnlpe( P_WAIT, ProgramName, ProgramName, ARG1, ARG2, ARG3, NULL, child_envp );
        VERIFY( rc == CHILD_RC );

        rc = spawnve( P_WAIT, ProgramName, child_args, child_envp );
        VERIFY( rc == CHILD_RC );

        rc = spawnvpe( P_WAIT, ProgramName, child_args, child_envp );
        VERIFY( rc == CHILD_RC );

        /* Modify our environment that child will inherit */
        VERIFY( !setenv( VAR_NAME, VAR_TEXT, 1 ) );
        
        rc = spawnl( P_WAIT, ProgramName, ProgramName, ARG1, ARG2, ARG3, NULL );
        VERIFY( rc == CHILD_RC );

        rc = spawnlp( P_WAIT, ProgramName, ProgramName, ARG1, ARG2, ARG3, NULL );
        VERIFY( rc == CHILD_RC );

        rc = spawnv( P_WAIT, ProgramName, child_args );
        VERIFY( rc == CHILD_RC );

        rc = spawnvp( P_WAIT, ProgramName, child_args );
        VERIFY( rc == CHILD_RC );

#ifdef __NT__

        rc = spawnv( P_DETACH, ProgramName, child_args );
        VERIFY( rc == 0 );

#endif

#if defined(__NT__) || defined(__OS2__)

        rc = spawnv( P_NOWAIT, ProgramName, child_args );
        rc = cwait( &status, rc, WAIT_CHILD );
        VERIFY( (status & 0xff) == 0 );
        VERIFY( (status >> 8) == CHILD_RC );

#endif

        /* Check inherited output redirection */
        handle_out = dup( STDOUT_FILENO );
        
        handle = creat( "test.fil", S_IREAD|S_IWRITE );
        VERIFY( handle != -1 );

        status = dup2( handle, STDOUT_FILENO );
        VERIFY( status != -1 );

        status = close( handle );
        VERIFY( status == 0 );

        rc = spawnl( P_WAIT, ProgramName, ProgramName, ARG_REDIR, NULL );
        VERIFY( rc == CHILD_RC );

        status = dup2( handle_out, STDOUT_FILENO );
        VERIFY( status != -1 );
        
        handle = open( "test.fil", O_RDWR );
        VERIFY( handle != -1 );

        size = filelength( handle );
        VERIFY( size == strlen( REDIR_TEXT ) );

        status = close( handle );
        VERIFY( status == 0 );

        status = unlink( "test.fil" );
        VERIFY( status == 0 );

        signal_count = 0;
        signal_number = 0;
        /* Install SIGBREAK handler */
        VERIFY( signal( SIGBREAK, break_handler ) == SIG_DFL );

        /* Raise signal and verify results */
        VERIFY( raise( SIGBREAK ) == 0 );
        VERIFY( signal_count == 1 );
        VERIFY( signal_number == SIGBREAK );

        /* Raise again - nothing should have happened */
        VERIFY( raise( SIGBREAK ) == 0 );
        VERIFY( signal_count == 1 );

        /*** Print a pass/fail message and quit ***/
        if( NumErrors != 0 ) {
            printf( "%s: FAILURE (%d errors).\n", ProgramName, NumErrors );
            return( EXIT_FAILURE );
        }
        printf( "Tests completed (%s).\n", ProgramName );
    }

    return( 0 );
}

#endif
