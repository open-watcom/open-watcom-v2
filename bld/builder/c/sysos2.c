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
* Description:  OS/2 specific functions for builder.
*
****************************************************************************/


#include <sys/types.h>
#include <direct.h>
#include <string.h>
#include <ctype.h>
#include <dos.h>
#include <io.h>
#include <process.h>
#include <fcntl.h>
#include "builder.h"

#define INCL_DOSQUEUES
#define INCL_DOSFILEMGR
#include <os2.h>

#define BUFSIZE 256

void SysInit( int argc, char *argv[] )
{
    argc = argc;
    argv = argv;
    setenv( "BLD_HOST", "OS2", 1 );
}

int SysRunCommandPipe( const char *cmd, int *readpipe )
{
    int         rc;
    HFILE       pipe_input;
    HFILE       pipe_output;
    HFILE       std_output;
    HFILE       std_error;
    char        *cmdnam = strdup( cmd );
    char        *sp = strchr( cmdnam, ' ' );

    if( sp != NULL ) {
        *sp = '\0';
        sp++;
    }

    std_output = 1;
    std_error  = 2;
    rc = DosCreatePipe( &pipe_input, &pipe_output, BUFSIZE );
    if( rc != 0 )
        return( rc );
    rc = DosDupHandle( pipe_output, &std_output );
    if( rc != 0 )
        return( rc );
    rc = DosDupHandle( pipe_output, &std_error );
    if( rc != 0 )
        return( rc );
    DosClose( pipe_output );
    rc = spawnl( P_NOWAIT, cmdnam, cmdnam, sp, NULL );
    DosClose( std_output );
    DosClose( std_error );
    *readpipe = _hdopen( ( int ) pipe_input, O_RDONLY );
    free( cmdnam );
    return rc;
}

int SysChdir( char *dir )
{
    return SysDosChdir( dir );
}
