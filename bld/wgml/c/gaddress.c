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
* Description:  WGML tags :ADDRESS and :eADDRESS and :ALINE
*                    with helper functions
*
****************************************************************************/
#include    "wgml.h"
#include    "gvars.h"

static  bool            first_aline;    // special for first :ALINE
static  int8_t          a_spacing;      // spacing between adr lines
static  font_number     font_save;      // save for font


/***************************************************************************/
/*  :ADDRESS                                                               */
/***************************************************************************/

extern  void    gml_address( const gmltag * entry )
{
    if( !((ProcFlags.doc_sect == doc_sect_titlep) ||
          (ProcFlags.doc_sect_nxt == doc_sect_titlep)) ) {
        g_err( err_tag_wrong_sect, entry->tagname, ":TITLEP section" );
        err_count++;
        show_include_stack();
        scan_start = scan_stop + 1;
        return;
    }
    ProcFlags.address_active = true;
    first_aline = true;
    font_save = g_curr_font;
    g_curr_font = layout_work.address.font;
    rs_loc = address_tag;

    init_nest_cb();
    nest_cb->p_stack = copy_to_nest_stack();
    nest_cb->c_tag = t_ADDRESS;

    spacing = layout_work.titlep.spacing;

    /************************************************************/
    /*  pre_skip is treated as pre_top_skip because             */
    /*  it is always used at the top of the page                */
    /*  this is not what the docs say                           */
    /************************************************************/

    set_skip_vars( NULL, &layout_work.address.pre_skip, NULL, spacing, g_curr_font );

    ProcFlags.group_elements = true;

    scan_start = scan_stop + 1;
    return;
}


/***************************************************************************/
/*  :eADDRESS                                                              */
/***************************************************************************/

extern  void    gml_eaddress( const gmltag * entry )
{
    tag_cb  *   wk;

    entry = entry;
    if( !ProcFlags.address_active ) {   // no preceding :ADDRESS tag
        g_err_tag_prec( "ADDRESS" );
        scan_start = scan_stop + 1;
        return;
    }
    g_curr_font = font_save;
    ProcFlags.address_active = false;
    rs_loc = titlep_tag;
    wk = nest_cb;
    nest_cb = nest_cb->prev;
    add_tag_cb_to_pool( wk );

    /*  place the accumulated ALINES on the proper page */

    ProcFlags.group_elements = false;
    if( t_doc_el_group.first != NULL ) {
        t_doc_el_group.depth += (t_doc_el_group.first->blank_lines +
                                t_doc_el_group.first->subs_skip);
    }

    if( (t_doc_el_group.depth + t_page.cur_depth) > t_page.max_depth ) {
        /*  the block won't fit on this page */

        if( t_doc_el_group.depth  <= t_page.max_depth ) {
            /*  the block will on the next page */

            do_page_out();
            reset_t_page();
        }
    }

    while( t_doc_el_group.first != NULL ) {
        insert_col_main( t_doc_el_group.first );
        t_doc_el_group.first = t_doc_el_group.first->next;
    }

    t_doc_el_group.depth    = 0;
    t_doc_el_group.last     = NULL;
    scan_start = scan_stop + 1;
    return;
}


/***************************************************************************/
/*  prepare address line for output                                        */
/***************************************************************************/

static void prep_aline( text_line * p_line, char * p )
{
    text_chars  *   curr_t;
    uint32_t        h_left;
    uint32_t        h_right;

    h_left = g_page_left + conv_hor_unit( &layout_work.address.left_adjust );
    h_right = g_page_right - conv_hor_unit( &layout_work.address.right_adjust );

    curr_t = alloc_text_chars( p, strlen( p ), g_curr_font );
    curr_t->count = len_to_trail_space( curr_t->text, curr_t->count );

    intrans( curr_t->text, &curr_t->count, g_curr_font );
    curr_t->width = cop_text_width( curr_t->text, curr_t->count, g_curr_font );
    while( curr_t->width > (h_right - h_left) ) {   // too long for line
        if( curr_t->count < 2) {        // sanity check
            break;
        }
        curr_t->count -= 1;             // truncate text
        curr_t->width = cop_text_width( curr_t->text, curr_t->count, g_curr_font );
    }
    p_line->first = curr_t;
    curr_t->x_address = h_left;
    if( layout_work.address.page_position == pos_center ) {
        if( h_left + curr_t->width < h_right ) {
            curr_t->x_address = h_left + (h_right - h_left - curr_t->width) / 2;
        }
    } else if( layout_work.address.page_position == pos_right ) {
        curr_t->x_address = h_right - curr_t->width;
    }

    return;
}


/***************************************************************************/
/*  :ALINE tag                                                             */
/***************************************************************************/

void    gml_aline( const gmltag * entry )
{
    char        *   p;
    doc_element *   cur_el;
    text_line   *   ad_line;

    if( !((ProcFlags.doc_sect == doc_sect_titlep) ||
          (ProcFlags.doc_sect_nxt == doc_sect_titlep)) ) {
        g_err( err_tag_wrong_sect, entry->tagname, ":TITLEP section" );
        err_count++;
        show_include_stack();
    }
    if( !ProcFlags.address_active ) {   // no preceding :ADDRESS tag
        g_err_tag_prec( "ADDRESS" );
    }
    p = scan_start;
    if( *p == '.' ) p++;                // over '.'

    start_doc_sect();               // if not already done
    a_spacing = layout_work.titlep.spacing;
    g_curr_font = layout_work.address.font;
    if( !first_aline ) {

        /************************************************************/
        /*  skip is treated as pre_top_skip because                 */
        /*  it is always used at the top of the page                */
        /*  this is not what the docs say                           */
        /************************************************************/

        set_skip_vars( NULL, &layout_work.aline.skip, NULL, a_spacing, g_curr_font );
    }

    ad_line = alloc_text_line();
    ad_line->line_height = wgml_fonts[g_curr_font].line_height;

    if( *p ) {
        prep_aline( ad_line, p );
    }

    cur_el = alloc_doc_el( el_text );
    cur_el->blank_lines = g_blank_lines;
    g_blank_lines = 0;
    cur_el->depth = ad_line->line_height + g_spacing;
    cur_el->subs_skip = g_subs_skip;
    cur_el->top_skip = g_top_skip;
    cur_el->element.text.overprint = ProcFlags.overprint;
    ProcFlags.overprint = false;
    cur_el->element.text.spacing = g_spacing;
    cur_el->element.text.first = ad_line;
    ProcFlags.skips_valid = false;
    ad_line = NULL;
    insert_col_main( cur_el );

    first_aline = false;
    scan_start = scan_stop + 1;
}

