/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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


#include <direct.h>
#include <string.h>
#include <ctype.h>
#include <io.h>
#include <process.h>
#include "builder.h"
#include "memutils.h"

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

int SysChdir( const char *dir )
{
    size_t      len;
    int         drive;

    if( dir[0] == '\0' )
        return( 0 );
    drive = ( dir[1] == ':' ) ? toupper( (unsigned char)dir[0] ) - 'A' + 1 : 0;
    if( dir[1] != '\0' ) {
        len = strlen( dir );
        if( dir[len - 1] == '\\' || dir[len - 1] == '/' ) {
            if( len > 3 || drive == 0 ) {
                len--;
                memcpy( tmp_buf, dir, len );
                tmp_buf[len] = '\0';
                dir = tmp_buf;
            }
        }
    }
    if( drive ) {
        _chdrive( drive );
    }
    return( chdir( dir ) );
}

static void convert_buffer( char *src, size_t len )
{
    char    *dst;
    char    c;

    dst = src;
    while( len-- > 0 ) {
        if( (c = *src++) != '\r' ) {
            *dst++ = c;
        }
    }
    *dst = '\0';
}

static int SysRunCommandPipe( const char *cmd, HFILE *readpipe )
{
    HFILE       pipe_output;
    HFILE       std_output;
    HFILE       std_error;
    char        *cmdnam;
    char        *sp;
    int         rc;

    cmdnam = MStrdup( cmd );
    if( cmdnam == NULL )
        return( -1 );
    sp = strchr( cmdnam, ' ' );
    if( sp != NULL ) {
        *sp = '\0';
        sp++;
    }
    rc = -1;
    std_output = 1;
    std_error  = 2;
    if( DosCreatePipe( readpipe, &pipe_output, BUFSIZE ) == 0 ) {
        if( DosDupHandle( pipe_output, &std_output ) == 0 ) {
            if( DosDupHandle( pipe_output, &std_error ) == 0 ) {
                DosClose( pipe_output );
                rc = spawnl( P_NOWAIT, cmdnam, cmdnam, sp, NULL );
                DosClose( std_output );
                DosClose( std_error );
            }
        }
    }
    MFree( cmdnam );
    return( rc );
}

int SysRunCommand( const char *cmd )
{
    int         my_std_output;
    int         my_std_error;
    ULONG       bytes_read;
    int         rc;
    HFILE       readpipe;
    char        buff[BUFSIZE + 1];

    readpipe = 0;
    my_std_output = dup( STDOUT_FILENO );
    my_std_error = dup( STDERR_FILENO );
    rc = SysRunCommandPipe( cmd, &readpipe );
    dup2( my_std_output, STDOUT_FILENO );
    dup2( my_std_error, STDERR_FILENO );
    close( my_std_output );
    close( my_std_error );
    if( rc == -1 ) {
        if( readpipe != 0 )
            DosClose( readpipe );
        return( rc );
    }
    if( readpipe != 0 ) {
        for( ;; ) {
            if( DosRead( readpipe, buff, sizeof( buff ) - 1, &bytes_read ) || bytes_read == 0 )
                break;
            convert_buffer( buff, bytes_read );
            Log( Quiet, "%s", buff );
        }
        DosClose( readpipe );
    }
    /* free up the zombie (if there is one) */
    while( wait( &rc ) == -1 && errno == EINTR )
        ;
    return( rc );
}
