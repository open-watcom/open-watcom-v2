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
* Description:  Checks files in a gendev source directory for
*               unknown device functions.
*               In addition to main(), these global functions are defined:
*                   print_banner()
*                   print_usage()
*               as well as these local functions:
*                   check_directory()
*                   get_dev_func()
*
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__ 1
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "banner.h"
#include "common.h"
#include "lhdirect.h"
#include "research.h"

/* From gtype.h, this is what wgml is using. */

#define BUF_SIZE        512             // default buffersize for filecb e.a.

/* Local structs. */

/* This was suggested by wgml's tag struct, which should be studied when
 * gendev is written for additional fields. For example, flags indicating
 * which functions are allowed in which blocks might be useful. This might
 * also be a reasonable place to store the bytecode for each device function.
 */

typedef struct {
    char        func_name[ 16 ];
} dev_func;

typedef struct {
    char    *   start;
    size_t      count;
} token;

/* This is based on the original version of struct filecb. */

typedef struct file_info {
    FILE                *   fp;             // FILE ptr
    size_t                  buflen;         // length of filebuf
    char                *   filebuf;
    char                *   scanptr;        // ptr into filebuf
    size_t                  usedlen;        // used data of filebuf
} file_info;

/* Local variables. */

static file_info   *   cur_file;

/* These should probably be globals in gendev. */

static token    *   cur_token;

static dev_func     dev_funcs[] = {
    { "add" },
    { "binary1" },
    { "binary2" },
    { "binary4" },
    { "cancel" },
    { "clear3270" },
    { "clearPC" },
    { "date" },
    { "decimal" },
    { "default_width" },
    { "divide" },
    { "endif" },
    { "flushpage" },
    { "font_height" },
    { "font_number" },
    { "font_outname1" },
    { "font_outname2" },
    { "font_resident" },
    { "font_space" },
    { "getnumsymbol" },
    { "getstrsymbol" },
    { "hex" },
    { "ifeqn" },
    { "ifeqs" },
    { "ifnen" },
    { "ifnes" },
    { "image" },
    { "line_height" },
    { "line_space" },
    { "lower" },
    { "page_depth" },
    { "page_width" },
    { "pages" },
    { "recordbreak" },
    { "remainder" },
    { "setsymbol" },
    { "sleep" },
    { "subtract" },
    { "tab_width" },
    { "text" },
    { "thickness" },
    { "time" },
    { "wait" },
    { "wgml_header" },
    { "x_address" },
    { "x_size" },
    { "y_address" },
    { "y_size" },
};

/* Load the usage text array. */

static  char const          *   usage_text[] = {
#include "ffusage.h"
NULL
};

/* Local function definitions. */

/* Function get_dev_func().
 * Determine the length and start position of the next device function name.
 *
 * Static Variables Used:
 *      cur_file is a pointer to the current file_info entry.
 *      cur_token is a pointer to the current file_cur_token.
 *
 * Static Variables modified on success:
 *      cur_file->scanptr is set to the first character after the device
 *          function name which was found.
 *      cur_token->start is set to the first character of the device 
 *          function name which was found.
 *      cur_token->count is set to the number of characters in the device
 *          function name which was found.
 *
 * Returns:
 *      SUCCESS if there is a next device function name.
 *      FAILURE if the file ends or a file error occurs.
 *
 * Notes:
 *      on the first call, file_cbs should be in this minimal state:
 *          cur_file->fp should be the FILE * for the current file.
 *          cur_file->scanptr should be NULL.
 *      FAILURE should be taken as meaning that all device function names have
 *          been found. The value of cur_token is undefined in this case.
 */

static int get_dev_func( void )
{
    char    *   end         = NULL;
    int         i;
    size_t      position;
    size_t      scanned;
    size_t      unscanned;
    
    if( cur_file->scanptr == NULL ) {

        /* First call: start by getting data into the buffer. */

        cur_file->usedlen = fread( cur_file->filebuf, 1, \
                                            cur_file->buflen, cur_file->fp );
        if( ferror( cur_file->fp ) ) return( FAILURE );
        if( cur_file->usedlen == 0 ) return( FAILURE );

        cur_file->scanptr = cur_file->filebuf;
    }

    /* Initialize the output values to clear any old data. */

    cur_token->start = NULL;
    cur_token->count = 0;

    for( ;; ) {        

        /* Find the next device function name. */

        position = cur_file->scanptr - cur_file->filebuf;
        for( i = 0; i < cur_file->usedlen - position; i++ ){
            if( *cur_file->scanptr == '%' ) break;
            cur_file->scanptr++;
        }

        if( *cur_file->scanptr == '%' ) {

            /* cur_file->scanptr points to the start of a device
             * function name.
             */

            cur_token->start = cur_file->scanptr;

            /* Find the length of the device function name. */

            end = cur_token->start;
            position = cur_file->scanptr - cur_file->filebuf;
            for( i = 0; i < cur_file->usedlen - position; i++ ){
                if( *end == '(' ) break;
                end++;
            }

            if( *end == '(' ) {

            /* end points to one position beyond the end of the name. */

                cur_file->scanptr = end;
                cur_token->count = end - cur_token->start;
                break;
            }

            if( feof( cur_file->fp ) ) return( FAILURE );

            /* If we get here, we ran out of data before finding the end of
             * the device function name: reset the buffer to start with the
             * start of the device function name and read more data in.
             */

            scanned = cur_token->start - cur_file->filebuf;
            unscanned = &cur_file->filebuf[cur_file->usedlen] - \
                                                            cur_token->start;
            memmove_s( cur_file->filebuf, cur_file->buflen, \
                                                cur_token->start, unscanned );

            cur_file->usedlen = fread( &cur_file->filebuf[ unscanned ], 1, \
                                cur_file->buflen - unscanned, cur_file->fp );
            cur_file->usedlen += unscanned;
            if( ferror( cur_file->fp ) ) return( FAILURE );
            if( cur_file->usedlen == 0 ) return( FAILURE );

            cur_file->scanptr = cur_file->filebuf;
            continue;
        }

        if( feof( cur_file->fp ) ) return( FAILURE );

        /* If we get here, then we ran out of buffer before finding the start
         * of a device function name: replace the entire buffer.
         */

        cur_file->usedlen = fread( cur_file->filebuf, 1, \
                                            cur_file->buflen, cur_file->fp );
        if( ferror( cur_file->fp ) ) return( FAILURE );
        if( cur_file->usedlen == 0 ) return( FAILURE );

        cur_file->scanptr = cur_file->filebuf;
    }
    
    return( SUCCESS ) ;
}

/* Function check_directory().
 * Checks the directory provided to the program: examines all .PCD files
 * to locate all device functions and identify any not already known to exist.
 *
 * Global Used:
 *      tgt_path contains the directory passed on the command line.
 *
 * Returns:
 *      FAILURE if the directory provided cannot be opened.
 *      SUCCESS if the directory can be opened (and so was checked).
 */

static int check_directory( void )
{
    char                *   extension       = NULL;
    DIR                 *   current_dir     = NULL;
    struct dirent       *   dir_entry       = NULL;
    FILE                *   current_file    = NULL;
    int                     dev_func_cnt    = sizeof( dev_funcs ) / \
                                              sizeof( dev_funcs[ 0 ] );
    int                     i;
    int                     j;
    int                     ret_val;

    ret_val = SUCCESS;
    current_dir = opendir( tgt_path );
    if( current_dir == NULL ) return( FAILURE );
    chdir( tgt_path );
    for(;;) {
        dir_entry = readdir( current_dir );
        if( dir_entry == NULL ) break;

        /* Ensure that the extension is ".PCD". */

        extension = strrchr( dir_entry->d_name, '.' );
        if( extension == NULL) continue;
        if( stricmp( extension + 1, "pcd" ) ) continue;

        /* Open the file. */

        fopen_s( &current_file, dir_entry->d_name, "rb" );
        if( current_file == NULL ) continue;

        /* Process the file. */

        cur_file->fp = current_file;
        cur_file->scanptr = NULL;
        ret_val = SUCCESS;

        while( ret_val != FAILURE ) {

            /* Get the first/next device function name. */
            
            ret_val = get_dev_func();
            if( ret_val == FAILURE ) break;

            /* Trim the device function name. */

            cur_token->start++;
            cur_token->count--;

            /* Search the table. */

            for( i = 0; i < dev_func_cnt; i++ ) {
                if( !memicmp( dev_funcs[i].func_name, cur_token->start, \
                                                    cur_token->count ) ) break;

            }

            if( i == dev_func_cnt ) {

                /* Print the device function name if not found. */

                cur_token->start--;
                cur_token->count++;

                for( j = 0; j < cur_token->count; j++ ) {
                    putchar( cur_token->start[j] );
                }
                puts( "" );
            }
        }

        fclose( current_file );
        current_file = NULL;
    }
    closedir( current_dir );
   
    return( SUCCESS );
}

/*  Function print_banner().
 *  Print the banner to the screen.
 */

void print_banner( void )
{
    puts( banner1w( "Device Function Research Program", _RESEARCH_VERSION_ ) );
    puts( banner2( "1983" ) );
    puts( banner3 );
    puts( banner3a );
}

/*  Function print_usage().
 *  Print the usage information to the screen.
 */

void print_usage( void )
{
    char const * *  list;

    list = usage_text;
    while( *list ) {
        puts( *list );
        ++list;
    }
}

/*  Function main().
 *  Given a valid directory path, check all .PCD files in that directory
 *  for device functions not already known to exist.
 *  The actual checks are performed in the function check_directory(); main()
 *  is concerned with overall program architecture, not details.
 *
 *  Returns:
 *      EXIT_FAILURE or EXIT_SUCCESS, as appropriate.
 */

int main()
{
    /* Declare automatic variables. */

    size_t  cmdlen  = 0;
    char *  cmdline = NULL;
    int     retval;

    /* Display the banner. */

    print_banner();

    /* Display the usage information if the command line is empty. */

    cmdlen = _bgetcmd( NULL, 0 );
    if( cmdlen == 0 ) {
        print_usage();
        return( EXIT_FAILURE );
    }

    /* Include space for the terminating null character. */

    cmdlen++;

    /* Get the command line. */

    cmdline = malloc( cmdlen );
    if( cmdline == NULL ) {
        return( EXIT_FAILURE );
    }

    cmdlen = _bgetcmd( cmdline, cmdlen );

    /* Initialize the globals. */

    initialize_globals();
    res_initialize_globals();

    /* Initialize the statics, which should be globals in gendev. */

    cur_file = (file_info *) malloc( sizeof( file_info ) );
    cur_file->filebuf = (char * ) malloc( BUF_SIZE );
    cur_file->buflen = BUF_SIZE;
    cur_file->scanptr = NULL;
    cur_file->usedlen = 0;

    cur_token = (token *) malloc( sizeof( token ) );

    /* Parse the command line: allocates and sets tgt_path. */

    retval = parse_cmdline( cmdline );
    if( retval == FAILURE ) {
        free( cmdline );
        return( EXIT_FAILURE );
    }

    /* Free the memory held by cmdline and reset it. */

    free( cmdline );
    cmdline = NULL;

    /* Check all files in current directory. */

    retval = check_directory();

    /* Free the memory held by tgt_path and the statics. */

    free( tgt_path );
    tgt_path = NULL;
    free( cur_file->filebuf );
    cur_file->filebuf = NULL;
    free( cur_file );
    cur_file = NULL;
    free( cur_token );
    cur_token = NULL;

    /* Print the useage if the process failed. */

    if( retval == FAILURE ) {
      print_usage();
      return( EXIT_FAILURE );
    }
    
    return( EXIT_SUCCESS );
}

