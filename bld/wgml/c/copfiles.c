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
* Description:  Implements the functions used to parse .COP files:
*                   cop_in_trans()
*                   cop_setup()
*                   cop_teardown()
*                   cop_text_width()
*                   cop_ti_table()
*                   fb_dbox()
*                   fb_document()
*                   fb_document_page()
*                   fb_finish()
*                   fb_hline()
*                   fb_new_section()
*                   fb_output_textline()
*                   fb_start()
*                   fb_vline()
*               plus these static variables:
*                   bin_fonts
*                   cur_token
*                   ti_table
*               and these static functions:
*                   compute_metrics()
*                   find_cop_font()
*                   find_dev_font()
*                   find_style()
*                   find_switch()
*                   free_opt_fonts()
*                   get_cop_device()
*                   get_cop_driver()
*                   get_cop_font()
*                   scale_basis_to_horizontal_base_units()
*
* Note:         The Wiki should be consulted for any term whose meaning is
*               not apparent and for how the various function blocks are used.
*               This should help in most cases.
****************************************************************************/


#include <math.h>
#include <time.h>
#include "wgml.h"
#include "copdev.h"
#include "copdrv.h"
#include "copfon.h"
#include "cophdr.h"
#include "devfuncs.h"
#include "outbuff.h"

#include "clibext.h"


/* Static data. */

static cop_font         *bin_fonts;         // binary fonts being used (linked list)
static record_buffer    *cur_token;         // Current token.
static unsigned char    ti_table[0x100];    // .TI-controlled translation table

/* Static function definitions. */

/* Function scale_basis_to_horizontal_base_units().
 * Converts a length expressed in scale_basis units to the same length
 * expressed in horizontal_base_units.
 *
 * Parameter:
 *      in_units contains the length in scale_basis units.
 *      in_font points to the font to use.
 *
 * Returns:
 *      the same length in horizontal_base_units.
 */

static unsigned scale_basis_to_horizontal_base_units( unsigned in_units, wgml_font *in_font )
{
    unsigned    divisor;
    uint64_t    units;
    unsigned    width;

    /* The conversion is done using this formula:
     * horizontal_base_units * font_height/100 * in_units
     * ------------------------------------------------
     *                     scale_basis
     * font_height is reduced from centipoints to points. This is done by
     * adjusting the divisor to avoid loss-of-precision problems with
     * integer arithmetic.
     */

    divisor = (in_font->bin_font->scale_basis * 100);
    units = (bin_device->horizontal_base_units * in_font->font_height);
    units *= in_units;
    width = units / divisor;

    /* This rounds width up if the division did not produce an integer and
     * the decimal part, if computed, would be 0.1 or greater. This produces
     * correct results when the tested with multiple copies of the same character,
     * but may need to be modified (or the entire algorithm reconsidered) when
     * side-by-side comparisons of wgml 4.0 and our wgml become possible.
     */

    if( ((units % divisor) * 10) >= divisor )
        width++;

    return( width );
}

/* Function compute_metrics().
 * Computes and initializes the default_width, line_height and line_space values
 * for the given wgml_font instance.
 *
 * Parameter:
 *      in_font points to the given wgml_font instance.
 */

static void compute_metrics( wgml_font * in_font )
{
    unsigned    glyph_height;
    unsigned    height;
    unsigned    shift_height;

    /* Compute the default character width. */

    if( in_font->bin_font->scale_basis == 0 ) {

        /* The default_width is the char_width. */

        in_font->default_width = in_font->bin_font->char_width;

    } else {

        /* The default_width is the char_width properly scaled. */

        in_font->default_width = scale_basis_to_horizontal_base_units(
                                        in_font->bin_font->char_width, in_font );
    }

    /* Compute the line height and space values. */

    if( in_font->font_height == 0 ) {

        /* Use the line_height and line_space values from the FONT block. */

        in_font->line_space = in_font->bin_font->line_space;
        in_font->line_height = in_font->bin_font->line_height + in_font->line_space;
    } else {

        /* Use the font_height, font_space, and vertical_base_units values.
         * The formula is:
         *    (font_height + font_space) * vertical_base_units
         *    ------------------------------------------------
         *                         7200
         * where the divisor is 72 points/inch times the correction of
         * (font_height + font_space) from centipoints to points.
         */


        height = (in_font->font_height + in_font->font_space) *
                                         bin_device->vertical_base_units;

        in_font->line_height = height / 7200;

        /* The rounding criteria may need to be tweaked when side-by-side
         * comparisons of wgml 4.0 and our wgml become possible.
         */

        if( (height % 7200) > 0 )
            in_font->line_height++;

        /* If font_space is "0", then line_space will be "0". */

        in_font->line_space = 0;
        if( in_font->font_space > 0) {

            /* glyph_height is set to the number of vertical_base_units
             * corresponding to the value of font_height (and so, presumably,
             * to the amount of vertical space occupied by the glyphs).
             */

            height = in_font->font_height * bin_device->vertical_base_units;
            glyph_height = height / 7200;

            /* The rounding criteria may need to be tweaked when side-by-side
             * comparisons of wgml 4.0 and our wgml become possible.
             */

            if( (height % 7200) >= 3600 )
                glyph_height++;

            /* The value of font_space is the difference between the computed
             * value of font_height and the value of glyph_height.
             */

            in_font->line_space = in_font->line_height - glyph_height;
        }
    }

    /* Compute the shift_height. This is only done for a PS device, as the
     * fields are already correct for a non-PS device.
     */

    if( ProcFlags.ps_device ) {

        /* The formula is: (font height * 3) / 10, rounded down.
         * The font height is in vertical base units, computed as above.
         * Since the maximum font height is 72 points or 1 inch, and the
         * number of vertical base units per inch is 1000 for device PS,
         * the maximum value is 300. Thus, a four-character array is
         * large enough in this case.
         */

        height = (in_font->font_height ) * bin_device->vertical_base_units;

        shift_height = height / 7200;
        if( (height % 7200) > 0 )
            shift_height++;

        height = (shift_height * 3 ) / 10;
        {
            char    numstr[6];
            int     len;

            if( height > 9999 )
                height = 9999;
            len = sprintf( numstr, "%u", height );
            strncpy( in_font->shift_height, numstr, len );
            in_font->shift_count = len;
        }
    }

    return;
}

/* Function get_cop_device().
 * Converts the defined name of a DEVICE block into a cop_device struct
 * containing the information in that DEVICE block.
 *
 * Parameter:
 *      dev_name points to the defined name of the device.
 *
 * Globals Used:
 *      try_file_name contains the name of the device file, if found.
 *
 * Return:
 *      on success, a cop_device instance containing the data.
 *      on failure, a NULL pointer.
 */

static cop_device *get_cop_device( char const *dev_name )
{
    cop_device      *out_device  = NULL;
    cop_file_type   file_type;
    FILE            *fp;

    /* Bail if no name was supplied. */
    if( dev_name == NULL
      || *dev_name == '\0' ) {
        return( out_device );
    }

    /* Acquire the file, if it exists. */

    fp = search_file_in_dirs( dev_name, "", "", DSEQ_bin_lib );
    if( fp == NULL ) {
        return( out_device );
    }

    /* Determine if the file encodes a DEVICE block. */

    file_type = parse_header( fp );

    switch( file_type ) {
    case COP_file_error:

        /* File error, including premature eof. */

        xx_simple_err_exit_c( ERR_DEV_DATA_FILE, try_file_name );
        /* never return */

    case COP_not_se_v4_1:

        /* File was created by a different version of gendev. */

        xx_simple_err_exit( ERR_WRONG_GENDEV );
        /* never return */

    case COP_not_bin_dev:
    case COP_dir_v4_1_se:

        /* Wrong type of file: something is wrong with the device library. */

        xx_simple_err_exit_c( ERR_DEV_LIB_DATA, try_file_name );
        /* never return */

    case COP_se_v4_1_not_dir:

        /* fp was a same-endian version 4.1 file, but not a directory file. */

        out_device = parse_device( fp );
        if( out_device == NULL ) {
            xx_simple_err_exit_c( ERR_DEV_LIB_DATA, try_file_name );
            /* never return */
        }
        break;

    default:

        /* parse_header() returned an unknown value. */

        internal_err_exit( __FILE__, __LINE__ );
        /* never return */
    }

    fclose( fp );

    return( out_device );
}

/* Function get_cop_driver().
 * Converts the defined name of a DRIVER block into a cop_driver struct
 * containing the information in that DRIVER block.
 *
 * Parameter:
 *      drv_name points to the defined name of the driver.
 *
 * Globals Used:
 *      try_file_name contains the name of the driver file, if found.
 *
 * Returns:
 *      on success, a cop_driver instance containing the data.
 *      on failure, a NULL pointer.
 */

static cop_driver *get_cop_driver( char const *drv_name )
{
    cop_driver      *out_driver  = NULL;
    cop_file_type   file_type;
    FILE            *fp;

    /* Acquire the file, if it exists. */

    fp = search_file_in_dirs( drv_name, "", "", DSEQ_bin_lib );
    if( fp == NULL ) {
        return( out_driver );
    }

    /* Determine if the file encodes a DRIVER block. */

    file_type = parse_header( fp );

    switch( file_type ) {
    case COP_file_error:

        /* File error, including premature eof. */

        xx_simple_err_exit_c( ERR_DEV_LIB_FILE, try_file_name );
        /* never return */

    case COP_not_se_v4_1:

        /* File was created by a different version of gendev. */

        xx_simple_err_exit( ERR_WRONG_GENDEV );
        /* never return */

    case COP_not_bin_dev:
    case COP_dir_v4_1_se:

        /* Wrong type of file: something is wrong with the device library. */

        xx_simple_err_exit_c( ERR_DEV_LIB_DATA, try_file_name );
        /* never return */

    case COP_se_v4_1_not_dir:

        /* fp was a same-endian version 4.1 file, but not a directory file. */

        out_driver = parse_driver( fp );
        if( out_driver == NULL ) {
            xx_simple_err_exit_c( ERR_DEV_DATA_FILE, try_file_name );
            /* never return */
        }
        break;

    default:

        /* parse_header() returned an unknown value. */

        internal_err_exit( __FILE__, __LINE__ );
        /* never return */
    }

    fclose( fp );

    return( out_driver );
}

/* Function get_cop_font().
 * Converts the defined name of a FONT block into a cop_font struct
 * containing the information in that FONT block.
 *
 * Parameter:
 *      fon_name points to the defined name of the font.
 *
 * Globals Used:
 *      try_file_name contains the name of the font file, if found.
 *
 * Returns:
 *      on success, a cop_font instance containing the data.
 *      on failure, a NULL pointer.
 */

static cop_font *get_cop_font( char const *fon_name )
{
    cop_font        *out_font    = NULL;
    cop_file_type   file_type;
    FILE            *fp;

    /* Acquire the file, if it exists. */

    fp = search_file_in_dirs( fon_name, "", "", DSEQ_bin_lib );
    if( fp == NULL ) {
        return( out_font );
    }

    /* Determine if the file encodes a FONT block. */

    file_type = parse_header( fp );

    switch( file_type ) {
    case COP_file_error:

        /* File error, including premature eof. */

        xx_simple_err_exit_c( ERR_DEV_LIB_FILE, try_file_name );
        /* never return */

    case COP_not_se_v4_1:

        /* File was created by a different version of gendev. */

        xx_simple_err_exit( ERR_WRONG_GENDEV );
        /* never return */

    case COP_not_bin_dev:
    case COP_dir_v4_1_se:

        /* Wrong type of file: something is wrong with the device library. */

        xx_simple_err_exit_c( ERR_DEV_LIB_DATA, try_file_name );
        /* never return */

    case COP_se_v4_1_not_dir:

        /* fp was a same-endian version 4.1 file, but not a directory file. */

        out_font = parse_font( fp, fon_name );
        if( out_font == NULL ) {
            xx_simple_err_exit_c( ERR_DEV_DATA_FILE, try_file_name );
            /* never return */
        }
        break;

    default:

        /* parse_header() returned an unknown value. */

        internal_err_exit( __FILE__, __LINE__ );
        /* never return */
    }

    fclose( fp );

    return( out_font );
}

/* Function find_cop_font().
 * Finds the cop_font instance for the requested font, creating and adding it
 * to the bin_fonts list if necessary.
 *
 * Parameter:
 *      fon_name is the defined name of the desired binary FONT block.
 *
 * Returns:
 *      a pointer to the cop_font instance on success.
 *      a NULL pointer on failure.
 *
 * Note:
 *      the comparison is not case-sensitive for compatability with wgml 4.0.
 */

static cop_font * find_cop_font( char const *fon_name )
{
    cop_font    *   current = NULL;
    cop_font    *   retval  = NULL;

    for( current = bin_fonts; current != NULL; current = current->next_font ) {
        if( stricmp( fon_name, current->defined_name ) == 0 ) {
            retval = current;
            break;
        }
    }

    if( retval == NULL ) {
        retval = get_cop_font( fon_name );
        if( retval == NULL ) {
            xx_simple_err_exit_c( ERR_DEV_NOT_FOUND, fon_name );
            /* never return */
        }
        retval->next_font = bin_fonts;
        bin_fonts = retval;
    }

    return( retval );
}

/* Function find_dev_font().
 * Finds the device_font instance for the requested font.
 *
 * Parameter:
 *      font_name is the defined name of the desired binary DEVICEFONT block.
 *
 * Returns:
 *      a pointer to the device_font instance on success.
 *      a NULL pointer on failure.
 *
 * Note:
 *      the comparison is not case-sensitive for compatability with wgml 4.0.
 */

static device_font * find_dev_font( char const *font_name )
{
    devicefont_block    *   current = NULL;
    device_font         *   retval  = NULL;
    int                     i;

    current = &bin_device->devicefonts;
    for( i = 0; i < current->font_count; i++ ) {
        if( stricmp( font_name, current->fonts[i].font_name ) == 0 ) {
            retval = &current->fonts[i];
            break;
        }
    }

    if( retval == NULL ) {
        xx_simple_err_exit_cc( ERR_BLOCK_NOT_FOUND, "DEVICEFONT", font_name );
        /* never return */
    }

    return( retval );
}

/* Function find_style().
 * Finds the fonstyle_block instance for the requested font style.
 *
 * Parameter:
 *      name is the defined name of the desired binary FONTSTYLE block.
 *
 * Returns:
 *      a pointer to the fonstyle_block instance on success.
 *      a NULL pointer on failure.
 *
 * Note:
 *      the comparison is case-insensitive for compatability with wgml 4.0.
 */

static fontstyle_block * find_style( char const *name )
{
    fontstyle_group  *   current = NULL;
    fontstyle_block  *   retval  = NULL;
    int                 i;

    current = &bin_driver->fontstyles;
    for( i = 0; i < current->count; i++ ) {
        if( stricmp( name, current->fontstyleblocks[i].type ) == 0 ) {
            retval = &current->fontstyleblocks[i];
            break;
        }
    }

    if( retval == NULL ) {
        xx_simple_err_exit_cc( ERR_BLOCK_NOT_FOUND, "FONTSTYLE", name );
        /* never return */
    }

    return( retval );
}

/* Function find_switch().
 * Finds the fontswitch_block instance for the requested font switch.
 *
 * Parameter:
 *      name is the defined name of the desired binary FONTSWITCH block.
 *
 * Returns:
 *      a pointer to the fontswitch_block instance on success.
 *      a NULL pointer on failure.
 *
 * Note:
 *      the comparison is case-insensitive for compatability with wgml 4.0.
 */

static fontswitch_block * find_switch( char const *name )
{
    fontswitch_funcs    *   current = NULL;
    fontswitch_block    *   retval  = NULL;
    int                     i;

    current = &bin_driver->fontswitches;
    for( i = 0; i < current->count; i++ ) {
        if( stricmp( name, current->fontswitchblocks[i].type ) == 0 ) {
            retval = &current->fontswitchblocks[i];
            break;
        }
    }

    if( retval == NULL ) {
        xx_simple_err_exit_cc( ERR_BLOCK_NOT_FOUND, "FONTSWITCH", name );
        /* never return */
    }

    return( retval );
}

/* Function free_opt_fonts().
 * Frees all memory allocated in setting up the opt_fonts global variable.
 */

static void free_opt_fonts( void )
{
    opt_font    *   old;

    while( opt_fonts != NULL) {
        old = opt_fonts;

        if( old->name != NULL) {
            mem_free( old->name );
            old->name = NULL;
        }

        if( old->style != NULL) {
            mem_free( old->style );
            old->style = NULL;
        }
        opt_fonts = opt_fonts->nxt;
        mem_free( old );
    }

    return;
}

/* Extern function definitions */

/* Function cop_in_trans().
 * Translates the given character per the tables associated with the .TI
 * command word and the various INTRANS blocks.
 *
 * Parameter:
 *      in_char contains the character to be translated.
 *
 * Returns:
 *      The appropriate character, which may be the same as in_char.
 */

char cop_in_trans( char in_char, font_number font )
{
    intrans_block   *block   = NULL;
    char            retval;

    CHECK_FONT( font );

    retval = ti_table[(unsigned char)in_char];
    if( retval == in_char ) {
        block = wgml_fonts[font].bin_font->intrans;
        if( block != NULL ) {
            retval = block->table[(unsigned char)in_char];
        }
    }

    if( retval == in_char ) {
        block = bin_device->intrans;
        if( block != NULL ) {
            retval = block->table[(unsigned char)in_char];
        }
    }

    return( retval );
}

/* Function cop_setup().
 * Initialize the static globals and those extern globals which depend either
 * contain information from or depend on information in the device library.
 *
 * Note: a missing device name is caught during option processing. If this function
 *       is reached, then the name exists.
 */

void cop_setup( void )
{
    default_font    *   cur_def_fonts   = NULL;
    device_font     *   cur_dev_font    = NULL;
    int                 font_base       = 0;
    int                 gen_cnt         = 0;
    int                 i;
    int                 j;
    opt_font        *   cur_opt         = NULL;
    wgml_font           def_font;

    /* Set the externs to known values. */

    bin_device = NULL;
    bin_driver = NULL;
    ProcFlags.has_aa_block = false;
    ProcFlags.ps_device = false;
    ProcFlags.wh_device = false;
    wgml_font_cnt = 0;
    wgml_fonts = NULL;

    /* Initialize the static variables. */

    bin_fonts = NULL;

    cur_token = mem_alloc( sizeof( record_buffer ) );
    init_record_buffer( cur_token, 10 );

    for( i = 0; i < 0x100; i++) {
        ti_table[i] = i;
    }

    /* Initialize the extern variables. */

    /* Emit the expected message. */

    g_info_lm( INF_PROC_DEV );

    /* Process the device. */

    bin_device = get_cop_device( g_dev_name );

    if( bin_device == NULL ) {
        xx_simple_err_exit_cc( ERR_BLOCK_NOT_FOUND, "DEVICE", g_dev_name );
        /* never return */
    }

    /* The value of horizontal_base_units cannot be "0". */

    if( bin_device->horizontal_base_units == 0 ) {
        internal_err_exit( __FILE__, __LINE__ );
        /* never return */
    }

    /* The value of vertical_base_units cannot be "0". */

    if( bin_device->vertical_base_units == 0 ) {
        internal_err_exit( __FILE__, __LINE__ );
        /* never return */
    }

    /* A driver name must exist. */

    if( bin_device->driver_name == NULL ) {
        internal_err_exit( __FILE__, __LINE__ );
        /* never return */
    }
    bin_driver = get_cop_driver( bin_device->driver_name );

    if( bin_driver == NULL ) {
        xx_simple_err_exit_cc( ERR_BLOCK_NOT_FOUND, "DRIVER", bin_device->driver_name );
        /* never return */
    }

    /* Attribute x_positive in PAGEADDRESS cannot be "no", since horizontal
     * positioning does not check this value or react to it. This prevents
     * the generation of negative values for horizontal positions.
     */

    if( bin_driver->x_positive == 0 ) {
        internal_err_exit( __FILE__, __LINE__ );
        /* never return */
    }

    /* If attribute y_positive in PAGEADDRESS was "no", then attribute
     * y_start in PAGESTART must not be "0". This prevents the generation
     * of negative values for vertical positions.
     */

    if( bin_driver->y_positive == 0 ) {
        if( bin_device->y_start == 0 ) {
            internal_err_exit( __FILE__, __LINE__ );
            /* never return */
        }
    }

    /* Set ProcFlags.has_aa_block to "true" if the driver defines the
     * ABSOLUTEADDRESS block.
     */

    if( bin_driver->absoluteaddress.text != NULL ) {
        ProcFlags.has_aa_block = true;
    }

    /* Set ProcFlags.ps_device to "true" if the driver name begins with "ps" or "PS". */

    if( strnicmp( "ps", bin_device->driver_name, 2 ) == 0 ) {
        ProcFlags.ps_device = true;
    }

    /* Set ProcFlags.wh_device to "true" if the driver name begins with "whelp" or "WHELP". */

    if( strnicmp( "whelp", bin_device->driver_name, 5 ) == 0 ) {
        ProcFlags.wh_device = true;
    }

    /* Get the highest font number and reduce it by one so it contains the
     * highest valid array index.
     */

    wgml_font_cnt = bin_device->defaultfonts.font_count;
    wgml_font_cnt--;

    /* Adjust the highest valid array index as needed to accomodate the font
     * numbers given with the FONT command-line option.
     */

    for( cur_opt = opt_fonts; cur_opt != NULL; cur_opt = cur_opt->nxt ) {
        if( wgml_font_cnt < cur_opt->font  ) {
           wgml_font_cnt = cur_opt->font;
        }
    }

    /* The value needed for the upper bound of the zero-based array is the
     * highest valid array index plus 1.
     */

     wgml_font_cnt++;

    /* If either BOX or UNDERSCORE provided a font name, increment the count.
     * If both BOX and UNDERSCORE provided a font name, increment the count
     * once if the font names were identical, twice if they were different.
     * But not if the device is PS: for PS, such fonts are never created.
     */

    if( !ProcFlags.ps_device ) {
        if( bin_device->box.font_name == NULL ) {
            if( bin_device->underscore.specified_font
              && (bin_device->underscore.font_name != NULL) ) {
                gen_cnt++;
            }
        } else {
            gen_cnt++;
            if( bin_device->underscore.specified_font
              && (bin_device->underscore.font_name != NULL) ) {
                if( stricmp( bin_device->box.font_name, bin_device->underscore.font_name ) != 0 ) {
                    gen_cnt++;
                }
            }
        }
    }

    font_base = wgml_font_cnt;
    wgml_font_cnt += gen_cnt;

    /* Initialize wgml_fonts, which is an array */

    wgml_fonts = mem_alloc( wgml_font_cnt * sizeof( wgml_font ) );
    for( i = 0; i < wgml_font_cnt; i++ ) {
        wgml_fonts[i].bin_font = NULL;
        wgml_fonts[i].font_switch = NULL;
        wgml_fonts[i].font_pause = NULL;
        wgml_fonts[i].font_style = NULL;
        wgml_fonts[i].outtrans = NULL;
        wgml_fonts[i].default_width = 0;
        wgml_fonts[i].em_base = 0;
        wgml_fonts[i].font_height = 0;
        wgml_fonts[i].font_space = 0;
        wgml_fonts[i].line_height = 0;
        wgml_fonts[i].line_space = 0;
        wgml_fonts[i].spc_width = 0;
        for( j = 0; j < 0x100; j++ ) {
            wgml_fonts[i].width.table[j] = 0;
        }
        wgml_fonts[i].font_resident = 'n';
        wgml_fonts[i].shift_count = 0;
        for( j = 0; j < 4; j++ ) {
            wgml_fonts[i].shift_height[j] = '\0';
        }
    }

    /* Process the DEFAULTFONT Blocks. */

    cur_def_fonts = bin_device->defaultfonts.fonts;
    for( i = 0; i < bin_device->defaultfonts.font_count; i++ ) {
        if( (cur_def_fonts[i].font_name == NULL)
          || (cur_def_fonts[i].font_name[0] == '\0') ) {
            continue; /* Do not initialize skipped font numbers. */
        }
        wgml_fonts[i].bin_font = find_cop_font( cur_def_fonts[i].font_name );
        if( (cur_def_fonts[i].font_style == NULL)
          || (cur_def_fonts[i].font_style[0] == '\0') ) {
            wgml_fonts[i].font_style = find_style( "plain" );
        } else {
            wgml_fonts[i].font_style = find_style( cur_def_fonts[i].font_style );
        }
        wgml_fonts[i].font_height = cur_def_fonts[i].font_height;
        wgml_fonts[i].font_space = cur_def_fonts[i].font_space;

        if( cur_def_fonts[i].font_name == NULL ) {
            cur_dev_font = find_dev_font( cur_def_fonts[0].font_name );
        } else {
            cur_dev_font = find_dev_font( cur_def_fonts[i].font_name );
        }
        if( cur_dev_font->font_switch == NULL ) {
            wgml_fonts[i].font_switch = NULL;
        } else {
            wgml_fonts[i].font_switch = find_switch( cur_dev_font->font_switch );
        }
        wgml_fonts[i].font_pause = cur_dev_font->font_pause;
        if( cur_dev_font->resident == 0 ) {
            wgml_fonts[i].font_resident = 'n';
        } else {
            wgml_fonts[i].font_resident = 'y';
        }
        if( wgml_fonts[i].bin_font->outtrans == NULL ) {
            wgml_fonts[i].outtrans = bin_device->outtrans;
        } else {
            wgml_fonts[i].outtrans = wgml_fonts[i].bin_font->outtrans;
        }

        /* If scale_basis is not "0", then font_height must not be "0". */

        if( (wgml_fonts[i].bin_font->scale_basis != 0)
          && (wgml_fonts[i].font_height == 0)) {
            xx_simple_err_exit_i( ERR_FONT_SCALED, i );
            /* never return */
        }

        compute_metrics( &wgml_fonts[i] );
    }

    /* Process the FONT command-line option instances. */

    for( cur_opt = opt_fonts; cur_opt != NULL; cur_opt = cur_opt->nxt ) {
        i = cur_opt->font;
        wgml_fonts[i].bin_font = find_cop_font( cur_opt->name );
        if( cur_opt->style == NULL ) {
            wgml_fonts[i].font_style = find_style( "plain" );
        } else {
            wgml_fonts[i].font_style = find_style( cur_opt->style );
        }
        wgml_fonts[i].font_height = cur_opt->height;
        wgml_fonts[i].font_space = cur_opt->space;
        cur_dev_font = find_dev_font( cur_opt->name );
        if( cur_dev_font->font_switch == NULL ) {
            wgml_fonts[i].font_switch = NULL;
        } else {
            wgml_fonts[i].font_switch = find_switch( cur_dev_font->font_switch );
        }
        wgml_fonts[i].font_pause = cur_dev_font->font_pause;
        if( cur_dev_font->resident == 0 ) {
            wgml_fonts[i].font_resident = 'n';
        } else {
            wgml_fonts[i].font_resident = 'y';
        }
        if( wgml_fonts[i].bin_font->outtrans == NULL ) {
            wgml_fonts[i].outtrans = bin_device->outtrans;
        } else {
            wgml_fonts[i].outtrans = wgml_fonts[i].bin_font->outtrans;
        }

        /* If scale_basis is not "0", then font_height must not be "0". */

        if( (wgml_fonts[i].bin_font->scale_basis != 0)
          && (wgml_fonts[i].font_height == 0)) {
            xx_simple_err_exit_i( ERR_FONT_OPT_SCALED, i );
            /* never return */
        }

        compute_metrics( &wgml_fonts[i] );
    }
    free_opt_fonts();

    /* Generate any entries required by the BOX and/or UNDERSCORE blocks.
     * Note that the font number will become non-zero and will be used in
     * document processing instead of the font name. If the device is PS,
     * then gen_cnt will be "0" and no fonts will be generated.
     */

    switch( gen_cnt ) {
    case 0:
        break;
    case 1:
        if( bin_device->underscore.font_name != NULL ) {
            i = font_base;
            bin_device->underscore.font = font_base;
            wgml_fonts[i].bin_font = find_cop_font( bin_device->underscore.font_name );
            wgml_fonts[i].font_style = find_style( "plain" );
            wgml_fonts[i].font_height = 0;
            wgml_fonts[i].font_space = 0;
            cur_dev_font = find_dev_font( bin_device->underscore.font_name );
            if( cur_dev_font->font_switch == NULL ) {
                wgml_fonts[i].font_switch = NULL;
            } else {
                wgml_fonts[i].font_switch = find_switch( cur_dev_font->font_switch );
            }
            wgml_fonts[i].font_pause = cur_dev_font->font_pause;
            wgml_fonts[i].default_width = 1;
            wgml_fonts[i].line_height = 1;
            wgml_fonts[i].line_space = 0;
            wgml_fonts[i].font_resident = 'n';

            /* If scale_basis is not "0", then font_height must not be "0". */

            if( (wgml_fonts[i].bin_font->scale_basis != 0)
              && (wgml_fonts[i].font_height == 0) ) {
                xx_simple_err_exit_i( ERR_FONT_SCALED, i );
                /* never return */
            }

            break;
        }
        if( bin_device->box.font_name != NULL ) {
            i = font_base;
            bin_device->box.font = font_base;
            wgml_fonts[i].bin_font = find_cop_font( bin_device->box.font_name );
            wgml_fonts[i].font_style = find_style( "plain" );
            wgml_fonts[i].font_height = 0;
            wgml_fonts[i].font_space = 0;
            cur_dev_font = find_dev_font( bin_device->box.font_name );
            if( cur_dev_font->font_switch == NULL ) {
                wgml_fonts[i].font_switch = NULL;
            } else {
                wgml_fonts[i].font_switch = find_switch( cur_dev_font->font_switch );
            }
            wgml_fonts[i].font_pause = cur_dev_font->font_pause;
            wgml_fonts[i].default_width = 1;
            wgml_fonts[i].line_height = 1;
            wgml_fonts[i].line_space = 0;
            wgml_fonts[i].font_resident = 'n';
            break;
        }
        break;
    case 2:
        if( bin_device->underscore.font_name != NULL ) {
            i = font_base;
            bin_device->underscore.font = font_base;
            wgml_fonts[i].bin_font = find_cop_font( bin_device->underscore.font_name );
            wgml_fonts[i].font_style = find_style( "plain" );
            wgml_fonts[i].font_height = 0;
            wgml_fonts[i].font_space = 0;
            cur_dev_font = find_dev_font( bin_device->underscore.font_name );
            if( cur_dev_font->font_switch == NULL ) {
                wgml_fonts[i].font_switch = NULL;
            } else {
                wgml_fonts[i].font_switch = find_switch( cur_dev_font->font_switch );
            }
            wgml_fonts[i].font_pause = cur_dev_font->font_pause;
            wgml_fonts[i].default_width = 1;
            wgml_fonts[i].line_height = 1;
            wgml_fonts[i].line_space = 0;
            wgml_fonts[i].font_resident = 'n';

            /* If scale_basis is not "0", then font_height must not be "0". */

            if( (wgml_fonts[i].bin_font->scale_basis != 0)
              && (wgml_fonts[i].font_height == 0) ) {
                xx_simple_err_exit_i( ERR_FONT_SCALED, i );
                /* never return */
            }
        }
        if( bin_device->box.font_name != NULL ) {
            font_base++;
            i = font_base;
            bin_device->box.font = font_base;
            wgml_fonts[i].bin_font = find_cop_font( bin_device->box.font_name );
            wgml_fonts[i].font_style = find_style( "plain" );
            wgml_fonts[i].font_height = 0;
            wgml_fonts[i].font_space = 0;
            cur_dev_font = find_dev_font( bin_device->box.font_name );
            if( cur_dev_font->font_switch == NULL ) {
                wgml_fonts[i].font_switch = NULL;
            } else {
                wgml_fonts[i].font_switch = find_switch( cur_dev_font->font_switch );
            }
            wgml_fonts[i].font_pause = cur_dev_font->font_pause;
            wgml_fonts[i].default_width = 1;
            wgml_fonts[i].line_height = 1;
            wgml_fonts[i].line_space = 0;
            wgml_fonts[i].font_resident = 'n';
        }
        break;
    default:
        internal_err_exit( __FILE__, __LINE__ );
        /* never return */
    }

    /* Ensure that font 0 was initialized */

    if( wgml_fonts[FONT0].bin_font == NULL ) {
        internal_err_exit( __FILE__, __LINE__ );
        /* never return */
    }

    /* Fill in any skipped entries with the values used for wgml_font 0 */

    def_font.bin_font = wgml_fonts[FONT0].bin_font;
    def_font.font_switch = wgml_fonts[FONT0].font_switch;
    def_font.font_pause = wgml_fonts[FONT0].font_pause;
    def_font.font_style = wgml_fonts[FONT0].font_style;
    def_font.font_height = wgml_fonts[FONT0].font_height;
    def_font.font_space = wgml_fonts[FONT0].font_space;
    def_font.default_width = wgml_fonts[FONT0].default_width;
    def_font.line_height = wgml_fonts[FONT0].line_height;
    def_font.line_space = wgml_fonts[FONT0].line_space;
    def_font.font_resident = wgml_fonts[FONT0].font_resident;

    for( i = 0; i < wgml_font_cnt; i++ ) {
        if( wgml_fonts[i].bin_font == NULL ) {
            wgml_fonts[i].bin_font = def_font.bin_font;
            wgml_fonts[i].font_switch = def_font.font_switch;
            wgml_fonts[i].font_pause = def_font.font_pause;
            wgml_fonts[i].font_style = def_font.font_style;
            wgml_fonts[i].font_height = def_font.font_height;
            wgml_fonts[i].font_space = def_font.font_space;
            wgml_fonts[i].default_width = def_font.default_width;
            wgml_fonts[i].line_height = def_font.line_height;
            wgml_fonts[i].line_space = def_font.line_space;
            wgml_fonts[i].font_resident = def_font.font_resident;
        }
    }

    /* Ensure that at least one binary FONT block was processed. */

    if( bin_fonts == NULL ) {
        internal_err_exit( __FILE__, __LINE__ );
        /* never return */
    }

    /* Initialize items dependent on the device library. */

    /* Initialize the width table entries. This is done here because it is
     * not clear whether or not a more efficient method is needed. Note that
     * all wgml_font instances will have a valid table, and that different
     * wgml_font instances may have identical tables. However, since the
     * conversion takes font_height into account, tables based on the same
     * cop_font may still differ. Also set the base values for the "Em" and
     * "Device Unit" ("DV") Horizontal Space Units, and record the width of
     * the space character for quick reference. It is not clear if this is
     * actually necessary, but it is a bit faster than using the width table
     * directly.
     */

    for( i = 0; i < wgml_font_cnt; i++ ) {
        if( wgml_fonts[i].bin_font->width == NULL ) {
            for( j = 0; j < 0x100; j++ ) {
                wgml_fonts[i].width.table[j] = wgml_fonts[i].default_width;
            }
        } else {
            if( wgml_fonts[i].bin_font->scale_basis == 0 ) {
                for( j = 0; j < 0x100; j++ ) {
                    wgml_fonts[i].width.table[j] = wgml_fonts[i].bin_font->width->table[j];
                }
            } else {
                for( j = 0; j < 0x100; j++ ) {
                    wgml_fonts[i].width.table[j] =
                        scale_basis_to_horizontal_base_units( wgml_fonts[i].bin_font->width->table[j], &wgml_fonts[i] );
                }
            }
        }
        wgml_fonts[i].em_base = wgml_fonts[i].width.table['M'];
        wgml_fonts[i].spc_width = wgml_fonts[i].width.table[' '];
    }

    /* Initialize the column width used with BX. */

    box_col_width = bin_device->horizontal_base_units / CPI;

    /* Initialize the horizontal offset used to position the HLINE output. */

    h_vl_offset = bin_device->horizontal_base_units / 20;

    /* Initialize the default tabs and related extern variables. */

    tab_col = bin_device->horizontal_base_units / CPI10;    // wgml 4.0 uses 10 cpi here regardless of what the user wants
    first_tab = (6 * tab_col) - 1;
    inter_tab = 5 * tab_col;

    def_tabs.tabs = mem_alloc( TAB_COUNT * sizeof( tab_stop ) );
    def_tabs.length = TAB_COUNT;
    def_tabs.tabs[0].column = first_tab;
    def_tabs.tabs[0].fill_char = ' ';
    def_tabs.tabs[0].alignment = ALIGN_left;
    for( i = 1; i < def_tabs.length; i++ ) {
        def_tabs.tabs[i].column = def_tabs.tabs[i - 1].column + inter_tab;
        def_tabs.tabs[i].fill_char = ' ';
        def_tabs.tabs[i].alignment = ALIGN_left;
    }
    def_tabs.current = def_tabs.length;

    /* Initialize user_tabs. */

    user_tabs.tabs = mem_alloc( TAB_COUNT * sizeof( tab_stop ) );
    user_tabs.length = TAB_COUNT;
    user_tabs.current = 0;

    /* Initialize the dependent modules. */

    df_setup();
    ob_setup();

    return;
}

/* Function cop_teardown().
 * Frees all memory allocated by cop_setup().
 * This is where making cop_device, cop_driver and cop_font monolithic blocks of
 * allocated memory pays off!
 */

void cop_teardown( void )
{
    cop_font    *   old;

    if( bin_device != NULL ) {
        mem_free( bin_device );
        bin_device = NULL;
    }

    if( bin_driver  != NULL ) {
        mem_free( bin_driver );
        bin_driver = NULL;
    }

    while( bin_fonts != NULL) {
        old = bin_fonts;
        bin_fonts = bin_fonts->next_font;
        mem_free( old );
    }

    if( wgml_fonts != NULL) {
        mem_free( wgml_fonts );
        wgml_fonts = NULL;
    }

    if( cur_token != NULL ) {
        if( cur_token->text != NULL ) {
            mem_free( cur_token->text);
            cur_token->text = NULL;
        }
        mem_free( cur_token );
        cur_token = NULL;
    }

    /* Free def_tabs.tabs. */

    if( def_tabs.tabs != NULL ) {
        mem_free( def_tabs.tabs );
        def_tabs.tabs = NULL;
    }

    /* Free user_tabs.tabs. */

    if( user_tabs.tabs != NULL ) {
        mem_free( user_tabs.tabs );
        user_tabs.tabs = NULL;
    }

    /* Release any memory allocated by the dependent modules. */

    df_teardown();
    ob_teardown();

    return;
}

/* Function cop_text_width().
 * Returns the width, in horizontal_base_units, of the text given.
 *
 * Parameters:
 *      text is a pointer to the first character.
 *      count is the number of characters.
 *      font is the font number of the available font to use.
 *
 * Returns:
 *      The sum of the widths of the count characters starting with *text.
 *
 * Note:
 *      This version simply adds up the widths, in horizontal_base_units, of
 *          the first count characters in text. For FONT blocks without
 *          a WIDTH block, it might be more efficient to use the
 *          product of default_width and count. However, many counts will be
 *          quite small, and the cost of determining whether or not the
 *          bin_font contains a width table must be considered.
 */

unsigned cop_text_width( const char *text, unsigned count, font_number font )
{
    unsigned        i;
    unsigned        width;

    CHECK_FONT( font );

    /* Compute the width. */

    width = 0;
    for( i = 0; i < count; i++ ) {
        width += wgml_fonts[font].width.table[(unsigned char)text[i]];
    }

    return( width );
}

/***************************************************************************/
/* update ti_table as specified by the data                                */
/***************************************************************************/

void cop_ti_table( const char *p )
{
    bool            first_found;
    bool            no_data;
    const char      *pa;
    int             i;
    char            token_char;
    char            first_char;
    unsigned        len;
    char            cwcurr[4];

    first_char = '\0';
    cwcurr[0] = SCR_char;
    cwcurr[1] = 't';
    cwcurr[2] = 'i';
    cwcurr[3] = '\0';

    // if there is no data, then the table will be reset
    first_found = false;
    no_data = true;

    pa = p;
    SkipNonSpaces( p );         // end of word
    len = p - pa;

    if( len > 0 ) {
        if( len > 2 ) { // check for ".ti set"
            if( len == 3
              && strnicmp( "SET", pa, len ) == 0 ) {
                SkipSpaces( p );        // set char start
                pa = p;
                SkipNonSpaces( p );     // set char start
                len = p - pa;
                if( len == 0 ) {        // no set char: set to ' '
                    ProcFlags.in_trans = false;
                    in_esc = ' ';
                } else if( len > 1 ) {  // hex digits are not allowed here
                    xx_line_err_exit_ci( ERR_CHAR_ONLY, pa, len );
                    /* never return */
                } else {
                    ProcFlags.in_trans = true;
                    in_esc = *pa;
                }

                SkipSpaces( p );        // text or '\0'
                pa = p;
                SkipNonSpaces( p );     // set char start
                len = p - pa;
                if( len > 0 ) {         // additional text not allowed
                    xx_line_err_exit_ci( ERR_CHAR_ONLY, pa, len );
                    /* never return */
                }
                return;     // done if was ".ti set"
            }
            xx_line_err_exit_cci( ERR_XX_OPT, cwcurr, pa, len );
            /* never return */
        }

        // not .ti set: get pairs of chars
        if( len > 0 ) {
            first_char = parse_char( pa, len );  // first or only char
            first_found = true;
            no_data = false;
        }
        while( *p != '\0' ) {
            SkipSpaces( p );            // next char start
            pa = p;
            SkipNonSpaces( p );         // next char start
            len = p - pa;

            if( len == 0 )
                break;                  // exit loop if no next char

            token_char = parse_char( pa, len );
            no_data = false;

            if( first_found ) {     // we now have two chars
                ti_table[(unsigned char)first_char] = token_char;
                first_found = false;
            } else {                // we found a first or only char
                first_char = token_char;
                first_found = true;
            }
        }
    }

    if( first_found ) {     // an only char, set table so it returns itself
        ti_table[(unsigned char)first_char] = first_char;
    }

    if( no_data ) {         // reset the table if no_data is still true
        for( i = 0; i < 0x100; i++ ) {
            ti_table[i] = i;
        }
    }

    return;
}

/* Function fb_dbox().
 * Interprets the DBOX block.
 *
 * Parameters:
 *      in_dbox points to a dbox_element
 *
 * Prerequisites:
 *      The DBOX block must exist.
 *      The ABSOLUTEADDRESS block must exist.
 *
 * Notes:
 *      The ABSOLUTEADDRESS block is required to position the print to the
 *          start of the line or box.
 *      The DBOX block must exist because the box-drawing code should be
 *          checking this and drawing the box using either HLINE and VLINE
 *          or the BOX block characters instead.
 */

void fb_dbox( dbox_element * in_dbox )
{
    fb_line_block( &(bin_driver->dbox), in_dbox->h_start, in_dbox->v_start,
                   in_dbox->h_len, in_dbox->v_len, false );
    return;
}

/* Function fb_document().
 * Performs the first document pass output seen when document processing begins.
 */

void fb_document( void )
{
    /* Interpret the DOCUMENT PAUSE block. */

    if( bin_device->pauses.document_pause != NULL )
        df_interpret_device_functions( bin_device->pauses.document_pause->text );

    /* Interpret the DOCUMENT INIT block. */

    if( bin_driver->inits.document != NULL )
        fb_init( bin_driver->inits.document );

    /* Perform the virtual %enterfont(0). */

    fb_enterfont();

    /* Perform the other actions needed at this point. */

    df_populate_driver_table();

    /* Set up for the first document page. */

    df_start_page();

    return;
}

/* Function fb_document_page().
 * Interprets the NEWPAGE block and increments the page number variable.
 *
 * Note:
 *      This function should be used for new document pages within a section,
 *      except as documented in the Wiki. The function fb_new_section() should
 *      be used at the start of each new section, and elsewhere as documented
 *      in the Wiki.
 */

void fb_document_page( void )
{
    /* Interpret a LINEPROC ENDVALUE block if appropriate. */

    fb_lineproc_endvalue();

    /* Interpret the NEWPAGE block. */

    df_interpret_driver_functions( bin_driver->newpage.text );

    /* Interpret the DOCUMENT_PAGE PAUSE block. */

    if( bin_device->pauses.docpage_pause != NULL )
        df_interpret_device_functions( bin_device->pauses.docpage_pause->text );

    /* Set up for a new document page. */

    df_start_page();

    return;
}

/* Function fb_finish().
 * Performs the processing which occurs after document processing ends.
 */

void fb_finish( void )
{
    /* Interpret a LINEPROC ENDVALUE block if appropriate. */

    fb_lineproc_endvalue();

    /* If the END FINISH block is present, interpret it. If the END
     * FINISH block is not present, then, if the DOCUMENT FINISH block
     * is present, interpret it.
     */

    if( bin_driver->finishes.end != NULL ) {
        df_interpret_driver_functions( bin_driver->finishes.end->text );
    } else if( bin_driver->finishes.document != NULL ) {
        df_interpret_driver_functions( bin_driver->finishes.document->text );
    }

    return;
}

/* Function fb_hline().
 * Interprets the HLINE block.
 *
 * Parameters:
 *      in_hline contains a pointer to an hline_element
 *
 * Prerequisites:
 *      The HLINE block must exist.
 *      The ABSOLUTEADDRESS block must exist.
 *
 * Notes:
 *      The ABSOLUTEADDRESS block is required to position the print to the
 *          start of the line or box.
 *      The HLINE block must exist because the box-drawing code should be
 *          checking this and, in some cases, drawing the line using the BOX
 *          block characters instead.
 */

void fb_hline( hline_element * in_hline )
{
    fb_line_block( &(bin_driver->hline), in_hline->h_start, in_hline->v_start,
                   in_hline->h_len, 0, false );
}

/* Function fb_output_textline.
 * Sends the text_line passed to the device via the output buffer.
 *
 * Parameter:
 *      out_line points to a text_line instance specifying the text to be
 *          sent to the device.
 *
 * Note:
 *      This function deals with the normal output sequence only.
 *      It is expected that lines drawn with BOX characters will require
 *          other, specialized fb_output_ functions.
 */

void fb_output_textline( text_line * out_line )
{
    int             i;
    text_chars  *   current;
    uint16_t        line_passes;

    /* Ensure out_line has at least one text_chars instance. */

    if( out_line->first == NULL ) {
        fb_empty_text_line( out_line );
        return;
    }

    /* Determine the maximum number of passes. */

    line_passes = 0;
    for( current = out_line->first; current != NULL; current = current->next ) {
        CHECK_FONT( current->font );
        if( wgml_fonts[current->font].font_style->line_passes > line_passes ) {
            line_passes = wgml_fonts[current->font].font_style->line_passes;
        }
    }

    /* Do the first pass. */

    fb_first_text_line_pass( out_line );

    /* Do the subsequent passes */

    for( i = 1; i < line_passes; i ++ ) {
        fb_subsequent_text_line_pass( out_line, i );
    }

    return;
}

/* Function fb_start().
 * Performs the processing which occurs before document processing starts.
 * Indeed, wgml 4.0 does not even look for the document specification file
 * until the actions taken here have been completed!
 */

void fb_start( void )
{
    /* Interpret the START PAUSE block and reset the function table. */

    if( bin_device->pauses.start_pause != NULL )
        df_interpret_device_functions( bin_device->pauses.start_pause->text );

    df_populate_device_table();

    /* Interpret the START INIT block. */

    if( bin_driver->inits.start != NULL )
        fb_init( bin_driver->inits.start );

    return;
}

/* Function fb_vline().
 * Interprets the VLINE block.
 *
 * Parameters:
 *      in_vline is a pointer to a vline_element
 *
 * Prerequisites:
 *      The VLINE block must exist.
 *      The ABSOLUTEADDRESS block must exist.
 *
 * Notes:
 *      The ABSOLUTEADDRESS block is required to position the print to the
 *          start of the line or box.
 *      The VLINE block must exist because the box-drawing code should be
 *          checking this and, in some cases, drawing the line using the BOX
 *          block characters instead.
 */

void fb_vline( vline_element * in_vline )
{
    fb_line_block( &(bin_driver->vline), in_vline->h_start, in_vline->v_start, 0,
                   in_vline->v_len, in_vline->twice );
    return;
}

void init_record_buffer( record_buffer *recb, unsigned size )
{
    recb->current = 0;
    recb->size = size;
    recb->text = mem_alloc( size + 1 );
}

void resize_record_buffer( record_buffer *recb, unsigned size )
{
    if( size > recb->size ) {
        while( recb->size < size )
            recb->size *= 2;
        recb->text = mem_realloc( recb->text, recb->size + 1 );
    }
    recb->current = size;
}

void init_record_buffer_fill( record_buffer *recb, unsigned size, unsigned char byte )
{
    recb->current = 0;
    recb->size = size;
    recb->text = mem_alloc( size + 1 );
    memset( recb->text, byte, size );
    recb->text[size] = '\0';
}

void resize_record_buffer_fill( record_buffer *recb, unsigned size, unsigned char byte )
{
    if( size > recb->size ) {
        recb->text = mem_realloc( recb->text, size + 1 );
        memset( recb->text, byte, size );
        recb->text[size] = '\0';
    }
}
