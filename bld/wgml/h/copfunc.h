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
* Description:  Declares structs and functions used to manipulate .COP
*               FunctionsBlocks:
*                   p_buffer
*                   functions_block
*                       code_block
*                   get_code_blocks()
*                   get_p_buffer()
*                   parse_functions_block()
*
* Note:         The field names are intended to correspond to the field names
*               shown in the Wiki. The Wiki structs are named when the structs
*               defined here are defined; they are not identical.
*
****************************************************************************/

#ifndef COPFUNC_H_INCLUDED
#define COPFUNC_H_INCLUDED

#include <stdint.h>
#include <stdio.h>

/* Struct declarations. */

#pragma disable_message( 128 ); // suppress: Warning! W128: 3 padding byte(s) added

/* This holds the raw contents of one or more contiguous P-buffers. The
 * buffer is to be interpreted as an array of count uint8_t value. The
 * value of count should always be a multiple of 80.
 */

typedef struct {
    uint16_t    count;
    uint8_t *   buffer;
} p_buffer;

/* To hold the data extracted from the CodeBlock struct. This is the CodeBlock
 * discussed in the Wiki with the flag fields omitted and the field
 * cumulative_index added for use when parsing the DeviceFile struct.
 */

typedef struct {
    uint8_t     designator;
    uint16_t    line_pass;
    uint16_t    count;
    uint16_t    cumulative_index;
    uint8_t *   text;
} code_block;

/* To hold the data extracted from the Variant A FunctionsBlock struct. */

typedef struct {
    uint16_t        count;
    code_block *    code_blocks;
} functions_block;

#pragma enable_message( 128 ); // reenable: Warning! W128: 3 padding byte(s) added

/* Function declarations. */

#ifdef  __cplusplus
extern "C" {    /* Use "C" linkage when in C++ mode. */
#endif

extern code_block      *   get_code_blocks( uint8_t * * position, uint16_t count, uint8_t * base );
extern p_buffer        *   get_p_buffer( FILE * file );
extern functions_block *   parse_functions_block( uint8_t * * position, uint8_t * base );

#ifdef  __cplusplus
}   /* End of "C" linkage for C++. */
#endif

#endif  /* COPFUNC_H_INCLUDED */
