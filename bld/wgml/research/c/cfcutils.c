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
* Description:  Implements the utility functions for cfcheck:
*                   check_directory()
*                   print_banner()
*                   print_usage()
*
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__ 1

#include <stdio.h>
#if defined( __UNIX__ ) || defined( __WATCOMC__ )
#include <unistd.h>
#else
#include <io.h>
#endif
#if defined( __UNIX__ )
#include <dirent.h>
#else
#include <direct.h>
#endif
#include "clibext.h"

#include "banner.h"
#include "cfheader.h"
#include "common.h"
#include "research.h"

/* Local variables */
/* Load the usage text array */

static  char const *    usage_text[] = {
#include "cfcusage.h"
NULL
};

/*  Function check_directory().
 *  Perform the check of the directory provided to the program.
 *  Only files, not subdirectories, are checked.
 *  The length of all files is checked to see if it is a multiple of 16.
 *  Function parse_header() is used to process the header of each file.
 *  The number of files of types 0x02, 0x03 and 0x04 is displayed.
 *  Any file types other than 0x02, 0x03 and 0x04 are displayed.
 *
 *  Global Used:
 *      tgt_path contains the directory passed on the command line.
 *
 *  Returns:
 *      FAILURE if the directory provided cannot be opened.
 *      SUCCESS if the directory can be opened (and so was checked).
 */

int check_directory( void )
{
    DIR  *              current_dir     = NULL;
    struct  dirent  *   dir_entry       = NULL;
    FILE *              current_file    = NULL;
    int                 datafile        = 0; /* counts files of type 03 */
    int                 v3directoryfile = 0; /* counts files of type 02 */
    int                 v4directoryfile = 0; /* counts files of type 04 */
    int                 retval;
    char                type;

    current_dir = opendir( tgt_path );
    if( current_dir == NULL ) return( FAILURE );

    chdir( tgt_path );

    for(;;) {

        dir_entry = readdir( current_dir );
        if( dir_entry == NULL ) break;

        /* Open the file. */

        fopen_s( &current_file, dir_entry->d_name, "rb" );
        if( current_file == NULL ) {
            continue;
        }

        /* Check the file size. */

        if( (filelength( fileno( current_file ) ) % 16) != 0)
            printf_s( "Size of file %s is not a multiple of 16\n", dir_entry->d_name );

        /* Process the file. */

        retval = parse_header( current_file, &type );
        if(retval == FAILURE)
        {
            printf_s( "%s is not a valid .COP file\n", dir_entry->d_name );
            fclose( current_file );
            current_file = NULL;
            continue;
        }
        switch( type ) {
        case( 0x02 ): 
            v3directoryfile++;
            break;
        case( 0x03 ):
            datafile++;
            break;
        case( 0x04 ): 
            v4directoryfile++;
            break;
        default:
            printf_s( "%s: unknown file type: %i\n", dir_entry->d_name, type );
        }
        fclose( current_file );
        current_file = NULL;
    }
    closedir( current_dir );
   
    /* Output counts. */

    printf_s( "Number of device/driver/font files:  %i\n", datafile );
    printf_s( "Number of version 3 directory files: %i\n", v3directoryfile );
    printf_s( "Number of version 4 directory files: %i\n", v4directoryfile );

    return( SUCCESS );
}

/*  Function print_banner().
 *  Print the banner to the screen.
 */

void print_banner( void )
{
    puts( banner1w( "Script/GML Binary File Type Check Program", _RESEARCH_VERSION_ ) );
    puts( banner2 );
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

