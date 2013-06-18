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
* Description:  POSIX trunc utility
*               Truncates files
*
****************************************************************************/


#include <errno.h>
#include <stdlib.h>
#include <string.h>
#if defined(__QNX__)
 #include <utime.h>
#else
 #include <sys/utime.h>
#endif
#include "wio.h"
#include "argvenv.h"
#include "getopt.h"
#include "argvrx.h"
#include "misc.h"

char *OptEnvVar = "trunc";

static const char *usageMsg[] = {
    "Usage: trunc [-?qX] [@env] [files]",
    "\tenv         : environment variable to expand",
    "\tfiles       : files to truncate",
    "\tOptions: -? : display this message",
    "\t\t -q : don't complain about non-existent files",
    "\t\t -X : match files by regular expressions",
    NULL
};

void errstr( char *str ) {

    size_t      len;

    len = strlen( str );
    write( 2, str, len );
}


void main( int argc, char **argv ) {

    int             i,rxflag,quietflag,ch;
    int             fh;
    struct stat     statbuf;
    struct utimbuf  utb;
    unsigned short  st_mode;

    argv = ExpandEnv( &argc, argv );
    rxflag = 0;
    quietflag = 0;
    while( 1 ) {
        ch = GetOpt( &argc, argv, "qX", usageMsg );
        if( ch == -1 ) {
            break;
        }
        switch( ch ) {
        case 'X':
            rxflag = 1;
            break;
        case 'q':
            quietflag = 1;
            break;
        }
    }

    argv = ExpandArgv( &argc, argv, rxflag );

    if( argc == 1 ) {
        Quit( usageMsg, NULL );
    } else {
        for( i = 1; i < argc; ++i ) {
            if( stat( argv[ i ], &statbuf ) != 0 ) {
                if( quietflag ) continue;
                errstr( "error accessing " );
                errstr( argv[ i ] );
                errstr( " for time stamp: " );
                errstr( strerror( errno ) );
                errstr( "\r\n" );
                exit( 1 );
            }
            utb.actime = statbuf.st_atime;
            utb.modtime = statbuf.st_mtime;
            st_mode = statbuf.st_mode;
            fh = open( argv[ i ], O_RDWR | O_TRUNC );
            if( fh == -1 ) {
                errstr( "error opening " );
                errstr( argv[ i ] );
                errstr( " for truncation: " );
                errstr( strerror( errno ) );
                errstr( "\r\n" );
                exit( 1 );
            }
            close( fh );
            if( stat( argv[ i ], &statbuf ) != 0 ) {
                errstr( "error accessing " );
                errstr( argv[ i ] );
                errstr( " for file attribute verification: " );
                errstr( strerror( errno ) );
                errstr( "\r\n" );
                exit( 1 );
            }
            if( st_mode != statbuf.st_mode ) {
                errstr( "warning: attribute fixup for " );
                errstr( argv[ i ] );
                errstr( "\r\n" );
                if( chmod( argv[ i ], st_mode ) != 0 ) {
                    errstr( "error on attribute fixup for " );
                    errstr( argv[ i ] );
                    errstr( " : " );
                    errstr( strerror( errno ) );
                    errstr( "\r\n" );
                    exit( 1 );
                }
            }
            if( utime( argv[ i ], &utb ) != 0 ) {
                errstr( "error modifying " );
                errstr( argv[ i ] );
                errstr( " for time stamp: " );
                errstr( strerror( errno ) );
                errstr( "\r\n" );
                exit( 1 );
            }
        }
    }
    exit( 0 );
}
