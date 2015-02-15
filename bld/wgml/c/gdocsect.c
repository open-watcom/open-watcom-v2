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
* Description:  WGML tags and routines for document section changes
*                    :GDOC, :FRONTM, :BODY, ...
*               and special for :INDEX tag
****************************************************************************/


#include    "wgml.h"
#include    "gvars.h"


line_number     titlep_lineno;      // TITLEP tag line number

/***************************************************************************/
/*  error routine for wrong sequence of doc section tags                   */
/***************************************************************************/
static  void    g_err_doc_sect( doc_section ds )
{
    static  char const  sect[][9] = {
        #define pick(t,e,p) t,
        #include "docsect.h"
        #undef pick
    };

    err_count++;
    scan_err = true;
    g_err( err_doc_sect, sect[ds] );
    file_mac_info();
}


/***************************************************************************/
/*  set banner pointers for specified doc section                          */
/***************************************************************************/
void set_section_banners( doc_section ds )
{
    banner_lay_tag  * ban;

    /***********************************************************************/
    /*  transform doc_section enum into ban_doc_sect enum                  */
    /***********************************************************************/
    static const ban_docsect sect_2_bansect[] = {
        #define pick(t,e,p) p,
        #include "docsect.h"
        #undef pick
    };

/* not yet coded banner place values               TBD
               not all are document section related
    head0_ban,
    head1_ban,
    head2_ban,
    head3_ban,
    head4_ban,
    head5_ban,
    head6_ban,
    letfirst_ban,
    letlast_ban,
    letter_ban,
    max_ban
} ban_docsect;
****************/

    sect_ban_top[0] = sect_ban_top[1] = NULL;
    sect_ban_bot[0] = sect_ban_bot[1] = NULL;

    if( no_ban != sect_2_bansect[ds]  ) {

        for( ban = layout_work.banner; ban != NULL; ban = ban->next ) {
            if( ban->docsect == sect_2_bansect[ds] ) {  // if our doc section
                switch( ban->place ) {
                case   top_place :
                    sect_ban_top[0] = ban;
                    sect_ban_top[1] = ban;
                    break;
                case   bottom_place :
                    sect_ban_bot[0] = ban;
                    sect_ban_bot[1] = ban;
                    break;

                case   topodd_place :
                    sect_ban_top[1] = ban;
                    break;
                case   topeven_place :
                    sect_ban_top[0] = ban;
                    break;

                case   botodd_place :
                    sect_ban_bot[1] = ban;
                    break;
                case   boteven_place :
                    sect_ban_bot[0] = ban;
                    break;
                default:
                    break;
                }
            }
        }
    }
}


/***************************************************************************/
/*  set some values for new section                                        */
/***************************************************************************/

static  void    new_section( doc_section ds )
{
    ProcFlags.doc_sect = ds;
    set_section_banners( ds );

    spacing = layout_work.defaults.spacing;
    g_curr_font = layout_work.defaults.font;
}


/***************************************************************************/
/*    finish page processing  and section change                           */
/***************************************************************************/

static  void    finish_page_section( doc_section ds, bool eject )
{
    if( eject ) {
        g_skip = 0;                     // ignore remaining skip value
    }
    new_section( ds );
    do_page_out();
}


/***************************************************************************/
/*    output section header ABSTRACT, PREFACE, ...                         */
/***************************************************************************/

static  void    doc_header( su *p_sk, su *top_sk, xx_str *h_string,
                            font_number font, int8_t spc, bool no_eject )
{
    doc_element     *   cur_el;
    font_number         font_save;
    int32_t             h_left;
    text_chars      *   curr_t;
    text_line       *   hd_line;

    font_save = g_curr_font;
    g_curr_font = font;
    g_curr_font = font_save;
    set_skip_vars( NULL, top_sk, p_sk, spc, g_curr_font );

    if( (h_string == NULL) || (*h_string == '\0') ||
        (*h_string == ' ') || (*h_string == '\t')  ) {

        /********************************************************/
        /*  header contained "yes" but the string was empty:    */
        /*  the OW docs do this with APPENDIX for PS/PDF output */
        /********************************************************/

        hd_line = alloc_text_line();    // defaults work
    } else {
        curr_t = alloc_text_chars( h_string, strlen( h_string ), font );
        curr_t->width = cop_text_width( curr_t->text, curr_t->count, font );
        h_left = g_page_left +(g_page_right - g_page_left - curr_t->width) / 2;
        curr_t->x_address = h_left;

        hd_line = alloc_text_line();
        hd_line->first = curr_t;
        hd_line->line_height = wgml_fonts[font].line_height;
    }

    if( input_cbs->fmflags & II_research ) {
        test_out_t_line( hd_line );
    }
    cur_el = alloc_doc_el( el_text );
    cur_el->blank_lines = g_blank_lines;
    g_blank_lines = 0;
    cur_el->depth = hd_line->line_height;
    cur_el->subs_skip = g_subs_skip;
    cur_el->top_skip = g_top_skip;
    cur_el->element.text.overprint = ProcFlags.overprint;
    ProcFlags.overprint = false;
    cur_el->element.text.spacing = g_spacing;
    cur_el->element.text.first = hd_line;
    ProcFlags.skips_valid = false;
    hd_line = NULL;

    if( no_eject ) {
        insert_col_main( cur_el );
    } else {
        insert_page_width( cur_el );
    }
}


/***************************************************************************/
/*  set new vertical position depending on banner existance                */
/***************************************************************************/

static void document_new_position( void )
{
    region_lay_tag  *   top_reg;
    uint32_t            top_pos;

    /* The value used for fb_position() should be the first line of text,
     * even if that is from a TITLE tag (top banners do not apply to TITLEP).
     * This handles documents starting with banners and text only.
     */

    if( sect_ban_top[page & 1] != NULL ) {
        top_reg = sect_ban_top[page & 1]->top_line;
        if( bin_driver->y_positive == 0x00 ) {
            top_pos = g_page_top_org - top_reg->reg_depth;
        } else {
            top_pos = g_page_top_org + top_reg->reg_depth;
        }
        g_cur_v_start = top_pos;
    } else {
        top_pos = g_page_top;
        if( bin_driver->y_positive == 0x00 ) {
            g_cur_v_start = g_page_top - wgml_fonts[0].line_height;
        } else {
            g_cur_v_start = g_page_top + wgml_fonts[0].line_height;
        }
    }
    g_cur_h_start = g_page_left_org;

    if( GlobalFlags.lastpass ) {
        if( ProcFlags.fb_position_done ) {
            fb_new_section( g_cur_v_start );
        } else {
            fb_position( g_cur_h_start, g_cur_v_start );
            ProcFlags.fb_position_done = true;
        }
    }
    g_cur_v_start = top_pos; // reset so first line positioning is correct
    return;
}


/***************************************************************************/
/*    start_doc_sect true section start                                    */
/***************************************************************************/

void    start_doc_sect( void )
{
    bool                first_section;
    bool                header;
    bool                page_r;
    doc_section         ds;
    font_number         font;
    int8_t              h_spc;
    page_ej             page_e;
    su              *   p_sk;
    su              *   top_sk;
    uint32_t            ind;
    xx_str          *   h_string;

    if( ProcFlags.start_section ) {
        return;                         // once is enough
    }
    if( !ProcFlags.fb_document_done ) { // the very first section/page
        do_layout_end_processing();
    }

    first_section = (ProcFlags.doc_sect == doc_sect_none);

    header = false;                 // no header string (ABSTRACT, ... )  output
    page_r = false;                 // no page number reset
    page_e = ej_no;                 // no page eject
    ProcFlags.start_section = true;
    ProcFlags.keep_left_margin = false;
    ds = ProcFlags.doc_sect_nxt;        // new section

    if( ds == doc_sect_none ) {
        ds = doc_sect_body;      // if text without section start assume body
    }

    /***********************************************************************/
    /*  process special section attributes                                 */
    /***********************************************************************/

    switch( ds ) {
    case   doc_sect_body:
        page_r   = layout_work.body.page_reset;
        page_e   = layout_work.body.page_eject;
        if( layout_work.body.header ) {
            header   = true;
            h_string = layout_work.body.string;
            top_sk   = &layout_work.body.pre_top_skip;
            p_sk     = &layout_work.body.post_skip;
            font     = layout_work.body.font;
            h_spc    = spacing;         // standard spacing
        }
        break;
    case   doc_sect_titlep:             // for preceding :BINCLUDE/:GRAPHIC
        page_e = ej_yes;
        init_nest_cb();
        nest_cb->p_stack = copy_to_nest_stack();
        nest_cb->c_tag = t_TITLEP;
        nest_cb->p_stack->lineno = titlep_lineno; // correct line number
        break;
    case   doc_sect_abstract:
        page_r   = layout_work.abstract.page_reset;
        page_e   = layout_work.abstract.page_eject;
        if( layout_work.abstract.header ) {
            header = true;
            h_string = layout_work.abstract.string;
            top_sk   = &layout_work.abstract.pre_top_skip;
            p_sk     = &layout_work.abstract.post_skip;
            font     = layout_work.abstract.font;
            h_spc    = layout_work.abstract.spacing;
        }
        break;
    case   doc_sect_preface:
        page_r   = layout_work.preface.page_reset;
        page_e   = layout_work.preface.page_eject;
        if( layout_work.preface.header ) {
            header = true;
            h_string = layout_work.preface.string;
            top_sk   = &layout_work.preface.pre_top_skip;
            p_sk     = &layout_work.preface.post_skip;
            font     = layout_work.preface.font;
            h_spc    = layout_work.preface.spacing;
        }
        break;
    case   doc_sect_appendix:
        page_r   = layout_work.appendix.page_reset;
        page_e   = layout_work.appendix.page_eject;
        if( layout_work.appendix.header ) {
            header = true;
            h_string = layout_work.appendix.string;
            top_sk   = &layout_work.appendix.pre_top_skip;
            p_sk     = &layout_work.appendix.post_skip;
            font     = layout_work.appendix.font;
            h_spc    = layout_work.appendix.spacing;
        }
        break;
    case   doc_sect_backm:
        page_r   = layout_work.backm.page_reset;
        page_e   = layout_work.backm.page_eject;
        if( layout_work.backm.header ) {
            header = true;
            h_string = layout_work.backm.string;
            top_sk   = &layout_work.backm.pre_top_skip;
            p_sk     = &layout_work.backm.post_skip;
            font     = layout_work.backm.font;
            h_spc    = spacing;         // standard spacing
        }
        break;
    case   doc_sect_index:
        page_r   = layout_work.index.page_reset;
        page_e   = layout_work.index.page_eject;
        if( layout_work.index.header ) {
            header = true;
            h_string = layout_work.index.index_string;
            top_sk   = &layout_work.index.pre_top_skip;
            p_sk     = &layout_work.index.post_skip;
            font     = layout_work.index.font;
            h_spc    = layout_work.index.spacing;
        }
        break;
    default:
        new_section( ds );
        break;
    }

    if( first_section ) {               // nothing precedes the first section
        if( page_e == ej_even ) {
            do_page_out();              // apage of first page is odd
            page = 0;                   // restart page for first text page
        }
        new_section( ds );
        reset_t_page();
        document_new_position();        // first text page ready for content
    } else {
        full_page_out();                // ensure are on last page
        switch( page_e ) {              // page eject requested
        case ej_yes :
            finish_page_section( ds, true );// emit last page in old section
            if( page_r ) {
                page = 0;
            }
            reset_t_page();
            break;
        case ej_odd :
            if( !(apage & 1) ) {        // first page would be even
                do_page_out();          // emit last page in old section
                reset_t_page();
            }
            finish_page_section( ds, true );// emit last page in old section
            if( page_r ) {
                page = 0;
            }
            reset_t_page();
            break;
        case ej_even :
            if( (apage & 1) ) {         // first page will be odd
                do_page_out();          // emit last page in old section
                reset_t_page();
            }
            finish_page_section( ds, true );// emit last page in old section
            if( page_r ) {
                page = 0;
            }
            reset_t_page();
            break;
        default:                        //  ej_no
            new_section( ds );

            /****************************************************/
            /*  set page bottom banner/limit for new section    */
            /****************************************************/

            ind = !(page & 1);
            t_page.bottom_banner = sect_ban_bot[ind];
            if( sect_ban_bot[ind] != NULL ) {
                if( bin_driver->y_positive == 0 ) {
                    g_page_bottom = g_page_bottom_org + sect_ban_bot[ind]->ban_depth;
                } else {
                    g_page_bottom = g_page_bottom_org - sect_ban_bot[ind]->ban_depth;
                }
            } else {
                g_page_bottom = g_page_bottom_org;
            }
            break;
        }
    }
    g_cur_left = g_page_left_org;
    g_cur_h_start = g_page_left_org + g_indent;
    if( header ) {
        doc_header( p_sk, top_sk, h_string, font, h_spc, page_e == ej_no );
    }
    ProcFlags.doc_sect = ds;
}


/***************************************************************************/
/*          routine to init    document section change                     */
/***************************************************************************/
static  void    gml_doc_xxx( doc_section ds )
{

    if( ProcFlags.doc_sect >= ds ) {    // wrong sequence of sections
        g_err_doc_sect( ds );
    }
    ProcFlags.doc_sect_nxt = ds;        // remember new section
    ProcFlags.start_section = false;    // do real section start later

    scan_start = scan_stop;
    return;
}


/***************************************************************************/
/*  Document section tags                                                  */
/*                                                                         */
/*  :GDOC                 the only one with attributes                     */
/*  :FRONTM    optional                                                    */
/*  :TITLEP    optional                                                    */
/*  :ETITLEP   optional                                                    */
/*  :ABSTRACT  optional                                                    */
/*  :PREFACE   optional                                                    */
/*  :TOC       optional                                                    */
/*  :FIGLIST   optional                                                    */
/*  :BODY              default                                             */
/*  :APPENDIX  optional                                                    */
/*  :BACKM     optional                                                    */
/*  :INDEX     optional                                                    */
/*  :EGDOC     optional                                                    */
/***************************************************************************/

void    gml_abstract( gml_tag tag )
{
    tag = tag;
    if( ProcFlags.doc_sect_nxt == doc_sect_egdoc ) {
        xx_line_err( err_eof_expected, tok_start );
        return;
    }
    if( !ProcFlags.frontm_seen ) {
        xx_line_err( err_doc_sec_expected_1, tok_start );
        return;
    }
    if( blank_lines > 0 ) {
        set_skip_vars( NULL, NULL, NULL, 0, 0 );    // set g_blank_lines
    }
    scr_process_break();
    gml_doc_xxx( doc_sect_abstract );
    spacing = layout_work.abstract.spacing;
    g_cur_left = g_page_left;
    g_cur_h_start = g_page_left;

}

void    gml_appendix( gml_tag tag )
{
    tag = tag;
    if( blank_lines > 0 ) {
        set_skip_vars( NULL, NULL, NULL, 0, 0 );    // set g_blank_lines
    }
    scr_process_break();
    gml_doc_xxx( doc_sect_appendix );
    spacing = layout_work.appendix.spacing;
    ProcFlags.frontm_seen = false;  // no longer in FRONTM section
    if( !ProcFlags.fb_document_done ) { // the very first section/page
        do_layout_end_processing();
    }
}

void    gml_backm( gml_tag tag )
{
    tag = tag;
    if( blank_lines > 0 ) {
        set_skip_vars( NULL, NULL, NULL, 0, 0 );    // set g_blank_lines
    }
    scr_process_break();
    gml_doc_xxx( doc_sect_backm );
    ProcFlags.frontm_seen = false;  // no longer in FRONTM section
    if( !ProcFlags.fb_document_done ) { // the very first section/page
        do_layout_end_processing();
    }
}

void    gml_body( gml_tag tag )
{
    tag = tag;
    if( blank_lines > 0 ) {
        set_skip_vars( NULL, NULL, NULL, 0, 0 );    // set g_blank_lines
    }
    scr_process_break();
    gml_doc_xxx( doc_sect_body );

    ProcFlags.just_override = true;     // justify for first line ?? TBD
    g_cur_left = g_page_left;
    g_cur_h_start = g_page_left
                    + conv_hor_unit( &layout_work.p.line_indent );

    ProcFlags.frontm_seen = false;      // no longer in FRONTM section
    if( !ProcFlags.fb_document_done ) { // the very first section/page
        do_layout_end_processing();
    }
}

void    gml_figlist( gml_tag tag )
{
    tag = tag;
    gml_doc_xxx( doc_sect_figlist );
    spacing = layout_work.figlist.spacing;
}

void    gml_frontm( gml_tag tag )
{
    tag = tag;
    gml_doc_xxx( doc_sect_frontm );
    spacing = layout_work.defaults.spacing;
    if( !ProcFlags.fb_document_done ) { // the very first section/page
        do_layout_end_processing();
    }
    ProcFlags.frontm_seen = true;
}


/***************************************************************************/
/*  :INDEX tag is special, not really a section                            */
/***************************************************************************/

void    gml_index( gml_tag tag )
{
    tag = tag;
    if( ProcFlags.doc_sect_nxt == doc_sect_egdoc ) {
        xx_line_err( err_eof_expected, tok_start );
        return;
    }

    if( ProcFlags.doc_sect_nxt == doc_sect_index ) {// duplicate :INDEX tag

        scan_start = scan_stop;         // ignore this call
        return;                         // wgml4 OS/2 crashes with page fault
    }

    if( !((ProcFlags.doc_sect == doc_sect_backm) ||
          (ProcFlags.doc_sect_nxt == doc_sect_backm)) ) {
        xx_line_err( err_doc_sec_expected_1, tok_start );
        return;
    }
    if( !GlobalFlags.index ) {          // index option not active
        g_err( wng_index_opt );         // give hint to activate index
        scan_start = scan_stop;
        return;
    }
    gml_doc_xxx( doc_sect_index );

    gen_index();                        // output the formatted index

}

void    gml_preface( gml_tag tag )
{
    tag = tag;
    if( ProcFlags.doc_sect_nxt == doc_sect_egdoc ) {
        xx_line_err( err_eof_expected, tok_start );
        return;
    }
    if( !ProcFlags.frontm_seen ) {
        xx_line_err( err_doc_sec_expected_1, tok_start );
        return;
    }
    if( blank_lines > 0 ) {
        set_skip_vars( NULL, NULL, NULL, 0, 0 );    // set g_blank_lines
    }
    scr_process_break();
    gml_doc_xxx( doc_sect_preface );
    spacing = layout_work.preface.spacing;
}

void    gml_titlep( gml_tag tag )
{
    tag = tag;
    if( ProcFlags.doc_sect_nxt == doc_sect_egdoc ) {
        xx_line_err( err_eof_expected, tok_start );
        return;
    }
    if( !ProcFlags.frontm_seen ) {
        xx_line_err( err_doc_sec_expected_1, tok_start );
        return;
    }
    gml_doc_xxx( doc_sect_titlep );
    spacing = layout_work.titlep.spacing;

    add_symvar( &global_dict, "$stitle", "", no_subscript, 0 );// set nullstring
    add_symvar( &global_dict, "$title", "", no_subscript, 0 );// set nullstring

    rs_loc = titlep_tag;
    if( input_cbs->fmflags & II_file ) {    // save line number
        titlep_lineno = input_cbs->s.f->lineno;
    } else if( input_cbs->fmflags & II_tag_mac ) {
        titlep_lineno = input_cbs->s.m->lineno;
    } else {
        titlep_lineno = 0;                  // not clear what to do here
    }
}

void    gml_etitlep( gml_tag tag )
{
    tag_cb  *   wk;

    tag = tag;
    gml_doc_xxx( doc_sect_etitlep );
    rs_loc = 0;
    titlep_lineno = 0;

    if( nest_cb != NULL ) { // guard against no FRONTM, empty TITLEP section
        wk = nest_cb;
        nest_cb = nest_cb->prev;
        add_tag_cb_to_pool( wk );
    }
}

void    gml_toc( gml_tag tag )
{
    tag = tag;
    gml_doc_xxx( doc_sect_toc );
    spacing = layout_work.toc.spacing;
}

void    gml_egdoc( gml_tag tag )
{
    tag = tag;
    if( blank_lines > 0 ) {
        set_skip_vars( NULL, NULL, NULL, 0, 0 );    // set g_blank_lines
    }
    scr_process_break();                // outputs last element in file
    if( !ProcFlags.start_section ) {
        start_doc_sect();               // if not already done
    }
    gml_doc_xxx( doc_sect_egdoc );
}

/***************************************************************************/
/*  :gdoc sec='TOP secret, destroy before reading'                         */
/***************************************************************************/

void    gml_gdoc( gml_tag tag )
{
    char        *   p;

    tag = tag;
    scan_err = false;
    p = scan_start;
    if( *p ) p++;

    while( *p == ' ' ) {                // over WS to attribute
        p++;
    }
    if( *p &&
        ! (strnicmp( "sec ", p, 4 ) &&  // look for "sec " or "sec="
           strnicmp( "sec=", p, 4 )) ) {
        char        quote;
        char    *   valstart;

        p += 3;
        while( *p == ' ' ) {
            p++;
        }
        if( *p == '=' ) {
            p++;
            while( *p == ' ' ) {
                p++;
            }
        }
        if( *p == '"' || *p == '\'' ) {
            quote = *p;
            ++p;
        } else {
            quote = ' ';
        }
        valstart = p;
        while( *p && *p != quote ) {
            ++p;
        }
        *p = '\0';

        add_symvar( &global_dict, "$sec", valstart, no_subscript, 0 );
    } else {
        add_symvar( &global_dict, "$sec", "", no_subscript, 0 );// set nullstring
    }

    gml_doc_xxx( doc_sect_gdoc );
    if( !ProcFlags.fb_document_done ) { // the very first section/page
        do_layout_end_processing();
    }
    return;
}

