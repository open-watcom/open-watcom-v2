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
* Description:  WGML tag :BINCLUDE processing
*
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


/***************************************************************************/
/*  :BINCLUDE tag                                                          */
/***************************************************************************/

void    gml_binclude( const gmltag * entry )
{
    bool            has_rec_type            = false;
    bool            reposition;
    char            file[_MAX_PATH];
    char            rt_buff[MAX_FILE_ATTR];
    char        *   p;
    char        *   pa;
    doc_element *   cur_el;
    inputcb     *   cb                      = input_cbs;
    su              depth_su;
    uint32_t        depth;
    FILE            *fp;

    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags
    if( (ProcFlags.doc_sect < doc_sect_gdoc) ) {
        if( (ProcFlags.doc_sect_nxt < doc_sect_gdoc) ) {
            xx_err_c( err_tag_before_gdoc, entry->tagname );
        }
    }

    scr_process_break();                // flush existing text
    start_doc_sect();                   // if not already done

    file[0] = '\0';
    rt_buff[0] = '\0';
    p = scan_start;
    if( *p == '.' ) {
        /* already at tag end */
    } else {
        for( ;; ) {
            pa = get_attribute( p );
            p = g_att_val.att_name;
            if( ProcFlags.reprocess_line ) {
                break;
            }
            if( strnicmp( "file", p, 4 ) == 0 ) {
                p += 4;
                p = get_value( p );
                if( AttrFlags.file ) {
                    xx_line_err_ci( err_att_dup, g_att_val.att_name,
                        g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                }
                AttrFlags.file = true;
                if( g_att_val.val_name == NULL ) {
                    break;
                }
                if( g_att_val.val_len > _MAX_PATH - 1 )
                    g_att_val.val_len = _MAX_PATH - 1;
                strncpy( file, g_att_val.val_name, g_att_val.val_len );
                file[g_att_val.val_len] = '\0';
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
            } else if( strnicmp( "depth", p, 5 ) == 0 ) {
                p += 5;
                p = get_value( p );
                if( AttrFlags.depth ) {
                    xx_line_err_ci( err_att_dup, g_att_val.att_name,
                        g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                }
                AttrFlags.depth = true;
                if( g_att_val.val_name == NULL ) {
                    break;
                }
                if( value_to_su( &depth_su, true ) ) {
                    break;
                }
                depth = conv_vert_unit( &depth_su, g_text_spacing, g_curr_font );
                if( depth > t_page.max_depth ) {
                    xx_line_err_c( err_inv_depth_binclude, g_att_val.val_name );
                }
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else if( strnicmp( "reposition", p, 10 ) == 0 ) {
                p += 10;
                p = get_value( p );
                if( AttrFlags.reposition ) {
                    xx_line_err_ci( err_att_dup, g_att_val.att_name,
                        g_att_val.val_name - g_att_val.att_name + g_att_val.val_len);
                }
                AttrFlags.reposition = true;
                if( g_att_val.val_name == NULL ) {
                    break;
                }
                if( strnicmp( "start", g_att_val.val_name, 5 ) == 0 ) {
                    reposition = true;  // moving following text down by depth
                } else if( strnicmp( "end", g_att_val.val_name, 3 ) == 0 ) {
                    reposition = false; // device at proper position after insertion
                } else {
                    xx_line_err_c( err_inv_att_val, g_att_val.val_name );
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

    // detect missing required attributes
    if( !AttrFlags.depth || !AttrFlags.file || !AttrFlags.reposition ) {
        xx_err( err_att_missing );
    }

    // only set up the doc_element if the file exists
    fp = search_file_in_dirs( file, "", "", ds_doc_spec );
    if( fp != NULL ) {
        if( depth == 0 ) {
            cur_el = alloc_doc_el(  el_binc );
        } else {
            set_skip_vars( NULL, NULL, NULL, 1, g_curr_font );
            if( reposition && depth ) {                 // otherwise, element depth will be "0"
                cur_el = init_doc_el( el_binc, depth );
            } else {
                cur_el = init_doc_el( el_binc, 0 );
            }
        }
        cur_el->element.binc.depth = depth;
        cur_el->element.binc.cur_left = t_page.cur_width;
        cur_el->element.binc.force_FONT0 = false;
        cur_el->element.binc.has_rec_type = has_rec_type;
        cur_el->element.binc.fp = fp;
        cur_el->element.binc.file = mem_strdup( try_file_name );

        if( GlobalFlags.inclist ) {
            g_info_lm( inf_curr_file, cur_el->element.binc.file );
            while( cb->fmflags & II_macro ) {                 // find prior file
                 cb = cb->prev;
            }
            g_info_lm( inf_curr_file, cb->s.f->filename );
        }

        insert_col_main( cur_el );

    } else {
        xx_err_c( err_file_not_found, file );
    }

    scan_start = scan_stop + 1;         // skip following text
    return;
}

