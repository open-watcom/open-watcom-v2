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
* Description:  WGML tag :BINCLUDE processing
*
****************************************************************************/

#include "wgml.h"

#include "clibext.h"

/***************************************************************************/
/*  :BINCLUDE tag                                                          */
/***************************************************************************/

void    gml_binclude( gml_tag gtag )
{
    bool            depth_found             = false;
    bool            file_found              = false;
    bool            has_rec_type            = false;
    bool            reposition;
    bool            reposition_found        = false;
    char            file[FILENAME_MAX];
    char            rt_buff[MAX_FILE_ATTR];
    char        *   p;
    doc_element *   cur_el;
    su              depth_su;
    uint32_t        depth;
    size_t          len;

    if( (ProcFlags.doc_sect < doc_sect_gdoc) ) {
        if( (ProcFlags.doc_sect_nxt < doc_sect_gdoc) ) {
            xx_tag_err( err_tag_before_gdoc, gml_tagname( gtag ) );
            scan_start = scan_stop;
            return;
        }
    }
    len = 0;
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
            len = val_len;
            if( len >= FILENAME_MAX )
                len = FILENAME_MAX - 1;
            memcpy( file, val_start, len );
            file[len] = '\0';
            split_attr_file( file, rt_buff, MAX_FILE_ATTR );
            if( (rt_buff[0] != '\0') ) {
                has_rec_type = true;
                if( rt_buff[0] != 't' ) {
                    xx_warn( wng_rec_type_binclude );
                }
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
            if( att_val_to_su( &depth_su, true ) ) {
                return;
            }
            depth = conv_vert_unit( &depth_su, g_spacing_ln );
            if( ProcFlags.tag_end_found ) {
                break;
            }
        } else if( !strnicmp( "reposition", p, 10 ) ) {
            p += 10;
            p = get_att_value( p );
            if( val_start == NULL ) {
                break;
            }
            reposition_found = true;
            if( !strnicmp( "start", val_start, 5 ) ) {
                reposition = true;  // moving following text down by depth
            } else if( !strnicmp( "end", val_start, 3 ) ) {
                reposition = false; // device at proper position after insertion
            } else {
                xx_line_err( err_inv_att_val, val_start );
                scan_start = scan_stop;
                return;
            }
            if( ProcFlags.tag_end_found ) {
                break;
            }
        } else {    // no match = end-of-tag in wgml 4.0
            ProcFlags.tag_end_found = true;
            break;
        }
    }
    // detect missing required attributes
    if( !depth_found || !file_found || !reposition_found ) {
        xx_err( err_att_missing );
        scan_start = scan_stop;
        return;
    }

    scr_process_break();                // flush existing text
    start_doc_sect();                   // if not already done

    cur_el = alloc_doc_el(  el_binc );
    if( reposition && depth ) {
        cur_el->depth = depth;          // otherwise, it will be "0"
    }
    if( depth > 0 ) {
        set_skip_vars( NULL, NULL, NULL, 1, g_curr_font );
        cur_el->blank_lines = g_blank_lines;
        g_blank_lines = 0;
        cur_el->subs_skip = g_subs_skip;
        cur_el->top_skip = g_top_skip;
    }
    cur_el->element.binc.depth = depth;
    cur_el->element.binc.cur_left = g_cur_h_start;
    cur_el->element.binc.has_rec_type = has_rec_type;
    ProcFlags.skips_valid = false;
    memcpy( cur_el->element.binc.file, file, len + 1 );
    insert_col_main( cur_el );

    scan_start = scan_stop;         // skip following text
}

