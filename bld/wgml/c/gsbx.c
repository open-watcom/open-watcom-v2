/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2004-2013 The Open Watcom Contributors. All Rights Reserved.
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
*       eop_bx_box()            end-of-page for boxes drawn with BX
*       eop_char_device()       end-of-page for character devices
*       eop_line_device()       end-of-page for line devices
*       merge_lines()           merge cur_line and/or prev_line into box_line
*       resize_box_cols()       resize a box_col_set.cols member
*       scr_bx()                parse BX line and use other functions to perform
*                                   the required actions
*   initial descriptive comments are from script-tso.txt
****************************************************************************/

#include "wgml.h"
#include "gvars.h"

#include "clibext.h"

/**************************************************************************/
/* BOX generates the horizontal lines and initiates the vertical lines of */
/* a box.                                                                 */
/*                                                                        */
/* 旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커           */
/* |     |                                                    |           */
/* |     | <h1 </> ... hn>                                    |           */
/* | .BX | <NEW|ON|OFF|SET> <h1 </> ... hn>                   |           */
/* |     | <CANCEL>                                           |           */
/* |     | <CHAR name>                                        |           */
/* |     |                                                    |           */
/* 읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸           */
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
    bx_eop,         // end-of-page
    bx_on,          // BX ON
    bx_off,         // BX OFF
    bx_new,         // BX NEW
    bx_set,         // BX SET
    bx_can,         // BX CAN or BX DEL
} bx_op;            // the BX operators

typedef enum {
    st_none,        // the normal case
    st_down,        // do stubs for bx_v_down columns
    st_ext,         // stub height is given in box_height, stubs only
    st_up,          // do stubs for bx_v_up columns
} stub_type;

static  bx_op       cur_op          = bx_none;  // current BX operator
static  spacing_bu  box_depth       = 0;        // depth of box (used with VLINES)
static  spacing_bu  def_height      = 0;        // default font line height
static  spacing_bu  box_skip        = 0;        // skip for outer (entire) box
static  spacing_bu  el_skip         = 0;        // skip for current element
static  spacing_bu  hl_depth        = 0;        // height from VLINE drawn to lower boundary of def_height
static  spacing_bu  v_offset        = 0;        // space reserved for the box line which is above the line drawn


/***************************************************************************/
/*  resize the cols member of a box_col_set                               */
/***************************************************************************/

static box_col_set * resize_box_cols( box_col_set * in_cols ) {

    in_cols->length += BOXCOL_COUNT;               // add space for new box columns
    in_cols->cols = mem_realloc( in_cols->cols, in_cols->length * sizeof( box_col ) );
    memset( &in_cols->cols[in_cols->current], 0, BOXCOL_COUNT * sizeof( box_col ));
    return( in_cols );
}
/***************************************************************************/
/*  output blank lines with vertical box characters                        */
/***************************************************************************/

static void box_blank_lines( spacing_bu lines_bu )
{
    box_col_set *   cur_hline;
    doc_element *   blank_el;
    int             i;
    int             i_b;        // box_line index
    text_chars  *   cur_chars;
    text_line   *   cur_blank;
    int             lines;

    if( lines_bu == 0 ) {          // why are we here?
        return;
    }

    blank_el = alloc_doc_el( el_text );
    blank_el->depth = lines_bu + g_spacing;
    lines = lines_bu / def_height;

    for( i = 0; i < lines; i++ ) {
        if( i == 0 ) {
            cur_blank = alloc_text_line();
            blank_el->element.text.first = cur_blank;
        } else {
            cur_blank->next = alloc_text_line();
            cur_blank = cur_blank->next;
        }
        cur_blank->line_height = def_height;
        if( cur_op == bx_eop ) {            // eop uses prev_line
            cur_hline = prev_line;
        } else {
            cur_hline = box_line->first;
        }
        while( cur_hline != NULL ) {
            for( i_b = 0; i_b < cur_hline->current; i_b++ ) {
                if( (cur_hline->cols[i_b].v_ind == bx_v_both)
                        || (cur_hline->cols[i_b].v_ind == bx_v_up) ) {  // ascender needed
                    if( cur_blank->first == NULL ) {
                        cur_chars = alloc_text_chars( &bin_device->box.vertical_line, 1,
                                                  g_curr_font );
                        cur_blank->first = cur_chars;
                    } else {
                        cur_chars->next = alloc_text_chars(
                                &bin_device->box.vertical_line, 1, g_curr_font );
                        cur_chars->next->prev = cur_chars;
                        cur_chars = cur_chars->next;
                    }
                    cur_chars->x_address = cur_hline->cols[i_b].col + g_page_left -
                                                                       box_col_width;
                    cur_chars->width = cop_text_width( cur_chars->text, cur_chars->count, g_curr_font );
                }
                cur_blank->last = cur_chars;
            }
            cur_hline = cur_hline->next;
        }
    }
    insert_col_main( blank_el );

    return;
}

/***************************************************************************/
/*  process a single doc_element                                           */
/*  Note: the doc_element should have "next" set to NULL as it is consumed */
/***************************************************************************/

static void box_char_element( doc_element * cur_el ) {

    bool            h_done;
    box_col_set *   cur_hline;
    int             i_b;                // box_line index
    text_chars  *   cur_chars   = NULL; // current text_chars in cur_text
    text_chars  *   new_chars;          // text_chars to be inserted into cur_text
    text_line   *   cur_text;           // current text_line
    uint32_t        cur_pos;            // current box column position (hbus)
    uint32_t        last_pos;           // last text_char text end position (hbus)

    switch( cur_el->type ) {
    case el_text:

        /* insert vertical ascenders into the text lines */

        cur_text = cur_el->element.text.first;
        if( cur_text != NULL ) {
            while( cur_text != NULL ) {
                cur_chars = cur_text->first;
                if( cur_op == bx_eop ) {            // eop uses prev_line
                    cur_hline = prev_line;
                } else {
                    cur_hline = box_line->first;
                }
                while( cur_hline != NULL ) {  // iterate over all horizontal lines
                    for( i_b = 0; i_b < cur_hline->current; i_b++ ) {
                        if( (cur_hline->cols[i_b].v_ind == bx_v_both)
                                || (cur_hline->cols[i_b].v_ind == bx_v_up) ) {  // ascender needed

                            cur_pos = cur_hline->cols[i_b].col + g_page_left
                                                                   - box_col_width;
                            if( cur_chars != NULL ) {   // insert ascender if cur_chars exists
                                h_done = false;
                                if( cur_chars == cur_text->first ) {    // first text_chars
                                    last_pos = g_page_left;
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
                                                    &bin_device->box.vertical_line,
                                                    1, g_curr_font );
                                        new_chars->x_address = cur_pos;
                                        new_chars->width = cop_text_width( new_chars->text, new_chars->count, g_curr_font );
                                        if( cur_chars->prev == NULL ) { // first text_chars in cur_text
                                            cur_text->first = new_chars;
                                        } else {
                                            new_chars->prev = cur_chars->prev;
                                            cur_chars->prev->next = new_chars;
                                        }
                                        new_chars->next = cur_chars;
                                        cur_chars->prev = new_chars;
                                        h_done = true;
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
                                if( cur_text->first == NULL ) { // empty line
                                    new_chars = alloc_text_chars(
                                                &bin_device->box.vertical_line,
                                                      1, g_curr_font );
                                    new_chars->prev = new_chars;
                                    new_chars->x_address = cur_pos;
                                    new_chars->width = cop_text_width( new_chars->text, new_chars->count, g_curr_font );
                                    cur_text->first = new_chars;
                                    cur_text->last = new_chars;
                                } else {
                                    new_chars = cur_text->last;
                                    new_chars->next = alloc_text_chars(
                                                    &bin_device->box.vertical_line,
                                                    1, g_curr_font );
                                    new_chars->next->prev = new_chars;
                                    new_chars->next->x_address = cur_pos;
                                    new_chars->width = cop_text_width( new_chars->text, new_chars->count, g_curr_font );
                                    cur_text->last = new_chars->next;
                                }
                            }
                        }
                    }
                    cur_hline = cur_hline->next;
                }
                cur_text = cur_text->next;
            }
        }
        /* insert the element into the page */

        insert_col_main( cur_el );
        break;

    case el_graph:

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

static void box_draw_vlines( box_col_set * hline, spacing_bu subs_skip,
                              spacing_bu top_skip, stub_type stub )
{
    bool            first_done;
    bx_v_ind        cur_col_type;
    doc_element *   v_line_el;
    int             i_h;                    // hline index
    spacing_bu      cur_depth;              // local box_depth (keeps box_depth unchanged)

    cur_depth = box_depth;
    first_done = false;
    if( (stub != st_ext) && (cur_depth > 0) ) { // preserve for 0 value & for st_ext stubs
        cur_depth += 10;                    // apparent constant used by wgml 4.0
    }
    for( i_h = 0; i_h < hline->current; i_h++ ) { // iterate over all output columns
        cur_col_type = hline->cols[i_h].v_ind;
        if( (cur_col_type == bx_v_up) || (stub == st_ext)
             || ((stub == st_none) && (cur_col_type == bx_v_both))
             || ((stub == st_down) && (cur_col_type == bx_v_down))
             || ((cur_col_type == bx_v_new) && (cur_op == bx_new))
             || ((cur_col_type == bx_v_new) && (cur_op == bx_set))
             || ((cur_col_type == bx_v_out) && (cur_op == bx_eop))
             || ((cur_col_type == bx_v_down) && (cur_op == bx_eop))
             || ((cur_col_type == bx_v_new) && (cur_op == bx_eop))
             || ((cur_col_type == bx_v_out) && (cur_op == bx_eop))
             ) {  // ascender needed

            ProcFlags.vline_done = true;

            /* Create the doc_element to hold the VLINE */

            v_line_el = alloc_doc_el( el_vline );
            v_line_el->blank_lines = 0; // no positional adjustments
            v_line_el->depth = 0;   // only the last VLINE can (sometimes) have a depth > 0
            v_line_el->element.vline.h_start = hline->cols[i_h].col
                                               + g_page_left - h_vl_offset;
            if( (((stub == st_down) || (stub == st_ext))
                    && (hline->cols[i_h].v_ind == bx_v_down))
                    || ((stub == st_up) && (cur_col_type == bx_v_up)) ) {    // stubs use hl_depth
                v_line_el->element.vline.v_len = hl_depth;
            } else {
                v_line_el->element.vline.v_len = cur_depth;
            }

            /* Add the column-specific depth */

            v_line_el->element.vline.v_len += hline->cols[i_h].depth;
            hline->cols[i_h].depth = 0;

            if( !first_done ) {                                 // first VLINE
                v_line_el->subs_skip = subs_skip;
                v_line_el->top_skip = top_skip;
                if( (stub == st_ext) || ((cur_depth == 0)
                     || (stub == st_down)) && (hline->current > 1)
                     || (cur_col_type == bx_v_new) && (cur_op != bx_set)
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
            insert_col_main( v_line_el );   // insert the VLINE
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
    int             i_h;
    stub_type       off_stub        = st_down;
    spacing_bu      sav_blank_lines;
    spacing_bu      sav_subs_skip;
    spacing_bu      sav_top_skip;

    if( cur_op == bx_can ) {            // no HLINES exist
        cur_hline = box_line->first;
        while( cur_hline != NULL ) {    // iterate over all horizontal lines
            box_draw_vlines( cur_hline, el_skip, el_skip, st_none );
            if( ProcFlags.vline_done) {
                el_skip = 0;            // skip used
            }
            cur_hline = cur_hline->next;
        }
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
        cur_hline = box_line->first;        // record additional VLINE height
        while( cur_hline != NULL ) {        
            for( i_h = 0; i_h < cur_hline->current; i_h++ ) {
                if( cur_hline->cols[i_h].v_ind == bx_v_out ) {           // use added height
                    cur_hline->cols[i_h].depth += box_depth;
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
                    box_depth = 0;
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
                cur_hline = box_line->first;        // record additional VLINE height
                while( cur_hline != NULL ) {        
                    for( i_h = 0; i_h < cur_hline->current; i_h++ ) {
                        if( cur_hline->cols[i_h].v_ind == bx_v_out ) {           // use added height
                            cur_hline->cols[i_h].depth += box_depth;
                        }
                    }
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
                cur_hline = box_line->first;        // record additional VLINE height
                while( cur_hline != NULL ) {        
                    for( i_h = 0; i_h < cur_hline->current; i_h++ ) {
                        if( cur_hline->cols[i_h].v_ind == bx_v_out ) {           // use added height
                            cur_hline->cols[i_h].depth += box_depth;
                        }
                    }
                    cur_hline = cur_hline->next;
                }
                box_depth = 0;                      // reset for new box
                break;
            default:
                internal_err( __FILE__, __LINE__ );
            }
        }
        el_skip = 0;            // el_skip used to position HLINEs/VLINEs
    }

    return;
}

/***************************************************************************/
/*  process a single doc_element for line devices                          */
/*  the doc_element t_doc_el_group.first is the doc_element processed      */
/***************************************************************************/

static void  box_line_element( void )
{
    doc_element *   cur_el          = NULL;

    cur_el = t_doc_el_group.first;

    if( ProcFlags.page_started ) {      // text line not at top of page
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
        ProcFlags.page_started = true;
    }

    switch( cur_el->type ) {
    case el_text:
        t_doc_el_group.first = cur_el->next;
        cur_el->next = NULL;
        insert_col_main( cur_el );   
        cur_el = t_doc_el_group.first;  // resume processing
        break;
    default:
        t_doc_el_group.first = cur_el->next;
        cur_el->next = NULL;
        insert_col_main( cur_el );   
        cur_el = t_doc_el_group.first;  // resume processing
        break;
    }

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
    int             i;                  // overall index
    int             i_b;                // box_line index
    size_t          len;
    text_chars  *   cur_chars   = NULL; // current text_chars in cur_text
    uint32_t        cur_col;            // current column (not hbus)
    spacing_bu      skippage;           // lines to skip (not hbus)

    /* process any accumulated doc_elements */

    while( t_doc_el_group.first != NULL ) {
        if( t_doc_el_group.depth <= g_max_depth ) { // doc_elements will all fit
            cur_el = t_doc_el_group.first;
            while( cur_el != NULL ) {
                if( ProcFlags.page_started ) {
                    skippage = cur_el->blank_lines + cur_el->subs_skip;
                    cur_el->subs_skip = 0;
                } else {
                    skippage = cur_el->blank_lines + cur_el->top_skip;
                    cur_el->top_skip = 0;
                    ProcFlags.page_started = true;
                }
                cur_el->blank_lines = 0;
                if( skippage > 0 ) {
                    box_blank_lines( skippage );
                }
                g_max_depth -= skippage + cur_el->depth;
                t_doc_el_group.first = cur_el->next;
                cur_el->next = NULL;
                box_char_element( cur_el );
                cur_el = t_doc_el_group.first;
            }
        } else {                                    // finish off current page
            do_page_out();
            reset_t_page();
        }
    }

    if( (box_line->next != NULL) && (cur_op == bx_off) && !box_line->had_cols ) {    
        box_blank_lines( 1 );
    } else {

        /* Create the doc_element to hold the box lines */

        box_el = alloc_doc_el( el_text );
        box_el->blank_lines = g_blank_lines;
        g_blank_lines = 0;
        box_el->subs_skip = g_subs_skip;
        box_el->top_skip = g_top_skip;

        if( (cur_op == bx_can) ) {
            box_el->element.text.overprint = true;  // force overprint
            box_el->element.text.force_op = true;   // even at top of page
        } else {
            box_el->element.text.overprint = ProcFlags.overprint;
            ProcFlags.overprint = false;
        }
        box_el->element.text.spacing = g_spacing;

        /* Create the text_line */

        box_el->element.text.first = alloc_text_line();
        box_el->element.text.first->line_height = def_height;
        box_el->depth = box_el->element.text.first->line_height + g_spacing;

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
                cur_chars->x_address = cur_hline->cols[0].col + g_page_left -
                                                                    box_col_width;

                /* Create the horizontal box line from the BOX characters. */
/// need to retest with BX OFF NN
/// was changed to accomodate BX OFF after BX NEW
                p = cur_chars->text;
                cur_col = cur_hline->cols[0].col;
                i_b = 0;
                for( i = 0; i < len; i++ ) {                    // iterate over all output columns
                    if( cur_col < cur_hline->cols[i_b].col ) {  // not a box column
                        *p = bin_device->box.horizontal_line;
                    } else {                                    // box column found
                        cur_v_ind = cur_hline->cols[i_b].v_ind;
                        if( cur_hline->current == 1 ) {         // vertical only
                            *p = bin_device->box.vertical_line;
                        } else if ( cur_v_ind == bx_v_hid ) {   // hidden in all boxes
                            *p = bin_device->box.horizontal_line;
                        } else if ( cur_v_ind == bx_v_out ) {   // visible but outside of current box
                            *p = bin_device->box.vertical_line;
                        } else if( i_b == 0 ) {                 // first box column
                            if( cur_v_ind == bx_v_new ) {       // hidden in current box only
                                if( cur_op == bx_new ) {        // BX NEW: up
                                    *p = bin_device->box.bottom_left;
                                } else if( cur_op == bx_off ) { // BX OFF: down
                                    *p = bin_device->box.top_left;
                                } else {                        // anything else
                                    *p = bin_device->box.horizontal_line;
                                }
                            } else if( cur_v_ind == bx_v_both ) {      // both up and down
                                *p = bin_device->box.left_join;
                            } else if( cur_v_ind == bx_v_down ) {   // down only
                                *p = bin_device->box.top_left;
                            } else {                            // up only
                                *p = bin_device->box.bottom_left;
                            }
                        } else if( i_b == cur_hline->current - 1 ) {    // last box column
                            if( cur_v_ind == bx_v_new ) {       // hidden in current box only
                                if( cur_op == bx_new ) {        // BX NEW: up
                                    *p = bin_device->box.bottom_right;
                                } else if( cur_op == bx_off ) { // BX OFF: down
                                    *p = bin_device->box.top_right;
                                } else {                        // anything else
                                    *p = bin_device->box.horizontal_line;
                                }
                            } else if( cur_v_ind == bx_v_both ) {              // both up and down
                                *p = bin_device->box.right_join;
                            } else if( cur_v_ind == bx_v_down ) {   // down only
                                *p = bin_device->box.top_right;
                            } else {                                    // up only
                                *p = bin_device->box.bottom_right;
                            }
                        } else {                            // all other box columns
                            if( cur_v_ind == bx_v_new ) {       // hidden in current box only
                                if( cur_op == bx_new ) {        // BX NEW: up
                                    *p = bin_device->box.bottom_join;
                                } else if( cur_op == bx_off ) { // BX OFF: down
                                    *p = bin_device->box.top_join;
                                } else {                        // anything else
                                    *p = bin_device->box.horizontal_line;
                                }
                            } else if( cur_v_ind == bx_v_both ) {   // both up and down
                                *p = bin_device->box.inside_join;
                            } else if( cur_v_ind == bx_v_down ) {   // down only
                                *p = bin_device->box.top_join;
                            } else {                        // up only
                                *p = bin_device->box.bottom_join;
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
                skippage = g_max_depth;
            }
            if( skippage > 0 ) {
                box_blank_lines( skippage );
                box_el->blank_lines = 0;
                box_el->subs_skip = 0;
                box_el->top_skip = 0;
            }
        }                           // skips before first BX line are treated normally

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
    box_col_set *   cur_hline;
    doc_element *   cur_el          = NULL;
    doc_element *   h_line_el;
    uint32_t        h_offset;
    spacing_bu      prev_height;

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

    /********************************************************/
    /* this code does what wgml 4.0 does in a limited       */
    /* context                                              */
    /* when the text inside a box uses a font other than    */
    /* font "0", even if the current font is now font "0",  */
    /* the v_offset and def_height are adjusted as shown,   */
    /* at least when the text font's line height is less    */
    /* than the font "0" line height                        */
    /********************************************************/

    prev_height = wgml_fonts[g_prev_font].line_height;
    if( prev_height < def_height ) {
        v_offset += (def_height - prev_height) / 2;
        hl_depth -= (def_height - prev_height) / 2;
    }

    /* process any accumulated doc_elements */

    while( t_doc_el_group.first != NULL ) {
        if( t_doc_el_group.depth <= g_max_depth ) { // doc_elements will all fit    
            while( t_doc_el_group.first != NULL ) {
                box_line_element();
           }
        } else {                        // finish off current page
            do_page_out();
            reset_t_page();
        }
    }

    /* Now deal with the HLINEs and associated VLINEs */

    ProcFlags.top_line = !ProcFlags.page_started && (t_doc_el_group.first == NULL);

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
                box_depth += g_subs_skip + g_blank_lines;
                el_skip += g_subs_skip + g_blank_lines;
                g_blank_lines = 0;
            }
        } else {

            /* Update box_skip directly since there are no HLINEs */

            if( ProcFlags.top_line ) {
                box_skip += g_top_skip;
            } else {
                box_skip += g_subs_skip + g_blank_lines;
                g_blank_lines = 0;
            }
        }

        if( (box_line->next != NULL) && (cur_op == bx_off) ) {    // closing last nested box
            el_skip += v_offset;
        }

    } else {

        /* Create the doc_elements to hold the HLINEs */

        cur_hline = box_line->first;
        while( cur_hline != NULL ) {  // iterate over all horizontal lines
            if( cur_el == NULL ) {
                cur_el = alloc_doc_el( el_hline );
                if( ProcFlags.in_bx_box ) {
                    cur_el->subs_skip = g_subs_skip + el_skip + v_offset;
                } else {
                    cur_el->subs_skip = g_subs_skip + box_skip + v_offset;
                    box_skip = 0;
                }
                cur_el->top_skip = g_top_skip;
                cur_el->blank_lines = g_blank_lines;
                g_blank_lines = 0;
                h_line_el = cur_el;
            } else {
                cur_el->next = alloc_doc_el( el_hline );
                cur_el = cur_el->next;
                cur_el->subs_skip = 0;
                cur_el->top_skip = 0;
                cur_el->blank_lines = 0;
            }
            cur_el->depth = 0;

            h_offset = cur_hline->cols[0].col + g_page_left - h_vl_offset;
            if( (int32_t) h_offset < 0 ) {
                h_offset = h_vl_offset;
            }
            if( (g_page_left + h_vl_offset) < h_offset ) {
                cur_el->element.hline.h_start = h_offset;
            } else {
                cur_el->element.hline.h_start = g_page_left + h_vl_offset;
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
    /*       the BX had a column list (cur_line has at      */
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

 /// this is probably not entirely accurate
    /****************************************************************/
    /* Perform the appropriate action:                              */
    /*   1. if there is not enough room on the page, move to the    */
    /*      next page (draws VLINEs and resets el_skip/box_depth    */
    /*   2. if there is an HLINE to draw, invoke draw_box_lines()   */
    /*      (ProcFlags.draw_v_line determines if VLINEs are to be   */
    /*      drawn, or just HLINEs)                                  */
    /*   3. if BX CAN (or BX DEL) is closing the outermost box,     */
    /*      invoke draw_box_lines() (VLINEs will be drawn)          */
    /*   4. if BX OFF or BX CAN (or BX DEL) is closing the last     */
    /*      inner box and there is no HLINE to be drawn, adjust     */
    /*      box_depth and el_skip appropriately                     */
    /* Note: the details vary a bit depending on whether or not     */
    /*   this BX line opens the outermost box: such a line must     */
    /*   have a column list and cannot be CAN (or DEL)              */
    /****************************************************************/

    g_max_depth = t_page.max_depth - t_page.cur_depth;          // reset value
    if( !ProcFlags.in_bx_box ) {                                // outermost box starts
        if( cur_op == bx_set ) {
            if( g_max_depth < (box_skip + (v_offset - 1)) ) {   // to top of next page
                do_page_out();
                reset_t_page();
                ProcFlags.top_line = !ProcFlags.page_started;   // reset for new page
            }
        } else {
            if( g_max_depth < (h_line_el->subs_skip + (v_offset - 1) + def_height) ) {        // to top of next page
                do_page_out();
                reset_t_page();
                ProcFlags.top_line = !ProcFlags.page_started;   // reset for new page
            }
        }
        box_depth = 0;                              // top line of box
        draw_box_lines( h_line_el );
        if( (cur_op == bx_set) && !ProcFlags.page_started ) { // first box line at top of page
            box_depth = def_height;
            ProcFlags.in_bx_box = true;             // box has started
        } else if( cur_op != bx_off ) {
            box_depth = hl_depth;
            ProcFlags.in_bx_box = true;             // box has started
        }
    } else {                                        // inside outermost box
/// needs to be verified again; at least box_skip shouldn't matter!
        if( g_max_depth < (el_skip + hl_depth + def_height) ) {    // HLINE to top of page
            do_page_out();
            reset_t_page();
            ProcFlags.top_line = !ProcFlags.page_started;     // reset for new page
        }
        if( (h_line_el != NULL) && ProcFlags.in_bx_box ) {     // adjust for HLINE skips
            if( ProcFlags.top_line ) {
                box_depth += h_line_el->top_skip;
            } else {
                box_depth += h_line_el->subs_skip + h_line_el->blank_lines;
            }
        }
        if( (h_line_el != NULL)
                || ((box_line->next == NULL) && (cur_op == bx_can))
                || (cur_op == bx_set)
                ) {
            draw_box_lines( h_line_el );
        } else if( (box_line->next != NULL) && (cur_op == bx_off) ) {
            el_skip += hl_depth;                        // invisible hline depth
        }

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
    /* t_page.cur_depth, with these exceptions:                 */
    /*      1. BX CAN was processed                             */
    /*      2. No HLINEs or VLINEs were done                    */
    /* Note: because of how doc_element.depth is handled,       */
    /*   hl_depth does not have to be added to el_skip          */
    /************************************************************/

    if( (t_page.last_col_main != NULL) && (cur_op != bx_can)
            && !ProcFlags.no_bx_hline ) {      // HLINEs, VLINEs, or both drawn, presumably
        t_page.last_col_main->depth = hl_depth;
        t_page.cur_depth += hl_depth;
    }

    /************************************************************/
    /* This matches the behavior of wgml 4.0:                   */
    /*   a new page is emitted if BX OFF or BX ON with no       */
    /*      column list is at the exact bottom of the page      */
    /*   BX CAN never positions its VLINEs at the exact bottom  */
    /*     of the page, so it does not do this -- so far        */
    /************************************************************/

    if( ((cur_op == bx_off) || ((cur_op == bx_on) && !ProcFlags.box_cols_cur)) &&
            (t_page.max_depth == t_page.cur_depth) ) {
        do_page_out();
        reset_t_page();
    }

    return;
}


/***************************************************************************/
/*  end-of-page processing for character devices                           */
/***************************************************************************/

static void eop_char_device( void ) {
    bool            splittable;
    doc_element *   cur_el;
    spacing_bu      cur_skip;               // top_skip or subs_skip, as appropriate
    spacing_bu      skippage;               // lines to skip (not hbus)

    /* process any accumulated doc_elements */

    cur_el = t_doc_el_group.first;
    while( cur_el != NULL ) {
        if( ProcFlags.page_started ) {
            cur_skip = cur_el->subs_skip;
            cur_el->subs_skip = 0;
        } else {
            cur_skip = cur_el->top_skip;
            cur_el->top_skip = 0;
            ProcFlags.page_started = true;
        }
        skippage = cur_el->blank_lines + cur_skip;
        if( (t_page.cur_depth + skippage + cur_el->depth) <= t_page.max_depth ) {

            /* the entire element will fit */

            cur_el->blank_lines = 0;
            if( skippage > 0 ) {            // convert skipped lines to output lines
                box_blank_lines( skippage );
            }
            g_max_depth -= skippage + cur_el->depth;
            t_doc_el_group.depth -= skippage + cur_el->depth;
            t_doc_el_group.first = cur_el->next;
            cur_el->next = NULL;
            box_char_element( cur_el );
            cur_el = t_doc_el_group.first;
        } else {                // the entire element will not fit
            if( (t_page.cur_depth + skippage) > t_page.max_depth ) {    // skippage too large
                if( cur_el->blank_lines > g_max_depth ) { // split blank_lines
                    cur_el->blank_lines -= g_max_depth;
                    skippage = g_max_depth;
                } else {
                    cur_el->blank_lines = 0;
                }
                if( cur_skip > g_max_depth ) {    // cap top_skip or subs_skip
                    skippage = g_max_depth;
                }
                box_blank_lines( skippage );
            } else if( (t_page.cur_depth + skippage + cur_el->depth) > t_page.max_depth ) {    // element too large
                splittable = split_element( cur_el, t_page.max_depth -
                                                    t_page.cur_depth - skippage );
                if( splittable ) {
                    if( cur_el->next != NULL ) {    // cur_el was split
                        t_doc_el_group.depth -= cur_el->depth;
                        t_doc_el_group.first = cur_el->next;
                        cur_el->next = NULL;
                        box_char_element( cur_el );
                    }
                }
            }
            cur_el = NULL;                      // nothing more goes on current page
        }
    }

    /* Now finish off the page */

    if( t_page.max_depth > t_page.cur_depth ) {
        box_blank_lines( t_page.max_depth - t_page.cur_depth );
    }

    return;
}


/***************************************************************************/
/*  end-of-page processing for line-drawing devices                        */
/***************************************************************************/

static void eop_line_device( void ) {
    bool            splittable;
    box_col_set *   cur_hline;
    spacing_bu      cur_skip;               // top_skip or subs_skip, as appropriate
    spacing_bu      skippage       = 0;     // current element skip

    /* process any accumulated doc_elements */

    while( t_doc_el_group.first != NULL ) {
        if( ProcFlags.page_started ) {      // text line not at top of page
            cur_skip = t_doc_el_group.first->subs_skip;
        } else {
            cur_skip = t_doc_el_group.first->top_skip;
            ProcFlags.page_started = true;
        }
        g_max_depth = t_page.max_depth - t_page.cur_depth;    // reset value
        skippage = t_doc_el_group.first->blank_lines + cur_skip;
        if( (t_doc_el_group.first->depth + skippage) <= g_max_depth ) { // t_doc_el_group.first will fit on the page
            box_line_element();
        } else {        // the entire element will not fit
            if( skippage > g_max_depth ) {    // skippage too large
                if( t_doc_el_group.first->blank_lines > g_max_depth ) { // split blank_lines
                    t_doc_el_group.first->blank_lines -= g_max_depth;
                    skippage = g_max_depth;
                } else {
                    t_doc_el_group.first->blank_lines = 0;
                }
                if( cur_skip > g_max_depth ) {    // cap top_skip or subs_skip
                    skippage = g_max_depth;
                }
            } else if( (t_doc_el_group.first->depth + skippage) > g_max_depth ) {  // t_doc_el_group.first will fill the page
                splittable = split_element( t_doc_el_group.first, t_page.max_depth -
                                                t_page.cur_depth - skippage );
                if( splittable ) {
                    box_depth += skippage + t_doc_el_group.first->depth;
                    if( t_doc_el_group.first->next != NULL ) {    // t_doc_el_group.first was split
                        t_doc_el_group.depth -= skippage + t_doc_el_group.first->depth;
                        box_line_element();
                    }
                }
            }
            break;  // end processing & finish page
        }
    }

    /* Now finish off the page */

    g_max_depth = t_page.max_depth - t_page.cur_depth;    // reset value

    /* This is a guess: the box_depth used must be > 0 */

    if( (box_depth + g_max_depth) > hl_depth ) {
        box_depth += g_max_depth - hl_depth;              // finalize current page box_depth
        cur_hline = prev_line;
        while( cur_hline != NULL ) {                    // iterate over all horizontal lines
            if( cur_hline == box_line->first ) {
                box_draw_vlines( cur_hline, g_max_depth - hl_depth, 0, st_none );
            } else {
                box_draw_vlines( cur_hline, 0, 0, st_none );
            }
            cur_hline = cur_hline->next;
        }
    }

    box_depth = 0;
    el_skip = 0;

    return;
}


/***************************************************************************/
/*  merge prev_line and/or cur_line into box_line->first                   */
/***************************************************************************/

static void merge_lines( void )
{
    bool            cur_break   = false;
    bool            inner_box   = false;
    bool            new_break   = false;
    bool            prev_break  = false;
    box_col_set *   box_temp;
    box_col_set *   cur_temp;
    box_col_set *   prev_temp;
    int             box_col;
    int             cur_col;
    int             prev_col;

/// temp
    out_msg( "Existing\n" );
    if( prev_line == NULL ) {
        out_msg( "No prev_line\n" );
    } else {
        out_msg( "prev_line length: %i\n", prev_line->length );
        prev_temp = prev_line;
        while( prev_temp != NULL ) {
            out_msg( "prev_line hline:\n" );
            for( prev_col = 0; prev_col < prev_temp->current; prev_col++ ) {
                out_msg( "column: %i depth: %i v_ind: %i\n",
                        prev_temp->cols[prev_col].col, prev_temp->cols[prev_col].depth,
                                           prev_temp->cols[prev_col].v_ind );
            }
            prev_temp = prev_temp->next;
        }
    }
/// end temp section

    if( ((cur_op == bx_new) || (cur_op == bx_set))
            && (cur_line != NULL) && (prev_line != NULL) ) {

        /* Mark prior boxes' columns */

        cur_temp = cur_line;
        prev_temp = prev_line;
        while( prev_temp != NULL ) {
            for( prev_col = 0; prev_col < prev_temp->current; prev_col++ ) {
                if( prev_temp->cols[prev_col].v_ind == bx_v_new ) {
                    prev_temp->cols[prev_col].v_ind = bx_v_hid;
                } else if( prev_temp->cols[prev_col].v_ind != bx_v_hid ) {
                    if( cur_temp != NULL ) {
                        if( cur_temp->cols[0].col <
                                        prev_temp->cols[prev_col].col ) {
                            if( cur_temp->cols[cur_temp->current - 1].col <
                                        prev_temp->cols[prev_col].col ) {
                                cur_temp = cur_temp->next;
                                prev_col --; // check same prev_col with new cur_temp
                            } else {
                                prev_temp->cols[prev_col].v_ind = bx_v_new;
                            }
                        } else {    // columns to left of cur_box segment
                            prev_temp->cols[prev_col].v_ind = bx_v_out;
                            inner_box = true;
                        }
                    } else {        // columns to right of final cur_box segment
                        prev_temp->cols[prev_col].v_ind = bx_v_out;
                        inner_box = true;
                    }
                }
            }
            prev_temp = prev_temp->next;
        }
        box_line->inner_box = inner_box;
    }

/// temp
    if( (cur_op == bx_new) || (cur_op == bx_set) ) {
        out_msg( "Existing after NEW/SET\n" );
        if( prev_line == NULL ) {
            out_msg( "No prev_line\n" );
        } else {
            out_msg( "prev_line length: %i\n", prev_line->length );
            prev_temp = prev_line;
            while( prev_temp != NULL ) {
                out_msg( "prev_line hline:\n" );
                for( prev_col = 0; prev_col < prev_temp->current; prev_col++ ) {
                    out_msg( "column: %i depth: %i v_ind: %i\n",
                            prev_temp->cols[prev_col].col, prev_temp->cols[prev_col].depth,
                                           prev_temp->cols[prev_col].v_ind );
                }
                prev_temp = prev_temp->next;
            }
        }
    }

    out_msg( "Current\n" );
    if( cur_line == NULL ) {
        out_msg( "No cur_line\n" );
    } else {
        out_msg( "cur_line length: %i\n", cur_line->length );
        cur_temp = cur_line;
        while( cur_temp != NULL ) {
            out_msg( "cur_line hline:\n" );
            for( cur_col = 0; cur_col < cur_temp->current; cur_col++ ) {
                out_msg( "column: %i depth: %i v_ind: %i\n",
                        cur_temp->cols[cur_col].col, cur_temp->cols[cur_col].depth,
                                           cur_temp->cols[cur_col].v_ind );
            }
            cur_temp = cur_temp->next;
        }
    }
/// end temp section

    if( (prev_line == NULL) && (cur_line == NULL) ) {
        /* This might be an error if only possible for degenerate cases */
        internal_err( __FILE__, __LINE__ );
    } else if( prev_line == NULL ) {    // cur_line becomes box_line->first
        box_line->first = cur_line;
    } else if( cur_line == NULL ) {     // prev_line becomes box_line->first
        if( cur_op == bx_none ) {
            prev_temp = prev_line;
            while( prev_temp != NULL ) {
                for( prev_col = 0; prev_col < prev_temp->current; prev_col++ ) {
                    if( (prev_temp->cols[prev_col].v_ind != bx_v_hid) &&
                            (prev_temp->cols[prev_col].v_ind != bx_v_new) &&
                            (prev_temp->cols[prev_col].v_ind != bx_v_out)) {
                        prev_temp->cols[prev_col].v_ind = bx_v_both;
                    }
                }
                prev_temp = prev_temp->next;
            }
        } else if( cur_op == bx_off ) {
            prev_temp = prev_line;
            while( prev_temp != NULL ) {
                for( prev_col = 0; prev_col < prev_temp->current; prev_col++ ) {
                    if( (prev_temp->cols[prev_col].v_ind != bx_v_hid)
                        && (prev_temp->cols[prev_col].v_ind != bx_v_new)
                        && (prev_temp->cols[prev_col].v_ind != bx_v_out)
                        ) {
                        prev_temp->cols[prev_col].v_ind = bx_v_up;
                    }
                }
                prev_temp = prev_temp->next;
            }
        }
        box_line->first = prev_line;
    } else {        // both cur_line & prev_line have entries
        box_col = 0;
        cur_col = 0;
        prev_col = 0;
        box_temp = alloc_box_col_set(); // initialize box_line->first
        cur_temp = cur_line;
        prev_temp = prev_line;
        box_line->first = box_temp;

        while( (cur_temp != NULL) || (prev_temp != NULL) ) {    // at least one has not ended
            switch( cur_op ) {
            case bx_none:                                   // continuous horizontal line
                if( prev_temp != NULL ) {                   // prev_line still has entries
                    if( prev_col == prev_temp->current ) {  // end of hline
                        prev_temp = prev_temp->next;
                        prev_col = 0;
                        continue;
                    }
                }
                if( cur_temp != NULL ) {                    // cur_line still has entries
                    if( cur_col == cur_temp->current ) {    // end of hline
                        cur_temp = cur_temp->next;
                        cur_col = 0;
                        continue;
                    }
                }
                break;

            case bx_new:                                    // special handling depending on device
            case bx_set:
                if( cur_temp != NULL ) {                    // cur_line still has entries
                    if( cur_col == cur_temp->current ) {    // end of hline
                        cur_temp = cur_temp->next;
                        cur_col = 0;
                        continue;
                    }
                }
                if( prev_temp != NULL ) {                           // prev_line still has entries
                    if( prev_col == prev_temp->current ) {          // end of hline
                        prev_temp = prev_temp->next;
                        prev_col = 0;
                        continue;
                    }
                    if( cur_temp != NULL ) {                    // cur_line still has entries
                        if( (prev_temp->cols[prev_col].col < cur_temp->cols[0].col) &&
                                (prev_temp->cols[prev_col].v_ind != bx_v_hid) &&
                                (prev_temp->cols[prev_col].v_ind != bx_v_new) ) { // before new box definition
                            new_break = true;
                        } else if( (cur_temp->cols[cur_temp->current - 1].col < prev_temp->cols[prev_col].col) &&
                                (prev_temp->cols[prev_col].v_ind != bx_v_hid) &&
                                (prev_temp->cols[prev_col].v_ind != bx_v_new) ) { // before new box definition)
                            prev_break = true;
                        }
                    } else {                                    // only "out" columns left
                        prev_break = true;
                    }
                }
                break;

            case bx_on:             // possible gap between prev_line & cur_line
                if( cur_temp != NULL ) {                            // cur_line still has entries
                    if( cur_col == cur_temp->current ) {            // end of hline
                        if( (prev_temp == prev_line) && (prev_col == 0) &&
                                (cur_temp->cols[cur_col - 1].col
                                < prev_temp->cols[prev_col].col) ) {// gap exists
                            prev_break = true;
                        }                        
                        cur_temp = cur_temp->next;
                        cur_col = 0;
                        continue;
                    }
                }
                if( prev_temp != NULL ) {                           // prev_line still has entries
                    if( prev_col == prev_temp->current ) {          // end of hline
                        if( (cur_temp == cur_line) && (cur_col == 0) &&
                                (prev_temp->cols[prev_col - 1].col
                                < cur_temp->cols[cur_col].col) ) {  // gap exists
                            cur_break = true;
                        }                        
                        prev_temp = prev_temp->next;
                        prev_col = 0;
                        continue;
                    }
                }
                break;

            case bx_off:                    // BX OFF is not entirely clear
                if( cur_temp != NULL ) {                            // cur_line still has entries
                    if( cur_col == cur_temp->current ) {            // end of hline
                        cur_temp = cur_temp->next;
                        cur_col = 0;
                        if( cur_temp != NULL ) {
                            cur_break = true;
                        }
                        continue;
                    }
                }
                if( prev_temp != NULL ) {                           // prev_line still has entries
                    if( prev_col == prev_temp->current ) {          // end of hline
                        prev_temp = prev_temp->next;
                        prev_col = 0;
                        if( prev_temp != NULL ) {
                            prev_break = true;
                        }
                        continue;
                    }
                }
                break;

            default:
                internal_err( __FILE__, __LINE__ );
                break;
            }
            if( box_temp->current == box_temp->length) {
                resize_box_cols( box_temp );
            }

            if( (cur_temp != NULL) && (prev_temp == NULL) ) {   // cur_line alone still has entries
                if( cur_break ) {
                    box_temp->next = alloc_box_col_set();
                    box_temp = box_temp->next;
                    box_col = 0;
                    cur_break = false;
                }
                box_temp->cols[box_col].col = cur_temp->cols[cur_col].col;
                box_temp->cols[box_col].v_ind = cur_temp->cols[cur_col].v_ind;
                box_temp->current++;
                box_col++;
                cur_col++;
            } else if( (cur_temp == NULL) && (prev_temp != NULL) ) {    // prev_line alone still has entries
                if( prev_break ) {
                    box_temp->next = alloc_box_col_set();
                    box_temp = box_temp->next;
                    box_col = 0;
                    prev_break = false;
                }
                box_temp->cols[box_col].col = prev_temp->cols[prev_col].col;
                box_temp->cols[box_col].depth += prev_temp->cols[prev_col].depth;
                if( (cur_op == bx_off)
                        && (prev_temp->cols[prev_col].v_ind == bx_v_both) ) {
                    box_temp->cols[box_col].v_ind = bx_v_up;
                } else {            
                    box_temp->cols[box_col].v_ind = prev_temp->cols[prev_col].v_ind;
                }
                box_temp->current++;
                box_col++;
                prev_col++;
            } else if( cur_temp->cols[cur_col].col < prev_temp->cols[prev_col].col ) {
                if( cur_break || (prev_break && (cur_op == bx_off)) ) {
                    box_temp->next = alloc_box_col_set();
                    box_temp = box_temp->next;
                    box_col = 0;
                    cur_break = false;
                    if( prev_break ) {
                        prev_break = false;
                    }
                }
                box_temp->cols[box_col].col = cur_temp->cols[cur_col].col;
                box_temp->cols[box_col].v_ind = cur_temp->cols[cur_col].v_ind;
                box_temp->current++;
                box_col++;
                cur_col++;
            } else if( cur_temp->cols[cur_col].col > prev_temp->cols[prev_col].col ) {
                if( new_break ) {
                    box_temp->cols[box_col].col = prev_temp->cols[prev_col].col;
                    box_temp->cols[box_col].depth += prev_temp->cols[prev_col].depth;
                    box_temp->cols[box_col].v_ind = prev_temp->cols[prev_col].v_ind;
                    box_temp->current++;
                    box_temp->next = alloc_box_col_set();
                    box_temp = box_temp->next;
                    box_col = 0;
                    new_break = false;
                } else {
                    if( prev_break ) {
                        box_temp->next = alloc_box_col_set();
                        box_temp = box_temp->next;
                        box_col = 0;
                        prev_break = false;
                    }
                    box_temp->cols[box_col].col = prev_temp->cols[prev_col].col;
                    box_temp->cols[box_col].depth += prev_temp->cols[prev_col].depth;
                    box_temp->cols[box_col].v_ind = prev_temp->cols[prev_col].v_ind;
                    box_temp->current++;
                    box_col++;
                }
                prev_col++;
            } else {                            // equal column values
                if( cur_break || prev_break ) {
                    box_temp->next = alloc_box_col_set();
                    box_temp = box_temp->next;
                    box_col = 0;
                    cur_break = false;
                    prev_break = false;
                }
                box_temp->cols[box_col].col = prev_temp->cols[prev_col].col;
                box_temp->cols[box_col].depth += prev_temp->cols[prev_col].depth;
                if( (cur_op == bx_can) || (cur_op == bx_off) ) {    // box ends: up only
                    box_temp->cols[box_col].v_ind = bx_v_up;
                } else {
                    box_temp->cols[box_col].v_ind = bx_v_both;
                }
                box_temp->current++;
                box_col++;
                cur_col++;
                prev_col++;
            }
        }
    }

    /************************************************************/
    /* Note: at this point, prev_line should still contain the  */
    /* same column list it had on entry, as this will be needed */
    /* should the end of a page be encountered.                 */
    /************************************************************/

/// temp
    out_msg( "Merged\n" );
    if( box_line->first == NULL ) {
        out_msg( "No box_line (this should never happen)\n" );
    } else {
        out_msg( "box_line inner_box: %i\n", box_line->inner_box );
        out_msg( "box_line length: %i\n", box_line->first->length );
        for( box_temp = box_line->first; box_temp != NULL; box_temp = box_temp->next ) {
            out_msg( "box_line hline:\n" );
            for( box_col = 0; box_col < box_temp->current; box_col++ ) {
                out_msg( "column: %i depth: %i v_ind: %i\n",
                        box_temp->cols[box_col].col, box_temp->cols[box_col].depth,
                                           box_temp->cols[box_col].v_ind );
            }
        }
    }
/// end temp section

    return;
}

/***************************************************************************/
/*  end-of-page processing for do_page_out()                               */
/*  box_line should be NULL at entry and exit, but be useable in between   */
/*  the method used only works because box_line is not modified            */
/***************************************************************************/

void eop_bx_box( void ) {

    bool    sav_group_elements;
    bx_op   sav_cur_op;

    sav_cur_op = cur_op;
    cur_op = bx_eop;                    // do eop processing
    sav_group_elements = ProcFlags.group_elements;
    ProcFlags.group_elements = false;   // processed doc_elements go direct to page
    ProcFlags.page_started = (t_page.last_col_main != NULL);

    g_max_depth = t_page.max_depth - t_page.cur_depth;

    /************************************************************/
    /* This should always match the equivalent code in scr_bx() */
    /************************************************************/

    if( bin_driver->hline.text == NULL ) {
        eop_char_device();
    } else {
        eop_line_device();
    }

    cur_op = sav_cur_op;                            // restore value on entry
    ProcFlags.group_elements = sav_group_elements;  // restore value on entry

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
    box_col_set     *   box_temp;
    box_col_set     *   cur_temp;
    box_col_set     *   prev_temp;
    box_col_stack   *   stack_temp;
    char            *   p;
    char            *   pa;
    int                 i;
    int32_t             cur_col;    // signed to catch negative relative values
    size_t              len;
    su                  boxcolwork;
    uint32_t            prev_col;   // previous value across horizontal splits
    
    ProcFlags.box_cols_cur = false;     // new BX line: no box column list yet
    ProcFlags.group_elements = false;   // stop accumulating doc_elements
    ProcFlags.no_bx_hline = false;      // emit horizontal line by default

    p = scan_start;
    while( *p && (*p == ' ') ) {
        p++;
    }
    pa = p;
    while( *p && (*p != ' ') ) {
        p++;
    }
    len = p - pa;

    /* Identify any non-numeric operand */

    cur_op = bx_none;
    if( (len == 2) && !memicmp( pa , "on", len ) ) {
        cur_op = bx_on;
    } else if( len == 3 ) {
        if( !memicmp( pa , "can", len ) || !memicmp( pa , "del", len ) ) {
            cur_op = bx_can;
        } else if( !memicmp( pa , "new", len ) ) {
            cur_op = bx_new;
        } else if( !memicmp( pa , "off", len ) ) {
            cur_op = bx_off;
        } else if( !memicmp( pa , "set", len ) ) {
            cur_op = bx_set;
        }
    } else if( (len == 4) && !memicmp( pa , "char", len ) ) {
        scan_restart = scan_stop;
        return;
    }

    if( cur_op == bx_none ) {               // reuse token if not recognized
        p = pa;
    }

    while( *p && (*p == ' ') ) {
        p++;
    }

    if( !ProcFlags.in_bx_box && !*p ) {     // if not in a box, box columns must be given
        scan_restart = scan_stop;
        return;
    }

    if( *p && (cur_op == bx_can) ) {        // CAN and DEL cannot have column lists
        xx_line_err( err_too_many_ops, pa );
        scan_restart = scan_stop;
        return;
    }

    /* Now for the box column list, if any */

    if( *p ) {
        ProcFlags.box_cols_cur = true;      // box column list found
        cur_temp = alloc_box_col_set();
        cur_line = cur_temp;
        first_col = true;                   // first column not yet found
        prev_col = 0;
        while( *p ) {
            if( cur_temp->current == cur_temp->length ) {
                resize_box_cols( cur_temp );
            }
            cur_temp->cols[cur_temp->current].v_ind = bx_v_down;
            if( cur_temp->current == 0 ) {
                if( *p == '/' ) {
                    xx_line_err( err_spc_not_valid, p );
                    p++;
                    while( *p && (*p == ' ') ) {
                        p++;
                    }
                    continue;
                }
            } else if( *p == '/' ) {
                cur_temp->next = alloc_box_col_set();
                cur_temp = cur_temp->next;
                p++;
                while( *p && (*p == ' ') ) {
                    p++;
                }
                continue;
            }
            pa = p;
            if( !cw_val_to_su( &p, &boxcolwork ) ) {
                cur_col = conv_hor_unit( &boxcolwork );
                if( cur_col <= 0 ) {
                    xx_line_err( err_inv_box_pos, pa );
                } else if( first_col ) {   // no prior column
                    first_col = false;
                } else {
                    if( boxcolwork.su_relative ) {
                        cur_col += prev_col;
                    }
                    if( cur_col <= prev_col ) {
                        xx_line_err( err_box_bad_order, pa );
                    }
                }
                if( cur_col <= 0 ) {        // treat as "+0" to minimize mischief
                    cur_col = prev_col;
                }
                cur_temp->cols[cur_temp->current].col = cur_col; 
                prev_col = cur_col;
            } else {
                xx_line_err( err_spc_not_valid, pa );
            }
            while( *p && (*p == ' ') ) {
                p++;
            }
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
       if( prev_line != NULL ) {    // add prev_line to stack

            /* set up first box_line->next member in box_temp */

            box_temp = alloc_box_col_set();
            box_line->first = box_temp;
            prev_temp = prev_line;
            if( box_temp->length >= prev_temp->length) {
                resize_box_cols( box_temp );
            }
            while( prev_temp != NULL) {
                for( i = 0; i < prev_temp->current; i++ ) {
                    box_temp->cols[i].col = prev_temp->cols[i].col;
                    box_temp->cols[i].v_ind = prev_temp->cols[i].v_ind;
                    box_temp->current++;
                }
                prev_temp = prev_temp->next;
                if( prev_temp != NULL ) {
  
                    /* set up next box_line->next member in box_temp */
  
                    box_temp->next = alloc_box_col_set();
                    box_temp = box_temp->next;
                    if( box_temp->length >= prev_temp->length) {
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
        internal_err( __FILE__, __LINE__ );
        break;    
    }

    /* set the ProcFlags specific to BX */

    ProcFlags.page_started = (t_page.last_col_main != NULL);
    ProcFlags.no_bx_hline = (cur_op == bx_set) ||
                            ((cur_op == bx_new) && !ProcFlags.box_cols_cur);

    /* set the basic layout values for BX */

    def_height = wgml_fonts[g_curr_font].line_height;       // normal box line height
    g_max_depth = t_page.max_depth - t_page.cur_depth;        // maximum depth available

    if( !ProcFlags.no_bx_hline || (cur_op == bx_set) ) {
        set_skip_vars( NULL, NULL, NULL, g_spacing_ln, bin_device->box.font );

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

    if( prev_line != NULL ) {
        if( prev_line != box_line->first ) {               // clear prev_line 
            add_box_col_set_to_pool( prev_line );
        }
        prev_line = NULL;
    }

    if( (cur_op == bx_off) || (cur_op == bx_can)
            || ((cur_op == bx_on) && !ProcFlags.box_cols_cur) ) {

        /****************************************************/
        /* Propagate depth increments to prior stack entry  */
        /*   Note: since inner_box is only set by BX NEW or */
        /*   BX SET, it cannot be true on the outer box,    */
        /*   and box_line->next cannot be NULL              */
        /****************************************************/

        if( box_line->inner_box ) {
            cur_temp = box_line->first;
            prev_temp = box_line->next->first;
            prev_col = 0;
            while( cur_temp != NULL ) {
                for( cur_col = 0; cur_col < cur_temp->current; cur_col++ ) {
                    if( cur_temp->cols[cur_col].v_ind == bx_v_out ) {
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
            if( stack_temp->first != cur_line ) {   // protect cur_line TBD
                add_box_col_set_to_pool( stack_temp->first );        
                stack_temp->first = NULL;
            }
        }
        add_box_col_stack_to_pool( stack_temp );

        /* If the stack is empty, the entire box has ended */

        if( box_line == NULL ) {
            ProcFlags.in_bx_box = false;
            box_depth = 0;
            el_skip = 0;
            cur_op = bx_none;
        } else {
            ProcFlags.group_elements = true;    // start accumulating doc_elements
        }
    } else {
        ProcFlags.group_elements = true;    // start accumulating doc_elements
    }

    if( box_line != NULL ) {

        /********************************************************************/
        /* box_line->first is often the boundary between two boxes, one of  */
        /*   which is being closed or stacked and the other of which is     */
        /*   being opened; in general, next_line should include the box     */
        /*   being closed only when there was no column list no operator    */
        /*   or when NEW or SET are used, to keep the columns that need     */
        /*   to have risers drawn even though they are not part of the      */
        /*   current box                                                    */
        /* prev_line is based on box_line->first under these conditions:    */
        /*   -- the BX line had no column list and                          */
        /*      -- the BX line had no operator                              */
        /*   -- the BX line had a column list and                           */
        /*      -- the BX line had operator NEW                             */
//        /*      -- the BX line had operator ON                              */
        /*      -- the BX line had operator SET                             */
        /*   -- BX OFF or BX CAN/BX DEL which closed an inner box           */
        /* prev_line becomes empty under these conditions:                  */
        /*   -- the BX line had no column list and                          */
        /*      -- the BX line had operator NEW                             */
        /*      -- the BX line had operator ON                              */
        /*      -- the BX line had operator SET                             */
        /*   -- BX OFF or BX CAN/BX DEL which closed to outermost box       */
        /* prev_line is based on cur_line under these conditions:           */
        /*   -- the BX line had a column list and                           */
        /*      -- the BX line had no operator                              */
        /*      -- the BX line had operator ON                              */
        /*   -- the BX line had no column list and                          */
        /*      -- the BX line had operator NEW                             */
        /*      -- the BX line had operator SET                             */
        /********************************************************************/

        if( ((cur_op == bx_none) && !ProcFlags.box_cols_cur)
//                || (((cur_op == bx_new) || (cur_op == bx_on) || (cur_op == bx_set))
                || (((cur_op == bx_new) || (cur_op == bx_set))
                    && ProcFlags.box_cols_cur)
                || ((cur_op == bx_off) || (cur_op == bx_can)) ) {
            prev_line = box_line->first;            
            box_line->first = NULL;
        } else if( (cur_op == bx_off) || (cur_op == bx_can)
                || (!ProcFlags.box_cols_cur
                    && ((cur_op == bx_new) || (cur_op == bx_on) || (cur_op == bx_set)))
                ) {
            prev_line = NULL;
        } else {
            prev_line = cur_line;
        }

        if( box_line->first != NULL ) {
            if( box_line->first != prev_line ) {       // clear box_line->first
                add_box_col_set_to_pool( box_line->first );
            }
            box_line->first = NULL;
        }
    }

    if( cur_line != NULL ) {
        if( cur_line != prev_line ) {           // clear cur_line 
            add_box_col_set_to_pool( cur_line );
        }
        cur_line = NULL;
    }

    if( prev_line != NULL ) {               // set up for next BX line

        /* change "down" columns to "up" */

        prev_temp = prev_line;
        while( prev_temp != NULL ) {
            for( prev_col = 0; prev_col < prev_temp->current; prev_col++ ) {
                if( prev_temp->cols[prev_col].v_ind == bx_v_down ) {
                    prev_temp->cols[prev_col].v_ind = bx_v_up;
                }
            }
            prev_temp = prev_temp->next;
        }
    }

out_msg( "apage = %i\n", apage);
out_msg( "after updating prev_line for next pass\n" );
if( mem_validate() ) {
    out_msg( ">>> heap valid\n" );
} else {
    out_msg( ">>> heap NOT valid\n" );
}

    ProcFlags.skips_valid = false;          // ensures following text will use correct skips
    set_h_start();                          // pick up any indents

    scan_restart = scan_stop;

    return;
}

