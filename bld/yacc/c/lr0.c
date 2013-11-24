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


#include <stdlib.h>
#include <string.h>
#include "yacc.h"
#include "alloc.h"

index_t nbstate;
index_t nstate;
index_t nvtrans;
index_t nredun;

a_state **statetab, *statelist, **statetail, *startstate, *errstate;

static a_state *addState( a_state **enter, an_item **s, an_item **q, a_state *parent )
{
    a_parent    *add_parent;
    an_item     **p, **t;
    int         kersize;

    for( p = s; p != q; ++p ) {
        Mark( **p );
    }
    kersize = q - s;
    for( ; *enter != NULL; enter = &(*enter)->same_enter_sym ) {
        if( (*enter)->kersize == kersize ) {
            p = (*enter)->name.item;
            for( t = p + kersize; p != t; ++p ) {
                if( !IsMarked( **p ) ) {
                    goto contin;
                }
            }
            break;
        }
contin:;
    }
    for( p = s; p != q; ++p ) {
        Unmark( **p );
    }
    if( *enter == NULL ) {
        *enter = CALLOC( 1, a_state );
        State( **enter );
        *statetail = *enter;
        statetail = &(*enter)->next;
        (*enter)->kersize = kersize;
        (*enter)->name.item = CALLOC( kersize + 1, an_item * );
        memcpy( (*enter)->name.item, s, kersize * sizeof( an_item * ) );
        (*enter)->sidx = nstate++;
    }
    if( parent != NULL ) {
        add_parent = CALLOC( 1, a_parent );
        add_parent->state = parent;
        add_parent->next = (*enter)->parents;
        (*enter)->parents = add_parent;
    }
    return( *enter );
}

/*  Heap Sort.  Reference:  Knuth, Vol. 3, pages 146, 147. */
static void Sort( void **vec, int n, bool (*lt)( void *, void * ) )
{
    int         i, j, l, r;
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
                if( --r <= 0 ) {
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
        return( b->p.sym && a[0].p.sym > b[0].p.sym );
    } else {
        return( b->p.sym || a[1].p.pro > b[1].p.pro );
    }
}

static void Complete( a_state *x, an_item **s )
{
    an_item         **p, **q;
    a_reduce_action *rx;
    a_shift_action  *tx;
    a_pro           *pro;
    int             n;

    q = s;
    for( p = x->name.item; *p != NULL; ++p ) {
        Mark( **p );
        *q++ = *p;
    }
    for( p = s; p < q; ++p ) {
        if( (*p)->p.sym != NULL ) {
            for( pro = (*p)->p.sym->pro; pro != NULL; pro = pro->next ) {
                if( !IsMarked( *pro->item ) ) {
                    Mark( *pro->item );
                    *q++ = pro->item;
                }
            }
          }
    }
    for( p = s; p < q; ++p ) {
        Unmark( **p );
    }
    Sort( (void **)s, q - s, itemlt );
    for( p = s; p < q && (*p)->p.sym == NULL; ) {
        ++p;
    }
    n = p - s;
    nredun += n;
    rx = CALLOC( n + 1, a_reduce_action );
    x->redun = rx;
    for( p = s; p < q && (*p)->p.sym == NULL; ++p ) {
        (rx++)->pro = (*p)[1].p.pro;
    }
    if( p == q ) {
        x->trans = CALLOC( 1, a_shift_action );
    } else {
        n = 1;
        s = p;
        while( ++p < q ) {
            n += (p[-1]->p.sym != p[0]->p.sym);
        }
        tx = CALLOC( n + 1, a_shift_action );
        x->trans = tx;
        do {
            tx->sym = (*s)->p.sym;
            if( tx->sym->pro != NULL ) {
                ++nvtrans;
            }
            for( p = s; p < q && (*p)->p.sym == tx->sym; ++p ) {
                ++*p;
            }
            tx->state = addState( &tx->sym->enter, s, p, x );
            s = p;
            ++tx;
        } while( s < q );
    }
}

void lr0( void )
{
    a_state     *x;
    an_item     **s;

    s = CALLOC( nitem, an_item * );
    statetail = &statelist;
    *s = startsym->pro->item;
    startstate = addState( &startsym->enter, s, s + 1, NULL );
    for( x = statelist; x != NULL; x = x->next ) {
        Complete( x, s );
    }
    errstate = addState( &errsym->enter, s, s, NULL );
    Complete( errstate, s );
    FREE( s );
}

void SetupStateTable( void )
{
    a_state     *x;

    FREE( statetab );
    statetab = CALLOC( nstate, a_state * );
    for( x = statelist; x != NULL; x = x->next ) {
        statetab[x->sidx] = x;
    }
}

void RemoveDeadStates( void )
{
    int         i;
    int         j;
    a_state     *x;

    j = 0;
    for( i = 0; i < nstate; ++i ) {
        x = statetab[i];
        if( ! IsDead( *x ) ) {
            x->sidx = j;
            statetab[j] = x;
            ++j;
        }
    }
    nstate = j;
}
