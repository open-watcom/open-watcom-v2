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
* Description: WGML set default layout values
*
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */

#include "wgml.h"
#include "gvars.h"


/***************************************************************************/
/*  init_def_lay            construct the default layout                   */
/***************************************************************************/

void    init_def_lay( void )
{
    static  char    z0[] = "0";         // the
    static  char    i966[] = "9.66i";   // default
    static  char    i7[] = "7i";        // values
    static  char    i1[] = "1i";
    static  char    i05[] = "0.5i";
    static  char    i04[] = "0.4i";
    static  char    i03[] = "0.3i";
    static  char    i02[] = "0.2i";
    static  char    i025[] = "0.25i";
    static  char    n1[] = "1";
    static  char    n2[] = "2";
    static  char    n3[] = "3";
    static  char    n4[] = "4";
    static  char    n15[] = "15";
    static  char    n25[] = "25";
    int             k;

    /***********************************************************************/
    /*  :PAGE  values                                                      */
    /***********************************************************************/
    lay_init_su( z0, &layout_work.page.top_margin );
    lay_init_su( i1, &layout_work.page.left_margin );
    lay_init_su( i7, &layout_work.page.right_margin );
    lay_init_su( i966, &layout_work.page.depth );

    /***********************************************************************/
    /*  :DEFAULT values                                                    */
    /***********************************************************************/
    layout_work.defaults.spacing = 1;
    layout_work.defaults.columns = 1;
    layout_work.defaults.font = 0;
    layout_work.defaults.justify = 1;
    layout_work.defaults.input_esc = ' ';
    lay_init_su( i05, &layout_work.defaults.gutter );
    lay_init_su( z0, &layout_work.defaults.binding );

    /***********************************************************************/
    /* :WIDOW values                                                       */
    /***********************************************************************/
    layout_work.widow.threshold = 2;

    /***********************************************************************/
    /* :FN    values                                                       */
    /***********************************************************************/
    lay_init_su( z0, &layout_work.fn.line_indent );
    lay_init_su( i04, &layout_work.fn.align );
    lay_init_su( n2, &layout_work.fn.pre_lines );
    lay_init_su( n2, &layout_work.fn.skip );
    layout_work.fn.spacing = 1;
    layout_work.fn.font = 0;
    layout_work.fn.number_font = 0;
    layout_work.fn.number_style = h_style;
    layout_work.fn.frame = 0;

    /***********************************************************************/
    /* :FNREF  values                                                      */
    /***********************************************************************/
    layout_work.fnref.font = 0;
    layout_work.fnref.number_style = h_style | xp_style;

    /***********************************************************************/
    /*  :P     values                                                      */
    /***********************************************************************/
    lay_init_su( z0, &layout_work.p.line_indent );
    lay_init_su( n1, &layout_work.p.pre_skip );
    lay_init_su( z0, &layout_work.p.post_skip );

    /***********************************************************************/
    /*  :PC    values                                                      */
    /***********************************************************************/
    lay_init_su( z0, &layout_work.pc.line_indent );
    lay_init_su( n1, &layout_work.pc.pre_skip );
    lay_init_su( z0, &layout_work.pc.post_skip );

    /***********************************************************************/
    /*  :FIG   values                                                      */
    /***********************************************************************/
    lay_init_su( z0, &layout_work.fig.left_adjust );
    lay_init_su( z0, &layout_work.fig.right_adjust );
    lay_init_su( n2, &layout_work.fig.pre_skip );
    lay_init_su( z0, &layout_work.fig.post_skip );
    layout_work.fig.spacing = 1;
    layout_work.fig.font = 0;
    layout_work.fig.default_place = top_place;
    layout_work.fig.default_frame.type = rule_frame;
    strcpy( layout_work.fig.default_frame.string, "????" ); // not used

    /***********************************************************************/
    /*  :XMP   values                                                      */
    /***********************************************************************/
    lay_init_su( i025, &layout_work.xmp.left_indent );
    lay_init_su( z0, &layout_work.xmp.right_indent );
    lay_init_su( n2, &layout_work.xmp.pre_skip );
    lay_init_su( z0, &layout_work.xmp.post_skip );
    layout_work.xmp.spacing = 1;
    layout_work.xmp.font = 0;

    /***********************************************************************/
    /*  :NOTE  values                                                      */
    /***********************************************************************/
    lay_init_su( z0, &layout_work.note.left_indent );
    lay_init_su( z0, &layout_work.note.right_indent );
    lay_init_su( n1, &layout_work.note.pre_skip );
    lay_init_su( n1, &layout_work.note.post_skip );
    layout_work.note.spacing = 1;
    layout_work.note.font = 2;
    strcpy( layout_work.note.string, "NOTE: " );

    /***********************************************************************/
    /*  :H0 - :H6 values                                                   */
    /***********************************************************************/

    k = 0;                              // -----------------
    layout_work.hx[k].group = 0;
    lay_init_su( i05, &layout_work.hx[k].indent );
    lay_init_su( n4, &layout_work.hx[k].pre_top_skip );
    lay_init_su( z0, &layout_work.hx[k].pre_skip );
    lay_init_su( n4, &layout_work.hx[k].post_skip );
    layout_work.hx[k].spacing = 1;
    layout_work.hx[k].font = 3;
    layout_work.hx[k].number_font = 3;
    layout_work.hx[k].number_form = num_none;
    layout_work.hx[k].page_position = pos_left;
    layout_work.hx[k].number_style = h_style;
    layout_work.hx[k].page_eject = ej_yes;
    layout_work.hx[k].line_break = true;
    layout_work.hx[k].display_heading = true;
    layout_work.hx[k].number_reset = true;
    layout_work.hx[k].cases = case_mixed;
    lay_init_su( z0, &layout_work.hx[k].align );

    k = 1;                              // -----------------
    memcpy( &layout_work.hx[1], &layout_work.hx[0],
            sizeof( layout_work.hx[0] ) );
    lay_init_su( z0, &layout_work.hx[k].indent );
    lay_init_su( n3, &layout_work.hx[k].pre_top_skip );
    lay_init_su( n3, &layout_work.hx[k].post_skip );
    layout_work.hx[k].number_form = num_new;

    k = 2;                              // -----------------
    memcpy( &layout_work.hx[2], &layout_work.hx[1],
            sizeof( layout_work.hx[0] ) );
    lay_init_su( n2, &layout_work.hx[k].pre_top_skip );
    lay_init_su( n2, &layout_work.hx[k].post_skip );
    layout_work.hx[k].number_form = num_prop;
    layout_work.hx[k].page_eject = ej_no;

    k = 3;                              // -----------------
    memcpy( &layout_work.hx[3], &layout_work.hx[2],
            sizeof( layout_work.hx[0] ) );

    k = 4;                              // -----------------
    memcpy( &layout_work.hx[4], &layout_work.hx[3],
            sizeof( layout_work.hx[0] ) );
    layout_work.hx[k].font = 2;

    k = 5;                              // -----------------
    memcpy( &layout_work.hx[5], &layout_work.hx[4],
            sizeof( layout_work.hx[0] ) );
    layout_work.hx[k].line_break = false;

    k = 6;                              // -----------------
    memcpy( &layout_work.hx[6], &layout_work.hx[5],
            sizeof( layout_work.hx[0] ) );

    layout_work.hx[k].font = 1;

    /***********************************************************************/
    /*  :HEADING  values                                                   */
    /***********************************************************************/
    layout_work.heading.delim = '.';
    layout_work.heading.stop_eject = false;
    layout_work.heading.para_indent = false;
    layout_work.heading.threshold = 2;
    layout_work.heading.max_group = 10;

    /***********************************************************************/
    /*  :LQ       values                                                   */
    /***********************************************************************/
    lay_init_su( i025, &layout_work.lq.left_indent );
    lay_init_su( i025, &layout_work.lq.right_indent );
    lay_init_su( n1, &layout_work.lq.pre_skip );
    lay_init_su( n1, &layout_work.lq.post_skip );
    layout_work.lq.spacing = 1;
    layout_work.lq.font = 0;

    /***********************************************************************/
    /*  tags  with only font as value                                      */
    /***********************************************************************/
    layout_work.cit.font = 1;
    layout_work.dd.font = 0;
    layout_work.ddhd.font = 1;
    layout_work.dt.font = 2;
    layout_work.dthd.font = 1;
    layout_work.gd.font = 0;
    layout_work.gt.font = 2;
    layout_work.ixmajor.font = 2;
    layout_work.ixpgnum.font = 0;

    /***********************************************************************/
    /*  :FIGCAP     values                                                 */
    /***********************************************************************/
    lay_init_su( n1, &layout_work.figcap.pre_lines );
    strcpy( layout_work.figcap.string, "Figure " );
    layout_work.figcap.font = 0;
    layout_work.figcap.string_font = 0;
    layout_work.figcap.delim = '.';

    /***********************************************************************/
    /*  :FIGDESC    values                                                 */
    /***********************************************************************/
    lay_init_su( n1, &layout_work.figdesc.pre_lines );
    layout_work.figdesc.font = 0;

    /***********************************************************************/
    /*  :DD         values                                                 */
    /***********************************************************************/
    lay_init_su( i05, &layout_work.dd.line_left );
    layout_work.dd.font = 0;

    /***********************************************************************/
    /*  :ABSTRACT   values                                                 */
    /***********************************************************************/
    lay_init_su( n1, &layout_work.abstract.post_skip );
    lay_init_su( n1, &layout_work.abstract.pre_top_skip );
    layout_work.abstract.font = 1;
    layout_work.abstract.spacing = 1;
    layout_work.abstract.header = true;
    strcpy( layout_work.abstract.string, "ABSTRACT" );
    layout_work.abstract.page_eject = ej_yes;
    layout_work.abstract.page_reset = true;
    layout_work.abstract.columns = 1;

    /***********************************************************************/
    /*  :PREFACE    values                                                 */
    /***********************************************************************/
    lay_init_su( n1, &layout_work.preface.post_skip );
    lay_init_su( n1, &layout_work.preface.pre_top_skip );
    layout_work.preface.font = 1;
    layout_work.preface.spacing = 1;
    layout_work.preface.header = true;
    strcpy( layout_work.preface.string, "PREFACE" );
    layout_work.preface.page_eject = ej_yes;
    layout_work.preface.page_reset = false;
    layout_work.preface.columns = 1;

    /***********************************************************************/
    /*  :BODY       values                                                 */
    /***********************************************************************/
    lay_init_su( z0, &layout_work.body.post_skip );
    lay_init_su( z0, &layout_work.body.pre_top_skip );
    layout_work.body.header = false;
    layout_work.body.string[0] = '\0';
    layout_work.body.page_eject = ej_yes;
    layout_work.body.page_reset = true;
    layout_work.body.font = 1;

    /***********************************************************************/
    /*  :BACKM      values                                                 */
    /***********************************************************************/
    lay_init_su( z0, &layout_work.backm.post_skip );
    lay_init_su( z0, &layout_work.backm.pre_top_skip );
    layout_work.backm.header = false;
    layout_work.backm.string[0] = '\0';
    layout_work.backm.page_eject = ej_yes;
    layout_work.backm.page_reset = false;
    layout_work.backm.columns = 1;
    layout_work.backm.font = 1;

    /***********************************************************************/
    /*  :LP       values                                                   */
    /***********************************************************************/
    lay_init_su( z0, &layout_work.lp.left_indent );
    lay_init_su( z0, &layout_work.lp.right_indent );
    lay_init_su( z0, &layout_work.lp.line_indent );
    lay_init_su( n1, &layout_work.lp.pre_skip );
    lay_init_su( n1, &layout_work.lp.post_skip );
    layout_work.lp.spacing = 1;

    /***********************************************************************/
    /*  :INDEX    values                                                   */
    /***********************************************************************/
    lay_init_su( z0, &layout_work.index.post_skip );
    lay_init_su( z0, &layout_work.index.pre_top_skip );
    lay_init_su( z0, &layout_work.index.left_adjust );
    lay_init_su( z0, &layout_work.index.right_adjust );
    layout_work.index.spacing = 1;
    layout_work.index.columns = 1;
    strcpy( layout_work.index.see_string, "See " );
    strcpy( layout_work.index.see_also_string, "See also " );
    layout_work.index.header = false;
    strcpy( layout_work.index.index_string, "Index" );
    layout_work.index.page_eject = ej_yes;
    layout_work.index.page_reset = false;
    layout_work.index.font = 1;

    /***********************************************************************/
    /*  :IXHEAD   values                                                   */
    /***********************************************************************/
    lay_init_su( n2, &layout_work.ixhead.pre_skip );
    lay_init_su( z0, &layout_work.ixhead.post_skip );
    layout_work.ixhead.font = 2;
    lay_init_su( z0, &layout_work.ixhead.indent );
    layout_work.ixhead.frame.type = box_frame;
    layout_work.ixhead.frame.string[0] = '\0';
    layout_work.ixhead.header = true;

    /***********************************************************************/
    /*  :I1 - :I3  values                                                   */
    /***********************************************************************/

    k = 0;                              // -----------------
    lay_init_su( n1, &layout_work.ix[k].pre_skip );
    lay_init_su( n1, &layout_work.ix[k].post_skip );
    lay_init_su( n1, &layout_work.ix[k].skip );
    layout_work.ix[0].font = 0;
    lay_init_su( z0, &layout_work.ix[k].indent );
    lay_init_su( i04, &layout_work.ix[k].wrap_indent );
    layout_work.ix[k].index_delim[0] = ' ';
    layout_work.ix[k].index_delim[1] = ' ';
    layout_work.ix[k].index_delim[2] = '\0';
    layout_work.ix[k].string_font = 0;

    k = 1;                              // -----------------
    lay_init_su( z0, &layout_work.ix[k].pre_skip );
    lay_init_su( z0, &layout_work.ix[k].post_skip );
    lay_init_su( z0, &layout_work.ix[k].skip );
    layout_work.ix[0].font = 0;
    lay_init_su( i03, &layout_work.ix[k].indent );
    lay_init_su( i04, &layout_work.ix[k].wrap_indent );
    layout_work.ix[k].index_delim[0] = ' ';
    layout_work.ix[k].index_delim[1] = ' ';
    layout_work.ix[k].index_delim[2] = '\0';
    layout_work.ix[k].string_font = 0;

    k = 2;                              // -----------------
    lay_init_su( z0, &layout_work.ix[k].pre_skip );
    lay_init_su( z0, &layout_work.ix[k].post_skip );
    lay_init_su( z0, &layout_work.ix[k].skip );
    layout_work.ix[k].font = 0;
    lay_init_su( i03, &layout_work.ix[k].indent );
    lay_init_su( i04, &layout_work.ix[k].wrap_indent );
    layout_work.ix[k].index_delim[0] = ' ';
    layout_work.ix[k].index_delim[1] = ' ';
    layout_work.ix[k].index_delim[2] = '\0';
//  layout_work.ix[k].string_font = 0; no string_font for :I3

    /***********************************************************************/
    /*  :TOC        values                                                 */
    /***********************************************************************/
    lay_init_su( z0, &layout_work.toc.left_adjust );
    lay_init_su( z0, &layout_work.toc.right_adjust );
    layout_work.toc.spacing = 1;
    layout_work.toc.columns = 1;
    layout_work.toc.toc_levels = 4;
    layout_work.toc.fill_string[0] = '.';
    layout_work.toc.fill_string[1] = '\0';

    /***********************************************************************/
    /*  :TOCPGNUM   values                                                 */
    /***********************************************************************/
    lay_init_su( i04, &layout_work.tocpgnum.size );
    layout_work.tocpgnum.font = 0;

    /***********************************************************************/
    /*  :TOCHx      values                                                 */
    /***********************************************************************/
    k = 0;                              // --------------
    layout_work.tochx[k].group = 0;
    lay_init_su( z0, &layout_work.tochx[k].indent );
    lay_init_su( n1, &layout_work.tochx[k].skip );
    lay_init_su( n1, &layout_work.tochx[k].pre_skip );
    lay_init_su( n1, &layout_work.tochx[k].post_skip );
    layout_work.tochx[k].font = 0;
    lay_init_su( z0, &layout_work.tochx[k].align );
    layout_work.tochx[k].display_in_toc = true;

    k = 1;                              // --------------
    layout_work.tochx[k].group = 0;
    lay_init_su( z0, &layout_work.tochx[k].indent );
    lay_init_su( n1, &layout_work.tochx[k].skip );
    lay_init_su( z0, &layout_work.tochx[k].pre_skip );
    lay_init_su( n1, &layout_work.tochx[k].post_skip );
    layout_work.tochx[k].font = 0;
    lay_init_su( z0, &layout_work.tochx[k].align );
    layout_work.tochx[k].display_in_toc = true;

    k = 2;                              // --------------
    layout_work.tochx[k].group = 0;
    lay_init_su( i02, &layout_work.tochx[k].indent );
    lay_init_su( z0, &layout_work.tochx[k].skip );
    lay_init_su( z0, &layout_work.tochx[k].pre_skip );
    lay_init_su( z0, &layout_work.tochx[k].post_skip );
    layout_work.tochx[k].font = 0;
    lay_init_su( z0, &layout_work.tochx[k].align );
    layout_work.tochx[k].display_in_toc = true;

    k = 3;                              // --------------
    layout_work.tochx[k].group = 0;
    lay_init_su( i02, &layout_work.tochx[k].indent );
    lay_init_su( z0, &layout_work.tochx[k].skip );
    lay_init_su( z0, &layout_work.tochx[k].pre_skip );
    lay_init_su( z0, &layout_work.tochx[k].post_skip );
    layout_work.tochx[k].font = 0;
    lay_init_su( z0, &layout_work.tochx[k].align );
    layout_work.tochx[k].display_in_toc = true;

    k = 4;                              // --------------
    layout_work.tochx[k].group = 0;
    lay_init_su( i02, &layout_work.tochx[k].indent );
    lay_init_su( z0, &layout_work.tochx[k].skip );
    lay_init_su( z0, &layout_work.tochx[k].pre_skip );
    lay_init_su( z0, &layout_work.tochx[k].post_skip );
    layout_work.tochx[k].font = 0;
    lay_init_su( z0, &layout_work.tochx[k].align );
    layout_work.tochx[k].display_in_toc = false;

    k = 5;                              // --------------
    layout_work.tochx[k].group = 0;
    lay_init_su( i02, &layout_work.tochx[k].indent );
    lay_init_su( z0, &layout_work.tochx[k].skip );
    lay_init_su( z0, &layout_work.tochx[k].pre_skip );
    lay_init_su( z0, &layout_work.tochx[k].post_skip );
    layout_work.tochx[k].font = 0;
    lay_init_su( z0, &layout_work.tochx[k].align );
    layout_work.tochx[k].display_in_toc = false;

    k = 6;                              // --------------
    layout_work.tochx[k].group = 0;
    lay_init_su( i02, &layout_work.tochx[k].indent );
    lay_init_su( z0, &layout_work.tochx[k].skip );
    lay_init_su( z0, &layout_work.tochx[k].pre_skip );
    lay_init_su( z0, &layout_work.tochx[k].post_skip );
    layout_work.tochx[k].font = 0;
    lay_init_su( z0, &layout_work.tochx[k].align );
    layout_work.tochx[k].display_in_toc = false;

    /***********************************************************************/
    /*  :FIGLIST    values                                                 */
    /***********************************************************************/
    lay_init_su( z0, &layout_work.figlist.left_adjust );
    lay_init_su( z0, &layout_work.figlist.right_adjust );
    lay_init_su( z0, &layout_work.figlist.skip );
    layout_work.figlist.spacing = 1;
    layout_work.figlist.columns = 1;
    layout_work.figlist.fill_string[0] = '.';
    layout_work.figlist.fill_string[1] = '\0';

    /***********************************************************************/
    /*  :FLPGNUM   values                                                  */
    /***********************************************************************/
    lay_init_su( i04, &layout_work.flpgnum.size );
    layout_work.flpgnum.font = 0;

    /***********************************************************************/
    /*  :TITLEP    values                                                  */
    /***********************************************************************/
    layout_work.titlep.spacing = 1;
    layout_work.titlep.columns = 1;

    /***********************************************************************/
    /*  :TITLE      values                                                 */
    /***********************************************************************/
    lay_init_su( z0, &layout_work.title.left_adjust );
    lay_init_su( i1, &layout_work.title.right_adjust );
    layout_work.title.page_position = pos_right;
    layout_work.title.font = 2;
    lay_init_su( n15, &layout_work.title.pre_top_skip );
    lay_init_su( n2, &layout_work.title.skip );

    /***********************************************************************/
    /*  :DOCNUM     values                                                 */
    /***********************************************************************/
    lay_init_su( z0, &layout_work.docnum.left_adjust );
    lay_init_su( i1, &layout_work.docnum.right_adjust );
    layout_work.docnum.page_position = pos_right;
    layout_work.docnum.font = 0;
    lay_init_su( n2, &layout_work.docnum.pre_skip );
    strcpy( layout_work.docnum.string, "Document Number " );

    /***********************************************************************/
    /*  :DATE       values                                                 */
    /***********************************************************************/
    strcpy( layout_work.date.date_form, "$ml $dsn, $yl" );
    lay_init_su( z0, &layout_work.date.left_adjust );
    lay_init_su( i1, &layout_work.date.right_adjust );
    layout_work.date.page_position = pos_right;
    layout_work.date.font = 0;
    lay_init_su( n2, &layout_work.date.pre_skip );

    /***********************************************************************/
    /*  :AUTHOR     values                                                 */
    /***********************************************************************/
    lay_init_su( z0, &layout_work.author.left_adjust );
    lay_init_su( i1, &layout_work.author.right_adjust );
    layout_work.author.page_position = pos_right;
    layout_work.author.font = 0;
    lay_init_su( n25, &layout_work.author.pre_skip );
    lay_init_su( n1, &layout_work.author.skip );

    /***********************************************************************/
    /*  :ADDRESS     values                                                 */
    /***********************************************************************/
    lay_init_su( z0, &layout_work.address.left_adjust );
    lay_init_su( i1, &layout_work.address.right_adjust );
    layout_work.address.page_position = pos_right;
    layout_work.address.font = 0;
    lay_init_su( n2, &layout_work.address.pre_skip );

    /***********************************************************************/
    /*  :ALINE       values                                                 */
    /***********************************************************************/
    lay_init_su( n1, &layout_work.aline.skip );

    /***********************************************************************/
    /*  :APPENDIX   values                                                 */
    /***********************************************************************/
    lay_init_su( z0, &layout_work.appendix.indent );
    lay_init_su( z0, &layout_work.appendix.pre_top_skip );
    lay_init_su( z0, &layout_work.appendix.pre_skip );
    lay_init_su( n3, &layout_work.appendix.post_skip );
    layout_work.appendix.spacing = 1;
    layout_work.appendix.font = 3;
    layout_work.appendix.number_font = 3;
    layout_work.appendix.number_form = num_new;
    layout_work.appendix.page_position = pos_left;
    layout_work.appendix.number_style = b_style;
    layout_work.appendix.page_eject = ej_yes;
    layout_work.appendix.line_break = true;
    layout_work.appendix.display_heading = true;
    layout_work.appendix.number_reset = true;
    layout_work.appendix.cases = case_mixed;
    lay_init_su( z0, &layout_work.appendix.align );
    layout_work.appendix.header = true;
    strcpy( layout_work.appendix.string, "APPENDIX " );
    layout_work.appendix.page_reset = false;
    layout_work.appendix.section_eject = ej_yes;
    layout_work.appendix.columns = 1;

    /***********************************************************************/
    /*  :SL         values                                                 */
    /***********************************************************************/
    layout_work.sl.level = 1;
    lay_init_su( z0, &layout_work.sl.left_indent );
    lay_init_su( z0, &layout_work.sl.right_indent );
    lay_init_su( n1, &layout_work.sl.pre_skip );
    lay_init_su( n1, &layout_work.sl.skip );
    lay_init_su( n1, &layout_work.sl.post_skip );
    layout_work.sl.spacing = 1;
    layout_work.sl.font = 0;

    /***********************************************************************/
    /*  :OL         values                                                 */
    /***********************************************************************/
    layout_work.ol.level = 1;
    lay_init_su( z0, &layout_work.ol.left_indent );
    lay_init_su( z0, &layout_work.ol.right_indent );
    lay_init_su( z0, &layout_work.ol.pre_skip );
    lay_init_su( n1, &layout_work.ol.skip );
    lay_init_su( n1, &layout_work.ol.post_skip );
    lay_init_su( i04, &layout_work.ol.align );
    layout_work.ol.spacing = 1;
    layout_work.ol.font = 0;
    layout_work.ol.number_style = h_style | xd_style;;
    layout_work.ol.number_font = 0;

    /***********************************************************************/
    /*  :UL         values                                                 */
    /***********************************************************************/
    layout_work.ul.level = 1;
    lay_init_su( z0, &layout_work.ul.left_indent );
    lay_init_su( z0, &layout_work.ul.right_indent );
    lay_init_su( n1, &layout_work.ul.pre_skip );
    lay_init_su( n1, &layout_work.ul.skip );
    lay_init_su( n1, &layout_work.ul.post_skip );
    lay_init_su( i04, &layout_work.ul.align );
    layout_work.ul.spacing = 1;
    layout_work.ul.font = 0;
    layout_work.ul.bullet = '*';
    layout_work.ul.bullet_translate = true;
    layout_work.ul.bullet_font = 0;

    /***********************************************************************/
    /*  :DL         values                                                 */
    /***********************************************************************/
    layout_work.dl.level = 1;
    lay_init_su( z0, &layout_work.dl.left_indent );
    lay_init_su( z0, &layout_work.dl.right_indent );
    lay_init_su( n1, &layout_work.dl.pre_skip );
    lay_init_su( n1, &layout_work.dl.skip );
    lay_init_su( n1, &layout_work.dl.post_skip );
    lay_init_su( i1, &layout_work.dl.align );
    layout_work.dl.spacing = 1;
    layout_work.dl.line_break = false;

    /***********************************************************************/
    /*  :GL         values                                                 */
    /***********************************************************************/
    layout_work.gl.level = 1;
    lay_init_su( z0, &layout_work.gl.left_indent );
    lay_init_su( z0, &layout_work.gl.right_indent );
    lay_init_su( n1, &layout_work.gl.pre_skip );
    lay_init_su( n1, &layout_work.gl.skip );
    lay_init_su( n1, &layout_work.gl.post_skip );
    lay_init_su( z0, &layout_work.gl.align );
    layout_work.gl.spacing = 1;
    layout_work.gl.delim = ':';

    /***********************************************************************/
    /*  :BANNER     values                                                 */
    /***********************************************************************/
    banner_defaults();

}
