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


#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <errno.h>
#include <string.h>

/* Add environment strings to be searched here */
char *paths_to_check[] = {
        "DOS4GPATH",
        "PATH"};


char *dos4g_path()
{
    int         i;
    char        *dos4gpath;
    static char fullpath[_MAX_PATH];

    /* If DOS4GPATH points to an executable file name, don't bother
       searching any paths for DOS4GW.EXE.
    */
    if( dos4gpath = getenv( "DOS4GPATH" ) )
    {
        strlwr( strcpy( fullpath, dos4gpath ) );
        if( strstr( fullpath, ".exe" ) ) return( fullpath );
    }
    for( i = 0;
         i < sizeof( paths_to_check ) / sizeof( paths_to_check[0] );
         i++ ) {
        _searchenv( "dos4gw.exe", paths_to_check[i], fullpath );
        if( fullpath[0] ) return( fullpath );
    }
    for( i = 0;
         i < sizeof( paths_to_check ) / sizeof( paths_to_check[0] );
         i++ ) {
        _searchenv( "dos4g.exe", paths_to_check[i], fullpath );
        if( fullpath[0] ) return( fullpath );
    }
    /* Don't bother returning current directory
       since _searchenv covers this case */
    return( "\\dos4gw.exe" );
}

main( int argc, char *argv[] )
{
    char        *av[4];
    auto char   cmdline[128];

    av[0] = dos4g_path();           /* Locate the DOS/4GW loader */
    av[1] = argv[0];                /* name of executable to run */
    av[2] = getcmd( cmdline );      /* command line */
    av[3] = NULL;                   /* end of list */
#ifdef QUIET
    putenv( "DOS4G=QUIET" );        /* disables DOS/4GW Copyright banner */
                                        /* (note: you must display elsewhere) */
#endif
    execvp( (const char *)av[0], (const char **)av );
    puts( "Stub exec failed:" );
    puts( av[0] );
    puts( strerror( errno ) );
    exit( 1 );                      /* indicate error */
}
