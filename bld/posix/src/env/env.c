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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <process.h>
#include <ctype.h>
#include <errno.h>

#include "misc.h"
#include "getopt.h"
#include "argvrx.h"
#include "argvenv.h"

char *OptEnvVar="env";

static char *usageMsg[] = {
    "Usage: env [-] [-?] [@var] [variable=value ...] [command arguments ...]",
    "\tvar                : environment variable to expand",
    "\tvariable=value     : set environment variable to value",
    "\tcommand arguments  : call command with arguments and new environment",
    "\tOptions: -?        : display this message",
    "\t\t -         : do not use environment inherited by env",
    NULL
};

static void insertVar( char **env, char *var )
{
    char        *p;
    char         ch;
    unsigned     len;
    char         delete = 0;

    p = strchr( var, '=' ) + 1;

    if( *p != '\0' ) {
        ch = *p;
        *p = '\0';
    } else {
        ch = '\0';
        delete = 1;
    }

    len = strlen( var );

    for( ; *env != NULL; env++ ) {
        if( strnicmp( *env, var, len ) == 0 ) {
            if( !delete ) {
                *p = ch;
                *env = var;
                return;
            } else {
                while( *env != NULL ) {
                    *env = *(env + 1);
                    env++;
                }
                return;
            }
        }
    }
    *p = ch;
    *env = var;
    *(env + 1) = NULL;
}

static int isVarAssgn( char *var )
{
    char        *p;

    p = strchr( var, '=' );

    if( p == NULL  ||  p == var ) {
        return( 0 );
    }

    for( ; var < p; var++ ) {
        if( !isalnum( *var )  &&  *var != '_' ) {
            return( 0 );
        }
    }

    return( 1 );
}

static void printEnv( char **env )
{
    while( *env != NULL ) {
        fprintf( stdout, "%s\n", *env );
        env++;
    }
}

void main( int argc, char **argv )
{
    char        **envptr, **cmdptr;
    char          alloc;

    argv = ExpandEnv( &argc, argv );
    GetOpt( &argc, argv, "", usageMsg );

    if( argc == 1 ) {
        printEnv( environ );
        return;
    } else if( !strcmp( argv[1], "-" ) ) {
        envptr  = (char **) malloc( argc * sizeof( char * ) );
        *envptr = NULL;
        alloc   = 1;

        argv  += 2;

        while( *argv != NULL ) {
            if( isVarAssgn( *argv ) ) {
                insertVar( envptr, *argv );
                argv++;
            } else {
                break;
            }
        }
        cmdptr = argv;
    } else {
        alloc = 0;
        argv++;

        while( *argv != NULL ) {
            if( isVarAssgn( *argv ) ) {
                if( putenv( *argv ) ) {
                    Die( "env: out of environment space\n" );
                }
                argv++;
            } else {
                break;
            }
        }
        envptr = environ;
        cmdptr = argv;
    }

    if( *cmdptr == NULL ) {
        printEnv( envptr );
    } else {
        errno = 0;
        spawnvpe( P_WAIT, *cmdptr, cmdptr, envptr );

        switch( errno ) {
            case E2BIG:
                Die( "env: command argument list too big\n" );
            case ENOENT:
                Die( "env: cannot find command\n" );
            case ENOMEM:
                Die( "env: insufficient memory to execute command\n" );
            default:
                break;
        }
    }
    if( alloc ) {
        free( envptr );
    }
}
