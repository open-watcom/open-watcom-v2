/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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


#include <stdio.h>
#include <ctype.h>
#include "yacc.h"
#include "yaccins.h"
#include "alloc.h"


#define PROENTRY(x)     ((x)+1)

#define ACCEPT          (-1)
#define DEFAULT         0

#define VBLENTRY(x)     (3*(x)+1+npro)
#define TOKENTRY(x)     (3*(x)+2+npro)
#define OPTENTRY(x)     (3*(x)+3+npro)

extern void     emitins( short, short );
extern void     writeobj( int );

static int      label;

static int newlabel( void )
{
    return( ++label );
}

static void emitt( sym_n *symbol, short *target, sym_n n, short redun )
{
    sym_n           i;
    sym_n           sym_idx;

    for( i = 0; i < n; ++i ) {
        sym_idx = symbol[i];
        emitins( TCMP, symtab[sym_idx]->token );
        emitins( JEQ, target[sym_idx] );
    }
    emitins( JMP, redun );
}

static void emitv( sym_n *symbol, short *target, sym_n n )
{
    sym_n           m;
    int             l1;
    int             l2;

    if( n == 1 ) {
        emitins( JMP, target[symbol[0]] );
    } else if( n != 0 ) {
        m = n / 2;
        n -= m + 1;
        l1 = 0;
        l2 = 0;
        emitins( VCMP, symtab[symbol[m]]->token );
        if( m == 1 ) {
            emitins( JLT, target[symbol[0]] );
        } else if( m != 0 ) {
            l1 = newlabel();
            emitins( JLT, l1 );
        }
        if( n == 1 ) {
            emitins( JGT, target[symbol[m + 1]] );
        } else if( n != 0 ) {
            l2 = newlabel();
            emitins( JGT, l2 );
        }
        emitins( JMP, target[symbol[m]] );
        if( m > 1 ) {
            emitins( LBL, l1 );
            emitv( symbol, target, m );
        }
        if( n > 1 ) {
            emitins( LBL, l2 );
            emitv( &symbol[m + 1], target, n );
        }
    }
}

void genobj( FILE *fp )
{
    sym_n           *symbol;
    sym_n           *p;
    sym_n           *q;
    sym_n           *r;
    short           *target;
    short           action;
    bitnum          *mp;
    a_sym           *sym;
    a_pro           *pro;
    an_item         *item;
    a_state         *state;
    a_shift_action  *tx;
    a_reduce_action *rx;
    int             i;
    rule_n          j;
    sym_n           sym_idx;
    unsigned        max_savings;
    unsigned        savings;

    for( sym_idx = nterm; sym_idx < nsym; ++sym_idx ) {
        symtab[sym_idx]->token = sym_idx - nterm;
    }
    label = OPTENTRY( nstate - 1 );

    emitins( JMP, TOKENTRY( startstate->sidx ) );

    target = CALLOC( nsym, short );
    for( sym_idx = 0; sym_idx < nsym; ++sym_idx ) {
        target[sym_idx] = DEFAULT;
    }
    symbol = CALLOC( nsym, sym_n );
    for( i = 0; i < nstate; ++i ) {
        state = statetab[i];
        r = q = symbol;
        for( tx = state->trans; (sym = tx->sym) != NULL; ++tx ) {
            if( sym == eofsym ) {
                action = ACCEPT;
            } else if( sym->idx < nterm ) {
                action = TOKENTRY(tx->state->sidx);
            } else {
                action = OPTENTRY(tx->state->sidx);
            }
            *q++ = sym->idx;
            target[sym->idx] = action;
        }
        max_savings = 0;
        for( rx = state->redun; (pro = rx->pro) != NULL; ++rx ) {
            mp = Members( rx->follow );
            savings = mp - setmembers;
            if( savings == 0 )
                continue;
            action = PROENTRY( pro->pidx );
            if( max_savings < savings ) {
                max_savings = savings;
                r = q;
            }
            while( mp-- != setmembers ) {
                *q++ = *mp;
                target[*mp] = action;
            }
        }
        action = DEFAULT;
        if( max_savings ) {
            action = target[*r];
            p = r;
            while( max_savings-- > 0 ) {
                target[*p++] = DEFAULT;
            }
        }

        emitins( LBL, TOKENTRY( state->sidx ) );
        emitins( SCAN, 0 );
        emitins( LBL, OPTENTRY( state->sidx ) );
        emitins( CALL, VBLENTRY( state->sidx ) );
        q = symbol;
        for( sym_idx = nterm; sym_idx < nsym; ++sym_idx ) {
            if( target[sym_idx] != DEFAULT ) {
                *q++ = sym_idx;
            }
        }
        if( q != symbol ) {
            emitv( symbol, target, q - symbol );
            for( p = symbol; p < q; ++p ) {
                target[*p] = DEFAULT;
            }
        }
        emitins( LBL, VBLENTRY( state->sidx ) );

        q = symbol;
        for( sym_idx = 0; sym_idx < nterm; ++sym_idx ) {
            if( target[sym_idx] != DEFAULT ) {
                *q++ = sym_idx;
            }
        }
        emitt( symbol, target, q - symbol, action );
        for( p = symbol; p < q; ++p ) {
            target[*p] = DEFAULT;
        }
    }

    FREE( target );
    FREE( symbol );

    for( j = 0; j < npro; ++j ) {
        pro = protab[j];
        if( pro != startpro ) {
            for( item = pro->items; item->p.sym != NULL; ) {
                ++item;
            }
            emitins( LBL, PROENTRY( pro->pidx ) );
            emitins( ACTION, PROPACK( item - pro->items, j ) );
            emitins( REDUCE, PROPACK( item - pro->items, pro->sym->token ) );
        }
    }

    writeobj( label + 1 );
}
