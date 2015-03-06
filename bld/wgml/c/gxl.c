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
* Description:  WGML tags :OL, :SL, :UL  :DL, :GL  and corresponding
*                         :eXX processing
*                         :LI and :LP processing
*               only some of the attributes are supported   TBD
****************************************************************************/

#include "wgml.h"

#include "clibext.h"

/***************************************************************************/
/*  end_lp  processing as if the non existant :eLP tag was seen            */
/***************************************************************************/
static  void    end_lp( void )
{
    tag_cb  *   wk;

    if( nest_cb->c_tag == t_LP ) {      // terminate current :LP
        wk = nest_cb;
        nest_cb = nest_cb->prev;
        add_tag_cb_to_pool( wk );
    }
}


/***************************************************************************/
/*  :SL, ... common processing                                             */
/***************************************************************************/

static void gml_xl_lp_common( gml_tag gtag, e_tags t )
{
    char        *   p;

    gtag = gtag;

    end_lp();                           // terminate :LP if active

    init_nest_cb();
    nest_cb->p_stack = copy_to_nest_stack();

    nest_cb->c_tag = t;

    scan_err = false;
    p = scan_start;
    if( *p == '.' ) p++;                    // possible tag end
    if( t != t_LP ) {                       // text only allowed for :LP
        if( t != t_DL && t != t_GL ) {      // DL/GL don't require LI/LP
            ProcFlags.need_li_lp = true;    // :LI or :LP  next
        }
        start_doc_sect();                   // if not already done
        scr_process_break();
        if( *p ) {
            process_text( p, g_curr_font );
        }
    }
    return;
}



/***************************************************************************/
/* Format:  :DL [compact]                                                  */
/*              [break]                                                    */
/*              [headhi=head-highlight]                                    */
/*              [termhi=term-highlight]                                    */
/*              [tsize='hor-space-unit'].                                  */
/*                                                                         */
/* The definition list tag signals the start of a definition list.  Each   */
/* list item in a definition list has two parts.  The first part is the    */
/* definition term and is defined with the :dt tag.  The second part is    */
/* the definition description and is defined with the :dd tag.  A          */
/* corresponding :edl tag must be specified for each :dl tag.  The compact */
/* corresponding :edl tag must be specified for each :dl tag.  The compact */
/* attribute indicates that the list items should be compacted.  Blank     */
/* lines that are normally placed between the list items will be           */
/* suppressed.  The compact attribute is one of the few WATCOM Script/GML  */
/* attributes which does not have an attribute value associated with it.   */
/* The break attribute indicates that the definition description should be */
/* started on a new output line if the size of the definition term exceeds */
/* the maximum horizontal space normally allowed for it.  If this          */
/* attribute is not specified, the definition description will be placed   */
/* after the definition term.  The break attribute is one of the few       */
/* WATCOM Script/GML attributes which does not have an attribute value     */
/* associated with it.  The headhi attribute allows you to set the         */
/* highlighting level of the definition list headings.  Non-negative       */
/* integer numbers are valid highlighting values.  The termhi attribute    */
/* allows you to set the highlighting level of the definition term.        */
/* Non-negative integer numbers are valid highlighting values.  The tsize  */
/* attribute allows you to set the minimum horizontal space taken by the   */
/* definition term.  Any valid horizontal space unit may be specified.     */
/* The attribute value is linked to the font of the :DT tag if the termhi  */
/* attribute is not specified.                                             */
/*                                                                         */
/***************************************************************************/

void    gml_dl( gml_tag gtag )  // not tested TBD
{
    char    *   p;
    bool        compact;

    p = scan_start;
    p++;
    while( *p == ' ' ) {
        p++;
    }
    scan_start = p;                     // over spaces
    if( !strnicmp( "compact", p, 7 ) ) {
        compact = true;
        scan_start = p + 7;
    } else {
        compact = false;
    }
    gml_xl_lp_common( gtag, t_DL );

    nest_cb->compact = compact;

    nest_cb->li_number    = 0;
    nest_cb->left_indent  = conv_hor_unit( &layout_work.dl.left_indent );
    nest_cb->right_indent = conv_hor_unit( &layout_work.dl.right_indent );
    nest_cb->lay_tag      = &layout_work.dl;

    nest_cb->lm = g_cur_left;
    nest_cb->rm = g_page_right;

    scan_start = scan_stop;
    return;
}


/***************************************************************************/
/* Format:  :GL [compact] [termhi=term-highlight].                         */
/*                                                                         */
/* The glossary list tag signals the start of a glossary list, and is      */
/* usually used in the back material section.  Each list item in a         */
/* glossary list has two parts.  The first part is the glossary term and   */
/* is defined with the :gt tag.  The second part is the glossary           */
/* description and is defined with the :gd tag.  A corresponding :egl tag  */
/* must be specified for each :gl tag.  The compact attribute indicates    */
/* that the list items should be compacted.  Blank lines that are normally */
/* placed between the list items will be suppressed.  The compact          */
/* attribute is one of the few WATCOM Script/GML attributes which does not */
/* have an attribute value associated with it.  The termhi attribute       */
/* allows you to set the highlighting level of the glossary term.          */
/* Non-negative integer numbers are valid highlighting values.             */
/*                                                                         */
/***************************************************************************/

void    gml_gl( gml_tag gtag )  // not tested TBD
{
    char    *   p;
    bool        compact;

    p = scan_start;
    p++;
    while( *p == ' ' ) {
        p++;
    }
    scan_start = p;                     // over spaces
    if( !strnicmp( "compact", p, 7 ) ) {
        compact = true;
        scan_start = p + 7;
    } else {
        compact = false;
    }
    gml_xl_lp_common( gtag, t_GL );

    nest_cb->compact = compact;

    nest_cb->li_number    = 0;
    nest_cb->left_indent  = conv_hor_unit( &layout_work.gl.left_indent );
    nest_cb->right_indent = conv_hor_unit( &layout_work.gl.right_indent );
    nest_cb->lay_tag      = &layout_work.gl;

    nest_cb->lm = g_cur_left;
    nest_cb->rm = g_page_right;

    scan_start = scan_stop;
    return;
}

/***************************************************************************/
/*Format: :OL [compact].                                                   */
/*                                                                         */
/*This tag signals the start of an ordered list. Items in the list are     */
/*specified using the :li tag. The list items are preceded by the number   */
/*of the list item. The layout determines the style of the number. An      */
/*ordered list may be used wherever a basic document element is permitted  */
/*to appear. A corresponding :eol tag must be specified for each :ol tag.  */
/*                                                                         */
/*The compact attribute indicates that the list items should be compacted. */
/*Blank lines that are normally placed between the list items will be      */
/*suppressed. The compact attribute is one of the few WATCOM Script/GML    */
/*attributes which does not have an attribute value associated with it.    */
/***************************************************************************/

void    gml_ol( gml_tag gtag )
{
    char    *   p;
    bool        compact;

    p = scan_start + 1;
    while( *p == ' ' ) {
        p++;
    }
    scan_start = p;
    if( !strnicmp( "compact", p, 7 ) ) {
        compact = true;
        scan_start = p + 7;
    } else {
        compact = false;
    }
    if( ProcFlags.need_li_lp ) {
        xx_nest_err( err_no_li_lp );
    }
    gml_xl_lp_common( gtag, t_OL );

    nest_cb->compact = compact;

    nest_cb->li_number    = 0;
    nest_cb->left_indent  = conv_hor_unit( &layout_work.ol.left_indent );
    nest_cb->right_indent = conv_hor_unit( &layout_work.ol.right_indent );
    nest_cb->lay_tag      = &layout_work.ol;

    nest_cb->lm = g_cur_left;
    nest_cb->rm = g_page_right;

    scan_start = scan_stop;
    return;
}


/***************************************************************************/
/* Format:  :SL [compact].                                                 */
/*                                                                         */
/* This tag signals the start of a simple list.                            */
/* Items in the list are specified using the :li tag.  A simple list may   */
/* occur wherever a basic document element is permitted to appear.  A      */
/* corresponding :esl tag must be specified for each :sl tag.  The compact */
/* attribute indicates that the list items should be compacted.  Blank     */
/* lines that are normally placed between the list items will be           */
/* suppressed.  The compact attribute is one of the few WATCOM Script/GML  */
/* attributes which does not have an attribute value associated with it.   */
/*                                                                         */
/***************************************************************************/

void    gml_sl( gml_tag gtag )
{
    char    *   p;
    bool        compact;

    p = scan_start;
    p++;
    while( *p == ' ' ) {
        p++;
    }
    scan_start = p;                     // over spaces
    if( !strnicmp( "compact", p, 7 ) ) {
        compact = true;
        scan_start = p + 7;
    }
    if( ProcFlags.need_li_lp ) {
        xx_nest_err( err_no_li_lp );
    }
    gml_xl_lp_common( gtag, t_SL );

    nest_cb->compact = compact;

    nest_cb->li_number    = 0;
    nest_cb->left_indent  = conv_hor_unit( &layout_work.sl.left_indent );
    nest_cb->right_indent = conv_hor_unit( &layout_work.sl.right_indent );
    nest_cb->lay_tag      = &layout_work.sl;

    nest_cb->lm = g_cur_left;
    nest_cb->rm = g_page_right;

    scan_start = scan_stop;
    return;
}

/***************************************************************************/
/* Format:  :UL [compact].                                                 */
/*                                                                         */
/* This tag signals the start of an unordered                              */
/* list.  Items in the list are specified using the :li tag.  The list     */
/* items are preceded by a symbol such as an asterisk or a bullet.  This   */
/* tag may be used wherever a basic document element is permitted to       */
/* appear.  A corresponding :eul tag must be specified for each :ul tag.   */
/*                                                                         */
/* The compact attribute indicates that the list items should be           */
/* compacted.  Blank lines that are normally placed between the list items */
/* will be suppressed.  The compact attribute is one of the few WATCOM     */
/* Script/GML attributes which does not have an attribute value associated */
/* with it.                                                                */
/*                                                                         */
/***************************************************************************/

void    gml_ul( gml_tag gtag )
{
    char    *   p;
    bool        compact;

    p = scan_start;
    p++;
    while( *p == ' ' ) {
        p++;
    }
    scan_start = p;                     // over spaces
    if( !strnicmp( "compact", p, 7 ) ) {
        compact = true;
        scan_start = p + 7;
    }
    if( ProcFlags.need_li_lp ) {
        xx_nest_err( err_no_li_lp );
    }
    gml_xl_lp_common( gtag, t_UL );

    nest_cb->compact = compact;

    nest_cb->li_number    = 0;
    nest_cb->left_indent  = conv_hor_unit( &layout_work.ul.left_indent );
    nest_cb->right_indent = conv_hor_unit( &layout_work.ul.right_indent );
    nest_cb->lay_tag      = &layout_work.ul;

    nest_cb->lm = g_cur_left;
    nest_cb->rm = g_page_right;

    scan_start = scan_stop;
    return;
}

/***************************************************************************/
/*  common :eXXX processing                                                */
/***************************************************************************/

void    gml_exl_common( gml_tag gtag, e_tags t )
{
    char    *   p;
    tag_cb  *   wk;

    gtag = gtag;
    if( nest_cb->c_tag == t_LP ) {      // terminate :LP if active
        end_lp();
    }

    if( nest_cb->c_tag != t ) {         // unexpected exxx tag
        if( nest_cb->c_tag == t_NONE ) {
            g_err_tag_no( str_tags[t + 1] );// no exxx expected, no tag active
        } else {
            g_err_tag_nest( nest_cb->c_tag ); // exxx expected
        }
    } else {
        g_cur_left = nest_cb->lm;
        g_cur_h_start = nest_cb->lm;
        g_page_right = nest_cb->rm;

        wk = nest_cb;
        nest_cb = nest_cb->prev;
        add_tag_cb_to_pool( wk );
        g_curr_font = nest_cb->font;
        scan_err = false;
        p = scan_start;
        if( *p == '.' ) p++;            // over '.'
        if( *p ) {
            process_text( p, g_curr_font );
        }
    }

    ProcFlags.need_li_lp = false;        // :LI or :LP no longer needed
    scan_start = scan_stop;
}


/***************************************************************************/
/* Format:  :eDL.                                                          */
/*                                                                         */
/* This tag signals the end of a definition list.  A                       */
/* corresponding :DL tag must be previously specified for each :eDL tag.   */
/*                                                                         */
/*                                                                         */
/* Format:  :eGL.                                                          */
/*                                                                         */
/* This tag signals the end of a glossary list.  A                         */
/* corresponding :GL tag must be previously specified for each :eGL tag.   */
/*                                                                         */
/*                                                                         */
/* Format:  :eOL.                                                          */
/*                                                                         */
/* This tag signals the end of an ordered list.  A                         */
/* corresponding :OL tag must be previously specified for each :eOL tag.   */
/*                                                                         */
/*                                                                         */
/* Format:  :eSL.                                                          */
/*                                                                         */
/* This tag signals the end of a simple list.  A                           */
/* corresponding :SL tag must be previously specified for each :eSL tag.   */
/*                                                                         */
/*                                                                         */
/* Format:  :eUL.                                                          */
/*                                                                         */
/* This tag signals the end of a unordered list.  A                        */
/* corresponding :UL tag must be previously specified for each :eUL tag.   */
/*                                                                         */
/***************************************************************************/

void    gml_edl( gml_tag gtag ) // not tested TBD
{
    scr_process_break();
    if( nest_cb->c_tag == t_DL ) {
        set_skip_vars( NULL, NULL,
            &((dl_lay_tag *)(nest_cb->lay_tag))->post_skip, 1, g_curr_font );
    }
    gml_exl_common( gtag, t_DL );
}

void    gml_egl( gml_tag gtag ) // not tested TBD
{
    scr_process_break();
    if( nest_cb->c_tag == t_GL ) {
        set_skip_vars( NULL, NULL,
            &((gl_lay_tag *)(nest_cb->lay_tag))->post_skip, 1, g_curr_font );
    }
    gml_exl_common( gtag, t_GL );
}

void    gml_eol( gml_tag gtag )
{
    scr_process_break();
    if( nest_cb->c_tag == t_OL ) {
        set_skip_vars( NULL, NULL,
            &((ol_lay_tag *)(nest_cb->lay_tag))->post_skip, 1, g_curr_font );
    }
    gml_exl_common( gtag, t_OL );
}

void    gml_esl( gml_tag gtag )
{
    scr_process_break();
    if( nest_cb->c_tag == t_SL ) {
        set_skip_vars( NULL, NULL,
            &((sl_lay_tag *)(nest_cb->lay_tag))->post_skip, 1, g_curr_font );
    }
    gml_exl_common( gtag, t_SL );
}

void    gml_eul( gml_tag gtag )
{
    scr_process_break();
    if( nest_cb->c_tag == t_UL ) {
        set_skip_vars( NULL, NULL,
            &((ul_lay_tag *)(nest_cb->lay_tag))->post_skip, 1, g_curr_font );
    }
    gml_exl_common( gtag, t_UL );
}


/***************************************************************************/
/* :LI within :OL tag                                                      */
/***************************************************************************/

static  void    gml_li_ol( gml_tag gtag )
{
    char        *   p;
    char        *   pn;
    uint32_t        num_len;
    char            charnumber[MAX_L_AS_STR];

    gtag = gtag;
    scan_err = false;
    p = scan_start;

    if( nest_cb == NULL ) {
        xx_nest_err( err_li_lp_no_list );   // tag must be in a list
        scan_start = scan_stop;
        return;
    }

    nest_cb->li_number++;
    pn = format_num( nest_cb->li_number, charnumber, MAX_L_AS_STR,
                     ((ol_lay_tag *)(nest_cb->lay_tag))->number_style );
    if( pn != NULL ) {
        num_len = strlen( pn );
        *(pn + num_len) = ' ';          // trailing space like wgml4 does
        *(pn + num_len + 1) = '\0';
        num_len++;
    } else {
        pn = charnumber;
        *pn = '?';
        *(pn + 1) = 0;
        num_len = 1;
    }

    scr_process_break();

    g_curr_font = ((ol_lay_tag *)(nest_cb->lay_tag))->number_font;

    if( ProcFlags.need_li_lp ) {        // first :li for this list
        set_skip_vars( &((ol_lay_tag *)(nest_cb->lay_tag))->pre_skip, NULL, NULL, 1, g_curr_font );
    } else if( !nest_cb->compact ) {
        set_skip_vars( &((ol_lay_tag *)(nest_cb->lay_tag))->skip, NULL, NULL, 1, g_curr_font );
    } else {                            // compact
        set_skip_vars( NULL, NULL, NULL, 1, g_curr_font );
    }

    post_space = 0;

    g_cur_left = nest_cb->lm + nest_cb->left_indent;
    g_cur_h_start = g_cur_left;
    g_page_right = nest_cb->rm - nest_cb->right_indent;

    ProcFlags.keep_left_margin = true;  // keep special Note indent

    start_line_with_string( charnumber, g_curr_font, true );
    g_cur_h_start = g_cur_left + conv_hor_unit( &(((ol_lay_tag *)(nest_cb->lay_tag))->align) );

    if( t_line != NULL ) {
        if( t_line->last != NULL ) {
            g_cur_left += t_line->last->width + post_space;
        }
    }
    post_space = 0;
    if( g_cur_h_start > g_cur_left ) {
        g_cur_left = g_cur_h_start;
    }
    g_cur_h_start = g_cur_left;
    ju_x_start = g_cur_h_start;

    g_spacing_ln = ((ol_lay_tag *)(nest_cb->lay_tag))->spacing;
    g_curr_font = ((ol_lay_tag *)(nest_cb->lay_tag))->font;
    if( *p == '.' ) p++;                // over '.'
    while( *p == ' ' ) p++;             // skip initial spaces
    ProcFlags.need_li_lp = false;       // 1. item in list processed
    if( *p ) {
        process_text( p, g_curr_font ); // if text follows
    }

    scan_start = scan_stop;
    return;
}


/***************************************************************************/
/* :LI within :SL tag                                                      */
/***************************************************************************/

static  void    gml_li_sl( gml_tag gtag )
{
    char        *   p;

    gtag = gtag;
    scan_err = false;
    p = scan_start;

    if( nest_cb == NULL ) {
        xx_nest_err( err_li_lp_no_list );   // tag must be in a list
        scan_start = scan_stop;
        return;
    }

    scr_process_break();

    if( ProcFlags.need_li_lp ) {        // first :li for this list
        set_skip_vars( &((sl_lay_tag *)(nest_cb->lay_tag))->pre_skip, NULL, NULL, 1, g_curr_font );
    } else if( !nest_cb->compact ) {
        set_skip_vars( &((sl_lay_tag *)(nest_cb->lay_tag))->skip, NULL, NULL, 1, g_curr_font );
    } else {                            // compact
        set_skip_vars( NULL, NULL, NULL, 1, g_curr_font );
    }

    ProcFlags.keep_left_margin = true;  // keep special Note indent

    g_cur_left = nest_cb->lm + nest_cb->left_indent;
    g_cur_h_start = g_cur_left;
    g_page_right = nest_cb->rm - nest_cb->right_indent;

    post_space = 0;
    g_cur_h_start = g_cur_left;
    ju_x_start = g_cur_h_start;

    g_spacing_ln = ((sl_lay_tag *)(nest_cb->lay_tag))->spacing;
    g_curr_font = ((sl_lay_tag *)(nest_cb->lay_tag))->font;
    if( *p == '.' ) p++;                // over '.'
    while( *p == ' ' ) p++;             // skip initial spaces
    ProcFlags.need_li_lp = false;
    if( *p ) {
        process_text( p, g_curr_font ); // if text follows
    }

    scan_start = scan_stop;
    return;
}


/***************************************************************************/
/* :LI within :UL tag                                                      */
/***************************************************************************/

static  void    gml_li_ul( gml_tag gtag )
{
    char        *   p;
    char            bullet[3];

    gtag = gtag;
    scan_err = false;
    p = scan_start;

    if( nest_cb == NULL ) {
        xx_nest_err( err_li_lp_no_list );   // tag must be in a list
        scan_start = scan_stop;
        return;
    }

    if( ((ul_lay_tag *)(nest_cb->lay_tag))->bullet_translate ) {
        bullet[0] = cop_in_trans( ((ul_lay_tag *)(nest_cb->lay_tag))->bullet,
                            ((ul_lay_tag *)(nest_cb->lay_tag))->bullet_font );
    } else {
        bullet[0] = ((ul_lay_tag *)(nest_cb->lay_tag))->bullet;
    }
    bullet[1] = ' ';                    // 1 trailing space as wgml4 does
    bullet[2] = 0;

    scr_process_break();

    if( ProcFlags.need_li_lp ) {        // first :li for this list
        set_skip_vars( &((ul_lay_tag *)(nest_cb->lay_tag))->pre_skip, NULL, NULL, 1, g_curr_font );
    } else if( !nest_cb->compact ) {
        set_skip_vars( &((ul_lay_tag *)(nest_cb->lay_tag))->skip, NULL, NULL, 1, g_curr_font );
    } else {                            // compact
        set_skip_vars( NULL, NULL, NULL, 1, g_curr_font );
    }


    g_spacing_ln = ((ul_lay_tag *)(nest_cb->lay_tag))->spacing;
    g_curr_font = ((ul_lay_tag *)(nest_cb->lay_tag))->bullet_font;
    post_space = 0;

    g_cur_left = nest_cb->lm + nest_cb->left_indent;
    g_cur_h_start = g_cur_left;
    g_page_right = nest_cb->rm - nest_cb->right_indent;

    ProcFlags.keep_left_margin = true;  // keep special Note indent

    start_line_with_string( bullet, g_curr_font, true );
    g_cur_h_start = g_cur_left + conv_hor_unit( &(((ul_lay_tag *)(nest_cb->lay_tag))->align) );

    if( t_line != NULL ) {
        if( t_line->last != NULL ) {
            g_cur_left += t_line->last->width + post_space;
        }
    }
    post_space = 0;
    if( g_cur_h_start > g_cur_left ) {
        g_cur_left = g_cur_h_start;
    }
    g_cur_h_start = g_cur_left;
    ju_x_start = g_cur_h_start;

    g_spacing_ln = ((ul_lay_tag *)(nest_cb->lay_tag))->spacing;
    g_curr_font = ((ul_lay_tag *)(nest_cb->lay_tag))->font;
    if( *p == '.' ) p++;                // over '.'
    while( *p == ' ' ) p++;             // skip initial spaces
    ProcFlags.need_li_lp = false;
    if( *p ) {
        process_text( p, g_curr_font ); // if text fullows
    }

    scan_start = scan_stop;
    return;
}



/****************************************************************************/
/*Format: :LI [id='id-name'].<paragraph elements>                           */
/*                           <basic document elements>                      */
/*                                                                          */
/*This tag signals the start of an item in a simple, ordered, or unordered  */
/*list. The unordered list items are preceded by an annotation symbol, such */
/*as an asterisk. The ordered list items are annotated by an ordered        */
/*sequence. The id attribute associates an identifier name with the list    */
/*item, and may only be used when the list item is in an ordered list. The  */
/*identifier name is used when processing a list item reference, and must   */
/*be unique within the document.                                            */
/*wgml 4.0 does not allow LI inside a DL or GL, but does produce an error   */
/****************************************************************************/

void    gml_li( gml_tag gtag )
{
    if( nest_cb->c_tag == t_LP ) {      // terminate :LP if active
        end_lp();
    }

    switch( nest_cb->c_tag ) {
    case t_OL :
        gml_li_ol( gtag );
        break;
    case t_SL :
        gml_li_sl( gtag );
        break;
    case t_UL :
        gml_li_ul( gtag );
        break;
#if 0
    case t_DL :
        gml_li_dl( gtag );             // error message here?
        break;
    case t_GL :
        gml_li_gl( gtag );             // error message here?
        break;
#endif
    default:
        break;
    }
    return;
}


/***************************************************************************/
/* :LP                                                                     */
/***************************************************************************/

void    gml_lp( gml_tag gtag )
{
    char        *   p;
    su          *   list_skip_su;
    su          *   lp_skip_su;
    su          *   pre_skip_su;

    scan_err = false;
    p = scan_start;

    if( nest_cb == NULL ) {
        xx_nest_err( err_li_lp_no_list );   // tag must be in a list
        scan_start = scan_stop;
        return;
    }

    switch( nest_cb->c_tag ) {
    case t_OL :
        list_skip_su = &((ol_lay_tag *)(nest_cb->lay_tag))->pre_skip;
        break;
    case t_SL :
        list_skip_su = &((sl_lay_tag *)(nest_cb->lay_tag))->pre_skip;
        break;
    case t_UL :
        list_skip_su = &((ul_lay_tag *)(nest_cb->lay_tag))->pre_skip;
        break;
    case t_DL :             // TBD
        list_skip_su = &((dl_lay_tag *)(nest_cb->lay_tag))->pre_skip;
        break;
    case t_GL :             // TBD
        list_skip_su = &((gl_lay_tag *)(nest_cb->lay_tag))->pre_skip;
        break;
    default:
        break;
    }
    lp_skip_su = &layout_work.lp.pre_skip;

    gml_xl_lp_common( gtag, t_LP );

    nest_cb->compact = false;

    nest_cb->li_number    = 0;
    nest_cb->left_indent  = conv_hor_unit( &layout_work.lp.left_indent );
    nest_cb->right_indent = conv_hor_unit( &layout_work.lp.right_indent );
    nest_cb->lay_tag      = &layout_work.lp;

    nest_cb->lm = nest_cb->prev->lm + nest_cb->prev->left_indent;
    nest_cb->rm = nest_cb->prev->rm - nest_cb->prev->right_indent;

    scr_process_break();

    g_spacing_ln = ((lp_lay_tag *)(nest_cb->lay_tag))->spacing;

    ProcFlags.keep_left_margin = true;  // keep special Note indent
    post_space = 0;

    if( ProcFlags.need_li_lp ) {        // :LP first tag in list
        pre_skip_su = greater_su( lp_skip_su, list_skip_su, g_spacing_ln );
    } else {
        pre_skip_su = &((lp_lay_tag *)(nest_cb->lay_tag))->pre_skip;
    }
    set_skip_vars( NULL, pre_skip_su,
                   &((ol_lay_tag *)(nest_cb->lay_tag))->post_skip,
                   1, g_curr_font );

    g_cur_left = nest_cb->lm + nest_cb->left_indent;// left start
                                        // possibly indent first line
    g_cur_h_start = g_cur_left + conv_hor_unit( &(layout_work.lp.line_indent) );

    g_page_right = nest_cb->rm - nest_cb->right_indent;

    ju_x_start = g_cur_h_start;

    if( *p == '.' ) p++;                // over '.'
    while( *p == ' ' ) p++;             // skip initial spaces
    ProcFlags.need_li_lp = false;       // :LI or :LP seen
    if( *p ) {
        process_text( p, g_curr_font ); // if text follows
    }

    scan_start = scan_stop;
    return;
}

