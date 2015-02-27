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
* Description:  Implements script macros (tables and access routines)
*
****************************************************************************/

#include "wgml.h"



/***************************************************************************/
/*  init_macro_dict   initialize dictionary pointer                        */
/***************************************************************************/

void    init_macro_dict( mac_entry * * dict )
{
    *dict = NULL;
    return;
}


/***************************************************************************/
/*  add_macro_entry   add macro entry to dictionary                        */
/***************************************************************************/

void    add_macro_entry( mac_entry * * dict, mac_entry * me )
{
    mac_entry   *   wk;

    if( *dict == NULL ) {           // empty dictionary
        *dict = me;
    } else {
        wk = *dict;
        while( wk->next != NULL ) { // search last entry in dictionary
            wk = wk->next;
        }
        wk->next = me;
    }
}


/***************************************************************************/
/*  free_macro_entry_short  free storage for a macro entry                 */
/*  without chain update                                                   */
/***************************************************************************/

static  void    free_macro_entry_short( mac_entry * me )
{
    inp_line    *   ml;
    inp_line    *   mln;
    labelcb     *   cb;

    if( me != NULL ) {
        cb = me->label_cb;
        if( GlobalFlags.research ) {
            print_labels( cb, me->name );   // print label info
        }
        while( cb != NULL ) {
            me->label_cb = cb->prev;
            mem_free( cb );
            cb = me->label_cb;
        }
        ml = me->macline;
        while( ml != NULL ) {           // free all macro lines
            mln = ml->next;
            mem_free( ml );
            ml = mln;
        }
        mem_free( me );                 // now the entry itself
    }
    return;
}

/***************************************************************************/
/*  free_macro_entry  delete single macroentry with chain update           */
/***************************************************************************/
void    free_macro_entry( mac_entry * * dict, mac_entry * me )
{
    inp_line    *   ml;
    inp_line    *   mln;
    mac_entry   *   wk;
    mac_entry   *   wkn;
    labelcb     *   cb;

    if( me != NULL ) {
        cb = me->label_cb;
        if( GlobalFlags.research ) {
            print_labels( cb, me->name );// print label info
        }
        while( cb != NULL ) {
            me->label_cb = cb->prev;
            mem_free( cb );
            cb = me->label_cb;
        }
        ml = me->macline;
        while( ml != NULL ) {           // free all macro lines
             mln = ml->next;
             mem_free( ml );
             ml = mln;
        }
        if( *dict == me ) {                // delete first entry
            *dict = me->next;
        } else {
            wk = *dict;
            while( wk != NULL ) {     // search the entry in macro dictionary
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
/*  free_macro_dict   free all macro dictionary entries                    */
/***************************************************************************/

void    free_macro_dict( mac_entry * * dict )
{
    mac_entry   *   wk;
    mac_entry   *   wkn;

    wk = *dict;
    while( wk != NULL ) {
        wkn = wk->next;
        free_macro_entry_short( wk );
        wk = wkn;
    }
    *dict = NULL;                       // dictionary is empty
    return;
}



/***************************************************************************/
/*  search macro entry in specified dictionary                             */
/*  returns ptr to macro or NULL if not found                              */
/***************************************************************************/

mac_entry   * find_macro( mac_entry * dict, const char * name )
{
    mac_entry   *   wk;
    mac_entry   *   curr;

    wk   = NULL;
    curr = dict;
    while( curr != NULL) {
        if( !strcmp( curr->name, name ) ) {
            wk = curr;
            break;
        }
        curr = curr->next;
    }
    return( wk );
}


/***************************************************************************/
/*  print_macro_dict  output all of the macro dictionary                   */
/***************************************************************************/

void    print_macro_dict( mac_entry * dict, bool with_mac_lines )
{
    mac_entry           *   wk;
    int                     cnt;
    int                     len;
    inp_line            *   ml;
    int                     lc;
    static  const   char    fill[10] = "         ";

    cnt = 0;
    wk = dict;
    out_msg( "\nList of defined macros:\n\n" );
    for( wk = dict; wk != NULL; wk = wk->next ) {

        len =  strlen( wk->name );
        out_msg( "Macro='%s'%sdefined line %d file '%s'\n", wk->name,
                &fill[len], wk->lineno, wk->mac_file_name );
        if( with_mac_lines ) {
            for( ml = wk->macline, lc = 1; ml != NULL; ml = ml->next, lc++ ) {
                out_msg("%+.3d %s\n", lc, ml->value );
            }
        }
        cnt++;
    }
    out_msg( "\nTotal macros defined: %d\n", cnt );
    return;
}

