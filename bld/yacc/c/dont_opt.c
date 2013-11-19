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
* Description:  Mark states that cannot optimize unit reductions.
*
****************************************************************************/


#include <stdio.h>

#include "yacc.h"
#include "yaccins.h"
#include "alloc.h"

static int hasReduceByPro( a_state *state, a_pro *unit_pro )
{
    a_reduce_action *raction;
    a_pro *pro;

    if( IsDead( *state ) ) {
        return( 0 );
    }
    // iterate over all reductions in state
    for( raction = state->redun; (pro = raction->pro) != NULL; ++raction ) {
        if( pro->unit && pro == unit_pro ) {
            return( 1 );
        }
    }
    return( 0 );
}

/*
    Don't unit rule optimize in states that reduce a unit production
    that results in a GOTO into an ambiguous state.
*/
static void dontOptimizeUnitGOTOStates( a_state *ambig_state, a_pro *pro )
{
    a_sym *unit_rhs;
    a_parent *parent;
    a_state *parent_state;
    a_state *shift_state;
    a_shift_action *saction;

    unit_rhs = pro->item[0].p.sym;
    for( parent = ambig_state->parents; parent != NULL; parent = parent->next ) {
        parent_state = parent->state;
        saction = parent_state->trans;
        for(;;) {
            if( saction->sym == NULL ) break;
            if( saction->sym == unit_rhs ) {
                shift_state = saction->state;
                if( hasReduceByPro( shift_state, pro ) ) {
                    DontOptimize( *shift_state );
                }
            }
            ++saction;
        }
    }
}

static a_sym *findEntrySym( a_state *state )
{
    a_state *parent_state;
    a_parent *parent;
    a_state *shift_state;
    a_shift_action *saction;

    parent = state->parents;
    if( parent == NULL ) {
        return( NULL );
    }
    parent_state = parent->state;
    saction = parent_state->trans;
    for(;;) {
        if( saction->sym == NULL ) break;
        shift_state = saction->state;
        if( shift_state == state ) {
            return( saction->sym );
        }
        ++saction;
    }
    return( NULL );
}

void MarkNoUnitRuleOptimizationStates( void )
{
    int i;
    a_sym *entry_sym;
    a_state *state;
    a_pro *pro;

    for( i = 0; i < nstate; ++i ) {
        state = statetab[i];
        //if( IsDead( *state ) ) continue;
        if( ! IsAmbiguous( *state ) ) continue;
        entry_sym = findEntrySym( state );
        for( pro = entry_sym->pro; pro != NULL; pro = pro->next ) {
            if( ! pro->unit ) continue;
            dontOptimizeUnitGOTOStates( state, pro );
        }
    }
}
