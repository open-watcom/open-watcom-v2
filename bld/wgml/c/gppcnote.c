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
* Description:  WGML tags :P, :PC and :NOTE processing
*
****************************************************************************/

#include    "wgml.h"
#include    "gvars.h"

/***************************************************************************/
/*  :P. :PC common routine                                                 */
/***************************************************************************/
void    proc_p_pc( p_lay_tag * p_pc )
{
    char        *   p;

    scan_err = false;
    p = scan_start;

    ProcFlags.keep_left_margin = true;  //    special Note indent
    start_doc_sect();                   // if not already done

    scr_process_break();
    if( nest_cb->c_tag == t_NONE ) {
        g_cur_left = g_page_left + g_indent;// left start    TBD
    } else {
        g_cur_left = g_cur_left;
    }
                                        // possibly indent first line
    g_cur_h_start = g_cur_left + conv_hor_unit( &(p_pc->line_indent) );

    g_cur_threshold = layout_work.widow.threshold; // standard threshold

    if( *p == '.' ) p++;                // over '.'

    set_skip_vars( &(p_pc->pre_skip), NULL, &(p_pc->post_skip), g_spacing_ln, g_curr_font );

    post_space = 0;

    process_text( p, g_curr_font );

    scan_start = scan_stop;
    return;
}

/***************************************************************************/
/*  :P.perhaps paragraph elements                                          */
/***************************************************************************/
extern  void    gml_p( gml_tag tag )
{
    tag = tag;
    proc_p_pc( &layout_work.p );
    ProcFlags.empty_doc_el = true;  // for next break, not this tag's break
}

/***************************************************************************/
/*  :PC.perhaps paragraph elements                                         */
/***************************************************************************/
extern  void    gml_pc( gml_tag tag )
{
    tag = tag;
    proc_p_pc( &layout_work.pc );
}

/***************************************************************************/
/*  :NOTE.perhaps paragraph elements                                       */
/***************************************************************************/
extern  void    gml_note( gml_tag tag )
{
    char            *p;
    font_number     font_save;
    text_chars      *marker;
    uint32_t        spc_cnt;

    tag = tag;
    scan_err = false;
    p = scan_start;

    start_doc_sect();                   // if not already done

    scr_process_break();

    font_save = g_curr_font;
    g_curr_font = layout_work.note.font;
    set_skip_vars( &layout_work.note.pre_skip, NULL, &layout_work.note.post_skip, g_spacing_ln, g_curr_font );
    post_space = 0;

    if( nest_cb->c_tag == t_NONE ) {
        g_cur_left = g_page_left + conv_hor_unit( &layout_work.note.left_indent );
    } else {
        g_cur_left += conv_hor_unit( &layout_work.note.left_indent );
    }
    g_cur_h_start = g_cur_left;
    ProcFlags.keep_left_margin = true;  // keep special Note indent

    start_line_with_string( layout_work.note.string, layout_work.note.font, false );

    /* the value of post_space after start_line_with_string() is wrong for  */
    /* two reasons: 1) it uses the wrong font; 2) it is at most "1" even if */
    /* more than one space appears at the end of the note_string.           */

    spc_cnt = post_space / wgml_fonts[g_curr_font].spc_width;
    post_space = spc_cnt * wgml_fonts[font_save].spc_width;
    if( (t_line != NULL)  && (t_line->last != NULL) ) {
        g_cur_left += t_line->last->width + post_space;
    }
    g_cur_h_start = g_cur_left;
    ju_x_start = g_cur_h_start;

    g_spacing_ln = layout_work.note.spacing;
    g_curr_font = layout_work.defaults.font;

    set_skip_vars( NULL, NULL, NULL, g_spacing_ln, g_curr_font );
    if( *p == '.' ) p++;                // over '.'
    while( *p == ' ' ) p++;             // skip initial space
    if( *p ) {                          // if text follows
        post_space = 0;
        process_text( p, g_curr_font );
    } else if( !ProcFlags.concat && ProcFlags.has_aa_block &&
               (t_line != NULL) && (post_space > 0) ) {

        /* only create marker if line not empty,                            */
        /* :NOTE note_string is not nullstring and ends in at least 1 space */

        marker = alloc_text_chars( NULL, 0, font_save );
        marker->x_address = g_cur_h_start;
        if( t_line->first == NULL ) {
            t_line->first = marker;
            t_line->last = t_line->first;
        } else {
            marker->prev = t_line->last;
            t_line->last->next = marker;
            t_line->last = t_line->last->next;
        }
        post_space = 0;
    }
    g_curr_font = font_save;
    scan_start = scan_stop;
    return;
}

