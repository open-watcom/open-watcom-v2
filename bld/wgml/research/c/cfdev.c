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
* Description:  Implements the functions declared in cfdev.h:
*                   is_dev_file()
*                   parse_device()
*               and contains these local functions:
*                   find_cumulative_index()
*                   resize_cop_device()
*                   set_cumulative_index()
*
* Note:         The Wiki should be consulted for any term whose meaning is
*               not apparent. This should help in most cases.
*
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1
#include <stdlib.h>
#include <string.h>

#include "cfdev.h"
#include "cffunc.h"
#include "common.h"

/*  Local macros */

#define START_SIZE 2048
#define INC_SIZE   1024

#define OUT_DEV_MAP(x)      (void *)((char *)out_device + (size_t)x)
#define OUT_DEV_MAP_OFF()   (void *)((char *)out_device + (size_t)out_device->next_offset)
#define OUT_DEV_GET_OFF()   (void *)(out_device->next_offset)
#define OUT_DEV_ADD_OFF(x)  out_device->next_offset += x
#define OUT_DEV_REMAP(x)    out_device->x = (void *)((char *)out_device + (size_t)out_device->x)

/*  Local functions definitions. */

/*  Function find_cumulative_index().
 *  Finds the index of the CodeBlock matching the in_max value.
 *
 *  Parameter:
 *      in_block contains the functions_block to search.
 *      in_max contains the value sought.
 *      out_index will contain the corresponding index.
 *
 *  Returns:
 *      SUCCESS if in_max is found in in_block.
 *      FAILURE otherwise.
 */

static int find_cumulative_index( functions_block * in_block, uint16_t in_max, uint8_t * out_index )
{
    uint8_t i;

    for( i = 0; i < in_block->count; i++ ) {
        if( in_block->code_blocks[i].cumulative_index == in_max ) {
            *out_index = i;
            return(SUCCESS);
        }
    }

    return(FAILURE);
}

/* Function resize_cop_device().
 * Resizes a cop_device instance.
 *
 * Parameters:
 *      in_device is a pointer to the cop_device to be resized.
 *      in_size is the minimum acceptable increase in size.
 *
 * Returns:
 *      A pointer to a cop_device instance at least in_size larger with the same
 *          data (except for the allocated_size field, which reflects the new size)
 *          on success.
 *      A NULL pointer on failure.
 *
 * Notes:
 *      realloc() will free in_device if the instance is actually moved to a
 *          new location.
 *      if realloc() returns NULL, then in_device will be freed.
 *      The intended use is for the pointer passed as in_device to be used to
 *          store the return value.
 */

static cop_device * resize_cop_device( cop_device * in_device, size_t in_size )
{
    cop_device *    local_device = NULL;
    size_t          increment = INC_SIZE;
    size_t          new_size;
    size_t          scale;

    /* Compute how much larger to make the cop_device struct. */

    if( in_size > INC_SIZE ) {
        scale = in_size / INC_SIZE;
        ++scale;
        increment = scale * INC_SIZE;
    }
    new_size = in_device->allocated_size + increment;

    /* Reallocate the cop_device. */

    local_device = realloc( in_device, new_size );
    if( local_device == NULL ) {
        free( in_device );
    } else {
        local_device->allocated_size = new_size;
    }
    return( local_device );
}

/* Function set_cumulative_index().
 * Sets the cumulative_index field in each code_block of a functions_block.
 *
 * Parameter:
 *      in_block contains a pointer to functions_block to index.
 */

static void set_cumulative_index( functions_block * in_block )
{
    uint8_t         i;

    in_block->code_blocks[0].cumulative_index = 0x0000;

    if( in_block->count > 1 ) {
        for( i = 1; i < in_block->count; i++ ) {
            in_block->code_blocks[i].cumulative_index = 
                in_block->code_blocks[i-1].cumulative_index + in_block->code_blocks[i-1].count;
        }
    }

    return;
}

/* Extern function definitions. */

/* Function is_dev_file().
 * Determines whether or not in_file points to the start of a binary device
 * file (the first byte after the header).
 *
 * Parameter:
 *      in_file points to the presumed start of a binary device file.
 *
 * Returns:
 *      true if this has the correct descriminator.
 *      false otherwise.
 */

bool is_dev_file( FILE * in_file )
{
    char descriminator[3];

    /* Get the descriminator. */

    fread( &descriminator, 3, 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) return( false );

    /* Verify that the descriminator is for a binary device file. */

    if( memcmp( descriminator, "DEV", 3 ) ) return( false );
    
    return( true );
}

/* Function parse_device().
 * Constructs a cop_device instance from the given input stream.
 *  
 * Parameters:
 *      in_file points to the first byte of a binary device file encoding a 
 *          :DEVICE struct after the "DEV" descriminator.
 *
 * Returns:
 *      A pointer to a cop_device struct containing the data from in_file
 *          on success.
 *      A NULL pointer on failure.
 */

cop_device * parse_device( FILE * in_file )
{
    /* The cop_device instance. */
    
    cop_device *        out_device          = NULL;

    /* Used to acquire string attributes. */

    uint8_t             length;
    char                *string_ptr         = NULL;

    /* Used to acquire numeric attributes. */

    uint16_t            designator;
    uint16_t            numeric_16;

    /* Used to acquire the TranslationBlock. */

    uint8_t *           byte_ptr            = NULL;
    uint8_t             data_count;
    uint16_t            i;
    uint8_t             intrans_flag;
    uint8_t *           outtrans_data       = NULL;
    uint8_t             outtrans_data_size;
    uint8_t             outtrans_flag;
    outtrans_block *    outtrans_ptr        = NULL;
    translation *       translation_ptr     = NULL;
    uint8_t *           translation_start   = NULL;
    uint8_t             uint8_array[0x100];    
    uint16_t            uint16_array[0x100];    

    /* Used to acquire the DefaultFonts and DeviceFonts. */

    default_font *      defaultfont_ptr     = NULL;
    device_font  *      devicefont_ptr      = NULL;

    /* Used to acquire the PauseBlock and DevicefontBlock CodeBlocks. */

    code_text *         pause_ptr           = NULL;
    functions_block *   cop_functions       = NULL;
    p_buffer *          raw_functions       = NULL;
    char                *current;
    uint8_t             j;
    uint16_t            cumulative_index;
    uint16_t            nulls;
    int                 return_value;
    size_t              size;

    /* Used for count and other values. */

    uint8_t             count8;

    /* Used for the next_codeblock value. */

    uint16_t            next_codeblock;

    /* Initialize the out_device. */
        
    out_device = malloc( START_SIZE );
    if( out_device == NULL )
        return( out_device );

    out_device->allocated_size = START_SIZE;
    out_device->next_offset = sizeof( cop_device );

    /* Note: The various pointers must be entered, initially, as offsets
     * and then converted to pointers before returning because out_device
     * may be reallocated at any point and that invalidates actual pointers.
     */

    /* Get the driver_name. */

    fread( &length, sizeof( length ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( out_device );
        out_device = NULL;
        return( out_device );
    }

    if( length > 0 ) {

        if( out_device->allocated_size < (out_device->next_offset + length) ) {
            out_device = resize_cop_device( out_device, length );
            if( out_device == NULL ) {
                return( out_device );
            }
        }

        string_ptr = OUT_DEV_MAP_OFF();

        fread( string_ptr, length, 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_device );
            out_device = NULL;
            return( out_device );
        }
        out_device->driver_name = OUT_DEV_GET_OFF();
        string_ptr[length] = '\0';
        OUT_DEV_ADD_OFF( length + 1 );
    } else {
        out_device->driver_name = NULL;
    }

    /* Get the output_name. */

    fread( &length, sizeof( length ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( out_device );
        out_device = NULL;
        return( out_device );
    }

    if( length > 0 ) {
        if( out_device->allocated_size < (out_device->next_offset + length) ) {
            out_device = resize_cop_device( out_device, length );
            if( out_device == NULL ) {
                return( out_device );
            }
        }

        string_ptr = OUT_DEV_MAP_OFF();

        fread( string_ptr, length, 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_device );
            out_device = NULL;
            return( out_device );
        }
    
        out_device->output_name = OUT_DEV_GET_OFF();
        string_ptr[length] = '\0';
        OUT_DEV_ADD_OFF( length + 1 );
    } else {
        out_device->output_name = NULL;
    }

    /* Get the output_extension. */

    fread( &length, sizeof( length ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( out_device );
        out_device = NULL;
        return( out_device );
    }

    if( length > 0 ) {
        if( out_device->allocated_size < (out_device->next_offset + length) ) {
            out_device = resize_cop_device( out_device, length );
            if( out_device == NULL ) {
                return( out_device );
            }
        }

        string_ptr = OUT_DEV_MAP_OFF();

        fread( string_ptr, length, 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_device );
            out_device = NULL;
            return( out_device );
        }
    
        out_device->output_extension = OUT_DEV_GET_OFF();
        string_ptr[length] = '\0';
        OUT_DEV_ADD_OFF( length + 1 );
    } else {
        out_device->output_extension = NULL;
    }
    
    /* Get the numeric attributes, next_codeblock, and the page geometry
     * attributes. 
     */

    /* The designator shows if the numeric attributes and the page geometry
     * attributes are uint16_t or uint32_t; the next_codeblock is always 16-bit.
     */

    fread( &designator, sizeof( designator ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( out_device );
        out_device = NULL;
        return( out_device );
    }
    
    switch( designator ) {
    case 0x1200:

        /* Get the 16-bit page_width. */

        fread( &numeric_16, sizeof( numeric_16 ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        out_device->page_width = numeric_16;

        /* Get the 16-bit page_depth. */

        fread( &numeric_16, sizeof( numeric_16 ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        out_device->page_depth = numeric_16;

        /* Get the 16-bit horizontal_base_units. */

        fread( &numeric_16, sizeof( numeric_16 ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        out_device->horizontal_base_units = numeric_16;

        /* Get the 16-bit vertical_base_units. */

        fread( &numeric_16, sizeof( numeric_16 ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        out_device->vertical_base_units = numeric_16;

        /* Get the next_codeblock value. */

        fread( &next_codeblock, sizeof( next_codeblock ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Get the page geometry values. */
        
        /* Get the 16-bit x_start. */

        fread( &numeric_16, sizeof( numeric_16 ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        out_device->x_start = numeric_16;

        /* Get the 16-bit y_start. */

        fread( &numeric_16, sizeof( numeric_16 ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        out_device->y_start = numeric_16;

        /* Get the 16-bit x_offset. */

        fread( &numeric_16, sizeof( numeric_16 ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        out_device->x_offset = numeric_16;

        /* Get the 16-bit y_offset. */

        fread( &numeric_16, sizeof( numeric_16 ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        out_device->y_offset = numeric_16;

        break;
    case 0x2200:

        /* Get the 32-bit page_width. */

        fread( &out_device->page_width, sizeof( out_device->page_width ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Get the 32-bit page_depth. */

        fread( &out_device->page_depth, sizeof( out_device->page_depth ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Get the 32-bit horizontal_base_units. */

        fread( &out_device->horizontal_base_units, sizeof( out_device->horizontal_base_units ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Get the 32-bit vertical_base_units. */

        fread( &out_device->vertical_base_units, sizeof( out_device->vertical_base_units ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Get the next_codeblock. */

        fread( &next_codeblock, sizeof( next_codeblock ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_device );
            out_device = NULL;
            return( out_device );
        }
        
        /* Get the page geometry values. */
        
        /* Get the 32-bit x_start. */

        fread( &out_device->x_start, sizeof( out_device->x_start ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Get the 32-bit y_start. */

        fread( &out_device->y_start, sizeof( out_device->y_start ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Get the 32-bit x_offset. */

        fread( &out_device->x_offset, sizeof( out_device->x_offset ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Get the 32-bit y_offset. */

        fread( &out_device->y_offset, sizeof( out_device->y_offset ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        break;
    default:
        printf_s( "Bad numeric parameters designator: %i\n", designator );
        free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Get the BoxBlock values. */

    /* Get the font attribute, which can be a string or a number. */

    /* Get the font type designator. */

    fread( &designator, sizeof( designator ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( out_device );
        out_device = NULL;
        return( out_device );
    }
    
    switch( designator ) {
    case 0x0101:

        /* The font attribute is numeric: get the font_number. */

        fread( &count8, sizeof( count8 ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        fread( &out_device->box.font, sizeof( out_device->box.font ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Ensure that the font_name is NULL. */

        out_device->box.font_name = NULL;
        break;
    case 0x0201:

        /* The font attribute is a character string: get the font_name. */

        fread( &length, sizeof( length ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        if( length > 0 ) {
            if( out_device->allocated_size < (out_device->next_offset + length) ) {
                out_device = resize_cop_device( out_device, length );
                if( out_device == NULL ) return( out_device );
            }

            string_ptr = OUT_DEV_MAP_OFF();

            fread( string_ptr, length, 1, in_file );
            if( ferror( in_file ) || feof( in_file ) ) {
                free( out_device );
                out_device = NULL;
                return( out_device );
            }
    
            out_device->box.font_name = OUT_DEV_GET_OFF();
            string_ptr[length] = '\0';
            OUT_DEV_ADD_OFF( length + 1 );
        } else {

            /* It is an error if the string is empty. */

            puts( "BoxBlock font was empty string." );
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Ensure that the font_number is 0. */

        out_device->box.font = 0;
        break;
    default:
        printf_s( "Bad BoxBlock font type designator: %i\n", designator );
        free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Get the count field and verify that it contains 0x0F. */

    fread( &count8, sizeof( count8 ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( out_device );
        out_device = NULL;
        return( out_device );
    }

    if( count8 != 0x0F ) {
        printf_s( "Bad BoxBlock count field: %i\n", count8 );
        free( out_device );
        out_device = NULL;
        return( out_device );
    }
    
    /* There are 0x0F bytes in the file but only 11 values. */

    fread( &out_device->box.horizontal_line, sizeof( out_device->box.horizontal_line ), 11, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Skip the 4 unused bytes. */

    fseek( in_file, 4, SEEK_CUR );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( out_device );
        out_device = NULL;
        return( out_device );
    }
    
    /* Get the UnderscoreBlock values. */

    /* Get the font attribute, which can be a string or a number. */

    /* Get the font type designator. */

    fread( &designator, sizeof( designator ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( out_device );
        out_device = NULL;
        return( out_device );
    }
    
    switch( designator ) {
    case 0x0101:

        /* The font attribute is numeric: get the font_number. */

        fread( &count8, sizeof( count8 ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        fread( &out_device->underscore.font, sizeof( out_device->underscore.font ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Ensure that the font_name is NULL. */

        out_device->underscore.font_name = NULL;
        break;
    case 0x0201:

        /* The font attribute is a character string: get the font_name. */

        fread( &length, sizeof( length ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        if( length > 0 ) {
            if( out_device->allocated_size < (out_device->next_offset + length) ) {
                out_device = resize_cop_device( out_device, length );
                if( out_device == NULL ) return( out_device );
            }

            string_ptr = OUT_DEV_MAP_OFF();

            fread( string_ptr, length, 1, in_file );
            if( ferror( in_file ) || feof( in_file ) ) {
                free( out_device );
                out_device = NULL;
                return( out_device );
            }

            out_device->underscore.font_name = OUT_DEV_GET_OFF();
            string_ptr[length] = '\0';
            OUT_DEV_ADD_OFF( length + 1 );

            /* Ensure that the font_name is used. */

            out_device->underscore.specified_font = true;
        } else {

            /* Ensure that the current font is used. */

            out_device->underscore.specified_font = false;
            out_device->underscore.font_name = NULL;
        }

        /* Ensure that the font_number is 0. */

        out_device->underscore.font = 0;
        break;
    default:
        printf_s( "Bad UnderscoreBlock font type designator: %i\n", designator );
        free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Get the count byte and verify that it contains 0x05. */

    fread( &count8, sizeof( count8 ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( out_device );
        out_device = NULL;
        return( out_device );
    }

    if( count8 != 0x05 ) {
        printf_s( "Bad UnderscoreBlock count field: %i\n", count8 );
        free( out_device );
        out_device = NULL;
        return( out_device );
    }
    
    /* There are 0x05 bytes in the file but only one value. */

    fread( &out_device->underscore.underscore_char, sizeof( out_device->underscore.underscore_char ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Skip the 4 unused bytes. */

    fseek( in_file, 4, SEEK_CUR );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Get the TranslationBlock. */

    /* Get the count and verify that it contains 0x03. */

    fread( &count8, sizeof( count8 ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( out_device );
        out_device = NULL;
        return( out_device );
    }

    if( count8 != 0x03 ) {
        printf_s( "Bad count field for TranslationBlock: %i\n", count8 );
        free( out_device );
        out_device = NULL;
        return( out_device );
    }
    
    /* Now get the data_count and flags. */

    fread( &data_count, sizeof( data_count ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( out_device );
        out_device = NULL;
        return( out_device );
    }

    fread( &outtrans_flag, sizeof( outtrans_flag ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( out_device );
        out_device = NULL;
        return( out_device );
    }

    fread( &intrans_flag, sizeof( intrans_flag ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Get the IntransBlock, if present. */

    if( intrans_flag == 0 ) {
        out_device->intrans = NULL;
    } else {

        /* Verify that the next byte is 0x81. */

        fread( &count8, sizeof( count8 ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        if( count8 != 0x81 ) {
            printf_s( "Incorrect IntransBlock designator: %i\n", count8 );
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Get the count byte and verify that it contains 0x00. */
        
        fread( &count8, sizeof( count8 ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        if( count8 != 0x00 ) {
            printf_s( "Incorrect IntransBlock count: %i\n", count8 );
            free( out_device );
            out_device = NULL;
            return( out_device );
        }
    
        /* Get the data into the array. */

        if( out_device->allocated_size < (out_device->next_offset + sizeof( out_device->intrans->table )) ) {
            out_device = resize_cop_device( out_device, sizeof( out_device->intrans->table ) );
            if( out_device == NULL ) return( out_device );
        }

        byte_ptr = OUT_DEV_MAP_OFF();
        
        fread( byte_ptr, sizeof( out_device->intrans->table ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        out_device->intrans = OUT_DEV_GET_OFF();
        OUT_DEV_ADD_OFF( sizeof( out_device->intrans->table ) );

    }  

    /* Get the OuttransBlock, if present. */

    if( outtrans_flag == 0 ) {
        out_device->outtrans = NULL;
    } else {

        /* Get the next byte, which indicates the OuttransBlock data size. */

        fread( &outtrans_data_size, sizeof( outtrans_data_size ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Read the count byte. */

        fread( &count8, sizeof( count8 ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* The file is positioned at the start of the data. */

        switch( outtrans_data_size) {
        case 0x81:

            /* The count byte should be 0x00. */

            if( count8 != 0x00 ) {
                printf_s( "Incorrect OuttransBlock count: %i\n", count8 );
                free( out_device );
                out_device = NULL;
                return( out_device );
            }

            /* Get the data into the local buffer. */

            fread( &uint8_array, sizeof( uint8_array ), 1, in_file );
            if( ferror( in_file ) || feof( in_file ) ) {
                free( out_device );
                out_device = NULL;
                return( out_device );
            }
            
            /* Reserve space for the outtrans_block. */

            if( out_device->allocated_size < (out_device->next_offset + sizeof( out_device->outtrans->table )) ) {
                out_device = resize_cop_device( out_device, sizeof( out_device->outtrans->table ) );
                if( out_device == NULL ) return( out_device );
            }

            out_device->outtrans = OUT_DEV_GET_OFF();
            OUT_DEV_ADD_OFF( sizeof( out_device->outtrans->table ) );

            outtrans_ptr = OUT_DEV_MAP( out_device->outtrans );

            /* Build the actual table, which requires actual pointers in
             * place of the offsets recorded in *out_device:
             *   outtrans_ptr is the pointer version of out_device->outtrans.
             *   for each iteration,
             *     translation_ptr is the pointer version of
             *                                             outtrans->ptr->table[i].
             *     byte_ptr is the pointer version of translation->ptr.data.
             */

            for( i = 0; i < 0x100; i++ ) {

                /* If the first byte matches the index, there is no translation. */

                if( uint8_array[i] == i ) {
                    outtrans_ptr->table[i] = NULL;
                } else {

                    /* Reserve space for the translation. */

                    if( out_device->allocated_size < ( out_device->next_offset + sizeof( translation )) ) {
                        out_device = resize_cop_device( out_device, sizeof( translation ) );
                        if( out_device == NULL )
                            return( out_device );
                        outtrans_ptr = OUT_DEV_MAP( out_device->outtrans );
                    }

                    outtrans_ptr->table[i] = OUT_DEV_GET_OFF();
                    OUT_DEV_ADD_OFF( sizeof( translation ) );

                    /* Get the translation for the current character. */

                    translation_ptr = OUT_DEV_MAP( outtrans_ptr->table[i] );

                    /* The translation always contains exactly one character. */
                    
                    size = translation_ptr->count = 1;

                    if( out_device->allocated_size < (out_device->next_offset + size ) ) {
                        out_device = resize_cop_device( out_device, size );
                        if( out_device == NULL )
                            return( out_device );
                        outtrans_ptr = OUT_DEV_MAP( out_device->outtrans );
                        translation_ptr = OUT_DEV_MAP( outtrans_ptr->table[i] );
                    }

                    translation_ptr->data = OUT_DEV_GET_OFF();
                    OUT_DEV_ADD_OFF( size );

                    byte_ptr = OUT_DEV_MAP( translation_ptr->data );

                    /* The translation character is the value in the input array.
                     */
                    
                    *byte_ptr = uint8_array[i];
                }
            }
            break;
        case 0x82:

            /* The count byte should be equal to data_count. */
        
            if( count8 != data_count ) {
                printf_s( "Incorrect OuttransBlock data_count: %i instead of %i\n", data_count, count8 );
                free( out_device );
                out_device = NULL;
                return( out_device );
            }

            /* Get the outtrans array into the local array. */

            fread( &uint16_array, sizeof( uint16_array ), 1, in_file );
            if( ferror( in_file ) || feof( in_file ) ) {
                free( out_device );
                out_device = NULL;
                return( out_device );
            }

            /* Allocate a buffer and read the translation characters into it. */

            outtrans_data = malloc( data_count );

            fread( outtrans_data, sizeof( *outtrans_data ), data_count, in_file );
            if( ferror( in_file ) || feof( in_file ) ) {
                free( outtrans_data );
                outtrans_data = NULL;
                free( out_device );
                out_device = NULL;
                return( out_device );
            }

            /* Initialize outtrans_ptr and the outtrans pointer in out_device. */

            if( out_device->allocated_size < (out_device->next_offset + sizeof( out_device->outtrans->table )) ) {
                out_device = resize_cop_device( out_device, sizeof( out_device->outtrans->table ) );
                if( out_device == NULL ) {
                    free( outtrans_data );
                    outtrans_data = NULL;
                    return( out_device );
                }
            }

            out_device->outtrans = OUT_DEV_GET_OFF();
            OUT_DEV_ADD_OFF( sizeof( out_device->outtrans->table ) );

            outtrans_ptr = OUT_DEV_MAP( out_device->outtrans );

            /* Convert the data in uint16_array to our format, which requires
             * actual pointers in place of the offsets recorded in *out_device:
             *   outtrans_ptr is the pointer version of out_device->outtrans.
             *   for each iteration,
             *     translation_ptr is the pointer version of
             *                                             outtrans->ptr->table[i].
             *     byte_ptr is the pointer version of translation->ptr.data.
             */

            for( i = 0; i < 0x100; i++ ) {

                /* If the first byte matches the index, there is no translation. */

                if( uint16_array[i] == i ) {
                    outtrans_ptr->table[i] = NULL;
                } else {

                    /* Reserve space for the translation. */

                    if( out_device->allocated_size < (out_device->next_offset + sizeof( translation )) ) {
                        out_device = resize_cop_device( out_device, sizeof( translation ) );
                        if( out_device == NULL ) {
                            free( outtrans_data );
                            outtrans_data = NULL;
                            return( out_device );
                        }
                        outtrans_ptr = OUT_DEV_MAP( out_device->outtrans );
                    }

                    outtrans_ptr->table[i] = OUT_DEV_GET_OFF();
                    OUT_DEV_ADD_OFF( sizeof( translation ) );

                    /* Get the translation for the current character. */

                    translation_ptr = OUT_DEV_MAP( outtrans_ptr->table[i] );

                    /* If the first byte at uint16_array[i] is 00, then the
                     * second byte is the one-byte translation. Otherwise, the
                     * second byte is an offset. For little-endian computers,
                     * at least!
                     */

                    /* Set the count correctly. */

                    if( (uint16_array[i] & 0xff00) == 0x00 ) {
                        size = 1;
                    } else {
                        translation_start = outtrans_data + (uint16_array[i] & 0x00ff);
                        size = *translation_start;
                    }
                    translation_ptr->count = size;

                    /* Allocate space and perform other common operations. */

                    if( out_device->allocated_size < (out_device->next_offset + size) ) {
                        out_device = resize_cop_device( out_device, size );
                        if( out_device == NULL ) {
                            free( outtrans_data );
                            outtrans_data = NULL;
                            return( out_device );
                        }
                        outtrans_ptr = OUT_DEV_MAP( out_device->outtrans );
                        translation_ptr = OUT_DEV_MAP( outtrans_ptr->table[i] );
                    }

                    translation_ptr->data = OUT_DEV_GET_OFF();
                    OUT_DEV_ADD_OFF( size );

                    byte_ptr = OUT_DEV_MAP( translation_ptr->data );

                    /* Put the data into the buffer. */

                    if( (uint16_array[i] & 0xff00) == 0x00 ) {
                        *byte_ptr = (uint16_array[i] & 0x00ff);
                    } else {
                        ++translation_start;
                        memcpy_s( byte_ptr, size, translation_start, size );
                    }
                }
            }
            free( outtrans_data );
            outtrans_data = NULL;
            break;
        default:
            printf_s( "Incorrect OuttransBlock designator: %i\n", count8 );
            free( out_device );
            out_device = NULL;
            return( out_device );
        }
    }  

    /* Get the DefaultfontBlock. */

    /* Get the count and verify that it is 0x02. */

    fread( &count8, sizeof( count8 ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( out_device );
        out_device = NULL;
        return( out_device );
    }

    if( count8 != 0x02 ) {
        printf_s( "Incorrect DefaultfontBlock designator: %i\n", count8 );
        free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Get the number of DefaultFonts. */
    
    fread( &out_device->defaultfonts.font_count, sizeof( out_device->defaultfonts.font_count ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Set defaultfont_ptr and defaultfonts.font and adjust next_offset. */

    size = sizeof( *out_device->defaultfonts.fonts ) * out_device->defaultfonts.font_count;
    if( out_device->allocated_size < (out_device->next_offset + size) ) {
        out_device = resize_cop_device( out_device, size );
        if( out_device == NULL ) {
            return( out_device );
        }
   }

    out_device->defaultfonts.fonts = OUT_DEV_GET_OFF();
    OUT_DEV_ADD_OFF( size );

    defaultfont_ptr = OUT_DEV_MAP( out_device->defaultfonts.fonts );

    /* Get the DefaultFonts. */

    for( i = 0; i < out_device->defaultfonts.font_count; i++ ) {

        /* Get the Font Style. */

        fread( &length, sizeof( length ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        if( out_device->allocated_size < (out_device->next_offset + length) ) {
            out_device = resize_cop_device( out_device, length );
            if( out_device == NULL )
                return( out_device );
            defaultfont_ptr = OUT_DEV_MAP( out_device->defaultfonts.fonts );
        }

        if( length > 0 ) {
            string_ptr = OUT_DEV_MAP_OFF();
            fread( string_ptr, length, 1, in_file );
            if( ferror( in_file ) || feof( in_file ) ) {
                free( out_device );
                out_device = NULL;
                return( out_device );
            }

            defaultfont_ptr[i].font_style = OUT_DEV_GET_OFF();
            string_ptr[length] = '\0';
            OUT_DEV_ADD_OFF( length + 1 );
        } else {
            defaultfont_ptr[i].font_style = NULL;
        }
        
        /* Get the count and verify that it is 0x04. */

        fread( &count8, sizeof( count8 ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        if( count8 != 0x04 ) {
            printf_s( "Incorrect DefaultFont count: %i\n", count8 );
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Get the Font Height. */

        fread( &defaultfont_ptr[i].font_height, sizeof( defaultfont_ptr[i].font_height ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Get the Font Space. */

        fread( &defaultfont_ptr[i].font_space, sizeof( defaultfont_ptr[i].font_space ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Get the Font Name. */

        fread( &length, sizeof( length ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        if( out_device->allocated_size < (out_device->next_offset + length) ) {
            out_device = resize_cop_device( out_device, length );
            if( out_device == NULL )
                return( out_device );
            defaultfont_ptr = OUT_DEV_MAP( out_device->defaultfonts.fonts );
        }

        if( length > 0 ) {
            string_ptr = OUT_DEV_MAP_OFF();
            fread( string_ptr, length, 1, in_file );
            if( ferror( in_file ) || feof( in_file ) ) {
                free( out_device );
                out_device = NULL;
                return( out_device );
            }

            defaultfont_ptr[i].font_name = OUT_DEV_GET_OFF();
            string_ptr[length] = '\0';
            OUT_DEV_ADD_OFF( length + 1 );
        } else {
            defaultfont_ptr[i].font_style = NULL;
        }
    }

    /* Now get the FunctionsBlock and position in_file to the start of 
     * the PauseBlock. This must be done even if no functions are present.
     */

    raw_functions = get_p_buffer( in_file );
    if( raw_functions == NULL) {
        free( out_device );
        out_device = NULL;
        return( out_device );
    }

    if( raw_functions->buffer == NULL) {
        free( raw_functions );
        raw_functions = NULL;
        free( out_device );
        out_device = NULL;
        return( out_device );
    }

    current = raw_functions->buffer;
    cop_functions = parse_functions_block( &current, raw_functions->buffer, "DEVICE" );

    if( cop_functions == NULL) {
        free( raw_functions );
        raw_functions = NULL;
        free( out_device );
        out_device = NULL;
        return( out_device );
    }
    
    if( cop_functions->count > 0)
        set_cumulative_index( cop_functions );

    /* Get the PauseBlock. */

    /* Get the start_pause. */

    /* Get the count and verify that it is 0x02. */

    fread( &count8, sizeof( count8 ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( raw_functions );
        raw_functions = NULL;
        if( cop_functions->code_blocks != NULL ) {
            free( cop_functions->code_blocks );
            cop_functions->code_blocks = NULL;
        }
        free( cop_functions );
        cop_functions = NULL;
        free( out_device );
        out_device = NULL;
        return( out_device );
    }

    if( count8 != 0x02 ) {
        printf_s( "Incorrect START Pause count: %i\n", count8 );
        free( raw_functions );
        raw_functions = NULL;
        if( cop_functions->code_blocks != NULL ) {
            free( cop_functions->code_blocks );
            cop_functions->code_blocks = NULL;
        }
        free( cop_functions );
        cop_functions = NULL;
        free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Get the value to use to find the CodeBlock. */

    fread( &cumulative_index, sizeof( cumulative_index ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( raw_functions );
        raw_functions = NULL;
        if( cop_functions->code_blocks != NULL ) {
            free( cop_functions->code_blocks );
            cop_functions->code_blocks = NULL;
        }
        free( cop_functions );
        cop_functions = NULL;
        free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* The value 0xFFFF indicates that no such pause exists. */

    if( cumulative_index == 0xFFFF ) {
        out_device->pauses.start_pause = NULL;
    } else {
        return_value = find_cumulative_index( cop_functions, cumulative_index, &j );
        if( return_value == FAILURE ) {
            puts( "START :PAUSE CodeBlock not found!" );
            free( raw_functions );
            raw_functions = NULL;
            if( cop_functions->code_blocks != NULL ) {
                free( cop_functions->code_blocks );
                cop_functions->code_blocks = NULL;
            }
            free( cop_functions );
            cop_functions = NULL;
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Allocate space for the code_text instance. */

        if( out_device->allocated_size < (out_device->next_offset + sizeof( code_text ) ) ) {
            out_device = resize_cop_device( out_device, sizeof( code_text ) );
            if( out_device == NULL ) {
                free( raw_functions );
                raw_functions = NULL;
                if( cop_functions->code_blocks != NULL ) {
                    free( cop_functions->code_blocks );
                    cop_functions->code_blocks = NULL;
                }
                free( cop_functions );
                cop_functions = NULL;
                return( out_device );
            }
        }

        out_device->pauses.start_pause = OUT_DEV_GET_OFF();
        OUT_DEV_ADD_OFF( sizeof( code_text ) );

        pause_ptr = OUT_DEV_MAP( out_device->pauses.start_pause );

        /* Initialize the code_text instance. */

        size = pause_ptr->count = cop_functions->code_blocks[j].count;

        if( out_device->allocated_size < (out_device->next_offset + size) ) {
            out_device = resize_cop_device( out_device, size );
            if( out_device == NULL ) {
                free( raw_functions );
                raw_functions = NULL;
                if( cop_functions->code_blocks != NULL ) {
                    free( cop_functions->code_blocks );
                    cop_functions->code_blocks = NULL;
                }
                free( cop_functions );
                cop_functions = NULL;
                return( out_device );
            }
            pause_ptr = OUT_DEV_MAP( out_device->pauses.start_pause );
        }

        pause_ptr->text = OUT_DEV_GET_OFF();
        OUT_DEV_ADD_OFF( size );

        byte_ptr = OUT_DEV_MAP( pause_ptr->text );
        memcpy_s(byte_ptr, size, cop_functions->code_blocks[j].text, size );
    }
    
    /* Get the document_pause. */

    /* Get the count and verify that it is 0x02. */

    fread( &count8, sizeof( count8 ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( raw_functions );
        raw_functions = NULL;
        if( cop_functions->code_blocks != NULL ) {
            free( cop_functions->code_blocks );
            cop_functions->code_blocks = NULL;
        }
        free( cop_functions );
        cop_functions = NULL;
        free( out_device );
        out_device = NULL;
        return( out_device );
    }

    if( count8 != 0x02 ) {
        free( raw_functions );
        raw_functions = NULL;
        if( cop_functions->code_blocks != NULL ) {
            free( cop_functions->code_blocks );
            cop_functions->code_blocks = NULL;
        }
        free( cop_functions );
        cop_functions = NULL;
        free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Get the value to use to find the CodeBlock. */

    fread( &cumulative_index, sizeof( cumulative_index ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( raw_functions );
        raw_functions = NULL;
        if( cop_functions->code_blocks != NULL ) {
            free( cop_functions->code_blocks );
            cop_functions->code_blocks = NULL;
        }
        free( cop_functions );
        cop_functions = NULL;
        free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* The value 0xFFFF indicates that no such pause exists. */

    if( cumulative_index == 0xFFFF ) {
        out_device->pauses.document_pause = NULL;
    } else {

        /* Ensure that the CodeBlock can be found. */

        return_value = find_cumulative_index( cop_functions, cumulative_index, &j );
        if( return_value == FAILURE ) {
            puts( "DOCUMENT :PAUSE CodeBlock not found!" );
            free( raw_functions );
            raw_functions = NULL;
            if( cop_functions->code_blocks != NULL ) {
                free( cop_functions->code_blocks );
                cop_functions->code_blocks = NULL;
            }
            free( cop_functions );
            cop_functions = NULL;
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Allocate space for the code_text instance. */

        if( out_device->allocated_size < (out_device->next_offset + sizeof( code_text ) ) ) {
            out_device = resize_cop_device( out_device, sizeof( code_text ) );
            if( out_device == NULL ) {
                free( raw_functions );
                raw_functions = NULL;
                if( cop_functions->code_blocks != NULL ) {
                    free( cop_functions->code_blocks );
                    cop_functions->code_blocks = NULL;
                }
                free( cop_functions );
                cop_functions = NULL;
                return( out_device );
            }
        }

        out_device->pauses.document_pause = OUT_DEV_GET_OFF();
        OUT_DEV_ADD_OFF( sizeof( code_text ) );

        pause_ptr = OUT_DEV_MAP( out_device->pauses.document_pause );

        /* Initialize the code_text instance. */

        size = pause_ptr->count = cop_functions->code_blocks[j].count;

        if( out_device->allocated_size < (out_device->next_offset + size) ) {
            out_device = resize_cop_device( out_device, size );
            if( out_device == NULL ) {
                free( raw_functions );
                raw_functions = NULL;
                if( cop_functions->code_blocks != NULL ) {
                    free( cop_functions->code_blocks );
                    cop_functions->code_blocks = NULL;
                }
                free( cop_functions );
                cop_functions = NULL;
                return( out_device );
            }
            pause_ptr = OUT_DEV_MAP( out_device->pauses.document_pause );
        }

        pause_ptr->text = OUT_DEV_GET_OFF();
        OUT_DEV_ADD_OFF( size );

        byte_ptr = OUT_DEV_MAP( pause_ptr->text );
        memcpy_s(byte_ptr, size, cop_functions->code_blocks[j].text, size );
    }
    
    /* Get the docpage_pause. */

    /* Get the count and verify that it is 0x02. */

    fread( &count8, sizeof( count8 ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( raw_functions );
        raw_functions = NULL;
        if( cop_functions->code_blocks != NULL ) {
            free( cop_functions->code_blocks );
            cop_functions->code_blocks = NULL;
        }
        free( cop_functions );
        cop_functions = NULL;
        free( out_device );
        out_device = NULL;
        return( out_device );
    }

    if( count8 != 0x02 ) {
        free( raw_functions );
        raw_functions = NULL;
        if( cop_functions->code_blocks != NULL ) {
            free( cop_functions->code_blocks );
            cop_functions->code_blocks = NULL;
        }
        free( cop_functions );
        cop_functions = NULL;
        free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Get the value to use to find the CodeBlock. */

    fread( &cumulative_index, sizeof( cumulative_index ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( raw_functions );
        raw_functions = NULL;
        if( cop_functions->code_blocks != NULL ) {
            free( cop_functions->code_blocks );
            cop_functions->code_blocks = NULL;
        }
        free( cop_functions );
        cop_functions = NULL;
        free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* The value 0xFFFF indicates that no such pause exists. */

    if( cumulative_index == 0xFFFF ) {
        out_device->pauses.docpage_pause = NULL;
    } else {

        /* Ensure that the CodeBlock can be found. */

        return_value = find_cumulative_index( cop_functions, cumulative_index, &j );
        if( return_value == FAILURE ) {
            puts( "DOCUMENT_PAGE :PAUSE CodeBlock not found!" );
            free( raw_functions );
            raw_functions = NULL;
            if( cop_functions->code_blocks != NULL ) {
                free( cop_functions->code_blocks );
                cop_functions->code_blocks = NULL;
            }
            free( cop_functions );
            cop_functions = NULL;
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Allocate space for the code_text instance. */

        if( out_device->allocated_size < (out_device->next_offset + sizeof( code_text ) ) ) {
            out_device = resize_cop_device( out_device, sizeof( code_text ) );
            if( out_device == NULL ) {
                free( raw_functions );
                raw_functions = NULL;
                if( cop_functions->code_blocks != NULL ) {
                    free( cop_functions->code_blocks );
                    cop_functions->code_blocks = NULL;
                }
                free( cop_functions );
                cop_functions = NULL;
                return( out_device );
            }
        }

        out_device->pauses.docpage_pause = OUT_DEV_GET_OFF();
        OUT_DEV_ADD_OFF( sizeof( code_text ) );

        pause_ptr = OUT_DEV_MAP( out_device->pauses.docpage_pause );

        /* Initialize the code_text instance. */

        size = pause_ptr->count = cop_functions->code_blocks[j].count;

        if( out_device->allocated_size < (out_device->next_offset + size) ) {
            out_device = resize_cop_device( out_device, size );
            if( out_device == NULL ) {
                free( raw_functions );
                raw_functions = NULL;
                if( cop_functions->code_blocks != NULL ) {
                    free( cop_functions->code_blocks );
                    cop_functions->code_blocks = NULL;
                }
                free( cop_functions );
                cop_functions = NULL;
                return( out_device );
            }
            pause_ptr = OUT_DEV_MAP( out_device->pauses.docpage_pause );
        }

        pause_ptr->text = OUT_DEV_GET_OFF();
        OUT_DEV_ADD_OFF( size );

        byte_ptr = OUT_DEV_MAP( pause_ptr->text );
        memcpy_s(byte_ptr, size, cop_functions->code_blocks[j].text, size );
    }
    
    /* Get the devpage_pause. */

    /* Get the count and verify that it is 0x02. */

    fread( &count8, sizeof( count8 ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( raw_functions );
        raw_functions = NULL;
        if( cop_functions->code_blocks != NULL ) {
            free( cop_functions->code_blocks );
            cop_functions->code_blocks = NULL;
        }
        free( cop_functions );
        cop_functions = NULL;
        free( out_device );
        out_device = NULL;
        return( out_device );
    }

    if( count8 != 0x02 ) {
        free( raw_functions );
        raw_functions = NULL;
        if( cop_functions->code_blocks != NULL ) {
            free( cop_functions->code_blocks );
            cop_functions->code_blocks = NULL;
        }
        free( cop_functions );
        cop_functions = NULL;
        free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Get the value to use to find the CodeBlock. */

    fread( &cumulative_index, sizeof( cumulative_index ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( raw_functions );
        raw_functions = NULL;
        if( cop_functions->code_blocks != NULL ) {
            free( cop_functions->code_blocks );
            cop_functions->code_blocks = NULL;
        }
        free( cop_functions );
        cop_functions = NULL;
        free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* The value 0xFFFF indicates that no such pause exists. */

    if( cumulative_index == 0xFFFF ) {
        out_device->pauses.devpage_pause = NULL;
    } else {

        /* Ensure that the CodeBlock can be found. */

        return_value = find_cumulative_index( cop_functions, cumulative_index, &j );
        if( return_value == FAILURE ) {
            puts( "DEVICE_PAGE :PAUSE CodeBlock not found!" );
            free( raw_functions );
            raw_functions = NULL;
            if( cop_functions->code_blocks != NULL ) {
                free( cop_functions->code_blocks );
                cop_functions->code_blocks = NULL;
            }
            free( cop_functions );
            cop_functions = NULL;
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Allocate space for the code_text instance. */

        if( out_device->allocated_size < (out_device->next_offset + sizeof( code_text ) ) ) {
            out_device = resize_cop_device( out_device, sizeof( code_text ) );
            if( out_device == NULL ) {
                free( raw_functions );
                raw_functions = NULL;
                if( cop_functions->code_blocks != NULL ) {
                    free( cop_functions->code_blocks );
                    cop_functions->code_blocks = NULL;
                }
                free( cop_functions );
                cop_functions = NULL;
                return( out_device );
            }
        }

        out_device->pauses.devpage_pause = OUT_DEV_GET_OFF();
        OUT_DEV_ADD_OFF( sizeof( code_text ) );

        pause_ptr = OUT_DEV_MAP( out_device->pauses.devpage_pause );

        /* Initialize the code_text instance. */

        size = pause_ptr->count = cop_functions->code_blocks[j].count;

        if( out_device->allocated_size < (out_device->next_offset + size) ) {
            out_device = resize_cop_device( out_device, size );
            if( out_device == NULL ) {
                free( raw_functions );
                raw_functions = NULL;
                if( cop_functions->code_blocks != NULL ) {
                    free( cop_functions->code_blocks );
                    cop_functions->code_blocks = NULL;
                }
                free( cop_functions );
                cop_functions = NULL;
                return( out_device );
            }
            pause_ptr = OUT_DEV_MAP( out_device->pauses.devpage_pause );
        }

        pause_ptr->text = OUT_DEV_GET_OFF();
        OUT_DEV_ADD_OFF( size );

        byte_ptr = OUT_DEV_MAP( pause_ptr->text );
        memcpy_s(byte_ptr, size, cop_functions->code_blocks[j].text, size );
    }

    /* Get the DevicefontBlock. */

    /* Get the count and verify that it is 0x02. */

    fread( &count8, sizeof( count8 ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( raw_functions );
        raw_functions = NULL;
        if( cop_functions->code_blocks != NULL ) {
            free( cop_functions->code_blocks );
            cop_functions->code_blocks = NULL;
        }
        free( cop_functions );
        cop_functions = NULL;
        free( out_device );
        out_device = NULL;
        return( out_device );
    }

    if( count8 != 0x02 ) {
        printf_s( "Incorrect DevicefontBlock count: %i\n", count8 );
        free( raw_functions );
        raw_functions = NULL;
        if( cop_functions->code_blocks != NULL ) {
            free( cop_functions->code_blocks );
            cop_functions->code_blocks = NULL;
        }
        free( cop_functions );
        cop_functions = NULL;
        free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Get the number of Devicefonts. */

    fread( &out_device->devicefonts.font_count, \
           sizeof( out_device->devicefonts.font_count ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( raw_functions );
        raw_functions = NULL;
        if( cop_functions->code_blocks != NULL ) {
            free( cop_functions->code_blocks );
            cop_functions->code_blocks = NULL;
        }
        free( cop_functions );
        cop_functions = NULL;
        free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Get the Devicefonts. */

    size = out_device->devicefonts.font_count * sizeof( *out_device->devicefonts.fonts );
    if( out_device->allocated_size < (out_device->next_offset + size ) ) {
        out_device = resize_cop_device( out_device, size );
        if( out_device == NULL ) {
            free( raw_functions );
            raw_functions = NULL;
            if( cop_functions->code_blocks != NULL ) {
                free( cop_functions->code_blocks );
                cop_functions->code_blocks = NULL;
            }
            free( cop_functions );
            cop_functions = NULL;
            return( out_device );
        }
    }

    out_device->devicefonts.fonts = OUT_DEV_GET_OFF();
    OUT_DEV_ADD_OFF( size );
    
    devicefont_ptr = OUT_DEV_MAP( out_device->devicefonts.fonts );

    for( i = 0; i < out_device->devicefonts.font_count; i++ ) {

        /* Get the font_name. */

        fread( &length, sizeof( length ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( raw_functions );
            raw_functions = NULL;
            if( cop_functions->code_blocks != NULL ) {
                free( cop_functions->code_blocks );
                cop_functions->code_blocks = NULL;
            }
            free( cop_functions );
            cop_functions = NULL;
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        if( length > 0 ) {

            if( out_device->allocated_size < (out_device->next_offset + length) ) {
                out_device = resize_cop_device( out_device, length );
                if( out_device == NULL ) {
                    free( raw_functions );
                    raw_functions = NULL;
                    if( cop_functions->code_blocks != NULL ) {
                        free( cop_functions->code_blocks );
                        cop_functions->code_blocks = NULL;
                    }
                    free( cop_functions );
                    cop_functions = NULL;
                    return( out_device );
                }
                devicefont_ptr = OUT_DEV_MAP( out_device->devicefonts.fonts );
            }

            string_ptr = OUT_DEV_MAP_OFF();

            fread( string_ptr, length, 1, in_file );
            if( ferror( in_file ) || feof( in_file ) ) {
                raw_functions = NULL;
                if( cop_functions->code_blocks != NULL ) {
                    free( cop_functions->code_blocks );
                    cop_functions->code_blocks = NULL;
                }
                free( cop_functions );
                cop_functions = NULL;
                free( out_device );
                out_device = NULL;
                return( out_device );
            }
            devicefont_ptr[i].font_name = OUT_DEV_GET_OFF();
            string_ptr[length] = '\0';
            OUT_DEV_ADD_OFF( length + 1 );
        } else {

            /* An empty font_name is an error. */

            printf_s( "Devicefont %i has an empty font_name.\n", i );
            raw_functions = NULL;
            if( cop_functions->code_blocks != NULL ) {
                free( cop_functions->code_blocks );
                cop_functions->code_blocks = NULL;
            }
            free( cop_functions );
            cop_functions = NULL;
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Get the font_switch. */

        fread( &length, sizeof( length ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( raw_functions );
            raw_functions = NULL;
            if( cop_functions->code_blocks != NULL ) {
                free( cop_functions->code_blocks );
                cop_functions->code_blocks = NULL;
            }
            free( cop_functions );
            cop_functions = NULL;
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        if( length > 0 ) {

            if( out_device->allocated_size < (out_device->next_offset + length) ) {
                out_device = resize_cop_device( out_device, length );
                if( out_device == NULL ) {
                    free( raw_functions );
                    raw_functions = NULL;
                    if( cop_functions->code_blocks != NULL ) {
                        free( cop_functions->code_blocks );
                        cop_functions->code_blocks = NULL;
                    }
                    free( cop_functions );
                    cop_functions = NULL;
                    return( out_device );
                }
                devicefont_ptr = OUT_DEV_MAP( out_device->devicefonts.fonts );
            }

            string_ptr = OUT_DEV_MAP_OFF();

            fread( string_ptr, length, 1, in_file );
            if( ferror( in_file ) || feof( in_file ) ) {
                free( raw_functions );
                raw_functions = NULL;
                if( cop_functions->code_blocks != NULL ) {
                    free( cop_functions->code_blocks );
                    cop_functions->code_blocks = NULL;
                }
                free( cop_functions );
                cop_functions = NULL;
                free( out_device );
                out_device = NULL;
                return( out_device );
            }
            devicefont_ptr[i].font_switch = OUT_DEV_GET_OFF();
            string_ptr[length] = '\0';
            OUT_DEV_ADD_OFF( length + 1 );
        } else {
            devicefont_ptr[i].font_switch = NULL;
        }

        /* Get the nulls and verify that they are, in fact, nulls. */

        fread( &nulls, sizeof( nulls ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( raw_functions );
            raw_functions = NULL;
            if( cop_functions->code_blocks != NULL ) {
                free( cop_functions->code_blocks );
                cop_functions->code_blocks = NULL;
            }
            free( cop_functions );
            cop_functions = NULL;
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        if( nulls != 0x0000 ) {
            printf_s( "Devicefont %i has this for the nulls: %i\n", i, nulls );
            free( raw_functions );
            raw_functions = NULL;
            if( cop_functions->code_blocks != NULL ) {
                free( cop_functions->code_blocks );
                cop_functions->code_blocks = NULL;
            }
            free( cop_functions );
            cop_functions = NULL;
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Get the count and verify that it is 0x03. */

        fread( &count8, sizeof( count8 ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( raw_functions );
            raw_functions = NULL;
            if( cop_functions->code_blocks != NULL ) {
                free( cop_functions->code_blocks );
                cop_functions->code_blocks = NULL;
            }
            free( cop_functions );
            cop_functions = NULL;
            free( out_device );
            out_device = NULL;
            return( out_device );
        }   

        if( count8 != 0x03 ) {
            printf_s( "Incorrect Devicefont count: %i\n", count8 );
            free( raw_functions );
            raw_functions = NULL;
            if( cop_functions->code_blocks != NULL ) {
                free( cop_functions->code_blocks );
                cop_functions->code_blocks = NULL;
            }
            free( cop_functions );
            cop_functions = NULL;
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Get the resident flag. */

        fread( &devicefont_ptr[i].resident, sizeof( devicefont_ptr[i].resident ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( raw_functions );
            raw_functions = NULL;
            if( cop_functions->code_blocks != NULL ) {
                free( cop_functions->code_blocks );
                cop_functions->code_blocks = NULL;
            }
            free( cop_functions );
            cop_functions = NULL;
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Get the fontpause. */

        /* Get the value to use to find the CodeBlock. */
 
        fread( &cumulative_index, sizeof( cumulative_index ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( raw_functions );
            raw_functions = NULL;
            if( cop_functions->code_blocks != NULL ) {
                free( cop_functions->code_blocks );
                cop_functions->code_blocks = NULL;
            }
            free( cop_functions );
            cop_functions = NULL;
            free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* The value 0xFFFF indicates that no such pause exists. */

        if( cumulative_index == 0xFFFF ) {
            devicefont_ptr[i].font_pause = NULL;
        } else {

            /* Ensure that the CodeBlock can be found. */

            return_value = find_cumulative_index( cop_functions, cumulative_index, &j );
            if( return_value == FAILURE ) {
                printf_s( "Devicefont %i :FONTPAUSE CodeBlock not found!\n", i );
                free( raw_functions );
                raw_functions = NULL;
                if( cop_functions->code_blocks != NULL ) {
                    free( cop_functions->code_blocks );
                    cop_functions->code_blocks = NULL;
                }
                free( cop_functions );
                cop_functions = NULL;
                free( out_device );
                out_device = NULL;
                return( out_device );
            }

            /* Allocate space for the code_text instance. */

            if( out_device->allocated_size < (out_device->next_offset + sizeof( code_text ) ) ) {
                out_device = resize_cop_device( out_device, sizeof( code_text ) );
                devicefont_ptr = OUT_DEV_MAP( out_device->devicefonts.fonts );
                pause_ptr = OUT_DEV_MAP( devicefont_ptr[i].font_pause );
            }

            devicefont_ptr[i].font_pause = OUT_DEV_GET_OFF();
            OUT_DEV_ADD_OFF( sizeof( code_text ) );

            pause_ptr = OUT_DEV_MAP( devicefont_ptr[i].font_pause );

            /* Initialize the code_text instance. */

            size = pause_ptr->count = cop_functions->code_blocks[j].count;

            if( out_device->allocated_size < (out_device->next_offset + size) ) {
                out_device = resize_cop_device( out_device, size );
                if( out_device == NULL ) {
                    free( raw_functions );
                    raw_functions = NULL;
                    if( cop_functions->code_blocks != NULL ) {
                        free( cop_functions->code_blocks );
                        cop_functions->code_blocks = NULL;
                    }
                    free( cop_functions );
                    cop_functions = NULL;
                    return( out_device );
                }
                devicefont_ptr = OUT_DEV_MAP( out_device->devicefonts.fonts );
                pause_ptr = OUT_DEV_MAP( devicefont_ptr[i].font_pause );
            }

            pause_ptr->text = OUT_DEV_GET_OFF();
            OUT_DEV_ADD_OFF( size );

            byte_ptr = OUT_DEV_MAP( pause_ptr->text );
            memcpy_s(byte_ptr, size, cop_functions->code_blocks[j].text, size );
        }
    }

    /*  Free the memory used in parsing the CodeBlocks. */

    free( raw_functions );
    raw_functions = NULL;
    if( cop_functions->code_blocks != NULL ) {
        free( cop_functions->code_blocks );
        cop_functions->code_blocks = NULL;
    }
    free( cop_functions );
    cop_functions = NULL;

    /* Convert non-NULL offsets to pointers. */

    if( out_device->driver_name != NULL ) {
        OUT_DEV_REMAP( driver_name );
    }
    
    if( out_device->output_name != NULL ) {
        OUT_DEV_REMAP( output_name );
    }
    
    if( out_device->output_extension != NULL ) {
        OUT_DEV_REMAP( output_extension );
    }
    
    if( out_device->box.font_name  != NULL ) {
        OUT_DEV_REMAP( box.font_name );
    }
    
    if( out_device->underscore.font_name != NULL ) {
        OUT_DEV_REMAP( underscore.font_name );
    }
    
    if( out_device->intrans != NULL ) {
        OUT_DEV_REMAP( intrans );
    }
    
    if( out_device->outtrans != NULL ) {
        OUT_DEV_REMAP( outtrans );
    
        for( i = 0; i < sizeof( outtrans_block ) / sizeof( translation * ); i++ ) {
            if( out_device->outtrans->table[i] != NULL ) {
                OUT_DEV_REMAP( outtrans->table[i] );
                if( out_device->outtrans->table[i]->data != NULL ) {
                    OUT_DEV_REMAP( outtrans->table[i]->data );
                }
            }
        }
    }
    
    if( out_device->defaultfonts.fonts != NULL ) {
       OUT_DEV_REMAP( defaultfonts.fonts );
        for( i = 0; i < out_device->defaultfonts.font_count; i++ ) {
            if( out_device->defaultfonts.fonts[i].font_name != NULL ) {
                OUT_DEV_REMAP( defaultfonts.fonts[i].font_name );
            }
            if( out_device->defaultfonts.fonts[i].font_style != NULL ) {
                OUT_DEV_REMAP( defaultfonts.fonts[i].font_style );
            }
        }
    }
    
    if( out_device->pauses.start_pause != NULL ) {
        OUT_DEV_REMAP( pauses.start_pause );
        if( out_device->pauses.start_pause->text != NULL ) {
            OUT_DEV_REMAP( pauses.start_pause->text );
        }
    }
    
    if( out_device->pauses.document_pause != NULL ) {
        OUT_DEV_REMAP( pauses.document_pause );
        if( out_device->pauses.document_pause->text != NULL ) {
            OUT_DEV_REMAP( pauses.document_pause->text );
        }
    }
    
    if( out_device->pauses.docpage_pause != NULL ) {
        OUT_DEV_REMAP( pauses.docpage_pause );
        if( out_device->pauses.docpage_pause->text != NULL ) {
            OUT_DEV_REMAP( pauses.docpage_pause->text );
        }
    }
    
    if( out_device->pauses.devpage_pause != NULL ) {
        OUT_DEV_REMAP( pauses.devpage_pause );
        if( out_device->pauses.devpage_pause->text != NULL ) {
            OUT_DEV_REMAP( pauses.devpage_pause->text );
        }
    }
    
    if( out_device->devicefonts.fonts != NULL ) {
       OUT_DEV_REMAP( devicefonts.fonts );
        for( i = 0; i < out_device->devicefonts.font_count; i++ ) {
            if( out_device->devicefonts.fonts[i].font_name != NULL ) {
                OUT_DEV_REMAP( devicefonts.fonts[i].font_name );
            }
            if( out_device->devicefonts.fonts[i].font_switch != NULL ) {
                OUT_DEV_REMAP( devicefonts.fonts[i].font_switch );
            }
            if( out_device->devicefonts.fonts[i].font_pause != NULL ) {
                OUT_DEV_REMAP( devicefonts.fonts[i].font_pause );
                if( out_device->devicefonts.fonts[i].font_pause->text != NULL ) {
                    OUT_DEV_REMAP( devicefonts.fonts[i].font_pause->text );
                }
            }
        }
    }
    
    return( out_device );
}

