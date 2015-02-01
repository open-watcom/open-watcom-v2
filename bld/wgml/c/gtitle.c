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
* Description:  WGML tag :TITLE processing
*
****************************************************************************/
#include    "wgml.h"
#include    "findfile.h"
#include    "gvars.h"


/***************************************************************************/
/*  prepare title line                                                     */
/*  Note: only used when text exists                                       */
/***************************************************************************/

static void prep_title_line( text_line * p_line, char * p )
{
    text_chars  *   curr_t;
    uint32_t        h_left;
    uint32_t        h_right;
    uint32_t        curr_x;

    h_left = g_page_left + conv_hor_unit( &layout_work.title.left_adjust );
    h_right = g_page_right - conv_hor_unit( &layout_work.title.right_adjust );

    curr_t = alloc_text_chars( p, strlen( p ), g_curr_font );
    curr_t->count = len_to_trail_space( curr_t->text, curr_t->count );
    curr_t->count = intrans( curr_t->text, curr_t->count, g_curr_font );
    curr_t->font = layout_work.title.font;
    curr_t->width = cop_text_width( curr_t->text, curr_t->count, g_curr_font );
    while( curr_t->width > (h_right - h_left) ) {   // too long for line
        if( curr_t->count < 2 ) {        // sanity check
            break;
        }
        curr_t->count -= 1;             // truncate text
        curr_t->width = cop_text_width( curr_t->text, curr_t->count, g_curr_font );
    }
    p_line->first = curr_t;
    p_line->last  = curr_t;
    curr_x = h_left;
    if( layout_work.title.page_position == pos_center ) {
        if( h_left + curr_t->width < h_right ) {
            curr_x += (h_right - h_left - curr_t->width) / 2;
        }
    } else if( layout_work.title.page_position == pos_right ) {
        curr_x = h_right - curr_t->width;
    }
    curr_t->x_address = curr_x;
    ju_x_start = curr_x;

    return;
}


/***************************************************************************/
/*  :TITLE tag                                                             */
/***************************************************************************/

void    gml_title( const gmltag * entry )
{
    char        *   p;
    doc_element *   cur_el;
    text_line   *   p_line      = NULL;
    int8_t          t_spacing;
    font_number     font_save;

    if( !((ProcFlags.doc_sect == doc_sect_titlep) ||
          (ProcFlags.doc_sect_nxt == doc_sect_titlep)) ) {
        g_err( err_tag_wrong_sect, entry->tagname, ":TITLEP section" );
        err_count++;
        show_include_stack();
    }
    p = scan_start;
    if( *p && *p != '.' ) p++;

    while( *p == ' ' ) {                // over WS to attribute
        p++;
    }
    if( *p &&
        ! (strnicmp( "stitle ", p, 7 ) &&   // look for stitle
           strnicmp( "stitle=", p, 7 )) ) {
        char        quote;
        char    *   valstart;

        p += 6;
        while( *p == ' ' ) {
            p++;
        }
        if( *p == '=' ) {
            p++;
            while( *p == ' ' ) {
                p++;
            }
        }
        if( *p == '"' || *p == '\'' ) {
            quote = *p;
            ++p;
        } else {
            quote = ' ';
        }
        valstart = p;
        while( *p && *p != quote ) {
            ++p;
        }
        *p = '\0';
        if( !ProcFlags.stitle_seen ) {  // first stitle goes into dictionary
            add_symvar( &global_dict, "$stitle", valstart, no_subscript, 0 );
            ProcFlags.stitle_seen = true;
        }
        p++;
    }

    if( *p == '.' ) p++;                // over '.'
    if( !ProcFlags.title_text_seen ) {
        if( *p ) {                      // first title goes into dictionary
            add_symvar( &global_dict, "$title", p, no_subscript, 0 );
        }
    }

    start_doc_sect();                   // if not already done

    font_save = g_curr_font;
    g_curr_font = layout_work.title.font;
    t_spacing = layout_work.titlep.spacing;
    if( !ProcFlags.title_tag_top ) {
        set_skip_vars( NULL, &layout_work.title.pre_top_skip, NULL, t_spacing, g_curr_font );
        ProcFlags.title_tag_top = true;
    } else {
        set_skip_vars( &layout_work.title.skip, NULL, NULL, t_spacing, g_curr_font );
    }
    
    p_line = alloc_text_line();
    p_line->line_height = wgml_fonts[g_curr_font].line_height;
    if( *p ) {
        prep_title_line( p_line, p );
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
    scan_start = scan_stop + 1;
}
