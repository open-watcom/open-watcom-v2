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
* Description: implement .br (break)  script control word
*                    and related routines                            TBD
*
*  comments are from script-tso.txt
****************************************************************************/


#include "wgml.h"


/****************************************************************************/
/*  correctly place g_eol_ix when between two breaks                        */
/*    g_eol_ix is only non-NULL following a break (or no index items exist) */
/*    and is only non-NULL here if this is a second break with nothing      */
/*    to attach the index items to                                          */
/****************************************************************************/

static void attach_eol( void )
{
    eol_ix      *       cur_eol;
    text_line   *       cur_tl;

    /* Find last entry in g_eol_ix */

    cur_eol = g_eol_ix;
    while( cur_eol->next != NULL ) {
        cur_eol = cur_eol->next;      // find last text_line
    }

    /* Prepend g_eol_ix to the appropriate eol_index field */

    if( (t_doc_el_group != NULL ) && (t_doc_el_group->first != NULL ) ) {
        switch( t_doc_el_group->last->type ) {
        case ELT_binc:
            if( t_doc_el_group->last->element.binc.eol_index == NULL ) {
                t_doc_el_group->last->element.binc.eol_index = g_eol_ix;
            } else {
                cur_eol->next = t_doc_el_group->last->element.binc.eol_index;
                t_doc_el_group->last->element.binc.eol_index = g_eol_ix;
            }
            g_eol_ix = NULL;
            break;
        case ELT_dbox:
            if( t_doc_el_group->last->element.dbox.eol_index == NULL ) {
                t_doc_el_group->last->element.dbox.eol_index = g_eol_ix;
            } else {
                cur_eol->next = t_doc_el_group->last->element.dbox.eol_index;
                t_doc_el_group->last->element.dbox.eol_index = g_eol_ix;
            }
            g_eol_ix = NULL;
            break;
        case ELT_graph:
            if( t_doc_el_group->last->element.graph.eol_index == NULL ) {
                t_doc_el_group->last->element.graph.eol_index = g_eol_ix;
            } else {
                cur_eol->next = t_doc_el_group->last->element.graph.eol_index;
                t_doc_el_group->last->element.graph.eol_index = g_eol_ix;
            }
            g_eol_ix = NULL;
            break;
        case ELT_hline:
            if( t_doc_el_group->last->element.hline.eol_index == NULL ) {
                t_doc_el_group->last->element.hline.eol_index = g_eol_ix;
            } else {
                cur_eol->next = t_doc_el_group->last->element.hline.eol_index;
                t_doc_el_group->last->element.hline.eol_index = g_eol_ix;
            }
            g_eol_ix = NULL;
            break;
        case ELT_text:
            cur_tl = t_doc_el_group->last->element.text.first;
            if( cur_tl != NULL ) {              // text_lines exist
                while( cur_tl->next != NULL ) {
                    cur_tl = cur_tl->next;      // find last text_line
                }
                if( cur_tl->eol_index == NULL ) {
                    cur_tl->eol_index = g_eol_ix;
                } else {
                    cur_eol->next = cur_tl->eol_index;
                    cur_tl->eol_index = g_eol_ix;
                }
                g_eol_ix = NULL;
            }
            break;
        case ELT_vline:
            if( t_doc_el_group->last->element.vline.eol_index == NULL ) {
                t_doc_el_group->last->element.vline.eol_index = g_eol_ix;
            } else {
                cur_eol->next = t_doc_el_group->last->element.vline.eol_index;
                t_doc_el_group->last->element.vline.eol_index = g_eol_ix;
            }
            g_eol_ix = NULL;
            break;
        case ELT_vspace:
            if( t_doc_el_group->last->element.vspace.eol_index == NULL ) {
                t_doc_el_group->last->element.vspace.eol_index = g_eol_ix;
            } else {
                cur_eol->next = t_doc_el_group->last->element.vspace.eol_index;
                t_doc_el_group->last->element.vspace.eol_index = g_eol_ix;
            }
            g_eol_ix = NULL;
            break;
        default :
            internal_err_exit( __FILE__, __LINE__ ); // bad element type value
            /* never return */
        }
    } else if( t_page.last_col_main != NULL ) {
        switch( t_page.last_col_main->type ) {
        case ELT_binc:
            if( t_page.last_col_main->element.binc.eol_index == NULL ) {
                t_page.last_col_main->element.binc.eol_index = g_eol_ix;
            } else {
                cur_eol->next = t_page.last_col_main->element.binc.eol_index;
                t_page.last_col_main->element.binc.eol_index = g_eol_ix;
            }
            g_eol_ix = NULL;
            break;
        case ELT_dbox:
            if( t_page.last_col_main->element.dbox.eol_index == NULL ) {
                t_page.last_col_main->element.dbox.eol_index = g_eol_ix;
            } else {
                cur_eol->next = t_page.last_col_main->element.dbox.eol_index;
                t_page.last_col_main->element.dbox.eol_index = g_eol_ix;
            }
            g_eol_ix = NULL;
            break;
        case ELT_graph:
            if( t_page.last_col_main->element.graph.eol_index == NULL ) {
                t_page.last_col_main->element.graph.eol_index = g_eol_ix;
            } else {
                cur_eol->next = t_page.last_col_main->element.graph.eol_index;
                t_page.last_col_main->element.graph.eol_index = g_eol_ix;
            }
            g_eol_ix = NULL;
            break;
        case ELT_hline:
            if( t_page.last_col_main->element.hline.eol_index == NULL ) {
                t_page.last_col_main->element.hline.eol_index = g_eol_ix;
            } else {
                cur_eol->next = t_page.last_col_main->element.hline.eol_index;
                t_page.last_col_main->element.hline.eol_index = g_eol_ix;
            }
            g_eol_ix = NULL;
            break;
        case ELT_text:
            cur_tl = t_page.last_col_main->element.text.first;
            if( cur_tl != NULL ) {              // text_lines exist
                while( cur_tl->next != NULL ) {
                    cur_tl = cur_tl->next;      // find last text_line
                }
                if( cur_tl->eol_index == NULL ) {
                    cur_tl->eol_index = g_eol_ix;
                } else {
                    cur_eol->next = cur_tl->eol_index;
                    cur_tl->eol_index = g_eol_ix;
                }
                g_eol_ix = NULL;
            } else {    // only t_page is left!
                if( t_page.eol_index == NULL ) {
                    t_page.eol_index = g_eol_ix;
                } else {
                    cur_eol->next = t_page.eol_index;
                    t_page.eol_index = g_eol_ix;
                }
                g_eol_ix = NULL;
            }
            break;
        case ELT_vline:
            if( t_page.last_col_main->element.vline.eol_index == NULL ) {
                t_page.last_col_main->element.vline.eol_index = g_eol_ix;
            } else {
                cur_eol->next = t_page.last_col_main->element.vline.eol_index;
                t_page.last_col_main->element.vline.eol_index = g_eol_ix;
            }
            g_eol_ix = NULL;
            break;
        case ELT_vspace:
            if( t_page.last_col_main->element.vspace.eol_index == NULL ) {
                t_page.last_col_main->element.vspace.eol_index = g_eol_ix;
            } else {
                cur_eol->next = t_page.last_col_main->element.vspace.eol_index;
                t_page.last_col_main->element.vspace.eol_index = g_eol_ix;
            }
            g_eol_ix = NULL;
            break;
        default :
            internal_err_exit( __FILE__, __LINE__ ); // bad element type value
            /* never return */
        }
    } else {    // only t_page is left!
        if( t_page.eol_index == NULL ) {
            t_page.eol_index = g_eol_ix;
        } else {
            cur_eol->next = t_page.eol_index;
            t_page.eol_index = g_eol_ix;
        }
        g_eol_ix = NULL;
    }
    return;
}


/**************************************************************************/
/* BREAK forces  the current partially-full output  line (if any)   to be */
/* printed without  justification (if on),  and  a new output line  to be */
/* begun.                                                                 */
/*                                                                        */
/*      +-------+--------------------------------------------------+      */
/*      |       |                                                  |      */
/*      |  .BR  |    <line>                                        |      */
/*      |       |                                                  |      */
/*      +-------+--------------------------------------------------+      */
/*                                                                        */
/* The optional  "line" operand starts one  blank after the  control word */
/* and may be text or another control word.                               */
/*                                                                        */
/* NOTES                                                                  */
/* (1) Many other control words also cause  a Break.   No Break is neces- */
/*     sary  when one  of these  is  present.   The  description of  each */
/*     control word  indicates whether  it causes  a break,   and summary */
/*     lists are contained in other components of this document.          */
/* (2) A blank or tab in column one of  an input line has the effect of a */
/*     Break immediately before the line.    See Leading Blank (.LB)  and */
/*     Leading Tab (.LT) to control this automatic Break.                 */
/* (3) If Concatenate NO is in effect, all lines appear to be followed by */
/*     a Break.                                                           */
/*                                                                        */
/* EXAMPLES                                                               */
/* (1) This is a text line                                                */
/*     .br                                                                */
/*     followed by a .BR control word.                                    */
/*     produces:                                                          */
/*     This is a text line                                                */
/*     followed by a .BR control word.                                    */
/*                                                                        */
/*     Without the Break, it would print as:                              */
/*     This is a text line followed by a .BR control word.                */
/**************************************************************************/

void    scr_br( void )
{
    char        *   p;

    p = g_scandata.s;

    if( *p != '\0' ) {
        SkipSpaces( p );
        if( *p != '\0' ) {
            split_input( g_scandata.s, p, input_cbs->fmflags );   // line operand
        }
    }
    scr_process_break();                // break processing

    scan_restart = g_scandata.e;
    return;
}


/***************************************************************************/
/*  insert incomplete line if any into t_element                           */
/*  insert t_element into t_page and reset it to NULL                      */
/***************************************************************************/

void  scr_process_break( void )
{
    doc_element     *cur_el;
    text_chars      *marker  = NULL;

    if( (g_script_style.style != SCT_none) && ProcFlags.scr_scope_eip ) {
        scr_style_end();
    }

    if( g_eol_ix != NULL ) {
        attach_eol();
    }

    if( t_line != NULL ) {
        if( t_line->first != NULL ) {

            /* Insert a marker if CO OFF and post_space > 0 */

            if( !ProcFlags.concat && (post_space > 0) ) {
                marker = process_word( NULL, 0, g_curr_font, false );
                marker->type = TXT_norm;
                t_page.cur_width += post_space;
                post_space = 0;
                marker->x_address = t_page.cur_width;
                t_line->last->next = marker;
                marker->prev = t_line->last;
                if( t_line->line_height < wgml_fonts[g_curr_font].line_height ) {
                    t_line->line_height = wgml_fonts[g_curr_font].line_height;
                }
                t_line->last = marker;
                marker = NULL;
            }

            /********************************************************************/
            /* The last line of a paragraph is not left/right-justified or      */
            /* half-justified, but it is left-justified (ie, left alone),       */
            /* right-justified (ragged left) or centered                        */
            /********************************************************************/

            process_line_full( t_line, ((ProcFlags.justify != JUST_off) &&
                (ProcFlags.justify != JUST_on) && (ProcFlags.justify != JUST_half)) );
            t_line = NULL;
        }
    }
    if( t_element != NULL ) {
        if( ProcFlags.concat && ProcFlags.in_reduced ) {
            g_blank_units_lines = wgml_fonts[g_curr_font].line_height;
            cur_el = init_doc_el( ELT_vspace, 0 );
            insert_col_main( cur_el );
            if( t_element->type == ELT_text ) {   // not clear what to do for other types
                t_element->element.text.overprint = true;
            }
        }

        while( t_element != NULL ) {
            cur_el = t_element;
            t_element = t_element->next;
            cur_el->next = NULL;
            insert_col_main( cur_el );
        }
        t_el_last = NULL;
    } else if( ProcFlags.titlep_starting ) {    // TITLE : no text before break
        set_skip_vars( NULL, NULL, NULL, g_text_spacing, g_curr_font);
        g_subs_skip = 0;                        // matches wgml 4.0
        t_element = init_doc_el( ELT_text, wgml_fonts[g_curr_font].line_height );
        t_element->element.text.first = alloc_text_line();
        t_element->element.text.first->line_height = wgml_fonts[g_curr_font].line_height;
        t_element->element.text.first->first = NULL;
        insert_col_main( t_element );
        t_element = NULL;
        t_el_last = NULL;
    } else if( ProcFlags.note_starting
      || ProcFlags.para_starting ) {        // NOTE with no text before break

        if( ProcFlags.block_starting ) {    // P, PC, NOTE paragraph is empty
            g_subs_skip += g_post_skip;
            g_post_skip = 0;
            ProcFlags.block_starting = false;
        }

        if( ProcFlags.note_starting ) {     // NOTE with no text before break
            t_page.cur_left = note_lm;
            ProcFlags.note_starting = false;
        } else if( ProcFlags.para_starting ) {  // LP, P or PC with no text before break
            /*
             * ProcFlags.block_starting must be done before this point or the
             * call to set_skip_vars() will change the results
             * Putting set_skip_vars() first can affect the result of the if()
             */
            if( (g_line_indent > 0) || (g_blank_units_lines > 0) ) {
                set_skip_vars( NULL, NULL, NULL, g_text_spacing, g_curr_font);

                t_element = init_doc_el( ELT_text, wgml_fonts[g_curr_font].line_height );
                if( g_line_indent == 0 ) {  // special case
                    t_element->depth = 0;
                }
                t_element->element.text.first = alloc_text_line();

                if( g_line_indent > 0 ) {
                    t_element->element.text.first->line_height = wgml_fonts[g_curr_font].line_height;
                } else {
                    t_element->element.text.first->line_height = 0;
                }
                t_element->element.text.first->first = NULL;
                insert_col_main( t_element );
                t_element = NULL;
                t_el_last = NULL;
            } else {
                set_skip_vars( NULL, NULL, NULL, g_text_spacing, g_curr_font);
            }
        }
    } else if( g_blank_text_lines > 0  ) {                  // pending blank lines
        set_skip_vars( NULL, NULL, NULL, 1, g_curr_font );  // emits vspace doc_element
    } else if( g_blank_units_lines > 0 ) {   // arbitrary blank space
        t_element = init_doc_el( ELT_vspace, 0 );
        insert_col_main( t_element );
        t_element = NULL;
        t_el_last = NULL;
    }
    set_h_start();      // to stop paragraph indent from being used after a break
    ProcFlags.br_done = true;
    ProcFlags.cont_char = false;
    ProcFlags.dd_space = false;
    ProcFlags.para_starting = false;
    ProcFlags.para_has_text = false;
    ProcFlags.scr_scope_eip = false;
    ProcFlags.skips_valid = false;
    ProcFlags.titlep_starting = false;
    c_stop = NULL;
    kbtab_count = 0;
    post_space = 0;

    return;
}

