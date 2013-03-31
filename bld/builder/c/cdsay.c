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
* Description:  CDSAY command used in the build process.
*
****************************************************************************/

#include <string.h>
#include <ctype.h>
#ifdef __UNIX__
#include <unistd.h>
#else
#include <direct.h>
#include <dos.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "builder.h"

void LogDir( char *dir )
{
    printf( "%s", LogDirEquals( dir ) );
}

static unsigned ChgDir( char *dir )
{
    char        *end;
    size_t      len;
#ifndef __UNIX__
    unsigned    total;
#endif

    if( dir[0] == '\0' )
        return( 0 );
    len = strlen( dir );
    end = &dir[len - 1];
#ifdef __UNIX__
    if( len > 1 && ( *end == '\\' || *end == '/' ) ) {
        *end = '\0';
    }
#else
    if( len > 1 && ( *end == '\\' || *end == '/' ) && *(end - 1) != ':' ) {
        *end = '\0';
        --len;
    }
    if( len > 1 && dir[1] == ':' ) {
        _dos_setdrive( toupper( dir[0] ) - 'A' + 1, &total );
    }
#endif
    return( chdir( dir ) );
}

int main( int argc, char **argv )
{
    int    res = 0;
    char   cwd[_MAX_PATH];

    if( argc > 1 ) {
        res = ChgDir( argv[1] );
    }
    if( res == 0 ) {
        getcwd( cwd, sizeof( cwd ) );
#ifdef __UNIX__
        LogDir( cwd );
#else
        LogDir( strupr( cwd ) );
#endif
    } else {
        printf( "Error! CDSAY: invalid directory\n" );
    }
    return res;
}
