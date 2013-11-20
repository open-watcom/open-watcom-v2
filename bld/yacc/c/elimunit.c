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
* Description:  Eliminate unit productions.
*
****************************************************************************/


#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "yacc.h"
#include "yaccins.h"
#include "alloc.h"

static unsigned changeOccurred;
static unsigned deadStates;

#if 0
void dumpInternalState( a_state *x )
{
    a_parent *parent;
    a_shift_action *tx;
    a_reduce_action *rx;
    unsigned col, new_col;
    short int *p;
    short int *mp;
    a_name name;

    printf( "state %d: %p (%u)\n", x->sidx, x, x->kersize );
    printf( "  parent states:" );
    col = 4;
    for( parent = x->parents; parent != NULL; parent = parent->next ) {
        printf( " %d(%p)", parent->state->sidx, parent->state );
        --col;
        if( col == 0 ) {
            printf( "\n" );
            col = 5;
        }
    }
    printf( "\n" );
    if( x->name.item[0] == NULL || !IsState( *x->name.item[0] ) ) {
        for( name.item = x->name.item; *name.item != NULL; ++name.item ) {
            showitem( *name.item, " ." );
        }
    } else {
        for( name.state = x->name.state; *name.state != NULL; ++name.state ) {
            printf( " %d", (*name.state)->sidx );
        }
        printf( "\n" );
    }
    printf( "actions:" );
    col = 8;
    for( tx = x->trans; tx->sym != NULL; ++tx ) {
        new_col = col + 1 + strlen( tx->sym->name ) + 1 + 1 + 3;
        if( new_col > 79 ) {
            putchar('\n');
            new_col -= col;
        }
        col = new_col;
        printf( " %s:s%03d", tx->sym->name, tx->state->sidx );
    }
    putchar( '\n' );
    col = 0;
    for( rx = x->redun; rx->pro != NULL; ++rx ) {
        for( mp = Members( rx->follow ); --mp >= setmembers; ) {
            new_col = col + 1 + strlen( symtab[*mp]->name );
            if( new_col > 79 ) {
                putchar('\n');
                new_col -= col;
            }
            col = new_col;
            printf( " %s", symtab[*mp]->name );
        }
        new_col = col + 1 + 5;
        if( new_col > 79 ) {
            putchar('\n');
            new_col -= col;
        }
        col = new_col;
        printf( ":r%03d", rx->pro->pidx );
    }
    putchar( '\n' );
}
#endif

static a_state *findNewShiftState( a_state *state, a_sym *sym )
{
    a_shift_action *saction;
    a_sym *shift_sym;

    for( saction = state->trans; (shift_sym = saction->sym) != NULL; ++saction ) {
        if( shift_sym == sym ) {
            return( saction->state );
        }
    }
    return( NULL );
}

static a_pro *analyseParents( a_state *state, a_pro *pro, a_word *reduce_set )
{
    a_pro *test_pro;
    a_pro *new_pro;
    a_sym *old_lhs;
    a_state *parent_state;
    a_state *new_state;
    a_parent *parent;
    a_parent *split_parent;
    a_reduce_action *raction;

    split_parent = NULL;
    new_pro = NULL;
    old_lhs = pro->sym;
    for( parent = state->parents; parent != NULL; parent = parent->next ) {
        parent_state = parent->state;
        new_state = findNewShiftState( parent_state, old_lhs );
        if( new_state == NULL ) {
            printf( "error! %u %s %u\n", state->sidx, old_lhs->name, parent_state->sidx );
            exit(1);
        }
        for( raction = new_state->redun; (test_pro = raction->pro) != NULL; ++raction ) {
            if( !test_pro->unit ) {
                continue;
            }
            if( EmptyIntersection( reduce_set, raction->follow ) ) {
                continue;
            }
            if( new_pro == NULL ) {
                new_pro = test_pro;
            } else if( new_pro != test_pro ) {
                new_pro = NULL;
                break;
            }
            /* we have a reduce of a unit rule on similar tokens */
            Intersection( reduce_set, raction->follow );
            break;
        }
        if( new_pro == NULL || test_pro == NULL ) {
            split_parent = parent;
        }
    }
    if( Empty( reduce_set ) || split_parent != NULL ) {
        new_pro = NULL;
    }
    return( new_pro );
}

static a_shift_action *addShiftAction( a_sym *sym, a_state *state, a_shift_action *s )
{
    a_shift_action *saction;
    a_shift_action *new_saction;
    int i;

    for( saction = s; saction->sym != NULL; ) {
        ++saction;
    }
    i = saction - s;
    new_saction = realloc( s, ( i + 2 ) * sizeof(a_shift_action) );
    memset( &new_saction[i], 0, sizeof( *new_saction ) * 2 );
    new_saction[i].sym = sym;
    new_saction[i].state = state;
    new_saction[i+1].sym = NULL;
    new_saction[i+1].state = NULL;
    return( new_saction );
}

static a_reduce_action *addReduceAction( a_pro *pro, a_word *follow, a_reduce_action *r )
{
    a_reduce_action *raction;
    a_reduce_action *new_raction;
    a_word *new_follow;
    int i;

    for( raction = r; raction->pro != NULL; ) {
        ++raction;
    }
    i = raction - r;
    new_follow = AllocSet( 1 );
    Assign( new_follow, follow );
    new_raction = realloc( r, ( i + 2 ) * sizeof(a_reduce_action) );
    new_raction[i].pro = pro;
    new_raction[i].follow = new_follow;
    new_raction[i+1].pro = NULL;
    new_raction[i+1].follow = NULL;
    return( new_raction );
}

static a_reduce_action *removeReduceAction( a_reduce_action *remove, a_reduce_action *r )
{
    a_reduce_action *raction;
    a_reduce_action *copy_raction;

    copy_raction = NULL;
    for( raction = r; ; ++raction ) {
        if( raction == remove ) {
            copy_raction = remove;
        } else {
            if( copy_raction != NULL ) {
                *copy_raction = *raction;
                ++copy_raction;
            }
        }
        if( raction->pro == NULL ) {
            break;
        }
    }
    return( r );
}

static a_sym *onlyOneReduction( a_state *state )
{
    a_reduce_action *raction;
    a_shift_action *saction;
    a_pro *pro;
    a_pro *save_pro;
    a_sym *shift_sym;

    /*
        We shouldn't kill ambiguous states because a user that has to deal
        with a crazy language (like C++) might want to keep the state around.
        This check has the dual benefit of eliminating a lot of checking
        and solving part of the ambiguous state problem.
    */
    if( state->kersize != 1 ) {
        return( NULL );
    }
    if( IsAmbiguous( *state ) ) {
        /* catch all of the ambiguous states */
        return( NULL );
    }
    /* iterate over all shifts in the state */
    saction = state->trans;
    shift_sym = saction->sym;
    if( shift_sym != NULL ) {
        /* state contains at least one shift */
        return( NULL );
    }
    // iterate over all reductions in state
    save_pro = NULL;
    for( raction = state->redun; (pro = raction->pro) != NULL; ++raction ) {
        if( save_pro != NULL ) {
            /* state contains at least two reductions */
            return( NULL );
        }
        if( ! pro->unit ) {
            /* state contains at least one reduction by a non-unit production */
            return( NULL );
        }
        save_pro = pro;
    }
    if( save_pro == NULL ) {
        /* should never execute this but just in case... */
        return( NULL );
    }
    return( save_pro->sym );
}

static void removeParent( a_state *child, a_state *parent )
{
    a_parent **prev;
    a_parent *curr;

    if( child->parents == NULL ) {
        return;
    }
    prev = &(child->parents);
    for( curr = *prev; curr != NULL; curr = curr->next ) {
        if( curr->state == parent ) {
            *prev = curr->next;
            free( curr );
            break;
        }
        prev = &(curr->next);
    }
    if( child->parents == NULL ) {
        ++deadStates;
        Dead( *child );
    }
}

static a_state *onlyShiftsOnTerminals( a_state *state )
{
    a_shift_action *saction;
    a_sym *shift_sym;

    /*
        If there are shifts on non-terminals then the unit reduction
        is important because it moves to the correct state for more
        reductions.  We cannot remove this unit reduction.
    */
    for( saction = state->trans; (shift_sym = saction->sym) != NULL; ++saction ) {
        if( shift_sym->pro != NULL ) {
            return( NULL );
        }
    }
    return( state );
}

static int immediateShift( a_state *state, a_reduce_action *raction, a_pro *pro )
{
    a_sym *unit_lhs;
    a_sym *term_sym;
    a_state *after_lhs_state;
    a_state *final_state;
    a_state *check_state;
    a_parent *parent;
    a_word *follow;
    short *mp;
    int change_occurred;

    /*
        requirements:
        (1) state must have a reduction by a unit production (L1 <- r1) on
            a set of tokens (s)
        (2) all parents must shift to a state where a shift on a terminal
            in s ends up in a new state that is the same for all parents

        action:
            add shift on terminal to common parent shift state
    */
    //dumpInternalState( state );
    follow = raction->follow;
    unit_lhs = pro->sym;
    change_occurred = 0;
    for( mp = Members( follow ); --mp >= setmembers; ) {
        term_sym = symtab[*mp];
        check_state = NULL;
        for( parent = state->parents; parent != NULL; parent = parent->next ) {
            after_lhs_state = findNewShiftState( parent->state, unit_lhs );
            after_lhs_state = onlyShiftsOnTerminals( after_lhs_state );
            if( after_lhs_state == NULL ) {
                check_state = NULL;
                break;
            }
            final_state = findNewShiftState( after_lhs_state, term_sym );
            if( final_state == NULL ) {
                check_state = NULL;
                break;
            }
            if( check_state != NULL && check_state != final_state ) {
                check_state = NULL;
                break;
            }
            check_state = final_state;
        }
        if( check_state != NULL ) {
            /* all shifts in *terminal ended up in the same state! */
            state->trans = addShiftAction( term_sym, check_state, state->trans );
            ClearBit( follow, *mp );
            change_occurred = 1;
            ++changeOccurred;
        }
    }
    if( Empty( follow ) ) {
        state->redun = removeReduceAction( raction, state->redun );
        change_occurred = 1;
    }
    return( change_occurred );
}

static int multiUnitReduce( a_state *state, a_reduce_action *raction, a_pro *pro, a_word *reduce_set )
{
    a_pro *new_pro;

    /*
        requirements:
        (1) state must have a reduction by a unit production (L1 <- r1) on
            a set of tokens (s)
        (2) all parents must reduce by an unit production (L2 <- L1) on
            a set of tokens (t)

        action:
            change state to reduce (L2<-r1) instead of (L1<-r1) on t
            (if s != t, we have to add a new reduce action)
    */
    Assign( reduce_set, raction->follow );
    new_pro = analyseParents( state, pro, reduce_set );
    if( new_pro != NULL ) {
        /* parents satisfied all the conditions */
        if( Equal( reduce_set, raction->follow ) ) {
            raction->pro = new_pro;
        } else {
            AndNot( raction->follow, reduce_set );
            state->redun = addReduceAction( new_pro, reduce_set, state->redun );
        }
        ++changeOccurred;
        return( 1 );
    }
    return( 0 );
}

static int shiftToSingleReduce( a_state *state, a_shift_action *saction )
{
    a_state *sub_state;
    a_sym *new_lhs;
    int made_change;

    /*
        requirements:
        (1) state (s1) must have a shift on token (t) into a state (s2)
            that only has one action and it must be a reduction by
            an unit production
            (after which state (s1) will shift into state (s3))

        action:
            change shift action for token (t) to shift into state (s3)
    */
    made_change = 0;
    for( sub_state = saction->state; (new_lhs = onlyOneReduction( sub_state )) != NULL; sub_state = saction->state ) {
        saction->state = findNewShiftState( state, new_lhs );
        removeParent( sub_state, state );
        made_change = 1;
        ++changeOccurred;
    }
    saction->units_checked = TRUE;
    return( made_change );
}

static void tryElimination( a_state *state, a_word *reduce_set )
{
    a_reduce_action *raction;
    a_pro *pro;

    if( IsDead( *state ) ) {
        return;
    }
    if( IsAmbiguous( *state ) ) {
        return;
    }
    // iterate over all reductions in state
    for( pro = state->redun->pro; pro != NULL; ) {
        for( raction = state->redun; (pro = raction->pro) != NULL; ++raction ) {
            if( pro->unit ) {
                if( multiUnitReduce( state, raction, pro, reduce_set ) ) {
                    /* state->redun could have changed */
                    break;
                }
                if( immediateShift( state, raction, pro ) ) {
                    /* state->redun could have changed */
                    break;
                }
            }
        }
    }
}

static void tossSingleReduceStates( a_state *state )
{
    a_shift_action *saction;
    a_sym *shift_sym;

    if( IsDead( *state ) ) {
        return;
    }
    /* iterate over all shifts in the state */
    for( saction = state->trans; (shift_sym = saction->sym) != NULL; ++saction ) {
        if( saction->units_checked )
            continue;
        shiftToSingleReduce( state, saction );
    }
}

void EliminateUnitReductions( void )
/**********************************/
{
    unsigned sum;
    a_word *reduce_set;
    int i;

    sum = 0;
    do {
        changeOccurred = 0;
        for( i = 0; i < nstate; ++i ) {
            tossSingleReduceStates( statetab[i] );
        }
        sum += changeOccurred;
    } while( changeOccurred );
    reduce_set = AllocSet( 1 );
    do {
        changeOccurred = 0;
        for( i = 0; i < nstate; ++i ) {
            tryElimination( statetab[i], reduce_set );
        }
        sum += changeOccurred;
    } while( changeOccurred );
    free( reduce_set );
    dumpstatistic( "unit reduction states removed", deadStates );
    dumpstatistic( "unit reduction optimizations", sum );
}
