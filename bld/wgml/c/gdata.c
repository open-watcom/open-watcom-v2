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
* Description:  define and initialize global variables for wgml
*
****************************************************************************/

#include    "wgml.h"
#include    "swchar.h"

#define global              // allocate storage for global vars
#define tag_strings         // initialize tag names as strings
#include "gvars.h"

#define global
#include "copfiles.h"       // allocate storage for global vars

#define global
#include "findfile.h"       // allocate storage for global vars


/***************************************************************************/
/*  Init some global variables                                             */
/***************************************************************************/

void init_global_vars( void )
{
    int     i;

    memset( &GlobalFlags, 0, sizeof( GlobalFlags ) );
    GlobalFlags.wscript = 1;            // (w)script support + warnings
    GlobalFlags.warning = 1;

    rs_loc              = 0;            // restricted location

    try_file_name[0]    = '\0';

    master_fname        = NULL;         // Master input file name
    master_fname_attr   = NULL;         // Master input file name attributes
    line_from           = FIRST_LINE;   // default first line to process
    line_to             = LAST_LINE;    // default last line to process
    g_dev_name          = NULL;         // device defined_name
    opt_fonts           = NULL;         // option FONT parameters (linked list)

    input_cbs           = NULL;         // list of active input sources
    fn_stack            = NULL;         // list of input filenames
    inc_level           = 0;            // include nesting level
    max_inc_level       = 0;            // maximum include level
    out_file            = NULL;         // output file name
    out_file_attr       = NULL;         // output file name attributes

    alt_ext             = mem_alloc( 5 );   // alternate extension   .xxx
    *alt_ext            = '\0';
    def_ext             = mem_alloc( sizeof( GML_EXT ) );
    strcpy( def_ext, GML_EXT );
    ampchar             = '&';

    gotarget[0]         = '\0';         // no .go to target yet
    gotargetno          = 0;            // no .go to target lineno

    err_count           = 0;            // total error count
    wng_count           = 0;            // total warnig count

    GML_char            = GML_CHAR_DEFAULT;     // GML start char
    SCR_char            = SCR_CHAR_DEFAULT;     // script start char
    cw_sep_char         = CW_SEP_CHAR_DEFAULT;  // script control word seperator

    CPI                 = 10;           // chars per inch
    CPI_units           = SU_chars;

    LPI                 = 6;            // lines per inch
    LPI_units           = SU_lines;

    g_resh              = 1;            // dummy value to prevent divide by zero
    g_resv              = 1;            // dummy value to prevent divide by zero

    memset( &bind_odd, 0, sizeof( bind_odd ) ); // bind value odd pages
    bind_odd.su_u       = SU_chars_lines;

    memset( &bind_even, 0, sizeof( bind_even ) );   // bind value Even pages
    bind_even.su_u      = SU_chars_lines;

    passes              = 1;            // default number of passes

    g_apage             = 0;            // absolute pageno 1 - n
    g_page              = 0;            // current pageno (in body 1 - n)
    g_line              = 0;            // current output lineno on page
    g_curr_font         = FONT0;
    g_prev_font         = FONT0;
    g_tm                = 0;            // top margin              &$tm
    g_bm                = 0;            // bottom margin           &$bm
    g_fm                = 0;            // footing margin          &$fm
    g_hm                = 0;            // heading margin          &$hm

    in_esc              = ' ';
    tab_char            = 0x09;


    box_col_set_pool    = NULL;
    box_col_stack_pool  = NULL;
    box_line            = NULL;
    g_cur_line          = NULL;
    g_prev_line         = NULL;
    max_depth           = 0;

    c_stop              = NULL;
    tt_stack            = NULL;

    block_queue         = NULL;
    block_queue_end     = NULL;

    cur_group_type      = GRT_none;
    cur_doc_el_group    = NULL;
    doc_el_group_pool   = NULL;
    t_doc_el_group      = NULL;

    t_element               = NULL;
    t_el_last               = NULL;
    t_line                  = NULL;

    t_page.page_top         = 0;
    t_page.panes_top        = 0;
    t_page.bot_ban_top      = 0;
    t_page.max_depth        = 0;
    t_page.cur_depth        = 0;
    t_page.page_left        = 0;
    t_page.page_width       = 0;
    t_page.max_width        = 0;
    t_page.cur_left         = 0;
    t_page.cur_width        = 0;
    t_page.last_pane        = mem_alloc( sizeof(doc_pane) );
    t_page.cur_col          = &t_page.last_pane->cols[0];
    t_page.last_col_main    = NULL;
    t_page.last_col_fn      = NULL;
    t_page.top_banner       = NULL;
    t_page.bottom_banner    = NULL;
    t_page.topheadsub       = NULL;
    t_page.botheadsub       = NULL;
    t_page.top_ban          = NULL;
    t_page.panes            = t_page.last_pane;
    t_page.bot_ban          = NULL;
    t_page.eol_index        = NULL;

    t_page.panes->next              = NULL;
    t_page.panes->page_width_top    = 0;
    t_page.panes->col_width_top     = 0;
    t_page.panes->col_count         = 0;
    t_page.panes->col_width         = 0;
    t_page.panes->cur_col           = 0;
    t_page.panes->page_width        = 0;

    for( i = 0; i < MAX_COL; i++ ) {
        t_page.panes->cols[i].main_top  = 0;
        t_page.panes->cols[i].fig_top   = 0;
        t_page.panes->cols[i].fn_top    = 0;
        t_page.panes->cols[i].col_left  = 0;
        t_page.panes->cols[i].col_width = NULL;
        t_page.panes->cols[i].main      = NULL;
        t_page.panes->cols[i].bot_fig   = NULL;
        t_page.panes->cols[i].footnote  = NULL;
    }

    n_page.last_fk_queue    = NULL;
    n_page.last_page_width  = NULL;
    n_page.last_col_width   = NULL;
    n_page.last_col_main    = NULL;
    n_page.last_col_bot     = NULL;
    n_page.last_col_fn      = NULL;
    n_page.fk_queue         = NULL;
    n_page.page_width       = NULL;
    n_page.col_width        = NULL;
    n_page.col_main         = NULL;
    n_page.col_bot          = NULL;
    n_page.col_fn           = NULL;
    n_page.prev_pg_depth    = 0;

    text_pool               = NULL;
    line_pool               = NULL;
    doc_el_pool             = NULL;

    line_position           = PPOS_left;

    lay_files           = NULL;         // filename(s) from ( LAYout option

    eol_ix_pool         = NULL;
    index_dict          = NULL;

    init_dict( &global_dict );
    init_macro_dict( &macro_dict );
    init_sys_dict( &sys_dict );
    init_tag_dict( &tags_dict );

    init_ref_dict( &fig_ref_dict );
    init_ref_dict( &fn_ref_dict );
    init_ref_dict( &hd_ref_dict );
    init_ref_dict( &ix_ref_dict );

    init_record_buffer( &line_buff, 80 );

    fig_fwd_refs        = NULL;
    fn_fwd_refs         = NULL;
    hd_fwd_refs         = NULL;

    fig_list            = NULL;
    fn_list             = NULL;
    hd_list             = NULL;

    pgnum_style[0]      = STYLE_h;
    pgnum_style[1]      = STYLE_h;
    pgnum_style[2]      = STYLE_h;
    pgnum_style[3]      = STYLE_h;
    pgnum_style[4]      = STYLE_h;

    g_tagname[0]        = '*';          // last defined GML tag name none
    g_tag_entry         = NULL;         // ... entry in tags_dict
    g_attname[0]        = '*';          // last defined GML attribute none
    g_att_entry         = NULL;         // ... entry in tags_dict

    research_file_name[0] = '\0';
    research_from         = 0;
    research_to           = 0;

    buff2               = mem_alloc( BUF_SIZE + 1 );
    workbuf             = mem_alloc( BUF_SIZE + 1 );

    post_space          = 0;

    g_blank_units_lines = 0;
    g_post_skip         = 0;
    g_subs_skip         = 0;
    g_top_skip          = 0;
    g_units_spacing     = 0;
    g_text_spacing      = 1;            // needed for init_next_cb()

    init_nest_cb();                     // base of stack must exist at start

    msg_indent          = 0;            // should be correct initial value

    g_script_style.font         = FONT0;// initialize BD/BI/US scope control
    g_script_style.style        = SCT_none;
    g_script_style.cw_bd.count  = 0;
    g_script_style.cw_bd.scope  = SCS_none;
    g_script_style.cw_us.count  = 0;
    g_script_style.cw_us.scope  = SCS_none;
}


/***************************************************************************/
/*  ProcFlags are initialized at each document pass start                  */
/*  As are at least some system symbols                                    */
/***************************************************************************/

void init_pass_data( void )
{
    bool        aa_save     = ProcFlags.has_aa_block;
    bool        ps_save     = ProcFlags.ps_device;
    bool        wh_save     = ProcFlags.wh_device;
    int         i;

    memset( &ProcFlags, 0, sizeof( ProcFlags ) );
    ProcFlags.blanks_allowed = 1;       // blanks during scanning
                                        // i.e. .se var  =    7
                                        // .se var=7  without
    ProcFlags.concat    = true;         // .co on default
    ProcFlags.justify   = layout_work.defaults.justify; // match LAYOUT value
    ProcFlags.doc_sect  = DSECT_none;// no document section yet
    ProcFlags.doc_sect_nxt = DSECT_none;// no document section yet
    ProcFlags.frontm_seen = false;     // FRONTM not yet seen
    ProcFlags.in_trans  = (in_esc != ' ');// translation wanted
    ProcFlags.has_aa_block = aa_save;
    ProcFlags.ps_device = ps_save;
    ProcFlags.wh_device = wh_save;

    hd_nums[HLVL_h0].headn = 0;          // reset used header headn numbers (numeric)
    hd_nums[HLVL_h1].headn = 0;
    hd_nums[HLVL_h2].headn = 0;
    hd_nums[HLVL_h3].headn = 0;
    hd_nums[HLVL_h4].headn = 0;
    hd_nums[HLVL_h5].headn = 0;
    hd_nums[HLVL_h6].headn = 0;

    hd_nums[HLVL_h0].hnumstr[0] = '\0';  // reset used header headn numbers (char)
    hd_nums[HLVL_h1].hnumstr[0] = '\0';
    hd_nums[HLVL_h2].hnumstr[0] = '\0';
    hd_nums[HLVL_h3].hnumstr[0] = '\0';
    hd_nums[HLVL_h4].hnumstr[0] = '\0';
    hd_nums[HLVL_h5].hnumstr[0] = '\0';
    hd_nums[HLVL_h6].hnumstr[0] = '\0';

    /* The heading variables are also used to set the corresponding keywords */

    if( pass == 1 ) {

        /* First pass: create the heading symbols & set them to an empty string */

        add_symvar_addr( global_dict, "$tophead", "", 0, SI_no_subscript, SF_none,
                                                        &t_page.topheadsub );
        add_symvar_addr( global_dict, "$bothead", "", 0, SI_no_subscript, SF_none,
                                                        &t_page.botheadsub );

        add_symvar_addr( global_dict, "$head0", "", 0, SI_no_subscript, SF_none,
                                                        &hd_nums[HLVL_h0].headsub );
        add_symvar_addr( global_dict, "$head1", "", 0, SI_no_subscript, SF_none,
                                                        &hd_nums[HLVL_h1].headsub );
        add_symvar_addr( global_dict, "$head2", "", 0, SI_no_subscript, SF_none,
                                                        &hd_nums[HLVL_h2].headsub );
        add_symvar_addr( global_dict, "$head3", "", 0, SI_no_subscript, SF_none,
                                                        &hd_nums[HLVL_h3].headsub );
        add_symvar_addr( global_dict, "$head4", "", 0, SI_no_subscript, SF_none,
                                                        &hd_nums[HLVL_h4].headsub );
        add_symvar_addr( global_dict, "$head5", "", 0, SI_no_subscript, SF_none,
                                                        &hd_nums[HLVL_h5].headsub );
        add_symvar_addr( global_dict, "$head6", "", 0, SI_no_subscript, SF_none,
                                                        &hd_nums[HLVL_h6].headsub );

        add_symvar_addr( global_dict, "$hnum0", "", 0, SI_no_subscript, SF_none,
                                                        &hd_nums[HLVL_h0].hnumsub );
        add_symvar_addr( global_dict, "$hnum1", "", 0, SI_no_subscript, SF_none,
                                                        &hd_nums[HLVL_h1].hnumsub );
        add_symvar_addr( global_dict, "$hnum2", "", 0, SI_no_subscript, SF_none,
                                                        &hd_nums[HLVL_h2].hnumsub );
        add_symvar_addr( global_dict, "$hnum3", "", 0, SI_no_subscript, SF_none,
                                                        &hd_nums[HLVL_h3].hnumsub );
        add_symvar_addr( global_dict, "$hnum4", "", 0, SI_no_subscript, SF_none,
                                                        &hd_nums[HLVL_h4].hnumsub );
        add_symvar_addr( global_dict, "$hnum5", "", 0, SI_no_subscript, SF_none,
                                                        &hd_nums[HLVL_h5].hnumsub );
        add_symvar_addr( global_dict, "$hnum6", "", 0, SI_no_subscript, SF_none,
                                                        &hd_nums[HLVL_h6].hnumsub );

        add_symvar_addr( global_dict, "$htext0", "", 0, SI_no_subscript, SF_none,
                                                        &hd_nums[HLVL_h0].htextsub );
        add_symvar_addr( global_dict, "$htext1", "", 0, SI_no_subscript, SF_none,
                                                        &hd_nums[HLVL_h1].htextsub );
        add_symvar_addr( global_dict, "$htext2", "", 0, SI_no_subscript, SF_none,
                                                        &hd_nums[HLVL_h2].htextsub );
        add_symvar_addr( global_dict, "$htext3", "", 0, SI_no_subscript, SF_none,
                                                        &hd_nums[HLVL_h3].htextsub );
        add_symvar_addr( global_dict, "$htext4", "", 0, SI_no_subscript, SF_none,
                                                        &hd_nums[HLVL_h4].htextsub );
        add_symvar_addr( global_dict, "$htext5", "", 0, SI_no_subscript, SF_none,
                                                        &hd_nums[HLVL_h5].htextsub );
        add_symvar_addr( global_dict, "$htext6", "", 0, SI_no_subscript, SF_none,
                                                        &hd_nums[HLVL_h6].htextsub );

        /* These never change, so can be done on the first pass only */

        strcpy( hd_nums[HLVL_h0].tag, "H0");
        strcpy( hd_nums[HLVL_h1].tag, "H1");
        strcpy( hd_nums[HLVL_h2].tag, "H2");
        strcpy( hd_nums[HLVL_h3].tag, "H3");
        strcpy( hd_nums[HLVL_h4].tag, "H4");
        strcpy( hd_nums[HLVL_h5].tag, "H5");
        strcpy( hd_nums[HLVL_h6].tag, "H6");

    } else {

        /* Second pass: set the heading variables to an empty string */

        t_page.topheadsub->value[0] = '\0';
        t_page.botheadsub->value[0] = '\0';
        for( i = 0; i < HLVL_MAX; i++ ) {
            hd_nums[i].headsub->value[0] = '\0';
            hd_nums[i].hnumsub->value[0] = '\0';
            hd_nums[i].htextsub->value[0] = '\0';
        }
    }

    g_tm = (bin_device->vertical_base_units * 6 ) / LPI;    // top margin &systm
    g_bm = g_tm;                                            // bottom margin &sysbm
    g_hm = bin_device->vertical_base_units / LPI;           // heading margin &syshm
    g_fm = g_hm;                                            // footing margin &sysfm

    g_indentl   = 0;
    g_indentr   = 0;

    g_oc_hpos = bin_device->x_start;

    figlist_toc = GENSEC_none;

    fig_count   = 0;
    fig_entry   = fig_list;     // start each pass at start of FIG list

    fn_count    = 0;
    fn_entry    = fn_list;      // start each pass at start of FN list

    hd_entry    = hd_list;      // start each pass at start of Hx list

    ixhlvl[0]   = false;        // index starts with no level 1 entry
    ixhlvl[1]   = false;        // index starts with no level 2 entry

    ixhtag[0]   = NULL;         // current level 1 entry in index
    ixhtag[1]   = NULL;         // current level 2 entry in index
    ixhtag[2]   = NULL;         // current level 3 entry in index

    kbtab_count = 0;

    new_file_parms = NULL;      // set for initial file

    CONT_char   = CONT_CHAR_DEFAULT;    // CONT start char

    g_script_style_sav.font        = FONT0; // initialize BD/BI/US scope control save
    g_script_style_sav.style       = SCT_none;
    g_script_style_sav.cw_bd.count = 0;
    g_script_style_sav.cw_bd.scope = SCS_none;
    g_script_style_sav.cw_us.count = 0;
    g_script_style_sav.cw_us.scope = SCS_none;
}
