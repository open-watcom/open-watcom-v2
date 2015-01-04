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

int SysRunCommandPipe( const char *cmd, int *readpipe )
{
    int         pipe_fd[2];
    pid_t       pid;
    char        *cmdnam = strdup( cmd );
    char        *sp = cmdnam;
    const char  **argv = malloc( strlen( cmd )* sizeof( char * ) );
    int         i = 0;

    while( sp != NULL ) {
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
    if( pipe( pipe_fd ) == -1 )
        return( errno );
    if( dup2( pipe_fd[1], STDOUT_FILENO ) == -1 )
        return( errno );
    if( dup2( pipe_fd[1], STDERR_FILENO ) == -1 )
        return( errno );
    close( pipe_fd[1] );
    pid = fork();
    if( pid == -1 )
        return( -1 );
    if( pid == 0 ) {
#ifdef __WATCOMC__
        execvp( cmdnam, argv );
#else
        execvp( cmdnam, (char * const *)argv );
#endif
        /* If above call to execvp() failed, do *not* call library termination routines! */
        _exit( 127 );
    }
    free( cmdnam );
    free( argv );
    *readpipe = pipe_fd[0];
    return 0;
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
