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
#include <ctype.h>
#include <stdio.h>
#include "substr.h"
#include "globals.h"
#include "dfa.h"
#include "parser.h"

// there must be at least one span in list;  all spans must cover
// same range

typedef struct BitMap {
    Go              *go;
    State           *on;
    struct BitMap   *next;
    uint            i;
    uchar           m;
} BitMap;

typedef struct SCC {
    State       **top, **stk;
} SCC;

static BitMap       *BitMap_first = NULL;
static const uint   cInfinity = ~0;

static char *prtCh( uchar c )
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
            b[0] = c;
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

static void Go_unmap( Go *g, Go *base, State *x )
{
    Span *s = g->span, *b = base->span, *e = &b[base->nSpans];
    uint lb = 0;

    s->ub = 0;
    s->to = NULL;
    for(; b != e; ++b){
        if(b->to == x){
            if((s->ub - lb) > 1)
                s->ub = b->ub;
        } else {
            if(b->to != s->to){
                if(s->ub){
                    lb = s->ub; ++s;
                }
                s->to = b->to;
            }
            s->ub = b->ub;
        }
    }
    s->ub = e[-1].ub; ++s;
    g->nSpans = s - g->span;
}

static void doGen( Go *g, State *s, uchar *bm, uchar m )
{
    Span *b = g->span, *e = &b[g->nSpans];
    uint lb = 0;
    for(; b < e; ++b){
        if(b->to == s)
            for(; lb < b->ub; ++lb) bm[lb] |= m;
        lb = b->ub;
    }
}

static bool matches( Go *g1, State *s1, Go *g2, State *s2 )
{
    Span *b1 = g1->span, *e1 = &b1[g1->nSpans];
    uint lb1 = 0;
    Span *b2 = g2->span, *e2 = &b2[g2->nSpans];
    uint lb2 = 0;
    for( ;; ){
        for( ; b1 < e1 && b1->to != s1; ++b1 )
            lb1 = b1->ub;
        for( ; b2 < e2 && b2->to != s2; ++b2 )
            lb2 = b2->ub;
        if( b1 == e1 )
            return( b2 == e2 );
        if( b2 == e2 )
            return( FALSE );
        if( lb1 != lb2 || b1->ub != b2->ub )
            return( FALSE );
        ++b1; ++b2;
    }
}

static BitMap *BitMap_new( Go *g, State *x )
{
    BitMap  *b = malloc( sizeof( BitMap ) );

    b->go = g;
    b->on = x;
    b->next = BitMap_first;
    BitMap_first = b;
    return( b );
}

static BitMap *BitMap_find_go( Go *g, State *x )
{
    BitMap  *b;

    for( b = BitMap_first; b != NULL; b = b->next ) {
        if( matches( b->go, b->on, g, x ) ) {
            return( b );
        }
    }
    return( BitMap_new( g, x ) );
}

static BitMap *BitMap_find( State *x )
{
    BitMap  *b;

    for( b = BitMap_first; b != NULL; b = b->next ) {
        if( b->on == x ) {
            return( b );
        }
    }
    return( NULL );
}

static void BitMap_gen( FILE *o, uint lb, uint ub )
{
    BitMap *b = BitMap_first;

    if( b != NULL ){
        uint    n = ub - lb;
        uchar   *bm = malloc( n );
        uint    i, j;
        uchar   m;

        fputs( "\tstatic unsigned char yybm[] = {", o );
        memset( bm, 0, n );
        for( i = 0; b != NULL; i += n ) {
            for( m = 0x80; b != NULL && m; b = b->next, m >>= 1 ) {
                b->i = i; b->m = m;
                doGen( b->go, b->on, &bm[-lb], m );
            }
            for( j = 0; j < n; ++j ) {
                if( j % 8 == 0 ) {
                    fputs( "\n\t", o );
                    ++oline;
                }
                fprintf( o, "%3u, ", (uint)bm[j] );
            }
        }
        fputs( "\n\t};\n", o );
        oline += 2;
        free( bm );
    }
}

static void genGoTo( FILE *o, State *to )
{
    fprintf( o, "\tgoto yy%u;\n", to->label );
    ++oline;
}

static void genIf( FILE *o, char *cmp, uint v )
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
    int     i;

    switch( a->type ) {
    case MATCHACT:
        if( a->state->link ){
            fputs( "\t++YYCURSOR;\n", o );
            need( o, a->state->depth );
        } else {
            fputs("\tyych = *++YYCURSOR;\n", o);
        }
        oline++;
        break;
    case ENTERACT:
        if( a->state->link ) {
            fputs( "\t++YYCURSOR;\n", o);
        } else {
            fputs("\tyych = *++YYCURSOR;\n", o);
        }
        fprintf(o, "yy%u:\n", a->u.Enter.label);
        oline += 2;
        if( a->state->link ) {
            need( o, a->state->depth );
        }
        break;
    case SAVEMATCHACT:
        fprintf(o, "\tyyaccept = %u;\n", a->u.SaveMatch.selector);
        oline++;
        if( a->state->link ){
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
            bool    first = TRUE;

            for( i = 0; i < a->u.Accept.nRules; ++i ) {
                if( a->u.Accept.saves[i] != ~0u ) {
                    if( first ) {
                        first = FALSE;
                        bUsedYYAccept = TRUE;
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

            if( back != ~0 && back > 0 )
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

static void doLinear( FILE *o, uint i, Span *s, uint n, State *next )
{
    for( ;; ) {
        State *bg = s[0].to;
        while( n >= 3 && s[2].to == bg && ( s[1].ub - s[0].ub ) == 1 ) {
            if( s[1].to == next && n == 3 ) {
                indent( o, i ); genIf( o, "!=", s[0].ub ); genGoTo( o, bg );
                return;
            } else {
                indent( o, i ); genIf( o, "==", s[0].ub ); genGoTo( o, s[1].to );
            }
            n -= 2; s += 2;
        }
        if( n == 1 ) {
            if( bg != next ) {
                indent( o, i); genGoTo(o, s[0].to );
            }
            return;
        } else if( n == 2 && bg == next ) {
            indent( o, i ); genIf( o, ">=", s[0].ub ); genGoTo( o, s[1].to );
            return;
        } else {
            indent( o, i ); genIf( o, ( s[0].ub > 1 ) ? "<=" : "==", s[0].ub - 1 ); genGoTo( o, bg );
            n -= 1; s += 1;
        }
    }
}

static void Go_genLinear( Go *g, FILE *o, State *next )
{
    doLinear( o, 0, g->span, g->nSpans, next );
}

static void genCases( FILE *o, uint lb, Span *s )
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

static void Go_genSwitch( Go *g, FILE *o, State *next )
{
    uint    i;

    if( g->nSpans <= 2 ) {
        Go_genLinear( g, o, next );
    } else {
        State   *def = g->span[g->nSpans - 1].to;
        Span    **r, **s, **t;
        Span    **sP = malloc( ( g->nSpans - 1 ) * sizeof( Span * ) );

        t = &sP[0];
        for( i = 0; i < g->nSpans; ++i ) {
            if( g->span[i].to != def ) {
                *(t++) = &g->span[i];
            }
        }
        fputs( "\tswitch(yych){\n", o );
        ++oline;
        while( t != &sP[0] ) {
            State   *to;

            r = s = &sP[0];
            if( *s == &g->span[0] ) {
                genCases( o, 0, *s );
            } else {
                genCases( o, (*s)[-1].ub, *s );
            }
            to = (*s)->to;
            while( ++s < t ) {
                if( (*s)->to == to ) {
                    genCases( o, (*s)[-1].ub, *s );
                } else {
                    *(r++) = *s;
                }
            }
            genGoTo( o, to );
            t = r;
        }
        fputs( "\tdefault:", o );
        genGoTo(o, def);
        fputs( "\t}\n", o );
        ++oline;
        free( sP );
    }
}

static void doBinary( FILE *o, uint i, Span *s, uint n, State *next )
{
    if( n <= 4 ) {
        doLinear( o, i, s, n, next );
    } else {
        uint    h = n/2;

        indent( o, i ); genIf( o, "<=", s[h-1].ub - 1 ); fputs( "{\n", o ); ++oline;
        doBinary( o, i + 1, &s[0], h, next );
        indent( o, i ); fputs( "\t} else {\n", o ); ++oline;
        doBinary( o, i + 1, &s[h], n - h, next );
        indent( o, i ); fputs( "\t}\n", o ); ++oline;
    }
}

static void Go_genBinary( Go *g, FILE *o, State *next )
{
    doBinary( o, 0, g->span, g->nSpans, next );
}

static void Go_genBase( Go *g, FILE *o, State *next )
{
    if( g->nSpans == 0 )
        return;
    if( !sFlag ) {
        Go_genSwitch( g, o, next );
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
            Go_genSwitch( g, o, next );
            return;
        }
    }
    if( g->nSpans > 5 ) {
        Go_genBinary( g, o, next );
    } else {
        Go_genLinear( g, o, next );
    }
}

static void Go_genGoto( Go *g, FILE *o, State *next )
{
    uint    i;

    if( bFlag ) {
        for( i = 0; i < g->nSpans; ++i ) {
            State *to = g->span[i].to;
            if( to && to->isBase ) {
                BitMap *b = BitMap_find( to );
                if( b != NULL && matches( b->go, b->on, g, to ) ) {
                    Go go;
                    go.span = malloc( g->nSpans * sizeof( Span ) );
                    Go_unmap( &go, g, to );
                    fprintf( o, "\tif(yybm[%u+yych] & %u)", b->i, (uint)b->m );
                    genGoTo( o, to );
                    Go_genBase( &go, o, next );
                    free( go.span );
                    return;
                }
            }
        }
    }
    Go_genBase( g, o, next );
}

static void State_emit( State *s, FILE *o )
{
    fprintf( o, "yy%u:", s->label );
    Action_emit( s->action, o );
}

static uint merge( Span *x0, State *fg, State *bg )
{
    Span *x = x0, *f = fg->go.span, *b = bg->go.span;
    uint nf = fg->go.nSpans, nb = bg->go.nSpans;
    State *prev = NULL, *to;
    // NB: we assume both spans are for same range
    for( ;; ) {
        if( f->ub == b->ub ) {
            to = f->to == b->to? bg : f->to;
            if( to == prev ) {
                --x;
            } else {
                x->to = prev = to;
            }
            x->ub = f->ub;
            ++x; ++f; --nf; ++b; --nb;
            if( nf == 0 && nb == 0 ) {
                return( x - x0 );
            }
        }
        while( f->ub < b->ub ) {
            to = f->to == b->to? bg : f->to;
            if( to == prev ) {
                --x;
            } else {
                x->to = prev = to;
            }
            x->ub = f->ub;
            ++x; ++f; --nf;
        }
        while( b->ub < f->ub ) {
            to = b->to == f->to? bg : f->to;
            if( to == prev ) {
                --x;
            } else {
                x->to = prev = to;
            }
            x->ub = b->ub;
            ++x; ++b; --nb;
        }
    }
}

static void SCC_init( SCC *s, uint size )
{
    s->top = s->stk = malloc( size * sizeof( State ) );
}

static void SCC_destroy( SCC *s )
{
    free( s->stk );
}

static void SCC_traverse( SCC *s, State *x )
{
    uint    k;
    uint    i;

    *s->top = x;
    k = ++s->top - s->stk;
    x->depth = k;
    for( i = 0; i < x->go.nSpans; ++i ) {
        State *y = x->go.span[i].to;
        if( y ) {
            if( y->depth == 0 )
                SCC_traverse( s, y );
            if( y->depth < x->depth )
                x->depth = y->depth;
        }
    }
    if( x->depth == k ) {
        do {
            (*--s->top)->depth = cInfinity;
            (*s->top)->link = x;
        } while( *s->top != x );
    }
}

static uint maxDist( State *s )
{
    uint    mm = 0;
    uint    i;

    for( i = 0; i < s->go.nSpans; ++i ) {
        State *t = s->go.span[i].to;

        if( t ) {
            uint m = 1;
            if( !t->link )
                m += maxDist( t );
            if( m > mm ) {
                mm = m;
            }
        }
    }
    return( mm );
}

static void calcDepth( State *head )
{
    State   *t;
    State   *s;
    uint    i;

    for( s = head; s != NULL; s = s->next ) {
        if( s->link == s ) {
            for( i = 0; i < s->go.nSpans; ++i ) {
                t = s->go.span[i].to;
                if( t != NULL && t->link == s ) {
                    s->depth = maxDist(s);
                    break;
                }
            }
            if( i == s->go.nSpans ) {
                s->link = NULL;
            }
        } else {
            s->depth = maxDist(s);
        }
    }
}

static void DFA_findSCCs( DFA *d )
{
    SCC     scc;
    State   *s;

    SCC_init( &scc, d->nStates );
    for( s = d->head; s != NULL; s = s->next ) {
        s->depth = 0;
        s->link = NULL;
    }
    for( s = d->head; s != NULL; s = s->next ) {
        if( !s->depth ) {
            SCC_traverse( &scc, s );
        }
    }
    calcDepth( d->head );
    SCC_destroy( &scc );
}

static void DFA_split( DFA *d, State *s )
{
    State *move = State_new();
    Action_new_Move( move );
    DFA_addState( d, &s->next, move );
    move->link = s->link;
    move->rule = s->rule;
    move->go = s->go;
    s->rule = NULL;
    s->go.nSpans = 1;
    s->go.span = malloc( sizeof( Span ) );
    s->go.span[0].ub = d->ubChar;
    s->go.span[0].to = move;
}

void DFA_emit( DFA *d, FILE *o )
{
    static uint label = 0;
    State       *s;
    uint        i;
    uint        nRules = 0;
    uint        nSaves = 0;
    uint        *saves;
    State       **rules;
    State       *accept;
    Span        *span;

    DFA_findSCCs( d );
    d->head->link = d->head;
    d->head->depth = maxDist( d->head );

    for( s = d->head; s != NULL; s = s->next ) {
        if( s->rule != NULL && s->rule->u.RuleOp.accept >= nRules ) {
            nRules = s->rule->u.RuleOp.accept + 1;
        }
    }

    saves = malloc( nRules * sizeof( *saves ) );
    memset( saves, ~0, nRules * sizeof( *saves ) );

    // mark backtracking points
    for( s = d->head; s != NULL; s = s->next ) {
        RegExp  *ignore = NULL; /* RuleOp */

        if( s->rule != NULL ) {
            for( i = 0; i < s->go.nSpans; ++i ) {
                if( s->go.span[i].to && !s->go.span[i].to->rule ) {
                    free( s->action );
                    if( saves[s->rule->u.RuleOp.accept] == ~0 )
                        saves[s->rule->u.RuleOp.accept] = nSaves++;
                    Action_new_Save( s, saves[s->rule->u.RuleOp.accept] );
                    continue;
                }
            }
            ignore = s->rule;
        }
    }

    // insert actions
    rules = malloc( nRules * sizeof( *rules ) );
    memset( rules, 0, nRules * sizeof( *rules ) );
    accept = NULL;
    for( s = d->head; s != NULL; s = s->next ) {
        State *ow;
        if( !s->rule ) {
            ow = accept;
        } else {
            if( !rules[s->rule->u.RuleOp.accept] ) {
                State *n = State_new();
                Action_new_Rule( n, s->rule );
                rules[s->rule->u.RuleOp.accept] = n;
                DFA_addState( d, &s->next, n );
            }
            ow = rules[s->rule->u.RuleOp.accept];
        }
        for( i = 0; i < s->go.nSpans; ++i ) {
            if( !s->go.span[i].to ) {
                if( !ow ) {
                    ow = accept = State_new();
                    Action_new_Accept( accept, nRules, saves, rules );
                    DFA_addState( d, &s->next, accept );
                }
                s->go.span[i].to = ow;
            }
        }
    }

    // split ``base'' states into two parts
    for(s = d->head; s != NULL; s = s->next ) {
        s->isBase = FALSE;
        if( s->link != NULL ) {
            for( i = 0; i < s->go.nSpans; ++i ) {
                if( s->go.span[i].to == s ) {
                    s->isBase = TRUE;
                    DFA_split( d, s );
                    if( bFlag )
                        BitMap_find_go( &s->next->go, s );
                    s = s->next;
                    break;
                }
            }
        }
    }

    // find ``base'' state, if possible
    span = malloc( ( d->ubChar - d->lbChar ) * sizeof( *span ) );
    for( s = d->head; s != NULL; s = s->next ) {
        if( s->link == NULL ) {
            for( i = 0; i < s->go.nSpans; ++i ) {
                State *to = s->go.span[i].to;
                if( to != NULL && to->isBase ) {
                    uint    nSpans;

                    to = to->go.span[0].to;
                    nSpans = merge( span, s, to );
                    if( nSpans < s->go.nSpans ) {
                        free( s->go.span );
                        s->go.nSpans = nSpans;
                        s->go.span = malloc( nSpans * sizeof( Span ) );
                        memcpy( s->go.span, span, nSpans * sizeof( Span ) );
                    }
                    break;
                }
            }
        }
    }
    free( span );

    free( d->head->action );

    fputs( "{\n\tYYCTYPE yych;\n\tunsigned int yyaccept;\n", o );
    oline += 3;
    if( bFlag ) {
        BitMap_gen( o, d->lbChar, d->ubChar );
    }

    fprintf( o, "\tgoto yy%u;\n", label );
    ++oline;
    Action_new_Enter( d->head, label++ );

    for( s = d->head; s != NULL; s = s->next ) {
        s->label = label++;
    }

    for( s = d->head; s != NULL; s = s->next ) {
        State_emit( s, o );
        Go_genGoto( &s->go, o, s->next );
    }
    fputs( "}\n", o );
    ++oline;

    BitMap_first = NULL;

    free( saves );
    free( rules );
}
