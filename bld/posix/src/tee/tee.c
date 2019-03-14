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
* Description:  POSIX tee utility
*               Sends input to two outputs
*
****************************************************************************/


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#if defined( __NT__ )
#include <windows.h>
#endif
#include "wio.h"
#include "getopt.h"
#include "misc.h"

char *OptEnvVar = "tee";

static const char *usageTxt[] = {
    "Usage: tee [-?a] [files]",
    "\tfiles       : files to tee",
    "\t\t      Input is copied to each output `file' as well as to",
    "\t\t      the standard output.",
    "\tOptions: -? : print this list",
    "\t\t -a : append, rather than overwrite, outputs",
NULL
};


#define BUFFER_SIZE     16384
char buffer[ BUFFER_SIZE ];

int main( int argc, char *argv[] )
{
    int         ch;
    int         *out_fh;
    int         i;
    int         bytes_read;
    int         append_flag;
    int         ret;

    append_flag = 0;
    for(;;) {
        ch = GetOpt( &argc, argv, "a", usageTxt );
        if( ch == -1 )
            break;
        if( ch == 'a' ) {
            append_flag = 1;
        }
    }
    if( argc < 2 ) {
        Quit( usageTxt, "must specify at least 1 output file\n" );
    }
    --argc;             /* ignore argv[0] */
    ++argv;
    setmode( STDIN_FILENO, O_BINARY );
    setmode( STDOUT_FILENO, O_BINARY );
    out_fh = malloc( sizeof( int ) * argc );
    if( out_fh == NULL ) {
        Die( "not enough memory for file handles\n" );
    }
    ret = EXIT_SUCCESS;
    /* open output files */
    for( i = 0; i < argc; ++i ) {
        out_fh[i] = open( argv[i], append_flag ?
                ( O_WRONLY | O_APPEND | O_CREAT | O_BINARY ) :
                ( O_WRONLY | O_TRUNC | O_CREAT | O_BINARY ),
                S_IREAD | S_IWRITE );
        if( out_fh[i] == -1 ) {
            ret = EXIT_FAILURE;
            Error( "unable to open %s for writing: %s\n", argv[i], strerror( errno ) );
            break;
        }
    }

    /* do the tee-ing */
    for( ; ret == EXIT_SUCCESS; ) {
        bytes_read = read( STDIN_FILENO, buffer, BUFFER_SIZE );
        if( bytes_read < 0 ) {
            ret = EXIT_FAILURE;
#ifdef __NT__
            if( GetLastError() == 109 ) {   // BROKEN_PIPE
                break;
            }
#endif
            Error( "error reading (stdin): %s\n", strerror( errno ) );
            break;
        }
        if( bytes_read == 0 )
            break;
        if( write( STDOUT_FILENO, buffer, bytes_read ) != bytes_read ) {
            ret = EXIT_FAILURE;
            Error( "error writing (stdout): %s\n", strerror( errno ) );
            break;
        }
        for( i = 0; i < argc; ++i ) {
            if( write( out_fh[i], buffer, bytes_read ) != bytes_read ) {
                ret = EXIT_FAILURE;
                Error( "error writing %s: %s\n", strerror( errno ) );
                break;
            }
        }
    }

    for( i = 0; i < argc; ++i ) {
        if( out_fh[i] == -1 )
            break;
        close( out_fh[i] );
    }
    free( out_fh );
    return( ret );
}
