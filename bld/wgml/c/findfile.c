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
* Description:  Implements the global function used by wgml to find and
*               open files:
*                   ff_setup()
*                   ff_teardown()
*                   free_inc_fp()
*                   search_file_in_dirs()
*               plus these local items:
*                   cur_dir_list
*                   gml_lib_dirs
*                   gml_inc_dirs
*                   path_dirs
*                   initialize_env_directory_list()
*                   try_open()
*
* Note:         The Wiki should be consulted for any term whose meaning is
*               not apparent which is related to the binary device library.
*               This should help in most such cases.
*
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__ 1
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "wgml.h"
#include "copdir.h"
#include "gvars.h"
#include "iopath.h"
#include "pathlist.h"

/* Local struct. */

/* Local data. */

static  char  *cur_dir_list = NULL;
static  char  *gml_lib_dirs = NULL;
static  char  *gml_inc_dirs = NULL;
static  char  *path_dirs = NULL;

/* Define the global variables. */

#define global
#include "findfile.h"

/* Local function definitions. */

/* Function try_open().
 * Compose full path / filename and try to open for reading.
 *
 * Parameters:
 *      prefix contains the path to use.
 *      filename contains the file name to use.
 *
 * Returns:
 *      1 if the file is found.
 *      0 if the file is not found.
 *
 * Note:
 *      If the file is found, try_file_name and try_fp will be set to the name
 *      as found and FILE * of the file.
 */

static int try_open( char *prefix, char *filename )
{
    FILE        *fp;
    char        buff[FILENAME_MAX];
    errno_t     erc;
    size_t      filename_length;

    /* Prevent buffer overflow. */

    filename_length = strnlen_s( prefix, FILENAME_MAX ) + strnlen_s( filename, FILENAME_MAX ) + 1;
    if( filename_length > FILENAME_MAX ) {
        out_msg( "File name is too long and will not be searched for:\n%s%s\n", prefix, filename );
        return(0);
    }

    /* Create the full file name to search for. */

    strcpy_s( buff, FILENAME_MAX, prefix );
    strcat_s( buff, FILENAME_MAX, filename );

    /* Clear the global variables used to contain the results. */

    if( try_file_name != NULL ) {
        mem_free( try_file_name );
        try_file_name = NULL;
    }

    if( try_fp != NULL ) {
        fclose( try_fp );
        try_fp = NULL;
    }

    /* Try to open the file. Return 0 on failure. */

    for( ;; ) {
        strlwr( buff );                 // for the sake of linux use lower only
        erc = fopen_s( &fp, buff, "rb" );
        if( erc == 0 ) break;
        if( errno != ENOMEM && errno != ENFILE && errno != EMFILE ) break;
        if( !free_resources( errno ) ) break;
    }
    if( fp == NULL ) return( 0 );

    /* Set the globals on success. */

    try_file_name = mem_alloc( filename_length );
    strcpy_s( try_file_name, filename_length, buff );
    try_fp = fp;

    return( 1 );
}

static void initialize_env_directory_list( const char *name, char **owner )
{
    size_t      len;
    char        *env;
    char        *p;
    char        c;

    len = 0;
    env = getenv( name );
    if( env != NULL && *env != '\0' ) {
        len = strlen( env );
        p = *owner = mem_alloc( len + 1 );
        while( (c = *env) != '\0' ) {
            if( p != *owner )
                *p++ = PATH_LIST_SEP;
            env = GetPathElement( env, &p );
        }
    } else {
        p = *owner = mem_alloc( 1 );
    }
    *p = '\0';
}

/* Extern function definitions. */

/* Function ff_setup().
 * Initializes the directory lists.
 */

void ff_setup( void )
{
    /* Initialize the globals. */

    try_file_name = NULL;
    try_fp = NULL;

    /* This directory list encodes the current directory. */

    cur_dir_list = mem_alloc( sizeof( char ) );
    if( cur_dir_list != NULL ) {
        cur_dir_list = mem_alloc( 1 );
        *cur_dir_list = '\0';
    }

    /* Initialize the directory list for GMLINC. */

    initialize_env_directory_list( "GMLINC", &gml_inc_dirs );

    /* Initialize the directory list for GMLLIB. */

    initialize_env_directory_list( "GMLLIB", &gml_lib_dirs );

    /* Initialize the directory list for PATH. */

    initialize_env_directory_list( "PATH", &path_dirs );

    return;
}

/* Function ff_teardown().
 * Releases the memory allocated by functions in this module.
 */

void ff_teardown( void )
{
    if( try_file_name != NULL ) {
        mem_free( try_file_name );
        try_file_name = NULL;
    }

    if( try_fp != NULL) {
        fclose( try_fp );
        try_fp = NULL;
    }

    if( cur_dir_list != NULL ) {
        mem_free( cur_dir_list );
    }

    if( gml_inc_dirs != NULL ) {
        mem_free( gml_inc_dirs );
    }

    if( gml_lib_dirs != NULL ) {
        mem_free( gml_lib_dirs );
    }

    if( path_dirs != NULL ) {
        mem_free( path_dirs );
    }

    return;
}

/* Function search_file_in_dirs().
 * Searches for filename in curdir and the directories given in the
 * environment variables, as appropriate to the value of sequence.
 *
 * Parameters:
 *      in all cases, dirseq indicates the type of file sought.
 *      if dirseq is ds_bin_lib, then:
 *          filename contains the defined name of a device, driver or font;
 *          defext and altext are ignored.
 *      if dirseq is ds_opt_file, then:
 *          filename contains the name of the file as provided by the user;
 *          defext and altext are ignored.
 *      if dirseq is ds_doc_spec or ds_lib_src, then:
 *          filename contains the name of the file as provided by the user;
 *          defext points to the first extension to use;
 *          altext points to the second extension to use, if any.
 *
 * Returns:
 *      0 if the file is not found.
 *      1 if the file is found.
 *
 * Note:
 *      if the file is found, then try_file() will have set try_file_name
 *      and try_fp to the name as found and FILE * of the file.
 */

int search_file_in_dirs( const char *filename, char *defext, char *altext, dirseq sequence )
{
    char            buff[_MAX_PATH2];
    char            alternate_file[FILENAME_MAX];
    char            default_file[FILENAME_MAX];
    char            primary_file[FILENAME_MAX];
    char            *fn_dir;
    char            *fn_drive;
    char            *fn_ext;
    char            *fn_name;
    char            *member_name = NULL;
    char            *searchdirs[4];
    char            *path_list;
    char            *p;
    char            c;
    char            dir_name[FILENAME_MAX];
    int             i;
    size_t          member_length;

    /* Ensure filename will fit into buff. */

    if( strnlen_s( filename, FILENAME_MAX ) == FILENAME_MAX ) {
        out_msg( "File name is too long and will not be searched for:\n%s\n", filename );
        err_count++;
        g_suicide();
    }

    /* Initialize the filename buffers. */

    primary_file[0] = '\0';
    alternate_file[0] = '\0';
    default_file[0] = '\0';

    /* For ds_bin_lib, filename contains a defined name. */

    if( sequence != ds_bin_lib ) {

        /* Determine if filename contains path information. */

        _splitpath2( filename, buff, &fn_drive, &fn_dir, &fn_name, &fn_ext );

        if( fn_drive[0] != '\0' || fn_dir[0] != '\0' ) {
            out_msg( "File names cannot contain path information! Filename:\n%s\n", filename );
            err_count++;
            g_suicide();
        }

        /* Ensure the file name will fit in the buffers if the literal extensions
         * are used. Note that all literal extensions contain 4 characters.
         */

        if( *fn_ext == '\0' ) {
            if( strnlen_s( filename, FILENAME_MAX ) + 4 == FILENAME_MAX ) {
                out_msg( "File name is too long and will not be searched for:\n%s\nNote: length used includes a default extension.", filename );
                err_count++;
                g_suicide();
            }
        }
    }

    /* Set up the file names and the dirs for the specified sequence. */

    switch( sequence ) {
    case ds_opt_file:
        strcpy_s( primary_file, FILENAME_MAX, fn_name );
        if( *fn_ext == '\0' ) {
            strcat_s( primary_file, FILENAME_MAX, ".opt" );
        } else {
            strcat_s( primary_file, FILENAME_MAX, fn_ext );
        }
        searchdirs[0] = cur_dir_list;
        searchdirs[1] = gml_lib_dirs;
        searchdirs[2] = gml_inc_dirs;
        searchdirs[3] = path_dirs;
        break;
    case ds_doc_spec:
        strcpy_s( primary_file, FILENAME_MAX, fn_name );
        if( *fn_ext == '\0' ) {
            strcat_s( primary_file, FILENAME_MAX, ".gml" );
        } else {
            strcat_s( primary_file, FILENAME_MAX, fn_ext );
        }
        if( *altext != '\0' && *fn_ext == '\0' ) {
            strcpy_s( alternate_file, FILENAME_MAX, fn_name );
            strcat_s( alternate_file, FILENAME_MAX, altext );
        }
        if( *fn_ext == '\0' && strcmp( defext, ".gml" )) {
            strcpy_s( default_file, FILENAME_MAX, fn_name );
            strcat_s( default_file, FILENAME_MAX, ".gml" );
        }
        searchdirs[0] = cur_dir_list;
        searchdirs[1] = gml_inc_dirs;
        searchdirs[2] = gml_lib_dirs;
        searchdirs[3] = path_dirs;
        break;
    case ds_bin_lib:
        searchdirs[0] = gml_lib_dirs;
        searchdirs[1] = gml_inc_dirs;
        searchdirs[2] = path_dirs;
        searchdirs[3] = NULL;
        break;
    case ds_lib_src:
        strcpy_s( primary_file, FILENAME_MAX, fn_name );
        if( *fn_ext == '\0' ) {
            strcat_s( primary_file, FILENAME_MAX, ".pcd" );
        } else {
            strcat_s( primary_file, FILENAME_MAX, fn_ext );
        }
        strcpy_s( alternate_file, FILENAME_MAX, fn_name );
        if( *altext == '\0' ) {
            strcat_s( alternate_file, FILENAME_MAX, ".fon" );
        } else {
            strcat_s( alternate_file, FILENAME_MAX, altext );
        }
        searchdirs[0] = cur_dir_list;
        searchdirs[1] = gml_inc_dirs;
        searchdirs[2] = NULL;
        searchdirs[3] = NULL;
        break;
    default:
        out_msg( "findfile internal error\n" );
        err_count++;
        g_suicide();
    }

    /* Search each directory for each filename. */

    for( i = 0; i < 4 && searchdirs[i] != NULL; i++ ) {
        path_list = searchdirs[i];
        while( (c = *path_list) != '\0' ) {
            p = dir_name;
            do {
                ++path_list;
                if( IS_PATH_LIST_SEP( c ) )
                    break;
                *p++ = c;
            } while( (c = *path_list) != '\0' );
            c = p[-1];
            if( !IS_PATH_SEP( c ) ) {
                *p++ = DIR_SEP;
            }
            *p = '\0';

            /* For ds_bin_lib, set primary file from the defined name. */

            if( sequence == ds_bin_lib ) {

            /* See if dir_ptr contains a wgmlst.cop file. */

                if( try_open( dir_name, "wgmlst.cop" ) == 0 ) continue;

                /* try_fp now contains a FILE * to the directory file. */

                member_name = get_member_name( filename );
                if( member_name == NULL ) continue;

                /* Construct primary_file and open it normally. */

                member_length = strnlen_s( member_name, FILENAME_MAX );
                if( memchr( member_name, '.', member_length ) == NULL ) {

                    /* Avoid buffer overflow from member_name. */

                    if( member_length < FILENAME_MAX ) {
                        strcpy_s( primary_file, FILENAME_MAX, member_name );

                        /* Avoid buffer overflow from the extension. */

                        if( member_length + 4 < FILENAME_MAX ) {
                            strcat_s( primary_file, FILENAME_MAX, ".cop" );
                            mem_free( member_name );
                            member_name = NULL;
                        } else {
                            out_msg( "Member name is too long and will not be searched for:\n%s.cop\n", member_name );
                            mem_free( member_name );
                            member_name = NULL;
                            err_count++;
                            g_suicide();
                        }
                    } else {
                        out_msg( "Member name is too long and will not be searched for:\n%s\n", member_name );
                        mem_free( member_name );
                        member_name = NULL;
                        err_count++;
                        g_suicide();
                    }
                }
            }

            if( try_open( dir_name, primary_file ) != 0 ) return( 1 );

            /* Not finding the file is only a problem for ds_bin_lib. */

            if( sequence == ds_bin_lib ) {
                out_msg( "Member file not found in same directory as directory file:\n%s%s\n", dir_name, primary_file );
                err_count++;
                g_suicide();
            }

            if( alternate_file != NULL ) {
                if( try_open( dir_name, alternate_file ) != 0 ) return( 1 );
            }

            if( default_file != NULL ) {
                if( try_open( dir_name, default_file ) != 0 ) return( 1 );
            }
        }
    }

    return( 0 );
}
