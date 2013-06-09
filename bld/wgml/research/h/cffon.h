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
* Description:  Declares the structs and functions used to manipulate .COP
*               font files:
*                   cop_font
*                     width_block
*                   is_fon_file()
*                   parse_font()
*
* Note:         The field names are intended to correspond to the field names 
*               shown in the Wiki. The Wiki structs are named when the structs
*               defined here are defined; they are not identical.
*
****************************************************************************/

#ifndef CFFON_H_INCLUDED
#define CFFON_H_INCLUDED

#include <stdint.h>
#include <stdio.h>
#include "bool.h"
#include "cftrans.h"

/* Structure declarations. */

/* To hold the data extracted from a WidthBlock struct.
 * width_block is a struct for consistency with outtrans_block.
 */

typedef struct {
    uint32_t         table[0x100];
} width_block;

/* This struct embodies the binary form of the :FONT block.
 *
 * The comments within the structs refer to the "blocks" discussed in the Wiki. 
 *
 * The first two fields are used internally and were used for sizing during
 * development
 *
 * The instance returned will be allocated as a single block and so can be
 * freed in one statement.
 */

typedef struct {
    size_t           allocated_size;
    size_t           next_offset;
    /* The Attributes */
    char *           font_out_name1;
    char *           font_out_name2;
    uint32_t         line_height;
    uint32_t         line_space;
    uint32_t         scale_basis;
    uint32_t         scale_min;
    uint32_t         scale_max;
    uint32_t         char_width;
    /* CharacterDescriptionBlock */
    intrans_block *  intrans;
    outtrans_block * outtrans;
    width_block *    width;
} cop_font;

/* Function declarations. */

#ifdef  __cplusplus
extern "C" {    /* Use "C" linkage when in C++ mode. */
#endif

extern bool            is_fon_file( FILE * in_file );
extern cop_font    *   parse_font( FILE * in_file );

#ifdef  __cplusplus
}   /* End of "C" linkage for C++. */
#endif

#endif  /* CFFON_H_INCLUDED */
