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

#include "plusplus.h"

#include <stdio.h>

#include "dbg.h"

#define MX_FSTK         10
#define default_file    "_CPPDBG_."

static FILE* fstk[ MX_FSTK ];   // suspended files
static unsigned index;          // top of files stack
static int logging;             // true ==> logging at level 0

static void reDirSwitch         // SWITCH TWO FILE AREAS
    ( void )
{
    FILE temp;                  // - temporary area
    FILE* fp;                   // - file

    fflush( stdout );
    temp = *stdout;
    fp = fstk[ index ];
    *stdout = *fp;
    *fp = temp;
}

static void reDirBeg            // START REDIRECTION FOR A FILE
    ( void )
{
    if( index >= MX_FSTK ) {
        puts( "DBGIO -- too many log files active" );
        fflush( stdout );
    } else {
        char fname[32];
        FILE* fp;
        strcpy( fname, default_file );
        itoa( index, &fname[ sizeof( default_file ) - 1 ], 10 );
        fp =  fopen( fname, "wt" );
        if( NULL == fp ) {
            puts( "DBGIO -- failure to open file" );
            puts( fname );
            fstk[ index ] = 0;
        } else {
            fstk[ index ] = fp;
            reDirSwitch();
        }
    }
    ++ index;
}

static void reDirEnd            // COMPLETE REDIRECTION FOR A FILE
    ( void )
{
    if( index == 0 ) {
        puts( "DBGIO -- too many files closed" );
    } else {
        -- index;
        if( index < MX_FSTK ) {
            FILE* fp = fstk[ index ];
            if( fp != 0 ) {
                reDirSwitch();
                fclose( fstk[ index ] );
            }
        }
    }
}

void DbgRedirectBeg             // START REDIRECTION
    ( void )
{
    if( index == 0 ) {
        reDirBeg();
        logging = 0;
    }
}

int DbgRedirectEnd              // COMPLETE REDIRECTION
    ( void )
{
    int retn;                   // - # of file to view

    retn = index - 1;
    if( index > 1 || logging ) {
        fflush( stdout );
    } else {
        reDirEnd();
    }
    return retn;
}

void DbgLogBeg                  // START LOGGING
    ( void )
{
    if( index == 0 ) {
        logging = 1;
    }
    reDirBeg();
}

int DbgLogEnd                   // END LOGGING
    ( void )
{
    if( index > 0 ) {
        reDirEnd();
    }
    return index;
}
