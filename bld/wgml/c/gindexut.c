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
* Description: WGML helper functions for index processing
*                   for  .ix control word
*                   and  :I1 :I2 :I3 :IH1 :IH2 :IH3 :IREF tags
*
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


/***************************************************************************/
/*  find an index item number reference in index_dict                      */
/***************************************************************************/

static bool find_num_ref( ix_e_blk **base, unsigned page_nr )
{
    bool            retval;
    ix_e_blk        *cur_ieh;
    ix_e_blk        *old_ieh;

    retval = false;
    old_ieh = NULL;                     // will hold entry to insert after
    for( cur_ieh = *base; cur_ieh != NULL; cur_ieh = cur_ieh->next ) {
        if( page_nr < cur_ieh->u.pagenum.page_no ) {
            *base = old_ieh;            // use old_ixh as insert point
            break;                      // entry found, and is in *entry
        }
        if( page_nr == cur_ieh->u.pagenum.page_no ) {
            *base = cur_ieh;            // use cur_ixh as insert point
            retval = true;
            break;                      // entry found, and is in *entry
        }
        old_ieh = cur_ieh;
    }

    if( cur_ieh == NULL ) {             // insert at end of list
        *base = old_ieh;                // use old_ixh as insert point
    }

    return( retval );
}

/***************************************************************************/
/*  find an index item string reference in index_dict                      */
/***************************************************************************/

static bool find_string_ref( char * ref, unsigned len, ix_e_blk * * base )
{
    bool            retval;
    unsigned        comp_len;           // compare length for searching existing entries
    int             comp_res;           // compare result
    ix_e_blk    *   cur_ieh;
    ix_e_blk    *   old_ieh;

    retval = false;
    old_ieh = NULL;                     // will hold entry to insert after
    for( cur_ieh = *base; cur_ieh != NULL; cur_ieh = cur_ieh->next ) {
        comp_len = len;
        if( comp_len > cur_ieh->u.pageref.page_text_len ) {
            comp_len = cur_ieh->u.pageref.page_text_len;
        }
        comp_res = strnicmp( ref, cur_ieh->u.pageref.page_text, len );
        if( comp_res < 0 ) {
            *base = old_ieh;            // use old_ixh as insert point
            break;                      // entry found, and is in *entry
        }
        if( comp_res == 0 ) {           // is equal
            if( len == cur_ieh->u.pageref.page_text_len ) {
                *base = cur_ieh;        // use cur_ixh as insert point
                retval = true;
                break;                  // entry found, and is in *entry
            }
            if( len < cur_ieh->u.pageref.page_text_len ) { // shouldn't be possible
                internal_err_exit( __FILE__, __LINE__ );
                /* never return */
            }
        }
        old_ieh = cur_ieh;
    }

    if( cur_ieh == NULL ) {             // insert at end of list
        *base = old_ieh;                // use old_ixh as insert point
    }

    return( retval );
}


/***************************************************************************/
/*  find an index item in index_dict                                       */
/***************************************************************************/

static bool find_index_item( char *item, unsigned len, ix_h_blk **entry )
{
    bool            retval;
    unsigned        comp_len;           // compare length for searching existing entries
    int             comp_res;           // compare result
    ix_h_blk    *   cur_ixh;
    ix_h_blk    *   old_ixh;

    retval = false;
    old_ixh = NULL;                    // will hold entry to insert after
    for( cur_ixh = *entry; cur_ixh != NULL; cur_ixh = cur_ixh->next ) {
        comp_len = len;
        if( comp_len > cur_ixh->ix_term_len ) {
            comp_len = cur_ixh->ix_term_len;
        }
        comp_res = strnicmp( item, cur_ixh->ix_term, len );
        if( comp_res < 0 ) {            // new is earlier in alphabet
            *entry = old_ixh;           // use old_ixh as insert point
            break;                      // entry found, and is in *entry
        }
        if( comp_res == 0 ) {           // is equal
            if( len == cur_ixh->ix_term_len ) {
                *entry = cur_ixh;       // use cur_ixh as insert point
                retval = true;
                break;                  // entry found, and is in *entry
            }
            if( len < cur_ixh->ix_term_len ) {
                *entry = old_ixh;       // use old_ixh as insert point
                break;                  // entry found, and is in *entry
            }
        }
        old_ixh = cur_ixh;
    }

    if( cur_ixh == NULL ) {             // insert at end of list
        *entry = old_ixh;               // use old_ixh as insert point
    }

    return( retval );
}


/***************************************************************************/
/*  find or create/insert new index reference entry                        */
/***************************************************************************/

void find_create_ix_e_entry( ix_h_blk *ixhwork, char *ref, unsigned len,
                             ix_h_blk *seeidwork, ereftyp type )
{
    bool                found       = true;
    eol_ix      *       cur_eol;
    ix_e_blk    *   *   base;
    ix_e_blk    *       ixework;
    ix_e_blk    *       ixewk;
    text_line   *       cur_tl;

    ixework = NULL;
    switch( type ) {
    case PGREF_major :
    case PGREF_pageno :
    case PGREF_start :
    case PGREF_end :
        if( (t_line == NULL) && (t_element == NULL) ) {

            /**********************************************************/
            /*  if there is no pending text, then add this entry to   */
            /*  g_eol_ix so that it can be assigned properly in       */
            /*  attach_eol() (used by scr_process_break())            */
            /*                                                        */
            /*  NOTE: flag "found" is borrowed from the code that     */
            /*        determines whether to create a new ix_e_block   */
            /*        its usage here may appear confusing             */
            /**********************************************************/

            if( g_eol_ix == NULL ) {
                g_eol_ix = alloc_eol_ix( ixhwork, type );
                cur_eol = g_eol_ix;
            } else {
                cur_eol = g_eol_ix;
                while( cur_eol->next != NULL ) {
                    cur_eol = cur_eol->next;        // append at end of list
                }
                cur_eol->next = alloc_eol_ix( ixhwork, type );
            }
        } else {
            if( t_line != NULL ) {
                if( t_line->eol_index == NULL ) {
                    t_line->eol_index = alloc_eol_ix( ixhwork, type );
                    found = false;
                }
                cur_eol = t_line->eol_index;
            } else if( t_element != NULL ) {
                cur_tl = t_element->element.text.first;
                while( cur_tl->next != NULL ) {
                    cur_tl = cur_tl->next;    // find last text_line
                }
                if( cur_tl->eol_index == NULL ) {
                    cur_tl->eol_index = alloc_eol_ix( ixhwork, type );
                    found = false;
                }
                cur_eol = cur_tl->eol_index;
            } else {
                internal_err_exit( __FILE__, __LINE__ ); // bad element type value
                /* never return */
            }
            if( found ) {
                while( cur_eol->next != NULL ) {
                    cur_eol = cur_eol->next;        // append at end of list
                }
                cur_eol->next = alloc_eol_ix( ixhwork, type );
            } else {
                found = true;
            }
        }
        break;
    case PGREF_majorstring :
        base = &ixhwork->entry->major_string;
        ixework = ixhwork->entry->major_string;
        found = find_string_ref( ref, len, &ixework );
        break;
    case PGREF_string :
        base = &ixhwork->entry->normal_string;
        ixework = ixhwork->entry->normal_string;
        found = find_string_ref( ref, len, &ixework );
        break;
    case PGREF_see :
        base = &ixhwork->entry->see_string;
        ixework = ixhwork->entry->see_string;
        if( (seeidwork != 0) && ixhwork->prt_term_len > 0 ) {   // insert per seeid->ix_term, display prt_term
            found = find_string_ref( seeidwork->ix_term, seeidwork->ix_term_len, &ixework );
        } else {
            found = find_string_ref( ref, len, &ixework );
        }
        break;
    case PGREF_none :       // should never appear here, but nothing to do
    default :           // out-of-range enum value
        internal_err_exit( __FILE__, __LINE__ );
        /* never return */
    }

    if( found ) {

        /* Item found and ixework points to it */

        ixewk = ixework;

    } else {                            // create block

        /* Item not found and ixework points to insertion point */

        ixewk = mem_alloc( sizeof( ix_e_blk ) );
        ixewk->next = NULL;
        ixewk->entry_typ = type;
        ixewk->prt_text = NULL;
        if( (seeidwork != NULL) && (seeidwork->prt_term_len > 0) ) {
            ixewk->prt_text = mem_tokdup( seeidwork->prt_term, seeidwork->prt_term_len );
        }
        ixewk->u.pageref.page_text = mem_tokdup( ref, len );
        ixewk->u.pageref.page_text_len = len;

        if( *base == NULL ) {
            if( ixework != NULL ) {         // displace prior reference list head
                ixewk->next = ixework;
                *base = ixewk;
            } else {                        // new reference list head
                *base = ixewk;
            }
        } else {                            // insert in list at current point
            if( ixework != NULL ) {         // displace prior reference list head
                ixewk->next  = ixework->next;
                ixework->next = ixewk;
            } else {                        // new referhence list head
                ixewk->next = *base;
                *base = ixewk;
            }
        }
    }
    return;
}

/***************************************************************************/
/*  find or create/insert new index reference entry (at eol)               */
/***************************************************************************/

void eol_index_page( eol_ix * eol_index, unsigned page_nr )
{
    bool                found;
    eol_ix      *       save;
    ix_e_blk    *   *   base;
    ix_e_blk    *       ixework;
    ix_e_blk    *       ixewk;

    ixework = NULL;
    while( eol_index != NULL ) {
        switch( eol_index->type ) {
        case PGREF_major :
            base = &eol_index->ixh->entry->major_pgnum;
            ixework = eol_index->ixh->entry->major_pgnum;
            find_num_ref( &ixework, page_nr );
            found = false;      // allow multiple entries of same page number
            break;
        case PGREF_pageno :
            base = &eol_index->ixh->entry->normal_pgnum;
            ixework = eol_index->ixh->entry->normal_pgnum;
            found = find_num_ref( &ixework, page_nr );
            break;
        case PGREF_start :
        case PGREF_end :
            base = &eol_index->ixh->entry->normal_pgnum;
            ixework = eol_index->ixh->entry->normal_pgnum;
            found = find_num_ref( &ixework, page_nr );
            if( found ) {           // ensure correct type
                ixework->entry_typ = eol_index->type;
            }
            break;
        case PGREF_majorstring :// should never appear used here, error
        case PGREF_string :
        case PGREF_see :
        case PGREF_none :
        default :           // out-of-range enum value
            internal_err_exit( __FILE__, __LINE__ );
            /* never return */
        }

        if( found ) {

            /* Item found and ixework points to it */

            ixewk = ixework;

        } else {                            // create block

            /* Item not found and ixework points to insertion point */

            ixewk = mem_alloc( sizeof( ix_e_blk ) );
            ixewk->next = NULL;
            ixewk->entry_typ = eol_index->type;
            ixewk->prt_text = NULL;
            ixewk->u.pagenum.page_no = page_nr;
            ixewk->u.pagenum.style = find_pgnum_style();

            if( *base == NULL ) {
                if( ixework != NULL ) {         // displace prior reference list head
                    ixewk->next = ixework;
                    *base = ixewk;
                } else {                        // new reference list head
                    *base = ixewk;
                }
            } else {                            // insert in list at current point
                if( ixework != NULL ) {         // displace prior reference list head
                    ixewk->next  = ixework->next;
                    ixework->next = ixewk;
                } else {                        // new reference list head
                    ixewk->next = *base;
                    *base = ixewk;
                }
            }
        }
        save = eol_index;
        eol_index = eol_index->next;
        save->next = NULL;
        add_eol_ix_to_pool( save );
    }
    return;
}

/***************************************************************************/
/*  find or create/insert new index header block                           */
/*  returns created block                                                  */
/***************************************************************************/

ix_h_blk * find_create_ix_h_entry( ix_h_blk *ixhwork, ix_h_blk *ixhbase,
                                   char *printtxt, unsigned printtxtlen,
                                   char *txt, unsigned txtlen, int lvl )
{
    ix_h_blk    *   ixhwk;

    if( find_index_item( txt, txtlen, &ixhwork ) ) {

        /* Item found and ixhwork points to it */

        ixhwk = ixhwork;

        /* Replace print term text if one is given and it differs at all */

        if( printtxt != NULL ) {
            if( ixhwork->prt_term == NULL ) {
                ixhwork->prt_term = mem_tokdup( printtxt, printtxtlen );
                ixhwork->prt_term_len = printtxtlen;
            } else if( ixhwork->prt_term_len < printtxtlen ) {
                mem_free( ixhwork->prt_term );
                ixhwork->prt_term = mem_tokdup( printtxt, printtxtlen );
                ixhwork->prt_term_len = printtxtlen;
            } else if( printtxtlen == 0 ) {
                ixhwork->prt_term[0] = '\0';
            } else if( ixhwork->prt_term != printtxt ) {
                strcpy( ixhwork->prt_term, printtxt );
            }
        }
    } else {                            // create block

        /* Item not found and ixhwork points to insertion point */

        ixhwk = mem_alloc( sizeof( ix_h_blk ) );
        ixhwk->next  = NULL;
        ixhwk->lower = NULL;
        ixhwk->entry = NULL;
        ixhwk->ix_lvl = lvl;
        ixhwk->ix_term = mem_tokdup( txt, txtlen );
        ixhwk->ix_term_len = txtlen;
        if( printtxt != NULL ) {
            ixhwk->prt_term_len = printtxtlen;
            ixhwk->prt_term = printtxt;
            printtxt = NULL;
        } else {
            ixhwk->prt_term_len = 0;
            ixhwk->prt_term = NULL;
        }
        if( ixhwork == NULL ) {
            if( lvl == 0 ) {                    // topmost list
                if( ixhbase != NULL ) {         // displace prior index_dict head
                    ixhwk->next  = ixhbase;
                    ixhwork = ixhwk;
                    index_dict = ixhwk;
                } else {                        // new head of index_dict
                    ixhwk->next = index_dict;
                    ixhwork = ixhwk;
                    index_dict = ixhwk;
                }
            } else {                            // sub-list
                if( ixhbase != NULL ) {         // new head of sub-list
                    ixhwk->next  = ixhbase->lower;
                    ixhbase->lower = ixhwk;
                } else {                        // cannot be NULL for sub-list
                    internal_err_exit( __FILE__, __LINE__ );
                    /* never return */
                }
            }
        } else {                                // insert in list at current point
            ixhwk->next  = ixhwork->next;
            ixhwork->next = ixhwk;
        }
    }
    return( ixhwk );
}


/***************************************************************************/
/*  free ix_e_blk chain                                                    */
/***************************************************************************/

static void free_ix_e_entries( ix_e_blk * e )
{
    ix_e_blk    *   ewk;
    ix_e_blk    *   ew  = e;

    while( ew != NULL ) {
        ewk = ew->next;
        if( ew->entry_typ >= PGREF_string ) {
            mem_free( ew->u.pageref.page_text );
        }
        mem_free( ew );
        ew = ewk;
    }
}


/***************************************************************************/
/*  free entry block                                                       */
/***************************************************************************/

static  void    free_entry_block( entry_list * e )
{
    free_ix_e_entries( e->major_pgnum );
    free_ix_e_entries( e->major_string );
    free_ix_e_entries( e->normal_pgnum );
    free_ix_e_entries( e->normal_string );
    free_ix_e_entries( e->see_string );
    mem_free( e );
}


/***************************************************************************/
/*  free ix_h_blk   header entry                                           */
/***************************************************************************/

static  void    free_ix_h_entry( ix_h_blk * h )
{
    if( h->prt_term != NULL ) {         // free different display txt
        mem_free( h->prt_term );
    }
    mem_free( h->ix_term );             // free index term
    mem_free( h );                      // and the entry itself
}


/***************************************************************************/
/*  free all entries in index_dict                                         */
/***************************************************************************/

void    free_index_dict( ix_h_blk * * dict )
{
    ix_h_blk    *   ixh1;
    ix_h_blk    *   ixh2;
    ix_h_blk    *   ixh3;
    ix_h_blk    *   ixhw;

    ixh1 = *dict;
    while( ixh1 != NULL ) {             // level 1 entries

        if( ixh1->entry != NULL ) {
            free_entry_block( ixh1->entry );
        }

        ixh2 = ixh1->lower;
        while( ixh2 != NULL ) {         // level 2 entries
            if( ixh2->entry != NULL ) {
                free_entry_block( ixh2->entry );
            }

            ixh3 = ixh2->lower;
            while( ixh3 != NULL ) {     // level 3 entries
                if( ixh3->entry != NULL ) {
                    free_entry_block( ixh3->entry );
                }

                ixhw = ixh3->next;
                free_ix_h_entry( ixh3 );
                ixh3 = ixhw;
            }
            ixhw = ixh2->next;
            free_ix_h_entry( ixh2 );
            ixh2 = ixhw;
        }
        ixhw = ixh1->next;
        free_ix_h_entry( ixh1 );
        ixh1 = ixhw;
    }
    *dict = NULL;                       // dict is now empty
}


/***************************************************************************/
/*  allocate and initialize the entry list                                 */
/***************************************************************************/

void init_entry_list( ix_h_blk * term )

{
    term->entry = mem_alloc( sizeof( entry_list ) );
    term->entry->major_pgnum = NULL;
    term->entry->major_string = NULL;
    term->entry->normal_pgnum = NULL;
    term->entry->normal_string = NULL;
    term->entry->see_string = NULL;
    return;
}
