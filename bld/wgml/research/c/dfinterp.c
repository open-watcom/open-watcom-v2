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
* Description:  Implements the function declared in dfinterp.h:
*                   interpret_function()
*               and many items which will probably be global in wgml:
*               typedefs and structs:
*                   current_df_data
*                   df_data
*                   df_function
*                   function_table
*                   parameters
*               functions:
*                   char_literal
*                   df_add
*                   df_bad_code
*                   df_binary
*                   df_binary1
*                   df_binary2
*                   df_binary4
*                   df_cancel
*                   df_clear3270
*                   df_clearPC
*                   df_date
*                   df_decimal
*                   df_default_width
*                   df_divide
*                   df_dotab
*                   df_endif
*                   df_enterfont
*                   df_font_height
*                   df_font_number
*                   df_font_outname1
*                   df_font_outname2
*                   df_font_resident
*                   df_font_space
*                   df_flushpage
*                   df_getstrsymbol
*                   df_getnumsymbol
*                   df_hex
*                   df_ifeqn
*                   df_ifnen
*                   df_ifeqs
*                   df_ifnes
*                   df_image
*                   df_line_height
*                   df_line_space
*                   df_lower
*                   df_page_depth
*                   df_page_width
*                   df_pages
*                   df_recordbreak
*                   df_remainder
*                   df_setsymbol
*                   df_sleep
*                   df_subtract
*                   df_tab_width
*                   df_text
*                   df_textpass
*                   df_thickness
*                   df_time
*                   df_ulineoff
*                   df_ulineon
*                   df_wait
*                   df_wgml_header
*                   df_x_address
*                   df_x_size
*                   df_y_address
*                   df_y_size
*                   get_parameters
*                   numeric_literal
*                   process_parameter
*
* Notes:        The Wiki should be consulted for any term whose meaning is
*               not apparent. This should help in most cases.
*
*               Information on the device functions can be found in the
*               Wiki. This is a first version of the interpreter.
*
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "bool.h"
#include "common.h"
#include "dfinterp.h"

/* The "local" items may well become global in wgml. */

/* Local data definitions. */

/* This will be simplified or eliminated in wgml. */

typedef struct {
    bool        warning;
    char        *base;
    char        *current;
    uint8_t     parameter_type;
    uint8_t     df_code;
} df_data;

static df_data current_df_data;

/* This should still be useful in wgml. */

typedef struct {
    uint16_t    first;
    uint16_t    second;
} parameters;

/* This will become void * ( *df_function ) (void) in wgml. */

typedef void (*df_function) (void);

static df_function function_table[0x3D];

/* Local function definitions. */

/* Function df_bad_code().
 * Reports byte codes not known to be used.
 */

static void df_bad_code( void )
{
    printf_s( "\nUnknown byte code: 0x%02x -- parameter type: 0x%02x\n", current_df_data.df_code, current_df_data.parameter_type);
    return;
}

/* These functions are for device functions which take no parameters. */

/* Type I device functions have an associated parameter type byte, which should
 * always be "0x00" for the functions in this section. 
 */

/* Function df_clear3270().
 * Implements device function %clear3270().
 */
 
static void df_clear3270( void )
{
    if( current_df_data.parameter_type != 0x00 )
        printf_s( "Incorrect parameter type 0x%02x for device function ", current_df_data.parameter_type );
    puts( "%clear3270()" );
    return;
}

/* Function df_clearPC().
 * Implements device function %clearPC().
 */
 
static void df_clearPC( void )
{
    if( current_df_data.parameter_type != 0x00 )
        printf_s( "Incorrect parameter type 0x%02x for device function ", current_df_data.parameter_type );
    puts( "%clearPC()" );
    return;
}

/* Function df_dotab().
 * Implements device function %dotab().
 */
 
static void df_dotab( void )
{
    if( current_df_data.parameter_type != 0x00 )
        printf_s( "Incorrect parameter type 0x%02x for device function ", current_df_data.parameter_type );
    puts( "%dotab()" );
    return;
}

/* Function df_endif().
 * Implements device function %endif().
 */
 
static void df_endif( void )
{
    if( current_df_data.parameter_type != 0x00 )
        printf_s( "Incorrect parameter type 0x%02x for device function ", current_df_data.parameter_type );
    puts( "%endif()" );
    return;
}

/* Function df_flushpage().
 * Implements device function %flushpage().
 */
 
static void df_flushpage( void )
{
    if( current_df_data.parameter_type != 0x00 )
        printf_s( "Incorrect parameter type 0x%02x for device function ", current_df_data.parameter_type );
    puts( "%flushpage()" );
    return;
}

/* Function df_recordbreak().
 * Implements device function %recordbreak().
 */
 
static void df_recordbreak( void )
{
    if( current_df_data.parameter_type != 0x00 )
        printf_s( "Incorrect parameter type 0x%02x for device function ", current_df_data.parameter_type );
    puts( "%recordbreak()" );
    return;
}

/* Function df_textpass().
 * Implements device function %textpass().
 */
 
static void df_textpass( void )
{
    if( current_df_data.parameter_type != 0x00 )
        printf_s( "Incorrect parameter type 0x%02x for device function ", current_df_data.parameter_type );
    puts( "%textpass()" );
    return;
}

/* Function df_ulineoff().
 * Implements device function %ulineoff().
 */
 
static void df_ulineoff( void )
{
    if( current_df_data.parameter_type != 0x00 )
        printf_s( "Incorrect parameter type 0x%02x for device function ", current_df_data.parameter_type );
    puts( "%ulineoff()" );
    return;
}

/* Function df_ulineon().
 * Implements device function %ulineon().
 */
 
static void df_ulineon( void )
{
    if( current_df_data.parameter_type != 0x00 )
        printf_s( "Incorrect parameter type 0x%02x for device function ", current_df_data.parameter_type );
    puts( "%ulineon()" );
    return;
}

/* Function df_wait().
 * Implements device function %wait().
 */
 
static void df_wait( void )
{
    if( current_df_data.parameter_type != 0x00 )
        printf_s( "Incorrect parameter type 0x%02x for device function ", current_df_data.parameter_type );
    puts( "%wait()" );
    return;
}

/* Type II device functions have no parameter type byte. */

/* Function df_date().
 * Implements device function %date().
 */
 
static void df_date( void )
{
    printf_s( "%%date()" );
    return;
}

/* Function df_default_width().
 * Implements device function %default_width().
 */
 
static void df_default_width( void )
{
    printf_s( "%%default_width()" );
    return;
}

/* Function df_font_height().
 * Implements device function %font_height().
 */
 
static void df_font_height( void )
{
    printf_s( "%%font_height()" );
    return;
}

/* Function df_font_number().
 * Implements device function %font_number().
 */
 
static void df_font_number( void )
{
    printf_s( "%%font_number()" );
    return;
}

/* Function df_font_outname1().
 * Implements device function %font_outname1().
 */
 
static void df_font_outname1( void )
{
    printf_s( "%%font_outname1()" );
    return;
}

/* Function df_font_outname2().
 * Implements device function %font_outname2().
 */
 
static void df_font_outname2( void )
{
    printf_s( "%%font_outname2()" );
    return;
}

/* Function df_font_resident().
 * Implements device function %font_resident().
 */
 
static void df_font_resident( void )
{
    printf_s( "%%font_resident()" );
    return;
}

/* Function df_font_space().
 * Implements device function %font_space().
 */
 
static void df_font_space( void )
{
    printf_s( "%%font_space()" );
    return;
}

/* Function df_line_height().
 * Implements device function %line_height().
 */
 
static void df_line_height( void )
{
    printf_s( "%%line_height()" );
    return;
}

/* Function df_line_space().
 * Implements device function %line_space().
 */
 
static void df_line_space( void )
{
    printf_s( "%%line_space()" );
    return;
}

/* Function df_page_depth().
 * Implements device function %page_depth().
 */
 
static void df_page_depth( void )
{
    printf_s( "%%page_depth()" );
    return;
}

/* Function df_page_width().
 * Implements device function %page_width().
 */
 
static void df_page_width( void )
{
    printf_s( "%%page_width()" );
    return;
}

/* Function df_pages().
 * Implements device function %pages().
 */
 
static void df_pages( void )
{
    printf_s( "%%pages()" );
    return;
}

/* Function df_tab_width().
 * Implements device function %tab_width().
 */
 
static void df_tab_width( void )
{
    printf_s( "%%tab_width()" );
    return;
}

/* Function df_thickness().
 * Implements device function %thickness().
 */
 
static void df_thickness( void )
{
    printf_s( "%%thickness()" );
    return;
}

/* Function df_time().
 * Implements device function %time().
 */
 
static void df_time( void )
{
    printf_s( "%%time()" );
    return;
}

/* Function df_wgml_header().
 * Implements device function %wgml_header().
 */
 
static void df_wgml_header( void )
{
    printf_s( "%%wgml_header()" );
    return;
}

/* Function df_x_address().
 * Implements device function %x_address().
 */
 
static void df_x_address( void )
{
    printf_s( "%%x_address()" );
    return;
}

/* Function df_x_size().
 * Implements device function %x_size().
 */
 
static void df_x_size( void )
{
    printf_s( "%%x_size()" );
    return;
}

/* Function df_y_address().
 * Implements device function %y_address().
 */
 
static void df_y_address( void )
{
    printf_s( "%%y_address()" );
    return;
}

/* Function df_y_size().
 * Implements device function %y_size().
 */
 
static void df_y_size( void )
{
    printf_s( "%%y_size()" );
    return;
}

/* Parameter block parsing functions. */

/* Function get_parameters().
 * Place offset2 and offset3 into in_parameters->first and
 * in_parameters->second. This function also skips both offset1 and offset4.
 *
 * Parameter:
 *      in_parameters points to the parameters instance to be initialized.
 *
 * Global Variable Used:
 *      current_df_data.current must point to the first byte of 
 *      offset1 on entry.
 *
 * Global Variable Modified:
 *      current_df_data.current will point to the first byte after
 *      offset4 on exit.
 *
*/

static void get_parameters ( parameters * in_parameters )
{
    uint16_t    offset;

    /* Skip the offset1 value. */

    memcpy_s( &offset, sizeof( offset ), current_df_data.current, sizeof( offset ) );
    current_df_data.current += sizeof( offset );

    /* Get the first parameter offset (offset2). */

    memcpy_s( &in_parameters->first, sizeof( in_parameters->first ), current_df_data.current, sizeof( in_parameters->first ) );
    current_df_data.current += sizeof( in_parameters->first );

    /* Get the second parameter offset (offset3). */

    memcpy_s( &in_parameters->second, sizeof( in_parameters->second ), current_df_data.current, sizeof( in_parameters->second ) );
    current_df_data.current += sizeof( in_parameters->second );

    /* Skip the offset4 value. */

    memcpy_s( &offset, sizeof( offset ), current_df_data.current, sizeof( offset ) );
    current_df_data.current += sizeof( offset );

    return;    
}

/* Function process_parameter().
 * Extracts the byte code from the Directive instance, updates
 * current_df_data.current, and invokes the function which handles
 * that byte code.
 *
 * Global Variable Used:
 *      current_df_data.current must point to the first byte in the
 *      Directive instance. 
 *
 * Global Variable Altered:
 *      current_df_data will be altered as needed to parse the rest of the
 *      function sequence.
*/

void process_parameter( void )
{
    /* Reset current_df_data for the parameter. */

    memcpy_s( &current_df_data.df_code, sizeof( current_df_data.df_code ), current_df_data.current, sizeof( current_df_data.df_code ) );
    current_df_data.current += sizeof( current_df_data.df_code );

    /* Invoke parameter function. */

    function_table[current_df_data.df_code]();

    return;
}

/* Functions which use parameters.
 * These functions all take parameters and so have common effects on the global
 * current_df_data. 
 *
 * Global Variables Used:
 *      current_df_data is updated to reflect the current function.
 *      function_table[] is used to invoke the handler for each device function.
 *
 * Global Variable Modified:
 *      current_df_data.current will point to the first byte after
 *      the byte code for the function being processed when the handler in
 *      function_table[] is invoked. This occurs when a parameter block is
 *      present.
 *
 *      current_df_data.current will point to the first byte after
 *      the character data found by %image() and %text() when they have a
 *      parameter which is not in a parameter block.
 */

/* Device functions %image() and %text() have an associated parameter type byte
 * which can be either "0x00" (for a literal parameter) or "0x10" (for a
 * parameter block).
 */

/* Function df_image().
 * Implements device function %image().
 */
 
static void df_image( void )
{
    int         i;
    parameters  my_parameters;
    uint16_t    count;

    switch( current_df_data.parameter_type) {
    case 0x00:

        /* Character literal parameter. */
        
        memcpy_s( &count, sizeof( count ), current_df_data.current, sizeof( count ) );
        current_df_data.current += sizeof( count );

        for( i = 0; i < count; i++ ) {
            if( isprint( current_df_data.current[i] ) ) {

                /* Emit an %image() function with character parameter. */

                printf_s( "%%image('" );
                for( ; i < count; i++ ) {
                    if( !isprint( current_df_data.current[i] ) ) break;
                    printf_s( "%c", current_df_data.current[i] );
               }
                puts( "')" );

            } else {

            /* The current character cannot have been an %image() parameter. */
            
                printf_s( "%%binary($%02x)\n", current_df_data.current[i] );

            }
        }
        break;

    case 0x10:

        /* Parameter block. */

        /* Ensure that this is either a ShortHeader or a LongHeader. */

        get_parameters( &my_parameters );
        if( (my_parameters.first != 0x0009) && (my_parameters.first != 0x000d) ) {
            printf_s( "Neither ShortHeader nor LongHeader found: offset is: 0x%04x\n", my_parameters.first );
            return;
        }

        /* Now invoke the parameter's handler. */

        printf_s( "%%image(" );
        current_df_data.current = current_df_data.base + my_parameters.first;
        process_parameter();
        puts( ")" );

        break;

    default:
        printf_s( "Incorrect parameter type: 0x%02x for device function ", current_df_data.parameter_type );
        puts( "%image(-?-)" );
    }
    return;
}

/* Function df_text().
 * Implements device function %text().
 */
 
static void df_text( void )

{
    int         i;
    parameters  my_parameters;
    uint16_t    count;
    
    switch( current_df_data.parameter_type) {
    case 0x00:

        /* Character literal parameter. */

        memcpy_s( &count, sizeof( count ), current_df_data.current, sizeof( count ) );
        current_df_data.current += sizeof( count );

        current_df_data.warning = false;

        /* Emit a %text() function with character parameter. */

        printf_s( "%%text('" );
        for( i = 0; i < count; i++ ) {
            printf_s( "%c", current_df_data.current[i] );
            if( !isprint( current_df_data.current[i] ) )
                current_df_data.warning = true;
        }
        puts( "')" );
        if( current_df_data.warning )
            puts( "String shown above may contain unprintable characters." );
        break;

    case 0x10:

        /* Parameter block. */

        /* Ensure that this is either a ShortHeader or a LongHeader. */

        get_parameters( &my_parameters );
        if( (my_parameters.first != 0x0009) && (my_parameters.first != 0x000d) ) {
            printf_s( "Neither ShortHeader nor LongHeader found: offset is: 0x%04x\n", my_parameters.first );
            return;
        }

        /* Now invoke the parameter's handler. */

        printf_s( "%%text(" );
        current_df_data.current = current_df_data.base + my_parameters.first;
        process_parameter();
        puts( ")" );

        break;

    default:
        printf_s( "Incorrect parameter type: 0x%02x for device function ", current_df_data.parameter_type );
        puts( "%text(-?-)" );
    }
    return;
}

/* These functions handle parameter blocks containing literals. */

/* Function char_literal().
 * Displays the value of a character literal.
 */
 
static void char_literal( void )
{
    int         i;
    uint16_t    count;

    /* Skip the rest of the Directive. */

    current_df_data.current += 0x0c;

    /* Get the count. */

    memcpy_s( &count, sizeof( count ), current_df_data.current, sizeof( count ) );
    current_df_data.current += sizeof( count );

    /* Output the char literal, with delimiters */

    printf_s( "'" );
    for( i = 0; i < count; i++ ) {
        printf_s( "%c", current_df_data.current[i] );
        if( !isprint( current_df_data.current[i] ) )
            current_df_data.warning = true;
    }
    printf_s( "'" );

    return;
}

/* Function numeric_literal().
 * Displays the value of a numeric literal.
 */
 
static void numeric_literal( void )
{
    uint16_t    value;

    /* Skip the Offsets. */

    current_df_data.current += 0x08;

    /* Get and print the value. */

    memcpy_s( &value, sizeof( value ), current_df_data.current, sizeof( value ) );
    printf_s( "$%04x", value );

    return;
}

/* These functions take parameters in parameter blocks only. */

/* Type I device functions have an associated parameter type byte, which should
 * always be "0x10" for the functions in this section.
 */

/* Function df_cancel().
 * Implements device function %cancel().
 */
 
static void df_cancel( void )
{
    parameters   my_parameters;
    
    /* Ensure that this is either a ShortHeader or a LongHeader. */

    get_parameters( &my_parameters );
    if( (my_parameters.first != 0x0009) && (my_parameters.first != 0x000d) ) {
        printf_s( "Neither ShortHeader nor LongHeader found: offset is: 0x%04x\n", my_parameters.first );
        return;
    }

    /* Ensure the parameter_type is correct */

    if( current_df_data.parameter_type != 0x10 )
        printf_s( "Incorrect parameter type 0x%02x for device function ", current_df_data.parameter_type );

    /* Now invoke the parameter's handler. */

    printf_s( "%%cancel(" );
    current_df_data.warning = false;
    current_df_data.current = current_df_data.base + my_parameters.first;
    process_parameter();
    puts( ")" );
    if( current_df_data.warning )
        puts( "String shown above may contain unprintable characters." );

    return;
}

/* Function df_enterfont().
 * Implements device function %enterfont().
 */
 
static void df_enterfont( void )
{
    parameters   my_parameters;

    /* Ensure that this is either a ShortHeader or a LongHeader. */

    get_parameters( &my_parameters );
    if( (my_parameters.first != 0x0009) && (my_parameters.first != 0x000d) ) {
        printf_s( "Neither ShortHeader nor LongHeader found: offset is: 0x%04x\n", my_parameters.first );
        return;
    }

    /* Ensure the parameter_type is correct */

    if( current_df_data.parameter_type != 0x10 )
        printf_s( "Incorrect parameter type 0x%02x for device function ", current_df_data.parameter_type );

    /* Now invoke the parameter's handler. */

    printf_s( "%%enterfont(" );
    current_df_data.warning = false;
    current_df_data.current = current_df_data.base + my_parameters.first;
    process_parameter();
    puts( ")" );
    if( current_df_data.warning )
        puts( "String(s) shown above may contain unprintable characters." );

    return;
}

/* Function df_sleep().
 * Implements device function %sleep().
 *
 * Note:
 *      Added for completeness. This device function causes gendev to hang
 *      if used with a literal, and wgml to hang if used with a non-literal.
 */
 
static void df_sleep( void )
{
    parameters   my_parameters;

    /* Ensure that this is either a ShortHeader or a LongHeader. */

    get_parameters( &my_parameters );
    if( (my_parameters.first != 0x0009) && (my_parameters.first != 0x000d) ) {
        printf_s( "Neither ShortHeader nor LongHeader found: offset is: 0x%04x\n", my_parameters.first );
        return;
    }

    /* Ensure the parameter_type is correct */

    if( current_df_data.parameter_type != 0x10 )
        printf_s( "Incorrect parameter type 0x%02x for device function ", current_df_data.parameter_type );

    /* Now invoke the parameter's handler. */

    printf_s( "%%sleep(" );
    current_df_data.warning = false;

    /* This is wrong because gendev generates a ShortHeader but encodes the first
     * parameter as if it were a LongHeader. Hence, the literal.
     * The original line:
     * current_df_data.current = current_df_data.base + my_parameters.first;
     */

    current_df_data.current = current_df_data.base + 0x09;
    process_parameter();
    puts( ")" );
    if( current_df_data.warning )
        puts( "String(s) shown above may contain unprintable characters." );

    return;
}

/* Function df_setsymbol().
 * Implements device function %setsymbol().
 */

static void df_setsymbol( void )
{
    parameters   my_parameters;

    /* Ensure that this is either a ShortHeader or a LongHeader. */

    get_parameters( &my_parameters );
    if( (my_parameters.first != 0x0009) && (my_parameters.first != 0x000d) ) {
        printf_s( "Neither ShortHeader nor LongHeader found: offset is: 0x%04x\n", my_parameters.first );
        return;
    }

    /* Ensure the parameter_type is correct */

    if( current_df_data.parameter_type != 0x10 )
        printf_s( "Incorrect parameter type 0x%02x for device function ", current_df_data.parameter_type );

    /* Now invoke the first parameter's handler. */

    printf_s( "%%setsymbol(" );
    current_df_data.warning = false;
    current_df_data.current = current_df_data.base + my_parameters.first;
    process_parameter();
    printf_s( "," );

    /* Now invoke the second parameter's handler. */

    current_df_data.current = current_df_data.base + my_parameters.second;
    process_parameter();

    puts( ")" );
    if( current_df_data.warning )
        puts( "String(s) shown above may contain unprintable characters." );

    return;
}

/* Function df_binary1().
 * Implements device function %binary1().
 *
 * Note:
 *      Device function %binary() has the same byte code, and hence uses
 *      the same implementation, as device function %binary1().
 */
 
static void df_binary1( void )
{
    parameters   my_parameters;

    /* Ensure that this is either a ShortHeader or a LongHeader. */

    get_parameters( &my_parameters );
    if( (my_parameters.first != 0x0009) && (my_parameters.first != 0x000d) ) {
        printf_s( "Neither ShortHeader nor LongHeader found: offset is: 0x%04x\n", my_parameters.first );
        return;
    }

    /* Ensure the parameter_type is correct */

    if( current_df_data.parameter_type != 0x10 )
        printf_s( "Incorrect parameter type 0x%02x for device function ", current_df_data.parameter_type );

    /* Now invoke the parameter's handler. */

    printf_s( "%%binary1(" );
    current_df_data.warning = false;
    current_df_data.current = current_df_data.base + my_parameters.first;
    process_parameter();
    puts( ")" );
    if( current_df_data.warning )
        puts( "String(s) shown above may contain unprintable characters." );

    return;
}

/* Function df_binary2().
 * Implements device function %binary2().
 */
 
static void df_binary2( void )
{
    parameters   my_parameters;

    /* Ensure that this is either a ShortHeader or a LongHeader. */

    get_parameters( &my_parameters );
    if( (my_parameters.first != 0x0009) && (my_parameters.first != 0x000d) ) {
        printf_s( "Neither ShortHeader nor LongHeader found: offset is: 0x%04x\n", my_parameters.first );
        return;
    }

    /* Ensure the parameter_type is correct */

    if( current_df_data.parameter_type != 0x10 )
        printf_s( "Incorrect parameter type 0x%02x for device function ", current_df_data.parameter_type );

    /* Now invoke the parameter's handler. */

    printf_s( "%%binary2(" );
    current_df_data.warning = false;
    current_df_data.current = current_df_data.base + my_parameters.first;
    process_parameter();
    puts( ")" );
    if( current_df_data.warning )
        puts( "String(s) shown above may contain unprintable characters." );

    return;
}

/* Function df_binary4().
 * Implements device function %binary4().
 */
 
static void df_binary4( void )
{
    parameters   my_parameters;

    /* Ensure that this is either a ShortHeader or a LongHeader. */

    get_parameters( &my_parameters );
    if( (my_parameters.first != 0x0009) && (my_parameters.first != 0x000d) ) {
        printf_s( "Neither ShortHeader nor LongHeader found: offset is: 0x%04x\n", my_parameters.first );
        return;
    }

    /* Ensure the parameter_type is correct */

    if( current_df_data.parameter_type != 0x10 )
        printf_s( "Incorrect parameter type 0x%02x for device function ", current_df_data.parameter_type );

    /* Now invoke the parameter's handler. */

    printf_s( "%%binary4(" );
    current_df_data.warning = false;
    current_df_data.current = current_df_data.base + my_parameters.first;
    process_parameter();
    puts( ")" );
    if( current_df_data.warning )
        puts( "String(s) shown above may contain unprintable characters." );

    return;
}

/* Function df_ifeqn().
 * Implements device function %ifeqn().
 */
 
static void df_ifeqn( void )
{
    parameters   my_parameters;

    /* Ensure that this is either a ShortHeader or a LongHeader. */

    get_parameters( &my_parameters );
    if( (my_parameters.first != 0x0009) && (my_parameters.first != 0x000d) ) {
        printf_s( "Neither ShortHeader nor LongHeader found: offset is: 0x%04x\n", my_parameters.first );
        return;
    }

    /* Ensure the parameter_type is correct */

    if( current_df_data.parameter_type != 0x10 )
        printf_s( "Incorrect parameter type 0x%02x for device function ", current_df_data.parameter_type );

    /* Now invoke the first parameter's handler. */

    printf_s( "%%ifeqn(" );
    current_df_data.warning = false;
    current_df_data.current = current_df_data.base + my_parameters.first;
    process_parameter();
    printf_s( "," );

    /* Now invoke the second parameter's handler. */

    current_df_data.current = current_df_data.base + my_parameters.second;
    process_parameter();

    puts( ")" );
    if( current_df_data.warning )
        puts( "String(s) shown above may contain unprintable characters." );

    return;
}

/* Function df_ifnen().
 * Implements device function %ifnen().
 */
 
static void df_ifnen( void )
{
    parameters   my_parameters;

    /* Ensure that this is either a ShortHeader or a LongHeader. */

    get_parameters( &my_parameters );
    if( (my_parameters.first != 0x0009) && (my_parameters.first != 0x000d) ) {
        printf_s( "Neither ShortHeader nor LongHeader found: offset is: 0x%04x\n", my_parameters.first );
        return;
    }

    /* Ensure the parameter_type is correct */

    if( current_df_data.parameter_type != 0x10 )
        printf_s( "Incorrect parameter type 0x%02x for device function ", current_df_data.parameter_type );

    /* Now invoke the first parameter's handler. */

    printf_s( "%%ifnen(" );
    current_df_data.warning = false;
    current_df_data.current = current_df_data.base + my_parameters.first;
    process_parameter();
    printf_s( "," );

    /* Now invoke the second parameter's handler. */

    current_df_data.current = current_df_data.base + my_parameters.second;
    process_parameter();

    puts( ")" );
    if( current_df_data.warning )
        puts( "String(s) shown above may contain unprintable characters." );

    return;
}

/* Function df_ifeqs().
 * Implements device function %ifeqs().
 */
 
static void df_ifeqs( void )
{
    parameters   my_parameters;

    /* Ensure that this is either a ShortHeader or a LongHeader. */

    get_parameters( &my_parameters );
    if( (my_parameters.first != 0x0009) && (my_parameters.first != 0x000d) ) {
        printf_s( "Neither ShortHeader nor LongHeader found: offset is: 0x%04x\n", my_parameters.first );
        return;
    }

    /* Ensure the parameter_type is correct */

    if( current_df_data.parameter_type != 0x10 )
        printf_s( "Incorrect parameter type 0x%02x for device function ", current_df_data.parameter_type );

    /* Now invoke the first parameter's handler. */

    printf_s( "%%ifeqs(" );
    current_df_data.warning = false;
    current_df_data.current = current_df_data.base + my_parameters.first;
    process_parameter();
    printf_s( "," );

    /* Now invoke the second parameter's handler. */

    current_df_data.current = current_df_data.base + my_parameters.second;
    process_parameter();

    puts( ")" );
    if( current_df_data.warning )
        puts( "String(s) shown above may contain unprintable characters." );

    return;
}

/* Function df_ifnes().
 * Implements device function %ifnes().
 */
 
static void df_ifnes( void )
{
    parameters   my_parameters;

    /* Ensure that this is either a ShortHeader or a LongHeader. */

    get_parameters( &my_parameters );
    if( (my_parameters.first != 0x0009) && (my_parameters.first != 0x000d) ) {
        printf_s( "Neither ShortHeader nor LongHeader found: offset is: 0x%04x\n", my_parameters.first );
        return;
    }

    /* Ensure the parameter_type is correct */

    if( current_df_data.parameter_type != 0x10 )
        printf_s( "Incorrect parameter type 0x%02x for device function ", current_df_data.parameter_type );

    /* Now invoke the first parameter's handler. */

    printf_s( "%%ifnes(" );
    current_df_data.warning = false;
    current_df_data.current = current_df_data.base + my_parameters.first;
    process_parameter();
    printf_s( "," );

    /* Now invoke the second parameter's handler. */

    current_df_data.current = current_df_data.base + my_parameters.second;
    process_parameter();

    puts( ")" );
    if( current_df_data.warning )
        puts( "String(s) shown above may contain unprintable characters." );

    return;
}

/* Type II device functions have no parameter type byte or ShortHeader/Longheader
 * check.
 */

/* Function df_add().
 * Implements device function %add().
 */
 
static void df_add( void )
{
    parameters   my_parameters;

    /* Extract parameter offsets. */

    get_parameters( &my_parameters );

    /* Now invoke the first parameter's handler. */

    printf_s( "%%add(" );
    current_df_data.warning = false;
    current_df_data.current = current_df_data.base + my_parameters.first;
    process_parameter();
    printf_s( "," );

    /* Now invoke the second parameter's handler. */

    current_df_data.current = current_df_data.base + my_parameters.second;
    process_parameter();

    printf_s( ")" );
    if( current_df_data.warning )
        puts( "String(s) shown above may contain unprintable characters." );

    return;
}

/* Function df_decimal().
 * Implements device function %decimal().
 */
 
static void df_decimal( void )
{
    parameters   my_parameters;

    /* Extract parameter offset. */

    get_parameters( &my_parameters );

    /* Now invoke the parameter's handler. */

    printf_s( "%%decimal(" );
    current_df_data.warning = false;
    current_df_data.current = current_df_data.base + my_parameters.first;
    process_parameter();
    printf_s( ")" );
    if( current_df_data.warning )
        puts( "String(s) shown above may contain unprintable characters." );

    return;
}

/* Function df_divide().
 * Implements device function %divide().
 */
 
static void df_divide( void )
{
    parameters   my_parameters;

    /* Extract parameter offsets. */

    get_parameters( &my_parameters );

    /* Now invoke the first parameter's handler. */

    printf_s( "%%divide(" );
    current_df_data.warning = false;
    current_df_data.current = current_df_data.base + my_parameters.first;
    process_parameter();
    printf_s( "," );

    /* Now invoke the second parameter's handler. */

    current_df_data.current = current_df_data.base + my_parameters.second;
    process_parameter();

    printf_s( ")" );
    if( current_df_data.warning )
        puts( "String(s) shown above may contain unprintable characters." );

    return;
}

/* Function df_getstrsymbol().
 * Implements device function %getstrsymbol().
 */
 
static void df_getstrsymbol( void )
{
    parameters   my_parameters;

    /* Extract parameter offset. */

    get_parameters( &my_parameters );

    /* Now invoke the parameter's handler. */

    printf_s( "%%getstrsymbol(" );
    current_df_data.warning = false;
    current_df_data.current = current_df_data.base + my_parameters.first;
    process_parameter();
    printf_s( ")" );
    if( current_df_data.warning )
        puts( "String(s) shown above may contain unprintable characters." );

    return;
}

/* Function df_getnumsymbol().
 * Implements device function %getnumsymbol().
 */
 
static void df_getnumsymbol( void )
{
    parameters   my_parameters;

    /* Extract parameter offset. */

    get_parameters( &my_parameters );

    /* Now invoke the parameter's handler. */

    printf_s( "%%getnumsymbol(" );
    current_df_data.warning = false;
    current_df_data.current = current_df_data.base + my_parameters.first;
    process_parameter();
    printf_s( ")" );
    if( current_df_data.warning )
        puts( "String(s) shown above may contain unprintable characters." );

    return;
}

/* Function df_hex().
 * Implements device function %hex().
 */
 
static void df_hex( void )
{
    parameters   my_parameters;

    /* Extract parameter offset. */

    get_parameters( &my_parameters );

    /* Now invoke the parameter's handler. */

    printf_s( "%%hex(" );
    current_df_data.warning = false;
    current_df_data.current = current_df_data.base + my_parameters.first;
    process_parameter();

    printf_s( ")" );
    if( current_df_data.warning )
        puts( "String(s) shown above may contain unprintable characters." );

    return;
}

/* Function df_lower().
 * Implements device function %lower().
 */
 
static void df_lower( void )
{
    parameters   my_parameters;

    /* Extract parameter offset. */

    get_parameters( &my_parameters );

    /* Now invoke the parameter's handler. */

    printf_s( "%%lower(" );
    current_df_data.warning = false;
    current_df_data.current = current_df_data.base + my_parameters.first;
    process_parameter();

    printf_s( ")" );
    if( current_df_data.warning )
        puts( "String(s) shown above may contain unprintable characters." );

    return;
}

/* Function df_remainder().
 * Implements device function %remainder().
 */
 
static void df_remainder( void )
{
    parameters   my_parameters;

    /* Extract parameter offsets. */

    get_parameters( &my_parameters );

    /* Now invoke the first parameter's handler. */

    printf_s( "%%remainder(" );
    current_df_data.warning = false;
    current_df_data.current = current_df_data.base + my_parameters.first;
    process_parameter();
    printf_s( "," );

    /* Now invoke the second parameter's handler. */

    current_df_data.current = current_df_data.base + my_parameters.second;
    process_parameter();

    printf_s( ")" );
    if( current_df_data.warning )
        puts( "String(s) shown above may contain unprintable characters." );

    return;
}

/* Function df_subtract().
 * Implements device function %subtract().
 */
 
static void df_subtract( void )
{
    parameters   my_parameters;

    /* Extract parameter offsets. */

    get_parameters( &my_parameters );

    /* Now invoke the first parameter's handler. */

    printf_s( "%%subtract(" );
    current_df_data.warning = false;
    current_df_data.current = current_df_data.base + my_parameters.first;
    process_parameter();
    printf_s( "," );

    /* Now invoke the second parameter's handler. */

    current_df_data.current = current_df_data.base + my_parameters.second;
    process_parameter();

    printf_s( ")" );
    if( current_df_data.warning )
        puts( "String(s) shown above may contain unprintable characters." );

    return;
}


/* Now specify the contents of function_table. */

static df_function function_table[0x3D] = {
    &char_literal,      // 0x00 (character parameter in parameter block)
    &df_recordbreak,    // 0x01 %recordbreak()
    &df_bad_code,       // 0x02 (none)
    &df_bad_code,       // 0x03 (none)
    &df_bad_code,       // 0x04 (none)
    &df_bad_code,       // 0x05 (none)
    &df_enterfont,      // 0x06 %enterfont()
    &df_bad_code,       // 0x07 (none)
    &df_bad_code,       // 0x08 (none)
    &df_binary1,        // 0x09 %binary() and %binary1()
    &df_binary2,        // 0x0A %binary2()
    &df_binary4,        // 0x0B %binary4()
    &df_decimal,        // 0x0C %decimal()
    &df_hex,            // 0x0D %hex()
    &df_add,            // 0x0E %add()
    &df_subtract,       // 0x0F %subtract()
    &df_divide,         // 0x10 %divide()
    &df_remainder,      // 0x11 %remainder()
    &df_getnumsymbol,   // 0x12 %getnumsymbol()
    &df_getstrsymbol,   // 0x13 %getstrsymbol()
    &df_lower,          // 0x14 %lower()
    &df_image,          // 0x15 %image()
    &df_text,           // 0x16 %text()
    &df_setsymbol,      // 0x17 %setsymbol()
    &df_ifeqs,          // 0x18 %ifeqs()
    &df_ifnes,          // 0x19 %ifnes()
    &df_ifeqn,          // 0x1A %ifeqn()
    &df_ifnen,          // 0x1B %ifnen()
    &df_endif,          // 0x1C %endif()
    &df_flushpage,      // 0x1D %flushpage()
    &df_clearPC,        // 0x1E %clearPC()
    &df_clear3270,      // 0x1F %clear3270()
    &df_textpass,       // 0x20 %textpass()
    &df_ulineon,        // 0x21 %ulineon()
    &df_ulineoff,       // 0x22 %ulineoff()
    &df_dotab,          // 0x23 %dotab()
    &df_cancel,         // 0x24 %cancel()
    &df_wait,           // 0x25 %wait()
    &df_sleep,          // 0x26 %sleep()
    &df_default_width,  // 0x27 %default_width()
    &df_font_number,    // 0x28 %font_number()
    &df_tab_width,      // 0x29 %tab_width()
    &df_page_depth,     // 0x2A %page_depth()
    &df_page_width,     // 0x2B %page_width()
    &df_x_address,      // 0x2C %x_address()
    &df_y_address,      // 0x2D %y_address()
    &df_x_size,         // 0x2E %x_size()
    &df_y_size,         // 0x2F %y_size()
    &df_thickness,      // 0x30 %thickness()
    &df_font_height,    // 0x31 %font_height()
    &df_font_space,     // 0x32 %font_space()
    &df_line_height,    // 0x33 %line_height()
    &df_line_space,     // 0x34 %line_space()
    &df_pages,          // 0x35 %pages()
    &df_wgml_header,    // 0x36 %wgml_header()
    &df_font_outname1,  // 0x37 %font_outname1()
    &df_font_outname2,  // 0x38 %font_outname2()
    &df_font_resident,  // 0x39 %font_resident()
    &df_time,           // 0x3A %time()
    &df_date,           // 0x3B %date()
    &numeric_literal,   // 0x3C (numeric parameter in parameter block)
};

/*  Global function definition. */

/* Function interpret_function().
 * Display the device functions and literal parameters which, when compiled
 * by gendev, will produce the contents of in_function. This function directly
 * handles the linked list of Type I device functions only; the individual
 * functions in function_table[] handle parameters, including parameter blocks
 * and Type II device functions, as appropriate.
 *
 * Parameter:
 *      in_function points to a compiled function block.
 *
 * Global Variables Used:
 *      current_df_data is updated to reflect the current function.
 *      function_table[] is used to invoke the handler for each device function.
 *
 * Global Variable Modified:
 *      current_df_data.current will point to the first byte after
 *      the byte code for the function being processed when the handler in
 *      function_table[] is invoked.
 *
 * Note:
 *      current_function is used to reset current_df_data.current
 *      on each pass through the loop. It is not necessary that the functions
 *      invoked through function_table[] leave current_df_data.current
 *      in any particular state.
*/

void interpret_function( char *in_function )
{
    bool        last_function_done;
    char        *current_function    = NULL;
    uint16_t    current_offset;

    /* Initialize the loop parameters. */

    last_function_done = false;
    current_function = in_function;

    while( !last_function_done )
    {

        /* Start at the proper location. For current_df_data.base, this is
         * the byte before where the parameter block starts, if one is present.
         */

        current_df_data.base = current_function;
        current_df_data.base += 3;
        current_df_data.current = current_function;

        /* Get the offset to the next element in the linked list. */

        memcpy_s( &current_offset, sizeof( current_offset ),
            current_df_data.current, sizeof( current_offset ) );
        current_df_data.current += sizeof( current_offset );

        /* Get the parameter type for the current device function */

        memcpy_s( &current_df_data.parameter_type, sizeof( current_df_data.parameter_type ), current_df_data.current, sizeof( current_df_data.parameter_type ) );
        current_df_data.current += sizeof( current_df_data.parameter_type );

        /* Either reset current_function to the next list element
         * or record that the last function will be done this iteration.
         */

        if( current_offset < 0xFFFF ) {
            current_function = current_df_data.current + current_offset;
        } else {
            last_function_done = true;
        }

        /* Get the function code. */

        memcpy_s( &current_df_data.df_code, sizeof( current_df_data.df_code ), current_df_data.current, sizeof( current_df_data.df_code ) );
        current_df_data.current += sizeof( current_df_data.df_code );

        /* This is where the df_code processing occurs. */

        function_table[current_df_data.df_code]();

    }
    return;
}

