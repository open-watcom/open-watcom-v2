/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  POSIX unexpand utility
*               Converts spaces to tabs
*
****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "bool.h"
#include "misc.h"
#include "getopt.h"
#include "argvrx.h"
#include "argvenv.h"


char *OptEnvVar="unexpand";

static const char *usageMsg[] = {
    "Usage: unexpand [-?Xa] [@env] [files...]",
    "\tenv                : environment variable to expand",
    "\tfiles              : files to compress",
    "\tOptions: -?        : display this message",
    "\t\t -a        : compress spaces to tabs wherever result is shorter",
    "\t\t -X        : match files by regular expressions",
    NULL
};

typedef enum mode { LEADING, ALWAYS } mode;     // when to compress

/*
 * Local functions.
 */

static void unexpandFile( FILE *fp, mode m )
{
    int         ch;
    unsigned    col = 0, spc = 0, i;
    bool        notab;
    bool        flag;

    notab = false;
    flag = false;
    for( ;; ) {
        ch = fgetc( fp );

        if( (char) ch == ' '  ||  (char) ch == '\t' ) {
            if( !notab ) {
                if( !flag ) {
                    spc = col;
                    flag = true;
                }
            } else {
                fputc( ch, stdout );
            }
            if( (char) ch == ' ' ) {
                col++;
            } else {
                col += 8 - col % 8;
            }
        } else {
            if( m == LEADING ) {
                notab = true;
            }
            if( flag ) {
                if( spc / 8 == col / 8 ) {
                    for( ; spc < col; spc++ ) {
                        fputc( ' ', stdout );
                    }
                } else {
                    for( i = 0; i < col/8 - spc/8; i++ ) {
                        fputc( '\t', stdout );
                    }
                    for( i = 0; i < col % 8; i++ ) {
                        fputc( ' ', stdout );
                    }
                }
                flag = false;
            }

            if( ch == EOF ) {
                break;
            } else if( (char) ch == '\n' ) {
                col = 0;
                notab = false;
            } else {
                col++;
            }
            fputc( ch, stdout );
        }
    }
}

int main( int argc, char **argv )
{
    FILE       *fp;
    int         ch;
    bool        regexp;
    mode        m = LEADING;

    argv = ExpandEnv( &argc, argv );

    regexp = false;
    for( ;; ) {
        ch = GetOpt( &argc, argv, "Xa", usageMsg );
        if( ch == -1 ) {
            break;
        } else if( ch == 'a' ) {
            m = ALWAYS;
        } else if( ch == 'X' ) {
            regexp = true;
        }
    }

    argv = ExpandArgv( &argc, argv, regexp );
    argv++;
    if( argv[0] == NULL ) {
        unexpandFile( stdin, m );
    } else {
        while( *argv != NULL ) {
            fp = fopen( *argv, "r" );
            if( fp == NULL ) {
                fprintf( stderr, "unexpand: cannot open input file \"%s\"\n",
                                                                        *argv );
            } else {
                if( argc > 2 ) {
                    fprintf( stdout, "%s:\n", *argv );
                }
                unexpandFile( fp, m );
                fclose( fp );
            }
            argv++;
        }
    }
    return( 0 );
}
