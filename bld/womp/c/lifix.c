/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "watcom.h"
#include "memutil.h"
#include "lifix.h"
#include "myassert.h"

/*
    This file is used by genmsomf.c and genphar.c to handle changes to the
    fixups attached to LIDATAs with the wrong size rpt_count field.
    i.e., LIDA32 has a 32bit rpt_count whereas LIDATA (PharLap) has a 16bit
    rpt_count.  So we need to convert one to the other... a lifix_list is a
    type that stores offsets in decreasing order and the deltas that have to
    be applied to any fixup in that range.
*/

void LifixInit( lifix_list *lif ) {
/*******************************/
    lif->head = NULL;
}

int_16 LifixDelta( lifix_list *lif, uint_16 old_offset ) {
/******************************************************/
/*
    Given an offset in the original LIDATA, return the delta to apply to get
    the offset in the new LIDATA.
*/
    lifix   *walk;

    walk = lif->head;
    while( walk != NULL ) {
        if( old_offset >= walk->lower_bound ) {
            return( walk->delta );
        }
        walk = walk->next;
    }
    return( 0 );    /* default delta to apply */
}

void LifixDestroy( lifix_list *lif ) {
/**********************************/
    lifix   *cur;
    lifix   *next;

    cur = lif->head;
    while( cur ) {
        next = cur->next;
        MemFree( cur );
        cur = next;
    }
}


#if 0
void LifixAdd( lifix_list *lif, uint_16 lower_bound, int_16 delta ) {
/*****************************************************************/
/*
    This code is too general... we know the lower_bounds will occur in
    strictly increasing order; so we can just implement a stack.  The
    else section just implements a stack.
*/
    lifix   **walk;
    lifix   *new;

    new = MemAlloc( sizeof( *new ) );
    new->lower_bound = lower_bound;
    new->delta = delta;
    walk = &lif->head;
    while( *walk ) {
        if( (*walk)->lower_bound < lower_bound ) {
            break;
        }
        walk = &(*walk)->next;
    }
    new->next = *walk;
    *walk = new;
}
#else
void LifixAdd( lifix_list *lif, uint_16 lower_bound, int_16 delta ) {
/*****************************************************************/
    lifix *new;

    /* this assertion guarantees we are building the list in decreasing order*/
/**/myassert( lif->head == NULL || ( lif->head->lower_bound < lower_bound ) );
    new = MemAlloc( sizeof( *new ) );
    new->lower_bound = lower_bound;
    new->delta = delta;
    new->next = lif->head;
    lif->head = new;
}
#endif

