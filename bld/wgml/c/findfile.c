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
*                   wgml_lib_dirs
*                   wgml_inc_dirs
*                   initialize_directory_list()
*                   path_dirs
*                   try_open()
*
* Note:         The Wiki should be consulted for any term whose meaning is
*               not apparent which is related to the binary device library.
*               This should help in most such cases.
*
****************************************************************************/


#include <errno.h>
#include "wgml.h"
#include "copdir.h"
#include "iopath.h"
#include "pathlist.h"
#include "pathgrp2.h"

#include "clibext.h"


typedef char    *directory_list;


char    try_file_name[_MAX_PATH];

/* Local struct. */

/* The count contains the number of entries in directories. */

/* Local data. */

static  directory_list  cur_dir_list;
static  directory_list  wgml_lib_dirs;
static  directory_list  wgml_inc_dirs;
static  directory_list  path_dirs;

/* Local function definitions. */

/* Function initialize_directory_list().
 * Allocates and initializes list from items_list.
 *
 * Parameters:
 *      items_list is a string containing the contents of a path list.
 *
 * Modified Parameter:
 *      none
 *
 * Notes:
 *      it alocates new list with paths and it contains reparsed items_list.
 *      It is normalized that contains only host PATH_LIST_SEP.
     */
static directory_list initialize_directory_list( const char *items_list )
{
    directory_list  list;
    char            *p;

    list = mem_strdup( items_list );
    p = list;
    if( items_list != NULL ) {
        while( *items_list != '\0' ) {
            if( p != list )
                *p++ = PATH_LIST_SEP;
            items_list = GetPathElement( items_list, NULL, &p );
        }
    }
    *p = '\0';
    return( list );
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
 *      If the file is found, try_file_name and fp will be set to the name
 *      as found and FILE * of the file.
 */

static FILE *try_open( char *prefix, char *filename )
{
    FILE        *fp;
    char        buff[_MAX_PATH];
    unsigned    filename_length;

    /* Prevent buffer overflow. */

    filename_length = strlen( prefix ) + strlen( filename ) + 1;
    if( filename_length > _MAX_PATH ) {
        xx_simple_err_exit_cc( ERR_FILE_MAX, prefix, filename );
        /* never return */
    }

    /* Create the full file name to search for. */

    strcpy( buff, prefix );
    strcat( buff, filename );

    /* Clear the global variables used to contain the results. */

    try_file_name[0] = '\0';

    /* Try to open the file. Return 0 on failure. */

    fp = fopen( buff, "rb" );
#if defined( __UNIX__ )
    if( fp == NULL ) {
        strlwr( buff );                 // for the sake of linux try again with lower case filename
        fp = fopen( buff, "rb" );
    }
#else       // DOS, OS/2, Windows
    if( fp != NULL ) {
        strlwr( buff );                 // to match wgml 4.0
    }
#endif

    /* Set the globals on success. */

    if( fp == NULL ) {
        if( errno == ENOMEM ) {
            xx_simple_err_exit( ERR_NO_MEMORY );
                /* never return */
        }
        if( errno == ENFILE
          || errno == EMFILE ) {
            xx_simple_err_exit( ERR_NO_HANDLES );
                /* never return */
        }
    } else {
        strcpy( try_file_name, buff );
    }
    return( fp );
}

/* Extern function definitions. */

/* Function ff_setup().
 * Initializes the directory lists.
 */

void ff_setup( void )
{
    /* Initialize the directory list for current directory. */

    cur_dir_list = initialize_directory_list( "." );

    /* Initialize the directory list for GMLINC. */

    wgml_inc_dirs = initialize_directory_list( getenv( "GMLINC" ) );

    /* Initialize the directory list for GMLLIB. */

    wgml_lib_dirs = initialize_directory_list( getenv( "GMLLIB" ) );

    /* Initialize the directory list for PATH. */

    path_dirs = initialize_directory_list( getenv( "PATH" ) );
}

/* Function ff_set_incpath().
 * The -I command line option overrides the GMLINC path.
 */

void ff_set_incpath( const char *path )
{
    mem_free( wgml_inc_dirs );
    wgml_inc_dirs = initialize_directory_list( path );
}

/* Function ff_set_libpath().
 * The -L command line option overrides the GMLLIB path.
 */

void ff_set_libpath( const char *path )
{
    mem_free( wgml_lib_dirs );
    wgml_lib_dirs = initialize_directory_list( path );
}

/* Function ff_teardown().
 * Releases the memory allocated by functions in this module.
 */

void ff_teardown( void )
{
    try_file_name[0] = '\0';

    /* directories points to a single block of allocated memory. */

    if( cur_dir_list != NULL ) {
        mem_free( cur_dir_list );
        cur_dir_list = NULL;
    }

    if( wgml_inc_dirs != NULL ) {
        mem_free( wgml_inc_dirs );
        wgml_inc_dirs = NULL;
    }

    if( wgml_lib_dirs != NULL) {
        mem_free( wgml_lib_dirs );
        wgml_lib_dirs = NULL;
    }

    if( path_dirs != NULL ) {
        mem_free( path_dirs );
        path_dirs = NULL;
    }
}

/* Function search_file_in_dirs().
 * Searches for filename in curdir and the directories given in the
 * environment variables, as appropriate to the value of sequence.
 *
 * Parameters:
 *      in all cases, dirseq indicates the type of file sought.
 *      if dirseq is DSEQ_bin_lib, then:
 *          filename contains the defined name of a device, driver or font;
 *          defext and altext are ignored.
 *      if dirseq is DSEQ_opt_file, then:
 *          filename contains the name of the file as provided by the user;
 *          defext and altext are ignored.
 *      if dirseq is DSEQ_doc_spec or DSEQ_lib_src, then:
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
 *      to the name as found and FILE * of the file.
 */

FILE *search_file_in_dirs( const char *filename, const char *defext, const char *altext, dirseq sequence )
{
    char            alternate_file[_MAX_PATH];
    char            default_file[_MAX_PATH];
    char            primary_file[_MAX_PATH];
    char            *member_name = NULL;
    directory_list  path_list;
    directory_list  searchdirs[5];
    unsigned        member_length;
    pgroup2         pg;
    directory_list  *pd;
    char            c;
    char            *p;
    char            dir_name[_MAX_PATH];
    FILE            *fp;

    /* Ensure filename will fit into buff. */

    if( strlen( filename ) > _MAX_PATH - 1 ) {
        xx_simple_err_exit_c( ERR_FILE_MAX, filename );
        /* never return */
    }

    /* Initialize the filename buffers. */

    primary_file[0] = '\0';
    alternate_file[0] = '\0';
    default_file[0] = '\0';

    _splitpath2( filename, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );

    /* For DSEQ_bin_lib, filename contains a defined name. */

    if( sequence != DSEQ_bin_lib ) {

        /* Determine if filename contains path information. */

        if( pg.drive[0] != '\0'
          || pg.dir[0] != '\0' ) {
            xx_simple_err_exit_c( ERR_FILE_NAME, filename );
            /* never return */
        }

        /* Ensure the file name will fit in the buffers if the literal extensions
         * are used. Note that all literal extensions contain 4 characters.
         */

        if( pg.ext[0] == '\0' ) {
            if( strlen( filename ) >= _MAX_PATH - 4 ) {
                const char  *ext;

                switch( sequence ) {
                case DSEQ_opt_file:
                    ext = "." OPT_EXT;
                    break;
                case DSEQ_doc_spec:
                    ext = "." GML_EXT;
                    break;
                case DSEQ_bin_lib:
                    ext = "." COP_EXT;
                    break;
                case DSEQ_lib_src:
                    ext = "." PCD_EXT;
                    break;
                default:
                    ext = ".xxx";
                    break;
                }
                xx_simple_err_exit_cc( ERR_FILE_MAX, filename, ext );
                /* never return */
            }
        }
    }

    /* Set up the file names and the dirs for the specified sequence. */

    pd = searchdirs;
    switch( sequence ) {
    case DSEQ_opt_file:
        if( pg.ext[0] == '\0' )
            pg.ext = OPT_EXT;
        _makepath( primary_file, NULL, NULL, pg.fname, pg.ext );
        *pd++ = cur_dir_list;
        *pd++ = wgml_lib_dirs;
        *pd++ = wgml_inc_dirs;
        *pd++ = path_dirs;
        break;
    case DSEQ_doc_spec:
        if( pg.ext[0] == '\0' ) {
            if( altext != NULL
              && altext[0] != '\0' ) {
                _makepath( alternate_file, NULL, NULL, pg.fname, altext );
            }
            if( defext == NULL
              || FNAMECMPSTR( defext, GML_EXT )) {
                _makepath( default_file, NULL, NULL, pg.fname, GML_EXT );
            }
            pg.ext = GML_EXT;
        }
        _makepath( primary_file, NULL, NULL, pg.fname, pg.ext );
        *pd++ = cur_dir_list;
        *pd++ = wgml_inc_dirs;
        *pd++ = wgml_lib_dirs;
        *pd++ = path_dirs;
        break;
    case DSEQ_bin_lib:
        *pd++ = wgml_lib_dirs;
        *pd++ = wgml_inc_dirs;
        *pd++ = path_dirs;
        break;
    case DSEQ_lib_src:
        if( altext == NULL
          || altext[0] == '\0' )
            altext = FON_EXT;
        _makepath( alternate_file, NULL, NULL, pg.fname, altext );
        if( pg.ext[0] == '\0' )
            pg.ext = PCD_EXT;
        _makepath( primary_file, NULL, NULL, pg.fname, pg.ext );
        *pd++ = cur_dir_list;
        *pd++ = wgml_inc_dirs;
        break;
    default:
        internal_err_exit( __FILE__, __LINE__ );
        /* never return */
    }
    /*
     * add terminate NULL pointer as last search directory to the array
     */
    *pd = NULL;

    /* Search each directory for each filename. */

    for( pd = searchdirs; (path_list = *pd) != NULL; pd++ ) {
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

            /* For DSEQ_bin_lib, set primary file from the defined name. */

            if( sequence == DSEQ_bin_lib ) {

                /* See if dir_ptr contains a wgmlst.cop file. */

                member_name = search_member_name( dir_name, filename );
                if( member_name == NULL ) {
                    continue;
                }

                /* Construct primary_file and open it normally. */

                member_length = strlen( member_name );
                pg.ext = NULL;
                if( memchr( member_name, '.', member_length ) == NULL ) {

                    /* Avoid buffer overflow from member_name. */

                    if( member_length >= _MAX_PATH - 4 ) {
                        xx_simple_err_exit_cc( ERR_FILE_MAX, member_name, "." COP_EXT );
                        /* never return */
                    }
                    pg.ext = "COP";
                }
                _makepath( primary_file, NULL, NULL, member_name, pg.ext );
                strlwr( primary_file );
                fp = try_open( dir_name, primary_file );
#ifdef __UNIX__
                if( fp == NULL ) {
                    strupr( primary_file );
                    fp = try_open( dir_name, primary_file );
                }
#endif
                mem_free( member_name );

                /* Not finding the file is only a problem for DSEQ_bin_lib. */

                if( fp == NULL ) {
                    xx_simple_err_exit_cc( ERR_MEM_DIR, dir_name, primary_file );
                    /* never return */
                }
                return( fp );
            }
            fp = try_open( dir_name, primary_file );
            if( fp != NULL ) {
                return( fp );
            }

            if( *alternate_file != '\0' ) {
                fp = try_open( dir_name, alternate_file );
                if( fp != NULL ) {
                    return( fp );
                }
            }

            if( *default_file != '\0' ) {
                fp = try_open( dir_name, default_file );
                if( fp != NULL ) {
                    return( fp );
                }
            }
        }
    }

    return( NULL );
}
