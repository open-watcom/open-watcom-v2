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
* Description:  Page-oriented output
*
****************************************************************************/


#include "wgml.h"

static  uint32_t    bottom_depth;   // used in setting banners
static  uint32_t    old_max_depth;  // used in splitting elements
static  uint32_t    top_depth;      // used in setting banners

/***************************************************************************/
/*  does the actual output to the device                                   */
/***************************************************************************/

static void do_el_list_out( doc_element * in_element )
{
    doc_element *   save;
    text_line   *   cur_line;
    text_chars  *   cur_chars;

    while( in_element != NULL ) {
        switch( in_element->type ) {
        case el_binc :
            if( GlobalFlags.lastpass ) {
                if( in_element->next != NULL ) {
                    if( in_element->next->type == el_text ) {
                        if( in_element->next->element.text.first != NULL ) {
                            if( in_element->next->element.text.first->first != NULL ) {
                                if( in_element->next->element.text.first->first->font > 0 ) {
                                    in_element->element.binc.force_FONT0 = true;
                                }
                            }
                        }
                    } else if( in_element->next->type == el_vspace ) {
                        if( in_element->next->element.vspace.font > 0 ) {
                            in_element->element.binc.force_FONT0 = true;
                        }
                    }
                }
                ob_binclude( &in_element->element.binc );
                if( in_element->element.binc.eol_index != NULL ) {
                    eol_index_page( in_element->element.binc.eol_index, g_page );
                }
            }
            break;
        case el_dbox :  // should only be found if DBOX block exists
            if( GlobalFlags.lastpass ) {
                fb_dbox( &in_element->element.dbox );
                if( in_element->element.dbox.eol_index != NULL ) {
                    eol_index_page( in_element->element.dbox.eol_index, g_page );
                }
            }
            break;
        case el_graph :
            if( GlobalFlags.lastpass ) {
                if( ProcFlags.ps_device ) {   // only available to PS device
                    ob_graphic( &in_element->element.graph );
                }
                if( in_element->element.graph.eol_index != NULL ) {
                    eol_index_page( in_element->element.graph.eol_index, g_page );
                }
            }
            break;
        case el_hline :  // should only be found if HLINE block exists
            if( GlobalFlags.lastpass ) {
                fb_hline( &in_element->element.hline );
                if( in_element->element.hline.eol_index != NULL ) {
                    eol_index_page( in_element->element.hline.eol_index, g_page );
                }
            }
            break;
        case el_text :
            if( in_element->element.text.entry != NULL ) {       // heading
                out_head_page( in_element->element.text.entry,
                               in_element->element.text.ref, g_page );
            }
            if( GlobalFlags.lastpass ) {
                ProcFlags.force_op = in_element->element.text.force_op;
                ProcFlags.overprint = in_element->element.text.overprint;
                for( cur_line = in_element->element.text.first;
                        cur_line != NULL; cur_line = cur_line->next ) {
                    fb_output_textline( cur_line );
                    if( cur_line->eol_index != NULL ) {
                        eol_index_page( cur_line->eol_index, g_page );
                    }
                    /* Set value for OC output (if all goes well) */
                    if( (in_element->next == NULL) && (cur_line->next == NULL) ) {
                        if( cur_line->first != NULL ) {
                            cur_chars = cur_line->first;
                            while( cur_chars->next != NULL ) {
                                cur_chars = cur_chars->next;
                            }
                            g_oc_hpos = cur_chars->x_address + cur_chars->width;
                        }
                    }
                }
                ProcFlags.force_op = false;
            }
            break;
        case el_vline :  // should only be found if VLINE block exists
            if( GlobalFlags.lastpass ) {
               fb_vline( &in_element->element.vline );
                if( in_element->element.vline.eol_index != NULL ) {
                    eol_index_page( in_element->element.vline.eol_index, g_page );
                }
            }
            break;
        case el_vspace :
            /* next element is positioned to skip the space */
            if( GlobalFlags.lastpass ) {
                if( in_element->element.vspace.eol_index != NULL ) {
                    eol_index_page( in_element->element.vspace.eol_index, g_page );
                }
            }
            break;
        default :
            internal_err( __FILE__, __LINE__ );
            break;
        }
        save = in_element->next;
        in_element->next = NULL;            // clear only current element
        add_doc_el_to_pool( in_element );
        in_element = save;
    }

    return;
}

/***************************************************************************/
/*  consolidate multiple columns as appropriate                            */
/*  NOTE: count should be at least 2                                       */
/*        only columns i < count are processed                             */
/*        implemented as several relatively simple steps rather than as    */
/*        fewer relatively complicated steps                               */
/***************************************************************************/

static void consolidate_array( doc_element * array[MAX_COL], uint8_t count )
{
    bool                done;
    doc_element     *   cur_h_el;
    doc_element     *   cur_nt_el[MAX_COL]; // non-text doc_elements
    doc_element     *   cur_nt_el_list;     // non-text doc_elements
    doc_element     *   cur_out_el;         // text doc_elements
    doc_element     *   cur_te_el[MAX_COL]; // text doc_elements
    doc_element     *   cur_v_el;
    doc_element     *   nt_el[MAX_COL];     // non-text doc_elements
    doc_element     *   nt_el_list;         // non-text doc_elements
    doc_element     *   out_el;             // text doc_elements/doc_elements in general
    doc_element     *   sav_el;
    doc_element     *   sav_nt_el;
    doc_element     *   sav_te_el;
    doc_element     *   te_el[MAX_COL];     // text doc_elements
    doc_el_group    *   cur_nt_el_group;    // non-text doc_element group
    doc_el_group    *   nt_el_group;        // non-text doc_element group
    int                 i;
    text_line       *   cur_tl_list;        // text_lines
    text_line       *   cur_tl[MAX_COL];    // text_lines
    text_line       *   sav_tl;
    text_line       *   tl[MAX_COL];        // text_lines
    uint32_t            left_pos;
    uint32_t            top_pos;

    /* Initialize the local arrays */

    for( i = 0; i < count; i++ ) {
        cur_nt_el[i] = NULL;
        cur_te_el[i] = NULL;
        nt_el[i] = NULL;
        te_el[i] = NULL;
    }

    /* Separate the doc_elements into text and non-text elements arrays */

    for( i = 0; i < count; i++ ) {
        sav_nt_el = NULL;
        sav_te_el = NULL;
        while( array[i] != NULL ) {
            if( array[i]->type == el_text ) {
                if( cur_te_el[i] == NULL ) {
                    cur_te_el[i] = array[i];
                    te_el[i] = array[i];
                } else {
                    sav_te_el = cur_te_el[i];
                    cur_te_el[i]->next = array[i];
                    cur_te_el[i] = cur_te_el[i]->next;
                }
                array[i] = array[i]->next;
                cur_te_el[i]->next = NULL;
            } else {
                if( cur_nt_el[i] == NULL ) {
                    cur_nt_el[i] = array[i];
                    nt_el[i] = array[i];
                } else {
                    sav_nt_el = cur_nt_el[i];
                    cur_nt_el[i]->next = array[i];
                    cur_nt_el[i] = cur_nt_el[i]->next;
                }
                array[i] = array[i]->next;
                cur_nt_el[i]->next = NULL;
            }
        }
    }

    /* Merge the non-text elements array into a linked list of doc_el_groups */

    done = false;
    nt_el_group = NULL;
    cur_nt_el_group = NULL;
    cur_v_el = NULL;

    while( !done ) {
        if( bin_driver->y_positive == 0x00 ) {
            top_pos = g_page_bottom_org;
        } else {
            top_pos = t_page.page_top;
        }
        for( i = 0; i < count; i++ ) {
            if( nt_el[i] == NULL ) continue;    // some columns may be empty
            if( bin_driver->y_positive == 0x00 ) {
                if( top_pos < nt_el[i]->v_pos ) {
                    top_pos = nt_el[i]->v_pos;
                }
            } else {
                if( top_pos > nt_el[i]->v_pos ) {
                    top_pos = nt_el[i]->v_pos;
                }
            }
        }

        for( i = 0; i < count; i++ ) {
            if( nt_el[i] == NULL ) continue;    // some columns may be empty
            if( nt_el[i]->v_pos == top_pos ) {
                if( cur_v_el == NULL ) {
                    cur_nt_el_group = alloc_doc_el_group( gt_none );
                    cur_nt_el_group->first = nt_el[i];
                    cur_v_el = nt_el[i];
                    nt_el_group = cur_nt_el_group;
                } else {
                    if( cur_nt_el_group->first->v_pos == top_pos ) {
                        cur_v_el->next = nt_el[i];
                        cur_v_el = nt_el[i];
                    } else {
                        cur_nt_el_group->next = alloc_doc_el_group( gt_none );
                        cur_nt_el_group = cur_nt_el_group->next;
                        cur_nt_el_group->first = nt_el[i];
                        cur_v_el = nt_el[i];
                    }
                }
                nt_el[i] = nt_el[i]->next;
                cur_v_el->next = NULL;
            }
        }

        done = true;
        for( i = 0; i < count; i++ ) {
            if( nt_el[i] != NULL ) {
                done = false;
                break;
            }
        }
    }

    /* Sort each v_pos set by h_pos & copy to final list */

    nt_el_list = NULL;
    cur_nt_el_list = nt_el_list;
    while( nt_el_group != NULL ) {
        cur_v_el = nt_el_group->first;          // detach doc_element list
        nt_el_group->first = NULL;
        top_pos = cur_v_el->v_pos;
        if( cur_v_el->next == NULL ) {
            if( nt_el_list == NULL ) {
                cur_nt_el_list = cur_v_el;
                nt_el_list = cur_v_el;
            } else {
                cur_nt_el_list->next = cur_v_el;
                cur_nt_el_list = cur_nt_el_list->next;
            }
        } else if( cur_v_el->next->v_pos == top_pos ) {
            while( cur_v_el != NULL) {
                sav_nt_el = NULL;
                left_pos = t_page.page_left + t_page.page_width;
                cur_h_el = cur_v_el;
                while( cur_h_el != NULL) {
                    if( cur_h_el->h_pos < left_pos ) {
                        left_pos = cur_h_el->h_pos;
                    }
                    cur_h_el = cur_h_el->next;
                }
                cur_h_el = cur_v_el;
                while( cur_h_el != NULL) {
                    if( cur_h_el->h_pos == left_pos ) {
                        if( nt_el_list == NULL ) {
                            cur_nt_el_list = cur_h_el;
                            nt_el_list = cur_h_el;
                        } else {
                            cur_nt_el_list->next = cur_h_el;
                            cur_nt_el_list = cur_nt_el_list->next;
                        }
                    } else if( sav_nt_el == NULL ) {
                        sav_nt_el = cur_h_el;
                    }
                    cur_h_el = cur_h_el->next;
                    cur_nt_el_list->next = NULL;
                }
                cur_v_el = sav_nt_el;
            }
        }
        cur_nt_el_group = nt_el_group;
        nt_el_group = nt_el_group->next;
        cur_nt_el_group->next = NULL;
        add_doc_el_group_to_pool( cur_nt_el_group );
    }

    /* Initialize the text_lines array */

    for( i = 0; i < count; i++ ) {
        tl[i] = NULL;
    }

    /* Extract the text_lines from the remaining doc_elements into an array */

    for( i = 0; i < count; i++ ) {
        while( te_el[i] != NULL ) {
            if( te_el[i] == NULL ) continue;    // some columns may be empty
            while( te_el[i]->element.text.first != NULL ) {
                if( tl[i] == NULL ) {
                    cur_tl[i] = te_el[i]->element.text.first;
                    tl[i] = te_el[i]->element.text.first;
                } else {
                    cur_tl[i]->next = te_el[i]->element.text.first;
                    cur_tl[i] = cur_tl[i]->next;
                }
                te_el[i]->element.text.first = te_el[i]->element.text.first->next;
                cur_tl[i]->next = NULL;
            }
            sav_el = te_el[i];
            te_el[i] = te_el[i]->next;
            sav_el->next = NULL;
            add_doc_el_to_pool( sav_el );
        }
    }

    /* Merge the text_lines array into a single text element */

    done = false;
    out_el = NULL;
    cur_tl_list = NULL;

    while( !done ) {
        if( bin_driver->y_positive == 0x00 ) {
            top_pos = g_page_bottom_org;
        } else {
            top_pos = t_page.page_top;
        }
        for( i = 0; i < count; i++ ) {
            if( tl[i] == NULL ) continue;       // some columns may be empty
            if( bin_driver->y_positive == 0x00 ) {
                if( top_pos < tl[i]->y_address ) {
                    top_pos = tl[i]->y_address;
                }
            } else {
                if( top_pos > tl[i]->y_address ) {
                    top_pos = tl[i]->y_address;
                }
            }
        }

        for( i = 0; i < count; i++ ) {
            if( tl[i] == NULL ) continue;          // some columns may be empty
            if( tl[i]->y_address == top_pos ) {
                if( out_el == NULL ) {
                    out_el = alloc_doc_el( el_text );
                }
                if( cur_tl_list == NULL ) {
                    cur_tl_list = tl[i];
                    out_el->element.text.first = tl[i];
                    tl[i] = tl[i]->next;
                } else {
                    cur_tl_list->next = tl[i];
                    cur_tl_list = cur_tl_list->next;
                    tl[i] = tl[i]->next;
                }
            }
        }

        done = true;
        for( i = 0; i < count; i++ ) {
            if( tl[i] != NULL ) {
                done = false;
                break;
            }
        }
    }

    if( out_el == NULL ) {                      // page consists entirely of non-text elements
        out_el = nt_el_list;
    } else if( nt_el_list != NULL ) {           // out_el is done if there are no non-text elements

        /* Insert the non-text list into the text element list */

        done = false;
        cur_out_el = out_el;
        sav_el = cur_out_el;
        cur_tl_list = out_el->element.text.first;
        sav_nt_el = NULL;
        sav_tl = NULL;
        while( (nt_el_list != NULL) || (cur_tl_list != NULL) ) {

            if( cur_tl_list == NULL ) {

                /* No more text doc elements: append remaining non-text elements */

                if( cur_out_el == NULL ) {
                    cur_out_el = nt_el_list;
                } else {
                    cur_out_el->next = nt_el_list;
                    cur_out_el = cur_out_el->next;
                }
                nt_el_list = NULL;

            } else if( nt_el_list == NULL ) {

                /* No more non-text elements: do split if non-text element was just inserted */

                if( cur_out_el == NULL ) {
                    if( sav_el->type != el_text ) {
                        sav_el->next = alloc_doc_el( el_text );
                        sav_el->next->element.text.first = cur_tl_list->next;
                    }
                } else {
                    if( cur_out_el->type != el_text ) {
                        cur_out_el->next = alloc_doc_el( el_text );
                        cur_out_el->next->element.text.first = cur_tl_list->next;
                    }
                }
                cur_tl_list = NULL;

            } else {                            // both have entries

                /* Find next non-text element to insert */

                if( bin_driver->y_positive == 0x00 ) {
                    while( (cur_tl_list != NULL) && (cur_tl_list->y_address > nt_el_list->v_pos) ) {
                        sav_tl = cur_tl_list;
                        cur_tl_list = cur_tl_list->next;
                    }
                } else {
                    while( (cur_tl_list != NULL) && (cur_tl_list->y_address < nt_el_list->v_pos) ) {
                        sav_tl = cur_tl_list;
                        cur_tl_list = cur_tl_list->next;
                    }
                }

                while( (cur_tl_list != NULL) && (sav_tl != NULL) &&
                       (cur_tl_list->y_address == nt_el_list->v_pos) &&
                       (cur_tl_list->first->x_address < nt_el_list->h_pos) ) {
                    sav_tl = cur_tl_list;
                    cur_tl_list = cur_tl_list->next;
                }

                if( cur_tl_list != NULL ) {

                    if( sav_tl == NULL ) {

                        /* Insert the non-text element(s) before the first out_el element */

                        sav_el = out_el;
                        out_el = nt_el_list;
                        sav_nt_el = nt_el_list;
                        nt_el_list = nt_el_list->next;
                        cur_out_el = out_el;
                        if( bin_driver->y_positive == 0x00 ) {
                            while( (nt_el_list != NULL) &&
                                   (cur_tl_list->y_address < nt_el_list->v_pos) ) {
                                sav_nt_el = nt_el_list;
                                nt_el_list = nt_el_list->next;
                            }
                        } else {
                            while( (nt_el_list != NULL) &&
                                   (cur_tl_list->y_address > nt_el_list->v_pos) ) {
                                sav_nt_el = nt_el_list;
                                nt_el_list = nt_el_list->next;
                            }
                        }
                        cur_out_el = sav_nt_el;
                        cur_out_el->next = sav_el;
                        cur_out_el = cur_out_el->next;

                    } else {

                        /* Insert the non-text element(s) found into the output list */

                        cur_out_el->next = nt_el_list;
                        sav_nt_el = nt_el_list;
                        nt_el_list = nt_el_list->next;
                        cur_out_el = cur_out_el->next;
                        if( bin_driver->y_positive == 0x00 ) {
                            while( (nt_el_list != NULL) &&
                                    (cur_tl_list->y_address < nt_el_list->v_pos) ) {
                                sav_nt_el = nt_el_list;
                                nt_el_list = nt_el_list->next;
                            }
                        } else {
                            while( (nt_el_list != NULL) &&
                                   (cur_tl_list->y_address > nt_el_list->v_pos) ) {
                                sav_nt_el = nt_el_list;
                                nt_el_list = nt_el_list->next;
                            }
                        }
                        cur_out_el = sav_nt_el;
                        cur_out_el->next = alloc_doc_el( el_text );
                        cur_out_el = cur_out_el->next;
                        cur_out_el->element.text.first = cur_tl_list;
                        sav_tl->next = NULL;
                        sav_tl = cur_tl_list;
                        cur_tl_list = cur_tl_list->next;
                    }
                }
            }
        }
    }

    /* text_lines at the same vertical position in the same doc_element must be merged */

    cur_out_el = out_el;
    while( cur_out_el != NULL ) {
        if( cur_out_el->type == el_text ) {         // text elements only
            cur_tl_list = cur_out_el->element.text.first;
            sav_tl = cur_tl_list;
            cur_tl_list = cur_tl_list->next;
            while( cur_tl_list != NULL ) {
                if( sav_tl->y_address == cur_tl_list->y_address ) {
                    if( cur_tl_list->first == NULL ) {      // empty line
                        /* nothing to do, will be discarded below */
                    } else if( sav_tl->first == NULL ) {    // empty line
                        sav_tl->first = cur_tl_list->first;
                        sav_tl->last = cur_tl_list->last;
                    } else {
                        sav_tl->last->next = cur_tl_list->first;
                        cur_tl_list->first->prev = sav_tl->last;
                        sav_tl->last = cur_tl_list->last;
                    }
                    sav_tl->next = cur_tl_list->next;
                    cur_tl_list->next = NULL;
                    cur_tl_list->first = NULL;
                    cur_tl_list->last = NULL;
                    add_text_line_to_pool( cur_tl_list );
                    cur_tl_list = sav_tl->next;
                    if( cur_tl_list != NULL ) {
                        cur_tl_list = cur_tl_list->next;
                    }
                } else {
                    sav_tl = sav_tl->next;
                    if( cur_tl_list == sav_tl ) {
                        cur_tl_list = cur_tl_list->next;
                    }
                }
            }
        }
        cur_out_el = cur_out_el->next;
    }

    /* Output linked list of doc_elements */

    do_el_list_out( out_el );

    return;
}

/***************************************************************************/
/*  set the horizontal and vertical positions in a linked list of elements */
/*  returns value of at_top                                                */
/***************************************************************************/

static bool set_positions( doc_element * list, uint32_t h_start, uint32_t v_start )
{
    bool            at_top;
    bool            op_done;
    bool            shade_done;
    bool            use_spacing;
    doc_element *   cur_el;
    text_chars  *   cur_text;
    text_line   *   cur_line;
    uint32_t        cur_spacing;
    uint32_t        offset;
    uint32_t        old_v_start;

    at_top = !ProcFlags.page_started;
    g_cur_v_start = v_start;
    shade_done = false;

    for( cur_el = list; cur_el != NULL; cur_el = cur_el->next ) {
        use_spacing = false;
        if( at_top ) {
            if( cur_el->type == el_vspace ) {
                cur_spacing = cur_el->blank_lines + cur_el->top_skip;
            } else if( cur_el->blank_lines > 0 ) {
                cur_spacing = cur_el->blank_lines + cur_el->subs_skip;
            } else {
                cur_spacing = cur_el->top_skip;
            }
        } else {
            cur_spacing = cur_el->blank_lines + cur_el->subs_skip;
            use_spacing = (cur_spacing == 0);       // see el_text below
        }

        switch( cur_el->type ) {
        case el_binc :
            cur_el->element.binc.cur_left += h_start;
            cur_el->h_pos = cur_el->element.binc.cur_left;
            cur_el->element.binc.at_top = at_top && (t_page.top_banner == NULL);
            if( bin_driver->y_positive == 0x00 ) {
                g_cur_v_start -= cur_spacing;
            } else {
                g_cur_v_start += cur_spacing;
            }
            cur_el->element.binc.y_address = g_cur_v_start;
            cur_el->v_pos = g_cur_v_start;
            if( bin_driver->y_positive == 0x00 ) {
                g_cur_v_start -= cur_el->depth;
            } else {
                g_cur_v_start += cur_el->depth;
            }
            at_top = false;
            break;
        case el_dbox :
            cur_el->element.dbox.h_start += h_start;
            cur_el->h_pos = cur_el->element.dbox.h_start;
            if( bin_driver->y_positive == 0x00 ) {
                g_cur_v_start -= cur_spacing;
            } else {
                g_cur_v_start += cur_spacing;
            }
            cur_el->element.dbox.v_start = g_cur_v_start;
            cur_el->v_pos = g_cur_v_start;
            if( bin_driver->y_positive == 0x00 ) {
                g_cur_v_start -= cur_el->depth;
            } else {
                g_cur_v_start += cur_el->depth;
            }
            at_top = false;
            break;
        case el_graph :
            cur_el->element.graph.cur_left += h_start;
            cur_el->h_pos = cur_el->element.graph.cur_left;
            cur_el->element.graph.at_top = at_top && (t_page.top_banner == NULL);
            if( bin_driver->y_positive == 0x00 ) {
                g_cur_v_start -= cur_spacing;
            } else {
                g_cur_v_start += cur_spacing;
            }
            cur_el->element.graph.y_address = g_cur_v_start;
            cur_el->v_pos = g_cur_v_start;
            if( bin_driver->y_positive == 0x00 ) {
                g_cur_v_start -= cur_el->depth;
            } else {
                g_cur_v_start += cur_el->depth;
            }
            at_top = false;
            break;
        case el_hline :
            cur_el->element.hline.h_start += h_start;
            cur_el->h_pos = cur_el->element.hline.h_start;
            if( bin_driver->y_positive == 0x00 ) {
                g_cur_v_start -= cur_spacing;
            } else {
                g_cur_v_start += cur_spacing;
            }
            cur_el->element.hline.v_start = g_cur_v_start;
            cur_el->v_pos = g_cur_v_start;
            if( bin_driver->y_positive == 0x00 ) {
                g_cur_v_start -= cur_el->depth;
            } else {
                g_cur_v_start += cur_el->depth;
            }
            at_top = false;
            break;
        case el_text :
            if( cur_el->element.text.first == NULL ) {          // empty heading

                /* Use cur_spacing to adjust vertical position */

                if( bin_driver->y_positive == 0x00 ) {
                    g_cur_v_start -= cur_spacing;
                } else {
                    g_cur_v_start += cur_spacing;
                }
            } else {
                op_done = false;
                for( cur_line = cur_el->element.text.first; cur_line != NULL;
                                                    cur_line = cur_line->next ) {
                    if( (cur_line->first != NULL) &&
                            ((int32_t)(cur_line->first->x_address + h_start)) < 0 ) {
                        offset = -1 * (int32_t)cur_line->first->x_address + h_start;
                        cur_text = cur_line->first;
                        while( cur_text != NULL ) {         // rebase line to keep on physical page
                            cur_text->x_address += offset;
                            cur_text = cur_text->next;
                        }
                    }
                    cur_text = cur_line->first;
                    while( cur_text != NULL ) {
                        cur_text->x_address += h_start;
                        cur_text = cur_text->next;
                    }
                    if( use_spacing ) {
                        cur_spacing += cur_line->units_spacing + cur_line->line_height;
                    } else {
                        cur_spacing += cur_line->line_height;
                        use_spacing = true;         // use between lines
                    }
                    if( !at_top ) {                 // not first element
                        if( !op_done && cur_el->element.text.overprint ) {  // overprint
                            cur_spacing -= cur_line->line_height;
                            op_done = true;
                        }
                    } else {

                        /********************************************************/
                        /*  Possible future complexities:                       */
                        /*    it is not clear how forced overprint & minimum    */
                        /*      height interact                                 */
                        /*    it applies to the very first line on the top,     */
                        /*      even if that is a banner, and so does not       */
                        /*      automatically apply if Proc_flags.page_started  */
                        /*      is true, as that happens at the start of each   */
                        /*      section of the page to suppress any subs_skip   */
                        /********************************************************/

                        if( cur_el->element.text.overprint && cur_el->element.text.force_op ) {
                            if( use_spacing ) {
                                cur_spacing -= cur_line->units_spacing + cur_line->line_height;
                            } else {
                                cur_spacing -= cur_line->line_height;
                            }
                        } else if( (t_page.top_banner == NULL) &&
                                    (t_page.panes->page_width == NULL) ) {    // minimum height
                            if( cur_spacing < wgml_fonts[g_curr_font].line_height ) {
                                cur_spacing = wgml_fonts[g_curr_font].line_height;
                            }
                        }
                        if( cur_el->element.text.overprint ) {
                           op_done = true;          // done in the sense of "considered"
                        }
                        at_top = false;
                    }

                    /****************************************************/
                    /* Apply cur_spacing to the vertical position       */
                    /* Then fix an overprint line at the top of the     */
                    /* page, at least for character devices like TASA.  */
                    /* g_cur_v_start cannot be above the y_start which  */
                    /* the device specified for the overprint line, but */
                    /* must be restored for the remaining lines.        */
                    /****************************************************/

                    if( bin_driver->y_positive == 0x00 ) {
                        g_cur_v_start -= cur_spacing;
                        old_v_start = g_cur_v_start;
                        if( g_cur_v_start > bin_device->y_start ) {
                            g_cur_v_start = bin_device->y_start;
                        }
                    } else {
                        g_cur_v_start += cur_spacing;
                        old_v_start = g_cur_v_start;
                        if( g_cur_v_start < bin_device->y_start ) {
                            g_cur_v_start = bin_device->y_start;
                        }
                    }

                    cur_line->y_address = g_cur_v_start;
                    cur_spacing = 0;
                    g_cur_v_start = old_v_start;
                }
                at_top = false;
            }
            break;
        case el_vline :
            cur_el->element.vline.h_start += h_start;
            cur_el->h_pos = cur_el->element.vline.h_start;
            if( bin_driver->y_positive == 0x00 ) {
                g_cur_v_start -= cur_spacing;
            } else {
                g_cur_v_start += cur_spacing;
            }
            cur_el->element.vline.v_start = g_cur_v_start;
            cur_el->v_pos = g_cur_v_start;
            if( bin_driver->y_positive == 0x00 ) {
                g_cur_v_start -= cur_el->depth;
            } else {
                g_cur_v_start += cur_el->depth;
            }
            at_top = false;
            break;
        case el_vspace :
            cur_el->h_pos = h_start;
            if( bin_driver->y_positive == 0x00 ) {
                g_cur_v_start -= cur_spacing;
            } else {
                g_cur_v_start += cur_spacing;
            }
            cur_el->v_pos = g_cur_v_start;
            if( bin_driver->y_positive == 0x00 ) {
                g_cur_v_start -= cur_el->depth;
            } else {
                g_cur_v_start += cur_el->depth;
            }
            at_top = false;
            break;
        default :
            internal_err( __FILE__, __LINE__ );
        }
    }

    return( at_top );
}


/***************************************************************************/
/*  output the doc_pane(s)                                                 */
/*                                                                         */
/*  Convert the linked list of panes into an array of doc_elements, such   */
/*  that each column contains all the doc_elements in the corresponding    */
/*  column of each pane, and the first column contains the doc_element,    */
/*  if any, in each pane's page_width field as if it were, in fact, in the */
/*  first column of that pane                                              */
/***************************************************************************/

static void do_doc_panes_out( void )
{
    doc_element *       out_el[MAX_COL];
    doc_element *       cur_el[MAX_COL];
    doc_pane    *       cur_pane;
    int                 i;
    uint32_t            col_count;

    for( i = 0; i < MAX_COL; i++ ) {
        cur_el[i] = NULL;
        out_el[i] = NULL;
    }

    col_count = 0;
    cur_pane = t_page.panes;
    while( cur_pane != NULL ) {
        if( col_count < cur_pane->col_count ) { // maximum number of colums used
            col_count = cur_pane->col_count;
        }
        if( cur_pane->page_width != NULL ) {
            set_positions( cur_pane->page_width, t_page.page_left,
                           cur_pane->page_width_top );
            out_el[0] = cur_pane->page_width;
            cur_el[0] = cur_pane->page_width;
            while( cur_el[0]->next != NULL) cur_el[0] = cur_el[0]->next;
            cur_pane->page_width = NULL;
        }
        for( i = 0; i < cur_pane->col_count; i++ ) {
            ProcFlags.page_started = false;
            if( cur_pane->cols[i].col_width != NULL ) {
                ProcFlags.page_started = !set_positions( cur_pane->cols[i].col_width,
                               cur_pane->cols[i].col_left, cur_pane->col_width_top );
                if( out_el[i] == NULL ) {
                    out_el[i] = cur_pane->cols[i].col_width;
                }
                if( cur_el[i] == NULL ) {
                    cur_el[i] = cur_pane->cols[i].col_width;
                } else {
                    cur_el[i]->next = cur_pane->cols[i].col_width;
                }
                while( cur_el[i]->next != NULL ) cur_el[i] = cur_el[i]->next;
                cur_pane->cols[i].col_width = NULL;
            }
            if( cur_pane->cols[i].main != NULL ) {
                set_positions( cur_pane->cols[i].main, cur_pane->cols[i].col_left,
                               cur_pane->cols[i].main_top );
                if( out_el[i] == NULL ) {
                    out_el[i] = cur_pane->cols[i].main;
                }
                if( cur_el[i] == NULL ) {
                    cur_el[i] = cur_pane->cols[i].main;
                } else {
                    cur_el[i]->next = cur_pane->cols[i].main;
                }
                while( cur_el[i]->next != NULL ) cur_el[i] = cur_el[i]->next;
                cur_pane->cols[i].main = NULL;
            }
            ProcFlags.page_started = true;
            if( cur_pane->cols[i].bot_fig != NULL ) {
                set_positions( cur_pane->cols[i].bot_fig, cur_pane->cols[i].col_left,
                               cur_pane->cols[i].fig_top );
                if( out_el[i] == NULL ) {
                    out_el[i] = cur_pane->cols[i].bot_fig;
                }
                if( cur_el[i] == NULL ) {
                    cur_el[i] = cur_pane->cols[i].bot_fig;
                } else {
                    cur_el[i]->next = cur_pane->cols[i].bot_fig;
                }
                while( cur_el[i]->next != NULL ) cur_el[i] = cur_el[i]->next;
                cur_pane->cols[i].bot_fig = NULL;
            }
            if( cur_pane->cols[i].footnote != NULL ) {
                set_positions( cur_pane->cols[i].footnote, cur_pane->cols[i].col_left,
                               cur_pane->cols[i].fn_top );
                if( out_el[i] == NULL ) {
                    out_el[i] = cur_pane->cols[i].footnote;
                }
                if( cur_el[i] == NULL ) {
                    cur_el[i] = cur_pane->cols[i].footnote;
                } else {
                    cur_el[i]->next = cur_pane->cols[i].footnote;
                }
                while( cur_el[i]->next != NULL ) cur_el[i] = cur_el[i]->next;
                cur_pane->cols[i].footnote = NULL;
            }
        }
        cur_pane = cur_pane->next;
    }

    if( col_count == 1 ) {
        do_el_list_out( out_el[0] );   // no need to consolidate one column
    } else {
        consolidate_array( out_el, col_count );
    }

    return;
}


/***************************************************************************/
/*  fill the current column                                                */
/*  the first doc_element returned will fill the current column            */
/*  the second doc_element will contain the rest of the content            */
/*                                                                         */
/*  NOTE: this should only be called if split_element reports that the     */
/*        element is not splittable and it will not fit in any column      */
/*  NOTE: only text elements are actually split here; boxes and figures    */
/*        that are split are handled elsewhere and should always fit       */
/***************************************************************************/

static void fill_column( doc_element * a_element )
{
    doc_element *   split_el;
    text_line   *   cur_line;
    text_line   *   last        =   NULL;
    uint32_t        cur_depth   =   0;

    switch( a_element->type ) {
    // add/move code for other element types as appropriate/desired
    case el_binc :
    case el_dbox :
    case el_graph :
    case el_hline :
    case el_vline :
    case el_vspace :
        break;
    case el_text :

        cur_line = a_element->element.text.first;

        /* Error if first line will not fit on any page */

        if( ( cur_line->line_height + cur_line->units_spacing ) > old_max_depth ) {
            xx_err( err_text_line_too_deep );
            break;
        }

        /****************************************************************/
        /* determine how many lines will fit in the remaining space     */
        /****************************************************************/

        while( cur_line != NULL ) {
            if( (cur_line->line_height + cur_line->units_spacing) >
                    (t_page.max_depth - t_page.cur_depth - cur_depth) ) {
                break;
            }
            cur_depth += (cur_line->line_height + cur_line->units_spacing);
            last = cur_line;
            cur_line = cur_line->next;
        }

        if( (cur_line == NULL) || (last == NULL) ) {        // just in case
            break;
        }

        /************************************************************/
        /*  if we get here, a_element is splittable, cur_line is    */
        /*  the first line of the new element, and last is the last */
        /*  line that can be left in the original element           */
        /************************************************************/

        split_el = alloc_doc_el( el_text ); // most defaults are correct

        split_el->depth = a_element->depth - cur_depth;
        split_el->element.text.first = cur_line;
        last->next = NULL;                  // this restricts cur_el to the lines allowed
        a_element->depth = cur_depth;
        if( a_element->next == NULL ) {
            a_element->next = split_el;
        } else {
            split_el->next = a_element->next;
            a_element->next = split_el;
        }
        break;
    default :
        internal_err( __FILE__, __LINE__ );
    }
    return;
}


/***************************************************************************/
/*  update a single column from the elements in n_page                     */
/*  the various n_page fields may or may not be empty on return            */
/*  NOTE: the order used is:                                               */
/*    col_width                                                            */
/*    col_bot                                                              */
/*    footnote                                                             */
/*    main                                                                 */
/*  and this appears to match what wgml 4.0 does                           */
/***************************************************************************/

static void update_column( void )
{
    bool                splittable;
    doc_element     *   cur_el;
    doc_el_group    *   cur_group;
    uint32_t            depth;

    t_page.cur_col = &t_page.last_pane->cols[t_page.last_pane->cur_col];
    t_page.cur_col->main_top = t_page.last_pane->col_width_top;
    t_page.cur_col->fig_top = t_page.bot_ban_top;
    t_page.cur_col->fn_top = t_page.bot_ban_top;
    if( bin_driver->y_positive == 0x00 ) {
        t_page.max_depth = t_page.cur_col->main_top - t_page.cur_col->fig_top;
    } else {
        t_page.max_depth = t_page.cur_col->fig_top - t_page.cur_col->main_top;
    }
    ProcFlags.col_started = false;

    /***********************************************************************/
    /*  The first block in n_page.col_width is processed                   */
    /*  Remaining blocks will be processed at the rate of one per page     */
    /*  until none are left in n_page.col_width                            */
    /*  Note: t_page.cols and so t_page.cols->col_width will always be     */
    /*        NULL at this point                                           */
    /***********************************************************************/

    if( n_page.col_width != NULL ) {                // at most one item can be placed
        cur_group = n_page.col_width;
        depth = cur_group->depth + cur_group->post_skip;
        t_page.cur_col->col_width = cur_group->first;
        if( bin_driver->y_positive == 0 ) {
            t_page.cur_col->main_top -= depth;
        } else {
            t_page.cur_col->main_top += depth;
        }
        t_page.max_depth -= depth;
        n_page.col_width = n_page.col_width->next;
        cur_group->next = NULL;
        cur_group->first = NULL;
        add_doc_el_group_to_pool( cur_group );
    }

    /***********************************************************************/
    /*  The first block in n_page.col_bot is processed                     */
    /*  Remaining blocks will be processed at the rate of one per page     */
    /*  until none are left in n_page.col_bot                              */
    /*  Note: t_page.cols will only be NULL at this point if no top column */
    /*        width FIG has been placed on the page                        */
    /*        t_page.cols->bot_fig will always be NULL at this point       */
    /***********************************************************************/

    if( n_page.col_bot != NULL ) {              // at most one item can be placed
        cur_group = n_page.col_bot;
        t_page.cur_col->bot_fig = cur_group->first;
        if( bin_driver->y_positive == 0) {
            t_page.cur_col->fig_top += cur_group->depth;
        } else {
            t_page.cur_col->fig_top -= cur_group->depth;
        }
        t_page.max_depth -= cur_group->depth;
        n_page.col_bot = n_page.col_bot->next;
        cur_group->next = NULL;
        cur_group->first = NULL;
        add_doc_el_group_to_pool( cur_group );
    }

    /***********************************************************************/
    /*  As many blocks from n_page.col_fn as will fit are processed        */
    /*  Until a block that fits is found, each block is dropped and a      */
    /*    warning is issued. Once such a block is found, all blocks are    */
    /*    processed until one that will not fit is found                   */
    /*  That block and all remaining blocks will be processed on future    */
    /*    until none are left in n_page.col_fn                             */
    /*  Note: t_page.cols will only be NULL at this point if no top column */
    /*        width or bottom FIG has been placed on the page              */
    /***********************************************************************/

    if( n_page.col_fn != NULL ) {
        while( n_page.col_fn != NULL ) {
            cur_group = n_page.col_fn;
            if( t_page.cur_depth + cur_group->depth > t_page.max_depth ) {
                while( (cur_group->first != NULL) &&
                        (t_page.cur_depth + cur_group->first->depth <= t_page.max_depth) ) {
                    cur_el = cur_group->first;
                    if( t_page.cur_col->footnote == NULL ) {
                        t_page.cur_col->footnote = cur_el;
                    } else {
                        t_page.last_col_fn->next = cur_el;
                    }
                    t_page.last_col_fn = cur_el;
                    cur_group->first = cur_group->first->next;
                    cur_el->next = NULL;
                    if( bin_driver->y_positive == 0) {
                        t_page.cur_col->fig_top += cur_el->depth;
                        t_page.cur_col->fn_top += cur_el->depth;
                    } else {
                        t_page.cur_col->fig_top -= cur_el->depth;
                        t_page.cur_col->fn_top -= cur_el->depth;
                    }
                    t_page.max_depth -= cur_el->depth;
                    cur_group->depth -= cur_el->depth;
                }
            } else {
                cur_el = cur_group->first;          // here, cur_el is the last element
                while( cur_el->next != NULL ) {
                    cur_el = cur_el->next;
                }
                if( t_page.cur_col->footnote == NULL ) {
                    t_page.cur_col->footnote = cur_group->first;
                } else {
                    t_page.last_col_fn->next = cur_group->first;
                }
                t_page.last_col_fn = cur_el;
                if( bin_driver->y_positive == 0) {
                    t_page.cur_col->fig_top += cur_group->depth;
                    t_page.cur_col->fn_top += cur_group->depth;
                } else {
                    t_page.cur_col->fig_top -= cur_group->depth;
                    t_page.cur_col->fn_top -= cur_group->depth;
                }
                t_page.max_depth -= cur_group->depth;
            }
            n_page.col_fn = n_page.col_fn->next;
            cur_group->next = NULL;
            cur_group->first = NULL;
            add_doc_el_group_to_pool( cur_group );
        }
    }

    /***********************************************************************/
    /*  As many blocks from n_page.fk_queue as will fit are processed      */
    /*  Note: t_page.cols will only be NULL at this point if no top column */
    /*        width or bottom FIG has been placed on the page              */
    /***********************************************************************/

    if( n_page.fk_queue != NULL ) {
        cur_group = n_page.fk_queue;
        while( cur_group != NULL ) {
#if 0
            /* kept in case actual criteria ever discovered */
            if( (t_page.cur_depth != 0) &&
                    ((t_page.cur_depth + cur_group->depth) > t_page.max_depth) &&
                    ((cur_group->depth <= t_page.max_depth)) ) {
                /* Put block in new column */
                break;
            }
#endif
            cur_el = cur_group->first;
            while( cur_el != NULL ) {
                if( cur_el->blank_lines > 0 ) {
                    if( (t_page.cur_depth + cur_el->blank_lines) >= t_page.max_depth ) {
                        cur_el->blank_lines -= (t_page.max_depth - t_page.cur_depth);
                        /* Put block in new column */
                        break;
                    } else if( !ProcFlags.col_started && ((t_page.cur_depth +
                            cur_el->blank_lines + cur_el->top_skip) >=
                            t_page.max_depth) ) {
                        cur_el->top_skip -= (t_page.max_depth - t_page.cur_depth);
                        cur_el->top_skip += cur_el->blank_lines;
                        cur_el->blank_lines = 0;
                        /* Put block in new column */
                        break;
                    } else if( (t_page.cur_depth + cur_el->blank_lines +
                            cur_el->subs_skip) >= t_page.max_depth ) {
                        cur_el->blank_lines = 0;
                    /* Put block in new column */
                        break;
                    }
                }
                if( !ProcFlags.col_started ) {
                    if( cur_el->blank_lines > 0 ) {
                        depth = cur_el->blank_lines + cur_el->subs_skip;
                    } else {
                        depth = cur_el->top_skip;
                        cur_group->depth -= cur_el->subs_skip;
                    }
                    ProcFlags.col_started = true;
                } else {
                    depth = cur_el->blank_lines + cur_el->subs_skip;
                }
                if( (t_page.cur_depth + depth) >= t_page.max_depth ) {  // skip fills page
                    /* Put block in new column */
                    break;
                }

                /****************************************************************/
                /*  Does the first line minimum apply here? If so, it needs to  */
                /*  be implemented. Note that cur_el->depth does not reflect it */
                /*  because there is no way to tell if it will apply when the   */
                /*  cur_el->depth is computed.                                  */
                /****************************************************************/

                if( (t_page.cur_depth + cur_el->depth + depth) > t_page.max_depth ) {    // split element
                    splittable = split_element( cur_el, t_page.max_depth -
                                                                    t_page.cur_depth - depth );
                    if( splittable ) {
                        if( t_page.cur_col->main == NULL ) {
                            t_page.cur_col->main = cur_el;
                        } else {
                            t_page.last_col_main->next = cur_el;
                        }
                        t_page.last_col_main = cur_el;
                        cur_group->first = cur_el->next;
                        t_page.last_col_main->next = NULL;
                        t_page.cur_depth += cur_el->depth + depth;
                        cur_group->depth -= cur_el->depth + depth;
                        ProcFlags.col_started = true;
                    }
                    break;                              // column is now full
                } else {                                // fits as-is
                    if( t_page.cur_col->main == NULL ) {
                        t_page.cur_col->main = cur_el;
                    } else {
                        t_page.last_col_main->next = cur_el;
                    }
                    t_page.last_col_main = cur_el;
                    cur_group->first = cur_el->next;
                    t_page.last_col_main->next = NULL;
                    t_page.cur_depth += cur_el->depth + depth;
                    cur_group->depth -= cur_el->depth + depth;
                    ProcFlags.col_started = true;
                }
                cur_el = cur_group->first;
            }

            /**************************************************************************/
            /* cur_group is still n_page.fk_queue                                     */
            /* if it is empty, then discard it and update n_page.fk_queue             */
            /* if it is not empty, the n_page.fk_queue contains the rest of the group */
            /**************************************************************************/

            if( cur_group->depth == 0 ) {       // this is n_page.fk_queue
                n_page.fk_queue = n_page.fk_queue->next;
                if( n_page.fk_queue == NULL ) {
                    n_page.last_fk_queue = NULL;
                }
                cur_group->next = NULL;
                cur_group->first = NULL;
                add_doc_el_group_to_pool( cur_group );
                cur_group = n_page.fk_queue;
                ProcFlags.col_started = true;
            } else {
                break;
            }
        }
    }

    /* This appears to replicate the behavior of wgml 4.0 */

    if( n_page.fk_queue == NULL ) {

        /***********************************************************************/
        /*  As many blocks from n_page.col_main as will fit are processed      */
        /*  A page is not full unless there is at least one element left in    */
        /*    at least one of the sections of n_page                           */
        /*  The original value of max_depth is used to prevent looping if the  */
        /*    current element is too large to fit on any page                  */
        /*  Note: a page which is not full will only be emitted if it is the   */
        /*    last page in a section (or the document)                         */
        /*  Note: t_page.cols will only be NULL at this point if neither       */
        /*    footnotes nor a buttom FIG has been placed on the page           */
        /***********************************************************************/

        while( n_page.col_main != NULL ) {
            cur_el = n_page.col_main;

            /****************************************************************/
            /*  this section identifies skips and blank lines that finish   */
            /*  the current page and then exits the loop after adjusting    */
            /*  the element field values as needed                          */
            /****************************************************************/

            if( cur_el->blank_lines > 0 ) {
                if( (t_page.cur_depth + cur_el->blank_lines) >= t_page.max_depth ) {
                    cur_el->blank_lines -= (t_page.max_depth - t_page.cur_depth);
                    break;
                } else if( !ProcFlags.col_started && ((t_page.cur_depth +
                            cur_el->blank_lines + cur_el->top_skip) >=
                            t_page.max_depth) ) {
                    cur_el->top_skip -= (t_page.max_depth - t_page.cur_depth);
                    cur_el->top_skip += cur_el->blank_lines;
                    cur_el->blank_lines = 0;
                    break;
                } else if( (t_page.cur_depth + cur_el->blank_lines +
                            cur_el->subs_skip) >= t_page.max_depth ) {
                    cur_el->blank_lines = 0;
                    break;
                }
            }
            if( !ProcFlags.col_started ) {
                if( cur_el->blank_lines > 0 ) {
                    depth = cur_el->blank_lines + cur_el->subs_skip;
                } else {
                    depth = cur_el->top_skip;
                }
                ProcFlags.col_started = true;
            } else {
                depth = cur_el->blank_lines + cur_el->subs_skip;
            }

            if( (t_page.cur_depth + depth) >= t_page.max_depth ) {  // skip fills page
                break;
            }

            /****************************************************************/
            /*  Does the first line minimum apply here? If so, it needs to  */
            /*  be implemented. Note that cur_el->depth does not reflect it */
            /*  because there is no way to tell if it will apply when the   */
            /*  cur_el->depth is computed.                                  */
            /****************************************************************/

            if( (t_page.cur_depth + depth + cur_el->depth) > t_page.max_depth ) {    // cur_el will fill the column
                splittable = split_element( cur_el, t_page.max_depth -
                                                    t_page.cur_depth - depth );
                if( splittable ) {              // cur_el was split
                    n_page.col_main = cur_el->next;
                    n_page.last_col_main = n_page.col_main;
                    while( n_page.last_col_main->next != NULL ) {
                        n_page.last_col_main = n_page.last_col_main->next;
                    }
                    cur_el->next = NULL;
                    if( t_page.cur_col->main == NULL ) {
                        t_page.cur_col->main = cur_el;
                    } else {
                        t_page.last_col_main->next = cur_el;
                    }
                    t_page.last_col_main = cur_el;
                    t_page.last_col_main->next = NULL;
                    t_page.cur_depth += cur_el->depth + depth;
                } else {
                    fill_column( cur_el );
                    if( cur_el->next != NULL ) {        // cur_el was split
                        n_page.col_main = cur_el->next;
                        if( n_page.last_col_main == NULL ) {
                            n_page.last_col_main = n_page.col_main;
                        }
                        cur_el->next = NULL;
                        if( t_page.cur_col->main == NULL ) {
                            t_page.cur_col->main = cur_el;
                        } else {
                            t_page.last_col_main->next = cur_el;
                        }
                        t_page.last_col_main = cur_el;
                        t_page.last_col_main->next = NULL;
                        t_page.cur_depth += cur_el->depth + depth;
                    }
                }
                break;
            } else {                                    // cur_el fits as-is
                if( t_page.cur_col->main == NULL ) {
                    t_page.cur_col->main = cur_el;
                    t_page.last_col_main = t_page.cur_col->main;
                } else {
                    t_page.last_col_main->next = cur_el;
                    t_page.last_col_main = t_page.last_col_main->next;
                }
                n_page.col_main = n_page.col_main->next;
                if( n_page.col_main == NULL ) {
                    n_page.last_col_main = NULL;
                }
                t_page.last_col_main->next = NULL;
                if( (cur_el->type == el_text) && cur_el->element.text.overprint
                                              && cur_el->element.text.force_op ) {
                    /* do nothing, adjusts for top-of-page overprint */
                } else {
                    t_page.cur_depth += cur_el->depth + depth;
                }
            }
        }
    }
    return;
}

/***************************************************************************/
/*  update t_page from the elements in n_page                              */
/*  the various n_page fields may or may not be empty on return            */
/*  NOTE: the order used is:                                               */
/*    page_width                                                           */
/*    then in each column:                                                 */
/*      col_width                                                          */
/*      col_bot                                                            */
/*      footnote                                                           */
/*      main                                                               */
/*    and this appears to match what wgml 4.0 does                         */
/***************************************************************************/

static void update_t_page( void )
{
    doc_el_group    *   cur_group;
    uint32_t            depth;

    reset_t_page();

    /***********************************************************************/
    /*  The first block in n_page.page_width is processed                  */
    /*  Remaining blocks will be processed at the rate of one per page     */
    /*  until none are left in n_page.page_width                           */
    /*  Note: t_page.page_width will always be NULL at this point          */
    /***********************************************************************/

    if( n_page.page_width != NULL ) {           // at most one item can be placed
        cur_group = n_page.page_width;
        depth = cur_group->depth + cur_group->post_skip;
        t_page.panes->page_width = cur_group->first;
        if( bin_driver->y_positive == 0 ) {
            t_page.panes_top -= depth;
            t_page.last_pane->col_width_top -= depth;
            t_page.cur_col->main_top -= depth;
        } else {
            t_page.panes_top += depth;
            t_page.last_pane->col_width_top -= depth;
            t_page.cur_col->main_top += depth;
        }
        t_page.max_depth -= depth;
        n_page.page_width = n_page.page_width->next;
        cur_group->next = NULL;
        cur_group->first = NULL;
        add_doc_el_group_to_pool( cur_group );
    }

    update_column();

    return;
}

/***************************************************************************/
/*  initialize the next column                                             */
/*  if the page is full, output it and initialize the new page             */
/***************************************************************************/

void next_column( void )
{
    /****************************************************************/
    /* Finish drawing any open box.                                 */
    /* Note: this may need to be done for other control words and   */
    /* tags that accumulate doc_elements                            */
    /****************************************************************/

    if( ProcFlags.in_bx_box ) {
        eoc_bx_box();
    }

    t_page.last_pane->cur_col++;
    if( t_page.last_pane->cur_col < t_page.last_pane->col_count ) {
        t_page.cur_depth = 0;
        update_column();
    } else {
        do_page_out();
        update_t_page();
    }
    return;
}

/***************************************************************************/
/*  actually output t_page to the device                                   */
/***************************************************************************/

void do_page_out( void )
{
    doc_column  *   first_col;
    doc_element *   work_el;
    font_number     save_prev;
    uint32_t        curr_height;
    uint32_t        hl_depth;
    uint32_t        op_hdg_cnt  = 0;
    uint32_t        prev_height;
    uint32_t        sav_hs;
    uint32_t        v_offset;

    static uint32_t nh_pages    = 0;

    /* Set up for the new page */

    if( g_apage && GlobalFlags.lastpass ) {   // don't do before first page
       fb_document_page();                  // NEWPAGE is interpreted here
    }
    g_apage++;
    g_page++;

    /****************************************************************/
    /*  test section to see if a kludge can be found for the        */
    /*  "reduce TOC page number" problem                            */
    /****************************************************************/

    if( ProcFlags.op_done ) {
        work_el = t_page.panes->cols[0].main;
        while( work_el != NULL ) {
            if( (work_el->type == el_text) && (work_el->element.text.entry != NULL) ) {
                op_hdg_cnt++;
            }
            work_el = work_el->next;
        }
        work_el = t_page.panes->cols[0].main;
        if( op_hdg_cnt > 0 ) {
            while( work_el != NULL ) {
                if( work_el->type != el_text ) {
                    ProcFlags.op_done = false;  // cancel for non-text doc_el: TBD
                    break;
                }
                if( (work_el->type == el_text) && (work_el->element.text.entry != NULL) ) {
                    if( nh_pages < 2 ) {
                        ProcFlags.op_done = false;  // cancel for heading too far down on page: TBD
                    }
                    break;                          // keep ProcFlags.op_done set
                }
                work_el = work_el->next;
            }
        } else {
            nh_pages++;
        }
    }

    /* Process any page-specific index items */

    if( t_page.eol_index != NULL ) {
        eol_index_page( t_page.eol_index, g_page );
    }

    /* Get the banner text into the proper sections */

    if( ProcFlags.keep_left_margin ) {
        sav_hs = t_page.cur_width;
    }
    t_page.cur_width = 0;

    if( (t_page.top_banner != NULL) && (t_page.top_banner->by_line != NULL) ) {
        save_prev = g_prev_font;
        out_ban_top();                      // resets g_prev_font for top banner

        /********************************************************************/
        /* Record g_prev_font for a GRAPHIC element                         */
        /* Adjust height of first element                                   */
        /* if hline with ban_adjust == true, use old code section           */
        /* if hline with ban_adjust == false, use new code section          */
        /* Note: since this applies only to hline, page_width must be empty */
        /*       and the hline must be the first or second element in main, */
        /*       specifically in main.main                                  */
        /* The first text element on the page is also adjusted              */
        /* Note: in the OW docs, this will always be just after the top     */
        /*       box line                                                   */
        /********************************************************************/

        first_col = &t_page.panes->cols[0];
        if( (t_page.panes->page_width == NULL) && (first_col->main != NULL) ) {
            if( first_col->main->type == el_graph ) {               // first element on page
                /* Left expanded in case future work is needed */
                if( first_col->main->next->type == el_text ) {      // text element follows
                    if( first_col->main->next->element.text.first != NULL ) {
                        if( first_col->main->next->element.text.first->first != NULL ) {
                            first_col->main->element.graph.next_font =
                                        first_col->main->next->element.text.first->first->font;
                        }
                    }
                }
            }
            work_el = NULL;
            if( (first_col->main->type == el_hline) ) {
                work_el = first_col->main;
            } else if( (first_col->main->type == el_text) &&
                    (first_col->main->next != NULL) &&
                    (first_col->main->element.text.first != NULL) &&
                    (first_col->main->element.text.first->first == NULL) &&
                    (first_col->main->next->type == el_hline) ) {
                work_el = first_col->main->next;
            }

            if( (work_el != NULL) && work_el->element.hline.ban_adjust ) {  // top line is hline

                /********************************************************************/
                /* This is the "old code section" referred to above                 */
                /* It appears to be linked with the BX code that emits HLINEs       */
                /* Commenting it out produced very strange diffs in cguide/cguideq  */
                /********************************************************************/

                curr_height = wgml_fonts[g_curr_font].line_height;
                prev_height = wgml_fonts[g_prev_font].line_height;
                if( prev_height < curr_height ) {
                    work_el->subs_skip += (curr_height - prev_height) / 2;
                    work_el->top_skip += (curr_height - prev_height) / 2;
                } else if( prev_height > curr_height ) {
                    work_el->subs_skip -= (prev_height - curr_height) / 2;
                    work_el->top_skip -= (prev_height - curr_height) / 2;
                }
                while( (work_el != NULL) && (work_el->type != el_text) ) {
                    work_el = work_el->next;
                }
                if( work_el != NULL ) {             // element is text
                    if( prev_height < curr_height ) {
                        work_el->subs_skip -= (curr_height - prev_height) / 2;
                    } else if( prev_height > curr_height ) {
                        work_el->subs_skip += (prev_height - curr_height) / 2;
                    }
                }
                work_el = first_col->main;
                while( (work_el != NULL) && (work_el->type != el_vline) ) {
                    work_el = work_el->next;
                }
                while( (work_el != NULL) && (work_el->type == el_vline)  ) {
                    if( prev_height < curr_height ) {
                        work_el->element.vline.v_len -= (curr_height - prev_height) / 2;
                    } else if( prev_height > curr_height ) {
                        work_el->element.vline.v_len += (prev_height - curr_height) / 2;
                    }
                    work_el = work_el->next;
                }
            } else if( work_el != NULL ) {  // hline at top without ban_adjust set

                /********************************************************************/
                /* This is the "new code section" referred to above                 */
                /* It was needed because an FK block, when added to the page, is    */
                /* fully capable of putting a box at the top of a page which the    */
                /* BX code that emits HLINEs could not predict would end up thers   */
                /********************************************************************/

                /* Restore HLINE to a neutral posture */

                work_el->depth = 0;
                work_el->subs_skip = work_el->element.hline.o_subs_skip;
                work_el->top_skip = work_el->element.hline.o_top_skip;

                /* Compute base values of v_offset and hl_depth */

                curr_height = wgml_fonts[g_curr_font].line_height;
                v_offset = curr_height / 2;
                if( (curr_height % 2) > 0 ) {
                    v_offset++;
                }
                hl_depth = curr_height - v_offset;

                /* Adjust hline for (in this case) the banner text font */

                prev_height = wgml_fonts[g_prev_font].line_height;

                if( prev_height < curr_height ) {
                    v_offset += (curr_height - prev_height) / 2;
                    hl_depth -= (curr_height - prev_height) / 2;
                } else if( prev_height > curr_height ) {
                    v_offset -= (prev_height - curr_height) / 2;
                    hl_depth += (prev_height - curr_height) / 2;
                }
                work_el->depth = hl_depth;
                work_el->subs_skip += v_offset;
                work_el->top_skip += v_offset;

                /* Find and adjust the vlines */

                while( (work_el != NULL) && (work_el->type != el_vline) ) {
                    work_el = work_el->next;
                }
                while( (work_el != NULL) && (work_el->type == el_vline)  ) {
                    if( prev_height < curr_height ) {
                        work_el->element.vline.v_len -= (curr_height - prev_height) / 2;
                    } else if( prev_height > curr_height ) {
                        work_el->element.vline.v_len += (prev_height - curr_height) / 2;
                    }
                    work_el = work_el->next;
                }
            }
        }
        g_prev_font = save_prev;            // restore old g_prev_font value
    }

    if( ProcFlags.keep_left_margin ) {
        t_page.cur_width = sav_hs;
    }

    if( (t_page.bottom_banner != NULL) && (t_page.bottom_banner->by_line != NULL) ) {
        out_ban_bot();
    }

    if( n_page.fk_queue != NULL ){
        n_page.prev_pg_depth = t_page.cur_depth;
    }

    /* Output the page section by section */

    ProcFlags.page_started = false;
    if( t_page.top_ban != NULL ) {
        do_el_list_out( t_page.top_ban );
        t_page.top_ban = NULL;
    }

    if( t_page.panes != NULL ) {
        ProcFlags.page_started = false;
        do_doc_panes_out();
        ProcFlags.page_started = true;
    }

    if( t_page.bot_ban != NULL ) {
        do_el_list_out( t_page.bot_ban );
        t_page.bot_ban = NULL;
    }

    /************************************************************/
    /* If this page used banners associated with an Hn heading, */
    /* reset the banners for the next page                      */
    /************************************************************/

    if( ProcFlags.heading_banner ) {
        set_section_banners( ProcFlags.doc_sect );
        ProcFlags.heading_banner = false;
    }

    ProcFlags.page_started = false;             // reset after output done
    ProcFlags.tophead_done = false;             // reset after output done
    return;
}


/***************************************************************************/
/*  output all full pages                                                  */
/*  t_page will not be empty but n_page will be empty on return            */
/*  Note: may need to be expanded to consider doc_elements accumulated by  */
/*  the various tags and control words that accumulate doc_elements        */
/***************************************************************************/

void full_col_out( void )
{
    while( (n_page.fk_queue != NULL) || (n_page.page_width != NULL) || (n_page.col_width != NULL)
            || (n_page.col_main != NULL) || (n_page.col_bot != NULL)
            || (n_page.col_fn != NULL) ) {
        next_column();
    }
    return;
}


/****************************************************************************/
/*  insert a doc_element_group into t_page.cur_col->bot_fig                 */
/****************************************************************************/

void insert_col_bot( doc_el_group * a_group )
{
    uint32_t    depth;

    /****************************************************************/
    /*  if t_page.cols->bot_fig is empty and if it fits, place      */
    /*  cur_doc_el_group in t_page.cols->bot_fig                    */
    /*  otherwise, append cur_doc_el_groupto n_page.last_col_bot    */
    /*  NOTE: FIG/eFIG blocks must fit on an empty page or, rather, */
    /*        must be split so as to fit before they are submitted  */
    /****************************************************************/

    depth = a_group->depth;

    if( (t_page.cur_col->bot_fig == NULL) &&
        ((t_page.cur_depth + depth) <= t_page.max_depth) ) {    // the figure will fit

        t_page.cur_col->bot_fig = a_group->first;
        a_group->first = NULL;
        add_doc_el_group_to_pool( a_group );
        if( bin_driver->y_positive == 0) {
            t_page.cur_col->fig_top += depth;
        } else {
            t_page.cur_col->fig_top -= depth;
        }
        t_page.max_depth -= depth;
    } else {
        if( n_page.col_bot == NULL ) {
            n_page.col_bot = a_group;
        } else {
            n_page.last_col_bot->next = a_group;
        }
        n_page.last_col_bot = a_group;
    }

    return;
}


/***************************************************************************/
/*  insert a doc_element into t_page.cur-col->footnote                     */
/***************************************************************************/

void insert_col_fn( doc_el_group * a_group )
{
    doc_element *   cur_el;
    uint32_t        depth;

    /****************************************************************/
    /*  if it fits, put the footnote on the current page            */
    /*  if it does not fit, and it is the firt, split it            */
    /*  otherwise, put the footnote on n_page.last_col_fn           */
    /****************************************************************/

    depth = a_group->depth;
    if( (t_page.cur_depth + depth) > t_page.max_depth ) {
        while( (a_group->first != NULL) &&
                (t_page.cur_depth + a_group->first->depth <= (t_page.max_depth)) ) {
            cur_el = a_group->first;
            if( t_page.cur_col->footnote == NULL ) {
                t_page.cur_col->footnote = cur_el;
            } else {
                t_page.last_col_fn->next = cur_el;
            }
            t_page.last_col_fn = cur_el;
            a_group->first = a_group->first->next;
            cur_el->next = NULL;
            if( bin_driver->y_positive == 0) {
                t_page.cur_col->fig_top += cur_el->depth;
                t_page.cur_col->fn_top += cur_el->depth;
            } else {
                t_page.cur_col->fig_top -= cur_el->depth;
                t_page.cur_col->fn_top -= cur_el->depth;
            }
            t_page.max_depth -= cur_el->depth;
            a_group->depth -= cur_el->depth;
        }
        if( a_group->first != NULL ) {          // more footnote text remains
            if( n_page.col_fn == NULL ) {
                n_page.col_fn = a_group;
            } else {
                n_page.last_col_fn->next = a_group;
            }
            n_page.last_col_fn = a_group;
        }
    } else {
        cur_el = a_group->first;            // here, cur_el is the last element
        while( cur_el->next != NULL ) {
            cur_el = cur_el->next;
        }
        if( t_page.cur_col->footnote == NULL ) {
            t_page.cur_col->footnote = a_group->first;
        } else {
            t_page.last_col_fn->next = a_group->first;
        }
        t_page.last_col_fn = cur_el;
        a_group->first = NULL;
        add_doc_el_group_to_pool( a_group );
        if( bin_driver->y_positive == 0) {
            t_page.cur_col->fig_top += depth;
            t_page.cur_col->fn_top += depth;
        } else {
            t_page.cur_col->fig_top -= depth;
            t_page.cur_col->fn_top -= depth;
        }
        t_page.max_depth -= depth;
    }

    return;
}


/***************************************************************************/
/*  insert a doc_element into t_page.cur_col->main                         */
/***************************************************************************/

void insert_col_main( doc_element * a_element )
{
    bool                page_full;
    bool                splittable;
    uint32_t            cur_skip;
    uint32_t            depth;

    static  bool        last_co;
    static  bool        op_done     = false;
    static  uint32_t    op_hdg_cnt  = 0;

    /****************************************************************/
    /*  alternate procesing: accumulate elements for later          */
    /*  submission                                                  */
    /*  this is the basis for all block processing                  */
    /****************************************************************/

    if( t_doc_el_group != NULL ) {
        if( t_doc_el_group->first == NULL ) {
            t_doc_el_group->first = a_element;
            t_doc_el_group->last = t_doc_el_group->first;
            t_doc_el_group->depth = (a_element->blank_lines + a_element->subs_skip +
                                     a_element->depth);
            last_co = ProcFlags.concat;
        } else {
            if( (last_co != ProcFlags.concat) && (nest_cb->c_tag != t_FIG) ) { // FB/FK, at least, need this
                a_element->do_split = true;     // split block when closed
            }
            t_doc_el_group->last->next = a_element;
            t_doc_el_group->last = t_doc_el_group->last->next;
            t_doc_el_group->depth += (a_element->blank_lines + a_element->subs_skip +
                                      a_element->depth);
            if( a_element->type == el_text ) {  // subsequent text elements only
                a_element->depth += g_units_spacing;
                t_doc_el_group->depth += g_units_spacing;
            }
            last_co = ProcFlags.concat;
        }
        return;
    }

    /****************************************************************/
    /*  normal processing of doc_elements and page finalization is  */
    /*      done here, the rest is done in update_t_page()          */
    /****************************************************************/

    page_full = false;

    /****************************************************************/
    /*  this section sets page_full to "true" if any of the skips   */
    /*  or blank_lines finishes the page, alone or in various       */
    /*  combinations                                                */
    /*  element field values are adjusted as needed                 */
    /****************************************************************/

    if( a_element->blank_lines > 0 ) {
        if( (t_page.cur_depth + a_element->blank_lines) >= t_page.max_depth ) {
            a_element->blank_lines -= (t_page.max_depth - t_page.cur_depth);
            page_full = true;
        } else if( !ProcFlags.col_started && ((t_page.cur_depth +
                    a_element->blank_lines + a_element->top_skip) >=
                    t_page.max_depth) ) {
            a_element->top_skip -= (t_page.max_depth - t_page.cur_depth);
            a_element->top_skip += a_element->blank_lines;
            a_element->blank_lines = 0;
            page_full = true;
        } else if( (t_page.cur_depth + a_element->blank_lines +
                     a_element->subs_skip) >= t_page.max_depth ) {
            a_element->blank_lines = 0;
            page_full = true;
        }
    } else if( !ProcFlags.col_started ) {
        if( a_element->top_skip >= t_page.max_depth ) {
            a_element->top_skip -= t_page.max_depth;
            page_full = true;
        }
    }

    if( !page_full ) {

        /****************************************************************/
        /*  this test is done separately because an element may fail to */
        /*  set page_full to "true" above but the skip actually used    */
        /*  may still fill the page                                     */
        /****************************************************************/

        if( !ProcFlags.col_started ) {
            cur_skip = a_element->blank_lines + a_element->top_skip;
        } else {
            cur_skip = a_element->blank_lines + a_element->subs_skip;
        }

        if( (cur_skip + t_page.cur_depth) > t_page.max_depth ) {
            page_full = true;
        }
    }

    if( !page_full ) {

        /****************************************************************/
        /*  at least part of the element should fit on the page         */
        /*  does the first line minimum apply here? If so, it needs to  */
        /*  be implemented. Note that cur_el->depth does not reflect it */
        /*  because there is no way to tell if it will apply when the   */
        /*  is computed.                                                */
        /*  overprint lines pose a problem:                             */
        /*    anywhere but at the top of the page, they do not count as */
        /*      part of the page depth                                  */
        /*    they do, however, decrease t_page.max_depth by the height */
        /*      of the line when they occur when either CO is OFF or    */
        /*      an index tag or control word preceded the text          */
        /*      this affects how many lines can appear on the page      */
        /*      and may cause lines to move to the next page            */
        /*    if the page is full (t_page.cur_depth == t_page.max_depth)*/
        /*      they do count ... and start a new page                  */
        /****************************************************************/

        depth = cur_skip + a_element->depth;
        if( (a_element->type == el_text) && a_element->element.text.overprint
                && (t_page.cur_depth != t_page.max_depth) ) {
            depth -= a_element->element.text.first->line_height;
            if( !a_element->op_co_on || a_element->element.text.first->first->post_ix ) {
                t_page.max_depth -= a_element->element.text.first->line_height;
            }
            if( ProcFlags.in_reduced ) {
                a_element->element.text.overprint = false;  // line must appear in output
            }
            op_done = true; // will be true when current page processed; not clear if in_reduced should prevent this
            op_hdg_cnt = 0; // ignore headings before overprint line
        }

        if( ((a_element->type == el_text) && a_element->element.text.first != NULL)
                && (a_element->element.text.entry != NULL) ) {
            op_hdg_cnt++;   // heading found
        }

        if( ((a_element->type == el_text) && a_element->element.text.first == NULL) ) {

            /****************************************************************/
            /* Empty headings are added to the page but do not set          */
            /* ProcFlags.col_start                                          */
            /* set_positions adjusts the current v_position by the current  */
            /* spacing when it encounters them                              */
            /* Also, this allows for their orderly disposal and reuse       */
            /****************************************************************/

            if( t_page.cur_col->main == NULL ) {
                t_page.cur_col->main = a_element;
                t_page.last_col_main = t_page.cur_col->main;
            } else {
                t_page.last_col_main->next = a_element;
                t_page.last_col_main = t_page.last_col_main->next;
            }

        } else {
            if( ((a_element->type == el_text) && a_element->element.text.vspace_next) ) {

                /****************************************************************/
                /* Implements a wgml 4.0 bug in XMP/eXMP blocks:                */
                /*     when a text line is followed by a blank line             */
                /*     then it stays on the current page even if there is not   */
                /*     enough room                                              */
                /*     and then adjusts the vspace_element, which is now the    */
                /*     first in cur_doc_el_group, by the excess, if any         */
                /* This also affects CO OFF/CO ON blocks, but those do not      */
                /* affect the OW Docs and so are not "fixed"                    */
                /****************************************************************/

                if( t_page.cur_col->main == NULL ) {
                    t_page.cur_col->main = a_element;
                    t_page.last_col_main = t_page.cur_col->main;
                } else {
                    t_page.last_col_main->next = a_element;
                    t_page.last_col_main = t_page.last_col_main->next;
                }
                t_page.cur_depth += depth;
                a_element->element.text.vspace_next = false;
                if( t_page.cur_depth > t_page.max_depth ) {
                    cur_doc_el_group->first->depth -= (t_page.cur_depth - t_page.max_depth);
                }
            } else if( (depth + t_page.cur_depth) > t_page.max_depth ) {

                /****************************************************************/
                /* Normal processing: check to see if a_element will fit only   */
                /* if it can be split; includes special heading processing      */
                /****************************************************************/

                splittable = split_element( a_element, t_page.max_depth - t_page.cur_depth - cur_skip );
                if( a_element->next != NULL ) { // a_element was split
                    if( t_page.cur_col->main == NULL ) {
                        t_page.cur_col->main = a_element;
                        t_page.last_col_main = t_page.cur_col->main;
                    } else {
                        t_page.last_col_main->next = a_element;
                        t_page.last_col_main = t_page.last_col_main->next;
                    }
                    t_page.cur_depth += a_element->depth + cur_skip;
                    a_element = a_element->next;
                    t_page.last_col_main->next = NULL;
                } else if( (t_page.last_col_main != NULL) && (t_page.last_col_main->type == el_text) &&
                        (t_page.last_col_main->element.text.prev) != NULL ) {  // detach heading and reset a_element
                    t_page.last_col_main->next = a_element;
                    a_element = t_page.last_col_main;
                    t_page.last_col_main = t_page.last_col_main->element.text.prev;
                    t_page.last_col_main->next = NULL;
                }
                page_full = true;
            } else {        // the entire element fits on the current page
                if( t_page.cur_col->main == NULL ) {
                    t_page.cur_col->main = a_element;
                    t_page.last_col_main = t_page.cur_col->main;
                } else {
                    t_page.last_col_main->next = a_element;
                    t_page.last_col_main = t_page.last_col_main->next;
                }
                t_page.cur_depth += depth;
            }
            ProcFlags.col_started = true;
        }
    }

    if( page_full ) {

        /****************************************************************/
        /*  if page_full is true then a_element goes to n_page and      */
        /*  t_page must be output                                       */
        /****************************************************************/

        if( n_page.col_main == NULL ) {
            n_page.col_main = a_element;
            n_page.last_col_main = n_page.col_main;
        } else {
            n_page.last_col_main->next = a_element;
            n_page.last_col_main = n_page.last_col_main->next;
        }
        while( n_page.last_col_main->next != NULL ) {
            n_page.last_col_main = n_page.last_col_main->next;
        }

        text_col_out();
        if( !ProcFlags.op_done && (op_hdg_cnt == 1) ) {
            ProcFlags.op_done = op_done;        // once set, set until used
            op_done = false;
            op_hdg_cnt = 0;
        }
    }

    return;
}


/***************************************************************************/
/*  insert a doc_element into t_page.cols->col_width                       */
/***************************************************************************/

void insert_col_width( doc_el_group * a_group )
{
    uint32_t    depth;

    /****************************************************************/
    /*  if t_page.cols->col_width is empty and if it fits, place    */
    /*    cur_doc_el_group in t_page.cols->col_width                */
    /*  otherwise, append cur_doc_el_group to n_page.last_col_width */
    /*  only FIG/eFIG blocks with place set to top and width no     */
    /*    greater than "column" come here                           */
    /*  NOTE: FIG/eFIG blocks must fit on an empty page or, rather, */
    /*        must be split so as to fit before they are submitted  */
    /****************************************************************/

    depth = a_group->depth;
    if( depth <= t_page.max_depth ) {
        if( t_page.cur_col->col_width == NULL ) {          // must be empty
            t_page.cur_col->col_width = a_group->first;
            depth += a_group->post_skip;
            a_group->first = NULL;
            if( bin_driver->y_positive == 0 ) {
                t_page.cur_col->main_top -= depth;
            } else {
                t_page.cur_col->main_top += depth;
            }
            t_page.max_depth -= depth;
            a_group->first = NULL;
            add_doc_el_group_to_pool( a_group );
        } else {        // save on n_page as a block, not as doc_elements
            if( n_page.col_width == NULL ) {
                n_page.col_width = a_group;
            } else {
                n_page.last_col_width->next = a_group;
            }
            n_page.last_col_width = a_group;
        }
    } else {
        if( n_page.col_width == NULL ) {
            n_page.col_width = a_group;
        } else {
            n_page.last_col_width->next = a_group;
        }
        n_page.last_col_width = a_group;
    }

    return;
}


/***************************************************************************/
/*  insert a doc_element into t_page.page_width                            */
/***************************************************************************/

void insert_page_width( doc_el_group * a_group )
{
    uint32_t    depth;

    /****************************************************************/
    /*  headings with page_eject set to any value except ej_no or   */
    /*  which will not fit on the current page and FIG/eFIG blocks  */
    /*  with place set to top and width greater than "column" come  */
    /*  here                                                        */
    /*  headings must be sized before submission as they cannot be  */
    /*  split or delayed                                            */
    /*  NOTE: FIG/eFIG blocks must fit on an empty page or, rather, */
    /*        must be split so as to fit before they are submitted  */
    /****************************************************************/

    depth = a_group->depth;
    if( depth <= t_page.max_depth ) {
        if( t_page.panes->page_width == NULL ) {       // must be empty
            t_page.panes->page_width = a_group->first;
            a_group->first = NULL;
            depth += a_group->post_skip;
            ProcFlags.page_started = true;
            if( bin_driver->y_positive == 0 ) {
                t_page.panes_top -= depth;
                t_page.last_pane->col_width_top -= depth;
                t_page.cur_col->main_top -= depth;
            } else {
                t_page.panes_top += depth;
                t_page.last_pane->col_width_top -= depth;
                t_page.cur_col->main_top += depth;
            }
            t_page.max_depth -= depth;
            a_group->first = NULL;
            add_doc_el_group_to_pool( a_group );
        } else {        // save on n_page as a block, not as doc_elements
            if( n_page.page_width == NULL ) {
                n_page.page_width = a_group;
            } else {
                n_page.last_page_width->next = a_group;
            }
            n_page.last_page_width = a_group;
        }
    } else {
        internal_err( __FILE__, __LINE__ );
    }

    return;
}


/***************************************************************************/
/*  force output of the current page, even if not full                     */
/*  t_page and n_page will both be empty on return                         */
/*  Note: may need to be expanded to consider doc_elements accumulated by  */
/*  the various tags and control words that accumulate doc_elements        */
/***************************************************************************/

void last_col_out( void )
{
    full_col_out();

    /****************************************************************/
    /* Emit last page only if it has content, not just banners      */
    /* Note: this should be sufficient: it appears to be impossible */
    /*       for a page with any content at all to not have some    */
    /*       content in the first column of the first pane          */
    /****************************************************************/

    if( (t_page.panes->page_width != NULL) ||
        (t_page.panes->cols[0].col_width != NULL) ||
        (t_page.panes->cols[0].main != NULL) ||
        (t_page.panes->cols[0].bot_fig != NULL) ||
        (t_page.panes->cols[0].footnote != NULL) ) {

        /****************************************************************/
        /* Finish drawing any open box.                                 */
        /* Note: this may need to be done for other control words and   */
        /*       tags that accumulate doc_elements                      */
        /****************************************************************/

        if( ProcFlags.in_bx_box ) {
            eoc_bx_box();
        }

        next_column();
    }

    return;
}


/***************************************************************************/
/*  force output of the current page, even if not full                     */
/*  t_page and n_page will both be empty on return                         */
/*  Note: may need to be expanded to consider doc_elements accumulated by  */
/*  the various tags and control words that accumulate doc_elements        */
/***************************************************************************/

void last_page_out( void )
{
    full_col_out();

    /****************************************************************/
    /* Emit last page only if it has content, not just banners      */
    /* Note: this should be sufficient: it appears to be impossible */
    /*       for a page with any content at all to not have some    */
    /*       content in the first column of the first pane          */
    /****************************************************************/

    if( (t_page.panes->page_width != NULL) ||
        (t_page.panes->cols[0].col_width != NULL) ||
        (t_page.panes->cols[0].main != NULL) ||
        (t_page.panes->cols[0].bot_fig != NULL) ||
        (t_page.panes->cols[0].footnote != NULL) ) {

        /****************************************************************/
        /* Finish drawing any open box.                                 */
        /* Note: this may need to be done for other control words and   */
        /*       tags that accumulate doc_elements                      */
        /****************************************************************/

        if( ProcFlags.in_bx_box ) {
            eoc_bx_box();
        }

        do_page_out();
        reset_t_page();
    }

    return;
}


/***************************************************************************/
/*  set the bottom banner and adjust the max depth                         */
/***************************************************************************/

void reset_bot_ban( void )
{
    uint32_t    old_depth;

    if( t_page.bottom_banner != NULL ) {
        old_depth = t_page.bottom_banner->ban_depth;
    } else {
        old_depth = 0;
    }

    t_page.bottom_banner = sect_ban_bot[(g_page & 1) == 0];

    if( t_page.bottom_banner != NULL ) {
        bottom_depth = t_page.bottom_banner->ban_depth;
        if( bin_driver->y_positive == 0x00 ) {
            t_page.bot_ban_top = g_page_bottom_org + bottom_depth;
        } else {
            t_page.bot_ban_top = g_page_bottom_org - bottom_depth;
        }
    } else {
        bottom_depth = 0;
        t_page.bot_ban_top = g_page_bottom_org;
    }

    if( old_depth < bottom_depth ) {
        max_depth -= (bottom_depth - old_depth);
    } else if( bottom_depth < old_depth ) {
        max_depth += (old_depth - bottom_depth);
    }
}


/***************************************************************************/
/*  set the top banner and adjust the max depth                            */
/***************************************************************************/

void reset_top_ban( void )
{
    uint32_t    old_depth;

    if( t_page.top_banner != NULL ) {
        old_depth = t_page.top_banner->ban_depth;
    } else {
        old_depth = 0;
    }

    t_page.top_banner = sect_ban_top[(g_page & 1) == 0];

    if( t_page.top_banner != NULL ) {
        top_depth = t_page.top_banner->ban_depth;
        if( bin_driver->y_positive == 0x00 ) {
            t_page.panes_top = t_page.page_top - top_depth;
        } else {
            t_page.panes_top = t_page.page_top + top_depth;
        }
    } else {
        top_depth = 0;
        t_page.panes_top = t_page.page_top;
    }
    t_page.panes->page_width_top = t_page.panes_top;

    if( old_depth < top_depth ) {
        t_page.max_depth -= (top_depth - old_depth);
    } else if( top_depth < old_depth ) {
        t_page.max_depth += (old_depth - top_depth);
    }
}


/***************************************************************************/
/* reset t_page and related externs                                        */
/***************************************************************************/

void reset_t_page( void )
{
    doc_pane    *   cur_pane;
    doc_pane    *   sav_pane;
    int             i;

    if( n_page.fk_queue == NULL ){
        t_page.cur_depth = n_page.prev_pg_depth;
        n_page.prev_pg_depth = 0;
    } else {
        t_page.cur_depth = 0;
    }

    reset_top_ban();
    reset_bot_ban();

    if( t_page.top_banner != NULL ) {
        top_depth = t_page.top_banner->ban_depth;
    } else {
        top_depth = 0;
    }

    if( t_page.bottom_banner != NULL ) {
        bottom_depth = t_page.bottom_banner->ban_depth;
    } else {
        bottom_depth = 0;
    }

    t_page.max_depth = g_page_depth - top_depth - bottom_depth;
    old_max_depth = t_page.max_depth;
    if( t_page.panes->cols[0].main == NULL ) {
        t_page.last_col_main = NULL;
    }
    if( t_page.panes->cols[0].footnote == NULL ) {
        t_page.last_col_fn = NULL;
    }
    max_depth = t_page.max_depth - t_page.cur_depth;

    if( t_page.panes->next != NULL ) {  // keep only the first pane
        cur_pane = t_page.panes->next;
        while( cur_pane != NULL ) {
            sav_pane = cur_pane->next;
            cur_pane->next = NULL;
            mem_free( cur_pane );
            cur_pane = sav_pane;
        }
    }
    t_page.last_pane = t_page.panes;
    t_page.panes->page_width_top = t_page.panes_top;
    t_page.panes->col_width_top = t_page.panes_top;
    for( i = 0; i < t_page.panes->col_count; i++ ) {
        t_page.panes->cols[i].main_top = t_page.panes_top;
        t_page.panes->cols[i].fig_top = t_page.bot_ban_top;
        t_page.panes->cols[i].fn_top = t_page.bot_ban_top;
    }
    t_page.panes->cur_col = 0;
    t_page.cur_col = &t_page.panes->cols[0];
    t_page.eol_index = NULL;
    ProcFlags.col_started = false;
}


/***************************************************************************/
/*  merge the various skips and set the externs appropriately              */
/***************************************************************************/

void set_skip_vars( su * pre_skip, su * pre_top_skip, su * post_skip,
                    text_space text_spacing, font_number font )
{
    int32_t skiptop;
    int32_t skippost;
    int32_t skippre;

    g_units_spacing = ( text_spacing - 1 ) * wgml_fonts[font].line_height;
    g_blank_units_lines = g_blank_text_lines * wgml_fonts[font].line_height;
    if( g_blank_text_lines > 0 ) {
        g_blank_units_lines += ( g_blank_text_lines - 1 ) * g_units_spacing;
    }
    g_blank_units_lines += g_space;
    g_blank_text_lines = 0;

    /* SK can tell if it follows SP or blank lines, but not if it precedes them */

    if( !ProcFlags.sk_2nd ) {
        if( g_blank_units_lines > g_skip ) {
            g_skip = 0;                         // use g_blank_units_lines
        } else {
            g_top_skip += g_blank_units_lines;  // use g_blank_units_lines at top of page
            g_blank_units_lines = 0;            // use SK skip elsewhere
        }
    } else {
        ProcFlags.sk_2nd = false;               // if was true, make false
    }

    if( g_blank_units_lines > 0 ) {             // blank space into el_vspace element
        t_element = init_doc_el( el_vspace, 0 );
        insert_col_main( t_element );
        t_element = NULL;
        t_el_last = NULL;
        g_blank_units_lines = 0;
    }

    if( pre_skip != NULL ) {
        skippre = conv_vert_unit( pre_skip, text_spacing, font );
    } else {
        skippre = 0;
    }

    if( pre_top_skip != NULL ) {
        skiptop = conv_vert_unit( pre_top_skip, text_spacing, font );
    } else {
        skiptop = 0;
    }

    if( g_subs_skip > g_skip ) {    // provisional: merge with existing value
        g_skip = g_subs_skip;       // not yet in Wiki per server failure
    }

    if( g_post_skip > g_skip ) {    // merge sk-skip & post-skip per Wiki
        skippost = g_post_skip;
    } else {
        skippost = g_skip;
    }

    if( skippre > skiptop ) {   // merge pre-skip & pre-top-skip per Wiki
        if( skippre > (2 * skiptop) ) {
            skippre -= skiptop;
        } else {
            skippre = skiptop;
        }
    } else {
        skippre = skiptop;
    }

    if( skippost > skippre ) {  // final merger per Wiki
        skippre = skippost;
    }

    g_subs_skip = skippre;
    g_top_skip += skiptop;

    if( post_skip != NULL ) {
        g_post_skip = conv_vert_unit( post_skip, text_spacing, font );
    } else {
        g_post_skip = 0;
    }
    ProcFlags.skips_valid = true;
    g_skip = 0;
    g_space = 0;

    return;
}


/***************************************************************************/
/*  split one doc_element into two                                         */
/*  the first doc_element returned will fit in the depth specified         */
/*  the return value will be false if the doc_element could not be split   */
/*                                                                         */
/*  NOTE: this should only be called if the element will not fit in the    */
/*        available space.                                                 */
/*  NOTE: only text elements are actually split here; boxes and figures    */
/*        that are split are handled elsewhere and should always fit       */
/***************************************************************************/

bool split_element( doc_element * a_element, uint32_t req_depth )
{
    bool            splittable  =   true;
    doc_element *   split_el;
    text_line   *   cur_line;
    text_line   *   last        =   NULL;
    uint32_t        cur_count   =   0;
    uint32_t        tot_count   =   0;
    uint32_t        cur_depth   =   0;

    switch( a_element->type ) {
    // add code for other element types as appropriate
    case el_binc :
    case el_dbox :
    case el_graph :
    case el_hline :
    case el_vline :
    case el_vspace :
        splittable = false;
        break;
    case el_text :

        cur_line = a_element->element.text.first;

        /* Error if first line will not fit on any page */

        if( ( cur_line->line_height + cur_line->units_spacing ) > old_max_depth ) {
            xx_err( err_text_line_too_deep );
            break;
        }

        if( g_cur_threshold == 1 ) {                // simplest case

            while( cur_line != NULL ) {
                if( ( cur_depth + cur_line->line_height + cur_line->units_spacing ) > req_depth ) {
                    break;
                }
                cur_depth += cur_line->line_height + cur_line->units_spacing;
                last = cur_line;
                cur_line = cur_line->next;
            }

            if( cur_line == NULL ) {                // all lines fit; unlikely, but seen
                splittable = false;
                break;
            }

        } else {                                    // not so simple

            /* compute tot_count, the total number of lines in the paragraph */

            while( cur_line != NULL ) {
                tot_count++;
                cur_line = cur_line->next;
            }

            /* compute cur_depth, the depth of the first g_cur_threshold lines */

            cur_line = a_element->element.text.first;
            while( cur_line != NULL ) {
                if( cur_count < g_cur_threshold ) {
                    cur_depth += cur_line->line_height + cur_line->units_spacing;
                } else {
                    break;
                }
                cur_count++;
                last = cur_line;
                cur_line = cur_line->next;
            }

            /****************************************************************/
            /* if there is not enough room for the first g_cur_threshold    */
            /* lines or if there are not enough lines to put at least       */
            /* g_cur_threshold lines on BOTH pages, then the paragraph      */
            /* cannot be split but must be placed on the next page.         */
            /****************************************************************/

            if( (tot_count < (2 * g_cur_threshold)) || (cur_depth > req_depth) ) {
                last = NULL;
            } else {
                while( cur_line != NULL ) {
                    if( (cur_count >= (tot_count - g_cur_threshold)) || ((cur_line->next != NULL) &&
                        (cur_depth + (cur_line->line_height + cur_line->units_spacing)) > req_depth) ) {
                        break;
                    }
                    cur_count++;
                    cur_depth += (cur_line->line_height + cur_line->units_spacing);
                    last = cur_line;
                    cur_line = cur_line->next;
                }
            }
        }

        if( cur_line == NULL ) {        // all lines fit; unlikely, but seen
            splittable = false;
            break;
        }

        if( last == NULL ) {

            /* nothing to split; cur_line is the first line in a_element */

            splittable = false;     // widow criteria failed
            break;

        }

        /************************************************************/
        /*  if we get here, a_element is splittable, cur_line is    */
        /*  the first line of the new element, and last is the last */
        /*  line that can be left in the original element           */
        /************************************************************/

        split_el = alloc_doc_el( el_text ); // most defaults are correct

        split_el->depth = a_element->depth - cur_depth;
        split_el->element.text.first = cur_line;
        last->next = NULL;                  // this restricts cur_el to the lines allowed
        a_element->depth = cur_depth;
        if( a_element->next == NULL ) {
            a_element->next = split_el;
        } else {
            split_el->next = a_element->next;
            a_element->next = split_el;
        }
        break;
    default :
        internal_err( __FILE__, __LINE__ );
    }
    return( splittable );
}


/***************************************************************************/
/*  output all pages in which t_page.cols->main is full                    */
/*  t_page will not be empty but n_page.col_main will be empty on return   */
/*  Note: may need to be expanded to consider doc_elements accumulated by  */
/*  the various tags and control words that accumulate doc_elements        */
/***************************************************************************/

void text_col_out( void )
{
    while( n_page.col_main != NULL ) {
        next_column();
    }
    return;
}

