/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2008 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  WGML processing :DATE tag
*
****************************************************************************/


#include    "wgml.h"


/***************************************************************************/
/*  :DATE.date   tag                                                       */
/***************************************************************************/

void    gml_date( const gmltag * entry )
{
    char        *   p;
    font_number     font_save;
    int32_t         rc;
    page_pos        old_line_pos;
    symsub      *   dateval;
    uint32_t        left_indent;
    uint32_t        right_indent;

    if( !((ProcFlags.doc_sect == doc_sect_titlep) ||
          (ProcFlags.doc_sect_nxt == doc_sect_titlep)) ) {
        xx_nest_err_cc( err_tag_wrong_sect, entry->tagname, ":TITLEP section" );
    }

    if( ProcFlags.date_tag_seen ) {     // only one DATE tag allowed
        xx_line_err_c( err_2nd_date, buff2 );
    }

    p = scan_start;
    SkipDot( p );                       // over . to docnum
    SkipSpaces( p );                    // over WS to attribute

    if( *p != '\0' ) {                  // date specified
        if( GlobalFlags.firstpass  ) {
            add_symvar( global_dict, "date", p, no_subscript, 0 );
        }
    } else {
        rc = find_symvar( sys_dict, "$date", no_subscript, &dateval );
        p = dateval->value;
    }

    scr_process_break();
    start_doc_sect();                       // if not already done

    font_save = g_curr_font;
    g_curr_font = layout_work.date.font;
    g_text_spacing = layout_work.titlep.spacing;

    /************************************************************/
    /*  pre_skip is treated as pre_top_skip because it is       */
    /*  always used at the top of the page, despite the docs    */
    /************************************************************/

    set_skip_vars( NULL, &layout_work.date.pre_skip, NULL, g_text_spacing, g_curr_font );

    left_indent = conv_hor_unit( &layout_work.date.left_adjust, g_curr_font );
    right_indent = conv_hor_unit( &layout_work.date.right_adjust, g_curr_font );

    t_page.cur_left += left_indent;
    t_page.cur_width = t_page.cur_left;
    if( t_page.max_width < right_indent ) {
        xx_line_err_c( err_page_width_too_small, val_start );
    } else {
        t_page.max_width -= right_indent;
    }
    ProcFlags.keep_left_margin = true;  // keep special indent
    old_line_pos = line_position;
    line_position = layout_work.author.page_position;
    ProcFlags.as_text_line = true;
    if( *p != '\0' ) {
        process_text( p, g_curr_font );
    } else {
        ProcFlags.titlep_starting = true;
    }
    scr_process_break();                // commit date line (or blank line)

    line_position = old_line_pos;
    g_curr_font = font_save;
    ProcFlags.date_tag_seen = true;
    scan_start = scan_stop + 1;
}
