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
* Description:  Mark default actions for states.
*
****************************************************************************/


#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#include "yacc.h"
#include "yaccins.h"
#include "alloc.h"

void MarkDefaultReductions( void )
/********************************/
{
    a_pro *pro;
    a_state *state;
    a_reduce_action *raction;
    a_reduce_action *default_raction;
    a_shift_action *saction;
    unsigned i;
    unsigned max_tokens;
    unsigned count_tokens;
    unsigned nshifts;
    unsigned nreduces;

    for( i = 0; i < nstate; ++i ) {
        // iterate over all reductions in state
        state = statetab[i];
        nshifts = 0;
        for( saction = state->trans; saction->sym != NULL; ++saction ) {
            ++nshifts;
            if( saction->sym == errsym ) {
                // if there is a shift on the error token then
                // we don't want default reduce actions
                state = NULL;
                break;
            }
        }
        if( state == NULL ) {
            continue;
        }
        default_raction = NULL;
        max_tokens = 0;
        nreduces = 0;
        for( raction = state->redun; (pro = raction->pro) != NULL; ++raction ) {
            ++nreduces;
            count_tokens = Cardinality( raction->follow );
            if( count_tokens >= max_tokens ) {
                max_tokens = count_tokens;
                default_raction = raction;
            }
        }
        if( nshifts == 0 && nreduces == 1 ) {
            OnlyReduce( *state );
            ++nstate_1_reduce;
            assert( default_raction != NULL );
        }
        if( default_raction != NULL ) {
            state->default_reduction = default_raction;
        }
    }
}
