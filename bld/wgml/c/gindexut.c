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
* Description: WGML helper functions for index processing
*                   for  .ix control word
*                   and  :I1 :I2 :I3 :IH1 :IH2 :IH3 :IREF tags
*
*                   :INDEX output dummy implementation        TBD
****************************************************************************/

#include "wgml.h"
#include "gvars.h"

#include "clibext.h"

static  symsub      *   ixrefval;       // &sysixref value


/***************************************************************************/
/*  helper functions for .ix .dump processing (experimental)         TBD   */
/***************************************************************************/

static void ix_out_sp( int spaces )
{
    static const char blanks[12] = "           ";
    int k;

    k = 2 * spaces;
    if( k > 10 || k < 0 ) {
        k = 10;
    }
    out_msg("%s", &blanks[10 - k] );
}

static void ix_out( const char * t, int spaces )
{
    if( spaces > 0 ) {
        ix_out_sp( spaces );
    }
    out_msg("%s", t );
}

static void ix_out_cr( void )
{
    out_msg("\n" );
}



static void ix_out_pagenos( ix_e_blk * e, int spaces )
{
    ix_e_blk    *   ew;
    char            str[16];
    bool            first = true;

    for( ew = e; ew != NULL ; ew = ew->next ) {
        if( ew->entry_typ >= pgstring ) {   // 'pageno' is text
            if( ew->entry_typ == pgsee ) {
                ix_out_cr();
                ix_out( "See ", spaces + 1 );
                if( !first ) {
                    ix_out( "also ", 0 );
                }
            } else {
                if( first ) {
                    ix_out( " ", 0 );
                } else {
                    ix_out( ixrefval->value, 0 );
                }
            }
            ix_out( ew->u.page_text, 0 );
        } else {
            utoa( ew->u.page_no, str, 10 );
            if( first ) {
                ix_out( " ", 0 );
            } else {
                if( ew->entry_typ == pgend ) {
                    ix_out( "-", 0 );
                } else {
                    ix_out( ixrefval->value, 0 );
                }
            }
            ix_out( str, 0 );
        }
        first = false;
    }
    ix_out_cr();
}

/***************************************************************************/
/*  .ix . dump processing                                                  */
/***************************************************************************/

void    ixdump( ix_h_blk * dict )
{
    ix_h_blk    *   ixh1;
    ix_h_blk    *   ixh2;
    ix_h_blk    *   ixh3;
    char            letter[2];

    if( dict == NULL ) {                // empty dict nothing to do
        return;
    }
    letter[0]  = 0;
    letter[1]  = 0;
    ixh1 = dict;
    find_symvar( &sys_dict, "$ixref", no_subscript, &ixrefval);

    out_msg( ".IX . DUMP print the index structure --- still nearly dummy!\n" );

    while( ixh1 != NULL ) {              // level 1
        if( letter[0] != toupper( *(ixh1->ix_term) ) ) {
            letter[0] = toupper( *(ixh1->ix_term) );
            ix_out_cr();
            ix_out( letter, 1 );
            ix_out_cr();
            ix_out_cr();
        }
        if( ixh1->prt_term == NULL ) {
            ix_out( ixh1->ix_term, 1 );
        } else {
            ix_out( ixh1->prt_term, 1 );
        }

        ix_out_pagenos( ixh1->entry, 1 );
        ixh2 = ixh1->lower;
        while( ixh2 != NULL ) {     // level 2
            if( ixh2->prt_term == NULL ) {
                ix_out( ixh2->ix_term, 2 );
            } else {
                ix_out( ixh2->prt_term, 2 );
            }
            ix_out_pagenos( ixh2->entry, 2 );

            ixh3 = ixh2->lower;
            while( ixh3 != NULL ) {     // level 3
                 if( ixh3->prt_term == NULL ) {
                     ix_out( ixh3->ix_term, 3 );
                 } else {
                     ix_out( ixh3->prt_term, 3 );
                 }
                 ix_out_pagenos( ixh3->entry, 3 );

                 if( ixh3->lower != NULL ) {// this is error TBD
                    ix_h_blk * ixhp = ixh3->lower;

                    ix_out( "lower 3 nn\n", 0);
                    while( ixhp != NULL ) { // level 4
                         if( ixhp->prt_term == NULL ) {
                             ix_out( ixhp->ix_term, 4 );
                         } else {
                             ix_out( ixhp->prt_term, 4 );
                         }
                         ix_out_pagenos( ixhp->entry, 4 );
                         ixhp = ixhp->next;
                    }
                }
                ixh3 = ixh3->next;
            }
            ixh2 = ixh2->next;
        }
        ixh1 = ixh1->next;
    }
    ix_out_cr();
    out_msg( ".IX . DUMP print the index structure --- still nearly dummy!\n" );
    ix_out_cr();

    wng_count++;
}


/***************************************************************************/
/*  allocate and fill a new index entry                                    */
/*  with pageno reference or supplied text                                 */
/*                                                                         */
/*  return ptr to entry                                                    */
/***************************************************************************/

ix_e_blk *  fill_ix_e_blk( ix_e_blk * * anchor, ix_h_blk * ref, ereftyp ptyp,
                    char * text, size_t text_len )
{
    ix_e_blk    * ixewk;

    ixewk = mem_alloc( sizeof( ix_e_blk ) );
    ixewk->next      = NULL;
    ixewk->corr      = ref;
    if( ptyp == pgnone ) {
        ptyp = pgpageno;                // set default if necessary
    }
    ixewk->entry_typ = ptyp;
    if( ptyp >= pgstring ) {
        ixewk->u.page_text = mem_alloc( text_len + 1);
        strcpy( ixewk->u.page_text, text );
    } else {
        ixewk->u.page_text = NULL;
//      if( ProcFlags.page_started ) {
//          ixewk->u.page_no = page;
//      } else {
            ixewk->u.page_no = page + 1;  // + 1 ??  TBD
//      }
    }
    *anchor = ixewk;
    return( ixewk );
}


/***************************************************************************/
/*  free ix_e_blk chain                                                    */
/***************************************************************************/

static  void    free_ix_e_entries( ix_e_blk * e )
{
    ix_e_blk    *   ewk;
    ix_e_blk    *   ew  = e;

    while( ew != NULL ) {
        ewk = ew->next;
        if( ew->entry_typ >= pgstring ) {
            mem_free( ew->u.page_text );
        }
        mem_free( ew );
        ew = ewk;
    }
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

        free_ix_e_entries( ixh1->entry );

        ixh2 = ixh1->lower;
        while( ixh2 != NULL ) {         // level 2 entries
            free_ix_e_entries( ixh2->entry );

            ixh3 = ixh2->lower;
            while( ixh3 != NULL ) {     // level 3 entries
                free_ix_e_entries( ixh3->entry );

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
/*  free only ix_e entries in index_dict                                   */
/***************************************************************************/

void    free_ix_e_index_dict( ix_h_blk * * dict )
{
    ix_h_blk    *   ixh1;
    ix_h_blk    *   ixh2;
    ix_h_blk    *   ixh3;

    ixh1 = *dict;
    while( ixh1 != NULL ) {             // level 1 entries

        free_ix_e_entries( ixh1->entry );

        ixh2 = ixh1->lower;
        while( ixh2 != NULL ) {         // level 2 entries
            free_ix_e_entries( ixh2->entry );

            ixh3 = ixh2->lower;
            while( ixh3 != NULL ) {     // level 3 entries
                free_ix_e_entries( ixh3->entry );

//                ixh3->lower = NULL;
                ixh3->entry = NULL;
                ixh3        = ixh3->next;
            }
//            ixh2->lower = NULL;
            ixh2->entry = NULL;
            ixh2        = ixh2->next;
        }
//        ixh1->lower = NULL;
        ixh1->entry = NULL;
        ixh1        = ixh1->next;
    }
//    *dict = NULL;                       // dict is now empty
}


/***************************************************************************/
/*  gen_index  at the moment this is only an ixdump call                   */
/*          real formatting still                         TBD              */
/***************************************************************************/

void    gen_index( void )
{
    ixdump( index_dict );
}
