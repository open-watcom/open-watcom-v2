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
* Description:  ExpandArgv - expand argv array, using regular expressions if needed
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined( __OS_qnx__ ) || defined( __OS_qnx16__ )
 #include <dir.h>
#else
 #include <direct.h>
#endif
#include "misc.h"
#include "filerx.h"
#include "argvrx.h"
#include "fnutils.h"

char **ExpandArgv( int *oargc, char *oargv[], int isrx )
{
    int                 argc, i;
    char                *err;
    char                **argv;
    DIR                 *directory;
    struct dirent       *nextdirentry;
    char                wild[_MAX_PATH];
    char                sp_buf[_MAX_PATH2];
    char                *drive;
    char                *dir;
    char                *name;
    char                *extin;
    char                path[_MAX_PATH];
    void                *crx;

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
            directory = OpenDirAll( oargv[i], wild );
        } else {
            directory = opendir( oargv[i] );
        }
        if( directory == NULL ) {
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
        _splitpath2( oargv[i], sp_buf, &drive, &dir, &name, &extin );
        while( ( nextdirentry = readdir( directory ) ) != NULL ) {
            FNameLower( nextdirentry->d_name );
            if( isrx ) {
                if( !FileMatch( crx, nextdirentry->d_name ) ) {
                    continue;
                }
            }
#if defined( __OS_qnx__ ) || defined( __OS_qnx16__ )
            if( S_ISREG( nextdirentry->d_stat.st_mode ) ) {
#else
            if( !( nextdirentry->d_attr & _A_SUBDIR ) ) {
#endif
                _makepath( path, drive, dir, nextdirentry->d_name, NULL );
                argv = MemRealloc( argv, ( argc + 2 ) * sizeof( char * ) );
                argv[argc] = MemAlloc( strlen( path ) + 1 );
                strcpy( argv[argc], path );
                argc++;
            }
        }
        closedir( directory );
        if( isrx ) {
            FileMatchFini( crx );
        }
    }
    argv[argc] = NULL;
    *oargc = argc;
    return( argv );
}
