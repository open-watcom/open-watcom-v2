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
*               device files:
*                   cop_device
*                       box_block
*                       underscore_block
*                       defaultfont_block
*                           default_font
*                       pause_block
*                           code_text
*                       devicefont_block
*                           device_font
*                               code_text
*                   is_dev_file()
*                   parse_device()
*
* Note:         The field names are intended to correspond to the field names 
*               shown in the Wiki. The Wiki structs are named when the structs
*               defined here are defined; they are not identical.
*
****************************************************************************/

#ifndef CFDEV_H_INCLUDED
#define CFDEV_H_INCLUDED

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "cffunc.h"
#include "cftrans.h"

/* Structure declarations. */

/* Fonts in box_block and underscore_block.
 *
 * These structs have two font fields: font_name and font_number.
 *
 * For the UnderscoreBlock only, if specified_font is false, then both font_name
 * and font_number are to be ignored and whatever font is in use when the
 * underscore character is needed is to be used.
 *
 * If font_name is NULL, then font_name is to be ignored and font_number
 * is to be used.
 *
 * If font_name is not NULL, then font_name is to be used and font_number is
 * to be ignored.
 *
 * If the font_name is not NULL but the font indicated cannot be found, then
 * font number "0" should be used. There is always a font numbered "0".
 */

/* To hold the data from the BoxBlock struct. */

typedef struct {
    char *          font_name;
    uint8_t         font_number;
    char            horizontal_line;
    char            vertical_line;
    char            top_left;
    char            top_right;
    char            bottom_left;
    char            bottom_right;
    char            top_join;
    char            bottom_join;
    char            left_join;
    char            right_join;
    char            inside_join;
} box_block;

/* To hold the data from the UnderscoreBlock struct. */

typedef struct {
    bool            specified_font;
    char *          font_name;
    uint8_t         font_number;
    char            underscore_char;
} underscore_block;

/* To hold the data from the DefaultFont struct. */

typedef struct {
    char *          font_name;
    char *          font_style;
    uint16_t        font_height;
    uint16_t        font_space;    
} default_font;

/* To hold the data from the DefaultfontBlock struct. */

typedef struct {
    uint16_t        font_count;
    default_font *  fonts;
} defaultfont_block;

/* This struct duplicates the PauseBlock struct. */

typedef struct {
    code_text *     start_pause;
    code_text *     document_pause;
    code_text *     docpage_pause;
    code_text *     devpage_pause;
} pause_block;

/* To hold the data from the DeviceFont struct. */

typedef struct {
    char *          font_name;
    char *          font_switch;
    uint8_t         resident;
    code_text *     font_pause;
} device_font;

/* To hold the data from the DevicefontBlock struct. */

typedef struct {
    uint16_t        font_count;
    device_font *   fonts;
} devicefont_block;

/* This struct embodies the binary form of the :DEVICE block. 
 *
 * Note that the FunctionsBlock is not mentioned. The various CodeBlocks
 * are instead provided as part of PauseBlock and DevicefontBlock.
 *
 * The first two fields are used internally and were used for sizing during
 * development.
 *
 * The instance returned will be allocated as a single block and so can be
 * freed in one statement.
 */

typedef struct {
    size_t              allocated_size;
    size_t              next_offset;
    /* The Attributes */
    char *              driver_name;
    char *              output_name;
    char *              output_extension;
    uint32_t            page_width;
    uint32_t            page_depth;
    uint32_t            horizontal_base_units;
    uint32_t            vertical_base_units;
    /* PagegeometryBlock */
    uint32_t            x_start;
    uint32_t            y_start;
    uint32_t            x_offset;
    uint32_t            y_offset;
    /* BoxBlock */
    box_block           box;
    /* UnderscoreBlock */
    underscore_block    underscore;
    /* TranslationBlock */
    intrans_block *     intrans;
    outtrans_block  *   outtrans;
    /* DefaultfontBlock */
    defaultfont_block   defaultfonts;
    /* PauseBlock */
    pause_block         pauses;
    /* DevicefontBlock */
    devicefont_block    devicefonts;
} cop_device;

/* Function declarations. */

#ifdef  __cplusplus
extern "C" {    /* Use "C" linkage when in C++ mode. */
#endif

extern bool            is_dev_file( FILE * in_file );
extern cop_device  *   parse_device( FILE * in_file );

#ifdef  __cplusplus
}   /* End of "C" linkage for C++. */
#endif

#endif  /* CFDEV_H_INCLUDED */
