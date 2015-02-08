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
* Description:  WGML tag :GRAPHIC processing
*
****************************************************************************/
#define __STDC_WANT_LIB_EXT1__  1
#include <string.h>
#include    "wgml.h"
//#include    "findfile.h"
#include    "gvars.h"

/***************************************************************************/
/*  :GRAPHIC tag                                                           */
/***************************************************************************/

void    gml_graphic( const gmltag * entry )
{
    bool            depth_found             = false;
    bool            file_found              = false;
    char            file[FILENAME_MAX];
    char            rt_buff[MAX_FILE_ATTR];
    char        *   p;
    char        *   pa;
    doc_element *   cur_el;
    su              cur_su;
    uint32_t        depth;
    uint32_t        scale                   = 100;
    // the initial value of width is only correct for one-column pages.
    uint32_t    width                   = g_net_page_width;
    int32_t     xoff                    = 0;
    int32_t     yoff                    = 0;

    if( (ProcFlags.doc_sect < doc_sect_gdoc) ) {
        if( (ProcFlags.doc_sect_nxt < doc_sect_gdoc) ) {
            xx_tag_err( err_tag_before_gdoc, entry->tagname );
            scan_start = scan_stop + 1;
            return;
        }
    }
    file[0] = '\0';
    rt_buff[0] = '\0';
    p = scan_start;
    for( ;; ) {
        while( *p == ' ' ) {            // over WS to attribute
            p++;
        }
        if( *p == '\0' ) {              // end of line: get new line
            if( !(input_cbs->fmflags & II_eof) ) {
                if( get_line( true ) ) {      // next line for missing attribute
 
                    process_line();
                    scan_start = buff2;
                    scan_stop  = buff2 + buff2_lg - 1;
                    if( (*scan_start == SCR_char) ||    // cw found: end-of-tag
                        (*scan_start == GML_char) ) {   // tag found: end-of-tag
                        ProcFlags.tag_end_found = true; 
                        break;          
                    } else {
                        p = scan_start; // new line is part of current tag
                        continue;
                    }
                }
            }
        }
        if( !strnicmp( "file", p, 4 ) ) {
            p += 4;
            p = get_att_value( p );
            if( val_start == NULL ) {
                break;
            }
            file_found = true;
            memcpy_s( file, FILENAME_MAX, val_start, val_len );
            if( val_len < FILENAME_MAX ) {
                file[val_len] = '\0';
            } else {
                file[FILENAME_MAX - 1] = '\0';
            }
            split_attr_file( file, rt_buff, sizeof( rt_buff ) );
            if( (rt_buff[0] != '\0') ) {
                xx_warn( wng_rec_type_graphic );
            }
            if( ProcFlags.tag_end_found ) {
                break;
            }
        } else if( !strnicmp( "depth", p, 5 ) ) {
            p += 5;
            p = get_att_value( p );
            if( val_start == NULL ) {
                break;
            }
            depth_found = true;
            pa = val_start;
            if( att_val_to_su( &cur_su, true ) ) {
                return;
            }
            depth = conv_vert_unit( &cur_su, spacing );
            if( depth == 0 ) {
                xx_line_err( err_inv_depth_graphic, pa );
                scan_start = scan_stop + 1;
                return;
            }
            if( ProcFlags.tag_end_found ) {
                break;
            }
        } else if( !strnicmp( "width", p, 5 ) ) {
            p += 5;
            p = get_att_value( p );
            if( val_start == NULL ) {
                break;
            }
            if( !strnicmp( "page", val_start, 4 ) ) {
                // default value is the correct value to use
            } else if( !strnicmp( "column", val_start, 6 ) ) {
                // default value is the correct value to use
            } else {    // value actually specifies the width
                pa = val_start;
                if( att_val_to_su( &cur_su, true ) ) {
                    return;
                }
                width = conv_hor_unit( &cur_su );
                if( width == 0 ) {
                    xx_line_err( err_inv_width_graphic, pa );
                    scan_start = scan_stop + 1;
                    return;
                }
                /* there should be a check somewhere for width > page width */
            }
            if( ProcFlags.tag_end_found ) {
                break;
            }
        } else if( !strnicmp( "scale", p, 5 ) ) {
            p += 5;
            p = get_att_value( p );
            if( val_start == NULL ) {
                break;
            }
            pa = val_start;
            if( (*pa == '+') || (*pa == '-') ) {  // signs not allowed
                xx_line_err( err_num_too_large, pa );
                scan_start = scan_stop + 1;
                return;
            }
            scale = 0;
            while( (*pa >= '0') && (*pa <= '9') ) { // convert to number
                scale = (10 * scale) + (*pa - '0');
                pa++;
                if( (pa - val_start) > val_len ) {  // value end reached
                    break;
                }
            }
            if( scale > 0x7fffffff ) {              // wgml 4.0 limit
                xx_line_err( err_num_too_large, val_start );
                scan_start = scan_stop + 1;
                return;
            }
            if( (pa - val_start) < val_len ) {      // value continues on
                xx_line_err( err_num_too_large, val_start );
                scan_start = scan_stop + 1;
                return;
            }
            if( ProcFlags.tag_end_found ) {
                break;
            }
        } else if( !strnicmp( "xoff", p, 4 ) ) {
            p += 4;
            p = get_att_value( p );
            if( val_start == NULL ) {
                break;
            }
            if( att_val_to_su( &cur_su, false ) ) {
                return;
            }
            xoff = conv_hor_unit( &cur_su );
            if( ProcFlags.tag_end_found ) {
                break;
            }
        } else if( !strnicmp( "yoff", p, 4 ) ) {
            p += 4;
            p = get_att_value( p );
            if( val_start == NULL ) {
                break;
            }
            if( att_val_to_su( &cur_su, false ) ) {
                return;
            }
            yoff = conv_vert_unit( &cur_su, spacing );
            if( ProcFlags.tag_end_found ) {
                break;
            }
        } else {    // no match = end-of-tag in wgml 4.0
            ProcFlags.tag_end_found = true;
            break;
        }
    }
    if( !depth_found || !file_found ) { // detect missing required attributes
        xx_err( err_att_missing );
        scan_start = scan_stop + 1;
        return;
    }

    scr_process_break();                // flush existing text
    start_doc_sect();                   // if not already done

    cur_el = alloc_doc_el(  el_graph );
    cur_el->depth = depth;              // always used with GRAPHIC
    if( !ProcFlags.ps_device ) {        // character devices ignore SK & post_skip
        g_skip = 0;
        g_post_skip = 0;
    }
    set_skip_vars( NULL, NULL, NULL, 1, g_curr_font );
    cur_el->blank_lines = g_blank_lines;
    g_blank_lines = 0;
    cur_el->subs_skip = g_subs_skip;
    g_subs_skip = 0;
    cur_el->top_skip = g_top_skip;
    g_top_skip = 0;
    cur_el->element.graph.cur_left = g_cur_h_start;
    cur_el->element.graph.depth = depth;
    cur_el->element.graph.scale = scale;
    cur_el->element.graph.width = width;
    cur_el->element.graph.xoff = xoff;
    cur_el->element.graph.yoff = yoff;
    ProcFlags.skips_valid = false;
    strncpy_s( cur_el->element.graph.file, FILENAME_MAX, file, FILENAME_MAX );

    insert_col_main( cur_el );

    if( *p == '.' ) {
        p++;
    }
    scan_start = p;                 // process following text    
}

