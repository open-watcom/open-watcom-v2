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
* Description:  wgml global variables.
*
****************************************************************************/


#ifndef GVARS_H_INCLUDED

#include <setjmp.h>
#include <time.h>

#include "gtype.h"
#include "gtypelay.h"

typedef att_val_type    *lay_attr_i;
typedef lay_att         lay_attr_o;

#endif  /* GVARS_H_INCLUDED */

#if !defined( GVARS_H_INCLUDED ) || defined( global )
#define GVARS_H_INCLUDED

#ifndef global
    #define global  extern
#endif

global struct tm    doc_tm;             // document time/date

global jmp_buf      *environment;       // var for GSuicide()

global tok_type     g_scandata;
global char         *new_file_parms;    // command tail for IM/AP
global char         *scan_restart;      // used by character scanning routines
global bool         g_scan_err;         // used by character scanning routines
global char         *g_tok_start;       // start of scanned token
global unsigned     arg_flen;           // arg length
global locflags     rs_loc;             // restricted location

global int          switch_char;        // DOS switch character
global char         *alt_ext;           // alternate extension
global char         *def_ext;           // default extension
global char         ampchar;            // symbol substitution/attribute/function marker

global char         *master_fname;      // Primary input file name
global char         *master_fname_attr; // Primary input file name attributes
global line_number  print_from;         // first page to print
global line_number  print_to;           // last page to print
global char         *g_dev_name;        // device defined_name
global opt_font     *opt_fonts;         // option FONT parameters (linked list)

global inputcb      *input_cbs;         // GML input stack (files + macros)
global fnstack      *fn_stack;          // input filename stack
global char         *out_file;          // output file name
global char         *out_file_attr;     // output file attributes (T:2222)
global unsigned     inc_level;          // include nesting level 1 = MasterFname
global unsigned     max_inc_level;      // maximum include level depth
global line_number  line_from;          // starting lineno to process
global line_number  line_to;            // ending lineno to process
#define LINEFROM_DEFAULT    1
#define LINETO_DEFAULT      (0x1000000) // 16 MiB lines should be enough

global char         gotarget[LABEL_NAME_LENGTH + 1]; // .go to target name
global line_number  gotargetno;         // .go to line no

global int          err_count;          // Overall Errorcount
global int          wng_count;          // Overall warning count

global char         CONT_char;          // CONTINUE char (normally 0x03)
global char         GML_char;           // GML Keywword start char (normally ":")
global char         SCR_char;           // SCRIPT keywword start char (normally ".")
global char         cw_sep_char;        // Control Word separator char (normally ";")

global int          CPI;                // chars per inch
global space_units  CPI_units;          // unit for chars per inch
global int          LPI;                // lines per inch
global space_units  LPI_units;          // unit for lines per inch

global su           bind_odd;           // Bind value for odd pages

global su           bind_even;          // Bind value for even pages

global int          passes;             // Max no of document passes
global int          pass;               // current document pass no

global unsigned     g_apage;            // current absolute pageno &$apage
global unsigned     g_page;             // current document pageno &$page
global line_number  g_line;             // current output lineno   &$line

global int          g_hm;               // heading margin          &$hm
global int          g_tm;               // top margin              &$tm

global int          g_bm;               // bottom margin           &$bm
global int          g_fm;               // footing margin          &$fm

global int          g_lm;               // left margin             &$pagelm
global int          g_rm;               // right margin            &$pagerm

global symdict_hdl  global_dict;        // global symbol dictionary
global symdict_hdl  sys_dict;           // global system symbol dictionary
global mac_dict     macro_dict;         // macro dictionary
global tag_dict     tags_dict;          // user tag dictionary

global char         research_file_name[48]; // filename for research
global line_number  research_from;      // line no start for research output
global line_number  research_to;        // line no end   for research output

global global_flags GlobalFlags;        // global flags

global proc_flags   ProcFlags;          // processing flags

global char         *token_buf;

global char         *workbuf;           // work for input buffer
global char         *buff2;             // input buffer
global unsigned     buff2_lg;           // input buffer used length

// the following to manage .gt * and .ga * * syntax
global char         g_tagname[TAG_NAME_LENGTH + 1];// last defined GML tag name
global gtentry      *g_tag_entry;       // ... entry in tags_dict
global char         g_attname[TAG_ATT_NAME_LENGTH + 1];// last defined GML attribute
global gaentry      *g_att_entry;       // ... entry in tags_dict

global int          li_cnt;             // remaining count for .li processing

global uint8_t      in_esc;             // input escape char from .ti


// file block support
global doc_el_group *block_queue;       // queue of FB blocks; blocks removed from here
global doc_el_group *block_queue_end;   // add point for new FB blocks

// box support
global box_col_set      *box_col_set_pool;  // pool of box_col_set instances
global box_col_set      *g_cur_line;        // the line from the current BX line
global box_col_set      *g_prev_line;       // the previously drawn line
global box_col_stack    *box_col_stack_pool;// pool of box_col_stack instances
global box_col_stack    *box_line;          // the current line to be drawn
global unsigned         box_col_width;      // width of one column, as used with BX
global unsigned         h_vl_offset;        // horizontal offset used to position VLINE output
global unsigned         max_depth;          // space left on page (used by BX)

// figure support
global unsigned         fig_count;      // figure number
global ffh_entry        *fig_entry;     // current fig_list entry
global fwd_ref          *fig_fwd_refs;  // forward reference/undefined id/page change
global ffh_entry        *fig_list;      // list of figures in order encountered
global record_buffer    line_buff;      // used for some frame types
global ref_entry        *fig_ref_dict;  // reference dictionary :FIG tags

// footnote support
global unsigned     fn_count;           // footnote number
global ffh_entry    *fn_entry;          // current fn_list entry
global fwd_ref      *fn_fwd_refs;       // forward reference/undefined id
global ref_entry    *fn_ref_dict;       // reference dictionary :FN tags
global ffh_entry    *fn_list;           // list of footnotes in order encountered

// heading support
global ffh_entry    *hd_entry;          // current hd_list entry
global fwd_ref      *hd_fwd_refs;       // forward reference/undefined id/page change
global hdlvl        hd_level;           // current heading level
global ref_entry    *hd_ref_dict;       // reference dictionary :Hx tags
global ffh_entry    *hd_list;           // list of headings in order encountered
global hd_num_data  hd_nums[HLVL_MAX];  // heading hierarchy numbering

// index support
global bool         ixhlvl[2];          // true for levels that exist
global eol_ix       *eol_ix_pool;       // eol_ix pool
global eol_ix       *g_eol_ix;          // global list of eol_ix instances
global ix_h_blk     *index_dict;        // index structure dictionary
global ix_h_blk     *ixhtag[3];         // current entry for each level in index
global ref_entry    *ix_ref_dict;       // reference id dictionary :Ix :IHx :IREF

// NOTE support
global unsigned     note_lm;            // left margin on entering NOTE

// page number format
global num_style    pgnum_style[PGNST_max];

// symbol support
global sym_list_entry   *sym_list_pool; // sym_list_entry pool
//global sym_list_entry   *g_sym_list;    // global stack of sym_list_entry instances

// keyboard tab support
global unsigned     kbtab_count;        // chars processed; used for keyboard tab expansion

// tab support
global tab_stop     *c_stop;            // current tab_stop
global unsigned     first_tab;          // first default tab position
global unsigned     inter_tab;          // distance between default tabs
global char         tab_char;           // tab character from .tb
global unsigned     tab_col;            // width of one column, as used with tabs
global tag_cb       *tt_stack;          // font stack entry to modify for tab tables

// the document page and related items
global doc_element      *doc_el_pool;       // for reuse of doc_element structs
global group_type       cur_group_type;     // current tag/cw in effect (gt_bx, gt_co not allowed)
global doc_el_group     *cur_doc_el_group;  // current doc_el_group, if any
global doc_el_group     *t_doc_el_group;    // stack of groups of doc_elements
global doc_el_group     *doc_el_group_pool; // for reuse of doc_el_group structs
global doc_element      *t_element;         // the current element for main
global doc_page         t_page;             // for constructing output page
global doc_next_page    n_page;             // for deferred elements
global page_pos         line_position;      // left, center, right
global text_chars       *text_pool;         // for reuse of text_chars structs
global text_line        *t_el_last;         // attachment point to t_element
global text_line        *t_line;            // for constructing output line
global text_line        *line_pool;         // for reuse of text_line structs

// document section support

global gen_sect     figlist_toc;        // used with FIGLIST, TOC and eGDOC

/***************************************************************************/
/*  some globals which are to be redesigned when the :LAYOUT tag is coded. */
/*  Defined here so some script control words can be prototyped            */
/*  These are very slowly being refactored.                                */
/***************************************************************************/

global unsigned     g_cur_v_start;
global unsigned     g_page_bottom_org;
global unsigned     g_page_left_org;
global unsigned     g_page_right_org;
global unsigned     g_page_depth;
global unsigned     g_max_char_width;
global unsigned     g_max_line_height;
global unsigned     g_net_page_depth;
global unsigned     g_net_page_width;

global int          g_resh;             // horiz base units
global int          g_resv;             // vert base units

global font_number  g_curr_font;        // the font to use for current line
global font_number  g_phrase_font;      // the font used with SF, even if too large
global font_number  g_prev_font;        // the font used for the last text output

global unsigned     g_cl;               // column length
global unsigned     g_ll;               // line length
global unsigned     g_cd;               // no of columns
global unsigned     g_gutter;           // space between columns

global unsigned     g_oc_hpos;          // horizontal position for OC output

global unsigned     g_blank_text_lines; // blank lines (line count)
global units_space  g_blank_units_lines;// blank lines (in vertical base units)
global unsigned     g_post_skip;        // post_skip
global unsigned     g_subs_skip;        // subs_skip
global unsigned     g_top_skip;         // top_skip
global text_space   g_text_spacing;     // spacing between lines (line count)
global units_space  g_units_spacing;    // spacing (in vertical base units)
global int          g_skip;             // .sk skip value (in vbus)
global int          g_space;            // .sp space value (in vbus)

global unsigned     post_space;         // spacing within a line
global unsigned     ju_x_start;         // .. formatting

global unsigned     g_indentl;          // .in 1st value (left) default 0
global unsigned     g_indentr;          // .in 2nd value (right) default 0
global unsigned     g_line_indent;      // :LP, :P, :PC line indent
global unsigned     g_wrap_indent;      // :I1/:I2/:I3 wrap_indent value

global unsigned     g_cur_threshold;    // current widow threshold value
                                        // from layout (widow or heading)

global fwd_ref      *fwd_ref_pool;      // pool of unused fwd_ref instances

global tag_cb       *nest_cb;           // infos about nested tags
global tag_cb       *tag_pool;          // list of reusable tag_cbs

global banner_lay_tag   *sect_ban_top[2];// top even / odd banner for curr sect
global banner_lay_tag   *sect_ban_bot[2];// bot even / odd banner for curr sect

global unsigned     msg_indent;         // indent for message output (to screen, not to device)

global script_style_info    g_script_style; // BD/US etc scope control
global script_style_info    g_script_style_sav; // BD/BI/US scope control saved for inline tags

/***************************************************************************/
/* The tab lists.                                                          */
/***************************************************************************/

global tab_list     def_tabs;           // tabs at columns 6, 11, 16, ..., 81
global tab_list     user_tabs;          // for tabs defined by control word TB

/***************************************************************************/
/*  :LAYOUT  data                                                          */
/***************************************************************************/

global layout_data  layout_work;        // layout used for formatting
global laystack     *lay_files;         // layout file(s) specified on cmdline

/***************************************************************************/
/*  document sections for banner definition                                */
/***************************************************************************/

extern const ban_sections   doc_sections[max_ban];

/***************************************************************************/
/*  place names for fig and banner definition                              */
/***************************************************************************/

extern const char   *const ban_places[];

/***************************************************************************/
/* Layout attribute names as character strings                             */
/*  array initialized in glconvrt.c                                        */
/*  longest attribute name is extract_threshold  (=17)                     */
/*                            ....+....1....+..                            */
/***************************************************************************/
extern const char   *const lay_att_names[];

/* Reset so can be reused with other headers. */
#undef global
#undef tag_strings

#endif  /* GVARS_H_INCLUDED */

