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


#include "wgml.h"
#include "copdev.h"
#include "copfunc.h"


/* Local macros. */

#define START_SIZE 2048
#define INC_SIZE   1024

/* Local enumeration. */

typedef enum {
    valid,
    not_valid
} index_status;

/* Local function definitions. */

/* Function find_cumulative_index().
 * Finds the index the cumulative_max value.
 *
 * Parameter:
 *     in_block contains the functions_block to search.
 *     in_max contains the value sought.
 *     out_index will contain the corresponding index.
 *
 * Parameter modified:
 *      out_index will be changed if valid is returned.
 *
 * Returns:
 *      valid if in_max is found in in_block.
 *      not_valid otherwise.
 */

static index_status find_cumulative_index( functions_block * in_block, uint16_t in_max, uint8_t * out_index )
{
    uint8_t i;

    for( i = 0; i < in_block->count; i++ ) {
        if( in_block->code_blocks[i].cumulative_index == in_max ) {
            *out_index = i;
            return(valid);
        }
    }

    return(not_valid);
}

/* Function resize_cop_device().
 * Resizes a cop_device instance.
 *
 * Parameters:
 *      in_device is a pointer to the cop_device to be resized.
 *      in_size is the minimum acceptable increase in size.
 *
 * Returns:
 *      A pointer to a cop_device instance at least in_size larger with
 *          the same data (except for the allocated_size field, which
 *          reflects the new size) on success.
 *
 * Notes:
 *      mem_realloc() will call exit() if the reallocation fails.
 *      mem_realloc() will free in_device if the instance is actually moved to a
 *          new location.
 *      The intended use is for the pointer passed as in_device to be used to
 *          store the return value.
 */

static cop_device *resize_cop_device( cop_device *in_device, unsigned in_size )
{
    cop_device      *local_device;
    unsigned        increment;
    unsigned        new_size;

    /* Compute how much larger to make the cop_device struct. */

    increment = INC_SIZE;
    if( in_size > INC_SIZE ) {
        increment = ( ( in_size / INC_SIZE ) + 1 ) * INC_SIZE;
    }
    new_size = in_device->allocated_size + increment;

    /* Reallocate the cop_device. */

    local_device = mem_realloc( in_device, new_size );
    local_device->allocated_size = new_size;
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

    in_block->code_blocks[0].cumulative_index = 0;

    if( in_block->count > 1 ) {
        for( i = 1; i < in_block->count; i++ ) {
            in_block->code_blocks[i].cumulative_index =
                in_block->code_blocks[i-1].cumulative_index + in_block->code_blocks[i-1].count;
        }
    }

    return;
}

/* Extern function definitions. */

/* Function parse_device().
 * Constructs a cop_device instance from the given input stream.
 *
 * Parameters:
 *      fp points to the first byte of a .COP file encoding a :DEVICE
 *          struct after the "DEV" discriminator.
 *
 * Returns:
 *      A pointer to a cop_device struct containing the data from fp
 *          on success.
 *      A NULL pointer on failure.
 *
 *  Notes:
 *      resize_cop_device() uses mem_realloc(), which calls exit() if the
 *          allocation fails.
 *      parse_functions_block() uses mem_alloc, which calls exit() if the
 *          allocation fails.
 *      NULL is returned for file errors and for formatting errors. It is
 *          suggested that a file error be treated as a format error since
 *          a file error indicates premature termination of the file and
 *          the format must be entirely present for there to be no format error.
 */

cop_device * parse_device( FILE *fp )
{
    /* The cop_device instance. */

    cop_device *        out_device          = NULL;

    /* Used to acquire string attributes. */

    int                 length;
    char *              string_ptr          = NULL;

    /* Used to acquire numeric attributes. */

    uint16_t            designator;

    /* Used to acquire the TranslationBlock. */

    uint8_t *           byte_ptr            = NULL;
    int                 data_count;
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

    default_font        *defaultfont_ptr    = NULL;
    device_font         *devicefont_ptr     = NULL;

    /* Used to acquire the PauseBlock and DevicefontBlock CodeBlocks. */

    code_text *         pause_ptr           = NULL;
    functions_block *   cop_functions       = NULL;
    index_status        return_value;
    p_buffer *          raw_functions       = NULL;
    const char          *current;
    uint8_t             j;
    uint16_t            cumulative_index;
    uint16_t            nulls;

    /* Used for count and other values. */

    int                 count;

    unsigned            size;

    char                discriminator[3];

    /* Initialize the out_device. */

    out_device = mem_alloc( START_SIZE );

    out_device->allocated_size = START_SIZE;
    out_device->next_offset = sizeof( cop_device );

    /* Note: The various pointers must be entered, initially, as offsets
     * and then converted to pointers before returning because out_device
     * may be reallocated at any point and that invalidates actual pointers.
     */

    /* Get the discriminator. */

    fread_buff( discriminator, 3, fp );
    if( ferror( fp )
      || feof( fp ) ) {
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Verify that the discriminator is for a .COP device file. */

    if( memcmp( discriminator, "DEV", 3 ) != 0 ) {
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Get the driver_name. */

    length = fread_u8( fp );
    if( ferror( fp )
      || feof( fp ) ) {
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    if( length > 0 ) {
        if( OUT_DEV_EXPAND_CHK( length + 1 ) ) {
            out_device = resize_cop_device( out_device, length + 1 );
        }
        string_ptr = OUT_DEV_SET_OFF( out_device->driver_name, length + 1 );

        fread_buff( string_ptr, length, fp );
        if( ferror( fp )
          || feof( fp ) ) {
            mem_free( out_device );
            out_device = NULL;
            return( out_device );
        }
        string_ptr[length] = '\0';
    } else {
        out_device->driver_name = NULL;
    }

    /* Get the output_name. */

    length = fread_u8( fp );
    if( ferror( fp )
      || feof( fp ) ) {
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    if( length > 0 ) {
        if( OUT_DEV_EXPAND_CHK( length + 1 ) ) {
            out_device = resize_cop_device( out_device, length + 1 );
        }
        string_ptr = OUT_DEV_SET_OFF( out_device->output_name, length + 1 );

        fread_buff( string_ptr, length, fp );
        if( ferror( fp )
          || feof( fp ) ) {
            mem_free( out_device );
            out_device = NULL;
            return( out_device );
        }
        string_ptr[length] = '\0';
    } else {
        out_device->output_name = NULL;
    }

    /* Get the output_extension. */

    length = fread_u8( fp );
    if( ferror( fp )
      || feof( fp ) ) {
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    if( length > 0 ) {
        if( OUT_DEV_EXPAND_CHK( length + 1 ) ) {
            out_device = resize_cop_device( out_device, length + 1 );
        }
        string_ptr = OUT_DEV_SET_OFF( out_device->output_extension, length + 1 );

        fread_buff( string_ptr, length, fp );
        if( ferror( fp )
          || feof( fp ) ) {
            mem_free( out_device );
            out_device = NULL;
            return( out_device );
        }
        string_ptr[length] = '\0';
    } else {
        out_device->output_extension = NULL;
    }

    /* Get the numeric attributes, the "next codeblock" value, and the
     * PagegeometryBlock attributes.
     */

    /* The designator must be 0x2200 in a same-endian version 4.1 file. */

    designator = fread_u16( fp );
    if( ferror( fp )
      || feof( fp ) ) {
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    if( designator != 0x2200 ) {
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Get the page_width. */

    out_device->page_width = fread_u32( fp );
    if( ferror( fp )
      || feof( fp ) ) {
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Get the page_depth. */

    out_device->page_depth = fread_u32( fp );
    if( ferror( fp )
      || feof( fp ) ) {
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Get the horizontal_base_units. */

    out_device->horizontal_base_units = fread_u32( fp );
    if( ferror( fp )
      || feof( fp ) ) {
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Get the vertical_base_units. */

    out_device->vertical_base_units = fread_u32( fp );
    if( ferror( fp )
      || feof( fp ) ) {
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Skip the next_codeblock value. */

    fseek( fp, U16_SIZE, SEEK_CUR );
    if( ferror( fp )
      || feof( fp ) ) {
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Get the x_start value. */

    out_device->x_start = fread_u32( fp );
    if( ferror( fp )
      || feof( fp ) ) {
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Get the y_start value. */

    out_device->y_start = fread_u32( fp );
    if( ferror( fp )
      || feof( fp ) ) {
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Get the x_offset value. */

    out_device->x_offset = fread_u32( fp );
    if( ferror( fp )
      || feof( fp ) ) {
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Get the y_offset value. */

    out_device->y_offset = fread_u32( fp );
    if( ferror( fp )
      || feof( fp ) ) {
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Get the BoxBlock. */

    /* Get the FontAttribute, which can be a string or a number. */

    designator = fread_u16( fp );
    if( ferror( fp )
      || feof( fp ) ) {
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    switch( designator ) {
    case 0x0101:

        /* The font attribute is numeric: get the font number. */

        count = fread_u8( fp );
        if( ferror( fp )
          || feof( fp ) ) {
            mem_free( out_device );
            out_device = NULL;
            return( out_device );
        }

        out_device->box.font = fread_u8( fp );
        if( ferror( fp )
          || feof( fp ) ) {
            mem_free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Ensure that the font_name is NULL. */

        out_device->box.font_name = NULL;
        break;
    case 0x0201:

        /* The font attribute is a character string: get the font_name. */

        length = fread_u8( fp );
        if( ferror( fp )
          || feof( fp ) ) {
            mem_free( out_device );
            out_device = NULL;
            return( out_device );
        }

        if( length > 0 ) {
            if( OUT_DEV_EXPAND_CHK( length + 1 ) ) {
                out_device = resize_cop_device( out_device, length + 1 );
            }
            string_ptr = OUT_DEV_SET_OFF( out_device->box.font_name, length + 1 );

            fread_buff( string_ptr, length, fp );
            if( ferror( fp )
              || feof( fp ) ) {
                mem_free( out_device );
                out_device = NULL;
                return( out_device );
            }
            string_ptr[length] = '\0';
        } else {
            mem_free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Ensure that the font number is 0. */

        out_device->box.font = FONT0;
        break;
    default:
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Get the count and verify that it contains 0x0F. */

    count = fread_u8( fp );
    if( ferror( fp )
      || feof( fp ) ) {
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    if( count != 0x0F ) {
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* There are 0x0F bytes in the file but only 11 values. */

    fread_buff( &out_device->box.chars, 11, fp );
    if( ferror( fp )
      || feof( fp ) ) {
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Skip the 4 unused bytes. */

    fseek( fp, 4, SEEK_CUR );
    if( ferror( fp )
      || feof( fp ) ) {
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Get the UnderscoreBlock. */

    /* Get the FontAttribute, which can be a string or a number. */

    designator = fread_u16( fp );
    if( ferror( fp )
      || feof( fp ) ) {
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    switch( designator ) {
    case 0x0101:

        /* The font attribute is numeric: get the font number. */

        count = fread_u8( fp );
        if( ferror( fp )
          || feof( fp ) ) {
            mem_free( out_device );
            out_device = NULL;
            return( out_device );
        }

        out_device->underscore.font = fread_u8( fp );
        if( ferror( fp )
          || feof( fp ) ) {
            mem_free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Ensure that the font number is used. */

        out_device->underscore.specified_font = true;
        out_device->underscore.font_name = NULL;
        break;
    case 0x0201:

        /* The font attribute is a character string: get the font_name. */

        length = fread_u8( fp );
        if( ferror( fp )
          || feof( fp ) ) {
            mem_free( out_device );
            out_device = NULL;
            return( out_device );
        }

        if( length > 0 ) {
            if( OUT_DEV_EXPAND_CHK( length + 1 ) ) {
                out_device = resize_cop_device( out_device, length + 1 );
            }
            string_ptr = OUT_DEV_SET_OFF( out_device->underscore.font_name, length + 1 );

            fread_buff( string_ptr, length, fp );
            if( ferror( fp )
              || feof( fp ) ) {
                mem_free( out_device );
                out_device = NULL;
                return( out_device );
            }
            string_ptr[length] = '\0';

            /* Ensure that the font_name is used. */

            out_device->underscore.specified_font = true;
        } else {

            /* Ensure that the current font is used. */

            out_device->underscore.specified_font = false;
            out_device->underscore.font_name = NULL;
        }

        /* Ensure that the font number is 0. */

        out_device->underscore.font = FONT0;
        break;
    default:
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Get the count and verify that it contains 0x05. */

    count = fread_u8( fp );
    if( ferror( fp )
      || feof( fp ) ) {
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    if( count != 0x05 ) {
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* There are 0x05 bytes in the file but only one value. */

    out_device->underscore.underscore_char = fread_u8( fp );
    if( ferror( fp )
      || feof( fp ) ) {
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Skip the 4 unused bytes. */

    fseek( fp, 4, SEEK_CUR );
    if( ferror( fp )
      || feof( fp ) ) {
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Get the TranslationBlock. */

    /* Get the count and verify that it containss 0x03. */

    count = fread_u8( fp );
    if( ferror( fp )
      || feof( fp ) ) {
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    if( count != 0x03 ) {
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Now get the data_count and flags. */

    data_count = fread_u8( fp );
    if( ferror( fp )
      || feof( fp ) ) {
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    outtrans_flag = fread_u8( fp );
    if( ferror( fp )
      || feof( fp ) ) {
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    intrans_flag = fread_u8( fp );
    if( ferror( fp )
      || feof( fp ) ) {
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Get the IntransBlock, if present. */

    if( intrans_flag == 0 ) {
        out_device->intrans = NULL;
    } else {

        /* Verify that the next byte is 0x81. */

        count = fread_u8( fp );
        if( ferror( fp )
          || feof( fp ) ) {
            mem_free( out_device );
            out_device = NULL;
            return( out_device );
        }

        if( count != 0x81 ) {
            mem_free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Get the count byte and verify that it contains 0x00. */

        count = fread_u8( fp );
        if( ferror( fp )
          || feof( fp ) ) {
            mem_free( out_device );
            out_device = NULL;
            return( out_device );
        }

        if( count != 0x00 ) {
            mem_free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Get the data into the array. */

        if( OUT_DEV_EXPAND_CHK( sizeof( out_device->intrans->table ) ) ) {
            out_device = resize_cop_device( out_device, sizeof( out_device->intrans->table ) );
        }
        byte_ptr = OUT_DEV_SET_OFF( out_device->intrans, sizeof( out_device->intrans->table ) );

        fread_buff( byte_ptr, 0x100, fp );
        if( ferror( fp )
          || feof( fp ) ) {
            mem_free( out_device );
            out_device = NULL;
            return( out_device );
        }
    }

    /* Get the OuttransBlock, if present. */

    if( outtrans_flag == 0 ) {
        out_device->outtrans = NULL;
    } else {

        /* Get the next byte, which indicates the OuttransBlock data size. */

        outtrans_data_size = fread_u8( fp );
        if( ferror( fp )
          || feof( fp ) ) {
            mem_free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Read the count byte. */

        count = fread_u8( fp );
        if( ferror( fp )
          || feof( fp ) ) {
            mem_free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* The file is positioned at the start of the data. */

        switch( outtrans_data_size) {
        case 0x81:

            /* The count byte should be 0x00. */

            if( count != 0x00 ) {
                mem_free( out_device );
                out_device = NULL;
                return( out_device );
            }

            /* Get the data into the local buffer. */

            fread_buff( uint8_array, 0x100, fp );
            if( ferror( fp )
              || feof( fp ) ) {
                mem_free( out_device );
                out_device = NULL;
                return( out_device );
            }

            /* Reserve space for the outtrans_block. */

            if( OUT_DEV_EXPAND_CHK( sizeof( out_device->outtrans->table ) ) ) {
                out_device = resize_cop_device( out_device, sizeof( out_device->outtrans->table ) );
            }
            outtrans_ptr = OUT_DEV_SET_OFF( out_device->outtrans, sizeof( out_device->outtrans->table ) );

            /* Build the actual table, which requires actual pointers in
             * place of the offsets recorded in *out_device:
             *      outtrans_ptr is the pointer version of out_device->outtrans.
             *      for each iteration:
             *          translation_ptr is the pointer version of
             *              outtrans->ptr->table[i].
             *          byte_ptr is the pointer version of
             *              translation_ptr->ptr.data.
             */

            for( i = 0; i < 0x100; i++ ) {

                /* If the first byte matches the index, there is no
                 * translation.
                 */

                if( uint8_array[i] == i ) {
                    outtrans_ptr->table[i] = NULL;
                } else {

                    /* Reserve space for the translation. */

                    /* The translation always contains exactly one character. */

                    size = 1;

                    if( OUT_DEV_EXPAND_CHK( sizeof( translation ) + size ) ) {
                        out_device = resize_cop_device( out_device, sizeof( translation ) + size );
                        outtrans_ptr = OUT_DEV_MAP_OFF( out_device->outtrans );
                    }
                    translation_ptr = OUT_DEV_SET_OFF( outtrans_ptr->table[i], sizeof( translation ) );

                    translation_ptr->count = size;
                    byte_ptr = OUT_DEV_SET_OFF( translation_ptr->data, size );

                    /* Get the translation for the current character. */

                    /* The translation character is the value in the input
                     * array.
                     */

                    *byte_ptr = uint8_array[i];
                }
            }
            break;
        case 0x82:

            /* The count byte should be equal to data_count. */

            if( count != data_count ) {
                mem_free( out_device );
                out_device = NULL;
                return( out_device );
            }

            /* Get the outtrans array into the local array. */

            for( i = 0; i < 0x100; i++ ) {
                uint16_array[i] = fread_u16( fp );
                if( ferror( fp )
                  || feof( fp ) ) {
                    mem_free( out_device );
                    out_device = NULL;
                    return( out_device );
                }
            }

            /* Allocate a buffer and read the translation characters into it. */

            outtrans_data = mem_alloc( data_count );

            fread_buff( outtrans_data, data_count, fp );
            if( ferror( fp )
              || feof( fp ) ) {
                mem_free( outtrans_data );
                outtrans_data = NULL;
                mem_free( out_device );
                out_device = NULL;
                return( out_device );
            }

            /* Initialize outtrans_ptr and the outtrans pointer in
             * out_device.
             */

            if( OUT_DEV_EXPAND_CHK( sizeof( out_device->outtrans->table ) ) ) {
                out_device = resize_cop_device( out_device, sizeof( out_device->outtrans->table ) );
            }
            outtrans_ptr = OUT_DEV_SET_OFF( out_device->outtrans, sizeof( out_device->outtrans->table ) );

            /* Convert the data in uint16_array to our format, which requires
             * actual pointers in place of the offsets recorded in *out_device:
             *      outtrans_ptr is the pointer version of out_device->outtrans.
             *      for each iteration:
             *          translation_ptr is the pointer version of
             *              outtrans->ptr->table[i].
             *          byte_ptr is the pointer version of
             *              translation_ptr->ptr.data.
             */

            for( i = 0; i < 0x100; i++ ) {

                /* If the first byte matches the index, there is no
                 *  translation.
                 */

                if( uint16_array[i] == i ) {
                    outtrans_ptr->table[i] = NULL;
                } else {

                    /* Set the count correctly. */

                    if( (uint16_array[i] & 0xff00) == 0x00 ) {
                        translation_start = NULL;
                        size = 1;
                    } else {
                        translation_start = outtrans_data + (uint16_array[i] & 0x00ff);
                        size = *translation_start;
                    }

                    /* Reserve space for the translation. */

                    if( OUT_DEV_EXPAND_CHK( sizeof( translation ) + size ) ) {
                        out_device = resize_cop_device( out_device, sizeof( translation ) + size );
                        outtrans_ptr = OUT_DEV_MAP_OFF( out_device->outtrans );
                    }
                    translation_ptr = OUT_DEV_SET_OFF( outtrans_ptr->table[i], sizeof( translation ) );

                    /* Allocate space and perform other common operations. */

                    translation_ptr->count = size;
                    byte_ptr = OUT_DEV_SET_OFF( translation_ptr->data, size );

                    /* Get the translation for the current character */

                    /* If the first byte at uint16_array[i] is 00, then the
                     * second byte is the one-byte translation. Otherwise, the
                     * second byte is an offset. For little-endian computers,
                     * at least!
                     */

                    /* Put the data into the buffer. */

                    if( (uint16_array[i] & 0xff00) == 0x00 ) {
                        *byte_ptr = (uint16_array[i] & 0x00ff);
                    } else {
                        ++translation_start;
                        memcpy( byte_ptr, translation_start, size );
                    }
                }
            }
            mem_free( outtrans_data );
            outtrans_data = NULL;
            break;
        default:
            mem_free( out_device );
            out_device = NULL;
            return( out_device );
        }
    }

    /* Get the DefaultfontBlock. */

    /* Get the count and verify that it is 0x02. */

    count = fread_u8( fp );
    if( ferror( fp )
      || feof( fp ) ) {
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    if( count != 0x02 ) {
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Get the number of DefaultFonts. */

    out_device->defaultfonts.font_count = fread_u16( fp );
    if( ferror( fp )
      || feof( fp ) ) {
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Set defaultfont_ptr and defaultfonts.font and adjust next_offset. */

    size = out_device->defaultfonts.font_count * sizeof( default_font );

    if( OUT_DEV_EXPAND_CHK( size ) ) {
        out_device = resize_cop_device( out_device, size );
    }
    defaultfont_ptr = OUT_DEV_SET_OFF( out_device->defaultfonts.fonts, size );

    /* Get the DefaultFonts. */

    for( i = 0; i < out_device->defaultfonts.font_count; i++ ) {

        /* Get the font_style. */

        length = fread_u8( fp );
        if( ferror( fp )
          || feof( fp ) ) {
            mem_free( out_device );
            out_device = NULL;
            return( out_device );
        }

        if( length > 0 ) {
            if( OUT_DEV_EXPAND_CHK( length + 1 ) ) {
                out_device = resize_cop_device( out_device, length + 1 );
                defaultfont_ptr = OUT_DEV_MAP_OFF( out_device->defaultfonts.fonts );
            }
            string_ptr = OUT_DEV_SET_OFF( defaultfont_ptr[i].font_style, length + 1 );

            fread_buff( string_ptr, length, fp );
            if( ferror( fp )
              || feof( fp ) ) {
                mem_free( out_device );
                out_device = NULL;
                return( out_device );
            }
            string_ptr[length] = '\0';
        } else {
            defaultfont_ptr[i].font_style = NULL;
        }

        /* Get the count and verify that it is 0x04. */

        count = fread_u8( fp );
        if( ferror( fp )
          || feof( fp ) ) {
            mem_free( out_device );
            out_device = NULL;
            return( out_device );
        }

        if( count != 0x04 ) {
            mem_free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Get the font_height. */

        defaultfont_ptr[i].font_height = fread_u16( fp );
        if( ferror( fp )
          || feof( fp ) ) {
            mem_free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Get the font_space. */

        defaultfont_ptr[i].font_space = fread_u16( fp );
        if( ferror( fp )
          || feof( fp ) ) {
            mem_free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Get the font_name. */

        length = fread_u8( fp );
        if( ferror( fp )
          || feof( fp ) ) {
            mem_free( out_device );
            out_device = NULL;
            return( out_device );
        }

        if( length > 0 ) {
            if( OUT_DEV_EXPAND_CHK( length + 1 ) ) {
                out_device = resize_cop_device( out_device, length + 1 );
                defaultfont_ptr = OUT_DEV_MAP_OFF( out_device->defaultfonts.fonts );
            }
            string_ptr = OUT_DEV_SET_OFF( defaultfont_ptr[i].font_name, length + 1 );

            fread_buff( string_ptr, length, fp );
            if( ferror( fp )
              || feof( fp ) ) {
                mem_free( out_device );
                out_device = NULL;
                return( out_device );
            }
            string_ptr[length] = '\0';
        } else {
            defaultfont_ptr[i].font_name = NULL;
        }
    }

    /* Now get the FunctionsBlock and position fp to the start of
     * the PauseBlock. This must be done even if no functions are present.
     */

    raw_functions = get_p_buffer( fp );
    if( raw_functions == NULL ) {
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Get the Version A FunctionsBlock. This contains all CodeBlocks
     * generated by the :PAUSE and :FONTPAUSE blocks, if there are any.
     */

    current = raw_functions->buffer;
    cop_functions = parse_functions_block( &current, raw_functions->buffer );

    if( cop_functions->count > 0 )
        set_cumulative_index( cop_functions );

    /* Get the PauseBlock. */

    /* Get the start_pause. */

    /* Get the count and verify that it is 0x02. */

    count = fread_u8( fp );
    if( ferror( fp )
      || feof( fp ) ) {
        mem_free( raw_functions );
        raw_functions = NULL;
        if( cop_functions->code_blocks != NULL ) {
            mem_free( cop_functions->code_blocks );
            cop_functions->code_blocks = NULL;
        }
        mem_free( cop_functions );
        cop_functions = NULL;
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    if( count != 0x02 ) {
        mem_free( raw_functions );
        raw_functions = NULL;
        if( cop_functions->code_blocks != NULL ) {
            mem_free( cop_functions->code_blocks );
            cop_functions->code_blocks = NULL;
        }
        mem_free( cop_functions );
        cop_functions = NULL;
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Get the value to use to find the CodeBlock. */

    cumulative_index = fread_u16( fp );
    if( ferror( fp )
      || feof( fp ) ) {
        mem_free( raw_functions );
        raw_functions = NULL;
        if( cop_functions->code_blocks != NULL ) {
            mem_free( cop_functions->code_blocks );
            cop_functions->code_blocks = NULL;
        }
        mem_free( cop_functions );
        cop_functions = NULL;
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* The value 0xFFFF indicates that no such pause exists. */

    if( cumulative_index == 0xFFFF ) {
        out_device->pauses.start_pause = NULL;
    } else {

        /* Ensure that the CodeBlock can be found. */

        return_value = find_cumulative_index( cop_functions, cumulative_index, &j );
        if( return_value == not_valid ) {
            mem_free( raw_functions );
            raw_functions = NULL;
            if( cop_functions->code_blocks != NULL ) {
                mem_free( cop_functions->code_blocks );
                cop_functions->code_blocks = NULL;
            }
            mem_free( cop_functions );
            cop_functions = NULL;
            mem_free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Allocate space for the code_text instance. */

        size = cop_functions->code_blocks[j].count;

        if( OUT_DEV_EXPAND_CHK( sizeof( code_text ) + size ) ) {
            out_device = resize_cop_device( out_device, sizeof( code_text ) + size );
        }
        pause_ptr = OUT_DEV_SET_OFF( out_device->pauses.start_pause, sizeof( code_text ) );

        pause_ptr->count = size;
        byte_ptr = OUT_DEV_SET_OFF( pause_ptr->text, size );

        /* Initialize the code_text instance. */

        memcpy( byte_ptr, cop_functions->code_blocks[j].text, size );
    }

    /* Get the document_pause. */

    /* Get the count and verify that it is 0x02. */

    count = fread_u8( fp );
    if( ferror( fp )
      || feof( fp ) ) {
        mem_free( raw_functions );
        raw_functions = NULL;
        if( cop_functions->code_blocks != NULL ) {
            mem_free( cop_functions->code_blocks );
            cop_functions->code_blocks = NULL;
        }
        mem_free( cop_functions );
        cop_functions = NULL;
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    if( count != 0x02 ) {
        mem_free( raw_functions );
        raw_functions = NULL;
        if( cop_functions->code_blocks != NULL ) {
            mem_free( cop_functions->code_blocks );
            cop_functions->code_blocks = NULL;
        }
        mem_free( cop_functions );
        cop_functions = NULL;
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Get the value to use to find the CodeBlock. */

    cumulative_index = fread_u16( fp );
    if( ferror( fp )
      || feof( fp ) ) {
        mem_free( raw_functions );
        raw_functions = NULL;
        if( cop_functions->code_blocks != NULL ) {
            mem_free( cop_functions->code_blocks );
            cop_functions->code_blocks = NULL;
        }
        mem_free( cop_functions );
        cop_functions = NULL;
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* The value 0xFFFF indicates that no such pause exists. */

    if( cumulative_index == 0xFFFF ) {
        out_device->pauses.document_pause = NULL;
    } else {

        /* Ensure that the CodeBlock can be found. */

        return_value = find_cumulative_index( cop_functions, cumulative_index, &j );
        if( return_value == not_valid ) {
            mem_free( raw_functions );
            raw_functions = NULL;
            if( cop_functions->code_blocks != NULL ) {
                mem_free( cop_functions->code_blocks );
                cop_functions->code_blocks = NULL;
            }
            mem_free( cop_functions );
            cop_functions = NULL;
            mem_free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Allocate space for the code_text instance. */

        size = cop_functions->code_blocks[j].count;

        if( OUT_DEV_EXPAND_CHK( sizeof( code_text ) + size ) ) {
            out_device = resize_cop_device( out_device, sizeof( code_text ) + size );
        }
        pause_ptr = OUT_DEV_SET_OFF( out_device->pauses.document_pause, sizeof( code_text ) );

        pause_ptr->count = size;
        byte_ptr = OUT_DEV_SET_OFF( pause_ptr->text, size );

        /* Initialize the code_text instance. */

        memcpy( byte_ptr, cop_functions->code_blocks[j].text, size );
    }

    /* Get the docpage_pause. */

    /* Get the count and verify that it is 0x02. */

    count = fread_u8( fp );
    if( ferror( fp )
      || feof( fp ) ) {
        mem_free( raw_functions );
        raw_functions = NULL;
        if( cop_functions->code_blocks != NULL ) {
            mem_free( cop_functions->code_blocks );
            cop_functions->code_blocks = NULL;
        }
        mem_free( cop_functions );
        cop_functions = NULL;
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    if( count != 0x02 ) {
        mem_free( raw_functions );
        raw_functions = NULL;
        if( cop_functions->code_blocks != NULL ) {
            mem_free( cop_functions->code_blocks );
            cop_functions->code_blocks = NULL;
        }
        mem_free( cop_functions );
        cop_functions = NULL;
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Get the value to use to find the CodeBlock. */

    cumulative_index = fread_u16( fp );
    if( ferror( fp )
      || feof( fp ) ) {
        mem_free( raw_functions );
        raw_functions = NULL;
        if( cop_functions->code_blocks != NULL ) {
            mem_free( cop_functions->code_blocks );
            cop_functions->code_blocks = NULL;
        }
        mem_free( cop_functions );
        cop_functions = NULL;
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* The value 0xFFFF indicates that no such pause exists. */

    if( cumulative_index == 0xFFFF ) {
        out_device->pauses.docpage_pause = NULL;
    } else {

        /* Ensure that the CodeBlock can be found. */

        return_value = find_cumulative_index( cop_functions, cumulative_index, &j );
        if( return_value == not_valid ) {
            mem_free( raw_functions );
            raw_functions = NULL;
            if( cop_functions->code_blocks != NULL ) {
                mem_free( cop_functions->code_blocks );
                cop_functions->code_blocks = NULL;
            }
            mem_free( cop_functions );
            cop_functions = NULL;
            mem_free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Allocate space for the code_text instance. */

        size = cop_functions->code_blocks[j].count;

        if( OUT_DEV_EXPAND_CHK( sizeof( code_text ) + size ) ) {
            out_device = resize_cop_device( out_device, sizeof( code_text ) + size );
        }
        pause_ptr = OUT_DEV_SET_OFF( out_device->pauses.docpage_pause, sizeof( code_text ) );

        pause_ptr->count = size;
        byte_ptr = OUT_DEV_SET_OFF( pause_ptr->text, size );

        /* Initialize the code_text instance. */

        memcpy( byte_ptr, cop_functions->code_blocks[j].text, size );
    }

    /* Get the devpage_pause. */

    /* Get the count and verify that it is 0x02. */

    count = fread_u8( fp );
    if( ferror( fp )
      || feof( fp ) ) {
        mem_free( raw_functions );
        raw_functions = NULL;
        if( cop_functions->code_blocks != NULL ) {
            mem_free( cop_functions->code_blocks );
            cop_functions->code_blocks = NULL;
        }
        mem_free( cop_functions );
        cop_functions = NULL;
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    if( count != 0x02 ) {
        mem_free( raw_functions );
        raw_functions = NULL;
        if( cop_functions->code_blocks != NULL ) {
            mem_free( cop_functions->code_blocks );
            cop_functions->code_blocks = NULL;
        }
        mem_free( cop_functions );
        cop_functions = NULL;
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Get the value to use to find the CodeBlock. */

    cumulative_index = fread_u16( fp );
    if( ferror( fp )
      || feof( fp ) ) {
        mem_free( raw_functions );
        raw_functions = NULL;
        if( cop_functions->code_blocks != NULL ) {
            mem_free( cop_functions->code_blocks );
            cop_functions->code_blocks = NULL;
        }
        mem_free( cop_functions );
        cop_functions = NULL;
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* The value 0xFFFF indicates that no such pause exists. */

    if( cumulative_index == 0xFFFF ) {
        out_device->pauses.devpage_pause = NULL;
    } else {

        /* Ensure that the CodeBlock can be found. */

        return_value = find_cumulative_index( cop_functions, cumulative_index, &j );
        if( return_value == not_valid ) {
            mem_free( raw_functions );
            raw_functions = NULL;
            if( cop_functions->code_blocks != NULL ) {
                mem_free( cop_functions->code_blocks );
                cop_functions->code_blocks = NULL;
            }
            mem_free( cop_functions );
            cop_functions = NULL;
            mem_free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Allocate space for the code_text instance. */

        size = cop_functions->code_blocks[j].count;

        if( OUT_DEV_EXPAND_CHK( sizeof( code_text ) + size ) ) {
            out_device = resize_cop_device( out_device, sizeof( code_text ) + size );
        }
        pause_ptr = OUT_DEV_SET_OFF( out_device->pauses.devpage_pause, sizeof( code_text ) );

        pause_ptr->count = size;
        byte_ptr = OUT_DEV_SET_OFF( pause_ptr->text, size );

        /* Initialize the code_text instance. */

        memcpy( byte_ptr, cop_functions->code_blocks[j].text, size );
    }

    /* Get the DevicefontBlock. */

    /* Get the count and verify that it is 0x02. */

    count = fread_u8( fp );
    if( ferror( fp )
      || feof( fp ) ) {
        mem_free( raw_functions );
        raw_functions = NULL;
        if( cop_functions->code_blocks != NULL ) {
            mem_free( cop_functions->code_blocks );
            cop_functions->code_blocks = NULL;
        }
        mem_free( cop_functions );
        cop_functions = NULL;
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    if( count != 0x02 ) {
        mem_free( raw_functions );
        raw_functions = NULL;
        if( cop_functions->code_blocks != NULL ) {
            mem_free( cop_functions->code_blocks );
            cop_functions->code_blocks = NULL;
        }
        mem_free( cop_functions );
        cop_functions = NULL;
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Get the number of Devicefonts. */

    out_device->devicefonts.font_count = fread_u16( fp );
    if( ferror( fp )
      || feof( fp ) ) {
        mem_free( raw_functions );
        raw_functions = NULL;
        if( cop_functions->code_blocks != NULL ) {
            mem_free( cop_functions->code_blocks );
            cop_functions->code_blocks = NULL;
        }
        mem_free( cop_functions );
        cop_functions = NULL;
        mem_free( out_device );
        out_device = NULL;
        return( out_device );
    }

    /* Get the Devicefonts. */

    size = out_device->devicefonts.font_count * sizeof( device_font );
    if( OUT_DEV_EXPAND_CHK( size ) ) {
        out_device = resize_cop_device( out_device, size );
    }
    devicefont_ptr = OUT_DEV_SET_OFF( out_device->devicefonts.fonts, size );

    for( i = 0; i < out_device->devicefonts.font_count; i++ ) {

        /* Get the font_name. */

        length = fread_u8( fp );
        if( ferror( fp )
          || feof( fp ) ) {
            mem_free( raw_functions );
            raw_functions = NULL;
            if( cop_functions->code_blocks != NULL ) {
                mem_free( cop_functions->code_blocks );
                cop_functions->code_blocks = NULL;
            }
            mem_free( cop_functions );
            cop_functions = NULL;
            mem_free( out_device );
            out_device = NULL;
            return( out_device );
        }

        if( length > 0 ) {
            if( OUT_DEV_EXPAND_CHK( length + 1 ) ) {
                out_device = resize_cop_device( out_device, length + 1 );
                devicefont_ptr = OUT_DEV_MAP_OFF( out_device->devicefonts.fonts );
            }
            string_ptr = OUT_DEV_SET_OFF( devicefont_ptr[i].font_name, length + 1 );

            fread_buff( string_ptr, length, fp );
            if( ferror( fp )
              || feof( fp ) ) {
                raw_functions = NULL;
                if( cop_functions->code_blocks != NULL ) {
                    mem_free( cop_functions->code_blocks );
                    cop_functions->code_blocks = NULL;
                }
                mem_free( cop_functions );
                cop_functions = NULL;
                mem_free( out_device );
                out_device = NULL;
                return( out_device );
            }
            string_ptr[length] = '\0';
        } else {
            raw_functions = NULL;
            if( cop_functions->code_blocks != NULL ) {
                mem_free( cop_functions->code_blocks );
                cop_functions->code_blocks = NULL;
            }
            mem_free( cop_functions );
            cop_functions = NULL;
            mem_free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Get the font_switch. */

        length = fread_u8( fp );
        if( ferror( fp )
          || feof( fp ) ) {
            mem_free( raw_functions );
            raw_functions = NULL;
            if( cop_functions->code_blocks != NULL ) {
                mem_free( cop_functions->code_blocks );
                cop_functions->code_blocks = NULL;
            }
            mem_free( cop_functions );
            cop_functions = NULL;
            mem_free( out_device );
            out_device = NULL;
            return( out_device );
        }

        if( length > 0 ) {
            if( OUT_DEV_EXPAND_CHK( length + 1 ) ) {
                out_device = resize_cop_device( out_device, length + 1 );
                devicefont_ptr = OUT_DEV_MAP_OFF( out_device->devicefonts.fonts );
            }
            string_ptr = OUT_DEV_SET_OFF( devicefont_ptr[i].font_switch, length + 1 );

            fread_buff( string_ptr, length, fp );
            if( ferror( fp )
              || feof( fp ) ) {
                mem_free( raw_functions );
                raw_functions = NULL;
                if( cop_functions->code_blocks != NULL ) {
                    mem_free( cop_functions->code_blocks );
                    cop_functions->code_blocks = NULL;
                }
                mem_free( cop_functions );
                cop_functions = NULL;
                mem_free( out_device );
                out_device = NULL;
                return( out_device );
            }
            string_ptr[length] = '\0';
        } else {
            devicefont_ptr[i].font_switch = NULL;
        }

        /* Get the nulls and verify that they are, in fact, nulls. */

        nulls = fread_u16( fp );
        if( ferror( fp )
          || feof( fp ) ) {
            mem_free( raw_functions );
            raw_functions = NULL;
            if( cop_functions->code_blocks != NULL ) {
                mem_free( cop_functions->code_blocks );
                cop_functions->code_blocks = NULL;
            }
            mem_free( cop_functions );
            cop_functions = NULL;
            mem_free( out_device );
            out_device = NULL;
            return( out_device );
        }

        if( nulls != 0x0000 ) {
            mem_free( raw_functions );
            raw_functions = NULL;
            if( cop_functions->code_blocks != NULL ) {
                mem_free( cop_functions->code_blocks );
                cop_functions->code_blocks = NULL;
            }
            mem_free( cop_functions );
            cop_functions = NULL;
            mem_free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Get the count and verify that it is 0x03. */

        count = fread_u8( fp );
        if( ferror( fp )
          || feof( fp ) ) {
            mem_free( raw_functions );
            raw_functions = NULL;
            if( cop_functions->code_blocks != NULL ) {
                mem_free( cop_functions->code_blocks );
                cop_functions->code_blocks = NULL;
            }
            mem_free( cop_functions );
            cop_functions = NULL;
            mem_free( out_device );
            out_device = NULL;
            return( out_device );
        }

        if( count != 0x03 ) {
            mem_free( raw_functions );
            raw_functions = NULL;
            if( cop_functions->code_blocks != NULL ) {
                mem_free( cop_functions->code_blocks );
                cop_functions->code_blocks = NULL;
            }
            mem_free( cop_functions );
            cop_functions = NULL;
            mem_free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Get the resident font indicator. */

        devicefont_ptr[i].resident = fread_u8( fp );
        if( ferror( fp )
          || feof( fp ) ) {
            mem_free( raw_functions );
            raw_functions = NULL;
            if( cop_functions->code_blocks != NULL ) {
                mem_free( cop_functions->code_blocks );
                cop_functions->code_blocks = NULL;
            }
            mem_free( cop_functions );
            cop_functions = NULL;
            mem_free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* Get the fontpause. */

        /* Get the value to use to find the CodeBlock. */

        cumulative_index = fread_u16( fp );
        if( ferror( fp )
          || feof( fp ) ) {
            mem_free( raw_functions );
            raw_functions = NULL;
            if( cop_functions->code_blocks != NULL ) {
                mem_free( cop_functions->code_blocks );
                cop_functions->code_blocks = NULL;
            }
            mem_free( cop_functions );
            cop_functions = NULL;
            mem_free( out_device );
            out_device = NULL;
            return( out_device );
        }

        /* The value 0xFFFF indicates that no such pause exists. */

        if( cumulative_index == 0xFFFF ) {
            devicefont_ptr[i].font_pause = NULL;
        } else {

            /* Ensure that the CodeBlock can be found. */

            return_value = find_cumulative_index( cop_functions, cumulative_index, &j );
            if( return_value == not_valid ) {
                mem_free( raw_functions );
                raw_functions = NULL;
                if( cop_functions->code_blocks != NULL ) {
                    mem_free( cop_functions->code_blocks );
                    cop_functions->code_blocks = NULL;
                }
                mem_free( cop_functions );
                cop_functions = NULL;
                mem_free( out_device );
                out_device = NULL;
                return( out_device );
            }

            /* Allocate space for the code_text instance. */

            size = cop_functions->code_blocks[j].count;

            if( OUT_DEV_EXPAND_CHK( sizeof( code_text ) + size ) ) {
                out_device = resize_cop_device( out_device, sizeof( code_text ) + size );
                devicefont_ptr = OUT_DEV_MAP_OFF( out_device->devicefonts.fonts );
                pause_ptr = OUT_DEV_MAP_OFF( devicefont_ptr[i].font_pause );
            }
            pause_ptr = OUT_DEV_SET_OFF( devicefont_ptr[i].font_pause, sizeof( code_text ) );

            pause_ptr->count = size;
            byte_ptr = OUT_DEV_SET_OFF( pause_ptr->text, size );

            /* Initialize the code_text instance. */

            memcpy( byte_ptr, cop_functions->code_blocks[j].text, size );
        }
    }

    /*  Free the memory used in parsing the CodeBlocks. */

    mem_free( raw_functions );
    raw_functions = NULL;
    if( cop_functions->code_blocks != NULL ) {
        mem_free( cop_functions->code_blocks );
        cop_functions->code_blocks = NULL;
    }
    mem_free( cop_functions );
    cop_functions = NULL;

    /* Convert non-NULL offsets to pointers. */

    if( out_device->driver_name != NULL ) {
        OUT_DEV_REMAP_MBR( driver_name );
    }

    if( out_device->output_name != NULL ) {
        OUT_DEV_REMAP_MBR( output_name );
    }

    if( out_device->output_extension != NULL ) {
        OUT_DEV_REMAP_MBR( output_extension );
    }

    if( out_device->box.font_name != NULL ) {
        OUT_DEV_REMAP_MBR( box.font_name );
    }

    if( out_device->underscore.font_name != NULL ) {
        OUT_DEV_REMAP_MBR( underscore.font_name );
    }

    if( out_device->intrans != NULL ) {
        OUT_DEV_REMAP_MBR( intrans );
    }

    if( out_device->outtrans != NULL ) {
        OUT_DEV_REMAP_MBR( outtrans );

        for( i = 0; i < sizeof( outtrans_block ) / sizeof( translation * ); i++ ) {
            if( out_device->outtrans->table[i] != NULL ) {
                OUT_DEV_REMAP_MBR( outtrans->table[i] );
                if( out_device->outtrans->table[i]->data != NULL ) {
                    OUT_DEV_REMAP_MBR( outtrans->table[i]->data );
                }
            }
        }
    }

    if( out_device->defaultfonts.fonts != NULL ) {
        OUT_DEV_REMAP_MBR( defaultfonts.fonts );
        for( i = 0; i < out_device->defaultfonts.font_count; i++ ) {
            OUT_DEV_REMAP_MBR( defaultfonts.fonts[i].font_name );
            if( out_device->defaultfonts.fonts[i].font_style != NULL ) {
                OUT_DEV_REMAP_MBR( defaultfonts.fonts[i].font_style );
            }
        }
    }

    if( out_device->pauses.start_pause != NULL ) {
        OUT_DEV_REMAP_MBR( pauses.start_pause );
        if( out_device->pauses.start_pause->text != NULL ) {
            OUT_DEV_REMAP_MBR( pauses.start_pause->text );
        }
    }

    if( out_device->pauses.document_pause != NULL ) {
        OUT_DEV_REMAP_MBR( pauses.document_pause );
        if( out_device->pauses.document_pause->text != NULL ) {
            OUT_DEV_REMAP_MBR( pauses.document_pause->text );
        }
    }

    if( out_device->pauses.docpage_pause != NULL ) {
        OUT_DEV_REMAP_MBR( pauses.docpage_pause );
        if( out_device->pauses.docpage_pause->text != NULL ) {
            OUT_DEV_REMAP_MBR( pauses.docpage_pause->text );
        }
    }

    if( out_device->pauses.devpage_pause != NULL ) {
        OUT_DEV_REMAP_MBR( pauses.devpage_pause );
        if( out_device->pauses.devpage_pause->text != NULL ) {
            OUT_DEV_REMAP_MBR( pauses.devpage_pause->text );
        }
    }

    if( out_device->devicefonts.fonts != NULL ) {
        OUT_DEV_REMAP_MBR( devicefonts.fonts );
        for( i = 0; i < out_device->devicefonts.font_count; i++ ) {
            if( out_device->devicefonts.fonts[i].font_name != NULL ) {
                OUT_DEV_REMAP_MBR( devicefonts.fonts[i].font_name );
            }
            if( out_device->devicefonts.fonts[i].font_switch != NULL ) {
                OUT_DEV_REMAP_MBR( devicefonts.fonts[i].font_switch );
            }
            if( out_device->devicefonts.fonts[i].font_pause != NULL ) {
                OUT_DEV_REMAP_MBR( devicefonts.fonts[i].font_pause );
                if( out_device->devicefonts.fonts[i].font_pause->text != NULL ) {
                    OUT_DEV_REMAP_MBR( devicefonts.fonts[i].font_pause->text );
                }
            }
        }
    }

    return( out_device );
}
