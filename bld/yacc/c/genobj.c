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


#include <stdio.h>
#include <ctype.h>
#include "yacc.h"
#include "yaccins.h"
#include "alloc.h"

#define PROENTRY(i)     ((i)+1)

#define ACCEPT          (-1)
#define DEFAULT         0

#define VBLENTRY(i)     (3*(i)+1+npro)
#define TOKENTRY(i)     (3*(i)+2+npro)
#define OPTENTRY(i)     (3*(i)+3+npro)

static int label;

extern void emitins( unsigned, unsigned );
extern void writeobj( int );

void genobj( void )
{
    short int *symbol, *target;
    short int *p, *q, *r;
    short int action;
    set_size *mp;
    a_sym *sym;
    a_pro *pro;
    an_item *item;
    a_state *x;
    a_shift_action *tx;
    a_reduce_action *rx;
    int i, j, savings;

    for( i = nterm; i < nsym; ++i )
        symtab[i]->token = i - nterm;

    label = OPTENTRY( nstate - 1 );

    emitins( JMP, TOKENTRY( startstate->sidx ) );

    target = CALLOC( nsym, short int );
    for( i = 0; i < nsym; ++i )
        target[i] = DEFAULT;
    symbol = CALLOC( nsym, short int );
    for( i = 0; i < nstate; ++i ) {
        x = statetab[i];
        q = symbol;
        for( tx = x->trans; (sym = tx->sym) != NULL; ++tx ) {
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
        savings = 0;
        for( rx = x->redun; (pro = rx->pro) != NULL; ++rx ) {
            action = PROENTRY( pro->pidx );
            mp = Members( rx->follow );
            if( mp - setmembers > savings ) {
                savings = mp - setmembers;
                r = q;
            }
            while( --mp >= setmembers ) {
                *q++ = *mp;
                target[*mp] = action;
            }
        }
        action = DEFAULT;
        if( savings ) {
            action = target[*r];
            p = r;
            while( --savings >= 0 ) {
                target[*p++] = DEFAULT;
            }
        }

        emitins( LBL, TOKENTRY( x->sidx ) );
        emitins( SCAN, 0 );
        emitins( LBL, OPTENTRY( x->sidx ) );
        emitins( CALL, VBLENTRY( x->sidx ) );
        q = symbol;
        for( j = nterm; j < nsym; ++j ) {
            if( target[j] != DEFAULT ) {
                *q++ = j;
            }
        }
        if( q != symbol ) {
            emitv( symbol, target, q - symbol );
            for( p = symbol; p < q; ++p ) {
                target[*p] = DEFAULT;
            }
        }
        emitins( LBL, VBLENTRY( x->sidx ) );

        q = symbol;
        for( j = 0; j < nterm; ++j ) {
            if( target[j] != DEFAULT ) {
                *q++ = j;
            }
        }
        emitt( symbol, target, q - symbol, action );
        for( p = symbol; p < q; ++p ) {
            target[*p] = DEFAULT;
        }
    }

    FREE( target );
    FREE( symbol );

    for( i = 0; i < npro; ++i ) {
        pro = protab[i];
        if( pro != startpro ) {
            for( item = pro->item; item->p.sym != NULL; ) {
                ++item;
            }
            emitins( LBL, PROENTRY( pro->pidx ) );
            emitins( ACTION, PROPACK( item - pro->item, i ) );
            emitins( REDUCE, PROPACK( item - pro->item, pro->sym->token ) );
        }
    }

    writeobj( label + 1 );
}

static emitt( symbol, target, n, redun )
  short int *symbol, *target, redun;
  unsigned n;
{
    unsigned i, j;

    for( i = 0; i < n; ++i ) {
        j = symbol[i];
        emitins( TCMP, symtab[j]->token );
        emitins( JEQ, target[j] );
    }
    emitins( JMP, redun );
}

static emitv( symbol, target, n )
  short int *symbol, *target;
  unsigned n;
{
    unsigned m;
    int l1, l2;

    if( n == 1 ) {
        emitins( JMP, target[symbol[0]] );
    } else if( n != 0 ) {
        m = n / 2;
        n -= m + 1;
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

static int newlabel()
{
    return( ++label );
}
