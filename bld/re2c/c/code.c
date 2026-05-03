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


#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "substr.h"
#include "globals.h"
#include "dfa.h"
#include "parser.h"
#include "mem.h"


// there must be at least one span in list;  all spans must cover
// same range

typedef struct BitMap {
    struct BitMap   *next;
    Go              *go;
    State           *on;
    uint            i;
    byte            m;
} BitMap;

typedef struct SCC {
    State       **top;
    State       **stk;
} SCC;

static BitMap       *bitmap_pool = NULL;
static const uint   cInfinity = ~0U;

static char *prtCh( Char c )
{
    static char b[5];

    switch( c ) {
    case '\'': return( "\\'" );
    case '\n': return( "\\n" );
    case '\t': return( "\\t" );
    case '\v': return( "\\v" );
    case '\b': return( "\\b" );
    case '\r': return( "\\r" );
    case '\f': return( "\\f" );
    case '\a': return( "\\a" );
    case '\\': return( "\\\\" );
    default:
        if( isprint( c ) ) {
            b[0] = (char)c;
            b[1] = '\0';
        } else {
            b[0] = '\\';
            b[1] = ( ( c / 64 ) % 8 ) + '0';
            b[2] = ( ( c / 8 ) % 8 ) + '0';
            b[3] = ( c % 8 ) + '0';
            b[4] = '\0';
        }
        return( b );
    }
}

static void Go_unmap( Go *g, Go *base, State *st )
{
    Span *s;
    Span *b;
    Span *e;
    Char lb;

    s = g->span;
    b = base->span;
    e = &b[base->nSpans];
    lb = 0;
    s->ub = 0;
    s->to = NULL;
    for( ; b != e; ++b ) {
        if( b->to == st ) {
            if( ( s->ub - lb ) > 1 ) {
                s->ub = b->ub;
            }
        } else {
            if( b->to != s->to ) {
                if( s->ub ) {
                    lb = s->ub;
                    ++s;
                }
                s->to = b->to;
            }
            s->ub = b->ub;
        }
    }
    s->ub = e[-1].ub;
    ++s;
    g->nSpans = (uint)( s - g->span );
}

static void doGen( Go *g, State *st, byte *bm, byte m )
{
    Span *b;
    Span *e;
    Char lb;

    b = g->span;
    e = &b[g->nSpans];
    lb = 0;
    for( ; b < e; ++b ) {
        if( b->to == st ) {
            for( ; lb < b->ub; ++lb ) {
                bm[lb] |= m;
            }
        }
        lb = b->ub;
    }
}

static bool matches( Go *g1, State *st1, Go *g2, State *st2 )
{
    Span *b1;
    Span *e1;
    Char lb1;
    Span *b2;
    Span *e2;
    Char lb2;

    b1 = g1->span;
    e1 = &b1[g1->nSpans];
    lb1 = 0;
    b2 = g2->span;
    e2 = &b2[g2->nSpans];
    lb2 = 0;
    for( ;; ) {
        for( ; b1 < e1 && b1->to != st1; ++b1 )
            lb1 = b1->ub;
        for( ; b2 < e2 && b2->to != st2; ++b2 )
            lb2 = b2->ub;
        if( b1 == e1 )
            return( b2 == e2 );
        if( b2 == e2 )
            return( false );
        if( lb1 != lb2
          || b1->ub != b2->ub )
            return( false );
        ++b1;
        ++b2;
    }
}

static BitMap *BitMap_new( Go *g, State *st )
{
    BitMap  *b;

    b = MemAlloc( sizeof( *b ) );
    b->go = g;
    b->on = st;
    b->next = bitmap_pool;
    bitmap_pool = b;
    return( b );
}

static BitMap *BitMap_find_go( Go *g, State *st )
{
    BitMap  *b;

    for( b = bitmap_pool; b != NULL; b = b->next ) {
        if( matches( b->go, b->on, g, st ) ) {
            return( b );
        }
    }
    return( BitMap_new( g, st ) );
}

static BitMap *BitMap_find( State *st )
{
    BitMap  *b;

    for( b = bitmap_pool; b != NULL; b = b->next ) {
        if( b->on == st ) {
            return( b );
        }
    }
    return( NULL );
}

static void BitMap_gen( FILE *o, Char lb, Char ub )
{
    BitMap *b;

    if( bitmap_pool != NULL ) {
        uint    n;
        byte    *bm;
        uint    i;
        uint    j;
        byte    m;

        n = ub - lb;
        bm = MemAlloc( n );
        memset( bm, 0, n );
        fputs( "\tstatic unsigned char yybm[] = {", o );
        b = bitmap_pool;
        for( i = 0; b != NULL; i += n ) {
            for( m = 0x80; b != NULL && m; b = b->next, m >>= 1 ) {
                b->i = i;
                b->m = m;
                doGen( b->go, b->on, &bm[-lb], m );
            }
            for( j = 0; j < n; ++j ) {
                if( ( j % 8 ) == 0 ) {
                    fputs( "\n\t", o );
                    ++oline;
                }
                fprintf( o, "%3u, ", (uint)bm[j] );
            }
        }
        fputs( "\n\t};\n", o );
        oline += 2;
        MemFree( bm );
    }
}

void BitMap_delete( void )
{
    while( bitmap_pool != NULL ) {
        BitMap  *next = bitmap_pool->next;
        MemFree( bitmap_pool );
        bitmap_pool = next;
    }
}

static void genGoTo( FILE *o, State *st )
{
    fprintf( o, "\tgoto yy%u;\n", st->label );
    ++oline;
}

static void genIf( FILE *o, char *cmp, Char v )
{
    fprintf( o, "\tif(yych %s '%s')", cmp, prtCh( v ) );
}

static void indent( FILE *o, uint i )
{
    while( i-- > 0 ) {
        fputc( '\t', o );
    }
}

static void need( FILE *o, uint n )
{
    if( n == 1 ) {
        fputs( "\tif(YYLIMIT == YYCURSOR) YYFILL(1);\n", o );
    } else {
        fprintf( o, "\tif((YYLIMIT - YYCURSOR) < %d) YYFILL(%d);\n", n, n );
    }
    ++oline;
    fputs( "\tyych = *YYCURSOR;\n", o );
    ++oline;
}

static void Action_emit( Action *a, FILE *o )
{
    uint    i;

    switch( a->type ) {
    case MATCHACT:
        if( a->state->link != NULL ) {
            fputs( "\t++YYCURSOR;\n", o );
            need( o, a->state->depth );
        } else {
            fputs("\tyych = *++YYCURSOR;\n", o);
        }
        oline++;
        break;
    case ENTERACT:
        if( a->state->link != NULL ) {
            need( o, a->state->depth );
        }
        break;
    case SAVEMATCHACT:
        fprintf(o, "\tyyaccept = %u;\n", a->u.SaveMatch.selector);
        oline++;
        if( a->state->link != NULL ) {
            fputs("\tYYMARKER = ++YYCURSOR;\n", o);
            oline++;
            need( o, a->state->depth );
        } else {
            fputs("\tyych = *(YYMARKER = ++YYCURSOR);\n", o);
            oline++;
        }
        break;
    case MOVEACT:
        break;
    case ACCEPTACT:
        {
            bool    first = true;

            for( i = 0; i < a->u.Accept.nRules; ++i ) {
                if( a->u.Accept.saves[i] != ~0u ) {
                    if( first ) {
                        first = false;
                        fputs( "\tYYCURSOR = YYMARKER;\n\tswitch(yyaccept){\n", o );
                        oline += 2;
                    }
                    fprintf( o, "\tcase %u:", a->u.Accept.saves[i] );
                    genGoTo( o, a->u.Accept.rules[i] );
                }
            }
            if( !first ) {
                fputs( "\t}\n", o );
                oline++;
            }
        }
        break;
    case RULEACT:
        {
            uint    back = RegExp_fixedLength( a->u.Rule.rule->u.RuleOp.ctx );

            if( back != ~0
              && back > 0 )
                fprintf( o, "\tYYCURSOR -= %u;", back );
            fputc( '\n', o );
            oline++;
            if( !iFlag ) {
                fprintf( o, "#line %u\n", a->u.Rule.rule->u.RuleOp.code->line );
                oline++;
            }
            fputc( '\t', o );
            SubStr_out( &a->u.Rule.rule->u.RuleOp.code->text, o );
            fputc( '\n', o );
            oline++;
        }
        break;
    }
}

static void doLinear( FILE *o, uint i, Span *s, uint n, State *st )
{
    for( ;; ) {
        State *st_bg = s[0].to;
        while( n >= 3 && s[2].to == st_bg && ( s[1].ub - s[0].ub ) == 1 ) {
            if( s[1].to == st
              && n == 3 ) {
                indent( o, i );
                genIf( o, "!=", s[0].ub );
                genGoTo( o, st_bg );
                return;
            } else {
                indent( o, i );
                genIf( o, "==", s[0].ub );
                genGoTo( o, s[1].to );
            }
            n -= 2;
            s += 2;
        }
        if( n == 1 ) {
            if( st_bg != st ) {
                indent( o, i);
                genGoTo(o, s[0].to );
            }
            return;
        } else if( n == 2
          && st_bg == st ) {
            indent( o, i );
            genIf( o, ">=", s[0].ub );
            genGoTo( o, s[1].to );
            return;
        } else {
            indent( o, i );
            genIf( o, ( s[0].ub > 1 ) ? "<=" : "==", s[0].ub - 1 );
            genGoTo( o, st_bg );
            n -= 1;
            s += 1;
        }
    }
}

static void Go_genLinear( Go *g, FILE *o, State *st )
{
    doLinear( o, 0, g->span, g->nSpans, st );
}

static void genCases( FILE *o, Char lb, Span *s )
{
    if( lb < s->ub ) {
        for( ;; ) {
            fprintf( o, "\tcase '%s':", prtCh( lb ) );
            if( ++lb == s->ub )
                break;
            fputc( '\n', o );
            oline++;
        }
    }
}

static void Go_genSwitch( Go *g, FILE *o, State *st )
{
    uint    i;

    if( g->nSpans <= 2 ) {
        Go_genLinear( g, o, st );
    } else {
        State   *st_def = g->span[g->nSpans - 1].to;
        Span    **r, **s, **t;
        Span    **sP = MemAlloc( ( g->nSpans - 1 ) * sizeof( *sP ) );

        t = &sP[0];
        for( i = 0; i < g->nSpans; ++i ) {
            if( g->span[i].to != st_def ) {
                *(t++) = &g->span[i];
            }
        }
        fputs( "\tswitch(yych){\n", o );
        ++oline;
        while( t != &sP[0] ) {
            State   *st_to;

            r = s = &sP[0];
            if( *s == &g->span[0] ) {
                genCases( o, 0, *s );
            } else {
                genCases( o, (*s)[-1].ub, *s );
            }
            st_to = (*s)->to;
            while( ++s < t ) {
                if( (*s)->to == st_to ) {
                    genCases( o, (*s)[-1].ub, *s );
                } else {
                    *(r++) = *s;
                }
            }
            genGoTo( o, st_to );
            t = r;
        }
        fputs( "\tdefault:", o );
        genGoTo(o, st_def);
        fputs( "\t}\n", o );
        ++oline;
        MemFree( sP );
    }
}

static void doBinary( FILE *o, uint i, Span *s, uint n, State *st )
{
    if( n <= 4 ) {
        doLinear( o, i, s, n, st );
    } else {
        uint    h = n/2;

        indent( o, i );
        genIf( o, "<=", s[h - 1].ub - 1 );
        fputs( "{\n", o );
        ++oline;
        doBinary( o, i + 1, &s[0], h, st );
        indent( o, i );
        fputs( "\t} else {\n", o );
        ++oline;
        doBinary( o, i + 1, &s[h], n - h, st );
        indent( o, i );
        fputs( "\t}\n", o );
        ++oline;
    }
}

static void Go_genBinary( Go *g, FILE *o, State *st )
{
    doBinary( o, 0, g->span, g->nSpans, st );
}

static void Go_genBase( Go *g, FILE *o, State *st )
{
    if( g->nSpans == 0 )
        return;
    if( !sFlag ) {
        Go_genSwitch( g, o, st );
        return;
    }
    if( g->nSpans > 8 ) {
        Span *bot = &g->span[0], *top = &g->span[g->nSpans - 1];
        uint util;
        if( bot[0].to == top[0].to ) {
            util = ( top[-1].ub - bot[0].ub ) / ( g->nSpans - 2 );
        } else {
            if( bot[0].ub > ( top[0].ub - top[-1].ub ) ) {
                util = ( top[0].ub - bot[0].ub ) / ( g->nSpans - 1 );
            } else {
                util = top[-1].ub / ( g->nSpans - 1 );
            }
        }
        if( util <= 2 ) {
            Go_genSwitch( g, o, st );
            return;
        }
    }
    if( g->nSpans > 5 ) {
        Go_genBinary( g, o, st );
    } else {
        Go_genLinear( g, o, st );
    }
}

static void Go_genGoto( Go *g, FILE *o, State *st )
{
    uint    i;

    if( bFlag ) {
        for( i = 0; i < g->nSpans; ++i ) {
            State *st_to = g->span[i].to;
            if( st_to != NULL
              && st_to->isBase ) {
                BitMap *b = BitMap_find( st_to );
                if( b != NULL
                  && matches( b->go, b->on, g, st_to ) ) {
                    Go go;
                    go.span = MemAlloc( g->nSpans * sizeof( *go.span ) );
                    Go_unmap( &go, g, st_to );
                    fprintf( o, "\tif(yybm[%u+yych] & %u)", b->i, (uint)b->m );
                    genGoTo( o, st_to );
                    Go_genBase( &go, o, st );
                    MemFree( go.span );
                    return;
                }
            }
        }
    }
    Go_genBase( g, o, st );
}

static void State_emit( State *st, FILE *o )
{
    if( st->referenced ) {
        fprintf( o, "yy%u: ", st->label );
    }
    Action_emit( st->action, o );
}

static uint merge( Span *x0, State *st_fg, State *st_bg )
{
    Span *x;
    Span *f;
    Span *b;
    uint nf;
    uint nb;
    State *st_prev;
    State *st_to;

    // NB: we assume both spans are for same range
    x = x0;
    f = st_fg->go.span;
    b = st_bg->go.span;
    nf = st_fg->go.nSpans;
    nb = st_bg->go.nSpans;
    st_prev = NULL;
    for( ;; ) {
        if( f->ub == b->ub ) {
            st_to = ( f->to == b->to ) ? st_bg : f->to;
            if( st_to == st_prev ) {
                --x;
            } else {
                x->to = st_prev = st_to;
            }
            x->ub = f->ub;
            ++x;
            ++f;
            --nf;
            ++b;
            --nb;
            if( nf == 0
              && nb == 0 ) {
                return( (uint)( x - x0 ) );
            }
        }
        while( f->ub < b->ub ) {
            st_to = ( f->to == b->to ) ? st_bg : f->to;
            if( st_to == st_prev ) {
                --x;
            } else {
                x->to = st_prev = st_to;
            }
            x->ub = f->ub;
            ++x;
            ++f;
            --nf;
        }
        while( b->ub < f->ub ) {
            st_to = ( b->to == f->to ) ? st_bg : f->to;
            if( st_to == st_prev ) {
                --x;
            } else {
                x->to = st_prev = st_to;
            }
            x->ub = b->ub;
            ++x;
            ++b;
            --nb;
        }
    }
}

static void SCC_init( SCC *scc, uint size )
{
    scc->top = scc->stk = (State **)MemAlloc( size * sizeof( *scc->stk ) );
}

static void SCC_fini( SCC *scc )
{
    MemFree( scc->stk );
}

static void SCC_traverse( SCC *scc, State *st )
{
    uint    k;
    uint    i;

    *scc->top = st;
    k = (uint)( ++scc->top - scc->stk );
    st->depth = k;
    for( i = 0; i < st->go.nSpans; ++i ) {
        State *st_to = st->go.span[i].to;
        if( st_to != NULL ) {
            if( st_to->depth == 0 )
                SCC_traverse( scc, st_to );
            if( st->depth > st_to->depth ) {
                st->depth = st_to->depth;
            }
        }
    }
    if( st->depth == k ) {
        do {
            (*--scc->top)->depth = cInfinity;
            (*scc->top)->link = st;
        } while( *scc->top != st );
    }
}

static uint maxDist( State *st )
{
    uint    mm;
    uint    i;

    mm = 0;
    for( i = 0; i < st->go.nSpans; ++i ) {
        State *st_to = st->go.span[i].to;

        if( st_to != NULL ) {
            uint m = 1;
            if( st_to->link == NULL )
                m += maxDist( st_to );
            if( m > mm ) {
                mm = m;
            }
        }
    }
    return( mm );
}

static void DFA_calcDepth( DFA *d )
{
    State   *st_to;
    State   *st;
    uint    i;

    for( st = d->head; st != NULL; st = st->next ) {
        if( st->link == st ) {
            for( i = 0; i < st->go.nSpans; ++i ) {
                st_to = st->go.span[i].to;
                if( st_to != NULL
                  && st_to->link == st ) {
                    st->depth = maxDist( st );
                    break;
                }
            }
            if( i == st->go.nSpans ) {
                st->link = NULL;
            }
        } else {
            st->depth = maxDist( st );
        }
    }
}

static void DFA_findSCCs( DFA *d )
{
    SCC     scc;
    State   *st;

    SCC_init( &scc, d->nStates );
    for( st = d->head; st != NULL; st = st->next ) {
        st->depth = 0;
        st->link = NULL;
    }
    for( st = d->head; st != NULL; st = st->next ) {
        if( st->depth == 0 ) {
            SCC_traverse( &scc, st );
        }
    }
    DFA_calcDepth( d );
    SCC_fini( &scc );
}

static void DFA_split( DFA *d, State *st )
{
    State *st_move;

    st_move = State_new();
    Action_new_Move( st_move );
    DFA_addState( d, &st->next, st_move );
    st_move->link = st->link;
    st_move->rule = st->rule;
    st_move->go = st->go;
    st->rule = NULL;
    st->go.nSpans = 1;
    st->go.span = MemAlloc( 1 * sizeof( *st->go.span ) );
    st->go.span[0].ub = d->ubChar;
    st->go.span[0].to = st_move;
}

static void tree_reference( State *st, int isBase )
{
    uint    i;

    if( st->referenced == 0 ) {
        if( isBase == 0 )
            st->referenced = 1;
        if( st->action->type == MATCHACT
          && st->go.nSpans == 1
          && st->go.span[0].to->action->type == RULEACT
          && st->go.span[0].to->go.nSpans == 0 ) {
        } else {
            for( i = 0; i < st->go.nSpans; i++ ) {
                tree_reference( st->go.span[i].to, st->isBase );
            }
        }
    }
}

void DFA_emit( DFA *d, FILE *o )
{
    State       *st;
    uint        i;
    uint        nRules;
    uint        nSaves;
    uint        *saves;
    State       **rules;
    State       *accept;
    Span        *span;

    bUsedYYAccept = false;
    DFA_findSCCs( d );
    d->head->link = d->head;
    d->head->depth = maxDist( d->head );

    nRules = 0;
    for( st = d->head; st != NULL; st = st->next ) {
        if( st->rule != NULL
          && st->rule->u.RuleOp.accept >= nRules ) {
            nRules = st->rule->u.RuleOp.accept + 1;
        }
    }

    saves = MemAlloc( nRules * sizeof( *saves ) );
    memset( saves, ~0, nRules * sizeof( *saves ) );

    // mark backtracking points
    nSaves = 0;
    for( st = d->head; st != NULL; st = st->next ) {
        if( st->rule != NULL ) {
            for( i = 0; i < st->go.nSpans; ++i ) {
                if( st->go.span[i].to != NULL
                  && st->go.span[i].to->rule == NULL ) {
                    Action_delete( st );
                    if( saves[st->rule->u.RuleOp.accept] == ~0 )
                        saves[st->rule->u.RuleOp.accept] = nSaves++;
                    Action_new_Save( st, saves[st->rule->u.RuleOp.accept] );
                    continue;
                }
            }
        }
    }

    // insert actions
    rules = MemAlloc( nRules * sizeof( *rules ) );
    memset( rules, 0, nRules * sizeof( *rules ) );
    accept = NULL;
    for( st = d->head; st != NULL; st = st->next ) {
        State *ow;
        if( st->rule == NULL ) {
            ow = accept;
        } else {
            if( rules[st->rule->u.RuleOp.accept] == NULL ) {
                State *st_n = State_new();
                Action_new_Rule( st_n, st->rule );
                DFA_addState( d, &st->next, st_n );
                rules[st->rule->u.RuleOp.accept] = st_n;
            }
            ow = rules[st->rule->u.RuleOp.accept];
        }
        for( i = 0; i < st->go.nSpans; ++i ) {
            if( st->go.span[i].to == NULL ) {
                if( ow == NULL ) {
                    ow = accept = State_new();
                    Action_new_Accept( accept, nRules, saves, rules );
                    DFA_addState( d, &st->next, accept );
                }
                st->go.span[i].to = ow;
            }
        }
    }

    // split ``base'' states into two parts
    for( st = d->head; st != NULL; st = st->next ) {
        st->isBase = false;
        if( st->link != NULL ) {
            for( i = 0; i < st->go.nSpans; ++i ) {
                if( st->go.span[i].to == st ) {
                    st->isBase = true;
                    DFA_split( d, st );
                    if( bFlag )
                        BitMap_find_go( &st->next->go, st );
                    st = st->next;
                    break;
                }
            }
        }
    }

    // find ``base'' state, if possible
    span = MemAlloc( ( d->ubChar - d->lbChar ) * sizeof( *span ) );
    for( st = d->head; st != NULL; st = st->next ) {
        if( st->link == NULL ) {
            for( i = 0; i < st->go.nSpans; ++i ) {
                State *st_to = st->go.span[i].to;
                if( st_to != NULL
                  && st_to->isBase ) {
                    uint    nSpans;

                    st_to = st_to->go.span[0].to;
                    nSpans = merge( span, st, st_to );
                    if( nSpans < st->go.nSpans ) {
                        MemFree( st->go.span );
                        st->go.nSpans = nSpans;
                        st->go.span = MemAlloc( nSpans * sizeof( *st->go.span ) );
                        memcpy( st->go.span, span, nSpans * sizeof( *st->go.span ) );
                    }
                    tree_reference( st, 0 );
                    break;
                }
            }
        }
    }
    MemFree( span );

    tree_reference( d->head, 0 );

    Action_delete( d->head );

    Action_new_Enter( d->head );

    fputs( "{\n\tYYCTYPE yych;\n", o );
    oline += 2;
    if( bUsedYYAccept ) {
        fputs( "\tunsigned int yyaccept = 0;\n", o );
        oline += 1;
    }
    if( bFlag ) {
        BitMap_gen( o, d->lbChar, d->ubChar );
        BitMap_delete();
    }

    fprintf( o, "\tgoto yy%u;\n", d->head->label );
    ++oline;

    for( st = d->head; st != NULL; st = st->next ) {
        State_emit( st, o );
        Go_genGoto( &st->go, o, st->next );
    }
    fputs( "}\n", o );
    ++oline;

    MemFree( rules );
    MemFree( saves );
}
