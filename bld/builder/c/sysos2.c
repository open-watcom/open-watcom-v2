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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <sys/types.h>
#include <direct.h>
#include <string.h>
#include <ctype.h>
#include <dos.h>
#include <process.h>
#include "builder.h"

#define INCL_DOSQUEUES
#define INCL_DOSFILEMGR
#include <os2.h>

#define BUFSIZE 32768

char    *CmdProc;
extern  bool Quiet;
char    buff[BUFSIZE+1];

void SysInit( int argc, char *argv[] )
{
    argc = argc;
    argv = argv;
    CmdProc = getenv( "COMSPEC" );
    if( CmdProc == NULL ) {
        Fatal( "Can not find command processor" );
    }
}

unsigned SysRunCommand( const char *cmd )
{
    int         rc;
    HFILE       pipe_input;
    HFILE       pipe_output;
    HFILE       my_std_output;
    HFILE       my_std_error;
    HFILE       std_output;
    HFILE       std_error;
    ULONG       bytes_read;

        std_output = 1;
        std_error = 2;
        my_std_output = -1;
        my_std_error = -1;
        rc = DosDupHandle( std_output, &my_std_output );
        rc = DosDupHandle( std_error, &my_std_error );
        rc = DosCreatePipe( &pipe_input, &pipe_output, BUFSIZE );
        if( rc != 0 ) return( rc );
        rc = DosDupHandle( pipe_output, &std_output );
        if( rc != 0 ) return( rc );
        rc = DosDupHandle( pipe_output, &std_error );
        if( rc != 0 ) return( rc );
        DosClose( pipe_output );
        rc = spawnl( P_NOWAITO, CmdProc, CmdProc, "/c", cmd, NULL );
        if( rc == -1 ) return( -1 );
        DosClose( std_output );
        DosClose( std_error );
        DosDupHandle( my_std_output, &std_output );
        DosDupHandle( my_std_error, &std_error );
        for (;;) {
                DosRead( pipe_input, buff, BUFSIZE-1, &bytes_read );
                if( bytes_read == 0 )
                        break;
                buff[bytes_read] = '\0';
                Log( Quiet, "%s", buff );
        }
        DosClose( pipe_input );
        DosClose( my_std_output );
        DosClose( my_std_error );
    return( 0 );
}

unsigned SysChDir( const char *dir )
{
    char        *end;
    unsigned    len;
    unsigned    total;

    if( dir[0] == '\0' ) return( 0 );
    len = strlen( dir );
    end = &dir[len-1];
    switch( *end ) {
    case '\\':
    case '/':
        if( end > dir && end[-1] != ':' ) {
            *end = '\0';
            --len;
        }
        break;
    }
    if( len > 2 && dir[1] == ':' ) {
        _dos_setdrive( toupper( dir[0] ) - 'A' + 1, &total );
    }
    return( chdir( dir ) );
}
