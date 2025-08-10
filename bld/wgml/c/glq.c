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
* Description:  WGML tags :LQ :eLQ processing
*
****************************************************************************/


#include    "wgml.h"


/********************************************************************************************/
/* Format: :LQ.<basic document elements>                                                    */
/*                                                                                          */
/* This tag starts a long quotation. WATCOM Script/GML does not surround a long             */
/* quotation with quotes. The long quote is made distinct from the rest of the text by the  */
/* way in which it is formatted. The :elq tag terminates a long quotation.                  */
/*                                                                                          */
/********************************************************************************************/

void gml_lq( const gmltag * entry )
{
    char        *   p;

    start_doc_sect();                   // if not already done
    scr_process_break();

    if( is_ip_tag( nest_cb->gtag ) ) {         // inline phrase not closed
        g_tag_nest_err_exit( nest_cb->gtag );   // end tag expected
        /* never return */
    }

    init_nest_cb();
    nest_cb->p_stack = copy_to_nest_stack();

    nest_cb->gtag = entry->u.tagid;

    nest_cb->left_indent = conv_hor_unit( &layout_work.lq.left_indent, g_curr_font );
    nest_cb->right_indent = -1 * conv_hor_unit( &layout_work.lq.right_indent, g_curr_font );

    nest_cb->font = g_curr_font;
    g_curr_font = layout_work.lq.font;
    g_text_spacing = layout_work.lq.spacing;

    set_skip_vars( &layout_work.lq.pre_skip, NULL, NULL, 1, g_curr_font );

    t_page.cur_left += nest_cb->left_indent;
    t_page.max_width += nest_cb->right_indent;

    ProcFlags.keep_left_margin = true;  // keep special LQ indent

    if( t_page.cur_width > t_page.cur_left ) {
        t_page.cur_left = t_page.cur_width;
    }
    t_page.cur_width = t_page.cur_left;
    ju_x_start = t_page.cur_width;

    p = g_scandata.s;
    SkipSpaces( p );                        // skip spaces
    SkipDot( p );                           // skip tag end
    if( *p != '\0' ) {
        process_text( p, g_curr_font );
    }

    g_scandata.s = g_scandata.e;
}


/****************************************************************************************/
/* Format: :eLQ.                                                                        */
/*                                                                                      */
/* This tag signals the end of a long quote. A corresponding :lq tag must be previously */
/* specified for each :elq tag.                                                         */
/*                                                                                      */
/****************************************************************************************/

void gml_elq( const gmltag * entry )
{
    char    *   p;
    tag_cb  *   wk;

    scr_process_break();

    check_close_tag_err_exit( entry->u.tagid );

    set_skip_vars( NULL, NULL, &layout_work.lq.post_skip, 1, g_curr_font );

    t_page.cur_left = nest_cb->lm;
    t_page.max_width = nest_cb->rm;

    wk = nest_cb;
    nest_cb = nest_cb->prev;
    add_tag_cb_to_pool( wk );
    g_curr_font = nest_cb->font;

    t_page.cur_width = t_page.cur_left;
    g_scan_err = false;
    p = g_scandata.s;
    SkipDot( p );                       // over '.'
    if( *p != '\0' ) {
        ProcFlags.skips_valid = false;
        process_text( p, g_curr_font);  // if text follows
    } else {
        ProcFlags.force_pc = true;
    }

    g_scandata.s = g_scandata.e;
}

