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
* Description:  Implements reference dictionary functions
*                   for :Hx, :HDREF, :FIG, :FN, :FIGREF, :FNREF tags
*
*                   :FIG, :FN, :FIGREF, :FNREF tags not yet implemented TBD
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */

#include "wgml.h"
#include "gvars.h"


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

    if( *dict == NULL ) {           // empty dictionary
        *dict = re;
    } else {
        wk = *dict;
        while( wk->next != NULL ) { // search last entry in dictionary
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
        if( me->text_cap != NULL ) {
            mem_free( me->text_cap );
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
/*  if id length shorter than max fill with '\0'                           */
/***************************************************************************/
void fill_id( ref_entry * re, char * id, size_t len )
{

    strcpy_s( re->id, ID_LEN + 1, strlwr( id ) );
    if( len < ID_LEN ) {
        for( ; len <= ID_LEN; len++ ) {
            (re->id)[len] = '\0';
        }
    }
}


/***************************************************************************/
/* init ref entry with some values                                         */
/***************************************************************************/
void init_ref_entry( ref_entry * re, char * id, size_t len )
{

    re->next = NULL;
    re->text_cap = NULL;

    fill_id( re, id, len );

    if( input_cbs->fmflags & II_macro ) {
        re->lineno = input_cbs->s.m->lineno;
    } else {
        re->lineno = input_cbs->s.f->lineno;
    }
    re->number = 0;
    re->pageno = page;
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
/***************************************************************************/

void    print_ref_dict( ref_entry * dict, const char * type )
{
    ref_entry           *   wk;
    int                     cnt;
    int                     len;
    static  const   char    fill[17] = "                ";
    bool                    withnumber;

    cnt = 0;
    if( dict != NULL ) {
        withnumber = strcmp( "HDREF", type );   // true for :FIG and :FN
        out_msg( "\nList of %s entries:\n\n", type );
        for( wk = dict; wk != NULL; wk = wk->next ) {
            len = strlen( wk->id );
            if( withnumber ) {
                out_msg( "%4ld ID='%s'%spage %-3ld text='%s'\n", wk->number,
                         wk->id, &fill[len], wk->pageno, wk->text_cap );
            } else {
                out_msg( "ID='%s'%spage %-3ld text='%s'\n", wk->id,
                        &fill[len], wk->pageno, wk->text_cap );
            }
            cnt++;
        }
    }
    out_msg( "\nTotal %s entries: %d\n", type, cnt );
    return;
}

