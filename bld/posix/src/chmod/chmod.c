/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  POSIX chmod utility
*               Changes file mode (attributes)
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dos.h>
#include <io.h>
#ifdef __NT__
    #include <windows.h>
#endif
#include "bool.h"
#include "misc.h"
#include "getopt.h"
#include "argvrx.h"
#include "argvenv.h"

#include "clibext.h"


#ifdef __NT__
typedef DWORD       fattrs;
#else
typedef unsigned    fattrs;
#endif

char *OptEnvVar = "chmod";

static const char *usageMsg[] = {
    "Usage: chmod [-?X] -|+[ahsr] [@env] [files...]",
    "\tenv         : environment variable to expand",
    "\tfiles       : files to modify the attributes of",
    "\tOptions: -? : display this message",
    "\t\t +a : turn on archive attribute",
    "\t\t +h : turn on hidden attribute",
    "\t\t +s : turn on system attribute",
    "\t\t +r : turn on read-only attribute",
    "\t\t -a : turn off archive attribute",
    "\t\t -h : turn off hidden attribute",
    "\t\t -s : turn off system attribute",
    "\t\t -r : turn off read-only attribute",
    "\t\t -X : match files by regular expressions",
    NULL
};

static fattrs   attrToAdd;
static fattrs   attrToRemove;

int main( int argc, char *argv[] )
{
    int         ch;
    int         i;
    fattrs      attr;
    bool        rxflag;

    AltOptChar = '+';
    rxflag = false;

    argv = ExpandEnv( &argc, argv );

    while( (ch = GetOpt( &argc, argv, "Xarhs", usageMsg )) != -1 ) {
        attr = 0;
        switch( ch ) {
#ifdef __NT__
        case 'a':
            attr = FILE_ATTRIBUTE_ARCHIVE;
            break;
        case 'h':
            attr = FILE_ATTRIBUTE_HIDDEN;
            break;
        case 'r':
            attr = FILE_ATTRIBUTE_READONLY;
            break;
        case 's':
            attr = FILE_ATTRIBUTE_SYSTEM;
            break;
#else
        case 'a':
            attr = _A_ARCH;
            break;
        case 'h':
            attr = _A_HIDDEN;
            break;
        case 'r':
            attr = _A_RDONLY;
            break;
        case 's':
            attr = _A_SYSTEM;
            break;
#endif
        case 'X':
            rxflag = true;
            break;
        }
        if( OptChar == '-' ) {
            attrToRemove |= attr;
        } else {
            attrToAdd |= attr;
        }
    }
    argv = ExpandArgv( &argc, argv, rxflag );

    if( argc == 1 ) {
        Quit( usageMsg, "No filename specified\n" );
    }
    for( i = 1; i < argc; i++ ) {
#ifdef __NT__
        attr = GetFileAttributes( argv[i] );
        if( attr == INVALID_FILE_ATTRIBUTES )
            continue;
#else
        _dos_getfileattr( argv[i], &attr );
#endif
#ifdef __NT__
        SetFileAttributes( argv[i], (attr | attrToAdd) & ~attrToRemove );
#else
        _dos_setfileattr( argv[i], (attr | attrToAdd) & ~attrToRemove );
#endif
    }
    return( 0 );
}
