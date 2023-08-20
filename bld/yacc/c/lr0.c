/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023      The Open Watcom Contributors. All Rights Reserved.
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


#include <stdlib.h>
#include <string.h>
#include "yacc.h"
#include "alloc.h"

index_n nbstate;
index_n nstate;
index_n nvtrans;
index_n nredun;

a_state **statetab, *statelist, **statetail, *startstate, *errstate;

static a_state *addState( a_state **state, an_item **s, an_item **q, a_state *parent )
{
    a_parent        *add_parent;
    an_item         **p, **t;
    unsigned short  kersize;

    for( p = s; p != q; ++p ) {
        Mark( *p );
    }
    kersize = (unsigned short)( q - s );
    for( ; *state != NULL; state = &(*state)->sym_next ) {
        if( (*state)->kersize == kersize ) {
            p = (*state)->items;
            for( t = p + kersize; p != t; ++p ) {
                if( !IsMarked( *p ) ) {
                    break;
                }
            }
            if( p == t ) {
                break;
            }
        }
    }
    for( p = s; p != q; ++p ) {
        Unmark( *p );
    }
    if( *state == NULL ) {
        *state = CALLOC( 1, a_state );
        *statetail = *state;
        statetail = &(*state)->next;
        (*state)->kersize = kersize;
        (*state)->items = CALLOC( kersize + 1, an_item * );
        memcpy( (*state)->items, s, kersize * sizeof( an_item * ) );
        (*state)->idx = nstate++;
    }
    if( parent != NULL ) {
        add_parent = CALLOC( 1, a_parent );
        add_parent->state = parent;
        add_parent->next = (*state)->parents;
        (*state)->parents = add_parent;
    }
    return( *state );
}

static void Sort( void **vec, unsigned n, bool (*lt)( void *, void * ) )
/***********************************************************************
 * Heap Sort.  Reference:  Knuth, Vol. 3, pages 146, 147.
 */
{
    unsigned    i, j, l, r;
    void        *k;

    if( n > 1 ) {
        l = n / 2;
        r = n - 1;
        for( ;; ) {
            if( l > 0 ) {
                k = vec[--l];
            } else {
                k = vec[r];
                vec[r] = vec[0];
                if( --r == 0 ) {
                    vec[0] = k;
                    return;
                }
            }
            j = l;
            for( ;; ) {
                i = j;  j = 2 * j + 1;
                if( j > r )
                    break;
                if( j < r && (*lt)( vec[j], vec[j + 1] ) )
                    ++j;
                if( !(*lt)( k, vec[j] ) )
                    break;
                vec[i] = vec[j];
            }
            vec[i] = k;
        }
    }
}

static bool itemlt( void *_a, void *_b )
{
    an_item     *a = _a;
    an_item     *b = _b;

    if( a->p.sym != NULL ) {
        return( b->p.sym != NULL && a[0].p.sym->idx > b[0].p.sym->idx );
    } else {
        return( b->p.sym != NULL || a[1].p.pro->pidx > b[1].p.pro->pidx );
    }
}

static void Complete( a_state *state, an_item **s )
{
    an_item         **p, **q;
    a_reduce_action *rx;
    a_shift_action  *tx;
    a_pro           *pro;
    index_n         n;

    q = s;
    for( p = state->items; *p != NULL; ++p ) {
        Mark( *p );
        *q++ = *p;
    }
    for( p = s; p < q; ++p ) {
        if( (*p)->p.sym != NULL ) {
            for( pro = (*p)->p.sym->pro; pro != NULL; pro = pro->next ) {
                if( !IsMarked( pro->items ) ) {
                    Mark( pro->items );
                    *q++ = pro->items;
                }
            }
        }
    }
    for( p = s; p < q; ++p ) {
        Unmark( *p );
    }
    Sort( (void **)s, (unsigned)( q - s ), itemlt );
    for( p = s; p < q && (*p)->p.sym == NULL; ) {
        ++p;
    }
    n = (index_n)( p - s );
    nredun += n;
    rx = CALLOC( n + 1, a_reduce_action );
    state->redun = rx;
    for( p = s; p < q && (*p)->p.sym == NULL; ++p ) {
        (rx++)->pro = (*p)[1].p.pro;
    }
    if( p == q ) {
        state->trans = CALLOC( 1, a_shift_action );
    } else {
        n = 1;
        s = p;
        while( ++p < q ) {
            if( p[-1]->p.sym != p[0]->p.sym ) {
                ++n;
            }
        }
        tx = CALLOC( n + 1, a_shift_action );
        state->trans = tx;
        do {
            tx->sym = (*s)->p.sym;
            if( tx->sym->pro != NULL ) {
                ++nvtrans;
            }
            for( p = s; p < q && (*p)->p.sym == tx->sym; ++p ) {
                ++*p;
            }
            tx->state = addState( &tx->sym->state, s, p, state );
            s = p;
            ++tx;
        } while( s < q );
    }
}

void lr0( void )
{
    a_state     *state;
    an_item     **s;

    nvtrans = 0;
    nredun = 0;
    s = CALLOC( nitem, an_item * );
    statetail = &statelist;
    *s = startsym->pro->items;
    startstate = addState( &startsym->state, s, s + 1, NULL );
    for( state = statelist; state != NULL; state = state->next ) {
        Complete( state, s );
    }
    errstate = addState( &errsym->state, s, s, NULL );
    Complete( errstate, s );
    FREE( s );
}

void SetupStateTable( void )
{
    a_state     *state;

    FREE( statetab );
    statetab = CALLOC( nstate, a_state * );
    for( state = statelist; state != NULL; state = state->next ) {
        statetab[state->idx] = state;
    }
}

void RemoveDeadStates( void )
{
    index_n     old_state_idx;
    index_n     new_state_idx;
    a_state     *state;

    new_state_idx = 0;
    for( old_state_idx = 0; old_state_idx < nstate; old_state_idx++ ) {
        state = statetab[old_state_idx];
        if( ! IsDead( state ) ) {
            state->idx = new_state_idx;
            statetab[new_state_idx] = state;
            new_state_idx++;
        }
    }
    nstate = new_state_idx;
}
