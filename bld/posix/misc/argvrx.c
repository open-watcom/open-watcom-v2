/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  ExpandArgv - expand argv array, using regular expressions if needed
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined( __QNX__ )
 #include <sys/dir.h>
#else
 #include <direct.h>
#endif
#include "bool.h"
#include "wio.h"
#include "watcom.h"
#include "misc.h"
#include "argvrx.h"
#include "fnutils.h"
#include "filerx.h"
#include "pathgrp2.h"

#include "clibext.h"


char **ExpandArgv( int *oargc, char *oargv[], bool isrx )
{
    int                 argc, i;
    char                *err;
    char                **argv;
    DIR                 *dirp;
    struct dirent       *dire;
    char                wild[_MAX_PATH];
    pgroup2             pg;
    char                path[_MAX_PATH];
    void                *crx = NULL;

    argc = 1;
    argv = MemAlloc( 2 * sizeof( char * ) );
    argv[0] = oargv[0];
    for( i = 1; i < *oargc; i++ ) {
        if( !FileNameWild( oargv[i], isrx ) ) {
            argv = MemRealloc( argv, ( argc + 2 ) * sizeof( char * ) );
            argv[argc] = oargv[i];
            argc++;
            continue;
        }
        if( isrx ) {
            dirp = opendir( FileMatchDirAll( oargv[i], path, wild ) );
        } else {
            dirp = opendir( oargv[i] );
        }
        if( dirp == NULL ) {
            argv = MemRealloc( argv, ( argc + 2 ) * sizeof( char * ) );
            argv[argc] = oargv[i];
            argc++;
            continue;
        }
        if( isrx ) {
            err = FileMatchInit( &crx, wild );
            if( err != NULL ) {
                Die( "\"%s\": %s\n", err );
            }
        }
        _splitpath2( oargv[i], pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
        while( (dire = readdir( dirp )) != NULL ) {
            FNameLower( dire->d_name );
            if( isrx ) {
                if( !FileMatch( crx, dire->d_name ) ) {
                    continue;
                }
            }
#if defined( __QNX__ )
            if( S_ISREG( dire->d_stat.st_mode ) ) {
#else
            if( (dire->d_attr & _A_SUBDIR) == 0 ) {
#endif
                _makepath( path, pg.drive, pg.dir, dire->d_name, NULL );
                argv = MemRealloc( argv, ( argc + 2 ) * sizeof( char * ) );
                argv[argc] = MemAlloc( strlen( path ) + 1 );
                strcpy( argv[argc], path );
                argc++;
            }
        }
        closedir( dirp );
        if( isrx ) {
            FileMatchFini( crx );
        }
    }
    argv[argc] = NULL;
    *oargc = argc;
    return( argv );
}
