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
* Description:  T pipe fitting - send input to two or more outputs.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include "util.h"


static const char *usage_text[] = {
    "Usage: tee [-?ai] [file...]",
    "\tfile        : files to tee",
    "\t\t      Input is copied to each output `file' as well as to",
    "\t\t      the standard output.",
    "\tOptions:",
    "\t\t -? : print this list",
    "\t\t -a : append to (rather than overwrite) outputs",
    "\t\t -i : ignore the SIGINT signal",
    NULL
};


#define BUFFER_SIZE     16384
static char buffer[ BUFFER_SIZE ];

int main( int argc, char **argv )
/*******************************/
{
    int         ch;
    int         *out_fh;
    int         i;
    int         bytes_read;
    int         append_flag;
    int         sigign_flag;

    append_flag = sigign_flag = 0;
    while( (ch = getopt( argc, argv, ":ai" )) != -1 ) {
        switch( ch ) {
        case 'a':
            append_flag = 1;
            break;
        case 'i':
            sigign_flag = 1;
            break;
        case '?':
            util_quit( usage_text, NULL );
        }
    }
    /* Skip option arguments and argv[0] */
    argc = argc - optind;
    argv += optind;

    setmode( STDIN_FILENO, O_BINARY );
    setmode( STDOUT_FILENO, O_BINARY );
    if( argc > 0 ) {    /* It is allowed not to specify any output files */
        out_fh = malloc( sizeof( int ) * argc );
        if( out_fh == NULL ) {
            util_die( "tee: not enough memory for file handles\n" );
        }
    }
    if( sigign_flag ) {
        signal( SIGINT, SIG_IGN );
    }

    /* Open output files */
    for( i = 0; i < argc; ++i ) {
        out_fh[i] = open( argv[i], append_flag ?
                ( O_WRONLY | O_APPEND | O_CREAT | O_BINARY ) :
                ( O_WRONLY | O_TRUNC | O_CREAT | O_BINARY ),
                S_IREAD | S_IWRITE );
        if( out_fh[i] == -1 ) {
            util_die( "tee: unable to open %s for writing: %s\n", argv[i], strerror( errno ) );
        }
    }

    /* Do the tee-ing */
    for( ;; ) {
        bytes_read = read( STDIN_FILENO, buffer, BUFFER_SIZE );
        if( bytes_read < 0 ) {
            util_die( "tee: error reading (stdin): %s\n", strerror( errno ) );
        }
        if( bytes_read == 0 ) break;
        if( write( STDOUT_FILENO, buffer, bytes_read ) != bytes_read ) {
            util_die( "tee: error writing (stdout): %s\n", strerror( errno ) );
        }
        for( i = 0; i < argc; ++i ) {
            if( write( out_fh[i], buffer, bytes_read ) != bytes_read ) {
                util_die( "tee: error writing %s: %s\n", strerror( errno ) );
            }
        }
    }

    for( i = 0; i < argc; ++i ) {
        close( out_fh[ i ] );
    }
    free( out_fh );
    return( EXIT_SUCCESS );
}
