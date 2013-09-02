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
* Description: implement .bx (box)  script control word
*                    and related routines                            TBD
*
*  comments are from script-tso.txt
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */

#include "wgml.h"
#include "gvars.h"

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
    bx_none,
    bx_on,
    bx_off,
    bx_new,
    bx_set,
    bx_can,
} bx_op;

typedef enum {
    st_none,        // the normal case
    st_ext,         // stub height is given in box_height, stubs only
    st_int,         // stub height not given in box_height, stubs intermingled
} stub_type;

static  uint32_t    def_height      = 0;        // default font line height
static  uint32_t    hl_depth        = 0;        // height from VLINE drawn to lower boundary of def_height
static  uint32_t    max_depth       = 0;        // space left on page

/***************************************************************************/
/*  output blank lines with vertical box characters                        */
/***************************************************************************/

static void box_blank_lines( uint32_t lines )
{
    box_col_set *   cur_hline;
    doc_element *   blank_el;
    int             i;
    int             i_b;        // box_line index
    text_chars  *   cur_chars;
    text_line   *   cur_blank;

    if( lines == 0 ) {          // why are we here?
        return;
    }

    blank_el = alloc_doc_el( el_text );
    blank_el->depth = lines + g_spacing;
    lines /= def_height;

    for( i = 0; i < lines; i++ ) {
        if( i == 0 ) {
            cur_blank = alloc_text_line();
            blank_el->element.text.first = cur_blank;
        } else {
            cur_blank->next = alloc_text_line();
            cur_blank = cur_blank->next;
        }
        cur_blank->line_height = def_height;
        for( cur_hline = box_line; cur_hline != NULL; cur_hline = cur_hline->next ) {
            for( i_b = 0; i_b < cur_hline->current; i_b++ ) {
                if( (cur_hline->cols[i_b].v_ind == bx_v_both)
                        || (cur_hline->cols[i_b].v_ind == bx_v_up) ) {  // ascender needed
                    if( cur_blank->first == NULL ) {
                        cur_chars = alloc_text_chars( &bin_device->box.vertical_line, 1, g_curr_font );
                        cur_blank->first = cur_chars;
                    } else {
                        cur_chars->next = alloc_text_chars( &bin_device->box.vertical_line, 1, g_curr_font );
                        cur_chars->next->prev = cur_chars;
                        cur_chars = cur_chars->next;
                    }
                    cur_chars->x_address = cur_hline->cols[i_b].col + g_page_left -
                                                                       box_col_width;
                    cur_chars->width = cop_text_width( cur_chars->text, cur_chars->count, g_curr_font );
                }
                cur_blank->last = cur_chars;
            }
        }
    }
    insert_col_main( blank_el );

    return;
}

/***************************************************************************/
/*  output vline elements for a given depth and pre-skip                   */
/*  Note: only processes columns for one specific box_col_set              */
/***************************************************************************/

static void box_draw_vlines( box_col_set * hline, uint32_t box_depth,
                              uint32_t subs_skip, uint32_t top_skip,
                              bool once, stub_type stub )
{
    bool            first_done;
    doc_element *   v_line_el;
    int             i_h;                    // hline index

    first_done = false;
    if( (stub != st_ext) && (box_depth > 0) ) { // preserve for 0 value & for st_ext stubs
        box_depth += 10;                    // apparent constant used by wgml 4.0
    }
    for( i_h = 0; i_h < hline->current; i_h++ ) { // iterate over all output columns
        if( ((stub != st_ext) && (hline->cols[i_h].v_ind == bx_v_both))
             || (hline->cols[i_h].v_ind == bx_v_up)
             || ((stub == st_int) && (hline->cols[i_h].v_ind == bx_v_down))
                ) {  // ascender needed

            /* Create the doc_element to hold the VLINE */

            v_line_el = alloc_doc_el( el_vline );
            v_line_el->blank_lines = 0; // no positional adjustments
            v_line_el->depth = 0;   // only the last VLINE can (sometimes) have a depth > 0
            v_line_el->element.vline.h_start = hline->cols[i_h].col
                                               + g_page_left - h_vl_offset;
            if( (stub == st_int) && (hline->cols[i_h].v_ind == bx_v_down) ) {
                v_line_el->element.vline.v_len = hl_depth;
            } else {
                v_line_el->element.vline.v_len = box_depth;
            }
            if( !first_done ) {                                 // first VLINE
                v_line_el->subs_skip = subs_skip;
                v_line_el->top_skip = top_skip;
                if( once ) {                // HLINE was done first or box is split
                    v_line_el->element.vline.twice = false;     // most first VLINEs do AA once
                }
                first_done = true;
            } else if( (box_depth == 0) && !((stub == st_int) &&
                        (hline->cols[i_h].v_ind == bx_v_down) ) ) {
                v_line_el->element.vline.twice = false;         // stub VLINEs do AA once
            }
            insert_col_main( v_line_el );   // insert the VLINE
        }
    }

    return;
}

/***************************************************************************/
/*  create the box for character devices                                   */
/***************************************************************************/

static void  do_char_device( bx_op cur_op )
{
    bool            h_done;
    box_col_set *   cur_hline;
    bx_v_ind        cur_v_ind;
    char        *   p;
    doc_element *   box_el;             // holds the box line itself, for char devices
    doc_element *   cur_el;
    doc_element *   sav_el;             // used to submit elements safely
    int             i;                  // overall index
    int             i_b;                // box_line index
    size_t          len;
    text_chars  *   cur_chars   = NULL; // current text_chars in cur_text
    text_chars  *   new_chars;          // text_chars to be inserted into cur_text
    text_line   *   cur_text;           // current text_line
    uint32_t        cur_col;            // current column (not hbus)
    uint32_t        cur_pos;            // current box column position (hbus)
    uint32_t        last_pos;           // last text_char text end position (hbus)
    uint32_t        skippage;           // lines to skip (not hbus)

    set_skip_vars( NULL, NULL, NULL, spacing, bin_device->box.font );

    /* Create the doc_element to hold the box line */

    box_el = alloc_doc_el( el_text );
    box_el->blank_lines = g_blank_lines;
    g_blank_lines = 0;
    box_el->subs_skip = g_subs_skip;
    box_el->top_skip = g_top_skip;
    box_el->element.text.overprint = ProcFlags.overprint;
    ProcFlags.overprint = false;
    box_el->element.text.spacing = g_spacing;

    /* Create the text_line */

    box_el->element.text.first = alloc_text_line();
    box_el->element.text.first->line_height = def_height;
    box_el->depth = box_el->element.text.first->line_height + g_spacing;

    // iterate over all horizontal lines
    for( cur_hline = box_line; cur_hline != NULL; cur_hline = cur_hline->next ) {
        len = (cur_hline->cols[cur_hline->current - 1].col - cur_hline->cols[0].col);

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
        cur_chars->x_address = cur_hline->cols[0].col + g_page_left - box_col_width;

        p = cur_chars->text;
        cur_col = cur_hline->cols[0].col;
        i_b = 0;
        for( i = 0; i <= len; i++ ) {                   // iterate over all output columns
            if( cur_col < cur_hline->cols[i_b].col ) {  // not a box column
                *p = bin_device->box.horizontal_line;
            } else {                                    // box column found
                cur_v_ind = cur_hline->cols[i_b].v_ind;
                if( cur_hline->current == 1 ) {         // vertical only
                    *p = bin_device->box.vertical_line;
                } else if( i_b == 0 ) {                 // first box column
                    if( cur_v_ind == bx_v_both ) {      // both up and down
                        *p = bin_device->box.left_join;
                    } else if( (cur_v_ind == bx_v_down) && (cur_op != bx_off) ) {   // down only
                        *p = bin_device->box.top_left;
                    } else {                            // up only
                        *p = bin_device->box.bottom_left;
                    }
                } else if( i_b == cur_hline->current - 1 ) {    // last box column
                    if( cur_v_ind == bx_v_both ) {              // both up and down
                        *p = bin_device->box.right_join;
                    } else if( (cur_v_ind == bx_v_down) && (cur_op != bx_off) ) {   // down only
                        *p = bin_device->box.top_right;
                    } else {                                    // up only
                        *p = bin_device->box.bottom_right;
                    }
                } else {                            // all other box columns
                    if( cur_v_ind == bx_v_both ) {   // both up and down
                        *p = bin_device->box.inside_join;
                    } else if( (cur_v_ind == bx_v_down) && (cur_op != bx_off) ) {   // down only
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
    }

    /* process any accumulated doc_elements */

    cur_el = t_doc_el_group.first;
    t_doc_el_group.depth    = 0;
    t_doc_el_group.first    = NULL;
    t_doc_el_group.last     = NULL;

    while( cur_el != NULL ) {

        /********************************************************/
        /* There are three cases:                               */
        /* 1. The entire cur_el will fit on the current page    */
        /* 2. cur_el->element will not fit, but part or all of  */
        /*    the preceding skip will                           */
        /* 3. None of cur_el will fit on the current page       */
        /********************************************************/

        skippage = cur_el->blank_lines + cur_el->subs_skip;
        if( (t_page.cur_depth + skippage + cur_el->depth) <= t_page.max_depth ) {

            /* The first case: the entire element will fit */

            if( skippage > 0 ) {            // convert skipped lines to output lines
                box_blank_lines( skippage );
                cur_el->top_skip = 0;
                cur_el->blank_lines = 0;
                cur_el->subs_skip = 0;
            }
        } else if( (t_page.cur_depth + skippage) >= (t_page.max_depth + def_height) ) {

            /* The second case: the skip equals or exceeds the available space */

            skippage = max_depth;
            if( skippage > 0 ) {            // convert skipped lines to output lines
                box_blank_lines( skippage );
                cur_el->blank_lines = 0;
                cur_el->subs_skip = 0;
            }
        } else {

            /* The third case: none of the current element will fit */
                
            if( skippage > 0 ) {            // convert skipped lines to output lines
                box_blank_lines( skippage );
                cur_el->top_skip = 0;
                cur_el->blank_lines = 0;
                cur_el->subs_skip = 0;
            }
        }

        max_depth -= (skippage + cur_el->depth);
        switch( cur_el->type ) {
        case el_text:

            /* insert vertical ascenders into the text lines */

            cur_hline = box_line;
            for( cur_text = cur_el->element.text.first; cur_text != NULL; cur_text = cur_text->next ) {
                // iterate over all horizontal lines
                for( ; cur_hline != NULL; cur_hline = cur_hline->next ) {
                    cur_chars = cur_text->first;
                    for( i_b = 0; i_b < cur_hline->current; i_b++ ) {
                        if( (cur_hline->cols[i_b].v_ind == bx_v_both) || 
                            (cur_hline->cols[i_b].v_ind == bx_v_up) ) {  // ascender needed
                            h_done = false;
                            cur_pos = cur_hline->cols[i_b].col + 
                                                        g_page_left - box_col_width;
                            while( cur_chars != NULL ) {
                                if( cur_chars == cur_text->first ) {    // first text_chars
                                    last_pos = g_page_left - 1;
                                } else {
                                    last_pos = cur_chars->prev->x_address +
                                                            cur_chars->prev->width;
                                }
                                if( (cur_pos > last_pos) &&
                                        (cur_chars->x_address > cur_pos) ) {

                                    /* box col position is not inside any text_chars */

                                    new_chars = alloc_text_chars(
                                            &bin_device->box.vertical_line,
                                            1, g_curr_font );
                                new_chars->x_address = cur_pos;
                                new_chars->width = cop_text_width( new_chars->text,
                                                new_chars->count, g_curr_font );
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
                                cur_chars = cur_chars->next;
                            }
                            if( h_done ) {      // process next box column
                                continue;
                            }

                            if( (cur_pos > last_pos) && ((cur_text->last->x_address
                                            + cur_text->last->width) < cur_pos) ) {

                                /************************************************/ 
                                /* At this point, there are no more text_chars  */
                                /* but the ascender is still needed and has not */
                                /* been drawn.                                  */
                                /************************************************/ 

                                if( cur_text->first == NULL ) { // empty line
                                    new_chars = alloc_text_chars(
                                        &bin_device->box.vertical_line,
                                        1, g_curr_font );
                                new_chars->prev = new_chars;
                                new_chars->x_address = cur_pos;
                                new_chars->width = cop_text_width( new_chars->text,
                                                new_chars->count, g_curr_font );
                                cur_text->first = new_chars;
                                cur_text->last = new_chars;
                            } else {
                                new_chars = cur_text->last;
                                new_chars->next = alloc_text_chars(
                                                  &bin_device->box.vertical_line,
                                                  1, g_curr_font );
                                new_chars->next->prev = new_chars;
                                new_chars->next->x_address = cur_pos;
                                new_chars->width = cop_text_width( new_chars->text,
                                                new_chars->count, g_curr_font );
                                cur_text->last = new_chars->next;
                            }
                            }
                        }
                    }
                }
            }

            /* insert the element into the page */

            sav_el = cur_el;
            cur_el = cur_el->next;
            sav_el->next = NULL;
            insert_col_main( sav_el );
            break;

        case el_graph:

            /* replace GRAPHIC elements with blank lines */

            box_blank_lines( cur_el->depth );
            sav_el = cur_el;
            cur_el = cur_el->next;
            sav_el->next = NULL;
            add_doc_el_to_pool( sav_el );
            break;

        default:

            /* insert the element into the page */

            sav_el = cur_el;
            cur_el = cur_el->next;
            sav_el->next = NULL;
            insert_col_main( sav_el );
        }
    }

    if( ProcFlags.in_bx_box ) {                                       // not first BX line
        if( t_page.cur_depth == t_page.max_depth ) {
            skippage = box_el->top_skip;
        } else {
            skippage = box_el->blank_lines + box_el->subs_skip;
            if( (t_page.cur_depth + skippage) > t_page.max_depth ) {
                skippage = max_depth;
            }
        }
        if( skippage > 0 ) {
            box_blank_lines( skippage );
            box_el->top_skip = 0;
            box_el->blank_lines = 0;
            box_el->subs_skip = 0;
        }
    }                           // process first BX line as usual

    insert_col_main( box_el );  // insert the box line

    if( !ProcFlags.in_bx_box ) {    // first BX line: start of box
        ProcFlags.in_bx_box = true; // box has started
    }

    return;
}

/***************************************************************************/
/*  create the box for line_drawing devices                                */
/***************************************************************************/

static void do_line_device( bx_op cur_op )
{
    bool            draw_v_line;
    bool            eop             = false;    // current BX line will be last line on page
    bool            splittable;
    bool            t_page_empty;
    bool            top_line;
    box_col_set *   cur_hline;
    doc_element *   cur_el          = NULL;
    doc_element *   h_line_el;
    uint32_t        cur_skip       = 0;     // current element skip
    uint32_t        h_offset;
    uint32_t        v_adjust       = 0;     // adjust box_depth for top-of-page
    uint32_t        v_offset;               // space reserved for the box line which is above the line drawn

    static  uint32_t    box_depth   = 0;        // depth of box (used with VLINES)

    t_page_empty = (t_page.last_col_main == NULL) && (t_page.last_col_bot == NULL) &&
                   (t_page.last_col_fn == NULL) && (t_page.top_banner == NULL);

    set_skip_vars( NULL, NULL, NULL, spacing, g_curr_font );

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

    /* Create the doc_elements to hold the HLINEs */

    // iterate over all horizontal lines
    for( cur_hline = box_line; cur_hline != NULL; cur_hline = cur_hline->next ) {
        if( cur_el == NULL ) {
            cur_el = alloc_doc_el( el_hline );
            cur_el->subs_skip = g_subs_skip + v_offset;
            cur_el->top_skip = g_top_skip + v_offset;
            h_line_el = cur_el;
        } else {
            cur_el->next = alloc_doc_el( el_hline );
            cur_el = cur_el->next;
            cur_el->subs_skip = 0;
            cur_el->top_skip = 0;
        }
        cur_el->blank_lines = g_blank_lines;
        g_blank_lines = 0;
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
    }                           

    /* process each element in t_doc_el_group in turn */

    ProcFlags.group_elements = false;   // stop accumulating doc_elements
    cur_el = t_doc_el_group.first;
    while( cur_el != NULL ) {
        if( t_page_empty ) {            // catch text line at top of page
            box_depth = 0;              // top of page
            cur_skip = cur_el->top_skip;
            t_page_empty = false;
            v_adjust = v_offset;        // will be subtracted from box_depth
        } else {
            cur_skip = cur_el->subs_skip;
            v_adjust = 0;               // box_depth needs to be adjusted at most once
        }
        if( cur_skip >= max_depth ) {   // cur_skip will fill the page: draw VLINES
            box_depth += max_depth - hl_depth;
            // iterate over all horizontal lines
            for( cur_hline = box_line; cur_hline != NULL; cur_hline = cur_hline->next ) {
                if( cur_hline == box_line ) {
                    box_draw_vlines( cur_hline, box_depth, max_depth - hl_depth, 0,
                                                        box_depth == 0, st_none );
                } else {
                    box_draw_vlines( cur_hline, box_depth, 0, 0, box_depth == 0,
                                                                        st_none );
                }
            }
            do_page_out();
            reset_t_page();
            t_page_empty = true;        // reset several values for new page
            max_depth = t_page.max_depth - t_page.cur_depth;
            cur_el->top_skip = 0;
            cur_el->subs_skip = 0;
        } else {
            eop = false;
            if( (cur_el->depth + cur_skip) > max_depth ) {  // cur_el will fill the page
                eop = true;
                splittable = split_element( cur_el, (max_depth - cur_skip) );
                if( splittable ) {
                    box_depth += cur_skip + cur_el->depth;
                    if( cur_el->next != NULL ) {    // cur_el was split
                        t_doc_el_group.first = cur_el->next;
                        cur_el->next = NULL;
                        insert_col_main( cur_el );   
                        max_depth = t_page.max_depth - t_page.cur_depth;    // reset value
                    }
                } else {    // this element will start the next page
                    box_depth += max_depth - hl_depth;  // finalize current page box_depth
                }
            } else {        // this element will fit on the current page
                t_doc_el_group.first = cur_el->next;
                cur_el->next = NULL;
                insert_col_main( cur_el );   
                max_depth = t_page.max_depth - t_page.cur_depth;    // reset value
                box_depth += cur_skip + cur_el->depth;
                box_depth -= v_adjust;      // adjust VLINE height as if an HLINE were at the top of the page
            }

            if( eop ) {                     // draw the VLINEs at the bottom of the page
                // iterate over all horizontal lines
                for( cur_hline = box_line; cur_hline != NULL; cur_hline = cur_hline->next ) {
                    if( cur_hline == box_line ) {
                        box_draw_vlines( cur_hline, box_depth, max_depth - hl_depth,
                                                                0, false, st_none );
                    } else {
                        box_draw_vlines( cur_hline, box_depth, 0, 0, false, st_none );
                    }
                }
                do_page_out();
                reset_t_page();
                box_depth = 0;              // reset several values for new page
                t_page_empty = true;        // reset for new page
                max_depth = t_page.max_depth - t_page.cur_depth;    // reset value
            }
            cur_el = t_doc_el_group.first;  // resume processing
        }
    }

    /* Now output the HLINEs */

    /* Get the skip for the HLINEs */

    top_line = t_page_empty && (t_doc_el_group.first == NULL);
    if( top_line ) {                    // HLINEs will be at top of page
        cur_skip = h_line_el->top_skip;
    } else {
        cur_skip = h_line_el->subs_skip;
    }

    /********************************************************/
    /* determine if VLINEs will be drawn in conjunction     */
    /* with the current HLINEs                              */
    /* the criteria used here are:                          */
    /*   the VLINE block must be defined for this device    */
    /*   box_line must have at least one column             */
    /*   at least one of these conditions must be true:     */
    /*     the end of the box has been reached (BX OFF)     */
    /*     or                                               */
    /*       the BX had a column list (cur_line has at      */
    /*       least one column) and there was a prior BX     */
    /*       line                                           */
    /* these criteria may be expanded in the future         */
    /********************************************************/

    draw_v_line = (bin_driver->vline.text != NULL) && (box_line->current > 0)
                   && ((cur_op == bx_off) ||
                   (ProcFlags.box_cols_cur && ProcFlags.in_bx_box) );

    /****************************************************************/
    /* Process first and last HLINEs separately from other HLINEs   */
    /* ProcFlags.group_elements is still FALSE: unless altered, all */
    /* insertions shown are done to t_page                          */
    /****************************************************************/

    if( !ProcFlags.in_bx_box ) {    // first BX line: start of box
        ProcFlags.in_bx_box = true; // box has started
        if( (max_depth >= def_height) && (max_depth < (2 * def_height)) ) {   // at bottom of current page
            // insert the HLINEs
            for( cur_el = h_line_el; cur_el != NULL; cur_el = h_line_el ) {
                h_line_el = h_line_el->next;
                cur_el->next = NULL;
                insert_col_main( cur_el );
            }
        } else {                        // anywhere else
            if ( max_depth < def_height ) {  // force HLINE to top of next page
                do_page_out();
                reset_t_page();
            }
            // insert the HLINEs
            for( cur_el = h_line_el; cur_el != NULL; cur_el = h_line_el ) {
                h_line_el = h_line_el->next;
                if( cur_el->next == NULL ) { // only final HLINE gets the depth
                    cur_el->depth = hl_depth;
                }
                cur_el->next = NULL;
                insert_col_main( cur_el );
            }
            box_depth = hl_depth;                   // start box_depth with space from HLINE to bottom of def_height
        }
    } else if( cur_op == bx_off ) { // last BX line: end of box
        if( top_line ) {                            // final HLINE at top of page
            ProcFlags.group_elements = true;            // start accumulating doc_elements
            box_depth = 0;                          // VLINEs are not to be drawn, just the AA blocks
            // insert the HLINEs
            for( cur_el = h_line_el; cur_el != NULL; cur_el = h_line_el ) {
                h_line_el = h_line_el->next;
                cur_el->next = NULL;
                insert_col_main( cur_el );
            }
            // iterate over all horizontal lines
            for( cur_hline = box_line; cur_hline != NULL; cur_hline = cur_hline->next ) {
                box_draw_vlines( cur_hline, box_depth, 0, 0, true, st_none );
            }
            t_doc_el_group.last->depth = hl_depth;
        } else if( max_depth < (hl_depth + cur_skip) ) {    // final HLINE to top of next page
            box_depth += max_depth - hl_depth;
            // iterate over all horizontal lines
            for( cur_hline = box_line; cur_hline != NULL; cur_hline = cur_hline->next ) {
                if( cur_hline == box_line ) {
                    box_draw_vlines( cur_hline, box_depth, max_depth - hl_depth, 0,
                                                                    false, st_none );
                } else {
                    box_draw_vlines( cur_hline, box_depth, 0, 0, false, st_none );
                }
            }
            do_page_out();
            reset_t_page();
            ProcFlags.group_elements = true;        // start accumulating doc_elements
            box_depth = 0;                          // VLINEs are not to be drawn, just the AA blocks
            cur_el = h_line_el;                     // insert the HLINEs
            cur_hline = box_line;
            while( cur_el != NULL && cur_hline != NULL ) {
                h_line_el = h_line_el->next;
                cur_el->next = NULL;
                insert_col_main( cur_el );
                box_draw_vlines( cur_hline, box_depth, 0, 0, true, st_int );
                cur_el = h_line_el;
                cur_hline = cur_hline->next;
            }
            t_doc_el_group.last->depth = hl_depth;
        } else {                                    // final HLINE in middle of page
            ProcFlags.group_elements = true;            // start accumulating doc_elements
            box_depth += cur_skip;
            cur_el = h_line_el;                         // insert the HLINEs and VLINEs
            cur_hline = box_line;
            while( cur_el != NULL && cur_hline != NULL ) {
                h_line_el = h_line_el->next;
                cur_el->next = NULL;
                insert_col_main( cur_el );
                box_draw_vlines( cur_hline, box_depth, 0, 0, true, st_int );
                cur_el = h_line_el;
                cur_hline = cur_hline->next;
            }
            box_depth = 0;                          // end of box
            t_doc_el_group.last->depth = hl_depth;
        }
    } else {                        // internal BX line
        if( top_line ) {                        // HLINE at top of page
            // insert the HLINEs
            for( cur_el = h_line_el; cur_el != NULL; cur_el = h_line_el ) {
                h_line_el = h_line_el->next;
                if( cur_el->next == NULL ) { // only final HLINE gets the depth
                    cur_el->depth = hl_depth;
                }
                cur_el->next = NULL;
                insert_col_main( cur_el );
            }
            box_depth = hl_depth;
        } else if( (max_depth > (def_height + cur_skip)) &&
                    (max_depth < (2 * def_height)) ) {   // at bottom of current page
            if( draw_v_line ) {
                box_depth += cur_skip;
                // iterate over all horizontal lines
                for( cur_hline = box_line; cur_hline != NULL; cur_hline = cur_hline->next ) {
                    if( cur_hline == box_line ) {
                        box_draw_vlines( cur_hline, box_depth, h_line_el->subs_skip,
                                                                0, false, st_none );
                    } else {
                        box_draw_vlines( cur_hline, box_depth, 0, 0, false, st_none );
                    }
                }
                h_line_el->subs_skip = 0;
                // insert the HLINEs
                for( cur_el = h_line_el; cur_el != NULL; cur_el = h_line_el ) {
                    h_line_el = h_line_el->next;
                    cur_el->next = NULL;
                    insert_col_main( cur_el );
                }
                // iterate over all horizontal lines
                for( cur_hline = box_line; cur_hline != NULL; cur_hline = cur_hline->next ) {
                    box_draw_vlines( cur_hline, hl_depth, 0, 0, true, st_ext );
                }
            } else {
                // insert the HLINEs
                for( cur_el = h_line_el; cur_el != NULL; cur_el = h_line_el ) {
                    h_line_el = h_line_el->next;
                    cur_el->next = NULL;
                    insert_col_main( cur_el );
                }
                box_depth += max_depth - hl_depth;
                // iterate over all horizontal lines
                for( cur_hline = box_line; cur_hline != NULL; cur_hline = cur_hline->next ) {
                    if( cur_hline == box_line ) {
                        box_draw_vlines( cur_hline, box_depth, max_depth - def_height,
                                                                0, false, st_none );
                    } else {
                        box_draw_vlines( cur_hline, box_depth, 0, 0, false, st_none );
                    }
                }
                do_page_out();
                reset_t_page();
            }
            box_depth = 0;                      // VLINEs were drawn to cover pre_depth
        } else {                                // HLINE in middle of page
            if( max_depth < (cur_skip + hl_depth) ) {  // HLINE to top of next page; eop VLINEs start from bottom of page
                box_depth += max_depth - hl_depth;
                // iterate over all horizontal lines
                for( cur_hline = box_line; cur_hline != NULL; cur_hline = cur_hline->next ) {
                    if( cur_hline == box_line ) {
                        box_draw_vlines( cur_hline, box_depth, max_depth - hl_depth,
                                                        0, box_depth == 0, st_none );
                    } else {
                        box_draw_vlines( cur_hline, box_depth, 0, 0, box_depth == 0,
                                                                            st_none );
                    }
                }
                do_page_out();
                reset_t_page();
                ProcFlags.group_elements = true;        // start accumulating doc_elements
                h_line_el->subs_skip = 0;
                // insert the HLINEs
                for( cur_el = h_line_el; cur_el != NULL; cur_el = h_line_el ) {
                    h_line_el = h_line_el->next;
                    cur_el->next = NULL;
                    insert_col_main( cur_el );
                }
                box_depth = hl_depth;               // reset for new page
                t_doc_el_group.last->depth = hl_depth;
            } else if( (max_depth >= (hl_depth + cur_skip))
                    && (max_depth < (2 * def_height)) ) {   // HLINE at bottom of current page
                max_depth = t_page.max_depth - t_page.cur_depth;    // reset value
                box_depth += v_offset;
                if( cur_skip > v_offset ) { // more than just v_offset, eg, SK lines
                    box_depth += cur_skip - v_offset;
                }
                // iterate over all horizontal lines
                for( cur_hline = box_line; cur_hline != NULL; cur_hline = cur_hline->next ) {
                    if( cur_hline == box_line ) {
                        box_draw_vlines( cur_hline, box_depth, h_line_el->subs_skip,
                                                                0, false, st_none );
                    } else {
                        box_draw_vlines( cur_hline, box_depth, 0, 0, false, st_none );
                    }
                }
                h_line_el->subs_skip = 0;
                // insert the HLINEs
                for( cur_el = h_line_el; cur_el != NULL; cur_el = h_line_el ) {
                    h_line_el = h_line_el->next;
                    cur_el->next = NULL;
                    insert_col_main( cur_el );
                }
                // iterate over all horizontal lines
                for( cur_hline = box_line; cur_hline != NULL; cur_hline = cur_hline->next ) {
                    box_draw_vlines( cur_hline, hl_depth, 0, 0, true, st_ext ); 
                }
                max_depth = t_page.max_depth - t_page.cur_depth;    // reset value
                box_depth = 0;                  // VLINEs were drawn to cover pre_depth
            } else if( draw_v_line ) {          // VLINEs will be drawn between HLINEs
                ProcFlags.group_elements = true;            // start accumulating doc_elements
                box_depth += cur_skip;
                // iterate over all horizontal lines
                for( cur_hline = box_line; cur_hline != NULL; cur_hline = cur_hline->next ) {
                    if( cur_hline == box_line ) {
                        box_draw_vlines( cur_hline, box_depth, h_line_el->subs_skip,
                                                                0, false, st_none );
                    } else {
                        box_draw_vlines( cur_hline, box_depth, 0, 0, false, st_none );
                    }
                }
                h_line_el->subs_skip = 0;
                // insert the HLINEs
                for( cur_el = h_line_el; cur_el != NULL; cur_el = h_line_el ) {
                    h_line_el = h_line_el->next;
                    cur_el->next = NULL;
                    insert_col_main( cur_el );
                }
                // iterate over all horizontal lines
                for( cur_hline = box_line; cur_hline != NULL; cur_hline = cur_hline->next ) {
                    box_draw_vlines( cur_hline, hl_depth, 0, 0, true, st_ext );
                }
                t_doc_el_group.last->depth = hl_depth;
                box_depth = hl_depth;           // reset to depth from HLINE
            } else {                            // do the HLINE only
                // insert the HLINEs
                for( cur_el = h_line_el; cur_el != NULL; cur_el = h_line_el ) {
                    h_line_el = h_line_el->next;
                    if( cur_el->next == NULL ) { // only final HLINE gets the depth
                        cur_el->depth = hl_depth;
                    }
                    cur_el->next = NULL;
                    insert_col_main( cur_el );
                }
                box_depth += cur_skip + hl_depth;
            }
        }
    }

    /* Move any accumulated elements to the current page */

    ProcFlags.group_elements = false;   // ensure elements will be inserted into t_page
    cur_el = t_doc_el_group.first;
    while( cur_el != NULL ) {
        t_doc_el_group.first = cur_el->next;
        cur_el->next = NULL;
        insert_col_main( cur_el );
        cur_el = t_doc_el_group.first;
    }

    return;
}

/***************************************************************************/
/*  merge prev_line and cur_line into box_line                             */
/***************************************************************************/

static void merge_lines( bx_op cur_op )
{
    box_col_set *   box_temp;
    box_col_set *   cur_temp;
    box_col_set *   prev_temp;
    int             box_col;
    int             cur_col;
    int             prev_col;

    // all columns become "up"
    for( prev_temp = prev_line; prev_temp != NULL; prev_temp = prev_temp->next ) {
        for( prev_col = 0; prev_col < prev_temp->current; prev_col++ ) {
            prev_temp->cols[prev_col].v_ind = bx_v_up;
        }
    }

/// temp
    out_msg( "Existing\n" );
    if( prev_line == NULL ) {
        out_msg( "No prev_line\n" );
    } else {
        out_msg( "prev_line length: %i\n", prev_line->length );
        for( prev_temp = prev_line; prev_temp != NULL; prev_temp = prev_temp->next ) {
            out_msg( "prev_line hline:\n" );
            for( prev_col = 0; prev_col < prev_line->current; prev_col++ ) {
                out_msg( "column: %i v_ind: %i\n", prev_temp->cols[prev_col].col,
                                           prev_temp->cols[prev_col].v_ind );
            }
        }
    }

    out_msg( "Current\n" );
    if( cur_line == NULL ) {
        out_msg( "No cur_line\n" );
    } else {
        out_msg( "cur_line length: %i\n", cur_line->length );
        for( cur_temp = cur_line; cur_temp != NULL; cur_temp = cur_temp->next ) {
            out_msg( "cur_line hline:\n" );
            for( cur_col = 0; cur_col < cur_temp->current; cur_col++ ) {
                out_msg( "column: %i v_ind: %i\n", cur_temp->cols[cur_col].col,
                                           cur_temp->cols[cur_col].v_ind );
            }
        }
    }
/// end temp section

    box_col = 0;
    cur_col = 0;
    prev_col = 0;
    box_temp = box_line;
    cur_temp = cur_line;
    prev_temp = prev_line;
    if( (prev_line == NULL) && (cur_line == NULL) ) {   // nothing to do
        internal_err( __FILE__, __LINE__ );
    } else if( prev_line == NULL ) {    // cur_line becomes box_line
        box_line = cur_line;
        cur_line = NULL;
    } else if( cur_line == NULL ) {     // prev_line becomes box_line
        box_line = prev_line;
        prev_line = NULL;
    } else {                            // both cur_line & prev_line have entries
        box_temp = alloc_box_col_set(); // initialize box_line
        box_line = box_temp;
        while( (cur_temp != NULL) || (prev_temp != NULL) ) {    // at least one has not ended
            if( cur_temp != NULL ) {                            // cur_line still has entries
                if( cur_col == cur_temp->current ) {            // end of hline
                    cur_temp = cur_temp->next;
                    cur_col = 0;
                    if( cur_temp != NULL ) {
                        box_temp->next = alloc_box_col_set();
                        box_temp = box_temp->next;
                        box_col = 0;
                    }
                    continue;
                }
            }
            if( prev_temp != NULL ) {                           // prev_line still has entries
                if( prev_col == prev_temp->current ) {          // end of hline
                    prev_temp = prev_temp->next;
                    prev_col = 0;
                    continue;
                }
            }
            if( box_temp->current == box_temp->length) {
                box_temp->length += BOXCOL_COUNT;               // add space for new box columns
                box_temp->cols = mem_realloc( box_temp->cols, box_temp->length *
                                              sizeof( box_col ) );
            }

            if( (cur_temp != NULL) && (prev_temp == NULL) ) {   // cur_line alone still has entries
                box_temp->cols[box_col].col = cur_temp->cols[cur_col].col;
                box_temp->cols[box_col].v_ind = cur_temp->cols[cur_col].v_ind;
                box_temp->current++;
                box_col++;
                cur_col++;
            } else if( (cur_temp == NULL) && (prev_temp != NULL) ) {    // prev_line alone still has entries
                box_temp->cols[box_col].col = prev_temp->cols[prev_col].col;
                if( (cur_op == bx_off) && (prev_temp->cols[prev_col].v_ind == bx_v_both) ) {
                    box_temp->cols[box_col].v_ind = bx_v_up;
                } else {            
                    box_temp->cols[box_col].v_ind = prev_temp->cols[prev_col].v_ind;
                }
                box_temp->current++;
                box_col++;
                prev_col++;
            } else if( cur_temp->cols[cur_col].col < prev_temp->cols[prev_col].col ) {
                box_temp->cols[box_col].col = cur_temp->cols[cur_col].col;
                box_temp->cols[box_col].v_ind = cur_temp->cols[cur_col].v_ind;
                box_temp->current++;
                box_col++;
                cur_col++;
            } else if( cur_temp->cols[cur_col].col > prev_temp->cols[prev_col].col ) {
                box_temp->cols[box_col].col = prev_temp->cols[prev_col].col;
                box_temp->cols[box_col].v_ind = prev_temp->cols[prev_col].v_ind;
                box_temp->current++;
                box_col++;
                prev_col++;
            } else {                            // equal column values
                box_temp->cols[box_col].col = cur_temp->cols[cur_col].col;
                box_temp->cols[box_col].v_ind = bx_v_both;
                box_temp->current++;
                box_col++;
                cur_col++;
                prev_col++;
            }
        }
    }

    /* Ensure cur_line and prev_line are empty */

    if( cur_line != NULL ) {
        add_box_col_set_to_pool( cur_line );        
        cur_line = NULL;
    }
    if( prev_line != NULL ) {
        add_box_col_set_to_pool( prev_line );        
        prev_line = NULL;
    }

/// temp
    out_msg( "Merged\n" );
    if( box_line == NULL ) {
        out_msg( "No box_line (this should never happen)\n" );
    } else {
        out_msg( "box_line length: %i\n", box_line->length );
        for( box_temp = box_line; box_temp != NULL; box_temp = box_temp->next ) {
            out_msg( "box_line hline:\n" );
            for( box_col = 0; box_col < box_temp->current; box_col++ ) {
                out_msg( "column: %i v_ind: %i\n", box_temp->cols[box_col].col,
                                           box_temp->cols[box_col].v_ind );
            }
        }
    }
/// end temp section

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
    bool            first_col;
    box_col_set *   cur_temp;
    bx_op           cur_op;
    char        *   p;
    char        *   pa;
    int32_t         cur_col;            // signed to catch negative relative values
    size_t          len;
    su              boxcolwork;
    uint32_t        prev_col;           // previous value across horizontal splits
    
    ProcFlags.box_cols_cur = false;     // new BX line: no box column list yet
    ProcFlags.group_elements = false;   // stop accumulating doc_elements
    scr_process_break();                // break processing

    p = scan_start;
    while( *p && *p != ' ' ) {          // over cw
        p++;
    }

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
        scan_restart = scan_stop + 1;
        return;
    }

    if( cur_op == bx_none ) {               // reuse token if not recognized
        p = pa;
    }

    /* Now for the numerics, if any */

    if( *p ) {
        ProcFlags.box_cols_cur = true;      // box column list found
        cur_temp = alloc_box_col_set();
        cur_line = cur_temp;
        first_col = true;                   // first column not yet found
        prev_col = 0;
        while( *p ) {
            if( cur_temp->current == cur_temp->length) {
                cur_temp->length += BOXCOL_COUNT;  // add space for new box columns
                cur_temp->cols = mem_realloc( cur_temp->cols, cur_temp->length *
                                             sizeof( box_col ) );
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

    switch( cur_op ) {
    case bx_none :
        merge_lines( cur_op );
        break;    
    case bx_on :
        merge_lines( cur_op );
        break;    
    case bx_off :
        merge_lines( cur_op );
        break;    
    case bx_new :
        merge_lines( cur_op );
        break;    
    case bx_can :
        merge_lines( cur_op );
        break;    
    default :
        internal_err( __FILE__, __LINE__ );
        break;    
    }

    if( (cur_op != bx_set) && (cur_op != bx_can) ) {

        /* set some static globals used in drawing boxes */

        def_height = wgml_fonts[g_curr_font].line_height;
        max_depth = t_page.max_depth - t_page.cur_depth;   // maximum depth available

        /************************************************************/
        /* This will cause the box to be drawn with BOX characters  */
        /* even for a page addressing device which does not define  */
        /* an HLINE block. This is not quite the same as wgml 4.0   */
        /* since in that case wgml 4.0 does not draw the vertical   */
        /* ascenders but do_char_device() will. This may or may not */
        /* require future adjustment.                               */
        /************************************************************/

        if( bin_driver->hline.text == NULL ) {
            do_char_device( cur_op );
        } else {
            do_line_device( cur_op );
        }
    }

    ProcFlags.skips_valid = false;          // ensures following text will use correct skips
    set_h_start();                          // pick up any indents
    if( ( cur_op == bx_off ) || ( cur_op == bx_can ) ) {
        ProcFlags.in_bx_box = false;        // box has ended
        if( box_line != NULL ) {            // clear the box_line
            add_box_col_set_to_pool( box_line );        
            box_line = NULL;
        }
    } else {
        ProcFlags.group_elements = true;    // start accumulating doc_elements
    }

    prev_line = box_line;                   // box_line becomes prev_line
    box_line = NULL;                        // clear box_line

    scan_restart = scan_stop + 1;

    return;
}

