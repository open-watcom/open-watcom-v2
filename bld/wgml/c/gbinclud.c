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
    char            rt_buff[MAX_FILE_ATTR + 1];
    char            *p;
    char            *pa;
    doc_element     *cur_el;
    inputcb         *cb                      = input_cbs;
    su              depth_su;
    unsigned        depth;
    FILE            *fp;
    att_name_type   attr_name;
    att_val_type    attr_val;
    struct {
        unsigned    file        :1;
        unsigned    depth       :1;
        unsigned    reposition  :1;
    } AttrFlags;

    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags
    if( (ProcFlags.doc_sect < DSECT_gdoc) ) {
        if( (ProcFlags.doc_sect_nxt < DSECT_gdoc) ) {
            xx_err_exit_c( ERR_TAG_BEFORE_GDOC, entry->tagname );
            /* never return */
        }
    }

    scr_process_break();                // flush existing text
    start_doc_sect();                   // if not already done

    file[0] = '\0';
    rt_buff[0] = '\0';
    depth = 0;
    p = g_scandata.s;
    if( *p == '.' ) {
        /* already at tag end */
    } else {
        for( ;; ) {
            p = get_tag_att_name( p, &pa, &attr_name );
            if( ProcFlags.reprocess_line )
                break;
            if( ProcFlags.tag_end_found )
                break;
            if( strcmp( "file", attr_name.attname.t ) == 0 ) {
                p = get_att_value( p, &attr_val );
                if( AttrFlags.file ) {
                    xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                        attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                    /* never return */
                }
                AttrFlags.file = true;
                if( attr_val.tok.s == NULL ) {
                    break;
                }
                if( attr_val.tok.l > _MAX_PATH - 1 )
                    attr_val.tok.l = _MAX_PATH - 1;
                strncpy( file, attr_val.tok.s, attr_val.tok.l );
                file[attr_val.tok.l] = '\0';
                split_attr_file( file, rt_buff, sizeof( rt_buff ) - 1 );
                if( (rt_buff[0] != '\0') ) {
                    has_rec_type = true;
                    if( rt_buff[0] != 't' ) {
                        xx_warn( WNG_REC_TYPE_BINCLUDE );
                    }
                }
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else if( strcmp( "depth", attr_name.attname.t ) == 0 ) {
                p = get_att_value( p, &attr_val );
                if( AttrFlags.depth ) {
                    xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                        attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                    /* never return */
                }
                AttrFlags.depth = true;
                if( attr_val.tok.s == NULL ) {
                    break;
                }
                if( att_val_to_su( &depth_su, true, &attr_val, false ) ) {
                    break;
                }
                depth = conv_vert_unit( &depth_su, g_text_spacing, g_curr_font );
                if( depth > t_page.max_depth ) {
                    xx_line_err_exit_c( ERR_INV_DEPTH_BINCLUDE, attr_val.tok.s );
                    /* never return */
                }
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else if( strcmp( "reposition", attr_name.attname.t ) == 0 ) {
                p = get_att_value( p, &attr_val );
                if( AttrFlags.reposition ) {
                    xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                        attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                    /* never return */
                }
                AttrFlags.reposition = true;
                if( attr_val.tok.s == NULL ) {
                    break;
                }
                if( strcmp( "start", attr_val.specval ) == 0 ) {
                    reposition = true;  // moving following text down by depth
                } else if( strcmp( "end", attr_val.specval ) == 0 ) {
                    reposition = false; // device at proper position after insertion
                } else {
                    xx_line_err_exit_c( ERR_INV_ATT_VAL, attr_val.tok.s );
                    /* never return */
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
    if( !AttrFlags.depth
      || !AttrFlags.file
      || !AttrFlags.reposition ) {
        xx_err_exit( ERR_ATT_MISSING );
        /* never return */
    }

    // only set up the doc_element if the file exists
    fp = search_file_in_dirs( file, "", "", DSEQ_doc_spec );
    if( fp != NULL ) {
        if( depth == 0 ) {
            cur_el = alloc_doc_el( ELT_binc );
        } else {
            set_skip_vars( NULL, NULL, NULL, 1, g_curr_font );
            if( reposition
              && depth ) {              // otherwise, element depth will be "0"
                cur_el = init_doc_el( ELT_binc, depth );
            } else {
                cur_el = init_doc_el( ELT_binc, 0 );
            }
        }
        cur_el->element.binc.depth = depth;
        cur_el->element.binc.cur_left = t_page.cur_width;
        cur_el->element.binc.force_FONT0 = false;
        cur_el->element.binc.has_rec_type = has_rec_type;
        cur_el->element.binc.fp = fp;
        cur_el->element.binc.file = mem_strdup( try_file_name );

        if( GlobalFlags.inclist ) {
            g_info_lm( INF_CURR_FILE, cur_el->element.binc.file );
            while( cb->fmflags & II_macro ) {   // find prior file
                 cb = cb->prev;
            }
            g_info_lm( INF_CURR_FILE, cb->s.f->filename );
        }

        insert_col_main( cur_el );

    } else {
        xx_err_exit_c( ERR_FILE_NOT_FOUND, file );
        /* never return */
    }

    g_scandata.s = g_scandata.e;         // skip following text
    return;
}

