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
*                   cur_dir
*                   cur_dir_list
*                   directory_list
*                   gml_lib_dirs
*                   gml_inc_dirs
*                   initialize_directory_list()
*                   path_dirs
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

#if 0
/* This macro is retained in case search_file_in_dirs() is ever extended to accept
 * filenames which include path information. The intended use of this macro is to
 * determine if fn_path begins with an OS-appropriate slash character.
 */

#if defined( __UNIX__ )
    #define IS_PATH_SEP( ch ) ((ch) == '/')
#else
    #define IS_PATH_SEP( ch ) ((ch) == '/' || (ch) == '\\')
#endif
#endif

/* Local struct. */

/* The count contains the number of entries in directories. */

typedef struct {
    char * * directories;
    uint16_t count;
} directory_list;

/* Local data. */

static  size_t          env_var_length  = 0;
static  char    *       env_var_buffer  = NULL;
static  char    *       cur_dir = "";
static  directory_list  cur_dir_list;
static  directory_list  gml_lib_dirs;
static  directory_list  gml_inc_dirs;
static  directory_list  path_dirs;

/* Define the global variables. */

#define global
#include "findfile.h"

/* Local function definitions. */

/* Function initialize_directory_list().
 * Initializes in_directory_list from in_path_list.
 *
 * Parameters:
 *      in_path_list is a string containing the contents of a path list.
 *      in_directory_list is a pointer to the directory list to initialize.
 *
 * Modified Parameter:
 *      *in_directory_list is modified as shown below.
 *
 * Notes:
 *      in_directory_list should be in its as-created state.
 *      If in_path_list is NULL, *in_directory_list will be cleared.
 *      If *in_directory_list->directories is not NULL on exit, it is a
 *          single block of memory and can be freed with one mem_free().
 */

static void initialize_directory_list( char const * in_path_list, \
                                        directory_list * in_list )
{
    char *          current;
    char * *        array_base;
    directory_list  local_list;
    int             i;
    int             j;
    int             k;
    uint16_t        byte_count;
    uint16_t        path_count;

    /* If in_path_list is NULL, return at once. */

    if( in_path_list == NULL ) {
        in_list->count = 0;
        mem_free( in_list->directories );
        in_list->directories = NULL;
        return;
    }

    /* Note: the two for loops which parse the path names were tested and
     * tweaked. Please do not change them without thoroughly testing them.
     * They are intended to correctly handle all directory lists, even if
     * spaces and quotation marks are involved.
     */

    /* Determine the number of paths and the total length needed. */

    byte_count = 0;
    path_count = 0;
    for( i = 0; i < strlen( in_path_list ); i++ ) {
        if( in_path_list[i] == '"' ) {
            for( j = i + 1; j < strlen( in_path_list ); j++ ) {
                if( in_path_list[j] == '"' ) break;
                byte_count++;
            }
            i = j;
            if( in_path_list[i] == '\0' ) {
                if( in_path_list[i - 1] == INCLUDE_SEP ) path_count++;
                if( in_path_list[i - 1] != PATH_SEP ) byte_count++;
                break;
            }
            continue;
        }
        if( in_path_list[i] == INCLUDE_SEP ) {
            if( in_path_list[i - 1] != INCLUDE_SEP ) {
                path_count++;
                if( in_path_list[i - 1] == '"' ) {
                    if( in_path_list[i - 2] != PATH_SEP ) byte_count++;
                }
                if( in_path_list[i - 1] != PATH_SEP ) byte_count++;
                continue;
            }
        }
        byte_count++;
    }
    if( in_path_list[i - 1] != INCLUDE_SEP ) {
        path_count++;
        if( in_path_list[i - 1] == '"' ) {
            if( in_path_list[i - 2] != PATH_SEP ) byte_count++;
        }
        if( in_path_list[i - 1] != PATH_SEP ) byte_count++;
    }

    /* Initialize local_list. */

    local_list.count = path_count;
    local_list.directories = (char * *) mem_alloc( \
                    (path_count * sizeof( char * )) + byte_count + path_count );

    array_base = local_list.directories;
    current = (char *) array_base + (path_count * sizeof( char * ));

    k = 0;
    array_base[0] = current;
    for( i = 0; i < strlen( in_path_list ); i++ ) {
        if( in_path_list[i] == '"' ) {
            for( j = i + 1; j < strlen( in_path_list ); j++ ) {
                if( in_path_list[j] == '"' ) break;
                *current++ = in_path_list[j];
            }
            i = j;
            if( in_path_list[i] == '\0' ) {
                if( in_path_list[i - 1] == INCLUDE_SEP ) {
                    if( ++k < path_count ) array_base[k] = current;
                }
                if( in_path_list[i - 1] != PATH_SEP ) *current++ = PATH_SEP;
                *current++ = '\0';
                break;
            }
            continue;
        }
        if( in_path_list[i] == INCLUDE_SEP ) {
            if( in_path_list[i - 1] != INCLUDE_SEP ) {
                if( in_path_list[i - 1] != PATH_SEP ) {
                    if( in_path_list[i - 1] == '"' ) {
                        if( in_path_list[i - 2] != PATH_SEP ) \
                                                        *current++ = PATH_SEP;
                    } else *current++ = PATH_SEP;
                }
                *current++ = '\0';
                if( ++k < path_count ) array_base[k] = current;
            }
            continue;
        }
        *current++ = in_path_list[i];
    }
    if( in_path_list[i - 1] != INCLUDE_SEP ) {
        if( in_path_list[i - 1] == '"' ) {
            if( in_path_list[i - 2] != PATH_SEP ) *current++ = PATH_SEP;
        }
        if( in_path_list[i - 1] != PATH_SEP ) *current++ = PATH_SEP;
        *current++ = '\0';
    }

    /* Check for over-length directory names and remove any found. */

    byte_count = 0;
    path_count = local_list.count;

    for( i = 0; i < local_list.count; i++ ){
        if( strlen( local_list.directories[i] ) > FILENAME_MAX ) {
            path_count--;
            out_msg( "Directory path is too long and will not be used:\n  %s\n", \
                                                    local_list.directories[i] );
            local_list.directories[i] = NULL;
        } else {
            byte_count += strnlen_s( local_list.directories[i], FILENAME_MAX );
        }
    }

    /* If local_list has no entries at all, free local_list.directories and
     * clear in_list and return.
     */

    if( path_count == 0 ) {
        mem_free( local_list.directories );
        local_list.directories = NULL;
        in_list->count = 0;
        mem_free( in_list->directories );
        in_list->directories = NULL;
        return;
    }

    /* If some paths were eliminated for length, then initialize in_list with
     * the remaining paths (only) and free local_list.directories. Otherwise,
     * local_list.directories becomes in_list->directories.
     */

    if( path_count < local_list.count) {

        in_list->count = path_count;
        in_list->directories = (char * *) mem_alloc( \
                    (path_count * sizeof( char * )) + byte_count + path_count );

        array_base = in_list->directories;
        current = (char *) (array_base + path_count * sizeof( char * ));

        byte_count += path_count;
        j = 0;
        for( i = 0; i < local_list.count; i++ ) {
            if( local_list.directories[i] != NULL ) {
                array_base[j] = current;
                strcpy_s( current, byte_count, local_list.directories[i] );
                byte_count -= (strlen( local_list.directories[i] ) + 1);
                current += (strlen( local_list.directories[i] ) + 1);
                j++;
            }
        }
        mem_free( local_list.directories );
        local_list.directories = NULL;
    } else {
        in_list->count = local_list.count;
        in_list->directories = local_list.directories;
    }

    return;
}

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

static int try_open( char * prefix, char * filename )
{
    FILE    *   fp;
    char        buff[FILENAME_MAX];
    errno_t     erc;
    size_t      filename_length;

    /* Prevent buffer overflow. */

    filename_length = strnlen_s( prefix, FILENAME_MAX ) + \
                                        strnlen_s( filename, FILENAME_MAX ) + 1;
    if( filename_length > FILENAME_MAX ) {
        out_msg( "File name is too long and will not be searched for:\n%s%s\n", \
                                                                prefix, filename );
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

/* Extern function definitions. */

/* Function ff_setup().
 * Initializes the directory lists.
 */

void ff_setup( void )
{
    errno_t rc;
    size_t  gmlinc_length;
    size_t  gmllib_length;
    size_t  max_length;
    size_t  path_length;

    /* Initialize the globals. */

    try_file_name = NULL;
    try_fp = NULL;

    /* This directory list encodes the current directory. */

    cur_dir_list.count = 1;
    cur_dir_list.directories = &cur_dir;

    /* Initialize the directory lists. */

    gml_inc_dirs.count = 0;
    gml_inc_dirs.directories = NULL;
    gml_lib_dirs.count = 0;
    gml_lib_dirs.directories = NULL;
    path_dirs.count = 0;
    path_dirs.directories = NULL;

    /* Get the lengths of the environment variables. */

    getenv_s( &gmlinc_length, NULL, 0, "GMLINC" );
    getenv_s( &gmllib_length, NULL, 0, "GMLLIB" );
    getenv_s( &path_length, NULL, 0, "PATH" );

    /* Set max_length to the largest of the three. */

    max_length = path_length;
    if( gmlinc_length > max_length ) max_length = gmlinc_length;
    if( gmllib_length > max_length ) max_length = gmllib_length;

    /* Allocate the buffer, allowing for a terminating '\0'. */

    max_length++;
    env_var_buffer = mem_alloc( max_length );

    /* Initialize the directory list for GMLINC. */

    rc = getenv_s( &env_var_length, env_var_buffer, max_length, "GMLINC" );

    if( rc == 0 ) env_var_buffer[env_var_length] = '\0';
    else env_var_length = 0;

    if( env_var_length > 0 ) initialize_directory_list( env_var_buffer, \
                                                        &gml_inc_dirs );

    /* Initialize the directory list for GMLLIB. */

    rc = getenv_s( &env_var_length, env_var_buffer, max_length, "GMLLIB" );

    if( rc == 0 ) env_var_buffer[env_var_length] = '\0';
    else env_var_length = 0;

    if( env_var_length > 0 ) initialize_directory_list( env_var_buffer, \
                                                        &gml_lib_dirs );
    /* Initialize the directory list for PATH. */

    rc = getenv_s( &env_var_length, env_var_buffer, max_length, "PATH" );

    if( rc == 0 ) env_var_buffer[env_var_length] = '\0';
    else env_var_length = 0;

    if( env_var_length > 0 ) initialize_directory_list( env_var_buffer, \
                                                        &path_dirs );

    /* Free the environment variable buffer. */

    mem_free( env_var_buffer );
    env_var_buffer = NULL;

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

    /* directories points to a single block of allocated memory. */

    if( gml_inc_dirs.directories != NULL ) {
        mem_free( gml_inc_dirs.directories );
        gml_inc_dirs.directories = NULL;
    }

    if( gml_lib_dirs.directories != NULL) {
        mem_free( gml_lib_dirs.directories );
        gml_lib_dirs.directories = NULL;
    }

    if( path_dirs.directories != NULL ) {
        mem_free( path_dirs.directories );
        path_dirs.directories = NULL;
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

int search_file_in_dirs( char * filename, char * defext, char * altext,
                          dirseq sequence )
{
    char                buff[_MAX_PATH2];
    char                alternate_file[FILENAME_MAX];
    char                default_file[FILENAME_MAX];
    char                primary_file[FILENAME_MAX];
    char            *   dir_ptr;
    char            *   fn_dir;
    char            *   fn_drive;
    char            *   fn_ext;
    char            *   fn_name;
    char            *   member_name = NULL;
    directory_list  *   list_ptr;
    directory_list  *   searchdirs[4];
    int                 i;
    int                 j;
    size_t              member_length;

    /* Ensure filename will fit into buff. */

    if( strnlen_s( filename, FILENAME_MAX ) == FILENAME_MAX ) {
        out_msg( "File name is too long and will not be searched for:\n%s\n", \
                                                                        filename );
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
            out_msg( "File names cannot contain path information! Filename:" \
                     "\n%s\n", filename );
            err_count++;
            g_suicide();
        }

        /* Ensure the file name will fit in the buffers if the literal extensions
         * are used. Note that all literal extensions contain 4 characters.
         */

        if( *fn_ext == '\0' ) {
            if( strnlen_s( filename, FILENAME_MAX ) + 4 == FILENAME_MAX ) {
            out_msg( "File name is too long and will not be searched for:\n%s\n" \
                     "Note: length used includes a default extension.", filename );
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
        searchdirs[0] = &cur_dir_list;
        searchdirs[1] = &gml_lib_dirs;
        searchdirs[2] = &gml_inc_dirs;
        searchdirs[3] = &path_dirs;
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
        searchdirs[0] = &cur_dir_list;
        searchdirs[1] = &gml_inc_dirs;
        searchdirs[2] = &gml_lib_dirs;
        searchdirs[3] = &path_dirs;
        break;
    case ds_bin_lib:
        searchdirs[0] = &gml_lib_dirs;
        searchdirs[1] = &gml_inc_dirs;
        searchdirs[2] = &path_dirs;
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
        searchdirs[0] = &cur_dir_list;
        searchdirs[1] = &gml_inc_dirs;
        searchdirs[2] = NULL;
        searchdirs[3] = NULL;
        break;
    default:
        out_msg( "findfile internal error\n" );
        err_count++;
        g_suicide();
    }

    /* Search each directory for each filename. */

    for( i = 0; i < 4; i++ ) {
        list_ptr = searchdirs[i];
        if( list_ptr == NULL ) break;

        for( j = 0; j < list_ptr->count; j++ ) {

            dir_ptr = list_ptr->directories[j];

            /* For ds_bin_lib, set primary file from the defined name. */

            if( sequence == ds_bin_lib ) {

            /* See if dir_ptr contains a wgmlst.cop file. */

                if( try_open( dir_ptr, "wgmlst.cop" ) == 0 ) continue;

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
                            out_msg( "Member name is too long and will not be" \
                                        " searched for:\n%s.cop\n", member_name );
                            mem_free( member_name );
                            member_name = NULL;
                            err_count++;
                            g_suicide();
                        }
                    } else {
                        out_msg( "Member name is too long and will not be" \
                                            " searched for:\n%s\n", member_name );
                        mem_free( member_name );
                        member_name = NULL;
                        err_count++;
                        g_suicide();
                    }
                }
            }

            if( try_open( dir_ptr, primary_file ) != 0 ) return( 1 );

            /* Not finding the file is only a problem for ds_bin_lib. */

            if( sequence == ds_bin_lib ) {
                out_msg( "Member file not found in same directory as directory" \
                                        " file:\n%s%s\n", dir_ptr, primary_file );
                err_count++;
                g_suicide();
            }

            if( alternate_file != NULL ) {
                if( try_open( dir_ptr, alternate_file ) != 0 ) return( 1 );
            }

            if( default_file != NULL ) {
                if( try_open( dir_ptr, default_file ) != 0 ) return( 1 );
            }
        }
    }

    return( 0 );
}

