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


#define SEEK_POSBACK(x)     (-(long)(x))

#define U8_SIZE     sizeof( uint8_t )
#define U16_SIZE    sizeof( uint16_t )
#define U32_SIZE    sizeof( uint32_t )

/* Struct declarations. */

/* This holds the raw contents of one or more contiguous P-buffers. The
 * buffer is to be interpreted as an array of count uint8_t value. The
 * value of count should always be a multiple of 80.
 */

typedef struct {
    uint16_t        count;
    const char *    buffer;
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
    const char  *text;
} code_block;

/* To hold the data extracted from the Variant A FunctionsBlock struct. */

typedef struct {
    uint16_t        count;
    code_block *    code_blocks;
} functions_block;

/* Function declarations. */

#ifdef  __cplusplus
extern "C" {    /* Use "C" linkage when in C++ mode. */
#endif

extern code_block       *get_code_blocks( const char **position, int count, const char *base );
extern p_buffer         *get_p_buffer( FILE *file );
extern functions_block  *parse_functions_block( const char **position, const char *base );

extern unsigned char    fread_u8( FILE *fp );
extern unsigned short   fread_u16( FILE *fp );
extern unsigned         fread_u32( FILE *fp );
extern unsigned         fread_buff( void *buff, int len, FILE *fp );
extern unsigned char    get_u8( const char **buff );
extern unsigned short   get_u16( const char **buff );
extern unsigned         get_u32( const char **buff );
extern void             get_buff( void *obuff, int len, const char **buff );

#ifdef  __cplusplus
}   /* End of "C" linkage for C++. */
#endif

#endif  /* COPFUNC_H_INCLUDED */
