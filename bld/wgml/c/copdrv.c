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
* Description:  Implements the functions declared in copdrv.h:
*                   is_drv_file()
*                   parse_driver()
*               and contains these local functions:
*                   parse_finish_block()
*                   parse_font_style()
*                   parse_init_block()
*                   resize_cop_driver()
*
* Note:         The Wiki should be consulted for any term whose meaning is
*               not apparent. This should help in most cases.
*
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__ 1
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wgml.h"
#include "copdrv.h"
#include "copfunc.h"

/* Local macros. */

#define START_SIZE 2048
#define INC_SIZE   1024

/* Local function definitions. */

/* Function resize_cop_driver().
 * Resizes a cop_driver instance.
 *
 * Parameters:
 *      in_driver is a pointer to the cop_driver to be resized.
 *      in_size is the minimum acceptable increase in size.
 *
 *  Returns:
 *      A pointer to a cop_driver instance at least in_size larger with 
 *          the same data (except for the allocated_size field, which reflects 
 *          the new size).
 *
 * Notes:
 *      mem_realloc() will call exit() if the reallocation fails.
 *      mem_realloc() will free in_driver if the instance is actually moved to a
 *          new location.
 *      The intended use is for the pointer passed as in_driver to be used to
 *          store the return value.
 */

static cop_driver * resize_cop_driver( cop_driver * in_driver, size_t in_size )
{
    cop_driver *    local_driver = NULL;
    size_t          increment = INC_SIZE;
    size_t          new_size;
    size_t          scale;

    /* Compute how much larger to make the cop_driver struct. */

    if( in_size > INC_SIZE ) {
        scale = in_size / INC_SIZE;
        ++scale;
        increment = scale * INC_SIZE;
    }
    new_size = in_driver->allocated_size + increment;

    /* Reallocate the cop_driver. */

    local_driver = (cop_driver *) mem_realloc( in_driver, new_size );
    local_driver->allocated_size = new_size;

    return( local_driver );
}

/* Function parse_finish_block().
 * Processes a single FinishBlock. This may be called up to two times when
 * parsing a .COP file encoding a :DRIVER block. 
 *
 * Note: although a :FINISH block can contain more than one :VALUE block, and
 * gendev will put more than one CodeBlock into the .COP file, wgml only
 * interprets the first one. Thus, this function only parses the first CodeBlock.
 *
 * Parameters:
 *      in_driver contains the cop_driver being initialized.
 *      *current contains the current position in a p_buffer.buffer.
 *      base points to the first byte of the underlying P-buffer.
 *
 *  Parameter modified:
 *      if the function succeeds, then *current will point to the first junk
 *          byte following the last CodeBlock for this FinishBlock.
 *      if the function fails, the value of *current should be regarded as
 *          invalid.
 *
 *  Returns:
 *      in_driver, possibly relocated, on success.
 *      NULL on failure, which indicates an incorrectly formatted file.
 *
 * Notes:
 *      resize_cop_driver() calls mem_realloc(), which will call exit() if
 *          the reallocation fails.
 *      get_code_blocks() calls mem_alloc(), which will call exit() if
 *          the allocation fails.
 */

static cop_driver * parse_finish_block( cop_driver * in_driver, \
                                         uint8_t * * current, uint8_t * base )
{

    code_block *    cop_codeblocks  = NULL;
    code_text *     code_text_ptr   = NULL;
    uint8_t         designator;    
    uint8_t *       text_ptr        = NULL;
    uint16_t        count;

    /* Get the designator. */

    memcpy_s( &designator, 1, *current, 1 );
    (*current)++;

    /* Process the FinishBlock. */

    switch( designator ) {
    case 0x01 :
        memcpy_s( &count, sizeof( count ), *current, sizeof( count ) );
        *current += sizeof( count );
        if( count == 0x0000 ) break;

        /* Add the code_text struct itself */

        if( in_driver->allocated_size < (in_driver->next_offset + \
                                                        sizeof( code_text )) ) {
            in_driver = resize_cop_driver( in_driver, sizeof( code_text ) );
        }
        code_text_ptr = (code_text *) ((uint8_t *) in_driver + \
                                                        in_driver->next_offset);

        in_driver->finishes.end = (code_text *) in_driver->next_offset;
        in_driver->next_offset += sizeof( code_text );

        /* Get the CodeBlocks. */
        
        cop_codeblocks = get_code_blocks( current, count, base );
        if( cop_codeblocks == NULL ) break;

        /* Initialize the code_text struct. */

        if( cop_codeblocks[0].designator != 0x00 ) {
            mem_free( cop_codeblocks );
            cop_codeblocks = NULL;
            break;
        }

        code_text_ptr->count = cop_codeblocks[0].count;

        if( in_driver->allocated_size < (in_driver->next_offset + \
                                                        code_text_ptr->count) ) {
            in_driver = resize_cop_driver( in_driver, code_text_ptr->count );
            code_text_ptr = (code_text *) ((uint8_t *) in_driver + \
                                           (size_t) in_driver->finishes.end);
        }

        text_ptr = (uint8_t *) in_driver + in_driver->next_offset;

        memcpy_s( text_ptr, code_text_ptr->count, cop_codeblocks[0].text, \
                                                        code_text_ptr->count );
        code_text_ptr->text = (uint8_t *) in_driver->next_offset;
        in_driver->next_offset += code_text_ptr->count;

        mem_free( cop_codeblocks );
        cop_codeblocks = NULL;

        return( in_driver );
    case 0x02 :
        memcpy_s( &count, sizeof( count ), *current, sizeof( count ) );
        *current += sizeof( count );
        if( count == 0 ) break;

        /* Add the code_text struct itself. */

        if( in_driver->allocated_size < (in_driver->next_offset + \
                                                        sizeof( code_text )) ) {
            in_driver = resize_cop_driver( in_driver, sizeof( code_text ) );
        }
        code_text_ptr = (code_text *) ((uint8_t *) in_driver + \
                                                        in_driver->next_offset);
                                                        
        in_driver->finishes.document = (code_text *) in_driver->next_offset;
        in_driver->next_offset += sizeof( code_text );

        /* Get the CodeBlocks. */
        
        cop_codeblocks = get_code_blocks( current, count, base );
        if( cop_codeblocks == NULL ) break;
        
        /* Initialize the code_text struct. */

        if( cop_codeblocks[0].designator != 0x00 ) {
            mem_free( cop_codeblocks );
            cop_codeblocks = NULL;
            break;
        }

        code_text_ptr->count = cop_codeblocks[0].count;

        if( in_driver->allocated_size < (in_driver->next_offset + \
                                                        code_text_ptr->count) ) {
            in_driver = resize_cop_driver( in_driver, code_text_ptr->count );
            code_text_ptr = (code_text *) ((uint8_t *) in_driver + \
                                        (size_t) in_driver->finishes.document);
        }
        text_ptr = (uint8_t *) in_driver + in_driver->next_offset;

        memcpy_s( text_ptr, code_text_ptr->count, cop_codeblocks[0].text, \
                                                        code_text_ptr->count );
        code_text_ptr->text = (uint8_t *) in_driver->next_offset;
        in_driver->next_offset += code_text_ptr->count;

        mem_free( cop_codeblocks );
        cop_codeblocks = NULL;

        return( in_driver );
    }

    /* If we get here, then an error has occurred. */

    mem_free( in_driver );
    in_driver = NULL;

    return( in_driver );
}

/* Function parse_font_style().
 * Processes a single ShortFontstyleBlock. This may be called any number of times
 * when parsing a .COP file encoding a :DRIVER block.
 *
 * Note:
 *      The Wiki should be consulted if the treatment of p_buffer_set and
 *      count appears inexplicable. The structure in the .COP file is very
 *      odd and something like the contortions shown here are unavoidable.
 *
 * Parameters:
 *      in_file is the file being parsed.
 *      in_driver contains the cop_drivr being initialized.
 *      fontstyle_block_ptr is the current fontstyle_block instance.
 *      p_buffer_set should be NULL and any memory formerly pointed to freed.
 *      count contains the total length of the pre-P-buffer data.
 *
 *  Parameter modified:
 *      if the function succeeds, then *p_buffer_set will point to the
 *          current set of P-buffers
 *      if the function fails, then *p_buffer_set will be NULL
 *
 * Returns:
 *      in_driver, possibly relocated, on success.
 *      NULL on failure.
 *
 * Note:
 *      resize_cop_driver() calls mem_realloc(), which will call exit() if
 *          the reallocation fails.
 *      get_code_blocks() calls mem_alloc(), which will call exit() if
 *          the allocation fails.
 *      NULL is returned for file errors and for formatting errors. It is
 *          suggested that a file error be treated as a format error since
 *          a file error indicated premature termination of the file and
 *          the format must be entirely present for there to be no error.
*/

static cop_driver * parse_font_style( FILE * in_file, cop_driver * in_driver, \
                                       fontstyle_block * fontstyle_block_ptr, \
                                       p_buffer * * p_buffer_set, \
                                       uint8_t * * current, uint8_t count )
{

    char *          string_ptr              = NULL;
    code_block *    cop_codeblocks          = NULL;
    code_text *     code_text_ptr           = NULL;
    int             i;
    line_proc *     line_proc_ptr           = NULL;
    ptrdiff_t       fontstyle_block_offset;
    uint8_t *       text_ptr                = NULL;
    uint16_t        count16;

    /* Get the number of line passes, which can be 0. */

    fread( &fontstyle_block_ptr->line_passes, \
           sizeof( fontstyle_block_ptr->line_passes ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        mem_free( in_driver );
        in_driver = NULL;
        return( in_driver );
    }
    count -= sizeof( fontstyle_block_ptr->line_passes );

    /* Get the unknown_count, and verify that it is 1. */

    fread( &count16, sizeof( count16 ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        mem_free( in_driver );
        in_driver = NULL;
        return( in_driver );
    }

    if( count16 != 0x0001 ) {
        mem_free( in_driver );
        in_driver = NULL;
        return( in_driver );
    }
    count -= sizeof( count16 );

    /* Get the two nulls, and verify they contain the value 0. */

    fread( &count16, sizeof( count16 ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        mem_free( in_driver );
        in_driver = NULL;
        return( in_driver );
    }

    if( count16 != 0x0000 ) {
        mem_free( in_driver );
        in_driver = NULL;
        return( in_driver );
    }
    count -= sizeof( count16 );

    /* count should contain the length of type, including the terminal null. */

    if( count == 0 ) {
        mem_free( in_driver );
        in_driver = NULL;
        return( in_driver );
    }

    if( count > 1 ) {

        /* Add the space for the type. */

        if( in_driver->allocated_size < (in_driver->next_offset + count) ) {
            fontstyle_block_offset = ((uint8_t *) fontstyle_block_ptr - \
                                                        (uint8_t *) in_driver);
            in_driver = resize_cop_driver( in_driver, count );
            fontstyle_block_ptr = (fontstyle_block *) ((uint8_t *) in_driver + \
                                                        fontstyle_block_offset);
        }

        string_ptr = (char *) in_driver + in_driver->next_offset;

        fontstyle_block_ptr->type = (char *) in_driver->next_offset;
        in_driver->next_offset += count;

        /* Acquire the type. */
        
        fread( string_ptr, count, 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            mem_free( in_driver );
            in_driver = NULL;
            return( in_driver );
        }
    } else {
        fseek( in_file, 1, SEEK_CUR );
        fontstyle_block_ptr->type = NULL;
    }

    /* Done here so lineprocs can be used as an array. */

    if( fontstyle_block_ptr->line_passes == 0 ) {
        fontstyle_block_ptr->lineprocs = NULL;
    } else {

        /* Add the space for the line_proc struct instances. */

        if( in_driver->allocated_size < (in_driver->next_offset + \
                (fontstyle_block_ptr->line_passes * sizeof( line_proc )) ) ) {
            fontstyle_block_offset = ((uint8_t *) fontstyle_block_ptr - \
                                                        (uint8_t *) in_driver);
            in_driver = resize_cop_driver( in_driver, \
                    fontstyle_block_ptr->line_passes * sizeof( line_proc ) );
            fontstyle_block_ptr = (fontstyle_block *) ((uint8_t *) in_driver + \
                                                        fontstyle_block_offset);
        }
        line_proc_ptr = (line_proc *) ((uint8_t *) in_driver + \
                                                        in_driver->next_offset);

        fontstyle_block_ptr->lineprocs = (line_proc *) in_driver->next_offset;
        in_driver->next_offset += fontstyle_block_ptr->line_passes * \
                                                            sizeof( line_proc );

        /* Set the line_proc struct instance pointers to null. */

        for( i = 0; i < fontstyle_block_ptr->line_passes; i++ ) {
            line_proc_ptr[i].startvalue = NULL;
            line_proc_ptr[i].firstword = NULL;
            line_proc_ptr[i].startword = NULL;
            line_proc_ptr[i].endword = NULL;
            line_proc_ptr[i].endvalue = NULL;
        }
    }

    /* Now get the P-buffers. */
        
    *p_buffer_set = get_p_buffer( in_file );
    if( *p_buffer_set == NULL) {
        mem_free( in_driver );
        in_driver = NULL;
        return( in_driver );
    }

    *current = (*p_buffer_set)->buffer;

    /* Skip the flags. */

    *current += 21;

    /* Get the number of CodeBlocks, which can be 0. */

    memcpy_s( &count16, sizeof( count16 ), *current, sizeof( count16 ) );
    *current += sizeof( count16 );
    
    /* If the count is 0, we are done: the P-buffer is empty. */

    if( count16 == 0 ) return( in_driver );

    /* Get the CodeBlocks. */

    cop_codeblocks = get_code_blocks( current, count16, (*p_buffer_set)->buffer );

    /* Process the CodeBlocks into the fontstyle_block instance. */

    for( i = 0; i < count16; i++ ) {

        /* Trap zero-length CodeBlocks. */
        
        if( cop_codeblocks[i].count == 0 ) continue;
        
        switch( cop_codeblocks[i].designator ) {
        case 0x04 :

            /* The :FONTSTYLE :ENDVALUE block. */

            /* Add the code_text struct. */

            if( in_driver->allocated_size < (in_driver->next_offset + \
                                                        sizeof( code_text )) ) {
                fontstyle_block_offset = ((uint8_t *) fontstyle_block_ptr - \
                                                        (uint8_t *) in_driver);
                in_driver = resize_cop_driver( in_driver, sizeof( code_text ) );
                fontstyle_block_ptr = (fontstyle_block *) ((uint8_t *) in_driver + \
                                                        fontstyle_block_offset);
                line_proc_ptr = (line_proc *) ((uint8_t *) in_driver + \
                                        (size_t) fontstyle_block_ptr->lineprocs);
            }
            code_text_ptr = (code_text *) ((uint8_t *) in_driver + \
                                                        in_driver->next_offset);

            fontstyle_block_ptr->endvalue = (code_text *) in_driver->next_offset;
            in_driver->next_offset += sizeof( code_text );

            /* Initialize the code_text struct. */

            code_text_ptr->count = cop_codeblocks[i].count;

            if( in_driver->allocated_size < (in_driver->next_offset + \
                                                    code_text_ptr->count) ) {
                fontstyle_block_offset = ((uint8_t *) fontstyle_block_ptr - \
                                                        (uint8_t *) in_driver);
                in_driver = resize_cop_driver( in_driver, code_text_ptr->count );
                fontstyle_block_ptr = (fontstyle_block *) ((uint8_t *) in_driver + \
                                                        fontstyle_block_offset);
                line_proc_ptr = (line_proc *) ((uint8_t *) in_driver + \
                                        (size_t) fontstyle_block_ptr->lineprocs);
                code_text_ptr = (code_text *) ((uint8_t *) in_driver + \
                                        (size_t) fontstyle_block_ptr->endvalue);
            }
            text_ptr = (uint8_t *) in_driver + in_driver->next_offset;

            memcpy_s( text_ptr, code_text_ptr->count, cop_codeblocks[i].text, \
                                                        code_text_ptr->count );
            code_text_ptr->text = (uint8_t *) in_driver->next_offset;
            in_driver->next_offset += code_text_ptr->count;

            break;
        case 0x05 :

            /* The :FONTSTYLE :STARTVALUE block. */

            /* Add the code_text struct. */

            if( in_driver->allocated_size < (in_driver->next_offset + \
                                                        sizeof( code_text )) ) {
                fontstyle_block_offset = ((uint8_t *) fontstyle_block_ptr - \
                                                        (uint8_t *) in_driver);
                in_driver = resize_cop_driver( in_driver, sizeof( code_text ) );
                fontstyle_block_ptr = (fontstyle_block *) ((uint8_t *) in_driver + \
                                                        fontstyle_block_offset);
                line_proc_ptr = (line_proc *) ((uint8_t *) in_driver + \
                                        (size_t) fontstyle_block_ptr->lineprocs);
            }
            code_text_ptr = (code_text *) ((uint8_t *) in_driver + \
                                                        in_driver->next_offset);

            fontstyle_block_ptr->startvalue = (code_text *) in_driver->next_offset;
            in_driver->next_offset += sizeof( code_text );

            /* Initialize the code_text struct. */

            code_text_ptr->count = cop_codeblocks[i].count;

            if( in_driver->allocated_size < (in_driver->next_offset + \
                                                        code_text_ptr->count) ) {
                fontstyle_block_offset = ((uint8_t *) fontstyle_block_ptr - \
                                                        (uint8_t *) in_driver);
                in_driver = resize_cop_driver( in_driver, code_text_ptr->count );
                fontstyle_block_ptr = (fontstyle_block *) ((uint8_t *) in_driver \
                                                        + fontstyle_block_offset);
                line_proc_ptr = (line_proc *) ((uint8_t *) in_driver + \
                                        (size_t) fontstyle_block_ptr->lineprocs);
                code_text_ptr = (code_text *) ((uint8_t *) in_driver + \
                                        (size_t) fontstyle_block_ptr->startvalue);
            }
            text_ptr = (uint8_t *) in_driver + in_driver->next_offset;

            memcpy_s( text_ptr, code_text_ptr->count, \
                            cop_codeblocks[i].text, code_text_ptr->count );
            code_text_ptr->text = (uint8_t *) in_driver->next_offset;
            in_driver->next_offset += code_text_ptr->count;

            break;
        case 0x08 :

            /* A :FONTSTYLE :LINEPROC :ENDVALUE block. */

            /* Ensure that the line pass is within range. */

            if( cop_codeblocks[i].line_pass > fontstyle_block_ptr->line_passes ) {
                mem_free( *p_buffer_set );
                *p_buffer_set = NULL;
                mem_free( in_driver );
                in_driver = NULL;
                break;
            }

            /* Add the code_text struct. */

            if( in_driver->allocated_size < (in_driver->next_offset + \
                                                        sizeof( code_text )) ) {
                fontstyle_block_offset = ((uint8_t *) fontstyle_block_ptr - \
                                                        (uint8_t *) in_driver);
                in_driver = resize_cop_driver( in_driver, sizeof( code_text ) );
                fontstyle_block_ptr = (fontstyle_block *) ((uint8_t *) in_driver \
                                                        + fontstyle_block_offset);
                line_proc_ptr = (line_proc *) ((uint8_t *) in_driver + \
                                        (size_t) fontstyle_block_ptr->lineprocs);
            }
            code_text_ptr = (code_text *) ((uint8_t *) in_driver + \
                                                        in_driver->next_offset);

            line_proc_ptr[cop_codeblocks[i].line_pass - 1].endvalue = \
                                            (code_text *) in_driver->next_offset;
            in_driver->next_offset += sizeof( code_text );

            /* Initialize the code_text struct. */

            code_text_ptr->count = cop_codeblocks[i].count;

            if( in_driver->allocated_size < (in_driver->next_offset + \
                                                        code_text_ptr->count) ) {
                fontstyle_block_offset = ((uint8_t *) fontstyle_block_ptr - \
                                                        (uint8_t *) in_driver);
                in_driver = resize_cop_driver( in_driver, code_text_ptr->count );
                fontstyle_block_ptr = (fontstyle_block *) ((uint8_t *) in_driver \
                                                        + fontstyle_block_offset);
                line_proc_ptr = (line_proc *) ((uint8_t *) in_driver + \
                                        (size_t) fontstyle_block_ptr->lineprocs);
                code_text_ptr = (code_text *) ((uint8_t *) in_driver + \
                    (size_t) line_proc_ptr[cop_codeblocks[i].line_pass - 1].endvalue);
            }
            text_ptr = (uint8_t *) in_driver + in_driver->next_offset;

            memcpy_s( text_ptr, code_text_ptr->count, \
                            cop_codeblocks[i].text, code_text_ptr->count );
            code_text_ptr->text = (uint8_t *) in_driver->next_offset;
            in_driver->next_offset += code_text_ptr->count;

            break;
        case 0x09 :

            /* A :FONTSTYLE :LINEPROC :STARTVALUE block. */

            /* Ensure that the line pass is within range. */

            if( cop_codeblocks[i].line_pass > fontstyle_block_ptr->line_passes ) {
                mem_free( *p_buffer_set );
                *p_buffer_set = NULL;
                mem_free( in_driver );
                in_driver = NULL;
                break;
            }

            /* Add the code_text struct. */

            if( in_driver->allocated_size < (in_driver->next_offset + \
                                                        sizeof( code_text )) ) {
                fontstyle_block_offset = ((uint8_t *) fontstyle_block_ptr - \
                                                        (uint8_t *) in_driver);
                in_driver = resize_cop_driver( in_driver, sizeof( code_text ) );
                fontstyle_block_ptr = (fontstyle_block *) ((uint8_t *) in_driver \
                                                        + fontstyle_block_offset);
                line_proc_ptr = (line_proc *) ((uint8_t *) in_driver + \
                                        (size_t) fontstyle_block_ptr->lineprocs);
            }
            code_text_ptr = (code_text *) ((uint8_t *) in_driver + \
                                                        in_driver->next_offset);

            line_proc_ptr[cop_codeblocks[i].line_pass - 1].startvalue = \
                                            (code_text *) in_driver->next_offset;
            in_driver->next_offset += sizeof( code_text );

            /* Initialize the code_text struct. */

            code_text_ptr->count = cop_codeblocks[i].count;

            if( in_driver->allocated_size < (in_driver->next_offset + \
                                                        code_text_ptr->count) ) {
                fontstyle_block_offset = ((uint8_t *) fontstyle_block_ptr - \
                                                        (uint8_t *) in_driver);
                in_driver = resize_cop_driver( in_driver, code_text_ptr->count );
                fontstyle_block_ptr = (fontstyle_block *) ((uint8_t *) in_driver \
                                                        + fontstyle_block_offset);
                line_proc_ptr = (line_proc *) ((uint8_t *) in_driver + \
                                        (size_t) fontstyle_block_ptr->lineprocs);
                code_text_ptr = (code_text *) ((uint8_t *) in_driver + \
                    (size_t) line_proc_ptr[cop_codeblocks[i].line_pass - 1].startvalue);
            }
            text_ptr = (uint8_t *) in_driver + in_driver->next_offset;

            memcpy_s( text_ptr, code_text_ptr->count, \
                            cop_codeblocks[i].text, code_text_ptr->count );
            code_text_ptr->text = (uint8_t *) in_driver->next_offset;
            in_driver->next_offset += code_text_ptr->count;

            break;
        case 0x28 :

            /* A :FONTSTYLE :LINEPROC :ENDWORD block. */

            /* Ensure that the line pass is within range. */

            if( cop_codeblocks[i].line_pass > fontstyle_block_ptr->line_passes ) {
                mem_free( *p_buffer_set );
                *p_buffer_set = NULL;
                mem_free( in_driver );
                in_driver = NULL;
                break;
            }

            /* Add the code_text struct. */

            if( in_driver->allocated_size < (in_driver->next_offset + \
                                                        sizeof( code_text )) ) {
                fontstyle_block_offset = ((uint8_t *) fontstyle_block_ptr - \
                                                        (uint8_t *) in_driver);
                in_driver = resize_cop_driver( in_driver, sizeof( code_text ) );
                fontstyle_block_ptr = (fontstyle_block *) ((uint8_t *) in_driver \
                                                        + fontstyle_block_offset);
                line_proc_ptr = (line_proc *) ((uint8_t *) in_driver + \
                                        (size_t) fontstyle_block_ptr->lineprocs);
            }
            code_text_ptr = (code_text *) ((uint8_t *) in_driver + \
                                                        in_driver->next_offset);

            line_proc_ptr[cop_codeblocks[i].line_pass - 1].endword = (code_text *) \
                                                        in_driver->next_offset;
            in_driver->next_offset += sizeof( code_text );

            /* Initialize the code_text struct. */

            code_text_ptr->count = cop_codeblocks[i].count;

            if( in_driver->allocated_size < (in_driver->next_offset + \
                                                    code_text_ptr->count) ) {
                fontstyle_block_offset = ((uint8_t *) fontstyle_block_ptr - \
                                                        (uint8_t *) in_driver);
                in_driver = resize_cop_driver( in_driver, code_text_ptr->count );
                fontstyle_block_ptr = (fontstyle_block *) ((uint8_t *) in_driver \
                                                        + fontstyle_block_offset);
                line_proc_ptr = (line_proc *) ((uint8_t *) in_driver + \
                                        (size_t) fontstyle_block_ptr->lineprocs);
                code_text_ptr = (code_text *) ((uint8_t *) in_driver + \
                    (size_t) line_proc_ptr[cop_codeblocks[i].line_pass - 1].endword);
            }
            text_ptr = (uint8_t *) in_driver + in_driver->next_offset;

            memcpy_s( text_ptr, code_text_ptr->count, \
                            cop_codeblocks[i].text, code_text_ptr->count );
            code_text_ptr->text = (uint8_t *) in_driver->next_offset;
            in_driver->next_offset += code_text_ptr->count;

            break;
        case 0x29 :

            /* A :FONTSTYLE :LINEPROC :STARTWORD block. */

            /* Ensure that the line pass is within range. */

            if( cop_codeblocks[i].line_pass > fontstyle_block_ptr->line_passes ) {
                mem_free( *p_buffer_set );
                *p_buffer_set = NULL;
                mem_free( in_driver );
                in_driver = NULL;
                break;
            }

            /* Add the code_text struct. */

            if( in_driver->allocated_size < (in_driver->next_offset + \
                                                        sizeof( code_text )) ) {
                fontstyle_block_offset = ((uint8_t *) fontstyle_block_ptr - \
                                                        (uint8_t *) in_driver);
                in_driver = resize_cop_driver( in_driver, sizeof( code_text ) );
                fontstyle_block_ptr = (fontstyle_block *) ((uint8_t *) in_driver \
                                                        + fontstyle_block_offset);
                line_proc_ptr = (line_proc *) ((uint8_t *) in_driver + \
                                        (size_t) fontstyle_block_ptr->lineprocs);
            }
            code_text_ptr = (code_text *) ((uint8_t *) in_driver + \
                                                        in_driver->next_offset);

            line_proc_ptr[cop_codeblocks[i].line_pass - 1].startword = \
                                            (code_text *) in_driver->next_offset;
            in_driver->next_offset += sizeof( code_text );

            /* Initialize the code_text struct. */

            code_text_ptr->count = cop_codeblocks[i].count;

            if( in_driver->allocated_size < (in_driver->next_offset + \
                                                        code_text_ptr->count) ) {
                fontstyle_block_offset = ((uint8_t *) fontstyle_block_ptr - \
                                                        (uint8_t *) in_driver);
                in_driver = resize_cop_driver( in_driver, code_text_ptr->count );
                fontstyle_block_ptr = (fontstyle_block *) ((uint8_t *) in_driver \
                                                        + fontstyle_block_offset);
                line_proc_ptr = (line_proc *) ((uint8_t *) in_driver + \
                                        (size_t) fontstyle_block_ptr->lineprocs);
                code_text_ptr = (code_text *) ((uint8_t *) in_driver + \
                    (size_t) line_proc_ptr[cop_codeblocks[i].line_pass - 1].startword);
            }
            text_ptr = (uint8_t *) in_driver + in_driver->next_offset;

            memcpy_s( text_ptr, code_text_ptr->count, \
                            cop_codeblocks[i].text, code_text_ptr->count );
            code_text_ptr->text = (uint8_t *) in_driver->next_offset;
            in_driver->next_offset += code_text_ptr->count;

            break;
        case 0x49 :

            /* A :FONTSTYLE :LINEPROC :FIRSTWORD block. */

            /* Ensure that the line pass is within range. */

            if( cop_codeblocks[i].line_pass > fontstyle_block_ptr->line_passes ) {
                mem_free( *p_buffer_set );
                *p_buffer_set = NULL;
                mem_free( in_driver );
                in_driver = NULL;
                break;
            }

            /* Add the code_text struct. */

            if( in_driver->allocated_size < (in_driver->next_offset + \
                                                        sizeof( code_text )) ) {
                fontstyle_block_offset = ((uint8_t *) fontstyle_block_ptr - \
                                                        (uint8_t *) in_driver);
                in_driver = resize_cop_driver( in_driver, sizeof( code_text ) );
                fontstyle_block_ptr = (fontstyle_block *) ((uint8_t *) in_driver \
                                                        + fontstyle_block_offset);
                line_proc_ptr = (line_proc *) ((uint8_t *) in_driver + \
                                        (size_t) fontstyle_block_ptr->lineprocs);
            }
            code_text_ptr = (code_text *) ((uint8_t *) in_driver + \
                                                        in_driver->next_offset);

            line_proc_ptr[cop_codeblocks[i].line_pass - 1].firstword = \
                                            (code_text *) in_driver->next_offset;
            in_driver->next_offset += sizeof( code_text );

            /* Initialize the code_text struct. */

            code_text_ptr->count = cop_codeblocks[i].count;

            if( in_driver->allocated_size < (in_driver->next_offset + \
                                                        code_text_ptr->count) ) {
                fontstyle_block_offset = ((uint8_t *) fontstyle_block_ptr - \
                                                        (uint8_t *) in_driver);
                in_driver = resize_cop_driver( in_driver, code_text_ptr->count );
                fontstyle_block_ptr = (fontstyle_block *) ((uint8_t *) in_driver \
                                                        + fontstyle_block_offset);
                line_proc_ptr = (line_proc *) ((uint8_t *) in_driver + \
                                        (size_t) fontstyle_block_ptr->lineprocs);
                code_text_ptr = (code_text *) ((uint8_t *) in_driver + \
                    (size_t) line_proc_ptr[cop_codeblocks[i].line_pass - 1].firstword);
            }
            text_ptr = (uint8_t *) in_driver + in_driver->next_offset;

            memcpy_s( text_ptr, code_text_ptr->count, cop_codeblocks[i].text, \
                                                            code_text_ptr->count );
            code_text_ptr->text = (uint8_t *) in_driver->next_offset;
            in_driver->next_offset += code_text_ptr->count;

            break;
        default :
            mem_free( *p_buffer_set );
            *p_buffer_set = NULL;
            mem_free( in_driver );
            in_driver = NULL;
        }
    }

    mem_free(cop_codeblocks);
    cop_codeblocks = NULL;

    return( in_driver );
}

/* Function parse_init_block().
 * Processes a single InitBlock. This may be called up to two times when
 * parsing a .COP file encoding a :DRIVER block. 
 *
 * Parameters:
 *      in_driver contains the cop_driver being initialized.
 *      *current contains the current position in a p_buffer.buffer.
 *      base points to the first byte of the underlying P-buffer
 *
 * Parameter modified:
 *      if the function succeeds, then *current will point to the first junk
 *          byte following the last CodeBlock for this init_block.
 *      if the function fails, the value of *current should be regarded as
 *          invalid.
 *
 *  Returns:
 *      in_driver, possibly relocated, on success.
 *      NULL on failure, which indicates an incorrectly formatted file.
 *
 * Notes:
 *      resize_cop_driver() calls mem_realloc(), which will call exit() if
 *          the reallocation fails.
 *      get_code_blocks() calls mem_alloc(), which will call exit() if
 *          the allocation fails.
 */
static cop_driver * parse_init_block( cop_driver * in_driver, \
                                       uint8_t * * current, uint8_t * base )
{

    code_block *    cop_codeblocks  = NULL;
    init_block *    init_block_ptr  = NULL;
    init_text *     init_text_ptr   = NULL;
    int             i;
    uint8_t         designator;    
    uint8_t *       text_ptr        = NULL;
    uint16_t        count;

    /* Get the designator. */

    memcpy_s( &designator, 1, *current, 1 );
    (*current)++;

    /* Process the InitBlock. */

    switch( designator ) {
    case 0x01 :
        memcpy_s( &count, sizeof( count ), *current, sizeof( count ) );
        *current += sizeof( count );
        if( count == 0 ) {
            mem_free( in_driver );
            in_driver = NULL;
            return( in_driver );
        }

        /* Add the init_block struct itself. */

        if( in_driver->allocated_size < (in_driver->next_offset + \
                                                    sizeof( init_block )) ) {
            in_driver = resize_cop_driver( in_driver, sizeof( init_block ) );
        }
        init_block_ptr = (init_block *) ((uint8_t *) in_driver + \
                                                        in_driver->next_offset);

        in_driver->inits.start = (init_block *) in_driver->next_offset;
        in_driver->next_offset += sizeof( init_block );

        /* Get the CodeBlocks. */
        
        cop_codeblocks = get_code_blocks( current, count, base );
        
        /* Add the init_text struct instances. */
        
        if( in_driver->allocated_size < (in_driver->next_offset + \
                                                count * sizeof( init_text )) ) {
            in_driver = resize_cop_driver( in_driver, \
                                                count * sizeof( init_text ) );
            init_block_ptr = (init_block *) ((uint8_t *) in_driver + \
                                                (size_t) in_driver->inits.start);
        }
        init_text_ptr = (init_text *) ((uint8_t *) in_driver + \
                                                        in_driver->next_offset);

        init_block_ptr->count = count;
        init_block_ptr->codeblock = (init_text *) in_driver->next_offset;
        in_driver->next_offset += count * sizeof( init_text );

        /* Initialize the init_text struct instances. */

        for(i = 0; i < init_block_ptr->count; i++ ) {

            switch( cop_codeblocks[i].designator ) {
            case 0x00:
                init_text_ptr[i].is_fontvalue = false;
                break;
            case 0x02:
                init_text_ptr[i].is_fontvalue = true;
                break;
            default:
                mem_free( cop_codeblocks );
                cop_codeblocks = NULL;
                mem_free( in_driver );
                in_driver = NULL;
                return( in_driver );
            }

            init_text_ptr[i].count = cop_codeblocks[i].count;

            if( in_driver->allocated_size < (in_driver->next_offset + \
                                                    init_text_ptr[i].count) ) {
                in_driver = resize_cop_driver( in_driver, \
                                                    init_text_ptr[i].count );
                init_block_ptr = (init_block *) ((uint8_t *) in_driver + \
                                                (size_t) in_driver->inits.start);
                init_text_ptr = (init_text *) ((uint8_t *) in_driver + \
                                            (size_t) init_block_ptr->codeblock);
            }
            text_ptr = (uint8_t *) in_driver + in_driver->next_offset;

            memcpy_s( text_ptr, init_text_ptr[i].count, \
                            cop_codeblocks[i].text, init_text_ptr[i].count );
            init_text_ptr[i].text = (uint8_t *) in_driver->next_offset;
            in_driver->next_offset += init_text_ptr[i].count;
        }

        mem_free( cop_codeblocks );
        cop_codeblocks = NULL;

        break;
    case 0x02 :
        memcpy_s( &count, sizeof( count ), *current, sizeof( count ) );
        *current += sizeof( count );
        if( count == 0 ) {
            mem_free( in_driver );
            in_driver = NULL;
            return( in_driver );
        }

        /* Add the init_block struct itself. */

        if( in_driver->allocated_size < (in_driver->next_offset + \
                                                        sizeof( init_block )) ) {
            in_driver = resize_cop_driver( in_driver, sizeof( init_block ) );
        }
        init_block_ptr = (init_block *) ((uint8_t *) in_driver + \
                                                        in_driver->next_offset);

        in_driver->inits.document = (init_block *) in_driver->next_offset;
        in_driver->next_offset += sizeof( init_block );

        /* Get the CodeBlocks. */
        
        cop_codeblocks = get_code_blocks( current, count, base );
        
        /* Add the init_text struct instances. */
        
        if( in_driver->allocated_size < (in_driver->next_offset + \
                                            count * sizeof( init_text )) ) {
            in_driver = resize_cop_driver( in_driver, count * sizeof( init_text ) );
            init_block_ptr = (init_block *) ((uint8_t *) in_driver + \
                                            (size_t) in_driver->inits.document);
        }

        init_text_ptr = (init_text *) ((uint8_t *) in_driver + \
                                                        in_driver->next_offset);

        init_block_ptr->count = count;
        init_block_ptr->codeblock = (init_text *) in_driver->next_offset;
        in_driver->next_offset += count * sizeof( init_text );

        /* Initialize the init_text struct instances. */

        for(i = 0; i < init_block_ptr->count; i++ ) {

            switch( cop_codeblocks[i].designator ) {
            case 0x00:
                init_text_ptr[i].is_fontvalue = false;
                break;
            case 0x02:
                init_text_ptr[i].is_fontvalue = true;
                break;
            default:
                mem_free( cop_codeblocks );
                cop_codeblocks = NULL;
                mem_free( in_driver );
                in_driver = NULL;
                return( in_driver );
            }

            init_text_ptr[i].count = cop_codeblocks[i].count;

            if( in_driver->allocated_size < (in_driver->next_offset + \
                                                    init_text_ptr[i].count) ) {
                in_driver = resize_cop_driver( in_driver, \
                                                    init_text_ptr[i].count );
                init_block_ptr = (init_block *) ((uint8_t *) in_driver + \
                                            (size_t) in_driver->inits.document);
                init_text_ptr = (init_text *) ((uint8_t *) in_driver + \
                                            (size_t) init_block_ptr->codeblock);
            }
            text_ptr = (uint8_t *) in_driver + in_driver->next_offset;

            memcpy_s( text_ptr, init_text_ptr[i].count, \
                            cop_codeblocks[i].text, init_text_ptr[i].count );
            init_text_ptr[i].text = (uint8_t *) in_driver->next_offset;
            in_driver->next_offset += init_text_ptr[i].count;
        }

        mem_free( cop_codeblocks );
        cop_codeblocks = NULL;

        break;
    default :
        mem_free( in_driver );
        in_driver = NULL;
    }    

    return( in_driver );
}

/* Extern function definitions. */

/* Function is_drv_file().
 * Determines whether or not in_file points to the start of a .COP driver
 * file (the first byte after the header).
 *
 * Parameter:
 *      in_file points to the presumed start of a .COP driver file.
 *
 * Returns:
 *      true if this has the correct descriminator.
 *      false otherwise.
 */

bool is_drv_file( FILE * in_file)
{
    char descriminator[3];

    /* Get the descriminator. */

    fread( &descriminator, 3, 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) return( false );

    /* Verify that the descriminator is for a .COP driver file. */

    if( memcmp( descriminator, "DRV", 3 ) ) return( false );
    
    return( true );
}

/* Function parse_driver().
 * Constructs a cop_driver instance from the given input stream.
 *  
 * Parameters:
 *      in_file points to the first byte of a .COP file encoding a :DRIVER
 *          struct after the "DEV" descriminator.
 *
 * Returns:
 *      A pointer to a cop_driver struct containing the data from in_file
 *          on success.
 *      A NULL pointer on failure.
 *
 *  Notes:
 *      mem_alloc() calls exit() if the allocation fails. 
 *      resize_cop_driver() uses mem_realloc(), which calls exit() if the
 *          allocation fails.
 *      get_code_blocks() uses mem_alloc(), which calls exit() if the
 *          allocation fails. 
 *      get_p_buffers() uses mem_alloc(), which calls exit() if the
 *          allocation fails; however, a NULL is returned for a file error; if
 *          a non-NULL value is returned, then p_buffer.buffer is non-NULL
 *          as well.
 *      parse_functions_block() uses mem_alloc(), which calls exit() if the
 *          allocation fails. 
 *      NULL is returned for file errors and for formatting errors. It is
 *          suggested that a file error be treated as a format error since
 *          a file error indicated premature termination of the file and
 *          the format must be entirely present for there to be no error.
 */

cop_driver * parse_driver( FILE * in_file )
{
    /* The cop_driver instance. */
    
    cop_driver *        out_driver              = NULL;

    /* Used to acquire string attributes. */

    uint8_t             length;
    char *              string_ptr              = NULL;

    /* Used to acquire and work with the P-buffers. */

    code_block *        cop_codeblocks          = NULL;
    code_text *         code_text_ptr           = NULL;
    fontstyle_block *   fontstyle_block_ptr     = NULL;
    fontswitch_block *  fontswitch_block_ptr    = NULL;
    functions_block *   cop_functions           = NULL;
    int                 factor;
    newline_block *     newline_block_ptr       = NULL;
    p_buffer *          p_buffer_set            = NULL;
    size_t              span;
    uint8_t *           byte_ptr                = NULL;
    uint8_t *           current                 = NULL;
    uint8_t *           text_ptr                = NULL;
    uint8_t             the_flags[21];

    /* Used for counts and related values. */

    uint8_t             count8;
    uint16_t            count16;

    /* Used in for loops. */

    int                 i;
    int                 j;

    /* Initialize the out_driver. */
        
    out_driver = (cop_driver *) mem_alloc( START_SIZE );

    out_driver->allocated_size = START_SIZE;
    out_driver->next_offset = sizeof( cop_driver );

    /* Note: The various pointers must be entered, initially, as offsets
     * and then converted to pointers before returning because out_driver
     * may be reallocated at any point and that invalidates actual pointers.
     */

    /* Get the rec_spec. */

    fread( &length, sizeof( length ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        mem_free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }

    if( length > 0 ) {

        if( out_driver->allocated_size < (out_driver->next_offset + length) ) {
            out_driver = resize_cop_driver( out_driver, length );
        }

        string_ptr = (char *) out_driver + out_driver->next_offset;

        fread( string_ptr, length, 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            mem_free( out_driver );
            out_driver = NULL;
            return( out_driver );
        }
        out_driver->rec_spec = (char *) out_driver->next_offset;
        out_driver->next_offset += length;
        string_ptr[length] = '\0';
        ++out_driver->next_offset;
    } else {
        out_driver->rec_spec = NULL;
    }

    /* Get the unknown value and verify that it contains 0x04. */

    fread( &count8, sizeof( count8 ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        mem_free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }

    if( count8 != 0x04 ) {
        mem_free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }

    /* Get the fill_char. */

    fread( &out_driver->fill_char, sizeof( out_driver->fill_char ), 1, \
        in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        mem_free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }

    /* Get the x_positive flag. */

    fread( &out_driver->x_positive, sizeof( out_driver->x_positive ), 1, \
        in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        mem_free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }

    /* Get the y_positive flag. */

    fread( &out_driver->y_positive, sizeof( out_driver->y_positive ), 1, \
        in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        mem_free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }

    /* Get the null byte and verify that it is, in fact, null. */

    fread( &count8, sizeof( count8 ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        mem_free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }

    if( count8 != 0x00 ) {
        mem_free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }

    /* Get the first set of P-buffers. */

    p_buffer_set = get_p_buffer( in_file );
    if( p_buffer_set == NULL) {
        mem_free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }

    current = p_buffer_set->buffer;

    /* Parse the InitFuncs. */

    /* Get the number of InitBlocks. */

    memcpy_s( &count16, sizeof( count16 ), current, sizeof( count16 ) );
    current += sizeof( count16 );

    out_driver->inits.start = NULL;
    out_driver->inits.document = NULL;

    switch( count16 ) {
    case 0x0000 :
        break;
    case 0x0001 :
        out_driver = parse_init_block( out_driver, &current, \
                                                        p_buffer_set->buffer );
        if( out_driver == NULL ) {
            mem_free( p_buffer_set );
            p_buffer_set = NULL;
            return( out_driver );
        }
        break;
    case 0x0002 :
        out_driver = parse_init_block( out_driver, &current, \
                                                        p_buffer_set->buffer );
        if( out_driver == NULL ) {
            mem_free( p_buffer_set );
            p_buffer_set = NULL;
            return( out_driver );
        }

        /* Reset to the start of the next P-buffer's data. */

        if( (current - p_buffer_set->buffer) % 80 != 0 ) { 
            factor = (current - p_buffer_set->buffer) / 80;
            factor++;
            current = p_buffer_set->buffer + factor * 80;
        }

        out_driver = parse_init_block( out_driver, &current, \
                                                        p_buffer_set->buffer );
        if( out_driver == NULL ) {
            mem_free( p_buffer_set );
            p_buffer_set = NULL;
            return( out_driver );
        }
        break;
    default:
        mem_free( p_buffer_set );
        p_buffer_set = NULL;
        mem_free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }

    /* Reset to the start of the next P-buffer's data. */

    if( (current - p_buffer_set->buffer) % 80 != 0 ) { 
        factor = (current - p_buffer_set->buffer) / 80;
        factor++;
        current = p_buffer_set->buffer + factor * 80;
    }
    
    /* Parse the FinishFuncs. */

    /* Get the number of FinishBlocks. */

    memcpy_s( &count16, sizeof( count16 ), current, sizeof( count16 ) );
    current += sizeof( count16 );

    out_driver->finishes.end = NULL;
    out_driver->finishes.document = NULL;

    switch( count16 ) {
    case 0x0000 :
        break;
    case 0x0001 :
        out_driver = parse_finish_block( out_driver, &current, \
                                                        p_buffer_set->buffer );
        if( out_driver == NULL ) {
            mem_free( p_buffer_set );
            p_buffer_set = NULL;
            return( out_driver );
        }
        break;
    case 0x0002 :
        out_driver = parse_finish_block( out_driver, &current, \
                                                        p_buffer_set->buffer );
        if( out_driver == NULL ) {
            mem_free( p_buffer_set );
            p_buffer_set = NULL;
            return( out_driver );
        }

        /* Reset to the start of the next P-buffer's data. */

        if( (current - p_buffer_set->buffer) % 80 != 0 ) { 
            factor = (current - p_buffer_set->buffer) / 80;
            factor++;
            current = p_buffer_set->buffer + factor * 80;
        }

        out_driver = parse_finish_block( out_driver, &current, \
                                                        p_buffer_set->buffer );
        if( out_driver == NULL ) {
            mem_free( p_buffer_set );
            p_buffer_set = NULL;
            return( out_driver );
        }
        break;
    default:
        mem_free( p_buffer_set );
        p_buffer_set = NULL;
        mem_free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }
    
    /* Reset to the start of the next P-buffer's data. */

    if( (current - p_buffer_set->buffer) % 80 != 0 ) { 
        factor = (current - p_buffer_set->buffer) / 80;
        factor++;
        current = p_buffer_set->buffer + factor * 80;
    }

    /* Parse the NewlineFuncs. */

    /* Get the number of NewlineBlocks */

    memcpy_s( &out_driver->newlines.count, sizeof( out_driver->newlines.count ), \
                                current, sizeof( out_driver->newlines.count ) );
    current += sizeof( out_driver->newlines.count );

    /* Add the newline_block structs. */

    if( out_driver->allocated_size < (out_driver->next_offset + \
                        out_driver->newlines.count * sizeof( newline_block )) ) {
        out_driver = resize_cop_driver( out_driver, \
                        out_driver->newlines.count * sizeof( newline_block ) );
    }
    newline_block_ptr = (newline_block *) ((uint8_t *) out_driver + \
                                                        out_driver->next_offset);

    out_driver->newlines.newlineblocks = (newline_block *) \
                                                        out_driver->next_offset;
    out_driver->next_offset += out_driver->newlines.count * \
                                                        sizeof( newline_block );

    /* Initialize the newline_block structs. */

    for(i = 0; i < out_driver->newlines.count; i++ ) {

        /* Get the advance for the current NewlineBlock. */

        memcpy_s( &newline_block_ptr[i].advance, \
                                sizeof( newline_block_ptr[i].advance ), current, \
                                sizeof( newline_block_ptr[i].advance ) );
        current += sizeof( newline_block_ptr[i].advance );

        /* Get the number of CodeBlocks, and verify that it is 0x01. */

        memcpy_s( &count16, sizeof( count16 ), current, sizeof( count16 ) );
        current += sizeof( count16 );

        if( count16 != 0x0001 ) {
            mem_free( p_buffer_set );
            p_buffer_set = NULL;
            mem_free( out_driver );
            out_driver = NULL;
            return( out_driver );
        }

        /* Get the CodeBlock. */
        
        cop_codeblocks = get_code_blocks( &current, count16, \
                                                        p_buffer_set->buffer );
        if( cop_codeblocks == NULL ) {
            mem_free( p_buffer_set );
            p_buffer_set = NULL;
            mem_free( out_driver );
            out_driver = NULL;
            return( out_driver );
        }

        if( cop_codeblocks->designator != 0x00 ) {
            mem_free( p_buffer_set );
            p_buffer_set = NULL;
            mem_free( cop_codeblocks );
            cop_codeblocks = NULL;
            mem_free( out_driver );
            out_driver = NULL;
            return( out_driver );
        }

        newline_block_ptr[i].count = cop_codeblocks->count;

        if( out_driver->allocated_size < (out_driver->next_offset + \
                                                newline_block_ptr[i].count) ) {
            out_driver= resize_cop_driver( out_driver, \
                                                newline_block_ptr[i].count );
            newline_block_ptr = (newline_block *) ((uint8_t *) out_driver + \
                                    (size_t) out_driver->newlines.newlineblocks);
        }
        text_ptr = (uint8_t *) out_driver + out_driver->next_offset;

        memcpy_s( text_ptr, newline_block_ptr[i].count, \
                        cop_codeblocks->text, newline_block_ptr[i].count );
        newline_block_ptr[i].text = (uint8_t *) out_driver->next_offset;
        out_driver->next_offset += newline_block_ptr[i].count;

        mem_free(cop_codeblocks);
        cop_codeblocks = NULL;

        /* Reset to the start of the next P-buffer's data. */

        if( (current - p_buffer_set->buffer) % 80 != 0 ) { 
            factor = (current - p_buffer_set->buffer) / 80;
            factor++;
            current = p_buffer_set->buffer + factor * 80;
        }
    }    
        
    /* Reset to the start of the next P-buffer's data. */

    if( (current - p_buffer_set->buffer) % 80 != 0 ) { 
        factor = (current - p_buffer_set->buffer) / 80;
        factor++;
        current = p_buffer_set->buffer + factor * 80;
    }

    /* Skip the unknown (empty) P-buffer. */

    /* Get the count and verify that it is 0. */

    memcpy_s( &count16, sizeof( count16 ), current, sizeof( count16 ) );
    current += sizeof( count16 );

    if( count16 != 0x0000 ) {
        mem_free( p_buffer_set );
        p_buffer_set = NULL;
        mem_free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }

    /* Reset to the start of the next P-buffer's data. */

    if( (current - p_buffer_set->buffer) % 80 != 0 ) { 
        factor = (current - p_buffer_set->buffer) / 80;
        factor++;
        current = p_buffer_set->buffer + factor * 80;
    }

    /* Parse the newpage FunctionsBlock. */

    cop_functions = parse_functions_block( &current, p_buffer_set->buffer );

    /* Verify that the number of CodeBlocks is 1. */

    if( cop_functions->count != 0x0001 ) {
        mem_free( p_buffer_set );
        p_buffer_set = NULL;
        if( cop_functions->code_blocks != NULL ) {
            mem_free( cop_functions->code_blocks );
            cop_functions->code_blocks = NULL;
        }
        mem_free( cop_functions );
        cop_functions = NULL;
        mem_free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }

    out_driver->newpage.count = cop_functions->code_blocks->count;

    if( out_driver->allocated_size < (out_driver->next_offset + \
                                                out_driver->newpage.count) ) {
        out_driver= resize_cop_driver( out_driver, out_driver->newpage.count );
    }
    text_ptr = (uint8_t *) out_driver + out_driver->next_offset;

    memcpy_s( text_ptr, out_driver->newpage.count, \
                cop_functions->code_blocks->text, out_driver->newpage.count );
    out_driver->newpage.text = (uint8_t *) out_driver->next_offset;
    out_driver->next_offset += out_driver->newpage.count;

    mem_free( cop_functions->code_blocks );
    cop_functions->code_blocks = NULL;
    mem_free( cop_functions );
    cop_functions = NULL;

    /* Reset to the start of the next P-buffer's data. */

    if( (current - p_buffer_set->buffer) % 80 != 0 ) { 
        factor = (current - p_buffer_set->buffer) / 80;
        factor++;
        current = p_buffer_set->buffer + factor * 80;
    }

    /* Parse the htab FunctionsBlock. */

    cop_functions = parse_functions_block( &current, p_buffer_set->buffer );

    /* This block is optional: a count of 0 is allowed. */

    switch( cop_functions->count ) {
    case 0x0000 :
        out_driver->htab.count = 0;
        out_driver->htab.text = NULL;
        break;
    case 0x0001 :
        out_driver->htab.count = cop_functions->code_blocks->count;

        if( out_driver->allocated_size < (out_driver->next_offset + \
            out_driver->htab.count) ) {

            out_driver= resize_cop_driver( out_driver, out_driver->htab.count );
        }
        text_ptr = (uint8_t *) out_driver + out_driver->next_offset;

        memcpy_s( text_ptr, out_driver->htab.count, \
            cop_functions->code_blocks->text, out_driver->htab.count );
        out_driver->htab.text = (uint8_t *) out_driver->next_offset;
        out_driver->next_offset += out_driver->htab.count;
        break;
    default :
        mem_free( p_buffer_set );
        p_buffer_set = NULL;
        mem_free( cop_functions->code_blocks );
        cop_functions->code_blocks = NULL;
        mem_free( cop_functions );
        cop_functions = NULL;
        mem_free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }

    if( cop_functions->code_blocks != NULL ) {
        mem_free( cop_functions->code_blocks );
        cop_functions->code_blocks = NULL;
    }
    mem_free( cop_functions );
    cop_functions = NULL;

    /* Reset to the start of the next P-buffer's data. */

    if( (current - p_buffer_set->buffer) % 80 != 0 ) { 
        factor = (current - p_buffer_set->buffer) / 80;
        factor++;
        current = p_buffer_set->buffer + factor * 80;
    }

    /* Parse the FontswitchFuncs. */

    /* This block is optional: a count of 0 is allowed. */

    memcpy_s( &out_driver->fontswitches.count, \
                            sizeof( out_driver->fontswitches.count ), current, \
                            sizeof( out_driver->fontswitches.count ) );
    current += sizeof( out_driver->fontswitches.count );

    if( out_driver->fontswitches.count == 0x0000 ) {
        out_driver->fontswitches.fontswitchblocks = NULL;

        /* Reset to the start of the next P-buffer's data. */

        if( (current - p_buffer_set->buffer) % 80 != 0 ) { 
            factor = (current - p_buffer_set->buffer) / 80;
            factor++;
            current = p_buffer_set->buffer + factor * 80;
        }

    } else {
        
        /* Add the fontswitch_block structs. */

        if( out_driver->allocated_size < (out_driver->next_offset + \
                out_driver->fontswitches.count * sizeof( fontswitch_block )) ) {
            out_driver = resize_cop_driver( out_driver, \
                    out_driver->fontswitches.count * sizeof( fontswitch_block ) );
        }
        fontswitch_block_ptr = (fontswitch_block *) ((uint8_t *) out_driver + \
                                                        out_driver->next_offset);

        out_driver->fontswitches.fontswitchblocks = (fontswitch_block *) \
                                                        out_driver->next_offset;
        out_driver->next_offset += out_driver->fontswitches.count * \
                                                    sizeof( fontswitch_block );

        /* Now get the FontswitchBlock instances. */

        for( i = 0; i < out_driver->fontswitches.count; i++ ) {

            /* The type is a null-terminated character string. */
            
            length = strlen( current );
            if( length > 0 ) {
                length++;
                if( out_driver->allocated_size < (out_driver->next_offset + \
                                                                    length) ) {
                    out_driver = resize_cop_driver( out_driver, \
                                            out_driver->next_offset + length );
                    fontswitch_block_ptr = (fontswitch_block *) ((uint8_t *) \
                out_driver + (size_t) out_driver->fontswitches.fontswitchblocks);
                }

                string_ptr = (char *) out_driver + out_driver->next_offset;
                strcpy_s( string_ptr, length, current );
                current += length;

                fontswitch_block_ptr[i].type = (char *) out_driver->next_offset;
                out_driver->next_offset += length;
            } else {
                current++;
                fontswitch_block_ptr[i].type = NULL;
            }                

            /* Process some of the 21 flags. */

            memcpy_s( &the_flags, sizeof( the_flags ), current, \
                                                        sizeof( the_flags ) );
            current += sizeof( the_flags );

            /* Set do_always to true or false per the Wiki. The device
             * functions in the order checked are: %wgml_header(), %time(),
             * %date(), %font_number(), and %pages().
             */

            fontswitch_block_ptr[i].do_always = false;
            if( the_flags[0] == 0x01 ) fontswitch_block_ptr[i].do_always = true;
            if( the_flags[4] == 0x01 ) fontswitch_block_ptr[i].do_always = true;
            if( the_flags[5] == 0x01 ) fontswitch_block_ptr[i].do_always = true;
            if( the_flags[7] == 0x01 ) fontswitch_block_ptr[i].do_always = true;
            if( the_flags[20] == 0x01 ) fontswitch_block_ptr[i].do_always = true;

            /* Set the _flag members to true or false. */

            fontswitch_block_ptr[i].font_outname1_flag = (the_flags[1] == 0x01);
            fontswitch_block_ptr[i].font_outname2_flag = (the_flags[2] == 0x01);
            fontswitch_block_ptr[i].font_resident_flag = (the_flags[3] == 0x01);
            fontswitch_block_ptr[i].default_width_flag = (the_flags[6] == 0x01);
            fontswitch_block_ptr[i].font_height_flag = (the_flags[16] == 0x01);
            fontswitch_block_ptr[i].font_space_flag = (the_flags[17] == 0x01);
            fontswitch_block_ptr[i].line_height_flag = (the_flags[18] == 0x01);
            fontswitch_block_ptr[i].line_space_flag = (the_flags[19] == 0x01);

            /* Get the number of CodeBlocks; only 1 or 2 is valid. */

            memcpy_s( &count16, sizeof( count16 ), current, sizeof( count16 ) );
            current += sizeof( count16 );

            if( (count16 == 0x00) || (count16 > 0x02) ) {
                mem_free( p_buffer_set );
                p_buffer_set = NULL;
                mem_free( out_driver );
                out_driver = NULL;
                return( out_driver );
            } else {
            
                /* Get the CodeBlock(s). */
        
                cop_codeblocks = get_code_blocks( &current, count16, \
                                                        p_buffer_set->buffer );
                if( cop_codeblocks == NULL ) {
                    mem_free( p_buffer_set );
                    p_buffer_set = NULL;
                    mem_free( out_driver );
                    out_driver = NULL;
                    return( out_driver );
                }

                /* Process the CodeBlocks. */
            
                fontswitch_block_ptr[i].startvalue = NULL;
                fontswitch_block_ptr[i].endvalue = NULL;

                for( j = 0; j < count16; j++ ) {
                    switch( cop_codeblocks[j].designator ) {
                    case 0x04:

                        /* Add the code_text struct for endvalue. */

                        if( out_driver->allocated_size < (out_driver->next_offset \
                                                        + sizeof( code_text )) ) {
                            out_driver= resize_cop_driver( out_driver, \
                                                            sizeof( code_text ) );
                            fontswitch_block_ptr = (fontswitch_block *) \
    ((uint8_t *) out_driver + (size_t) out_driver->fontswitches.fontswitchblocks);
                        }
                        code_text_ptr = (code_text *) ((uint8_t *) out_driver \
                                                    + out_driver->next_offset);

                        fontswitch_block_ptr[i].endvalue = (code_text *) \
                                            ((uint8_t *) out_driver->next_offset);
                        out_driver->next_offset += sizeof( code_text );

                        /* Now get the CodeBlock. */

                        code_text_ptr->count = cop_codeblocks[j].count;
                
                        if( out_driver->allocated_size < (out_driver->next_offset \
                                                        + code_text_ptr->count) ) {
                            out_driver = resize_cop_driver( out_driver, \
                                                        code_text_ptr->count );
                            fontswitch_block_ptr = (fontswitch_block *) \
    ((uint8_t *) out_driver + (size_t) out_driver->fontswitches.fontswitchblocks);
                            code_text_ptr = (code_text *) ((uint8_t *) out_driver \
                                    + (size_t) fontswitch_block_ptr[i].endvalue);
                        }
                        text_ptr = (uint8_t *) out_driver + \
                                                        out_driver->next_offset;

                        memcpy_s( text_ptr, code_text_ptr->count, \
                                    cop_codeblocks[j].text, code_text_ptr->count );
                        code_text_ptr->text = (uint8_t *) \
                                                        out_driver->next_offset;
                        out_driver->next_offset += code_text_ptr->count;

                        break;
                    case 0x05:

                        /* Add the code_text struct for startvalue. */

                        if( out_driver->allocated_size < \
                               (out_driver->next_offset + sizeof( code_text )) ) {
                            out_driver= resize_cop_driver( out_driver, \
                                                        sizeof( code_text ) );
                            fontswitch_block_ptr = (fontswitch_block *) \
    ((uint8_t *) out_driver + (size_t) out_driver->fontswitches.fontswitchblocks);
                        }
                        code_text_ptr = (code_text *) ((uint8_t *) \
                                            out_driver + out_driver->next_offset);

                        fontswitch_block_ptr[i].startvalue = (code_text *) \
                                            ((uint8_t *) out_driver->next_offset);
                        out_driver->next_offset += sizeof( code_text );

                        /* Now get the CodeBlock. */

                        code_text_ptr->count = cop_codeblocks[j].count;
                
                        if( out_driver->allocated_size < \
                               (out_driver->next_offset + code_text_ptr->count) ) {
                            out_driver = resize_cop_driver( out_driver, \
                                                        code_text_ptr->count );
                            fontswitch_block_ptr = (fontswitch_block *) \
    ((uint8_t *) out_driver + (size_t) out_driver->fontswitches.fontswitchblocks);
                            code_text_ptr = (code_text *) ((uint8_t *) out_driver \
                                + (size_t) fontswitch_block_ptr[i].startvalue);
                        }
                        text_ptr = (uint8_t *) out_driver + \
                                                        out_driver->next_offset;

                        memcpy_s( text_ptr, code_text_ptr->count, \
                                    cop_codeblocks[j].text, code_text_ptr->count );
                        code_text_ptr->text = (uint8_t *) \
                                                        out_driver->next_offset;
                        out_driver->next_offset += code_text_ptr->count;

                        break;
                    default:
                        mem_free( p_buffer_set );
                        p_buffer_set = NULL;
                        mem_free( cop_codeblocks );
                        cop_codeblocks = NULL;
                        mem_free( out_driver );
                        out_driver = NULL;
                        return( out_driver );
                    }
                }
            }

            mem_free(cop_codeblocks);
            cop_codeblocks = NULL;

            /* Reset to the start of the next P-buffer's data. */

            if( (current - p_buffer_set->buffer) % 80 != 0 ) { 
                factor = (current - p_buffer_set->buffer) / 80;
                factor++;
                current = p_buffer_set->buffer + factor * 80;
            }
        }
    }

    /* Verify that current is pointing to the end of the p_buffer_set. */

    factor = (current - p_buffer_set->buffer) / 80;
    if( factor * 80 < p_buffer_set->count ) {

        /* Rewind the file to the count byte of the FontstyleGroup. */
        /* The number of false P-buffers must be added to the span. */

        span = (p_buffer_set->count - (factor * 80));
        fseek( in_file, -1 * (span + span / 80), SEEK_CUR );
        if( ferror( in_file ) || feof( in_file ) ) {
            mem_free( p_buffer_set );
            p_buffer_set = NULL;
            mem_free( out_driver );
            out_driver = NULL;
            return( out_driver );
        }
    } 

    mem_free( p_buffer_set );
    p_buffer_set = NULL;
    
    /* Parse the FontstyleGroup. */

    /* Get the data_count and ensure it is not 0. */
    
    fread( &count8, sizeof( count8 ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        mem_free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }

    if( count8 == 0x00 ) {
        mem_free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }

    /* Get the fontstyle_count and ensure it is not 0. */

    fread( &out_driver->fontstyles.count, sizeof( out_driver->fontstyles.count ), \
                                                                    1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        mem_free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }

    if( out_driver->fontstyles.count == 0x00 ) {
        mem_free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }
    count8 -= sizeof( out_driver->fontstyles.count );

    /* Add the fontstyle_block struct instances. */
    
    if( out_driver->allocated_size < (out_driver->next_offset + \
                    out_driver->fontstyles.count * sizeof( fontstyle_block )) ) {
        out_driver = resize_cop_driver( out_driver, \
                    out_driver->fontstyles.count * sizeof( fontstyle_block ) );
    }
    fontstyle_block_ptr = (fontstyle_block *) ((uint8_t *) out_driver + \
                                                        out_driver->next_offset);

    out_driver->fontstyles.fontstyleblocks = (fontstyle_block *) \
                                                        out_driver->next_offset;
    out_driver->next_offset += out_driver->fontstyles.count * \
                                                        sizeof( fontstyle_block );

    /* Initialize the fontstyle_block struct instances. */

    /* Set the pointers in the fontstyle_block struct instances to NULL. */

    for( i = 0; i < out_driver->fontstyles.count; i++ ) {
        fontstyle_block_ptr[i].startvalue = NULL;
        fontstyle_block_ptr[i].endvalue = NULL;
        fontstyle_block_ptr[i].lineprocs = NULL;
    }

    /* Note: see the Wiki for the file structure. It is a little odd. */

    /* The initial ShortFontstyleBlock must be processed separately. */
    
    out_driver = parse_font_style( in_file, out_driver, fontstyle_block_ptr, \
                                            &p_buffer_set, &current, count8 );
    if( out_driver == NULL ) return( out_driver );
    fontstyle_block_ptr = (fontstyle_block *) ((uint8_t *) out_driver + \
                                (size_t) out_driver->fontstyles.fontstyleblocks);


    /* The FontstyleBlocks, if any, can be done in a loop. */

    for( i = 1; i < out_driver->fontstyles.count; i++ ) {

        /* Locate the start of the "next" P-buffer & verify that it is not
         * present -- that, that the count byte of the next FontstyleBlock
         * was not 80. */

        factor = (current - p_buffer_set->buffer) / 80;
        factor++;
        if( factor * 80 < p_buffer_set->count ) {

            /* Rewind the file to the count byte of the FontstyleBlock. */
            /* The number of false P-buffers must be added to the span. */

            span = (p_buffer_set->count - (factor * 80));
            fseek( in_file, -1 * (span + span / 80), SEEK_CUR );
            if( ferror( in_file ) || feof( in_file ) ) {
                mem_free( p_buffer_set );
                p_buffer_set = NULL;
                mem_free( out_driver );
                out_driver = NULL;
                return( out_driver );
            }
        } 
        mem_free(p_buffer_set);
        p_buffer_set = NULL;
        
        /* Get the data_count and ensure it is not 0. */
    
        fread( &count8, sizeof( count8 ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            mem_free( out_driver );
            out_driver = NULL;
            return( out_driver );
        }

        if( count8 == 0x00 ) {
            mem_free( out_driver );
            out_driver = NULL;
            return( out_driver );
        }

        out_driver = parse_font_style( in_file, out_driver, \
                        &fontstyle_block_ptr[i], &p_buffer_set, &current, count8 );
        if( out_driver == NULL ) return( out_driver );
        fontstyle_block_ptr = (fontstyle_block *) ((uint8_t *) out_driver + \
                                (size_t) out_driver->fontstyles.fontstyleblocks);
    }
    
    /* Reset to the start of the next P-buffer's data. */

    if( (current - p_buffer_set->buffer) % 80 != 0 ) { 
        factor = (current - p_buffer_set->buffer) / 80;
        factor++;
        current = p_buffer_set->buffer + factor * 80;
    }

    /* Parse the absoluteaddress FunctionsBlock. */

    cop_functions = parse_functions_block( &current, p_buffer_set->buffer );

    /* The number of CodeBlocks may be 0 or 1. */

    switch( cop_functions->count ) {
    case 0x0000 :
        out_driver->absoluteaddress.count = 0;
        out_driver->absoluteaddress.text = NULL;
        break;
    case 0x0001 :
        out_driver->absoluteaddress.count = cop_functions->code_blocks->count;

        if( out_driver->allocated_size < (out_driver->next_offset + \
                                            out_driver->absoluteaddress.count) ) {
            out_driver = resize_cop_driver( out_driver, \
                                            out_driver->absoluteaddress.count );
        }
        text_ptr = (uint8_t *) out_driver + out_driver->next_offset;

        memcpy_s( text_ptr, out_driver->absoluteaddress.count, \
            cop_functions->code_blocks->text, out_driver->absoluteaddress.count );
        out_driver->absoluteaddress.text = (uint8_t *) out_driver->next_offset;
        out_driver->next_offset += out_driver->absoluteaddress.count;
        break;
    default:
        mem_free( p_buffer_set );
        p_buffer_set = NULL;
        mem_free( cop_functions->code_blocks );
        cop_functions->code_blocks = NULL;
        mem_free( cop_functions );
        cop_functions = NULL;
        mem_free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }

    if( cop_functions->code_blocks != NULL ) {
        mem_free( cop_functions->code_blocks );
        cop_functions->code_blocks = NULL;
    }
    mem_free( cop_functions );
    cop_functions = NULL;

    /* Reset to the start of the next P-buffer's data. */

    if( (current - p_buffer_set->buffer) % 80 != 0 ) { 
        factor = (current - p_buffer_set->buffer) / 80;
        factor++;
        current = p_buffer_set->buffer + factor * 80;
    }

    /* Parse the HlineBlock. */

    /* First parse FunctionsBlock. */

    cop_functions = parse_functions_block( &current, p_buffer_set->buffer );

    /* The number of CodeBlocks may be 0 or 1. */

    switch( cop_functions->count ) {
    case 0x0000 :
        out_driver->hline.count = 0;
        out_driver->hline.text = NULL;
        break;
    case 0x0001 :
        out_driver->hline.count = cop_functions->code_blocks->count;

        if( out_driver->allocated_size < (out_driver->next_offset + \
                                                    out_driver->hline.count) ) {
            out_driver= resize_cop_driver( out_driver, \
                                                    out_driver->hline.count );
        }
        text_ptr = (uint8_t *) out_driver + out_driver->next_offset;

        memcpy_s( text_ptr, out_driver->hline.count, \
                cop_functions->code_blocks->text, out_driver->hline.count );
        out_driver->hline.text = (uint8_t *) out_driver->next_offset;
        out_driver->next_offset += out_driver->hline.count;
        break;
    default:
        mem_free( p_buffer_set );
        p_buffer_set = NULL;
        mem_free( cop_functions->code_blocks );
        cop_functions->code_blocks = NULL;
        mem_free( cop_functions );
        cop_functions = NULL;
        mem_free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }

    if( cop_functions->code_blocks != NULL ) {
        mem_free( cop_functions->code_blocks );
        cop_functions->code_blocks = NULL;
    }
    mem_free( cop_functions );
    cop_functions = NULL;

    /* The thickness is present only if the HlineBlock was present. */

    if( out_driver->hline.text == NULL) {
        
        /* Reset to the start of the next P-buffer's data. */

        if( (current - p_buffer_set->buffer) % 80 != 0 ) { 
            factor = (current - p_buffer_set->buffer) / 80;
            factor++;
            current = p_buffer_set->buffer + factor * 80;
        }
    } else {

        /* The thickness halted the set of P-buffers so it is exhausted. */
        
        mem_free( p_buffer_set );
        p_buffer_set = NULL;

        /* Ensure that the count is 0x04. */
    
        fread( &count8, sizeof( count8 ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            mem_free( out_driver );
            out_driver = NULL;
            return( out_driver );
        }
    
        if( count8 != 0x04 ) {
            mem_free( out_driver );
            out_driver = NULL;
            return( out_driver );
        }
    
        /* Get the thickness. */

        fread( &out_driver->hline.thickness, \
                            sizeof( out_driver->hline.thickness ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            mem_free( out_driver );
            out_driver = NULL;
            return( out_driver );
        }
    }
    
    /* Parse the VlineBlock. */

    /* If there was an HlineBlock, then get the next set of P-buffers. */

    if( out_driver->hline.text != NULL) {
        p_buffer_set = get_p_buffer( in_file );
        if( p_buffer_set == NULL) {
            mem_free( out_driver );
            out_driver = NULL;
            return( out_driver );
        }

        if( p_buffer_set->buffer == NULL) {
            mem_free( p_buffer_set );
            p_buffer_set = NULL;
            mem_free( out_driver );
            out_driver = NULL;
            return( out_driver );
        }

        current = p_buffer_set->buffer;
    }

    /* Now parse the FunctionsBlock. */

    cop_functions = parse_functions_block( &current, p_buffer_set->buffer );

    /* The number of CodeBlocks may be 0 or 1. */

    switch( cop_functions->count ) {
    case 0x0000 :
        out_driver->vline.count = 0;
        out_driver->vline.text = NULL;
        break;
    case 0x0001 :
        out_driver->vline.count = cop_functions->code_blocks->count;

        if( out_driver->allocated_size < (out_driver->next_offset + \
            out_driver->vline.count) ) {

            out_driver= resize_cop_driver( out_driver, \
                out_driver->vline.count );
        }
        text_ptr = (uint8_t *) out_driver + out_driver->next_offset;

        memcpy_s( text_ptr, out_driver->vline.count, \
                    cop_functions->code_blocks->text, out_driver->vline.count );
        out_driver->vline.text = (uint8_t *) out_driver->next_offset;
        out_driver->next_offset += out_driver->vline.count;
        break;
    default:
        mem_free( p_buffer_set );
        p_buffer_set = NULL;
        mem_free( cop_functions->code_blocks );
        cop_functions->code_blocks = NULL;
        mem_free( cop_functions );
        cop_functions = NULL;
        mem_free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }

    if( cop_functions->code_blocks != NULL ) {
        mem_free( cop_functions->code_blocks );
        cop_functions->code_blocks = NULL;
    }
    mem_free( cop_functions );
    cop_functions = NULL;

    /* The thickness is present only if the VlineBlock was present. */

    if( out_driver->vline.text == NULL) {
        
        /* Reset to the start of the next P-buffer's data. */

        if( (current - p_buffer_set->buffer) % 80 != 0 ) { 
            factor = (current - p_buffer_set->buffer) / 80;
            factor++;
            current = p_buffer_set->buffer + factor * 80;
        }
    } else {

        /* The thickness halted the set of P-buffers so it is exhausted. */
        
        mem_free( p_buffer_set );
        p_buffer_set = NULL;

        /* Ensure that the count in 0x04. */
    
        fread( &count8, sizeof( count8 ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            mem_free( out_driver );
            out_driver = NULL;
            return( out_driver );
        }
    
        if( count8 != 0x04 ) {
            mem_free( out_driver );
            out_driver = NULL;
            return( out_driver );
        }
    
        /* Get the thickness. */

        fread( &out_driver->vline.thickness, \
                            sizeof( out_driver->vline.thickness ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            mem_free( out_driver );
            out_driver = NULL;
            return( out_driver );
        }
    }
    
    /* Parse the DboxBlock. */

    /* If there was a VlineBlock, then get the final set of P-buffers. */

    if( out_driver->vline.text != NULL) {

        /* Get the set of P-buffers */

        p_buffer_set = get_p_buffer( in_file );
        if( p_buffer_set == NULL) {
            mem_free( out_driver );
            out_driver = NULL;
            return( out_driver );
        }

        if( p_buffer_set->buffer == NULL) {
            mem_free( p_buffer_set );
            p_buffer_set = NULL;
            mem_free( out_driver );
            out_driver = NULL;
            return( out_driver );
        }

        current = p_buffer_set->buffer;
    }
    
    /* Now parse the FunctionsBlock. */

    cop_functions = parse_functions_block( &current, p_buffer_set->buffer );

    /* The number of CodeBlocks may be 0 or 1. */

    switch( cop_functions->count ) {
    case 0x0000 :
        out_driver->dbox.count = 0;
        out_driver->dbox.text = NULL;
        break;
    case 0x0001 :
        out_driver->dbox.count = cop_functions->code_blocks->count;

        if( out_driver->allocated_size < (out_driver->next_offset + \
            out_driver->dbox.count) ) {

            out_driver= resize_cop_driver( out_driver, out_driver->dbox.count );
        }
        text_ptr = (uint8_t *) out_driver + out_driver->next_offset;

        memcpy_s( text_ptr, out_driver->dbox.count, \
                    cop_functions->code_blocks->text, out_driver->dbox.count );
        out_driver->dbox.text = (uint8_t *) out_driver->next_offset;
        out_driver->next_offset += out_driver->dbox.count;
        break;
    default:
        mem_free( p_buffer_set );
        p_buffer_set = NULL;
        mem_free( cop_functions->code_blocks );
        cop_functions->code_blocks = NULL;
        mem_free( cop_functions );
        cop_functions = NULL;
        mem_free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }

    /* DboxBlock is the last item in the .COP file: the P-buffers are ended. */

    mem_free( p_buffer_set );
    p_buffer_set = NULL;
    if( cop_functions->code_blocks != NULL ) {
        mem_free( cop_functions->code_blocks );
        cop_functions->code_blocks = NULL;
    }
    mem_free( cop_functions );
    cop_functions = NULL;

    /* The thickness is present only if DboxBlock was present. */

    if( out_driver->dbox.text != NULL) {
        
        /* Ensure that the count in 0x04. */
    
        fread( &count8, sizeof( count8 ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            mem_free( out_driver );
            out_driver = NULL;
            return( out_driver );
        }
    
        if( count8 != 0x04 ) {
            mem_free( out_driver );
            out_driver = NULL;
            return( out_driver );
        }
    
        /* Get the thickness. */

        fread( &out_driver->dbox.thickness, \
                            sizeof( out_driver->dbox.thickness ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            mem_free( out_driver );
            out_driver = NULL;
            return( out_driver );
        }
    }

    /* Convert non-NULL offsets to pointers. */

    if( out_driver->rec_spec != NULL ) {
        string_ptr = (char *) out_driver + (size_t) out_driver->rec_spec;
        out_driver->rec_spec = string_ptr;
    }

    if( out_driver->inits.start != NULL ) {
        byte_ptr = (uint8_t *) out_driver + (size_t) out_driver->inits.start;
        out_driver->inits.start = (init_block *) byte_ptr;
        if(out_driver->inits.start->codeblock != NULL ) {
            byte_ptr = (uint8_t *) out_driver + (size_t) \
                                            out_driver->inits.start->codeblock;
            out_driver->inits.start->codeblock = (init_text *) byte_ptr;
            for( i = 0; i < out_driver->inits.start->count; i++ ) {
                if( out_driver->inits.start->codeblock[i].text != NULL ) {
                    byte_ptr = (uint8_t *) out_driver + (size_t) \
                                    out_driver->inits.start->codeblock[i].text;
                    out_driver->inits.start->codeblock[i].text = byte_ptr;
                }
            }
        }
    }
    
    if( out_driver->inits.document != NULL ) {
        byte_ptr = (uint8_t *) out_driver + (size_t) out_driver->inits.document;
        out_driver->inits.document = (init_block *) byte_ptr;
        if(out_driver->inits.document->codeblock != NULL ) {
            byte_ptr = (uint8_t *) out_driver + (size_t) \
                                            out_driver->inits.document->codeblock;
            out_driver->inits.document->codeblock = (init_text *) byte_ptr;
            for( i = 0; i < out_driver->inits.document->count; i++ ) {
                if( out_driver->inits.document->codeblock[i].text != NULL ) {
                    byte_ptr = (uint8_t *) out_driver + (size_t) \
                                    out_driver->inits.document->codeblock[i].text;
                    out_driver->inits.document->codeblock[i].text = byte_ptr;
                }
            }
        }
    }
    
    if( out_driver->finishes.end != NULL ) {
        byte_ptr = (uint8_t *) out_driver + (size_t) out_driver->finishes.end;
        out_driver->finishes.end = (code_text *) byte_ptr;
        if(out_driver->finishes.end->text != NULL ) {
            byte_ptr = (uint8_t *) out_driver + (size_t) \
                                                out_driver->finishes.end->text;
            out_driver->finishes.end->text = byte_ptr;
        }
    }
    
    if( out_driver->finishes.document != NULL ) {
        byte_ptr = (uint8_t *) out_driver + (size_t) \
                                                out_driver->finishes.document;
        out_driver->finishes.document = (code_text *) byte_ptr;
        if(out_driver->finishes.document->text != NULL ) {
            byte_ptr = (uint8_t *) out_driver + (size_t) \
                                            out_driver->finishes.document->text;
            out_driver->finishes.document->text = byte_ptr;
        }
    }
    
    if( out_driver->newlines.newlineblocks != NULL ) {
        byte_ptr = (uint8_t *) out_driver + (size_t) \
                                            out_driver->newlines.newlineblocks;
        out_driver->newlines.newlineblocks = (newline_block *) byte_ptr;
        for( i = 0; i < out_driver->newlines.count; i++ ) {
            if(out_driver->newlines.newlineblocks[i].text != NULL ) {
                byte_ptr = (uint8_t *) out_driver + (size_t) \
                                    out_driver->newlines.newlineblocks[i].text;
                out_driver->newlines.newlineblocks[i].text = byte_ptr;
            }
        }
    }

    if( out_driver->newpage.text != NULL ) {
        byte_ptr = (uint8_t *) out_driver + (size_t) out_driver->newpage.text;
        out_driver->newpage.text = byte_ptr;
    }

    if( out_driver->htab.text != NULL ) {
        byte_ptr = (uint8_t *) out_driver + (size_t) out_driver->htab.text;
        out_driver->htab.text = byte_ptr;
    }

    if( out_driver->fontswitches.fontswitchblocks != NULL ) {
        byte_ptr = (uint8_t *) out_driver + (size_t) \
                                        out_driver->fontswitches.fontswitchblocks;
        out_driver->fontswitches.fontswitchblocks = (fontswitch_block *) \
                                                                        byte_ptr;
        for( i = 0; i < out_driver->fontswitches.count; i++ ) {
            if( out_driver->fontswitches.fontswitchblocks[i].type != NULL ) {
                string_ptr = (char *) out_driver + (size_t) \
                                out_driver->fontswitches.fontswitchblocks[i].type;
                out_driver->fontswitches.fontswitchblocks[i].type = string_ptr;
            }
            if( out_driver->fontswitches.fontswitchblocks[i].startvalue != \
                                                                        NULL ) {
                byte_ptr = (uint8_t *) out_driver + (size_t) \
                        out_driver->fontswitches.fontswitchblocks[i].startvalue;
                out_driver->fontswitches.fontswitchblocks[i].startvalue = \
                                                        (code_text *) byte_ptr;
                if( out_driver->fontswitches.fontswitchblocks[i].\
                                                    startvalue->text != NULL ) {
                    byte_ptr = (uint8_t *) out_driver + (size_t) \
                out_driver->fontswitches.fontswitchblocks[i].startvalue->text;
                    out_driver->fontswitches.fontswitchblocks[i].\
                                                    startvalue->text = byte_ptr;
                }
            }
            if( out_driver->fontswitches.fontswitchblocks[i].endvalue != \
                                                                        NULL ) {
                byte_ptr = (uint8_t *) out_driver + (size_t) \
                        out_driver->fontswitches.fontswitchblocks[i].endvalue;
                out_driver->fontswitches.fontswitchblocks[i].endvalue = \
                                                        (code_text *) byte_ptr;
                if( out_driver->fontswitches.fontswitchblocks[i].\
                                                        endvalue->text != NULL ) {
                    byte_ptr = (uint8_t *) out_driver + (size_t) \
                out_driver->fontswitches.fontswitchblocks[i].endvalue->text;
                    out_driver->fontswitches.fontswitchblocks[i].\
                                                        endvalue->text = byte_ptr;
                }
            }
        }
    }

    if( out_driver->fontstyles.fontstyleblocks != NULL ) {
        byte_ptr = (uint8_t *) out_driver + (size_t) \
                                            out_driver->fontstyles.fontstyleblocks;
        out_driver->fontstyles.fontstyleblocks = (fontstyle_block *) byte_ptr;
        for( i = 0; i < out_driver->fontstyles.count; i++ ) {
            if( out_driver->fontstyles.fontstyleblocks[i].type != NULL ) {
                string_ptr = (char *) out_driver + (size_t) \
                                    out_driver->fontstyles.fontstyleblocks[i].type;
                out_driver->fontstyles.fontstyleblocks[i].type = string_ptr;
            }
            if( out_driver->fontstyles.fontstyleblocks[i].startvalue != NULL ) {
                byte_ptr = (uint8_t *) out_driver + (size_t) \
                            out_driver->fontstyles.fontstyleblocks[i].startvalue;
                out_driver->fontstyles.fontstyleblocks[i].startvalue = \
                                                        (code_text *) byte_ptr;
                if( out_driver->fontstyles.fontstyleblocks[i].startvalue->text \
                                                                    != NULL ) {
                    byte_ptr = (uint8_t *) out_driver + (size_t) \
                        out_driver->fontstyles.fontstyleblocks[i].startvalue->text;
                    out_driver->fontstyles.fontstyleblocks[i].startvalue->text \
                                                                    = byte_ptr;
                }
            }
            if( out_driver->fontstyles.fontstyleblocks[i].endvalue != NULL ) {
                byte_ptr = (uint8_t *) out_driver + (size_t) \
                            out_driver->fontstyles.fontstyleblocks[i].endvalue;
                out_driver->fontstyles.fontstyleblocks[i].endvalue = \
                                                        (code_text *) byte_ptr;
                if( out_driver->fontstyles.fontstyleblocks[i].endvalue->text \
                                                                    != NULL ) {
                    byte_ptr = (uint8_t *) out_driver + (size_t) \
                        out_driver->fontstyles.fontstyleblocks[i].endvalue->text;
                    out_driver->fontstyles.fontstyleblocks[i].endvalue->text = \
                                                                        byte_ptr;
                }
            }
            if( out_driver->fontstyles.fontstyleblocks[i].lineprocs != NULL ) {
                byte_ptr = (uint8_t *) out_driver + (size_t) \
                            out_driver->fontstyles.fontstyleblocks[i].lineprocs;
                out_driver->fontstyles.fontstyleblocks[i].lineprocs = \
                                                        (line_proc *) byte_ptr;
                for( j = 0; j < out_driver->fontstyles.fontstyleblocks[i].\
                                                            line_passes; j++ ) {
                    if( out_driver->fontstyles.fontstyleblocks[i].\
                                            lineprocs[j].startvalue != NULL ) {
                        byte_ptr = (uint8_t *) out_driver + (size_t) \
                out_driver->fontstyles.fontstyleblocks[i].lineprocs[j].startvalue;
                        out_driver->fontstyles.fontstyleblocks[i].\
                                lineprocs[j].startvalue = (code_text *) byte_ptr;
                        if( out_driver->fontstyles.fontstyleblocks[i].\
                                        lineprocs[j].startvalue->text != NULL ) {
                            byte_ptr = (uint8_t *) out_driver + (size_t) \
        out_driver->fontstyles.fontstyleblocks[i].lineprocs[j].startvalue->text;
                            out_driver->fontstyles.fontstyleblocks[i].\
                                        lineprocs[j].startvalue->text = byte_ptr;
                        }
                    }
                    if( out_driver->fontstyles.fontstyleblocks[i].\
                                                lineprocs[j].firstword != NULL ) {
                        byte_ptr = (uint8_t *) out_driver + (size_t) \
                out_driver->fontstyles.fontstyleblocks[i].lineprocs[j].firstword;
                        out_driver->fontstyles.fontstyleblocks[i].\
                                lineprocs[j].firstword = (code_text *) byte_ptr;
                        if( out_driver->fontstyles.fontstyleblocks[i].\
                                        lineprocs[j].firstword->text != NULL ) {
                            byte_ptr = (uint8_t *) out_driver + (size_t)
            out_driver->fontstyles.fontstyleblocks[i].lineprocs[j].firstword->text;
                            out_driver->fontstyles.fontstyleblocks[i].\
                                        lineprocs[j].firstword->text = byte_ptr;
                        }
                    }
                    if( out_driver->fontstyles.fontstyleblocks[i].\
                                                lineprocs[j].startword != NULL ) {
                        byte_ptr = (uint8_t *) out_driver + (size_t) \
                out_driver->fontstyles.fontstyleblocks[i].lineprocs[j].startword;
                        out_driver->fontstyles.fontstyleblocks[i].\
                                lineprocs[j].startword = (code_text *) byte_ptr;
                        if( out_driver->fontstyles.fontstyleblocks[i].\
                                        lineprocs[j].startword->text != NULL ) {
                            byte_ptr = (uint8_t *) out_driver + (size_t) \
            out_driver->fontstyles.fontstyleblocks[i].lineprocs[j].startword->text;
                            out_driver->fontstyles.fontstyleblocks[i].\
                                        lineprocs[j].startword->text = byte_ptr;
                        }
                    }
                    if( out_driver->fontstyles.fontstyleblocks[i].\
                                                lineprocs[j].endword != NULL ) {
                        byte_ptr = (uint8_t *) out_driver + (size_t) \
                    out_driver->fontstyles.fontstyleblocks[i].lineprocs[j].endword;
                        out_driver->fontstyles.fontstyleblocks[i].\
                                    lineprocs[j].endword = (code_text *) byte_ptr;
                        if( out_driver->fontstyles.fontstyleblocks[i].\
                                            lineprocs[j].endword->text != NULL ) {
                            byte_ptr = (uint8_t *) out_driver + (size_t) \
            out_driver->fontstyles.fontstyleblocks[i].lineprocs[j].endword->text;
                            out_driver->fontstyles.fontstyleblocks[i].\
                                            lineprocs[j].endword->text = byte_ptr;
                        }
                    }
                    if( out_driver->fontstyles.fontstyleblocks[i].\
                                                lineprocs[j].endvalue != NULL ) {

                        byte_ptr = (uint8_t *) out_driver + (size_t) \
                out_driver->fontstyles.fontstyleblocks[i].lineprocs[j].endvalue;
                        out_driver->fontstyles.fontstyleblocks[i].\
                                lineprocs[j].endvalue = (code_text *) byte_ptr;
                        if( out_driver->fontstyles.fontstyleblocks[i].\
                                        lineprocs[j].endvalue->text != NULL ) {
                            byte_ptr = (uint8_t *) out_driver + (size_t) \
            out_driver->fontstyles.fontstyleblocks[i].lineprocs[j].endvalue->text;
                            out_driver->fontstyles.fontstyleblocks[i].\
                                            lineprocs[j].endvalue->text = byte_ptr;
                        }
                    }
                }
            }
        }
    }

    if( out_driver->absoluteaddress.text != NULL ) {
        byte_ptr = (uint8_t *) out_driver + (size_t) \
                                                out_driver->absoluteaddress.text;
        out_driver->absoluteaddress.text = byte_ptr;
    }

    if( out_driver->hline.text != NULL ) {
        byte_ptr = (uint8_t *) out_driver + (size_t) out_driver->hline.text;
        out_driver->hline.text = byte_ptr;
    }

    if( out_driver->vline.text != NULL ) {
        byte_ptr = (uint8_t *) out_driver + (size_t) out_driver->vline.text;
        out_driver->vline.text = byte_ptr;
    }

    if( out_driver->dbox.text != NULL ) {
        byte_ptr = (uint8_t *) out_driver + (size_t) out_driver->dbox.text;
        out_driver->dbox.text = byte_ptr;
    }

    return( out_driver );
}

