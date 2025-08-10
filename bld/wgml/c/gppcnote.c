/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
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


/***************************************************************************/
/*  Setup for both proc_p_pc() and do_force_pc()                           */
/***************************************************************************/

static void p_pc_setup( p_lay_tag * p_pc )
{
    ProcFlags.keep_left_margin = true;  // special Note indent
    if( ProcFlags.overprint && ProcFlags.cc_cp_done ) {
        ProcFlags.overprint = false;    // cancel overprint
    }
    ProcFlags.cc_cp_done = false;       // cancel CC/CP notification
    start_doc_sect();                   // if not already done

    if( g_line_indent == 0 ) {
        ProcFlags.para_starting = false;    // clear for this tag's break
    }
    scr_process_break();

    g_line_indent = conv_hor_unit( &(p_pc->line_indent), g_curr_font );

    t_page.cur_width = t_page.cur_left + g_line_indent; // possibly indent first line

    g_cur_threshold = layout_work.widow.threshold; // standard threshold

    set_skip_vars( &(p_pc->pre_skip), NULL, &(p_pc->post_skip), g_text_spacing, g_curr_font );

    ProcFlags.para_starting = true;     // for next break, not this tag's break

    post_space = 0;

    return;
}

/***************************************************************************/
/*  :P. :PC common routine                                                 */
/***************************************************************************/

static void proc_p_pc( p_lay_tag *p_pc, g_tags t )
{
    char    *   p;

    p_pc_setup( p_pc );

    ProcFlags.block_starting = true;    // to catch empty paragraphs

    g_scan_err = false;
    p = g_scandata.s;

    SkipDot( p );                       // over '.'
    if( *p != '\0' ) {
        if( (t == T_P) && !ProcFlags.concat ) {
            if( input_cbs->fmflags & II_tag ) {
                g_post_skip = 0;
            } else {
                g_subs_skip = g_post_skip;
                g_post_skip = 0;
            }
        }
        process_text( p, g_curr_font );
    } else if( (t == T_P) && !ProcFlags.concat ) {
        g_post_skip = 0;
    }

    g_scandata.s = g_scandata.e;
    return;
}

/***************************************************************************/
/*  :P.perhaps paragraph elements                                          */
/***************************************************************************/

extern void gml_p( const gmltag * entry )
{
    proc_p_pc( &layout_work.p, entry->u.tagid );
}

/***************************************************************************/
/*  :PC.perhaps paragraph elements                                         */
/***************************************************************************/

extern void gml_pc( const gmltag * entry )
{
    proc_p_pc( &layout_work.pc, entry->u.tagid );
}

/***************************************************************************/
/*  :NOTE.perhaps paragraph elements                                       */
/***************************************************************************/

extern void gml_note( const gmltag * entry )
{
    char        *   p;
    font_number     font_save;
    text_chars  *   marker;

    (void)entry;

    g_scan_err = false;
    p = g_scandata.s;

    start_doc_sect();                   // if not already done

    scr_process_break();

    note_lm = t_page.cur_left;
    font_save = g_curr_font;
    set_skip_vars( &layout_work.note.pre_skip, NULL, NULL,
                    g_text_spacing, layout_work.note.font );

    t_page.cur_left += conv_hor_unit( &layout_work.note.left_indent, layout_work.note.font );
    t_page.cur_width = t_page.cur_left;
    ju_x_start = t_page.cur_width;
    ProcFlags.keep_left_margin = true;  // keep special Note indent

    if( strlen( layout_work.note.string ) > 0 ) {
        process_text( layout_work.note.text, layout_work.note.font );
    }
    insert_hard_spaces( layout_work.note.spaces, strlen( layout_work.note.spaces ), FONT0 );
    t_page.cur_left = t_page.cur_width; // set indent for following text
    ProcFlags.note_starting = true;
    ProcFlags.zsp = true;

    g_text_spacing = layout_work.note.spacing;
    g_curr_font = layout_work.defaults.font;

    set_skip_vars( NULL, NULL, &layout_work.note.post_skip, g_text_spacing, g_curr_font );
    SkipDot( p );                       // over '.'
    SkipSpaces( p );                    // skip initial space
    if( *p != '\0' ) {                  // if text follows
        post_space = 0;
        process_text( p, g_curr_font );
    } else if( !ProcFlags.concat && ProcFlags.has_aa_block &&
               (t_line != NULL) && (post_space > 0) ) {

        /* only create marker if line not empty,                            */
        /* :NOTE note_string is not nullstring and ends in at least 1 space */

        marker = alloc_text_chars( NULL, 0, font_save );
        marker->x_address = t_page.cur_width;
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

    ProcFlags.block_starting = true;    // to catch empty paragraphs

    g_curr_font = font_save;
    g_scandata.s = g_scandata.e;
    return;
}

/***************************************************************************/
/*  Force PC on text line following certain blocks                         */
/*  Note: only called with text, so ProcFlags.block_starting is not set    */ 
/***************************************************************************/

extern void do_force_pc( char * p )
{
    p_pc_setup( &layout_work.pc );

    /* Inline tags use NULL because the text font is different from the font needed by PC */
    if( (p != NULL) && (*p != '\0') ) {
        process_text( p, g_curr_font );
    }

    return;
}
