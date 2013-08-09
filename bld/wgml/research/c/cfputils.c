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
* Description:  Implements the utility functions for cfparse():
*                   parse_cop_file()
*                   print_banner()
*                   print_usage()
*               and these local functions:
*                   check_directory()
*                   display_device()
*                   display_driver()
*                   display_font()
*                   verify_device()
*                   verify_driver()
*                   verify_font()
*
* Note:         The Wiki should be consulted for any term whose meaning is
*               not apparent. This should help in most cases.
*
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__ 1
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "banner.h"
#include "cfdev.h"
#include "cfdir.h"
#include "cfdrv.h"
#include "cffon.h"
#include "cfheader.h"
#include "cfparse.h"
#include "common.h"
#include "dfinterp.h"
#include "research.h"

/*  Return values for the verify_ local functions. */

#define OPEN_ERROR 0
#define BAD_HEADER 1
#define READ_ERROR 2
#define BAD_MATCH  3
#define GOOD_MATCH 4

/*  Local variables. */

/*  Load the usage text array. */

static  char const *    usage_text[] = {
#include "cfpusage.h"
NULL
};

/*  Local function declarations. */

static void check_directory( FILE * in_file, uint32_t count );
static void display_device( cop_device * in_device );
static void display_driver( cop_driver * in_driver );
static void display_font( cop_font * in_font );
static int  verify_device( char * in_path, char * in_name );
static int  verify_driver( char * in_path, char * in_name );
static int  verify_font( char * in_path, char * in_name );

/*  Extern function definitions. */

/*  Function parse_cop_file().
 *  Verify that the file provided to the program is a .COP file and parse it
 *  if it is.
 *  The actual parsing is done using functions declared in other headers:
 *      cfdir.h for directory files.
 *      cfdev.h for device files.
 *      cfdrv.h for driver files.
 *      cffon.h for font files.
 *  The number of each type of directory file record (0x101, 0x201, 0x401)
 *  is displayed.
 *
 *  Global Used:
 *      tgt_path contains the file passed on the command line.
 *
 *  Returns:
 *      FAILURE if the directory provided cannot be opened.
 *      SUCCESS if the directory can be opened (and so was checked).
 */

int parse_cop_file( void )
{
    cop_device *    current_device  = NULL;
    cop_driver *    current_driver  = NULL;
    cop_font   *    current_font    = NULL;
    FILE       *    current_file    = NULL;
    char            designator[4];
    uint16_t        entry_count;
    char            file_type;
    int             retval;    

    /* Open the file. */

    fopen_s( &current_file, tgt_path, "rb" );
    if( current_file == NULL ) return( FAILURE );

    /* Process the file. */

    retval = parse_header( current_file, &file_type );
    if(retval == FAILURE)
    {
        printf_s( "%s is not a valid .COP file\n", tgt_path );
        return( FAILURE) ;
    }
    switch( file_type ) {
    case( 0x02 ): 
        /* This is a version 3.33 directory file. */

        fread( &entry_count, sizeof( entry_count ), 1, current_file );
        if( ferror( current_file ) || feof( current_file ) ) return( FAILURE );
        check_directory( current_file, entry_count );
        break;
    case( 0x03 ):
        /* This is a device, driver or font file -- or an error. */

        if( is_dev_file( current_file ) ) {
            printf_s( "%s is a device file\n", tgt_path );
            current_device = parse_device( current_file );
            if( current_device ) display_device( current_device );
            break;
        }
        fseek( current_file, -3, SEEK_CUR ); /* Reset file to designator */
        if( is_drv_file( current_file ) ) {
            printf_s( "%s is a driver file\n", tgt_path );
            current_driver = parse_driver( current_file );
            if( current_driver ) display_driver( current_driver );
            break;
        }
        fseek( current_file, -3, SEEK_CUR ); /* Reset file to designator */
        if( is_fon_file( current_file ) ) {
            printf_s( "%s is a font file\n", tgt_path );
            current_font = parse_font( current_file );
            if( current_font ) display_font( current_font );
            break;
        }
        fseek( current_file, -3, SEEK_CUR ); /* Reset file to designator */
        fread( &designator, 3, 1, current_file );
        if( ferror( current_file ) || feof( current_file ) ) {
            puts("Incorrect file type: file error on attempt to get designator");
            break;
        }
        designator[3] = '\0';
        printf_s("Incorrect file type: %s\n", designator);
        break;
    case( 0x04 ): 
        /* This is a version 4.1 directory file */

        fread( &entry_count, sizeof( entry_count ), 1, current_file );
        if( ferror( current_file ) || feof( current_file ) ) return( FAILURE );
        check_directory( current_file, entry_count );
        break;
    default:
        printf_s( "Unknown file type: %i\n", tgt_path, retval );
    }
    fclose( current_file );
    current_file = NULL;

    return( SUCCESS );
}

/*  Function print_banner().
 *  Print the banner to the screen.
 */

void print_banner( void )
{
    puts( banner1w( "Script/GML Binary File Parser Program", _RESEARCH_VERSION_ ) );
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

/*  Local function definitions. */

/*  Function check_directory().
 *  Checks the entry types. This function will print a message if an unknown
 *  entry type is found and prints the total number of each known type after
 *  processing the last entry. The check starts with the entry pointed to by
 *  in_file and only checks count entries.
 *
 *  Parameters:
 *      in_file points to the start of the first directory entry to check
 *      count contains the number of entries to check.
 */

void check_directory( FILE * in_file, uint32_t count )
{
    /* Used for processing the directory file. */

    char                dir[_MAX_DIR];
    char                drive[_MAX_DRIVE];
    char                ext[_MAX_EXT];
    char                file_path[_MAX_PATH];
    char                fname[_MAX_FNAME];
    directory_entry      current_entry;
    uint32_t            i;
    int                 retval;
    uint16_t            entry_type;

    /* Used for diagnostic counts. */

    int                 bad_file_count  = 0; /* files with errors. */
    int                 dev_file_count  = 0; /* entries of type 0x101. */
    int                 drv_file_count  = 0; /* entries of type 0x201. */
    int                 fon_file_count  = 0; /* entries of type 0x401. */
    int                 mismatch_count  = 0; /* files with wrong designator. */

    /* Split tgt_path. */

    _splitpath( tgt_path, drive, dir, fname, ext );

    /* Construct file_path. */

    strcpy_s( file_path, sizeof( file_path ), drive );
    strcat_s( file_path, sizeof( file_path ), dir );

    /* Tabulate the entry types. */

    for( i = 0; i < count; i++) {
        fread( &entry_type, sizeof( entry_type ), 1, in_file );
        if( feof( in_file ) || ferror( in_file ) ) {
            puts( "File error or EOF: entry counts may not match expected total");
            break;
        }

        switch( entry_type) {
        case 0x0000:
            /* Not an entry type, reset counter. */

            i--;
            break;    
        case 0x0001:
            /* Start of an ExtendedDirEntry. */
            /* Ensure loop is always exited at some point. */

            for( ;; ) {
                fread( &entry_type, sizeof( entry_type ), 1, in_file );
                if( feof( in_file ) || ferror( in_file ) ) {
                    puts( "File error or EOF: entry counts may not match " \
                          "expected total");
                    break;
                }
                switch( entry_type) {
                case 0x0000:
                    /* Not an entry type, reset counter. */

                    i--;
                    break;    
                case 0x0001:
                    /* Start of an ExtendedDirEntry. */
                    /* This is the only case where the loop is not exited. */
                
                    continue;    
                case 0x0101:
                    /* This ExtendedDirEntry is for a device file. */

                    dev_file_count++;
                    retval = get_extended_entry( in_file, &current_entry );
                    if( retval == FAILURE ) printf_s( "No data for device entry " \
                                            "%i of type %i\n", i+1, entry_type );
                    else printf_s( "Entry: %i Device Name: %s File Name: %s\n", \
                        i+1, current_entry.defined_name, current_entry.member_name );
                    retval = verify_device( file_path, current_entry.member_name );
                    if( retval == BAD_MATCH ) mismatch_count++;
                    else if( retval != GOOD_MATCH) bad_file_count++;
                    break;
                case 0x0201:
                    /* This ExtendedDirEntry is for a driver file. */

                    drv_file_count++;
                    retval = get_extended_entry( in_file, &current_entry );
                    if( retval == FAILURE ) \
                            printf_s( "No data for device entry %i of type %i\n", \
                                                                i+1, entry_type );
                    else printf_s( "Entry: %i Driver Name: %s File Name: %s\n", \
                        i+1, current_entry.defined_name, current_entry.member_name );
                    retval = verify_driver( file_path, current_entry.member_name );
                    if( retval == BAD_MATCH ) mismatch_count++;
                    else if( retval != GOOD_MATCH) bad_file_count++;
                    break;
                case 0x0401:
                    /* This ExtendedDirEntry is for a font file. */

                    fon_file_count++;
                    retval = get_extended_entry( in_file, &current_entry );
                    if( retval == FAILURE ) \
                            printf_s( "No data for device entry %i of type %i\n", \
                                                                i+1, entry_type );
                    else printf_s( "Entry: %i Font Name: %s File Name: %s\n", \
                        i+1, current_entry.defined_name, current_entry.member_name );
                    retval = verify_font( file_path, current_entry.member_name );
                    if( retval == BAD_MATCH ) mismatch_count++;
                    else if( retval != GOOD_MATCH) bad_file_count++;
                    break;
                default:
                    /* Unknown ExtendedDirEntry type. */

                    retval = get_extended_entry( in_file, &current_entry );
                    if( retval == FAILURE ) \
                        printf_s( "No data for unknown entry %i of type %i\n", \
                                                            i+1, entry_type);
                    else printf_s( "Entry: %i Unknown Item Type: %i Name: %20s " \
                                    "File Name: %s\n", i+1, entry_type, \
                                    current_entry.defined_name, \
                                    current_entry.member_name );
                }
                /* Ensure loop is exited for any ExtendedDirEntry. */

                break;
            }
            break;
        case 0x0101:
            /* This CompactDirEntry is for a device file. */

            dev_file_count++;
            retval = get_compact_entry( in_file, &current_entry );
            if( retval == FAILURE ) \
                printf_s( "No data for entry %i of type %i\n", i+1, entry_type);
            else printf_s( "Entry: %i Device Name: %s File Name: %s\n", i+1, \
                            current_entry.defined_name, current_entry.member_name );
            retval = verify_device( file_path, current_entry.member_name );
            if( retval == BAD_MATCH ) mismatch_count++;
            else if( retval != GOOD_MATCH) bad_file_count++;
            break;
        case 0x0201:
            /* This CompactDirEntry is for a driver file. */

            drv_file_count++;
            retval = get_compact_entry( in_file, &current_entry );
            if( retval == FAILURE ) \
                printf_s( "No data for entry %i of type %i\n", i+1, entry_type);
            else printf_s( "Entry: %i Driver Name: %s File Name: %s\n", i+1, \
                            current_entry.defined_name, current_entry.member_name );
            retval = verify_driver( file_path, current_entry.member_name );
            if( retval == BAD_MATCH ) mismatch_count++;
            else if( retval != GOOD_MATCH) bad_file_count++;
            break;
        case 0x0401:
            /* This CompactDirEntry is for a font file. */

            fon_file_count++;
            retval = get_compact_entry( in_file, &current_entry );
            if( retval == FAILURE ) \
                printf_s( "No data for entry %i of type %i\n", i+1, entry_type);
            else printf_s( "Entry: %i Font Name: %s File Name: %s\n", i+1, \
                            current_entry.defined_name, current_entry.member_name );
            retval = verify_font( file_path, current_entry.member_name );
            if( retval == BAD_MATCH ) mismatch_count++;
            else if( retval != GOOD_MATCH) bad_file_count++;
            break;
          default:
            /* Unknown CompactDirEntry type. */

            retval = get_compact_entry( in_file, &current_entry );
            if( retval == FAILURE ) \
                printf_s( "No data for unknown entry %i of type %i\n", i+1, \
                                                                    entry_type);
            else printf_s( "Entry: %i Unknown Item Type: %i Name: %20s " \
                           "File Name: %s\n", i+1, entry_type, \
                           current_entry.defined_name, current_entry.member_name );
      }
    }

    /* Report the number of each type of entry. */

    printf_s( "Entries requested: %i\n", count );
    printf_s( "Entries found:     %i\n", dev_file_count + drv_file_count + \
                                                                fon_file_count );
    printf_s( "   of which %i could not be opened or, if opened, read\n", \
                                                                bad_file_count++);
    printf_s( "   and of which %i had a different designator (DEV, DRV, FON) " \
              "than was expected\n", mismatch_count);
    puts( "By type:");
    printf_s( "Device entries:    %i\n", dev_file_count );
    printf_s( "Driver entries:    %i\n", drv_file_count );
    printf_s( "Font entries:      %i\n", fon_file_count );

    return;
}

/*  Function display_device().
 *  Displays the contents of a cop_device instance.
 *
 *  Parameter:
 *      in_device is a pointer to the cop_device instance.
 */

void display_device( cop_device * in_device )
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
    else printf_s( "Output File Extension:     %s\n", \
                                                    in_device->output_extension );
    printf_s( "Page Width:                %i\n", in_device->page_width );
    printf_s( "Page Depth:                %i\n", in_device->page_depth );
    printf_s( "Horizontal Base Units:     %i\n", \
                                                in_device->horizontal_base_units );
    printf_s( "Vertical Base Units:       %i\n", in_device->vertical_base_units );
    printf_s( "Page Start X Value:        %i\n", in_device->x_start );
    printf_s( "Page Start Y Value:        %i\n", in_device->y_start );
    printf_s( "Page Offset X Value:       %i\n", in_device->x_offset );
    printf_s( "Page Offset Y Value:       %i\n", in_device->y_offset );
    if( in_device->box.font_name == NULL ) printf_s( "Box Font Number:           " \
                                           "%i\n", in_device->box.font );
    else printf_s( "Box Font Name:             %s\n", in_device->box.font_name );
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
                    display_char( translation, \
                                    in_device->outtrans->table[i]->data[j] );
                    printf_s( "%c%c ", translation[0], translation[1] );
                }
                puts( "" );
            }
        }
    }
    printf_s( "Number of Default Fonts: %i\n", \
                                            in_device->defaultfonts.font_count );
    for( i = 0; i < in_device->defaultfonts.font_count; i++ ) {
        printf_s( "  Default Font Number  %i:\n", i );
        if( in_device->defaultfonts.fonts[i].font_name == NULL ) \
                                                        puts( "    Font Name:");
        else printf_s( "    Font Name:         %s\n", \
                                    in_device->defaultfonts.fonts[i].font_name );
        if( in_device->defaultfonts.fonts[i].font_style == NULL ) \
                                                        puts( "    FontStyle:");
        else printf_s( "    Font Style:        %s\n", \
                                    in_device->defaultfonts.fonts[i].font_style );
        printf_s( "    Font Height:       %i\n", \
                                    in_device->defaultfonts.fonts[i].font_height );
        printf_s( "    Font Space:        %i\n", \
                                    in_device->defaultfonts.fonts[i].font_space );
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
    if( in_device->pauses.docpage_pause == NULL ) puts( "No DOCUMENT_PAGE Pause" );
    else {
        puts( "DOCUMENT_PAGE Pause:" );
        interpret_function( in_device->pauses.docpage_pause->text );
    }
    if( in_device->pauses.devpage_pause == NULL ) puts( "No DEVICE_PAGE Pause" );
    else {
        puts( "DEVICE_PAGE Pause:" );
        interpret_function( in_device->pauses.devpage_pause->text );
    }
    printf_s( "Number of Device Fonts: %i\n", in_device->devicefonts.font_count );
    for( i = 0; i < in_device->devicefonts.font_count; i++ ) {
        printf_s( "  Device Font Index:   %i:\n", i );
        printf_s( "    Font Name:         %s\n", \
                                    in_device->devicefonts.fonts[i].font_name );
        if( in_device->devicefonts.fonts[i].font_switch == NULL ) \
                                                        puts( "    Font Switch:");
        else printf_s( "    Font Switch:       %s\n", \
                                    in_device->devicefonts.fonts[i].font_switch );
        printf_s( "    Resident Font:     %i\n", \
                                        in_device->devicefonts.fonts[i].resident );
        if( in_device->devicefonts.fonts[i].font_pause == NULL ) \
                                                    puts( "    No Font Pause" );
        else {
            puts( "    Font Pause:" );
            interpret_function( in_device->devicefonts.fonts[i].font_pause->text );
        }
    }

    return;
}

/*  Function verify_device().   
 *  Verifies that the file is a device file.
 *
 *  Parameter:
 *      in_path contains the path from the command-line parameter.
 *      in_file contains the file name from the directory file entry.
 *
 *  Returns:
 *      OPEN_ERROR if the file cannot be opened (implies file does not exist).
 *      BAD_HEADER if the file does not start with a valid .COP file header.
 *      READ_ERROR if the file cannot be read (implies is not a valid .COP file).
 *      BAD_MATCH  if the bytes that should contain "DEV" contains something else.
 *      GOOD_MATCH if the bytes that should contain "DEV" do contain "DEV".
 */
 
int verify_device( char * in_path, char * in_name )
{
    char            designator[4];
    char            member_name[FILENAME_MAX];
    char            type;
    cop_device *    current_device = NULL;
    FILE *          device_file = NULL;
    
    /* Build the file name. */

    strcpy_s( member_name, sizeof( member_name ), in_path );
    strcat_s( member_name, sizeof( member_name ), in_name );
    strcat_s( member_name, sizeof( member_name ), COP_EXT );

    /* Open the file. */

    fopen_s( &device_file, member_name, "rb" );
    if( device_file == NULL ) {
        printf_s( "Could not open device file %s\n", member_name );
        return( OPEN_ERROR );
    }

    /* Skip the header. */

    if( parse_header( device_file, &type ) == FAILURE ) {
        printf_s( "%s is not a .COP file (bad header)\n", member_name );
        fclose( device_file );
        return( BAD_HEADER );
    }
    
    /* Perform the test and parse the file if appropriate. */

    if( is_dev_file( device_file ) ) {
        current_device = parse_device( device_file );
        if( current_device ) display_device( current_device );
        fclose( device_file );
        return( GOOD_MATCH );
    }

    /* Report the mismatched designator. */
    
    /* Reset file to designator. */

    fseek( device_file, -3, SEEK_CUR ); 
    fread( &designator, 3, 1, device_file );
    if( ferror( device_file ) || feof( device_file ) ) {
        puts("Incorrect file type: file error on attempt to get designator");
        fclose( device_file );
        return( READ_ERROR );
    }
    designator[3] = '\0';
    printf_s( "%s has incorrect designator for a device file: %s\n", tgt_path, \
                                                                    designator );
    fclose( device_file );
    return( BAD_MATCH );
}

/*  Function display_driver().
 *  Displays the contents of a cop_driver instance.
 *
 *  Parameter:
 *      in_driver is a pointer to the cop_driver instance.
 */

void display_driver( cop_driver * in_driver )
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
            if( in_driver->inits.start->codeblock[i].is_fontvalue ) \
                                                    puts( ":FONTVALUE Block:");
            else puts( ":VALUE Block:");
            interpret_function( in_driver->inits.start->codeblock[i].text );
        }
    }
    puts( "Document :INIT Block:" );
    if( in_driver->inits.document != NULL ) {
        for( i = 0; i < in_driver->inits.document->count; i++ ) {
            if( in_driver->inits.document->codeblock[i].is_fontvalue ) \
                                                    puts( ":FONTVALUE Block:");
            else puts( ":VALUE Block:");
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
            printf_s( "  Advance: %i\n", \
                                    in_driver->newlines.newlineblocks[i].advance );
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
    if( in_driver->fontswitches.fontswitchblocks == NULL ) \
                                                    puts( ":FONTSWITCH Block:");
    else {
        puts( ":FONTSWITCH Block(s):" );    
        for( i = 0; i < in_driver->fontswitches.count; i++ ) {
            printf_s( "  Type: %s\n", \
                                in_driver->fontswitches.fontswitchblocks[i].type );
            if( in_driver->fontswitches.fontswitchblocks[i].startvalue != NULL ) {
                puts( "  :STARTVALUE Block:");
                interpret_function( \
                    in_driver->fontswitches.fontswitchblocks[i].startvalue->text );
            }
            if( in_driver->fontswitches.fontswitchblocks[i].endvalue != NULL ) {
                puts( "  :ENDVALUE Block:");
                interpret_function( \
                    in_driver->fontswitches.fontswitchblocks[i].endvalue->text );
            }
        }
    }
    if( in_driver->fontstyles.fontstyleblocks == NULL ) puts( ":FONTSTYLE Block:");
    else {
        puts( ":FONTSTYLE Block(s):" );    
        for( i = 0; i < in_driver->fontstyles.count; i++ ) {
            printf_s( "  Type: %s\n", \
                                in_driver->fontstyles.fontstyleblocks[i].type );
            if( in_driver->fontstyles.fontstyleblocks[i].startvalue == NULL ) {
                puts( "  No :STARTVALUE Block");
            } else {
                puts( "  :STARTVALUE Block:");
                interpret_function( \
                    in_driver->fontstyles.fontstyleblocks[i].startvalue->text );
            }
            if( in_driver->fontstyles.fontstyleblocks[i].endvalue == NULL ) {
                puts( "  No :ENDVALUE Block");
            } else {
                puts( "  :ENDVALUE Block:");
                interpret_function( \
                        in_driver->fontstyles.fontstyleblocks[i].endvalue->text );
            }
            if(in_driver->fontstyles.fontstyleblocks[i].lineprocs == NULL ) {
                puts( "  No :LINEPROC Blocks");
            } else {
                puts( "  :LINEPROC Block(s):");
                for( j = 0; j < in_driver->fontstyles.fontstyleblocks[i].passes; \
                                                                            j++ ) { 
                    printf_s( "  Pass: %i\n", j+1 );
                    if( in_driver->fontstyles.fontstyleblocks[i].lineprocs[j].\
                                                            startvalue == NULL ) {
                        puts( "  No :STARTVALUE Block");
                    } else {
                        puts( "  :STARTVALUE Block:");
                        interpret_function( in_driver->fontstyles.\
                                fontstyleblocks[i].lineprocs[j].startvalue->text );
                    }
                    if( in_driver->fontstyles.fontstyleblocks[i].lineprocs[j].\
                                                            firstword == NULL ) {
                        puts( "  No :FIRSTWORD Block");
                    } else {
                        puts( "  :FIRSTWORD Block:");
                        interpret_function( in_driver->fontstyles.\
                                fontstyleblocks[i].lineprocs[j].firstword->text );
                    }
                    if( in_driver->fontstyles.fontstyleblocks[i].lineprocs[j].\
                                                            startword == NULL ) {
                        puts( "  No :STARTWORD Block");
                    } else {
                        puts( "  :STARTWORD Block");
                        interpret_function( in_driver->fontstyles.\
                                fontstyleblocks[i].lineprocs[j].startword->text );
                    }
                    if( in_driver->fontstyles.fontstyleblocks[i].lineprocs[j].\
                                                                endword == NULL ) {
                        puts( "  No :ENDWORD Block");
                    } else {
                        puts( "  :ENDWORD Block:");
                        interpret_function( in_driver->fontstyles.\
                                fontstyleblocks[i].lineprocs[j].endword->text );
                    }
                    if( in_driver->fontstyles.fontstyleblocks[i].lineprocs[j].\
                                                            endvalue == NULL ) {
                        puts( "  No :ENDVALUE Block");
                    } else {
                        puts( "  :ENDVALUE Block:");
                        interpret_function( in_driver->fontstyles.\
                                fontstyleblocks[i].lineprocs[j].endvalue->text );
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

/*  Function verify_driver().
 *  Verifies that the file is a driver file.
 *
 *  Parameter:
 *      in_path contains the path from the command-line parameter.
 *      in_file contains the file name from the directory file entry.
 *
 *  Returns:
 *      OPEN_ERROR if the file cannot be opened (implies file does not exist).
 *      BAD_HEADER if the file does not start with a valid .COP file header.
 *      READ_ERROR if the file cannot be read (implies is not a valid .COP file).
 *      BAD_MATCH  if the bytes that should contain "DRV" contains something else.
 *      GOOD_MATCH if the bytes that should contain "DRV" do contain "DRV".
 */
 
int verify_driver( char * in_path, char * in_name )
{
    char            designator[4];
    char            member_name[FILENAME_MAX];
    char            type;
    cop_driver *    current_driver = NULL;
    FILE *          driver_file = NULL;

    /* Build the file name. */

    strcpy_s( member_name, sizeof( member_name ), in_path );
    strcat_s( member_name, sizeof( member_name ), in_name );
    strcat_s( member_name, sizeof( member_name ), COP_EXT );

    /* Open the file. */

    fopen_s( &driver_file, member_name, "rb" );
    if( driver_file == NULL ) {
        printf_s( "Could not open driver file %s\n", member_name );
        return( OPEN_ERROR );
    }

    /* Skip the header. */

    if( parse_header( driver_file, &type ) == FAILURE ) {
        printf_s( "%s is not a .COP file (bad header)\n", member_name );
        fclose( driver_file );
        return( BAD_HEADER );
    }
    
    /* Perform the test and parse the file if appropriate. */

    if( is_drv_file( driver_file ) ) {
        current_driver = parse_driver( driver_file );
        if( current_driver ) display_driver( current_driver );
        fclose( driver_file );
        return( GOOD_MATCH );
    }

    /* Report the mismatched designator. */
    
    /* Reset file to designator. */

    fseek( driver_file, -3, SEEK_CUR );
    fread( &designator, 3, 1, driver_file );
    if( ferror( driver_file ) || feof( driver_file ) ) {
        puts("Incorrect file type: file error on attempt to get designator");
        fclose( driver_file );
        return( READ_ERROR );
    }
    designator[3] = '\0';
    printf_s( "%s has incorrect designator for a driver file: %s\n", tgt_path, designator );
    fclose( driver_file );
    return( BAD_MATCH );
}

/*  Function display_font().
 *  Displays the contents of a cop_font instance.
 *
 *  Parameter:
 *      in_font is a pointer to the cop_font instance.
 */

void display_font( cop_font * in_font )
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
                printf_s( "%c%c %c%c\n", font_character[0], font_character[1], \
                                         translation[0], translation[1] );
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
                    display_char( translation, \
                                        in_font->outtrans->table[i]->data[j] );
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
                printf_s( "%c%c %lu\n", font_character[0], \
                                    font_character[1], in_font->width->table[i] );
            }
        }
    }

    return;
}

/*  Function verify_font().
 *  Verifies that the file is a font file.
 *
 *  Parameter:
 *      in_path contains the path from the command-line parameter.
 *      in_file contains the file name from the directory file entry.
 *
 *  Returns:
 *      OPEN_ERROR if the file cannot be opened (implies file does not exist).
 *      BAD_HEADER if the file does not start with a valid .COP file header.
 *      READ_ERROR if the file cannot be read (implies is not a valid .COP file).
 *      BAD_MATCH  if the bytes that should contain "FON" contains something else.
 *      GOOD_MATCH if the bytes that should contain "FON" do contain "FON".
 */
 
int verify_font( char * in_path, char * in_name )
{
    char       designator[4];
    char       member_name[FILENAME_MAX];
    char       type;
    cop_font * current_font = NULL;
    FILE *     font_file = NULL;
    
    /* Build the file name. */

    strcpy_s( member_name, sizeof( member_name ), in_path );
    strcat_s( member_name, sizeof( member_name ), in_name );
    strcat_s( member_name, sizeof( member_name ), COP_EXT );

    /* Open the file. */

    fopen_s( &font_file, member_name, "rb" );
    if( font_file == NULL ) {
        printf_s( "Could not open font file %s\n", member_name );
        return( OPEN_ERROR );
    }

    /* Skip the header. */

    if( parse_header( font_file, &type ) == FAILURE ) {
        printf_s( "%s is not a .COP file (bad header)\n", member_name );
        fclose( font_file );
        return( BAD_HEADER );
    }
    
    /* Perform the test and parse the file if appropriate. */

    if( is_fon_file( font_file ) ) {
        current_font = parse_font( font_file );
        if( current_font ) display_font( current_font );
        fclose( font_file );
        return( GOOD_MATCH );
    }
    
    /* Report the mismatched designator. */
    
    /* Reset file to designator. */

    fseek( font_file, -3, SEEK_CUR );
    fread( &designator, 3, 1, font_file );
    if( ferror( font_file ) || feof( font_file ) ) {
        puts("Incorrect file type: file error on attempt to get designator");
        fclose( font_file );
        return( READ_ERROR );
    }
    designator[3] = '\0';
    printf_s( "%s has incorrect designator for a font file: %s\n", tgt_path, \
                                                                    designator );
    fclose( font_file );
    return( BAD_MATCH );
}

