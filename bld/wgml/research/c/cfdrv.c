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
* Description:  Implements the functions declared in cfdrv.h:
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
#include "cfdrv.h"
#include "cffunc.h"

/*  Local macros */

#define START_SIZE 2048
#define INC_SIZE   1024

#define IN_DRV_MAP(x)       (void *)((char *)in_driver + (size_t)x)
#define IN_DRV_MAP_OFF()    (void *)((char *)in_driver + (size_t)in_driver->next_offset)
#define IN_DRV_GET_OFF()    (void *)(in_driver->next_offset)
#define IN_DRV_ADD_OFF(x)   in_driver->next_offset += x
#define IN_DRV_REMAP(x)     in_driver->x = (void *)((char *)in_driver + (size_t)in_driver->x)

#define OUT_DRV_MAP(x)      (void *)((char *)out_driver + (size_t)x)
#define OUT_DRV_MAP_OFF()   (void *)((char *)out_driver + (size_t)out_driver->next_offset)
#define OUT_DRV_GET_OFF()   ((void *)out_driver->next_offset)
#define OUT_DRV_ADD_OFF(x)  out_driver->next_offset += x
#define OUT_DRV_REMAP(x)    out_driver->x = (void *)((char *)out_driver + (size_t)out_driver->x)

/*  Local function definitions */

/*  Function resize_cop_driver().
 *  Resizes a cop_driver instance.
 *
 *  Parameters:
 *      in_driver is a pointer to the cop_driver to be resized.
 *      in_size is the minimum acceptable increase in size.
 *
 *  Returns:
 *      A pointer to a cop_driver instance at least in_size larger with the same
 *          data (except for the allocated_size field, which reflects the new size)
 *          on success.
 *      A NULL pointer on failure.
 *
 * Notes:
 *      realloc() will free in_driver if the instance is actually moved to a
 *          new location.
 *      if realloc() returns NULL, then in_driver will be freed.
 *      The intended use is for the pointer passed as in_driver to be used to
 *          store the return value.
 */

static cop_driver *resize_cop_driver( cop_driver *in_driver, size_t in_size )
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

    local_driver = realloc( in_driver, new_size );
    if( local_driver == NULL ) {
        free( in_driver );
    } else {
        local_driver->allocated_size = new_size;
    }
    return( local_driver );
}

/*  Function parse_finish_block().
 *  Processes a single FinishBlock. This may be called up to two times when
 *  parsing a .COP file encoding a :DRIVER block. 
 *
 *  Note: although a :FINISH block can contain more than one :VALUE block, and
 *  gendev will put more than one CodeBlock into the .COP file, wgml only
 *  invokes the first one. Thus, this function only parses the first CodeBlock.
 *
 *  Parameters:
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
 *      NULL on failure.
 */

static cop_driver *parse_finish_block( cop_driver *in_driver, char **current, char *base )
{

    code_block *    cop_codeblocks  = NULL;
    code_text *     code_text_ptr   = NULL;
    uint8_t         designator;    
    uint8_t *       text_ptr        = NULL;
    uint16_t        count;
    size_t          size;

    /* Get the designator. */

    memcpy_s( &designator, 1, *current, 1 );
    (*current)++;

    /* Parse the FinishBlock. */

    switch( designator ) {
    case 0x01 :
        memcpy_s( &count, sizeof( count ), *current, sizeof( count ) );
        *current += sizeof( count );
        if( count == 0x0000 ) {
            puts( "END :FINISH block has no code!" );
            free( in_driver );
            in_driver = NULL;
            return( in_driver );
        }

        if( count > 0x0001 ) {
            printf_s( "END :FINISH block has unexpected number of CodeBlocks: %i\n", count);
        }

        /* Add the code_text struct itself. */

        if( in_driver->allocated_size < (in_driver->next_offset + sizeof( code_text )) ) {
            in_driver = resize_cop_driver( in_driver, sizeof( code_text ) );
            if( in_driver == NULL ) return( in_driver );
        }
        code_text_ptr = IN_DRV_MAP_OFF();

        in_driver->finishes.end = IN_DRV_GET_OFF();
        IN_DRV_ADD_OFF( sizeof( code_text ) );

        /* Get the CodeBlocks. */
        
        cop_codeblocks = get_code_blocks( current, count, base, "END :FINISH" );
        if( cop_codeblocks == NULL ) {
            free( in_driver );
            in_driver = NULL;
            return( in_driver );
        }
        
        /* Initialize the code_text struct. */

        if( cop_codeblocks[0].designator != 0x00 ) {
            printf_s( "END :FINISH block has unexpected CodeBlock designator: %i\n", designator );
            free( cop_codeblocks );
            cop_codeblocks = NULL;
            free( in_driver );
            in_driver = NULL;
            return( in_driver );
        }

        size = code_text_ptr->count = cop_codeblocks[0].count;

        if( in_driver->allocated_size < (in_driver->next_offset + size) ) {
            in_driver = resize_cop_driver( in_driver, size );
            if( in_driver == NULL ) {
                free( cop_codeblocks );
                cop_codeblocks = NULL;
                return( in_driver );
            }
            code_text_ptr = IN_DRV_MAP( in_driver->finishes.end );
        }

        text_ptr = IN_DRV_MAP_OFF();

        memcpy_s( text_ptr, size, cop_codeblocks[0].text, size );
        code_text_ptr->text = IN_DRV_GET_OFF();
        IN_DRV_ADD_OFF( size );

        free( cop_codeblocks );
        cop_codeblocks = NULL;

        break;
    case 0x02 :
        memcpy_s( &count, sizeof( count ), *current, sizeof( count ) );
        *current += sizeof( count );
        if( count == 0 ) {
            puts( "DOCUMENT :FINISH block has no code!" );
            free( in_driver );
            in_driver = NULL;
            return( in_driver );
        }

        if( count > 0x0001 ) {
            printf_s( "DOCUMENT :FINISH block has unexpected number of CodeBlocks: %i\n", count);
        }

        /* Add the code_text struct itself. */

        if( in_driver->allocated_size < (in_driver->next_offset + sizeof( code_text )) ) {
            in_driver = resize_cop_driver( in_driver, sizeof( code_text ) );
            if( in_driver == NULL ) return( in_driver );
        }
        code_text_ptr = IN_DRV_MAP_OFF();

        in_driver->finishes.document = IN_DRV_GET_OFF();
        IN_DRV_ADD_OFF( sizeof( code_text ) );

        /* Get the CodeBlocks. */
        
        cop_codeblocks = get_code_blocks( current, count, base, "DOCUMENT :FINISH" );
        if( cop_codeblocks == NULL ) {
            free( in_driver );
            in_driver = NULL;
            return( in_driver );
        }
        
        /* Initialize the code_text struct. */

        if( cop_codeblocks[0].designator != 0x00 ) {
            printf_s( "DOCUMENT :FINISH block has unexpected CodeBlock designator: %i\n", designator );
            free( cop_codeblocks );
            cop_codeblocks = NULL;
            free( in_driver );
            in_driver = NULL;
            return( in_driver );
        }

        size = code_text_ptr->count = cop_codeblocks[0].count;

        if( in_driver->allocated_size < (in_driver->next_offset + size) ) {
            in_driver = resize_cop_driver( in_driver, size );
            if( in_driver == NULL ) {
                free( cop_codeblocks );
                cop_codeblocks = NULL;
                return( in_driver );
            }
            code_text_ptr = IN_DRV_MAP( in_driver->finishes.document );
        }
        text_ptr = IN_DRV_MAP_OFF();

        memcpy_s( text_ptr, size, cop_codeblocks[0].text, size );
        code_text_ptr->text = IN_DRV_GET_OFF();
        IN_DRV_ADD_OFF( size );

        free( cop_codeblocks );
        cop_codeblocks = NULL;

        break;
    default :
        printf_s( ":FINISH block designator incorrect: %i\n", designator );
        free( in_driver );
        in_driver = NULL;
        return( in_driver );
    }

    return( in_driver );
}

/*  Function parse_font_style().
 *  Processes a single ShortFontStyle. This may be called any number of times
 *  when parsing a .COP file encoding a :DRIVER block.
 *
 *  Note: The Wiki should be consulted if the treatment of p_buffer_set and
 *        count appears inexplicable. The structure in the .COP file is very
 *        odd and something like the contortions shown here are unavoidable.
 *
 *  Parameters:
 *      in_file is the file being parsed.
 *      in_driver contains the cop_drivr being initialized.
 *      fontstyle_block_ptr is the current fontstyle_block instance.
 *      p_buffer_set should be NULL and any memory formerly pointed to freed.
 *      count contains the total length of the pre-P-buffer data.
 *
 *  Parameter modified:
 *      if the function succeeds, then *p_buffer_set will point to the
 *          current set of P-buffers.
 *      if the function fails, then *p_buffer_set will be NULL.
 *
 *  Returns:
 *      in_driver, possibly relocated, on success.
 *      NULL on failure.
 */

static cop_driver * parse_font_style( FILE * in_file, cop_driver * in_driver, 
                                       fontstyle_block * fontstyle_block_ptr,
                                       p_buffer **p_buffer_set,
                                       char **current, uint8_t count )
{

    char *          string_ptr              = NULL;
    code_block *    cop_codeblocks          = NULL;
    code_text *     code_text_ptr           = NULL;
    int             i;
    line_proc *     line_proc_ptr           = NULL;
    ptrdiff_t       fontstyle_block_offset;
    uint8_t         save_designator;
    uint8_t *       text_ptr                = NULL;
    uint16_t        save_pass;
    uint16_t        count16;
    size_t          size;

    /* Get the number of passes, which can be 0. */

    fread( &fontstyle_block_ptr->passes, sizeof( fontstyle_block_ptr->passes ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( in_driver );
        in_driver = NULL;
        return( in_driver );
    }
    count -= sizeof( fontstyle_block_ptr->passes );

    /* Get the unknown_count, and verify that it is 1. */

    fread( &count16, sizeof( count16 ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( in_driver );
        in_driver = NULL;
        return( in_driver );
    }

    if( count16 != 0x0001 ) {
        printf_s( "ShortFontstyleBlock has incorrect 'unknown count' value: %i\n", count16 );
        free( in_driver );
        in_driver = NULL;
        return( in_driver );
    }
    count -= sizeof( count16 );

    /* Get the two nulls, and verify they contain the value 0. */

    fread( &count16, sizeof( count16 ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( in_driver );
        in_driver = NULL;
        return( in_driver );
    }

    if( count16 != 0x0000 ) {
        printf_s( "ShortFontstyleBlock has incorrect 'two-byte nulls' value: %i\n", count16 );
        free( in_driver );
        in_driver = NULL;
        return( in_driver );
    }
    count -= sizeof( count16 );

    /* count should now contain the length of type, including the terminal null. */

    if( count == 0 ) {
        puts( ":FONTSTYLE block has no type");
        free( in_driver );
        in_driver = NULL;
        return( in_driver );
    }

    /* Add the space for the type. */

    if( in_driver->allocated_size < (in_driver->next_offset + count) ) {
        fontstyle_block_offset = (char *)fontstyle_block_ptr - (char *)in_driver;
        in_driver = resize_cop_driver( in_driver, count );
        if( in_driver == NULL ) {
            free( *p_buffer_set );
            *p_buffer_set = NULL;
            return( in_driver );
        }
        fontstyle_block_ptr = IN_DRV_MAP( fontstyle_block_offset );
    }

    string_ptr = IN_DRV_MAP_OFF();

    fontstyle_block_ptr->type = IN_DRV_GET_OFF();
    IN_DRV_ADD_OFF( count );

    /* Acquire the type. */
        
    fread( string_ptr, count, 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( in_driver );
        in_driver = NULL;
        return( in_driver );
    }

    /* Done here so lineprocs can be used as an array. */

    if( fontstyle_block_ptr->passes == 0 ) {
        fontstyle_block_ptr->lineprocs = NULL;
    } else {

        /* Add the space for the line_proc struct instances. */
        size = fontstyle_block_ptr->passes * sizeof( line_proc );
        if( in_driver->allocated_size < (in_driver->next_offset + size ) ) {
            fontstyle_block_offset = (char *)fontstyle_block_ptr - (char *)in_driver;
            in_driver = resize_cop_driver( in_driver, size );
            if( in_driver == NULL ) {
                free( *p_buffer_set );
                *p_buffer_set = NULL;
                return( in_driver );
            }
            fontstyle_block_ptr = IN_DRV_MAP( fontstyle_block_offset );
        }
        line_proc_ptr = IN_DRV_MAP_OFF();

        fontstyle_block_ptr->lineprocs = IN_DRV_GET_OFF();
        IN_DRV_ADD_OFF( size );

        /* Set the line_proc struct instance pointers to null. */

        for( i = 0; i < fontstyle_block_ptr->passes; i++ ) {
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
        free( in_driver );
        in_driver = NULL;
        return( in_driver );
    }

    *current = (*p_buffer_set)->buffer;

    /* Skip the flags. */

    *current += 21;

    /* Get the number of codeblocks, which can be 0. */

    memcpy_s( &count16, sizeof( count16 ), *current, sizeof( count16 ) );
    *current += sizeof( count16 );
    
    /* If the count is 0, we are done: the P-buffer is empty. */

    if( count16 == 0 ) return( in_driver );

    /* Get the CodeBlocks, after displaying the current font style name. */
        
    printf( "Fontstyle: %s\n", string_ptr );
    cop_codeblocks = get_code_blocks( current, count16, (*p_buffer_set)->buffer, ":FONTSTYLE" );
    if( cop_codeblocks == NULL ) {
        free( *p_buffer_set );
        *p_buffer_set = NULL;
        free( in_driver );
        in_driver = NULL;
        return( in_driver );
    }

    /* Process the CodeBlocks into the fontstyle_block instance. */

    save_designator = 0;
    save_pass = 0;

    for( i = 0; i < count16; i++ ) {

        /* Trap zero-length CodeBlocks. */

        if( cop_codeblocks[i].count == 0 ) {
            printf("  designator = %x, cb05_flag = %x, lp_flag = %x, pass = %x\n", cop_codeblocks[i].designator,
                   cop_codeblocks[i].cb05_flag, cop_codeblocks[i].lp_flag, cop_codeblocks[i].pass);
            continue;
        }
        
        switch( cop_codeblocks[i].designator ) {
        case 0x04 :

            /* The :FONTSTYLE :ENDVALUE block. */

            /* Add the code_text struct. */

            if( in_driver->allocated_size < (in_driver->next_offset + sizeof( code_text )) ) {
                fontstyle_block_offset = (char *)fontstyle_block_ptr - (char *)in_driver;
                in_driver = resize_cop_driver( in_driver, sizeof( code_text ) );
                if( in_driver == NULL ) {
                    free( *p_buffer_set );
                    *p_buffer_set = NULL;
                    free( cop_codeblocks );
                    cop_codeblocks = NULL;
                    return( in_driver );
                }
                fontstyle_block_ptr = IN_DRV_MAP( fontstyle_block_offset );
                line_proc_ptr = IN_DRV_MAP( fontstyle_block_ptr->lineprocs );
            }
            code_text_ptr = IN_DRV_MAP_OFF();

            fontstyle_block_ptr->endvalue = IN_DRV_GET_OFF();
            IN_DRV_ADD_OFF( sizeof( code_text ) );

            /* Initialize the code_text struct. */

            size = code_text_ptr->count = cop_codeblocks[i].count;

            if( in_driver->allocated_size < (in_driver->next_offset + size) ) {
                fontstyle_block_offset = (char *)fontstyle_block_ptr - (char *)in_driver;
                in_driver = resize_cop_driver( in_driver, size );
                if( in_driver == NULL ) {
                    free( *p_buffer_set );
                    *p_buffer_set = NULL;
                    free( cop_codeblocks );
                    cop_codeblocks = NULL;
                    return( in_driver );
                }
                fontstyle_block_ptr = IN_DRV_MAP( fontstyle_block_offset );
                line_proc_ptr = IN_DRV_MAP( fontstyle_block_ptr->lineprocs );
                code_text_ptr = IN_DRV_MAP( fontstyle_block_ptr->endvalue );
            }
            text_ptr = IN_DRV_MAP_OFF();

            memcpy_s( text_ptr, size, cop_codeblocks[i].text, size );
            code_text_ptr->text = IN_DRV_GET_OFF();
            IN_DRV_ADD_OFF( size );

            /* Research code. */

            printf("  designator = %x, cb05_flag = %x, lp_flag = %x, pass = %x\n", cop_codeblocks[i].designator,
                   cop_codeblocks[i].cb05_flag, cop_codeblocks[i].lp_flag, cop_codeblocks[i].pass);
            save_designator = cop_codeblocks[i].designator;
            save_pass = cop_codeblocks[i].pass;

            break;
        case 0x05 :

            /* The :FONTSTYLE :STARTVALUE block. */

            /* Add the code_text struct. */

            if( in_driver->allocated_size < (in_driver->next_offset + sizeof( code_text )) ) {
                fontstyle_block_offset = (char *)fontstyle_block_ptr - (char *)in_driver;
                in_driver = resize_cop_driver( in_driver, sizeof( code_text ) );
                if( in_driver == NULL ) {
                    free( *p_buffer_set );
                    *p_buffer_set = NULL;
                    free( cop_codeblocks );
                    cop_codeblocks = NULL;
                    return( in_driver );
                }
                fontstyle_block_ptr = IN_DRV_MAP( fontstyle_block_offset );
                line_proc_ptr = IN_DRV_MAP( fontstyle_block_ptr->lineprocs );
            }
            code_text_ptr = IN_DRV_MAP_OFF();

            fontstyle_block_ptr->startvalue = IN_DRV_GET_OFF();
            IN_DRV_ADD_OFF( sizeof( code_text ) );

            /* Initialize the code_text struct. */

            size = code_text_ptr->count = cop_codeblocks[i].count;

            if( in_driver->allocated_size < (in_driver->next_offset + size) ) {
                fontstyle_block_offset = (char *)fontstyle_block_ptr - (char *)in_driver;
                in_driver = resize_cop_driver( in_driver, size );
                if( in_driver == NULL ) {
                    free( *p_buffer_set );
                    *p_buffer_set = NULL;
                    free( cop_codeblocks );
                    cop_codeblocks = NULL;
                    return( in_driver );
                }
                fontstyle_block_ptr = IN_DRV_MAP( fontstyle_block_offset );
                line_proc_ptr = IN_DRV_MAP( fontstyle_block_ptr->lineprocs );
                code_text_ptr = IN_DRV_MAP( fontstyle_block_ptr->startvalue );
            }
            text_ptr = IN_DRV_MAP_OFF();

            memcpy_s( text_ptr, size, cop_codeblocks[i].text, size );
            code_text_ptr->text = IN_DRV_GET_OFF();
            IN_DRV_ADD_OFF( size );

            /* Research code. */

            printf("  designator = %x, cb05_flag = %x, lp_flag = %x, pass = %x\n", cop_codeblocks[i].designator,
                   cop_codeblocks[i].cb05_flag, cop_codeblocks[i].lp_flag, cop_codeblocks[i].pass);
            if( (save_designator == 0x04) && (save_pass == cop_codeblocks[i].pass) ) {
                puts( " <-inversion!" );
            } else {
                puts("");
            }
            save_designator = cop_codeblocks[i].designator;
            save_pass = cop_codeblocks[i].pass;

            break;
        case 0x08 :

            /* A :FONTSTYLE :LINEPROC :ENDVALUE block. */

            /* Ensure that the pass is within range */

            if( cop_codeblocks[i].pass > fontstyle_block_ptr->passes ) {
                printf_s( ":FONTSTYLE :LINEPROC :ENDVALUE block has incorrect pass value: %i\n", cop_codeblocks[i].pass );
                free( *p_buffer_set );
                *p_buffer_set = NULL;
                free( cop_codeblocks );
                cop_codeblocks = NULL;
                free( in_driver );
                in_driver = NULL;
                return( in_driver );
            }

            /* Add the code_text struct. */

            if( in_driver->allocated_size < (in_driver->next_offset + sizeof( code_text )) ) {
                fontstyle_block_offset = (char *)fontstyle_block_ptr - (char *)in_driver;
                in_driver = resize_cop_driver( in_driver, sizeof( code_text ) );
                if( in_driver == NULL ) {
                    free( *p_buffer_set );
                    *p_buffer_set = NULL;
                    free( cop_codeblocks );
                    cop_codeblocks = NULL;
                    return( in_driver );
                }
                fontstyle_block_ptr = IN_DRV_MAP( fontstyle_block_offset );
                line_proc_ptr = IN_DRV_MAP( fontstyle_block_ptr->lineprocs );
            }
            code_text_ptr = IN_DRV_MAP_OFF();

            line_proc_ptr[cop_codeblocks[i].pass - 1].endvalue = IN_DRV_GET_OFF();
            IN_DRV_ADD_OFF( sizeof( code_text ) );

            /* Initialize the code_text struct. */

            size = code_text_ptr->count = cop_codeblocks[i].count;

            if( in_driver->allocated_size < (in_driver->next_offset + size) ) {
                fontstyle_block_offset = (char *)fontstyle_block_ptr - (char *)in_driver;
                in_driver = resize_cop_driver( in_driver, size );
                if( in_driver == NULL ) {
                    free( *p_buffer_set );
                    *p_buffer_set = NULL;
                    free( cop_codeblocks );
                    cop_codeblocks = NULL;
                    return( in_driver );
                }
                fontstyle_block_ptr = IN_DRV_MAP( fontstyle_block_offset );
                line_proc_ptr = IN_DRV_MAP( fontstyle_block_ptr->lineprocs );
                code_text_ptr = IN_DRV_MAP( line_proc_ptr[cop_codeblocks[i].pass - 1].endvalue );
            }
            text_ptr = IN_DRV_MAP_OFF();

            memcpy_s( text_ptr, size, cop_codeblocks[i].text, size );
            code_text_ptr->text = IN_DRV_GET_OFF();
            IN_DRV_ADD_OFF( size );

            /* Research code. */

            printf("  designator = %x, cb05_flag = %x, lp_flag = %x, pass = %x\n", cop_codeblocks[i].designator,
                   cop_codeblocks[i].cb05_flag, cop_codeblocks[i].lp_flag, cop_codeblocks[i].pass);
            save_designator = cop_codeblocks[i].designator;
            save_pass = cop_codeblocks[i].pass;

            break;
        case 0x09 :

            /* A :FONTSTYLE :LINEPROC :STARTVALUE block. */

            /* Ensure that the pass is within range. */

            if( cop_codeblocks[i].pass > fontstyle_block_ptr->passes ) {
                printf_s( ":FONTSTYLE :LINEPROC :STARTVALUE block has incorrect pass value: %i\n", cop_codeblocks[i].pass );
                free( *p_buffer_set );
                *p_buffer_set = NULL;
                free( cop_codeblocks );
                cop_codeblocks = NULL;
                free( in_driver );
                in_driver = NULL;
                return( in_driver );
            }

            /* Add the code_text struct. */

            if( in_driver->allocated_size < (in_driver->next_offset + sizeof( code_text )) ) {
                fontstyle_block_offset = (char *)fontstyle_block_ptr - (char *)in_driver;
                in_driver = resize_cop_driver( in_driver, sizeof( code_text ) );
                if( in_driver == NULL ) {
                    free( *p_buffer_set );
                    *p_buffer_set = NULL;
                    free( cop_codeblocks );
                    cop_codeblocks = NULL;
                    return( in_driver );
                }
                fontstyle_block_ptr = IN_DRV_MAP( fontstyle_block_offset );
                line_proc_ptr = IN_DRV_MAP( fontstyle_block_ptr->lineprocs );
            }
            code_text_ptr = IN_DRV_MAP_OFF();

            line_proc_ptr[cop_codeblocks[i].pass - 1].startvalue = IN_DRV_GET_OFF();
            IN_DRV_ADD_OFF( sizeof( code_text ) );

            /* Initialize the code_text struct. */

            size = code_text_ptr->count = cop_codeblocks[i].count;

            if( in_driver->allocated_size < (in_driver->next_offset + size) ) {
                fontstyle_block_offset = (char *)fontstyle_block_ptr - (char *)in_driver;
                in_driver = resize_cop_driver( in_driver, size );
                if( in_driver == NULL ) {
                    free( *p_buffer_set );
                    *p_buffer_set = NULL;
                    free( cop_codeblocks );
                    cop_codeblocks = NULL;
                    return( in_driver );
                }
                fontstyle_block_ptr = IN_DRV_MAP( fontstyle_block_offset );
                line_proc_ptr = IN_DRV_MAP( fontstyle_block_ptr->lineprocs );
                code_text_ptr = IN_DRV_MAP( line_proc_ptr[cop_codeblocks[i].pass - 1].startvalue );
            }
            text_ptr = IN_DRV_MAP_OFF();

            memcpy_s( text_ptr, size, cop_codeblocks[i].text, size );
            code_text_ptr->text = IN_DRV_GET_OFF();
            IN_DRV_ADD_OFF( size );

            /* Research code. */

            printf("  designator = %x, cb05_flag = %x, lp_flag = %x, pass = %x\n", cop_codeblocks[i].designator,
                   cop_codeblocks[i].cb05_flag, cop_codeblocks[i].lp_flag, cop_codeblocks[i].pass);
            if( (save_designator == 0x08) && (save_pass == cop_codeblocks[i].pass) ) {
                puts( " <-inversion!" );
            } else {
                puts("");
            }
            save_designator = cop_codeblocks[i].designator;
            save_pass = cop_codeblocks[i].pass;

            break;
        case 0x28 :

            /* A :FONTSTYLE :LINEPROC :ENDWORD block. */

            /* Ensure that the pass is within range. */

            if( cop_codeblocks[i].pass > fontstyle_block_ptr->passes ) {
                printf_s( ":FONTSTYLE :LINEPROC :ENDWORD block has incorrect pass value: %i\n", cop_codeblocks[i].pass );
                free( *p_buffer_set );
                *p_buffer_set = NULL;
                free( cop_codeblocks );
                cop_codeblocks = NULL;
                free( in_driver );
                in_driver = NULL;
                return( in_driver );
            }

            /* Add the code_text struct. */

            if( in_driver->allocated_size < (in_driver->next_offset + sizeof( code_text )) ) {
                fontstyle_block_offset = (char *)fontstyle_block_ptr - (char *)in_driver;
                in_driver = resize_cop_driver( in_driver, sizeof( code_text ) );
                if( in_driver == NULL ) {
                    free( *p_buffer_set );
                    *p_buffer_set = NULL;
                    free( cop_codeblocks );
                    cop_codeblocks = NULL;
                    return( in_driver );
                }
                fontstyle_block_ptr = IN_DRV_MAP( fontstyle_block_offset );
                line_proc_ptr = IN_DRV_MAP( fontstyle_block_ptr->lineprocs );
            }
            code_text_ptr = IN_DRV_MAP_OFF();

            line_proc_ptr[cop_codeblocks[i].pass - 1].endword = IN_DRV_GET_OFF();
            IN_DRV_ADD_OFF( sizeof( code_text ) );

            /* Initialize the code_text struct. */

            size = code_text_ptr->count = cop_codeblocks[i].count;

            if( in_driver->allocated_size < (in_driver->next_offset + size) ) {
                fontstyle_block_offset = (char *)fontstyle_block_ptr - (char *)in_driver;
                in_driver = resize_cop_driver( in_driver, size );
                if( in_driver == NULL ) {
                    free( *p_buffer_set );
                    *p_buffer_set = NULL;
                    free( cop_codeblocks );
                    cop_codeblocks = NULL;
                    return( in_driver );
                }
                fontstyle_block_ptr = IN_DRV_MAP( fontstyle_block_offset );
                line_proc_ptr = IN_DRV_MAP( fontstyle_block_ptr->lineprocs );
                code_text_ptr = IN_DRV_MAP( line_proc_ptr[cop_codeblocks[i].pass - 1].endword );
            }
            text_ptr = IN_DRV_MAP_OFF();

            memcpy_s( text_ptr, size, cop_codeblocks[i].text, size );
            code_text_ptr->text = IN_DRV_GET_OFF();
            IN_DRV_ADD_OFF( size );

            /* Research code. */

            printf("  designator = %x, cb05_flag = %x, lp_flag = %x, pass = %x\n", cop_codeblocks[i].designator,
                   cop_codeblocks[i].cb05_flag, cop_codeblocks[i].lp_flag, cop_codeblocks[i].pass);
            save_designator = cop_codeblocks[i].designator;
            save_pass = cop_codeblocks[i].pass;

            break;
        case 0x29 :

            /* A :FONTSTYLE :LINEPROC :STARTWORD block. */

            /* Ensure that the pass is within range. */

            if( cop_codeblocks[i].pass > fontstyle_block_ptr->passes ) {
                printf_s( ":FONTSTYLE :LINEPROC :STARTWORD block has incorrect pass value: %i\n", cop_codeblocks[i].pass );
                free( *p_buffer_set );
                *p_buffer_set = NULL;
                free( cop_codeblocks );
                cop_codeblocks = NULL;
                free( in_driver );
                in_driver = NULL;
                return( in_driver );
            }

            /* Add the code_text struct. */

            if( in_driver->allocated_size < (in_driver->next_offset + sizeof( code_text )) ) {
                fontstyle_block_offset = (char *)fontstyle_block_ptr - (char *)in_driver;
                in_driver = resize_cop_driver( in_driver, sizeof( code_text ) );
                if( in_driver == NULL ) {
                    free( *p_buffer_set );
                    *p_buffer_set = NULL;
                    free( cop_codeblocks );
                    cop_codeblocks = NULL;
                    return( in_driver );
                }
                fontstyle_block_ptr = IN_DRV_MAP( fontstyle_block_offset );
                line_proc_ptr = IN_DRV_MAP( fontstyle_block_ptr->lineprocs );
            }
            code_text_ptr = IN_DRV_MAP_OFF();

            line_proc_ptr[cop_codeblocks[i].pass - 1].startword = IN_DRV_GET_OFF();
            IN_DRV_ADD_OFF( sizeof( code_text ) );

            /* Initialize the code_text struct. */

            size = code_text_ptr->count = cop_codeblocks[i].count;

            if( in_driver->allocated_size < (in_driver->next_offset + size) ) {
                fontstyle_block_offset = (char *)fontstyle_block_ptr - (char *)in_driver;
                in_driver = resize_cop_driver( in_driver, size );
                if( in_driver == NULL ) {
                    free( *p_buffer_set );
                    *p_buffer_set = NULL;
                    free( cop_codeblocks );
                    cop_codeblocks = NULL;
                    return( in_driver );
                }
                fontstyle_block_ptr = IN_DRV_MAP( fontstyle_block_offset );
                line_proc_ptr = IN_DRV_MAP( fontstyle_block_ptr->lineprocs );
                code_text_ptr = IN_DRV_MAP( line_proc_ptr[cop_codeblocks[i].pass - 1].startword );
            }
            text_ptr = IN_DRV_MAP_OFF();

            memcpy_s( text_ptr, size, cop_codeblocks[i].text, size );
            code_text_ptr->text = IN_DRV_GET_OFF();
            IN_DRV_ADD_OFF( size );

            /* Research code. */

            printf("  designator = %x, cb05_flag = %x, lp_flag = %x, pass = %x\n", cop_codeblocks[i].designator,
                   cop_codeblocks[i].cb05_flag, cop_codeblocks[i].lp_flag, cop_codeblocks[i].pass);
            if( (save_designator == 0x28) && (save_pass == cop_codeblocks[i].pass) ) {
                puts( " <-inversion!" );
            } else {
                puts("");
            }
            save_designator = cop_codeblocks[i].designator;
            save_pass = cop_codeblocks[i].pass;

            break;
        case 0x49 :

            /* A :FONTSTYLE :LINEPROC :FIRSTWORD block. */

            /* Ensure that the pass is within range. */

            if( cop_codeblocks[i].pass > fontstyle_block_ptr->passes ) {
                printf_s( ":FONTSTYLE :LINEPROC :FIRSTWORD block has incorrect pass value: %i\n", cop_codeblocks[i].pass );
                free( *p_buffer_set );
                *p_buffer_set = NULL;
                free( cop_codeblocks );
                cop_codeblocks = NULL;
                free( in_driver );
                in_driver = NULL;
                return( in_driver );
            }

            /* Add the code_text struct. */

            if( in_driver->allocated_size < (in_driver->next_offset + sizeof( code_text )) ) {
                fontstyle_block_offset = (char *)fontstyle_block_ptr - (char *)in_driver;
                in_driver = resize_cop_driver( in_driver, sizeof( code_text ) );
                if( in_driver == NULL ) {
                    free( *p_buffer_set );
                    *p_buffer_set = NULL;
                    free( cop_codeblocks );
                    cop_codeblocks = NULL;
                    return( in_driver );
                }
                fontstyle_block_ptr = IN_DRV_MAP( fontstyle_block_offset );
                line_proc_ptr = IN_DRV_MAP( fontstyle_block_ptr->lineprocs );
            }
            code_text_ptr = IN_DRV_MAP_OFF();

            line_proc_ptr[cop_codeblocks[i].pass - 1].firstword = IN_DRV_GET_OFF();
            IN_DRV_ADD_OFF( sizeof( code_text ) );

            /* Initialize the code_text struct. */

            size = code_text_ptr->count = cop_codeblocks[i].count;

            if( in_driver->allocated_size < (in_driver->next_offset + size) ) {
                fontstyle_block_offset = (char *)fontstyle_block_ptr - (char *)in_driver;
                in_driver = resize_cop_driver( in_driver, size );
                if( in_driver == NULL ) {
                    free( *p_buffer_set );
                    *p_buffer_set = NULL;
                    free( cop_codeblocks );
                    cop_codeblocks = NULL;
                    return( in_driver );
                }
                fontstyle_block_ptr = IN_DRV_MAP( fontstyle_block_offset );
                line_proc_ptr = IN_DRV_MAP( fontstyle_block_ptr->lineprocs );
                code_text_ptr = IN_DRV_MAP( line_proc_ptr[cop_codeblocks[i].pass - 1].firstword );
            }
            text_ptr = IN_DRV_MAP_OFF();

            memcpy_s( text_ptr, size, cop_codeblocks[i].text, size );
            code_text_ptr->text = IN_DRV_GET_OFF();
            IN_DRV_ADD_OFF( size );

            /* Research code. */

            printf("  designator = %x, cb05_flag = %x, lp_flag = %x, pass = %x\n", cop_codeblocks[i].designator,
                   cop_codeblocks[i].cb05_flag, cop_codeblocks[i].lp_flag, cop_codeblocks[i].pass);
            if( (save_designator == 0x08) && (save_pass == cop_codeblocks[i].pass) ) {
                puts( " <-inversion!" );
            } else {
                puts("");
            }
            save_designator = cop_codeblocks[i].designator;
            save_pass = cop_codeblocks[i].pass;

            break;
        default :
            printf_s( "  CodeBlock %i has unknown designator: %i\n", string_ptr, i, cop_codeblocks[i].designator );
            free( *p_buffer_set );
            *p_buffer_set = NULL;
            free(cop_codeblocks);
            cop_codeblocks = NULL;
            free( in_driver );
            in_driver = NULL;
            return( in_driver );
        }
    }

    /* Both in_driver and *p_buffer_set must be returned pointing to data. */

    free(cop_codeblocks);
    cop_codeblocks = NULL;

    return( in_driver );
}

/*  Function parse_init_block().
 *  Processes a single InitBlock. This may be called up to two times when
 *  parsing a .COP file encoding a :DRIVER block. 
 *
 *  Parameters:
 *      in_driver contains the cop_driver being initialized.
 *      *current contains the current position in a p_buffer.buffer.
 *      base points to the first byte of the underlying P-buffer.
 *
 *  Parameter modified:
 *      if the function succeeds, then *current will point to the first junk
 *          byte following the last CodeBlock for this InitBlock.
 *      if the function fails, the value of *current should be regarded as
 *          invalid.
 *
 *  Returns:
 *      in_driver, possibly relocated, on success.
 *      NULL on failure.
 */
static cop_driver * parse_init_block( cop_driver * in_driver, char **current, char *base )
{

    code_block *    cop_codeblocks  = NULL;
    init_block *    init_block_ptr  = NULL;
    init_text *     init_text_ptr   = NULL;
    int             i;
    uint8_t         designator;    
    uint8_t *       text_ptr        = NULL;
    uint16_t        count;
    size_t          size;

    /* Get the designator. */

    memcpy_s( &designator, 1, *current, 1 );
    (*current)++;

    /* Process the InitBlock. */

    switch( designator ) {
    case 0x01 :
        memcpy_s( &count, sizeof( count ), *current, sizeof( count ) );
        *current += sizeof( count );
        if( count == 0 ) {
            puts( "START :INIT block has no code!" );
            free( in_driver );
            in_driver = NULL;
            return( in_driver );
        }

        /* Add the init_block struct itself. */

        if( in_driver->allocated_size < (in_driver->next_offset + sizeof( init_block )) ) {
            in_driver = resize_cop_driver( in_driver, sizeof( init_block ) );
            if( in_driver == NULL ) return( in_driver );
        }
        init_block_ptr = IN_DRV_MAP_OFF();

        in_driver->inits.start = IN_DRV_GET_OFF();
        IN_DRV_ADD_OFF( sizeof( init_block ) );

        /* Get the CodeBlocks. */
        
        cop_codeblocks = get_code_blocks( current, count, base, "START :INIT" );
        if( cop_codeblocks == NULL ) {
            free( in_driver );
            in_driver = NULL;
            return( in_driver );
        }
        
        /* Add the init_text struct instances. */

        size = count * sizeof( init_text );
        if( in_driver->allocated_size < (in_driver->next_offset + size) ) {
            in_driver = resize_cop_driver( in_driver, size );
            if( in_driver == NULL ) {
                free( cop_codeblocks );
                cop_codeblocks = NULL;
                return( in_driver );
            }
            init_block_ptr = IN_DRV_MAP( in_driver->inits.start );
        }
        init_text_ptr = IN_DRV_MAP_OFF();

        init_block_ptr->count = count;
        init_block_ptr->codeblock = IN_DRV_GET_OFF();
        IN_DRV_ADD_OFF( size );

        /* Initialize the init_text struct instances. */

        for(i = 0; i < init_block_ptr->count; i++ ) {

            if( cop_codeblocks[i].designator == 0x02 ) {
                init_text_ptr[i].is_fontvalue = true;
            } else {
                if( cop_codeblocks[i].designator == 0x00 ) {
                    init_text_ptr[i].is_fontvalue = false;
                } else {
                    printf_s( "START :INIT block has unexpected CodeBlock designator: %i\n", designator );
                    free( cop_codeblocks );
                    cop_codeblocks = NULL;
                    free( in_driver );
                    in_driver = NULL;
                    return( in_driver );

                }
            }

            init_text_ptr[i].count = cop_codeblocks[i].count;

            size = init_text_ptr[i].count;
            if( in_driver->allocated_size < (in_driver->next_offset + size) ) {
                in_driver = resize_cop_driver( in_driver, size );
                if( in_driver == NULL ) {
                    free( cop_codeblocks );
                    cop_codeblocks = NULL;
                    return( in_driver );
                }
                init_block_ptr = IN_DRV_MAP( in_driver->inits.start );
                init_text_ptr = IN_DRV_MAP( init_block_ptr->codeblock );
            }
            text_ptr = IN_DRV_MAP_OFF();

            memcpy_s( text_ptr, size, cop_codeblocks[i].text, size );
            init_text_ptr[i].text = IN_DRV_GET_OFF();
            IN_DRV_ADD_OFF( size );

        }

        free( cop_codeblocks );
        cop_codeblocks = NULL;

        break;
    case 0x02 :
        memcpy_s( &count, sizeof( count ), *current, sizeof( count ) );
        *current += sizeof( count );
        if( count == 0 ) {
            puts( "DOCUMENT :INIT block has no code!" );
            free( in_driver );
            in_driver = NULL;
            return( in_driver );
        }

        /* Add the init_block struct itself. */

        if( in_driver->allocated_size < (in_driver->next_offset + sizeof( init_block )) ) {
            in_driver = resize_cop_driver( in_driver, sizeof( init_block ) );
            if( in_driver == NULL ) return( in_driver );
        }
        init_block_ptr = IN_DRV_MAP_OFF();

        in_driver->inits.document = IN_DRV_GET_OFF();
        IN_DRV_ADD_OFF( sizeof( init_block ) );

        /* Get the CodeBlocks. */
        
        cop_codeblocks = get_code_blocks( current, count, base, "DOCUMENT :INIT" );
        if( cop_codeblocks == NULL ) {
            free( in_driver );
            in_driver = NULL;
            return( in_driver );
        }
        
        /* Add the init_text struct instances. */

        size = count * sizeof( init_text );
        if( in_driver->allocated_size < (in_driver->next_offset + size) ) {
            in_driver = resize_cop_driver( in_driver, size );
            if( in_driver == NULL ) {
                free( cop_codeblocks );
                cop_codeblocks = NULL;
                return( in_driver );
            }
            init_block_ptr = IN_DRV_MAP( in_driver->inits.document );
        }

        init_text_ptr = IN_DRV_MAP_OFF();

        init_block_ptr->count = count;
        init_block_ptr->codeblock = IN_DRV_GET_OFF();
        IN_DRV_ADD_OFF( size );

        /* Initialize the init_text struct instances. */

        for(i = 0; i < init_block_ptr->count; i++ ) {

            if( cop_codeblocks[i].designator == 0x02 ) {
                init_text_ptr[i].is_fontvalue = true;
            } else {
                if( cop_codeblocks[i].designator == 0x00 ) {
                    init_text_ptr[i].is_fontvalue = false;
                } else {
                    printf_s( "DOCUMENT :INIT block has unexpected CodeBlock designator: %i\n", designator );
                    free( cop_codeblocks );
                    cop_codeblocks = NULL;
                    free( in_driver );
                    in_driver = NULL;
                    return( in_driver );
                }
            }

            init_text_ptr[i].count = cop_codeblocks[i].count;

            size = init_text_ptr[i].count;
            if( in_driver->allocated_size < (in_driver->next_offset + size) ) {
                in_driver = resize_cop_driver( in_driver, size );
                if( in_driver == NULL ) {
                    free( cop_codeblocks );
                    cop_codeblocks = NULL;
                    return( in_driver );
                }
                init_block_ptr = IN_DRV_MAP( in_driver->inits.document );
                init_text_ptr = IN_DRV_MAP( init_block_ptr->codeblock );
            }
            text_ptr = IN_DRV_MAP_OFF();

            memcpy_s( text_ptr, size, cop_codeblocks[i].text, size );
            init_text_ptr[i].text = IN_DRV_GET_OFF();
            IN_DRV_ADD_OFF( size );

        }

        free( cop_codeblocks );
        cop_codeblocks = NULL;

        break;
    default :
        printf_s( ":INIT block designator incorrect: %i\n", designator );
        free( in_driver );
        in_driver = NULL;
        return( in_driver );
    }    

    return( in_driver );
}

/*  Extern function definitions */

/*  Function is_drv_file().
 *  Determines whether or not in_file points to the start of a .COP driver
 *  file (the first byte after the header).
 *
 *  Parameter:
 *      in_file points to the presumed start of a .COP driver file.
 *
 *  Returns:
 *      true if this has the correct designator.
 *      false otherwise.
 */

bool is_drv_file( FILE * in_file)
{
    char descriminator[3];

    /* Get the descriminator. */

    fread( &descriminator, 3, 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) return( false );

    /* Verify that the descriminator is for a .COP driver file */

    if( memcmp( descriminator, "DRV", 3 ) ) return( false );
    
    return( true );
}

/* Function parse_driver().
 *  Constructs a cop_driver instance from the given input stream.
 *  
 *  Parameters:
 *      in_file points to the first byte of a .COP file encoding a :DRIVER
 *          struct after the "DEV" descriminator.
 *
 *  Returns:
 *      A pointer to a cop_driver struct containing the data from in_file
 *          on success.
 *      A NULL pointer on failure.
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
    char                *current                = NULL;
    uint8_t *           text_ptr                = NULL;

    /* Used for counts and related values. */

    uint8_t             count8;
    uint16_t            count16;

    /* Used in for loops. */

    int                 i;
    int                 j;

    size_t              size;

    /* Initialize the out_driver. */
        
    out_driver = malloc( START_SIZE );
    if( out_driver == NULL )
        return( out_driver );

    out_driver->allocated_size = START_SIZE;
    out_driver->next_offset = sizeof( cop_driver );

    /* Note: The various pointers must be entered, initially, as offsets
     * and then converted to pointers before returning because out_driver
     * may be reallocated at any point and that invalidates actual pointers.
     */

    /* Get the rec_spec. */

    fread( &length, sizeof( length ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }

    if( length > 0 ) {

        if( out_driver->allocated_size < (out_driver->next_offset + length) ) {
            out_driver = resize_cop_driver( out_driver, length );
            if( out_driver == NULL ) {
                return( out_driver );
            }
        }

        string_ptr = OUT_DRV_MAP_OFF();

        fread( string_ptr, length, 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_driver );
            out_driver = NULL;
            return( out_driver );
        }
        out_driver->rec_spec = OUT_DRV_GET_OFF();
        string_ptr[length] = '\0';
        OUT_DRV_ADD_OFF( length + 1 );
    } else {
        out_driver->rec_spec = NULL;
    }

    /* Get the unknown value and verify that it contains 0x04. */

    fread( &count8, sizeof( count8 ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }

    if( count8 != 0x04 ) {
        printf_s( "Bad count field for Attributes: %i\n", count8 );
        free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }

    /* Get the fill_char. */

    fread( &out_driver->fill_char, sizeof( out_driver->fill_char ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }

    /* Get the x_positive flag. */

    fread( &out_driver->x_positive, sizeof( out_driver->x_positive ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }

    /* Get the y_positive flag. */

    fread( &out_driver->y_positive, sizeof( out_driver->y_positive ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }

    /* Get the null byte and verify that it is, in fact, null. */

    fread( &count8, sizeof( count8 ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }

    if( count8 != 0x00 ) {
        printf_s( "Byte after y_positive flag should be null, not %i\n", count8 );
        free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }

    /* Get the first set of P-buffers. */

    p_buffer_set = get_p_buffer( in_file );
    if( p_buffer_set == NULL) {
        free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }

    if( p_buffer_set->buffer == NULL) {
        free( p_buffer_set );
        p_buffer_set = NULL;
        free( out_driver );
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
        out_driver = parse_init_block( out_driver, &current, p_buffer_set->buffer );
        if( out_driver == NULL ) {
            free( p_buffer_set );
            p_buffer_set = NULL;
            return( out_driver );
        }
        break;
    case 0x0002 :
        out_driver = parse_init_block( out_driver, &current, p_buffer_set->buffer );
        if( out_driver == NULL ) {
            free( p_buffer_set );
            p_buffer_set = NULL;
            return( out_driver );
        }

        /* Reset to the start of the next P-buffer's data. */

        if( (current - p_buffer_set->buffer) % 80 != 0 ) { 
            factor = (current - p_buffer_set->buffer) / 80;
            factor++;
            current = p_buffer_set->buffer + factor * 80;
        }

        out_driver = parse_init_block( out_driver, &current, p_buffer_set->buffer );
        if( out_driver == NULL ) {
            free( p_buffer_set );
            p_buffer_set = NULL;
            return( out_driver );
        }
        break;
    default:
        printf_s( "Too many InitBlocks: %i\n", count16 );
        free( p_buffer_set );
        p_buffer_set = NULL;
        free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }

    /* Reset to the start of the next P-buffer's data. */

    if( (current - p_buffer_set->buffer) % 80 != 0 ) { 
        factor = (current - p_buffer_set->buffer) / 80;
        factor++;
        current = p_buffer_set->buffer + factor * 80;
    }
    
    /* Parse the FinishFuncs */

    /* Get the number of FinishBlocks. */

    memcpy_s( &count16, sizeof( count16 ), current, sizeof( count16 ) );
    current += sizeof( count16 );

    out_driver->finishes.end = NULL;
    out_driver->finishes.document = NULL;

    switch( count16 ) {
    case 0x0000 :
        break;
    case 0x0001 :
        out_driver = parse_finish_block( out_driver, &current, p_buffer_set->buffer );
        if( out_driver == NULL ) {
            free( p_buffer_set );
            p_buffer_set = NULL;
            return( out_driver );
        }
        break;
    case 0x0002 :
        out_driver = parse_finish_block( out_driver, &current, p_buffer_set->buffer );
        if( out_driver == NULL ) {
            free( p_buffer_set );
            p_buffer_set = NULL;
            return( out_driver );
        }

        /* Reset to the start of the next P-buffer's data. */

        if( (current - p_buffer_set->buffer) % 80 != 0 ) { 
            factor = (current - p_buffer_set->buffer) / 80;
            factor++;
            current = p_buffer_set->buffer + factor * 80;
        }

        out_driver = parse_finish_block( out_driver, &current, p_buffer_set->buffer );
        if( out_driver == NULL ) {
            free( p_buffer_set );
            p_buffer_set = NULL;
            return( out_driver );
        }
        break;
    default:
        printf_s( "Too many FinishBlocks: %i\n", count16 );
        free( p_buffer_set );
        p_buffer_set = NULL;
        free( out_driver );
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

    /* Get the number of NewlineBlocks. */

    memcpy_s( &out_driver->newlines.count, sizeof( out_driver->newlines.count ),
                current, sizeof( out_driver->newlines.count ) );
    current += sizeof( out_driver->newlines.count );

    /* Add the newline_block structs. */

    size = out_driver->newlines.count * sizeof( newline_block );
    if( out_driver->allocated_size < (out_driver->next_offset + size) ) {
        out_driver = resize_cop_driver( out_driver, size );
        if( out_driver == NULL ) {
            free( p_buffer_set );
            p_buffer_set = NULL;
            return( out_driver );
        }
    }
    newline_block_ptr = OUT_DRV_MAP_OFF();

    out_driver->newlines.newlineblocks = OUT_DRV_GET_OFF();
    OUT_DRV_ADD_OFF( size );

    /* Initialize the newline_block structs. */

    for( i = 0; i < out_driver->newlines.count; i++ ) {

        /* Get the advance for the current NewlineBlock. */

        memcpy_s( &newline_block_ptr[i].advance, sizeof( newline_block_ptr[i].advance ),
                    current, sizeof( newline_block_ptr[i].advance ) );
        current += sizeof( newline_block_ptr[i].advance );

        /* Get the number of CodeBlocks, and verify that it is 0x01. */

        memcpy_s( &count16, sizeof( count16 ), current, sizeof( count16 ) );
        current += sizeof( count16 );

        if( count16 != 0x0001 ) {
            printf_s( ":NEWLINE block for advance %i has unexpected number of CodeBlocks: %i\n",
                        newline_block_ptr[i].advance, count16);
            free( p_buffer_set );
            p_buffer_set = NULL;
            free( out_driver );
            out_driver = NULL;
            return( out_driver );
        }

        /* Get the CodeBlock. */
        
        cop_codeblocks = get_code_blocks( &current, count16, p_buffer_set->buffer, ":NEWLINE" );
        if( cop_codeblocks == NULL ) {
            free( p_buffer_set );
            p_buffer_set = NULL;
            free( out_driver );
            out_driver = NULL;
            return( out_driver );
        }

        if( cop_codeblocks->designator != 0x00 ) {
            printf_s( "NewlineBlock CodeBlock for advance %i has unexpected designator: %i\n",
                        newline_block_ptr[i].advance, cop_codeblocks[i].designator );
            free( p_buffer_set );
            p_buffer_set = NULL;
            free( cop_codeblocks );
            cop_codeblocks = NULL;
            free( out_driver );
            out_driver = NULL;
            return( out_driver );
        }

        newline_block_ptr[i].count = cop_codeblocks->count;

        size = newline_block_ptr[i].count;
        if( out_driver->allocated_size < (out_driver->next_offset + size) ) {
            out_driver= resize_cop_driver( out_driver, size );
            if( out_driver == NULL ) {
                free( p_buffer_set );
                p_buffer_set = NULL;
                free( cop_codeblocks );
                cop_codeblocks = NULL;
                return( out_driver );
            }
            newline_block_ptr = OUT_DRV_MAP( out_driver->newlines.newlineblocks );
        }
        text_ptr = OUT_DRV_MAP_OFF();

        memcpy_s( text_ptr, size, cop_codeblocks->text, size );
        newline_block_ptr[i].text = OUT_DRV_GET_OFF();
        OUT_DRV_ADD_OFF( size );

        free(cop_codeblocks);
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
        printf_s( "Unknown block has non-zero count: %i\n", count16 );
        free( p_buffer_set );
        p_buffer_set = NULL;
        free( out_driver );
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

    cop_functions = parse_functions_block( &current, p_buffer_set->buffer, ":NEWPAGE" );

    /* Verify that the number of CodeBlocks is 1. */

    if( cop_functions->count != 0x0001 ) {
        printf_s( ":NEWPAGE block has unexpected number of CodeBlocks: %i\n", cop_functions->count );
        free( p_buffer_set );
        p_buffer_set = NULL;
        if( cop_functions->code_blocks != NULL ) {
            free( cop_functions->code_blocks );
            cop_functions->code_blocks = NULL;
        }
        free( cop_functions );
        cop_functions = NULL;
        free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }

    size = out_driver->newpage.count = cop_functions->code_blocks->count;

    if( out_driver->allocated_size < (out_driver->next_offset + size) ) {
        out_driver= resize_cop_driver( out_driver, size );
        if( out_driver == NULL ) {
            free( p_buffer_set );
            p_buffer_set = NULL;
            if( cop_functions->code_blocks != NULL ) {
                free( cop_functions->code_blocks );
                cop_functions->code_blocks = NULL;
            }
            free( cop_functions );
            cop_functions = NULL;
            return( out_driver );
        }
    }
    text_ptr = OUT_DRV_MAP_OFF();

    memcpy_s( text_ptr, size, cop_functions->code_blocks->text, size );
    out_driver->newpage.text = OUT_DRV_GET_OFF();
    OUT_DRV_ADD_OFF( size );

    free( cop_functions->code_blocks );
    cop_functions->code_blocks = NULL;
    free( cop_functions );
    cop_functions = NULL;

    /* Reset to the start of the next P-buffer's data. */

    if( (current - p_buffer_set->buffer) % 80 != 0 ) { 
        factor = (current - p_buffer_set->buffer) / 80;
        factor++;
        current = p_buffer_set->buffer + factor * 80;
    }

    /* Parse the htab FunctionsBlock. */

    cop_functions = parse_functions_block( &current, p_buffer_set->buffer, ":HTAB" );

    /* This block is optional: a count of 0 is allowed. */

    switch( cop_functions->count ) {
    case 0x0000 :
        out_driver->htab.count = 0;
        out_driver->htab.text = NULL;
        break;
    case 0x0001 :
        size = out_driver->htab.count = cop_functions->code_blocks->count;

        if( out_driver->allocated_size < (out_driver->next_offset + size) ) {
            out_driver= resize_cop_driver( out_driver, size );
            if( out_driver == NULL ) {
                free( p_buffer_set );
                p_buffer_set = NULL;
                free( cop_functions );
                cop_functions = NULL;
                return( out_driver );
            }
        }
        text_ptr = OUT_DRV_MAP_OFF();

        memcpy_s( text_ptr, size, cop_functions->code_blocks->text, size );
        out_driver->htab.text = OUT_DRV_GET_OFF();
        OUT_DRV_ADD_OFF( size );
        break;
    default :
        printf_s( ":HTAB block has unexpected number of CodeBlocks: %i\n", cop_functions->count );
        free( p_buffer_set );
        p_buffer_set = NULL;
        free( cop_functions->code_blocks );
        cop_functions->code_blocks = NULL;
        free( cop_functions );
        cop_functions = NULL;
        free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }

    if( cop_functions->code_blocks != NULL ) {
        free( cop_functions->code_blocks );
        cop_functions->code_blocks = NULL;
    }
    free( cop_functions );
    cop_functions = NULL;

    /* Reset to the start of the next P-buffer's data. */

    if( (current - p_buffer_set->buffer) % 80 != 0 ) { 
        factor = (current - p_buffer_set->buffer) / 80;
        factor++;
        current = p_buffer_set->buffer + factor * 80;
    }

    /* Parse the FontswitchFuncs. */

    /* This block is optional: a count of 0 is allowed. */

    memcpy_s( &out_driver->fontswitches.count, sizeof( out_driver->fontswitches.count ),
                current, sizeof( out_driver->fontswitches.count ) );
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

        size = out_driver->fontswitches.count * sizeof( fontswitch_block );
        if( out_driver->allocated_size < (out_driver->next_offset + size) ) {
            out_driver = resize_cop_driver( out_driver, size );
            if( out_driver == NULL ) {
                free( p_buffer_set );
                p_buffer_set = NULL;
                return( out_driver );
            }
        }
        fontswitch_block_ptr = OUT_DRV_MAP_OFF();

        out_driver->fontswitches.fontswitchblocks = OUT_DRV_GET_OFF();
        OUT_DRV_ADD_OFF( size );

        /* Now get the FontswitchBlock instances. */

        for( i = 0; i < out_driver->fontswitches.count; i++ ) {

            /* The type is a null-terminated character string. */
            
            length = strlen( current );
            
            if( length == 0 ) {
                printf_s( ":FONTSWITCH block %i has no 'type'\n", i );
                free( p_buffer_set );
                p_buffer_set = NULL;
                free( out_driver );
                out_driver = NULL;
                return( out_driver );
            }

            length++;
            if( out_driver->allocated_size < (out_driver->next_offset + length) ) {
                out_driver = resize_cop_driver( out_driver, length );
                if( out_driver == NULL ) {
                    free( p_buffer_set );
                    p_buffer_set = NULL;
                    return( out_driver );
                }
                fontswitch_block_ptr = OUT_DRV_MAP( out_driver->fontswitches.fontswitchblocks );
            }

            string_ptr = OUT_DRV_MAP_OFF();
            strcpy_s( string_ptr, length, current );
            current += length;

            fontswitch_block_ptr[i].type = OUT_DRV_GET_OFF();
            OUT_DRV_ADD_OFF( length );

            /* Skip the flags. */

            current += 21;

            /* Get the number of CodeBlocks; only 1 or 2 is valid. */

            memcpy_s( &count16, sizeof( count16 ), current, sizeof( count16 ) );
            current += sizeof( count16 );

            if( (count16 == 0x00) || (count16 > 0x02) ) {
                printf_s( ":FONTSWITCH block %s has an unexpected number of CodeBlocks: %i\n",
                            string_ptr, count16 );
                free( p_buffer_set );
                p_buffer_set = NULL;
                free( out_driver );
                out_driver = NULL;
                return( out_driver );
            } else {
            
                /* Get the CodeBlock(s). */
        
                cop_codeblocks = get_code_blocks( &current, count16, p_buffer_set->buffer, ":FONTSWITCH" );
                if( cop_codeblocks == NULL ) {
                    free( p_buffer_set );
                    p_buffer_set = NULL;
                    free( out_driver );
                    out_driver = NULL;
                    return( out_driver );
                }

                /* Process the CodeBlocks. */
            
                fontswitch_block_ptr[i].startvalue = NULL;
                fontswitch_block_ptr[i].endvalue = NULL;

                for( j = 0; j < count16; j++ ) {
                    switch( cop_codeblocks[j].designator) {
                    case 0x04:

                        /* Add the code_text struct for endvalue. */

                        if( out_driver->allocated_size < (out_driver->next_offset + sizeof( code_text )) ) {
                            out_driver = resize_cop_driver( out_driver, sizeof( code_text ) );
                            if( out_driver == NULL ) {
                                free( p_buffer_set );
                                p_buffer_set = NULL;
                                free( cop_codeblocks );
                                cop_codeblocks = NULL;
                                return( out_driver );
                            }
                            fontswitch_block_ptr = OUT_DRV_MAP( out_driver->fontswitches.fontswitchblocks );
                        }
                        code_text_ptr = OUT_DRV_MAP_OFF();

                        fontswitch_block_ptr[i].endvalue = OUT_DRV_GET_OFF();
                        OUT_DRV_ADD_OFF( sizeof( code_text ) );

                        /* Now get the CodeBlock. */

                        size = code_text_ptr->count = cop_codeblocks[j].count;
                
                        if( out_driver->allocated_size < (out_driver->next_offset + size) ) {
                            out_driver = resize_cop_driver( out_driver, size );
                            if( out_driver == NULL ) {
                                free( p_buffer_set );
                                p_buffer_set = NULL;
                                free( cop_codeblocks );
                                cop_codeblocks = NULL;
                                return( out_driver );
                            }
                            fontswitch_block_ptr = OUT_DRV_MAP( out_driver->fontswitches.fontswitchblocks );
                            code_text_ptr = OUT_DRV_MAP( fontswitch_block_ptr[i].endvalue );
                        }
                        text_ptr = OUT_DRV_MAP_OFF();

                        memcpy_s( text_ptr, size, cop_codeblocks[j].text, size );
                        code_text_ptr->text = OUT_DRV_GET_OFF();
                        OUT_DRV_ADD_OFF( size );

                        break;
                    case 0x05:

                        /* Add the code_text struct for startvalue. */

                        if( out_driver->allocated_size < (out_driver->next_offset + sizeof( code_text )) ) {
                            out_driver= resize_cop_driver( out_driver, sizeof( code_text ) );
                            if( out_driver == NULL ) {
                                free( p_buffer_set );
                                p_buffer_set = NULL;
                                free( cop_codeblocks );
                                cop_codeblocks = NULL;
                                return( out_driver );
                            }
                            fontswitch_block_ptr = OUT_DRV_MAP( out_driver->fontswitches.fontswitchblocks );
                        }
                        code_text_ptr = OUT_DRV_MAP_OFF();

                        fontswitch_block_ptr[i].startvalue = OUT_DRV_GET_OFF();
                        OUT_DRV_ADD_OFF( sizeof( code_text ) );

                        /* Now get the CodeBlock. */

                        size = code_text_ptr->count = cop_codeblocks[j].count;
                
                        if( out_driver->allocated_size < (out_driver->next_offset + size) ) {
                            out_driver = resize_cop_driver( out_driver, size );
                            if( out_driver == NULL ) {
                                free( p_buffer_set );
                                p_buffer_set = NULL;
                                free( cop_codeblocks );
                                cop_codeblocks = NULL;
                                return( out_driver );
                            }
                            fontswitch_block_ptr = OUT_DRV_MAP( out_driver->fontswitches.fontswitchblocks );
                            code_text_ptr = OUT_DRV_MAP( fontswitch_block_ptr[i].startvalue );
                        }
                        text_ptr = OUT_DRV_MAP_OFF();

                        memcpy_s( text_ptr, size, cop_codeblocks[j].text, size );
                        code_text_ptr->text = OUT_DRV_GET_OFF();
                        OUT_DRV_ADD_OFF( size );

                        break;
                    default:
                        printf_s( ":FONTSWITCH block %s has unexpected CodeBlock designator: %i\n", string_ptr, cop_codeblocks[i].designator );
                        free( p_buffer_set );
                        p_buffer_set = NULL;
                        free( cop_codeblocks );
                        cop_codeblocks = NULL;
                        free( out_driver );
                        out_driver = NULL;
                        return( out_driver );
                    }
                }
            }

            free(cop_codeblocks);
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
            puts( "Problem rewinding file before processing FontstyleGroup" );
            free( p_buffer_set );
            p_buffer_set = NULL;
            free( out_driver );
            out_driver = NULL;
            return( out_driver );
        }
    } 

    free( p_buffer_set );
    p_buffer_set = NULL;
    
    /* Parse the FontstyleGroup. */

    /* Get the data_count and ensure it is not 0. */
    
    fread( &count8, sizeof( count8 ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }

    if( count8 == 0x00 ) {
        puts( "Initial ShortFontstyleBlock data length was 0: invalid file" );
        free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }

    /* Get the fontstyle_count and ensure it is not 0. */

    fread( &out_driver->fontstyles.count, sizeof( out_driver->fontstyles.count ), 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }

    if( out_driver->fontstyles.count == 0x00 ) {
        puts( "No ShortFontstyleBlocks found: at least one ('plain') must exist" );
        free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }
    count8 -= sizeof( out_driver->fontstyles.count );

    /* Add the fontstyle_block struct instances. */

    size = out_driver->fontstyles.count * sizeof( fontstyle_block );
    if( out_driver->allocated_size < (out_driver->next_offset + size) ) {
        out_driver = resize_cop_driver( out_driver, size );
        if( out_driver == NULL ) return( out_driver );
    }
    fontstyle_block_ptr = OUT_DRV_MAP_OFF();

    out_driver->fontstyles.fontstyleblocks = OUT_DRV_GET_OFF();
    OUT_DRV_ADD_OFF( size );

    /* Initialize the fontstyle_block struct instances. */

    /* Set the pointers in the fontstyle_block struct instances to NULL */

    for( i = 0; i < out_driver->fontstyles.count; i++ ) {
        fontstyle_block_ptr[i].startvalue = NULL;
        fontstyle_block_ptr[i].endvalue = NULL;
        fontstyle_block_ptr[i].lineprocs = NULL;
    }

    /* Note: see the Wiki for the file structure. It is a little odd. */

    /* The initial ShortFontstyleBlock must be processed separately. */
    
    out_driver = parse_font_style( in_file, out_driver, fontstyle_block_ptr, &p_buffer_set, &current, count8 );
    if( out_driver == NULL )
        return( out_driver );
    fontstyle_block_ptr = OUT_DRV_MAP( out_driver->fontstyles.fontstyleblocks );

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
                printf_s( "Problem rewinding file before processing FontstyleBlock %i\n", i );
                free( p_buffer_set );
                p_buffer_set = NULL;
                free( out_driver );
                out_driver = NULL;
                return( out_driver );
            }
        } 
        free(p_buffer_set);
        p_buffer_set = NULL;
        
        /* Get the data_count and ensure it is not 0. */
    
        fread( &count8, sizeof( count8 ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_driver );
            out_driver = NULL;
            return( out_driver );
        }

        if( count8 == 0x00 ) {
            printf_s( "FontstyleBlock %i data length was 0: invalid file\n", i + 1 );
            free( out_driver );
            out_driver = NULL;
            return( out_driver );
        }

        out_driver = parse_font_style( in_file, out_driver, &fontstyle_block_ptr[i], &p_buffer_set, &current, count8 );
        if( out_driver == NULL )
            return( out_driver );
        fontstyle_block_ptr = OUT_DRV_MAP( out_driver->fontstyles.fontstyleblocks );
    }
    
    /* Reset to the start of the next P-buffer's data. */

    if( (current - p_buffer_set->buffer) % 80 != 0 ) { 
        factor = (current - p_buffer_set->buffer) / 80;
        factor++;
        current = p_buffer_set->buffer + factor * 80;
    }

    /* Parse the absoluteaddress FunctionsBlock. */

    cop_functions = parse_functions_block( &current, p_buffer_set->buffer,
                                           ":ABSOLUTEADDRESS" );

    /* The number of CodeBlocks may be 0 or 1. */

    switch( cop_functions->count ) {
    case 0x0000 :
        out_driver->absoluteaddress.count = 0;
        out_driver->absoluteaddress.text = NULL;
        break;
    case 0x0001 :
        size = out_driver->absoluteaddress.count = cop_functions->code_blocks->count;

        if( out_driver->allocated_size < (out_driver->next_offset + size) ) {
            out_driver = resize_cop_driver( out_driver, size );
            if( out_driver == NULL ) {
                free( p_buffer_set );
                p_buffer_set = NULL;
                free( cop_functions->code_blocks );
                cop_functions->code_blocks = NULL;
                free( cop_functions );
                cop_functions = NULL;
                return( out_driver );
            }
        }
        text_ptr = OUT_DRV_MAP_OFF();

        memcpy_s( text_ptr, size, cop_functions->code_blocks->text, size );
        out_driver->absoluteaddress.text = OUT_DRV_GET_OFF();
        OUT_DRV_ADD_OFF( size );
        break;
    default:
        printf_s( ":ABSOLUTEADDRESS block has unexpected number of CodeBlocks: %i\n", cop_functions->count );
        free( p_buffer_set );
        p_buffer_set = NULL;
        free( cop_functions->code_blocks );
        cop_functions->code_blocks = NULL;
        free( cop_functions );
        cop_functions = NULL;
        free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }

    if( cop_functions->code_blocks != NULL ) {
        free( cop_functions->code_blocks );
        cop_functions->code_blocks = NULL;
    }
    free( cop_functions );
    cop_functions = NULL;

    /* Reset to the start of the next P-buffer's data. */

    if( (current - p_buffer_set->buffer) % 80 != 0 ) { 
        factor = (current - p_buffer_set->buffer) / 80;
        factor++;
        current = p_buffer_set->buffer + factor * 80;
    }

    /* Parse the HlineBlock. */

    /* First parse the FunctionsBlock. */

    cop_functions = parse_functions_block( &current, p_buffer_set->buffer, ":HLINE" );

    /* The number of CodeBlocks may be 0 or 1. */

    switch( cop_functions->count ) {
    case 0x0000 :
        out_driver->hline.count = 0;
        out_driver->hline.text = NULL;
        break;
    case 0x0001 :
        size = out_driver->hline.count = cop_functions->code_blocks->count;

        if( out_driver->allocated_size < (out_driver->next_offset + size) ) {
            out_driver= resize_cop_driver( out_driver, size );
            if( out_driver == NULL ) {
                free( p_buffer_set );
                p_buffer_set = NULL;
                free( cop_functions->code_blocks );
                cop_functions->code_blocks = NULL;
                free( cop_functions );
                cop_functions = NULL;
                return( out_driver );
            }
        }
        text_ptr = OUT_DRV_MAP_OFF();

        memcpy_s( text_ptr, size, cop_functions->code_blocks->text, size );
        out_driver->hline.text = OUT_DRV_GET_OFF();
        OUT_DRV_ADD_OFF( size );
        break;
    default:
        printf_s( ":HLINE block has unexpected number of CodeBlocks: %i\n", cop_functions->count );
        free( p_buffer_set );
        p_buffer_set = NULL;
        free( cop_functions->code_blocks );
        cop_functions->code_blocks = NULL;
        free( cop_functions );
        cop_functions = NULL;
        free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }

    if( cop_functions->code_blocks != NULL ) {
        free( cop_functions->code_blocks );
        cop_functions->code_blocks = NULL;
    }
    free( cop_functions );
    cop_functions = NULL;

    /* The thickness is present only if the HlineBlock was present. */

    if( out_driver->hline.text == NULL) {
        
        /* Reset to the start of the next P-buffer's data */

        if( (current - p_buffer_set->buffer) % 80 != 0 ) { 
            factor = (current - p_buffer_set->buffer) / 80;
            factor++;
            current = p_buffer_set->buffer + factor * 80;
        }
    } else {

        /* The thickness halted the set of P-buffers so it is exhausted. */
        
        free( p_buffer_set );
        p_buffer_set = NULL;

        /* Get the thickness */
    
        fread( &count8, sizeof( count8 ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_driver );
            out_driver = NULL;
            return( out_driver );
        }
    
        switch( count8 ) {
        case 0x02:

            /* Get the 16-bit thickness. */

            fread( &count16, sizeof( count16 ), 1, in_file );
            if( ferror( in_file ) || feof( in_file ) ) {
                free( out_driver );
                out_driver = NULL;
                return( out_driver );
            }

            out_driver->hline.thickness  = count16;

            break;
        case 0x04:

            /* Get the 32-bit thickness. */

            fread( &out_driver->hline.thickness, sizeof( out_driver->hline.thickness ), 1, in_file );
            if( ferror( in_file ) || feof( in_file ) ) {
                free( out_driver );
                out_driver = NULL;
                return( out_driver );
            }

            break;
        default:
            printf_s( "Bad size for :HLINE thickness value: %i\n", count8 );
            free( out_driver );
            out_driver = NULL;
            return( out_driver );
        }
    }
    
    /* Parse the VlineBlock. */

    /* If there was an HlineBlock, then get the next set of P-buffers. */

    if( out_driver->hline.text != NULL) {
        p_buffer_set = get_p_buffer( in_file );
        if( p_buffer_set == NULL) {
            free( out_driver );
            out_driver = NULL;
            return( out_driver );
        }

        if( p_buffer_set->buffer == NULL) {
            free( p_buffer_set );
            p_buffer_set = NULL;
            free( out_driver );
            out_driver = NULL;
            return( out_driver );
        }

        current = p_buffer_set->buffer;
    }

    /* Now parse the FunctionsBlock. */

    cop_functions = parse_functions_block( &current, p_buffer_set->buffer,
                                                                    ":VLINE" );

    /* The number of CodeBlocks may be 0 or 1. */

    switch( cop_functions->count ) {
    case 0x0000 :
        out_driver->vline.count = 0;
        out_driver->vline.text = NULL;
        break;
    case 0x0001 :
        size = out_driver->vline.count = cop_functions->code_blocks->count;

        if( out_driver->allocated_size < (out_driver->next_offset + size) ) {
            out_driver= resize_cop_driver( out_driver, size );
            if( out_driver == NULL ) {
                free( p_buffer_set );
                p_buffer_set = NULL;
                free( cop_functions->code_blocks );
                cop_functions->code_blocks = NULL;
                free( cop_functions );
                cop_functions = NULL;
                return( out_driver );
            }
        }
        text_ptr = OUT_DRV_MAP_OFF();

        memcpy_s( text_ptr, size, cop_functions->code_blocks->text, size );
        out_driver->vline.text = OUT_DRV_GET_OFF();
        OUT_DRV_ADD_OFF( size );
        break;
    default:
        printf_s( ":VLINE block has unexpected number of CodeBlocks: %i\n", cop_functions->count );
        free( p_buffer_set );
        p_buffer_set = NULL;
        free( cop_functions->code_blocks );
        cop_functions->code_blocks = NULL;
        free( cop_functions );
        cop_functions = NULL;
        free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }

    if( cop_functions->code_blocks != NULL ) {
        free( cop_functions->code_blocks );
        cop_functions->code_blocks = NULL;
    }
    free( cop_functions );
    cop_functions = NULL;

    /* The thickness is present only if the VlineBlock was present. */

    if( out_driver->vline.text == NULL ) {
        
        /* Reset to the start of the next P-buffer's data. */

        if( (current - p_buffer_set->buffer) % 80 != 0 ) { 
            factor = (current - p_buffer_set->buffer) / 80;
            factor++;
            current = p_buffer_set->buffer + factor * 80;
        }
    } else {

        /* The thickness halted the set of P-buffers so it is exhausted. */
        
        free( p_buffer_set );
        p_buffer_set = NULL;

        /* Get the thickness. */
    
        fread( &count8, sizeof( count8 ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_driver );
            out_driver = NULL;
            return( out_driver );
        }
    
        switch( count8 ) {
        case 0x02:

            /* Get the 16-bit thickness. */

            fread( &count16, sizeof( count16 ), 1, in_file );
            if( ferror( in_file ) || feof( in_file ) ) {
                free( out_driver );
                out_driver = NULL;
                return( out_driver );
            }

            out_driver->vline.thickness  = count16;

            break;
        case 0x04:

            /* Get the 32-bit thickness. */

            fread( &out_driver->vline.thickness, sizeof( out_driver->vline.thickness ), 1, in_file );
            if( ferror( in_file ) || feof( in_file ) ) {
                free( out_driver );
                out_driver = NULL;
                return( out_driver );
            }

            break;
        default:
            printf_s( "Bad size for :VLINE thickness value: %i\n", count8 );
            free( out_driver );
            out_driver = NULL;
            return( out_driver );
        }
    }
    
    /* Parse the DboxBlock. */

    /* If there was a VlineBlock, then get the final set of P-buffers. */

    if( out_driver->vline.text != NULL) {

        /* Get the set of P-buffers. */

        p_buffer_set = get_p_buffer( in_file );
        if( p_buffer_set == NULL) {
            free( out_driver );
            out_driver = NULL;
            return( out_driver );
        }

        if( p_buffer_set->buffer == NULL) {
            free( p_buffer_set );
            p_buffer_set = NULL;
            free( out_driver );
            out_driver = NULL;
            return( out_driver );
        }

        current = p_buffer_set->buffer;
    }
    
    /* Now parse the FunctionsBlock. */

    cop_functions = parse_functions_block( &current, p_buffer_set->buffer, ":DBOX" );

    /* The number of CodeBlocks may be 0 or 1. */

    switch( cop_functions->count ) {
    case 0x0000 :
        out_driver->dbox.count = 0;
        out_driver->dbox.text = NULL;
        break;
    case 0x0001 :
        size = out_driver->dbox.count = cop_functions->code_blocks->count;

        if( out_driver->allocated_size < (out_driver->next_offset + size) ) {
            out_driver= resize_cop_driver( out_driver, size );
            if( out_driver == NULL ) {
                free( p_buffer_set );
                p_buffer_set = NULL;
                free( cop_functions->code_blocks );
                cop_functions->code_blocks = NULL;
                free( cop_functions );
                cop_functions = NULL;
                return( out_driver );
            }
        }
        text_ptr = OUT_DRV_MAP_OFF();

        memcpy_s( text_ptr, size, cop_functions->code_blocks->text, size );
        out_driver->dbox.text = OUT_DRV_GET_OFF();
        OUT_DRV_ADD_OFF( size );
        break;
    default:
        printf_s( ":DBOX block has unexpected number of CodeBlocks: %i\n", cop_functions->count );
        free( p_buffer_set );
        p_buffer_set = NULL;
        free( cop_functions->code_blocks );
        cop_functions->code_blocks = NULL;
        free( cop_functions );
        cop_functions = NULL;
        free( out_driver );
        out_driver = NULL;
        return( out_driver );
    }

    /* DboxBlock is the last item in the .COP file: the P-buffers are ended. */

    free( p_buffer_set );
    p_buffer_set = NULL;
    if( cop_functions->code_blocks != NULL ) {
        free( cop_functions->code_blocks );
        cop_functions->code_blocks = NULL;
    }
    free( cop_functions );
    cop_functions = NULL;

    /* The thickness is present only if DboxBlock was present. */

    if( out_driver->dbox.text != NULL) {
        
        /* Get the thickness. */
    
        fread( &count8, sizeof( count8 ), 1, in_file );
        if( ferror( in_file ) || feof( in_file ) ) {
            free( out_driver );
            out_driver = NULL;
            return( out_driver );
        }
    
        switch( count8 ) {
        case 0x02:

            /* Get the 16-bit thickness. */

            fread( &count16, sizeof( count16 ), 1, in_file );
            if( ferror( in_file ) || feof( in_file ) ) {
                free( out_driver );
                out_driver = NULL;
                return( out_driver );
            }

            out_driver->dbox.thickness  = count16;

            break;
        case 0x04:

            /* Get the 32-bit thickness. */

            fread( &out_driver->dbox.thickness, sizeof( out_driver->dbox.thickness ), 1, in_file );
            if( ferror( in_file ) || feof( in_file ) ) {
                free( out_driver );
                out_driver = NULL;
                return( out_driver );
            }

            break;
        default:
            printf_s( "Bad size for :DBOX thickness value: %i\n", count8 );
            free( out_driver );
            out_driver = NULL;
            return( out_driver );
        }
    }

    /* Convert non-NULL offsets to pointers */

    if( out_driver->rec_spec != NULL ) {
        OUT_DRV_REMAP( rec_spec );
    }

    if( out_driver->inits.start != NULL ) {
        OUT_DRV_REMAP( inits.start );
        if(out_driver->inits.start->codeblock != NULL ) {
            OUT_DRV_REMAP( inits.start->codeblock );
            for( i = 0; i < out_driver->inits.start->count; i++ ) {
                if( out_driver->inits.start->codeblock[i].text != NULL ) {
                    OUT_DRV_REMAP( inits.start->codeblock[i].text );
                }
            }
        }
    }
    
    if( out_driver->inits.document != NULL ) {
        OUT_DRV_REMAP( inits.document );
        if(out_driver->inits.document->codeblock != NULL ) {
            OUT_DRV_REMAP( inits.document->codeblock );
            for( i = 0; i < out_driver->inits.document->count; i++ ) {
                if( out_driver->inits.document->codeblock[i].text != NULL ) {
                    OUT_DRV_REMAP( inits.document->codeblock[i].text );
                }
            }
        }
    }
    
    if( out_driver->finishes.end != NULL ) {
        OUT_DRV_REMAP( finishes.end );
        if(out_driver->finishes.end->text != NULL ) {
            OUT_DRV_REMAP( finishes.end->text );
        }
    }
    
    if( out_driver->finishes.document != NULL ) {
        OUT_DRV_REMAP( finishes.document );
        if(out_driver->finishes.document->text != NULL ) {
            OUT_DRV_REMAP( finishes.document->text );
        }
    }
    
    if( out_driver->newlines.newlineblocks != NULL ) {
        OUT_DRV_REMAP( newlines.newlineblocks );
        for( i = 0; i < out_driver->newlines.count; i++ ) {
            if(out_driver->newlines.newlineblocks[i].text != NULL ) {
                OUT_DRV_REMAP( newlines.newlineblocks[i].text );
            }
        }
    }

    if( out_driver->newpage.text != NULL ) {
        OUT_DRV_REMAP( newpage.text );
    }

    if( out_driver->htab.text != NULL ) {
        OUT_DRV_REMAP( htab.text );
    }

    if( out_driver->fontswitches.fontswitchblocks != NULL ) {
        OUT_DRV_REMAP( fontswitches.fontswitchblocks );
        for( i = 0; i < out_driver->fontswitches.count; i++ ) {
            if( out_driver->fontswitches.fontswitchblocks[i].type != NULL ) {
                OUT_DRV_REMAP( fontswitches.fontswitchblocks[i].type );
            }
            if( out_driver->fontswitches.fontswitchblocks[i].startvalue != NULL ) {
                OUT_DRV_REMAP( fontswitches.fontswitchblocks[i].startvalue );
                if( out_driver->fontswitches.fontswitchblocks[i].startvalue->text != NULL ) {
                    OUT_DRV_REMAP( fontswitches.fontswitchblocks[i].startvalue->text );
                }
            }
            if( out_driver->fontswitches.fontswitchblocks[i].endvalue != NULL ) {
                OUT_DRV_REMAP( fontswitches.fontswitchblocks[i].endvalue );
                if( out_driver->fontswitches.fontswitchblocks[i].endvalue->text != NULL ) {
                    OUT_DRV_REMAP( fontswitches.fontswitchblocks[i].endvalue->text );
                }
            }
        }
    }

    if( out_driver->fontstyles.fontstyleblocks != NULL ) {
        OUT_DRV_REMAP( fontstyles.fontstyleblocks );
        for( i = 0; i < out_driver->fontstyles.count; i++ ) {
            if( out_driver->fontstyles.fontstyleblocks[i].type != NULL ) {
                OUT_DRV_REMAP( fontstyles.fontstyleblocks[i].type );
            }
            if( out_driver->fontstyles.fontstyleblocks[i].startvalue != NULL ) {
                OUT_DRV_REMAP( fontstyles.fontstyleblocks[i].startvalue );
                if( out_driver->fontstyles.fontstyleblocks[i].startvalue->text != NULL ) {
                    OUT_DRV_REMAP( fontstyles.fontstyleblocks[i].startvalue->text );
                }
            }
            if( out_driver->fontstyles.fontstyleblocks[i].endvalue != NULL ) {
                OUT_DRV_REMAP( fontstyles.fontstyleblocks[i].endvalue );
                if( out_driver->fontstyles.fontstyleblocks[i].endvalue->text != NULL ) {
                    OUT_DRV_REMAP( fontstyles.fontstyleblocks[i].endvalue->text );
                }
            }
            if( out_driver->fontstyles.fontstyleblocks[i].lineprocs != NULL ) {
                OUT_DRV_REMAP( fontstyles.fontstyleblocks[i].lineprocs );
                for( j = 0; j < out_driver->fontstyles.fontstyleblocks[i].passes; j++ ) {
                    if( out_driver->fontstyles.fontstyleblocks[i].lineprocs[j].startvalue != NULL ) {
                        OUT_DRV_REMAP( fontstyles.fontstyleblocks[i].lineprocs[j].startvalue );
                        if( out_driver->fontstyles.fontstyleblocks[i].lineprocs[j].startvalue->text != NULL ) {
                            OUT_DRV_REMAP( fontstyles.fontstyleblocks[i].lineprocs[j].startvalue->text );
                        }
                    }
                    if( out_driver->fontstyles.fontstyleblocks[i].lineprocs[j].firstword != NULL ) {
                        OUT_DRV_REMAP( fontstyles.fontstyleblocks[i].lineprocs[j].firstword );
                        if( out_driver->fontstyles.fontstyleblocks[i].lineprocs[j].firstword->text != NULL ) {
                            OUT_DRV_REMAP( fontstyles.fontstyleblocks[i].lineprocs[j].firstword->text );
                        }
                    }
                    if( out_driver->fontstyles.fontstyleblocks[i].lineprocs[j].startword != NULL ) {
                        OUT_DRV_REMAP( fontstyles.fontstyleblocks[i].lineprocs[j].startword );
                        if( out_driver->fontstyles.fontstyleblocks[i].lineprocs[j].startword->text != NULL ) {
                            OUT_DRV_REMAP( fontstyles.fontstyleblocks[i].lineprocs[j].startword->text );
                        }
                    }
                    if( out_driver->fontstyles.fontstyleblocks[i].lineprocs[j].endword != NULL ) {
                        OUT_DRV_REMAP( fontstyles.fontstyleblocks[i].lineprocs[j].endword );
                        if( out_driver->fontstyles.fontstyleblocks[i].lineprocs[j].endword->text != NULL ) {
                            OUT_DRV_REMAP( fontstyles.fontstyleblocks[i].lineprocs[j].endword->text );
                        }
                    }
                    if( out_driver->fontstyles.fontstyleblocks[i].lineprocs[j].endvalue != NULL ) {
                        OUT_DRV_REMAP( fontstyles.fontstyleblocks[i].lineprocs[j].endvalue );
                        if( out_driver->fontstyles.fontstyleblocks[i].lineprocs[j].endvalue->text != NULL ) {
                            OUT_DRV_REMAP( fontstyles.fontstyleblocks[i].lineprocs[j].endvalue->text );
                        }
                    }
                }
            }
        }
    }

    if( out_driver->absoluteaddress.text != NULL ) {
        OUT_DRV_REMAP( absoluteaddress.text );
    }

    if( out_driver->hline.text != NULL ) {
        OUT_DRV_REMAP( hline.text );
    }

    if( out_driver->vline.text != NULL ) {
        OUT_DRV_REMAP( vline.text );
    }

    if( out_driver->dbox.text != NULL ) {
        OUT_DRV_REMAP( dbox.text );
    }

    return( out_driver );
}

