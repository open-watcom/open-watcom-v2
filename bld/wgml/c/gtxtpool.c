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
* Description:  pool-handling functions for alloc / return to pool / reuse of
*               these structs:
*                   box_col_set
*                   box_col_stack
*                   doc_el_group
*                   doc_element
*                   eol_ix
*                   sym_list_entry
*                   tag_cb
*                   text_chars
*                   text_line
*               it also provides utility functions:
*                   init_doc_el() and clear_doc_element() to help with doc_elements
*                   free_pool_storage() to free all the allocated memory
*
* Note: this file was originally used for the text_chars and text_line structs,
*       hence the file name
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*  allocate / reuse and init a text_chars instance                        */
/*      optionally fill in text                                            */
/***************************************************************************/
text_chars *alloc_text_chars( const char *text, unsigned cnt, font_number font )
{
    text_chars   *   curr;
    text_chars   *   prev;

    for( curr = text_pool; (curr != NULL) && (curr->length <= cnt); curr = curr->next ) {
        prev = curr;
    }
    if( curr != NULL ) {                // we found one large enough
        if( curr == text_pool ) {       // first is large enough
            text_pool = curr->next;
        } else {
            prev->next = curr->next;    // unchain curr
        }
    } else {                            // no one large enough found
        curr = mem_alloc( sizeof( *curr ) + cnt );
        curr->length = cnt;             // set max text size

        if( text_pool == NULL ) {       // alloc 10 text_chars if pool empty
            int k;

            text_pool = mem_alloc( sizeof( *prev ) + TEXT_CHARS_DEF );
            prev = text_pool;
            for( k = 0; k < 10; k++ ) {
                prev->length = TEXT_CHARS_DEF;
                prev->next = mem_alloc( sizeof( *prev ) + TEXT_CHARS_DEF );
                prev = prev->next;
            }
            prev->next = NULL;
            prev->length = TEXT_CHARS_DEF;
        }
    }

    curr->prev = NULL;
    curr->next = NULL;
    curr->fmflags = 0;
    curr->post_ix = ProcFlags.post_ix;
    curr->pre_gap = false;
    curr->tab_align = ALIGN_left;
    curr->tab_pos = TAB_none;
    curr->ts_width = 0;
    curr->type = TXT_norm;
    curr->font = font;
    curr->f_switch = FSW_norm;
    curr->width = 0;
    if( text != NULL ) {                   // text supplied
        strncpy( curr->text, text, cnt ); // yes copy text
        curr->count = cnt;              // set current size
    } else {
        curr->count = 0;                // init current size
        curr->text[0] = '\0';
    }
    curr->text[cnt] = '\0';

    return( curr );
}


/***************************************************************************/
/*  add a single text_chars instance to the free pool for reuse            */
/*                                                                         */
/*  Note: very specialized, used to cancel a text_chars instance           */
/***************************************************************************/

void add_single_text_chars_to_pool( text_chars * a_chars )
{
    if( a_chars == NULL ) {
        return;
    }

    // free text_chars in pool only have a valid next ptr

    a_chars->next = text_pool;
    text_pool = a_chars;
}


/***************************************************************************/
/*  add text_chars instance(s) to free pool for reuse                      */
/***************************************************************************/

void add_text_chars_to_pool( text_line * a_line )
{
    text_chars      *   tw;

    if( (a_line == NULL) || (a_line->first == NULL) ) {
        return;
    }

    // free text_chars in pool only have a valid next ptr

    for( tw = a_line->first; tw->next != NULL; tw = tw->next ); //empty
    tw->next = text_pool;
    text_pool = a_line->first;

}


/***************************************************************************/
/*  allocate / reuse a text_line  instance                                 */
/***************************************************************************/

text_line * alloc_text_line( void )
{
    text_line   *   curr;
    text_line   *   prev;
    int             k;

    curr = line_pool;
    if( curr != NULL ) {                // there is one to use
        line_pool = curr->next;
    } else {                            // pool is empty
        curr = mem_alloc( sizeof( text_line ) );

        line_pool = mem_alloc( sizeof( *prev ) );
        prev = line_pool;
        for( k = 0; k < 10; k++ ) {     // alloc 10 text_lines if pool empty
            prev->next = mem_alloc( sizeof( *prev ) );
            prev = prev->next;
        }
        prev->next = NULL;
    }

    curr->eol_index = g_eol_ix;
    g_eol_ix = NULL;
    curr->first = NULL;
    curr->last = NULL;
    curr->units_spacing = g_units_spacing;
    curr->line_height = 0;
    curr->next = NULL;
    curr->y_address = 0;

    return( curr );
}


/***************************************************************************/
/*  add a text_line instance to free pool for reuse                        */
/***************************************************************************/

void add_text_line_to_pool( text_line * a_line )
{
    if( a_line == NULL ) {
        return;
    }

    a_line->next = line_pool;
    line_pool = a_line;
}


/***************************************************************************/
/*  allocate / reuse a box_col_set instance                                */
/***************************************************************************/

box_col_set * alloc_box_col_set( void )
{
    box_col_set *   curr;
    int             k;

    if( box_col_set_pool == NULL ) {                // pool is empty
        box_col_set_pool = mem_alloc( sizeof( *curr ) );
        curr = box_col_set_pool;
        curr->current = 0;
        curr->length = BOXCOL_COUNT;
        curr->cols = mem_alloc( BOXCOL_COUNT * sizeof( box_col_spec ));
        for( k = 0; k < 10; k++ ) {     // alloc 10 box_col_sets if pool empty
            curr->next = mem_alloc( sizeof( *curr ) );
            curr = curr->next;
            curr->current = 0;
            curr->length = BOXCOL_COUNT;
            curr->cols = mem_alloc( BOXCOL_COUNT * sizeof( box_col_spec ));
        }
        curr->next = NULL;
    }
    curr = box_col_set_pool;
    box_col_set_pool = curr->next;
    curr->next = NULL;
    curr->current = 0;                  // clear before returning
    memset( curr->cols, 0, curr->length * sizeof( box_col_spec ));

    return( curr );
}


/***************************************************************************/
/*  add a linked list of box_col_set instances to free pool for reuse      */
/***************************************************************************/

void add_box_col_set_to_pool( box_col_set * a_set )
{
    box_col_set * tw;

    if( a_set == NULL ) {
        return;
    }

    for( tw = a_set; tw->next != NULL; tw = tw->next ); //empty
    tw->next = box_col_set_pool;
    box_col_set_pool = a_set;
}


/***************************************************************************/
/*  allocate / reuse a box_col_stack instance                              */
/***************************************************************************/

box_col_stack * alloc_box_col_stack( void )
{
    box_col_stack   *   curr;
    int                 k;

    if( box_col_stack_pool == NULL ) {                // pool is empty
        box_col_stack_pool = mem_alloc( sizeof( *curr ) );
        curr = box_col_stack_pool;
        curr->first = NULL;
        for( k = 0; k < 10; k++ ) {     // alloc 10 box_col_sets if pool empty
            curr->next = mem_alloc( sizeof( *curr ) );
            curr = curr->next;
            curr->first = NULL;
        }
        curr->next = NULL;
    }
    curr = box_col_stack_pool;
    box_col_stack_pool = curr->next;
    curr->next = NULL;
    curr->first = NULL;                 // clear before returning
    curr->had_cols = false;
    curr->inner_box = false;

    return( curr );
}


/***************************************************************************/
/*  add a linked list of box_col_stack instances to free pool for reuse    */
/***************************************************************************/

void add_box_col_stack_to_pool( box_col_stack * a_stack )
{
    box_col_stack * tw;

    if( a_stack == NULL ) {
        return;
    }

    for( tw = a_stack; tw->next != NULL; tw = tw->next ); //empty
    tw->next = box_col_stack_pool;
    box_col_stack_pool = a_stack;
}


/***************************************************************************/
/*  allocate / reuse a doc_element instance                                */
/*  Note: called directly in a few cases, but mostly from init_doc_el()    */
/*        this produces a doc_element with default values                  */
/***************************************************************************/

doc_element * alloc_doc_el( element_type type )
{
    doc_element *   curr;
    int             k;

    if( doc_el_pool == NULL ) {         // pool is empty
        doc_el_pool = mem_alloc( sizeof( *curr ) );
        curr = doc_el_pool;
        for( k = 0; k < 10; k++ ) {     // alloc 10 doc_els if pool empty
            curr->next = mem_alloc( sizeof( *curr ) );
            curr = curr->next;
        }
        curr->next = NULL;
    }

    curr = doc_el_pool;
    doc_el_pool = curr->next;
    curr->next = NULL;
    curr->blank_lines = 0;
    curr->depth = 0;
    curr->subs_skip = 0;
    curr->top_skip = 0;
    curr->type = type;
    curr->h_pos = 0;
    curr->v_pos = 0;
    curr->do_split = ProcFlags.sk_co;           // used to split doc_el_groups
    curr->in_xmp = (cur_group_type == GRT_xmp);  // used by BX
    curr->op_co_on = ProcFlags.concat;          // used with FK output
    ProcFlags.sk_co = false;

    switch( type ) {
    case ELT_binc:
        curr->element.binc.cur_left = 0;
        curr->element.binc.depth = 0;
        curr->element.binc.y_address = 0;
        curr->element.binc.at_top = false;
        curr->element.binc.has_rec_type = false;
        curr->element.binc.file = NULL;
        curr->element.binc.fp = NULL;
        curr->element.binc.eol_index = g_eol_ix;
        g_eol_ix = NULL;
        break;
    case ELT_dbox:
        curr->element.dbox.h_start = 0;
        curr->element.dbox.v_start = 0;
        curr->element.dbox.h_len = 0;
        curr->element.dbox.v_len = 0;
        curr->element.dbox.eol_index = g_eol_ix;
        g_eol_ix = NULL;
        break;
    case ELT_graph:
        curr->element.graph.cur_left = 0;
        curr->element.graph.depth = 0;
        curr->element.graph.scale = 0;
        curr->element.graph.width = 0;
        curr->element.graph.y_address = 0;
        curr->element.graph.xoff = 0;
        curr->element.graph.yoff = 0;
        curr->element.graph.at_top = false;
        curr->element.graph.next_font = FONT0;
        curr->element.graph.short_name = NULL;
        curr->element.graph.file = NULL;
        curr->element.graph.fp = NULL;
        curr->element.graph.eol_index = g_eol_ix;
        g_eol_ix = NULL;
        break;
    case ELT_hline:
        curr->element.hline.h_start = 0;
        curr->element.hline.v_start = 0;
        curr->element.hline.h_len = 0;
        curr->element.hline.ban_adjust = false;
        curr->element.hline.eol_index = g_eol_ix;
        g_eol_ix = NULL;
        break;
    case ELT_text:
        curr->element.text.prev = NULL;
        curr->element.text.entry = NULL;
        curr->element.text.ref = NULL;
        curr->element.text.first = NULL;
        curr->element.text.bx_h_done = false;
        curr->element.text.force_op = false;
        curr->element.text.overprint = false;
        curr->element.text.vspace_next = false;
        break;
    case ELT_vline:
        curr->element.vline.h_start = 0;
        curr->element.vline.v_start = 0;
        curr->element.vline.v_len = 0;
        curr->element.vline.twice = true;
        curr->element.vline.eol_index = g_eol_ix;
        g_eol_ix = NULL;
        break;
    case ELT_vspace:
        curr->element.vspace.font = FONT0;
        curr->element.vspace.eol_index = g_eol_ix;
        g_eol_ix = NULL;
        break;
    default :
        internal_err_exit( __FILE__, __LINE__ );
        /* never return */
    }

    return( curr );
}


/***************************************************************************/
/*  add a doc_element instance to free pool for reuse                      */
/***************************************************************************/

void add_doc_el_to_pool( doc_element * a_element )
{
    if( a_element == NULL ) {
        return;
    }

    clear_doc_element( a_element );
    a_element->next = doc_el_pool;
    doc_el_pool = a_element;
    return;
}


/***************************************************************************/
/*  allocate / reuse a doc_el_group instance                                     */
/***************************************************************************/

doc_el_group * alloc_doc_el_group( group_type type )
{
    doc_el_group    *   curr;
    int             k;

    if( doc_el_group_pool != NULL ) {   // there is one to use
        curr = doc_el_group_pool;
        doc_el_group_pool = curr->next;
    } else {                            // pool is empty
        curr = mem_alloc( sizeof( doc_el_group ) );
        doc_el_group_pool = curr;
        for( k = 0; k < 10; k++ ) {     // alloc 10 box_col_sets if pool empty
            curr->next = mem_alloc( sizeof( *curr ) );
            curr = curr->next;
        }
        curr->next = NULL;
        curr = doc_el_group_pool;
        doc_el_group_pool = curr->next;
    }
    curr->next = NULL;
    curr->depth = 0;
    curr->post_skip = 0;
    curr->first = NULL;
    curr->last = NULL;
    curr->owner = type;
    curr->block_font = FONT0;

    return( curr );
}


/***************************************************************************/
/*  add a doc_el_group instance to free pool for reuse                           */
/***************************************************************************/

void add_doc_el_group_to_pool( doc_el_group * a_group )
{
    doc_element *   cur_el;

    if( a_group == NULL ) {
        return;
    }

    while( a_group->first != NULL ) {
        cur_el = a_group->first;
        a_group->first = a_group->first->next;
        cur_el->next = NULL;
        add_doc_el_to_pool( cur_el );
    }

    a_group->next = doc_el_group_pool;
    doc_el_group_pool = a_group;
    return;
}


/***************************************************************************/
/*  allocate / reuse an eol_ix instance                                    */
/***************************************************************************/

eol_ix * alloc_eol_ix( ix_h_blk * in_ixh, ereftyp in_type )
{
    eol_ix      *   curr;
    eol_ix      *   prev;
    int             k;

    curr = eol_ix_pool;
    if( curr != NULL ) {                // there is one to use
        eol_ix_pool = curr->next;
    } else {                            // pool is empty
        curr = mem_alloc( sizeof( eol_ix ) );

        eol_ix_pool = mem_alloc( sizeof( *prev ) );
        prev = eol_ix_pool;
        for( k = 0; k < 10; k++ ) {     // alloc 10 eol_ix instances if pool empty
            prev->next = mem_alloc( sizeof( *prev ) );
            prev = prev->next;
        }
        prev->next = NULL;
    }

    curr->next = NULL;
    curr->ixh = in_ixh;
    curr->type = in_type;

    return( curr );
}

/***************************************************************************/
/*  add an eol_ix instance to free pool for reuse                          */
/***************************************************************************/

void add_eol_ix_to_pool( eol_ix * an_eol_ix )
{
    if( an_eol_ix == NULL ) {
        return;
    }

    an_eol_ix->next = eol_ix_pool;
    eol_ix_pool = an_eol_ix;
}


/***************************************************************************/
/*  allocate / reuse a tag_cb instance                                     */
/*  Note: init_tag_cb() initializes the new instance                       */
/***************************************************************************/

tag_cb  * alloc_tag_cb( void )
{
    tag_cb  *   curr;
    tag_cb  *   prev;
    int         k;

    curr = tag_pool;
    if( curr != NULL ) {                // there is one to use
        tag_pool = curr->prev;
    } else {                            // pool is empty
        curr = mem_alloc( sizeof( tag_cb ) );

        prev = mem_alloc( sizeof( *prev ) );
        tag_pool = prev;
        for( k = 0; k < 10; k++ ) { // alloc 10 tag_cb if pool empty
            prev->prev = mem_alloc( sizeof( *prev ) );
            prev = prev->prev;
        }
        prev->prev = NULL;
    }

    return( curr );
}


/***************************************************************************/
/*  add a tag_cb instance to free pool for reuse                           */
/***************************************************************************/

void    add_tag_cb_to_pool( tag_cb * a_cb )
{
    nest_stack  *   ns;
    nest_stack  *   nsv;

    if( a_cb == NULL ) {        // nothing to do
        return;
    }

    if( a_cb == tt_stack ) {    // tt_stack will be invalid after a_cb is processedf
        tt_stack = NULL;
    }

    g_text_spacing = a_cb->spacing;         // reset spacing to prior value
    for( ns = a_cb->p_stack; ns != NULL; ns = nsv ) {
        nsv = ns->prev;
        mem_free( ns );
    }
    a_cb->prev = tag_pool;
    tag_pool = a_cb;
}


/***************************************************************************/
/*  allocate / reuse a sym_list_entry instance                             */
/***************************************************************************/

sym_list_entry * alloc_sym_list_entry( void )
{
    sym_list_entry  *   curr;
    sym_list_entry  *   prev;
    int                 k;

    curr = sym_list_pool;
    if( curr != NULL ) {                // there is one to use
        sym_list_pool = curr->prev;
    } else {                            // pool is empty
        curr = mem_alloc( sizeof( sym_list_entry ) );

        prev = mem_alloc( sizeof( *prev ) );
        sym_list_pool = prev;
        for( k = 0; k < 10; k++ ) { // alloc 10 tag_cb if pool empty
            prev->prev = mem_alloc( sizeof( *prev ) );
            prev = prev->prev;
        }
        prev->prev = NULL;
    }

    curr->prev = NULL;
    curr->value[0] = '\0';
    curr->orig.s = 0;
    curr->orig.e = 0;
    curr->type = SL_none;

    return( curr );
}


/***************************************************************************/
/*  add a sym_list_entry instance to free pool for reuse                   */
/***************************************************************************/

void add_sym_list_entry_to_pool( sym_list_entry * a_sl )
{
    sym_list_entry  *   curr;

    if( a_sl == NULL ) {
        return;
    }

    curr = a_sl;
    while( curr->prev != NULL)
        curr = curr->prev;
    curr->prev = sym_list_pool;
    sym_list_pool = a_sl;
}


/***************************************************************************/
/*  free all elements of our storage pools                                 */
/***************************************************************************/

void    free_pool_storage( void )
{
    void    *   v;
    void    *   wv;

    for( v = text_pool; v != NULL; ) {
        wv = ( (text_chars *) v)->next;
        mem_free( v );
        v = wv;
    }

    for( v = line_pool; v != NULL; ) {
        wv = ( (text_line *) v)->next;
        mem_free( v );
        v = wv;
    }

    for( v = box_col_set_pool; v != NULL; ) {
        if( ((box_col_set *) v)->cols != NULL ) {
            mem_free( (box_col_spec *) ((box_col_set *) v)->cols );
        }
        wv = ( (box_col_set *) v)->next;
        mem_free( v );
        v = wv;
    }

    for( v = box_col_stack_pool; v != NULL; ) {
        wv = ( (box_col_stack *) v)->next;
        mem_free( v );
        v = wv;
    }

    for( v = doc_el_pool; v != NULL; ) {
        wv = ( (doc_element *) v)->next;
        mem_free( v );
        v = wv;
    }

    for( v = doc_el_group_pool; v != NULL; ) {
        wv = ( (doc_el_group *) v)->next;
        mem_free( v );
        v = wv;
    }

    for( v = eol_ix_pool; v != NULL; ) {
        wv = ( (eol_ix *) v)->next;
        mem_free( v );
        v = wv;
    }

    for( v = fwd_ref_pool; v != NULL; ) {
        wv = ( (fwd_ref *) v)->next;
        mem_free( v );
        v = wv;
    }

    for( v = sym_list_pool; v != NULL; ) {
        wv = ( (sym_list_entry *) v)->prev;
        mem_free( v );
        v = wv;
    }

    for( v = tag_pool; v != NULL; ) {
        wv = ( (tag_cb *) v)->prev;
        mem_free( v );
        v = wv;
    }
}

/***************************************************************************/
/*  clear a doc_element                                                    */
/*  this is a separate function because t_element is not a pointer         */
/***************************************************************************/

void clear_doc_element( doc_element * a_element )
{
    doc_element *   cur_el;
    text_line   *   cur_line;
    text_line   *   save;

    for( cur_el = a_element; cur_el != NULL; cur_el = cur_el->next ) {
        switch( cur_el->type ) {
        case ELT_dbox:
        case ELT_hline:
        case ELT_vline:
        case ELT_vspace:
            break;      // should be nothing to do
        case ELT_binc:
        case ELT_graph:
            // TODO! close files and free allocated memory
            break;
        case ELT_text:
            cur_line = cur_el->element.text.first;
            while( cur_line != NULL ) {
                add_text_chars_to_pool( cur_line );
                save = cur_line->next;
                add_text_line_to_pool( cur_line );
                cur_line = save;
            }
            break;
        default :
            internal_err_exit( __FILE__, __LINE__ );
            /* never return */
        }
    }

    return;
}


/***************************************************************************/
/*  initalize a doc_element instance, which it obtains from alloc_doc_el() */
/***************************************************************************/

doc_element * init_doc_el( element_type type, unsigned depth )
{
    doc_element *   curr;

    curr = alloc_doc_el( type );

    curr->depth = depth;
    curr->blank_lines = g_blank_units_lines;
    g_blank_units_lines = 0;
    curr->subs_skip = g_subs_skip;
    g_subs_skip = 0;
    curr->top_skip = g_top_skip;
    g_top_skip = 0;

    if( type == ELT_text ) {             // overprint applies to text lines
        curr->element.text.overprint = ProcFlags.overprint;
        ProcFlags.overprint = false;
    } else if( type == ELT_vspace ) {    // the font is needed for post-BINCLUDE processing
        curr->element.vspace.font = g_curr_font;
    }

    ProcFlags.skips_valid = false;

    return( curr );
}


