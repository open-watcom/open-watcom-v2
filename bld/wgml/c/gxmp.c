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
* Description:  WGML tags :XMP and :eXMP and helper functions
*               depth attribute is not supported   TBD
*
*
****************************************************************************/
#include    "wgml.h"
#include    "gvars.h"

static  ju_enum     justify_save;           // for ProcFlags.justify
static  bool        first_xline;            // special for first xmp LINE
static  font_number font_save;              // save for font


/***************************************************************************/
/*      :XMP [depth='vert-space-unit'].                                    */
/*           <paragraph elements>                                          */
/*           <basic document elements>                                     */
/* This tag signals the start of an example.  Each line of source text     */
/* following the example tag is placed in the output document without      */
/* normal text processing.  Spacing between words is preserved, and the    */
/* input text is not right justified.  Input source lines which do not fit */
/* on a line in the output document are split into two lines on a          */
/* character, rather than a word basis.  An example may be used where a    */
/* basic document element is permitted to appear, except within a figure,  */
/* footnote, or example.  A corresponding :exmp tag must be specified for  */
/* each :xmp tag.  If the example does not fit on the current page or      */
/* column, it is forced to the next one.  If the current column is empty,  */
/* the example will be split into two parts.  The depth attribute accepts  */
/* vertical space units as possible values.  The amount of specified       */
/* vertical space is created in the output before any source input text is */
/* processed.  The value of the depth attribute is linked to the current   */
/* font                                                                    */
/*                                                                         */
/***************************************************************************/

extern  void    gml_xmp( gml_tag tag )
{
    char    *   p;

    tag = tag;

    start_doc_sect();
    scr_process_break();
    scan_err = false;
    p = scan_start;
    if( *p == '.' ) {
        /* already at tag end */
    } else {
        p++;
        while( *p == ' ' ) {
            p++;
        }

        if( !strnicmp( "depth=", p, 6 ) ) {
            p += 6;
           /***************************************************************/
           /*  Although unsupported, scan depth='xxx'                     */
           /***************************************************************/
           g_warn( wng_unsupp_att, "depth" );
           wng_count++;
           file_mac_info();

           while( *p && *p != '.' ) {   // ignore all up to tag end
               p++;
           }
        }
    }
    if( ProcFlags.xmp_active ) {        // nested :XMP tag not supported
        g_err_tag_nest( "eXMP" );
        scan_start = scan_stop;
        return;
    }

    /******************************************************************/
    /*  test for XMP within  :ADDRESS, :FIG , :FN                     */
    /******************************************************************/
    if( ProcFlags.address_active ) {
        g_err_tag_x_in_y( "XMP", "ADDRESS" );
        scan_start = scan_stop;
        return;
    } else {
        if( ProcFlags.fig_active ) {
            g_err_tag_x_in_y( "XMP", "FIG" );
            scan_start = scan_stop;
            return;
        } else {
            if( ProcFlags.fn_active ) {
                g_err_tag_x_in_y( "XMP", "FN" );
                scan_start = scan_stop;
                return;
            }
        }
    }

    ProcFlags.xmp_active = true;
    first_xline = true;
    font_save = g_curr_font;
    g_curr_font = layout_work.xmp.font;

    if( nest_cb->c_tag == t_NONE ) {
        g_cur_left = g_page_left + conv_hor_unit( &layout_work.xmp.left_indent );
    } else {
        g_cur_left += conv_hor_unit( &layout_work.xmp.left_indent );
    }
    g_cur_h_start = g_cur_left;
    ProcFlags.keep_left_margin = true;  // keep special indent


    init_nest_cb();
    nest_cb->p_stack = copy_to_nest_stack();
    nest_cb->c_tag = t_XMP;

    spacing = layout_work.xmp.spacing;

    set_skip_vars( NULL, &layout_work.xmp.pre_skip, NULL, spacing, g_curr_font );

    ProcFlags.group_elements = true;

    justify_save = ProcFlags.justify;
    ProcFlags.justify = ju_off;         // TBD

    if( *p == '.' ) p++;                // possible tag end
    if( *p ) {
        process_text( p, g_curr_font ); // if text follows
    }
    scan_start = scan_stop;
    return;
}


/***************************************************************************/
/* Format:  :eXMP.                                                         */
/*                                                                         */
/* This tag signals the end of an example. A corresponding :xmp tag must   */
/* be previously specified for each :exmp tag.                             */
/*                                                                         */
/***************************************************************************/

void    gml_exmp( gml_tag tag )
{
    tag_cb  *   wk;

    tag = tag;
    scr_process_break();
    if( !ProcFlags.xmp_active ) {       // no preceding :XMP tag
        g_err_tag_prec( "XMP" );
        scan_start = scan_stop;
        return;
    }
    g_curr_font = font_save;
    ProcFlags.xmp_active = false;
    ProcFlags.justify = justify_save;
    wk = nest_cb;
    nest_cb = nest_cb->prev;
    add_tag_cb_to_pool( wk );

    /*  place the accumulated xlines on the proper page */

    ProcFlags.group_elements = false;
    if( t_doc_el_group.first != NULL ) {
        t_doc_el_group.depth += (t_doc_el_group.first->blank_lines +
                                t_doc_el_group.first->subs_skip);
    }

    if( (t_doc_el_group.depth + t_page.cur_depth) > t_page.max_depth ) {
        /*  the block won't fit on this page */

        if( t_doc_el_group.depth  <= t_page.max_depth ) {
            /*  the block will be on the next page */

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
    scan_start = scan_stop;
    return;
}
