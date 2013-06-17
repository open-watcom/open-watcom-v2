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
* Description:  POSIX date utility
*               Displays current date and time
*
****************************************************************************/

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "getopt.h"
#include "misc.h"
#include "clibext.h"

static const char *usageTxt[] = {
    "Usage:\tdate [-?u] [\"+format\"]",
    "\t+format     : string with formatspecifiers for strftime",
    "\tOptions: -? : print this list",
    "\t\t -u : Use UTC0 instead of TZ setting",
    NULL
};

char *OptEnvVar = "";

static int      flagUTC;

void main( int argc, char **argv )
{
    struct tm   *time_of_day;
    time_t      ltime;
    char        buf[ 256 ];
    int         ch;
    char        fmt[ 256 ] = "%a %b %e %H:%M:%S %Z %Y";

    for(;;) {
        ch = GetOpt( &argc, argv, "u", usageTxt );
        if( ch == -1 ) break;
        switch( ch ) {
        case 'u':
            flagUTC = 1;
            break;
        }
    }
    if( argc > 2 ) {
        Quit( usageTxt, "invalid number of arguments\n" );
    }
    argv++;
    if( *argv != NULL ) {
        if( **argv == '+' ) {
            if( (*argv)[ 1 ] == '\0' ) {
                Quit( usageTxt, "empty formatstring not allowed\n" );
            }
            strncpy( fmt, &((*argv)[ 1 ]), sizeof( fmt ) );
            fmt[ sizeof( fmt ) - 1 ] = '\0';
        } else {
            Quit( usageTxt, "invalid argument %s\n", *argv );
        }
    }
    time( &ltime );
    if( flagUTC ) {
        time_of_day = gmtime( &ltime );
    } else {
        time_of_day = localtime( &ltime );
    }
    if( 0 == strftime( buf, sizeof( buf ), fmt, time_of_day ) ) {
        Quit( usageTxt, "strftime formatting error %s\n", fmt );
    } else {
        cputs( buf );
    }
    exit( EXIT_SUCCESS );
}
