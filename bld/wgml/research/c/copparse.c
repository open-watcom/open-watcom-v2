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
* Description:  Parses .COP files based on the defined name.
*               In addition to main(), these global functions are implemented:
*                   print_banner()
*                   print_usage()
*               as well as these local functions:
*                   cop_initialize_globals()
*                   display_device()
*                   display_driver()
*                   display_font()
*                   get_cop_device()
*                   get_cop_driver()
*                   get_cop_font()
*                   parse_defined_name()
*
* Notes:        The Wiki should be consulted for any term whose meaning is
*               not apparent. This should help in most cases.
*
*               This program uses/tests the production code for parsing the
*               binary device library. As such, all structs and field names
*               refer to those in "copfiles.h", not the research code.
*
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__ 1
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "bool.h"

#include "wgml.h"
#include "banner.h"
#include "common.h"
#include "copdev.h"
#include "copdrv.h"
#include "copfon.h"
#include "cophdr.h"
#include "dfinterp.h"
#include "findfile.h"
#include "gvars.h"
#include "research.h"

/*  Local variables. */

/*  Load the usage text array. */

static  char const *    usage_text[] = {
#include "copusage.h"
NULL
};

/* Local function definitions. */

/* Function display_device().
 * Displays the contents of a cop_device instance.
 *
 * Parameter:
 *      in_device is a pointer to the cop_device instance.
 */

static void display_device( cop_device * in_device )
{
    char        font_character[2];
    int         i;
    int         j;
    char        translation[2];

    printf_s( "Allocated size:            %i\n", in_device->allocated_size );
    printf_s( "Bytes used:                %i\n", in_device->next_offset );
    if( in_device->driver_name == NULL ) puts( "Driver Name:");
    else printf_s( "Driver Name:               %s\n", in_device->driver_name );
    if( in_device->output_name == NULL ) puts( "Output File Name:" );
    else printf_s( "Output File Name:          %s\n", in_device->output_name );
    if( in_device->output_extension == NULL ) puts( "Output File Extension:" );
    else printf_s( "Output File Extension:     %s\n", in_device->output_extension );
    printf_s( "Page Width:                %i\n", in_device->page_width );
    printf_s( "Page Depth:                %i\n", in_device->page_depth );
    printf_s( "Horizontal Base Units:     %i\n", in_device->horizontal_base_units );
    printf_s( "Vertical Base Units:       %i\n", in_device->vertical_base_units );
    printf_s( "Page Start X Value:        %i\n", in_device->x_start );
    printf_s( "Page Start Y Value:        %i\n", in_device->y_start );
    printf_s( "Page Offset X Value:       %i\n", in_device->x_offset );
    printf_s( "Page Offset Y Value:       %i\n", in_device->y_offset );
    if( in_device->box.font_name == NULL )
        printf_s( "Box Font Number:           %i\n", in_device->box.font);
    else
        printf_s( "Box Font Name:             %s\n", in_device->box.font_name );
    printf_s( "Horizontal line character: %c\n", in_device->box.horizontal_line );
    printf_s( "Vertical line character:   %c\n", in_device->box.vertical_line );
    printf_s( "Top left character:        %c\n", in_device->box.top_left );
    printf_s( "Top right character:       %c\n", in_device->box.top_right );
    printf_s( "Bottom left character:     %c\n", in_device->box.bottom_left );
    printf_s( "Bottom right character:    %c\n", in_device->box.bottom_right );
    printf_s( "Top join character:        %c\n", in_device->box.top_join );
    printf_s( "Bottom join character:     %c\n", in_device->box.bottom_join );
    printf_s( "Left join character:       %c\n", in_device->box.left_join );
    printf_s( "Right join character:      %c\n", in_device->box.right_join );
    printf_s( "Inside join character:     %c\n", in_device->box.inside_join );
    if( in_device->underscore.specified_font == false )
        puts( "Underscore Font Not Specified");
    else if( in_device->underscore.font_name == NULL )
        printf_s( "Underscore Font Number:    %i\n", in_device->underscore.font);
    else
        printf_s( "Underscore Font Name:      %s\n", in_device->underscore.font_name );
    printf_s( "Underscore character:      %c\n", in_device->underscore.underscore_char );
    if( in_device->intrans == NULL) {
        puts( "No Intrans Table");
    } else {
        puts( "Intrans Table:" );
        for( i = 0; i < 0x100; i++ ) {
            if( in_device->intrans->table[i] != i ) {
                display_char( font_character, (char) i );
                display_char( translation, in_device->intrans->table[i] );
                printf_s( "%c%c %c%c\n", font_character[0], font_character[1], translation[0], translation[1] );
            }
        }
    }
    if( in_device->outtrans == NULL) {
        puts( "No Outtrans Table");
    } else {
        puts( "Outtrans Table:" );
        for( i = 0; i < 0x100; i++ ) {
            if( in_device->outtrans->table[i] != NULL ) {
                display_char( font_character, (char) i );
                printf_s( "%c%c ", font_character[0], font_character[1] );
                for( j = 0; j < in_device->outtrans->table[i]->count; j++ ) {
                    display_char( translation, in_device->outtrans->table[i]->data[j] );
                    printf_s( "%c%c ", translation[0], translation[1] );
                }
                puts( "" );
            }
        }
    }
    printf_s( "Number of Default Fonts: %i\n", in_device->defaultfonts.font_count );
    for( i = 0; i < in_device->defaultfonts.font_count; i++ ) {
        printf_s( "  Default Font Number  %i:\n", i );
        if( in_device->defaultfonts.fonts[i].font_name == NULL )
            puts( "    Font Name:");
        else
            printf_s( "    Font Name:         %s\n", in_device->defaultfonts.fonts[i].font_name );
        if( in_device->defaultfonts.fonts[i].font_style == NULL )
            puts( "    FontStyle:");
        else
            printf_s( "    Font Style:        %s\n", in_device->defaultfonts.fonts[i].font_style );
        printf_s( "    Font Height:       %i\n", in_device->defaultfonts.fonts[i].font_height );
        printf_s( "    Font Space:        %i\n", in_device->defaultfonts.fonts[i].font_space );
    }
    if( in_device->pauses.start_pause == NULL ) puts( "No START Pause" );
    else {
        puts( "START Pause:" );
        interpret_function( in_device->pauses.start_pause->text );
    }
    if( in_device->pauses.document_pause == NULL ) puts( "No DOCUMENT Pause" );
    else {
        puts( "DOCUMENT Pause:" );
        interpret_function( in_device->pauses.document_pause->text );
    }
    if( in_device->pauses.docpage_pause == NULL )
        puts( "No DOCUMENT_PAGE Pause" );
    else {
        puts( "DOCUMENT_PAGE Pause:" );
        interpret_function( in_device->pauses.docpage_pause->text );
    }
    if( in_device->pauses.devpage_pause == NULL )
        puts( "No DEVICE_PAGE Pause" );
    else {
        puts( "DEVICE_PAGE Pause:" );
        interpret_function( in_device->pauses.devpage_pause->text );
    }
    printf_s( "Number of Device Fonts: %i\n", in_device->devicefonts.font_count );
    for( i = 0; i < in_device->devicefonts.font_count; i++ ) {
        printf_s( "  Device Font Index:   %i:\n", i );
        printf_s( "    Font Name:         %s\n", in_device->devicefonts.fonts[i].font_name );
        if( in_device->devicefonts.fonts[i].font_switch == NULL )
            puts( "    Font Switch:");
        else
            printf_s( "    Font Switch:       %s\n", in_device->devicefonts.fonts[i].font_switch );
        printf_s( "    Resident Font:     %i\n", in_device->devicefonts.fonts[i].resident );
        if( in_device->devicefonts.fonts[i].font_pause == NULL )
            puts( "    No Font Pause" );
        else {
            puts( "    Font Pause:" );
            interpret_function( in_device->devicefonts.fonts[i].font_pause->text );
        }
    }

    return;
}

/*  Function display_driver().
 *  Displays the contents of a cop_driver instance.
 *
 *  Parameter:
 *      in_driver is a pointer to the cop_driver instance.
 */

static void display_driver( cop_driver * in_driver )
{
    int i;
    int j;
    
    printf_s( "Allocated size:            %i\n", in_driver->allocated_size );
    printf_s( "Bytes used:                %i\n", in_driver->next_offset );
    if( in_driver->rec_spec == NULL ) puts( "Record Specification:");
    else printf_s( "Record Specification:      %s\n", in_driver->rec_spec );
    printf_s( "Fill Character:            %c\n", in_driver->fill_char );
    puts( "Page Address Flags:");
    printf_s( "  x_positive:              %i\n", in_driver->x_positive );
    printf_s( "  y_positive:              %i\n", in_driver->y_positive );
    puts( "Start :INIT Block:" );
    if( in_driver->inits.start != NULL ) {
        for( i = 0; i < in_driver->inits.start->count; i++ ) {
            if( in_driver->inits.start->codeblock[i].is_fontvalue )
                puts( ":FONTVALUE Block:");
            else
                puts( ":VALUE Block:");
            interpret_function( in_driver->inits.start->codeblock[i].text );
        }
    }
    puts( "Document :INIT Block:" );
    if( in_driver->inits.document != NULL ) {
        for( i = 0; i < in_driver->inits.document->count; i++ ) {
            if( in_driver->inits.document->codeblock[i].is_fontvalue )
                puts( ":FONTVALUE Block:");
            else
                puts( ":VALUE Block:");
            interpret_function( in_driver->inits.document->codeblock[i].text );
        }
    }
    puts( "End :FINISH Block:" );
    if( in_driver->finishes.end != NULL ) {
        interpret_function( in_driver->finishes.end->text );
    }
    puts( "Document :FINISH Block:" );
    if( in_driver->finishes.document != NULL ) {
        interpret_function( in_driver->finishes.document->text );
    }
    if( in_driver->newlines.newlineblocks == NULL ) puts( ":NEWLINE Block:");
    else {
        puts( ":NEWLINE Block(s):" );
        for( i = 0; i < in_driver->newlines.count; i++ ) {
            printf_s( "  Advance: %i\n", in_driver->newlines.newlineblocks[i].advance );
            if( in_driver->newlines.newlineblocks[i].text != NULL ) {
                interpret_function( in_driver->newlines.newlineblocks[i].text );
            }
        }
    }
    puts( ":NEWPAGE Block:" );
    if( in_driver->newpage.text != NULL ) {
        interpret_function( in_driver->newpage.text );
    }
    puts( ":HTAB Block:" );
    if( in_driver->htab.text != NULL ) {
        interpret_function( in_driver->htab.text );
    }
    if( in_driver->fontswitches.fontswitchblocks == NULL )
        puts( ":FONTSWITCH Block:");
    else {
        puts( ":FONTSWITCH Block(s):" );
        for( i = 0; i < in_driver->fontswitches.count; i++ ) {
            if( in_driver->fontswitches.fontswitchblocks[i].type == NULL )
                puts( "  Type:");
            else
                printf_s( "  Type: %s\n", in_driver->fontswitches.fontswitchblocks[i].type );
            printf_s( "  do_always: %x\n", in_driver->fontswitches.fontswitchblocks[i].do_always );
            printf_s( "  default_width_flag: %x\n", in_driver->fontswitches.fontswitchblocks[i].default_width_flag );
            printf_s( "  font_height_flag: %x\n", in_driver->fontswitches.fontswitchblocks[i].font_height_flag );
            printf_s( "  font_outname1_flag: %x\n", in_driver->fontswitches.fontswitchblocks[i].font_outname1_flag );
            printf_s( "  font_outname2_flag: %x\n", in_driver->fontswitches.fontswitchblocks[i].font_outname2_flag );
            printf_s( "  font_resident_flag: %x\n", in_driver->fontswitches.fontswitchblocks[i].font_resident_flag );
            printf_s( "  font_space_flag: %x\n", in_driver->fontswitches.fontswitchblocks[i].font_space_flag );
            printf_s( "  line_height_flag: %x\n", in_driver->fontswitches.fontswitchblocks[i].line_height_flag );
            printf_s( "  line_space_flag: %x\n", in_driver->fontswitches.fontswitchblocks[i].line_space_flag );
            if( in_driver->fontswitches.fontswitchblocks[i].startvalue != NULL ) {
                puts( "  :STARTVALUE Block:");
                interpret_function( in_driver->fontswitches.fontswitchblocks[i].startvalue->text );
            }
            if( in_driver->fontswitches.fontswitchblocks[i].endvalue != NULL ) {
                puts( "  :ENDVALUE Block:");
                interpret_function( in_driver->fontswitches.fontswitchblocks[i].endvalue->text );
            }
        }
    }
    if( in_driver->fontstyles.fontstyleblocks == NULL ) puts( ":FONTSTYLE Block:");
    else {
        puts( ":FONTSTYLE Block(s):" );    
        for( i = 0; i < in_driver->fontstyles.count; i++ ) {
            if( in_driver->fontstyles.fontstyleblocks[i].type == NULL )
                puts( "  Type:");
            else
                printf_s( "  Type: %s\n", in_driver->fontstyles.fontstyleblocks[i].type );
            if( in_driver->fontstyles.fontstyleblocks[i].startvalue == NULL ) {
                puts( "  No :STARTVALUE Block");
            } else {
                puts( "  :STARTVALUE Block:");
                interpret_function( in_driver->fontstyles.fontstyleblocks[i].startvalue->text );
            }
            if( in_driver->fontstyles.fontstyleblocks[i].endvalue == NULL ) {
                puts( "  No :ENDVALUE Block");
            } else {
                puts( "  :ENDVALUE Block:");
                interpret_function( in_driver->fontstyles.fontstyleblocks[i].endvalue->text );
            }
            if(in_driver->fontstyles.fontstyleblocks[i].lineprocs == NULL ) {
                puts( "  No :LINEPROC Blocks");
            } else {
                puts( "  :LINEPROC Block(s):");
                for( j = 0; j < in_driver->fontstyles.fontstyleblocks[i].line_passes; j++ ) { 
                    printf_s( "  Pass: %i\n", j+1 );
                    if( in_driver->fontstyles.fontstyleblocks[i].lineprocs[j].startvalue == NULL ) {
                        puts( "  No :STARTVALUE Block");
                    } else {
                        puts( "  :STARTVALUE Block:");
                        interpret_function( in_driver->fontstyles.fontstyleblocks[i].lineprocs[j].startvalue->text );
                    }
                    if( in_driver->fontstyles.fontstyleblocks[i].lineprocs[j].firstword == NULL ) {
                        puts( "  No :FIRSTWORD Block");
                    } else {
                        puts( "  :FIRSTWORD Block:");
                        interpret_function( in_driver->fontstyles.fontstyleblocks[i].lineprocs[j].firstword->text );
                    }
                    if( in_driver->fontstyles.fontstyleblocks[i].lineprocs[j].startword == NULL ) {
                        puts( "  No :STARTWORD Block");
                    } else {
                        puts( "  :STARTWORD Block");
                        interpret_function( in_driver->fontstyles.fontstyleblocks[i].lineprocs[j].startword->text );
                    }
                    if( in_driver->fontstyles.fontstyleblocks[i].lineprocs[j].endword == NULL ) {
                        puts( "  No :ENDWORD Block");
                    } else {
                        puts( "  :ENDWORD Block:");
                        interpret_function( in_driver->fontstyles.fontstyleblocks[i].lineprocs[j].endword->text );
                    }
                    if( in_driver->fontstyles.fontstyleblocks[i].lineprocs[j].endvalue == NULL ) {
                        puts( "  No :ENDVALUE Block");
                    } else {
                        puts( "  :ENDVALUE Block:");
                        interpret_function( in_driver->fontstyles.fontstyleblocks[i].lineprocs[j].endvalue->text );
                    }
                }
            }
        }
    }
    if( in_driver->absoluteaddress.text == NULL ) {
        puts( "No :ABSOLUTEADDRESS Block" );
    } else {
    puts( ":ABSOLUTEADDRESS Block:" );
        interpret_function( in_driver->absoluteaddress.text );
    }
    if( in_driver->hline.text == NULL ) {
        puts( "No :HLINE Block" );
    } else {
    puts( ":HLINE Block:" );
        printf_s( "  Thickness:               %i\n", in_driver->hline.thickness );
        interpret_function( in_driver->hline.text );
    }
    if( in_driver->vline.text == NULL ) {
        puts( "No :VLINE Block" );
    } else {
    puts( ":VLINE Block:" );
        printf_s( "  Thickness:               %i\n", in_driver->vline.thickness );
        interpret_function( in_driver->vline.text );
    }
    if( in_driver->dbox.text == NULL ) {
        puts( "No :DBOX Block" );
    } else {
    puts( ":DBOX Block:" );
        printf_s( "  Thickness:               %i\n", in_driver->dbox.thickness );
        interpret_function( in_driver->dbox.text );
    }

    return;
}

/*  Function display_font().
 *  Displays the contents of a cop_font instance.
 *
 *  Parameter:
 *      in_font is a pointer to the cop_font instance.
 */

static void display_font( cop_font * in_font )
{

    char        font_character[2];
    int         i;
    int         j;
    char        translation[2];

    printf_s( "Allocated size:            %i\n", in_font->allocated_size );
    printf_s( "Bytes used:                %i\n", in_font->next_offset );
    if( in_font->font_out_name1 == NULL ) puts( "Font Output Name 1:");
    else printf_s( "Font Output Name 1:        %s\n", in_font->font_out_name1 );
    if( in_font->font_out_name2 == NULL ) puts( "Font Output Name 2:" );
    else printf_s( "Font Output Name 2:        %s\n", in_font->font_out_name2 );
    printf_s( "Line Height:               %i\n", in_font->line_height );
    printf_s( "Line Space:                %i\n", in_font->line_space );
    printf_s( "Scale Basis:               %i\n", in_font->scale_basis );
    printf_s( "Scale Minimum:             %i\n", in_font->scale_min );
    printf_s( "Scale Maximum:             %i\n", in_font->scale_max );
    printf_s( "Character Width:           %i\n", in_font->char_width );
    if( in_font->intrans == NULL) {
        puts( "No Intrans Table");
    } else {
        puts( "Intrans Table:" );
        for( i = 0; i < 0x100; i++ ) {
            if( in_font->intrans->table[i] != i ) {
                display_char( font_character, (char) i );
                display_char( translation, in_font->intrans->table[i] );
                printf_s( "%c%c %c%c\n", font_character[0], font_character[1], translation[0], translation[1] );
            }
        }
    }
    if( in_font->outtrans == NULL) {
        puts( "No Outtrans Table");
    } else {
        puts( "Outtrans Table:" );
        for( i = 0; i < 0x100; i++ ) {
            if( in_font->outtrans->table[i] != NULL ) {
                display_char( font_character, (char) i );
                printf_s( "%c%c ", font_character[0], font_character[1] );
                for( j = 0; j < in_font->outtrans->table[i]->count; j++ ) {
                    display_char( translation, in_font->outtrans->table[i]->data[j] );
                    printf_s( "%c%c ", translation[0], translation[1] );
                }
                puts( "" );
            }
        }
    }
    if( in_font->width == NULL) {
        puts( "No Width Table");
    } else {
        puts( "Width Table:" );
        for( i = 0; i < 0x100; i++ ) {
            if( in_font->width->table[i] != in_font->char_width ) {
                display_char( font_character, (char) i );
                printf_s( "%c%c %lu\n", font_character[0], font_character[1], in_font->width->table[i] );
            }
        }
    }

    return;
}

/* Function get_cop_device().
 * Converts the defined name of a :DEVICE block into a cop_device struct
 * containing the information in that :DEVICE block.
 *
 * Parameter:
 *      in_name points to the defined name of the device.
 *
 * Globals Used:
 *      try_file_name contains the name of the device file, if found.
 *      try_fp contains the FILE * for the device file, if found.
 *
 * Return:
 *      on success, a cop_device instance containing the data.
 *      on failure, a NULL pointer.
 */

static cop_device * get_cop_device( char const * in_name )
{
    cop_device      *   out_device  = NULL;
    cop_file_type       file_type;
#if defined( __UNIX__ )
    char                fname[_MAX_PATH];

    strcpy( fname, in_name );
    strlwr( fname );
    in_name = fname;
#endif

    /* Acquire the file, if it exists. */

    if( !search_file_in_dirs( in_name, "", "", ds_bin_lib ) ) {
        return( out_device );
    }

    /* Determine if the file encodes a :DEVICE block. */
    
    file_type = parse_header( try_fp );

    switch( file_type ) {
    case file_error:

        /* File error, including premature eof. */

        out_msg( "ERR_FILE_IO %d %s\n", errno, try_file_name );
        err_count++;
        g_suicide();

    case not_bin_dev:
    case not_se_v4_1:
    case dir_v4_1_se:
    
        /* Wrong type of file: something is wrong with the device library. */

        out_msg( "Device library corrupt or wrong version: %s\n", try_file_name );
        return( out_device );

    case se_v4_1_not_dir:

        /* try_fp was a same-endian version 4.1 file, but not a directory file. */

        if( !is_dev_file( try_fp ) ) {
            out_msg( "Device library problem: file given for device %s does not encode a device:\n  %s\n", in_name, try_file_name );
            break;
        }

        out_device = parse_device( try_fp );
        if( out_device == NULL )
            out_msg( "Device library problem: file given for device %s appears to be corrupted:\n  %s\n", in_name, try_file_name );
        break;

    default:

        /* parse_header() returned an unknown value. */

        out_msg("wgml internal error\n");
        err_count++;
        g_suicide();
    }

    return( out_device );
}

/* Function get_cop_driver().
 * Converts the defined name of a :DRIVER block into a cop_driver struct
 * containing the information in that :DRIVER block.
 *
 * Parameter:
 *      in_name points to the defined name of the device.
 *
 * Returns:
 *      on success, a cop_driver instance containing the data.
 *      on failure, a NULL pointer.
 */

static cop_driver * get_cop_driver( char const * in_name )
{
    cop_driver      *   out_driver  = NULL;
    cop_file_type       file_type;
#if defined( __UNIX__ )
    char                fname[_MAX_PATH];

    strcpy( fname, in_name );
    strlwr( fname );
    in_name = fname;
#endif

    /* Acquire the file, if it exists. */

    if( !search_file_in_dirs( in_name, "", "", ds_bin_lib ) ) {
        return( out_driver );
    }

    /* Determine if the file encodes a :DRIVER block. */
    
    file_type = parse_header( try_fp );

    switch( file_type ) {
    case file_error:

        /* File error, including premature eof. */

        out_msg( "ERR_FILE_IO %d %s\n", errno, try_file_name );
        err_count++;
        g_suicide();

    case not_bin_dev:
    case not_se_v4_1:
    case dir_v4_1_se:
    
        /* Wrong type of file: something is wrong with the device library. */

        out_msg( "Device library corrupt or wrong version: %s\n", try_file_name );
        return( out_driver );

    case se_v4_1_not_dir:

        /* try_fp was a same-endian version 4.1 file, but not a directory file. */

        if( !is_drv_file( try_fp ) ) {
            out_msg( "Device library problem: file given for driver %s does not encode a driver:\n  %s\n", in_name, try_file_name );
            break;
        }

        out_driver = parse_driver( try_fp );
        if( out_driver == NULL )
            out_msg( "Device library problem: file given for driver %s appears to be corrupted:\n  %s\n", in_name, try_file_name );
        break;

    default:

        /* parse_header() returned an unknown value. */

        out_msg("wgml internal error\n");
        err_count++;
        g_suicide();
    }

    return( out_driver );
}

/* Function get_cop_font().
 * Converts the defined name of a :FONT block into a cop_font struct
 * containing the information in that :FONT block.
 *
 * Parameter:
 *      in_name points to the defined name of the font.
 *
 * Returns:
 *      on success, a cop_font instance containing the data.
 *      on failure, a NULL pointer.
 */

static cop_font * get_cop_font( char const * in_name )
{
    cop_font        *   out_font    = NULL;
    cop_file_type       file_type;
#if defined( __UNIX__ )
    char                fname[_MAX_PATH];

    strcpy( fname, in_name );
    strlwr( fname );
    in_name = fname;
#endif

    /* Acquire the file, if it exists. */

    if( !search_file_in_dirs( in_name, "", "", ds_bin_lib ) ) {
        return( out_font );
    }

    /* Determine if the file encodes a :FONT block. */
    
    file_type = parse_header( try_fp );

    switch( file_type ) {
    case file_error:

        /* File error, including premature eof. */

        out_msg( "ERR_FILE_IO %d %s\n", errno, try_file_name );
        err_count++;
        g_suicide();

    case not_bin_dev:
    case not_se_v4_1:
    case dir_v4_1_se:
    
        /* Wrong type of file: something is wrong with the device library. */

        out_msg( "Device library corrupt or wrong version: %s\n", try_file_name );
        return( out_font );

    case se_v4_1_not_dir:

        /* try_fp was a same-endian version 4.1 file, but not a directory file. */

        if( !is_fon_file( try_fp ) ) {
            out_msg( "Device library problem: file given for font %s does not encode a font:\n  %s\n", in_name, try_file_name );
            break;
        }

        out_font = parse_font( try_fp, in_name );
        if( out_font == NULL )
            out_msg( "Device library problem: file given for font %s appears to be corrupted:\n  %s\n", in_name, try_file_name );
        break;

    default:

        /* parse_header() returned an unknown value. */

        out_msg("wgml internal error\n");
        err_count++;
        g_suicide();
    }

    return( out_font );
}

/* Function parse_defined_name().
 * Parses the file corresponding to the defined name provided on the command
 * line, if any, to include displaying its contents.
 *
 * The actual parsing is done using the functions declared in copfiles.h.
 * This function is, in fact, the test function for those functions.
 *
 * Note: since mem_alloc() is used to allocate these structs, mem_free()
 *      must be used to free them. 
 *
 * Global Used:
 *      tgt_path contains the defined name passed on the command line.
 *
 * Returns:
 *      SUCCESS if the defined name was parsed without error.
 *      FAILURE if the any error occurred.
 */

static int parse_defined_name( void )
{
    cop_device *    current_device  = NULL;
    cop_driver *    current_driver  = NULL;
    cop_font   *    current_font    = NULL;

    printf_s( "Defined Name: %s\n", tgt_path );

    current_device = get_cop_device( tgt_path );
    if( current_device != NULL) {
        display_device( current_device );
        mem_free( current_device );
        return( SUCCESS );
    }

    current_driver = get_cop_driver( tgt_path );
    if( current_driver != NULL) {
        display_driver( current_driver );
        mem_free( current_driver );
        return( SUCCESS );
    }

    current_font = get_cop_font( tgt_path );
    if( current_font != NULL) {
        display_font( current_font );
        mem_free( current_font );
        return( SUCCESS );
    }

    return( FAILURE );
}

/* Function print_banner().
 * Print the banner to the screen.
 */

void print_banner( void )
{
    puts( banner1w( "Script/GML Defined Name Parser Program", _RESEARCH_VERSION_ ) );
    puts( banner2 );
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
 * Given a valid defined name, verify that it is a valid .COP file and parse 
 * it if it is.
 *
 * The actual parsing is performed in the function parse_defined_name(); main()
 * is concerned with overall program architecture, not details.
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
    init_global_vars();         // wgml globals
    res_initialize_globals();
    ff_setup();

    /* Parse the command line: allocates and sets tgt_path. */

    retval = parse_cmdline( cmdline );
    if( retval == FAILURE ) {
        free( cmdline );
        return( EXIT_FAILURE );
    }

    /* Free the memory held by cmdline and reset it. */

    free( cmdline );
    cmdline = NULL;

    /* Adjust tgt_path if necessary; see the Wiki. */

    if( !strcmp( tgt_path, "''" ) ) tgt_path[0] = '\0';

    /* Parse the alleged .COP file. */

    retval = parse_defined_name();

    /* Free tgt_path. */

    mem_free( tgt_path );
    tgt_path = NULL;

    ff_teardown();

    /* Respond to failure. */

    if( retval == FAILURE ) {
      print_usage();
      return( EXIT_FAILURE );
    }

    free_some_mem();            // wgml globals
 
    return( EXIT_SUCCESS );
}

