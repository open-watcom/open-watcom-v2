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
* Description:  Suspend execution for specified time.
*
****************************************************************************/


#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include "util.h"


static const char *usage_text[] = {
    "Usage: sleep [-?] time",
    "\ttime               : time to sleep in seconds, or a time description",
    "\t                     of the form [#h#m]#[s] (any part may be missing)",
    "\tOptions: -?        : display this message",
    NULL
};


int main( int argc, char **argv )
/*******************************/
{
    unsigned    secs, mins, hours;
    char        *p, *op;
    int         ch;

    while( (ch = getopt( argc, argv, ":" )) != -1 ) {
        if( ch == '?' ) {
            util_quit( usage_text, NULL );
        }
    }

    if( argc != 2 ) {
        util_die( "%s\n", usage_text[0] );
    } else {
        hours = 0;
        mins  = 0;
        secs  = 0;

        op = argv[1];
        for( p = argv[1]; *p != '\0'; p++ ) {
            switch( *p ) {
                case 'h':
                    *p = 0;
                    hours += atoi( op );
                    op = p + 1;
                    break;
                case 'm':
                    *p = 0;
                    mins += atoi( op );
                    op = p + 1;
                    break;
                case 's':
                    *p = 0;
                    secs += atoi( op );
                    op = p + 1;
                    break;
                default:
                    if( !isdigit( *p ) ) {
                        util_die( "sleep: invalid time specified\n" );
                    }
            }
        }
        secs += atoi( op ) + 60 * mins + 3600 * hours;
        sleep( secs );
    }
    return( 0 );
}
