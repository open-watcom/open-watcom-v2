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
* Description:  define and initialize global variables for wgml
*
****************************************************************************/
#include    "wgml.h"
#include    "findfile.h"
#include    "swchar.h"

#define global                          // allocate storage for global vars
#include    "gvars.h"
#undef  global


char str_tags[t_MAX + 1][10] = {
    { "NONE" },
    #define pickg( name, length, routine, gmlflags, locflags )  { #name },
    #include "gtags.h"
    #undef pickg
//    #define picks( name, routine, flags) { #name },
//    #define picklab( name, routine, flags) { #name },
//    #include "gscrcws.h" TBD
//    #undef picklab
//    #undef picks
    { "MAX" }
};

/***************************************************************************/
/*  Init some global variables                                             */
/***************************************************************************/

void init_global_vars( void )
{

    memset( &GlobalFlags, 0, sizeof( GlobalFlags ) );
    GlobalFlags.wscript = 1;            // (w)script support + warnings
    GlobalFlags.warning = 1;

    rs_loc              = 0;            // restricted location

    try_file_name       = NULL;

    master_fname        = NULL;         // Master input file name
    master_fname_attr   = NULL;         // Master input file name attributes
    line_from           = 1;            // default first line to process
    line_to             = ULONG_MAX -1; // default last line to process
    dev_name            = NULL;         // device defined_name
    opt_fonts           = NULL;         // option FONT parameters (linked list)

    input_cbs           = NULL;         // list of active input sources
    fn_stack            = NULL;         // list of input filenames
    inc_level           = 0;            // include nesting level
    max_inc_level       = 0;            // maximum include level
    out_file            = NULL;         // output file name
    out_file_attr       = NULL;         // output file name attributes

    switch_char         = _dos_switch_char();
    alt_ext             = mem_alloc( 5 );   // alternate extension   .xxx
    *alt_ext            = '\0';
    def_ext             = mem_alloc( sizeof( GML_EXT ) );
    strcpy_s( def_ext, sizeof( GML_EXT ), GML_EXT );

    gotarget[0]         = '\0';         // no .go to target yet
    gotargetno          = 0;            // no .go to target lineno

    err_count           = 0;            // total error count
    wng_count           = 0;            // total warnig count

    GML_char            = GML_CHAR_DEFAULT; // GML start char
    SCR_char            = SCR_CHAR_DEFAULT; // script start char
    CW_sep_char         = CW_SEP_CHAR_DEFAULT;// script control word seperator

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

    apage               = 0;            // absolute pageno 1 - n
    page                = 0;            // current pageno (in body 1 - n)
    line                = 0;            // current output lineno on page
    lc                  = 0;            // remaining lines on page
    g_curr_font         = 0;
    tm                  = 0;            // top margin              &$tm
    bm                  = 0;            // bottom margin           &$bm
    fm                  = 0;            // footing margin          &$fm
    fm                  = 0;            // heading margin          &$hm

    in_esc              = ' ';
    tab_char            = 0x09;

    box_col_set_pool    = NULL;
    box_line            = NULL;
    cur_line            = NULL;
    prev_line           = NULL;

    c_stop              = NULL;

    t_element               = NULL;
    t_el_last               = NULL;
    t_doc_el_group.depth    = 0;
    t_doc_el_group.first    = NULL;
    t_doc_el_group.last     = NULL;
    t_page.main_top         = 0;
    t_page.max_depth        = 0;
    t_page.cur_depth        = 0;
    t_page.last_col_main    = NULL;
    t_page.last_col_bot     = NULL;
    t_page.last_col_fn      = NULL;
    t_page.top_banner       = NULL;
    t_page.bottom_banner    = NULL;
    t_page.top_ban          = NULL;
    t_page.page_width       = NULL;
    t_page.main             = NULL;
    t_page.bot_ban          = NULL;
    n_page.last_col_top     = NULL;
    n_page.last_col_main    = NULL;
    n_page.last_col_bot     = NULL;
    n_page.last_col_fn      = NULL;
    n_page.col_top          = NULL;
    n_page.col_main         = NULL;
    n_page.col_bot          = NULL;
    n_page.col_fn           = NULL;
    t_line                  = NULL;
    text_pool               = NULL;
    line_pool               = NULL;
    doc_el_pool             = NULL;

    lay_files           = NULL;         // filename(s) from ( LAYout option

    index_dict          = NULL;
    init_ref_dict( &iref_dict );

    init_dict( &global_dict );
    init_macro_dict( &macro_dict );
    init_tag_dict( &tag_dict );
    init_sys_dict( &sys_dict );
    init_ref_dict( &ref_dict );
    init_ref_dict( &fig_dict );
    init_ref_dict( &fn_dict );

    tagname[0]          = '*';          // last defined GML tag name none
    tag_entry           = NULL;         // ... entry in tag_dict
    attname[0]          = '*';          // last defined GML attribute none
    att_entry           = NULL;         // ... entry in tag_dict

    research_file_name[0] = '\0';
    research_from         = 0;
    research_to           = 0;

    buf_size            = BUF_SIZE;
    buff2               = mem_alloc( buf_size );
    workbuf             = mem_alloc( buf_size );

    post_space          = 0;

    g_blank_lines       = 0;
    g_post_skip         = 0;
    g_subs_skip         = 0;
    g_top_skip          = 0;
    g_spacing           = 0;
    nest_cb             = NULL;         // no nested tags

    msg_indent          = 0;            // should be correct initial value
}

/***************************************************************************/
/*  ProcFlags are initialized at each document pass start                  */
/*  As are at least some system symbols                                    */
/***************************************************************************/

void init_pass_data( void )
{
    bool    flag_save   = ProcFlags.fb_document_done;
    bool    aa_save     = ProcFlags.has_aa_block;
    bool    ps_save     = ProcFlags.ps_device;

    memset( &ProcFlags, 0, sizeof( ProcFlags ) );
    ProcFlags.fb_document_done = flag_save; // keep value
    ProcFlags.blanks_allowed = 1;       // blanks during scanning
                                        // i.e. .se var  =    7
                                        // .se var=7  without
    ProcFlags.concat    = true;         // .co on default
    ProcFlags.justify   = ju_on;        // .ju on default
    ProcFlags.doc_sect  = doc_sect_none;// no document section yet
    ProcFlags.doc_sect_nxt  = doc_sect_none;// no document section yet
    ProcFlags.frontm_seen  = false;     // FRONTM not yet seen
    ProcFlags.in_trans  = (in_esc != ' ');// translation wanted
    ProcFlags.has_aa_block = aa_save;
    ProcFlags.ps_device = ps_save;
    layout_work.hx[0].headn = 0;        // reset used header headn numbers
    layout_work.hx[1].headn = 0;
    layout_work.hx[2].headn = 0;
    layout_work.hx[3].headn = 0;
    layout_work.hx[4].headn = 0;
    layout_work.hx[5].headn = 0;
    layout_work.hx[6].headn = 0;
    layout_work.hx[0].headnsub = NULL;   // and symvar values
    layout_work.hx[1].headnsub = NULL;
    layout_work.hx[2].headnsub = NULL;
    layout_work.hx[3].headnsub = NULL;
    layout_work.hx[4].headnsub = NULL;
    layout_work.hx[5].headnsub = NULL;
    layout_work.hx[6].headnsub = NULL;

    tm = (bin_device->vertical_base_units * 6 ) / LPI;  // top margin &systm
    bm = tm;                                            // bottom margin &sysbm
    hm = bin_device->vertical_base_units / LPI;         // heading margin &syshm
    fm = hm;                                            // footing margin &sysfm
    g_indent            = 0;

    ixhtag[0] = NULL;                   // last higher level :IH1 :IH2 tags
    ixhtag[1] = NULL;                   // last higher level :IH1 :IH2 tags
    ixhtag[2] = NULL;                   // last higher level :IH1 :IH2 tags
    ixhtag[3] = NULL;                   // last higher level :IH1 :IH2 tags
    free_ix_e_index_dict( &index_dict );// clear some index entries
}
