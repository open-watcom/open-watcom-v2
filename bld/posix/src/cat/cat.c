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
* Description:  POSIX cat utility
*               Sends input to stdout
*
****************************************************************************/


#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "misc.h"
#include "getopt.h"
#include "argvrx.h"
#include "argvenv.h"

#define  BUFSIZE    16384

char *OptEnvVar = "cat";

static const char *usageMsg[] = {
    "Usage: cat [-?X] [@env] [files...]",
    "\tenv         : environment variable to expand",
    "\tfiles       : files to cat together",
    "\tOptions: -? : display this message",
    "\t\t -X : match files by regular expressions",
    NULL
};

void DoCAT( int fh ) {
    static char     buf[ BUFSIZE ];
    int             bytes_read;
    int             bytes_written;
    int             err;

    for( ; ; ) {
        bytes_read = read( fh, buf, BUFSIZE );
        if( bytes_read <= 0 ) break;
        bytes_written = write( STDOUT_FILENO, buf, bytes_read );
        if( bytes_written < bytes_read ) {
            err = errno;
            Die( "cat: error on output: %s\n", strerror( err ) );
        }
    }
}

void main( int argc, char **argv ) {
    int         rxflag, ch;
    int         fh;

    argv = ExpandEnv( &argc, argv );

    rxflag = 0;
    while( 1 ) {
        ch = GetOpt( &argc, argv, "X", usageMsg );
        if( ch == -1 ) {
            break;
        } else if( ch == 'X' ) {
            rxflag = 1;
        }
    }

    argv = ExpandArgv( &argc, argv, rxflag );
    argv++;
    setmode( STDOUT_FILENO, O_BINARY );
    if( argc == 1 ) {
        setmode( STDIN_FILENO, O_BINARY );
        DoCAT( STDIN_FILENO );
    } else {
        while( *argv != NULL ) {
            fh = open( *argv, O_RDONLY | O_BINARY );
            if( fh == -1 ) {
                fprintf( stderr, "cat: cannot open input file \"%s\"\n", *argv );
            } else {
                DoCAT( fh );
                close( fh );
            }
            argv++;
        }
    }
    exit( EXIT_SUCCESS );
}
