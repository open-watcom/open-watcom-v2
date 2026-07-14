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
* Description:  WGML tags :OL, :SL, :UL :DL, :GL
*                         :eOL, :eSL, :eUL :eDL, :eGL
*                         :LI, :LP
*                         :DD, :DDHD, :DT, :DTHD
*                         :GD, :GT
*
* Note: :LIREF is not yet implemented; it might be better placed
*       with :FIGREF, :FNREF, and :HDREF than here, depending on how it works
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


static  bool        ddhd_done       = false;
static  bool        dl_gl_starting  = false;
static  bool        dl_gl_first     = false;
static  bool        dt_space_set    = false;
static  uint8_t     dl_cur_level    = 1;    // current DL list level
static  uint8_t     gl_cur_level    = 1;    // current GL list level
static  uint8_t     ol_cur_level    = 1;    // current OL list level
static  uint8_t     sl_cur_level    = 1;    // current SL list level
static  uint8_t     ul_cur_level    = 1;    // current UL list level


/***************************************************************************/
/* end_lp  processing as if the non existant :eLP tag was seen             */
/***************************************************************************/
static void end_lp( void )
{
    tag_cb  *   wk;

    if( nest_cb->gtag == T_LP ) {      // terminate current :LP
        wk = nest_cb;
        nest_cb = nest_cb->prev;
        add_tag_cb_to_pool( wk );
    }
}


/***************************************************************************/
/* :SL, ... common processing                                              */
/***************************************************************************/

static void gml_xl_lp_common( g_tags t )
{
    char        *   p;

    if( t != T_LP ) {
        if( is_ip_tag( nest_cb->gtag ) ) {         // inline phrase not closed
            g_tag_nest_err_exit( nest_cb->gtag );   // end tag expected
            /* never return */
        }
    }

    end_lp();                           // terminate :LP if active

    if( ProcFlags.overprint
      && ProcFlags.cc_cp_done ) {
        ProcFlags.overprint = false;    // cancel overprint
    }

    if( ProcFlags.dd_starting ) {
        scr_process_break();
        t_element = alloc_doc_el( ELT_vspace );
        t_element->depth = wgml_fonts[g_curr_font].line_height;
        insert_col_main( t_element );
        t_element = NULL;
        t_el_last = NULL;
        ProcFlags.dd_starting = false;
    }

    init_nest_cb();
    nest_cb->p_stack = copy_to_nest_stack();

    nest_cb->gtag = t;

    g_scan_err = false;
    p = g_scandata.s;
    SkipSpaces( p );                        // skip spaces
    SkipDot( p );                           // skip tag end
    if( t != T_LP ) {                       // text only allowed for :LP
        if( t != T_DL
          && t != T_GL ) {      // DL/GL don't require LI/LP
            ProcFlags.need_li_lp = true;    // :LI or :LP  next
        } else {
            dl_gl_starting = true;
            dl_gl_first = true;
        }
        start_doc_sect();                   // if not already done
        if( g_line_indent == 0 ) {
            ProcFlags.para_starting = false;    // clear for this tag's first break
        }
        scr_process_break();
        if( !ProcFlags.reprocess_line
          && *p != '\0' ) {
            process_text( p, g_curr_font );
        }
    }

    return;
}


/**************************************************************************************************/
/* Format:  :DL [compact]                                                                         */
/*              [break]                                                                           */
/*              [headhi=head-highlight]                                                           */
/*              [termhi=term-highlight]                                                           */
/*              [tsize='hor-space-unit'].                                                         */
/*                                                                                                */
/* The definition list tag signals the start of a definition list. Each list item in a definition */
/* list has two parts. The first part is the definition term and is defined with the :dt tag.     */
/* The second part is the definition description and is defined with the :dd tag. A               */
/* corresponding :edl tag must be specified for each :dl tag.                                     */
/*                                                                                                */
/* The compact attribute indicates that the list items should be compacted. Blank lines           */
/* that are normally placed between the list items will be suppressed. The compact                */
/* attribute is one of the few WATCOM Script/GML attributes which does not have an                */
/* attribute value associated with it.                                                            */
/*                                                                                                */
/* The break attribute indicates that the definition description should be started on a new       */
/* output line if the size of the definition term exceeds the maximum horizontal space            */
/* normally allowed for it. If this attribute is not specified, the definition description will   */
/* be placed after the definition term. The break attribute is one of the few WATCOM              */
/* Script/GML attributes which does not have an attribute value associated with it.               */
/*                                                                                                */
/* The headhi attribute allows you to set the highlighting level of the definition list           */
/* headings. Non-negative integer numbers are valid highlighting values.                          */
/*                                                                                                */
/* The termhi attribute allows you to set the highlighting level of the definition term.          */
/* Non-negative integer numbers are valid highlighting values.                                    */
/*                                                                                                */
/* The tsize attribute allows you to set the minimum horizontal space taken by the                */
/* definition term. Any valid horizontal space unit may be specified. The attribute value         */
/* is linked to the font of the :DT tag if the termhi attribute is not specified (see "Font       */
/* Linkage" on page 77).                                                                          */
/**************************************************************************************************/

void gml_dl( const gmltag * entry )
{
    bool            compact = false;
    bool            dl_break;
    char            *p;
    char            *pa;
    dl_lay_level    *dl_layout;
    font_number     headhi;
    font_number     termhi;
    su              cur_su;
    unsigned        tsize;
    att_name_type   attr_name;
    att_val_type    attr_val;

    if( !ProcFlags.start_section ) {
        start_doc_sect();
    }

    dl_layout = layout_work.dl.first;
    while( (dl_layout != NULL) && (dl_layout->level < dl_cur_level) ) {
        dl_layout = dl_layout->next;
    }

    if( dl_layout == NULL ) {
        internal_err_exit( __FILE__, __LINE__ );
        /* never return */
    }

    if( dl_cur_level < layout_work.dl.max_level ) {
        dl_cur_level++;
    } else {
        dl_cur_level = 1;
    }

    ProcFlags.block_starting = true;    // to catch empty lists

    dl_break = dl_layout->line_break;
    headhi = layout_work.dthd.font;
    termhi = layout_work.dt.font;
    tsize = conv_hor_unit( &dl_layout->align, g_curr_font );

    p = g_scandata.s;
    SkipSpaces( p );                    // over spaces
    if( *p == '.' ) {
        /* already at tag end */
    } else {
        for( ;; ) {
            p = get_tag_att_name( p, &pa, &attr_name );
            if( ProcFlags.reprocess_line )
                break;
            if( ProcFlags.tag_end_found )
                break;
            if( strcmp( "compact", attr_name.attname.t ) == 0 ) {
                compact = true;
            } else if( strcmp( "break", attr_name.attname.t ) == 0 ) {
                dl_break = true;
            } else if( strcmp( "headhi", attr_name.attname.t ) == 0 ) {
                p = get_att_value( p, &attr_val );
                if( attr_val.tok.s == NULL ) {
                    break;
                }
                headhi = get_font_number( attr_val.tok.s, attr_val.tok.l );
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else if( strcmp( "termhi", attr_name.attname.t ) == 0 ) {
                p = get_att_value( p, &attr_val );
                if( attr_val.tok.s == NULL ) {
                    break;
                }
                termhi = get_font_number( attr_val.tok.s, attr_val.tok.l );
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else if( strcmp( "tsize", attr_name.attname.t ) == 0 ) {
                p = get_att_value( p, &attr_val );
                if( attr_val.tok.s == NULL ) {
                    break;
                }
                if( att_val_to_su( &cur_su, true, &attr_val, false ) ) {
                    break;
                }
                tsize = conv_hor_unit( &cur_su, g_curr_font );
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else {
                p = pa; // restore any spaces before non-attribute value
                break;
            }
        }
    }
    g_scandata.s = p;

    gml_xl_lp_common( entry->u.tagid );

    nest_cb->u.dl_layout = dl_layout;
    nest_cb->compact = compact;
    nest_cb->dl_break = dl_break;
    nest_cb->font = g_curr_font;

    nest_cb->li_number = 0;
    nest_cb->align = conv_hor_unit( &nest_cb->u.dl_layout->align, g_curr_font );
    nest_cb->left_indent = conv_hor_unit( &nest_cb->u.dl_layout->left_indent, g_curr_font );
    nest_cb->right_indent = -1 * conv_hor_unit( &nest_cb->u.dl_layout->right_indent, g_curr_font );
    nest_cb->xl_pre_skip = conv_vert_unit( &nest_cb->u.dl_layout->pre_skip, g_text_spacing, g_curr_font );
    nest_cb->headhi = headhi;
    nest_cb->termhi = termhi;
    nest_cb->tsize = tsize;
    nest_cb->in_list = true;

    g_text_spacing = nest_cb->u.dl_layout->spacing;

    ProcFlags.null_value = false;
    g_scandata.s = g_scandata.e;
    return;
}


/***********************************************************************************************/
/* Format:  :GL [compact]                                                                      */
/*              [termhi=term-highlight].                                                       */
/*                                                                                             */
/* The glossary list tag signals the start of a glossary list, and is usually used in the back */
/* material section. Each list item in a glossary list has two parts. The first part is the    */
/* glossary term and is defined with the :gt tag. The second part is the glossary              */
/* description and is defined with the :gd tag. A corresponding :egl tag must be specified     */
/* for each :gl tag.                                                                           */
/*                                                                                             */
/* The compact attribute indicates that the list items should be compacted. Blank lines        */
/* that are normally placed between the list items will be suppressed. The compact             */
/* attribute is one of the few WATCOM Script/GML attributes which does not have an             */
/* attribute value associated with it.                                                         */
/*                                                                                             */
/* The termhi attribute allows you to set the highlighting level of the glossary term.         */
/* Non-negative integer numbers are valid highlighting values.                                 */
/***********************************************************************************************/

void gml_gl( const gmltag * entry )
{
    bool            compact =   false;
    char            *p;
    char            *pa;
    font_number     termhi  =   0;
    att_name_type   attr_name;
    att_val_type    attr_val;

    if( !ProcFlags.start_section ) {
        start_doc_sect();
    }

    ProcFlags.block_starting = true;    // to catch empty lists

    termhi = layout_work.gt.font;

    p = g_scandata.s;
    SkipSpaces( p );                        // over spaces
    if( *p == '.' ) {
        /* already at tag end */
    } else {
        for( ;; ) {
            p = get_tag_att_name( p, &pa, &attr_name );
            if( ProcFlags.reprocess_line )
                break;
            if( ProcFlags.tag_end_found )
                break;
            if( strcmp( "compact", attr_name.attname.t ) == 0 ) {
                compact = true;
            } else if( strcmp( "termhi", attr_name.attname.t ) == 0 ) {
                p = get_att_value( p, &attr_val );
                if( attr_val.tok.s == NULL ) {
                    break;
                }
                termhi = get_font_number( attr_val.tok.s, attr_val.tok.l );
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else {
                p = pa; // restore any spaces before non-attribute value
                break;
            }
        }
    }
    g_scandata.s = p;

    gml_xl_lp_common( entry->u.tagid );

    nest_cb->u.gl_layout = layout_work.gl.first;
    while( (nest_cb->u.gl_layout != NULL) && (nest_cb->u.gl_layout->level < gl_cur_level) ) {
        nest_cb->u.gl_layout = nest_cb->u.gl_layout->next;
    }

    if( nest_cb->u.gl_layout == NULL ) {
        internal_err_exit( __FILE__, __LINE__ );
        /* never return */
    }

    if( gl_cur_level < layout_work.gl.max_level ) {
        gl_cur_level++;
    } else {
        gl_cur_level = 1;
    }

    nest_cb->compact = compact;
    nest_cb->font = g_curr_font;

    nest_cb->li_number = 0;
    nest_cb->align = conv_hor_unit( &nest_cb->u.gl_layout->align, g_curr_font );
    nest_cb->left_indent = conv_hor_unit( &nest_cb->u.gl_layout->left_indent, g_curr_font );
    nest_cb->right_indent = -1 * conv_hor_unit( &nest_cb->u.gl_layout->right_indent, g_curr_font );
    nest_cb->xl_pre_skip = conv_vert_unit( &nest_cb->u.gl_layout->pre_skip, g_text_spacing, g_curr_font );
    nest_cb->tsize = conv_hor_unit( &nest_cb->u.gl_layout->align, g_curr_font );
    nest_cb->termhi = termhi;
    nest_cb->in_list = true;

    g_text_spacing = nest_cb->u.gl_layout->spacing;

    g_scandata.s = g_scandata.e;
    return;
}

/************************************************************************************************/
/* Format: :OL [compact].                                                                       */
/*                                                                                              */
/* This tag signals the start of an ordered list. Items in the list are specified using the :li */
/* tag. The list items are preceded by the number of the list item. The layout determines       */
/* the style of the number.                                                                     */
/*                                                                                              */
/* An ordered list may be used wherever a basic document element is permitted to appear.        */
/* A corresponding :eol tag must be specified for each :ol tag.                                 */
/*                                                                                              */
/* The compact attribute indicates that the list items should be compacted. Blank lines         */
/* that are normally placed between the list items will be suppressed. The compact              */
/* attribute is one of the few WATCOM Script/GML attributes which does not have an              */
/* attribute value associated with it.                                                          */
/************************************************************************************************/

void gml_ol( const gmltag * entry )
{
    bool            compact =   false;
    char            *p;
    char            *pa;
    att_name_type   attr_name;

    if( !ProcFlags.start_section ) {
        start_doc_sect();
    }

    ProcFlags.block_starting = true;    // to catch empty lists

    p = g_scandata.s;
    SkipSpaces( p );                        // over spaces
    if( *p == '.' ) {
        /* already at tag end */
    } else {
        for( ;; ) {
            p = get_tag_att_name( p, &pa, &attr_name );
            if( ProcFlags.reprocess_line )
                break;
            if( ProcFlags.tag_end_found )
                break;
            if( strcmp( "compact", attr_name.attname.t ) == 0 ) {
                compact = true;
            } else {
                p = pa; // restore any spaces before non-attribute value
                break;
            }
        }
    }
    g_scandata.s = p;

    if( ProcFlags.need_li_lp ) {
        xx_nest_err_exit( ERR_NO_LI_LP );
        /* never return */
    }
    gml_xl_lp_common( entry->u.tagid );

    nest_cb->u.ol_layout = layout_work.ol.first;
    while( (nest_cb->u.ol_layout != NULL) && (nest_cb->u.ol_layout->level < ol_cur_level) ) {
        nest_cb->u.ol_layout = nest_cb->u.ol_layout->next;
    }

    if( nest_cb->u.ol_layout == NULL ) {
        internal_err_exit( __FILE__, __LINE__ );
        /* never return */
    }

    if( ol_cur_level < layout_work.ol.max_level ) {
        ol_cur_level++;
    } else {
        ol_cur_level = 1;
    }

    nest_cb->compact = compact;
    nest_cb->font = g_curr_font;
    g_curr_font = nest_cb->u.ol_layout->font;

    nest_cb->li_number = 0;
    nest_cb->align = conv_hor_unit( &nest_cb->u.ol_layout->align, g_curr_font );
    nest_cb->left_indent = conv_hor_unit( &nest_cb->u.ol_layout->left_indent, g_curr_font );
    nest_cb->right_indent = -1 * conv_hor_unit( &nest_cb->u.ol_layout->right_indent, g_curr_font )
                            + nest_cb->right_indent;
    nest_cb->xl_pre_skip = conv_vert_unit( &nest_cb->u.ol_layout->pre_skip, g_text_spacing, g_curr_font );
    nest_cb->in_list = true;

    g_text_spacing = nest_cb->u.ol_layout->spacing;

    g_scandata.s = g_scandata.e;
    return;
}


/***************************************************************************************************/
/* Format:  :SL [compact].                                                                         */
/*                                                                                                 */
/* This tag signals the start of a simple list. Items in the list are specified using the :li tag. */
/*                                                                                                 */
/* A simple list may occur wherever a basic document element is permitted to appear. A             */
/* corresponding :esl tag must be specified for each :sl tag.                                      */
/*                                                                                                 */
/* The compact attribute indicates that the list items should be compacted. Blank lines            */
/* that are normally placed between the list items will be suppressed. The compact                 */
/* attribute is one of the few WATCOM Script/GML attributes which does not have an                 */
/* attribute value associated with it.                                                             */
/***************************************************************************************************/

void gml_sl( const gmltag * entry )
{
    bool            compact =   false;
    char            *p;
    char            *pa;
    att_name_type   attr_name;

    if( !ProcFlags.start_section ) {
        start_doc_sect();
    }

    ProcFlags.block_starting = true;    // to catch empty lists

    p = g_scandata.s;
    SkipSpaces( p );                        // over spaces
    if( *p == '.' ) {
        /* already at tag end */
    } else {
        for( ;; ) {
            p = get_tag_att_name( p, &pa, &attr_name );
            if( ProcFlags.reprocess_line )
                break;
            if( ProcFlags.tag_end_found )
                break;
            if( strcmp( "compact", attr_name.attname.t ) == 0 ) {
                compact = true;
            } else {
                p = pa; // restore any spaces before non-attribute value
                break;
            }
        }
    }
    g_scandata.s = p;

    if( ProcFlags.need_li_lp ) {
        xx_nest_err_exit( ERR_NO_LI_LP );
        /* never return */
    }
    gml_xl_lp_common( entry->u.tagid );

    nest_cb->u.sl_layout = layout_work.sl.first;
    while( (nest_cb->u.sl_layout != NULL) && (nest_cb->u.sl_layout->level < sl_cur_level) ) {
        nest_cb->u.sl_layout = nest_cb->u.sl_layout->next;
    }

    if( nest_cb->u.sl_layout == NULL ) {
        internal_err_exit( __FILE__, __LINE__ );
        /* never return */
    }

    if( sl_cur_level < layout_work.sl.max_level ) {
        sl_cur_level++;
    } else {
        sl_cur_level = 1;
    }

    nest_cb->compact = compact;
    nest_cb->font = g_curr_font;
    g_curr_font = nest_cb->u.sl_layout->font;

    nest_cb->li_number = 0;
    nest_cb->left_indent = conv_hor_unit( &nest_cb->u.sl_layout->left_indent, g_curr_font );
    nest_cb->right_indent = -1 * conv_hor_unit( &nest_cb->u.sl_layout->right_indent, g_curr_font )
                            + nest_cb->right_indent;
    nest_cb->xl_pre_skip = conv_vert_unit( &nest_cb->u.sl_layout->pre_skip, g_text_spacing, g_curr_font );
    nest_cb->in_list = true;

    g_text_spacing = nest_cb->u.sl_layout->spacing;

    g_scandata.s = g_scandata.e;
    return;
}

/**************************************************************************************************/
/* Format:  :UL [compact].                                                                        */
/*                                                                                                */
/* This tag signals the start of an unordered list. Items in the list are specified using the :li */
/* tag. The list items are preceded by a symbol such as an asterisk or a bullet.                  */
/*                                                                                                */
/* This tag may be used wherever a basic document element is permitted to appear. A               */
/* corresponding :eul tag must be specified for each :ul tag.                                     */
/*                                                                                                */
/* The compact attribute indicates that the list items should be compacted. Blank lines           */
/* that are normally placed between the list items will be suppressed. The compact                */
/* attribute is one of the few WATCOM Script/GML attributes which does not have an                */
/* attribute value associated with it.                                                            */
/**************************************************************************************************/

void gml_ul( const gmltag * entry )
{
    bool            compact =   false;
    char            *p;
    char            *pa;
    att_name_type   attr_name;

    if( !ProcFlags.start_section ) {
        start_doc_sect();
    }

    ProcFlags.block_starting = true;    // to catch empty lists

    p = g_scandata.s;
    SkipSpaces( p );                        // over spaces
    if( *p == '.' ) {
        /* already at tag end */
    } else {
        for( ;; ) {
            p = get_tag_att_name( p, &pa, &attr_name );
            if( ProcFlags.reprocess_line )
                break;
            if( ProcFlags.tag_end_found )
                break;
            if( strcmp( "compact", attr_name.attname.t ) == 0 ) {
                compact = true;
            } else {
                p = pa; // restore any spaces before non-attribute value
                break;
            }
        }
    }
    g_scandata.s = p;

    if( ProcFlags.need_li_lp ) {
        xx_nest_err_exit( ERR_NO_LI_LP );
        /* never return */
    }
    gml_xl_lp_common( entry->u.tagid );

    nest_cb->u.ul_layout = layout_work.ul.first;
    while( (nest_cb->u.ul_layout != NULL) && (nest_cb->u.ul_layout->level < ul_cur_level) ) {
        nest_cb->u.ul_layout = nest_cb->u.ul_layout->next;
    }

    if( nest_cb->u.ul_layout == NULL ) {
        internal_err_exit( __FILE__, __LINE__ );
        /* never return */
    }

    if( ul_cur_level < layout_work.ul.max_level ) {
        ul_cur_level++;
    } else {
        ul_cur_level = 1;
    }

    nest_cb->compact = compact;
    nest_cb->font = g_curr_font;
    g_curr_font = nest_cb->u.ul_layout->font;

    nest_cb->li_number = 0;
    nest_cb->align = conv_hor_unit( &nest_cb->u.ul_layout->align, g_curr_font );
    nest_cb->left_indent = conv_hor_unit( &nest_cb->u.ul_layout->left_indent, g_curr_font );
    nest_cb->right_indent = -1 * conv_hor_unit( &nest_cb->u.ul_layout->right_indent, g_curr_font )
                            + nest_cb->right_indent;
    nest_cb->xl_pre_skip = conv_vert_unit( &nest_cb->u.ul_layout->pre_skip, g_text_spacing, g_curr_font );
    nest_cb->in_list = true;

    g_text_spacing = nest_cb->u.ul_layout->spacing;

    g_scandata.s = g_scandata.e;
    return;
}

/***************************************************************************/
/* common :eXXX processing                                                 */
/***************************************************************************/

static bool    gml_exl_entry( const gmltag *entry )
{
    if( g_line_indent == 0 ) {
        ProcFlags.para_starting = false;    // clear for this tag's break
    }
    scr_process_break();
    if( nest_cb->gtag == T_LP ) {           // terminate :LP if active
        end_lp();
    }
    check_close_tag_err_exit( entry->u.tagid );
    return( true );
}

static void     gml_exl_common( const gmltag * entry )
{
    char        *   p;
    font_number     l_font;
    su              l_post_skip;
    tag_cb      *   wk;

    (void)entry;

    t_page.cur_left = nest_cb->lm;
    t_page.max_width = nest_cb->rm;

    switch( nest_cb->gtag ) {
    case T_DL :
        l_post_skip = nest_cb->u.dl_layout->post_skip;
        l_font = layout_work.dd.font;
        break;
    case T_GL :
        l_post_skip = nest_cb->u.gl_layout->post_skip;
        l_font = layout_work.gt.font;
        break;
    case T_OL :
        l_post_skip = nest_cb->u.ol_layout->post_skip;
        l_font = nest_cb->u.ol_layout->font;
        break;
    case T_SL :
        l_post_skip = nest_cb->u.sl_layout->post_skip;
        l_font = nest_cb->u.sl_layout->font;
        break;
    case T_UL :
        l_post_skip = nest_cb->u.ul_layout->post_skip;
        l_font = nest_cb->u.ul_layout->font;
        break;
    default :
        internal_err_exit( __FILE__, __LINE__ ); // not an inline phrase end tag
        /* never return */
    }

    wk = nest_cb;
    nest_cb = nest_cb->prev;
    add_tag_cb_to_pool( wk );
    g_curr_font = nest_cb->font;

    t_page.cur_width = t_page.cur_left;
    g_scan_err = false;
    p = g_scandata.s;
    SkipDot( p );                       // over '.'
    SkipSpaces( p );                    // over WS to <text line>
    if( *p != '\0' ) {
        if( !input_cbs->hidden_head->ip_start && (*(p + 1) == '\0') && (*p == CONT_char) ) { // text is continuation character only
//            if( &l_post_skip != NULL ) {
                g_post_skip = conv_vert_unit( &l_post_skip , g_text_spacing, l_font );
//            } else {
//                g_post_skip = 0;
//            }
        } else {
            do_force_pc( p );
        }
    } else {
        ProcFlags.force_pc = true;
        ProcFlags.skips_valid = false;  // force use of post_skip with following text element
    }

    ProcFlags.need_li_lp = false;       // :LI or :LP no longer needed
    dl_gl_starting = false;
    g_scandata.s = g_scandata.e;
}


/*********************************************************************************************/
/* Format:  :eDL.                                                                            */
/*                                                                                           */
/* This tag signals the end of a definition list. A corresponding :dl tag must be previously */
/* specified for each :edl tag.                                                              */
/*                                                                                           */
/*                                                                                           */
/* Format:  :eGL.                                                                            */
/*                                                                                           */
/* This tag signals the end of a glossary list. A corresponding :gl tag must be previously   */
/* specified for each :egl tag.                                                              */
/*                                                                                           */
/*                                                                                           */
/* Format:  :eOL.                                                                            */
/*                                                                                           */
/* This tag signals the end of an ordered list. A corresponding :ol tag must be previously   */
/* specified for each :eol tag.                                                              */
/*                                                                                           */
/*                                                                                           */
/* Format:  :eSL.                                                                            */
/*                                                                                           */
/* This tag signals the end of a simple list. A corresponding :sl tag must be previously     */
/* specified for each :esl tag.                                                              */
/*                                                                                           */
/*                                                                                           */
/* Format:  :eUL.                                                                            */
/*                                                                                           */
/* This tag signals the end of an unordered list. A corresponding :ul tag must be            */
/* previously specified for each :eul tag.                                                   */
/*********************************************************************************************/

void    gml_edl( const gmltag * entry )
{
    if( gml_exl_entry( entry ) ) {
        set_skip_vars( NULL, NULL, &nest_cb->u.dl_layout->post_skip, 1, g_curr_font );
        gml_exl_common( entry );
        if( dl_cur_level == 1 ) {
            dl_cur_level = layout_work.dl.max_level;
        } else {
            dl_cur_level--;
        }
    }
}

void    gml_egl( const gmltag * entry )
{
    if( gml_exl_entry( entry ) ) {
        set_skip_vars( NULL, NULL, &nest_cb->u.gl_layout->post_skip, 1, g_curr_font );
        gml_exl_common( entry );
        if( gl_cur_level == 1 ) {
            gl_cur_level = layout_work.gl.max_level;
        } else {
            gl_cur_level--;
        }
    }
}

void    gml_eol( const gmltag * entry )
{
    if( gml_exl_entry( entry ) ) {
        set_skip_vars( NULL, NULL, &nest_cb->u.ol_layout->post_skip, 1, g_curr_font );
        gml_exl_common( entry );
        if( ol_cur_level == 1 ) {
            ol_cur_level = layout_work.ol.max_level;
        } else {
            ol_cur_level--;
        }
    }
}

void    gml_esl( const gmltag * entry )
{
    if( gml_exl_entry( entry ) ) {
        set_skip_vars( NULL, NULL, &nest_cb->u.sl_layout->post_skip, 1, g_curr_font );
        gml_exl_common( entry );
        if( sl_cur_level == 1 ) {
            sl_cur_level = layout_work.sl.max_level;
        } else {
            sl_cur_level--;
        }
    }
}

void    gml_eul( const gmltag * entry )
{
    if( gml_exl_entry( entry ) ) {
        set_skip_vars( NULL, NULL, &nest_cb->u.ul_layout->post_skip, 1, g_curr_font );
        gml_exl_common( entry );
        if( ul_cur_level == 1 ) {
            ul_cur_level = layout_work.ul.max_level;
        } else {
            ul_cur_level--;
        }
    }
}


/***************************************************************************/
/* :LI within :OL tag                                                      */
/***************************************************************************/

static  void    gml_li_ol( const gmltag * entry )
{
    char            charnumber[NUM2STR_LENGTH + 2 + 1];
    char        *   p;
    char        *   pn;
    unsigned        num_len;

    (void)entry;

    if( ProcFlags.overprint && ProcFlags.cc_cp_done ) {
        ProcFlags.overprint = false;    // cancel overprint
    }

    scr_process_break();

    g_scan_err = false;
    p = g_scandata.s;

    nest_cb->li_number++;
    pn = format_num( nest_cb->li_number, charnumber, NUM2STR_LENGTH + 1,
                     nest_cb->u.ol_layout->number_style );
    if( pn != NULL ) {
        num_len = strlen( pn );
        *(pn + num_len) = CONT_char;
        *(pn + num_len + 1) = '\0';
        num_len += 2;
    } else {
        pn = charnumber;
        *pn = '?';
        *(pn + 1) = CONT_char;
        *(pn + 2) = '\0';
        num_len = 2;
    }

    g_curr_font = nest_cb->u.ol_layout->number_font;

    if( ProcFlags.need_li_lp ) {        // first :li for this list
        set_skip_vars( &nest_cb->u.ol_layout->pre_skip, NULL, NULL, g_text_spacing, g_curr_font );
    } else if( !nest_cb->compact ) {
        set_skip_vars( &nest_cb->u.ol_layout->skip, NULL, NULL, g_text_spacing, g_curr_font );
    } else {                            // compact
        set_skip_vars( NULL, NULL, NULL, 1, g_curr_font );
    }
    ProcFlags.need_li_lp = false;

    t_page.cur_left = nest_cb->lm + nest_cb->left_indent;
    t_page.max_width = nest_cb->rm + nest_cb->right_indent;
    t_page.cur_width = t_page.cur_left;

    ProcFlags.keep_left_margin = true;  // keep special Note indent
    process_text( charnumber, g_curr_font );    // insert item number
    insert_hard_spaces( " ", 1, FONT0 );
    ProcFlags.zsp = true;

    t_page.cur_left = nest_cb->lm + nest_cb->left_indent + nest_cb->align;   // left start
    if( t_page.cur_width < t_page.cur_left ) {  // set for current line
        t_page.cur_width = t_page.cur_left;
        post_space = 0;
    }
    ju_x_start = t_page.cur_width;

    g_curr_font = nest_cb->u.ol_layout->font;
    SkipDot( p );                       // over '.'
    SkipSpaces( p );                    // skip initial spaces
    if( *p != '\0' ) {
        process_text( p, g_curr_font ); // if text follows
    }

    g_scandata.s = g_scandata.e;
    return;
}


/***************************************************************************/
/* :LI within :SL tag                                                      */
/***************************************************************************/

static  void    gml_li_sl( const gmltag * entry )
{
    char        *   p;

    (void)entry;

    if( ProcFlags.overprint && ProcFlags.cc_cp_done ) {
        ProcFlags.overprint = false;    // cancel overprint
    }

    scr_process_break();

    g_scan_err = false;
    p = g_scandata.s;

    if( ProcFlags.need_li_lp ) {        // first :li for this list
        set_skip_vars( &nest_cb->u.sl_layout->pre_skip, NULL, NULL, g_text_spacing, g_curr_font );
    } else if( !nest_cb->compact ) {
        set_skip_vars( &nest_cb->u.sl_layout->skip, NULL, NULL, g_text_spacing, g_curr_font );
    } else {                            // compact
        set_skip_vars( NULL, NULL, NULL, 1, g_curr_font );
    }
    ProcFlags.need_li_lp = false;

    ProcFlags.keep_left_margin = true;  // keep special Note indent

    t_page.cur_left = nest_cb->lm + nest_cb->left_indent;
    t_page.max_width = nest_cb->rm + nest_cb->right_indent;
    t_page.cur_width = t_page.cur_left;

    post_space = 0;
    ju_x_start = t_page.cur_width;

    g_curr_font = nest_cb->u.sl_layout->font;
    SkipDot( p );                       // over '.'
    SkipSpaces( p );                    // skip initial spaces
    if( *p != '\0' ) {
        process_text( p, g_curr_font ); // if text follows
    }

    g_scandata.s = g_scandata.e;
    return;
}


/***************************************************************************/
/* :LI within :UL tag                                                      */
/***************************************************************************/

static  void    gml_li_ul( const gmltag * entry )
{
    char        *   p;
    char            bullet[3];

    (void)entry;

    if( ProcFlags.overprint && ProcFlags.cc_cp_done ) {
        ProcFlags.overprint = false;    // cancel overprint
    }

    scr_process_break();

    g_scan_err = false;
    p = g_scandata.s;

    if( nest_cb->u.ul_layout->bullet_translate ) {
        bullet[0] = cop_in_trans( nest_cb->u.ul_layout->bullet, nest_cb->u.ul_layout->bullet_font );
    } else {
        bullet[0] = nest_cb->u.ul_layout->bullet;
    }
    bullet[1] = CONT_char;
    bullet[2] = '\0';

    if( ProcFlags.need_li_lp ) {        // first :li for this list
        set_skip_vars( &nest_cb->u.ul_layout->pre_skip, NULL, NULL, g_text_spacing, g_curr_font );
    } else if( !nest_cb->compact ) {
        set_skip_vars( &nest_cb->u.ul_layout->skip, NULL, NULL, g_text_spacing, g_curr_font );
    } else {                            // compact
        set_skip_vars( NULL, NULL, NULL, 1, g_curr_font );
    }
    ProcFlags.need_li_lp = false;

    t_page.cur_left = nest_cb->lm + nest_cb->left_indent;
    t_page.max_width = nest_cb->rm + nest_cb->right_indent;
    t_page.cur_width = t_page.cur_left;

    ProcFlags.keep_left_margin = true;  // keep special Note indent
    g_curr_font = nest_cb->u.ul_layout->bullet_font;
    process_text( bullet, g_curr_font );    // insert bullet
    insert_hard_spaces( " ", 1, FONT0 );
    ProcFlags.zsp = true;

    t_page.cur_left = nest_cb->lm + nest_cb->left_indent + nest_cb->align;   // left start
    if( t_page.cur_width < t_page.cur_left ) {  // set for current line
        t_page.cur_width = t_page.cur_left;
        post_space = 0;
    }

    ju_x_start = t_page.cur_width;

    g_curr_font = nest_cb->u.ul_layout->font;
    SkipDot( p );                       // over '.'
    SkipSpaces( p );                    // skip initial spaces
    if( *p != '\0' ) {
        process_text( p, g_curr_font ); // if text fullows
    }

    t_page.cur_left = nest_cb->lm + nest_cb->left_indent + nest_cb->align;

    g_scandata.s = g_scandata.e;
    return;
}


/*******************************************************************************************/
/* Format: :LI [id='id-name'].<paragraph elements>                                         */
/*                            <basic document elements>                                    */
/*                                                                                         */
/* This tag signals the start of an item in a simple, ordered, or unordered list. The      */
/* unordered list items are preceded by an annotation symbol, such as an asterisk. The     */
/* ordered list items are annotated by an ordered sequence.                                */
/*                                                                                         */
/* The id attribute associates an identifier name with the list item, and may only be used */
/* when the list item is in an ordered list. The identifier name is used when processing a */
/* list item reference, and must be unique within the document.                            */
/*                                                                                         */
/* NOTE: wgml 4.0 produces an error if LI occurs inside a DL or GL                         */
/*******************************************************************************************/

void    gml_li( const gmltag * entry )
{
    if( nest_cb->gtag == T_LP ) {      // terminate :LP if active
        end_lp();
    }

    switch( nest_cb->gtag ) {
    case T_OL :
        gml_li_ol( entry );
        break;
    case T_SL :
        gml_li_sl( entry );
        break;
    case T_UL :
        gml_li_ul( entry );
        break;
    case T_DL :
    case T_GL :
        g_tag_nest_err_exit( nest_cb->gtag ); // end tag expected
        /* never return */
    default:
        break;
    }
    return;
}


/************************************************************************************************/
/* Format: :LP.<paragraph elements>                                                             */
/*                                                                                              */
/* The list part tag is used to insert an explanation into the middle of a list. It may be used */
/* in simple, ordered, unordered, definition and glossary lists.                                */
/************************************************************************************************/

void    gml_lp( const gmltag * entry )
{
    char        *   p;

    g_scan_err = false;
    p = g_scandata.s;

    if( nest_cb->gtag == T_LP ) {          // restore margins saved by prior LP
        t_page.cur_left = nest_cb->lm;
        t_page.max_width = nest_cb->rm;
    } else {
        t_page.cur_left = nest_cb->lm;
        t_page.max_width = nest_cb->rm;
    }

    gml_xl_lp_common( entry->u.tagid );

    nest_cb->font = g_curr_font;
    g_curr_font = layout_work.defaults.font;    // matches wgml 4.0

    if( g_line_indent == 0 ) {
        ProcFlags.para_starting = false;    // clear for this tag's first break
    }
    scr_process_break();

    ProcFlags.block_starting = true;    // to catch empty lists

    nest_cb->compact = false;
    nest_cb->font = g_curr_font;
    g_curr_font = layout_work.defaults.font;    // matches wgml 4.0

    nest_cb->li_number = 0;
    nest_cb->left_indent = conv_hor_unit( &layout_work.lp.left_indent, g_curr_font )
                            + nest_cb->prev->left_indent;
    nest_cb->right_indent = -1 * conv_hor_unit( &layout_work.lp.right_indent, g_curr_font )
                            + nest_cb->prev->right_indent;

    g_text_spacing = layout_work.lp.spacing;

    ProcFlags.keep_left_margin = true;  // keep indent
    post_space = 0;

    set_skip_vars( &layout_work.lp.pre_skip, NULL, &layout_work.lp.post_skip, g_text_spacing, g_curr_font );
    if( ProcFlags.need_li_lp || dl_gl_starting ) {  // :LP first tag in list
        if( nest_cb->prev->xl_pre_skip > g_subs_skip ) {
            g_subs_skip = nest_cb->prev->xl_pre_skip;
        }
    }

    t_page.cur_left = nest_cb->lm + nest_cb->left_indent;   // left start
                                        // possibly indent first line
    g_line_indent = conv_hor_unit( &(layout_work.lp.line_indent), g_curr_font );
    t_page.cur_width = t_page.cur_left + g_line_indent;

    t_page.max_width = nest_cb->rm + nest_cb->right_indent;

    ju_x_start = t_page.cur_width;

    ProcFlags.need_li_lp = false;       // :LI or :LP seen
    ProcFlags.para_starting = true;     // for next break, not this tag's break
    dl_gl_starting = false;

    SkipDot( p );                       // possible tag end
    SkipSpaces( p );                    // skip initial spaces
    if( *p != '\0' ) {
        process_text( p, g_curr_font ); // if text follows
    }

    g_scandata.s = g_scandata.e;
    return;
}


/**********************************************************************************************/
/* Format: :DTHD.<text line>                                                                  */
/*                                                                                            */
/* The definition term heading tag is used to specify a heading for the definition terms of a */
/* definition list. It is always followed by a :ddhd tag, and may only appear in a            */
/* definition list. The heading tag may be used more than once within a single definition     */
/* list.                                                                                      */
/**********************************************************************************************/

void gml_dthd( const gmltag * entry )
{
    char    *   p;
    char    *   pa;

    (void)entry;

    if( ProcFlags.overprint && ProcFlags.cc_cp_done ) {
        ProcFlags.overprint = false;    // cancel overprint
    }

    if( !ProcFlags.start_section ) {
        start_doc_sect();
    }
    scr_process_break();

    p = g_scandata.s;

    if( nest_cb->gtag == T_LP ) {      // terminate :LP if active
        end_lp();
    }

    g_curr_font = nest_cb->headhi;
    t_page.cur_left = nest_cb->lm + nest_cb->left_indent;   // left start
    t_page.cur_width = t_page.cur_left;
    t_page.max_width = nest_cb->rm + nest_cb->right_indent;

    if( dl_gl_starting ) {              // first :dthd for this list
        set_skip_vars( &nest_cb->u.dl_layout->pre_skip, NULL, NULL, g_text_spacing, g_curr_font );
    } else {                            // internal :dthd
        set_skip_vars( &nest_cb->u.dl_layout->skip, NULL, NULL, g_text_spacing, g_curr_font );
    }

    ProcFlags.keep_left_margin = true;  // keep special Note indent
    ju_x_start = t_page.cur_width;

    p = get_text_line( p );
    pa = p + strlen(p);
    if( (pa > p) && (*(pa - 1) != CONT_char) ) { // text exists and does not end with a continue character
        *pa = CONT_char;                        // add continue character to GT text
        *(pa + 1) = '\0';
    }

    if( !ProcFlags.reprocess_line ) {
        SkipSpaces( p );                    // skip initial spaces
        ProcFlags.as_text_line = true;
        if( *p != '\0' ) {
            process_text( p, g_curr_font ); // if text follows
        } else {
            ProcFlags.need_text = true;
        }
        g_scandata.s = g_scandata.e;
    }

    ProcFlags.need_ddhd = true;
    ProcFlags.need_tag = true;
    dl_gl_starting = false;
    dl_gl_first = false;

    return;
}


/*******************************************************************************************/
/* Format: :DDHD.<text line>                                                               */
/*                                                                                         */
/* The definition description heading tag is used to specify a heading for the definition  */
/* description of a definition list. It must be preceded by a corresponding :dthd tag, and */
/* may only appear in a definition list. The heading tag may be used more than once        */
/* within a single definition list.                                                        */
/*                                                                                         */
/* NOTE: the ProcFlags must be cleared to prevent generating an error per tag until        */
/*       DDHD is encountered.                                                              */
/*******************************************************************************************/

void gml_ddhd( const gmltag * entry )
{
    char    *   p;

    (void)entry;

    if( ProcFlags.need_ddhd ) {
        ProcFlags.need_ddhd = false;
    } else if( ProcFlags.need_dd ) {
        xx_err_exit_c( ERR_TAG_EXPECTED, "DD");
        /* never return */
    } else if( ProcFlags.need_gd ) {
        xx_err_exit_c( ERR_TAG_EXPECTED, "GD");
        /* never return */
    } else {
        xx_nest_err_exit_cc( ERR_TAG_PRECEDING_2, "DTHD", "DDHD" );
        /* never return */
    }

    p = g_scandata.s;

    t_page.cur_left = nest_cb->lm + nest_cb->left_indent + nest_cb->tsize;   // left start
    t_page.max_width = nest_cb->rm + nest_cb->right_indent;

    ju_x_start = t_page.cur_width;

    if( (t_page.cur_width + wgml_fonts[g_curr_font].spc_width) < t_page.cur_left ) {  // set for current line
        t_page.cur_width = t_page.cur_left;
        post_space = 0;
        ProcFlags.zsp = true;
    } else {
        post_space = wgml_fonts[layout_work.dthd.font].spc_width;
        ProcFlags.dd_space = true;
    }

    ddhd_done = true;                   // override compact if DT follows

    SkipDot( p );                       // skip tag end
    SkipSpaces( p );                    // skip initial spaces
    if( *p != '\0' ) {
        process_text( p, g_curr_font ); // if text follows
    } else {
        ProcFlags.need_text = true;
    }

    g_scandata.s = g_scandata.e;
    return;
}


/************************************************************************************************/
/* Format: :DT.<text line>                                                                      */
/*                                                                                              */
/* This tag is used to specify the term which is defined for each item in a definition list. It */
/* is always followed by a :dd tag, which specifies the start of the text to define the term,   */
/* and may only appear in a definition list.                                                    */
/************************************************************************************************/

void gml_dt( const gmltag * entry )
{
    char    *   p;
    char    *   pa;

    (void)entry;

    if( ProcFlags.overprint && ProcFlags.cc_cp_done ) {
        ProcFlags.overprint = false;    // cancel overprint
    }

    if( !ProcFlags.start_section ) {
        start_doc_sect();
    }
    scr_process_break();

    p = g_scandata.s;

    if( nest_cb->gtag == T_LP ) {      // terminate :LP if active
        end_lp();
    }

    if( ProcFlags.dd_starting ) {
        scr_process_break();
        t_element = alloc_doc_el( ELT_vspace );
        t_element->depth = wgml_fonts[g_curr_font].line_height;
        insert_col_main( t_element );
        t_element = NULL;
        t_el_last = NULL;
        ProcFlags.dd_starting = false;
    }

    g_curr_font = nest_cb->termhi;
    t_page.cur_left = nest_cb->lm + nest_cb->left_indent;   // left start
    t_page.cur_width = t_page.cur_left;
    t_page.max_width = nest_cb->rm + nest_cb->right_indent;

    if( dl_gl_starting ) {              // first :dd for this list
        set_skip_vars( &nest_cb->u.dl_layout->pre_skip, NULL, NULL, g_text_spacing, g_curr_font );
    } else if( (!nest_cb->compact && !dl_gl_first) || ddhd_done ) {
        set_skip_vars( &nest_cb->u.dl_layout->skip, NULL, NULL, g_text_spacing, g_curr_font );
    } else {                            // compact
        set_skip_vars( NULL, NULL, NULL, 1, g_curr_font );
    }

    ddhd_done = false;                  // cancel override
    ProcFlags.keep_left_margin = true;  // keep special Note indent
    ju_x_start = t_page.cur_width;

    p = get_text_line( p );
    pa = p + strlen(p);
    if( (pa > p) && (*(pa - 1) != CONT_char) ) { // text exists and does not end with a continue character
        *pa = CONT_char;                        // add continue character to GT text
        *(pa + 1) = '\0';
    }

    if( !ProcFlags.reprocess_line ) {
        if( (input_cbs->fmflags & II_macro) && ProcFlags.null_value ) {
            ProcFlags.dt_space = true;
            dt_space_set = true;
        }
        ProcFlags.null_value = false;
        SkipSpaces( p );                    // skip initial spaces
        ProcFlags.as_text_line = true;
        if( *p != '\0' ) {
            process_text( p, g_curr_font ); // if text follows
        } else {
            ProcFlags.need_text = true;
        }
        g_scandata.s = g_scandata.e;
    }

    ProcFlags.need_dd = true;
    ProcFlags.need_tag = true;
    dl_gl_starting = false;
    dl_gl_first = false;

    return;
}


/********************************************************************************************/
/* Format: :DD.<paragraph elements>                                                         */
/*             <basic document elements>                                                    */
/*                                                                                          */
/* This tag signals the start of the text for an item description in a definition list. The */
/* definition description tag must be preceded by a corresponding :dt tag, and may only     */
/* appear in a definition list.                                                             */
/*                                                                                          */
/* NOTE: the ProcFlags must be cleared to prevent generating an error per tag until         */
/*       DD is encountered.                                                                 */
/********************************************************************************************/

void gml_dd( const gmltag * entry )
{
    char        *   p;

    (void)entry;

    if( ProcFlags.need_dd ) {
        ProcFlags.need_dd = false;
    } else if( ProcFlags.need_ddhd ) {
        xx_err_exit_c( ERR_TAG_EXPECTED, "DDHD");
        /* never return */
    } else if( ProcFlags.need_gd ) {
        xx_err_exit_c( ERR_TAG_EXPECTED, "GD");
        /* never return */
    } else {
        xx_nest_err_exit_cc( ERR_TAG_PRECEDING_2, "DT", "DD" );
        /* never return */
    }

    ProcFlags.dd_starting = false;
    p = g_scandata.s;
    g_curr_font = layout_work.dd.font;
    t_page.cur_left = nest_cb->lm + nest_cb->left_indent + nest_cb->tsize;   // left start

    if( !dt_space_set && (input_cbs->fmflags & II_macro) ) {
        ProcFlags.dd_macro = true;
    }
    dt_space_set = false;

    if( (t_page.cur_width + wgml_fonts[g_curr_font].spc_width) < t_page.cur_left ) {  // set for current line
        t_page.cur_width = t_page.cur_left;
        post_space = 0;
        ProcFlags.zsp = true;
    } else if( nest_cb->dl_break ) {
        process_line_full( t_line, ((ProcFlags.justify != JUST_off) &&
                (ProcFlags.justify != JUST_on) && (ProcFlags.justify != JUST_half)) );
        t_line = NULL;              // commit term but as part of same doc_element as definition
        t_page.cur_width = t_page.cur_left;
        post_space = 0;
        ProcFlags.dd_break_done = true;     // move dd text to new line
    } else {                                // cur_width > cur_left and no break
        post_space = wgml_fonts[layout_work.dt.font].spc_width;
        ProcFlags.dd_space = true;
    }
    t_page.max_width = nest_cb->rm + nest_cb->right_indent;

    ju_x_start = t_page.cur_width;

    SkipDot( p );                           // possible tag end
    SkipSpaces( p );                        // skip initial spaces
    if( *p != '\0' ) {
        process_text( p, g_curr_font );     // if text follows
    } else {
        if( nest_cb->dl_break ) {
            ProcFlags.dd_starting = true;   // no text, set flag
        }
    }

    g_scandata.s = g_scandata.e;
    return;
}


/**********************************************************************************************/
/* Format: :GT.<text line>                                                                    */
/*                                                                                            */
/* This tag is used to specify the term which is defined for each item in a glossary list. It */
/* is always followed by a :gd tag, which specifies the start of the text to define the term, */
/* and may only appear in a glossary list.                                                    */
/**********************************************************************************************/

void gml_gt( const gmltag * entry )
{
    char    *   p;
    char    *   pa;

    (void)entry;

    if( ProcFlags.overprint && ProcFlags.cc_cp_done ) {
        ProcFlags.overprint = false;    // cancel overprint
    }

    if( !ProcFlags.start_section ) {
        start_doc_sect();
    }
    scr_process_break();

    p = g_scandata.s;

    if( nest_cb->gtag == T_LP ) {      // terminate :LP if active
        end_lp();
    }

    g_curr_font = nest_cb->termhi;
    t_page.cur_left = nest_cb->lm + nest_cb->left_indent;   // left start
    t_page.cur_width = t_page.cur_left;
    t_page.max_width = nest_cb->rm + nest_cb->right_indent;

    ju_x_start = t_page.cur_width;

    if( dl_gl_starting ) {              // first :gt for this list
        set_skip_vars( &nest_cb->u.gl_layout->pre_skip, NULL, NULL, g_text_spacing, g_curr_font );
    } else if( !nest_cb->compact && !dl_gl_first ) {
        set_skip_vars( &nest_cb->u.gl_layout->skip, NULL, NULL, g_text_spacing, g_curr_font );
    } else {                            // compact
        set_skip_vars( NULL, NULL, NULL, 1, g_curr_font );
    }

    p = get_text_line( p );
    pa = p + strlen(p);
    if( (pa > p) && (*(pa - 1) != CONT_char) ) { // text exists and does not end with a continue character
        *pa = CONT_char;                        // add continue character to GT text
        *(pa + 1) = '\0';
    }

    if( !ProcFlags.reprocess_line ) {
        SkipSpaces( p );                    // skip initial spaces
        ProcFlags.as_text_line = true;
        if( *p != '\0' ) {
            process_text( p, g_curr_font ); // if text follows
        } else {
            ProcFlags.need_text = true;
        }
        g_scandata.s = g_scandata.e;
    }

    ProcFlags.need_gd = true;
    ProcFlags.need_tag = true;
    dl_gl_starting = false;
    dl_gl_first = false;

    return;
}


/**********************************************************************************************/
/* Format: :GD.<paragraph elements>                                                           */
/*             <basic document elements>                                                      */
/*                                                                                            */
/* The glossary description tag signals the start of the text for an item in a glossary list. */
/* The glossary description tag must be preceded by a corresponding :gt tag, and may only     */
/* appear in a glossary list.                                                                 */
/*                                                                                            */
/* NOTE: the ProcFlags must be cleared to prevent generating an error per tag until           */
/*       GD is encountered.                                                                   */
/**********************************************************************************************/

void gml_gd( const gmltag * entry )
{
    char                *p;
    char            delim[3];
    text_chars          *marker;

    (void)entry;

    if( ProcFlags.need_gd ) {
        ProcFlags.need_gd = false;
    } else if( ProcFlags.need_ddhd ) {
        xx_err_exit_c( ERR_TAG_EXPECTED, "DDHD");
        /* never return */
    } else if( ProcFlags.need_dd ) {
        xx_err_exit_c( ERR_TAG_EXPECTED, "DD");
        /* never return */
    } else {
        xx_nest_err_exit_cc( ERR_TAG_PRECEDING_2, "GT", "GD" );
        /* never return */
    }

    p = g_scandata.s;

    ProcFlags.ct = true;
    post_space = 0;
    delim[0] = nest_cb->u.gl_layout->delim;
    delim[1] = CONT_char;
    delim[2] = '\0';
    ProcFlags.concat = true;        // even if was false on entry
    process_text( delim, g_curr_font );

    /* This is from GD processing, hence marker type used */

    if( ProcFlags.wh_device ) {             // Insert a marker
        marker = process_word( NULL, 0, g_curr_font, false );
        marker->f_switch = FSW_from;         // emit marker
        marker->x_address = t_page.cur_width;
        t_line->last->next = marker;
        marker->prev = t_line->last;
        t_line->last = marker;
        marker = process_word( NULL, 0, g_curr_font, false );
        marker->f_switch = FSW_full;         // emit marker
        marker->x_address = t_page.cur_width;
        t_line->last->next = marker;
        marker->prev = t_line->last;
        t_line->last = marker;
        marker = NULL;
    }

    g_curr_font = layout_work.gd.font;
    g_prev_font = g_curr_font;

    t_page.cur_left = nest_cb->lm + nest_cb->left_indent + nest_cb->tsize;   // left start

    post_space = 2 * wgml_fonts[layout_work.gt.font].spc_width;
    if( (t_page.cur_width + post_space) < t_page.cur_left ) {  // set for current line
        t_page.cur_width = t_page.cur_left;
        post_space = 0;
    }

    t_page.max_width = nest_cb->rm + nest_cb->right_indent;

    ju_x_start = t_page.cur_width;

    SkipDot( p );                       // possible tag end
    SkipSpaces( p );                    // skip initial spaces
    if( *p != '\0' ) {
        process_text( p, g_curr_font ); // if text follows
    }

    g_scandata.s = g_scandata.e;
    return;
}

