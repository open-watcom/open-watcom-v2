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
* Description:  Concatenate input files to standard output.
*
****************************************************************************/


#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "util.h"


#define  BUFSIZE    16384

static const char *usage_text[] = {
    "Usage: cat [-?u] [files...]",
    "\tfiles       : files to cat together",
    "\tOptions:",
    "\t\t -? : print this list",
    "\t\t -u : write data from input to output without delay",
    NULL
};


static void do_cat( int fh )
/**************************/
{
    static char     buf[BUFSIZE];
    int             bytes_read;
    int             bytes_written;
    int             err;

    for( ;; ) {
        bytes_read = read( fh, buf, BUFSIZE );
        if( bytes_read <= 0 ) break;
        bytes_written = write( STDOUT_FILENO, buf, bytes_read );
        if( bytes_written < bytes_read ) {
            err = errno;
            util_die( "cat: error on output: %s\n", strerror( err ) );
        }
    }
}


int main( int argc, char **argv )
/*******************************/
{
    int         ch;
    int         unbuff_flag = 0;
    int         fh;

    setvbuf( stdin, NULL, _IONBF, 0 );

    while( (ch = getopt( argc, argv, ":u" )) != -1 ) {
        switch( ch ) {
        case 'u':
            unbuff_flag = 1;    /* currently unused */
            break;
        case '?':
            util_quit( usage_text, NULL );
        }
    }
    /* Skip option arguments and argv[0] */
    argc = argc - optind;
    argv += optind;

    setmode( STDOUT_FILENO, O_BINARY );
    if( argc == 0 ) {
        setmode( STDIN_FILENO, O_BINARY );
        do_cat( STDIN_FILENO );
    } else {
        while( *argv != NULL ) {
            fh = open( *argv, O_RDONLY | O_BINARY );
            if( fh == -1 ) {
                fprintf( stderr, "cat: error opening '%s' for read: %s\n",
                         *argv, strerror( errno ) );
                return( EXIT_FAILURE );
            } else {
                do_cat( fh );
                close( fh );
            }
            argv++;
        }
    }
    return( EXIT_SUCCESS );
}
