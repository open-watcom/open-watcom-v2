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
* Description:  wgml global variables.
*
****************************************************************************/


#ifndef GVARS_H_INCLUDED

#include <setjmp.h>
#include <time.h>

#include "gtype.h"
#include "gtypelay.h"

#endif  /* GVARS_H_INCLUDED */

#if !defined( GVARS_H_INCLUDED ) || defined( global )
#define GVARS_H_INCLUDED

#ifndef global
    #define global  extern
#endif

global struct tm            doc_tm;         // document time/date

global  jmp_buf     *   environment;    // var for GSuicide()

global  char        *   scan_start;
global  char        *   scan_stop;
global  char        *   new_file_parms; // command tail for IM/AP
global  char        *   scan_char_ptr;  // used by character scanning routines
global  char        *   scan_restart;   // used by character scanning routines
global  bool            scan_err;       // used by character scanning routines
global  char        *   tok_start;      // start of scanned token
global  size_t          arg_flen;       // arg length
global  tag_att_val     g_att_val;      // current attribute/value info
global  char        *   att_start;      // (potential) attribute start
global  size_t          val_len;        // attribute value length
global  char        *   val_start;      // attribute value start
global  char            quote_char;     // value is quoted by this char or \0
global  locflags        rs_loc;         // restricted location

global  int             switch_char;    // DOS switch character
global  char        *   alt_ext;        // alternate extension
global  char        *   def_ext;        // default extension
global  char            ampchar;        // symbol substitution/attribute/function marker

global  char        *   master_fname;   // Primary input file name
global  char        *   master_fname_attr;// Primary input file name attributes
global  line_number     print_from;     // first page to print
global  line_number     print_to;       // last page to print
global  char        *   dev_name;       // device defined_name
global  opt_font    *   opt_fonts;      // option FONT parameters (linked list)

global  inputcb     *   input_cbs;      // GML input stack (files + macros)
global  fnstack     *   fn_stack;       // input filename stack
global  char        *   out_file;       // output file name
global  char        *   out_file_attr;  // output file attributes (T:2222)
global  unsigned        inc_level;      // include nesting level 1 = MasterFname
global  unsigned        max_inc_level;  // maximum include level depth
global  line_number     line_from;      // starting lineno to process
global  line_number     line_to;        // ending lineno to process
#define LINEFROM_DEFAULT    1
#define LINETO_DEFAULT      (0x1000000) // 16 MiB lines should be enough

global  char            gotarget[MAC_NAME_LENGTH +1];   // .go to target name
global  int32_t         gotargetno;     // .go to line no

global  int             err_count;      // Overall Errorcount
global  int             wng_count;      // Overall warning count

global  char            CONT_char;      // CONTINUE char (normally 0x03)
global  char            GML_char;       // GML Keywword start char (normally ":")
global  char            SCR_char;       // SCRIPT keywword start char (normally ".")
global  char            CW_sep_char;    // Control Word separator char (normally ";")

global  int             CPI;            // chars per inch
global  space_units     CPI_units;      // unit for chars per inch
global  int             LPI;            // lines per inch
global  space_units     LPI_units;      // unit for lines per inch

global  su              bind_odd;       // Bind value for odd pages

global  su              bind_even;      // Bind value for even pages

global  int             passes;         // Max no of document passes
global  int             pass;           // current document pass no

global  uint32_t        g_apage;        // current absolute pageno &$apage
global  uint32_t        g_page;         // current document pageno &$page
global  line_number     g_line;         // current output lineno   &$line
global  int32_t         lcmax;          // remaining lines on page initial

global  int32_t         g_hm;           // heading margin          &$hm
global  int32_t         g_tm;           // top margin              &$tm

global  int32_t         g_bm;           // bottom margin           &$bm
global  int32_t         g_fm;           // footing margin          &$fm

global  int32_t         g_lm;           // left margin             &$pagelm
global  int32_t         g_rm;           // right margin            &$pagerm

global symdict_hdl      global_dict;    // global symbol dictionary
global symdict_hdl      sys_dict;       // global system symbol dictionary
global  mac_dict    *   macro_dict;     // macro dictionary
global  gtentry     *   tag_dict;       // user tag dictionary

global  char            research_file_name[48]; // filename for research
global  line_number     research_from;  // line no start for research output
global  line_number     research_to;    // line no end   for research output

global  global_flags    GlobalFlags;    // global flags

global  proc_flags      ProcFlags;      // processing flags

global  attr_flags      AttrFlags;      // attribute flags

global  size_t          buf_size;       // default buffer size
global  char        *   token_buf;

global char         *   workbuf;        // work for input buffer
global char         *   buff2;          // input buffer
global size_t           buff2_lg;       // input buffer used length

// the following to manage .gt * and .ga * * syntax
global char         g_tagname[TAG_NAME_LENGTH + 1];// last defined GML tag name
global gtentry      *g_tag_entry;       // ... entry in tag_dict
global char         g_attname[ATT_NAME_LENGTH + 1];// last defined GML attribute
global gaentry      *g_att_entry;       // ... entry in tag_dict

global int          li_cnt;             // remaining count for .li processing

global uint8_t      in_esc;             // input escape char from .ti


// file block support
global doc_el_group     *   block_queue;        // queue of FB blocks; blocks removed from here
global doc_el_group     *   block_queue_end;    // add point for new FB blocks

// box support
global  box_col_set     *   box_col_set_pool;   // pool of box_col_set instances
global  box_col_set     *   g_cur_line;         // the line from the current BX line
global  box_col_set     *   g_prev_line;        // the previously drawn line
global  box_col_stack   *   box_col_stack_pool; // pool of box_col_stack instances
global  box_col_stack   *   box_line;           // the current line to be drawn
global  uint32_t            box_col_width;      // width of one column, as used with BX
global  uint32_t            h_vl_offset;        // horizontal offset used to position VLINE output
global  uint32_t            max_depth;          // space left on page (used by BX)

// figure support
global  uint32_t            fig_count;      // figure number
global  ffh_entry       *   fig_entry;      // current fig_list entry
global  fwd_ref         *   fig_fwd_refs;   // forward reference/undefined id/page change
global  ffh_entry       *   fig_list;       // list of figures in order encountered
global  record_buffer       line_buff;      // used for some frame types
global  ref_entry       *   fig_ref_dict;   // reference dictionary :FIG tags

// footnote support
global  uint32_t        fn_count;       // footnote number
global  ffh_entry   *   fn_entry;       // current fn_list entry
global  fwd_ref     *   fn_fwd_refs;    // forward reference/undefined id
global  ref_entry   *   fn_ref_dict;    // reference dictionary :FN tags
global  ffh_entry   *   fn_list;        // list of footnotes in order encountered

// heading support
global  ffh_entry   *   hd_entry;       // current hd_list entry
global  fwd_ref     *   hd_fwd_refs;    // forward reference/undefined id/page change
global  hdsrc           hd_level;       // current heading level
global  ref_entry   *   hd_ref_dict;    // reference dictionary :Hx tags
global  ffh_entry   *   hd_list;        // list of headings in order encountered
global  hd_num_data     hd_nums[hds_appendix];  // heading hierarchy numbering

// index support
global  bool            ixhlvl[2];      // true for levels that exist
global  eol_ix      *   eol_ix_pool;    // eol_ix pool
global  eol_ix      *   g_eol_ix;       // global list of eol_ix instances
global  ix_h_blk    *   index_dict;     // index structure dictionary
global  ix_h_blk    *   ixhtag[3];      // current entry for each level in index
global  ref_entry   *   ix_ref_dict;    // reference id dictionary :Ix :IHx :IREF

// page number format
global  num_style       pgnum_style[pns_max];

// symbol support
global  sym_list_entry  *   sym_list_pool;  // sym_list_entry pool
//global  sym_list_entry  *   g_sym_list;     // global stack of sym_list_entry instances

// keyboard tab support
global  uint32_t        kbtab_count;    // chars processed; used for keyboard tab expansion

// tab support
global  tab_stop    *   c_stop;         // current tab_stop
global  uint32_t        first_tab;      // first default tab position
global  uint32_t        inter_tab;      // distance between default tabs
global  char            tab_char;       // tab character from .tb
global  uint32_t        tab_col;        // width of one column, as used with tabs
global  tag_cb      *   tt_stack;       // font stack entry to modify for tab tables

// the document page and related items
global doc_element      *   doc_el_pool;        // for reuse of doc_element structs
global group_type           cur_group_type;     // current tag/cw in effect (gt_bx, gt_co not allowed)
global doc_el_group     *   cur_doc_el_group;   // current doc_el_group, if any
global doc_el_group     *   t_doc_el_group;     // stack of groups of doc_elements
global doc_el_group     *   doc_el_group_pool;  // for reuse of doc_el_group structs
global doc_element      *   t_element;          // the current element for main
global doc_page             t_page;             // for constructing output page
global doc_next_page        n_page;             // for deferred elements
global page_pos             line_position;      // left, center, right
global text_chars       *   text_pool;          // for reuse of text_chars structs
global text_line        *   t_el_last;          // attachment point to t_element
global text_line        *   t_line;             // for constructing output line
global text_line        *   line_pool;          // for reuse of text_line structs

// document section support

global gen_sect             figlist_toc;        // used with FIGLIST, TOC and eGDOC

/***************************************************************************/
/*  some globals which are to be redesigned when the :LAYOUT tag is coded. */
/*  Defined here so some script control words can be prototyped            */
/*  These are very slowly being refactored.                                */
/***************************************************************************/

global  uint32_t    g_cur_v_start;
global  uint32_t    g_page_bottom_org;
global  uint32_t    g_page_left_org;
global  uint32_t    g_page_right_org;
global  uint32_t    g_page_depth;
global  uint32_t    g_max_char_width;
global  uint32_t    g_max_line_height;
global  uint32_t    g_net_page_depth;
global  uint32_t    g_net_page_width;

global  int32_t     g_resh;             // horiz base units
global  int32_t     g_resv;             // vert base units

global  font_number g_curr_font;        // the font to use for current line
global  font_number g_phrase_font;      // the font used with SF, even if too large
global  font_number g_prev_font;        // the font used for the last text output

global  uint32_t    g_cl;               // column length
global  uint32_t    g_ll;               // line length
global  uint32_t    g_cd;               // no of columns
global  uint32_t    g_gutter;           // space between columns

global  uint32_t    g_oc_hpos;          // horizontal position for OC output

global  uint32_t    g_blank_text_lines; // blank lines (line count)
global  units_space g_blank_units_lines;// blank lines (in vertical base units)
global  uint32_t    g_post_skip;        // post_skip
global  uint32_t    g_subs_skip;        // subs_skip
global  uint32_t    g_top_skip;         // top_skip
global  text_space  g_text_spacing;     // spacing between lines (line count)
global  units_space g_units_spacing;    // spacing (in vertical base units)
global  int32_t     g_skip;             // .sk skip value (in vbus)
global  int32_t     g_space;            // .sp space value (in vbus)

global  uint32_t    post_space;         // spacing within a line
global  uint32_t    ju_x_start;         // .. formatting

global  int32_t     g_indent;           // .in 1st value (left) default 0
global  int32_t     g_indentr;          // .in 2nd value (right) default 0
global  int32_t     g_line_indent;      // :LP, :P, :PC line indent
global  int32_t     wrap_indent;        // :I1/:I2/:I3 wrap_indent value

global  int32_t     g_cur_threshold;    // current widow threshold value
                                        // from layout (widow or heading)

global  fwd_ref *   fwd_ref_pool;       // pool of unused fwd_ref instances

global  tag_cb  *   nest_cb;            // infos about nested tags
global  tag_cb  *   tag_pool;           // list of reusable tag_cbs

global  banner_lay_tag  * sect_ban_top[2];// top even / odd banner for curr sect
global  banner_lay_tag  * sect_ban_bot[2];// bot even / odd banner for curr sect

global  uint32_t    msg_indent;         // indent for message output (to screen, not to device)

global script_style_info    script_style;   // BD/US etc scope control

/***************************************************************************/
/*  tagnames as strings for msg display                                    */
/***************************************************************************/
global char str_tags[t_MAX + 1][10]
#if defined(tag_strings)
  = {
    { "NONE" },
    #define pickg( name, length, routine, gmlflags, locflags, classflags )  { #name },
    #include "gtags.h"
    #undef pickg
//    #define picklab( name, routine, flags )  extern void routine( void );
//    #define picks( name, routine, flags )  extern void routine( void );
//    #include "gscrcws.h" TBD
//    #undef picks
//    #undef picklab
    { "MAX" }
}
#endif
;

/***************************************************************************/
/* The tab lists.                                                          */
/***************************************************************************/

global  tab_list        def_tabs;   // tabs at columns 6, 11, 16, ..., 81
global  tab_list        user_tabs;  // for tabs defined by control word TB

/***************************************************************************/
/*  :LAYOUT  data                                                          */
/***************************************************************************/

global  int32_t         lay_ind;        // index into lay_tab for attribute processing
global  layout_data     layout_work;    // layout used for formatting
global  laystack    *   lay_files;      // layout file(s) specified on cmdline

/***************************************************************************/
/*  document sections for banner definition                                */
/***************************************************************************/

extern  const   ban_sections    doc_sections[max_ban];

/***************************************************************************/
/*  place names for fig and banner definition                              */
/***************************************************************************/

extern  const   ban_places    bf_places[max_place];

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
#undef tag_strings

#endif  /* GVARS_H_INCLUDED */

