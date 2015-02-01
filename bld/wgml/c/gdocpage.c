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
* Description:  Page-oriented output
*
*               clear_doc_element       return all text_lines/text_chars to pools
*               do_ban_column_out       output text from one or more ban_columns
*               do_doc_column_out       output text from one or more doc_columns
*               do_el_list_out          output text from an array of element lists
*               do_page_out             actually output t_page
*               full_page_out           output all full pages
*               insert_col_bot          insert doc_element into t_page.main->bot_fig
*               insert_col_fn           insert doc_element into t_page.main->footnote
*               insert_col_main         insert doc_element into t_page.main->main
*               insert_col_top          insert doc_element into top of t_page.main->main
*               insert_page_width       insert doc_element into t_page.full_page
*               last_page_out           force output of all remaining pages
*               reset_t_page            reset t_page and related externs
*               set_skip_vars           merge the various skips into the externs
*               set_v_positions         set vertical positions in an element list
*               split_element           splits an element at given depth
*               text_page_out           output all pages where main is full
*               update_t_page           update t_page from n_page
*
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */

#include "wgml.h"
#include "gvars.h"


/***************************************************************************/
/*  does the actual output to the device                                   */
/***************************************************************************/
 
static void do_el_list_out( doc_element * array, unsigned char count )
{
    doc_element *   cur_el;
    doc_element *   save;
    int             i;
    text_line   *   cur_line;

    /*  Advise the user that multiple columns are not, in fact, implemented */

    if( count > 1 ) {
        out_msg( "Multi-column output not implemented" );
    }

    /*  The array should have the doc_elements in linked-list form in the   */
    /*  same order as the columns they came from were linked together.      */
    /*  The columns should no longer point to these doc_element lists.      */

    for( i = 0; i < count; i++ ) {
        cur_el = &array[i];
        while( cur_el != NULL ) {
            if( i == 0 ) {      // restrict output to first column, for now
                switch( cur_el->type ) {
                case el_binc :
                    if( GlobalFlags.lastpass ) {
                        ob_binclude( &cur_el->element.binc );
                    }
                    break;
                case el_dbox :  // should only be found if DBOX block exists
                    if( GlobalFlags.lastpass ) {
                        fb_dbox( &cur_el->element.dbox );
                    }
                    break;
                case el_graph :
                    if( GlobalFlags.lastpass ) {
                        if( ProcFlags.ps_device ) {   // only available to PS device
                            ob_graphic( &cur_el->element.graph );
                        }
                    }
                    break;
                case el_hline :  // should only be found if HLINE block exists
                    if( GlobalFlags.lastpass ) {
                        fb_hline( &cur_el->element.hline );
                    }
                    break;
                case el_text :
                    if( GlobalFlags.lastpass ) {
                        for( cur_line = cur_el->element.text.first; cur_line != NULL; cur_line = cur_line ->next ) {
                            fb_output_textline( cur_line );
                        }
                    }
                    break;
                case el_vline :  // should only be found if VLINE block exists
                    if( GlobalFlags.lastpass ) {
                        fb_vline( &cur_el->element.vline );
                    }
                    break;
                default :
                    internal_err( __FILE__, __LINE__ );
                }
            }
            save = cur_el->next;
            cur_el->next = NULL;            // clear only current element
            clear_doc_element( cur_el );
            add_doc_el_to_pool( cur_el ); 
            cur_el = save;
        }
    }    

    return;
}

/***************************************************************************/
/*  set the vertical positions in a linked list of elements                */
/***************************************************************************/

static void set_v_positions( doc_element * list, uint32_t v_start )
{
    doc_element *   cur_el;
    text_line   *   cur_line;
    uint32_t        cur_spacing;

    g_cur_v_start = v_start;
    
    for( cur_el = list; cur_el != NULL; cur_el = cur_el->next ) {
        cur_spacing = cur_el->blank_lines;
        if( cur_el->type == el_text ) {
            cur_spacing += cur_el->element.text.spacing;
        }
        if( !ProcFlags.page_started ) {
            if( cur_el->blank_lines > 0 ) {
                cur_spacing = cur_el->blank_lines + cur_el->subs_skip;
            } else {
                cur_spacing = cur_el->top_skip;
            }
            ProcFlags.page_started = true;
        } else {
            cur_spacing += cur_el->subs_skip;
        }

        switch( cur_el->type ) {
        case el_binc :
            cur_el->element.binc.at_top = !ProcFlags.page_started &&
                                          (t_page.top_banner == NULL);
            if( bin_driver->y_positive == 0x00 ) {
                g_cur_v_start -= cur_spacing;
            } else {
                g_cur_v_start += cur_spacing;
            }
            cur_el->element.binc.y_address = g_cur_v_start;
            if( bin_driver->y_positive == 0x00 ) {
                g_cur_v_start -= cur_el->depth;
            } else {
                g_cur_v_start += cur_el->depth;
            }
            break;
        case el_dbox :
            if( bin_driver->y_positive == 0x00 ) {
                g_cur_v_start -= cur_spacing;
            } else {
                g_cur_v_start += cur_spacing;
            }
            cur_el->element.dbox.v_start = g_cur_v_start;
            if( bin_driver->y_positive == 0x00 ) {
                g_cur_v_start -= cur_el->depth;
            } else {
                g_cur_v_start += cur_el->depth;
            }
            break;
        case el_graph :
            cur_el->element.graph.at_top = !ProcFlags.page_started &&
                                          (t_page.top_banner == NULL);
            if( bin_driver->y_positive == 0x00 ) {
                g_cur_v_start -= cur_spacing;
            } else {
                g_cur_v_start += cur_spacing;
            }
            cur_el->element.graph.y_address = g_cur_v_start;
            if( bin_driver->y_positive == 0x00 ) {
                g_cur_v_start -= cur_el->depth;
            } else {
                g_cur_v_start += cur_el->depth;
            }
            break;
        case el_hline :
            if( bin_driver->y_positive == 0x00 ) {
                g_cur_v_start -= cur_spacing;
            } else {
                g_cur_v_start += cur_spacing;
            }
            cur_el->element.hline.v_start = g_cur_v_start;
            if( bin_driver->y_positive == 0x00 ) {
                g_cur_v_start -= cur_el->depth;
            } else {
                g_cur_v_start += cur_el->depth;
            }
            break;
        case el_text :
            for( cur_line = cur_el->element.text.first; cur_line != NULL;
                                                cur_line = cur_line->next ) {
                cur_spacing += cur_line->line_height;
                if( ProcFlags.page_started ) {          // not first element
                    if( cur_el->element.text.overprint ) {
                        cur_spacing -= cur_line->line_height;   // overprint
                        cur_el->element.text.overprint = false;
                    }
                } else {                                
                    if( t_page.top_ban == NULL ) {      // minimun height
                        if( cur_spacing < wgml_fonts[g_curr_font].line_height ) {
                            cur_spacing = wgml_fonts[g_curr_font].line_height;
                        }
                    }
                }
                if( bin_driver->y_positive == 0x00 ) {
                    g_cur_v_start -= cur_spacing;
                } else {
                    g_cur_v_start += cur_spacing;
                }
                cur_line->y_address = g_cur_v_start;
                cur_spacing = cur_el->element.text.spacing;
            }
            break;
        case el_vline :
            if( bin_driver->y_positive == 0x00 ) {
                g_cur_v_start -= cur_spacing;
            } else {
                g_cur_v_start += cur_spacing;
            }
            cur_el->element.vline.v_start = g_cur_v_start;
            if( bin_driver->y_positive == 0x00 ) {
                g_cur_v_start -= cur_el->depth;
            } else {
                g_cur_v_start += cur_el->depth;
            }
            break;
        default :
            internal_err( __FILE__, __LINE__ );
        }
    }

    return;
}


/***************************************************************************/
/*  output the ban_column(s)                                               */
/***************************************************************************/

static void do_ban_column_out( ban_column * a_column, uint32_t v_start )
{
    ban_column          *cur_col;
    doc_element         **cur_el;
    int                 i;
    unsigned char       col_count;

    col_count = 0;
    for( cur_col = a_column; cur_col != NULL; cur_col = cur_col->next ) {
        col_count++;
    }
    cur_el = (doc_element **)mem_alloc( col_count * sizeof( doc_element * ) );

    cur_col = a_column;
    for( i = 0; i < col_count; i++ ) {
        cur_el[i] = NULL;
        if( cur_col->first != NULL ) {
            set_v_positions( cur_col->first, v_start );
            cur_el[i] = cur_col->first;
            cur_col->first = NULL;
        }
        cur_col = cur_col->next;
    }

    do_el_list_out( *cur_el, col_count );
    mem_free( cur_el );

    return;
}


/***************************************************************************/
/*  output the doc_column(s)                                          */
/***************************************************************************/
 
static void do_doc_column_out( doc_column * a_column, uint32_t v_start )
{
    doc_column  *       cur_col;
    doc_element *   *   cur_el;
    doc_element *       last;
    int                 i;
    unsigned char       col_count;

    v_start = v_start;
    col_count = 0;
    for( cur_col = a_column; cur_col != NULL; cur_col = cur_col->next ) {
        col_count++;
    }
    cur_el = (doc_element **)mem_alloc( col_count * sizeof( doc_element * ) );

    cur_col = a_column;
    for( i = 0; i < col_count; i++ ) {
        last = NULL;
        cur_el[i] = NULL;
        if( cur_col->main != NULL ) {
            set_v_positions( cur_col->main, cur_col->main_top );
            if( cur_el[i] == NULL ) {
                cur_el[i] = cur_col->main;
                last = cur_col->main;
            } else {
                for( ; last->next != NULL; last = last->next );
                last->next = cur_col->main;
            }
            cur_col->main = NULL;
        }
        if( cur_col->bot_fig != NULL ) {
            set_v_positions( cur_col->bot_fig, cur_col->fig_top );
            if( cur_el[i] == NULL ) {
                cur_el[i] = cur_col->bot_fig;
                last = cur_col->bot_fig;
            } else {
                for( ; last->next != NULL; last = last->next );
                last->next = cur_col->bot_fig;
            }
            cur_col->bot_fig = NULL;
        }
        if( cur_col->footnote != NULL ) {
            set_v_positions( cur_col->footnote, cur_col->fn_top );
            if( cur_el[i] == NULL ) {
                cur_el[i] = cur_col->footnote;
                last = cur_col->footnote;
            } else {
                for( ; last->next != NULL; last = last->next );
                last->next = cur_col->footnote;
            }
            cur_col->footnote = NULL;
        }
        cur_col = cur_col->next;
    }

    do_el_list_out( *cur_el, col_count );
    mem_free( cur_el );

    return;
}


/***************************************************************************/
/*  update t_page from the elements in n_page                              */
/*  n_page.col_main may or may not be empty on return                      */
/***************************************************************************/

static void update_t_page( void )
{
    bool            fig_placed;
    bool            splittable;
    doc_element *   cur_el;
    uint32_t        depth;

    reset_t_page();

    /*  some section headings are placed in t_page.page_width when          */
    /*  processed. One FIG in n_page.col_top goes into t_page.page_width    */
    /*  if it is empty                                                      */
    /*  t_page.page_width can only hold one doc_element                     */
    /*  This is preliminary and may be changed as needed.                   */

    depth = t_page.cur_depth;
    fig_placed = false;
    if( t_page.page_width == NULL ) {       // skip if section full
        if( n_page.col_top != NULL ) {   // at most one item can be placed
            switch( n_page.col_top->type ) {
            // add code for FIG when needed
            case el_text :  // all elements should be FIGs
            default :
                internal_err( __FILE__, __LINE__ );
            }
        }
    }

    /*  one FIG in n_page.col_top goes into t_page.main->top_fig if none    */
    /*  was placed in t_page.page_width                                     */
    /*  this is preliminary and may be changed as needed                    */
    /*  Note: t_page.main is NULL at this point, initialize if needed       */
    
    if( !fig_placed ) {
        if( n_page.col_top != NULL ) {   // at most one item can be placed
            switch( n_page.col_top->type ) {
            // add code for FIG when needed
            case el_text :  // all elements should be FIGs
            default :
                internal_err( __FILE__, __LINE__ );
            }
        } 
    }

    /***********************************************************************/
    /*  The order here is not clear.                                       */
    /*  That shown is based on filling the t_page.main->bot_fig and the    */
    /*  t_page.main->footnote parts before filling t_page.main->main, thus */
    /*  having a firm depth for t_page.main->main to start with.           */
    /*  However, if there is no top_fig, then the first doc_element in     */
    /*  t_page.main->main must set its pre_skip to 0, and then what        */
    /*  happens if there is no room for anything to be placed there?       */
    /*  So this may need to be rethought when FN and FIG are implemented.  */
    /***********************************************************************/

    /*  one FIG in n_page.col_bot goes into t_page.main->bot_fig            */
    /*  entrained footnotes need to be moved to n_page.col_fn               */
    /*  this is preliminary and may be changed as needed                    */
    /*  Note: t_page.main may be NULL at this point, initialize if needed   */

    if( n_page.col_bot != NULL ) {   // at most one item can be placed
        switch( n_page.col_bot->type ) {
        // add code for FIG & entrained footnotes when needed
        case el_text :  // all elements should be FIGs or footnotes
        default :
            internal_err( __FILE__, __LINE__ );
        }
    }

    /*  all footnotes in n_page.col_fn go into t_page.main->footnote        */
    /*  this is preliminary and may be changed as needed                    */
    /*  Note: t_page.main may be NULL at this point, initialize if needed   */

    if( n_page.col_fn != NULL ) {   // at most one item can be placed
        switch( n_page.col_fn->type ) {
        // add code for footnotes when needed
        case el_text :  // all elements should be footnotes
        default :
            internal_err( __FILE__, __LINE__ );
        }
    }

    /*  t_page.main->main is used for the bulk of the elements              */
    /*  it is filled last because the other deferred items are believed to  */
    /*  have priority; this may change as the situation is clarified.       */
    /*  unless the section is ended, there must be at least on element left */
    /*  in n_page.col_main for the page to be full                          */
    /*  Note: when FIG/FN processing is implemented, t_page.main may not be */
    /*  NULL at this point                                                  */

    /****************************************************************/
    /*  test version until things get a bit more clear              */
    /*  the theory here is that only one processing step should be  */
    /*      here, and then the function calling update_t_page()     */
    /*      should be relied on to output the page                  */
    /****************************************************************/
    
    while( n_page.col_main != NULL ) {
        cur_el = n_page.col_main;
        n_page.col_main = n_page.col_main->next;
        if( n_page.col_main == NULL ) {
            n_page.last_col_main = NULL;
        }

        /****************************************************************/
        /*  this section identifies skips and blank lines that finish   */
        /*  the current page and then exits the loop after adjusting    */
        /*  the element field values as needed                          */
        /****************************************************************/
        
        if( cur_el->blank_lines > 0 ) {
            if( (t_page.cur_depth + cur_el->blank_lines) >= t_page.max_depth ) {
                cur_el->blank_lines -= (t_page.max_depth - t_page.cur_depth);
                break;
            } else if( !ProcFlags.page_started && ((t_page.cur_depth +
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

        if( !ProcFlags.page_started ) {
            if( cur_el->blank_lines > 0 ) {
                depth += cur_el->blank_lines + cur_el->subs_skip;
            } else {
                depth += cur_el->top_skip;
            }
            ProcFlags.page_started = true;
        } else {
            depth += cur_el->blank_lines + cur_el->subs_skip;
        }

        if( depth >= t_page.max_depth ) {    // skip fills page
            break;
        }

        /****************************************************************/
        /*  Does the first line minimum apply here? If so, it needs to  */
        /*  be implemented. Note that cur_el->depth does not reflect it */
        /*  because there is no way to tell if it will apply when the   */
        /*  cur_el->depth is computed.                                  */
        /****************************************************************/

        if( (depth + cur_el->depth) > t_page.max_depth ) {    // cur_el will fill the page
            splittable = split_element( cur_el, t_page.max_depth -
                                                t_page.cur_depth - depth );
            if( splittable ) {
                if( cur_el->next != NULL ) {    // cur_el was split
                    n_page.col_main = cur_el->next;
                    if( n_page.last_col_main == NULL ) {
                        n_page.last_col_main = n_page.col_main;
                    }
                    cur_el->next = NULL;
                }
                if( t_page.main == NULL ) {
                    t_page.main = alloc_doc_col();
                }
                if( t_page.main->main == NULL ) {
                    t_page.main->main = cur_el;
                    t_page.last_col_main = t_page.main->main;
                } else {
                    t_page.last_col_main->next = cur_el;
                    t_page.last_col_main = t_page.last_col_main->next;
                }
                t_page.last_col_main->next = NULL;
                t_page.cur_depth += cur_el->depth;
            } else {
                if( (t_page.main == NULL) || (t_page.main->main == NULL) ) { // adapt when FIG/FN done
                    xx_err( err_text_line_too_deep );
                    g_suicide();    // no line will fit on any page
                }
                n_page.col_main = cur_el->next;
                if( n_page.last_col_main == NULL ) {
                    n_page.last_col_main = n_page.col_main;
                }
                cur_el->next = NULL;
            }
        } else {                                    // cur_el fits as-is
            if( t_page.main == NULL ) {
                t_page.main = alloc_doc_col();
            }
            if( t_page.main->main == NULL ) {
                t_page.main->main = cur_el;
                t_page.last_col_main = t_page.main->main;
            } else {
                t_page.last_col_main->next = cur_el;
                t_page.last_col_main = t_page.last_col_main->next;
            }
            t_page.last_col_main->next = NULL;
            t_page.cur_depth += cur_el->depth;
        }
    }
    return;
}


/***************************************************************************/
/*  return all text_chars/text_lines in element to appropriate pool        */
/*  only does el_text currently may need expansion in future               */
/***************************************************************************/

void clear_doc_element( doc_element * element )
{
    doc_element *   cur_el;
    text_line   *   cur_line;
    text_line   *   save;

    for( cur_el = element; cur_el != NULL; cur_el = cur_el->next ) {
        switch( cur_el->type ) {
        case el_binc :
        case el_dbox :
        case el_graph :
        case el_hline :
        case el_vline :
            break;      // should be nothing to do
        case el_text :
            cur_line = cur_el->element.text.first;
            while( cur_line != NULL ) {
                add_text_chars_to_pool( cur_line );
                save = cur_line->next;
                add_text_line_to_pool( cur_line );
                cur_line = save;
            }
            break;
        default :
            internal_err( __FILE__, __LINE__ );
        }
    }

    return;
}


/***************************************************************************/
/*  actually output t_page to the device                                   */
/***************************************************************************/

void do_page_out( void )
{
// these were in document_top_banner()
    uint32_t    hs;
    uint32_t    hl;

    /* Set up for the new page */

    if( apage && GlobalFlags.lastpass ) {   // don't do before first page
       fb_document_page();                  // NEWPAGE is interpreted here
    }
    apage++;
    page++;

    /* Get the banner text into the proper sections */

// this was document_top_banner()
    if( ProcFlags.keep_left_margin ) {
        hs = g_cur_h_start;
        hl = g_cur_left;
    }
    g_cur_h_start = g_page_left_org;
    g_cur_left    = g_page_left_org;

    if( t_page.top_banner != NULL ) {
        out_ban_top();
    }

    if( ProcFlags.keep_left_margin ) {
        g_cur_h_start = hs;
        g_cur_left = hl;
    }
// end of former document_top_banner()

    if( t_page.bottom_banner != NULL ) {
        out_ban_bot();
    }

    /* Output the page section by section */

    ProcFlags.page_started = false;
    if( t_page.top_ban != NULL ) {
        do_ban_column_out( t_page.top_ban, g_page_top_org );
        add_ban_col_to_pool( t_page.top_ban );
        t_page.top_ban = NULL;
    }

    if( t_page.page_width != NULL ) {
        set_v_positions( t_page.page_width, g_page_top );
        do_el_list_out( t_page.page_width, 1 );
        t_page.page_width = NULL;
    }

    if( t_page.main != NULL ) {
        do_doc_column_out( t_page.main, t_page.main_top );
        add_doc_col_to_pool( t_page.main );
        t_page.main = NULL;
    }

    if( t_page.bot_ban != NULL ) {
        do_ban_column_out( t_page.bot_ban, g_page_bottom );
        add_ban_col_to_pool( t_page.bot_ban );
        t_page.bot_ban = NULL;
    }

    return;
}


/***************************************************************************/
/*  output all full pages                                                  */
/*  t_page will not be empty but n_page will be empty on return            */
/***************************************************************************/

void full_page_out( void )
{
    while( (n_page.col_top != NULL) || (n_page.col_main != NULL)
            || (n_page.col_bot != NULL) || (n_page.col_fn != NULL) ) {
        do_page_out();
        update_t_page();
    }
    return;
}


/****************************************************************************/
/*  insert a doc_element into t_page.main->bot_fig                          */
/*  may need to be updated as the use of this sections is clarified         */
/****************************************************************************/

void insert_col_bot( doc_element * a_element )
{
    bool        on_t_page;
    uint32_t    depth;

    /*  the model used, may require update when FIG implemented         */
    /*  if space exists, place a_element in t_page.main->bot_fig        */
    /*      no space exists unless t_main-bot_fig is empty              */
    /*      no space exists if a_element is too large to fit            */
    /*  if no space exists, append a_element to n_page.last_col_bot     */
    /*  note: "entrainment" of BOTTOM FIG by TOP FIG is ignored         */

    if( !ProcFlags.page_started ) {
        depth = a_element->top_skip;
    } else {
        depth = a_element->subs_skip;
    }
    depth += (t_page.cur_depth + a_element->depth);

    /****************************************************************/
    /*  Does the first line minimum apply here? If so, it needs to  */
    /*  be implemented. Note that cur_el->depth does not reflect it */
    /*  because there is no way to tell if it will apply when the   */
    /*  is computed.                                                */
    /****************************************************************/

    on_t_page = false;
    if( t_page.main == NULL ) {         // t_page.main->bot_fig can't be full
        if( depth <= t_page.max_depth ) {  // a_element will fit
            on_t_page = true;
        }
    } else {
        if( t_page.main->bot_fig == NULL ) { // t_page.main->bot_fig is empty
            if( depth <= t_page.max_depth ) {  // a_element will fit
                on_t_page = true;
            }
        }
    }

    if( on_t_page ) {
        if( t_page.main == NULL ) {
            t_page.main = alloc_doc_col();
        }
        t_page.main->bot_fig = a_element;
        t_page.last_col_bot = t_page.main->bot_fig;
        if( bin_driver->y_positive == 0) {
            t_page.main->fig_top += t_page.cur_depth; 
        } else {
            t_page.main->fig_top -= t_page.cur_depth; 
        }
    } else {
        if( n_page.last_col_bot == NULL ) {
            n_page.col_bot = a_element;
            n_page.last_col_bot = n_page.col_bot;
        } else {
            n_page.last_col_bot->next = a_element;
            n_page.last_col_bot = n_page.last_col_bot->next;
        }
    }

    return;
}

/***************************************************************************/
/*  insert a doc_element into t_page.main->footnote                        */
/*  may need to be updated as the situation is clarified                   */
/***************************************************************************/

void insert_col_fn( doc_element * a_element )
{
    bool        on_t_page;
    uint32_t    depth;

    /*  the model used, may require update when FN implemented          */
    /*  if space exists, place a_element in t_page.main->footnote       */
    /*      no space exists if a_element is too large to fit            */
    /*  if no space exists, append a_element to n_page.last_col_fn      */
    /*  note: "entrainment" of FN by FIG is ignored                     */

    if( !ProcFlags.page_started ) { // first element ignores pre_skip TBD
        depth = a_element->top_skip;
    } else {
        depth = a_element->subs_skip;
    }
    depth += (t_page.cur_depth + a_element->depth);

    /****************************************************************/
    /*  Does the first line minimum apply here? If so, it needs to  */
    /*  be implemented. Note that cur_el->depth does not reflect it */
    /*  because there is no way to tell if it will apply when the   */
    /*  is computed.                                                */
    /****************************************************************/

    if( depth <= t_page.max_depth ) {
        on_t_page = true;
    } else {
        on_t_page = false;
    }

    if( on_t_page ) {
        if( t_page.main == NULL ) {
            t_page.main = alloc_doc_col();
        }
        t_page.main->footnote = a_element;
        t_page.last_col_fn = t_page.main->footnote;
        if( bin_driver->y_positive == 0) {
            t_page.main->fig_top += t_page.cur_depth;
            t_page.main->fn_top += t_page.cur_depth;
        } else {
            t_page.main->fig_top -= t_page.cur_depth;
            t_page.main->fn_top -= t_page.cur_depth;
        }
    } else {
        if( n_page.last_col_fn == NULL ) {
            n_page.col_fn = a_element;
            n_page.last_col_fn = n_page.col_fn;
        } else {
            n_page.last_col_fn->next = a_element;
            n_page.last_col_fn = n_page.last_col_fn->next;
        }
    }

    return;
}


/***************************************************************************/
/*  insert a doc_element into t_page.main->main                            */
/*  may need update to work with multi-column pages: this would be a good  */
/*  place to add the next column to t_page.main                            */
/***************************************************************************/

void insert_col_main( doc_element * a_element )
{
    bool        page_full;
    bool        splittable;
    uint32_t    cur_skip;
    uint32_t    depth;

    /****************************************************************/
    /*  alternate procesing: accumulate elements for later          */
    /*  submission                                                  */
    /*  used currenlty by ADDRESS/eADDRESS                          */
    /****************************************************************/

    if( ProcFlags.group_elements ) {
        if( t_doc_el_group.first == NULL ) {
            t_doc_el_group.first = a_element;
            t_doc_el_group.last = t_doc_el_group.first;
        } else {
            t_doc_el_group.last->next = a_element;
            t_doc_el_group.last = t_doc_el_group.last->next;
        }
        t_doc_el_group.depth += a_element->depth;
        return;
    }

    /****************************************************************/
    /*  test version until things get a bit more clear              */
    /*  the theory here is that only one processing step should be  */
    /*      here, the rest being done in update_t_page()            */
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
        } else if( !ProcFlags.page_started && ((t_page.cur_depth +
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
    } else if( !ProcFlags.page_started ) {
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

        if( !ProcFlags.page_started ) {
            if( a_element->blank_lines > 0 ) {
                cur_skip = a_element->blank_lines + a_element->subs_skip;
            } else {
                cur_skip = a_element->top_skip;
            }
            ProcFlags.page_started = true;
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
        /*  Does the first line minimum apply here? If so, it needs to  */
        /*  be implemented. Note that cur_el->depth does not reflect it */
        /*  because there is no way to tell if it will apply when the   */
        /*  is computed.                                                */
        /****************************************************************/

        depth = cur_skip + a_element->depth;
        if( (depth + t_page.cur_depth) > t_page.max_depth ) {   // a_element fills the page
            splittable = split_element( a_element, t_page.max_depth -
                                        t_page.cur_depth - cur_skip );
            if( a_element->next != NULL ) { // a_element was split
                if( t_page.main == NULL ) {
                    t_page.main = alloc_doc_col();
                }
                if( t_page.main->main == NULL ) {
                    t_page.main->main = a_element;
                    t_page.last_col_main = t_page.main->main;
                } else {
                    t_page.last_col_main->next = a_element;
                    t_page.last_col_main = t_page.last_col_main->next;
                }
                t_page.cur_depth += depth;
                a_element = a_element->next;
                t_page.last_col_main->next = NULL;
                page_full = true;
            } else {                        // a_element could not be split
                if( t_page.main == NULL ) { // adapt when FIG/FN done
                    xx_err( err_text_line_too_deep );
                    g_suicide();    // no line will fit on any page
                }
                if( t_page.main->main == NULL ) { // adapt when FIG/FN done
                    xx_err( err_text_line_too_deep );
                    g_suicide();    // no line will fit on any page
                }
                page_full = true;
            }
        } else {    // the entire element fits on the current page
            if( t_page.main == NULL ) {
                    t_page.main = alloc_doc_col();
                }
            if( t_page.main->main == NULL ) {
                t_page.main->main = a_element;
                t_page.last_col_main = t_page.main->main;
            } else {
                t_page.last_col_main->next = a_element;
                t_page.last_col_main = t_page.last_col_main->next;
            }
            t_page.cur_depth += depth;
        }
    }

    if( page_full ) {

        /****************************************************************/
        /*  if page_full is true then a_element goes to n_page and      */
        /*  t_page must be output                                       */
        /****************************************************************/

        if( n_page.last_col_main == NULL ) {
            n_page.col_main = a_element;
            n_page.last_col_main = n_page.col_main;
        } else {
            n_page.last_col_main->next = a_element;
            n_page.last_col_main = n_page.last_col_main->next;
        }
        text_page_out();
    } 
    return;
}


/***************************************************************************/
/*  insert a doc_element into t_page.page_width                            */
/***************************************************************************/

void insert_page_width( doc_element * a_element )
{
    uint32_t    depth;

    /* depth is used to update t_page.cur_depth and so must be kept separate */

    if( !ProcFlags.page_started ) {
        depth = a_element->top_skip;
        ProcFlags.page_started = true;
    } else {
        depth = a_element->subs_skip;
    }
    depth += a_element->depth;

    /****************************************************************/
    /*  Does the first line minimum apply here? If so, it needs to  */
    /*  be implemented. Note that cur_el->depth does not reflect it */
    /*  because there is no way to tell if it will apply when the   */
    /*  is computed.                                                */
    /****************************************************************/

    switch( a_element->type ) {
    // TOP FIG processing goes here
    case el_text :                      // section heading: must go on t_page
        if( (depth + t_page.cur_depth) <= t_page.max_depth ) {
            if( t_page.page_width == NULL ) {   // must be empty
                t_page.page_width = a_element;
                t_page.last_col_main = t_page.page_width;
                t_page.cur_depth += depth;
                if( bin_driver->y_positive == 0 ) {
                    t_page.main_top -= depth;
                } else {
                    t_page.main_top += depth;
                }
            } else {        // discard second section heading
                internal_err( __FILE__, __LINE__ );
            }        
        } else {
            xx_err( err_heading_too_deep );
        }
        break;
    default:
        internal_err( __FILE__, __LINE__ );
    }

    return;
}


/***************************************************************************/
/*  force output of the current page, even if not full                     */
/*  t_page and n_page will both be empty on return                         */
/***************************************************************************/
 
void last_page_out( void )
{
    full_page_out();
    do_page_out();  // last page
    return;
}


/***************************************************************************/
/* reset t_page and related externs                                        */
/***************************************************************************/

void reset_t_page( void )
{
    uint32_t    bottom_depth;
    uint32_t    top_depth;

    t_page.top_banner = sect_ban_top[!(page & 1)];
    t_page.bottom_banner = sect_ban_bot[!(page & 1)];

    if( t_page.top_banner != NULL ) {
        top_depth = t_page.top_banner->ban_depth;
        if( bin_driver->y_positive == 0x00 ) {
            g_page_top = g_page_top_org - top_depth;
        } else {
            g_page_top = g_page_top_org + top_depth;
        }
    } else {
        top_depth = 0;
        g_page_top = g_page_top_org;
    }
    t_page.main_top = g_page_top;

    if( t_page.bottom_banner != NULL ) {
        bottom_depth = t_page.bottom_banner->ban_depth;
        if( bin_driver->y_positive == 0x00 ) {
            g_page_bottom = g_page_bottom_org + bottom_depth;
        } else {
            g_page_bottom = g_page_bottom_org - bottom_depth;
        }
    } else {
        bottom_depth = 0;
        g_page_bottom = g_page_bottom_org;
    }
    t_page.max_depth = g_page_depth - top_depth - bottom_depth;
    t_page.cur_depth = 0;
    t_page.last_col_main = NULL;
    t_page.last_col_bot = NULL;
    t_page.last_col_fn = NULL;
    ProcFlags.page_started = false;
}


/***************************************************************************/
/*  merge the various skips and set the externs appropriately              */
/***************************************************************************/

void set_skip_vars( su *pre_skip, su *pre_top_skip, su *post_skip, uint32_t spacing, font_number font )
{
    int32_t skiptop;
    int32_t skippost;
    int32_t skippre;
    int32_t skipsk;

    skipsk = calc_skip_value();     // pending .sk value?
    if( pre_skip != NULL ) {
        skippre = conv_vert_unit( pre_skip, spacing );
    } else {
        skippre = 0;
    }
    if( pre_top_skip != NULL ) {
        skiptop = conv_vert_unit( pre_top_skip, spacing );
    } else {
        skiptop = 0;
    }

    if( g_post_skip > skipsk ) {    // merge sk-skip & post-skip per Wiki
        skippost = g_post_skip;
    } else {
        skippost = skipsk;
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

    if( skippost > skippre ) { // final merger per Wiki
        skippre = skippost;
    }    

    g_subs_skip = skippre;
    g_top_skip = skiptop;

    if( post_skip != NULL ) {
        g_post_skip = conv_vert_unit( post_skip, spacing );
    } else {
        g_post_skip = 0;
    }
    g_blank_lines = blank_lines * wgml_fonts[font].line_height;
    blank_lines = 0;
    g_spacing = (spacing - 1) * wgml_fonts[font].line_height;
    ProcFlags.skips_valid = true;

    return;
}


/***************************************************************************/
/*  split one doc_element into two                                         */
/*  the first doc_element returned will fit in the depth specified         */
/*  the return value will be false if the doc_element could not be split   */
/***************************************************************************/

bool split_element( doc_element * a_element, uint32_t req_depth )
{
    bool            splittable;
    doc_element *   split_el;
    text_line   *   cur_line;
    text_line   *   last;
    uint32_t        count;
    uint32_t        cur_depth;

    count = 0;
    cur_depth = 0;
    last = NULL;
    splittable = true;

    switch( a_element->type ) {
    // add code for other element types; FIGs are documented to split only
    // when they will not fit by themselves on a page
    case el_binc :  // given how BINCLUDE/GRAPHIC work, this seems reasonable 
    case el_dbox :  // splitting boxes/lines is probably best done elsewhere
    case el_graph :
    case el_hline :
    case el_vline :
        splittable = false;     
        break;
    case el_text :
        for( cur_line = a_element->element.text.first; cur_line != NULL;
                                cur_line = cur_line->next ) {
            if( (cur_depth + cur_line->line_height) > req_depth ) {
                break;
            }
            count++;
            cur_depth += cur_line->line_height;
            last = cur_line;
        }

        if( cur_line != NULL ) {    // at least one more line
            if( count < g_cur_threshold ) {
                splittable = false;     // widow criteria failed
                a_element->blank_lines = 0;
                break;
            }
        }

        if( last == NULL ) {        // all lines fit; unlikely, but seen
            break;
        }

        /*  if we get here, a_element is splittable, cur_line is    */
        /*  the first line of the new element, and last is the last */
        /*  line that can be left in the original element           */

        split_el = alloc_doc_el( el_text ); // most defaults are correct

        split_el->depth = a_element->depth - cur_depth;
        split_el->element.text.first = cur_line;
        last->next = NULL;
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
/*  output all pages in which t_page.main->main is full                    */
/*  t_page will not be empty but n_page.col_main will be empty on return   */
/***************************************************************************/

void text_page_out( void )
{
    while( n_page.col_main != NULL ) {
        do_page_out();
        update_t_page();
    }
    return;
}

