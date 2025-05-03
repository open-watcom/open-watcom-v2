/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2010 The Open Watcom Contributors. All Rights Reserved.
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


#include "wgml.h"

#include "clibext.h"


/***************************************************************************/
/*  :TITLE tag                                                             */
/***************************************************************************/

void    gml_title( const gmltag * entry )
{
    char        *   p;
    char        *   pa;
    font_number     font_save;
    page_pos        old_line_pos;
    uint32_t        left_indent;
    uint32_t        right_indent;

    if( !((ProcFlags.doc_sect == doc_sect_titlep) ||
          (ProcFlags.doc_sect_nxt == doc_sect_titlep)) ) {
        xx_nest_err_cc( err_tag_wrong_sect, entry->tagname, ":TITLEP section" );
    }

    p = scan_start;

    if( *p == '.' ) {
        /* already at tag end */
    } else {
        for( ;; ) {
            pa = get_att_start( p );
            p = att_start;
            if( ProcFlags.reprocess_line ) {
                break;
            }
            if( !strnicmp( "stitle", p, 6 ) ) {
                p += 6;
                p = get_att_value( p );
                if( val_start == NULL ) {
                    break;
                }
                if( GlobalFlags.firstpass && !ProcFlags.stitle_seen ) {  // first stitle goes into dictionary
                    add_symvar( global_dict, "$stitle", val_start, no_subscript, 0 );
                    ProcFlags.stitle_seen = true;
                }
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else {    // no match = end-of-tag in wgml 4.0
                ProcFlags.tag_end_found = true;
                p = pa; // restore spaces before text
                break;
            }
        }
    }

    SkipDot( p );                       // over . to docnum
    SkipSpaces( p );                    // over WS to title

    if( GlobalFlags.firstpass && !ProcFlags.title_text_seen && *p != '\0' ) {   // first title goes into dictionary
        add_symvar( global_dict, "$title", p, no_subscript, 0 );
        ProcFlags.title_text_seen = true;
    }

    scr_process_break();
    start_doc_sect();                       // if not already done

    font_save = g_curr_font;
    g_curr_font = layout_work.title.font;
    g_text_spacing = layout_work.titlep.spacing;

    if( !ProcFlags.title_tag_top ) {    // first TITLE tag
        set_skip_vars( NULL, &layout_work.title.pre_top_skip, NULL, g_text_spacing, g_curr_font );
        ProcFlags.title_tag_top = true;
    } else {
        set_skip_vars( &layout_work.title.skip, NULL, NULL, g_text_spacing, g_curr_font );
    }

    left_indent = conv_hor_unit( &layout_work.title.left_adjust, g_curr_font );
    right_indent = conv_hor_unit( &layout_work.title.right_adjust, g_curr_font );

    t_page.cur_left += left_indent;
    t_page.cur_width = t_page.cur_left;
    if( t_page.max_width < right_indent ) {
        xx_line_err_c( err_page_width_too_small, val_start );
    } else {
        t_page.max_width -= right_indent;
    }
    ProcFlags.keep_left_margin = true;  // keep special indent
    old_line_pos = line_position;
    line_position = layout_work.title.page_position;
    ProcFlags.as_text_line = true;
    if( *p != '\0' ) {
        process_text( p, g_curr_font );
    } else {
        ProcFlags.titlep_starting = true;
    }
    scr_process_break();                // commit title line (or blank line)

    g_curr_font = font_save;
    line_position = old_line_pos;
    scan_start = scan_stop + 1;
}
