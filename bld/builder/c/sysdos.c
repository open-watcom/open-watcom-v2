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
* Description:  DOS specific functions for builder.
*
****************************************************************************/


#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <process.h>
#include <direct.h>
#include "builder.h"
#include "tinyio.h"
#include "memutils.h"


#define BUFSIZE 256

void SysInit( int argc, char *argv[] )
{
    argc = argc;
    argv = argv;
    setenv( "BLD_HOST", "DOS", 1 );
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
        if( ( dir[len - 1] == '\\' || dir[len - 1] == '/' ) && ( len > 3 || drive == 0 ) ) {
            len--;
            memcpy( tmp_buf, dir, len );
            tmp_buf[len] = '\0';
            dir = tmp_buf;
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

int SysRunCommand( const char *cmd )
{
    int         my_std_output;
    int         my_std_error;
    int         rc;
    char        *pgmname;
    char        *cmdline;
    tiny_ret_t  tinyrc;
    int         ofh;
    char        temp_name[BUFSIZE + 1 + 13];
    char        buff[BUFSIZE + 1];
    char        *p;

    pgmname = MStrdup( cmd );
    if( pgmname == NULL )
        return( -1 );
    cmdline = strchr( pgmname, ' ' );
    if( cmdline != NULL ) {
        *cmdline++ = '\0';
        while( *cmdline == ' ' ) {
            ++cmdline;
        }
    }
    rc = -1;
    getcwd( temp_name, BUFSIZE );
    p = temp_name + strlen( temp_name );
    if( p[-1] != '\\' )
        *p++ = '\\';
    memset( p, 0, 13 );
    tinyrc = TinyCreateTemp( temp_name, TIO_NORMAL );
    if( TINY_OK( tinyrc ) ) {
        ofh = TINY_INFO( tinyrc );
        my_std_output = TINY_INFO( TinyDup( STDOUT_FILENO ) );
        my_std_error = TINY_INFO( TinyDup( STDERR_FILENO ) );
        TinyDup2( ofh, STDOUT_FILENO);
        TinyDup2( ofh, STDERR_FILENO);
        /* no pipes for DOS so we call spawn with P_WAIT */
        rc = spawnlp( P_WAIT, pgmname, pgmname, cmdline, NULL );
        TinyDup2( my_std_output, STDOUT_FILENO );
        TinyDup2( my_std_error, STDERR_FILENO );
        TinyClose( my_std_output );
        TinyClose( my_std_error );
        TinyClose( ofh );
        tinyrc = TinyOpen( temp_name, TIO_READ );
        if( TINY_OK( tinyrc ) ) {
            ofh = TINY_INFO( tinyrc );
            for( ;; ) {
                tinyrc = TinyRead( ofh, buff, sizeof( buff ) - 1 );
                if( TINY_ERROR( tinyrc ) || TINY_INFO( tinyrc ) == 0 )
                    break;
                convert_buffer( buff, TINY_INFO( tinyrc ) );
                Log( Quiet, "%s", buff );
            }
            TinyClose( ofh );
        }
        TinyDelete( temp_name );
    }
    MFree( pgmname );
    return( rc );
}
