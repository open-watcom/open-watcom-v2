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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "bool.h"
#include "misc.h"
#include "argvrx.h"
#include "getopt.h"


typedef struct inpfile {
    struct inpfile *next;
    FILE *file;
} inpfile;

inpfile *inputFile;

char *OptEnvVar="paste";

static const char *usageMsg[] = {
    "Usage: paste [-?X] [files]",
    "\tfiles       : files to paste together. '-' may be specified for",
    "\t\t      standard input",
    "\tOptions: -? : print this list",
    "\t\t -X : match files by regular expressions",
    NULL
};

static void free_list( void )
{
    inpfile     *curr;

    while( inputFile != NULL ) {
        curr = inputFile;
        inputFile = inputFile->next;
        free( curr );
    }
}

int main( int argc, char **argv )
/*******************************/
{
    FILE        *fp;
    int         c;
    int         ch;
    unsigned    nfiles;
    bool        rxflag;
    inpfile     **curr;
    struct {
        int something_read : 1;
        int something_before : 1;
    } flags;

    rxflag = false;
    for( ;; ) {
        ch = GetOpt( &argc, argv, "X", usageMsg );
        if( ch == -1 ) {
            break;
        }
        if( ch == 'X' ) {
            rxflag = true;
        }
    }

    argv = ExpandArgv( &argc, argv, rxflag );

    if( argc == 1 ) {
        Quit( usageMsg, "No files specified\n" );
    }
    nfiles = 0;
    curr = &inputFile;
    for( ++argv; *argv != NULL; ++argv ) {
        if( strcmp( *argv, "-" ) == 0 ) {
            fp = stdin;
        } else {
            fp = fopen( *argv, "r" );
            if( fp == NULL ) {
                *curr = NULL;
                free_list();
                Die( "could not open: '%s'\n", *argv );
            }
        }
        *curr = malloc( sizeof( inpfile ) );
        (*curr)->file = fp;
        curr = &((*curr)->next);
        ++nfiles;
    }
    *curr = NULL;
    for(;;) {
        flags.something_before = 0;
        for( curr = &inputFile; *curr != NULL; curr = &((*curr)->next) ) {
            fp = (*curr)->file;
            if( fp == NULL )
                continue;
            flags.something_read = 0;
            for(;;) {
                c = fgetc( fp );
                if( c == EOF ) {
                    fclose( fp );
                    (*curr)->file = NULL;
                    --nfiles;
                    break;
                }
                flags.something_read = 1;
                if( c == '\n' )
                    break;
                if( flags.something_before ) {
                    /* full paste allows one to change this */
                    putchar( '\t' );
                    flags.something_before = 0;
                }
                putchar( c );
            }
            if( flags.something_read ) {
                flags.something_before = 1;
            }
        }
        if( nfiles == 0 )
            break;
        putchar( '\n' );
    }
    free_list();
    return(EXIT_SUCCESS);
}
