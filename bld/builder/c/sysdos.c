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
* Description:  DOS specific functions for builder.
*
****************************************************************************/


#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dos.h>
#include <process.h>
#include <direct.h>
#include "builder.h"
#include "tinyio.h"


void SysInit( int argc, char *argv[] )
{
    argc = argc;
    argv = argv;
    setenv( "BLD_HOST", "DOS", 1 );
}

int SysChdir( char *dir )
{
    return SysDosChdir( dir );
}

int SysRunCommand( const char *cmd )
{
    int         my_std_output;
    int         my_std_error;
    int         rc;
    char        *cmdnam;
    char        *sp;
    tiny_ret_t  tinyrc;
    int         ofh;
    char        temp_name[256];
    char        buff[256 + 1];
    unsigned    bytes_read;

    cmdnam = strdup( cmd );
    if( cmdnam == NULL )
        return( -1 );
    sp = strchr( cmdnam, ' ' );
    if( sp != NULL ) {
        *sp++ = '\0';
    }
    rc = -1;
    getcwd( temp_name, 256 );
    memset( temp_name + strlen( temp_name ), 0, 13 );
    tinyrc = TinyCreateTemp( temp_name, TIO_NORMAL );
    if( TINY_OK( tinyrc ) ) {
        ofh = TINY_INFO( tinyrc );
        my_std_output = TINY_INFO( TinyDup( STDOUT_FILENO ) );
        my_std_error = TINY_INFO( TinyDup( STDERR_FILENO ) );
        TinyDup2( ofh, STDOUT_FILENO);
        TinyDup2( ofh, STDERR_FILENO);
        /* no pipes for DOS so we call spawn with P_WAIT */
        rc = spawnlp( P_WAIT, cmdnam, cmdnam, sp, NULL );
        TinyDup2( my_std_output, STDOUT_FILENO );
        TinyDup2( my_std_error, STDERR_FILENO );
        TinyClose( my_std_output );
        TinyClose( my_std_error );
        TinyClose( ofh );
        tinyrc = TinyOpen( temp_name, TIO_READ );
        if( TINY_OK( tinyrc ) ) {
            ofh = TINY_INFO( tinyrc );
            tinyrc = TinyRead( ofh, buff, 256 );
            while( TINY_OK( tinyrc ) && (bytes_read = TINY_INFO( tinyrc )) != 0 ) {
                unsigned    i;
                char        *dst;

                dst = buff;
                for( i = 0; i < bytes_read; ++i ) {
                    if( buff[i] != '\r' ) {
                        *dst++ = buff[i];
                    }
                }
                *dst = '\0';
                Log( Quiet, "%s", buff );
                tinyrc = TinyRead( ofh, buff, 256 );
            }
            TinyClose( ofh );
        }
        TinyDelete( temp_name );
    }
    free( cmdnam );
    return( rc );
}
