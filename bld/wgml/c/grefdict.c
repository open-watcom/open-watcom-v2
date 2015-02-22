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
* Description:  Implements reference dictionary and helper functions
*                   for :Hx, :HDREF, :FIG, :FN, :FIGREF, :FNREF tags
*
*                   :FIG, :FN, :FIGREF, :FNREF tags not yet implemented TBD
****************************************************************************/

#include "wgml.h"
#include "gvars.h"


/***************************************************************************/
/*  get_refid_value        parse reference id                              */
/***************************************************************************/

char * get_refid_value( char * p )
{
    char  * pa;
    char  * pe;
    int     len;
    char    c;

    p = get_att_value( p );

    if( val_start == NULL || val_len == 0 ) {   // no valid id
        return( p );
    }
    pa = val_start;
    while( is_id_char( *pa ) ) {
        pa++;
    }

    len = pa - val_start;
    pe = val_start + len;
    if( len > 7 ) {                     // wgml 4 warning level
        c = *pe;
        *pe = '\0';
        g_warn( wng_id_xxx, val_start );
        *pe = c;
        g_info( inf_id_len );
        file_mac_info();
        wng_count++;
    }

    /***************************************************************/
    /*  restrict the length to ID_LEN (15) in the hope that no     */
    /*  truncation occurs                                          */
    /*  wgml4 warns about ids of more than 7 chars, but processes  */
    /*  much longer ids                                  TBD       */
    /***************************************************************/

    val_len = len;       // restrict length
    if( val_len > ID_LEN )
        val_len = ID_LEN;
    *(val_start + val_len) = '\0';
    return( p );
}


/***************************************************************************/
/*  init_ref_dict   initialize dictionary pointer                          */
/***************************************************************************/

void    init_ref_dict( ref_entry * * dict )
{
    *dict = NULL;
    return;
}


/***************************************************************************/
/*  add_ref_entry   add ref entry to dictionary                            */
/***************************************************************************/

void    add_ref_entry( ref_entry * * dict, ref_entry * re )
{
    ref_entry   *   wk;

    if( *dict == NULL ) {               // empty dictionary
        *dict = re;
    } else {
        wk = *dict;
        while( wk->next != NULL ) {     // search last entry in dictionary
            wk = wk->next;
        }
        wk->next = re;
    }
}


/***************************************************************************/
/*  free_ref_entry  delete single refentry                                 */
/***************************************************************************/

static void    free_ref_entry( ref_entry * * dict, ref_entry * me )
{
    ref_entry   *   wk;
    ref_entry   *   wkn;

    if( me != NULL ) {
        if( (me->flags < rf_ix) && (me->u.info.text_cap != NULL) ) {
            mem_free( me->u.info.text_cap );
        }
        if( *dict == me ) {             // delete first entry
            *dict = me->next;
        } else {
            wk = *dict;
            while( wk != NULL ) {       // search the entry in refdictionary
                wkn = wk->next;
                if( wkn == me ) {
                    wk->next = me->next;// chain update
                    break;
                }
                wk = wkn;
            }
        }
        mem_free( me );                 // now the entry itself
    }
    return;
}


/***************************************************************************/
/*  free_ref_dict   free all ref dictionary entries                        */
/***************************************************************************/

void    free_ref_dict( ref_entry * * dict )
{
    ref_entry   *   wk;
    ref_entry   *   wkn;

    wk = *dict;
    while( wk != NULL ) {
        wkn = wk->next;
        free_ref_entry( dict, wk );
        wk = wkn;
    }
    *dict = NULL;                       // dictionary is empty
    return;
}

/***************************************************************************/
/*  copy lowercase id to ref entry                                         */
/*  if id length shorter than max pad with '\0'                            */
/***************************************************************************/
void fill_id( ref_entry * re, char * id, size_t len )
{
    int     k;

    if( len > ID_LEN )
        len = ID_LEN;
    for( k = 0; k < len; k++ ) {
        (re->id)[k] = tolower( *(id + k) );
    }
    if( len < ID_LEN ) {
        for( ; k < ID_LEN; k++ ) {
            (re->id)[k] = '\0';
        }
    } else {
        (re->id)[ID_LEN] = '\0';
    }
}


/***************************************************************************/
/* init ref entry with some values    assumes :hx :fig variant             */
/***************************************************************************/
void init_ref_entry( ref_entry * re, char * id, size_t len )
{

    re->next = NULL;
    re->u.info.text_cap = NULL;

    fill_id( re, id, len );

    if( input_cbs->fmflags & II_tag_mac ) {
        re->lineno = input_cbs->s.m->lineno;
    } else {
        re->lineno = input_cbs->s.f->lineno;
    }
    re->u.info.number = 0;
    if( ProcFlags.page_started ) {
        re->u.info.pageno = page;
    } else {
        re->u.info.pageno = page + 1;
    }
}


/***************************************************************************/
/*  search ref   entry in specified dictionary                             */
/*  name is assumed to be lowercase                                        */
/*  returns ptr to entry or NULL if not found                              */
/***************************************************************************/

ref_entry   * find_refid( ref_entry * dict, const char * name )
{
    ref_entry   *   wk;
    ref_entry   *   curr;

    wk = NULL;
    for( curr = dict; curr != NULL; curr = curr->next ) {
        if( !strcmp( curr->id, name ) ) {
            wk = curr;
            break;
        }
    }
    return( wk );
}


/***************************************************************************/
/*  print_ref_dict  output all ref dictionary entries                      */
/*  type can be HDREF, FIGREF, FNREF                                       */
/*  and INDEX                                                              */
/***************************************************************************/

void    print_ref_dict( ref_entry * dict, const char * type )
{
    ref_entry           *wk;
    int                 cnt;
    size_t              len;
    static const char   fill[17] = "                ";
    bool                withnumber;
    char                *text;

    cnt = 0;
    if( dict != NULL ) {
        out_msg( "\nList of %s entries:\n\n", type );
        if( strcmp( "INDEX", type ) ) {
            withnumber = strcmp( "HDREF", type );   // true for :FIG and :FN
            for( wk = dict; wk != NULL; wk = wk->next ) {
                len = strlen( wk->id );
                if( withnumber ) {
                    out_msg( "%4ld ID='%s'%spage %-3ld text='%s'\n", wk->u.info.number,
                             wk->id, &fill[len], wk->u.info.pageno, wk->u.info.text_cap );
                } else {
                    out_msg( "ID='%s'%spage %-3ld text='%s'\n", wk->id,
                            &fill[len], wk->u.info.pageno, wk->u.info.text_cap );
                }
                cnt++;
            }
        } else {                        // "INDEX"
            for( wk = dict; wk != NULL; wk = wk->next ) {
                len = strlen( wk->id );
                if( wk->u.refb.hblk->prt_term != NULL ) {
                    text = wk->u.refb.hblk->prt_term;
                } else {
                    text = wk->u.refb.hblk->ix_term;
                }
                out_msg( "ID='%s'%s'%s'\n", wk->id,
                         &fill[len], text );
                cnt++;
            }
        }
    }
    out_msg( "\nTotal %s entries: %d\n", type, cnt );
    return;
}

