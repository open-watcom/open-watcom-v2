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
* Description:  GetOpt() - function to process traditional command line options
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include "getopt.h"
#include "misc.h"

char            *OptArg;
int             OptInd = 1;
char            OptChar;
#ifndef __ISVI__
char            AltOptChar = '/';
#else
char            AltOptChar = '+';
#endif

static int      optOff = 0;
static int      testedOptEnvVar;
#ifndef __ISVI__
static int      testedPosixrx;
#endif
static char     *envVar;

static void eatArg( int *argc, char *argv[], int num )
{
    int i;

    optOff = 0;
    if( envVar != NULL ) {
        envVar = NULL;
        return;
    }

    for( i= OptInd + num; i <= *argc; i++ ) {
        argv[i - num] = argv[i];
    }
    ( *argc ) -= num;

} /* eatArg */


int GetOpt( int *argc, char *argv[], char *optstr, const char *usage[] )
{
    char        ch;
    char        *ptr;
    char        *currarg;

    OptArg = NULL;
#ifndef __ISVI__
    if( !testedPosixrx ) {
        testedPosixrx = 1;
        if( getenv( "POSIXRX" ) != NULL ) {
            if( strchr( optstr, 'X' ) != NULL ) {
                return( 'X' );
            }
        }
    }
#endif
    if( !testedOptEnvVar ) {
        testedOptEnvVar = 1;
        envVar = getenv( OptEnvVar );
    }

    while( 1 ) {
        if( envVar != NULL ) {
            currarg = envVar;
        } else {
            currarg = argv[OptInd];
            if( currarg == NULL ) {
                return( -1 );
            }
        }
        while( 1 ) {
            ch = currarg[optOff];
            if( isspace( ch ) && envVar != NULL ) {
                optOff++;
                continue;
            }
            break;
        }
        if( optOff > 1 || ch == '-' || ch == AltOptChar ) {
            if( optOff > 1 ) {
                ch = currarg[optOff];
                if( ch == '-' || ch == AltOptChar ) {
                    OptChar = ch;
                    optOff++;
                    ch = currarg[optOff];
                }
            } 
            else {
                OptChar = ch;
                optOff++;
                ch = currarg[optOff];
            }
            if( ch == '\0' ) { // option char by itself should be
                return( -1 );           // left alone
            }
            if( ch == '-' && currarg[optOff+1] == '\0' ) {
                eatArg( argc, argv, 1 );
                return( -1 );  // "--" PoSIX end of options delimiter.
            }
            if( ch == '?' ) {
                Quit( usage, NULL );
            }
            if( optstr[0] == '#' && isdigit( ch ) ) {
                OptArg = &currarg[optOff];
                eatArg( argc, argv, 1 );
                return( '#' );
            }
            ptr = strchr( optstr, ch );
            if( ptr == NULL || *ptr == ':' ) {
                Quit( usage, "Invalid option '%c'\n", ch );
            }
            if( *( ptr + 1 ) == ':' ) {
#ifndef __ISVI__
                if( *( ptr + 2 ) == ':' ) {
                    if( currarg[optOff + 1] != 0 ) {
                        OptArg = &currarg[optOff + 1];
                    }
                    eatArg( argc, argv, 1 );
                    return( ch );
                }
#endif
                if( currarg[optOff + 1] == 0 ) {
                    if( argv[OptInd + 1] == NULL ) {
                        Quit( usage, "Option '%c' requires a parameter\n", ch );
                    }
                    OptArg = argv[OptInd + 1];
                    eatArg( argc, argv, 2 );
                    return( ch );
                }
                OptArg = &currarg[optOff + 1];
                eatArg( argc, argv, 1 );
                return( ch );
            }
            optOff++;
            if( currarg[optOff] == 0 ) {
                eatArg( argc, argv, 1 );
            }
            return( ch );
        } else {
            if( envVar != NULL ) {
                envVar = NULL;
            } else {
                OptInd++;
            }
        }
    }
}
