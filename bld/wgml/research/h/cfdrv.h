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
*               driver files:
*                   cop_driver
*                       init_funcs
*                           init_block
*                               init_text
*                       finish_funcs
*                           code_text
*                       newline_funcs
*                           newline_block
*                       fontswitch_funcs
*                           fontswitch_block
*                               code_text
*                       fontstyle_group
*                           fontstyle_block
*                               code_text
*                               line_proc
*                                   code_text
*                       line_block
*                   is_drv_file()
*                   parse_driver()
*
* Note:         The field names are intended to correspond to the field names 
*               shown in the Wiki. The Wiki structs are named when the structs
*               defined here are defined; they are not identical.
*
****************************************************************************/

#ifndef CFDRV_H_INCLUDED
#define CFDRV_H_INCLUDED

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include "cffunc.h"

/* Structure declarations */

/* In all cases: a count of "0" or a NULL pointer indicates that the
 * corresponding item did not exist in the .COP file
 */

/* To hold the data extracted from a CodeBlock struct.
 * The :INIT block is unique in allowing multiple intermixed :VALUE and
 * :FONTVALUE blocks. Testing shows wgml invoking each :FONTVALUE block
 * multiple times but each :VALUE block only once, so is_fontvalue is provided
 * to allow wgml to determine if an init_text instance came from a :FONTVALUE
 * block or a :VALUE block.
 */

typedef struct {
    bool                is_fontvalue;
    uint16_t            count;
    uint8_t *           text;
} init_text;

/* To hold the data from the InitBlock struct. */
/* The field names do not all correspond to those in the Wiki. */

typedef struct {
    uint16_t            count;
    init_text *         codeblock;
} init_block;

/* To hold the data from the InitFuncs struct.
 * This struct bears no resemblence to that in the Wiki: it takes advantage
 * of the fact that there can be at most two :INIT blocks, and that they must
 * be distinguished because wgml processes them at different times.
 */

typedef struct {
    init_block *        start;
    init_block *        document;
} init_funcs;

/* The FinishBlock struct is implemented by the code_text struct. */

/* To hold the data from the FinishFuncs struct.
 * This struct bears no resemblence to that in the Wiki: it takes advantage
 * of the fact that there can be at most two :FINISH blocks, that wgml will
 * only use the first :VALUE block in a :FINISH block, so that only that one
 * needs to be presented here, and that they must be distinguished because
 * wgml processes at most one of them. 
 */

typedef struct {
    code_text *         end;
    code_text *         document;
} finish_funcs;

/* To hold the data from the NewlineBlock struct.
 * There will be as many newline_blocks as there are distinct values of
 * "advance".
 */

typedef struct {
    uint16_t            advance;
    uint16_t            count;
    uint8_t *           text;
} newline_block;

/* To hold the data extracted from a NewlineFuncs struct. */

typedef struct {
    uint16_t            count;
    newline_block *     newlineblocks;
} newline_funcs;

/* To hold the data from the FontswitchBlock struct.
 * There will be as many fontswitch_blocks as there are distinct values of
 * "type". The other field names do not correspond to the Wiki: they take
 * advantage of the fact that there are at most two CodeBlocks, one from a
 * :STARTVALUE block and the other from an :ENDVALUE block.
 */

typedef struct {
    char *              type;
    code_text *         startvalue;
    code_text *         endvalue;
} fontswitch_block;

/* To hold the data extracted from a FontswitchFuncs struct. */

typedef struct {
    uint16_t            count;
    fontswitch_block *  fontswitchblocks;
} fontswitch_funcs;

/* To hold some of the data extracted from a FontstyleFuncs struct.
 * This struct does not correspond to the struct in the Wiki. Instead, it takes
 * advantage of the fact that each :LINEPROC block can define at most one of each
 * of its sub-blocks.
 */

typedef struct {
    code_text *         startvalue;
    code_text *         firstword;
    code_text *         startword;
    code_text *         endword;
    code_text *         endvalue;
} line_proc;

/* To hold the data extracted from a ShortFontstyleBlock struct. 
 * Only the first two fields are found in the Wiki struct. The next three take
 * advantage of the fact that a :FONTSTYLE block directly defines at most one of
 * each of two sub-blocks, plus any number of :LINEPROC blocks. The number of
 * line_proc instances is given by the value of the field "passes".
 */

typedef struct {
    uint16_t            passes;
    char *              type;
    code_text *         startvalue;
    code_text *         endvalue;
    line_proc *         lineprocs;
} fontstyle_block;

/* To hold the data extracted from a FontstyleGroup struct. 
 * This struct bears only a functional relationship to the struct in the Wiki,
 * which must be seen to be believed.
 */

typedef struct {
    uint16_t            count;
    fontstyle_block *   fontstyleblocks;
} fontstyle_group;

/* To hold the data extracted from an HlineBlock, a VlineBlock, or a DboxBlock
 * struct. 
 * This differs from the structs in the Wiki because each of them can contain at
 * most one CodeBlock, as well as the thickness.
 */

typedef struct {
    uint32_t            thickness;
    uint16_t            count;
    uint8_t *           text;
} line_block;

/* This struct embodies the binary form of the :DRIVER block.
 *
 * The "unknown" block is not mentioned because it never has any data in it.
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
    size_t              allocated_size;
    size_t              next_offset;
    /* The Attributes */
    char *              rec_spec;
    char                fill_char;
    /* PageAddressBlock */
    uint8_t             x_positive;
    uint8_t             y_positive;
    /* InitFuncs */
    init_funcs          inits;
    /* FinishFuncs */
    finish_funcs        finishes;
    /* NewlineFuncs */
    newline_funcs       newlines;
    /* Variant A FunctionsBlocks */
    code_text           newpage;
    code_text           htab;
    /* FontswitchFuncs */
    fontswitch_funcs    fontswitches;
    /* FontstyleGroup */
    fontstyle_group     fontstyles;
    /* Variant A FunctionsBlock */
    code_text           absoluteaddress;
    /* HlineBlock */
    line_block          hline;
    /* VlineBlock */
    line_block          vline;
    /* DboxBlock */
    line_block          dbox;
} cop_driver;

/* Function declarations. */

#ifdef  __cplusplus
extern "C" {    /* Use "C" linkage when in C++ mode. */
#endif

extern bool            is_drv_file( FILE * in_file );
extern cop_driver  *   parse_driver( FILE * in_file );

#ifdef  __cplusplus
}   /* End of "C" linkage for C++. */
#endif

#endif  /* CFDRV_H_INCLUDED */
