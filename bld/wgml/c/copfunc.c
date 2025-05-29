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
* Description:  Implements the functions declared in copfunc.h:
*                   get_code_blocks()
*                   get_p_buffer()
*                   parse_functions_block()
*
* Note:         The Wiki should be consulted for any term whose meaning is
*               not apparent. This should help in most cases.
*
****************************************************************************/


#include "wgml.h"
#include "copfunc.h"

/* Extern function definitions. */

/* Function get_code_blocks().
 * Return a pointer to an array of code_block structs containing the
 * CodeBlocks in a P-buffer.
 *
 * Parameters:
 *      current points to the first byte of the data to be processed.
 *      count contains the number of CodeBlocks expected.
 *      base points to the first byte of the underlying P-buffer.
 *
 * Modified parameter:
 *      current points to the byte after the last byte processed.
 *      on failure, it's position is not meaningful.
 *
 * Returns:
 *      a pointer to a code_block containing the CodeBlocks.
 *
 * Note:
 *      mem_alloc() will call exit() if the allocation fails.
 */

code_block *get_code_blocks( const char **current, int count, const char *base )
{
    code_block      *out_block   = NULL;
    unsigned        difference;
    unsigned        position;
    int             i;

    /* Allocate out_block. */

    out_block = mem_alloc( sizeof( code_block ) * count );

    /* Initialize each code_block. */

    for( i = 0; i < count; i++ ) {

       /* Get the position of the designator in the P-buffer. */

        difference = *current - base;
        position = difference % 80;

        /* Get the designator, shifting it if necessary. */

        if( position == 79 ) {
            *current += 1;
        }

        out_block[i].designator = get_u8( current );

        /* Skip the cb05_flag and the lp_flag. */

        *current += 2;

        /* Get the line pass, shifting it if necessary. */

        if( position == 76 ) {
            *current += 1;
        }

        out_block[i].line_pass = get_u16( current );

        /* Get the count, shifting it if necessary */

        if( position == 74 ) {
            *current += 1;
        }

        out_block[i].count = get_u16( current );

        /* Set function, which is the pointer to the actual compiled code. */

        if( &out_block[i].count == 0 ) {
            out_block[i].text = NULL;
        } else {
            out_block[i].text = *current;
            *current += out_block[i].count;
        }
    }

    return( out_block );
}

/* Function get_p_buffer().
 * Extract one or more contiguous P-buffers from fp.
 *
 * Parameter:
 *     fp points to the start of a P-buffer.
 *
 * Modified parameter:
 *     fp will point to the first non-P-buffer byte in the file on success.
 *     the status of fp is unpredictable on failure.
 *
 * Returns:
 *     a pointer to a p_buffer containing the raw data on success.
 *     a NULL pointer on failure.
 *
 * Note:
 *      mem_alloc() will call exit() if the allocation fails.
 *      if a non-NULL pointer is returned, it's buffer field will also be
 *          non-NULL.
 *      NULL is returned for file errors.
 */

p_buffer * get_p_buffer( FILE *fp )
{
    char        *current     = NULL;
    uint8_t     i;
    p_buffer    *out_buffer  = NULL;
    uint8_t     p_count;
    int         test_char;

    /* Determine the number of contiguous P-buffers in the file. */

    p_count = 0;
    test_char = fgetc( fp );
    if( ferror( fp ) || feof( fp ) ) {
        return( out_buffer );
    }
    while( test_char == 80 ) {
        ++p_count;
        fseek( fp, 80, SEEK_CUR );
        if( ferror( fp ) || feof( fp ) ) {
            return( out_buffer );
        }
        test_char = fgetc( fp );
        if( ferror( fp ) || feof( fp ) ) {
            return( out_buffer );
        }
    }

    /* There should always be at least one P-buffer. */

    if( p_count == 0 )
        return( out_buffer );

    /* Rewind the file by 81 bytes per P-buffer plus 1. */

    fseek( fp, SEEK_POSBACK( 81 * p_count + 1 ), SEEK_CUR );
    if( ferror( fp ) || feof( fp ) )
        return( out_buffer );

    /* Allocate the out_buffer. */

    current = mem_alloc( sizeof( p_buffer ) + 80 * p_count );
    out_buffer = (p_buffer *)current;
    current += sizeof( p_buffer );
    out_buffer->buffer = current;
    out_buffer->count = 80 * p_count;

    /* Now get the data into the out_buffer. */

    for( i = 0; i < p_count; i++ ) {
        test_char = fgetc( fp );
        if( ferror( fp ) || feof( fp ) ) {
            mem_free( out_buffer );
            out_buffer = NULL;
            return( out_buffer );
        }

        if( test_char != 80 ) {
            mem_free( out_buffer );
            out_buffer = NULL;
            return( out_buffer );
        }

        fread_buff( current, 80, fp );
        if( ferror( fp ) || feof( fp ) ) {
            mem_free( out_buffer );
            out_buffer = NULL;
            return( out_buffer );
        }

        current = current + 80;
    }

    return( out_buffer );
}

/* Function parse_functions_block().
 * Construct a functions_block containing the data in the P-buffer.
 *
 * Parameters:
 *      current contains the first byte of the data to be processed.
 *      base points to the first byte of the underlying P-buffer.
 *
 * Modified parameter:
 *      current will point to the first byte after the last byte parsed.
 *      on failure, the value of current will be meaningless.
 *
 * Note:
 *      if not NULL, out_block->code_blocks is allocated separately from
 *      out_block and so must be freed separately as well.
 *
 * Returns:
 *      a pointer to a functions_block containing the processed data.
 *
 * Notes:
 *      mem_alloc() will call exit() if the allocation fails.
 *      get_code_blocks() calls mem_alloc(), which will call exit() if
 *          the allocation fails.
 *      if there are no CodeBlocks in the file, then NULL will not be returned
 *          but its code_blocks field will be NULL.
 */

functions_block *parse_functions_block( const char **current, const char *base )
{
    unsigned        code_count;
    functions_block *out_block   = NULL;

    /* Get the number of CodeBlocks. */

    code_count = get_u16( current );

    /* Allocate the out_block. */

    out_block = mem_alloc( sizeof( functions_block ) );
    out_block->count = code_count;

    /* Now extract the CodeBlocks, if any. */

    if( out_block->count == 0 ) {
        out_block->code_blocks = NULL;
    } else {
        out_block->code_blocks = get_code_blocks( current, out_block->count, base );
    }

    return( out_block );
}

unsigned char fread_u8( FILE *fp )
{
    unsigned char   u8;

    fread( &u8, 1, U8_SIZE, fp );
    return( u8 );
}

unsigned short fread_u16( FILE *fp )
{
    uint16_t        u16;

    fread( &u16, 1, U16_SIZE, fp );
    return( u16 );
}

unsigned fread_u32( FILE *fp )
{
    uint32_t        u32;

    fread( &u32, 1, U32_SIZE, fp );
    return( u32 );
}

unsigned fread_buff( void *buff, int len, FILE *fp )
{
    return( fread( buff, 1, (unsigned)len, fp ) );
}

unsigned char get_u8( const char **buff )
{
    unsigned char   u8;

    memcpy( &u8, *buff, U8_SIZE );
    *buff += U8_SIZE;
    return( u8 );
}

unsigned short get_u16( const char **buff )
{
    uint16_t        u16;

    memcpy( &u16, *buff, U16_SIZE );
    *buff += U16_SIZE;
    return( u16 );
}

unsigned get_u32( const char **buff )
{
    uint32_t        u32;

    memcpy( &u32, *buff, U32_SIZE );
    *buff += U32_SIZE;
    return( u32 );
}

void get_buff( void *obuff, int len, const char **buff )
{
    memcpy( obuff, *buff, (unsigned)len );
    *buff += len;
}
