/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023-2026 The Open Watcom Contributors. All Rights Reserved.
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


#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "yacc.h"
#include "alloc.h"

#define INFINITY        (unsigned short)-1

static a_look **stk, **top;

static void Reads( a_look *x )
{
    a_shift_action  *saction;
    a_look          *y;
    unsigned short  k;
    a_sym           *sym;
    sym_n           sym_idx;

    *top++ = x;
    k = (unsigned short)( top - stk );
    x->depth = k;
    for( saction = x->trans->state->trans; (sym = saction->sym) != NULL; ++saction ) {
        if( sym->pro == NULL ) {
            sym_idx = sym->idx;
            SetBit( x->follow, sym_idx, WSIZE );
        }
    }
    for( y = x->trans->state->look; y->trans != NULL; ++y ) {
        if( y->trans->sym->nullable ) {
            if( y->depth == 0 ) {
                Reads( y );
            }
            if( y->depth < x->depth ) {
                x->depth = y->depth;
            }
            Union( x->follow, y->follow );
        }
    }
    if( x->depth == k ) {
        do {
            --top;
            (*top)->depth = INFINITY;
            Assign( (*top)->follow, x->follow );
        } while( *top != x );
    }
}

static void CalcReads( void )
{
    a_state     *state;
    a_look      *p;

    for( state = statelist; state != NULL; state = state->next ) {
        for( p = state->look; p->trans != NULL; ++p ) {
            if( p->depth == 0 ) {
                Reads( p );
            }
        }
    }
}

static void Nullable( void )
{
    a_sym       *sym;
    a_pro       *pro;
    an_item     *p;
    bool        nullable_added;

    for( sym = symlist; sym != NULL; sym = sym->next ) {
        sym->nullable = false;
    }
    do {
        nullable_added = false;
        for( sym = symlist; sym != NULL; sym = sym->next ) {
            if( !sym->nullable ) {
                for( pro = sym->pro; pro != NULL; pro = pro->next ) {
                    for( p = pro->items; p->p.sym != NULL; ++p ) {
                        if( !p->p.sym->nullable ) {
                            break;
                        }
                    }
                    if( p->p.sym == NULL ) {
                        /*
                         * all of the RHS symbols are nullable
                         * (the vacuous case means the LHS is nullable)
                         */
                        sym->nullable = true;
                        nullable_added = true;
                    }
                }
            }
        }
    } while( nullable_added );
}

static void Includes( a_look *x )
{
    a_look          *y;
    a_link          *link;
    unsigned short  k;

    *top++ = x;
    k = (unsigned short)( top - stk );
    x->depth = k;
    for( link = x->include; link != NULL; link = link->next ) {
        y = link->el;
        if( y->depth == 0 ) {
            Includes( y );
        }
        if( y->depth < x->depth ) {
            x->depth = y->depth;
        }
        Union( x->follow, y->follow );
    }
    if( x->depth == k ) {
        do {
            --top;
            (*top)->depth = INFINITY;
            Assign( (*top)->follow, x->follow );
        } while( *top != x );
    }
}

static void CalcIncludes( void )
{
    a_state         *state;
    a_state         *state1;
    a_shift_action  *saction;
    a_look          *p, *q;
    a_sym           *sym;
    a_pro           *pro;
    an_item         *nullable, *item;
    a_link          *free;

    for( state = statelist; state != NULL; state = state->next ) {
        for( p = state->look; p->trans != NULL; ++p ) {
            p->depth = 0;
            for( pro = p->trans->sym->pro; pro != NULL; pro = pro->next ) {
                nullable = pro->items;
                for( item = pro->items; (sym = item->p.sym) != NULL; ++item ) {
                    if( !sym->nullable ) {
                        nullable = item;
                    }
                }
                state1 = state;
                for( item = pro->items; (sym = item->p.sym) != NULL; ++item ) {
                    if( sym->pro == NULL ) {
                        for( saction = state1->trans; saction->sym != NULL; ++saction ) {
                            if( saction->sym == sym ) {
                                break;
                            }
                        }
                        state1 = saction->state;
                    } else {
                        for( q = state1->look; q->trans->sym != NULL; ++q ) {
                            if( q->trans->sym == sym ) {
                                break;
                            }
                        }
                        if( item >= nullable ) {
                            free = CALLOC( 1, a_link );
                            free->el = p;
                            free->next = q->include;
                            q->include = free;
                        }
                        state1 = q->trans->state;
                    }
                }
            }
        }
    }
    for( state = statelist; state != NULL; state = state->next ) {
        for( p = state->look; p->trans != NULL; ++p ) {
            if( p->depth == 0 ) {
                Includes( p );
            }
        }
    }
}

static void Lookback( void )
{
    a_state         *state;
    a_state         *state1;
    a_shift_action  *saction;
    a_look          *p;
    a_reduce_action *raction;
    a_sym           *sym;
    a_pro           *pro;
    an_item         *item;

    for( state = statelist; state != NULL; state = state->next ) {
        for( p = state->look; p->trans != NULL; ++p ) {
            for( pro = p->trans->sym->pro; pro != NULL; pro = pro->next ) {
                state1 = state;
                for( item = pro->items; (sym = item->p.sym) != NULL; ++item ) {
                    for( saction = state1->trans; saction->sym != NULL; ++saction ) {
                        if( saction->sym == sym ) {
                            break;
                        }
                    }
                    state1 = saction->state;
                }
                for( raction = state1->redun; raction->pro != NULL; ++raction ) {
                    if( raction->pro == pro ) {
                        break;
                    }
                }
                Union( raction->follow, p->follow );
            }
        }
    }
}

static a_pro *extract_pro( an_item *p )
{
    an_item     *q;

    for( q = p; q->p.sym != NULL; ) {
        ++q;
    }
    return( q[1].p.pro );
}

static void check_for_user_hooks( a_state *state, a_shift_action *saction, index_n rule )
{
    bool                min_max_set;
    bool                all_match;
    conflict_id         min_id;
    conflict_id         max_id;
    conflict_id         id;
    an_item             **item;
    a_pro               *pro;
    a_SR_conflict       *conflict;
    a_SR_conflict       *last_conflict;
    a_SR_conflict_list  *cx;
    a_sym               *sym;

    if( state->kersize < 2 ) {
        return;
    }
    sym = saction->sym;
    min_max_set = false;
    min_id = CONFLICT_MAX_ID;
    max_id = CONFLICT_MIN_ID;
    for( item = state->items; *item != NULL; ++item ) {
        pro = extract_pro( *item );
        if( pro->SR_conflicts == NULL ) {
            /*
             * production doesn't contain any conflicts
             */
            return;
        }
        for( cx = pro->SR_conflicts; cx != NULL; cx = cx->next ) {
            conflict = cx->conflict;
            if( conflict->sym != sym ) {
                continue;
            }
            id = conflict->id;
            if( min_id > id ) {
                min_id = id;
                min_max_set = true;
            }
            if( max_id < id ) {
                max_id = id;
                min_max_set = true;
            }
        }
        if( !min_max_set ) {
            /*
             * production doesn't contain a matching conflict
             */
            return;
        }
    }
    for( id = min_id; id <= max_id; ++id ) {
        last_conflict = NULL;
        all_match = true;
        for( item = state->items; *item != NULL; ++item ) {
            conflict = NULL;
            pro = extract_pro( *item );
            for( cx = pro->SR_conflicts; cx != NULL; cx = cx->next ) {
                conflict = cx->conflict;
                if( conflict->id != id ) {
                    continue;
                }
                if( conflict->sym != sym ) {
                    continue;
                }
                break;
            }
            if( cx == NULL ) {
                all_match = false;
                break;
            }
            last_conflict = conflict;
        }
        if( all_match ) {
            /*
             * found the desired S/R conflict
             */
            Ambiguous( state );
            last_conflict->state = state;
            last_conflict->shift_state = saction->state;
            last_conflict->reduce = rule;
            return;
        }
    }
}

static void resolve( a_state *state, bitnum *work, a_reduce_action **reduce )
{
    a_shift_action  *saction;
    a_shift_action  *ux;
    a_reduce_action *raction;
    bitnum          *w;
    bitnum          *mp;
    sym_n           sym_idx;
    a_prec          symprec;
    a_prec          proprec;
    a_prec          prevprec;
    a_sym           *sym;

    w = work;
    for( raction = state->redun; raction->pro != NULL; ++raction ) {
        for( mp = Members( raction->follow ); mp-- != setmembers; ) {
            sym_idx = *mp;
            if( reduce[sym_idx] != NULL ) {
                prevprec = reduce[sym_idx]->pro->prec;
                proprec = raction->pro->prec;
                if( prevprec.prec == 0 || proprec.prec == 0 || prevprec.prec == proprec.prec ) {
                    *w++ = sym_idx;
                    /*
                     * resolve to the earliest production
                     */
                    if( raction->pro->pidx >= reduce[sym_idx]->pro->pidx ) {
                        continue;
                    }
                } else if( prevprec.prec > proprec.prec ) {
                    /*
                     * previous rule had higher precedence so leave it alone
                     */
                    continue;
                }
            }
            reduce[sym_idx] = raction;
        }
    }
    while( w-- != work ) {
        sym_idx = *w;
        if( symtab[sym_idx]->token == errsym->token )
            continue;
        printf( "r/r conflict in state %d on %s:\n", state->sidx, symtab[sym_idx]->name);
        ++RR_conflicts;
        for( raction = state->redun; raction->pro != NULL; ++raction ) {
            if( IsBitSet( raction->follow, sym_idx, WSIZE ) ) {
                showitem( raction->pro->items, "" );
            }
        }
        printf( "\n" );
        for( raction = state->redun; raction->pro != NULL; ++raction ) {
            if( IsBitSet( raction->follow, sym_idx, WSIZE ) ) {
                ShowSentence( state, symtab[sym_idx], raction->pro, NULL );
            }
        }
        printf( "---\n\n" );
    }
    ux = state->trans;
    for( saction = state->trans; (sym = saction->sym) != NULL; ++saction ) {
        sym_idx = sym->idx;
        if( sym_idx >= nterm || reduce[sym_idx] == NULL ) {
            *ux++ = *saction;
        } else {
            /*
             * shift/reduce conflict detected
             */
            check_for_user_hooks( state, saction, reduce[sym_idx]->pro->pidx );
            symprec = sym->prec;
            proprec = reduce[sym_idx]->pro->prec;
            if( symprec.prec == 0 || proprec.prec == 0 ) {
                if( sym != errsym ) {
                    printf( "s/r conflict in state %d on %s:\n", state->sidx, sym->name );
                    ++SR_conflicts;
                    printf( "\tshift to %d\n", saction->state->sidx );
                    showitem( reduce[sym_idx]->pro->items, "" );
                    printf( "\n" );
                    ShowSentence( state, sym, reduce[sym_idx]->pro, NULL );
                    ShowSentence( state, sym, NULL, saction->state );
                    printf( "---\n\n" );
                }
                *ux++ = *saction;
                reduce[sym_idx] = NULL;
            } else if( symprec.prec > proprec.prec ) {
                *ux++ = *saction;
                reduce[sym_idx] = NULL;
            } else if( symprec.prec == proprec.prec ) {
                if( symprec.assoc == R_ASSOC ) {
                    *ux++ = *saction;
                    reduce[sym_idx] = NULL;
                } else if( symprec.assoc == NON_ASSOC ) {
                    ux->sym = sym;
                    ux->state = errstate;
                    ++ux;
                    reduce[sym_idx] = NULL;
                }
            }
        }
    }
    ux->sym = NULL;
    for( raction = state->redun; raction->pro != NULL; ++raction ) {
        Clear( raction->follow );
    }
    for( sym_idx = 0; sym_idx < nterm; ++sym_idx ) {
        if( reduce[sym_idx] != NULL ) {
            SetBit( reduce[sym_idx]->follow, sym_idx, WSIZE );
            reduce[sym_idx] = NULL;
        }
    }
}

static void Conflict( void )
{
    a_word          *set;
    a_state         *state;
    a_shift_action  *saction;
    a_reduce_action *raction;
    a_reduce_action **reduce;
    bitnum          *work;
    unsigned        i;
    a_sym           *sym;
    sym_n           sym_idx;

    set = AllocSet( 1 );
    reduce = CALLOC( nterm, a_reduce_action * );
    work = CALLOC( nterm, bitnum );
    for( state = statelist; state != NULL; state = state->next ) {
        Clear( set );
        for( saction = state->trans; (sym = saction->sym) != NULL; ++saction ) {
            if( sym->pro == NULL ) {
                sym_idx = sym->idx;
                SetBit( set, sym_idx, WSIZE );
            }
        }
        for( raction = state->redun; raction->pro != NULL; ++raction ) {
            for( i = 0; i < GetSetSize( 1 ); ++i ) {
                if( raction->follow[i] & set[i] ) {
                    resolve( state, work, reduce );
                    break;
                }
                set[i] |= raction->follow[i];
            }
            if( i < GetSetSize( 1 ) ) {
                break;
            }
        }
    }
    FREE( work );
    FREE( reduce );
    FreeSet( set );
}

void lalr1( void )
{
    a_state         *state;
    a_shift_action  *saction;
    a_reduce_action *raction;
    a_look          *lk, *look;
    a_word          *lp, *lset;
    a_word          *rp, *rset;

    InitSets( nterm );
    lp = lset = AllocSet( nvtrans );
    rp = rset = AllocSet( nredun );
    lk = look = CALLOC( nvtrans + nstate, a_look );
    for( state = statelist; state != NULL; state = state->next ) {
        state->look = lk;
        for( saction = state->trans; saction->sym != NULL; ++saction ) {
            if( saction->sym->pro != NULL ) {
                lk->trans = saction;
                lk->follow = lp;
                lp += GetSetSize( 1 );
                ++lk;
            }
        }
        ++lk;
        for( raction = state->redun; raction->pro != NULL; ++raction ) {
            raction->follow = rp;
            rp += GetSetSize( 1 );
        }
    }
    stk = CALLOC( nvtrans, a_look * );
    top = stk;
    Nullable();
    CalcReads();
    CalcIncludes();
    Lookback();
    if( lk - look != nvtrans + nstate ) {
        puts( "internal error" );
    }
    if( lp - lset != GetSetSize( nvtrans ) ) {
        puts( "internal error" );
    }
    if( rp - rset != GetSetSize( nredun ) ) {
        puts( "internal error" );
    }
    FREE( stk );
    FREE( look );
    Conflict();
    nbstate = nstate;
//    FreeSet( rset );
//    FreeSet( lset );
}

void showstates( void )
{
    int         i;

    for( i = 0; i < nstate; ++i ) {
        printf( "\n" );
        showstate( statetab[i] );
    }
}

void showstate( a_state *state )
{
    a_parent        *parent;
    a_shift_action  *saction;
    a_reduce_action *raction;
    size_t          col, new_col;
    bitnum          *mp;
    an_item         **item;
    a_pro           *pro;
    a_sym           *sym;
    sym_n           sym_idx;

    printf( "state %d:\n", state->sidx );
    col = printf( "  parent states:" );
    for( parent = state->parents; parent != NULL; parent = parent->next ) {
        col += printf( " %d", parent->state->sidx );
        if( col > 79 ) {
            printf( "\n" );
            col = 0;
        }
    }
    printf( "\n" );
    for( item = state->items; *item != NULL; ++item ) {
        showitem( *item, " ." );
    }
    printf( "actions:" );
    col = 8;
    for( saction = state->trans; (sym = saction->sym) != NULL; ++saction ) {
        new_col = col + 1 + strlen( sym->name ) + 1 + 1 + 3;
        if( new_col > 79 ) {
            putchar('\n');
            new_col -= col;
        }
        col = new_col;
        printf( " %s:s%03d", sym->name, saction->state->sidx );
    }
    putchar( '\n' );
    col = 0;
    for( raction = state->redun; (pro = raction->pro) != NULL; ++raction ) {
        for( mp = Members( raction->follow ); mp-- != setmembers; ) {
            sym_idx = *mp;
            new_col = col + 1 + strlen( symtab[sym_idx]->name );
            if( new_col > 79 ) {
                putchar('\n');
                new_col -= col;
            }
            col = new_col;
            printf( " %s", symtab[sym_idx]->name );
        }
        new_col = col + 1 + 5;
        if( new_col > 79 ) {
            putchar('\n');
            new_col -= col;
        }
        col = new_col;
        printf( ":r%03d", pro->pidx );
    }
    putchar( '\n' );
}
