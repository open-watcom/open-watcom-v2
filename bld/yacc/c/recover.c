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
    a_sym           *xsym;
    a_sym           *sym;
    a_pro           *xpro;
    a_pro           *pro;
    a_state         *state;
    a_state         **s;
    a_state         **t;
    a_shift_action  *saction;
    a_shift_action  *ty;
    a_shift_action  *trans;
    a_reduce_action *raction;
    a_reduce_action *ry;
    a_reduce_action *redun;
    int             i;
    a_word          *defined;
    a_word          *conflict;
    a_word          *rset;
    short           *at;
    rule_n          pidx;

    trans = CALLOC( nsym, a_shift_action );
    raction = redun = CALLOC( npro + 1, a_reduce_action );
    rset = conflict = AllocSet( npro + 2 );
    for( pidx = 0; pidx <= npro; ++pidx ) {
        raction->follow = rset;
        ++raction;
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
        raction = redun + 1;
        if( state != restart ) {
            while( xpro != NULL ) {
                pro = xpro;
                xpro = NULL;
                Clear( raction->follow );
                for( i = 0; i < state->kersize; ++i ) {
                    ry = &state->name.state[i]->redun[at[i]];
                    if( ry->pro == pro ) {
                        Union( raction->follow, ry->follow );
                        ++(at[i]);
                        ++ry;
                    }
                    if( ry->pro > xpro ) {
                        xpro = ry->pro;
                    }
                }
                UnionAnd( conflict, raction->follow, defined );
                Union( defined, raction->follow );
                raction->pro = pro;
                ++raction;
            }
        }
        xsym = NULL;
        for( i = 0; i < state->kersize; ++i ) {
            at[i] = 0;
            sym = state->name.state[i]->trans->sym;
            if( sym > xsym ) {
                xsym = sym;
            }
        }
        saction = trans;
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
            saction->sym = sym;
            if( sym->pro != NULL ) {
                ++nvtrans;
            } else {
                if( IsBitSet( defined, sym->id, WSIZE ) ) {
                    SetBit( conflict, sym->id, WSIZE );
                    while( --t >= s ) {
                        Unmark( **t );
                    }
                    continue;
                } else {
                    SetBit( defined, sym->id, WSIZE );
                }
            }
            saction->state = AddErrState( &errsym->enter, s, t );
            ++saction;
        }
        state->trans = CALLOC( ( saction - trans ) + 1, a_shift_action );
        memcpy( state->trans, trans, ( saction - trans ) * sizeof( a_shift_action ) );
        if( Empty( conflict ) ) {
            redun->pro = NULL;
            i = 0;
        } else {
            i = 1;
        }
        while( --raction > redun ) {
            AndNot( raction->follow, conflict );
            if( Empty( raction->follow ) ) {
                raction->pro = NULL;
            } else {
                ++i;
            }
        }
        state->redun = CALLOC( i + 1, a_reduce_action );
        if( i > 0 ) {
            rset = AllocSet( i );
            raction = redun;
            while( i > 0 ) {
                if( raction->pro != NULL ) {
                    --i;
                    state->redun[i].pro = raction->pro;
                    state->redun[i].follow = rset;
                    Assign( rset, raction->follow );
                    rset += GetSetSize( 1 );
                }
                ++raction;
            }
        }
    }
    FREE( trans );
    FREE( redun );
    FreeSet( conflict );
    FREE( s );
    FREE( at );
}

