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
* Description:  Find a file along PATH or other environment variable.
*               Note: This utility is not specified by POSIX.
*
****************************************************************************/


#ifdef __UNIX__
#include <dirent.h>
#else
#include <direct.h>  // FIXME: provide dirent.h for non-UNIX platforms
#endif
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "util.h"


static int found_file;
static int find_all;
static char drive[_MAX_DRIVE];
static char dir[_MAX_DIR];
static char fname[_MAX_FNAME];
static char ext[_MAX_EXT];
static char path[_MAX_PATH];
static char open_path[_MAX_PATH];


static const char *usage_text[] = {
    "Usage: which [-?a] [-e env] file...",
    "\tfile       : files to look for",
    "\tOptions:",
    "\t\t -?     : print this list",
    "\t\t -a     : display all matches, not just the first",
    "\t\t -e env : search variable `env' instead of PATH",
    "\treturns 0 if a file found, 1 if no files found or error.",
    NULL
};


static void check_dir( void )
/***************************/
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
            puts( open_path );
            found_file = 1;
            if( !find_all ) break;
        }
        entry = readdir( parent );
    }

    closedir( parent );
}


static void work( const char *path_list, const char *name )
/*********************************************************/
{
    char    *end_path;
    size_t  path_len;

    _splitpath( name, drive, dir, fname, ext );
    if( drive[0] != 0 || dir[0] != 0 ) {
        /* absolute path, so we just check the abs path */
        if( access( name, 0 ) == 0 ) {
            strcpy( path, name );
            puts( path );
            found_file = 1;
        }
        return;
    }
    if( ext[0] == 0 ) {
        strcpy( ext, ".*" );
    }

    while( path_list != NULL ) {
        end_path = strchr( path_list, PATH_SEP_CHAR );
        if( end_path == NULL ) {
            end_path = strchr( path_list, 0 );  /* find null terminator */
        }
        path_len = end_path - path_list;
        if( path_len > _MAX_PATH - 1 ) {
            puts( "which: path too long in environment variable!" );
            exit( 1 );
        }
        memcpy( path, path_list, path_len );
        path[path_len] = 0;
        check_dir();
        if( found_file && !find_all ) return;
        if( *end_path != PATH_SEP_CHAR ) break;
        path_list = end_path + 1;
        if( *path_list == 0 ) break;
    }
}


int main( int argc, char **argv )
/*******************************/
{
    int     i, ch;
    char    *env_value;

    env_value = getenv( "PATH" );
    while( (ch = getopt( argc, argv, ":ae:" )) != -1 ) {
        switch( ch ) {
        case 'a':
            find_all = 1;
            break;
        case 'e':
            if( *optarg == '=' ) {
                ++optarg;
            };
            env_value = getenv( optarg );
            break;
        case ':':
            util_quit( usage_text, "which: invalid argument\n" );
        case '?':
            util_quit( usage_text, NULL );
        }
    }

    /* skip option arguments */
    argc = argc - optind + 1;
    argv += optind - 1;

    if( argc < 2 ) {
        util_quit( usage_text, "which: must specify at least 1 filename\n" );
    }

    found_file = 0;
    for( i = 1; i < argc; ++i ) {
        work( env_value, argv[i] );
    }

    return( found_file == 0 );
}
