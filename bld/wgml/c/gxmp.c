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
* Description:  WGML tags :XMP and :eXMP and helper functions
*               depth attribute is not supported   TBD
*
*
****************************************************************************/
#include    "wgml.h"
#include    "gvars.h"

static  bool    concat_save;            // for ProcFlags.concat
static  ju_enum justify_save;           // for ProcFlags.justify
static  bool    first_xline;            // special for first xmp LINE
static  int8_t  a_spacing;              // spacing between adr lines
static  int8_t  font_save;              // save for font


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

extern  void    gml_xmp( const gmltag * entry )
{
    char    *   p;

    scan_err = false;
    p = scan_start;
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

       while( *p && *p != '.' ) {       // ignore all up to tag end
           p++;
       }
    }
    if( ProcFlags.xmp_active ) {        // nested :XMP tag not supported
        g_err_tag_nest( "eXMP" );
        scan_start = scan_stop + 1;
        return;
    }

    /******************************************************************/
    /*  test for XMP within  :ADDRESS, :FIG , :FN                     */
    /******************************************************************/
    if( ProcFlags.address_active ) {
        g_err_tag_x_in_y( "XMP", "ADDRESS" );
        scan_start = scan_stop + 1;
        return;
    } else {
        if( ProcFlags.fig_active ) {
            g_err_tag_x_in_y( "XMP", "FIG" );
            scan_start = scan_stop + 1;
            return;
        } else {
            if( ProcFlags.fn_active ) {
                g_err_tag_x_in_y( "XMP", "FN" );
                scan_start = scan_stop + 1;
                return;
            }
        }
    }

    ProcFlags.xmp_active = true;
    first_xline = true;
    font_save = g_curr_font_num;
    g_curr_font_num = layout_work.xmp.font;
//  rs_loc = xmp_tag;

    init_nest_cb();
    nest_cb->p_stack = copy_to_nest_stack();
    nest_cb->c_tag = t_XMP;

    spacing = layout_work.xmp.spacing;

    set_skip_vars( NULL, &layout_work.xmp.pre_skip, NULL, spacing,
                       g_curr_font_num );

    ProcFlags.group_elements = true;

    concat_save = ProcFlags.concat;
    justify_save = ProcFlags.justify;
    ProcFlags.concat = false;           // TBD
    ProcFlags.justify = ju_off;         // TBD

    if( *p == '.' ) p++;                // over '.'
    while( *p == ' ' ) p++;             // skip initial spaces
    if( *p == '.' ) p++;                    // possible tag end
    if( *p ) {
        process_text( p, g_curr_font_num ); // if text follows
    }
    scan_start = scan_stop + 1;
    return;
}


/***************************************************************************/
/* Format:  :eXMP.                                                         */
/*                                                                         */
/* This tag signals the end of an example. A corresponding :xmp tag must   */
/* be previously specified for each :exmp tag.                             */
/*                                                                         */
/***************************************************************************/

void    gml_exmp( const gmltag * entry )
{
    tag_cb  *   wk;

    if( !ProcFlags.xmp_active ) {       // no preceding :XMP tag
        g_err_tag_prec( "XMP" );
        scan_start = scan_stop + 1;
        return;
    }
    g_curr_font_num = font_save;
    ProcFlags.xmp_active = false;
    ProcFlags.concat = concat_save;
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
    scan_start = scan_stop + 1;
    return;
}


/***************************************************************************/
/*  prepare address line for output                                        */
/***************************************************************************/

static void prep_xline( text_line * p_line, char * p )
{
    text_chars  *   curr_t;
    uint32_t        h_left;
    uint32_t        h_right;

    h_left = g_page_left + conv_hor_unit( &layout_work.xmp.left_indent );
    h_right = g_page_right - conv_hor_unit( &layout_work.xmp.right_indent );

    curr_t = alloc_text_chars( p, strlen( p ), g_curr_font_num );
    curr_t->count = len_to_trail_space( curr_t->text, curr_t->count );

    intrans( curr_t->text, &curr_t->count, g_curr_font_num );
    curr_t->width = cop_text_width( curr_t->text, curr_t->count,
                                    g_curr_font_num );
    while( curr_t->width > (h_right - h_left) ) {   // too long for line
        if( curr_t->count < 2) {        // sanity check
            break;
        }
        curr_t->count -= 1;             // truncate text
        curr_t->width = cop_text_width( curr_t->text, curr_t->count,
                                        g_curr_font_num );
    }
    p_line->first = curr_t;
    curr_t->x_address = h_left;
#if 0
    if( layout_work.address.page_position == pos_center ) {
        if( h_left + curr_t->width < h_right ) {
            curr_t->x_address = h_left + (h_right - h_left - curr_t->width) / 2;
        }
    } else if( layout_work.address.page_position == pos_right ) {
        curr_t->x_address = h_right - curr_t->width;
    }
#endif
    return;
}


/***************************************************************************/
/*  process text line in :xmp                                              */
/***************************************************************************/

void    xmp_xline( const gmltag * entry )
{
    char        *   p;
    doc_element *   cur_el;
    text_line   *   ad_line;

    p = scan_start;
    if( *p == '.' ) p++;                // over '.'

    a_spacing = layout_work.xmp.spacing;
    g_curr_font_num = layout_work.xmp.font;
    if( !first_xline ) {
        set_skip_vars( NULL, NULL, NULL, a_spacing,
                       g_curr_font_num );
    } else {
        first_xline = false;
    }
    ad_line = alloc_text_line();
    ad_line->line_height = wgml_fonts[g_curr_font_num].line_height;

    if( *p ) {
        prep_xline( ad_line, p );
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
    ad_line = NULL;
    insert_col_main( cur_el );

    scan_start = scan_stop + 1;
}

