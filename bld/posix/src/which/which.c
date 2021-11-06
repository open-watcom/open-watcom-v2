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
* Description:  POSIX which utility
*               Finds a file along PATH or other environment variable
*
****************************************************************************/


#include <direct.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "wio.h"
#include "getopt.h"
#include "misc.h"
#include "fnutils.h"
#include "pathgrp2.h"

#include "clibext.h"


char    *OptEnvVar = "which";

static int          foundAFile;
static int          findAll;
static pgroup2      pg;
static char         path[_MAX_PATH];
static char         open_path[_MAX_PATH];

static const char * usageTxt[] = {
    "Usage: which [-?a] [-e env_name] filename",
    "-?\t\tdisplay this help",
    "-a\t\tdisplay all matches, not just the first",
    "-e env_name\tuse value of env_name instead of PATH for searching",
    "returns 0 if a file found, 1 if no files found or error.",
    NULL
};

static int skip_entry( const char *p )
{
    /* skip '.' and '..' entries */
    return( p[0] == '.' && ( p[1] == '\0' || p[1] == '.' && p[2] == '\0' ) );
}

static void writeNL( const char *buf )
{
    write( 1, buf, (unsigned)strlen( buf ) );
    write( 1, "\n", 1 );
}

static void checkDir( void )
{
    DIR                 *dirp;
    struct dirent       *dire;

    _makepath( open_path, NULL, path, pg.fname, pg.ext );
    dirp = opendir( open_path );
    if( dirp != NULL ) {
        while( (dire = readdir( dirp )) != NULL ) {
            if( skip_entry( dire->d_name ) )
                continue;
            _makepath( open_path, NULL, path, dire->d_name, NULL );
            FNameLower( open_path );
            writeNL( open_path );
            foundAFile = 1;
            if( !findAll ) {
                break;
            }
        }
        closedir( dirp );
    }
}


static void work( const char *path_list, const char *name )
{
    char    *end_path;
    size_t  path_len;

    _splitpath2( name, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
    if( pg.drive[0] != '\0' || pg.dir[0] != '\0' ) {
        /* absolute path, so we just check the abs path */
        if( access( name, 0 ) == 0 ) {
            strcpy( path, name );
            FNameLower( path );
            writeNL( path );
            foundAFile = 1;
        }
        return;
    }
    if( pg.ext[0] == '\0' ) {
        pg.ext = "*";
    }

    path[0] = '\0';    /* path to search, check current directory first */
    checkDir();
    if( foundAFile && !findAll )
        return;
    while( path_list != NULL ) {
        end_path = strchr( path_list, ';' );
        if( end_path == NULL ) {
            end_path = strchr( path_list, '\0' );  /* find null terminator */
        }
        path_len = end_path - path_list;
        if( path_len > _MAX_PATH - 1 ) {
            writeNL( "Path too long in environment variable!" );
            exit( 1 );
        }
        memcpy( path, path_list, path_len );
        path[path_len] = '\0';      /* path to search */
        checkDir();
        if( foundAFile && !findAll )
            return;
        if( *end_path != ';' )
            break;
        path_list = end_path + 1;
        if( *path_list == '\0' ) {
            break;
        }
    }
}


int main( int argc, char **argv )
{
    int     i;
    char    *env_value;

    findAll = 0;
    env_value = getenv( "PATH" );
    for( ;; ) {
        i = GetOpt( &argc, argv, "ae:", usageTxt );
        if( i == -1 )
            break;
        if( i == 'a' )
            findAll = 1;
        if( i == 'e' ) {
            if( OptArg[0] == '=' ) {
                env_value = getenv( OptArg+1 );
            } else {
                env_value = getenv( OptArg );
            }
        }
    }

    if( argc < 2 ) {
        Quit( usageTxt, "must specify at least 1 filename\n" );
    }

    foundAFile = 0;
    for( i = 1; i < argc; ++i ) {
        work( env_value, argv[i] );
    }

    return( ( foundAFile ) ? EXIT_SUCCESS : EXIT_FAILURE );
}
