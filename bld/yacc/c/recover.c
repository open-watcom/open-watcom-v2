/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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


#include "yacc.h"

static a_state *AddErrState( a_state **enter, a_state **s, a_state **t )
{
    if( t == s ) {
        return( NULL );
    } else if( t == s + 1 ) {
        Unmark( **s );
        return( *s );
    } else {
        return( AddState( enter, s, t ) );
    }
}

static AddError()
{
    a_sym *xsym, *sym;
    a_pro *xpro, *pro;
    a_state *state;
    a_state **s, **t;
    a_shift_action *tx, *ty, *trans;
    a_reduce_action *rx, *ry, *redun;
    int i;
    a_word *defined, *conflict, *rset;
    short *at;

    trans = CALLOC( nsym, a_shift_action );
    rx = redun = CALLOC( npro + 1, a_reduce_action );
    rset = conflict = AllocSet( npro + 2 );
    for( i = 0; i <= npro; ++i ) {
        (rx++)->follow = rset;
        rset += GetSetSize( 1 );
    }
    defined = rset;
    s = CALLOC( nstate, a_state * );
    at = CALLOC( nstate, short );
    s = t = CALLOC( nstate + 1, a_state * );
    for( state = statelist; state != NULL; state = state->next ) {
         Mark( *state );
         *t++ = state;
    }
    restart = AddErrState( &errsym->enter, s, t );
    for( state = restart; state != NULL; state = state->next ) {
        Clear( defined );
        Clear( conflict );
        xpro = NULL;
        for( i = 0; i < state->kersize; ++i ) {
            at[i] = 0;
            pro = state->name.state[i]->redun->pro;
            if( pro > xpro ) {
                xpro = pro;
            }
        }
        redun->pro = errpro;
        rx = redun + 1;
        if( state != restart )
            while( xpro != NULL ) {
                pro = xpro;
                xpro = NULL;
                Clear( rx->follow );
                for( i = 0; i < state->kersize; ++i ) {
                    ry = &state->name.state[i]->redun[at[i]];
                    if( ry->pro == pro ) {
                        Union( rx->follow, ry->follow );
                        ++(at[i]);
                        ++ry;
                    }
                    if( ry->pro > xpro ) {
                        xpro = ry->pro;
                    }
                }
                UnionAnd( conflict, rx->follow, defined );
                Union( defined, rx->follow );
                rx->pro = pro;
                ++rx;
            }
        xsym = NULL;
        for( i = 0; i < state->kersize; ++i ) {
            at[i] = 0;
            sym = state->name.state[i]->trans->sym;
            if( sym > xsym ) {
                xsym = sym;
            }
        }
        tx = trans;
        while( xsym != NULL ) {
            sym = xsym;
            xsym = NULL;
            t = s;
            for( i = 0; i < state->kersize; ++i ) {
                ty = &state->name.state[i]->trans[at[i]];
                if( ty->sym == sym ) {
                    if( !IsMarked( *ty->state ) ) {
                        Mark( *ty->state );
                        *t++ = ty->state;
                    }
                    ++(at[i]);
                    ++ty;
                }
                if( ty->sym > xsym ) {
                    xsym = ty->sym;
                }
            }
            tx->sym = sym;
            if( sym->pro != NULL ) {
                ++nvtrans;
            } else {
                if( IsBitSet( defined, sym->id ) ) {
                    SetBit( conflict, sym->id );
                    while( --t >= s ) {
                        Unmark( **t );
                    }
                    continue;
                } else {
                    SetBit( defined, sym->id );
                }
            }
            tx->state = AddErrState( &errsym->enter, s, t );
            ++tx;
        }
        state->trans = CALLOC( tx - trans + 1, a_shift_action );
        memcpy( state->trans, trans, (char *)tx - (char *)trans );
        if( Empty( conflict ) ) {
            redun->pro = NULL;
            i = 0;
        } else {
            i = 1;
        }
        while( --rx > redun ) {
            AndNot( rx->follow, conflict );
            if( Empty( rx->follow ) ) {
                rx->pro = NULL;
            } else {
                ++i;
            }
        }
        state->redun = CALLOC( i + 1, a_reduce_action );
        if( i > 0 ) {
            rset = AllocSet( i );
            rx = redun;
            while( i > 0 ) {
                if( rx->pro != NULL ) {
                    --i;
                    state->redun[i].pro = rx->pro;
                    state->redun[i].follow = rset;
                    Assign( rset, rx->follow );
                    rset += GetSetSize( 1 );
                }
                ++rx;
            }
        }
    }
    FREE( trans );
    FREE( redun );
    FreeSet( conflict );
    FREE( s );
    FREE( at );
}

