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
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include "yacc.h"
#include "alloc.h"

enum {
    ACTION_REDUCE       = 0x8000,
    ACTION_UNIT         = 0x4000
};

typedef struct {
    index_n             num_entries;
    token_n             min;
    token_n             max;
    action_n            index;
    action_n            *action_vector;
} av_info;

typedef struct compressed_action {
    action_n            action;
    token_n             token;
} compressed_action;

static index_n insertIntoBitVector( byte **bv, index_n *bs, byte *v, index_n size )
{
    index_n     i;
    index_n     s;
    index_n     ls;
    byte        *p;

    if( *bv == NULL ) {
        *bs = size;
        *bv = MALLOC( size, byte );
        memcpy( *bv, v, size );
        return( 0 );
    }
    p = *bv;
    ls = *bs;
    s = ( ls - size ) + 1;
    for( i = 0; i < s; ++i ) {
        if( memcmp( &p[i], v, size ) == 0 ) {
            // bitvector was found inside large vector
            return( i );
        }
    }
    for( i = size; i > 0; --i ) {
        if( memcmp( &p[ls - i], v, i ) == 0 ) {
            // bitvector has some common bits with the end of the large vector
            *bs += size - i;
            p = REALLOC( p, *bs, byte );
            *bv = p;
            memcpy( &p[ls], &v[i], size - i );
            return( ls - i );
        }
    }
    // bitvector has no common bits with large vector
    *bs += size;
    p = REALLOC( p, *bs, byte );
    *bv = p;
    memcpy( &p[ls], v, size );
    return( ls );
}

static int actcmp( action_n *actions, compressed_action *ca, index_n num_actions, token_n ntoken )
{
    action_n    a1, a2;
    index_n     i;
    token_n     ca_token;

    for( i = 0; i < num_actions; ++ca, ++i ) {
        ca_token = ca->token;
        if( ca_token >= ntoken )
            break;
        a1 = actions[ca_token];
        if( a1 == ACTION_NULL ) {
            continue;
        }
        // NB Know a2 != ACTION_NULL
        a2 = ca->action;
        if( a1 != a2 ) {
            return( ca_token + 1 );
        }
    }
    return( 0 );
}

static void actcpy( action_n *actions, compressed_action *ca, index_n num_actions )
{
    index_n i;

    for( i = 0; i < num_actions; ++ca, ++i ) {
        actions[ca->token] = ca->action;
    }
}

static action_n *actextend( action_n *a, index_n *psize, token_n incr )
{
    index_n i;

    i = *psize;
    *psize += incr;
    if( i == 0 ) {
        a = MALLOC( incr, action_n );
    } else {
        a = REALLOC( a, *psize, action_n );
    }
    while( i < *psize ) {
        a[i++] = ACTION_NULL;
    }
    return( a );
}

static index_n actcompress( compressed_action *ca, action_n *actions, token_n ntoken )
{
    token_n     token;
    index_n     num_actions;

    num_actions = 0;
    for( token = 0; token < ntoken; ++token ) {
        if( actions[token] != ACTION_NULL ) {
            ca[num_actions].action = actions[token];
            ca[num_actions].token = token;
            ++num_actions;
        }
    }
    return( num_actions );
}

static index_n insertIntoActionVector( action_n **bv, index_n *bs,
        compressed_action *ca, index_n num_actions, token_n ntoken )
{
    index_n  i;
    index_n  s;
    index_n  ls;
    action_n action;
    action_n *p;

    if( num_actions == 0 ) {
        // no action items!
        return( 0 );
    }
    ls = *bs;
    p = *bv;
    if( ls >= ntoken ) {
        // try action vector has common actions inside large vector
        s = ( ls - ntoken ) + 1;
        for( i = 0; i < s; ++i ) {
            // try a quick check with the last element that failed (may fail again!)
            // we know ca[0].action != ACTION_NULL
            action = p[i + ca[0].token];
            if( action == ACTION_NULL || action == ca[0].action ) {
                if( actcmp( &p[i], ca, num_actions, ntoken ) == 0 ) {
                    // action vector was found inside large vector
                    actcpy( &p[i], ca, num_actions );
                    return( i );
                }
            }
        }
    }
    // try action vector has some common actions with the end of the large vector
    i = ntoken;
    if( ls < ntoken ) {
        i = ls;
    }
    for( ; i > 0; --i ) {
        if( actcmp( &p[ls - i], ca, num_actions, i ) == 0 ) {
            // action vector has some common actions with the end of the large vector
            ntoken -= i;
            ls -= i;
            break;
        }
    }
    // add action vector to the large vector
    p = actextend( p, bs, ntoken );
    *bv = p;
    actcpy( &p[ls], ca, num_actions );
    return( ls );
}

static action_n reduceaction( a_state *state, a_reduce_action *raction )
{
    action_n    action;
    a_pro       *pro;

    action = ACTION_REDUCE;
    pro = raction->pro;
    if( pro->unit && ! IsDontOptimize( state ) ) {
        action |= ACTION_UNIT;
    }
    if( (pro->pidx & ACTION_MASK) != pro->pidx ) {
        printf( "Error: reduce action 0x%X is higher then 0x3FFF !\n", pro->pidx );
    }
    action |= pro->pidx;
    return( action );
}

static int cmp_action( const void *a1, const void *a2 )
{
    av_info **p1 = (av_info **)a1;
    av_info **p2 = (av_info **)a2;
    av_info *s1 = *p1;
    av_info *s2 = *p2;
    token_n n1, n2;
    token_n ne1, ne2;
    token_n mx1, mx2;
    token_n mn1, mn2;

    ne1 = s1->num_entries;
    ne2 = s2->num_entries;
    mx1 = s1->max;
    mx2 = s2->max;
    mn1 = s1->min;
    mn2 = s2->min;
    if( ne1 == 0 ) {
        n1 = 0;
    } else {
        n1 = ( mx1 - mn1 ) + 1;
    }
    if( ne2 == 0 ) {
        n2 = 0;
    } else {
        n2 = ( mx2 - mn2 ) + 1;
    }
    if( n1 < n2 ) {
        return( 1 );
    }
    if( n1 > n2 ) {
        return( -1 );
    }
    if( ne1 < ne2 ) {
        return( -1 );
    }
    if( ne1 > ne2 ) {
        return( 1 );
    }
    if( mx1 < mx2 ) {
        return( -1 );
    }
    if( mx1 > mx2 ) {
        return( 1 );
    }
    if( s1->index < s2->index ) {
        return( 1 );
    }
    if( s1->index > s2->index ) {
        return( -1 );
    }
    return( 0 );
}

static action_n *orderActionVectors( action_n **av, token_n ntoken )
{
    av_info **a;
    av_info *p;
    action_n *actions;
    index_n num_entries;
    token_n max;
    token_n min;
    token_n token;
    action_n i;
    action_n *map;

    a = MALLOC( nstate, av_info * );
    for( i = 0; i < nstate; ++i ) {
        actions = av[i];
        p = MALLOC( 1, av_info );
        a[i] = p;
        max = 0;
        min = ntoken;
        num_entries = 0;
        for( token = 0; token < ntoken; ++token ) {
            if( actions[token] != ACTION_NULL ) {
                if( num_entries == 0 ) {
                    min = token;
                }
                max = token;
                ++num_entries;
            }
        }
        p->min = min;
        p->max = max;
        p->num_entries = num_entries;
        p->index = i;
        p->action_vector = actions;
    }
    qsort( a, nstate, sizeof( av_info * ), cmp_action );
    map = MALLOC( nstate, action_n );
    for( i = 0; i < nstate; ++i ) {
        map[i] = a[i]->index;
        av[i] = a[i]->action_vector;
        FREE( a[i] );
        a[i] = NULL;
    }
    FREE( a );
    return( map );
}

void GenFastTables( void )
{
    index_n     i;
    index_n     j;
    index_n     asize;
    token_n     tokval;
    index_n     vsize;
    index_n     bsize;
    action_n    *mapping;
    value_size  bitv_base_size;
    byte        *state_vector;
    byte        *bvector;
    compressed_action *ca;
    index_n     num_actions;
    set_size    *mp;
    index_n     *base;
    index_n     *abase;
    index_n     *gbase;
    action_n    *state_actions;
    action_n    *avector;
    action_n    **all_actions;
    a_state     *state;
    a_shift_action *saction;
    a_reduce_action *raction;
    a_sym       *sym;
    a_pro       *pro;
    an_item     *item;
    index_n     empty_actions;
    action_n    *defaction;
    action_n    state_idx;
    token_n     ntoken_term;
    token_n     ntoken_all;


    ntoken_term = FirstNonTerminalTokenValue();
    ntoken_all = ntoken_term;
    for( i = nterm; i < nsym; ++i ) {
        symtab[i]->token = ntoken_all++;
    }

    bvector = NULL;
    bsize = 0;
    vsize = ( ntoken_term + ( 8 - 1 ) ) / 8;
    state_vector = MALLOC( vsize, byte );
    base = CALLOC( nstate, index_n );
    for( i = 0; i < nstate; ++i ) {
        state = statetab[i];
        memset( state_vector, 0, vsize );
        // iterate over all shifts in state
        for( saction = state->trans; (sym = saction->sym) != NULL; ++saction ) {
            if( sym->pro != NULL ) {
                // we only want terminals
                continue;
            }
            if( saction->is_default ) {
                // we want these to be default actions
                continue;
            }
            tokval = sym->token;
            state_vector[tokval >> 3] |= 1 << ( tokval & 0x07 );
        }
        // iterate over all reductions in state
        for( raction = state->redun; (pro = raction->pro) != NULL; ++raction ) {
            if( state->default_reduction == raction )
                continue;
            for( mp = Members( raction->follow ); mp != setmembers; ) {
                --mp;
                tokval = symtab[*mp]->token;
                state_vector[tokval >> 3] |= 1 << ( tokval & 0x07 );
            }
        }
        base[i] = insertIntoBitVector( &bvector, &bsize, state_vector, vsize );
    }
    FREE( state_vector );

    defaction = CALLOC( nstate, action_n );
    all_actions = MALLOC( nstate, action_n * );
    for( i = 0; i < nstate; ++i ) {
        state = statetab[i];
        state_actions = MALLOC( ntoken_term, action_n );
        all_actions[i] = state_actions;
        for( j = 0; j < ntoken_term; ++j ) {
            state_actions[j] = ACTION_NULL;
        }
        // iterate over all shifts in state
        for( saction = state->trans; (sym = saction->sym) != NULL; ++saction ) {
            if( sym->pro != NULL )
                continue;
            state_idx = saction->state->sidx;
            if( saction->is_default ) {
                defaction[i] = state_idx;
                continue;
            }
            state_actions[sym->token] = state_idx;
        }
        // iterate over all reductions in state
        for( raction = state->redun; (pro = raction->pro) != NULL; ++raction ) {
            if( state->default_reduction == raction ) {
                defaction[i] = reduceaction( state, raction );
                continue;
            }
            for( mp = Members( raction->follow ); mp != setmembers; ) {
                --mp;
                state_actions[symtab[*mp]->token] = reduceaction( state, raction );
            }
        }
    }
    mapping = orderActionVectors( all_actions, ntoken_term );
    avector = NULL;
    asize = 0;
    ca = CALLOC( ntoken_term, compressed_action );
    abase = CALLOC( nstate, set_size );
    for( i = 0; i < nstate; ++i ) {
        num_actions = actcompress( ca, all_actions[i], ntoken_term );
        abase[mapping[i]] = insertIntoActionVector( &avector, &asize, ca, num_actions, ntoken_term );
        FREE( all_actions[i] );
        all_actions[i] = NULL;
    }
    FREE( mapping );
    FREE( ca );
    for( i = 0; i < nstate; ++i ) {
        state = statetab[i];
        state_actions = MALLOC( ntoken_all, action_n );
        all_actions[i] = state_actions;
        for( j = 0; j < ntoken_all; ++j ) {
            state_actions[j] = ACTION_NULL;
        }
        // iterate over all shifts in state
        for( saction = state->trans; (sym = saction->sym) != NULL; ++saction ) {
            if( sym->pro == NULL )
                continue;
            state_actions[sym->token] = saction->state->sidx;
        }
    }
    mapping = orderActionVectors( all_actions, ntoken_all );
    ca = CALLOC( ntoken_all, compressed_action );
    gbase = CALLOC( nstate, set_size );
    for( i = 0; i < nstate; ++i ) {
        num_actions = actcompress( ca, all_actions[i], ntoken_all );
        gbase[mapping[i]] = insertIntoActionVector( &avector, &asize, ca, num_actions, ntoken_all );
        FREE( all_actions[i] );
        all_actions[i] = NULL;
    }
    FREE( mapping );
    FREE( ca );

    FREE( all_actions );

    putambigs( NULL );

    putnum( "YYNOACTION", 0 );
    putnum( "YYEOFTOKEN", eofsym->token );
    putnum( "YYERRTOKEN", errsym->token );
    putnum( "YYETOKEN", errsym->token );
    putnum( "YYSTART", startstate->sidx );
    putnum( "YYSTOP", eofsym->enter->sidx );
    putnum( "YYERR", errstate->sidx );
    putnum( "YYUSED", nstate );
    if( keyword_id_low != 0 && default_shiftflag ) {
        putnum( "YYKEYWORD_ID_LOW", keyword_id_low );
        putnum( "YYKEYWORD_ID_HIGH", keyword_id_high );
    }

    putcomment( "index by state to get default action for state" );
    begtab( "YYACTIONTYPE", "yydefaction" );
    for( i = 0; i < nstate; ++i ) {
        puttab( FITS_A_WORD, defaction[i] );
    }
    endtab();
    FREE( defaction );
    bitv_base_size = FITS_A_WORD;
    if( bsize < 257 ) {
        bitv_base_size = FITS_A_BYTE;
    }
    putcomment( "index by state to get offset into bit vector" );
    begtab( "YYBITBASETYPE", "yybitbase" );
    for( i = 0; i < nstate; ++i ) {
        puttab( bitv_base_size, base[i] );
    }
    endtab();
    putcomment( "index by token (from state base) to see if token is valid in state" );
    begtab( "YYBITTYPE", "yybitcheck" );
    for( i = 0; i < bsize; ++i ) {
        puttab( FITS_A_BYTE, bvector[i] );
    }
    endtab();
    putcomment( "index by state to get offset into action vector" );
    begtab( "YYACTIONBASETYPE", "yyactionbase" );
    for( i = 0; i < nstate; ++i ) {
        puttab( FITS_A_WORD, abase[i] );
    }
    endtab();
    putcomment( "index by state to get offset into action vector" );
    begtab( "YYACTIONBASETYPE", "yygotobase" );
    for( i = 0; i < nstate; ++i ) {
        puttab( FITS_A_WORD, gbase[i] );
    }
    endtab();
    putcomment( "index by token (from state base) to get action for state" );
    empty_actions = 0;
    begtab( "YYACTIONTYPE", "yyaction" );
    for( i = 0; i < asize; ++i ) {
        if( avector[i] == ACTION_NULL ) {
            ++empty_actions;
        }
        puttab( FITS_A_WORD, avector[i] );
    }
    endtab();
    putcomment( "index by rule to get length of rule" );
    begtab( "YYPLENTYPE", "yyplentab" );
    for( i = 0; i < npro; ++i ) {
        for( item = protab[i]->items; item->p.sym != NULL; ) {
            ++item;
        }
        puttab( FITS_A_BYTE, (unsigned)( item - protab[i]->items ) );
    }
    endtab();
    putcomment( "index by rule to get left hand side token" );
    begtab( "YYPLHSTYPE", "yyplhstab" );
    for( i = 0; i < npro; ++i ) {
        puttab( FITS_A_WORD, protab[i]->sym->token );
    }
    endtab();

    FREE( base );
    FREE( abase );
    FREE( gbase );
    FREE( bvector );
    FREE( avector );

    dumpstatistic( "bytes used in tables", bytesused );
    dumpstatistic( "table space utilization", 100 - ( empty_actions * 100L / asize ) );

    puttokennames( 0, FITS_A_WORD );

    FREE( protab );
    FREE( symtab );
}
