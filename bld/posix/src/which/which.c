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
* Description:  POSIX which utility
*               Finds a file along PATH or other environment variable
*
****************************************************************************/


#include <direct.h>
#include <io.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "getopt.h"
#include "misc.h"
#include "fnutils.h"

static int foundAFile;
static int findAll;
static char drive[ _MAX_DRIVE ];
static char dir[ _MAX_DIR ];
static char fname[ _MAX_FNAME ];
static char ext[ _MAX_EXT ];
static char path[ _MAX_PATH ];
static char open_path[ _MAX_PATH ];

static const char * usageTxt[] = {
"Usage: which [-?a] [-e env_name] filename",
"-?\t\tdisplay this help",
"-a\t\tdisplay all matches, not just the first",
"-e env_name\tuse value of env_name instead of PATH for searching",
"returns 0 if a file found, 1 if no files found or error.",
NULL
};

char *OptEnvVar = "which";


static void writeNL( const char *buf )
{
    write( 1, buf, strlen( buf ) );
    write( 1, "\n", 1 );
}


static void checkDir( void )
{
    DIR                 *parent;
    struct dirent       *entry;
    char                *p;

    _makepath( open_path, NULL, path, fname, ext );
    parent = opendir( open_path );
    if( parent == NULL ) {
        return;
    }
    entry = readdir( parent );
    while( entry != NULL ) {
        p = entry->d_name;
        if( p[0] != '.' || ( p[1] != 0 && ( p[1] != '.' || p[2] != 0 ) ) ) {
            _makepath( open_path, NULL, path, p, NULL );
            FNameLower( open_path );
            writeNL( open_path );
            foundAFile = 1;
            if( !findAll ) break;
        }
        entry = readdir( parent );
    }

    closedir( parent );
}


static void work( const char *path_list, const char *name )
{
    char    *end_path;
    size_t  path_len;

    _splitpath( name, drive, dir, fname, ext );
    if( drive[0] != 0 || dir[0] != 0 ) {
            /* absolute path, so we just check the abs path */
        if( access( name, 0 ) == 0 ) {
            strcpy( path, name );
            FNameLower( path );
            writeNL( path );
            foundAFile = 1;
        }
        return;
    }
    if( ext[0] == 0 ) {
        strcpy( ext, ".*" );
    }

    path[0] = 0;    /* check current directory first */
    checkDir();
    if( foundAFile && !findAll ) return;
    while( path_list != NULL ) {
        end_path = strchr( path_list, ';' );
        if( end_path == NULL ) {
            end_path = strchr( path_list, 0 );  /* find null terminator */
        }
        path_len = end_path - path_list;
        if( path_len > _MAX_PATH - 1 ) {
            writeNL( "Path too long in environment variable!" );
            exit( 1 );
        }
        memcpy( path, path_list, path_len );
        path[ path_len ] = 0;
        checkDir();
        if( foundAFile && !findAll ) return;
        if( *end_path != ';' ) break;
        path_list = end_path + 1;
        if( *path_list == 0 ) break;
    }
}


int main( int argc, char **argv )
{
    int     i;
    char    *env_value;

    env_value = getenv( "PATH" );
    for(;;) {
        i = GetOpt( &argc, argv, "ae:", usageTxt );
        if( i == -1 ) break;
        if( i == 'a' ) findAll = 1;
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
        work( env_value, argv[ i ] );
    }

    return( foundAFile == 0 );
}
