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
* Description:  Declares the items needed to parse and interpret the
*               information from .COP files:
*               the structs:
*                   cop_device
*                       box_block
*                       underscore_block
*                       intrans_block
*                       outtrans_block
*                           translation
*                       defaultfont_block
*                           default_font
*                       pause_block
*                           code_text
*                       devicefont_block
*                           device_font
*                               code_text
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
*                   cop_font
*                       intrans_block
*                       outtrans_block
*                           translation
*                       width_block
*                   record_buffer
*                   wgml_font
*               the variables:
*                   bin_device
*                   bin_driver
*                   wgml_font_cnt
*                   wgml_fonts
*
* Note:         The field names are intended to correspond to the field names
*               shown in the Wiki. The Wiki structs are named when the structs
*               defined here are defined; they are not identical.
****************************************************************************/

#ifndef COPFILE_H_INCLUDED

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef uint8_t font_number;

/* struct declarations. */

/* These structs are used by more than one of the top-level structs:
 *      intrans_block is used by both cop_device and cop_font
 *      outtrans_block is used by both cop_device and cop_font
 *      code_text is used by both cop_device and cop_driver
 */

/* To hold the data extracted from an IntransBlock struct.
 * intrans_block is a struct for consistency with outtrans_block.
 */

typedef struct {
    uint8_t         table[0x100];
} intrans_block;

/* To hold the data extracted from an OuttransData struct. */

typedef struct {
    uint8_t         count;
    uint8_t *       data;
} translation;

/* To hold the data extracted from an OuttransBlock struct.
 * The entry for a given character will be NULL if no out-translation
 * was specified.
 */

typedef struct {
    translation *   table[0x100];
} outtrans_block;

/* To hold the data extracted from a CodeBlock struct. */

typedef struct {
    uint16_t            count;
    char                *text;
} code_text;

/* These structs are unique to the top-level struct cop_device. */

/* Fonts in box_block and underscore_block.
 *
 * These structs have two font fields: font_name and font.
 *
 * For the UnderscoreBlock only, if specified_font is false, then both font_name
 * and font are to be ignored and whatever font is in use when the
 * underscore character is needed is to be used.
 *
 * If font_name is NULL, then font_name is to be ignored and font
 * is to be used.
 *
 * If font_name is not NULL, then font_name is to be used and font is
 * to be ignored.
 *
 * If the font_name is not NULL but the font indicated cannot be found, then
 * font number "0" should be used. There is always a font numbered "0".
 */

typedef struct box_chars {
    char                horizontal_line;
    char                vertical_line;
    char                top_left;
    char                top_right;
    char                bottom_left;
    char                bottom_right;
    char                top_join;
    char                bottom_join;
    char                left_join;
    char                right_join;
    char                inside_join;
} box_chars;

/* To hold the data from the BoxBlock struct. */

typedef struct {
    char                *font_name;
    font_number         font;
    box_chars           chars;
} box_block;

/* To hold the data from the UnderscoreBlock struct. */

typedef struct {
    char                *font_name;
    font_number         font;
    char                underscore_char;
    bool                specified_font;
} underscore_block;

/* To hold the data from the DefaultFont struct. */

typedef struct {
    char *              font_name;
    char *              font_style;
    uint16_t            font_height;
    uint16_t            font_space;
} default_font;

/* To hold the data from the DefaultfontBlock struct. */

typedef struct {
    uint16_t            font_count;
    default_font *      fonts;
} defaultfont_block;

/* To hold the data from the PauseBlock struct. */

typedef struct {
    code_text *         start_pause;
    code_text *         document_pause;
    code_text *         docpage_pause;
    code_text *         devpage_pause;
} pause_block;

/* To hold the data from the DeviceFont struct. */

typedef struct {
    char *              font_name;
    char *              font_switch;
    uint8_t             resident;
    code_text *         font_pause;
} device_font;

/* To hold the data from the DevicefontBlock struct. */

typedef struct {
    uint16_t            font_count;
    device_font *       fonts;
} devicefont_block;

/* These structs are unique to the top-level struct cop_driver. */

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
    char                *text;
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
    char                *text;
} newline_block;

/* To hold the data extracted from a NewlineFuncs struct. */

typedef struct {
    uint16_t            count;
    newline_block *     newlineblocks;
} newline_funcs;

/* To hold the data from the FontswitchBlock struct.
 * There will be as many fontswitch_blocks as there are distinct values of
 * "type". The bool fields are used to record information from some of the
 * 21 flags, as discussed in the Wiki. The other field names do not
 * correspond to the Wiki: they take advantage of the fact that there are
 * at most two CodeBlocks, one from a :STARTVALUE block and the other from
 * an :ENDVALUE block.
 */

typedef struct {
    char *              type;
    bool                do_always;
    bool                default_width_flag;
    bool                font_height_flag;
    bool                font_outname1_flag;
    bool                font_outname2_flag;
    bool                font_resident_flag;
    bool                font_space_flag;
    bool                line_height_flag;
    bool                line_space_flag;
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
 * line_proc instances is given by the value of the field "line_passes".
 */

typedef struct {
    uint16_t            line_passes;
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
    unsigned            thickness;
    uint16_t            count;
    char                *text;
} line_block;

/* This struct is unique to the top-level struct cop_font. */

/* To hold the data extracted from a WidthBlock struct.
 * width_block is a struct for consistency with outtrans_block.
 */

typedef struct {
    unsigned            table[0x100];
} width_block;

/* These are the top-level structs. These are the only structs intended to
 * be created and passed around independently.
 *
 * The comments within the structs refer to the blocks discussed in the Wiki.
 *
 * The first two fields are used internally and were used for sizing during
 * development
 *
 * The instance returned will be allocated as a single block and so can be
 * freed in one statement.
 */

/* This struct embodies the binary form of the :DEVICE block.
 *
 * Note that the "FunctionsBlock" is not mentioned. The various "CodeBlock"s
 * are instead provided as part of PauseBlock and DevicefontBlock.
 */

typedef struct {
    unsigned            allocated_size;
    unsigned            next_offset;
    /* The Attributes */
    char            *   driver_name;
    char            *   output_name;
    char            *   output_extension;
    unsigned            page_width;
    unsigned            page_depth;
    unsigned            horizontal_base_units;
    unsigned            vertical_base_units;
    /* PagegeometryBlock */
    unsigned            x_start;
    unsigned            y_start;
    unsigned            x_offset;
    unsigned            y_offset;
    /* BoxBlock */
    box_block           box;
    /* UnderscoreBlock */
    underscore_block    underscore;
    /* TranslationBlock */
    intrans_block   *   intrans;
    outtrans_block  *   outtrans;
    /* DefaultfontBlock */
    defaultfont_block   defaultfonts;
    /* PauseBlock */
    pause_block         pauses;
    /* DevicefontBlock */
    devicefont_block    devicefonts;
} cop_device;

#define OUT_DEV_MAP_OFF(x)      (void *)((char *)out_device + (unsigned)(uintptr_t)(x))
#define OUT_DEV_SET_OFF(x,s) \
    (void *)((char *)out_device + out_device->next_offset); \
    x = (void *)(uintptr_t)out_device->next_offset; \
    out_device->next_offset += s
#define OUT_DEV_REMAP_MBR(x)    out_device->x = OUT_DEV_MAP_OFF(out_device->x)
#define OUT_DEV_EXPAND_CHK(s)   (out_device->allocated_size < (out_device->next_offset + s))

/* This struct embodies the binary form of the :DRIVER block.
 *
 * The "unknown" block is not mentioned because it never has any data in it.
 */

typedef struct {
    unsigned            allocated_size;
    unsigned            next_offset;
    /* The Attributes */
    char            *   rec_spec;
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

#define OUT_DRV_MAP_OFF(x)      (void *)((char *)out_driver + (unsigned)(uintptr_t)(x))
#define OUT_DRV_SET_OFF(x,s) \
    (void *)((char *)out_driver + out_driver->next_offset); \
    x = (void *)(uintptr_t)out_driver->next_offset; \
    out_driver->next_offset += s
#define OUT_DRV_REMAP_MBR(x)    out_driver->x = OUT_DRV_MAP_OFF(out_driver->x)
#define OUT_DRV_EXPAND_CHK(s)   (out_driver->allocated_size < (out_driver->next_offset + s))

#define IN_DRV_MAP_OFF(x)       (void *)((char *)in_driver + (unsigned)(uintptr_t)(x))
#define IN_DRV_SET_OFF(x,s) \
    (void *)((char *)in_driver + in_driver->next_offset); \
    x = (void *)(uintptr_t)in_driver->next_offset; \
    in_driver->next_offset += s
#define IN_DRV_REMAP_MBR(x)     in_driver->x = IN_DRV_MAP_OFF(in_driver->x)
#define IN_DRV_EXPAND_CHK(s)    (in_driver->allocated_size < (in_driver->next_offset + s))
#define IN_DRV_GET_OFF(x)       ((char *)x - (char *)in_driver)

/* This struct embodies the binary form of the :FONT block.
 * Only the fonts need to be treated as a linked list.
 */

typedef struct cop_font {
    struct cop_font *   next_font;
    unsigned            allocated_size;
    unsigned            next_offset;
    /* For matching by defined name. */
    char            *   defined_name;
    /* The Attributes */
    char            *   font_out_name1;
    char            *   font_out_name2;
    unsigned            line_height;
    unsigned            line_space;
    unsigned            scale_basis;
    unsigned            scale_min;
    unsigned            scale_max;
    unsigned            char_width;
    /* CharacterDescriptionBlock */
    intrans_block   *   intrans;
    outtrans_block  *   outtrans;
    width_block     *   width;
} cop_font;

#define OUT_FONT_MAP_OFF(x)     (void *)((char *)out_font + (unsigned)(uintptr_t)(void *)(x))
#define OUT_FONT_SET_OFF(x,s) \
    (void *)((char *)out_font + out_font->next_offset); \
    x = (void *)(uintptr_t)out_font->next_offset; \
    out_font->next_offset += s
#define OUT_FONT_REMAP_MBR(x)   out_font->x = OUT_FONT_MAP_OFF(out_font->x)
#define OUT_FONT_EXPAND_CHK(s)  (out_font->allocated_size < (out_font->next_offset + s))

/* This struct was originally developed for use with the output buffer. It's
 * use has since expanded. current records the current write position, length
 * records the number of bytes pointed to by text, and text points to the
 * bytes to be inserted into the output buffer or otherwise processed.
 */

typedef struct {
    char                *text;
    unsigned            current;
    unsigned            size;
} record_buffer;

/* This struct implements the wgml_font struct in the Wiki. */

typedef struct {
    cop_font            *bin_font;
    fontswitch_block    *font_switch;
    code_text           *font_pause;
    fontstyle_block     *font_style;
    outtrans_block      *outtrans;
    unsigned            default_width;
    unsigned            em_base;
    unsigned            font_height;
    unsigned            font_space;
    unsigned            line_height;
    unsigned            line_space;
    unsigned            spc_width;
    width_block         width;
    char                font_resident;
    unsigned char       shift_count;
    char                shift_height[4];
} wgml_font;

#endif  /* COPFILE_H_INCLUDED */

#if !defined( COPFILE_H_INCLUDED ) || defined( global )
#define COPFILE_H_INCLUDED

/* Variable declarations. */

#ifndef global
    #define global  extern
#endif

global cop_device   *bin_device;     // binary device being used
global cop_driver   *bin_driver;     // binary driver being used
global font_number  wgml_font_cnt;   // number of available fonts
global wgml_font    *wgml_fonts;     // the available fonts

/* Reset so can be reused with other headers. */

#undef global

#endif  /* COPFILE_H_INCLUDED */

