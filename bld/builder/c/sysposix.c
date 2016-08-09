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
* Description:  UNIX specific functions for builder.
*
****************************************************************************/


#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "watcom.h"
#include "builder.h"

#define BUFSIZE 256

void SysInit( int argc, char *argv[] )
{
    argc = argc;
    argv = argv;
    setenv( "BLD_HOST", "UNIX", 1 );
}

static int SysRunCommandPipe( const char *cmd, int *readpipe )
{
    int         pipe_fd[2];
    pid_t       pid;
    char        *cmdnam;
    char        *sp;
    const char  **argv;
    int         i;

    cmdnam = strdup( cmd );
    if( cmdnam == NULL )
        return( -1 );
    argv = malloc( strlen( cmd ) * sizeof( char * ) );
    if( argv == NULL ) {
        free( cmdnam );
        return( -1 );
    }
    i = 0;
    for( sp = cmdnam; sp != NULL; ) {
        while( *sp != '\0' && *sp == ' ' )
            ++sp;
        argv[i++] = sp;
        sp = strchr( sp, ' ' );
        if( sp != NULL ) {
            *sp = '\0';
            sp++;
        }
    }
    argv[i] = NULL;
    pid = -1;
    if( pipe( pipe_fd ) != -1 ) {
        *readpipe = pipe_fd[0];
        if( dup2( pipe_fd[1], STDOUT_FILENO ) != -1 ) {
            if( dup2( pipe_fd[1], STDERR_FILENO ) != -1 ) {
                close( pipe_fd[1] );
                pid = fork();
                if( pid == 0 ) {
#ifdef __WATCOMC__
                    execvp( cmdnam, argv );
#else
                    execvp( cmdnam, (char * const *)argv );
#endif
                    /* If above call to execvp() failed, do *not* call library termination routines! */
                    _exit( 127 );
                }
            }
        }
    }
    free( cmdnam );
    free( argv );
    if( pid == -1 )
        return( -1 );
    return( 0 );
}

int SysChdir( char *dir )
{
    char        *end;

    if( dir[0] == '\0' )
        return( 0 );
    end = &dir[strlen( dir ) - 1];
    if( *end == '/' && end > dir ) {
        *end = '\0';
    }
    return( chdir( dir ) );
}

int SysRunCommand( const char *cmd )
{
    int         my_std_output;
    int         my_std_error;
    size_t      bytes_read;
    int         rc;
    int         readpipe;
    char        buff[256 + 1];

    readpipe = -1;
    my_std_output = dup( STDOUT_FILENO );
    my_std_error = dup( STDERR_FILENO );
    rc = SysRunCommandPipe( cmd, &readpipe );
    dup2( my_std_output, STDOUT_FILENO );
    dup2( my_std_error, STDERR_FILENO );
    close( my_std_output );
    close( my_std_error );
    if( rc == -1 ) {
        if( readpipe != -1 )
            close( readpipe );
        return( rc );
    }
    if( readpipe != -1 ) {
        while( (bytes_read = read( readpipe, buff, sizeof( buff ) - 1 )) != 0 ) {
            if( (ssize_t)bytes_read == -1 )
                break;
            buff[bytes_read] = '\0';
            Log( Quiet, "%s", buff );
        }
        close( readpipe );
    }
    /* free up the zombie (if there is one) */
    while( wait( &rc ) == -1 && errno == EINTR )
        ;
    return( rc );
}
