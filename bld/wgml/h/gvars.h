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
* Description:  wgml global variables.
*
****************************************************************************/


#ifndef GVARS_H_INCLUDED
#define GVARS_H_INCLUDED

#include <setjmp.h>
#include <time.h>

#ifndef global
    #define global  extern
#endif

#include "gtype.h"
#include "gtypelay.h"

#ifdef __WATCOMC__
#pragma enable_message( 128 ); // reenable: Warning! W128: 3 padding byte(s) added
#endif

global struct tm        doc_tm;         // document time/date

global  jmp_buf     *   environment;    // var for GSuicide()

global  char        *   scan_start;
global  char        *   scan_stop;
global  char        *   scan_char_ptr;  // used by character scanning routines
global  char        *   scan_restart;   // used by character scanning routines
global  bool            scan_err;       // used by character scanning routines
global  char        *   tok_start;      // start of scanned token
global  size_t          arg_flen;       // arg length
global  char        *   var_start;      // variable start
global  size_t          var_len;        // variable length
global  size_t          val_len;        // value length
global  char        *   val_start;      // value start
global  char            quote_char;     // value is quoted by this char or \0
global  locflags        rs_loc;         // restricted location

global  int             switch_char;    // DOS switch character
global  char        *   alt_ext;        // alternate extension
global  char        *   def_ext;        // default extension

global  char        *   master_fname;   // Primary input file name
global  char        *   master_fname_attr;// Primary input file name attributes
global  ulong           print_from;     // first page to print
global  ulong           print_to;       // last page to print
global  char        *   dev_name;       // device defined_name
global  opt_font    *   opt_fonts;      // option FONT parameters (linked list)

global  inputcb     *   input_cbs;      // GML input stack (files + macros)
global  fnstack     *   fn_stack;       // input filename stack
global  char        *   out_file;       // output file name
global  char        *   out_file_attr;  // output file attributes (T:2222)
global  unsigned        inc_level;   // include nesting level 1 = MasterFname
global  unsigned        max_inc_level;  // maximum include level depth
global  line_number     line_from;      // starting lineno to process
global  line_number     line_to;        // ending lineno to process
#define LINEFROM_DEFAULT    1
#define LINETO_DEFAULT      (0x1000000) // 16 MiB lines should be enough

global  char            gotarget[MAC_NAME_LENGTH +1];   // .go to target name
global  uint32_t        gotargetno;     // .go to line no

global  int             err_count;      // Overall Errorcount
global  int             wng_count;      // Overall warning count

global  char            GML_char;       // GML Keywword start char :
global  char            SCR_char;       // SCRIPT keywword start char .
global  char            CW_sep_char;    // Control Word separator char ;

global  int             CPI;            // chars per inch
global  space_units     CPI_units;      // unit for chars per inch
global  int             LPI;            // lines per inch
global  space_units     LPI_units;      // unit for lines per inch

global  su              bind_odd;       // Bind value for odd pages

global  su              bind_even;      // Bind value for even pages

global  int             passes;         // Max no of document passes
global  int             pass;           // current document pass no

global  uint32_t        apage;          // current absolute pageno &$apage
global  uint32_t        page;           // current document pageno &$page
global  line_number     line;           // current output lineno   &$line
global  int32_t         lcmax;          // remaining lines on page initial
global  int32_t         lc;             // remaining lines on page &$lc

global  int32_t         hm;             // heading margin          &$hm
global  int32_t         tm;             // top margin              &$tm

global  int32_t         bm;             // bottom margin           &$bm
global  int32_t         fm;             // footing margin          &$fm

global  int32_t         lm;             // left margin             &$pagelm
global  int32_t         rm;             // right margin            &$pagerm

global  ix_h_blk    *   ixhtag[4];// last higher level :IH1 :IH2 tags in index
global  ix_h_blk    *   index_dict;     // index structure dictionary
global  ref_entry   *   iref_dict;  // reference id dictionary :Ix :IHx :IREF
global  ref_entry   *   ref_dict;       // reference dictionary :Hx tags
global  ref_entry   *   fig_dict;       // reference dictionary :FIG tags
global  ref_entry   *   fn_dict;        // reference dictionary :FN tags

global  symvar      *   global_dict;    // global symbol dictionary
global  symvar      *   sys_dict;       // global system symbol dictionary
global  mac_entry   *   macro_dict;     // macro dictionary
global  gtentry     *   tag_dict;       // user tag dictionary

global  char            research_file_name[48]; // filename for research
global  ulong           research_from;  // line no start for research output
global  ulong           research_to;    // line no end   for research output

global  struct GlobalFlags {
    unsigned        quiet         : 1;  // suppress product info
    unsigned        bannerprinted : 1;  // product info shown
    unsigned        wscript       : 1;  // enable WATCOM script extension
    unsigned        firstpass     : 1;  // first or only pass
    unsigned        lastpass      : 1;  // last or only pass
    unsigned        inclist       : 1;  // show included files
    unsigned        warning       : 1;  // show warnings
    unsigned        statistics    : 1;  // output statistics at end

    unsigned        index         : 1;  // index option
    unsigned        free9         : 1;
    unsigned        freea         : 1;
    unsigned        freeb         : 1;
    unsigned        freec         : 1;
    unsigned        freed         : 1;
    unsigned        freee         : 1;
    unsigned        research      : 1;  // -r global research mode output
} GlobalFlags;                          // Global flags

global struct ProcFlags {
    doc_section     doc_sect;           // which part are we in (FRONTM, BODY, ...
    doc_section     doc_sect_nxt;       // next section (tag already seen)
    doc_section     header_sect;        // header      placeholder for now    TBD
    doc_section     header_sect_nxt;    // header nxt  placeholder for now    TBD
    unsigned        frontm_seen    : 1; // FRONTM tag seen
    unsigned        start_section  : 1; // start section call done

    unsigned        researchfile   : 1;// research for one file ( -r filename )

    unsigned        fb_document_done : 1;// true if fb_document() called
    unsigned        fb_position_done : 1;// 1. pos on new page done
    unsigned        page_started    : 1;// we have something for the curr page
    unsigned        line_started    : 1;// we have something for current line
    unsigned        just_override   : 1;// current line is to be justified

    unsigned        address_active  : 1;// within :ADDRESS tag ) only one of
    unsigned        fig_active      : 1;// within :FIG tag     ) these may be
    unsigned        fn_active       : 1;// within :FN tag      ) set at a
    unsigned        xmp_active      : 1;// within :XMP tag     ) time

    unsigned        author_tag_seen : 1;// remember first :AUTHOR tag
    unsigned        date_tag_seen   : 1;// :DATE is allowed only once
    unsigned        docnum_tag_seen : 1;// :DOCNUM is allowed only once
    unsigned        stitle_seen     : 1;// remember first stitle value
    unsigned        title_tag_top   : 1;// :TITLE pre_top_skip used
    unsigned        title_text_seen : 1;// remember first :TITLE tag text
    unsigned        empty_doc_el    : 1;// empty doc element allowed
    unsigned        group_elements  : 1;// currently grouping doc_elements
    unsigned        goto_active     : 1;// processing .go label
    unsigned        newLevelFile    : 1;// start new include Level (file)
    unsigned        gml_tag         : 1;// input buf starts with GML_char
    unsigned        scr_cw          : 1;// input buf starts with SCR_char
    unsigned        macro_ignore    : 1;// .. in col 1-2
    unsigned        CW_sep_ignore   : 1;// .' in col 1-2
    unsigned        in_macro_define : 1;// macro definition active
    unsigned        suppress_msg    : 1;// suppress error msg (during scanning)
    unsigned        blanks_allowed  : 1;// blanks allowed (during scanning)
    unsigned        keep_ifstate    : 1;// leave ifstack unchanged for next line
    unsigned        substituted     : 1;// variable substituted in current line
    unsigned        unresolved      : 1;// variable found, but not yet resolved
    unsigned        literal         : 1;// .li is active
    unsigned        concat          : 1;// .co ON if set
    unsigned        ct              : 1;// .ct continue text is active
    unsigned        in_trans        : 1;// esc char is specified (.ti set x)
    unsigned        reprocess_line  : 1;// unget for current input line
#if 0
    unsigned        sk_cond         : 1;// .sk n C found
#endif
    unsigned        overprint       : 1;// .sk -1 active or not
    unsigned        tag_end_found   : 1;// '.' ending tag found
    unsigned        skips_valid     : 1;// controls set_skip_vars() useage
    unsigned        in_bx_box       : 1;// identifies first BX line
    unsigned        box_cols_cur    : 1;// current BX line had column list

    unsigned        no_var_impl_err : 1;// suppress err_var_not_impl msg
    unsigned        keep_left_margin: 1;// for indent NOTE tag paragraph
    unsigned        need_li_lp      : 1;// just list tag (:SL,...) seen

    unsigned        has_aa_block    : 1;// true if device defined :ABSOLUTEADDRESS
    unsigned        ps_device       : 1;// true if device is PostScript

    unsigned        layout          : 1;// within :layout tag and sub tags
    unsigned        lay_specified   : 1;// LAYOUT option or :LAYOUT tag seen
    unsigned        banner          : 1;// within layout banner definition
    unsigned        banregion       : 1;// within layout banregion definition
    unsigned        hx_level        : 3;// 0 - 6  active Hx :layout sub tag
    lay_sub         lay_xxx         : 8;// active :layout sub tag

    ju_enum         justify         : 8;// .ju on half off ...

} ProcFlags;                            // processing flags

#ifdef __WATCOMC__
#pragma enable_message( 128 ); // reenable: Warning! W128: 3 padding byte(s) added
#endif

global  size_t          buf_size;       // default buffer size
global  char        *   token_buf;

global char         *   workbuf;        // work for input buffer
global char         *   buff2;          // input buffer
global size_t           buff2_lg;       // input buffer used length

// the following to manage .gt * and .ga * * syntax
global char         tagname[TAG_NAME_LENGTH + 1];// last defined GML tag name
global gtentry  *   tag_entry;          // ... entry in tag_dict
global char         attname[ATT_NAME_LENGTH + 1];// last defined GML attribute
global gaentry  *   att_entry;          // ... entry in tag_dict

global  long        li_cnt;             // remaining count for .li processing

global  uint8_t     in_esc;             // input escape char from .ti

global  box_col_set *   box_col_set_pool;   // pool of box_col_set instances
global  box_col_set *   box_line;       // the current line to be drawn
global  box_col_set *   cur_line;       // the line from the current BX line
global  box_col_set *   prev_line;      // the previously drawn line
global  uint32_t        box_col_width;  // width of one column, as used with BX
global  uint32_t        h_vl_offset;    // horizontal offset used to position VLINE output

global  tab_stop    *   c_stop;         // current tab_stop
global  uint32_t        first_tab;      // first default top position
global  uint32_t        inter_tab;      // distance between default tabs
global  char            tab_char;       // tab character from .tb
global  uint32_t        tab_col;        // width of one column, as used with tabs

// the document page and related items
global doc_el_group     t_doc_el_group; // for accumulating a group of doc_elements
global doc_element  *   t_element;      // the current element for main
global text_line    *   t_el_last;      // attachment point to t_element
global doc_page         t_page;         // for constructing output page
global doc_next_page    n_page;         // for deferred elements
global text_line    *   t_line;         // for constructing output line
global text_chars   *   text_pool;      // for reuse of text_chars structs
global text_line    *   line_pool;      // for reuse of text_line structs
global ban_column   *   ban_col_pool;   // for reuse of ban_column structs
global doc_column   *   doc_col_pool;   // for reuse of doc_column structs
global doc_element  *   doc_el_pool;    // for reuse of doc_element structs

/***************************************************************************/
/*  some globals which are to be redesigned when the :LAYOUT tag is coded. */
/*  Defined here so some script control words can be prototyped            */
/***************************************************************************/

global  uint32_t    g_cur_h_start;
global  uint32_t    g_cur_left;
global  uint32_t    g_cur_v_start;
global  uint32_t    g_page_bottom;
global  uint32_t    g_page_bottom_org;
global  uint32_t    g_page_left;
global  uint32_t    g_page_left_org;
global  uint32_t    g_page_right;
global  uint32_t    g_page_right_org;
global  uint32_t    g_page_top;
global  uint32_t    g_page_top_org;
global  uint32_t    g_page_depth;
global  uint32_t    g_max_char_width;
global  uint32_t    g_max_line_height;
global  uint32_t    g_net_page_depth;
global  uint32_t    g_net_page_width;

global  int32_t     g_resh;             // horiz base units
global  int32_t     g_resv;             // vert base units

global  font_number g_curr_font;        // the font to use for current line
global  uint32_t    g_cl;               // column length
global  uint32_t    g_ll;               // line length
global  uint32_t    g_cd;               // no of columns
global  uint32_t    g_gutter;           // space between columns
global  uint32_t    g_offset[9];        // column start offset

global  uint32_t    blank_lines;        // blank lines (line count)
global  uint32_t    g_blank_lines;      // blank lines (in vertical base units)
global  uint32_t    g_post_skip;        // post_skip
global  uint32_t    g_subs_skip;        // subs_skip
global  uint32_t    g_top_skip;         // top_skip
global  uint32_t    g_spacing;          // spacing (in vertical base units)
global  int32_t     g_skip;             // .sk skip value ( -1 to +nn )
global  uint32_t    spacing;            // spacing between lines (line count)

global  uint32_t    post_space;         // spacing within a line
global  uint32_t    ju_x_start;         // .. formatting

global  uint32_t    g_indent;           // .in 1. value (left) default 0
global  int32_t     g_indentr;          // .in 2. value (right) default 0

global  uint32_t    g_cur_threshold;    // current widow threshold value
                                        // from layout (widow or heading)

global  tag_cb  *   nest_cb;            // infos about nested tags
global  tag_cb  *   tag_pool;           // list of reusable tag_cbs

global  banner_lay_tag  * sect_ban_top[2];// top even / odd banner for curr sect
global  banner_lay_tag  * sect_ban_bot[2];// bot even / odd banner for curr sect

global  uint32_t    msg_indent;         // indent for message output (to screen, not to device)


/***************************************************************************/
/*  tagnames as strings for msg display                                    */
/***************************************************************************/

#define pickg( name, length, routine, gmlflags, locflags )  { #name },

global char str_tags[t_MAX + 1][10]
#if defined(tag_strings)
   = {
     { "NONE" },
#include "gtags.h"
//  #include "gscrcws.h" TBD
     { "MAX" }
}
#endif
;
#undef pickg
#undef xmystr
#undef mystr



/***************************************************************************/
/*  :LAYOUT  data                                                          */
/***************************************************************************/

global  int32_t         lay_ind;// index into lay_tab for attribute processing
global  layout_data     layout_work;    // layout used for formatting
global  laystack    *   lay_files;      // layout file(s) specified on cmdline


/***************************************************************************/
/* The tab lists.                                                          */
/***************************************************************************/

global  tab_list        def_tabs;   // tabs at columns 6, 11, 16, ..., 81
global  tab_list        user_tabs;  // for tabs defined by control word TB

/***************************************************************************/
/* Layout attribute names as character strings                             */
/*  array initialized in glconvrt.c                                        */
/*  longest attribute name is extract_threshold  (=17)                     */
/*                            ....+....1....+..                            */
/***************************************************************************/
extern  const   char    att_names[e_dummy_max + 1][18];


/***************************************************************************/
/*   declarations for the sequence of LAYOUT attribute values              */
/*   definitions are in the layout tag processing source file              */
/*   sequence as seen by :convert output                                   */
/***************************************************************************/

/***************************************************************************/
/*   :PAGE attributes                                                      */
/***************************************************************************/
extern  const   lay_att     page_att[5];

/***************************************************************************/
/*   :DEFAULT attributes                                                   */
/***************************************************************************/
extern  const   lay_att     default_att[8];

/***************************************************************************/
/*   :WIDOW attributes                                                     */
/***************************************************************************/
extern  const   lay_att     widow_att[2];

/***************************************************************************/
/*   :FN    attributes                                                     */
/***************************************************************************/
extern  const   lay_att     fn_att[11];

/***************************************************************************/
/*   :FNREF    attributes                                                  */
/***************************************************************************/
extern  const   lay_att     fnref_att[3];

/***************************************************************************/
/*   :P and :PC attributes                                                 */
/***************************************************************************/
extern  const   lay_att     p_att[4];

/***************************************************************************/
/*   :FIG   attributes                                                     */
/***************************************************************************/
extern  const   lay_att     fig_att[9];

/***************************************************************************/
/*   :XMP   attributes                                                     */
/***************************************************************************/
extern  const   lay_att     xmp_att[7];

/***************************************************************************/
/*   :NOTE  attributes                                                     */
/***************************************************************************/
extern  const   lay_att     note_att[8];

/***************************************************************************/
/*   :H0 - :H6  attributes                                                 */
/***************************************************************************/
extern  const   lay_att     hx_att[18];

/***************************************************************************/
/*   :HEADING   attributes                                                 */
/***************************************************************************/
extern  const   lay_att     heading_att[6];

/***************************************************************************/
/*   :LQ        attributes                                                 */
/***************************************************************************/
extern  const   lay_att     lq_att[7];

/***************************************************************************/
/* :DT :GT :DTHD :CIT :GD :DDHD :IXPGNUM :IXMAJOR                          */
/*              attributes                                                 */
/***************************************************************************/
extern  const   lay_att     xx_att[2];

/***************************************************************************/
/*   :FIGCAP    attributes                                                 */
/***************************************************************************/
extern  const   lay_att     figcap_att[6];

/***************************************************************************/
/*   :FIGDESC   attributes                                                 */
/***************************************************************************/
extern  const   lay_att     figdesc_att[3];

/***************************************************************************/
/*   :DD        attributes                                                 */
/***************************************************************************/
extern  const   lay_att     dd_att[3];

/***************************************************************************/
/*   :ABSTRACT and :PREFACE attributes                                     */
/***************************************************************************/
extern  const   lay_att     abspref_att[11];

/***************************************************************************/
/*   :BACKM and :BODY attributes                                           */
/***************************************************************************/
extern  const   lay_att     backbod_att[10];

/***************************************************************************/
/*   :LP        attributes                                                 */
/***************************************************************************/
extern  const   lay_att     lp_att[7];

/***************************************************************************/
/*   :INDEX     attributes                                                 */
/***************************************************************************/
extern  const   lay_att     index_att[14];

/***************************************************************************/
/*   :IXHEAD    attributes                                                 */
/***************************************************************************/
extern  const   lay_att     ixhead_att[7];

/***************************************************************************/
/*   :I1 :I2 :I3 attributes   nearly identical :i3 without string_font     */
/***************************************************************************/
extern  const   lay_att     ix_att[9];

/***************************************************************************/
/*   :TOC       attributes                                                 */
/***************************************************************************/
extern  const   lay_att     toc_att[7];

/***************************************************************************/
/*   :TOCPGNUM  attributes                                                 */
/***************************************************************************/
extern  const   lay_att     tocpgnum_att[3];

/***************************************************************************/
/*   :TOCPHx    attributes                                                 */
/***************************************************************************/
extern  const   lay_att     tochx_att[9];

/***************************************************************************/
/*   :FIGLIST   attributes                                                 */
/***************************************************************************/
extern  const   lay_att     figlist_att[7];

/***************************************************************************/
/*   :FLPGNUM   attributes                                                 */
/***************************************************************************/
extern  const   lay_att     flpgnum_att[3];

/***************************************************************************/
/*   :TITLEP    attributes                                                 */
/***************************************************************************/
extern  const   lay_att     titlep_att[3];

/***************************************************************************/
/*   :TITLE     attributes                                                 */
/***************************************************************************/
extern  const   lay_att     title_att[7];

/***************************************************************************/
/*   :DOCNUM    attributes                                                 */
/***************************************************************************/
extern  const   lay_att     docnum_att[7];

/***************************************************************************/
/*   :DATE      attributes                                                 */
/***************************************************************************/
extern  const   lay_att     date_att[7];

/***************************************************************************/
/*   :AUTHOR    attributes                                                 */
/***************************************************************************/
extern  const   lay_att     author_att[7];

/***************************************************************************/
/*   :ADDRESS   attributes                                                 */
/***************************************************************************/
extern  const   lay_att     address_att[6];

/***************************************************************************/
/*   :ALINE     attributes                                                 */
/***************************************************************************/
extern  const   lay_att     aline_att[2];

/***************************************************************************/
/*   :APPENDIX  attributes                                                 */
/***************************************************************************/
extern  const   lay_att     appendix_att[22];

/***************************************************************************/
/*   :DL :GL :OL :SL :UL attributes                                        */
/***************************************************************************/
extern  const   lay_att     dl_att[10];
extern  const   lay_att     gl_att[10];
extern  const   lay_att     ol_att[12];
extern  const   lay_att     sl_att[9];
extern  const   lay_att     ul_att[13];

/***************************************************************************/
/*   :BANNER and :BANREGION      attributes                                */
/***************************************************************************/
extern  const   lay_att     banner_att[8];
extern  const   lay_att     banregion_att[12];

/* Reset so can be reused with other headers. */
#undef global

#endif  /* GVARS_H_INCLUDED */

