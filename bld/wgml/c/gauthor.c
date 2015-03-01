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
* Description:  WGML tags :AUTHOR processing
*
****************************************************************************/

#include "wgml.h"
 

/***************************************************************************/
/*  prepare author line                                                    */
/***************************************************************************/
 
static void prep_author_line( text_line *p_line, const char *p )
{
    text_chars  *   curr_t;
    uint32_t        h_left;
    uint32_t        h_right;
    uint32_t        curr_x;
 
    h_left = g_page_left + conv_hor_unit( &layout_work.author.left_adjust );
    h_right = g_page_right - conv_hor_unit( &layout_work.author.right_adjust );
 
    curr_t = alloc_text_chars( p, strlen( p ), g_curr_font );
    curr_t->count = len_to_trail_space( curr_t->text, curr_t->count );
    curr_t->count = intrans( curr_t->text, curr_t->count, g_curr_font );
    curr_t->width = cop_text_width( curr_t->text, curr_t->count, g_curr_font );
    while( curr_t->width > (h_right - h_left) ) {   // too long for line
        if( curr_t->count < 2 ) {       // sanity check
            break;
        }
        curr_t->count -= 1;             // truncate text
        curr_t->width = cop_text_width( curr_t->text, curr_t->count, g_curr_font );
    }
    p_line->first = curr_t;
    curr_x = h_left;
    if( layout_work.author.page_position == pos_center ) {
        if( h_left + curr_t->width < h_right ) {
            curr_x = h_left + (h_right - h_left - curr_t->width) / 2;
        }
    } else if( layout_work.author.page_position == pos_right ) {
        curr_x = h_right - curr_t->width;
    }
    curr_t->x_address = curr_x;
    ju_x_start = curr_x;
 
    return;
}
 
/***************************************************************************/
/*  :author tag                                                             */
/***************************************************************************/
 
void    gml_author( gml_tag gtag )
{
    char        *   p;
    doc_element *   cur_el;
    text_line   *   p_line;
    spacing_line    spacing_ln;
    font_number     font_save;
    int32_t         rc;
    symsub      *   authorval;
 
    if( !((ProcFlags.doc_sect == doc_sect_titlep) ||
          (ProcFlags.doc_sect_nxt == doc_sect_titlep)) ) {
        g_err( err_tag_wrong_sect, gml_tagname( gtag ), ":TITLEP section" );
        err_count++;
        show_include_stack();
    }
    p = scan_start;
    if( *p && *p == '.' ) p++;          // over . to docnum

    while( *p == ' ' ) {                // over WS to attribute
        p++;
    }
    if( !ProcFlags.author_tag_seen ) {
        rc = find_symvar( &sys_dict, "$author", no_subscript, &authorval );
        if( *p ) {                      // author specified
            strcpy( authorval->value, p );
        } else {
            *(authorval->value) = 0;
        }
    }
 
    start_doc_sect();                   // if not already done

    font_save = g_curr_font;
    g_curr_font = layout_work.author.font;
    spacing_ln = layout_work.titlep.spacing;

    /************************************************************/
    /*  pre_skip and skip are treated as pre_top_skip because   */
    /*  they are always used at the top of the page             */
    /*  this is not what the docs say, at least about pre_skip  */
    /************************************************************/

    if( !ProcFlags.author_tag_seen ) {
        set_skip_vars( NULL, &layout_work.author.pre_skip, NULL, spacing_ln, g_curr_font );
    } else {
        set_skip_vars( NULL, &layout_work.author.skip, NULL, spacing_ln, g_curr_font );
    }

    p_line = alloc_text_line();
    p_line->line_height = wgml_fonts[g_curr_font].line_height;
    if( *p ) {
        prep_author_line( p_line, p );
    }

    cur_el = alloc_doc_el( el_text );
    cur_el->blank_lines = g_blank_lines;
    g_blank_lines = 0;
    cur_el->depth = p_line->line_height + g_spacing;
    cur_el->subs_skip = g_subs_skip;
    cur_el->top_skip = g_top_skip;
    cur_el->element.text.overprint = ProcFlags.overprint;
    ProcFlags.overprint = false;
    cur_el->element.text.spacing = g_spacing;
    cur_el->element.text.first = p_line;
    ProcFlags.skips_valid = false;
    p_line = NULL;
    insert_col_main( cur_el );

    g_curr_font = font_save;
    ProcFlags.author_tag_seen = true;
    scan_start = scan_stop;
}
