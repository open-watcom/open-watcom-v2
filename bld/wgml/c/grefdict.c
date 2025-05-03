/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2011 The Open Watcom Contributors. All Rights Reserved.
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
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*  get_refid_value        parse reference id                              */
/*                                                                         */
/*  Note: parameter refid should be a pointer to char[ID_LEN]              */
/***************************************************************************/

char * get_refid_value( char * p, char * refid )
{
    int k;

    p = get_att_value( p );

    if( val_start == NULL ) {       // no valid id
        return( p );
    }
    if( val_len < ID_LEN ) {
        for( k = 0; k < val_len; k++ ) {
            refid[k] = my_tolower( *(val_start + k) );
        }
        refid[val_len] = '\0';
    } else {
        for( k = 0; k < ID_LEN; k++ ) {
            refid[k] = my_tolower( *(val_start + k) );
        }
        refid[ID_LEN - 1] = '\0';
    }
    if( val_len > 7 ) {                     // wgml 4 warning level
        xx_warn_c_info( wng_id_xxx, refid, inf_id_len );
    }
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
/* init ref entry with some values    assumes :hx :fig variant             */
/***************************************************************************/

void init_ref_entry( ref_entry *re, char *id )
{
    re->next = NULL;
    strcpy( re->id, id );
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
    if( strlen( name ) > 0 ) {          // don't match case with no id
        for( curr = dict; curr != NULL; curr = curr->next ) {
            if( strcmp( curr->id, name ) == 0 ) {
                wk = curr;
                break;
            }
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
    ref_entry           *   wk;
    int                     cnt;
    int                     len;
    static  const   char    fill[17] = "                ";
    bool                    withnumber;
    char                *   text;
    cnt = 0;
    if( dict != NULL ) {
        out_msg( "\nList of %s entries:\n\n", type );
        if( strcmp( "INDEX", type ) != 0 ) {
            withnumber = ( strcmp( "HDREF", type ) != 0 );   // true for :FIG and :FN
            for( wk = dict; wk != NULL; wk = wk->next ) {
                len = strlen( wk->id );
                if( withnumber ) {
                    out_msg( "%4ld ID='%s'%spage %-3ld text='%s'\n", wk->u.ffh.entry->number,
                             wk->id, &fill[len], wk->u.ffh.entry->pageno, wk->u.ffh.entry->text );
                } else {
                    out_msg( "ID='%s'%spage %-3ld text='%s'\n", wk->id,
                            &fill[len], wk->u.ffh.entry->pageno, wk->u.ffh.entry->text );
                }
                cnt++;
            }
        } else {                        // "INDEX"
            for( wk = dict; wk != NULL; wk = wk->next ) {
                len = strlen( wk->id );
                if( wk->u.ix.hblk->prt_term != NULL ) {
                    text = wk->u.ix.hblk->prt_term;
                } else {
                    text = wk->u.ix.hblk->ix_term;
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

