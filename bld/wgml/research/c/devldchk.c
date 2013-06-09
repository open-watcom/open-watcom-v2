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
* Description:  Tests the device library load function for wgml.
*               In addition to main(), these global functions are implemented:
*                   print_banner()
*                   print_usage()
*               and this local function:
*                   display_binary_device_library()
*
* Notes:        The Wiki should be consulted for any term whose meaning is
*               not apparent. This should help in most cases.
*
*               This program uses/tests the production code for loading the
*               binary device library. As such, all structs and field names
*               refer to those in "copfiles.h", not the research code.
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__ 1
#include <process.h>
#include <stdlib.h>
#include <string.h>

#include "wgml.h"
#include "banner.h"
#include "common.h"
#include "findfile.h"
#include "gvars.h"
#include "research.h"

/*  Local variables. */

/*  Load the usage text array. */

static  char const *    usage_text[] = {
"Usage:  devldchk defined-name",
"'defined-name' is the defined name for the device to check",
"'defined-name' must be surrounded by double quotes if it contains spaces",
NULL
};

/* Local function definition. */

/* Function display_binary_device_library().
 * This function loads the binary device library using the same code as wgml,
 * and then displays the device name, the driver name, and information from
 * each of the available fonts.
 */

static void display_binary_device_library( void )
{
    int i;

    out_msg( "Device name: %s\n", dev_name );
    out_msg( "Driver name: %s\n", bin_device->driver_name );

    out_msg( "Available fonts:\n" );
    for( i = 0; i < wgml_font_cnt; i++ ) {
        out_msg( "  Nr: %i\n", i );
        if( wgml_fonts[i].bin_font == NULL ) {
            out_msg( "Font name not found: ERROR!\n" );
        } else {
            out_msg( "  Font name:          %s\n", \
                                        wgml_fonts[i].bin_font->defined_name );
        }
        if( wgml_fonts[1].font_switch == NULL ) {
            out_msg( "Font switch: none\n" );
        } else {
            out_msg( "  Font switch:        %s\n", \
                                                wgml_fonts[i].font_switch->type );
        }
        if( wgml_fonts[i].font_style == NULL ) {
            out_msg( "Font style not found: ERROR!\n" );
        } else {
            out_msg( "  Font style:         %s\n", \
                                                wgml_fonts[i].font_style->type );
        }
        out_msg( "  Font pause present: " );
        if( wgml_fonts[i].font_pause == NULL ) out_msg( "No\n" );
        else out_msg( "Yes\n" );
        out_msg( "  Font resident:      %c\n", wgml_fonts[i].font_resident );
        out_msg( "  Default Width:      %i\n", wgml_fonts[i].default_width );
        out_msg( "  'Em' base:          %i\n", wgml_fonts[i].em_base );
        out_msg( "  Font height:        %i\n", wgml_fonts[i].font_height );
        out_msg( "  Font space:         %i\n", wgml_fonts[i].font_space );
        out_msg( "  Line height:        %i\n", wgml_fonts[i].line_height );
        out_msg( "  Line space:         %i\n", wgml_fonts[i].line_space );
        out_msg( "  Space char width:   %i\n", wgml_fonts[i].spc_width );
        out_msg( "  Font resident:      %c\n", wgml_fonts[i].font_resident );
        if( wgml_fonts[i].shift_count == 0 ) {
            out_msg( "  Shift height:       0\n" );
        } else {
            out_msg( "  Shift height:       %s\n", wgml_fonts[i].shift_height );
        }
    }
}

/* Global function definitions. */

/* Function print_banner().
 * Print the banner to the screen.
 */

void print_banner( void )
{
    puts( banner1w( "Device Load Procedure Test Program", _RESEARCH_VERSION_ ) );
    puts( banner2( "1983" ) );
    puts( banner3 );
    puts( banner3a );
}

/* Function print_usage().
 * Print the usage information to the screen.
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

/* Function main().
 * Given a valid defined name for a device, loads the binary device library
 * and prints out the resulting information for verification. 
 *
 * Returns:
 *      EXIT_FAILURE or EXIT_SUCCESS, as appropriate.
 */

int main()
{
    char    *   cmdline = NULL;
    int         retval;
    jmp_buf     env;
    size_t      cmdlen  = 0;

    /* For compatibility with wgml modules. */

    environment = &env;
    if( setjmp( env ) ) {               // if fatal error has occurred
        my_exit( 16 );
    }

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
    init_global_vars();         // wgml globals

    /* Parse the command line: allocates and sets tgt_path. */

    retval = parse_cmdline( cmdline );
    if( retval == FAILURE ) {
        free( cmdline );
        return( EXIT_FAILURE );
    }

    /* Free the memory held by cmdline and reset it. */

    free( cmdline );
    cmdline = NULL;

    dev_name = tgt_path;
    master_fname = "dvldchk";
    out_file = NULL;
    out_file_attr = NULL;

    /* Special preparations for test01 and test02. */

    if( (!stricmp( dev_name, "test01") ) || (!stricmp( dev_name, "test02") ) ) {
        opt_font * current = mem_alloc( sizeof( opt_font ) );

        current->nxt = NULL;
        current->font = 11;
        current->name = mem_alloc( sizeof( "tfon08" ) + 1 );
        strcpy_s( current->name,  sizeof( "tfon08" ) + 1, "tfon08" );
        current->style = mem_alloc( sizeof( "plain" ) + 1 );
        strcpy_s( current->style, sizeof( "plain" ) + 1, "plain" );
        current->space = 0;
        current->height = 0;

        opt_fonts = current;

        current = mem_alloc( sizeof( opt_font ) );
        opt_fonts->nxt = current;

        current->nxt = NULL;
        current->font = 12;
        current->name = mem_alloc( sizeof( "tfon09" ) + 1 );
        strcpy_s( current->name,  sizeof( "tfon09" ) + 1, "tfon09" );
        current->style = mem_alloc( sizeof( "bold" ) + 1 );
        strcpy_s( current->style, sizeof( "bold" ) + 1, "bold" );
        current->space = 0;
        current->height = 0;
    }

    /* Initialize the binary device library. */

    ff_setup();
    cop_setup();

    /* Print out the data. */

    display_binary_device_library();

    /* Release the memory allocated. */

    cop_teardown();
    ff_teardown();

    mem_free(tgt_path);
    tgt_path = NULL;

    free_some_mem();            // wgml globals
 
    return( EXIT_SUCCESS );
}

