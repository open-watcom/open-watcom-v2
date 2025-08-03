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
* Description:  WGML tags :ADDRESS and :eADDRESS and :ALINE
*                    with helper functions
*
****************************************************************************/


#include    "wgml.h"


static  bool            first_aline;    // special for first :ALINE
static  font_number     font_save;      // save for font
static  group_type      sav_group_type; // save prior group type
static  page_pos        old_line_pos;   // save prior line position

/***************************************************************************/
/*  :ADDRESS                                                               */
/***************************************************************************/

void gml_address( const gmltag * entry )
{
    if( !((ProcFlags.doc_sect == DSECT_titlep) ||
          (ProcFlags.doc_sect_nxt == DSECT_titlep)) ) {
        xx_nest_err_exit_cc( ERR_TAG_WRONG_SECT, entry->tagname, ":TITLEP section" );
        /* never return */
    }
    first_aline = true;
    font_save = g_curr_font;
    g_curr_font = layout_work.address.font;
    rs_loc = TLOC_address;

    init_nest_cb();
    nest_cb->p_stack = copy_to_nest_stack();
    nest_cb->gtag = entry->u.tagid;
    nest_cb->left_indent = nest_cb->prev->left_indent + conv_hor_unit( &layout_work.address.left_adjust, g_curr_font );
    nest_cb->right_indent = nest_cb->prev->right_indent - conv_hor_unit( &layout_work.address.right_adjust, g_curr_font );

    g_text_spacing = layout_work.titlep.spacing;

    /************************************************************/
    /*  pre_skip is treated as pre_top_skip because             */
    /*  it is always used at the top of the page                */
    /*  this is not what the docs say                           */
    /************************************************************/

    set_skip_vars( NULL, &layout_work.address.pre_skip, NULL, g_text_spacing, g_curr_font );

    old_line_pos = line_position;
    sav_group_type = cur_group_type;
    cur_group_type = GRT_address;
    cur_doc_el_group = alloc_doc_el_group( GRT_address );
    cur_doc_el_group->next = t_doc_el_group;
    t_doc_el_group = cur_doc_el_group;
    cur_doc_el_group = NULL;

    g_scandata.s = g_scandata.e;
    return;
}


/***************************************************************************/
/*  :eADDRESS                                                              */
/***************************************************************************/

void gml_eaddress( const gmltag * entry )
{
    doc_element *   cur_el;
    tag_cb      *   wk;

    (void)entry;

    if( cur_group_type != GRT_address ) {   // no preceding :ADDRESS tag
        g_tag_prec_err_exit( T_ADDRESS );
        /* never return */
    }
    g_curr_font = font_save;
    rs_loc = TLOC_titlep;
    t_page.cur_left = nest_cb->lm;
    t_page.max_width = nest_cb->rm;
    wk = nest_cb;
    nest_cb = nest_cb->prev;
    add_tag_cb_to_pool( wk );

    /* Place the accumulated ALINES on the proper page */

    cur_group_type = sav_group_type;
    if( t_doc_el_group != NULL ) {
        cur_doc_el_group = t_doc_el_group;      // detach current element group
        t_doc_el_group = t_doc_el_group->next;  // processed doc_elements go to next group, if any
        cur_doc_el_group->next = NULL;

        scr_process_break();                    // commit any existing text
        if( first_aline ) {                     // empty ADDRESS block: no ALINEs
            set_skip_vars( NULL, NULL, NULL, g_text_spacing, g_curr_font);
            g_subs_skip = 0;                    // matches wgml 4.0
            t_element = init_doc_el( ELT_text, wgml_fonts[g_curr_font].line_height );
            t_element->element.text.first = alloc_text_line();
            t_element->element.text.first->line_height = wgml_fonts[g_curr_font].line_height;
            t_element->element.text.first->first = NULL;
            insert_col_main( t_element );
            t_element = NULL;
            t_el_last = NULL;
        }

        if( cur_doc_el_group->first != NULL ) {
            cur_doc_el_group->depth += (cur_doc_el_group->first->blank_lines +
                                cur_doc_el_group->first->subs_skip);
        }

        if( (cur_doc_el_group->depth + t_page.cur_depth) > t_page.max_depth ) {

            /*  the block won't fit on this column */

            if( cur_doc_el_group->depth <= t_page.max_depth ) {

                /*  the block will on the next column */

                next_column();
            }
        }

        while( cur_doc_el_group->first != NULL ) {
            cur_el = cur_doc_el_group->first;
            cur_doc_el_group->first = cur_doc_el_group->first->next;
            cur_el->next = NULL;
            insert_col_main( cur_el );
        }

        add_doc_el_group_to_pool( cur_doc_el_group );
        cur_doc_el_group = NULL;
    }

    scr_process_break();                // commit last address line
    line_position = old_line_pos;
    g_scandata.s = g_scandata.e;
    return;
}


/***************************************************************************/
/*  :ALINE tag                                                             */
/***************************************************************************/

void gml_aline( const gmltag * entry )
{
    char        *   p;

    if( !((ProcFlags.doc_sect == DSECT_titlep) ||
          (ProcFlags.doc_sect_nxt == DSECT_titlep)) ) {
        xx_nest_err_exit_cc( ERR_TAG_WRONG_SECT, entry->tagname, ":TITLEP section" );
        /* never return */
    }
    if( cur_group_type != GRT_address ) {    // no preceding :ADDRESS tag
        g_tag_prec_err_exit( T_ADDRESS );
    }
    p = g_scandata.s;
    SkipDot( p );                           // over '.'
    SkipSpaces( p );                        // over WS to <text line>

    scr_process_break();
    start_doc_sect();                       // if not already done

    g_curr_font = layout_work.address.font;
    g_text_spacing = layout_work.titlep.spacing;
    if( !first_aline ) {

        /************************************************************/
        /*  skip is treated as pre_top_skip because                 */
        /*  it is always used at the top of the page                */
        /*  this is not what the docs say                           */
        /************************************************************/

        set_skip_vars( NULL, &layout_work.aline.skip, NULL, g_text_spacing, g_curr_font );
    }

    t_page.cur_left += nest_cb->left_indent;
    t_page.cur_width = t_page.cur_left;
    if( t_page.max_width < -1 * nest_cb->right_indent ) {
        xx_line_err_exit_c( ERR_PAGE_WIDTH_TOO_SMALL, g_scandata.s );
        /* never return */
    }
    t_page.max_width += nest_cb->right_indent;
    ProcFlags.keep_left_margin = true;  // keep special indent
    line_position = layout_work.address.page_position;
    ProcFlags.as_text_line = true;
    if( *p != '\0' ) {
        process_text( p, g_curr_font );
    } else {
        ProcFlags.titlep_starting = true;
    }
    scr_process_break();                // commit address line (or blank line)

    first_aline = false;
    g_scandata.s = g_scandata.e;
}

