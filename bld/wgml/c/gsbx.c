/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2009 The Open Watcom Contributors. All Rights Reserved.
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
*   Description: implement .bx (box) script control word
*   Functions:
*       box_blank_lines()       draw blank lines for character devices
*       box_char_element()      process a single doc_element for character devices
*       box_draw_vlines()       draw VLINEs for line-drawing devices
*       box_line_element()      process a single doc_element for line devices
*       do_char_device()        process current box_line for character devices
*       do_line_device()        process current box_line for line-drawing devices
*       draw_box_lines()        draw HLINEs and/or VLINEs
*       eoc_bx_box()            end-of-column for boxes drawn with BX
*       eoc_char_device()       end-of-column for character devices
*       eoc_line_device()       end-of-column for line devices
*       merge_lines()           merge g_cur_line and/or g_prev_line into box_line
*       resize_box_cols()       resize a box_col_set.cols member
*       scr_bx()                parse BX line and use other functions to perform
*                                   the required actions
*   initial descriptive comments are from script-tso.txt
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


/**************************************************************************/
/* BOX generates the horizontal lines and initiates the vertical lines of */
/* a box.                                                                 */
/*                                                                        */
/* +-----+----------------------------------------------------+           */
/* |     |                                                    |           */
/* |     | <h1 </> ... hn>                                    |           */
/* | .BX | <NEW|ON|OFF|SET> <h1 </> ... hn>                   |           */
/* |     | <CANCEL>                                           |           */
/* |     | <CHAR name>                                        |           */
/* |     |                                                    |           */
/* +-----+----------------------------------------------------+           */
/*                                                                        */
/* SCRIPT generates a box by constructing line segments through character */
/* positions or "cells".                                                  */
/*                                                                        */
/* Line Segments                                                          */
/*                                                                        */
/* There are only four possible line segments in a character cell, each   */
/* of which can be described in terms of movement from the center of the  */
/* cell to one of its four sides:                                         */
/* (1) horizontal right (from the center of the cell to the right side of */
/*     the cell)                                                          */
/* (2) horizontal left (from the center of the cell to the left side of   */
/*     the cell)                                                          */
/* (3) vertical ascender (from the center of the cell to the top of the   */
/*     cell)                                                              */
/* (4) vertical descender (from the center of the cell to the bottom of   */
/*     the cell)                                                          */
/*                                                                        */
/* In the operand descriptions below, the following two generalizations   */
/* are made:                                                              */
/* (1) horizontal rule (all of the horizontal segments from the center of */
/*     one cell to the center of a cell somewhere to the right on the     */
/*     same output line)                                                  */
/* (2) vertical rule (a vertical line from the top of one cell to the     */
/*     bottom of that same cell; that is, a vertical ascender plus a      */
/*     vertical descender)                                                */
/*                                                                        */
/* Operands                                                               */
/*                                                                        */
/* <h1 </> ... hn>: When used with numeric operands "h1 ... hn" only,     */
/*    .BX begins a single box by generating a horizontal rule with        */
/*    vertical descenders in the specified columns; if a previous box is  */
/*    still in effect, then the horizontal rule is generated from the     */
/*    leftmost vertical position of the two boxes to the rightmost        */
/*    vertical position of the two boxes, with vertical ascenders in the  */
/*    vertical positions of the previous box, and the previous box defi-  */
/*    nition is terminated. On all subsequent output lines until the      */
/*    next .BX control word is encountered, vertical rules will be gener- */
/*    ated in the specified columns.                                      */
/*       The numeric operands are relative to the current ADJUST (.ad)    */
/*    value, and must be specified in increasing column order. A signed   */
/*    numeric operand is relative to the preceding operand.               */
/*       When a Box definition is in effect and a .BX with no operands is */
/*    encountered, a horizontal rule is generated from the leftmost       */
/*    vertical position to the rightmost vertical position, with vertical */
/*    ascenders and descenders in all of the vertical positions.          */
/*       A single numeric operand defines a box with only one vertical    */
/*    descender.                                                          */
/*       The character '/' between numeric operands defines multiple      */
/*    side-by-side boxes between which no horizontal rules will be gener- */
/*    ated.                                                               */
/* ON <h1 </> ... hn>: When a box definition is in effect and SCRIPT      */
/*    encounters a new box definition that is specified using only        */
/*    numeric operands, the horizontal rule that closes the first box and */
/*    starts the second box is generated from the leftmost to the right-  */
/*    most of the vertical positions of the two boxes. "ON" as the first  */
/*    operand of the second box definition will suppress those parts of   */
/*    the horizontal rule that are not part of either box, in cases where */
/*    the two boxes are not vertically aligned.                           */
/* NEW <h1 </> ... hn>: superimposes the definition for this box on top   */
/*    of the previous box definition. If the new box overlaps the         */
/*    previous box, none of the line segments "beneath" the new box will  */
/*    be generated. This allows boxes to be drawn within or on top of     */
/*    boxes. The superimposed segments of the previous box will become    */
/*    visible again once the new box definition is terminated.            */
/*       If no column operands are present or if no previous box is still */
/*    in effect, then the "NEW" operand is ignored.                       */
/* SET <h1 </> ... hn>: is treated in all respects like "NEW" with the    */
/*    exception that the definition for the new box is put into effect    */
/*    without generating the horizontal rule or the vertical ascenders    */
/*    and descenders.                                                     */
/* OFF <h1 </> ... hn>: terminates the definition of the current box by   */
/*    generating the "bottom" horizontal rule and the appropriate         */
/*    vertical ascenders.                                                 */
/*       Any numeric column operands used with "OFF" will cause the hori- */
/*    zontal rule to be generated from the leftmost to the rightmost of   */
/*    the combination of the defined and specified vertical positions,    */
/*    with vertical ascenders in all of those positions. If no box is in  */
/*    effect when the OFF is encountered, only the numeric operands spec- */
/*    ified with the OFF determine the positions of the horizontal rule   */
/*    and vertical ascenders.                                             */
/*       If current box was started with NEW or SET, the vertical         */
/*    descenders of the previous box will also be generated.              */
/* CANcel (alias "DELETE"): terminates the definition of the current box  */
/*    without generating its closing horizontal rule and vertical ascen-  */
/*    ders.                                                               */
/* CHAR <TRM|32T|TNC|38C|GPC|APL>: specifies the character set that is    */
/*    to be used by SCRIPT in constructing the four line segments needed  */
/*    for boxes. By default, when SCRIPT is producing output for a        */
/*    terminal, the CHAR TRM set of "-", "|" and "+" is used to give a    */
/*    gross approximation of horizontal rules, vertical rules, and inter- */
/*    sections. In other cases, such as output to a line printer, SCRIPT  */
/*    uses the CHAR TNC set of special box-drawing characters for the TN  */
/*    print train; this results in a better approximation, although still */
/*    not exact. With the CHAR operand, the named set of characters will  */
/*    always be used, independent of the output device.                   */
/*       The IBM 3270 "text terminal" is specified by "32T". The IBM      */
/*    3800 character set with the long "or" bar is specified by "38C".    */
/*    The IBM 3800 GP12 font without the long "or" bar is specified by    */
/*    "GPC". The APL character set is specified by "APL".                 */
/*                                                                        */
/* Debugging Aid                                                          */
/*                                                                        */
/* CHAR HEX: The HEX operand is provided as a "debugging aid". Each       */
/*    character cell in which one or more of the four line segments       */
/*    occurs will be printed using one of the hexadecimal characters in   */
/*    the sequence 0123456789ABCDEF, to represent combinations of a 4-bit */
/*    pattern "xxxx" where each "x" is a 0 or a 1.                        */
/*       This pattern can best be discussed in terms of "8421". The "8"   */
/*    position represents "vertical ascender". The "4" position repre-    */
/*    sents "vertical descender". The "2" position represents "horizontal */
/*    left". The "1" position represents "horizontal right".              */
/*    Therefore, a character cell printed as "E" represents "1110", which */
/*    indicates that the cell contains a vertical ascender, a vertical    */
/*    descender, and a horizontal left. If CHAR TRM were in effect        */
/*    instead, SCRIPT would approximate that cell with a "+", which is    */
/*    clearly incorrect but is the best that can be done for that kind of */
/*    output device. If SCRIPT must approximate, it does so by            */
/*    displaying more than really exists, since displaying less could     */
/*    result in a blank cell.                                             */
/*                                                                        */
/* This control word creates a break.                                     */
/**************************************************************************/

typedef enum {
    bx_none,        // BX
    bx_eoc,         // end-of-column
    bx_on,          // BX ON
    bx_off,         // BX OFF
    bx_new,         // BX NEW
    bx_set,         // BX SET
    bx_can,         // BX CAN or BX DEL
} bx_op;            // the BX operators

typedef enum {
    st_none,        // the normal case
    st_down,        // do stubs for BOXV_down columns
    st_ext,         // stub height is given in box_height, stubs only
    st_up,          // do stubs for BOXV_up columns
} stub_type;

static  bool        first_doc_el    = false;    // true if processing first doc_element in box
static  bool        skips_done      = false;    // true if subs_skip/blank_lines processed for char device
static  bx_op       cur_op          = bx_none;  // current BX operator
static  bx_op       prev_op         = bx_none;  // previous BX operator
static  font_number bx_font         = FONT0;    // font in effect before first BX line
static  font_number sk_font         = FONT0;    // font in effect before current BX line
static  unsigned    box_depth       = 0;        // depth of box (used with VLINES)
static  unsigned    def_height      = 0;        // default font line height
static  unsigned    box_skip        = 0;        // skip for outer (entire) box
static  unsigned    el_skip         = 0;        // skip for current element
static  unsigned    hl_depth        = 0;        // height from VLINE drawn to lower boundary of def_height
static  unsigned    v_offset        = 0;        // space reserved for the box line which is above the line drawn

/***************************************************************************/
/*  resize the cols member of a box_col_set                                */
/***************************************************************************/

static box_col_set * resize_box_cols( box_col_set * in_cols )
{
    in_cols->length += BOXCOL_COUNT;               // add space for new box columns
    in_cols->cols = mem_realloc( in_cols->cols, in_cols->length * sizeof( box_col_spec ) );
    memset( &in_cols->cols[in_cols->current], 0, BOXCOL_COUNT * sizeof( box_col_spec ));
    return( in_cols );
}

/***************************************************************************/
/*  output blank lines with vertical box characters                        */
/***************************************************************************/

static void box_blank_lines( unsigned lines )
{
    box_col_set *   cur_hline;
    doc_element *   blank_el;
    unsigned        i;
    unsigned        i_b;        // box_line index
    text_chars  *   cur_chars;
    text_line   *   cur_blank;

    if( lines == 0 ) {          // ensure lines exist
        skips_done = false;
    } else {
        skips_done = true;

        if( first_doc_el ) {
            sk_font = bx_font;  // sk_font may differ from g_prev_font for first doc element
        }

        cur_blank = NULL;
        cur_chars = NULL;

        blank_el = alloc_doc_el( ELT_text );
        blank_el->depth = lines + g_units_spacing;
        blank_el->element.text.bx_h_done = true;    // prevent being processed again as text element
        lines /= def_height;

        for( i = 0; i < lines; i++ ) {
            if( i == 0 ) {
                /* first blank line */
                cur_blank = alloc_text_line();
                blank_el->element.text.first = cur_blank;
            } else {
                /* subsequent doc_element or subsequent blank line */
                cur_blank->next = alloc_text_line();
                cur_blank = cur_blank->next;
            }
            /* insert marker before first column but not for first line */
            if( (i > 0) ) {
                cur_chars = alloc_text_chars( NULL, 0, g_prev_font );
                cur_chars->prev = NULL;
                cur_chars->x_address = 0;
                cur_chars->width = 0;
                cur_chars->f_switch = FSW_from2;
                cur_blank->first = cur_chars;
                cur_blank->last = cur_chars;
            }
            cur_blank->line_height = def_height;
            if( cur_op == bx_eoc ) {            // eoc uses g_prev_line
                cur_hline = g_prev_line;
            } else {
                cur_hline = box_line->first;
            }
            while( cur_hline != NULL ) {
                for( i_b = 0; i_b < cur_hline->current; i_b++ ) {
                    if( (cur_hline->cols[i_b].v_ind == BOXV_both)
                            || (cur_hline->cols[i_b].v_ind == BOXV_new)
                            || (cur_hline->cols[i_b].v_ind == BOXV_out)
                            || (cur_hline->cols[i_b].v_ind == BOXV_split)
                            || (cur_hline->cols[i_b].v_ind == BOXV_up) ) {  // ascender needed
                        if( cur_blank->first == NULL ) {
                            cur_chars = alloc_text_chars( &bin_device->box.chars.vertical_line, 1,
                                                      bin_device->box.font );
                            cur_blank->first = cur_chars;
                        } else {
                            cur_chars->next = alloc_text_chars(
                                    &bin_device->box.chars.vertical_line, 1, bin_device->box.font );
                            cur_chars->next->prev = cur_chars;
                            cur_chars = cur_chars->next;
                        }
                        cur_chars->x_address = cur_hline->cols[i_b].col - box_col_width;
                        cur_chars->width = cop_text_width( cur_chars->text,
                                           cur_chars->count, bin_device->box.font );
                    }
                    cur_blank->last = cur_chars;
                    /* Insert a marker after the riser */
                    if( first_doc_el ) {
                        cur_chars->next = alloc_text_chars( NULL, 0, sk_font );
                    } else {
                        cur_chars->next = alloc_text_chars( NULL, 0, g_prev_font );
                    }
                    cur_chars->next->prev = cur_chars;
                    cur_chars->next->x_address = cur_blank->last->x_address + cur_blank->last->width;
                    cur_chars->next->width = 0;
                    if( i_b < (cur_hline->current - 1) ) {  // all but last column
                        cur_chars->next->f_switch = FSW_full;
                    } else {                                // last column
                        cur_chars->next->f_switch = FSW_to2;
                    }
                    cur_chars = cur_chars->next;
                    cur_blank->last = cur_chars;
                }
                cur_hline = cur_hline->next;
            }
        }
        insert_col_main( blank_el );
    }

    return;
}

/***************************************************************************/
/*  process a single doc_element                                           */
/*  Note: the doc_element should have "next" set to NULL as it is consumed */
/***************************************************************************/

static void box_char_element( doc_element * cur_el ) {

    bool            h_done;
    bool            init_fs_done;
    box_col_set *   cur_hline;
    text_chars  *   cur_chars   = NULL; // current text_chars in cur_text
    text_chars  *   new_chars;          // text_chars to be inserted into cur_text
    text_line   *   cur_text;           // current text_line
    unsigned        cur_pos;            // current box column position (hbus)
    unsigned        i_b;                // box_line index
    unsigned        last_pos;           // last text_char text end position (hbus)

    last_pos = 0;
    switch( cur_el->type ) {
    case ELT_text:
        if( cur_el->element.text.bx_h_done ) {
            insert_col_main( cur_el );
            break;                      // do element only once
        }

        /* insert vertical ascenders into the text lines */

        init_fs_done = false;
        cur_el->element.text.bx_h_done = true;  // avoid doing again
        cur_text = cur_el->element.text.first;
        if( cur_text != NULL ) {
            while( cur_text != NULL ) {
                cur_chars = cur_text->first;
                if( cur_op == bx_eoc ) {            // eoc uses g_prev_line
                    cur_hline = g_prev_line;
                } else {
                    cur_hline = box_line->first;
                }
                /* prepend any required markers */
                if( skips_done ) {                  // skips were done
                    if( cur_el->in_xmp ) {          // contents were in an XMP block
                        /* will become first text_chars in cur_text */
                        new_chars = alloc_text_chars( NULL, 0, sk_font );
                        new_chars->prev = NULL;
                        new_chars->x_address = 0;
                        new_chars->width = 0;
                        new_chars->f_switch = FSW_from2;
                        /* now prepend new_chars/new_chars->next to cur_text->first */
                        new_chars->next = cur_text->first;
                        cur_text->first->prev = new_chars;
                        cur_text->first = new_chars;
                        skips_done = false;
                    } else {                        // contents were not in an XMP block
                        /* will become first text_chars in cur_text */
                        new_chars = alloc_text_chars( NULL, 0, sk_font );
                        new_chars->prev = NULL;
                        new_chars->x_address = 0;
                        new_chars->width = 0;
                        new_chars->f_switch = FSW_from;
                        /* will become second text_chars in cur_text */
                        new_chars->next = alloc_text_chars( NULL, 0, sk_font );
                        new_chars->next->prev = new_chars;
                        new_chars->next->x_address = 0;
                        new_chars->next->width = 0;
                        new_chars->next->f_switch = FSW_full;
                        /* now prepend new_chars/new_chars->next to cur_text->first */
                        new_chars->next->next = cur_text->first;
                        cur_text->first->prev = new_chars->next;
                        cur_text->first = new_chars;
                        skips_done = false;
                    }
                }
                while( cur_hline != NULL ) {  // iterate over all horizontal lines
                    for( i_b = 0; i_b < cur_hline->current; i_b++ ) {
                        if( (cur_hline->cols[i_b].v_ind == BOXV_both)
                                || (cur_hline->cols[i_b].v_ind == BOXV_new)
                                || (cur_hline->cols[i_b].v_ind == BOXV_out)
                                || (cur_hline->cols[i_b].v_ind == BOXV_split)
                                || (cur_hline->cols[i_b].v_ind == BOXV_up) ) {  // ascender needed

                            cur_pos = cur_hline->cols[i_b].col - box_col_width;
                            if( cur_chars != NULL ) {   // insert ascender if cur_chars exists
                                h_done = false;
                                if( cur_chars == cur_text->first ) {    // first text_chars
                                    last_pos = 0;
                                } else {
                                    last_pos = cur_chars->prev->x_address +
                                               cur_chars->prev->width;
                                }
                                if( cur_pos < last_pos ) {  // can't use current column
                                    continue;
                                }

                                /* The column is known to be possible */

                                while( cur_chars != NULL ) {
                                    if( cur_chars->x_address <= cur_pos ) { // need to check next text_chars
                                        if( cur_chars->next == NULL) {
                                            last_pos = cur_chars->x_address +
                                                       cur_chars->width;
                                            cur_chars = cur_chars->next;
                                            break;
                                        } else {
                                            cur_chars = cur_chars->next;
                                            last_pos = cur_chars->prev->x_address +
                                                       cur_chars->prev->width;
                                        }
                                        if( cur_pos < last_pos ) { // can't use current column
                                            break;
                                        }
                                    } else {

                                        /* box col position is not inside any text_chars */

                                        new_chars = alloc_text_chars(
                                                    &bin_device->box.chars.vertical_line,
                                                    1, bin_device->box.font );
                                        new_chars->x_address = cur_pos;
                                        new_chars->width = cop_text_width( new_chars->text,
                                                        new_chars->count, bin_device->box.font );
                                        if( cur_chars->prev == NULL ) { // first text_chars in cur_text
                                            cur_text->first = new_chars;
                                        } else {
                                            new_chars->prev = cur_chars->prev;
                                            cur_chars->prev->next = new_chars;
                                        }
                                        new_chars->next = cur_chars;
                                        cur_chars->prev = new_chars;
                                        h_done = true;
                                        if( cur_el->in_xmp ) {          // contents were in an XMP block
                                            /* first the two "to" markers */
                                            if( first_doc_el ) {
                                                new_chars = alloc_text_chars( NULL, 0, sk_font );
                                            } else {
                                                new_chars = alloc_text_chars( NULL, 0, g_prev_font );
                                            }
                                            new_chars->x_address = cur_pos;
                                            new_chars->width = 0;
                                            new_chars->f_switch = FSW_to2;
                                            new_chars->prev = cur_chars->prev;
                                            cur_chars->prev->next = new_chars;
                                            new_chars->next = cur_chars;
                                            cur_chars->prev = new_chars;
                                            /* then the "from" marker */
                                            if( first_doc_el ) {
                                                new_chars = alloc_text_chars( NULL, 0, sk_font );
                                            } else {
                                                new_chars = alloc_text_chars( NULL, 0, g_prev_font );
                                            }
                                            new_chars->x_address = cur_pos;
                                            new_chars->width = 0;
                                            new_chars->f_switch = FSW_from;
                                            new_chars->prev = cur_chars->prev;
                                            cur_chars->prev->next = new_chars;
                                            new_chars->next = cur_chars;
                                            cur_chars->prev = new_chars;
                                        }
                                        break;
                                    }
                                    if( cur_pos < last_pos ) { // can't use current column
                                        continue;
                                    }
                                }
                                if( h_done ) {      // process next box column
                                    continue;
                                }
                            }
                            if( (cur_chars == NULL) && (cur_pos >= last_pos) ) {   // append ascender if out of text_chars
                                if( !init_fs_done ) {               // just after text
                                /* Insert the marker after the text */
                                    if( cur_text->first == NULL ) { // empty line
                                        new_chars = alloc_text_chars( NULL, 0, g_prev_font );
                                        new_chars->prev = NULL;
                                        new_chars->x_address = cur_text->last->x_address
                                                                        + cur_text->last->width;
                                        new_chars->width = 0;
                                        new_chars->f_switch = FSW_from2;
                                        cur_text->first = new_chars;
                                        cur_text->last = new_chars;
                                    } else {
                                        new_chars = cur_text->last;
                                        new_chars->next = alloc_text_chars( NULL, 0, g_prev_font );
                                        new_chars->next->prev = new_chars;
                                        new_chars->next->x_address = cur_text->last->x_address
                                                                        + cur_text->last->width;
                                        new_chars->next->width = 0;
                                        new_chars->next->f_switch = FSW_from2;
                                        cur_text->last = new_chars->next;
                                    }
                                    init_fs_done = true;
                                }
                                /* Insert the ascender */
                                if( cur_text->first == NULL ) { // empty line
                                    new_chars = alloc_text_chars(
                                                &bin_device->box.chars.vertical_line,
                                                      1, g_curr_font );
                                    new_chars->prev = NULL;
                                    new_chars->x_address = 0;
                                    new_chars->width = cop_text_width( new_chars->text,
                                                                new_chars->count, g_curr_font );
                                    cur_text->first = new_chars;
                                    cur_text->last = new_chars;
                                } else {
                                    new_chars = cur_text->last;
                                    new_chars->next = alloc_text_chars(
                                                    &bin_device->box.chars.vertical_line,
                                                    1, g_curr_font );
                                    new_chars->next->prev = new_chars;
                                    new_chars->next->x_address = cur_pos;
                                    new_chars->next->width = cop_text_width(
                                            new_chars->next->text, new_chars->next->count,
                                            g_curr_font );
                                    cur_text->last = new_chars->next;
                                }
                                /* Insert the marker(s) after the ascender */
                                if( i_b == cur_hline->current - 1 ) {   // last ascender
                                    new_chars = cur_text->last;
                                    new_chars->next = alloc_text_chars( NULL, 0, g_prev_font );
                                    new_chars->next->prev = new_chars;
                                    new_chars->next->x_address = cur_text->last->x_address
                                                                    + cur_text->last->width;
                                    new_chars->next->width = 0;
                                    new_chars->next->f_switch = FSW_to;
                                    cur_text->last = new_chars->next;
                                }
                                new_chars = cur_text->last;             // after every ascender
                                new_chars->next = alloc_text_chars( NULL, 0, g_prev_font );
                                new_chars->next->prev = new_chars;
                                new_chars->next->x_address = cur_text->last->x_address
                                                                    + cur_text->last->width;
                                new_chars->next->width = 0;
                                new_chars->next->f_switch = FSW_full;
                                cur_text->last = new_chars->next;
                            }
                        }
                    }
                    first_doc_el = false;       // end special first line treatment
                    cur_hline = cur_hline->next;
                }
                cur_text = cur_text->next;
            }
        }

        /* insert the element into the page */

        insert_col_main( cur_el );
        break;

    case ELT_graph:

        /* replace GRAPHIC elements with blank lines */

        box_blank_lines( cur_el->depth );
        add_doc_el_to_pool( cur_el );
        break;

    default:

        /* insert the element into the page */

        insert_col_main( cur_el );
    }

    return;
}

/***************************************************************************/
/*  output vline elements for a given depth and pre-skip                   */
/*  Note: only processes columns for one specific box_col_set              */
/***************************************************************************/

static void box_draw_vlines( box_col_set * hline, unsigned subs_skip,
                              unsigned top_skip, stub_type stub )
{
    bool            first_done;
    bx_v_ind        cur_col_type;
    doc_element *   v_line_el;
    unsigned        i_h;                    // hline index
    unsigned        cur_depth;              // local box_depth (keeps box_depth unchanged)

    cur_depth = box_depth;
    first_done = false;
    if( (stub != st_ext) && (cur_depth > 0) ) { // preserve for 0 value & for st_ext stubs
        if( ((prev_op == bx_can) && (cur_op == bx_can))
            || ((prev_op == bx_set) && (cur_op == bx_set)) ) {
            cur_depth += hl_depth;
        }
        cur_depth += bin_driver->vline.thickness;
    }
    for( i_h = 0; i_h < hline->current; i_h++ ) { // iterate over all output columns
        cur_col_type = hline->cols[i_h].v_ind;
        if( (stub == st_ext) || (cur_col_type == BOXV_up)
             || ((stub == st_none) && (cur_col_type == BOXV_split))
             || ((stub == st_down) && (cur_col_type == BOXV_down))
             || ((cur_col_type == BOXV_new) && (cur_op == bx_none))
             || ((cur_col_type == BOXV_new) && (cur_op == bx_on))
             || ((cur_col_type == BOXV_new) && (cur_op == bx_new))
             || ((cur_col_type == BOXV_new) && (cur_op == bx_set))
             || ((cur_col_type == BOXV_both) && (cur_op == bx_eoc))
             || ((cur_col_type == BOXV_down) && (cur_op == bx_eoc))
             || ((cur_col_type == BOXV_new) && (cur_op == bx_eoc))
             || ((cur_col_type == BOXV_out) && (cur_op == bx_eoc))
             || ((cur_col_type == BOXV_split) && (cur_op == bx_eoc))
             ) {  // ascender needed

            ProcFlags.vline_done = true;

            /* Create the doc_element to hold the VLINE */

            /**************************NOTE************************************/
            /* Conversion to init_doc_el() was kept as simple as possible     */
            /* it might be asked whether g_blank_units_lines should be zeroed */
            /* even for the first VLINE, but that is how I found the code     */
            /******************************************************************/

            g_blank_units_lines = 0;                // no positional adjustments
            if( !first_done ) {                     // except first VLINE
                g_subs_skip = subs_skip;
                g_top_skip = top_skip;
            }
            v_line_el = init_doc_el( ELT_vline, 0 ); // only the last VLINE can (sometimes) have a depth > 0
            v_line_el->element.vline.h_start = hline->cols[i_h].col - h_vl_offset;
            if( (((stub == st_down) || (stub == st_ext))
                    && (hline->cols[i_h].v_ind == BOXV_down))
                    || ((stub == st_up) && ((cur_col_type == BOXV_up)
                        || (cur_col_type == BOXV_new))) ) {    // stubs use hl_depth
                v_line_el->element.vline.v_len = hl_depth;
            } else {
                v_line_el->element.vline.v_len = cur_depth;
            }

            /* Add the column-specific depth */

            v_line_el->element.vline.v_len += hline->cols[i_h].depth;

            /* Add the thickness so the correct line height will be used in PS */

            if( v_offset + hl_depth == box_depth ) {     // empty box
                v_line_el->element.vline.v_len += bin_driver->vline.thickness;
            }

            /* Set number of AA blocks to use for this column */

            if( !first_done ) {                                 // first VLINE
                if( (stub == st_ext) || ((cur_depth == 0)
                    || (stub == st_down)) && (hline->current > 1)
                    || (cur_col_type == BOXV_new) && (cur_op != bx_set)
                        ) {                            // these first VLINEs do AA once
                    v_line_el->element.vline.twice = false;
                }
                first_done = true;
            } else if( ((stub == st_none) && (cur_depth == 0) && (cur_op != bx_on))
                     || ((stub == st_none) && (cur_depth == 0) && (cur_op == bx_on
                        && ProcFlags.top_line))
                     || ((stub == st_ext)
                        && (v_line_el->element.vline.v_len == cur_depth)) ) {
                v_line_el->element.vline.twice = false;
            }

            if( (hline->cols[i_h].depth > 0) && (box_depth == 0)
                    && (((prev_op == bx_can) && (cur_op == bx_can))
                    || ((prev_op == bx_set) && (cur_op == bx_set))) ) {
// *** this next line may need to be modified *** //
                v_line_el->element.vline.v_len += hl_depth + bin_driver->vline.thickness;
                v_line_el->element.vline.twice = true;
            }
            hline->cols[i_h].depth = 0;

            insert_col_main( v_line_el );   // insert the VLINE
        } else if ( (cur_col_type != BOXV_down) ) {
            hline->cols[i_h].depth += box_depth;    // if not output, store box_depth
            if( (cur_col_type == BOXV_out) ) {
                if( ((prev_op != bx_can) && (cur_op == bx_can))
                       || ((prev_op != bx_set) && (cur_op == bx_set)) ) {
                    hline->cols[i_h].depth -= hl_depth;
                }
            }
        }
    }

    return;
}


/***************************************************************************/
/*  draw HLINEs and/or VLINEs                                              */
/***************************************************************************/

static void draw_box_lines( doc_element * h_line_el )
{
    box_col_set *   cur_hline;
    doc_element *   cur_el          = NULL;
    stub_type       off_stub        = st_down;
    unsigned        sav_blank_lines;
    unsigned        sav_subs_skip;
    unsigned        sav_top_skip;

    if( cur_op == bx_can ) {            // no HLINES exist
        cur_hline = box_line->first;
        while( cur_hline != NULL ) {    // iterate over all horizontal lines
            box_draw_vlines( cur_hline, el_skip, el_skip, st_none );
            if( ProcFlags.vline_done) {
                el_skip = 0;            // skip used
            }
            cur_hline = cur_hline->next;
        }
        box_depth = 0;                  // depth used
    } else if( cur_op == bx_set ) {     // no HLINES exist
        if( box_depth > hl_depth ) {    // guess: box_depth should not go negative
            box_depth -= hl_depth;
        }
        cur_hline = box_line->first;
        while( cur_hline != NULL ) {    // iterate over all horizontal lines
            if( ProcFlags.in_bx_box ) {
                box_draw_vlines( cur_hline, el_skip - hl_depth, el_skip, st_none );
                if( ProcFlags.vline_done) {
                    el_skip = 0;            // skip used
                }
            } else {
                box_draw_vlines( cur_hline, box_skip - hl_depth, box_skip, st_none );
                if( ProcFlags.vline_done) {
                    box_skip = 0;            // skip used
                }
            }
            cur_hline = cur_hline->next;
        }
        box_depth = 0;                      // reset for new box
    } else {
        sav_blank_lines = h_line_el->blank_lines;
        sav_subs_skip = h_line_el->subs_skip;
        sav_top_skip = h_line_el->top_skip;
        if( ProcFlags.top_line ) {
            off_stub = st_ext;
            sav_top_skip += v_offset;   // position for HLINE
        }

        if( !ProcFlags.in_bx_box ) {    // first BX line: start of box
            cur_el = h_line_el;
            cur_hline = box_line->first;
            switch( cur_op ) {
            case bx_off:                            // draws stubs
                while( cur_el != NULL ) {
                    h_line_el = h_line_el->next;
                    cur_el->next = NULL;
                    if( cur_hline->current > 1 ) {      // discard HLINEs with length 0
                        cur_el->blank_lines = sav_blank_lines;
                        cur_el->subs_skip = sav_subs_skip;
                        cur_el->top_skip = sav_top_skip;
                        insert_col_main( cur_el );
                        box_draw_vlines( cur_hline, 0, 0, off_stub );
                        sav_blank_lines = 0;            // skips used, zero saved skips
                        sav_subs_skip = 0;
                        sav_top_skip = 0;
                    } else {
                        add_doc_el_to_pool( cur_el );
                        box_draw_vlines( cur_hline, sav_subs_skip +
                                    sav_blank_lines, sav_top_skip, off_stub );
                        if( ProcFlags.vline_done ) {
                            sav_blank_lines = 0;        // skips used, zero saved skips
                            sav_subs_skip = 0;
                            sav_top_skip = 0;
                        }
                    }
                    cur_el = h_line_el;
                    cur_hline = cur_hline->next;
                }
                break;
            default:                                // all others: just the HLINE
                while( cur_el != NULL ) {
                    h_line_el = h_line_el->next;
                    cur_el->next = NULL;
                    if( cur_hline->current > 1 ) {  // discard HLINEs with length 0
                        cur_el->blank_lines = sav_blank_lines;
                        cur_el->subs_skip = sav_subs_skip;
                        cur_el->top_skip = sav_top_skip;
                        insert_col_main( cur_el );
                        sav_blank_lines = 0;            // skips used, zero saved skips
                        sav_subs_skip = 0;
                        sav_top_skip = 0;
                    } else {
                        add_doc_el_to_pool( cur_el );
                    }
                    cur_el = h_line_el;
                    cur_hline = cur_hline->next;
                }
                break;
            }
        } else {                        // box has started: VLINEs are possible
            switch( cur_op ) {
            case bx_none:               // VLINEs, HLINEs + stubs
            case bx_on:
                if( ProcFlags.draw_v_line ) {
                    cur_hline = box_line->first;
                    while( cur_hline != NULL ) {
                        box_draw_vlines( cur_hline, sav_subs_skip + sav_blank_lines,
                                                        sav_top_skip, st_none );
                        if( ProcFlags.vline_done ) {
                            sav_blank_lines = 0;        // skips used, zero saved skips
                            sav_subs_skip = 0;
                            sav_top_skip = 0;
                        }
                        cur_hline = cur_hline->next;
                    }
                    box_depth = 0;                      // box_depth used
                }
                cur_el = h_line_el;
                cur_hline = box_line->first;
                while( cur_el != NULL ) {
                    h_line_el = h_line_el->next;
                    cur_el->next = NULL;
                    if( cur_hline->current > 1 ) {      // discard HLINEs with length 0
                        cur_el->blank_lines = sav_blank_lines;
                        cur_el->subs_skip = sav_subs_skip;
                        cur_el->top_skip = sav_top_skip;
                        insert_col_main( cur_el );
                        sav_blank_lines = 0;            // skips used, zero saved skips
                        sav_subs_skip = 0;
                        sav_top_skip = 0;
                    } else {
                        add_doc_el_to_pool( cur_el );
                    }
                    if( ProcFlags.draw_v_line ) {
                        box_draw_vlines( cur_hline, 0, 0, st_up );  // draw stubs
                    }
                    cur_el = h_line_el;
                    cur_hline = cur_hline->next;
                }
                break;

            case bx_off:                // last BX line: end of box, special processing
                cur_el = h_line_el;
                cur_hline = box_line->first;
                while( cur_el != NULL ) {
                    h_line_el = h_line_el->next;
                    cur_el->next = NULL;
                    if( cur_hline->current > 1 ) {      // discard HLINEs with length 0
                        cur_el->blank_lines = sav_blank_lines;
                        cur_el->subs_skip = sav_subs_skip;
                        cur_el->top_skip = sav_top_skip;
                        insert_col_main( cur_el );
                        box_draw_vlines( cur_hline, 0, 0, off_stub );
                        sav_blank_lines = 0;            // skips used, zero saved skips
                        sav_subs_skip = 0;
                        sav_top_skip = 0;
                    } else {
                        add_doc_el_to_pool( cur_el );
                        box_draw_vlines( cur_hline, sav_subs_skip + sav_blank_lines,
                                                        sav_top_skip, off_stub );
                        if( ProcFlags.vline_done ) {
                            sav_blank_lines = 0;        // skips used, zero saved skips
                            sav_subs_skip = 0;
                            sav_top_skip = 0;
                        }
                    }
                    cur_el = h_line_el;
                    cur_hline = cur_hline->next;
                }
                box_depth = 0;                      // reset for new box
                break;
            case bx_new:
            case bx_set:                            // actually pretty straightforward
                cur_el = h_line_el;
                cur_hline = box_line->first;
                while( cur_el != NULL ) {
                    h_line_el = h_line_el->next;
                    cur_el->next = NULL;
                    if( cur_hline->current > 1 ) {      // discard HLINEs with length 0
                        cur_el->blank_lines = sav_blank_lines;
                        cur_el->subs_skip = sav_subs_skip;
                        cur_el->top_skip = sav_top_skip;
                        insert_col_main( cur_el );
                        sav_blank_lines = 0;            // skips used, zero saved skips
                        sav_subs_skip = 0;
                        sav_top_skip = 0;
                    } else {
                        add_doc_el_to_pool( cur_el );
                    }
                    cur_el = h_line_el;
                    cur_hline = cur_hline->next;
                }
                if( ProcFlags.draw_v_line ) {           // VLINEs will be drawn second
                    cur_hline = box_line->first;
                    while( cur_hline != NULL ) {
                        box_draw_vlines( cur_hline, sav_subs_skip + sav_blank_lines,
                                                        sav_top_skip, st_none );
                        if( ProcFlags.vline_done ) {
                            sav_blank_lines = 0;    // skips used, zero saved skips
                            sav_subs_skip = 0;
                            sav_top_skip = 0;
                        }
                        cur_hline = cur_hline->next;
                    }
                }
                box_depth = 0;                      // reset for new box
                break;
            default:
                internal_err_exit( __FILE__, __LINE__ );
                /* never return */
            }
        }
        el_skip = 0;            // el_skip used to position HLINEs/VLINEs
    }

    return;
}

/***************************************************************************/
/*  process a single doc_element for line devices                          */
/*  the doc_element cur_doc_el_group.first is the doc_element processed    */
/***************************************************************************/

static void  box_line_element( bool add_depth )
{
    doc_element *   cur_el          = NULL;

    cur_el = cur_doc_el_group->first;

    /* add_depth must be true only the first time the doc_element is processed */

    if( add_depth ) {
        if( ProcFlags.col_started ) {           // text line not at top of page
            box_depth += cur_el->blank_lines + cur_el->subs_skip + cur_el->depth;
            if( box_skip == 0 ) {
                cur_el->subs_skip += el_skip;   // add el_skip to both cur_el & box_depth
                box_depth += el_skip;
                el_skip = 0;                    // el_skip used for current element
            } else {
                cur_el->subs_skip += box_skip;  // add box_skip to cur_el only
                box_skip = 0;                   // box_skip used for current element
            }
        } else {                            // top of page
            box_skip = 0;                   // box_skip no longer relevant
            box_depth += cur_el->top_skip + cur_el->depth - v_offset;
            ProcFlags.col_started = true;
        }
    }

    cur_doc_el_group->first = cur_el->next;
    cur_el->next = NULL;
    insert_col_main( cur_el );
    cur_el = cur_doc_el_group->first;  // resume processing

    return;
}


/***************************************************************************/
/*  create the box for character devices                                   */
/***************************************************************************/

static void  do_char_device( void )
{
    box_col_set *   cur_hline;
    bx_v_ind        cur_v_ind;
    char        *   p;
    doc_element *   box_el;             // holds the box line itself, for char devices
    doc_element *   cur_el;
    unsigned        i;                  // overall index
    unsigned        i_b;                // box_line index
    unsigned        len;
    text_chars  *   cur_chars   = NULL; // current text_chars in cur_text
    text_chars  *   new_chars   = NULL; // text_chars used to prepend markers if needed
    unsigned        cur_col;            // current column (not hbus)
    unsigned        skippage;           // lines to skip (not hbus)

    /* process any accumulated doc_elements */

    if( cur_doc_el_group != NULL ) {
        if( cur_doc_el_group->first != NULL ) {
            while( cur_doc_el_group->first != NULL ) {
                if( cur_doc_el_group->depth <= max_depth ) {   // doc_elements will all fit
                    cur_el = cur_doc_el_group->first;
                    while( cur_el != NULL ) {
                        if( ProcFlags.col_started ) {
                            skippage = cur_el->blank_lines + cur_el->subs_skip;
                            cur_el->subs_skip = 0;
                        } else {
                            skippage = cur_el->blank_lines + cur_el->top_skip;
                            cur_el->top_skip = 0;
                            ProcFlags.col_started = true;
                        }
                        cur_el->blank_lines = 0;
                        if( skippage > 0 ) {
                            box_blank_lines( skippage );
                        }
                        max_depth -= skippage + cur_el->depth;
                        cur_doc_el_group->first = cur_el->next;
                        cur_el->next = NULL;
                        if( cur_el->depth == 0 ) {  // remove blank lines marker, which has one empty text_line
                            add_doc_el_to_pool( cur_el );
                        } else {
                            box_char_element( cur_el );
                        }
                        cur_el = cur_doc_el_group->first;
                    }
                } else {                                    // finish off current page
                    next_column();
                    max_depth = t_page.max_depth;
                }
            }
            add_doc_el_group_to_pool( cur_doc_el_group );
            cur_doc_el_group = NULL;
        } else {
            box_depth += hl_depth;
        }
    }

    if( (box_line->next != NULL) && (cur_op == bx_off) && !box_line->had_cols ) {
        box_blank_lines( 1 );
    } else {

        /* Create the doc_element to hold the box lines */

        box_el = init_doc_el( ELT_text, def_height );
        box_el->element.text.bx_h_done = true;      // prevent being processed again as text element

        if( (cur_op == bx_can) ) {                  // special processing
            box_el->element.text.overprint = true;  // force overprint
            box_el->element.text.force_op = true;   // even at top of page
        }

        /* Create the text_line */

        box_el->element.text.first = alloc_text_line();
        box_el->element.text.first->line_height = def_height;

        if( (cur_op != bx_can) && (cur_op != bx_set) ) {    // no horizontal line for BX CAN or BX SET
            cur_hline = box_line->first;
            while( cur_hline != NULL ) {
                len = (cur_hline->cols[cur_hline->current - 1].col -
                                                       cur_hline->cols[0].col + 1);

                /* Create the text_element for the current box line */

                if( cur_chars == NULL ) {
                    cur_chars = alloc_text_chars( NULL, len, bin_device->box.font );
                    box_el->element.text.first->first = cur_chars;
                } else {
                    cur_chars->next = alloc_text_chars( NULL, len, bin_device->box.font );
                    cur_chars->next->prev = cur_chars;
                    cur_chars = cur_chars->next;
                }
                box_el->element.text.first->last = cur_chars;
                cur_chars->x_address = cur_hline->cols[0].col - box_col_width;

                /* Create the horizontal box line from the BOX characters. */

                p = cur_chars->text;
                cur_col = cur_hline->cols[0].col;
                i_b = 0;
                for( i = 0; i < len; i++ ) {                    // iterate over all output columns
                    if( cur_col < cur_hline->cols[i_b].col ) {  // not a box column
                        *p = bin_device->box.chars.horizontal_line;
                    } else {                                    // box column found
                        cur_v_ind = cur_hline->cols[i_b].v_ind;
                        if( cur_hline->current == 1 ) {         // vertical only
                            *p = bin_device->box.chars.vertical_line;
                        } else if ( cur_v_ind == BOXV_hid ) {   // hidden in all boxes
                            *p = bin_device->box.chars.horizontal_line;
                        } else if ( cur_v_ind == BOXV_out ) {   // visible but outside of current box
                            *p = bin_device->box.chars.vertical_line;
                        } else if( i_b == 0 ) {                 // first box column
                            if( cur_v_ind == BOXV_new ) {       // hidden in current box only
                                if( cur_op == bx_new ) {        // BX NEW: up
                                    *p = bin_device->box.chars.bottom_left;
                                } else if( cur_op == bx_off ) { // BX OFF: down
                                    *p = bin_device->box.chars.top_left;
                                } else {                        // anything else
                                    *p = bin_device->box.chars.horizontal_line;
                                }
                            } else if( (cur_v_ind == BOXV_both)
                                    || (cur_v_ind == BOXV_split) ) {      // both up and down
                                *p = bin_device->box.chars.left_join;
                            } else if( cur_v_ind == BOXV_down ) {   // down only
                                *p = bin_device->box.chars.top_left;
                            } else {                            // up only
                                *p = bin_device->box.chars.bottom_left;
                            }
                        } else if( i_b == cur_hline->current - 1 ) {    // last box column
                            if( cur_v_ind == BOXV_new ) {       // hidden in current box only
                                if( cur_op == bx_new ) {        // BX NEW: up
                                    *p = bin_device->box.chars.bottom_right;
                                } else if( cur_op == bx_off ) { // BX OFF: down
                                    *p = bin_device->box.chars.top_right;
                                } else {                        // anything else
                                    *p = bin_device->box.chars.horizontal_line;
                                }
                            } else if( cur_v_ind == BOXV_both
                                    || (cur_v_ind == BOXV_split) ) {      // both up and down
                                *p = bin_device->box.chars.right_join;
                            } else if( cur_v_ind == BOXV_down ) {   // down only
                                *p = bin_device->box.chars.top_right;
                            } else {                                    // up only
                                *p = bin_device->box.chars.bottom_right;
                            }
                        } else {                            // all other box columns
                            if( cur_v_ind == BOXV_new ) {       // hidden in current box only
                                if( cur_op == bx_new ) {        // BX NEW: up
                                    *p = bin_device->box.chars.bottom_join;
                                } else if( cur_op == bx_off ) { // BX OFF: down
                                    *p = bin_device->box.chars.top_join;
                                } else {                        // anything else
                                    *p = bin_device->box.chars.inside_join;
                                }
                            } else if( cur_v_ind == BOXV_both
                                    || (cur_v_ind == BOXV_split) ) {      // both up and down
                                *p = bin_device->box.chars.inside_join;
                            } else if( cur_v_ind == BOXV_down ) {   // down only
                                *p = bin_device->box.chars.top_join;
                            } else {                        // up only
                                *p = bin_device->box.chars.bottom_join;
                            }
                        }
                        cur_col = cur_hline->cols[i_b].col;
                        i_b++;
                    }
                    p++;
                    cur_col += box_col_width;
                    cur_chars->count++;
                }

                /* text is known to contain no tabs */

                cur_chars->width = cop_text_width( cur_chars->text, cur_chars->count, bin_device->box.font );
                cur_hline = cur_hline->next;
            }
        }

        if( ProcFlags.in_bx_box ) {                                       // not first BX line
            if( t_page.cur_depth == t_page.max_depth ) {
                skippage = box_el->blank_lines + box_el->top_skip;
            } else {
                skippage = box_el->blank_lines + box_el->subs_skip;
            }
            if( (t_page.cur_depth + skippage) > t_page.max_depth ) {
                skippage = max_depth;
            }
            if( skippage > 0 ) {
                box_blank_lines( skippage );
                box_el->blank_lines = 0;
                box_el->subs_skip = 0;
                box_el->top_skip = 0;
            }
        }                           // skips before first BX line are treated normally

        /* Prepend markers to box line */

        if( skips_done ) {          // only if skips were done
            /* will become first text_chars in cur_text */
            new_chars = alloc_text_chars( NULL, 0, g_prev_font );
            new_chars->prev = NULL;
            new_chars->x_address = 0;
            new_chars->width = 0;
            new_chars->f_switch = FSW_from;
            /* will become second text_chars in cur_text */
            new_chars->next = alloc_text_chars( NULL, 0, g_prev_font );
            new_chars->next->prev = new_chars;
            new_chars->next->x_address = 0;
            new_chars->next->width = 0;
            new_chars->next->f_switch = FSW_full;
            /* now prepend new_chars/new_chars->next to cur_text->first */
            new_chars->next->next = box_el->element.text.first->first;
            box_el->element.text.first->first->prev = new_chars->next;
            box_el->element.text.first->first = new_chars;
        }

        /* Add marker to first line of box */

        if( g_prev_font != g_curr_font ) {
            cur_chars->next = alloc_text_chars( NULL, len, bin_device->box.font );
            cur_chars->next->prev = cur_chars;
            cur_chars = cur_chars->next;
            cur_chars->x_address = cur_chars->prev->x_address + cur_chars->prev->length;
            cur_chars->length = 0;
            cur_chars->font = g_prev_font;
            cur_chars->f_switch = FSW_full;
            box_el->element.text.first->last = cur_chars;
        }

        insert_col_main( box_el );  // insert the box line
    }
    ProcFlags.in_bx_box = true; // box has started

    return;
}


/***************************************************************************/
/*  create the box for line-drawing devices                                */
/***************************************************************************/

static void do_line_device( void )
{
    bool            do_v_adjust;
    box_col_set *   cur_hline;
    doc_element *   check_el;
    doc_element *   cur_el;
    doc_element *   h_line_el;
    unsigned        h_offset;
    unsigned        prev_height;

    h_line_el = NULL;

    /********************************************************/
    /* this code does not do what wgml 4.0 does             */
    /* unless the default font line height is equal to that */
    /* of font "0"                                          */
    /* otherwise, it is not even close                      */
    /* but what wgml 4.0 does makes no sense at all         */
    /********************************************************/

    v_offset = def_height / 2;
    if( (def_height % 2) > 0 ) {
        v_offset++;
    }
    hl_depth = def_height - v_offset;


    /************************************************************************/
    /* this code does what wgml 4.0 does in a limited context:              */
    /* when the text last output used a font other than font "0", even if   */
    /* the current font is now font "0", the v_offset and def_height are    */
    /* adjusted as shown when the text font's line height is different from */
    /* the font "0" line height                                             */
    /* This is known to apply to text within the box, and to text preceding */
    /* the box except at the top of a page                                  */
    /* Note: do_v_adjust is needed because, when P is followed by a break   */
    /* with no intervening text, a line is simulated by inserted an empty   */
    /* doc_element into t_page.cur_col->main. This causes the column to be  */
    /* started, and that triggers the v_offset adjustment inappropriately.  */
    /* do_v_adjust is false even when ProcFlags.col_started is true,        */
    /* provided that there is only one doc_element on the page and it       */
    /* matches the characteristics of the empty doc_element: it is the only */
    /* doc_element in t_page.cur_col->main, it is a text element, it has a  */
    /* text_line, and that text_line has no text_chars, ie, is empty        */
    /************************************************************************/

    do_v_adjust = ProcFlags.col_started;
    if( do_v_adjust ) {     // check for initial empty doc_element on page
        if( (t_page.last_pane->page_width == NULL) && (
                (t_page.cur_col->main == NULL) || (
                (t_page.cur_col->main->type == ELT_text)
                && (t_page.cur_col->main->element.text.first != NULL)
                && (t_page.cur_col->main->element.text.first->first == NULL))) ) {
            do_v_adjust = false;
        }
    }
    prev_height = wgml_fonts[g_prev_font].line_height;  // needed below even if equal to def_height
    if( ProcFlags.in_bx_box || do_v_adjust ) {
        if( prev_height < def_height ) {
            v_offset += (def_height - prev_height) / 2;
            hl_depth -= (def_height - prev_height) / 2;
        } else if( prev_height > def_height ) {
            v_offset -= (prev_height - def_height) / 2;
            hl_depth += (prev_height - def_height) / 2;
        }
    }

    /* process any accumulated doc_elements */

    if( cur_doc_el_group != NULL ) {
        while( cur_doc_el_group->first != NULL ) {
            if( cur_doc_el_group->depth <= max_depth ) {   // doc_elements will all fit
                cur_el = cur_doc_el_group->first;
                if( cur_el != NULL ) {
                    while( (cur_el != NULL)
                            && (cur_el->type != ELT_hline) && (cur_el->type != ELT_vline) ) {
                        cur_el = cur_el->next;
                    }
                    check_el = cur_el;
                } else {
                    check_el = NULL;
                }
                while( cur_doc_el_group->first != check_el ) {
                    box_line_element( check_el == NULL );
                }
                if( check_el != NULL ) {
                    while( cur_doc_el_group->first == check_el ) {
                        box_line_element( check_el == NULL );
                    }
                }
            } else {                            // finish off current column
                next_column();
                max_depth = t_page.max_depth;
            }
        }
//        box_depth += v_offset;
        add_doc_el_group_to_pool( cur_doc_el_group );
        cur_doc_el_group = NULL;
    }

    /* Now deal with the HLINEs and associated VLINEs */

    ProcFlags.top_line = !ProcFlags.col_started && ((cur_doc_el_group == NULL)
                                    || (cur_doc_el_group->first == NULL));

    if( (box_line->first == NULL) || (cur_op == bx_can) || (cur_op == bx_set) ||
            (!box_line->had_cols && (box_line->next != NULL) && (cur_op == bx_off)) ) {

        /* except for bx_can and bx_set, this means no vline output as well */

        h_line_el = NULL;

        if( ProcFlags.in_bx_box ) {     //  not on first line of box

            /* Update box_depth and el_skip directly since there are no HLINEs */

            if( ProcFlags.top_line ) {
                box_depth += g_top_skip;
                el_skip += g_top_skip;
            } else {
                box_depth += g_subs_skip + g_blank_units_lines;
                el_skip += g_subs_skip + g_blank_units_lines;
                g_blank_units_lines = 0;
            }
        } else {

            /* Update box_skip directly since there are no HLINEs */

            if( ProcFlags.top_line ) {
                box_skip += g_top_skip;
            } else {
                box_skip += g_subs_skip + g_blank_units_lines;
                g_blank_units_lines = 0;
            }
        }

        if( (box_line->next != NULL) && (cur_op == bx_off) ) {    // closing last nested box
            el_skip += v_offset;
        }

    } else {

        /* Create the doc_elements to hold the HLINEs */

        cur_el = NULL;
        cur_hline = box_line->first;
        while( cur_hline != NULL ) {  // iterate over all horizontal lines
            if( cur_el == NULL ) {
                cur_el = init_doc_el( ELT_hline, 0 );
                if( ProcFlags.in_bx_box ) {         // special processing
                    cur_el->subs_skip += el_skip + v_offset;

                    /* Adjust subs_skip if line height difference is odd */

                    if( prev_height < def_height ) {
                        if( (def_height - prev_height) % 2 ) {
//                            cur_el->subs_skip++;
                        }
                    } else if( prev_height > def_height ) {
                        if( (prev_height - def_height) % 2 ) {
//                            cur_el->subs_skip--;
                        }
                    }
                } else {
                    cur_el->subs_skip += box_skip + v_offset;
                    box_skip = 0;
                }
                cur_el->element.hline.ban_adjust = !do_v_adjust;
                h_line_el = cur_el;
            } else {
                cur_el->depth = 0;                          // no vertical drop until last HLINE
                cur_el->next = alloc_doc_el( ELT_hline );
                cur_el = cur_el->next;
                cur_el->subs_skip = 0;
                cur_el->top_skip = 0;
                cur_el->blank_lines = 0;
            }
            cur_el->element.hline.o_subs_skip = cur_el->subs_skip;  // save subs_skip in case needed
            cur_el->element.hline.o_top_skip = cur_el->top_skip;    // save top_skip in case needed

            h_offset = cur_hline->cols[0].col - h_vl_offset;
            if( (int) h_offset < 0 ) {
                h_offset = h_vl_offset;
            }
            if( h_vl_offset < h_offset ) {
                cur_el->element.hline.h_start = h_offset;
            } else {
                cur_el->element.hline.h_start = h_vl_offset;
            }
            if( cur_hline->current > 1) {   // if only one column, default is correct
                cur_el->element.hline.h_len =
                                    (cur_hline->cols[cur_hline->current - 1].col -
                                                        cur_hline->cols[0].col) + 1;
            }
            cur_hline = cur_hline->next;
        }
        if( ProcFlags.in_bx_box ) {     // adjust el_skip for HLINE skips
            if( ProcFlags.top_line ) {
                el_skip += h_line_el->top_skip;
            } else {
                el_skip += h_line_el->subs_skip + h_line_el->blank_lines;
            }
        }
    }

    /********************************************************/
    /* determine if VLINEs will be drawn in conjunction     */
    /* with the current HLINEs (or for BX CAN)              */
    /* the criteria used here are:                          */
    /*   box_line must have at least one column             */
    /* and                                                  */
    /*   at least one of these conditions must be true:     */
    /*       the BX had a column list (g_cur_line has at    */
    /*       least one column) and there was a prior BX     */
    /*       line                                           */
    /*     or                                               */
    /*       BX ON is being processed                       */
    /*     or                                               */
    /*       the end of the current box has been reached)   */
    /*       and it is the outermost box that is ending     */
    /* these criteria may be expanded in the future         */
    /********************************************************/

    ProcFlags.draw_v_line = ((box_line->first != NULL) && (box_line->first->current > 0)
                                && ProcFlags.box_cols_cur && ProcFlags.in_bx_box)
                            || (cur_op == bx_on)
                            || (((cur_op == bx_can ) || (cur_op == bx_off))
                                && (box_line->next != NULL));

    ProcFlags.vline_done = false;   // will be set true if actually drawn

    /****************************************************************/
    /* Perform the appropriate action:                              */
    /*   1. if there is not enough room on the page, move to the    */
    /*      next column (draws VLINEs and resets el_skip/box_depth  */
    /*   2. invoke draw_box_lines()                                 */

/// this may or may not still be correct
    /*   4. if BX OFF or BX CAN (or BX DEL) is closing the last     */
    /*      inner box and there is no HLINE to be drawn, adjust     */
    /*      box_depth and el_skip appropriately                     */
    /****************************************************************/

    max_depth = t_page.max_depth - t_page.cur_depth;    // reset value
    if( !ProcFlags.in_bx_box ) {                    // outermost box starts
        if( cur_op == bx_set ) {
            if( max_depth < (box_skip + (v_offset - 1)) ) {     // to top of next column
                next_column();
                max_depth = t_page.max_depth;
                ProcFlags.top_line = !ProcFlags.col_started;    // reset for new column
            }
        } else if( cur_op == bx_off ) {
            if( max_depth < (box_skip + (v_offset - 1) + def_height) ) {        // to top of next column
                next_column();
                max_depth = t_page.max_depth;
                ProcFlags.top_line = !ProcFlags.col_started;    // reset for new column
            }
        } else {
            if( max_depth < (h_line_el->subs_skip + (v_offset - 1) + def_height) ) {        // to top of next column
                next_column();
                max_depth = t_page.max_depth;
                ProcFlags.top_line = !ProcFlags.col_started;    // reset for new column
            }
        }
        box_depth = 0;                              // top line of box
        draw_box_lines( h_line_el );
        if( (cur_op == bx_set) && !ProcFlags.col_started ) {    // first box line at top of column
            box_depth = def_height;
            ProcFlags.in_bx_box = true;             // box has started
        } else if( cur_op != bx_off ) {
            box_depth = hl_depth;
            ProcFlags.in_bx_box = true;             // box has started
        }
    } else {                                        // inside a box
        if( cur_op == bx_off ) {
/// this may be correct inside an inner box
//            if( max_depth < (box_skip + (v_offset - 1) + def_height) ) {        // to top of next column
/// this appears to be correct for the bx off that closes the outermost box
            if( max_depth < (box_skip + def_height) ) {         // to top of next column
                next_column();
                max_depth = t_page.max_depth;
                ProcFlags.top_line = !ProcFlags.col_started;    // reset for new column
            }
        } else if( max_depth < (el_skip + hl_depth + def_height) ) {    // HLINE to top of column
            next_column();
            max_depth = t_page.max_depth;
            ProcFlags.top_line = !ProcFlags.col_started;        // reset for new column
        }
        if( (h_line_el != NULL) && ProcFlags.in_bx_box ) {      // adjust for HLINE skips
            if( ProcFlags.top_line ) {
                box_depth += h_line_el->top_skip;
            } else {
                box_depth += h_line_el->subs_skip + h_line_el->blank_lines;
            }
        }

        /* Restore box depth when prev_height and def_height differ by an odd amount */

        if( prev_height < def_height ) {
//            box_depth -= (def_height - prev_height) % 2;
        } else if( prev_height > def_height ) {
//            box_depth += (prev_height - def_height) % 2;
        }

        draw_box_lines( h_line_el );

        if( (box_line->next == NULL) && ((cur_op == bx_can ) || (cur_op == bx_off)) ) {          // outer box ends
            box_depth = 0;
            el_skip = 0;
        } else if( h_line_el != NULL ) {                // only if HLINE was drawn
            if( !ProcFlags.vline_done ) {               // if VLINEs not drawn
                box_depth += hl_depth;                  // increment box_depth
            } else {                                    // otherwise
                box_depth = hl_depth;                   // initialize box_depth
                el_skip = 0;                            // initialize el_skip
            }
        }
    }

    /************************************************************/
    /* Initialize the depth of the last HLINE/VLINE and adjust  */
    /* the cur_depth field in either t_page or t_doc_el_group,  */
    /* with these exceptions:                                   */
    /*      1. BX CAN was processed                             */
    /*      2. No HLINEs or VLINEs were done                    */
    /* Adjust element depth when difference in heights is odd   */
    /* Note: because of how doc_element.depth is handled,       */
    /*   hl_depth does not have to be added to el_skip          */
    /************************************************************/

    if( (cur_op != bx_can) && !ProcFlags.no_bx_hline ) {    // HLINEs, VLINEs, or both drawn, presumably
        if( (t_doc_el_group != NULL) && (t_doc_el_group->last != NULL) ) {  // first try t_doc_el_group
            t_doc_el_group->last->depth = hl_depth;
            t_doc_el_group->depth +=hl_depth;
            if( prev_height < def_height ) {
                if( (def_height - prev_height) % 2 ) {
//                    t_doc_el_group->last->depth--;
//                    t_doc_el_group->depth--;
                }
            } else if( prev_height > def_height ) {
                if( (prev_height - def_height) % 2 ) {
//                    t_doc_el_group->last->depth++;
//                    t_doc_el_group->depth++;
                }
            }
        } else if( t_page.last_col_main != NULL ) {         // and then t_page
            t_page.last_col_main->depth = hl_depth;
            t_page.cur_depth +=hl_depth;
            if( prev_height < def_height ) {
                if( (def_height - prev_height) % 2 ) {
//                    t_page.last_col_main->depth--;
//                    t_page.cur_depth--;
                }
            } else if( prev_height > def_height ) {
                if( (prev_height - def_height) % 2 ) {
//                    t_page.last_col_main->depth++;
//                    t_page.cur_depth++;
                }
            }
        }
    }

    /************************************************************/
    /* This matches the behavior of wgml 4.0:                   */
    /*   a new page is emitted if BX OFF or BX ON with no       */
    /*      column list is at the exact bottom of the page      */
    /*   BX CAN never positions its VLINEs at the exact bottom  */
    /*     of the page, so it does not do this -- so far        */
    /* Converted to per-column behavior without testing         */
    /************************************************************/

    if( ((cur_op == bx_off) || ((cur_op == bx_on) && !ProcFlags.box_cols_cur)) &&
            (t_page.max_depth == t_page.cur_depth) ) {
        next_column();
        max_depth = t_page.max_depth;
    }

    return;
}


/***************************************************************************/
/*  end-of-column processing for character devices                         */
/***************************************************************************/

static void eoc_char_device( void ) {
    bool            splittable;
    doc_element *   cur_el;
    unsigned        cur_skip;               // top_skip or subs_skip, as appropriate
    unsigned        skippage;               // lines to skip (not hbus)

    /* process any accumulated doc_elements */

    if( cur_doc_el_group != NULL ) {
        cur_el = cur_doc_el_group->first;
        while( cur_el != NULL ) {
            if( ProcFlags.col_started ) {
                cur_skip = cur_el->subs_skip;
                cur_el->subs_skip = 0;
            } else {
                cur_skip = cur_el->top_skip;
                cur_el->top_skip = 0;
                ProcFlags.col_started = true;
            }
            skippage = cur_el->blank_lines + cur_skip;
            if( (t_page.cur_depth + skippage + cur_el->depth) <= t_page.max_depth ) {

                /* the entire element will fit */

                cur_el->blank_lines = 0;
                if( skippage > 0 ) {            // convert skipped lines to output lines
                    box_blank_lines( skippage );
                }
                max_depth -= skippage + cur_el->depth;
                cur_doc_el_group->depth -= skippage + cur_el->depth;
                cur_doc_el_group->first = cur_el->next;
                cur_el->next = NULL;
                box_char_element( cur_el );
                cur_doc_el_group->depth -= cur_el->depth;
                cur_el = cur_doc_el_group->first;
            } else {                // the entire element will not fit
                if( (t_page.cur_depth + skippage) > t_page.max_depth ) {    // skippage too large
                    if( cur_el->blank_lines > max_depth ) { // split blank_lines
                        cur_el->blank_lines -= max_depth;
                        skippage = max_depth;
                    } else {
                        cur_el->blank_lines = 0;
                    }
                    if( cur_skip > max_depth ) {    // cap top_skip or subs_skip
                        skippage = max_depth;
                    }
                    box_blank_lines( skippage );
                } else if( (t_page.cur_depth + skippage + cur_el->depth) > t_page.max_depth ) {    // element too large
                    splittable = split_element( cur_el, t_page.max_depth -
                                                    t_page.cur_depth - skippage );
                    if( splittable ) {
                        if( cur_el->next != NULL ) {    // cur_el was split
                            cur_doc_el_group->depth -= cur_el->depth;
                            cur_doc_el_group->first = cur_el->next;
                            cur_el->next = NULL;
                            box_char_element( cur_el );
                        }
                    }
                }
                cur_el = NULL;                      // nothing more goes in current column
            }
        }
    }

    /* Now finish off the column */

    if( t_page.max_depth > t_page.cur_depth ) {
        box_blank_lines( t_page.max_depth - t_page.cur_depth );
    }

    return;
}


/***************************************************************************/
/*  end-of-column processing for line-drawing devices                      */
/*  NOTE: work was suspended when work on boxtest.gml began, as it is not  */
/*  clear this is needed for the OW docs; it is even less clear how to     */
/*  handle the problem that the parameter to box_line_element() was meant  */
/*  to solve: would ADDRESS and/or XMP (and perhaps FIG and FN) need to be */
/*  modified also and perhaps called first to decide if the text is to be  */
/*  broken or moved to the next columnpage? This is all very murky.        */
/***************************************************************************/

static void eoc_line_device( void ) {
    bool            splittable;
    box_col_set *   cur_hline;
    unsigned        cur_skip;               // top_skip or subs_skip, as appropriate
    unsigned        skippage       = 0;     // current element skip

    /* process any accumulated doc_elements */

    if( cur_doc_el_group != NULL ) {
        while( cur_doc_el_group->first != NULL ) {
            if( ProcFlags.col_started ) {      // text line not at top of column
                cur_skip = cur_doc_el_group->first->subs_skip;
            } else {
                cur_skip = cur_doc_el_group->first->top_skip;
                ProcFlags.col_started = true;
            }
            max_depth = t_page.max_depth - t_page.cur_depth;    // reset value
            skippage = cur_doc_el_group->first->blank_lines + cur_skip;
            if( (cur_doc_el_group->first->depth + skippage) <= max_depth ) {  // cur_doc_el_group.first will fit on the page
                cur_doc_el_group->depth -= cur_doc_el_group->first->depth;
                box_line_element( true );
            } else {        // the entire element will not fit
                if( skippage > max_depth ) {    // skippage too large
                    if( cur_doc_el_group->first->blank_lines > max_depth ) { // split blank_lines
                        cur_doc_el_group->first->blank_lines -= max_depth;
                        skippage = max_depth;
                    } else {
                        cur_doc_el_group->first->blank_lines = 0;
                    }
                    if( cur_skip > max_depth ) {    // cap top_skip or subs_skip
                        skippage = max_depth;
                    }
                } else if( (cur_doc_el_group->first->depth + skippage) > max_depth ) {  // cur_doc_el_group.first will fill the column
                    splittable = split_element( cur_doc_el_group->first, t_page.max_depth -
                                                t_page.cur_depth - skippage );
                    if( splittable ) {
                        box_depth += skippage + cur_doc_el_group->first->depth;
                        if( cur_doc_el_group->first->next != NULL ) {    // cur_doc_el_group.first was split
                            cur_doc_el_group->depth -= skippage + cur_doc_el_group->first->depth;
                            box_line_element( true );
                        }
                    }
                }
                break;  // end processing & finish column
            }
        }
    }

    /* Now finish off the column */

    max_depth = t_page.max_depth - t_page.cur_depth;    // reset value

    /* This is a guess: the box_depth used must be > 0 */

    if( (box_depth + max_depth) > hl_depth ) {
        box_depth += max_depth - hl_depth;              // finalize current page box_depth
        cur_hline = g_prev_line;
        while( cur_hline != NULL ) {                    // iterate over all horizontal lines
            if( cur_hline == box_line->first ) {
                box_draw_vlines( cur_hline, max_depth - hl_depth, 0, st_none );
            } else {
                box_draw_vlines( cur_hline, 0, 0, st_none );
            }
            cur_hline = cur_hline->next;
        }
    }

    return;
}


/***************************************************************************/
/*  merge g_prev_line and/or g_cur_line into box_line->first                 */
/***************************************************************************/

static void merge_lines( void )
{
    bool            inner_box   = false;    // true if any columns are marked "out" after g_prev_line is preprocessed
    bool            off_multi   = false;    // true for operand OFF when a multi-column segment has been encounterd
    bool            on_gap      = false;    // true if operand ON actually caused a gap
    box_col_set *   box_temp;
    box_col_set *   cur_temp;
    box_col_set *   eoc_save;
    box_col_set *   prev_temp;
    int             box_col;
    unsigned        cur_col;
    unsigned        prev_col;

    if( g_prev_line == NULL ) {
        eoc_save = NULL;
    } else {

        /* Save original g_prev_line for use if the box is split between columns */

        cur_temp = alloc_box_col_set();
        eoc_save = cur_temp;
        prev_temp = g_prev_line;
        while( cur_temp->length < prev_temp->current) {
            resize_box_cols( cur_temp );
        }
        while( prev_temp != NULL) {
            for( prev_col = 0; prev_col < prev_temp->current; prev_col++ ) {
                cur_temp->cols[prev_col].col = prev_temp->cols[prev_col].col;
                cur_temp->cols[prev_col].depth = prev_temp->cols[prev_col].depth;
                cur_temp->cols[prev_col].v_ind = prev_temp->cols[prev_col].v_ind;
                cur_temp->current++;
            }
            prev_temp = prev_temp->next;
            if( prev_temp != NULL ) {

                /* set up next eoc_line member in cur_temp */

                cur_temp->next = alloc_box_col_set();
                cur_temp= cur_temp->next;
                while( cur_temp->length < prev_temp->current) {
                    resize_box_cols( cur_temp );
                }
            }
        }

        /* Now set up prev_temp for merging */

        /* Convert "down" and "new" columns */

        prev_temp = g_prev_line;
        while( prev_temp != NULL ) {
            for( prev_col = 0; prev_col < prev_temp->current; prev_col++ ) {
                if( prev_temp->cols[prev_col].v_ind == BOXV_down ) {
                    prev_temp->cols[prev_col].v_ind = BOXV_up;
                } else if( prev_temp->cols[prev_col].v_ind == BOXV_new ) {
                    prev_temp->cols[prev_col].v_ind = BOXV_hid;
                }
            }
            prev_temp = prev_temp->next;
        }

        if( ProcFlags.box_cols_cur ) {  // if current BX line had columns

            /* Convert "both" and "split" columns to "up" if they do not continue */

            if( (cur_op == bx_none) || (cur_op == bx_on) ) {
                cur_temp = g_cur_line;
                prev_temp = g_prev_line;
                while( prev_temp != NULL ) {
                    for( prev_col = 0; prev_col < prev_temp->current; prev_col++ ) {
                        if( (prev_temp->cols[prev_col].v_ind == BOXV_both)
                            || (prev_temp->cols[prev_col].v_ind == BOXV_split) ) {
                            prev_temp->cols[prev_col].v_ind = BOXV_up;
                            while( cur_temp != NULL ) {
                                for( cur_col = 0; cur_col < cur_temp->current; cur_col++ ) {
                                    if( prev_temp->cols[prev_col].col ==
                                            cur_temp->cols[0].col ) {
                                        prev_temp->cols[prev_col].v_ind = BOXV_split;
                                    }
                                }
                                cur_temp = cur_temp->next;
                                cur_col = 0;
                            }
                        }
                    }
                    prev_temp = prev_temp->next;
                    prev_col = 0;
                }
            }

            /* Identify and mark "new" and "out" columns if appropriate */

            if( ((cur_op == bx_new) || (cur_op == bx_set) || (box_line->next != NULL)) ) {
                cur_temp = g_cur_line;
                prev_temp = g_prev_line;
                cur_col = 0;
                prev_col = 0;
                while( prev_temp != NULL ) {
                    while( cur_temp != NULL ) {
                        if( prev_temp->cols[prev_col].col
                                < cur_temp->cols[0].col ) {
                            prev_temp->cols[prev_col].v_ind = BOXV_out;
                            inner_box = true;
                            break;
                        } else if( prev_temp->cols[prev_col].col
                                < cur_temp->cols[cur_col].col ) {
                            prev_temp->cols[prev_col].v_ind = BOXV_new;
                            break;
                        } else if( prev_temp->cols[prev_col].col
                                == cur_temp->cols[cur_col].col ) {
                            prev_temp->cols[prev_col].v_ind = BOXV_both;
                            break;
                        } else if( (cur_temp->next == NULL)
                            && (prev_temp->cols[prev_col].col
                                > cur_temp->cols[cur_temp->current-1].col) ) {
                            prev_temp->cols[prev_col].v_ind = BOXV_out;
                            inner_box = true;
                            break;
                        } else {
                            cur_col++;
                            if( cur_col == cur_temp->current ) {
                                cur_temp = cur_temp->next;
                                if( cur_temp == NULL ) {
                                    prev_temp->cols[prev_col].v_ind = BOXV_out;
                                    inner_box = true;
                                }
                                cur_col = 0;
                            }
                        }
                    }
                    prev_col++;
                    if( prev_col == prev_temp->current ) {
                        prev_temp = prev_temp->next;
                        prev_col = 0;
                    }
                }
                box_line->inner_box = inner_box;
            }
        }
    }

    if( (g_prev_line == NULL) && (g_cur_line == NULL) ) {
        /* This might be an error if only possible for degenerate cases */
        internal_err_exit( __FILE__, __LINE__ );
        /* never return */
    }
    if( g_prev_line == NULL ) {    // g_cur_line becomes box_line->first
        box_line->first = g_cur_line;
        g_cur_line = NULL;
    } else if( g_cur_line == NULL ) {     // g_prev_line becomes box_line->first
        if( cur_op == bx_none ) {
            prev_temp = g_prev_line;
            while( prev_temp != NULL ) {
                for( prev_col = 0; prev_col < prev_temp->current; prev_col++ ) {
                    if( (prev_temp->cols[prev_col].v_ind != BOXV_hid) &&
                            (prev_temp->cols[prev_col].v_ind != BOXV_new) &&
                            (prev_temp->cols[prev_col].v_ind != BOXV_out)) {
                        prev_temp->cols[prev_col].v_ind = BOXV_both;
                    }
                }
                prev_temp = prev_temp->next;
            }
        } else if( (cur_op == bx_can) || (cur_op == bx_off) ) {
            prev_temp = g_prev_line;
            while( prev_temp != NULL ) {
                for( prev_col = 0; prev_col < prev_temp->current; prev_col++ ) {
                    if( (prev_temp->cols[prev_col].v_ind != BOXV_hid)
                        && (prev_temp->cols[prev_col].v_ind != BOXV_new)
                        && (prev_temp->cols[prev_col].v_ind != BOXV_out)
                        ) {
                        prev_temp->cols[prev_col].v_ind = BOXV_up;
                    }
                }
                prev_temp = prev_temp->next;
            }
        }
        box_line->first = g_prev_line;
        g_prev_line = NULL;

    } else {        // both g_cur_line & g_prev_line have entries
        box_col = 0;
        cur_col = 0;
        prev_col = 0;
        box_temp = alloc_box_col_set(); // initialize box_line->first
        cur_temp = g_cur_line;
        if( (cur_op == bx_off) && (cur_temp->current > 1) ) {
            off_multi = true;
        }
        prev_temp = g_prev_line;
        box_line->first = box_temp;

        /* If there is a head, copy it to box_line->first */

        if( (prev_temp != NULL) && (cur_temp != NULL) ) {
            if( prev_temp->cols[0].col < cur_temp->cols[0].col ) {
                while( (prev_temp->cols[prev_col].col < cur_temp->cols[0].col) ) {
                    box_temp->cols[box_col].col = prev_temp->cols[prev_col].col;
                    box_temp->cols[box_col].depth = prev_temp->cols[prev_col].depth;
                    box_temp->cols[box_col].v_ind = prev_temp->cols[prev_col].v_ind;
                    box_temp->current++;
                    box_col++;
                    prev_col++;
                    if( box_temp->current == box_temp->length) {
                        resize_box_cols( box_temp );
                    }
                    if( box_temp->cols[box_col - 1].v_ind == BOXV_out ) {    // break after "out" column
                        if( box_col > 0 ) {                 // segment must have a column
                            box_temp->next = alloc_box_col_set();
                            box_temp = box_temp->next;
                            box_col = 0;
                        }
                    }
                    if( prev_col == prev_temp->current ) {          // end of segment or "out" column
                        if( box_col > 0 ) {                 // segment must have a column
                            if( (cur_op == bx_new) || (cur_op == bx_set)
                                || (cur_op == bx_off) || (cur_op == bx_on) ) {  // new/off: break
                                box_temp->next = alloc_box_col_set();
                                box_temp = box_temp->next;
                                box_col = 0;
                                on_gap = true;
                            }
                        }
                        if( prev_temp->next == NULL ) {
                            break;          // need joint gap check
                        }
                        if( prev_temp->next->cols[0].col < cur_temp->cols[0].col ) {
                            prev_temp = prev_temp->next;
                            prev_col = 0;
                        } else {
                            break;  // need joint gap check
                        }
                    }
                }
            } else if( cur_temp->cols[0].col < prev_temp->cols[0].col ) {
                while( (cur_temp->cols[cur_col].col < prev_temp->cols[0].col) ) {
                    box_temp->cols[box_col].col = cur_temp->cols[cur_col].col;
                    box_temp->cols[box_col].depth = cur_temp->cols[cur_col].depth;
                    box_temp->cols[box_col].v_ind = cur_temp->cols[cur_col].v_ind;
                    box_temp->current++;
                    box_col++;
                    cur_col++;
                    if( box_temp->current == box_temp->length) {
                        resize_box_cols( box_temp );
                    }
                    if( cur_col == cur_temp->current ) {    // end of segment
                        if( box_col > 0 ) {                 // segment must have a column
                            if( ((cur_op == bx_off) && !off_multi)
                                || ((cur_op == bx_new) || (cur_op == bx_set))
                                || ((cur_op == bx_on) && ((cur_temp->next == NULL)
                        || (cur_temp->cols[0].col < prev_temp->cols[0].col)
                    || (cur_temp->next->cols[0].col < prev_temp->cols[0].col))) ) {
                                box_temp->next = alloc_box_col_set();
                                box_temp = box_temp->next;
                                box_col = 0;
                                on_gap = true;
                            } else {
                                box_temp->cols[box_col - 1].rebreak = true;
                            }
                        }
                        if( cur_temp->next == NULL ) {
                            break;          // need joint gap check
                        }
                        if( cur_temp->next->cols[0].col < prev_temp->cols[0].col ) {
                            cur_temp = cur_temp->next;
                            cur_col = 0;
                            if( (cur_op == bx_off) && (cur_temp->current > 1) ) {
                                off_multi = true;
                            }
                        } else {
                            break;  // need joint gap check
                        }
                    }
                }
            }
        }

        /* If there is a main body, do the merge per the Wiki */

        while( (prev_temp != NULL) && (cur_temp != NULL)
            && ((prev_temp->next != NULL) || (prev_col == prev_temp->current))
            && ((cur_temp->next != NULL) || (cur_col == cur_temp->current)) ) {

            if( (prev_col == prev_temp->current) && (cur_col == cur_temp->current) ) {

                /* Joint gap found */

                if( box_col > 0 ) {                 // segment must have a column
                    if( prev_temp->cols[prev_temp->current - 1].col
                            == cur_temp->cols[cur_temp->current - 1].col ) {
                        box_temp->cols[box_temp->current - 1].rebreak = true;
                    } else {
                        box_temp->next = alloc_box_col_set();
                        box_temp = box_temp->next;
                        box_col = 0;
                    }
                }

                cur_temp = cur_temp->next;      // end of segment per condition above
                if( cur_temp != NULL ) {
                    cur_col = 0;
                }

                prev_temp = prev_temp->next;    // end of segment per condition above
                if( prev_temp != NULL) {
                    prev_col = 0;
                }

            } else if( prev_col == prev_temp->current ) {

                /* Copy cur_temp segment to start of next prev_temp segment */

                while( cur_temp->cols[cur_col].col < prev_temp->next->cols[0].col ) {
                    box_temp->cols[box_col].col = cur_temp->cols[cur_col].col;
                    box_temp->cols[box_col].depth = cur_temp->cols[cur_col].depth;
                    box_temp->cols[box_col].v_ind = cur_temp->cols[cur_col].v_ind;
                    box_temp->current++;
                    if( box_temp->current == box_temp->length) {
                        resize_box_cols( box_temp );
                    }
                    box_col++;
                    cur_col++;
                    if( cur_col == cur_temp->current ) { // end of segment
                        if( box_col > 0 ) {                 // segment must have a column
                            box_temp->cols[box_col - 1].rebreak = true;
                        }
                        break;
                    }
                }
                if( cur_col < cur_temp->current ) { // segments overlap
                    prev_temp = prev_temp->next;    // end of segment per condition above
                    if( prev_temp != NULL) {
                        prev_col = 0;
                    }
                    if( prev_temp->next == NULL ) {
                        break;
                    }
                }
            } else if( cur_col == cur_temp->current ) {

                /* Copy prev_temp segment to start of next cur_temp segment */

                while( prev_temp->cols[prev_col].col < cur_temp->next->cols[0].col ) {
                    box_temp->cols[box_col].col = prev_temp->cols[prev_col].col;
                    box_temp->cols[box_col].depth = prev_temp->cols[prev_col].depth;
                    box_temp->cols[box_col].v_ind = prev_temp->cols[prev_col].v_ind;
                    box_temp->current++;
                    if( box_temp->current == box_temp->length) {
                        resize_box_cols( box_temp );
                    }
                    box_col++;
                    prev_col++;
                    if( prev_col == prev_temp->current ){
                        break;
                    }
                }
                if( prev_col < prev_temp->current ) { // segments overlap
                    cur_temp = cur_temp->next;      // end of segment per condition above
                    if( cur_temp != NULL ) {
                        cur_col = 0;
                    }
                    if( cur_temp->next == NULL ) {
                        break;
                    }
                }
            } else if( prev_temp->cols[prev_col].col < cur_temp->cols[cur_col].col ) {
                while( prev_temp->cols[prev_col].col < cur_temp->cols[cur_col].col ) {
                    box_temp->cols[box_col].col = prev_temp->cols[prev_col].col;
                    box_temp->cols[box_col].depth = prev_temp->cols[prev_col].depth;
                    box_temp->cols[box_col].v_ind = prev_temp->cols[prev_col].v_ind;
                    box_temp->current++;
                    if( box_temp->current == box_temp->length) {
                        resize_box_cols( box_temp );
                    }
                    box_col++;
                    prev_col++;
                    if( prev_col == prev_temp->current ){
                        break;
                    }
                }

            } else if( cur_temp->cols[cur_col].col < prev_temp->cols[prev_col].col ) {
                while( cur_temp->cols[cur_col].col < prev_temp->cols[prev_col].col ) {
                    box_temp->cols[box_col].col = cur_temp->cols[cur_col].col;
                    box_temp->cols[box_col].depth = cur_temp->cols[cur_col].depth;
                    box_temp->cols[box_col].v_ind = cur_temp->cols[cur_col].v_ind;
                    box_temp->current++;
                    if( box_temp->current == box_temp->length) {
                        resize_box_cols( box_temp );
                    }
                    box_col++;
                    cur_col++;
                    if( cur_col == cur_temp->current ){
                        if( box_col > 0 ) {                 // segment must have a column
                            box_temp->cols[box_col - 1].rebreak = true;
                        }
                        break;
                    }
                }

            } else {    // both have the same column
                while( cur_temp->cols[cur_col].col == prev_temp->cols[prev_col].col ) {
                    box_temp->cols[box_col].col = prev_temp->cols[prev_col].col;
                    box_temp->cols[box_col].depth = prev_temp->cols[prev_col].depth;
                    if( cur_op == bx_off ) {
                        box_temp->cols[box_col].v_ind = BOXV_up;
                    } else {
                        box_temp->cols[box_col].v_ind = BOXV_split;
                    }
                    box_temp->current++;
                    if( box_temp->current == box_temp->length) {
                        resize_box_cols( box_temp );
                    }
                    box_col++;
                    cur_col++;
                    prev_col++;
                    if( (cur_col == cur_temp->current)
                            || (prev_col == prev_temp->current) ){
                        break;
                    }
                }
            }
        }

        /* Now process the rest of the line, including the tail */

        while( (cur_temp != NULL) || (prev_temp != NULL) ) {
            if( cur_temp == NULL ) {    // only g_prev_line has columns
                while( prev_temp != NULL ) {
                    while( prev_col < prev_temp->current ) {
                        box_temp->cols[box_col].col = prev_temp->cols[prev_col].col;
                        box_temp->cols[box_col].depth = prev_temp->cols[prev_col].depth;
                        box_temp->cols[box_col].v_ind = prev_temp->cols[prev_col].v_ind;
                        box_temp->current++;
                        box_col++;
                        prev_col++;
                        if( box_temp->current == box_temp->length) {
                            resize_box_cols( box_temp );
                        }
                        if( box_col > 0 ) {                 // segment must have a column
                            if( box_temp->cols[box_col - 1].v_ind == BOXV_out ) {   // break after "out" column
                                box_temp->next = alloc_box_col_set();
                                box_temp = box_temp->next;
                                box_col = 0;
                            }
                        }
                    }
                    if( prev_col == prev_temp->current ) {  // end of segment
                        if( (box_col > 0) ) {               // segment must have a column
                            if( (cur_op == bx_off) || (on_gap && (cur_op == bx_on)) ) {
                                box_temp->next = alloc_box_col_set();
                                box_temp = box_temp->next;
                                box_col = 0;
                            }
                        }
                        prev_temp = prev_temp->next;
                        if( prev_temp != NULL) {
                            prev_col = 0;
                        }
                    }
                }
            } else if( prev_temp == NULL ) {    // only g_cur_line has columns
                while( cur_temp != NULL ) {
                    while( cur_col < cur_temp->current ) {
                        box_temp->cols[box_col].col = cur_temp->cols[cur_col].col;
                        box_temp->cols[box_col].depth = cur_temp->cols[cur_col].depth;
                        box_temp->cols[box_col].v_ind = cur_temp->cols[cur_col].v_ind;
                        box_temp->current++;
                        box_col++;
                        cur_col++;
                        if( box_temp->current == box_temp->length) {
                            resize_box_cols( box_temp );
                        }
                    }
                    if( cur_col == cur_temp->current ) {  // end of segment
                        if( (box_col > 0) ) {               // segment must have a column
                            if( (cur_op == bx_off) || (on_gap && (cur_op == bx_on)) ) {
                                box_temp->next = alloc_box_col_set();
                                box_temp = box_temp->next;
                                box_col = 0;
                            } else {
                                box_temp->cols[box_col - 1].rebreak = true;
                            }
                        }
                        cur_temp = cur_temp->next;
                        if( cur_temp != NULL) {
                            cur_col = 0;
                        }
                    }
                }
            } else {    // both have columns
                if( prev_temp->cols[prev_col].col < cur_temp->cols[cur_col].col ) {
                    while( (prev_temp->cols[prev_col].col < cur_temp->cols[cur_col].col)
                            && (prev_col < prev_temp->current) ) {
                        box_temp->cols[box_col].col = prev_temp->cols[prev_col].col;
                        box_temp->cols[box_col].depth = prev_temp->cols[prev_col].depth;
                        box_temp->cols[box_col].v_ind = prev_temp->cols[prev_col].v_ind;
                        box_temp->current++;
                        box_col++;
                        prev_col++;
                        if( box_temp->current == box_temp->length) {
                            resize_box_cols( box_temp );
                        }
                        if( box_col > 0 ) {                 // segment must have a column
                            if( box_temp->cols[box_col - 1].v_ind == BOXV_out ) {   // break after "out" column
                                box_temp->next = alloc_box_col_set();
                                box_temp = box_temp->next;
                                box_col = 0;
                            }
                        }
                    }
                    if( prev_col == prev_temp->current ) {  // end of segment
                        if( (box_col > 0) ) {               // segment must have a column
                            if( ((cur_op == bx_off) && (prev_temp->next == NULL)
                                && (prev_temp != g_prev_line))
                                || (on_gap && ((cur_op == bx_on)
                && ((cur_temp->current == 1) || (cur_col < cur_temp->current - 1)))) ) {
                                box_temp->next = alloc_box_col_set();
                                box_temp = box_temp->next;
                                box_col = 0;
                            }
                        }
                        prev_temp = prev_temp->next;
                        if( prev_temp != NULL ) {
                            prev_col = 0;
                        }
                    }
                } else if( cur_temp->cols[cur_col].col < prev_temp->cols[prev_col].col ) {
                    while( (cur_temp->cols[cur_col].col < prev_temp->cols[prev_col].col)
                            && (cur_col < cur_temp->current) ) {
                        box_temp->cols[box_col].col = cur_temp->cols[cur_col].col;
                        box_temp->cols[box_col].depth = cur_temp->cols[cur_col].depth;
                        box_temp->cols[box_col].v_ind = cur_temp->cols[cur_col].v_ind;
                        box_temp->current++;
                        box_col++;
                        cur_col++;
                        if( box_temp->current == box_temp->length) {
                            resize_box_cols( box_temp );
                        }
                    }
                    if( prev_temp->cols[prev_col].v_ind == BOXV_out ) { // break before "out" column
                        if( (box_col > 0) ) {               // segment must have a column
                            box_temp->next = alloc_box_col_set();
                            box_temp = box_temp->next;
                            box_col = 0;
                        }
                    }
                    if( cur_col == cur_temp->current ) {    // end of segment or of inner box
                        if( (box_col > 0) ) {               // segment must have a column
                            if( ((cur_op == bx_off) && (cur_temp->next == NULL)
                                    && (cur_temp != g_cur_line))
                                || ((cur_op == bx_new) || (cur_op == bx_set))
                                || (on_gap && (cur_op == bx_on)
                && ((prev_temp->current == 1) || (prev_col < prev_temp->current - 1))) ) {
                                box_temp->next = alloc_box_col_set();
                                box_temp = box_temp->next;
                                box_col = 0;
                            } else {
                                box_temp->cols[box_col - 1].rebreak = true;
                            }
                        }
                        cur_temp = cur_temp->next;
                        if( cur_temp != NULL) {
                            cur_col = 0;
                        }
                    }
                } else {        // both have the same column
                    while( (cur_temp->cols[cur_col].col == prev_temp->cols[prev_col].col)
                            && ((cur_col < cur_temp->current)
                            && (prev_col < prev_temp->current)) ) {
                        box_temp->cols[box_col].col = prev_temp->cols[prev_col].col;
                        box_temp->cols[box_col].depth = prev_temp->cols[prev_col].depth;
                        if( cur_op == bx_off ) {
                            box_temp->cols[box_col].v_ind = BOXV_up;
                        } else if( (cur_op == bx_new) || (cur_op == bx_set) ) {
                            box_temp->cols[box_col].v_ind = BOXV_both;
                        } else {
                            box_temp->cols[box_col].v_ind = BOXV_split;
                        }
                        box_temp->current++;
                        box_col++;
                        cur_col++;
                        prev_col++;
                        if( box_temp->current == box_temp->length) {
                            resize_box_cols( box_temp );
                        }
                    }
                    if( cur_col == cur_temp->current ) {  // end of segment
                        if( box_col > 0 ) {                 // segment must have a column
                            if( ((cur_op == bx_off) && (cur_temp->next == NULL)
                                    && (cur_temp != g_cur_line))
                                || ((cur_op == bx_new) || (cur_op == bx_set))
                                    || (on_gap && (cur_op == bx_on)) ) {
                                box_temp->next = alloc_box_col_set();
                                box_temp = box_temp->next;
                                box_col = 0;
                            } else {
                                box_temp->cols[box_col - 1].rebreak = true;
                            }
                        }
                        cur_temp = cur_temp->next;
                        if( cur_temp != NULL ) {
                            cur_col = 0;
                        }
                    }
                    if( (prev_col < prev_temp->current)
                        && (prev_temp->cols[prev_col].v_ind == BOXV_out) ) {    // break before "out" column
                        if( (box_col > 0) ) {               // segment must have a column
                            box_temp->next = alloc_box_col_set();
                            box_temp = box_temp->next;
                            box_col = 0;
                        }
                    } else if( prev_col == prev_temp->current ) {  // end of segment
                        if( (box_col > 0) ) {               // segment must have a column
                            if( ((cur_op == bx_off) && (prev_temp->next == NULL)
                                && (prev_temp != g_prev_line))
                                    || (on_gap && (cur_op == bx_on)) ) {
                                box_temp->next = alloc_box_col_set();
                                box_temp = box_temp->next;
                                box_col = 0;
                            }
                        }
                        prev_temp = prev_temp->next;
                        if( prev_temp != NULL ) {
                            prev_col = 0;
                        }
                    }
                }
            }
        }
        if( box_temp->current == 0 ) {     // final segment might be empty
            cur_temp = box_temp;           // detach & return to pool
            if( cur_temp == box_line->first ) {   // first segment, box_linw is empty
                g_prev_line = NULL;
            } else {                        // find preceding segment
                box_temp = box_line->first;
                while( box_temp->next->next != NULL ) {
                    box_temp = box_temp->next;
                }
                box_temp->next = NULL;     // detach last segment
            }
            add_box_col_set_to_pool( cur_temp );
        }
    }

    /* Restore original g_prev_line for use if the box is split between columns */

    if( g_prev_line != NULL ) {
        add_box_col_set_to_pool( g_prev_line  );
        g_prev_line = NULL;
    }
    g_prev_line = eoc_save;

    if( g_cur_line != NULL ) {
        add_box_col_set_to_pool( g_cur_line );
        g_cur_line = NULL;
    }

    return;
}

/***************************************************************************/
/*  end-of-column processing                                               */
/*  box_line should be NULL at entry and exit, but be useable in between   */
/*  the method used only works because box_line is not modified            */
/***************************************************************************/

void eoc_bx_box( void ) {

    bx_op               sav_cur_op;

    sav_cur_op = cur_op;
    cur_op = bx_eoc;                        // do eoc processing
    ProcFlags.col_started = (t_page.last_col_main != NULL);

    max_depth = t_page.max_depth - t_page.cur_depth;

    /************************************************************/
    /* This should always match the equivalent code in scr_bx() */
    /************************************************************/

    if( bin_driver->hline.text == NULL ) {
        eoc_char_device();
    } else {
        eoc_line_device();
    }

    cur_op = sav_cur_op;                    // restore value on entry

    box_depth = 0;
    el_skip = 0;

    return;
}

/***************************************************************************/
/*  implement control word BX                                              */
/*  some notes on how wgml 4.0 appears to differ from the TSO:             */
/*    ON <h1 </> ... hn> differs from <h1 </> ... hn> in that columns      */
/*      added outside the existing list of columns are treated as if "/"   */
/*      separated them from the first (if before) or last (if after)       */
/*      column, but only if there is one value after ON                    */
/*    SET, in character devices, produces a blank output line; in PS, it   */
/*      appears that no output at all occurs                               */
/*    CHAR is accepted but has no effect on the output; indeed, any token, */
/*      including HEX, or no token at all, can follow CHAR with no effect  */
/*      on the output and no error message                                 */
/*  despite what the TSO says at the start, operands BEGIN, END, YES, NO,  */
/*      and PURGE are not recognized by wgml 4.0 when used with BX         */
/*  control word AD is not used in the Open Watcom documents and so has    */
/*      not been implemented. Were it to be implemented, it's effects      */
/*      would be part of the left start position for text from which the   */
/*      BX columns are measured                                            */
/***************************************************************************/

void scr_bx( void )
{
    bool                first_col;
    box_col_set         *box_temp;
    box_col_set         *cur_temp;
    box_col_set         *prev_temp;
    box_col_stack       *stack_temp;
    const char          *p;
    const char          *pa;
    unsigned            box_col;
    unsigned            cur_col;
    unsigned            i;
    unsigned            prev_col;
    unsigned            len;
    su                  boxcolwork;
    int             boxcol_cur;
    int             boxcol_prev;

    start_doc_sect();                   // if not already done

    ProcFlags.box_cols_cur = false;     // new BX line: no box column list yet
    ProcFlags.no_bx_hline = false;      // emit horizontal line by default
    ProcFlags.cc_cp_done = false;       // cancel CC/CP notification
    if( !ProcFlags.in_bx_box ) {        // first BX line
        bx_font = g_prev_font;          // capture font before first bx line
        first_doc_el = true;            // mark first doc element in box
    }
    sk_font = g_prev_font;              // default value; may differ from bx_font

    skips_done = false;

    if( ProcFlags.in_bx_box && (t_doc_el_group != NULL) ) {
        cur_doc_el_group = t_doc_el_group;      // detach current element group
        t_doc_el_group = t_doc_el_group->next;  // processed doc_elements go to next group, if any
        cur_doc_el_group->next = NULL;
    } else {
        cur_doc_el_group = NULL;                // top line of outer box
    }

    p = scandata.s;
    SkipSpaces( p );
    pa = p;
    SkipNonSpaces( p );
    len = p - pa;

    /* Identify any non-numeric operand */

    cur_op = bx_none;
    if( (len == 2) && strnicmp( "on", pa , len ) == 0 ) {
        cur_op = bx_on;
    } else if( len == 3 ) {
        if( strnicmp( "can", pa , len ) == 0 || strnicmp( "del", pa , len ) == 0 ) {
            cur_op = bx_can;
        } else if( strnicmp( "new", pa , len ) == 0 ) {
            cur_op = bx_new;
        } else if( strnicmp( "off", pa , len ) == 0 ) {
            cur_op = bx_off;
        } else if( strnicmp( "set", pa , len ) == 0 ) {
            cur_op = bx_set;
        }
    } else if( (len == 4) && strnicmp( "char", pa , len ) == 0 ) {
        scan_restart = scandata.e;
        return;
    }

    if( cur_op == bx_none ) {                   // reuse token if not recognized
        p = pa;
    }

    SkipSpaces( p );

    if( !ProcFlags.in_bx_box && *p == '\0' ) {  // if not in a box, box columns must be given
        scan_restart = scandata.e;
        return;
    }

    if( *p != '\0' && (cur_op == bx_can) ) {    // CAN and DEL cannot have column lists
        xx_line_err_exit_c( ERR_TOO_MANY_OPS, pa );
        /* never return */
    }

    /* Now for the box column list, if any */

    if( *p != '\0' ) {
        ProcFlags.box_cols_cur = true;      // box column list found
        cur_temp = alloc_box_col_set();
        g_cur_line = cur_temp;
        first_col = true;                   // first column not yet found
        boxcol_prev = 0;
        while( *p != '\0' ) {
            if( cur_temp->current == cur_temp->length ) {
                resize_box_cols( cur_temp );
            }
            cur_temp->cols[cur_temp->current].v_ind = BOXV_down;
            if( cur_temp->current == 0 ) {
                if( *p == '/' ) {
                    xx_line_err_exit_c( ERR_SPC_NOT_VALID, p );
                    /* never return */
                }
            } else if( *p == '/' ) {
                cur_temp->next = alloc_box_col_set();
                cur_temp = cur_temp->next;
                p++;
                SkipSpaces( p );
                continue;
            }
            pa = p;
            if( !cw_val_to_su( &p, &boxcolwork ) ) {
                boxcol_cur = conv_hor_unit( &boxcolwork, g_curr_font );
                if( boxcol_cur <= 0 ) {
                    xx_line_err_exit_c( ERR_INV_BOX_POS, pa );
                    /* never return */
                }
                if( first_col ) {   // no prior column
                    first_col = false;
                } else {
                    if( boxcolwork.su_relative ) {
                        boxcol_cur += boxcol_prev;
                    }
                    if( boxcol_cur <= boxcol_prev ) {
                        xx_line_err_exit_c( ERR_BOX_BAD_ORDER, pa );
                        /* never return */
                    }
                }
                if( boxcol_cur > 0 ) {        // treat as "+0" to minimize mischief
                    boxcol_prev = boxcol_cur;
                }
                cur_temp->cols[cur_temp->current].col = boxcol_prev;
            } else {
                xx_line_err_exit_c( ERR_SPC_NOT_VALID, pa );
                /* never return */
            }
            SkipSpaces( p );
            cur_temp->current++;
        }
    }

    /* Generate the list of box columns for the current horizontal line */

    if( box_line == NULL ) {    // initialize stack
        box_line = alloc_box_col_stack();
    }
    switch( cur_op ) {
    case bx_new :
    case bx_set :                   // modify stack first
       if( g_prev_line != NULL ) {  // add g_prev_line to stack

            /* set up first box_line->next member in box_temp */

            box_temp = alloc_box_col_set();
            box_line->first = box_temp;
            prev_temp = g_prev_line;
            while( box_temp->length < prev_temp->length) {
                resize_box_cols( box_temp );
            }
            while( prev_temp != NULL) {
                for( i = 0; i < prev_temp->current; i++ ) {
                    box_temp->cols[i].col = prev_temp->cols[i].col;
                    if( prev_temp->cols[i].v_ind == BOXV_new ) {
                        box_temp->cols[i].v_ind = BOXV_down;
                    } else {
                        box_temp->cols[i].v_ind = prev_temp->cols[i].v_ind;
                    }
                    box_temp->current++;
                }
                prev_temp = prev_temp->next;
                if( prev_temp != NULL ) {

                    /* set up next box_line->next member in box_temp */

                    box_temp->next = alloc_box_col_set();
                    box_temp = box_temp->next;
                    while( box_temp->length < prev_temp->length) {
                        resize_box_cols( box_temp );
                    }
                }
            }
        }
        stack_temp = alloc_box_col_stack();
        stack_temp->next = box_line;
        box_line = stack_temp;
        box_line->had_cols = ProcFlags.box_cols_cur;    // drop through to merge_lines()
    case bx_none :
    case bx_can :
    case bx_off :
    case bx_on :
        merge_lines();
        break;
    default :
        internal_err_exit( __FILE__, __LINE__ );
        /* never return */
    }

    /* set the ProcFlags specific to BX */

    ProcFlags.col_started = (t_page.last_col_main != NULL);
    ProcFlags.no_bx_hline = (cur_op == bx_set) ||
                            ((cur_op == bx_new) && !ProcFlags.box_cols_cur);

    /* set the basic layout values for BX */

    def_height = wgml_fonts[g_curr_font].line_height;       // normal box line height
    max_depth = t_page.max_depth - t_page.cur_depth;        // maximum depth available

    if( !ProcFlags.no_bx_hline || (cur_op == bx_set) ) {
        set_skip_vars( NULL, NULL, NULL, g_text_spacing, bin_device->box.font );

        /************************************************************/
        /* This will cause the box to be drawn with BOX characters  */
        /* even for a page addressing device which does not define  */
        /* an HLINE block. This is not quite the same as wgml 4.0   */
        /* since in that case wgml 4.0 does not draw the vertical   */
        /* ascenders but do_char_device() will. This may or may not */
        /* require future adjustment.                               */
        /************************************************************/

        if( bin_driver->hline.text == NULL ) {
            do_char_device();
        } else {
            do_line_device();
        }
    }

    if( g_prev_line != NULL ) {
        add_box_col_set_to_pool( g_prev_line );
        g_prev_line = NULL;
    }

    if( (cur_op == bx_off) || (cur_op == bx_can)
            || ((cur_op == bx_on) && !ProcFlags.box_cols_cur) ) {

        /********************************************************/
        /* Propagate depth increments to prior stack entry and  */
        /*   change all "BOXV_hid" columns to "BOXV_down"       */
        /* Note: since inner_box is only set by BX NEW or BX    */
        /*   SET, it cannot be true on the outer box, and       */
        /*   and box_line->next cannot be NULL                  */
        /********************************************************/

        if( box_line->inner_box ) {
            cur_temp = box_line->first;
            prev_temp = box_line->next->first;
            prev_col = 0;
            while( cur_temp != NULL ) {
                for( cur_col = 0; cur_col < cur_temp->current; cur_col++ ) {
                    if( cur_temp->cols[cur_col].v_ind == BOXV_out ) {
                        while( prev_temp != NULL ) {
                            for( ; prev_col < prev_temp->current; prev_col++ ) {
                                if( cur_temp->cols[cur_col].col ==
                                            prev_temp->cols[prev_col].col ) {
                                    prev_temp->cols[prev_col].depth =
                                            cur_temp->cols[cur_col].depth;
                                    break;
                                }
                            }
                            if( cur_temp->cols[cur_col].col ==
                                            prev_temp->cols[prev_col].col ) {
                                break;
                            }
                            prev_temp = prev_temp->next;
                            prev_col = 0;
                        }
                    }
                }
                cur_temp = cur_temp->next;
            }
        }

        /* Remove the first element from the stack */

        stack_temp = box_line;
        box_line = box_line->next;
        stack_temp->next = NULL;
        if( stack_temp->first != NULL ) {
            add_box_col_set_to_pool( stack_temp->first );
            stack_temp->first = NULL;
        }
        add_box_col_stack_to_pool( stack_temp );

        /* If the stack is empty, the entire box has ended */

        if( box_line == NULL ) {
            ProcFlags.in_bx_box = false;
            box_depth = 0;
            el_skip = 0;
            cur_op = bx_none;
            first_doc_el = false;
        } else {
            cur_doc_el_group = alloc_doc_el_group( GRT_bx );
            cur_doc_el_group->next = t_doc_el_group;
            t_doc_el_group = cur_doc_el_group;
            cur_doc_el_group = NULL;
        }
    } else {
        cur_doc_el_group = alloc_doc_el_group( GRT_bx );
        cur_doc_el_group->next = t_doc_el_group;
        t_doc_el_group = cur_doc_el_group;
        cur_doc_el_group = NULL;
    }

    if( box_line != NULL ) {

        /********************************************************************/
        /* box_line->first is often the boundary between two boxes, one of  */
        /*   which is being closed and the other of which is being opened.  */
        /*   During the copy to g_prev_line, the columns unique to the box  */
        /*   being closed should be removed, any empty segments skipped,    */
        /*   and any "rebreak" segments restored in g_prev_line.            */
        /* NOTE: this is needed to retain the "out" columns, that is, the   */
        /*   columns of the outer box which must be drawn when the current  */
        /*   inner box is closed.                                           */
        /********************************************************************/

        box_temp = box_line->first;
        g_prev_line = alloc_box_col_set();
        prev_temp = g_prev_line;
        prev_col = 0;
        while( box_temp != NULL ) {
            while( prev_temp->length < box_temp->current) {
                resize_box_cols( prev_temp );
            }
            for( box_col = 0; box_col < box_temp->current; box_col++ ) {
                if( box_temp->cols[box_col].v_ind != BOXV_up ) {    // skip "up" columns
                    prev_temp->cols[prev_col].col = box_temp->cols[box_col].col;
                    prev_temp->cols[prev_col].depth = box_temp->cols[box_col].depth;
                    prev_temp->cols[prev_col].v_ind = box_temp->cols[box_col].v_ind;
                    prev_temp->current++;
                    if( box_temp->cols[box_col].rebreak ) {         // g_prev_line new segment
                        prev_temp->cols[prev_col].rebreak = false;
                        prev_temp->next = alloc_box_col_set();
                        prev_temp = prev_temp->next;
                        prev_col = 0;
                        while( prev_temp->length < box_temp->current) {
                            resize_box_cols( prev_temp );
                        }
                    } else {
                        prev_col++;
                    }
                }
            }
            box_temp = box_temp->next;
            if( box_temp != NULL ) {
                if( prev_col > 0 ) {        // segment must have a column
                    prev_temp->next = alloc_box_col_set();
                    prev_temp = prev_temp->next;
                    prev_col = 0;
                    while( prev_temp->length < box_temp->current) {
                        resize_box_cols( prev_temp );
                    }
                }
            }
        }

        if( prev_temp->current == 0 ) {     // final segment might be empty
            cur_temp = prev_temp;           // detach & return to pool
            if( cur_temp == g_prev_line ) { // first segment, g_prev_line is empty
                g_prev_line = NULL;
            } else {                        // find preceding segment
                prev_temp = g_prev_line;
                while( prev_temp->next->next != NULL ) {
                    prev_temp = prev_temp->next;
                }
                prev_temp->next = NULL;     // detach last segment
            }
            add_box_col_set_to_pool( cur_temp );
        }

        add_box_col_set_to_pool( box_line->first );
        box_line->first = NULL;

    }

    prev_op = cur_op;

    set_h_start();                          // pick up any indents

    scan_restart = scandata.e;

    return;
}

