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
* Description: implement .co concatenate control
*                        .fo format control
*                        .ju justification control
*                                             incomplete TBD
*  comments are from script-tso.txt
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


/***************************************************************************/
/*  set up to collect the elements of the block                            */
/***************************************************************************/

static void do_co_off( void )
{
    ProcFlags.concat = false;
    ProcFlags.sk_co = true;             // ensures first sub-block will be moved if appropriate
    cur_doc_el_group = alloc_doc_el_group( gt_co );
    cur_doc_el_group->block_font = g_curr_font;
    cur_doc_el_group->next = t_doc_el_group;
    t_doc_el_group = cur_doc_el_group;
    return;
}

/***************************************************************************/
/*  reinsert the doc_elements from the block                               */
/*  identify sub-blocks created with SK -1 or SK n, n > 0                  */
/*  move sub-blocks to the next column as appropriate                      */
/*    includes limited support for wgml 4's strange page break             */
/***************************************************************************/

static void do_co_on( void )
{
    bool                skip_blank = false; // blank line at bottom of page
    bool                text_first = false; // no blank lines precede the first text line
    doc_element     *   cur_el;
    doc_el_group    *   cur_group;          // current group from n_page, not cur_doc_el_group
    doc_el_group    *   last_group;         // group to which doc_elements are to be added
    uint32_t            b_cur_line  = 0;    // current blank line number
    uint32_t            b_line_tot  = 0;    // total blank lines in block
    uint32_t            break_point = 0;    // potential break point
    uint32_t            break_start = 0;    // first possible break point
    uint32_t            line_height = 0;    // line height for block
    uint32_t            lpp         = 0;    // lines per page
    uint32_t            interval    = 0;    // interval between break_point values and between max_lines values
    uint32_t            max_start   = 0;    // first possible maximum line number
    uint32_t            max_lines   = 0;    // maximum line number
    uint32_t            min_lines   = 0;    // minimum line number
    uint32_t            offset      = 0;    // offset from non-full first page
    uint32_t            page_line   = 0;    // current line number on current page
    uint32_t            start_page  = 0;    // apage on which block starts
    uint32_t            t_cur_line  = 0;    // current text line number
    uint32_t            t_line_tot  = 0;    // total text lines in block
    uint32_t            threshold   = 0;    // threshold line number

    /* must have text and it must have been started by CO OFF */

    if( (t_doc_el_group != NULL) && (t_doc_el_group->owner == gt_co) ) {
        cur_doc_el_group = t_doc_el_group;      // detach current element group
        t_doc_el_group = t_doc_el_group->next;  // processed doc_elements go to next group, if any
        cur_doc_el_group->next = NULL;

        if( t_doc_el_group == NULL ) {

            /* Determine the total number of lines. This may need to be changed. */

            line_height = wgml_fonts[cur_doc_el_group->block_font].line_height;
            if( cur_doc_el_group->first != NULL ) {
                cur_el = cur_doc_el_group->first;
                while( cur_el != NULL ) {
                    if( cur_el->type == el_text ) {
                        t_line_tot += cur_el->depth / line_height;
                    } else if( cur_el->type == el_vspace ) {    // other types may need to be added
                        b_line_tot += cur_el->blank_lines / line_height;
                    }
                    cur_el = cur_el->next;
                }
            }
            threshold = layout_work.widow.threshold;

            /* Split and move as needed and submit to t_page */

            if( ((cur_doc_el_group->depth + t_page.cur_depth) <= t_page.max_depth) ) {

                /* the entire block will fit on the current page */

                while( cur_doc_el_group->first != NULL ) {
                    cur_el = cur_doc_el_group->first;
                    cur_doc_el_group->first = cur_doc_el_group->first->next;
                    cur_el->next = NULL;
                    insert_col_main( cur_el );
                }
                add_doc_el_group_to_pool( cur_doc_el_group );
                cur_doc_el_group = NULL;
            } else {

                /* divide block into sub-blocks at each SK */

                last_group = cur_doc_el_group;              // initialize last_group
                cur_el = last_group->first->next;           // detach doc_elements after first
                last_group->first->next = NULL;
                last_group->last = last_group->first;
                last_group->depth = last_group->last->blank_lines + last_group->last->subs_skip +
                                    last_group->last->depth;

                while( cur_el != NULL ) {
                    if( cur_el->do_split ) {                // new sub-block
                        last_group->next = alloc_doc_el_group( gt_co );
                        last_group = last_group->next;
                        last_group->first = cur_el;
                        last_group->last = last_group->first;
                    } else {                                // add to current sub-block
                        last_group->last->next = cur_el;
                        last_group->last = last_group->last->next;
                    }
                    cur_el = cur_el->next;                  // detach from list
                    last_group->last->next = NULL;
                    last_group->depth += last_group->last->blank_lines +
                                         last_group->last->subs_skip + last_group->last->depth;
                }

                /* output sub-blocks */

                while( (cur_doc_el_group != NULL) && (cur_doc_el_group->first != NULL) ) {

                    /****************************************************************/
                    /* The text starts at the top of the next column under these    */
                    /* conditions:                                                  */
                    /* 1. This is the first block or an SK with -1 or an argument   */
                    /*    greater than 0 preceded the text in the block.            */
                    /* 2. The block will not fit in the current column.             */
                    /* 3. The block will fit in a column with no other text.        */
                    /* 4. See below for additional conditions.                      */
                    /****************************************************************/

                    if( cur_doc_el_group->first->do_split
                            && ((cur_doc_el_group->depth + t_page.cur_depth) > t_page.max_depth)
                            && (cur_doc_el_group->depth <= t_page.max_depth) ) {

                        next_column();
                        page_line = 0;
                    }

                    /* setup before text output */

                    if( t_page.cur_depth == 0 ) {           // use top skip
                        lpp = (t_page.max_depth - t_page.cur_depth
                              - cur_doc_el_group->first->top_skip) / line_height;  // for first page
                    } else {                                // use subs_skip
                        lpp = (t_page.max_depth - t_page.cur_depth
                              - cur_doc_el_group->first->subs_skip) / line_height;  // for first page
                    }
                    start_page = g_apage;
                    if( (cur_doc_el_group->first->type == el_text)
                            && ((cur_doc_el_group->first->top_skip == 0)
                                || (t_page.cur_depth == 0)) ) {            // first line is text
                        text_first = true;
                    }

                    /* Now output the text */

                    while( cur_doc_el_group->first != NULL ) {
                        cur_el = cur_doc_el_group->first;
                        if( (cur_el->next != NULL) && (cur_el->type == el_text) &&
                                (cur_el->next->type == el_vspace) ) {
                            cur_el->element.text.vspace_next = true;        // matches wgml 4.0
                        }
                        if( (page_line == lpp)
                                && ((cur_el->type != el_text)
                                || (!cur_el->element.text.vspace_next)) ) {
                            page_line = 0;
                            /* Skip blank line at bottom of page */
                            if( (cur_el->type == el_vspace) && (cur_el->blank_lines >= line_height) ) {
                                skip_blank = true;
                            } else {
                                skip_blank = false;
                            }
                        } else if( page_line == lpp + 1 ) {                 // extra line at bottomm of page
                            page_line = 0;
                        }

                        /****************************************************************/
                        /* This depends on only the first doc_element having skips > 0  */
                        /* It should work for included source files used as examples    */
                        /* If subblocks exist, this may not work properly               */
                        /* But that is probably true of this entire section -- which    */
                        /* is implementing what appears to be a bug in wgml 4.0         */
                        /****************************************************************/

                        if( (cur_el->type == el_text) || !skip_blank ) {
                            if( cur_el->type == el_text ) {
                                t_cur_line += cur_el->depth / line_height;
                                page_line += cur_el->depth / line_height;
                            } else if( cur_el->type == el_vspace ) {    // other types may need to be added
                                b_cur_line += cur_el->blank_lines / line_height;
                                page_line += cur_el->blank_lines / line_height;
                            }
                        } else {
                            skip_blank = false;
                        }
                        cur_doc_el_group->first = cur_doc_el_group->first->next;
                        cur_el->next = NULL;
                        insert_col_main( cur_el );

                        /****************************************************************/
                        /* wgml 4.0 moves text to a new column under these conditions:  */
                        /* 1. The break_point has a value greater than 0; see below.    */
                        /* 2. For the break point:                                      */
                        /*    The current line is the break point line.                 */
                        /*    The break point is not at the bottom of a column.         */
                        /*    The adjusted maximal line is not less than the total      */
                        /*        number of text lines.                                 */
                        /* 3. For the threshold:                                        */
                        /*    The current line is the threshold line.                   */
                        /*    The threshold line is so far down the page that the break */
                        /*    point is at the top of the next page.                     */
                        /* Note that each of 2 and 3 will occur if threshold is greater */
                        /*    than 1.                                                   */
                        /****************************************************************/

                        if( (break_point > 0) && (cur_el->type == el_text) ) {
                            if( (t_cur_line == (break_point - threshold))
                                    && (page_line >= (lpp - threshold + 1))
                                    && !text_first ) { // the threshold line is the line before the break
                                next_column();              // threshold break
                                page_line = 0;
                            } else if( (t_cur_line == break_point) && (page_line < lpp) ) {
                                /*Take for blank lines after break point into account */
                                if( (max_lines - (b_line_tot - b_cur_line)) >= t_line_tot ) {
                                    next_column();              // break point break
                                    page_line = 0;
                                    if( cur_doc_el_group->first->type == el_vspace ) {
                                        /* Skip blank line after break point */
                                        cur_el = cur_doc_el_group->first;
                                        cur_doc_el_group->first = cur_doc_el_group->first->next;
                                        cur_el->next = NULL;
                                        add_doc_el_to_pool( cur_el );
                                    }
                                }
                            }
                        }

                        /* do setup only once and only on second output page */

                        if( (g_apage == (start_page + 1)) && (page_line == 1) ) {
                            lpp = t_page.max_depth / line_height;               // for empty page
                            if( (threshold < lpp) ) { // threshold must be at most lpp - 1
                                interval = lpp + threshold + 2;
                                if( (t_cur_line + b_cur_line) < lpp ) {         // partial first page
                                    offset = lpp - t_cur_line - b_cur_line + 1; // offset from partial first page
                                    offset %= lpp;
                                }
                                if( t_line_tot > ((2 * lpp) - offset) ) {   // must have at least three pages
                                    break_start = lpp + threshold + 1;
                                    max_start = break_start + lpp;  // bottom of next page if break occurs
                                    max_lines = max_start + (((t_line_tot / interval) - 1)
                                                * interval);        // limit of possible effect
                                    if( !((t_line_tot + offset) % lpp) ) {  // min_lines past last page
                                        max_lines -= interval;
                                    }
                                    min_lines = ((lpp * ((t_line_tot + b_line_tot + offset) / lpp)) + 1);
                                    if( !((t_line_tot + b_line_tot + offset) % lpp) ) {  // min_lines past last page
                                        min_lines -= lpp;
                                    }
                                    min_lines -= offset;              // top of last block output page
                                    if( max_lines < min_lines ) {     // must be on last page
                                        max_lines += lpp;
                                    }
                                    /********************************************************/
                                    /* A break point will be designated under these         */
                                    /* conditions:                                          */
                                    /* 1. The total number of text lines is less or equal   */
                                    /*    to the value of max_lines                         */
                                    /* 2. The break point, if computed, is less than or     */
                                    /*    equal to the min_lines                            */
                                    /* 3. The min_lines, if moved up by the number of blank */
                                    /*    lines, is greater than the break point, if        */
                                    /*    computed would be -- that is, the break point has */
                                    /*    not been moved onto the last page of the block    */
                                    /********************************************************/

                                    if( (t_line_tot <= max_lines)
                                            && (min_lines > (max_lines - lpp))
                                            ) {
                                        break_point = max_lines - lpp;  // valid break point
                                    }
                                }
                            }
                        }
                    }
                    cur_group = cur_doc_el_group;
                    cur_doc_el_group = cur_doc_el_group->next;
                    cur_group->next = NULL;
                    add_doc_el_group_to_pool( cur_group );
                    cur_group = NULL;
                }
            }
        } else {

            /* append entire group to t_doc_el_group */

            while( cur_doc_el_group->first != NULL ) {
                cur_el = cur_doc_el_group->first;
                cur_doc_el_group->first = cur_doc_el_group->first->next;
                cur_el->next = NULL;
                insert_col_main( cur_el );
            }
            add_doc_el_group_to_pool( cur_doc_el_group );
            cur_doc_el_group = NULL;
        }
    }
    ProcFlags.concat = true;
    ProcFlags.sk_co = false;
    return;
}

/***************************************************************************/
/*  process .ju setting, .co too if both set                               */
/*                                                                         */
/*  NOTE: the implementation is correct for FO; CO actually behaves a bit  */
/*        differently                                                      */
/***************************************************************************/

static void process_fo_ju( bool both , char *cwcurr )
{
    char            *   pa;
    char            *   p;
    int                 len;

    p = scan_start;
    SkipSpaces( p );                    // next word start
    pa = p;
    SkipNonSpaces( p );                 // end of word
    len = p - pa;
    switch( len ) {
    case 0 :                            // omitted means ON
        if( both ) {
            ProcFlags.concat = true;
        }
        ProcFlags.justify = ju_on;
        scan_restart = p;
        break;
    case 2 :                            // only ON valid
        if( !strnicmp( "ON", pa, 2 ) ) {
            if( both ) {
                do_co_on();
            }
            ProcFlags.justify = ju_on;
            scan_restart = pa + len;
        } else {
            xx_line_err_cc( err_xx_opt, cwcurr, pa );
        }
        break;
    case 3 :                            // only OFF valid
        if( !strnicmp( "OFF", pa, 3 ) ) {
            if( both ) {
                do_co_off();
            }
            ProcFlags.justify = ju_off;
            scan_restart = pa + len;
        } else {
            xx_line_err_cc( err_xx_opt, cwcurr, pa );
        }
        break;
    case 4 :                            // Left or half valid
        if( !strnicmp( "LEFT", pa, 4 ) ) {
            if( both ) {
                ProcFlags.concat = true;
            }

            /***************************************************************/
            /*  .ju left is treated as .ju off by wgml4.0                  */
            /*  system variable &SYSJU is set to OFF                       */
            /***************************************************************/
            ProcFlags.justify = ju_off; // left is like off for wgml 4.0
            scan_restart = pa + len;
        } else {
            if( !strnicmp( "HALF", pa, 4 ) ) {
                if( both ) {
                    ProcFlags.concat = true;
                }
                ProcFlags.justify = ju_half;
                scan_restart = pa + len;
            } else {
                xx_line_err_cc( err_xx_opt, cwcurr, pa );
            }
        }
        break;
    case 5 :                            // only Right valid
        if( !strnicmp( "RIGHT", pa, 5 ) ) {
            if( both ) {
                ProcFlags.concat = true;
            }
            ProcFlags.justify = ju_right;
            scan_restart = pa + len;
        } else {
            xx_line_err_cc( err_xx_opt, cwcurr, pa );
        }
        break;
    case 6 :                            // center or inside valid
        if( !strnicmp( "CENTER", pa, 6 ) || !strnicmp( "CENTRE", pa, 6 ) ) {
            if( both ) {
                ProcFlags.concat = true;
            }
            ProcFlags.justify = ju_centre;
            scan_restart = pa + len;
        } else {
            if( !strnicmp( "INSIDE", pa, 6 ) ) {
                if( both ) {
                    ProcFlags.concat = true;
                }
                ProcFlags.justify = ju_inside;
                scan_restart = pa + len;
            } else {
                xx_line_err_cc( err_xx_opt, cwcurr, pa );
            }
        }
        break;
    case 7 :                            // only outside valid
        if( !strnicmp( "OUTSIDE", pa, 7 ) ) {
            if( both ) {
                ProcFlags.concat = true;
            }
            ProcFlags.justify = ju_outside;
            scan_restart = pa + len;
        } else {
            xx_line_err_cc( err_xx_opt, cwcurr, pa );
        }
        break;
    default:
        xx_line_err_cc( err_xx_opt, cwcurr, pa );
    }

    /********************************************************/
    /* FO and JU reduce any pending post_skip to 0          */
    /********************************************************/

    g_post_skip = 0;

    return;
}

/****************************************************************************/
/* FORMAT combines the effect of Concatenate and Justify.                   */
/*                                                                          */
/*      旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커        */
/*      |       |                                                  |        */
/*      |       |    <ON|OFF                                       |        */
/*      |  .FO  |     Left|Right|Centre|                           |        */
/*      |       |     INSIDE|OUTSIDE|HALF>                         |        */
/*      |       |                                                  |        */
/*      읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸        */
/*                                                                          */
/* This control word creates a break.  The "OFF" operand is equivalent to   */
/* the control words Concatenate OFF and Justify OFF.  An omitted operand   */
/* is treated as "ON",  which is equivalent to Concatenate ON and Justify   */
/* ON.  The other possible operands specify Concatenate ON and the appro-   */
/* priate mode of Justify.  See the .CO and .JU descriptions for details.   */
/****************************************************************************/

/***************************************************************************/
/* CONCATENATE  enables  or cancels  the  formation  of output  lines  by  */
/* concatenating input lines and truncating  at the nearest word boundary  */
/* to fit on the output line.                                              */
/*                                                                         */
/*      旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커       */
/*      |       |                                                  |       */
/*      |  .CO  |    <ON|OFF>                                      |       */
/*      |       |                                                  |       */
/*      읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸       */
/*                                                                         */
/*                                                                         */
/* ON:  Output lines will be created by adding words from the input lines  */
/*    until a word is found which will  not fit on the output line.   The  */
/*    resulting output line will therefore have as many words as possible  */
/*    without  exceeding the  line-length  restriction (or  column-length  */
/*    restriction, if in multiple-column mode).                            */
/* OFF:  Suppresses  the concatenation action,   which results in  a "one  */
/*    line in,  one  line out" effect that  may exceed the length  of the  */
/*    output line,   depending on  the lengths of  the input  lines.   If  */
/*    justification (.JU)  is  still enabled,  extra blanks  will then be  */
/*    added between words to extend the line to the right margin.          */
/*                                                                         */
/* This control word  causes a break.   An omitted operand  is treated as  */
/* "ON".   Concatenation is "ON" at the  start of SCRIPT processing,  and  */
/* can only be suppressed  by the .CO or .FO control  words with an "OFF"  */
/* operand.   Multiple  blanks that occur  together within an  input line  */
/* will  all be  retained  in the  concatenated  output  line unless  the  */
/* concatenation  process "breaks"  the  input line  at  that point  when  */
/* adding words to the output line.                                        */
/***************************************************************************/

/****************************************************************************/
/* JUSTIFY causes output lines to be padded with inter-word blanks to the   */
/* right margin.                                                            */
/*                                                                          */
/*      旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커        */
/*      |       |                                                  |        */
/*      |       |    <ON|HALF|OFF|                                 |        */
/*      |  .JU  |     Left|Right|Centre|                           |        */
/*      |       |     INSIDE|OUTSIDE>                              |        */
/*      |       |                                                  |        */
/*      읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸        */
/*                                                                          */
/*                                                                          */
/* OFF:  suppresses the  justification process.   If Concatenate  is OFF,   */
/*    input lines  will be  copied to  output lines  on a  "one for  one"   */
/*    basis;  if  Concatenate is  ON,  the  output line  will retain  its   */
/*    "ragged right" style from the point in the concatenation process at   */
/*    which SCRIPT decides it cannot get any more on the output line.       */
/* ON:  On output devices that support "proportional spacing", the inter-   */
/*    word blanks will be equal between all words on the output line.  On   */
/*    other devices,  the following justification algorithm is used until   */
/*    no blanks remain  to be distributed between the  words:   one extra   */
/*    blank is first placed after each period, exclamation mark, or ques-   */
/*    tion mark in the  output line;  then an extra blank  will be placed   */
/*    after any colon  or semi-colon,  and then after any  comma or right   */
/*    parenthesis;  if more blanks must still  be added they are distrib-   */
/*    uted at equidistant places between words.                             */
/* HALF:  produces an effect between  "ragged right" and "full justifica-   */
/*    tion" by only  distributing half of the blanks that  it would under   */
/*    "ON".                                                                 */
/* <Left|Right|Centre|Center>:   causes  the  output  line  to  be  left-   */
/*    justified, right-justified or centered.                               */
/* <INSIDE|OUTSIDE>:   INSIDE  causes  odd-numbered  pages  to  be  left-   */
/*    justified and  even-numbered pages to be  right-justified;  OUTSIDE   */
/*    does the opposite.                                                    */
/*                                                                          */
/* This control word causes a break.   ON  is in effect until a ".JU OFF"   */
/* or ".FO OFF" is encountered.   An omitted  operand is treated as "ON".   */
/* The  Format control  word combines  the functions  of Concatenate  and   */
/* Justify.                                                                 */
/****************************************************************************/

/***************************************************************************/
/*  scr_co    implement .co concatenate control word                       */
/*                                                                         */
/*  NOTE: the implementation is correct for CO; FO actually behaves a bit  */
/*        differently                                                      */
/*        each text line is assumed to be in its own doc_element           */
/*        until the block is split, there is only one doc_el_group to be   */
/*        processed                                                        */
/*        further work may be needed                                       */
/***************************************************************************/

void    scr_co( void )
{
    char                cwcurr[4 ];
    char            *   pa;
    char            *   p;
    int                 len;

    cwcurr[0] = SCR_char;
    cwcurr[1] = 'c';
    cwcurr[2] = 'o';
    cwcurr[3] = '\0';

    p = scan_start;
    SkipSpaces( p );                    // next word start
    pa = p;
    SkipNonSpaces( p );                 // end of word
    len = p - pa;
    switch( len ) {
    case 0 :                            // omitted means ON
        ProcFlags.concat = true;
        scan_restart = pa;
        break;
    case 2 :                            // only ON valid
        if( !strnicmp( "ON", pa, 2 ) ) {
            do_co_on();
            scan_restart = pa + len;
        } else {
            xx_line_err_cc( err_xx_opt, cwcurr, pa );
        }
        break;
    case 3 :                            // only OFF valid
        if( !strnicmp( "OFF", pa, 3 ) ) {
            do_co_off();
            scan_restart = pa + len;
        } else {
            xx_line_err_cc( err_xx_opt, cwcurr, pa );
        }
        break;
    default:
        xx_line_err_cc( err_xx_opt, cwcurr, pa );
    }
    return;
}


/***************************************************************************/
/*  scr_fo    implement .fo format control                                 */
/***************************************************************************/

void    scr_fo( void )
{
    char            cwcurr[4];

    cwcurr[0] = SCR_char;
    cwcurr[1] = 'f';
    cwcurr[2] = 'o';
    cwcurr[3] = '\0';

    process_fo_ju( true, cwcurr );      // .ju and .co processing
}


/***************************************************************************/
/*  scr_ju    implement .ju justify control                                */
/***************************************************************************/

void    scr_ju( void )
{
    char            cwcurr[4];

    cwcurr[0] = SCR_char;
    cwcurr[1] = 'j';
    cwcurr[2] = 'u';
    cwcurr[3] = '\0';

    process_fo_ju( false, cwcurr );     // only .ju processing

}
