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
    ACTION_UNIT         = 0x4000,
    ACTION_NULL         = 0
};

#define static

typedef unsigned short  action_t;

static unsigned         maxTerminal;
static unsigned         maxNonTerminal;

typedef struct {
    unsigned            num_entries;
    unsigned            min;
    unsigned            max;
    unsigned            index;
    action_t            *action_vector;
} av_info;

typedef struct compressed_action {
    action_t            action;
    unsigned short      index;
} compressed_action;

static void assignAllTokenValues( void )
{
    unsigned i;
    unsigned ntoken;

    ntoken = MaxTerminalTokenValue();
    ++ntoken;
    maxTerminal = ntoken;
    for( i = nterm; i < nsym; ++i ) {
        symtab[i]->token = ntoken++;
    }
    maxNonTerminal = ntoken;
}

static unsigned insertIntoBitVector( byte **bv, unsigned *bs, byte *v, unsigned size )
{
    int i;
    int s;
    int ls;
    byte *p;

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
        if( memcmp( &p[ls-i], v, i ) == 0 ) {
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

static int actcmp( action_t *a1, compressed_action * ca, unsigned num_comp,
                                unsigned size )
{
    action_t v1,v2;
    unsigned i;

    for( i = 0; i < num_comp; ++i, ++ ca ) {
        if( ca->index >= size ) break;
        v2 = ca->action;
        // NB Know v2 != ACTION_NULL
        v1 = a1[ca->index];
        if( v1 == ACTION_NULL ) {
            continue;
        }
        if( v1 != v2 ) {
            return( ca->index+1 );
        }
    }
    return( 0 );
}

static void actcpy( action_t *a1, compressed_action * ca, unsigned num_comp )
{
    int i;

    for( i = 0; i < num_comp; ++i, ++ ca ) {
        a1[ca->index] = ca->action;
    }
}

static action_t * actextend( action_t * a, unsigned * psize, unsigned incr )
{
    unsigned    i;

    i = *psize;
    *psize += incr;
    if( *psize == 0 ) {
        a = MALLOC( incr, action_t );
    } else {
        a = REALLOC( a, *psize, action_t );
    }
    while( i < *psize ) {
        a[i++] = ACTION_NULL;
    }
    return( a );
}

static unsigned actcompress( compressed_action * ca, action_t * a, unsigned size )
{
    unsigned    i;
    unsigned    num_actions;

    num_actions = 0;
    for( i = 0; i < size; ++i ) {
        if( a[i] != ACTION_NULL ) {
            ca[num_actions].action = a[i];
            ca[num_actions].index = i;
            ++ num_actions;
        }
    }
    return( num_actions );
}

static unsigned insertIntoActionVector( action_t **bv, unsigned *bs,
                                compressed_action * ca, unsigned num_actions,
                                unsigned size )
{
    int i;
    int s;
    int ls;
    action_t c;
    action_t *p;

    if( num_actions == 0 ) {
        // no action items!
        return( 0 );
    }
    if( *bs == 0 ) {
        *bv = actextend( *bv, bs, size );
        actcpy( *bv, ca, num_actions );
        return( 0 );
    }
    p = *bv;
    ls = *bs;
    s = ( ls - size ) + 1;
    for( i = 0; i < s; ++i ) {
        // try a quick check with the last element that failed (may fail again!)
        c = p[i+ca[0].index];
        // we know v[bi-1] != ACTION_NULL
        if( c != ACTION_NULL && c != ca[0].action ) {
            continue;
        }
        if( actcmp( &p[i], ca, num_actions, size ) == 0 ) {
            // action vector was found inside large vector
            actcpy( &p[i], ca, num_actions );
            return( i );
        }
    }
    for( i = size; i > 0; --i ) {
        if( actcmp( &p[ls-i], ca, num_actions, i ) == 0 ) {
            // action vector has some common actions with the end of the large vector
            p = actextend( p, bs, size - i );
            *bv = p;
            actcpy( &p[ls-i], ca, num_actions );
            return( ls - i );
        }
    }
    // action vector has no common actions with large vector
    p = actextend( p, bs, size );
    *bv = p;
    actcpy( &p[ls], ca, num_actions );
    return( ls );
}

static action_t reduceaction( a_state *state, a_reduce_action *raction )
{
    action_t action;
    a_pro *pro;

    action = ACTION_REDUCE;
    pro = raction->pro;
    if( pro->unit && ! IsDontOptimize( *state ) ) {
        action |= ACTION_UNIT;
    }
    action |= pro->pidx;
    return( action );
}

static int cmp_action( const void *v1, const void *v2 )
{
    av_info **p1 = (av_info **) v1;
    av_info **p2 = (av_info **) v2;
    av_info *s1 = *p1;
    av_info *s2 = *p2;
    unsigned n1, n2;
    unsigned ne1, ne2;
    unsigned mx1, mx2;
    unsigned mn1, mn2;

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

static unsigned *orderActionVectors( action_t **av, unsigned size )
{
    av_info **a;
    av_info *p;
    action_t *v;
    unsigned num_entries;
    unsigned max;
    unsigned min;
    unsigned i, j;
    unsigned *map;

    a = MALLOC( nstate, av_info * );
    for( j = 0; j < nstate; ++j ) {
        v = av[j];
        p = MALLOC( 1, av_info );
        a[j] = p;
        max = 0;
        min = size;
        num_entries = 0;
        for( i = 0; i < size; ++i ) {
            if( v[i] != ACTION_NULL ) {
                if( num_entries == 0 ) {
                    min = i;
                }
                ++num_entries;
                max = i;
            }
        }
        p->min = min;
        p->max = max;
        p->num_entries = num_entries;
        p->index = j;
        p->action_vector = av[j];
    }
    qsort( a, nstate, sizeof( av_info * ), cmp_action );
    map = MALLOC( nstate, unsigned );
    for( i = 0; i < nstate; ++i ) {
        map[i] = a[i]->index;
        av[i] = a[i]->action_vector;
        free( a[i] );
        a[i] = NULL;
    }
    free( a );
    return( map );
}

static void createYACCTables( void )
{
    unsigned i;
    unsigned j;
    unsigned asize;
    unsigned index;
    unsigned mask;
    unsigned token;
    unsigned vsize;
    unsigned bsize;
    unsigned *mapping;
    value_size bitv_base_size;
    byte *state_vector;
    byte *bvector;
    compressed_action * ca;
    unsigned num_actions;
    short *p;
    unsigned *base;
    unsigned *abase;
    unsigned *gbase;
    action_t *state_actions;
    action_t *avector;
    action_t **all_actions;
    a_state *state;
    a_shift_action *saction;
    a_reduce_action *raction;
    a_sym *sym;
    a_pro *pro;
    an_item *first_item;
    an_item *item;
    unsigned empty_actions;
    unsigned *defaction;
    index_t state_idx;

    bvector = NULL;
    bsize = 0;
    vsize = ( maxTerminal + (8-1) ) / 8 + 1;
    state_vector = MALLOC( vsize, byte );
    base = CALLOC( nstate, unsigned );
    for( i = 0; i < nstate; ++i ) {
        state = statetab[i];
        memset( state_vector, 0, vsize );
        // iterate over all shifts in state
        for( saction = state->trans; ; ++saction ) {
            sym = saction->sym;
            if( sym == NULL ) break;
            if( sym->pro != NULL ) {
                // we only want terminals
                continue;
            }
            if( saction->is_default ) {
                // we want these to be default actions
                continue;
            }
            token = sym->token;
            index = token >> 3;
            mask = 1 << ( token & 0x07 );
            state_vector[index] |= mask;
        }
        // iterate over all reductions in state
        for( raction = state->redun; ; ++raction ) {
            pro = raction->pro;
            if( pro == NULL ) break;
            if( state->default_reduction == raction ) continue;
            for( p = Members( raction->follow, setmembers ); --p >= setmembers; ) {
                token = symtab[*p]->token;
                index = token >> 3;
                mask = 1 << ( token & 0x07 );
                state_vector[index] |= mask;
            }
        }
        base[i] = insertIntoBitVector( &bvector, &bsize, state_vector, vsize );
    }
    defaction = CALLOC( nstate, unsigned );
    all_actions = MALLOC( nstate, action_t * );
    for( i = 0; i < nstate; ++i ) {
        state = statetab[i];
        state_actions = MALLOC( maxTerminal, action_t );
        all_actions[i] = state_actions;
        for( j = 0; j < maxTerminal; ++j ) {
            state_actions[j] = ACTION_NULL;
        }
        // iterate over all shifts in state
        for( saction = state->trans; ; ++saction ) {
            sym = saction->sym;
            if( sym == NULL ) break;
            if( sym->pro != NULL ) continue;
            state_idx = saction->state->sidx;
            if( saction->is_default ) {
                defaction[ i ] = state_idx;
                continue;
            }
            token = sym->token;
            state_actions[token] = state_idx;
        }
        // iterate over all reductions in state
        for( raction = state->redun; ; ++raction ) {
            pro = raction->pro;
            if( pro == NULL ) break;
            if( state->default_reduction == raction ) {
                defaction[i] = reduceaction( state, raction );
                continue;
            }
            for( p = Members( raction->follow, setmembers ); --p >= setmembers; ) {
                token = symtab[*p]->token;
                state_actions[token] = reduceaction( state, raction );
            }
        }
    }
    mapping = orderActionVectors( all_actions, maxTerminal );
    avector = NULL;
    asize = 0;
    ca = CALLOC( maxTerminal, compressed_action );
    abase = CALLOC( nstate, unsigned );
    for( i = 0; i < nstate; ++i ) {
        num_actions = actcompress( ca, all_actions[i], maxTerminal );
        abase[mapping[i]] = insertIntoActionVector( &avector, &asize,
                                        ca, num_actions, maxTerminal );
        free( all_actions[i] );
        all_actions[i] = NULL;
    }
    free( mapping );
    free( ca );
    for( i = 0; i < nstate; ++i ) {
        state = statetab[i];
        state_actions = MALLOC( maxNonTerminal, action_t );
        all_actions[i] = state_actions;
        for( j = 0; j < maxNonTerminal; ++j ) {
            state_actions[j] = ACTION_NULL;
        }
        // iterate over all shifts in state
        for( saction = state->trans; ; ++saction ) {
            sym = saction->sym;
            if( sym == NULL ) break;
            if( sym->pro == NULL ) continue;
            token = sym->token;
            state_actions[token] = saction->state->sidx;
        }
    }
    mapping = orderActionVectors( all_actions, maxNonTerminal );
    ca = CALLOC( maxNonTerminal, compressed_action );
    gbase = CALLOC( nstate, unsigned );
    for( i = 0; i < nstate; ++i ) {
        num_actions = actcompress( ca, all_actions[i], maxNonTerminal );
        gbase[mapping[i]] = insertIntoActionVector( &avector, &asize,
                                        ca, num_actions, maxNonTerminal );
        free( all_actions[i] );
        all_actions[i] = NULL;
    }
    free( mapping );
    free( ca );
    putcomment( "index by state to get default action for state" );
    begtab( "YYACTIONTYPE", "yydefaction" );
    for( i = 0; i < nstate; ++i ) {
        puttab( FITS_A_WORD, defaction[i] );
    }
    endtab();
    free( defaction );
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
        first_item = protab[i]->item;
        for( item = first_item; item->p.sym; ++item )
          /* do nothing */;
        puttab( FITS_A_BYTE, item - first_item );
    }
    endtab();
    putcomment( "index by rule to get left hand side token" );
    begtab( "YYPLHSTYPE", "yyplhstab" );
    for( i = 0; i < npro; ++i ) {
        puttab( FITS_A_WORD, protab[i]->sym->token );
    }
    endtab();
    free( all_actions );
    free( base );
    free( abase );
    free( gbase );
    free( bvector );
    free( avector );
    free( state_vector );
    dumpstatistic( "bytes used in tables", bytesused );
    dumpstatistic( "table space utilization", 100 - ( empty_actions * 100L / asize ) );
}

void GenFastTables( void )
{
    assignAllTokenValues();
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
    putambigs( NULL );
    createYACCTables();
    puttokennames( 0, FITS_A_WORD );
}
