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
* Description:  Implements the functions declared in devfuncs.h:
*                   df_interpret_device_functions()
*                   df_interpret_driver_functions()
*                   df_populate_device_table()
*                   df_populate_driver_table()
*                   df_setup()
*                   df_start_page()
*                   df_teardown()
*                   fb_binclude_support()
*                   fb_empty_text_line()
*                   fb_enterfont()
*                   fb_first_text_line_pass()
*                   fb_graphic_support()
*                   fb_init()
*                   fb_line_block()
*                   fb_lineproc_endvalue()
*                   fb_subsequent_text_line_pass()
*               and these functions declared in wgml.h:
*                   fb_absoluteaddress()
*                   fb_new_section()
*                   fb_position()
*               as well as a macro:
*                   MAX_FUNC_INDEX
*               some local typedefs and structs:
*                   df_data
*                   df_function
*                   page_state
*                   parameters
*               quite a few local variables:
*                   active_font
*                   at_start
*                   current_df_data
*                   current_function
*                   current_function_table
*                   current_state
*                   date_val
*                   desired_state
*                   device_function_table
*                   driver_function_table
*                   df_font
*                   has_htab
*                   htab_done
*                   line_pass_number
*                   pages
*                   page_start
*                   shift_done
*                   space_chars
*                   tab_width
*                   text_out_open
*                   textpass
*                   thickness
*                   time_val
*                   uline
*                   uscore_char
*                   uscore_chars
*                   wgml_header
*                   x_address
*                   x_size
*                   y_address
*                   y_size
*               and a large number of local functions:
*                   char_convert()
*                   char_literal()
*                   df_add()
*                   df_bad_code()
*                   df_binary()
*                   df_cancel()
*                   df_clearpc()
*                   df_date()
*                   df_decimal()
*                   df_default_width()
*                   df_divide()
*                   df_do_nothing_char()
*                   df_do_nothing_num()
*                   df_dotab()
*                   df_endif()
*                   df_enterfont()
*                   df_font_height()
*                   df_font_number()
*                   df_font_outname1()
*                   df_font_outname2()
*                   df_font_resident()
*                   df_font_space()
*                   df_flushpage()
*                   df_getstrsymbol()
*                   df_getnumsymbol()
*                   df_hex()
*                   df_ifeqn()
*                   df_ifnen()
*                   df_ifeqs()
*                   df_ifnes()
*                   df_image_driver()
*                   df_line_height()
*                   df_line_space()
*                   df_lower()
*                   df_out_text_device()
*                   df_page_depth()
*                   df_page_width()
*                   df_pages()
*                   df_recordbreak_device()
*                   df_recordbreak_driver()
*                   df_remainder()
*                   df_setsymbol()
*                   df_sleep()
*                   df_subtract()
*                   df_tab_width()
*                   df_text_driver()
*                   df_textpass()
*                   df_thickness()
*                   df_time()
*                   df_ulineoff()
*                   df_ulineon()
*                   df_wait()
*                   df_wgml_header()
*                   df_x_address()
*                   df_x_size()
*                   df_y_address()
*                   df_y_size()
*                   fb_first_text_chars()
*                   fb_firstword()
*                   fb_font_switch()
*                   fb_htab()
*                   fb_initial_horizontal_positioning()
*                   fb_internal_horizontal_positioning()
*                   fb_new_font_text_chars()
*                   fb_newline()
*                   fb_normal_vertical_positioning()
*                   fb_overprint_vertical_positioning()
*                   fb_subsequent_text_chars()
*                   get_parameters()
*                   interpret_functions()
*                   numeric_literal()
*                   out_text_driver()
*                   output_spaces()
*                   output_uscores()
*                   post_text_output()
*                   pre_text_output()
*                   process_parameter()
*                   skip_functions()
*
* Notes:        The Wiki should be consulted for any term whose meaning is
*               not apparent. This should help in most cases.
*
*               Information on the device functions can be found in the
*               Wiki.
****************************************************************************/

#include "wgml.h"
#include "wio.h"
#include "copfunc.h"
#include "devfuncs.h"
#include "outbuff.h"

#include "clibext.h"

/* Macros. */

/* Maximum index value for the function tables. */

#define MAX_FUNC_INDEX 0x3c

/* Local struct definitions. */

/* This keeps track of where we are in the compiled function block. */

typedef struct {
    const char      *base;
    const char      *current;
    bool            last_function_done;
    unsigned char   parameter_type;
    unsigned char   df_code;
} df_data;

/* This is used to record the state of the current page. */

typedef struct {
    uint32_t    x_address;
    uint32_t    y_address;
    font_number font;
    text_type   type;
} page_state;

/* Holds the offsets to the parameter(s). */

typedef struct {
    uint16_t    first;
    uint16_t    second;
} parameters;

/* Local typedef. */

/* All functions used in the function tables will use this signature. */

typedef void *(*df_function)(void);

/* Local data declarations. */

/* These are set by device functions and for use in outputting text. */

static bool             textpass                = false;
static bool             uline                   = false;

/* These are used to control some aspects of device function operation. */

static bool             at_start                = true;
static bool             htab_done               = false;
static bool             page_start              = false;
static bool             shift_done              = false;
static page_state       current_state           = { 0, 0, 0 };
static page_state       desired_state           = { 0, 0, 0 };
static uint32_t         line_pass_number        = 0;

/* These are used to hold values returned by device functions. */

static char             *date_val               = NULL;
static char             *time_val               = NULL;
static char             wgml_header[]           = "V4.0 PC/DOS";
static font_number      df_font                 = 0;
static uint32_t         tab_width               = 0;
static uint32_t         thickness               = 0;
static uint32_t         x_address               = 0;
static uint32_t         x_size                  = 0;
static uint32_t         y_address               = 0;
static uint32_t         y_size                  = 0;

/* These are used by the interpreter. */
static bool             has_htab                = false;
static bool             text_out_open           = false;
static font_number      active_font             = 0;
static df_data          current_df_data;
static df_function      device_function_table[MAX_FUNC_INDEX + 1];
static df_function      driver_function_table[MAX_FUNC_INDEX + 1];
static df_function      *current_function_table = NULL;
static const char       *current_function       = NULL;

/* These are used in outputting spaces and underscore characters. */

static char             uscore_char;
static record_buffer    space_chars     = { 0, 0, NULL };
static record_buffer    uscore_chars    = { 0, 0, NULL };

/* Local function definitions. */

/* Function fb_newline().
 * Uses the various :NEWLINE blocks to actually position the device to the
 * desired vertical position.
 *
 * Prerequisite:
 *      The :ABSOLUTEADDRESS block must not be defined.
 */

static void fb_newline( void )
{
    int             i;
    newline_block   *current_block   = NULL;
    int16_t         desired_units;
    int16_t         desired_lines;
    uint16_t        remainder;
    uint16_t        max_advance;

    /* Interpret a :LINEPROC :ENDVALUE block if appropriate. */

    fb_lineproc_endvalue();

    /* desired_units holds the number of vertical base units to be moved. */

    if( bin_driver->y_positive == 0x00 ) {

        /* desired_state.y_address was formed by subtraction. */

        desired_units = current_state.y_address - desired_state.y_address;
    } else {

        /* desired_state.y_address was formed by addition. */

        desired_units = desired_state.y_address - current_state.y_address;
    }

    /* Devices using :ABSOLUTEADDRESS may be able to move backwards, but
     * :NEWLINE blocks with negative values for advance are not accepted
     * by gendev and so negative values of lines must be an error.
     */

    if( desired_units < 0 ) {
        internal_err( __FILE__, __LINE__ );
    }

    /* desired_lines contains the number of lines, rounded up. Note: the
     * actual rounding algorithm used by wgml 4.0 has yet to be determined;
     * the rounding used here may require adjustment in the future.
     */

    desired_lines = desired_units / wgml_fonts[active_font].line_height;
    remainder = desired_lines % wgml_fonts[active_font].line_height;
    if( 2 * remainder >= wgml_fonts[active_font].line_height )
        desired_lines++;

    /* Devices using :ABSOLUTEADDRESS may be able to use partial line heights,
     * but devices using :NEWLINE blocks must advance at least one whole line
     * if any advance was called for. This is treated as an error because the
     * page layout code should not be cutting things this close for devices
     * which rely on :NEWLINE blocks.
     */

    if( ( desired_units > 0 ) && ( desired_lines == 0 ) ) {
        internal_err( __FILE__, __LINE__ );
    }

    /* lines might equal 0, in which case no action is needed. */

    while( desired_lines > 0 ) {
        max_advance = 0;
        for( i = 0; i < bin_driver->newlines.count; i++ ) {
            if( bin_driver->newlines.newlineblocks[i].advance <= desired_lines ) {
                if( max_advance < bin_driver->newlines.newlineblocks[i].advance ) {
                    max_advance = bin_driver->newlines.newlineblocks[i].advance;
                    current_block = &bin_driver->newlines.newlineblocks[i];
                }
            }
        }
        df_interpret_driver_functions( current_block->text );
        desired_lines -= max_advance;
    }

    /* If this is the Initial Vertical Positioning, interpret the :LINEPROC
     * :ENDVALUE block for line pass 1 of available font 0, unless it has
     * already been done. Note: this places the block at the proper location
     * when one or more :LINEPROC blocks have been interpreted.
     */

    if( at_start ) {
        if( wgml_fonts[0].font_style->lineprocs != NULL ) {
            if( wgml_fonts[0].font_style->lineprocs[0].endvalue != NULL ) {
                df_interpret_driver_functions( wgml_fonts[0].font_style->lineprocs[0].endvalue->text );
            }
        }
        at_start = false;
    }

    /* Update the state to reflect the new position. */

    current_state.y_address = desired_state.y_address;

    return;
}

/* Function char_convert().
 * This function returns a dynamically-allocated copy of the parameter. If the
 * parameter is a NULL pointer, then it returns a pointer to an empty string.
 *
 * Parameter:
 *      a pointer to the alleged character value.
 *
 * Returns:
 *      a pointer to a dynamically-allocated string.
 */

static char *char_convert( const char *in_val )
{
    char    *ret_val = NULL;

    if( in_val == NULL ) {
        ret_val = mem_alloc( 1 );
        ret_val[0] = '\0';
    } else {
        ret_val = mem_alloc( strlen( in_val ) + 1 );
        strcpy( ret_val, in_val );
    }

    return( ret_val );
}

/* Function output_spaces().
 * This function inserts the spaces, if any, needed to move the print head
 * from its current position to its desired position into the output buffer.
 *
 * Parameter:
 *      count contains the number of spaces to emit.
 *
 * Note:
 *      The calling function is responsible for detecting and handling
 *          cases in which the desired horizontal space cannot be treated
 *          as an exact multiple of the width of a space character.
 */

static void output_spaces( size_t count )
{
    size_t  i;

    if( space_chars.length < count ) {
        space_chars.text = mem_realloc( space_chars.text, count );
        space_chars.length = count;
        for( i = 0; i < space_chars.length; i++ ) {
            space_chars.text[i] = ' ';
        }
    }

    if( !text_out_open && ProcFlags.ps_device ) {
        ob_insert_ps_text_start();
        text_out_open = true;
    }
    ob_insert_block( space_chars.text, count, true, true, active_font );
    current_state.x_address = desired_state.x_address;

    return;
}

/* Function output_uscores().
 * This function inserts the underscore characters.
 *
 * Parameter:
 *      in_chars points to the current text_chars instance
 */

static void output_uscores( text_chars *in_chars )
{
    int         i;
    uint32_t    count;
    uint32_t    uscore_width;

    /* Undersore characters cannot be emitted "backwards". */

    if( current_state.x_address > desired_state.x_address ) {
        internal_err( __FILE__, __LINE__ );
    }

    /* This is simplified: since no known device specifies a font for use
     * with the underscore character, the current font is used.
     */

    uscore_width = wgml_fonts[in_chars->font].width.table[(unsigned char)uscore_char];

    /* The number of underscore characters is determined by the amount of
     * space from the current position to the text start point plus the
     * width of the text, divided by the width of the underscore character.
     */

    count = desired_state.x_address - current_state.x_address;
    count += in_chars->width;
    count /= uscore_width;

    if( uscore_chars.length < count ) {
        uscore_chars.text = mem_realloc( uscore_chars.text, count );
        uscore_chars.length = count;
        for( i = 0; i < uscore_chars.length; i++ ) {
            uscore_chars.text[i] = uscore_char;
        }
    }

    ob_insert_block( uscore_chars.text, count, true, true, active_font );
    current_state.x_address = desired_state.x_address;

    return;
}

/* Function post_text_output().
 * Performs any processing needed to terminate text output.
 *
 * Global Changed:
 *      htab_done is set to false.
 *      text_out_open is set to false.
 */

static void post_text_output( void )
{
    char    shift_neg[]     = " neg ";
    char    shift_rmoveto[] = "0 exch rmoveto ";
    char    shift_scale[]   = " 1 .7 div dup scale";
    size_t  ps_size;

    if( ProcFlags.ps_device ) {
        if( shift_done ) {

            /* Emit the appropriate post-subscript/superscript sequence. */

            switch( current_state.type ) {
            case sub:
                ps_size = strlen( shift_scale );
                ob_insert_block( shift_scale, ps_size, false, false, active_font );

                ps_size = strlen( shift_rmoveto );
                ob_insert_block( shift_rmoveto, ps_size, false, false, active_font );
                break;
            case sup:
                ps_size = strlen( shift_scale );
                ob_insert_block( shift_scale, ps_size, false, false, active_font );

                ps_size = strlen( shift_neg );
                ob_insert_block( shift_neg, ps_size, false, false, active_font );

                ps_size = strlen( shift_rmoveto );
                ob_insert_block( shift_rmoveto, ps_size, false, false, active_font );
                break;
            case norm:
                /* Since shift_done was true, norm is not allowed. */
            default:
                internal_err( __FILE__, __LINE__ );
            }
            current_state.type = norm;
        }

    }

    htab_done = false;
    shift_done = false;
    text_out_open = false;

    return;
}

/* Function pre_text_output().
 * Performs any processing needed to start text output.
 *
 * Global Changed:
 *      text_out_open is set to true.
 */

static void pre_text_output( void )
{
    char    shift_neg[]     = " neg";
    char    shift_rmoveto[] = " rmoveto";
    char    shift_scale[]   = " .7 .7 scale ";
    size_t  ps_size;

    if( ProcFlags.ps_device ) {

        if( current_state.type != desired_state.type ) {

            /* Emit the appropriate post-subscript/superscript sequence. */

            switch( desired_state.type ) {
            case norm:
                shift_done = false;
                break;
            case sub:
                ob_insert_block( " ", 1, false, false, active_font );

                ps_size = wgml_fonts[active_font].shift_count;
                ob_insert_block( wgml_fonts[active_font].shift_height, ps_size, false, false, active_font );

                ob_insert_block( " 0 ", 3, false, false, active_font );

                ps_size = wgml_fonts[active_font].shift_count;
                ob_insert_block( wgml_fonts[active_font].shift_height, ps_size, false, false, active_font );

                ps_size = strlen( shift_neg );
                ob_insert_block( shift_neg, ps_size, false, false, active_font );

                ps_size = strlen( shift_rmoveto );
                ob_insert_block( shift_rmoveto, ps_size, false, false, active_font );

                ps_size = strlen( shift_scale );
                ob_insert_block( shift_scale, ps_size, false, false, active_font );

                shift_done = true;
                break;
            case sup:
                ob_insert_block( " ", 1, false, false, active_font );

                ps_size = wgml_fonts[active_font].shift_count;
                ob_insert_block( wgml_fonts[active_font].shift_height, ps_size, false, false, active_font );

                ob_insert_block( " 0 ", 3, false, false, active_font );

                ps_size = wgml_fonts[active_font].shift_count;
                ob_insert_block( wgml_fonts[active_font].shift_height, ps_size, false, false, active_font );

                ps_size = strlen( shift_rmoveto );
                ob_insert_block( shift_rmoveto, ps_size, false, false, active_font );

                ps_size = strlen( shift_scale );
                ob_insert_block( shift_scale, ps_size, false, false, active_font );

                shift_done = true;
                break;
            default:
                internal_err( __FILE__, __LINE__ );
            }
        }
    }
    current_state.type = desired_state.type;

    return;
}

/* Function df_do_nothing_char().
 * Returns an empty string. Used in the function tables for device functions
 * that are not allowed to do anything when used in the blocks with which that
 * function table is used.
 */

static void *df_do_nothing_char( void )
{
    return( (void *)char_convert( NULL ) );
}

/* Function df_do_nothing_num().
 * Returns the number 0. Used in the function tables for device functions that
 * are not allowed to do anything when used in the blocks with which that
 * function table is used.
 */

static void *df_do_nothing_num( void )
{
    return( (void *)0 );
}

/* Function df_bad_code().
 * Reports byte codes not known to exist but nonetheless found. It never
 * returns to the caller. It should never actually run. Since it is used
 * in the function tables, it must conform to the function typedef.
 */

static void *df_bad_code( void )
{
    internal_err( __FILE__, __LINE__ );
    return( NULL );
}

/* These functions are for device functions which take no parameters. */

/* Type I device functions have an associated parameter type byte, which should
 * always be "0x00" for the functions in this section.
 */

/* Function df_clearpc().
 * Implements device function %clearpc(). This is one of several
 * solutions suggested on the developer's newsgroup. Better solutions
 * may exist. The Linux solution is based on information on what does
 * and does not work in the developer's newsgroup.
 */

static void *df_clearpc( void )
{
    if( current_df_data.parameter_type != 0x00 ) {
        internal_err( __FILE__, __LINE__ );
    }

#ifdef __UNIX__
    out_msg( "\033[2J" );
#else
    system( "cls" );
#endif
    return( NULL );
}

/* Function df_dotab().
 * Implements device function %dotab().
 *
 * Note: mem_realloc() will not return unless it succeeds.
 */

static void *df_dotab( void )
{
    static  int         instance = 0;
            size_t      spaces;

    /* Recursion is an error. */

    instance++;
    if( instance > 1 ) {
        xx_simple_err_c( err_rec_dev_func, "%dotab()" );
    }

    if( current_df_data.parameter_type != 0x00 ) {
        internal_err( __FILE__, __LINE__ );
    }

    /* desired_state.x_address must be greater than current_state.x_address
     * or no horizontal positioning occurs. Note that a 0 or negative value
     * is not an error: df_dotab() can be invoked at any time, but only
     * produces output when positive horizontal spacing is needed.
     */

    x_address = desired_state.x_address;
    if( desired_state.x_address > current_state.x_address ) {
        if( ProcFlags.has_aa_block ) {
            fb_absoluteaddress();
        } else {

            /* Set tab_width and spaces. */

            tab_width = desired_state.x_address - current_state.x_address;
            spaces = tab_width / wgml_fonts[active_font].spc_width;

            /* Since %dotab() never uses the :HTAB block, tab_width must be
             * an integral number of space character widths. This is a
             * warning because the document can still be produced, although
             * it may not be as well-justified as desired.
             */

            if( (tab_width % wgml_fonts[active_font].spc_width) > 0 ) {
                internal_err( __FILE__, __LINE__ );
            }

            /* Perform the %dotab() horizontal positioning. */

            if( !text_out_open ) {
                pre_text_output();
                if( ProcFlags.ps_device ) {
                    ob_insert_ps_text_start();
                    text_out_open = true;
                }
            }
            output_spaces( spaces );
            if( text_out_open ) {
                if( ProcFlags.ps_device ) {
                    ob_insert_ps_text_end( htab_done, active_font );
                }
                post_text_output();
            }
            tab_width = 0;
        }
    }

    instance--;

    return( NULL );
}

/* Function df_endif().
 * Implements device function %endif().
 */

static void *df_endif( void )
{
    if( current_df_data.parameter_type != 0x00 ) {
        internal_err( __FILE__, __LINE__ );
    }

    return( NULL );
}

/* Function df_flushpage().
 * Implements device function %flushpage().
 */

static void *df_flushpage( void )
{
    static  int         instance    = 0;
            uint16_t    current_pages;
            uint32_t    save_desired_y;

    /* Recursion is an error. */

    instance++;
    if( instance > 1 ) {
        xx_simple_err_c( err_rec_dev_func, "%flushpage()" );
    }

    if( current_df_data.parameter_type != 0x00 ) {
        internal_err( __FILE__, __LINE__ );
    }

    /* Save the value of desired_state.y_address. */

    save_desired_y = desired_state.y_address;

    /* Interpret a :LINEPROC :ENDVALUE block if appropriate. */

    fb_lineproc_endvalue();

    /* current_pages contains the number of device pages needed to reach
     * current_state.y_address; side-by-side camparisons with wgml 4.0
     * showed that current_state.y_address must be decremented to produce
     * the correct value. The effect, for TERM, is to map [1,20] on to [0,19].
     */

    if( current_state.y_address == 0 ) {
        current_pages = 0;
    } else {
        current_pages = (current_state.y_address - 1) / bin_device->page_depth;
    }

    /* The value needed in desired_state.y_address must reflect the
     * number of device pages included in current_state.y_address so
     * that fb_newlines() will have comparable values to work with.
     */

    if( bin_driver->y_positive == 0x00 ) {

        /* desired_state.y_address is to be formed by subtraction. */

        desired_state.y_address = current_pages * bin_device->page_depth;
    } else {

        /* desired_state.y_address is to be formed by addition. */

        desired_state.y_address = (current_pages + 1) * bin_device->page_depth;
    }

    /* The print head position is the start of the bottom line. */

    x_address = bin_device->x_start;
    y_address = bin_device->page_depth;

    /* If :ABSOLUTEADDRESS is not available, do the vertical positioning. */

    if( !ProcFlags.has_aa_block )
        fb_newline();

    /* If this is the Initial Vertical Positioning, interpret the :LINEPROC
     * :ENDVALUE block for line pass 1 of available font 0, unless it has
     * already been done. Note: this places the block at the proper location
     * when the :ABSOLUTEADDRESS block is defined and %flushpage() is invoked.
     */

    if( at_start ) {
        if( wgml_fonts[0].font_style->lineprocs != NULL ) {
            if( wgml_fonts[0].font_style->lineprocs[0].endvalue != NULL ) {
                df_interpret_driver_functions( wgml_fonts[0].font_style->lineprocs[0].endvalue->text );
            }
        }
        at_start = false;
    }

    /* current_state.y_address must be the start of the line before the first.
     * desired_state.y_address must have the same value that it had on entry.
     * y_address must have the line before the first line on the page.
     */

    current_state.y_address = current_pages * bin_device->page_depth;
    desired_state.y_address = save_desired_y;
    y_address = bin_device->y_start;

    instance--;

    return( NULL );
}

/* Function df_recordbreak_device().
 * Implements device function %recordbreak() for the :DEVICE block.
 */

static void *df_recordbreak_device( void )
{
    if( current_df_data.parameter_type != 0x00 ) {
        internal_err( __FILE__, __LINE__ );
    }

    out_msg( "\n" );
    return( NULL );
}

/* Function df_recordbreak_driver().
 * Implements device function %recordbreak() for the :DRIVER block.
 */

static void *df_recordbreak_driver( void )
{
    if( current_df_data.parameter_type != 0x00 ) {
        internal_err( __FILE__, __LINE__ );
    }

    ob_flush();
    return( NULL );
}

/* Function df_textpass().
 * Implements device function %textpass().
 */

static void *df_textpass( void )
{
    if( current_df_data.parameter_type != 0x00 ) {
        internal_err( __FILE__, __LINE__ );
    }

    textpass = true;
    return( NULL );
}

/* Function df_ulineoff().
 * Implements device function %ulineoff().
 */

static void *df_ulineoff( void )
{
    if( current_df_data.parameter_type != 0x00 ) {
        internal_err( __FILE__, __LINE__ );
    }

    uline = false;
    return( NULL );
}

/* Function df_ulineon().
 * Implements device function %ulineon().
 */

static void *df_ulineon( void )
{
    if( current_df_data.parameter_type != 0x00 ) {
        internal_err( __FILE__, __LINE__ );
    }

    uline = true;
    return( NULL );
}

/* Function df_wait().
 * Implements device function %wait().
 */

static void *df_wait( void )
{
    if( current_df_data.parameter_type != 0x00 ) {
        internal_err( __FILE__, __LINE__ );
    }

    getchar();
    return( df_recordbreak_device() );
}

/* Type II device functions have no parameter type byte. */

/* Function df_date().
 * Implements device function %date().
 */

static void *df_date( void )
{
    return( (void *)char_convert( date_val ) );
}

/* Function df_default_width().
 * Implements device function %default_width().
 */

static void *df_default_width( void )
{
    return( (void *)(uintptr_t)wgml_fonts[df_font].bin_font->char_width );
}

/* Function df_font_height().
 * Implements device function %font_height().
 */

static void *df_font_height( void )
{
    return( (void *)(uintptr_t)wgml_fonts[df_font].font_height );
}

/* Function df_font_number().
 * Implements device function %font_number().
 */

static void *df_font_number( void )
{
    return( (void *)(uintptr_t)df_font );
}

/* Function df_font_outname1().
 * Implements device function %font_outname1().
 */

static void *df_font_outname1( void )
{
    char    *ret_val;

    ret_val = char_convert( wgml_fonts[df_font].bin_font->font_out_name1 );

    return( (void *)ret_val );
}

/* Function df_font_outname2().
 * Implements device function %font_outname2().
 */

static void *df_font_outname2( void )
{
    char    *ret_val;

    ret_val = char_convert( wgml_fonts[df_font].bin_font->font_out_name2 );

    return( (void *)ret_val );
}

/* Function df_font_resident().
 * Implements device function %font_resident().
 */

static void *df_font_resident( void )
{
    char    *ret_val;

    ret_val = mem_alloc( 2 );
    ret_val[0] = wgml_fonts[df_font].font_resident;
    ret_val[1] = '\0';

    return( (void *)ret_val );
}

/* Function df_font_space().
 * Implements device function %font_space().
 */

static void *df_font_space( void )
{
    return( (void *)(uintptr_t)wgml_fonts[df_font].font_space );
}

/* Function df_line_height().
 * Implements device function %line_height().
 */

static void *df_line_height( void )
{
    return( (void *)(uintptr_t)wgml_fonts[df_font].line_height );
}

/* Function df_line_space().
 * Implements device function %line_space().
 */

static void *df_line_space( void )
{
    return( (void *)(uintptr_t)wgml_fonts[df_font].line_space );
}

/* Function df_page_depth().
 * Implements device function %page_depth().
 */

static void *df_page_depth( void )
{
    return( (void *)(uintptr_t)bin_device->page_depth );
}

/* Function df_page_width().
 * Implements device function %page_width().
 */

static void *df_page_width( void )
{
    return( (void *)(uintptr_t)bin_device->page_width );
}

/* Function df_pages().
 * Implements device function %pages().
 */

static void *df_pages( void )
{
    return( (void *)(uintptr_t)apage );
}

/* Function df_tab_width().
 * Implements device function %tab_width().
 */

static void *df_tab_width( void )
{
    return( (void *)(uintptr_t)tab_width );
}

/* Function df_thickness().
 * Implements device function %thickness().
 */

static void *df_thickness( void )
{
    return( (void *)(uintptr_t)thickness );
}

/* Function df_time().
 * Implements device function %time().
 */

static void *df_time( void )
{
    return( (void *)char_convert( time_val ) );
}

/* Function df_wgml_header().
 * Implements device function %wgml_header().
 */

static void *df_wgml_header( void )
{
    return( (void *)char_convert( wgml_header ) );
}

/* Function df_x_address().
 * Implements device function %x_address().
 */

static void *df_x_address( void )
{
    return( (void *)(uintptr_t)x_address );
}

/* Function df_x_size().
 * Implements device function %x_size().
 */

static void *df_x_size( void )
{
    return( (void *)(uintptr_t)x_size );
}

/* Function df_y_address().
 * Implements device function %y_address().
 */

static void *df_y_address( void )
{
    return( (void *)(uintptr_t)y_address );
}

/* Function df_y_size().
 * Implements device function %y_size().
 */

static void *df_y_size( void )
{
    return( (void *)(uintptr_t)y_size );
}

/* Parameter block parsing functions. */

/* Function get_parameters().
 * Place offset2 and offset3 into in_parameters->first and
 * in_parameters->second. This function also skips both offset1 and offset4.
 *
 * Parameter:
 *      in_parameters points to the parameters instance to be initialized.
 *
 * Global Variable Prerequisite:
 *      current_df_data.current must point to the first byte of
 *      offset1 on entry.
 *
 * Global Variable Modified:
 *      current_df_data.current will point to the first byte after
 *      offset4 on exit.
 *
*/

static void *get_parameters( parameters *in_parameters )
{
    uint16_t    offset;

    /* Skip the offset1 value. */

    offset = get_u16( &current_df_data.current );

    /* Get the first parameter offset (offset2). */

    in_parameters->first = get_u16( &current_df_data.current );

    /* Get the second parameter offset (offset3). */

    in_parameters->second = get_u16( &current_df_data.current );

    /* Skip the offset4 value. */

    offset = get_u16( &current_df_data.current );

    return( NULL );
}

/* Function process_parameter().
 * Extracts the byte code from the Directive instance, updates
 * current_df_data.current, and invokes the function which handles
 * that byte code.
 *
 * Global Variable Prerequisite:
 *      current_df_data.current must point to the first byte in the
 *      Directive instance.
 *
 * Global Variable Altered:
 *      current_df_data will be altered as needed to parse the rest of the
 *      function sequence.
 *
 * Returns:
 *      the value returned by the device function invoked.
*/

static void *process_parameter( void )
{
    /* Reset current_df_data for the parameter. */

    current_df_data.df_code = get_u8( &current_df_data.current );

    /* Invoke parameter function. */

    if( current_df_data.df_code > MAX_FUNC_INDEX ) {
        internal_err( __FILE__, __LINE__ );
    }

    return( current_function_table[current_df_data.df_code]() );
}

/* Functions which use parameters.
 * These functions all take parameters and so have common effects on the global
 * current_df_data.
 *
 * Notes:
 *      current_df_data.current will point to the first byte after
 *          the byte code for the function being processed when the handler in
 *          function_table[] is invoked. This occurs when a parameter block is
 *          present.
 *      current_df_data.current will point to the first byte after
 *          the character data found by %image() and %text() when they have a
 *          parameter which is not in a parameter block.
 */

/* Device functions %image() and %text() have an associated parameter type byte
 * which can be either "0x00" (for a literal parameter) or "0x10" (for a
 * parameter block).
 */

/* Function df_out_text_device().
 * Implements device functions %image() and %text() for the :DEVICE block.
 */

static void *df_out_text_device( void )
{
    char        *first;
    int         i;
    parameters  my_parameters;
    uint16_t    count;

    switch( current_df_data.parameter_type ) {
    case 0x00:

        /* Character literal parameter. */

        count = get_u16( &current_df_data.current );

        /* Emit parameter byte-by-byte since may contain nulls. */

        for( i = 0; i < count; i++ ) {
            out_msg( "%c", current_df_data.current[i] );
        }
        break;

    case 0x10:

        /* Parameter block. */

        /* Ensure that this is either a ShortHeader or a LongHeader. */

        get_parameters( &my_parameters );
        if( (my_parameters.first != 0x0009) && (my_parameters.first != 0x000d) ) {
            internal_err( __FILE__, __LINE__ );
        }

        /* Now get and emit the parameter. */

        current_df_data.current = current_df_data.base + my_parameters.first;
        first = process_parameter();
        out_msg( first );

        /* Free the memory allocated to the parameter. */

        mem_free( first );

        break;

    default:
        internal_err( __FILE__, __LINE__ );
    }
    return( NULL );
}

/* Function out_text_driver().
 * Implements device functions %image() and %text() for the :DRIVER block.
 */

static void out_text_driver( bool out_trans, bool out_text )
{
    char            *first;
    parameters      my_parameters;
    uint16_t        count;

    switch( current_df_data.parameter_type ) {
    case 0x00:

        /* Character literal parameter. */

        count = get_u16( &current_df_data.current );

        ob_insert_block( current_df_data.current, count, out_trans, out_text, active_font );
        break;

    case 0x10:

        /* Parameter block. */

        /* Ensure that this is either a ShortHeader or a LongHeader. */

        get_parameters( &my_parameters );
        if( (my_parameters.first != 0x0009) && (my_parameters.first != 0x000d) ) {
            internal_err( __FILE__, __LINE__ );
        }

        /* Now get and insert the parameter. */

        current_df_data.current = current_df_data.base + my_parameters.first;
        first = process_parameter();
        count = strlen( first );
        ob_insert_block( first, count, out_trans, out_text, active_font );

        /* Free the memory allocated to the parameter. */

        mem_free( first );

        break;

    default:
        internal_err( __FILE__, __LINE__ );
    }
    return;
}

/* Function df_image_driver().
 * Implements device function %image() for the :DRIVER block.
 */

static void *df_image_driver( void )
{
    out_text_driver( false, false );
    return( NULL );
}

/* Function df_text_driver().
 * Implements device function %text() for the :DRIVER block.
 */

static void *df_text_driver( void )

{
    out_text_driver( true, false );
    return( NULL );
}

/* These functions handle parameter blocks containing literals. */

/* Function char_literal().
 * Returns a pointer to the character literal.
 */

static void *char_literal( void )
{
    char        *ret_val = NULL;
    uint16_t    count;

    /* Skip the rest of the Directive. */

    current_df_data.current += 0x0c;

    /* Get the count. */

    count = get_u16( &current_df_data.current );

    /* Convert the character literal into a char *. */

    ret_val = char_convert( current_df_data.current );
    current_df_data.current += count + 1;

    return( (void *)ret_val );
}

/* Function numeric_literal().
 * Returns the value of a numeric literal.
 */

static void *numeric_literal( void )
{
    uint16_t    value;

    /* Skip the Offsets. */

    current_df_data.current += 0x08;

    /* Get and return the value. */

    value = get_u16( &current_df_data.current );

    return( (void *)(uintptr_t)value );
}

/* These functions take parameters in parameter blocks only. */

/* Type I device functions have an associated parameter type byte, which should
 * always be "0x10" for the functions in this section.
 */

/* Function df_cancel().
 * Implements device function %cancel().
 */

static void *df_cancel( void )
{
            char        *first;
    static  int         instance = 0;
            parameters  my_parameters;

    /* Recursion is an error. */

    instance++;
    if( instance > 1 ) {
        xx_simple_err_c( err_rec_dev_func, "%cancel()" );
    }

    /* Ensure that this is either a ShortHeader or a LongHeader. */

    get_parameters( &my_parameters );
    if( (my_parameters.first != 0x0009) && (my_parameters.first != 0x000d) ) {
        internal_err( __FILE__, __LINE__ );
    }

    /* Ensure the parameter_type is correct */

    if( current_df_data.parameter_type != 0x10 ) {
        internal_err( __FILE__, __LINE__ );
    }

    /* Now invoke the parameter's handler. */

    current_df_data.current = current_df_data.base + my_parameters.first;
    first = process_parameter();

    if( wgml_fonts[df_font].font_style != NULL ) {
        if( !stricmp( first, wgml_fonts[df_font].font_style->type ) ) {
            if( wgml_fonts[df_font].font_style->startvalue != NULL ) {
                df_interpret_driver_functions( wgml_fonts[df_font].font_style->startvalue->text );
            }
        }
    }

    if( wgml_fonts[df_font].font_switch != NULL ) {
        if( !stricmp( first, wgml_fonts[df_font].font_switch->type ) ) {
            if( wgml_fonts[df_font].font_switch->startvalue != NULL ) {
                df_interpret_driver_functions( wgml_fonts[df_font].font_switch->startvalue->text );
            }
        }
    }

    /* Free the buffer and reset the recursion check. */

    mem_free( first );
    instance--;

    return( NULL );
}

/* Function df_enterfont().
 * Implements device function %enterfont().
 */

static void *df_enterfont( void )
{
    static  int         instance = 0;

    /* Recursion is an error. */

    instance++;
    if( instance > 1 ) {
        xx_simple_err_c( err_rec_dev_func, "%enterfont()" );
    }

    /* Device function %enterfont() ignores its parameter. */

    fb_enterfont();
    instance--;

    return( NULL );
}

/* Function df_sleep().
 * Implements device function %sleep().
 *
 * Note:
 *      Added for completeness. This device function causes gendev 4.1 to hang
 *      if used with a literal, and wgml 4.0 to hang if used with a non-literal.
 *      It is used in device_function_table to keep the compiler happy.
 */

static void *df_sleep( void )
{
    parameters  my_parameters;
    uintptr_t   first;

    /* Ensure that this is either a ShortHeader or a LongHeader. */

    get_parameters( &my_parameters );
    if( (my_parameters.first != 0x0009) && (my_parameters.first != 0x000d) ) {
        internal_err( __FILE__, __LINE__ );
    }

    /* Ensure the parameter_type is correct */

    if( current_df_data.parameter_type != 0x10 ) {
        internal_err( __FILE__, __LINE__ );
    }

    /* Get the parameter. */

    /* The existing gendev generates a ShortHeader but encodes the first
     * parameter as if it were a LongHeader. Unfortunately, if that
     * parameter takes a parameter, it's offset is also incorrect. This
     * function will fail in process_parameter() since the next binary code
     * will be read as 0xFF, which exceeds the maximum value for the code.
     */

    current_df_data.current = current_df_data.base + my_parameters.first;
    first = (uintptr_t)process_parameter();

    sleep( (unsigned)first );

    return( NULL );
}

/* Function df_setsymbol().
 * Implements device function %setsymbol().
 */

static void *df_setsymbol( void )
{
    char            *first;
    char            *second;
    parameters      my_parameters;

    /* Ensure that this is either a ShortHeader or a LongHeader. */

    get_parameters( &my_parameters );
    if( (my_parameters.first != 0x0009) && (my_parameters.first != 0x000d) ) {
        internal_err( __FILE__, __LINE__ );
    }

    /* Ensure the parameter_type is correct */

    if( current_df_data.parameter_type != 0x10 ) {
        internal_err( __FILE__, __LINE__ );
    }

    /* Now get the first parameter. */

    current_df_data.current = current_df_data.base + my_parameters.first;
    first = process_parameter();

    /* Now get the second parameter. */

    current_df_data.current = current_df_data.base + my_parameters.second;
    second = process_parameter();

    /* Insert the symbol into the global symbol table. */

    add_symvar( &global_dict, first, second, no_subscript, 0 );

    /* Free the memory allocated to the parameters. */

    mem_free(first);
    mem_free(second);

    return( NULL );
}

/* Function df_binary().
 * Implements device function %binary().
 *
 * Note:
 *      Device function %binary1() has the same byte code, and hence uses
 *      the same implementation, as device function %binary().
 */

static void *df_binary( void )
{
    parameters  my_parameters;

    /* Ensure that this is either a ShortHeader or a LongHeader. */

    get_parameters( &my_parameters );
    if( (my_parameters.first != 0x0009) && (my_parameters.first != 0x000d) ) {
        internal_err( __FILE__, __LINE__ );
    }

    /* Ensure the parameter_type is correct */

    if( current_df_data.parameter_type != 0x10 ) {
        internal_err( __FILE__, __LINE__ );
    }

    /* Now invoke the parameter's handler. */

    current_df_data.current = current_df_data.base + my_parameters.first;
    ob_insert_byte( (uintptr_t)process_parameter() );

    return( NULL );
}

/* Function skip_functions().
 * Skips all function sequences until %endif() is encountered or the block ends.
 *
 * Global Variables Used:
 *      current_df_data is updated to reflect the current function.
 *      current_function is used to record the start of the current function.
 *
 * Global Variables Modified:
 *      current_df_data.current will point to the first byte after
 *          the byte code for the function being processed when the handler
 *          in function_table[] is invoked.
 *      current_function will point to the %endif() or to the next-to-last
 *          function if current_df_data.last_function_done is set to true.
 *
 * Note:
 *      current_function is used to reset current_df_data.current
 *      on each pass through the loop. It is global so that it will do so
 *      when interpret_functions() resumes processing.
 */

static void skip_functions( void )
{
    uint16_t    current_offset;

    /* current_df_data.base points at the binary code for the conditional
     * function. current_function needs to point to the next top-level
     * function: the two-byte offset needed to do this starts three bytes
     * bytes in front of the position pointed to by current_df_data.base but
     * must be added to current_df_data.base.
     */

    current_function = current_df_data.base - 3;
    current_offset = get_u16( &current_function );
    current_function = current_df_data.base + current_offset;

    for( ;; ) {
        /* Start at the proper location. For current_df_data.base, this is
         * the byte before where the parameter block starts, if one is present.
         */

        current_df_data.base = current_function + 3;
        current_df_data.current = current_function;

        /* Get the offset to the next element in the linked list. */

        current_offset = get_u16( &current_df_data.current );

        /* Get the parameter type for the current device function */

        current_df_data.parameter_type = get_u8( &current_df_data.current );

        /* Either reset current_function to the next list element
         * or exit the loop. If this is the last function, it is either
         * %endif() or it must be skipped anyway, so we are done.
         */

        if( current_offset < 0xFFFF ) {
            current_function = current_df_data.current + current_offset;
        } else {

            /* The flag must be set to prevent interpret_functions from
             * processing the last function in error.
             */

            current_df_data.last_function_done = true;
            break;
        }

        /* Get the function code. */

        current_df_data.df_code = get_u8( &current_df_data.current );

        /* If the function code is for %endif(), exit the loop. */

        if( current_df_data.df_code == 0x1C ) {
            break;
        }
    }

    return;
}

/* Function df_ifeqn().
 * Implements device function %ifeqn().
 */

static void *df_ifeqn( void )
{
    parameters  my_parameters;
    uintptr_t   first;
    uintptr_t   second;

    /* Ensure that this is either a ShortHeader or a LongHeader. */

    get_parameters( &my_parameters );
    if( (my_parameters.first != 0x0009) && (my_parameters.first != 0x000d) ) {
        internal_err( __FILE__, __LINE__ );
    }

    /* Ensure the parameter_type is correct */

    if( current_df_data.parameter_type != 0x10 ) {
        internal_err( __FILE__, __LINE__ );
    }

    /* Now get the first parameter. */

    current_df_data.current = current_df_data.base + my_parameters.first;
    first = (uintptr_t)process_parameter();

    /* Now get the second parameter. */

    current_df_data.current = current_df_data.base + my_parameters.second;
    second = (uintptr_t)process_parameter();

    /* if_eqn: skip the controlled functions if the values are not equal. */

    if( first != second )
        skip_functions();

    return( NULL );
}

/* Function df_ifnen().
 * Implements device function %ifnen().
 */

static void *df_ifnen( void )
{
    parameters  my_parameters;
    uintptr_t   first;
    uintptr_t   second;

    /* Ensure that this is either a ShortHeader or a LongHeader. */

    get_parameters( &my_parameters );
    if( (my_parameters.first != 0x0009) && (my_parameters.first != 0x000d) ) {
        internal_err( __FILE__, __LINE__ );
    }

    /* Ensure the parameter_type is correct */

    if( current_df_data.parameter_type != 0x10 ) {
        internal_err( __FILE__, __LINE__ );
    }

    /* Now get the first parameter. */

    current_df_data.current = current_df_data.base + my_parameters.first;
    first = (uintptr_t)process_parameter();

    /* Now get the second parameter. */

    current_df_data.current = current_df_data.base + my_parameters.second;
    second = (uintptr_t)process_parameter();

    /* if_nen: skip the controlled functions if the values are equal. */

    if( first == second )
        skip_functions();

    return( NULL );
}

/* Function df_ifeqs().
 * Implements device function %ifeqs().
 */

static void *df_ifeqs( void )
{
    char            *first;
    char            *second;
    parameters      my_parameters;

    /* Ensure that this is either a ShortHeader or a LongHeader. */

    get_parameters( &my_parameters );
    if( (my_parameters.first != 0x0009) && (my_parameters.first != 0x000d) ) {
        internal_err( __FILE__, __LINE__ );
    }

    /* Ensure the parameter_type is correct */

    if( current_df_data.parameter_type != 0x10 ) {
        internal_err( __FILE__, __LINE__ );
    }

    /* Now get the first parameter. */

    current_df_data.current = current_df_data.base + my_parameters.first;
    first = process_parameter();

    /* Now get the second parameter. */

    current_df_data.current = current_df_data.base + my_parameters.second;
    second = process_parameter();

    /* if_eqs: skip the controlled functions if the values are not equal. */

    if( strcmp( first, second ) )
        skip_functions();

    /* Free the memory allocated to produce the parameters */

    mem_free( first );
    mem_free( second );

    return( NULL );
}

/* Function df_ifnes().
 * Implements device function %ifnes().
 */

static void *df_ifnes( void )
{
    char            *first;
    char            *second;
    parameters      my_parameters;

    /* Ensure that this is either a ShortHeader or a LongHeader. */

    get_parameters( &my_parameters );
    if( (my_parameters.first != 0x0009) && (my_parameters.first != 0x000d) ) {
        internal_err( __FILE__, __LINE__ );
    }

    /* Ensure the parameter_type is correct */

    if( current_df_data.parameter_type != 0x10 ) {
        internal_err( __FILE__, __LINE__ );
    }

    /* Now get the first parameter. */

    current_df_data.current = current_df_data.base + my_parameters.first;
    first = process_parameter();

    /* Now get the second parameter. */

    current_df_data.current = current_df_data.base + my_parameters.second;
    second = process_parameter();

    /* if_nes: skip the controlled functions if the values are equal. */

    if( !strcmp( first, second ) ) skip_functions();

    /* Free the memory allocated to produce the parameters */

    mem_free( first );
    mem_free( second );

    return( NULL );
}

/* Type II device functions have no parameter type byte or
 * ShortHeader/Longheader check.
 */

/* Function df_add().
 * Implements device function %add().
 */

static void *df_add( void )
{
    parameters  my_parameters;
    uintptr_t   first;
    uintptr_t   second;

    /* Extract parameter offsets. */

    get_parameters( &my_parameters );

    /* Now get the first parameter. */

    current_df_data.current = current_df_data.base + my_parameters.first;
    first = (uintptr_t)process_parameter();

    /* Now get the second parameter. */

    current_df_data.current = current_df_data.base + my_parameters.second;
    second = (uintptr_t)process_parameter();

    return( (void *)(first + second) );
}

/* Function df_decimal().
 * Implements device function %decimal().
 */

static void *df_decimal( void )
{
    char        *value = NULL;
    parameters  my_parameters;
    uintptr_t   first;

    /* Extract parameter offset. */

    get_parameters( &my_parameters );

    /* Now get the parameter. */

    current_df_data.current = current_df_data.base + my_parameters.first;
    first = (uintptr_t)process_parameter();

    /* Convert and return the value. */

    value = mem_alloc( 12 );
    return( (void *)ltoa( first, value, 10 ) );
}

/* Function df_divide().
 * Implements device function %divide().
 */

static void *df_divide( void )
{
    parameters  my_parameters;
    uintptr_t   first;
    uintptr_t   second;

    /* Extract parameter offsets. */

    get_parameters( &my_parameters );

    /* Now get the first parameter. */

    current_df_data.current = current_df_data.base + my_parameters.first;
    first = (uintptr_t)process_parameter();

    /* Now get the second parameter. */

    current_df_data.current = current_df_data.base + my_parameters.second;
    second = (uintptr_t)process_parameter();

    if( second == 0 ) {
        xx_simple_err_c( err_zero_divisor, "%divide()" );
    }

    return( (void *)(first / second) );
}

/* Function df_getnumsymbol().
 * Implements device function %getnumsymbol().
 */

static void *df_getnumsymbol( void )
{
    char        *name    = NULL;
    parameters  my_parameters;
    symsub      *sym_val = NULL;
    uint32_t    ret_val = 0;

    /* Extract parameter offset. */

    get_parameters( &my_parameters );

    /* Now get the parameter. */

    current_df_data.current = current_df_data.base + my_parameters.first;
    name = process_parameter();

    /* Now get the symbol's value. */

    find_symvar( &global_dict, name, no_subscript, &sym_val );
    if( sym_val != NULL )
        ret_val = atol( sym_val->value );

    /* Free the memory allocated to the parameter. */

    mem_free( name );

    return( (void *)(uintptr_t)ret_val );
}

/* Function df_getstrsymbol().
 * Implements device function %getstrsymbol().
 */

static void *df_getstrsymbol( void )
{
    char        *name    = NULL;
    char        *ret_val = NULL;
    parameters  my_parameters;
    symsub      *sym_val = NULL;

    /* Extract parameter offset. */

    get_parameters( &my_parameters );

    /* Now get the parameter. */

    current_df_data.current = current_df_data.base + my_parameters.first;
    name = process_parameter();

    /* Now get the symbol's value. */

    find_symvar( &global_dict, name, no_subscript, &sym_val );
    if( sym_val == NULL ) {
        ret_val = char_convert( NULL );
    } else {
        ret_val = char_convert( sym_val->value );
    }

    /* Free the memory allocated to the parameter. */

    mem_free( name );

    return( (void *)ret_val );
}

/* Function df_hex().
 * Implements device function %hex().
 */

static void *df_hex( void )
{
    char        *value = NULL;
    parameters  my_parameters;
    uintptr_t   first;

    /* Extract parameter offset. */

    get_parameters( &my_parameters );

    /* Now get the parameter. */

    current_df_data.current = current_df_data.base + my_parameters.first;
    first = (uintptr_t)process_parameter();

    /* Convert and return a pointer to the parameter */

    value = mem_alloc( 9 );
    return( (void *)ltoa( first, value, 16 ) );
}

/* Function df_lower().
 * Implements device function %lower().
 */

static void *df_lower( void )
{
    char            *first;
    parameters      my_parameters;

    /* Extract parameter offset. */

    get_parameters( &my_parameters );

    /* Now get the parameter. */

    current_df_data.current = current_df_data.base + my_parameters.first;
    first = process_parameter();

    /* Convert and return the parameter. */

    return( (void *)strlwr( first ) );
}

/* Function df_remainder().
 * Implements device function %remainder().
 */

static void *df_remainder( void )
{
    parameters  my_parameters;
    uintptr_t   first;
    uintptr_t   second;

    /* Extract parameter offsets. */

    get_parameters( &my_parameters );

    /* Now get the first parameter. */

    current_df_data.current = current_df_data.base + my_parameters.first;
    first = (uintptr_t)process_parameter();

    /* Now get the second parameter. */

    current_df_data.current = current_df_data.base + my_parameters.second;
    second = (uintptr_t)process_parameter();

    if( second == 0 ) {
        xx_simple_err_c( err_zero_divisor, "%remainder()" );
    }

    return( (void *)(first % second) );
}

/* Function df_subtract().
 * Implements device function %subtract().
 */

static void *df_subtract( void )
{
    parameters  my_parameters;
    uintptr_t   first;
    uintptr_t   second;

    /* Extract parameter offsets. */

    get_parameters( &my_parameters );

    /* Now get the first parameter. */

    current_df_data.current = current_df_data.base + my_parameters.first;
    first = (uintptr_t)process_parameter();

    /* Now get the second parameter. */

    current_df_data.current = current_df_data.base + my_parameters.second;
    second = (uintptr_t)process_parameter();

    return( (void *)(first - second) );
}

/* The function table for function blocks defined in the :DEVICE block.
 * This is set up for the START :PAUSE block, hence the liberal use of
 * df_do_nothing_char() and df_do_nothing_num(). Many of them will be
 * replaced when the function populate_device_table() is invoked.
 */

static df_function device_function_table[MAX_FUNC_INDEX + 1] = {
    &char_literal,          // 0x00 (character parameter in parameter block)
    &df_recordbreak_device, // 0x01 %recordbreak()
    &df_bad_code,           // 0x02 (none)
    &df_bad_code,           // 0x03 (none)
    &df_bad_code,           // 0x04 (none)
    &df_bad_code,           // 0x05 (none)
    &df_do_nothing_num,     // 0x06 %enterfont()
    &df_bad_code,           // 0x07 (none)
    &df_bad_code,           // 0x08 (none)
    &df_do_nothing_num,     // 0x09 %binary() and %binary1()
    &df_do_nothing_num,     // 0x0A %binary2()
    &df_do_nothing_num,     // 0x0B %binary4()
    &df_decimal,            // 0x0C %decimal()
    &df_hex,                // 0x0D %hex()
    &df_add,                // 0x0E %add()
    &df_subtract,           // 0x0F %subtract()
    &df_divide,             // 0x10 %divide()
    &df_remainder,          // 0x11 %remainder()
    &df_getnumsymbol,       // 0x12 %getnumsymbol()
    &df_getstrsymbol,       // 0x13 %getstrsymbol()
    &df_lower,              // 0x14 %lower()
    &df_out_text_device,    // 0x15 %image()
    &df_out_text_device,    // 0x16 %text()
    &df_setsymbol,          // 0x17 %setsymbol()
    &df_ifeqs,              // 0x18 %ifeqs()
    &df_ifnes,              // 0x19 %ifnes()
    &df_ifeqn,              // 0x1A %ifeqn()
    &df_ifnen,              // 0x1B %ifnen()
    &df_endif,              // 0x1C %endif()
    &df_do_nothing_num,     // 0x1D %flushpage()
    &df_clearpc,            // 0x1E %clearpc()
    &df_do_nothing_num,     // 0x1F %clear3270()
    &df_do_nothing_num,     // 0x20 %textpass()
    &df_do_nothing_num,     // 0x21 %ulineon()
    &df_do_nothing_num,     // 0x22 %ulineoff()
    &df_do_nothing_num,     // 0x23 %dotab()
    &df_do_nothing_num,     // 0x24 %cancel()
    &df_wait,               // 0x25 %wait()
    &df_sleep,              // 0x26 %sleep()
    &df_do_nothing_num,     // 0x27 %default_width()
    &df_font_number,        // 0x28 %font_number()
    &df_tab_width,          // 0x29 %tab_width()
    &df_do_nothing_num,     // 0x2A %page_depth()
    &df_do_nothing_num,     // 0x2B %page_width()
    &df_x_address,          // 0x2C %x_address()
    &df_y_address,          // 0x2D %y_address()
    &df_x_size,             // 0x2E %x_size()
    &df_y_size,             // 0x2F %y_size()
    &df_thickness,          // 0x30 %thickness()
    &df_do_nothing_num,     // 0x31 %font_height()
    &df_do_nothing_num,     // 0x32 %font_space()
    &df_do_nothing_num,     // 0x33 %line_height()
    &df_do_nothing_num,     // 0x34 %line_space()
    &df_pages,              // 0x35 %pages()
    &df_do_nothing_char,    // 0x36 %wgml_header()
    &df_do_nothing_char,    // 0x37 %font_outname1()
    &df_do_nothing_char,    // 0x38 %font_outname2()
    &df_do_nothing_char,    // 0x39 %font_resident()
    &df_do_nothing_char,    // 0x3A %time()
    &df_do_nothing_char,    // 0x3B %date()
    &numeric_literal,       // 0x3C (numeric parameter in parameter block)
};

/* The function table for function blocks defined in the :DRIVER block.
 * Function df_do_nothing_num() is only used for those functions documented
 * to have no effect in this context.
 */

static df_function driver_function_table[MAX_FUNC_INDEX + 1] = {
    &char_literal,          // 0x00 (character parameter in parameter block)
    &df_recordbreak_driver, // 0x01 %recordbreak()
    &df_bad_code,           // 0x02 (none)
    &df_bad_code,           // 0x03 (none)
    &df_bad_code,           // 0x04 (none)
    &df_bad_code,           // 0x05 (none)
    &df_enterfont,          // 0x06 %enterfont()
    &df_bad_code,           // 0x07 (none)
    &df_bad_code,           // 0x08 (none)
    &df_binary,             // 0x09 %binary() and %binary1()
    &df_do_nothing_num,     // 0x0A %binary2()
    &df_do_nothing_num,     // 0x0B %binary4()
    &df_decimal,            // 0x0C %decimal()
    &df_hex,                // 0x0D %hex()
    &df_add,                // 0x0E %add()
    &df_subtract,           // 0x0F %subtract()
    &df_divide,             // 0x10 %divide()
    &df_remainder,          // 0x11 %remainder()
    &df_getnumsymbol,       // 0x12 %getnumsymbol()
    &df_getstrsymbol,       // 0x13 %getstrsymbol()
    &df_lower,              // 0x14 %lower()
    &df_image_driver,       // 0x15 %image()
    &df_text_driver,        // 0x16 %text()
    &df_setsymbol,          // 0x17 %setsymbol()
    &df_ifeqs,              // 0x18 %ifeqs()
    &df_ifnes,              // 0x19 %ifnes()
    &df_ifeqn,              // 0x1A %ifeqn()
    &df_ifnen,              // 0x1B %ifnen()
    &df_endif,              // 0x1C %endif()
    &df_do_nothing_num,     // 0x1D %flushpage()
    &df_do_nothing_num,     // 0x1E %clearpc()
    &df_do_nothing_num,     // 0x1F %clear3270()
    &df_textpass,           // 0x20 %textpass()
    &df_ulineon,            // 0x21 %ulineon()
    &df_ulineoff,           // 0x22 %ulineoff()
    &df_dotab,              // 0x23 %dotab()
    &df_cancel,             // 0x24 %cancel()
    &df_do_nothing_num,     // 0x25 %wait()
    &df_do_nothing_num,     // 0x26 %sleep()
    &df_default_width,      // 0x27 %default_width()
    &df_font_number,        // 0x28 %font_number()
    &df_tab_width,          // 0x29 %tab_width()
    &df_page_depth,         // 0x2A %page_depth()
    &df_page_width,         // 0x2B %page_width()
    &df_x_address,          // 0x2C %x_address()
    &df_y_address,          // 0x2D %y_address()
    &df_x_size,             // 0x2E %x_size()
    &df_y_size,             // 0x2F %y_size()
    &df_thickness,          // 0x30 %thickness()
    &df_font_height,        // 0x31 %font_height()
    &df_font_space,         // 0x32 %font_space()
    &df_line_height,        // 0x33 %line_height()
    &df_line_space,         // 0x34 %line_space()
    &df_pages,              // 0x35 %pages()
    &df_wgml_header,        // 0x36 %wgml_header()
    &df_font_outname1,      // 0x37 %font_outname1()
    &df_font_outname2,      // 0x38 %font_outname2()
    &df_font_resident,      // 0x39 %font_resident()
    &df_time,               // 0x3A %time()
    &df_date,               // 0x3B %date()
    &numeric_literal,       // 0x3C (numeric parameter in parameter block)
};

/* Function interpret_functions().
 * Interprets device functions for function blocks as selected by the value of
 * current_function_table. This function directly handles the linked list of
 * Type I device functions only; the individual functions in
 * current_function_table[] handle parameters, including parameter blocks and
 * Type II device functions, as appropriate.
 *
 * Parameter:
 *      in_function points to a compiled function block.
 *
 * Global Variables Used:
 *      current_df_data is updated to reflect the current function.
 *      current_function is used to record the start of the current function.
 *      current_function_table is used to invoke the handler for each device
 *          function.
 *
 * Global Variable Modified:
 *      current_df_data.current will point to the first byte after
 *          the byte code for the function being processed when the handler
 *          in function_table[] is invoked.
 *      current_function will point to the next-to-last function.
 *
 * Notes:
 *      current_function is used to reset current_df_data.current
 *          on each pass through the loop. It is not necessary that the
 *          functions invoked through function_table[] leave
 *          current_df_data.current in any particular state.
 *      This function is called recursively and so several globals must be
 *          restored to their value on entry on exit.
*/

static void interpret_functions( const char *in_function )
{
    bool            old_last_done = false;
    df_function     *old_function_table = NULL;
    const char      *old_function = NULL;
    uint16_t        current_offset;

    /* An empty or missing block is not an error, but a warning is issued
     * in case the calling code needs adjustment.
     */

    if( in_function == NULL ) {
        internal_err( __FILE__, __LINE__ );
        return;
    }

    /* Save the interpreter state. */

    old_function = current_function;
    old_function_table = current_function_table;
    old_last_done = current_df_data.last_function_done;

    /* Initialize the loop parameters. */

    current_function = in_function;
    current_df_data.last_function_done = false;

    while( !current_df_data.last_function_done ) {

        /* Start at the proper location. For current_df_data.base, this is
         * the byte before where the parameter block starts, if one is present.
         */

        current_df_data.base = current_function + 3;
        current_df_data.current = current_function;

        /* Get the offset to the next element in the linked list. */

        current_offset = get_u16( &current_df_data.current );

        /* Get the parameter type for the current device function */

        current_df_data.parameter_type = get_u8( &current_df_data.current );

        /* Either reset current_function to the next list element
         * or record that the last function will be done this iteration.
         */

        if( current_offset < 0xFFFF ) {
            current_function = current_df_data.current + current_offset;
        } else {
            current_df_data.last_function_done = true;
        }

        /* Get the function code. */

        current_df_data.df_code = get_u8( &current_df_data.current );

        /* This is where the df_code processing occurs. */

        if( current_df_data.df_code > MAX_FUNC_INDEX ) {
            internal_err( __FILE__, __LINE__ );
        }

        current_function_table[current_df_data.df_code]();

    }

    /* Restore interpreter state. */

    current_function = old_function;
    current_function_table = old_function_table;
    current_df_data.last_function_done = old_last_done ;

    return;
}

/* Function fb_firstword().
 * Interprets the :FIRSTWORD block of the given :LINEPROC block. The "trick"
 * here is that, if the :FIRSTWORD block does not exist but the :STARTWORD
 * block does, then the :STARTWORD block is interpreted instead.
 *
 * Parameter:
 *      in_block points to the :LINEPROC block.
 */

static void fb_firstword( line_proc *in_block )
{
    if( in_block->firstword == NULL ) {
        if( in_block->startword != NULL ) {
            if( text_out_open ) {
                if( ProcFlags.ps_device ) {
                    ob_insert_ps_text_end( htab_done, active_font );
                }
                post_text_output();
            }
            df_interpret_driver_functions( in_block->startword->text );
        }
    } else {
        if( text_out_open ) {
            if( ProcFlags.ps_device ) {
                ob_insert_ps_text_end( htab_done, active_font );
            }
            post_text_output();
        }
        df_interpret_driver_functions( in_block->firstword->text );
    }

    return;
}

/* Function fb_font_switch().
 * Performs the font switch sequence as described in the Wiki.
 */

static void fb_font_switch( void )
{
    bool                    do_now;
    char                    *from_string;
    char                    *to_string;
    fontswitch_block        *font_switch;
    font_number             old_df_font;
    uintptr_t               from_numeric;
    uintptr_t               to_numeric;
    wgml_font               *from_font;
    wgml_font               *to_font;

    /* Acquire the "from" and "to" fonts and save the font. */

    from_font = &wgml_fonts[current_state.font];
    to_font = &wgml_fonts[desired_state.font];
    old_df_font = df_font;

    /* The first test: do the fonts use the same :FONTSWITCH block? */

    do_now = (from_font->font_switch != to_font->font_switch);

    /* font_switch will only be used if the "from" and "to" fonts both
     * use the same :FONTSWITCH block (do_now is false here in that case).
     */

    if( do_now ) {
        font_switch = NULL;
    } else {
        font_switch = from_font->font_switch;
    }

    /* The second test: given only one :FONTSWITCH block, is do_always true
     * or false? Note: that :FONTSWITCH block could be NULL.
     */

    if( !do_now && (font_switch != NULL) )
        do_now = font_switch->do_always;

    /* The third test: evaluate the :FONTSWITCH block (if it exits) as
     * described in the Wiki. The trick here, of course, is to ensure that,
     * once set to "true", do_now is never reset to "false".
     */

    if( !do_now && (font_switch != NULL) ) {
        if( font_switch->default_width_flag ) {

            /* The default width is a numeric. */

            df_font = current_state.font;
            from_numeric = (uintptr_t)df_default_width();
            df_font = desired_state.font;
            to_numeric = (uintptr_t)df_default_width();
            if( !do_now ) {
                do_now = ( from_numeric != to_numeric );
            }
        }

        if( font_switch->font_height_flag ) {

            /* The font height is a numeric. */

            df_font = current_state.font;
            from_numeric = (uintptr_t)df_font_height();
            df_font = desired_state.font;
            to_numeric = (uintptr_t)df_font_height();
            if( !do_now ) {
                do_now = ( from_numeric != to_numeric );
            }
        }

        if( font_switch->font_outname1_flag ) {

            /* The font out name 1 is a string. */

            df_font = current_state.font;
            from_string = df_font_outname1();
            df_font = desired_state.font;
            to_string = df_font_outname1();
            if( !do_now )
                do_now = ( strcmp( from_string, to_string ) );
            mem_free( from_string );
            mem_free( to_string );
        }

        if( font_switch->font_outname2_flag ) {

            /* The font out name 2 is a string. */

            df_font = current_state.font;
            from_string = df_font_outname2();
            df_font = desired_state.font;
            to_string = df_font_outname2();
            if( !do_now )
                do_now = ( strcmp( from_string, to_string ) );
            mem_free( from_string );
            mem_free( to_string );
        }

        if( font_switch->font_resident_flag ) {

            /* The font resident flag is a string. */

            df_font = current_state.font;
            from_string = df_font_resident();
            df_font = desired_state.font;
            to_string = df_font_resident();
            if( !do_now )
                do_now = ( strcmp( from_string, to_string ) );
            mem_free( from_string );
            mem_free( to_string );
        }

        if( font_switch->font_space_flag ) {

            /* The font space is a numeric. */

            df_font = current_state.font;
            from_numeric = (uintptr_t)df_font_space();
            df_font = desired_state.font;
            to_numeric = (uintptr_t)df_font_space();
            if( !do_now ) {
                do_now = ( from_numeric != to_numeric );
            }
        }

        if( font_switch->line_height_flag ) {

            /* The line height is a numeric. */

            df_font = current_state.font;
            from_numeric = (uintptr_t)df_line_height();
            df_font = desired_state.font;
            to_numeric = (uintptr_t)df_line_height();
            if( !do_now ) {
                do_now = ( from_numeric != to_numeric );
            }
        }

        if( font_switch->line_space_flag ) {

            /* The line space is a numeric. */

            df_font = current_state.font;
            from_numeric = (uintptr_t)df_line_space();
            df_font = desired_state.font;
            to_numeric = (uintptr_t)df_line_space();
            if( !do_now ) {
                do_now = ( from_numeric != to_numeric );
            }
        }
    }

    /* Restore the value of font number. */

    df_font = old_df_font;

    /* Now for the font switch itself. */

    if( from_font->font_style != NULL ) {
        if( from_font->font_style->endvalue != NULL ) {
            df_interpret_driver_functions( from_font->font_style->endvalue->text );
        }
    }

    if( do_now ) {
        if( from_font->font_switch != NULL ) {
            if( from_font->font_switch->endvalue != NULL ) {
                df_interpret_driver_functions( from_font->font_switch->endvalue->text );
            }
        }
    }

    if( to_font->font_pause != NULL ) {
        df_interpret_device_functions( to_font->font_pause->text );
    }

    if( do_now ) {
        if( to_font->font_switch != NULL ) {
            if( to_font->font_switch->startvalue != NULL ) {
                df_interpret_driver_functions( to_font->font_switch->startvalue->text );
            }
        }
    }

    if( to_font->font_style != NULL ) {
        if( to_font->font_style->startvalue != NULL ) {
            df_interpret_driver_functions( to_font->font_style->startvalue->text );
        }
    }

    /* This ensures that switches from the new font will be detected. */

    current_state.font = desired_state.font;

    return;
}

/* Function fb_htab().
 * Function fb_htab() inteprets the :HTAB block and sets htab_done to "true".
 *
 * Global Used:
 *      tab_width must be set to the appropriate value, as this is
 *          (presumably) what the :HTAB block will use to determine
 *          how much horizontal movement is needed.
 *
 * Prerequisite:
 *      The :HTAB block must have been defined for the current device.
 */

static void fb_htab( void )
{
    if( text_out_open ) {
        if( ProcFlags.ps_device ) {
            ob_insert_ps_text_end( htab_done, active_font );
        }
        text_out_open = false;
    }
    df_interpret_driver_functions( bin_driver->htab.text );
    htab_done = true;
    current_state.x_address = desired_state.x_address;
    return;
}

/* Function fb_initial_horizontal_positioning().
 * Performs the initial horizontal positioning as described in the Wiki.
 */

static void fb_initial_horizontal_positioning( void )
{
    size_t      spaces;

    x_address = desired_state.x_address;
    if( ProcFlags.has_aa_block ) {
        fb_absoluteaddress();
    } else {

        /* Spaces cannot be emitted and tabs cannot be done "backwards". */

        if( current_state.x_address > desired_state.x_address ) {
            internal_err( __FILE__, __LINE__ );
        }

        /* Perform the initial horizontal positioning. */

        /* Use :HTAB if tab_width is not an integral number of spaces. */

        tab_width = desired_state.x_address - current_state.x_address;
        if( has_htab && (tab_width % wgml_fonts[active_font].spc_width > 0) ) {
            fb_htab();
        } else {

        /* Output the requested number of spaces. */

            spaces = tab_width / wgml_fonts[active_font].spc_width;
            if( has_htab && (tab_width > 8) ) {
                fb_htab();
            } else {
                if( !text_out_open )
                    pre_text_output();
                output_spaces( spaces );
            }
        }
        tab_width = 0;
    }
    return;
}

/* Function fb_internal_horizontal_positioning().
 * Performs the internal horizontal positioning as described in the Wiki.
 */

static void fb_internal_horizontal_positioning( void )
{
    size_t      spaces;

    /* Spaces cannot be emitted and tabs cannot be done "backwards". */

    if( current_state.x_address > desired_state.x_address ) {
        internal_err( __FILE__, __LINE__ );
    }

    /* Perform the internal horizontal positioning. */

    /* Use :HTAB if tab_width is not an integral number of spaces. */

    x_address = desired_state.x_address;
    tab_width = desired_state.x_address - current_state.x_address;
    if( has_htab && (tab_width % wgml_fonts[active_font].spc_width > 0) ) {
        fb_htab();
    } else {

        /* Output the requested number of spaces. */

        spaces = tab_width / wgml_fonts[active_font].spc_width;
        if( has_htab && (spaces > 8) ) {
            fb_htab();
        } else {
            if( !text_out_open )
                pre_text_output();
            output_spaces( spaces );
        }
    }
    tab_width = 0;

    return;
}

/* Function fb_first_text_chars().
 * Performs the "first text_chars instance" sequence per the Wiki.
 *
 * Parameters:
 *      in_chars points to the text_chars instance to be processed.
 *      in_lineproc points to the :LINEPROC to use
 *
 * Notes:
 *      If in_lineproc is NULL, then textpass is set to "true" and text
 *          output will occur.
 *      The first step in the Wiki is omitted because that step is only
 *          done on the first line pass, and this function is used on
 *          subsequent passes as well.
 */

static void fb_first_text_chars( text_chars *in_chars, line_proc *in_lineproc )
{
    bool    font_switch_needed  = true;
    bool    undo_shift          = false;

    /* Set font number and initialize the locals. */

    df_font = desired_state.font;
    active_font = desired_state.font;

    if( current_state.font == desired_state.font ) {
        font_switch_needed = false;
    }

    /* Do the font switch, if needed. If a font switch is not needed,
     * interpret the :FONTSTYLE block :STARTVALUE block.
     */

    if( font_switch_needed ) {
        if( text_out_open ) {
            if( ProcFlags.ps_device ) {
                ob_insert_ps_text_end( htab_done, active_font );
            }
            post_text_output();
        }
        fb_font_switch();
    } else {
        if( wgml_fonts[df_font].font_style != NULL ) {
            if( wgml_fonts[df_font].font_style->startvalue != NULL ) {
                if( text_out_open ) {
                    if( ProcFlags.ps_device ) {
                        ob_insert_ps_text_end( htab_done, active_font );
                    }
                    post_text_output();
                }
                df_interpret_driver_functions( wgml_fonts[df_font].font_style->startvalue->text );
            }
        }
    }

    /* If there is no :LINEPROC block, then set textpass to "true"; if there
     * is a :LINEPROC block, set textpass only if device function
     * %textpass() is processed.
     */

    if( in_lineproc == NULL ) {
        textpass = true;
        if( !text_out_open ) {
            pre_text_output();
        }
    } else {
        if( in_lineproc->startvalue != NULL ) {
            if( text_out_open ) {
                if( ProcFlags.ps_device ) {
                    ob_insert_ps_text_end( htab_done, active_font );
                }
                post_text_output();
            }
            df_interpret_driver_functions( in_lineproc->startvalue->text );
        }

        fb_firstword( in_lineproc );

        if( !text_out_open ) {
            pre_text_output();
        }

        if( !font_switch_needed ) {
            if( in_lineproc->startword != NULL ) {
                if( text_out_open ) {
                    if( ProcFlags.ps_device ) {
                        ob_insert_ps_text_end( htab_done, active_font );
                    }
                    post_text_output();
                }
                df_interpret_driver_functions( in_lineproc->startword->text );
            }
        }
    }

    /* Note that gendev ensures that %textpass() and %ulineon()/%ulineoff()
     * do not appear in the same :LINEPROC. As a result, at most one of
     * textpass and uline will be "true".
     */

    /* post_text_output() must be done if this text_chars was subscripted
     * or superscripted and the next text_chars either doesn't exist or
     * has a different type.
     */

    if( in_chars->next == NULL ) {
        undo_shift = true;
    } else if( in_chars->type != in_chars->next->type ) {
        undo_shift = true;
    } else {
        undo_shift = false;
    }

    /* If textpass is "true", output the text. The value of textpass is not
     * changed here: it will be used by fb_subsequent_text_chars().
     */

    if( textpass ) {
        fb_initial_horizontal_positioning();
        if( !text_out_open && ProcFlags.ps_device ) {
            ob_insert_ps_text_start();
            text_out_open = true;
        }
        ob_insert_block( in_chars->text, in_chars->count, true, true, in_chars->font);

        if( undo_shift && text_out_open && ProcFlags.ps_device ) {
            ob_insert_ps_text_end( htab_done, active_font );
            htab_done = false;
            text_out_open = false;
        }
    }

    /* If uline is "true", then emit the underscore characters. %dotab() must
     * be used to avoid underlining the initial spaces.
     */

    if( uline ) {
        if( !text_out_open && ProcFlags.ps_device ) {
            ob_insert_ps_text_start();
            text_out_open = true;
        }
        output_uscores( in_chars );
        if( undo_shift && text_out_open && ProcFlags.ps_device ) {
            ob_insert_ps_text_end( htab_done, active_font );
            htab_done = false;
            text_out_open = false;
        }
    }

    /* Update variables and interpret the post-output function block. */

    current_state.x_address += in_chars->width;
    x_address = current_state.x_address;

    if( in_lineproc != NULL ) {
        if( in_lineproc->endword != NULL ) {
            if( text_out_open && ProcFlags.ps_device ) {
                ob_insert_ps_text_end( htab_done, active_font );
                htab_done = false;
                text_out_open = false;
            }
            df_interpret_driver_functions( in_lineproc->endword->text );
        }
    }

    if( undo_shift ) {
        post_text_output();
    }

    return;
}

/* Function fb_new_font_text_chars().
 * Performs the "new font text_chars instance" sequence per the Wiki.
 *
 * Parameter:
 *      in_chars points to the text_chars instance to be processed.
 *      in_lineproc points to the :LINEPROC to use
 *
 * Note:
 *      If in_lineproc is NULL, then textpass is set to "true" and text
 *          output will occur.
 */

static void fb_new_font_text_chars( text_chars *in_chars, line_proc *in_lineproc )
{
    bool    undo_shift  = false;

    /* Interpret the pre-font switch function block. */

    fb_lineproc_endvalue();

    /* Set the appropriate globals. */

    df_font = desired_state.font;
    active_font = desired_state.font;

    /* Do the font switch, which is needed by definition. */

    if( text_out_open ) {
        if( ProcFlags.ps_device ) {
            ob_insert_ps_text_end( htab_done, active_font );
        }
        post_text_output();
    }
    fb_font_switch();

    /* If there is no :LINEPROC block, then set textpass to "true"; if there
     * is a :LINEPROC block, set textpass only if device function
     * %textpass() is processed.
     */

    if( in_lineproc == NULL ) {
        textpass = true;
        if( !text_out_open ) {
            pre_text_output();
        }
    } else {
        if( in_lineproc->startvalue != NULL ) {
            if( text_out_open ) {
                if( ProcFlags.ps_device ) {
                    ob_insert_ps_text_end( htab_done, active_font );
                }
                post_text_output();
            }
            df_interpret_driver_functions( in_lineproc->startvalue->text );
        }
        fb_firstword( in_lineproc );

        if( !text_out_open ) {
            pre_text_output();
        }

    }

    /* Note that gendev ensures that %textpass() and %ulineon()/%ulineoff()
     * do not appear in the same :LINEPROC. As a result, at most one of
     * textpass and uline will be "true".
     */

    /* post_text_output() must be done if this text_chars was subscripted
     * or superscripted and the next text_chars either doesn't exist or
     * has a different type.
     */

    if( in_chars->next == NULL ) {
        undo_shift = true;
    } else if( in_chars->type != in_chars->next->type ) {
        undo_shift = true;
    } else {
        undo_shift = false;
    }

    /* If textpass is "true", output the text. The value of textpass is not
     * changed here: it will be used by fb_subsequent_text_chars().
     */

    if( textpass ) {
        fb_internal_horizontal_positioning();
        if( !text_out_open && ProcFlags.ps_device ) {
            ob_insert_ps_text_start();
            text_out_open = true;
        }
        ob_insert_block( in_chars->text, in_chars->count, true, true, in_chars->font);

        if( undo_shift && text_out_open && ProcFlags.ps_device ) {
            ob_insert_ps_text_end( htab_done, active_font );
            htab_done = false;
            text_out_open = false;
        }
    }

    /* If uline is "true", then emit the underscore characters. %dotab() must
     * be used to avoid underlining the initial spaces.
     */

    if( uline ) {
        if( !text_out_open && ProcFlags.ps_device ) {
            ob_insert_ps_text_start();
            text_out_open = true;
        }
        output_uscores( in_chars );
        if( undo_shift && text_out_open && ProcFlags.ps_device ) {
            ob_insert_ps_text_end( htab_done, active_font );
            htab_done = false;
            text_out_open = false;
        }
    }

    /* Update variables and interpret the post-output function block. */

    current_state.x_address += in_chars->width;
    x_address = current_state.x_address;

    if( in_lineproc != NULL ) {
        if( in_lineproc->endword != NULL ) {
            if( text_out_open && ProcFlags.ps_device ) {
                ob_insert_ps_text_end( htab_done, active_font );
                htab_done = false;
                text_out_open = false;
            }
            df_interpret_driver_functions( in_lineproc->endword->text );
        }
    }

    if( undo_shift ) {
        post_text_output();
    }

    return;
}

/* Function fb_overprint_vertical_positioning().
 * Performs the overprint vertical positioning as described in the Wiki.
 */

static void fb_overprint_vertical_positioning( void )
{
    int                 i;
    newline_block       *current_block   = NULL;

    /* If :ABSOLUTEADDRESS is not available, do the vertical positioning. */

    if( !ProcFlags.has_aa_block ) {

        /* Use the :NEWLINE block with an advance of "0", if one exists. */

        for( i = 0; i < bin_driver->newlines.count; i++ ) {
            if( bin_driver->newlines.newlineblocks[i].advance == 0 ) {
                current_block = &bin_driver->newlines.newlineblocks[i];
                break;
            }
        }

        /* If no :NEWLINE block with an advance of "0" exists, then use the
         * required (since there is no :ABSOLUTEADDRESS block) :NEWLINE block
         * with an advance of "1".
         */

        if( current_block == NULL ) {
            for( i = 0; i < bin_driver->newlines.count; i++ ) {
                if( bin_driver->newlines.newlineblocks[i].advance == 1 ) {
                    current_block = &bin_driver->newlines.newlineblocks[i];
                    break;
                }
            }
        }

        df_interpret_driver_functions( current_block->text );

        /* This code:
         *  if( current_block->advance == 1 ) {
         *      desired_state.y_address += wgml_fonts[df_font].line_height;
         *      current_state.y_address = desired_state.y_address;
         *      y_address = current_state.y_address;
         *  }
         * caused synchronization problems when triggered by HELP.
         * As discussed in the Wiki, it should not be restored except as
         * part of a much more elaborate system.
         */
    }

    /* Reset the appropriate values. */

    current_state.x_address = bin_device->x_start;
    x_address = current_state.x_address;

    return;
}

/* Function fb_normal_vertical_positioning().
 * Performs the normal vertical positioning as described in the Wiki.
 */

static void fb_normal_vertical_positioning( void )
{
    uint32_t    i;
    uint32_t    current_pages;
    uint32_t    desired_pages;
    uint32_t    device_pages;

    /* A device using :ABSOLUTEADDRESS may be able to move upwards on a given
     * device page, but it cannot go back to a prior device page. A device
     * using :NEWLINE blocks, of course, cannot move upwards at all.
     */

    if( bin_driver->y_positive == 0x00 ) {

        /* y_address is formed by subtraction. */

        if( current_state.y_address < desired_state.y_address ) {
            internal_err( __FILE__, __LINE__ );
        }
    } else {

        /* y_address is formed by addition. */

        if( current_state.y_address > desired_state.y_address ) {
            internal_err( __FILE__, __LINE__ );
        }
    }

    if( current_state.y_address == desired_state.y_address ) {

        /* If there is no difference, reset to start of current line, except
         * for the first output line on each document page, including the
         * first document page in a section and the very first in the document.
         * This matches wgml 4.0's behavior, including the exception which
         * occurs with BX CAN and BX DEL and allows a page to start with an
         * overprinted blank line.
         */

        if( page_start && !ProcFlags.force_op ) {
            page_start = false;
        } else {
            current_state.x_address = bin_device->x_start;
            x_address = current_state.x_address;
            fb_overprint_vertical_positioning();
        }
    } else {

        /* Detect and process device pages. The need for the decrements was
         * determined by side-by-side testing with wgml 4.0. The effect, for
         * device TERM, is to map [1,20] to [0,19].
         * NOTE: checking desired_state.y_address to ensure that it is not "0"
         * should be unnecessary, unless fb_setup() is used with an initial
         * vertical position of "0".
         */

        if( current_state.y_address == 0 ) {
            current_pages = 0;
        } else {
            current_pages = (current_state.y_address - 1 ) / bin_device->page_depth;
        }
        if( desired_state.y_address == 0 ) {
            desired_pages = 0;
        } else {
            desired_pages = (desired_state.y_address - 1 ) / bin_device->page_depth;
        }
        device_pages = desired_pages - current_pages;

        /* Ensure that (current_pages + i) will contain the number of
         * device pages prior to the current device page.
         */

        current_pages++;

        /* device_pages contains the number of :NEWPAGE blocks needed. */

        if( device_pages > 0 ) {

            for( i=0; i < device_pages; i++ ) {

                /* Interpret the :NEWPAGE block. */

                df_interpret_driver_functions( bin_driver->newpage.text );

                /* Interpret the DEVICE_PAGE :PAUSE block. */

                if( bin_device->pauses.devpage_pause != NULL ) {
                    df_interpret_device_functions( bin_device->pauses.devpage_pause->text );
                }

                /* If the :ABSOLUTEADDRESS block is defined and this is the
                 * Initial Vertical Positioning, interpret the :LINEPROC
                 * :ENDVALUE block for line pass 1 of available font 0, unless
                 * it has already been done. Note: this places the block at the
                 * proper location when the :ABSOLUTEADDRESS block is defined
                 * but does not include %flushpage() and device paging has
                 * occurred.
                 */

                if( ProcFlags.has_aa_block ) {
                    if( at_start ) {
                        if( wgml_fonts[0].font_style->lineprocs != NULL ) {

                            /* Set the value of current_state.y_address and the
                             * value returned by %y_address() to the last line
                             * of the previous device page.
                             */

                            current_state.y_address = (current_pages + i) * bin_device->page_depth;
                            y_address = current_state.y_address;

                            if( wgml_fonts[0].font_style->lineprocs[0].endvalue != NULL ) {
                                df_interpret_driver_functions( wgml_fonts[0].font_style->lineprocs[0].endvalue->text );
                            }
                        }
                        at_start = false;
                    }
                }
            }
        }

        /* Set the value of current_state.x_address and the value returned by
         * device function %x_address() start of the line.
         */

        current_state.x_address = bin_device->x_start;
        x_address = current_state.x_address;

        /* Only reset current_state.y_address if one or more device pages
         * was emitted. Set y_address in either case.
         */

        if( device_pages == 0 ) {
            y_address = desired_state.y_address;
        } else {

            /* Set the value of current_state.y_address to the last line of the
             * previous device page and the value returned by device function
             * %y_address() to the correct value for the current page.
             */

            if( bin_driver->y_positive == 0x00 ) {

                /* y_address is formed by subtraction. */
                /* This should not be possible, but leave it for now. */

                current_state.y_address = (y_address + 1) - (desired_pages * bin_device->page_depth);
            } else {

                /* y_address is formed by addition. This has been confirmed
                 * in side-by-side comparison with wgml 4.0.
                 */

                current_state.y_address = (desired_pages *
                                           bin_device->page_depth) + 1;
            }

            y_address = desired_state.y_address % bin_device->page_depth;
        }

        /* If at_start is still "true", then no device paging occurred. */

        if( at_start ) {
            at_start = false;
        }

        /* If :ABSOLUTEADDRESS is not available, do the vertical positioning. */

        if( !ProcFlags.has_aa_block ) {
            fb_newline();
        }
    }
}

/* Function fb_subsequent_text_chars().
 * Performs the "subsequent text_chars instance" sequence per the Wiki.
 *
 * Parameter:
 *      in_chars points to the text_chars instance to be processed.
 *      in_lineproc points to the :LINEPROC to use
 *
 * Note:
 *      If in_lineproc is NULL, then textpass is set to "true" and text
 *          output will occur.
 */

static void fb_subsequent_text_chars( text_chars *in_chars, line_proc *in_lineproc )
{
    bool    undo_shift  = false;

    /* Initialize desired_state.x_address. */

    desired_state.x_address = in_chars->x_address;

    /* Since only the :STARTWORD (if present) is interpreted, and since device
     * funtion %textpass() is not allowed in a :STARTWORD block, it cannot be
     * set here. Instead, it retains its setting from the last time a new line
     * started or a new font was encountered.
     */

    if( !text_out_open ) {
        pre_text_output();
    }

    if( in_lineproc != NULL ) {
        if( in_lineproc->startword != NULL ) {
            df_interpret_driver_functions( in_lineproc->startword->text );
        }
    }

    /* Note that gendev ensures that %textpass() and %ulineon()/%ulineoff()
     * do not appear in the same :LINEPROC. As a result, at most one of
     * textpass and uline will be "true".
     */

    /* post_text_output() must be done if this text_chars was subscripted
     * or superscripted and the next text_chars either doesn't exist or
     * has a different type.
     */

    if( in_chars->next == NULL ) {
        undo_shift = true;
    } else if( in_chars->type != in_chars->next->type ) {
        undo_shift = true;
    } else {
        undo_shift = false;
    }

    /* If textpass is "true", output the text. The value of textpass is not
     * changed here: it will be used by fb_subsequent_text_chars().
     */

    if( textpass ) {
        fb_internal_horizontal_positioning();
        if( !text_out_open && ProcFlags.ps_device ) {
            ob_insert_ps_text_start();
            text_out_open = true;
        }
        ob_insert_block( in_chars->text, in_chars->count, true, true, in_chars->font);
        if( undo_shift && text_out_open && ProcFlags.ps_device ) {
            ob_insert_ps_text_end( htab_done, active_font );
            htab_done = false;
            text_out_open = false;
        }

    }

    /* If uline is "true", then emit the underscore characters. %dotab() must
     * be used to avoid underlining the initial spaces.
     */

    if( uline ) {
        if( !text_out_open && ProcFlags.ps_device ) {
            ob_insert_ps_text_start();
            text_out_open = true;
        }
        output_uscores( in_chars );
        if( undo_shift && text_out_open && ProcFlags.ps_device ) {
            ob_insert_ps_text_end( htab_done, active_font );
            htab_done = false;
            text_out_open = false;
        }
    }

    /* Update variables and interpret the post-output function block. */

    current_state.x_address += in_chars->width;
    x_address = current_state.x_address;

    if( in_lineproc != NULL ) {
        if( in_lineproc->endword != NULL ) {
            if( text_out_open && ProcFlags.ps_device ) {
                ob_insert_ps_text_end( htab_done, active_font );
                htab_done = false;
                text_out_open = false;
            }
            df_interpret_driver_functions( in_lineproc->endword->text );
        }
    }

    if( undo_shift ) {
        post_text_output();
    }

    return;
}

/*  Global function definitions. */

/* Function df_start_page().
 * Sets the state variables to the top of a new document page.
 */

void df_start_page( void )
{
    desired_state.x_address = bin_device->x_start;
    desired_state.y_address = bin_device->y_start;
    current_state.x_address = bin_device->x_start;
    current_state.y_address = bin_device->y_start;
    x_address = bin_device->x_start;
    y_address = bin_device->y_start;
    page_start = true;
    return;
}

/* Function df_interpret_device_functions().
 * Interprets device functions for function blocks in the :DEVICE block.
 *
 * Parameter:
 *      in_function points to a compiled function block.
 *
 * Global Variable Used:
 *      current_function_table is set to device_function_table.
*/

void df_interpret_device_functions( const char *in_function )
{
    /* Select the table and invoke the interpreter. */

    current_function_table = device_function_table;
    interpret_functions( in_function );

    return;
}

/* Function df_interpret_driver_functions().
 * Interprets device functions for function blocks in the :DRIVER block.
 *
 * Parameter:
 *      in_function points to a compiled function block.
 *
 * Global Variable Used:
 *      current_function_table is set to driver_function_table.
*/

void df_interpret_driver_functions( const char *in_function )
{
    /* Select the table and invoke the interpreter. */

    current_function_table = driver_function_table;
    interpret_functions( in_function );

    return;
}

/* Function df_populate_device_table().
 * Modifies the entries in device_function_table so that all the device
 * functions which are supposed to work for function blocks in :DEVICE blocks
 * will, in fact work. This should be called after the START :PAUSE is
 * interpreted and before the DOCUMENT :PAUSE is interpreted.
 */

void df_populate_device_table( void )
{
    device_function_table[0x27] = &df_default_width;
    device_function_table[0x2a] = &df_page_depth;
    device_function_table[0x2b] = &df_page_width;
    device_function_table[0x31] = &df_font_height;
    device_function_table[0x32] = &df_font_space;
    device_function_table[0x33] = &df_line_height;
    device_function_table[0x34] = &df_line_space;
    device_function_table[0x36] = &df_wgml_header;
    device_function_table[0x37] = &df_font_outname1;
    device_function_table[0x38] = &df_font_outname2;
    device_function_table[0x39] = &df_font_resident;
    device_function_table[0x3a] = &df_time;
    device_function_table[0x3b] = &df_date;
    return;
}

/* Function df_populate_driver_table().
 * Modifies the entry for %flushpage() in driver_function_table so that this
 * device function will now, in fact work. This should be called after the
 * virtual %enterfont(0) is performed and before the initial vertical
 * positioning.
 */

void df_populate_driver_table( void )
{
    driver_function_table[0x1D] = &df_flushpage;
    return;
}

/* Function df_setup().
 * Initializes those local variables that need more than a simple assignment
 * statement, or whose value needs to set once only based on information
 * not available when the local variable is created.
 */

void df_setup( void )
{
    int         i;
    symsub  *   sym_val = NULL;

    /* When called, each of symbols "date" and "time" contains either of
     * -- the value set from the system clock; or
     * -- the value set from the command-line option SETSYMBOL
     * but have not been affected by anything in the document specification.
     */

    find_symvar( &global_dict, "date", no_subscript, &sym_val );
    if( sym_val == NULL ) {
        date_val = char_convert( NULL );
    } else {
        date_val = char_convert( sym_val->value );
    }

    sym_val = NULL;
    find_symvar( &global_dict, "time", no_subscript, &sym_val );
    if( sym_val == NULL ) {
        time_val = char_convert( NULL );
    } else {
        time_val = char_convert( sym_val->value );
    }

    /* Set has_htab to true if the device defined the :HTAB block. */

    if( bin_driver->htab.text != NULL)
        has_htab = true;

    /* Initialize space_chars to hold 80 space characters. */

    space_chars.text = mem_alloc( 80 );
    space_chars.length = 80;
    space_chars.current = 0;
    for( i = 0; i < space_chars.length; i++ )
        space_chars.text[i] = ' ';

    /* Initialize uscore_chars to hold 80 :UNDERSCORE characters. */

    uscore_char = bin_device->underscore.underscore_char;
    uscore_chars.text = mem_alloc( 80 );
    uscore_chars.length = 80;
    uscore_chars.current = 0;
    for( i = 0; i < uscore_chars.length; i++ ) {
        uscore_chars.text[i] = uscore_char;
    }
    return;
}

/* Function df_teardown().
 * Releases the memory allocated by df_setup().
 */

void df_teardown( void )
{
    if( date_val != NULL ) {
        mem_free( date_val );
        date_val = NULL;
    }

    if( time_val != NULL ) {
        mem_free( time_val );
        time_val = NULL;
    }

    if( space_chars.text != NULL ) {
        mem_free( space_chars.text);
        space_chars.current = 0;
        space_chars.length = 0;
        space_chars.text = NULL;
    }

    if( uscore_chars.text != NULL ) {
        mem_free( uscore_chars.text);
        uscore_chars.current = 0;
        uscore_chars.length = 0;
        uscore_chars.text = NULL;
    }

    return;
}

/* Function fb_absoluteaddress().
 * Uses the :ABSOLUTEADDRESS block to actually position the print head to the
 * desired position.
 *
 * Prerequisite:
 *      The :ABSOLUTEADDRESS block must be defined.
 */

void fb_absoluteaddress( void )
{
    df_interpret_driver_functions( bin_driver->absoluteaddress.text );
    current_state.x_address = desired_state.x_address;
    current_state.y_address = desired_state.y_address;
    return;
}

/* Function fb_binclude_support().
 * Outputs the prefix for BINCLUDE when depth > 0.
 *
 * This function is extremely specialized.
 */

void fb_binclude_support( binclude_element *in_el )
{
    if( in_el->at_top ) {
        desired_state.y_address = bin_device->y_start;
        if( in_el->depth == 0 ) {
            desired_state.x_address = bin_device->x_start;
        } else {
            desired_state.x_address = in_el->cur_left;
        }
    } else {
        if( in_el->depth == 0 ) {
            desired_state.x_address = current_state.x_address;
            desired_state.y_address = current_state.y_address;
        } else {
            desired_state.x_address = in_el->cur_left;
            desired_state.y_address = in_el->y_address;
        }
    }
    if( ProcFlags.ps_device ) {   // always do ABSOLUTEADDRESS block
        y_address = desired_state.y_address;
        fb_initial_horizontal_positioning();
    } else {
        if( in_el->depth > 0 ) {    // do nothing when depth == 0
            if( current_state.y_address == desired_state.y_address ) {
                fb_overprint_vertical_positioning();
                page_start = false;
            } else {
                fb_normal_vertical_positioning();
            }
            current_state.x_address = bin_device->x_start;
            fb_initial_horizontal_positioning();
        }
    }

    return;
}

/* Function fb_empty_text_line().
 * Sets the internal and device states appropriately for an empty line.
 *
 * Parameter:
 *      out_line points to a text_line instance which has no text but which
 *          does have vertical positioning information.
 *
 * Note:
 *      This function is to be used only by fb_output_text() and is extremely
 *          specialized. The effect is that the vertical position specified
 *          is used to update the internal and device states.
 *      This function was added only when its need became clear while
 *          implementing page-based output.
 */

void fb_empty_text_line( text_line *out_line )
{
    /* Interpret a :LINEPROC :ENDVALUE block if appropriate. */

    fb_lineproc_endvalue();

    /* Update the internal state for the new text_line. */

    desired_state.y_address = out_line->y_address;

    /* Perform the Normal Vertical Positioning. */

    fb_normal_vertical_positioning();

    return;
}

/* Function fb_enterfont().
 * Performs the action of device function %enterfont(0), whether the device
 * function was invoked explicitly or implicitly. Parts of this function may
 * eventually be refactored if needed in other functions.
 *
 * Note:
 *      active_font and font number are set to "0" to ensure that all function
 *      blocks will be done in the context of the default font. They are
 *      restored to their initial value on exit.
 */

void fb_enterfont( void )
{
    font_number old_active_font;
    font_number old_df_font;

    old_active_font = active_font;
    old_df_font = df_font;
    active_font = 0;
    df_font = 0;

    if( wgml_fonts[0].font_pause != NULL ) {
        df_interpret_device_functions( wgml_fonts[0].font_pause->text );
    }

    if( wgml_fonts[0].font_switch != NULL ) {
        if( wgml_fonts[0].font_switch->startvalue != NULL ) {
            df_interpret_driver_functions( wgml_fonts[0].font_switch->startvalue->text );
        }
    }

    if( wgml_fonts[0].font_style != NULL ) {
        if( wgml_fonts[0].font_style->startvalue != NULL ) {
            df_interpret_driver_functions( wgml_fonts[0].font_style->startvalue->text );
        }
        if( wgml_fonts[0].font_style->lineprocs != NULL ) {
            if( wgml_fonts[0].font_style->lineprocs[0].startvalue != NULL ) {
                df_interpret_driver_functions( wgml_fonts[0].font_style->lineprocs[0].startvalue->text );
            }
            fb_firstword( &wgml_fonts[0].font_style->lineprocs[0] );
        }
    }
    active_font = old_active_font;
    df_font = old_df_font;

    return;
}

/* Function fb_first_text_line_pass().
 * Performs the first line pass for outputting ordinary text.
 *
 * Parameter:
 *      out_line points to a text_line instance specifying the text to be
 *          sent to the device.
 *
 * Note:
 *      This function is to be used only by fb_output_text(), which includes
 *          a test ensuring that out_line contains at least one text_chars
 *          instance (that is, that out_line->first is not NULL).
 *      Other functions are expected to be created for outputting lines
 *          created using :BOX characters.
 */

void fb_first_text_line_pass( text_line *out_line )
{
    line_proc       *cur_lineproc        = NULL;
    text_chars      *current             = NULL;

    /* Interpret a :LINEPROC :ENDVALUE block if appropriate. */

    fb_lineproc_endvalue();

    /* Update the internal state for the new text_line. */

    current = out_line->first;
    desired_state.x_address = current->x_address;
    desired_state.y_address = out_line->y_address;
    desired_state.font = current->font;
    desired_state.type = current->type;
    line_pass_number = 0;

    /* Perform the Normal Vertical Positioning. */

    fb_normal_vertical_positioning();

    /* Update the font number. */

    df_font = desired_state.font;
    active_font = desired_state.font;

    /* The First Line Pass Sequence. */

    /* The "first text_chars instance" sequence. */

    if( wgml_fonts[df_font].font_style != NULL ) {
        if( wgml_fonts[df_font].font_style->lineprocs != NULL ) {
            cur_lineproc = &wgml_fonts[df_font].font_style->lineprocs[0];
        }
    }
    x_address = desired_state.x_address;
    fb_first_text_chars( current, cur_lineproc );

    /* Now do the remaining text_chars instances. */

    for( current = current->next; current != NULL; current = current->next ) {
        desired_state.x_address = current->x_address;
        desired_state.type = current->type;
        if( current_state.font != current->font ) {
            if( wgml_fonts[current->font].font_style != NULL ) {
                if( wgml_fonts[current->font].font_style->lineprocs == NULL ) {
                    cur_lineproc = NULL;
                } else {
                    cur_lineproc = &wgml_fonts[current->font].font_style->lineprocs[0];
                }
            }
            desired_state.font = current->font;
            fb_new_font_text_chars( current, cur_lineproc );
        } else {
            fb_subsequent_text_chars( current, cur_lineproc );
        }
    }

    /* Close text output if still open at end of line. */

    if( text_out_open ) {
        if( ProcFlags.ps_device ) {
            ob_insert_ps_text_end( htab_done, active_font );
        }
        post_text_output();
    }

    return;
}


/* Function fb_graphic_support().
 * Sets the start position for GRAPHIC for the PS device.
 *
 * This function is extremely specialized.
 */

void fb_graphic_support( graphic_element *in_el )
{
    if( in_el->at_top ) {
        desired_state.y_address = bin_device->y_start;
    } else {
        desired_state.y_address = in_el->y_address;
    }
    desired_state.x_address = in_el->cur_left;
    y_address = desired_state.y_address;
    fb_initial_horizontal_positioning();

    return;
}


/* Function fb_init().
 * Interprets the :INIT block, which can contain multiple function blocks of two
 * types, one of which is interpreted for each available font.
 *
 * Parameter:
 *      in_block points to either the START :INIT block or the DOCUMENT :INIT
 *          block.
 */

void fb_init( init_block *in_block )
{
    int i;
    font_number j;
    font_number old_active_font;
    font_number old_df_font;

    /* An empty init_block is not an error. */

    if( in_block == NULL )
        return;

    /* These should be zero, but save them just to be sure. */

    old_active_font = active_font;
    old_df_font = df_font;

    /* :VALUE blocks are done once, :FONTVALUE blocks are done once for
     * each available font.
     */

    for( i = 0; i < in_block->count; i++ ) {
        if( !in_block->codeblock[i].is_fontvalue ) {
            df_interpret_driver_functions( in_block->codeblock[i].text );
        } else {
            for( j = 0; j < wgml_font_cnt; j++ ) {
                df_font = j;
                df_interpret_driver_functions( in_block->codeblock[i].text );
            }
        }
    }

    /* Restore the original values, which should be zero. */

    active_font = old_active_font;
    df_font = old_df_font;

    return;
}

/* Function fb_line_block().
 * Interprets any of the :HLINE, :VLINE, or :DBOX blocks, which define
 * attribute "thickness". Sets x_size, y_size, and thickness to match the
 * parameters.
 *
 * Parameters:
 *      in_line_block points to the :HLINE, :VLINE, or :DBOX block.
 *      h_start contains the horizontal start position.
 *      v_start contains the vertical start position.
 *      h_len contains the horizontal extent for a :HLINE or :DBOX block.
 *      v_len contains the vertical extent for a :VLINE or :DBOX block.
 *      twice indicates whether the initial AA block is to be done once or twice.
 *
 * Prerequisites:
 *      The block to be interpreted must exist.
 *      The :ABSOLUTEADDRESS block must exist.
 *
 * Notes:
 *      The :ABSOLUTEADDRESS block is required to position the print to the
 *          start of the line or box.
 *      The block must exist because the box-drawing code should be checking
 *          this and, in some cases, drawing the line or box using the :BOX
 *          block characters instead.
 *      However, in both cases, no error is reported: instead, the function
 *          interpreter will print a warning and nothing will happen. The
 *          warning is intended as an alert that further work on the block
 *          drawing code is needed.
 */

void fb_line_block( line_block *in_line_block, uint32_t h_start, uint32_t v_start,
                     uint32_t h_len, uint32_t v_len, bool twice )
{
    /* Set up for fb_absoluteaddress(). */

    desired_state.x_address = h_start;
    desired_state.y_address = v_start;
    x_address = h_start;
    y_address = v_start % bin_device->page_depth;

    fb_absoluteaddress();
    if( twice ) {
        fb_absoluteaddress();
    }

    /* Set up for in_function; current_state has been updated by
     * fb_absoluteaddress().
     */

    if( (h_len != 0) || (v_len != 0) ) { // do not draw if it has no length at all
        x_size = h_len;
        y_size = v_len;
        thickness = in_line_block->thickness;
        df_interpret_driver_functions( in_line_block->text );

        /* Clear the values not needed outside this block. */

        x_size = 0;
        y_size = 0;
        thickness = 0;
    }

    return;
}

/* Function fb_lineproc_endvalue()
 * Checks the value of the textpass and uline and interprets the :LINEPROC
 * :ENDVALUE block for the current font if either is "true".
 */

void fb_lineproc_endvalue( void )
{
    static  bool    local_start = true;

    /* Suppress the first call. This matches wgml 4.0's behavior. */

    if( local_start ) {
        local_start = false;
        return;
    }

    if( textpass || uline ) {
        if( text_out_open ) {
            if( ProcFlags.ps_device ) {
                ob_insert_ps_text_end( htab_done, active_font );
            }
            post_text_output();
        }
        if( wgml_fonts[df_font].font_style->lineprocs != NULL ) {
            if( wgml_fonts[df_font].font_style->lineprocs[line_pass_number].endvalue != NULL ) {
                df_interpret_driver_functions( wgml_fonts[df_font].font_style->lineprocs[line_pass_number].endvalue->text );
            }
        }
    }
    textpass = false;

    return;
}

/* Function fb_new_section().
 * Performs the Initial Vertical Positioning, except for the first instance,
 * for which see fb_position(). This function is very specialized.
 *
 * Parameter:
 *      v_start contains the desired starting vertical position.
 *
 * Note:
 *      This function should not be invoked at the start of the file; instead,
 *      fb_position() should be used as it will do the first initial vertical
 *      positioning. This function should be used in place of fb_document_page()
 *      when a new section or other event occurs where its action is needed, as
 *      partially described in the Wiki.
 */

void fb_new_section( uint32_t v_start )
{
    font_number font_save;

    /* Interpret a :LINEPROC :ENDVALUE block if appropriate. */

    fb_lineproc_endvalue();

    /* Save active_font and set it to 0 for the :NEWPAGE and :NEWLINE blocks. */

    font_save = active_font;
    active_font = 0;

    /* Interpret the :NEWPAGE block. */

    df_interpret_driver_functions( bin_driver->newpage.text );

    /* Interpret the DOCUMENT_PAGE :PAUSE block. */

    if( bin_device->pauses.docpage_pause != NULL ) {
        df_interpret_device_functions( bin_device->pauses.docpage_pause->text );
    }

    /* Set up for a new document page. */

    df_start_page();

    /* Do the initial vertical positioning for the section. Now that .sk -1
     * is supported, this must not be done if a :NEWLINE with advance 0
     * would be interpreted.
     */

    desired_state.y_address = v_start;
    if( current_state.y_address != desired_state.y_address ) {
        fb_normal_vertical_positioning();
    }

    /* Restore the value of active_font. This ensures that the next font
     * switch decision and font switch, if any, will be done using the
     * correct fonts.
     */

    active_font = font_save;

    return;
}

/* Function fb_position().
 * Initializes the print head position. This function is very specialized.
 *
 * Parameters:
 *      h_start contains the horizontal position.
 *      v_start contains the vertical position.
 *
 * Globals Modified:
 *      desired_state.x_address is set to h_start.
 *      desired_state.y_address is set to v_start.
 *      x_address is set to h_start
 *      y_address is set to v_start
 *
 * Notes:
 *      This function performs the first initial vertical positioning, as
 *          described in the Wiki.
 *      This is intended to be called only once, at the start of document
 *          processing.
 *      The internal state is updated without regard to whether any function
 *          blocks, or which function blocks, are interpreted.
 */

void fb_position( uint32_t h_start, uint32_t v_start )
{
    /* Set the desired state. */

    desired_state.x_address = h_start;
    desired_state.y_address = v_start;
    y_address = v_start;

    /* Do the vertical positioning if appropriate. */

    if( current_state.y_address != v_start ) {
        fb_normal_vertical_positioning();
    }

    /* If the :FONTSTYLE block exists, interpret the appropriate blocks. */

    if( wgml_fonts[0].font_style != NULL ) {
        if( wgml_fonts[0].font_style->lineprocs != NULL ) {
            df_interpret_driver_functions( wgml_fonts[0].font_style->lineprocs[0].endvalue->text );
            x_address = h_start;    // to match wgml 4.0
            df_interpret_driver_functions( wgml_fonts[0].font_style->lineprocs[0].startvalue->text );
            fb_firstword( &wgml_fonts[0].font_style->lineprocs[0] );
        }
    }
    return;
}

/* Function fb_subsequent_text_line_pass().
 * Performs the subsequent pass for outputting ordinary text.
 *
 * Parameter:
 *      out_line points to a text_line instance specifying the text to be
 *          sent to the device.
 *      line_pass contains the number of the current line_pass
 *
 * Note:
 *      This function is to be used only by fb_output_text(), which includes
 *          a test ensuring that out_line contains at least one text_chars
 *          instance (that is, that out_line->first is not NULL).
 *      Other functions are expected to be created for outputting lines
 *          created using :BOX characters.
 */

void fb_subsequent_text_line_pass( text_line *out_line, uint16_t line_pass )
{
    bool            tc_skipped;
    fontstyle_block *cur_fontstyle   = NULL;
    line_proc       *cur_lineproc;
    text_chars      *current         = NULL;

    /* Interpret a :LINEPROC :ENDVALUE block if appropriate. */

    fb_lineproc_endvalue();
    line_pass_number = line_pass;

    /* Update the internal state to the first text_chars that uses a
     * :FONTSTYLE block which has a :LINEPROC block for this pass.
     */

    cur_lineproc = NULL;
    for( current = out_line->first; current != NULL; current = current->next ) {
        cur_fontstyle = wgml_fonts[current->font].font_style;
        if( cur_fontstyle != NULL ) {
            if( line_pass < cur_fontstyle->line_passes ) {
                cur_lineproc = &cur_fontstyle->lineprocs[line_pass];
                break;
            }
        }
    }

    /* If current is NULL, this line does not have this pass. */

    if( current == NULL ) {
        internal_err( __FILE__, __LINE__ );
        return;
    }

    desired_state.x_address = current->x_address;
    desired_state.font = current->font;
    desired_state.type = current->type;

    /* Perform the Overprint Vertical Positioning. */

    fb_overprint_vertical_positioning();

    /* Update the font number. */

    df_font = desired_state.font;
    active_font = desired_state.font;

    /* The Subsequent Line Pass Sequence. */

    /* The "first text_chars instance" sequence. */

    fb_first_text_chars( current, cur_lineproc );

    /* Now do the remaining text_chars instances. */

    tc_skipped = false;
    for( current = current->next; current != NULL; current = current->next ) {
        if( (cur_fontstyle = wgml_fonts[current->font].font_style) == NULL )
            continue;
        if( line_pass >= cur_fontstyle->line_passes ) {
            tc_skipped = true;
            continue;
        }
        cur_lineproc = &cur_fontstyle->lineprocs[line_pass];
        desired_state.x_address = current->x_address;
        desired_state.type = current->type;
        if( current_state.font != current->font ) {
            desired_state.font = current->font;
            fb_new_font_text_chars( current, cur_lineproc );
        } else {
            if( tc_skipped ) {

                /* Close and reopen the font style. This prevents
                 * font style underline from underlining the
                 * intervening text between two separated text_chars.
                 */

                if( cur_lineproc->endvalue != NULL ) {
                    df_interpret_driver_functions( cur_lineproc->endvalue->text );
                }

                if( cur_fontstyle->endvalue != NULL ) {
                    df_interpret_driver_functions( cur_fontstyle->endvalue->text );
                }

                fb_internal_horizontal_positioning();

                if( cur_fontstyle->startvalue != NULL ) {
                    df_interpret_driver_functions( cur_fontstyle->startvalue->text );
                }

                if( cur_lineproc->startvalue != NULL ) {
                    df_interpret_driver_functions( cur_lineproc->startvalue->text );
                }

                fb_firstword( cur_lineproc );
            }

            fb_subsequent_text_chars( current, cur_lineproc );
        }
        tc_skipped = false;
    }

    /* Close text output if still open at end of line. */

    if( text_out_open ) {
        if( ProcFlags.ps_device ) {
            ob_insert_ps_text_end( htab_done, active_font );
        }
        post_text_output();
    }
}
