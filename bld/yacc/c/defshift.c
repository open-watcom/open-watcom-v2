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
* Description:  Mark default shifts.
*
****************************************************************************/


#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "yacc.h"
#include "yaccins.h"
#include "alloc.h"

unsigned keyword_id_low;
unsigned keyword_id_high;
unsigned nStates;
unsigned nActions;

static int okToConsider( a_sym *sym ) {
    if( sym->pro != NULL ) return( 0 );
    if( sym->token < keyword_id_low ) return( 0 );
    if( sym->token > keyword_id_high ) return( 0 );
    return( 1 );
}

static void doState(
    a_state     *state,
    unsigned    *state_freq,
    char        *all_used,
    unsigned    range_size )
{
    int i;
    unsigned max;
    index_t max_idx;
    a_shift_action *saction;
    a_sym *shift_sym;

    if( IsDead( *state ) ) {
        return;
    }
    if( IsAmbiguous( *state ) ) {
        return;
    }
    if( state->redun->pro != NULL ) {
        // has a reduction
        return;
    }
    memset( state_freq, 0, nstate * sizeof( *state_freq ) );
    memset( all_used, 0, range_size );
    /* find shift state frequencies */
    for( saction = state->trans; ; ++saction ) {
        shift_sym = saction->sym;
        if( shift_sym == NULL ) break;
        if( ! okToConsider( shift_sym ) ) continue;
        all_used[ shift_sym->token - keyword_id_low ] = 1;
        ++state_freq[ saction->state->sidx ];
    }
    /* verify entire range of tokens shift somewhere */
    for( i = 0; i < range_size; ++i ) {
        if( ! all_used[i] ) {
            return;
        }
    }
    /* find which state had the highest frequency */
    max_idx = 0;
    max = state_freq[ max_idx ];
    for( i = 1; i < nstate; ++i ) {
        unsigned test = state_freq[i];
        if( test > max ) {
            max = test;
            max_idx = i;
        }
    }
    if( max == 0 ) {
        return;
    }
    ++nStates;
    /* mark tokens used to shift to highest frequency state */
    for( saction = state->trans; ; ++saction ) {
        shift_sym = saction->sym;
        if( shift_sym == NULL ) break;
        if( ! okToConsider( shift_sym ) ) continue;
        if( saction->state->sidx == max_idx ) {
            saction->is_default = 1;
            ++nActions;
        }
    }
}

void MarkDefaultShifts( void )
/****************************/
{
    unsigned range_size;
    unsigned *state_freq;
    char *all_used;
    int i;

    state_freq = MALLOC( nstate, unsigned );
    range_size = ( keyword_id_high - keyword_id_low ) + 1;
    all_used = MALLOC( range_size, char );
    for( i = 0; i < nstate; ++i ) {
        doState( statetab[i], state_freq, all_used, range_size );
    }
    free( state_freq );
    free( all_used );
    dumpstatistic( "default shift optimization states", nStates );
    dumpstatistic( "default shift optimization actions", nActions );
}
