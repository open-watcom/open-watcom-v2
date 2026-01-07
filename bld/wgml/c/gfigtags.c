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
* Description:  tags FIG, eFIG, FIGCAP, FIGDESC, and FIGREF
*
* Note: FIGLIST is defined in gdocsect.c
*       FIGREF is defined in gxxref.c
*
****************************************************************************/

#include "wgml.h"

#include "clibext.h"


static  ban_place       place;                  // FIG attribute used by eFIG
static  bool            concat_save;            // for ProcFlags.concat
static  bool            figcap_done;            // FIGCAP done for current FIG
static  bool            page_width;             // FIG flag used by eFIG
static  bool            splitting;              // FIG is being split
static  bool            t_page_width    = false;// FIG will actually go into page_width section
static  char            figrefid[REFID_LEN + 1];// FIG attribute used by eFIG
static  def_frame       frame;                  // FIG attribute used by eFIG
static  group_type      sav_group_type;         // save prior group type
static  ju_enum         justify_save;           // for ProcFlags.justify
static  text_space      spacing_save;           // for spacing
static  unsigned        depth           = 0;    // FIG attribute used by eFIG
static  unsigned        left_inset;             // offset from frame to contents
static  unsigned        right_inset;            // offset from frame to contents
static  unsigned        width           = 0;    // FIG attribute used by eFIG

/******************************************************************************/
/* Add text_char instances containing one bin_device->box.chars.vertical_line */
/* character to the left and right of the parameter                           */
/* This is only used with character devices                                   */
/******************************************************************************/

static void add_risers( text_line * in_line )
{
    text_chars  *   riser;

    riser = alloc_text_chars( &bin_device->box.chars.vertical_line, 1, bin_device->box.font );
    riser->width = 1;
    riser->x_address = t_page.cur_left;
    if( in_line->first != NULL ) {
        in_line->first->prev = riser;
        riser->next = in_line->first;
    }
    in_line->first = riser;

    riser = alloc_text_chars( &bin_device->box.chars.vertical_line, 1, bin_device->box.font );
    riser->x_address = t_page.max_width - 1;
    riser->width = 1;
    if( in_line->last == NULL ) {
        riser->prev = in_line->first;
        in_line->first->next = riser;
    } else {
        riser->prev = in_line->last;
        in_line->last->next = riser;
    }
    in_line->last = riser;

    return;
}

/***************************************************************************/
/* Create a doc_element containting a box line                             */
/* This is only used when drawing the box with characters, not with PS     */
/* The box line text is in line_buff.text                                  */
/***************************************************************************/

static doc_element * get_box_line_el( void )
{
    doc_element *   cur_doc_el;

    cur_doc_el = alloc_doc_el( ELT_text );
    cur_doc_el->element.text.first = alloc_text_line();
    cur_doc_el->element.text.first->first =
            alloc_text_chars( line_buff.text, line_buff.current, bin_device->box.font );

    /* This may need additional work */

    cur_doc_el->element.text.first->first->width = line_buff.current;
    cur_doc_el->element.text.first->first->x_address = t_page.cur_left;
    cur_doc_el->element.text.first->line_height = wgml_fonts[bin_device->box.font].line_height;
    cur_doc_el->depth = wgml_fonts[bin_device->box.font].line_height;

    return( cur_doc_el );
}

/***************************************************************************/
/* Draw a box around the current box or box segment                        */
/* This function will use DBOX if that block is defined, and the BOX       */
/* characters if it is not                                                 */
/* NOTE: wgml 4.0 does something a bit different if the DBOX block is not  */
/*       defined but the HLINE block is; should such a device be added,    */
/*       this function will need to be adjusted to accomodate it           */
/* NOTE: use of FONT0 does not, in fact, match wgml 4.0 -- but since all   */
/*       wgml 4.0 does is the first LINEPASS for these items and since the */
/*       first LINEPASS invariably simply outputs the text, the effect is  */
/*       the same for character devices                                    */
/***************************************************************************/

static void draw_box( doc_el_group * in_group )
{
    doc_element *   cur_doc_el;
    doc_element *   sav_doc_el;
    unsigned        k;
    text_line   *   cur_line;
    text_line   *   sav_line;

    if( bin_driver->dbox.text == NULL ) {           // DBOX not available
        resize_record_buffer( &line_buff, width );
        memset( line_buff.text, bin_device->box.chars.horizontal_line, line_buff.current );
        line_buff.text[line_buff.current] = '\0';

        /* Finalize and insert the top box line */

        sav_doc_el = in_group->first;           // save original first element

        line_buff.text[0] = bin_device->box.chars.top_left;
        line_buff.text[line_buff.current - 1] = bin_device->box.chars.top_right;
        cur_doc_el = get_box_line_el();
        cur_doc_el->subs_skip = in_group->first->subs_skip;
        in_group->first->subs_skip = 0;
        cur_doc_el->top_skip = in_group->first->top_skip;
        in_group->first->top_skip = 0;

        cur_doc_el->next = in_group->first;
        in_group->first = cur_doc_el;
        in_group->depth += cur_doc_el->depth;

        /* Process lines inside figure */

        for( cur_doc_el = sav_doc_el; cur_doc_el != NULL; cur_doc_el = cur_doc_el->next ) {
            switch( cur_doc_el->type ) {
            // add code for other element types as appropriate
            case ELT_binc:
            case ELT_dbox:
            case ELT_graph:
            case ELT_hline:
            case ELT_vline:
                break;
            case ELT_text:

                /********************************************************/
                /* In reality, the box will have all text elements      */
                /* If the box has a non-zero depth, the first text      */
                /* element will have the depth in blank_lines and a     */
                /* blank text_line. Following elements will have        */
                /* text_lines which actually contain text               */
                /* If the box has zero depth, all text elements will    */
                /* have text_lines which actually contain text          */
                /********************************************************/

                if( cur_doc_el->blank_lines > 0 ) {
                    cur_line = cur_doc_el->element.text.first;
                    cur_line->line_height = wgml_fonts[bin_device->box.font].line_height;
                    add_risers( cur_line );
                    for( k = 1; k < cur_doc_el->blank_lines; k++ ) {
                        cur_line->next = alloc_text_line();
                        cur_line->next->line_height = wgml_fonts[bin_device->box.font].line_height;
                        add_risers( cur_line->next );
                        cur_line = cur_line->next;
                    }
                    cur_doc_el->blank_lines = 0;
                } else {
                    if( cur_doc_el->subs_skip > 0 ) {
                        sav_line = cur_doc_el->element.text.first;
                        for( k = 0; k < cur_doc_el->subs_skip; k++ ) {
                            cur_line = alloc_text_line();
                            cur_line->line_height = wgml_fonts[bin_device->box.font].line_height;
                            add_risers( cur_line );
                            cur_line->next = cur_doc_el->element.text.first;
                            cur_doc_el->element.text.first = cur_line;
                        }
                        cur_doc_el->subs_skip = 0;
                        cur_line = sav_line;
                    } else {
                        cur_line = cur_doc_el->element.text.first;
                    }
                    while( cur_line != NULL ) {
                        add_risers( cur_line);
                        cur_line = cur_line->next;
                    }
                }
                break;
            default :
                internal_err_exit( __FILE__, __LINE__ );
                /* never return */
            }
        }

        /* Finalize and insert the bottom box line */

        line_buff.text[0] = bin_device->box.chars.bottom_left;
        line_buff.text[line_buff.current - 1] = bin_device->box.chars.bottom_right;
        cur_doc_el = get_box_line_el();
        in_group->last->next = cur_doc_el;
        in_group->last = in_group->last->next;
        in_group->depth += cur_doc_el->depth;
    } else {                                        // use DBOX
        cur_doc_el = alloc_doc_el( ELT_dbox );
        if( place == inline_place ) {
            if( !splitting ) {
                cur_doc_el->subs_skip += wgml_fonts[layout_work.fig.font].line_height;
            } else {
                cur_doc_el->subs_skip += wgml_fonts[FONT0].line_height;
            }
        } else {
            cur_doc_el->subs_skip = wgml_fonts[layout_work.fig.font].line_height;
        }
        cur_doc_el->element.dbox.h_start = t_page.cur_left;
        cur_doc_el->element.dbox.h_len = width;
        cur_doc_el->element.dbox.v_len = in_group->depth;
        if( (place == inline_place) ) {

            /****************************************************************/
            /* It appears that the initial blank line must be FONT0 for the */
            /* initial text line but must be the FIG's font for v_len       */
            /* But only if the figure is not being split                    */
            /* This, of course, makes no sense, but does match wgml 4.0     */
            /****************************************************************/

            if( !splitting ) {
                cur_doc_el->element.dbox.v_len -= wgml_fonts[FONT0].line_height;
                cur_doc_el->element.dbox.v_len += wgml_fonts[layout_work.fig.font].line_height;
            }
        } else {
            cur_doc_el->element.dbox.v_len += wgml_fonts[layout_work.fig.font].line_height;
        }
        if( place != inline_place ) {
            in_group->depth += (wgml_fonts[layout_work.fig.font].line_height +
                                wgml_fonts[FONT0].line_height);
        } else {
            in_group->depth += wgml_fonts[layout_work.fig.font].line_height;
        }
        in_group->last->next = cur_doc_el;
        in_group->last = in_group->last->next;
    }
    return;
}
/***************************************************************************/
/* Insert a rule or char line into the current doc_el_group                */
/* This function must check whether a rule or char line is needed and what */
/* its length should be                                                    */
/* It is only called when appropriate: so position need not be checked,    */
/* nor need frame be checked to avoid "none" or "box"                      */
/***************************************************************************/

static void insert_frame_line( void )
{
    doc_element     *h_line_el;
    unsigned        i;
    unsigned        cur_count;
    unsigned        str_count;
    unsigned        cur_limit;  // number of whole copies of frame.string that will fit into line.buff
    unsigned        cur_width;
    unsigned        str_width;
    unsigned        limit;

    if( bin_driver->hline.text == NULL ) {              // character device
        resize_record_buffer( &line_buff, width );
        if( frame.type == FRAME_rule ) {
            memset( line_buff.text, bin_device->box.chars.horizontal_line, line_buff.current );
            line_buff.text[line_buff.current] = '\0';
        } else {                    // FRAME_char Note: wgml 4.0 uses font 0 regardless of the default font for the section
            line_buff.text[0] = '\0';
            str_count = strlen( frame.string );
            cur_limit = line_buff.current / str_count;  // number of complete strings that will fit
            cur_width = 0;
            for( i = 0; i < cur_limit; i++  ) {         // fill text with full string
                strcat( line_buff.text, frame.string );
                cur_width += strlen( frame.string );
            }
            if( cur_width < line_buff.current ) {       // text not full yet
                strncpy( &line_buff.text[cur_width], frame.string, line_buff.current - cur_width );
            }
            line_buff.text[line_buff.current] = '\0';
        }
        process_text( line_buff.text, FONT0 );  // matches wgml 4.0
        scr_process_break();        // commit line
    } else {                                        // page-oriented device
        if( frame.type == FRAME_rule ) {

        /*******************************************************************/
        /* This uses code written originally for use with control word BX  */
        /* That control word uses depth to indicate the amount by which at */
        /* the vertical position is to be adjusted after the hline is      */
        /* emitted, as it appears in the middle of the normal line depth   */
        /* Here, the line appears at the bottom of the line depth, but the */
        /* depth used must be 0 to prevent the next element from being     */
        /* placed one line too far down on the page                        */
        /*******************************************************************/

            h_line_el = init_doc_el( ELT_hline, 0 );
            h_line_el->element.hline.ban_adjust = false;   // TBD, may not apply to FIG
            h_line_el->element.hline.h_start = nest_cb->left_indent;
            h_line_el->element.hline.h_len = width;
            insert_col_main( h_line_el );
        } else {                    // FRAME_char Note: wgml 4.0 uses font 0 regardless of the default font for the section
            str_count = strlen( frame.string );
            str_width = 0;
            for( i = 0; i < strlen( frame.string ); i++ ) {
                str_width += wgml_fonts[FONT0].width.table[(unsigned char)frame.string[i]];
            }
            cur_limit = width / str_width;
            cur_count = 0;
            cur_width = 0;
            limit = cur_limit;
            if( (width % str_width) > 0 ) {                   // partial copy will be needed
                limit++;
            }
            resize_record_buffer( &line_buff, limit * str_count );  // length in characters
            line_buff.text[0] = '\0';
            for( i = 0; i < cur_limit; i++ ) {              // fill text with copies of full string
                strcat( line_buff.text, frame.string );
                cur_width += str_width;
                cur_count += str_count;
            }
            if( cur_width < width ) {       // text not full yet
                for( i = 0; i < strlen( frame.string ); i++ ) {
                    cur_width += wgml_fonts[FONT0].width.table[(unsigned char)frame.string[i]];
                    if( cur_width >= width ) {  // check what width would be if character were copied
                        break;
                    }
                    line_buff.text[cur_count] = frame.string[i];
                    cur_count++;
                }
            }
            line_buff.current = cur_count;
            line_buff.text[line_buff.current] = '\0';
            process_text( line_buff.text, FONT0 );  // matches wgml 4.0
            scr_process_break();        // commit line
        }
    }
    return;
}


/***************************************************************************/
/*      :FIG [depth='vert-space-unit']                                     */
/*           [frame=box                                                    */
/*                  rule                                                   */
/*                  none                                                   */
/*                  'character string']                                    */
/*           [id='id-name']                                                */
/*           [place=top                                                    */
/*                  bottom                                                 */
/*                  inline]                                                */
/*           [width=page                                                   */
/*                  column                                                 */
/*                  'hor-space-unit'].                                     */
/*           <paragraph elements>                                          */
/*           <basic document elements>                                     */
/* This tag signals the start of a figure. Each line of source text        */
/* following the figure tag is placed in the output document without       */
/* normal text processing. Spacing between words is preserved, and the     */
/* input text is not right justified. Input source lines which do not fit  */
/* on a line in the output document are split into two lines on a          */
/* character, rather than a word basis. A figure may be used where a basic */
/* document element is permitted, except within a figure, footnote, or     */
/* example. If the figure does not fit on the current page or column, it   */
/* is forced to the next one. If the current column is empty, the figure   */
/* will be split into two parts.                                           */
/***************************************************************************/

void gml_fig( const gmltag * entry )
{
    bool            id_seen     = false;
    bool            width_seen  = false;
    char            *p;
    char            *pa;
    ref_entry       *cur_ref     = NULL;
    su              cur_su;
    unsigned        max_width;
    att_name_type   attr_name;
    att_val_type    attr_val;

    (void)entry;

    start_doc_sect();
    scr_process_break();
    g_scan_err = false;

    if( is_ip_tag( nest_cb->gtag ) ) {                 // inline phrase not closed
        g_tag_nest_err_exit( nest_cb->gtag ); // end tag expected
        /* never return */
    }
    g_keep_nest( "Figure" );            // catch nesting errors

    ProcFlags.block_starting = true;    // to catch empty blocks

    figcap_done = false;                // reset for this FIG
    *figrefid = '\0';
    page_width = false;
    depth = 0;                          // default value; depth is space reserved for some other item
    frame.type = layout_work.fig.default_frame.type;
    if( frame.type == FRAME_char ) {
        strcpy( frame.string, layout_work.fig.default_frame.string );
    }
    place = layout_work.fig.default_place;
    max_width = t_page.last_pane->col_width;// default value regardless of number of columns
    g_curr_font = layout_work.fig.font;
    spacing_save = g_text_spacing;
    g_text_spacing = layout_work.fig.spacing;

    p = g_scandata.s;
    if( *p == '.' ) {
        /* already at tag end */
    } else {
        for( ;; ) {
            p = get_tag_att_name( p, &pa, &attr_name );
            if( ProcFlags.reprocess_line )
                break;
            if( ProcFlags.tag_end_found )
                break;
            if( strcmp( "depth", attr_name.attname.t ) == 0 ) {
                p = get_att_value( p, &attr_val );
                if( attr_val.tok.s == NULL ) {
                    break;
                }
                if( att_val_to_su( &cur_su, true, &attr_val, false ) ) {
                    break;
                }
                depth = conv_vert_unit( &cur_su, g_text_spacing, g_curr_font );
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else if( strcmp( "frame", attr_name.attname.t ) == 0 ) {
                p = get_att_value( p, &attr_val );
                if( attr_val.tok.s == NULL ) {
                    break;
                }
                if( strcmp( "none", attr_val.specval ) == 0 ) {
                    frame.type = FRAME_none;
                } else if( strcmp( "box", attr_val.specval ) == 0 ) {
                    frame.type = FRAME_box;
                } else if( strcmp( "rule", attr_val.specval ) == 0 ) {
                    frame.type = FRAME_rule;
                } else {
                    frame.type = FRAME_char;
                }
                if( frame.type == FRAME_char ) {
                    if( attr_val.tok.l > STRBLK_SIZE )
                        attr_val.tok.l = STRBLK_SIZE;
                    strncpy( frame.string, attr_val.tok.s, attr_val.tok.l );
                    frame.string[attr_val.tok.l] = '\0';
                    if( frame.string[0] == '\0' ) {
                        frame.type = FRAME_none;    // treat null string as "none"
                    }
                } else {                            // blank any existing frame.string value
                    frame.string[0] = '\0';
                }
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else if( strcmp( "id", attr_name.attname.t ) == 0 ) {
                p = get_refid_value( p, &attr_val, figrefid );
                if( attr_val.tok.s == NULL ) {
                    break;
                }
                id_seen = true;             // valid id attribute found
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else if( strcmp( "place", attr_name.attname.t ) == 0 ) {
                p = get_att_value( p, &attr_val );
                if( attr_val.tok.s == NULL ) {
                    break;
                }
                if( strcmp( "bottom", attr_val.specval ) == 0 ) {
                    place = bottom_place;
                } else if( strcmp( "inline", attr_val.specval ) == 0 ) {
                    place = inline_place;
                } else if( strcmp( "top", attr_val.specval ) == 0 ) {
                    place = top_place;
                } else {
                    xx_line_err_exit_c( ERR_INV_ATT_VAL, attr_val.tok.s );
                    /* never return */
                }
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else if( strcmp( "width", attr_name.attname.t ) == 0 ) {
                p = get_att_value( p, &attr_val );
                if( attr_val.tok.s == NULL ) {
                    break;
                }
                if( strcmp( "page", attr_val.specval ) == 0 ) {
                    // this will be used to set t_page_width and width below
                    page_width = true;
                } else if( strcmp( "column", attr_val.specval ) == 0 ) {
                    // default value is the correct value to use
                } else {    // value actually specifies the width
                    pa = attr_val.tok.s;
                    if( att_val_to_su( &cur_su, true, &attr_val, false ) ) {
                        break;
                    }
                    width = conv_hor_unit( &cur_su, g_curr_font );
                    if( width == 0 ) {
                        xx_line_err_exit_c( ERR_INV_WIDTH_FIG_1, attr_val.tok.s );
                        /* never return */
                    }
                    width_seen = true;
                }
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else {    // no match = end-of-tag in wgml 4.0
                p = pa; // restore spaces before text
                break;
            }
        }
    }

    set_skip_vars( &layout_work.fig.pre_skip, NULL, NULL, g_text_spacing, g_curr_font );

    /* Only page-width top figs on multi-column pages actually go into the page_width section */

    t_page_width = (place == top_place) && (page_width) && (t_page.last_pane->col_count > 1);

    /* For an inline dbox, the actual skip must be done before the box itself */

    if( place == inline_place ) {
        if( (frame.type == FRAME_box) && (bin_driver->dbox.text != NULL) ) {
            g_blank_units_lines += g_subs_skip;
            g_subs_skip = 0;
            scr_process_break();
        }
    }

    init_nest_cb();
    nest_cb->p_stack = copy_to_nest_stack();
    nest_cb->left_indent = conv_hor_unit( &layout_work.fig.left_adjust, g_curr_font );
    nest_cb->right_indent = conv_hor_unit( &layout_work.fig.right_adjust, g_curr_font );
    nest_cb->font = g_curr_font;
    nest_cb->gtag = entry->u.tagid;

    sav_group_type = cur_group_type;
    cur_group_type = GRT_fig;
    cur_doc_el_group = alloc_doc_el_group( GRT_fig );
    cur_doc_el_group->next = t_doc_el_group;
    t_doc_el_group = cur_doc_el_group;
    cur_doc_el_group = NULL;

    concat_save = ProcFlags.concat;
    ProcFlags.concat = false;
    justify_save = ProcFlags.justify;
    ProcFlags.justify = JUST_off;

    /* Only create the entry on the first pass */

    if( pass == 1 ) {                   // add this FIG to fig_list
        fig_entry = init_ffh_entry( fig_list, FFH_fig );    // mark as FIG
        if( fig_list == NULL ) {        // first entry
            fig_list = fig_entry;
        }
        if( id_seen ) {                 // add this entry to fig_ref_dict
            cur_ref = find_refid( fig_ref_dict, figrefid );
            if( cur_ref == NULL ) {             // new entry
                cur_ref = add_new_refid( &fig_ref_dict, figrefid, fig_entry );
            } else {                // duplicate id
                dup_refid_err_exit( cur_ref->refid, "figure" );
                /* never return */
            }
        }
    }

    /* insert_frame_line() uses width and nest_cb->left_indent */

    if( (place != top_place)
      && ((frame.type == FRAME_rule)
      || (frame.type == FRAME_char)) ) {
        if( (frame.type == FRAME_rule)
          && (bin_driver->hline.text != NULL)
          && (place == inline_place) ) {
            g_subs_skip += wgml_fonts[FONT0].line_height;   // this is actually the depth used by the HLINE
            g_top_skip += wgml_fonts[FONT0].line_height;    // for use if fig moved to top of next column
        }
        insert_frame_line();
    }

    if( (frame.type == FRAME_none)
      && (place != bottom_place) ) {
        if( depth > g_subs_skip ) {
            g_blank_units_lines = depth;
            g_subs_skip = 0;
            scr_process_break();
        }
    } else {
        g_blank_units_lines = depth;
        scr_process_break();
    }

    /* Select the width to use */

    if( t_page_width ) {                // t_page.page_width will be used
        max_width = t_page.page_width;
    }

    if( width_seen ) {                  // width entered will be used
        if( width > max_width ) {
            xx_line_err_exit_c( ERR_INV_WIDTH_FIG_3, attr_val.tok.s );
            /* never return */
        }
    } else {
        width = max_width;              // t_page.last_pane->col_width will be used
    }

    if( width_seen ) {                  // wgml 4.0 makes this distinction
        width -= nest_cb->right_indent;
    } else {
        width -= (nest_cb->left_indent + nest_cb->right_indent);
    }

    /* Initialize the insets */

    if( frame.type == FRAME_none ) {
        left_inset = 0;
        right_inset = 0;
    } else {
        if( ProcFlags.has_aa_block ) {      // matches wgml 4.0
            left_inset = wgml_fonts[FONT0].line_height + 1;
            right_inset = wgml_fonts[FONT0].line_height + 1;
        } else {
            left_inset = 2 * tab_col;
            right_inset = 2 * tab_col;
        }
    }

    /* This is for the overall figure, including any frame */

    t_page.cur_left = nest_cb->left_indent;
    t_page.max_width = width + nest_cb->left_indent;    // page/col width is not fig width

    if( width > t_page.last_pane->col_width ) {
        if( (t_page.last_pane->col_count > 1)
          && (place != top_place) ) {
            xx_line_err_exit_c( ERR_INV_WIDTH_FIG_2, attr_val.tok.s );
            /* never return */
        } else if( t_page.last_pane->col_count == 1 ) {
            xx_line_err_exit_c( ERR_INV_WIDTH_FIG_3, attr_val.tok.s );
            /* never return */
        }
    }

    if( (t_page.cur_left >= t_page.max_width)
      || (t_page.cur_left >= g_page_right_org) ) {
        if( frame.type == FRAME_none ) {
            xx_line_err_exit_c( ERR_INV_MARGINS_1, attr_val.tok.s );
        } else {
            xx_line_err_exit_c( ERR_INV_MARGINS_2, attr_val.tok.s );
        }
        /* never return */
    }

    /* Now set up margins for any text inside the figure */

    t_page.cur_left += left_inset;
    t_page.cur_width = t_page.cur_left;
    ProcFlags.keep_left_margin = true;  // keep special indent


    if( t_page.max_width < right_inset ) {
        if( frame.type == FRAME_none ) {
            xx_line_err_exit_c( ERR_INV_MARGINS_1, attr_val.tok.s );
        } else {
            xx_line_err_exit_c( ERR_INV_MARGINS_2, attr_val.tok.s );
        }
        /* never return */
    }
    t_page.max_width -= right_inset;
    t_page.cur_width = t_page.cur_left;
    ProcFlags.keep_left_margin = true;      // keep special indent

    if( !ProcFlags.reprocess_line
      && *p != '\0' ) {
        SkipDot( p );                       // possible tag end
        if( *p != '\0' ) {
            process_text( p, g_curr_font);  // if text follows
        }
    }
    g_scandata.s = g_scandata.e;
    return;
}


/***************************************************************************/
/*      :eFIG.                                                             */
/* This tag signals the end of a figure. A corresponding :fig tag must be  */
/* previously specified for each :efig tag.                                */
/***************************************************************************/

void gml_efig( const gmltag * entry )
{
    bool                splittable;
    bool                split_done;
    char            *   p;
    doc_el_group    *   cur_group;              // current group from n_page, not cur_doc_el_group
    doc_el_group    *   new_group;              // new group for use in splitting cur_doc_el_group
    doc_element     *   cur_el;
    doc_element     *   next_el;
    tag_cb          *   wk;
    unsigned            bias;
    unsigned            cur_depth;
    unsigned            page_pred;
    unsigned            raw_p_skip;

    (void)entry;

    scr_process_break();
    rs_loc = 0;

    /* Done here because needed for the minimum post_skip */

    p = g_scandata.s;
    SkipDot( p );                       // possible tag end

    if( cur_group_type != GRT_fig ) {    // no preceding :FIG tag
        g_tag_prec_err_exit( T_FIG );
        /* never return */
    }

    t_page.cur_left = nest_cb->left_indent; // reset various values in case needed for frame
    t_page.max_width += right_inset;
    ProcFlags.concat = false;
    set_skip_vars( NULL, NULL, &layout_work.fig.post_skip, g_text_spacing, layout_work.fig.font );

    if( ProcFlags.block_starting ) {    // block is empty
        g_subs_skip += g_post_skip;
        g_post_skip = 0;
        ProcFlags.block_starting = false;
    }

    raw_p_skip = g_post_skip;           // save for future use

    /*******************************************************************/
    /* t_doc_el_group->post_skip set here will only be used on the     */
    /* current page because, for other top figures, it is reset below  */
    /* Note: a post_skip of 1 is treated the same as a post_skip of 0  */
    /*       but the test for "1" must be the font line height         */
    /*******************************************************************/

    if( (g_post_skip == 0)
      || (g_post_skip == wgml_fonts[layout_work.fig.font].line_height) ) {
        if( *p != '\0' ) {
            g_post_skip = wgml_fonts[FONT0].line_height;
        }
        if( place == top_place ) {                      // top fig only
            t_doc_el_group->post_skip = wgml_fonts[FONT0].line_height;
        }
    } else {
        t_doc_el_group->post_skip = g_post_skip;
    }

    if( (place != bottom_place)
      && ((frame.type == FRAME_rule)
      || (frame.type == FRAME_char)) ) {
        if( (frame.type == FRAME_rule)
          && (bin_driver->hline.text != NULL) ) {
            g_subs_skip += wgml_fonts[layout_work.fig.font].line_height; // this is actually the depth used by the HLINE
        }
        insert_frame_line();
    }

    if( (place == inline_place)
      && (frame.type == FRAME_box)
      && (bin_driver->dbox.text != NULL) ) {
        t_doc_el_group->first->subs_skip += wgml_fonts[FONT0].line_height;
        t_doc_el_group->depth += wgml_fonts[FONT0].line_height;
    }

    ProcFlags.skips_valid = false;      // activate post_skip for next element

    if( (*figrefid != '\0')             // FIG id requires FIGCAP
      && !figcap_done ) {
        xx_err_exit( ERR_FIG_ID_CAP );
        /* never return */
    }

    /* Place the accumulated lines on the proper page */

    cur_group_type = sav_group_type;
    if( t_doc_el_group != NULL) {
        cur_doc_el_group = t_doc_el_group;      // detach current element group
        t_doc_el_group = t_doc_el_group->next;  // processed doc_elements go to next group, if any
        cur_doc_el_group->next = NULL;

        if( place == inline_place ) {       // inline
            split_done = false;
            while( (cur_doc_el_group != NULL) && (cur_doc_el_group->first != NULL) ) {
                splitting = false;
                if( frame.type == FRAME_box ) {
                    if( bin_driver->dbox.text != NULL ) {   // DBOX available
                        bias = 2 * wgml_fonts[FONT0].line_height;
                        cur_doc_el_group->first->top_skip = bias; // this is actually the depth used by the HLINE
                    } else {
                        bias = wgml_fonts[layout_work.fig.font].line_height +
                                                        wgml_fonts[FONT0].line_height;
                    }
                } else {
                    bias = 0;
                }
                if( (cur_doc_el_group->depth + t_page.cur_depth + bias) > t_page.max_depth ) {

                    /* the block won't fit in this column */

                    full_col_out();

                    if( (cur_doc_el_group->depth + bias) <= t_page.max_depth ) {

                        /* the block will be in the next column */

                        next_column();
                        if( frame.type == FRAME_box ) {

                            /* Last part of split box */

                            if( split_done ) {
                                cur_doc_el_group->depth += wgml_fonts[FONT0].line_height;
                                split_done = false;
                            }
                            draw_box( cur_doc_el_group );
                        }
                        while( cur_doc_el_group->first != NULL ) {
                            cur_el = cur_doc_el_group->first;
                            cur_doc_el_group->first = cur_doc_el_group->first->next;
                            cur_el->next = NULL;
                            insert_col_main( cur_el );
                        }
                    } else {

                        /* the block won't fit in any column */

                        /* first split the block */

                        splitting = true;
                        split_done = true;
                        cur_depth = 0;
                        new_group = alloc_doc_el_group( GRT_fig );
                        while( cur_doc_el_group->first != NULL ) {
                            cur_el = cur_doc_el_group->first;
                            cur_doc_el_group->first = cur_doc_el_group->first->next;
                            cur_el->next = NULL;
                            cur_depth = cur_el->blank_lines + cur_el->subs_skip + cur_el->depth;
                            if( cur_depth <= ((t_page.max_depth - t_page.cur_depth) -
                                (new_group->depth + bias)) ) {

                                /* the element will fit in this column */

                                if( new_group->first == NULL ) {
                                    new_group->first = cur_el;
                                } else {
                                    new_group->last->next = cur_el;
                                }
                                new_group->last = cur_el;
                                new_group->depth += cur_depth;
                                cur_doc_el_group->depth -= cur_depth;
                            } else {

                                /* the element must be split */

                                splittable = split_element( cur_el, (t_page.max_depth  - t_page.cur_depth)
                                                            - (new_group->depth + bias) );
                                next_el = cur_el->next;
                                cur_el->next = NULL;
                                if( splittable
                                  && (next_el != NULL) ) {     // cur_el was split
                                    if( new_group->first == NULL ) {
                                        new_group->first = cur_el;
                                    } else {
                                        new_group->last->next = cur_el;
                                    }
                                    new_group->last = cur_el;
                                    next_el->next = cur_doc_el_group->first;
                                    cur_doc_el_group->first = next_el;
                                    cur_doc_el_group->last = next_el;
                                    while( cur_doc_el_group->last->next != NULL ) {
                                        cur_doc_el_group->last = cur_doc_el_group->last->next;
                                    }
                                    new_group->depth += cur_el->depth;
                                    cur_doc_el_group->depth -= cur_el->depth;
                                } else {    // cur_el must go in next column
                                    cur_el->next = cur_doc_el_group->first;
                                    cur_doc_el_group->first = cur_el;
                                }
                                break;
                            }
                        }
                        if( new_group != NULL ) {
                            new_group->next = cur_doc_el_group;
                            cur_doc_el_group = new_group;
                            new_group = NULL;
                        }

                        /* then output the part that fits */

                        if( frame.type == FRAME_box ) {
                            draw_box( cur_doc_el_group );
                        }
                        while( cur_doc_el_group->first != NULL ) {
                            cur_el = cur_doc_el_group->first;
                            cur_doc_el_group->first = cur_doc_el_group->first->next;
                            cur_el->next = NULL;
                            insert_col_main( cur_el );
                        }
                        cur_group = cur_doc_el_group;
                        cur_doc_el_group = cur_doc_el_group->next;
                        cur_group->next = NULL;
                        add_doc_el_group_to_pool( cur_group );
                        cur_group = NULL;
                    }
                } else {
                    if( frame.type == FRAME_box ) {
                        draw_box( cur_doc_el_group );
                    }
                    while( cur_doc_el_group->first != NULL ) {
                        cur_el = cur_doc_el_group->first;
                        cur_doc_el_group->first = cur_doc_el_group->first->next;
                        cur_el->next = NULL;
                        insert_col_main( cur_el );
                    }
                }
            }
            page_pred = g_page + 1;         // set to page of bottom of FIG
            add_doc_el_group_to_pool( cur_doc_el_group );
            cur_doc_el_group = NULL;
        } else {

            /*************************************************************/
            /* blank_lines and the skips of the first doc_element are    */
            /*   used with the next doc_element (if frame is not "none") */
            /* the skips of the first doc_element are used with the next */
            /*   doc_element                                             */
            /* the doc_el_group depth was computed with these values,    */
            /*   and so must be reduced by them as well                  */
            /*************************************************************/

            if( frame.type != FRAME_none ) {
                g_blank_units_lines = cur_doc_el_group->first->blank_lines;
                cur_doc_el_group->first->blank_lines = 0;
                cur_doc_el_group->depth -= g_blank_units_lines;
            }
            g_subs_skip = cur_doc_el_group->first->subs_skip;
            cur_doc_el_group->first->subs_skip = 0;
            cur_doc_el_group->depth -= g_subs_skip;
            if( raw_p_skip > 0 ) {         // reset to default post_skip
                g_post_skip = wgml_fonts[FONT0].line_height;
                if( raw_p_skip == wgml_fonts[FONT0].line_height ) {
                    raw_p_skip = wgml_fonts[layout_work.fig.font].line_height;
                }
            }

            /* box drawn by dbox requires special handling */

            if( (frame.type == FRAME_box )
              && (bin_driver->dbox.text != NULL) ) {
                cur_doc_el_group->first->blank_lines = wgml_fonts[FONT0].line_height; // this is actually the depth used by the HLINE
            }

            /* top rule in a bottom fig requires special handling */

            if( (place == bottom_place)
              && (frame.type == FRAME_rule) ) {
                cur_doc_el_group->first->subs_skip = wgml_fonts[FONT0].line_height; // this is actually the depth used by the HLINE
                cur_doc_el_group->depth += wgml_fonts[FONT0].line_height; // this is actually the depth used by the HLINE
            }
            g_top_skip = cur_doc_el_group->first->top_skip;
            cur_doc_el_group->first->top_skip = 0;

            if( frame.type == FRAME_box ) {
                draw_box( cur_doc_el_group );
            }

            cur_group = NULL;
            if( place == bottom_place ) {       // bottom
                insert_col_bot( cur_doc_el_group );
                cur_group = n_page.col_bot;
            } else {
                if( t_page_width ) {
                    insert_page_width( cur_doc_el_group );
                    cur_group = n_page.page_width;
                } else {          // width was "column" or was "page" and page is single-column
                    insert_col_width( cur_doc_el_group );
                    cur_group = n_page.col_width;
                }
            }

            /************************************************************/
            /* 1. The destinations (t_page.page_width, t_page.cols->    */
            /*    col_width, t_page.cols->col_bot) take one item only   */
            /* 2. If the destination was empty, then the figure just    */
            /*    submitted went into it and is on the current page or  */
            /*    in the current column                                 */
            /* 3. If the destination was full, then the figure just     */
            /*    submitted went into n_page->page_width, or n_page->   */
            /*    col_width, or n_page->col_bot and is the last item    */
            /* 4. Each item in the n_page sections will go onto a new   */
            /*    page or column whose number can be computed from the  */
            /*    number of items in the list plus the current page     */
            /*    number                                                */
            /* NOTE: no attempt is made at this time to divide these    */
            /*    FIG/eFIG blocks, as that may force some or all of     */
            /*    figure processing into docpage.c (the insert_x()      */
            /*    functions and update_column()/update_t_page())        */
            /************************************************************/

            page_pred = g_page + 1;         // set to current page
            while( cur_group != NULL ) {
                page_pred++;                // set to future page
                cur_group = cur_group->next;
            }

            /************************************************************/
            /* This value is used after figs at the top of subsequent   */
            /* pages only: cur_doc_el_group has already been submitted  */
            /* and, if on the current page, its post_skip already       */
            /* copied                                                   */
            /************************************************************/

            if( (page_pred > (g_page + 1))
              && (place == top_place) ) {
                cur_doc_el_group->post_skip = raw_p_skip;
            }
        }
        if( pass == 1 ) {
            fig_entry->pageno = page_pred;
        } else {
            if( page_pred != fig_entry->pageno ) {  // page number changed
                fig_entry->pageno = page_pred;
                if( GlobalFlags.lastpass ) {        // last pass only
                    if( *figrefid != '\0' ) {     // FIG id exists
                        fig_fwd_refs = init_fwd_ref( fig_fwd_refs, figrefid );
                    }
                    ProcFlags.new_pagenr = true;
                }
            }
        }
    }

    t_page.cur_left = nest_cb->lm;          // reset various values
    t_page.max_width = nest_cb->rm;
    t_page.cur_width = 0;

    ProcFlags.concat = concat_save;
    ProcFlags.justify = justify_save;

    wk = nest_cb;
    nest_cb = nest_cb->prev;
    add_tag_cb_to_pool( wk );

    g_curr_font = nest_cb->font;

    t_page.cur_width = t_page.cur_left;

    g_scan_err = false;
    if( *p != '\0' ) {
        if( !input_cbs->hidden_head->ip_start
          && (*(p + 1) == '\0')
          && (*p == CONT_char) ) {      // text is continuation character only
//            if( &layout_work.fig.post_skip != NULL ) {
                g_post_skip = conv_vert_unit( &layout_work.fig.post_skip, g_text_spacing, layout_work.fig.font );
//            } else {
//                g_post_skip = 0;
//            }
        } else {
            do_force_pc( p );
        }
    } else {
        ProcFlags.force_pc = true;
    }
    if( t_page_width ) {
        t_page.max_width = t_page.last_pane->col_width;
    }
    if( pass > 1 ) {                    // not on first pass
        fig_entry = fig_entry->next;    // get to next FIG
    }
    g_text_spacing = spacing_save;
    g_scandata.s = g_scandata.e;
    return;
}


/***************************************************************************/
/*      :FIGCAP.<text line>                                                */
/* The figure caption tag is used within a figure to specify the caption   */
/* for the figure. The figure caption tag must be specified if the figure  */
/* has an identifier name associated with it. Layout defined text followed */
/* by the figure number and a delimiter is inserted before the caption     */
/* text (the default text and delimiter is "Figure" and a period). The     */
/* figure caption follows the main text of the figure.                     */
/***************************************************************************/

void gml_figcap( const gmltag * entry )
{
    char            buffer[NUM2STR_LENGTH + 1];
    char        *   prefix;
    char        *   p;
    unsigned        count;
    unsigned        current;
    text_chars  *   marker;

    (void)entry;

    start_doc_sect();
    scr_process_break();
    rs_loc = TLOC_figcap;

    g_scan_err = false;
    p = g_scandata.s;

    g_curr_font = layout_work.figcap.string_font;
    set_skip_vars( NULL, &layout_work.figcap.pre_lines, NULL, g_text_spacing, g_curr_font );

    ProcFlags.concat = true;            // even if was false on entry
    if( pass == 1 ) {                   // only on the first pass

        /* Only FIGs with captions are numbered */

        fig_count++;
        fig_entry->number = fig_count;

        /* Now build, save, and output the prefix */

        count = strlen( layout_work.figcap.string );
        sprintf( buffer, "%d", fig_entry->number );
        count += strlen( buffer );
        count++;                       // for the delimiter character
        prefix = (char *)mem_alloc( count + 1 );
        strcpy( prefix, layout_work.figcap.string );
        current = strlen( prefix );
        strcat( &prefix[current], buffer );
        current = strlen( prefix );
        prefix[current] = layout_work.figcap.delim;
        prefix[current + 1] = '\0';
        fig_entry->prefix = prefix;
        process_text( prefix, g_curr_font );
        prefix = NULL;
    } else {                            // use existing prefix
        process_text( fig_entry->prefix, g_curr_font );
    }

    if( ProcFlags.wh_device ) {             // Insert a marker
        marker = process_word( NULL, 0, g_curr_font, false );
        marker->type |= TXT_figcap;           // mark as from prefix string
        marker->x_address = t_page.cur_width;
        t_line->last->next = marker;
        marker->prev = t_line->last;
        t_line->last = marker;
        marker = NULL;
    }

    /* Output the caption text, if any */

    ProcFlags.ct = true;                    // emulate CT
    g_curr_font = layout_work.figcap.font;
    t_page.cur_width += wgml_fonts[g_curr_font].spc_width;
    t_page.cur_left = t_page.cur_width;

    if( *p != '\0' ) {
        SkipDot( p );                       // possible tag end
        SkipSpaces( p );                    // skip preceding spaces
        post_space = 0;                     // no additional space
        if( pass == 1 ) {                   // only on first pass
            fig_entry->text = mem_strdup( p );
        }
        process_text( fig_entry->text, g_curr_font );   // if text follows
    } else {
        ProcFlags.need_text = true;
    }
    fig_entry->flags |= FFH_figcap;         // mark as FIGCAP present, with or without text
    figcap_done = true;

    g_scandata.s = g_scandata.e;
    return;
}


/***************************************************************************/
/*      :FIGDESC.<paragraph elements>                                      */
/*               <basic document elements>                                 */
/* This tag signals the start of the description for a figure. The tag is  */
/* placed after the optional :figcap tag within a figure. The GML          */
/* processor automatically adds a colon(:) following the caption when a    */
/* figure description is present.                                          */
/***************************************************************************/

void gml_figdesc( const gmltag * entry )
{
    char    *   p;

    (void)entry;

    start_doc_sect();
    rs_loc = 0;

    g_scan_err = false;
    p = g_scandata.s;

    if( figcap_done ) {                         // FIGCAP was present
        post_space = 0;
        ProcFlags.ct = true;                    // emulate CT
        process_text( ":", g_curr_font);        // uses FIGCAP font
        g_curr_font = layout_work.figdesc.font; // change to FIGDESC font
    } else {                                    // FIGCAP not present
        scr_process_break();
        if( ProcFlags.has_aa_block ) {          // matches wgml 4.0
            t_page.max_width += tab_col;
        }
        g_curr_font = layout_work.figdesc.font;
        set_skip_vars( &layout_work.figdesc.pre_lines, NULL, NULL, g_text_spacing, g_curr_font );
    }

    nest_cb->font = g_curr_font;        // support font changes inside description

    SkipDot( p );                       // possible tag end
    if( *p != '\0' ) {
        ProcFlags.concat = true;        // even if was false on entry
        process_text( p, g_curr_font);  // if text follows
    } else {
        if( !figcap_done ) {            // if no FIGCAP was present
            g_blank_units_lines += g_subs_skip;
            g_subs_skip = 0;
            scr_process_break();
        }
    }
    g_scandata.s = g_scandata.e;
    return;
}

