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
* Description:  WGML tags and routines for document section changes
*                    :GDOC, :FRONTM, :BODY, ...
*               and special for :FIGLIST :INDEX :TOC tags
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


#define CHAR_FRAME_LEN 5    // works for monospaced fonts with char width 1

static  bool            concat_save;                // for ProcFlags.concat
static  bool            ref_done;                   // true if a reference has been done
static  char            frame_line_1[CHAR_FRAME_LEN + 1]; // box top line/rule line/'character string' line
static  char            frame_line_2[CHAR_FRAME_LEN + 1]; // box blank/middle line
static  char            frame_line_3[CHAR_FRAME_LEN + 1]; // box bottom line
static  unsigned        save_indentl;               // used with TITLEP/eTITLEP
static  unsigned        save_indentr;               // used with TITLEP/eTITLEP
static  ju_enum         justify_save;               // for ProcFlags.justify
static  line_number     titlep_lineno;              // TITLEP tag line number
static  unsigned        cur_count;                  // current number of characters copied
static  unsigned        frame_line_len = CHAR_FRAME_LEN; // length of frame lines
static  unsigned        str_count;                  // IXHEAD 'character string' strlen()
static  symsub          *ixjval;                    // &sysixj value
static  symsub          *ixrefval;                  // &sysixref value
static  unsigned        cur_width;                  // current IXHEAD line width
static  unsigned        wrap[3];                    // I1/I2/I3 wrap_indent values
static  unsigned        ixh_indent;                 // IXHEAD indent
static  unsigned        str_width;                  // IXHEAD 'character string' width

static struct {
    char            *text;
    ban_docsect     ban;
} sect_info[] = {
    #define pick(text,gml,ban)  { text, ban },
    #include "docsect.h"
    #undef pick
};

/***************************************************************************/
/*  error routine for wrong sequence of doc section tags                   */
/***************************************************************************/
NO_RETURN( static void g_doc_sect_err_exit( doc_section  ds ) );

static void g_doc_sect_err_exit( doc_section  ds )
{
    err_count++;
    g_scan_err = true;
    xx_err_exit_c( ERR_DOC_SECT, sect_info[ds].text );
    /* never return */
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

    sect_ban_top[0] = sect_ban_top[1] = NULL;
    sect_ban_bot[0] = sect_ban_bot[1] = NULL;

    if( no_ban != sect_info[ds].ban ) {

        for( ban = layout_work.banner; ban != NULL; ban = ban->next ) {
            if( ban->docsect == sect_info[ds].ban ) {  // if our doc section
                switch( ban->place ) {
                case top_place :
                    sect_ban_top[0] = ban;
                    sect_ban_top[1] = ban;
                    break;
                case bottom_place :
                    sect_ban_bot[0] = ban;
                    sect_ban_bot[1] = ban;
                    break;
                case topodd_place :
                    sect_ban_top[1] = ban;
                    break;
                case topeven_place :
                    sect_ban_top[0] = ban;
                    break;
                case botodd_place :
                    sect_ban_bot[1] = ban;
                    break;
                case boteven_place :
                    sect_ban_bot[0] = ban;
                    break;
                default:
                    break;
                }
            }
        }
    }
}

/**************************************************************************/
/*  this function sets up or removes tabbing for the fill-string and page */
/*  number in the PAGELIST or TOC.                                        */
/*  Since TB only supports fill chars, only the first character of fill   */
/*  will actually be used                                                 */
/**************************************************************************/

static void figlist_toc_tabs( char * fill, unsigned size, bool setup )
{
    if( setup ) {                       // set up tabbing

        /* Set tab char to "$" */

        tab_char = '$';
        add_to_sysdir( "$tb", tab_char );
        add_to_sysdir( "$tab", tab_char );

        /************************************************************/
        /* Set the tab stops                                        */
        /* the one-column shift is normal                           */
        /************************************************************/

        user_tabs.current = 2;
        user_tabs.tabs[0].column = t_page.max_width - tab_col - size;
        user_tabs.tabs[0].fill_char = fill[0];
        user_tabs.tabs[0].alignment = ALIGN_right;

        user_tabs.tabs[1].column = t_page.max_width - tab_col;
        user_tabs.tabs[1].fill_char = ' ';
        user_tabs.tabs[1].alignment = ALIGN_right;

    } else {                        // remove tabbing (restore to default)
        tab_char = 0x09;
        add_to_sysdir( "$tb", tab_char );
        add_to_sysdir( "$tab", tab_char );

        /************************************************************/
        /* Set the tab stops                                        */
        /* the one-column shift is normal                           */
        /************************************************************/

        user_tabs.current = 0;
    }

    return;
}

/***************************************************************************/
/*  output IXHEAD with box                                                 */
/***************************************************************************/

static void gen_box_head( char * letter )
{
    doc_element *   h_box_el;       // PS only
    unsigned        full_line;      // PS only: original frame_line_len

    if( bin_driver->hline.text == NULL ) {                      // character device
        process_text( frame_line_1, layout_work.ixhead.font );  // top line
        scr_process_break();
        t_page.cur_width += ixh_indent;
        frame_line_2[2] = letter[0];
        process_text( frame_line_2, layout_work.ixhead.font );  // middle line
        scr_process_break();
        t_page.cur_width += ixh_indent;
        process_text( frame_line_3, layout_work.ixhead.font );  // bottom line
        scr_process_break();
    } else {                                            // page-oriented device

        /*******************************************************************/
        /* This uses code written originally for use with control word BX  */
        /* That control word uses depth to indicate the amount by which at */
        /* the vertical position is to be adjusted after the hline is      */
        /* emitted, as it appears in the middle of the normal line depth   */
        /* Here, the line appears at the bottom of the line depth, but the */
        /* depth used must be 0 to prevent the next element from being     */
        /* placed one line too far down on the page                        */
        /*******************************************************************/

        g_subs_skip += wgml_fonts[layout_work.ixhead.font].line_height;
        full_line = frame_line_len;
        full_line +=
            wgml_fonts[layout_work.ixhead.font].width.table[*(unsigned char *)letter];
        t_page.cur_width += frame_line_len / 2;
        process_text( letter, layout_work.ixhead.font );    // middle line
        scr_process_break();
        t_page.cur_width += ixh_indent;
        g_subs_skip += wgml_fonts[layout_work.ixhead.font].line_height;
        h_box_el = init_doc_el( ELT_dbox, 0 );               // DBOX
        h_box_el->element.dbox.h_start = t_page.cur_width;
        h_box_el->element.dbox.h_len = full_line;
        h_box_el->element.dbox.v_len = 2 * wgml_fonts[layout_work.ixhead.font].line_height;
        insert_col_main( h_box_el );
    }
    return;
}


/***************************************************************************/
/*  output IXHEAD with rule                                                */
/***************************************************************************/

static void gen_rule_head( char * letter )
{
    doc_element *   h_line_el;      // PS only
    int             i;
    unsigned        cur_limit;
    unsigned        full_line;      // PS only: original frame_line_len
    unsigned        half_line;      // one half of frame_line_len

    half_line = frame_line_len / 2;
    if( bin_driver->hline.text == NULL ) {                      // character device
        process_text( frame_line_1, layout_work.ixhead.font );  // top line
        scr_process_break();
        t_page.cur_width += ixh_indent;
        t_page.cur_width += half_line;
        process_text( letter, layout_work.ixhead.font );        // middle line
        scr_process_break();
        t_page.cur_width += ixh_indent;
        process_text( frame_line_1, layout_work.ixhead.font );  // bottom line
        scr_process_break();
    } else {                                            // page-oriented device
        full_line = frame_line_len;
        full_line += wgml_fonts[layout_work.ixhead.font].width.table[*(unsigned char *)letter];
        if( layout_work.ixhead.frame.type == FRAME_rule ) {

        /*******************************************************************/
        /* This uses code written originally for use with control word BX  */
        /* That control word uses depth to indicate the amount by which at */
        /* the vertical position is to be adjusted after the hline is      */
        /* emitted, as it appears in the middle of the normal line depth   */
        /* Here, the line appears at the bottom of the line depth, but the */
        /* depth used must be 0 to prevent the next element from being     */
        /* placed one line too far down on the page                        */
        /*******************************************************************/

            g_subs_skip += wgml_fonts[layout_work.ixhead.font].line_height;
            h_line_el = init_doc_el( ELT_hline, 0 );             // top line
            h_line_el->element.hline.ban_adjust = false;
            h_line_el->element.hline.h_start = t_page.cur_width;
            h_line_el->element.hline.h_len = full_line;
            insert_col_main( h_line_el );
            t_page.cur_width += half_line;
            process_text( letter, layout_work.ixhead.font );    // middle line
            scr_process_break();
            t_page.cur_width += ixh_indent;
            g_subs_skip += wgml_fonts[layout_work.ixhead.font].line_height;
            h_line_el = init_doc_el( ELT_hline, 0 );             // bottom line
            h_line_el->element.hline.ban_adjust = false;
            h_line_el->element.hline.h_start = t_page.cur_width;
            h_line_el->element.hline.h_len = full_line;
            insert_col_main( h_line_el );
        } else {                        // FRAME_char
            cur_limit = full_line / str_width;
            cur_count = 0;
            cur_width = 0;
            resize_record_buffer( &line_buff, cur_limit * str_count );  // length in characters
            line_buff.text[0] = '\0';
            for( i = 0; i < cur_limit; i++ ) {              // fill text with copies of full string
                strcat( line_buff.text, layout_work.ixhead.frame.string );
                cur_width += str_width;
                cur_count += str_count;
            }
            if( cur_width < full_line ) {                  // text not full yet
                for( i = 0; i < strlen( layout_work.ixhead.frame.string ); i++ ) {
                    cur_width += wgml_fonts[layout_work.ixhead.font].width.table[(unsigned char)layout_work.ixhead.frame.string[i]];
                    if( cur_width >= full_line ) {  // check what width would be if character were copied
                        break;
                    }
                    line_buff.text[cur_count] = layout_work.ixhead.frame.string[i];
                    cur_count++;
                }
            }
            line_buff.current = cur_count;
            line_buff.text[line_buff.current] = '\0';
            process_text( line_buff.text, layout_work.ixhead.font );    // top line
            scr_process_break();
            t_page.cur_width += ixh_indent;
            t_page.cur_width += half_line;
            process_text( letter, layout_work.ixhead.font );            // middle line
            scr_process_break();
            t_page.cur_width += ixh_indent;
            process_text( line_buff.text, layout_work.ixhead.font );    // bottom line
            scr_process_break();
            line_buff.current = cur_count;
            line_buff.text[line_buff.current] = '\0';
        }
    }
    return;
}


/***************************************************************************/
/*  output list of references                                              */
/***************************************************************************/

static void gen_ref_list( ix_e_blk * refs, font_number font )
{
    char            buffer[NUM2STR_LENGTH + 1];
    ix_e_blk    *   cur_ref;
    unsigned        predict     = 0;

    for( cur_ref = refs; cur_ref != NULL; cur_ref = cur_ref->next ) {
        ProcFlags.ct = true;
        post_space = 0;
        if( ref_done ) {

            /* This sequence appears to be hard-wired in wgml 4.0 */

            if( cur_ref->entry_typ == PGREF_major ) {   // major refs differ from normal refs
                process_text( ",", layout_work.ixpgnum.font );
                insert_hard_spaces( " ", 1, font );
            } else {
                process_text( ", ", font );
            }
        }

        switch( cur_ref->entry_typ ) {
        case PGREF_majorstring:
            ProcFlags.ct = true;
            post_space = wgml_fonts[font].spc_width;
            /* fall through */
        case PGREF_string:
            if( cur_ref->u.pageref.page_text[0] != '\0' ) {     // if not null string
                process_text( cur_ref->u.pageref.page_text, font );
            }
            break;
        case PGREF_start:
            format_num( cur_ref->u.pagenum.page_no, buffer, sizeof( buffer ), cur_ref->u.pagenum.style );
            process_text( buffer, font );
            ProcFlags.ct = true;
            post_space = 0;
            process_text( ixjval->value, font );
            while( cur_ref->entry_typ != PGREF_end )
                cur_ref = cur_ref->next;
            if( cur_ref == NULL ) {
                xx_simple_err_exit( ERR_OPEN_PAGE_RANGE );
                /* never return */
            }
            ProcFlags.ct = true;
            post_space = 0;
            format_num( cur_ref->u.pagenum.page_no, buffer, sizeof( buffer ), cur_ref->u.pagenum.style );
            process_text( buffer, font );
            break;
        case PGREF_end:
            xx_simple_err_exit( ERR_OPEN_PAGE_RANGE );
            /* never return */
        case PGREF_major:
            ProcFlags.ct = true;
            post_space = 0;
            /* fall through */
        case PGREF_pageno:
            format_num( cur_ref->u.pagenum.page_no, buffer, sizeof( buffer ), cur_ref->u.pagenum.style );
            process_text( buffer, font );
            if( cur_ref->next != NULL ) {                   // done if last page number
                predict = cur_ref->u.pagenum.page_no;
                predict++;
                if( (cur_ref->entry_typ != PGREF_major )
                  && (cur_ref->next->u.pagenum.page_no == predict) ) {   // sequence detected
                    ProcFlags.ct = true;
                    post_space = 0;
                    process_text( ixjval->value, font );
                    // find final page number in sequence
                    for( cur_ref = cur_ref->next; cur_ref->next != NULL; cur_ref = cur_ref->next ) {
                        predict++;
                        if( cur_ref->next->u.pagenum.page_no != predict ) {
                            break;
                        }
                    }
                    format_num( cur_ref->u.pagenum.page_no, buffer, sizeof( buffer ), cur_ref->u.pagenum.style );
                    ProcFlags.ct = true;
                    post_space = 0;
                    process_text( buffer, font );
                }
            }
            break;
        case PGREF_see:
            if( cur_ref->prt_text != NULL ) {
                process_text( cur_ref->prt_text, g_curr_font );
            } else if( cur_ref->u.pageref.page_text_len > 0 ) {  // if not null string
                process_text( cur_ref->u.pageref.page_text, g_curr_font );
            }
            break;
        default:
            internal_err_exit( __FILE__, __LINE__ );
            /* never return */
        }
        ref_done = true;
    }

    return;
}

/***************************************************************************/
/*  output list of 'see'/'see also' references                             */
/***************************************************************************/

static void gen_see_list( ix_e_blk * refs, font_number font, unsigned level,
                          bool has_sub )
{
    ix_e_blk    *   cur_ref;

    for( cur_ref = refs; cur_ref != NULL; cur_ref = cur_ref->next ) {
        scr_process_break();
        t_page.cur_width += wrap[level];
        ProcFlags.ct = true;
        post_space = 0;
        if( ref_done
          || has_sub ) {
            process_text( layout_work.index.see_also_string, font );
        } else {
            process_text( layout_work.index.see_string, font );
        }
        if( cur_ref->prt_text != NULL ) {
            process_text( cur_ref->prt_text, g_curr_font );
        } else if( cur_ref->u.pageref.page_text_len > 0 ) {  // if not null string
            process_text( cur_ref->u.pageref.page_text, g_curr_font );
        }
    }

    return;
}


/***************************************************************************/
/*  output entire set of references                                        */
/***************************************************************************/

static void gen_all_refs( entry_list * entry, unsigned level, bool has_sub )
{
    ProcFlags.concat = true;
    ref_done = false;
    if( entry->major_pgnum != NULL ) {  // first the numeric major references
        gen_ref_list( entry->major_pgnum, layout_work.ixmajor.font );
    }

    if( entry->major_string != NULL ) { // then the string major references
        gen_ref_list( entry->major_string, layout_work.ixmajor.font );
    }

    if( entry->normal_pgnum != NULL ) { // then the numeric normal references
        gen_ref_list( entry->normal_pgnum, layout_work.ixpgnum.font );
    }

    if( entry->normal_string != NULL ) {// and finally the string normal references
        gen_ref_list( entry->normal_string, layout_work.ixpgnum.font  );
    }

    if( entry->see_string != NULL ) {   // see/seeid strings go each on its own line
        gen_see_list( entry->see_string, layout_work.ix[level].string_font, level,
                      has_sub );
    }
    ProcFlags.concat = false;
    return;
}

/***************************************************************************/
/*  set up the columns                                                     */
/*                                                                         */
/*  matches wgml 4.0 for 1 column                                          */
/*  matches wgml 4.0 for 2 columns generally                               */
/*  matches wgml 4.0 for 2 column INDEX                                    */
/*  may or may not match wgml 4.0 for more than 2 columns                  */
/*    particularly when script is used to create the columns               */
/***************************************************************************/

static void set_cols( doc_pane * a_pane )
{
    int         i;
    unsigned    col_count;
    unsigned    cur_col;
    unsigned    gutter;
    unsigned    width_avail;

    col_count = a_pane->col_count;
    if( col_count == 1 ) {
        a_pane->col_width = t_page.page_width;
        a_pane->cols[0].col_left = t_page.page_left;
        t_page.max_width = t_page.page_width;
    } else {
        gutter = layout_work.defaults.def_gutter;
        width_avail = t_page.page_width - (gutter * (col_count - 1));
        a_pane->col_width = width_avail / col_count;
        a_pane->cols[0].col_left = t_page.page_left;
        for( i = 1; i < col_count; i++ ) {
            cur_col = a_pane->cols[i - 1].col_left;
            cur_col += a_pane->col_width;
            cur_col += gutter;
            a_pane->cols[i].col_left = cur_col;
        }
        if( ProcFlags.doc_sect_nxt == DSECT_index ) {    // INDEX-specific
            a_pane->col_width = 27.5 * tab_col;             // empirical, to match wgml 4.0
        }
        t_page.max_width = a_pane->col_width;
    }
    return;
}

/***************************************************************************/
/*  output FIGLIST                                                         */
/***************************************************************************/

static void gen_figlist( void )
{
    char            buffer[NUM2STR_LENGTH + 1];
    char            postfix[NUM2STR_LENGTH + 1 + 1];
    ffh_entry   *   curr;
    unsigned        size;

    if( fig_list == NULL )              // no fig_list, no FIGLIST
        return;

    /* Insert FIGLIST into current section */

    ProcFlags.start_section = true;     // prevent ABSTRACT/PREFACE start if pending
    last_page_out();                    // ensure are on new page
    g_skip = 0;                         // ignore remaining skip value
    set_section_banners( DSECT_figlist );
    reset_t_page();

    /* Set FIGLIST columns */

    t_page.panes->col_count = layout_work.figlist.columns;
    set_cols( t_page.panes );           // will need to be updated if multiple panes activated

    /* Set FIGLIST margins and other values */

    t_page.cur_left = 2 * conv_hor_unit( &layout_work.figlist.left_adjust, g_curr_font );    // matches wgml 4.0
    t_page.max_width = t_page.page_width - conv_hor_unit( &layout_work.figlist.right_adjust, g_curr_font );
    size = conv_hor_unit( &layout_work.flpgnum.size, g_curr_font );  // space from fill to right edge

    /* Output FIGLIST */

    concat_save = ProcFlags.concat;
    ProcFlags.concat = true;
    ProcFlags.in_figlist_toc = true;
    justify_save = ProcFlags.justify;
    ProcFlags.justify = JUST_off;
    ProcFlags.keep_left_margin = true;  // keep all indents while outputting text
    for( curr = fig_list; curr != NULL; curr = curr->next ) {
        if( curr->flags & FFH_figcap ) {    // no FIGCAP used, no FIGLIST output
            g_curr_font = FONT0;            // wgml 4.0 uses font 0
            if( ProcFlags.col_started ) {   // not on first entry
                g_text_spacing = layout_work.figlist.spacing;
                set_skip_vars( NULL, NULL, NULL, g_text_spacing, g_curr_font );
            }
            t_page.cur_left = 0;
            t_page.cur_width = t_page.cur_left;
            process_text( curr->prefix, g_curr_font );  // caption prefix

            if( curr->text != NULL ) {
                t_page.cur_left = t_line->last->x_address + t_line->last->width +
                                  wgml_fonts[g_curr_font].spc_width;
                t_page.cur_width = t_page.cur_left;
                ProcFlags.ct = true;                // emulate CT
                ProcFlags.as_text_line = true;      // suppress 2nd space after stops
                post_space = 0;
                t_page.max_width -= size;
                if( ProcFlags.has_aa_block ) {      // matches wgml 4.0
                    t_page.max_width -= tab_col;
                } else {
                    t_page.max_width -= 3 * tab_col;
                }
                if( !ProcFlags.col_started ) {      // first entry wrapping
                    g_text_spacing = layout_work.figlist.spacing;
                }
                set_skip_vars( &layout_work.figlist.skip, NULL, NULL,
                               g_text_spacing, g_curr_font );
                process_text( curr->text, g_curr_font );// caption text
                if( ProcFlags.has_aa_block ) {       // matches wgml 4.0
                    t_page.max_width += tab_col;
                } else {
                    t_page.max_width += 3 * tab_col;
                }
                t_page.max_width += size;
            }
            ProcFlags.ct = true;                    // emulate CT
            g_curr_font = FONT0;
            figlist_toc_tabs( layout_work.figlist.fill_string, size, true );
            process_text( "$", g_curr_font );
            format_num( curr->pageno, buffer, sizeof( buffer ), curr->style );
            strcpy( postfix, "$" );           // insert tab characters
            strcat( postfix, buffer );        // append page number
            g_curr_font = layout_work.flpgnum.font;
            process_text( postfix, g_curr_font );
            figlist_toc_tabs( layout_work.figlist.fill_string, size, false );
        }
        scr_process_break();                // ensure line break
    }

    ProcFlags.concat = concat_save;
    ProcFlags.in_figlist_toc = false;
    ProcFlags.justify = justify_save;

    /* Re-establish current section */

    last_page_out();                // ensure all pages output
    set_section_banners( ProcFlags.doc_sect_nxt );
    reset_t_page();

    return;
}

/***************************************************************************/
/*  output INDEX                                                           */
/***************************************************************************/

static void gen_index( void )
{
    bool            first[3];           // tag first index entry in group
    bool            in_trans_sav;
    char            letter[2];
    int             i;
    ix_h_blk    *   ixh1;
    ix_h_blk    *   ixh2;
    ix_h_blk    *   ixh3;
    unsigned        cur_limit;
    unsigned        indent[3];          // I1/I2/I3 cumulative indents
    unsigned        spc_count;

    scr_process_break();                // flush any pending text
    start_doc_sect();                   // emit heading regardless of pass

    /* No index is output without an index_dict or before the last pass */

    if( !GlobalFlags.lastpass
      || (index_dict == NULL) )
        return;

    in_trans_sav = ProcFlags.in_trans;

    /* Set up for device/frame selected by the LAYOUT tag IXHEAD*/

    if( layout_work.ixhead.frame.type != FRAME_none ) {
        if( layout_work.ixhead.frame.type == FRAME_box ) {  // frame is box
            if( bin_driver->dbox.text == NULL ) {           // character device
                memset( &frame_line_1[1], bin_device->box.chars.horizontal_line, frame_line_len - 2 );
                frame_line_1[0] = bin_device->box.chars.top_left;
                frame_line_1[frame_line_len - 1 ] = bin_device->box.chars.top_right;
                frame_line_2[0] = bin_device->box.chars.vertical_line;
                memset( &frame_line_2[1], ' ', frame_line_len - 2 );
                frame_line_2[frame_line_len - 1 ] = bin_device->box.chars.vertical_line;
                memset( &frame_line_3[1], bin_device->box.chars.horizontal_line, frame_line_len - 2 );
                frame_line_3[0] = bin_device->box.chars.bottom_left;
                frame_line_3[frame_line_len - 1 ] = bin_device->box.chars.bottom_right;
            }
        } else if( layout_work.ixhead.frame.type == FRAME_rule ) { // rule frame
            if( bin_driver->hline.text == NULL ) {          // character device
                memset( frame_line_1, bin_device->box.chars.horizontal_line, frame_line_len );
            }
        } else if( layout_work.ixhead.frame.type == FRAME_char ) {   // frame is 'character string'
            str_count = strlen( layout_work.ixhead.frame.string );
            str_width = 0;
            for( i = 0; i < strlen( layout_work.ixhead.frame.string ); i++ ) {
                str_width += wgml_fonts[layout_work.ixhead.font].width.table[(unsigned char)layout_work.ixhead.frame.string[i]];
            }
            if( bin_driver->hline.text == NULL ) {          // character device
                frame_line_1[0] = '\0';
                cur_limit = frame_line_len / str_count;     // number of complete strings that will fit
                cur_width = 0;
                for( i = 0; i < cur_limit; i++  ) {         // fill text with full string
                    strcat( frame_line_1, layout_work.ixhead.frame.string );
                    cur_width += str_count;
                }
                if( cur_width < frame_line_len ) {          // text not full yet
                    strncpy( &frame_line_1[cur_width], layout_work.ixhead.frame.string, frame_line_len - cur_width );
                }
                frame_line_1[frame_line_len] = '\0';
            }
        } else {                                        // shouldn't be here
            internal_err_exit( __FILE__, __LINE__ );
            /* never return */
        }
    }

    /* Initialize values used in outputting the index */

    letter[0]  = 0;
    letter[1]  = 0;

    indent[0] = conv_hor_unit( &layout_work.ix[0].indent, layout_work.ix[0].font );
    indent[1] = indent[0] + conv_hor_unit( &layout_work.ix[1].indent, layout_work.ix[1].font );
    if( ProcFlags.has_aa_block ) {
        indent[1]++;    // to match wgml 4.0 with PS
    }
    indent[2] = indent[1] + conv_hor_unit( &layout_work.ix[2].indent, layout_work.ix[2].font );
    if( ProcFlags.has_aa_block ) {
        indent[2]++;    // to match wgml 4.0 with PS
    }

    ixh_indent = indent[0] + conv_hor_unit( &layout_work.ixhead.indent, layout_work.ixhead.font );
    find_symvar( sys_dict, "$ixj", SI_no_subscript, &ixjval );
    find_symvar( sys_dict, "$ixref", SI_no_subscript, &ixrefval );

    t_page.cur_left += conv_hor_unit( &layout_work.index.left_adjust, layout_work.ixhead.font );
    t_page.cur_width = t_page.cur_left;

    t_page.max_width -= (conv_hor_unit( &layout_work.index.right_adjust, layout_work.ixhead.font ) * 9) / 10;
    t_page.max_width += tab_col;    // matches wgml 4.0

    wrap[0] = indent[0] +
            conv_hor_unit( &layout_work.ix[0].wrap_indent, layout_work.ix[0].font );
    wrap[1] = indent[1] +
            conv_hor_unit( &layout_work.ix[1].wrap_indent, layout_work.ix[1].font );
    wrap[2] = indent[2] +
            conv_hor_unit( &layout_work.ix[2].wrap_indent, layout_work.ix[2].font );

    concat_save = ProcFlags.concat;
    ProcFlags.concat = false;
    justify_save = ProcFlags.justify;
    ProcFlags.justify = JUST_off;

    /* Output the index */

    /*
     * level 1
     */
    first[0] = true;
    for( ixh1 = index_dict; ixh1 != NULL; ixh1 = ixh1->next ) {
        if( letter[0] != my_toupper( *(ixh1->ix_term) ) ) {
            /*
             * Set g_subs_skip for IXHEAD
             */
            set_skip_vars( &layout_work.ixhead.pre_skip, NULL, NULL, g_text_spacing,
                           layout_work.ixhead.font );
            /*
             * Generate IXHEAD heading
             */
            letter[0] = my_toupper( *(ixh1->ix_term) );
            /*
             * The factor 4.6 is empirical and chosen to match wgml 4.0
             */
            if( ((4.6 * wgml_fonts[layout_work.ixhead.font].line_height)
                   + g_subs_skip + t_page.cur_depth) >= t_page.max_depth ) {
                next_column();
            }

            if( layout_work.ixhead.header ) {
                spc_count = wgml_fonts[layout_work.ixhead.font].width.table[*(unsigned char *)letter] /
                            wgml_fonts[layout_work.ixhead.font].spc_width;  // integer ratio
                if( (wgml_fonts[layout_work.ixhead.font].width.table[*(unsigned char *)letter] %
                            wgml_fonts[layout_work.ixhead.font].spc_width) > 0 ) {
                    spc_count++;                            // increment unless exact ratio
                }
                frame_line_len = (2 + (2 * spc_count)) *
                    wgml_fonts[layout_work.ixhead.font].spc_width;  // length without letter width
                t_page.cur_width += ixh_indent;

                switch( layout_work.ixhead.frame.type ) {
                case FRAME_none:
                    process_text( letter, layout_work.ixhead.font );
                    break;
                case FRAME_box:
                    if( !ProcFlags.col_started ) {      // at top of page
                        g_top_skip = wgml_fonts[layout_work.ixhead.font].line_height;
                    }
                    gen_box_head( letter );
                    break;
                case FRAME_rule:
                    if( !ProcFlags.col_started ) {      // at top of page
                        g_top_skip = wgml_fonts[layout_work.ixhead.font].line_height;
                    }
                    /* fall through */
                case FRAME_char:                   // no top-of-page correction in wgml 4.0
                    gen_rule_head( letter );
                    break;
                default:
                    internal_err_exit( __FILE__, __LINE__ );
                    /* never return */
                }
                scr_process_break();        // flush letter heading
            }

            /* Set g_post_skip for IXHEAD */

            set_skip_vars( NULL, NULL, &layout_work.ixhead.post_skip, g_text_spacing,
                           layout_work.ixhead.font );
            g_subs_skip += g_post_skip;     // added, not combined
            g_post_skip = 0;
        }

        if( first[0] ) {                    // first entry in group
            g_text_spacing = layout_work.hx.hx_sect[HDS_index].spacing;
            set_skip_vars( &layout_work.ix[0].skip, NULL, NULL,
                           g_text_spacing, layout_work.ix[0].font );
            first[0] = false;
        } else {
            g_text_spacing = 1;
            set_skip_vars( &layout_work.ix[0].pre_skip, NULL,
                           &layout_work.ix[0].post_skip,
                           g_text_spacing, layout_work.ix[0].font );
        }
        t_page.cur_width += indent[0];
        ProcFlags.wrap_indent = true;
        g_wrap_indent = wrap[0];
        if( ixh1->prt_term == NULL ) {
            ProcFlags.in_trans = false;         // turn off input translation
            process_text( ixh1->ix_term, layout_work.ix[0].font );
        } else {
            if( ixh1->prt_term[0] != '\0' ) {   // if not null string
                ProcFlags.in_trans = true;      // allow input translation
                process_text( ixh1->prt_term, layout_work.ix[0].font );
            }
        }

        if( ixh1->entry != NULL ) {
            post_space = 0;
            if( (ixh1->entry->major_pgnum != NULL)
              || (ixh1->entry->major_string != NULL)
              || (ixh1->entry->normal_pgnum != NULL)
              || (ixh1->entry->normal_string != NULL) ) {    // see_string alone doesn't need the index_delim
                ProcFlags.in_trans = false;         // turn off input translation
                process_text( layout_work.ix[0].index_delim, layout_work.ix[0].font );
            }
            gen_all_refs( ixh1->entry, 0, ixh1->lower != NULL );
        }
        post_space = 0;
        scr_process_break();

        /*
         * level 2
         */
        first[1] = true;
        for( ixh2 = ixh1->lower; ixh2 != NULL; ixh2 = ixh2->next ) {
            if( first[1] ) {                    // first entry in group
                g_text_spacing = layout_work.hx.hx_sect[HDS_index].spacing;
                set_skip_vars( &layout_work.ix[1].skip, NULL, NULL,
                               g_text_spacing, layout_work.ix[1].font );
                first[1] = false;
            } else {
                g_text_spacing = 1;
                set_skip_vars( &layout_work.ix[1].pre_skip, NULL,
                               &layout_work.ix[1].post_skip,
                               g_text_spacing, layout_work.ix[1].font );
            }
            t_page.cur_width += indent[1];
            ProcFlags.wrap_indent = true;
            g_wrap_indent = wrap[1];
            if( ixh2->prt_term == NULL ) {
                ProcFlags.in_trans = false;         // turn off input translation
                process_text( ixh2->ix_term, layout_work.ix[1].font );
            } else {
                if( ixh2->prt_term[0] != '\0' ) { // if not null string
                    ProcFlags.in_trans = true;      // allow input translation
                    process_text( ixh2->prt_term, layout_work.ix[1].font );
                }
            }

            if( ixh2->entry != NULL ) {
                post_space = 0;
                if( (ixh2->entry->major_pgnum != NULL)
                  || (ixh2->entry->major_string != NULL)
                  || (ixh2->entry->normal_pgnum != NULL)
                  || (ixh2->entry->normal_string != NULL) ) {    // see_string alone doesn't need the index_delim
                    ProcFlags.in_trans = false;         // turn off input translation
                    process_text( layout_work.ix[1].index_delim, layout_work.ix[1].font );
                }
                gen_all_refs( ixh2->entry, 1, ixh2->lower != NULL );
            }
            post_space = 0;
            scr_process_break();

            /*
             * level 3
             */
            first[2] = true;
            for( ixh3 = ixh2->lower; ixh3 != NULL; ixh3 = ixh3->next ) {
                if( first[2] ) {                    // first entry in group
                    g_text_spacing = layout_work.hx.hx_sect[HDS_index].spacing;
                    set_skip_vars( &layout_work.ix[2].skip, NULL, NULL,
                                   g_text_spacing, layout_work.ix[2].font );
                    first[2] = false;
                } else {
                    g_text_spacing = 1;
                    set_skip_vars( &layout_work.ix[2].pre_skip, NULL,
                                   &layout_work.ix[2].post_skip,
                                   g_text_spacing, layout_work.ix[2].font );
                }
                t_page.cur_width += indent[2];
                ProcFlags.wrap_indent = true;
                g_wrap_indent = wrap[2];
                if( ixh3->prt_term == NULL ) {
                    ProcFlags.in_trans = false;         // turn off input translation
                    process_text( ixh3->ix_term, layout_work.ix[2].font );
                } else {
                    if( ixh3->prt_term[0] != '\0' ) { // if not null string
                        ProcFlags.in_trans = true;      // allow input translation
                        process_text( ixh3->prt_term, layout_work.ix[2].font );
                    }
                }

                if( ixh3->entry != NULL ) {
                    post_space = 0;
                    if( (ixh3->entry->major_pgnum != NULL)
                      || (ixh3->entry->major_string != NULL)
                      || (ixh3->entry->normal_pgnum != NULL)
                      || (ixh3->entry->normal_string != NULL) ) {    // see_string alone doesn't need the index_delim
                        ProcFlags.in_trans = false;         // turn off input translation
                        process_text( layout_work.ix[2].index_delim, layout_work.ix[2].font );
                    }
                    gen_all_refs( ixh3->entry, 2, ixh3->lower != NULL );
                }
                post_space = 0;
                scr_process_break();
            }
            if( !first[2] ) {   // at least one I3 entry existed
                first[1] = true;
            }
        }
        if( !first[1] ) {   // at least one I2 entry existed
            first[0] = true;
        }
    }
    ProcFlags.concat = concat_save;
    ProcFlags.in_trans = in_trans_sav;
    ProcFlags.justify = justify_save;
}

/***************************************************************************/
/*  output TOC                                                             */
/*  Note: these attributes appear to have no effect:                       */
/*        TOCHn attribute align                                            */
/*        TOCHn attribute display_in_toc                                   */
/*  Note: this attribute is ignored (but may have an effect - TBD):        */
/*        TOCHn attribute group                                            */
/***************************************************************************/

static void gen_toc( void )
{
    bool            levels[HLVL_MAX];              // track levels
    char            buffer[NUM2STR_LENGTH + 1];
    char            postfix[NUM2STR_LENGTH + 1 + 1];
    ffh_entry   *   curr;
    hdlvl           hlvl;
    hdlvl           hlvl2;
    hdlvl           cur_level;
    unsigned        indent[HLVL_MAX];
    unsigned        size;

    if( hd_list == NULL )
        return;                         // no hd_list, no TOC

    /* Insert TOC into current section */

    ProcFlags.start_section = true;     // prevent ABSTRACT/PREFACE start if pending
    last_page_out();                    // ensure are on new page
    g_skip = 0;                         // ignore remaining skip value
    set_section_banners( DSECT_toc );
    reset_t_page();

    /* Set TOC columns */

    t_page.panes->col_count = layout_work.toc.columns;
    set_cols( t_page.panes );           // will need to be updated if multiple panes activated

    /* Set TOC margins and other values */

    t_page.cur_left = 2 * conv_hor_unit( &layout_work.toc.left_adjust, g_curr_font );    // matches wgml 4.0
    t_page.max_width = t_page.page_width -
                   conv_hor_unit( &layout_work.toc.right_adjust, g_curr_font );
    size = conv_hor_unit( &layout_work.tocpgnum.size, g_curr_font );     // space from fill to right edge

    /* Initialize levels and indent values */

    for( hlvl = 0; hlvl < HLVL_MAX; hlvl++ ) {
        levels[hlvl] = false;
        indent[hlvl] = 0;
    }
    levels[HLVL_h0] = true;             // H0 is active at start

    /* Get converted indent values, which are cumulative */

    for( hlvl = 0; hlvl < HLVL_MAX; hlvl++ ) {
        for( hlvl2 = hlvl; hlvl2 < HLVL_MAX; hlvl2++ ) {
            indent[hlvl2] += conv_hor_unit( &layout_work.tochx[hlvl].indent, g_curr_font );
        }
    }

    /* Output TOC */

    concat_save = ProcFlags.concat;
    ProcFlags.concat = true;
    ProcFlags.in_figlist_toc = true;
    justify_save = ProcFlags.justify;
    ProcFlags.justify = JUST_off;
    ProcFlags.keep_left_margin = true;  // keep all indents while outputting text
    for( curr = hd_list; curr != NULL; curr = curr->next ) {
        cur_level = curr->number;
        for( hlvl = 0; hlvl < HLVL_MAX; hlvl++ ) {
            if( hlvl > cur_level ) {    // all lower levels are inactive
                levels[hlvl] = false;
            }
        }
        if( cur_level < layout_work.toc.toc_levels ) {
            g_curr_font = layout_work.tochx[cur_level].font;
            if( levels[cur_level] && !curr->abs_pre ) {
                g_text_spacing = layout_work.toc.spacing;
                set_skip_vars( &layout_work.tochx[cur_level].skip, NULL, NULL,
                               g_text_spacing, g_curr_font );
            } else {
                g_text_spacing = 1;
                set_skip_vars( &layout_work.tochx[cur_level].pre_skip, NULL,
                               &layout_work.tochx[cur_level].post_skip,
                               g_text_spacing, g_curr_font );
            }
            t_page.cur_left = indent[cur_level];
            t_page.cur_width = t_page.cur_left;

            if( curr->flags & FFH_prefix ) {
                process_text( curr->prefix, g_curr_font );
                t_page.cur_left = t_line->last->x_address + t_line->last->width +
                                  wgml_fonts[g_curr_font].spc_width;
                t_page.cur_width = t_page.cur_left;
                ProcFlags.ct = true;                // emulate CT
                post_space = 0;
            } else {
                t_line = alloc_text_line();         // capture spacing if no prefix
            }
            if( !levels[cur_level] ) {
                g_text_spacing = layout_work.toc.spacing;
                set_skip_vars( NULL, NULL, NULL, g_text_spacing, g_curr_font );
            }
            if( curr->text != NULL ) {
                t_page.max_width -= size;
                if( ProcFlags.has_aa_block ) {      // matches wgml 4.0
                    t_page.max_width -= tab_col;
                } else {
                    t_page.max_width -= 3 * tab_col;
                }
                ProcFlags.as_text_line = true;      // suppress 2nd space after stop
                process_text( curr->text, g_curr_font );
                if( ProcFlags.has_aa_block ) {      // matches wgml 4.0
                    t_page.max_width += tab_col;
                } else {
                    t_page.max_width += 3 * tab_col;
                }
                t_page.max_width += size;
            }
            ProcFlags.ct = true;                    // emulate CT
            g_curr_font = FONT0;
            figlist_toc_tabs( layout_work.toc.fill_string, size, true );
            process_text( "$", g_curr_font );
            format_num( curr->pageno, buffer, sizeof( buffer ), curr->style );
            strcpy( postfix, "$" );           // insert tab characters
            strcat( postfix, buffer );        // append page number
            g_curr_font = layout_work.tocpgnum.font;
            process_text( postfix, g_curr_font );
            figlist_toc_tabs( layout_work.toc.fill_string, size, false );
        }
        scr_process_break();                        // ensure line break
        levels[cur_level] = true;                   // first entry of level done
    }

    ProcFlags.concat = concat_save;
    ProcFlags.in_figlist_toc = false;
    ProcFlags.justify = justify_save;

    /* Re-establish current section */

    last_page_out();                // ensure all pages output
    set_section_banners( ProcFlags.doc_sect_nxt );
    reset_t_page();

    return;
}

/***************************************************************************/
/*  set new vertical position depending on banner existance                */
/***************************************************************************/

static void document_new_position( void )
{
    region_lay_tag  *   top_reg;
    unsigned            top_pos;

    /* The value used for fb_position() should be the first line of text,
     * even if that is from a TITLE tag (top banners do not apply to TITLEP).
     * This handles documents starting with banners and text only.
     */

    if( sect_ban_top[g_page & 1] != NULL ) {
        top_reg = sect_ban_top[g_page & 1]->by_line->first;
        if( bin_driver->y_positive == 0x00 ) {
            top_pos = t_page.page_top - top_reg->reg_depth;
        } else {
            top_pos = t_page.page_top + top_reg->reg_depth;
        }
        g_cur_v_start = top_pos;
    } else {
        top_pos = t_page.panes_top;
        if( bin_driver->y_positive == 0x00 ) {
            g_cur_v_start = t_page.panes_top - wgml_fonts[FONT0].line_height;
        } else {
            g_cur_v_start = t_page.panes_top + wgml_fonts[FONT0].line_height;
        }
    }
    t_page.cur_width = 0;

    if( GlobalFlags.lastpass ) {
        if( ProcFlags.fb_position_done ) {
            fb_new_section( g_cur_v_start );
        } else {
            fb_position( t_page.cur_width, g_cur_v_start );
            ProcFlags.fb_position_done = true;
        }
    }
    g_cur_v_start = top_pos; // reset so first line positioning is correct
    return;
}

/***************************************************************************/
/*  start_doc_sect true section start                                      */
/*  Note: not used with FIGLIST or TOC, as these are not sections but      */
/*        rather are inserted into sections                                */
/***************************************************************************/

void start_doc_sect( void )
{
    bool                clear_banners;
    bool                first_section;
    bool                header;
    bool                lvl_reset;
    bool                page_r;         // page reset
    char            *   h_text;
    doc_section         ds;
    hdsrc               hds_lvl;
    hdlvl               hlvl;
    page_ej             page_e;         // page eject tag
    unsigned            page_c;         // page columns
    text_space          page_s;         // page spacing


    if( ProcFlags.start_section ) {
        return;                         // once is enough
    }
    if( !ProcFlags.fb_document_done ) { // the very first section/page
        do_layout_end_processing();
    }

    scr_process_break();                // commit any prior text

    first_section = (ProcFlags.doc_sect == DSECT_none);

    ProcFlags.start_section = true;
    ProcFlags.keep_left_margin = false;

    page_c = layout_work.defaults.columns;
    ds = ProcFlags.doc_sect_nxt;        // new section

    if( ds == DSECT_none ) {
        ds = DSECT_body;                // if text without section start assume body
    }

    /***********************************************************************/
    /*  process special section attributes                                 */
    /***********************************************************************/

    clear_banners = false;
    switch( ds ) {
    case DSECT_titlep:
        page_c = layout_work.titlep.columns;
        page_e = ej_yes;
        page_r = false;                 // no page number reset
        page_s = layout_work.titlep.spacing;
        header = false;                 // no header string output
        lvl_reset = false;
        init_nest_cb();
        nest_cb->p_stack = copy_to_nest_stack();
        nest_cb->gtag = T_TITLEP;
        nest_cb->p_stack->lineno = titlep_lineno; // correct line number
        break;
    case DSECT_abstract:
        hd_level = HLVL_h1;              // H0 and H1 treated as already present
        page_c = layout_work.abstract.columns;
        page_e = layout_work.abstract.page_eject;
        page_r = layout_work.abstract.page_reset;
        page_s = layout_work.hx.hx_sect[HDS_abstract].spacing;
        header = layout_work.hx.hx_sect[HDS_abstract].header;
        if( header ) {
            h_text = layout_work.abstract.string;
            hds_lvl = HDS_abstract;
        }
        lvl_reset = false;
        break;
    case DSECT_preface:
        hd_level = HLVL_h1;              // H0 and H1 treated as already present
        page_c = layout_work.preface.columns;
        page_e = layout_work.preface.page_eject;
        page_r = layout_work.preface.page_reset;
        page_s = layout_work.hx.hx_sect[HDS_preface].spacing;
        header = layout_work.hx.hx_sect[HDS_preface].header;
        if( header ) {
            h_text = layout_work.preface.string;
            hds_lvl = HDS_preface;
        }
        lvl_reset = false;
        break;
    case DSECT_body:
        hd_level = HLVL_force_h0;        // force H0 to be used
        page_c = layout_work.body.columns;
        page_e = layout_work.body.page_eject;
        page_r = layout_work.body.page_reset;
        page_s = layout_work.hx.hx_sect[HDS_body].spacing;
        header = layout_work.hx.hx_sect[HDS_body].header;
        if( header ) {
            h_text = layout_work.body.string;
            hds_lvl = HDS_body;
        }
        lvl_reset = true;
        break;
    case DSECT_appendix:
        hd_level = HLVL_h0;              // H0 treated as already present
        page_c = layout_work.appendix.columns;
        page_e = layout_work.appendix.section_eject;
        page_r = layout_work.appendix.page_reset;
        page_s = layout_work.hx.hx_sect[HDS_appendix].spacing;
        header = false;                 // no section header string output, as such
        if( page_e != ej_no ) {
            page_e = ej_yes;            // "even" and "odd" act like "yes"
        }
        lvl_reset = true;
        break;
    case DSECT_backm:
        hd_level = HLVL_h0;              // H0 treated as already present
        page_c = layout_work.backm.columns;
        page_e = layout_work.backm.page_eject;
        page_r = layout_work.backm.page_reset;
        page_s = layout_work.hx.hx_sect[HDS_backm].spacing;
        header = layout_work.hx.hx_sect[HDS_backm].header;
        if( header ) {
            h_text = layout_work.backm.string;
            hds_lvl = HDS_backm;
        }
        lvl_reset = true;
        break;
    case DSECT_index:
        clear_banners = true;
        page_c = layout_work.index.columns;
        page_e = layout_work.index.page_eject;
        page_r = layout_work.index.page_reset;
        page_s = layout_work.hx.hx_sect[HDS_index].spacing;
        header = layout_work.hx.hx_sect[HDS_index].header;
        if( header ) {
            h_text = layout_work.index.index_string;
            hds_lvl = HDS_index;
        }
        lvl_reset = false;
        break;
    case DSECT_gdoc:
    case DSECT_etitlep:
    case DSECT_frontm:
        page_c = layout_work.defaults.columns;
        page_e = ej_no;                         // no page eject
        page_r = false;                         // no page number reset
        page_s = layout_work.defaults.spacing;  // default spacing
        header = false;                         // no section header
        lvl_reset = false;
        break;
    case DSECT_egdoc:
        page_c = 1;                             // as per wgml 4.0
        page_e = ej_odd;                        // as per wgml 4.0
        page_r = false;                         // no page number reset
        page_s = layout_work.defaults.spacing;  // default spacing
        header = false;                         // no section header
        lvl_reset = false;
        break;
    default:
        internal_err_exit( __FILE__, __LINE__ );
        /* never return */
    }

    /***********************************************************************/
    /*  set up/move to the next section                                    */
    /***********************************************************************/

    if( first_section ) {               // nothing precedes the first section
        set_section_banners( ds );
        reset_t_page();
        document_new_position();        // page is now ready for output
        if( page_e == ej_even ) {
            do_page_out();              // apage since first page is odd
            g_page = 0;                 // restart page for first text page
            ProcFlags.page_ejected = true;
        }
    } else if( page_e == ej_no ) {
        full_col_out();                 // ensure are in last column
        ProcFlags.page_ejected = false; // this would be a good place to add a pane
        set_section_banners( ds );      // since the section begins mid-page
        reset_bot_ban();
    } else {
        last_page_out();                // ensure last page output
        ProcFlags.page_ejected = true;  // only first section has nothing to output

        if( page_e == ej_odd ) {
            if( g_page & 1 ) {          // first page will be odd
                if( clear_banners ) {   // emit blank page: no banners
                    t_page.top_banner = NULL;
                    t_page.bottom_banner = NULL;
                }
                do_page_out();          // emit blank page
            }
        } else if( page_e == ej_even ) {
            if( (g_page & 1) == 0 ) {   // first page will be even
                if( clear_banners ) {   // emit blank page: no banners
                    t_page.top_banner = NULL;
                    t_page.bottom_banner = NULL;
                }
                do_page_out();          // emit blank page
            }
        } else if( page_e != ej_yes ) {
            internal_err_exit( __FILE__, __LINE__ );
            /* never return */
        }
        if( ds == DSECT_egdoc ) {
            set_section_banners( ProcFlags.doc_sect );  // retain last section's banners
        } else {
            set_section_banners( ds );
        }
        reset_t_page();

        g_skip = 0;                     // ignore remaining skip value
    }

    ProcFlags.doc_sect = ds;
    ProcFlags.first_hdr = false;
    t_page.panes->col_count = page_c;   // implicitly accomodates only one pane
    set_cols( t_page.panes );           // will need to be updated if multiple panes activated
    if( page_r ) {
        g_page = 0;
    }
    g_text_spacing = page_s;

    /* Reset all heading numbers for ABSTRACT, BODY, APPENDIX and BACKM */

    if( lvl_reset ) {
        for( hlvl = 0; hlvl < HLVL_MAX; hlvl++ ) {
            hd_nums[hlvl].headn = 0;       // reset all levels
            hd_nums[hlvl].hnumstr[0] = '\0';
            if( hd_nums[hlvl].hnumsub != NULL ) {
                *(hd_nums[hlvl].hnumsub->value) = '\0';
            }
        }
    }

    t_page.cur_left = 0;
    if( header ) {
        concat_save = ProcFlags.concat;
        ProcFlags.concat = true;
        justify_save = ProcFlags.justify;
        ProcFlags.justify = JUST_off;
        gen_heading( h_text, NULL, HLVL_h0, hds_lvl );
        g_indentl = 0;                  // reset for section body
        ProcFlags.concat = concat_save;
        ProcFlags.justify = justify_save;
    }
    g_curr_font = layout_work.defaults.font;
    t_page.cur_width = g_indentl;
    ProcFlags.doc_sect = ds;

    if( (block_queue != NULL) && (ds == DSECT_egdoc) ) {
        fb_blocks_out();
    }
}

/***************************************************************************/
/*  routine to init document section change                                */
/***************************************************************************/

static void gml_doc_xxx( doc_section ds )
{

    if( ProcFlags.doc_sect >= ds ) {    // wrong sequence of sections
        g_doc_sect_err_exit( ds );
        /* never return */
    }
    ProcFlags.doc_sect_nxt = ds;        // remember new section
    ProcFlags.start_section = false;    // do real section start later

    g_scandata.s = g_scandata.e;
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

extern void gml_abstract( const gmltag * entry )
{
    (void)entry;

    if( ProcFlags.doc_sect_nxt == DSECT_egdoc ) {
        xx_line_err_exit_c( ERR_EOF_EXPECTED, g_tok_start );
        /* never return */
    }
    if( !ProcFlags.frontm_seen ) {
        xx_line_err_exit_c( ERR_DOC_SEC_EXPECTED_1, g_tok_start );
        /* never return */
    }
    if( g_blank_text_lines > 0 ) {
        set_skip_vars( NULL, NULL, NULL, 1, 0 );    // set g_blank_units_lines
    }
    scr_process_break();
    gml_doc_xxx( DSECT_abstract );
    t_page.cur_left = 0;
    t_page.cur_width = 0;

    if( layout_work.hx.hx_sect[HDS_abstract].header ) {
        start_doc_sect();                           // a header is enough
    }
    g_indentl = 0;
    g_indentr = 0;
    set_h_start();
}

extern void gml_appendix( const gmltag * entry )
{
    (void)entry;

    if( g_blank_text_lines > 0 ) {
        set_skip_vars( NULL, NULL, NULL, 1, 0 );    // set g_blank_units_lines
    }
    scr_process_break();
    gml_doc_xxx( DSECT_appendix );
    ProcFlags.frontm_seen = false;  // no longer in FRONTM section
    if( !ProcFlags.fb_document_done ) { // the very first section/page
        do_layout_end_processing();
    }
    g_indentl = 0;
    g_indentr = 0;
    set_h_start();
}

extern void gml_backm( const gmltag * entry )
{
    (void)entry;

    if( !ProcFlags.fb_document_done ) { // the very first section/page
        do_layout_end_processing();
    }

    if( g_blank_text_lines > 0 ) {
        set_skip_vars( NULL, NULL, NULL, 1, 0 );    // set g_blank_units_lines
    }
    scr_process_break();
    gml_doc_xxx( DSECT_backm );
    ProcFlags.frontm_seen = false;      // no longer in FRONTM section
    t_page.cur_left = 0;
    t_page.cur_width = 0;

    if( layout_work.hx.hx_sect[HDS_backm].header ) {
        start_doc_sect();               // a header is enough
    }
    g_indentl = 0;
    g_indentr = 0;
}

extern void gml_body( const gmltag * entry )
{
    (void)entry;

    if( g_blank_text_lines > 0 ) {
        set_skip_vars( NULL, NULL, NULL, 1, 0 );    // set g_blank_units_lines
    }
    scr_process_break();
    gml_doc_xxx( DSECT_body );

    ProcFlags.just_override = true;     // justify for first line ?? TBD
    t_page.cur_left = 0;
    t_page.cur_width = conv_hor_unit( &layout_work.p.line_indent, g_curr_font );

    ProcFlags.frontm_seen = false;      // no longer in FRONTM section
    if( !ProcFlags.fb_document_done ) { // the very first section/page
        do_layout_end_processing();
    }
    if( layout_work.hx.hx_sect[HDS_body].header ) {
        start_doc_sect();               // a header is enough
    }
    g_indentl = 0;
    g_indentr = 0;
    set_h_start();
}

extern void gml_figlist( const gmltag * entry )
{
    (void)entry;

    scr_process_break();
    figlist_toc |= GENSEC_figlist;
    if( pass > 1 ) {
        gen_figlist();
    }
}

extern void gml_frontm( const gmltag * entry )
{
    (void)entry;

    scr_process_break();
    gml_doc_xxx( DSECT_frontm );
    if( !ProcFlags.fb_document_done ) { // the very first section/page
        do_layout_end_processing();
    }
    ProcFlags.frontm_seen = true;
}

/***************************************************************************/
/*  :INDEX tag is special, not really a section                            */
/***************************************************************************/

extern void gml_index( const gmltag * entry )
{
    (void)entry;

    if( ProcFlags.doc_sect_nxt == DSECT_egdoc ) {
        xx_line_err_exit_c( ERR_EOF_EXPECTED, g_tok_start );
        /* never return */
    }

    if( ProcFlags.doc_sect_nxt == DSECT_index ) {// duplicate :INDEX tag
        g_scandata.s = g_scandata.e;         // ignore this call
        return;                         // wgml4 OS/2 crashes with page fault
    }

    if( !((ProcFlags.doc_sect == DSECT_backm)
      || (ProcFlags.doc_sect_nxt == DSECT_backm)) ) {
        xx_line_err_exit_c( ERR_DOC_SEC_EXPECTED_1, g_tok_start );
        /* never return */
    }
    if( !GlobalFlags.index ) {          // index option not active
        return;
    }

    scr_process_break();
    gml_doc_xxx( DSECT_index );

    /* When gen_index() is finalized, the resets may need to be moved */

    g_indentl = 0;
    g_indentr = 0;
    set_h_start();

    /* Must be called each pass */

    gen_index();                        // output the formatted index
}

extern void gml_preface( const gmltag * entry )
{
    (void)entry;

    if( ProcFlags.doc_sect_nxt == DSECT_egdoc ) {
        xx_line_err_exit_c( ERR_EOF_EXPECTED, g_tok_start );
        /* never return */
    }
    if( !ProcFlags.frontm_seen ) {
        xx_line_err_exit_c( ERR_DOC_SEC_EXPECTED_1, g_tok_start );
        /* never return */
    }
    if( g_blank_text_lines > 0 ) {
        set_skip_vars( NULL, NULL, NULL, 1, 0 );    // set g_blank_units_lines
    }
    scr_process_break();
    gml_doc_xxx( DSECT_preface );
    if( layout_work.hx.hx_sect[HDS_preface].header ) {
        start_doc_sect();                           // a header is enough
    }
    g_indentl = 0;
    g_indentr = 0;
    set_h_start();
}

extern void gml_titlep( const gmltag * entry )
{
    (void)entry;

    if( ProcFlags.doc_sect_nxt == DSECT_egdoc ) {
        xx_line_err_exit_c( ERR_EOF_EXPECTED, g_tok_start );
        /* never return */
    }
    if( !ProcFlags.frontm_seen ) {
        xx_line_err_exit_c( ERR_DOC_SEC_EXPECTED_1, g_tok_start );
        /* never return */
    }
    scr_process_break();
    gml_doc_xxx( DSECT_titlep );

    add_symvar( global_dict, "$stitle", "", 0, SI_no_subscript, SF_none );  // set null string
    add_symvar( global_dict, "$title", "", 0, SI_no_subscript, SF_none );   // set null string

    rs_loc = TLOC_titlep;
    if( input_cbs->fmflags & II_file ) {    // save line number
        titlep_lineno = input_cbs->s.f->lineno;
    } else if( input_cbs->fmflags & II_tag_mac ) {
        titlep_lineno = input_cbs->s.m->lineno;
    } else {
        titlep_lineno = 0;              // not clear what to do here
    }
    save_indentl = g_indentl;
    save_indentr = g_indentr;
    g_indentl = 0;
    g_indentr = 0;
    set_h_start();
}

extern void gml_etitlep( const gmltag * entry )
{
    tag_cb  *   wk;

    (void)entry;

    scr_process_break();
    gml_doc_xxx( DSECT_etitlep );
    rs_loc = 0;
    titlep_lineno = 0;

    if( nest_cb != NULL ) {             // guard against no FRONTM, empty TITLEP section
        g_indentl = save_indentl;
        g_indentr = save_indentr;
        set_h_start();
        wk = nest_cb;
        nest_cb = nest_cb->prev;
        add_tag_cb_to_pool( wk );
    }
}

extern void gml_toc( const gmltag * entry )
{
    (void)entry;

    scr_process_break();
    figlist_toc |= GENSEC_toc;
    if( pass > 1 ) {
        gen_toc();
    }
}

extern void gml_egdoc( const gmltag * entry )
{
    fwd_ref *   curr;

    (void)entry;

    if( g_blank_text_lines > 0 ) {
        set_skip_vars( NULL, NULL, NULL, 1, 0 ); // set g_blank_units_lines
    }
    scr_process_break();                // outputs last element in file

    if( GlobalFlags.lastpass ) {        // output on last pass only
        if( passes == 1 ) {             // first and only pass
            if( figlist_toc & GENSEC_toc ) { // only if TOC was found
                gen_toc();
            }
            if( figlist_toc & GENSEC_figlist ) { // only if FIGLIST was found
                gen_figlist();
            }
            // output figure forward/undefined references
            for( curr = fig_fwd_refs; curr != NULL; curr = curr->next ) {
                if( find_refid( fig_ref_dict, curr->refid ) != NULL ) {
                    xx_simple_warn_info_cc( WNG_ID_XXX, curr->refid, INF_ID_FORWARD, "figure" );
                } else {
                    xx_simple_warn_info_cc( WNG_ID_XXX, curr->refid, INF_ID_UNKNOWN, "Figure" );
                }
            }
            // output header forward/undefined references
            for( curr = hd_fwd_refs; curr != NULL; curr = curr->next ) {
                if( find_refid( hd_ref_dict, curr->refid ) != NULL ) {
                    xx_simple_warn_info_cc( WNG_ID_XXX, curr->refid, INF_ID_FORWARD, "heading" );
                } else {
                    xx_simple_warn_info_cc( WNG_ID_XXX, curr->refid, INF_ID_UNKNOWN, "Heading" );
                }
            }
            // output footnote forward/undefined references
            for( curr = fn_fwd_refs; curr != NULL; curr = curr->next ) {
                if( find_refid( fn_ref_dict, curr->refid ) != NULL ) {
                    xx_simple_warn_info_cc( WNG_ID_XXX, curr->refid, INF_ID_FORWARD, "footnote" );
                } else {
                    xx_simple_warn_info_cc( WNG_ID_XXX, curr->refid, INF_ID_UNKNOWN, "Footnote" );
                }
            }
            if( figlist_toc != GENSEC_none ) {
                xx_simple_warn( WNG_PASS_1 );       // more than one pass needed
            }
        } else {                                    // last pass of at least 2
            // output figure undefined/page change references
            for( curr = fig_fwd_refs; curr != NULL; curr = curr->next ) {
                xx_simple_warn_info_cc( WNG_ID_XXX, curr->refid, INF_ID_FORWARD, "figure" );
            }
            // output header undefined/page change references
            for( curr = hd_fwd_refs; curr != NULL; curr = curr->next ) {
                xx_simple_warn_info_cc( WNG_ID_XXX, curr->refid, INF_ID_FORWARD, "heading" );
            }
            if( ProcFlags.new_pagenr ) {
                xx_simple_warn( WNG_PASS_MANY );    // at least one more pass needed
            }
        }
        if( !GlobalFlags.index && ProcFlags.index_tag_cw_seen ) {   // index option needed
            xx_simple_warn( WNG_INDEX_OPT );                        // give hint to activate index
        }
    }

    gml_doc_xxx( DSECT_egdoc );
    if( block_queue != NULL ) {         // avoids blank last page if nothing will follow
        start_doc_sect();
    }
}

/***************************************************************************/
/*  :gdoc sec='TOP secret, destroy before reading'                         */
/***************************************************************************/

extern void gml_gdoc( const gmltag *entry )
{
    char            *p;
    char            *pa;
    att_name_type   attr_name;
    att_val_type    attr_val;
    struct {
        unsigned    sec :1;
    } AttrFlags;

    (void)entry;

    g_scan_err = false;
    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags
    p = g_scandata.s;
    if( *p == '.' ) {
        add_symvar( global_dict, "$sec", "", 0, SI_no_subscript, SF_none ); // set null string
    } else {
        for( ;; ) {
            p = get_tag_att_name( p, &pa, &attr_name );
            if( ProcFlags.reprocess_line )
                break;
            if( ProcFlags.tag_end_found )
                break;
            if( strcmp( "sec", attr_name.attname.t ) == 0 ) {
                p = get_att_value( p, &attr_val );
                if( AttrFlags.sec ) {
                    xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                        attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                    /* never return */
                }
                AttrFlags.sec = true;
                if( attr_val.tok.s == NULL ) {
                    add_symvar( global_dict, "$sec", "", 0, SI_no_subscript, SF_none ); // set null string
                    break;
                } else {
                    add_symvar( global_dict, "$sec", p, strlen( p ), SI_no_subscript, SF_none );
                }
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else {
                xx_line_err_exit_c( ERR_TAG_NOT_TEXT, p );
                /* never return */
            }
        }
    }

    SkipDot( p );
    if( (*p != '\0') ) {
        xx_line_err_exit_c( ERR_TAG_NOT_TEXT, p );
        /* never return */
    }

    gml_doc_xxx( DSECT_gdoc );
    if( !ProcFlags.fb_document_done ) { // the very first section/page
        do_layout_end_processing();
    }
}

