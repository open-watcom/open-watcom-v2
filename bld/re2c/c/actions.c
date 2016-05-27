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


#include <time.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "globals.h"
#include "parser.h"
#include "dfa.h"

static Symbol *first = NULL;

static void RegExp_compile( RegExp *re, Char *rep, Ins *i );

static void Symbol_init( Symbol *r, const SubStr *str )
{
    r->next = first;
    Str_init( &r->name, str );
    r->re = NULL;
    first = r;
}

static Symbol *Symbol_new( const SubStr *str )
{
    Symbol *r = malloc( sizeof( Symbol ) );

    Symbol_init( r, str );
    return( r );
}

Symbol *Symbol_find( SubStr str )
{
    Symbol *sym;
    for( sym = first; sym != NULL; sym = sym->next ) {
        if( SubStr_eq( &sym->name, &str ) ) {
            return( sym );
        }
    }
    return( Symbol_new( &str ) );
}

static Range *Range_new( uint l, uint u )
{
    Range   *r = malloc( sizeof( Range ) );

    r->next = NULL;
    r->lb = l;
    r->ub = u;
    return r;
}

static Range *Range_new_copy( Range *r )
{
    return( Range_new( r->lb, r->ub ) );
}

static Range *doUnion( Range *r1, Range *r2 )
{
    Range   *r, **rP = &r;

    for( ;; ){
        Range *s;
        if( r1->lb <= r2->lb ) {
            s = Range_new_copy( r1 );
        } else {
            s = Range_new_copy( r2 );
        }
        *rP = s;
        rP = &s->next;
        for( ;; ) {
            if( r1->lb <= r2->lb ) {
                if( r1->lb > s->ub )
                    break;
                if( r1->ub > s->ub )
                    s->ub = r1->ub;
                if( !(r1 = r1->next) ) {
                    uint ub = 0;
                    for( ; r2 && r2->lb <= s->ub; r2 = r2->next )
                        ub = r2->ub;
                    if( ub > s->ub )
                        s->ub = ub;
                    *rP = r2;
                    return( r );
                }
            } else {
                if( r2->lb > s->ub )
                    break;
                if( r2->ub > s->ub )
                    s->ub = r2->ub;
                if( !(r2 = r2->next) ) {
                    uint ub = 0;
                    for( ; r1 && r1->lb <= s->ub; r1 = r1->next )
                        ub = r1->ub;
                    if( ub > s->ub )
                        s->ub = ub;
                    *rP = r1;
                    return( r );
                }
            }
        }
    }
}

static Range *doDiff( Range *r1, Range *r2 )
{
    Range   *r, *s, **rP = &r;

    for( ; r1; r1 = r1->next ) {
        uint lb = r1->lb;
        for( ; r2 && r2->ub <= r1->lb; r2 = r2->next ) ;
        for( ; r2 && r2->lb <  r1->ub; r2 = r2->next ) {
            if( lb < r2->lb ) {
                *rP = s = Range_new( lb, r2->lb );
                rP = &s->next;
            }
            if( (lb = r2->ub) >= r1->ub ) {
                goto noMore;
            }
        }
        *rP = s = Range_new( lb, r1->ub );
        rP = &s->next;
noMore: ;
    }
    *rP = NULL;
    return r;
}

static uchar unescape( SubStr *s )
{
    uchar   c;
    uchar   v;

    s->len--;
    if( (c = *s->str++) != '\\' || s->len == 0 )
        return( c );
    s->len--;
    switch( c = *s->str++ ) {
    case 'n':
        return( '\n' );
    case 't':
        return( '\t' );
    case 'v':
        return( '\v' );
    case 'b':
        return( '\b' );
    case 'r':
        return( '\r' );
    case 'f':
        return( '\f' );
    case 'a':
        return( '\a' );
    case '0': case '1': case '2': case '3':
    case '4': case '5': case '6': case '7': {
        v = c - '0';
        for( ; s->len != 0 && '0' <= (c = *s->str) && c <= '7'; s->len--, s->str++ )
            v = v * 8 + ( c - '0' );
        return( v );
    } default:
        return( c );
    }
}

static Range *getRange( SubStr *s )
{
    uchar lb = unescape( s ), ub;

    if( s->len < 2 || *s->str != '-' ) {
        ub = lb;
    } else {
        s->len--; s->str++;
        ub = unescape( s );
        if( ub < lb ) {
            uchar tmp;
            tmp = lb; lb = ub; ub = tmp;
        }
    }
    return( Range_new( lb, ub + 1 ) );
}

#if 0
static void Range_out( FILE *o, const Range *r )
{
    if( r == NULL )
        return;

    if( ( r->ub - r->lb ) == 1 ) {
        fputc( r->lb, o );
    } else {
        fputc( r->lb, o );
        fputc('-', o);
        fputc( r->ub - 1, o );
    }
    Range_out( o, r->next );
}
#endif

static uint AltOp_fixedLength( RegExp *r )
{
    uint l1 = RegExp_fixedLength( r->u.AltOp.exp1 );
    /* XXX? Should be exp2? */
    uint l2 = RegExp_fixedLength( r->u.AltOp.exp1 );

    if( l1 != l2 || l1 == ~0u )
        return( ~0u );
    return( l1 );
}

static uint CatOp_fixedLength( RegExp *r )
{
    uint l1, l2;

    if( (l1 = RegExp_fixedLength( r->u.CatOp.exp1 )) != ~0u ) {
        if( (l2 = RegExp_fixedLength( r->u.CatOp.exp2 )) != ~0u ) {
            return( l1 + l2 );
        }
    }
    return( ~0u );
}

uint RegExp_fixedLength( RegExp *r )
{
    switch( r->type ) {
    case NULLOP:
        return( 0 );
    case MATCHOP:
        return( 1 );
    case ALTOP:
        return( AltOp_fixedLength( r ) );
    case CATOP:
        return( CatOp_fixedLength( r ) );
    default:
        break;
    }
    return( ~0u );
}

static void RegExp_calcSize( RegExp *re, Char *rep )
{
    Range   *r;
    uint    c;

    switch( re->type ) {
    case NULLOP:
        re->size = 0;
        break;
    case MATCHOP:
        re->size = 1;
        for( r = re->u.MatchOp.match; r != NULL; r = r->next ) {
            for( c = r->lb; c < r->ub; ++c ) {
                if( rep[c] == c ) {
                    ++re->size;
                }
            }
        }
        break;
    case RULEOP:
        RegExp_calcSize( re->u.RuleOp.exp, rep );
        RegExp_calcSize( re->u.RuleOp.ctx, rep );
        re->size = re->u.RuleOp.exp->size + re->u.RuleOp.ctx->size + 1;
        break;
    case ALTOP:
        RegExp_calcSize( re->u.AltOp.exp1, rep );
        RegExp_calcSize( re->u.AltOp.exp2, rep );
        re->size = re->u.AltOp.exp1->size
                   + re->u.AltOp.exp2->size + 2;
        break;
    case CATOP:
        RegExp_calcSize( re->u.CatOp.exp1, rep );
        RegExp_calcSize( re->u.CatOp.exp2, rep );
        re->size = re->u.CatOp.exp1->size + re->u.CatOp.exp2->size;
        break;
    case CLOSEOP:
        RegExp_calcSize( re->u.CloseOp.exp, rep );
        re->size = re->u.CloseOp.exp->size + 1;
        break;
    case CLOSEVOP:
        RegExp_calcSize( re->u.CloseVOp.exp, rep );
        if( re->u.CloseVOp.max >= 0 ) {
            re->size = re->u.CloseVOp.exp->size * re->u.CloseVOp.min +
                ( 1 + re->u.CloseVOp.exp->size ) * ( re->u.CloseVOp.max - re->u.CloseVOp.min );
        } else {
            re->size = (re->u.CloseVOp.exp->size * re->u.CloseVOp.min) + 1;
        }
        break;
    }
}

static void MatchOp_compile( RegExp *re, Char *rep, Ins *i )
{
    Ins     *j;
    uint    bump;
    Range   *r;
    uint    c;

    i->i.tag = CHAR;
    i->i.link = &i[re->size];
    j = &i[1];
    bump = re->size;
    for( r = re->u.MatchOp.match; r != NULL; r = r->next ) {
        for( c = r->lb; c < r->ub; ++c ) {
            if( rep[c] == c ) {
                j->c.value = c;
                j->c.bump = --bump;
                j++;
            }
        }
    }
}
static void MatchOp_split( RegExp *re, CharSet *s )
{
    Range   *r;
    uint    c;

    for( r = re->u.MatchOp.match; r != NULL; r = r->next ) {
        for( c = r->lb; c < r->ub; ++c ) {
            CharPtn *x = s->rep[c], *a = x->nxt;

            if( a == NULL ) {
                if( x->card == 1 )
                    continue;
                x->nxt = a = s->freeHead;
                if( !(s->freeHead = s->freeHead->nxt) )
                    s->freeTail = &s->freeHead;
                a->nxt = NULL;
                x->fix = s->fix;
                s->fix = x;
            }
            if( --(x->card) == 0 ) {
                *s->freeTail = x;
                *(s->freeTail = &x->nxt) = NULL;
            }
            s->rep[c] = a;
            ++(a->card);
        }
    }
    for( ; s->fix != NULL; s->fix = s->fix->fix ) {
        if( s->fix->card ) {
            s->fix->nxt = NULL;
        }
    }
}

static void RegExp_split( RegExp *re, CharSet *s )
{
    switch( re->type ) {
    case NULLOP:
        break;
    case MATCHOP:
        MatchOp_split( re, s );
        break;
    case RULEOP:
        RegExp_split( re->u.RuleOp.exp, s );
        RegExp_split( re->u.RuleOp.ctx, s );
        break;
    case ALTOP:
        RegExp_split( re->u.AltOp.exp1, s );
        RegExp_split( re->u.AltOp.exp2, s );
        break;
    case CATOP:
        RegExp_split( re->u.CatOp.exp1, s );
        RegExp_split( re->u.CatOp.exp2, s );
        break;
    case CLOSEOP:
        RegExp_split( re->u.CloseOp.exp, s );
        break;
    case CLOSEVOP:
        RegExp_split( re->u.CloseVOp.exp, s );
        break;
    }
}

#if 0
static void RegExp_display( RegExp *re, FILE *o )
{
    switch( re->type ) {
    case NULLOP:
        fputc( '_', o );
        break;
    case MATCHOP:
        Range_out( o, re->u.MatchOp.match );
        break;
    case RULEOP:
        RegExp_display( re->u.RuleOp.exp, o );
        fputc('/', o);
        RegExp_display( re->u.RuleOp.ctx, o );
        fputc( ';', o );
        break;
    case ALTOP:
        RegExp_display( re->u.AltOp.exp1, o );
        fputc( '|', o );
        RegExp_display( re->u.AltOp.exp2, o );
        break;
    case CATOP:
        RegExp_display( re->u.CatOp.exp1, o );
        RegExp_display( re->u.CatOp.exp2, o );
        break;
    case CLOSEOP:
        RegExp_display( re->u.CloseOp.exp, o );
        fputc( '+', o );
        break;
    }
}
#endif

static RegExp *RegExp_new_MatchOp( Range *m )
{
    RegExp  *r = malloc( sizeof( RegExp ) );

    r->type = MATCHOP;
    r->u.MatchOp.match = m;
    return( r );
}

static RegExp *RegExp_new_AltOp( RegExp *e1, RegExp *e2 )
{
    RegExp  *r = malloc( sizeof( RegExp ) );

    r->type = ALTOP;
    r->u.AltOp.exp1 = e1;
    r->u.AltOp.exp2 = e2;
    return( r );
}

RegExp *RegExp_new_RuleOp( RegExp *e, RegExp *c, Token *t, uint a )
{
    RegExp *r = malloc( sizeof( RegExp ) );

    r->type = RULEOP;
    r->u.RuleOp.exp = e;
    r->u.RuleOp.ctx = c;
    r->u.RuleOp.ins = NULL;
    r->u.RuleOp.accept = a;
    r->u.RuleOp.code = t;
    return r;
}

RegExp *RegExp_new_NullOp( void )
{
    RegExp  *r = malloc( sizeof( RegExp ) );

    r->type = NULLOP;
    return( r );
}

RegExp *RegExp_new_CatOp( RegExp *e1, RegExp *e2 )
{
    RegExp  *r = malloc( sizeof( RegExp ) );

    r->type = CATOP;
    r->u.CatOp.exp1 = e1;
    r->u.CatOp.exp2 = e2;
    return( r );
}

RegExp *RegExp_new_CloseOp( RegExp *e )
{
    RegExp  *r = malloc( sizeof( RegExp ) );

    r->type = CLOSEOP;
    r->u.CloseOp.exp = e;
    return( r );
}

static RegExp *merge( RegExp *m1, RegExp *m2 )
{
    if( !m1 )
        return( m2 );
    if( !m2 )
        return( m1 );
    return( RegExp_new_MatchOp( doUnion( m1->u.MatchOp.match, m2->u.MatchOp.match ) ) );
}

static RegExp *RegExp_isA( RegExp *r, RegExpType t )
{
    return( r->type == t ? r : NULL );
}

RegExp *mkDiff( RegExp *e1, RegExp *e2 )
{
    RegExp  *m1, *m2;
    Range   *r;

    if( !(m1 = RegExp_isA( e1, MATCHOP )) )
        return( NULL );
    if( !(m2 = RegExp_isA( e2, MATCHOP )) )
        return( NULL );
    r = doDiff( m1->u.MatchOp.match, m2->u.MatchOp.match );
    return( r ? RegExp_new_MatchOp( r ) : RegExp_new_NullOp() );
}

static RegExp *doAlt( RegExp *e1, RegExp *e2 )
{
    if( !e1 )
        return( e2 );
    if( !e2 )
        return( e1 );
    return( RegExp_new_AltOp( e1, e2 ) );
}

RegExp *mkAlt( RegExp *e1, RegExp *e2 )
{
    RegExp  *a;
    RegExp  *m1, *m2;

    if( (a = RegExp_isA( e1, ALTOP )) != NULL ) {
        if( (m1 = RegExp_isA( a->u.AltOp.exp1, MATCHOP )) != NULL ) {
            e1 = a->u.AltOp.exp2;
        }
    } else if( (m1 = RegExp_isA( e1, MATCHOP )) != NULL ) {
        e1 = NULL;
    }
    if( (a = RegExp_isA( e2, ALTOP )) != NULL ) {
        if( (m2 = RegExp_isA( a->u.AltOp.exp1, MATCHOP )) != NULL ) {
            e2 = a->u.AltOp.exp2;
        }
    } else if( (m2 = RegExp_isA( e2, MATCHOP )) != NULL ) {
        e2 = NULL;
    }
    return( doAlt( merge( m1, m2 ), doAlt( e1, e2 ) ) );
}
static RegExp *matchChar( uint c )
{
    return( RegExp_new_MatchOp( Range_new( c, c + 1 ) ) );
}

RegExp *strToRE( SubStr s )
{
    RegExp  *re;

    s.len -= 2; s.str += 1;
    if( s.len == 0 )
        return( RegExp_new_NullOp() );
    re = matchChar( unescape( &s ) );
    while( s.len > 0 )
        re = RegExp_new_CatOp( re, matchChar( unescape( &s ) ) );
    return( re );
}

RegExp *ranToRE( SubStr s )
{
    Range   *r;

    s.len -= 2; s.str += 1;
    if( s.len == 0 )
        return( RegExp_new_NullOp() );
    r = getRange( &s );
    while( s.len > 0 )
        r = doUnion( r, getRange( &s ) );
    return( RegExp_new_MatchOp( r ) );
}

static void optimize( Ins *i )
{
    while( !i->i.marked ) {
        i->i.marked = true;
        if( i->i.tag == CHAR ) {
            i = (Ins *)i->i.link;
        } else if( i->i.tag == GOTO || i->i.tag == FORK ) {
            Ins *target = (Ins *)i->i.link;
            optimize( target );
            if( target->i.tag == GOTO )
                i->i.link = target->i.link == target? i : target;
            if( i->i.tag == FORK ) {
                Ins *follow = (Ins *)&i[1];
                optimize( follow );
                if( follow->i.tag == GOTO && follow->i.link == follow ) {
                    i->i.tag = GOTO;
                } else if( i->i.link == i ) {
                    i->i.tag = GOTO;
                    i->i.link = follow;
                }
            }
            return;
        } else {
            ++i;
        }
    }
}

static void AltOp_compile( RegExp *re, Char *rep, Ins *i )
{
    Ins     *j;

    i->i.tag = FORK;
    j = &i[re->u.AltOp.exp1->size + 1];
    i->i.link = &j[1];
    RegExp_compile( re->u.AltOp.exp1, rep, &i[1] );
    j->i.tag = GOTO;
    j->i.link = &j[re->u.AltOp.exp2->size + 1];
    RegExp_compile( re->u.AltOp.exp2, rep, &j[1] );
}

static void RegExp_compile( RegExp *re, Char *rep, Ins *i )
{
    Ins     *jumppoint;
    int     st = 0;

    switch( re->type ) {
    case NULLOP:
        break;
    case MATCHOP:
        MatchOp_compile( re, rep, i );
        break;
    case RULEOP:
        re->u.RuleOp.ins = i;
        RegExp_compile( re->u.RuleOp.exp, rep, &i[0] );
        i += re->u.RuleOp.exp->size;
        RegExp_compile( re->u.RuleOp.ctx, rep, &i[0] );
        i += re->u.RuleOp.ctx->size;
        i->i.tag = TERM;
        i->i.link = re;
        break;
    case ALTOP:
        AltOp_compile( re, rep, i );
        break;
    case CATOP:
        RegExp_compile( re->u.CatOp.exp1, rep, &i[0] );
        RegExp_compile( re->u.CatOp.exp2, rep,
                       &i[re->u.CatOp.exp1->size] );
        break;
    case CLOSEOP:
        RegExp_compile( re->u.CloseOp.exp, rep, &i[0] );
        i += re->u.CloseOp.exp->size;
        i->i.tag = FORK;
        i->i.link = i - re->u.CloseOp.exp->size;
        break;
    case CLOSEVOP:
        jumppoint = i + ( ( 1 + re->u.CloseVOp.exp->size ) *
                         ( re->u.CloseVOp.max - re->u.CloseVOp.min ) );
        for( st = re->u.CloseVOp.min; st < re->u.CloseVOp.max; st++ ) {
            i->i.tag = FORK;
            i->i.link = jumppoint;
            i+=1;
            RegExp_compile( re->u.CloseVOp.exp, rep, &i[0] );
            i += re->u.CloseVOp.exp->size;
        }
        for( st = 0; st < re->u.CloseVOp.min; st++ ) {
            RegExp_compile( re->u.CloseVOp.exp, rep, &i[0] );
            i += re->u.CloseVOp.exp->size;
            if( re->u.CloseVOp.max < 0 && st == 0 ) {
                i->i.tag = FORK;
                i->i.link = i - re->u.CloseVOp.exp->size;
                i++;
            }
        }
        break;
    }
}

void genCode( FILE *o, RegExp *re )
{
    CharSet     cs;
    uint        j;
    Char        rep[NCHARS];
    Ins         *ins, *eoi;
    DFA         *dfa;

    memset( &cs, 0, sizeof( cs ) );
    for( j = 0; j < NCHARS; ++j ) {
        cs.rep[j] = &cs.ptn[0];
        cs.ptn[j].nxt = &cs.ptn[j+1];
    }
    cs.freeHead = &cs.ptn[1];
    *(cs.freeTail = &cs.ptn[NCHARS - 1].nxt) = NULL;
    cs.ptn[0].card = NCHARS;
    cs.ptn[0].nxt = NULL;
    RegExp_split( re, &cs );
/*
    for( uint k = 0; k < NCHARS; ) {
        for( j = k; ++k < NCHARS && cs.rep[k] == cs.rep[j]; ) ;
        printSpan( cerr, j, k );
        cerr << "\t" << cs.rep[j] - &cs.ptn[0] << endl;
    }
*/
    for(j = 0; j < NCHARS; ++j){
        if( !cs.rep[j]->nxt )
            cs.rep[j]->nxt = &cs.ptn[j];
        rep[j] = (Char)( cs.rep[j]->nxt - &cs.ptn[0] );
    }

    RegExp_calcSize( re, rep );
    ins = malloc( ( re->size + 1 ) * sizeof( Ins ) );
    memset(ins, 0, ( re->size + 1 ) * sizeof( Ins ) );
    RegExp_compile( re, rep, ins );
    eoi = &ins[re->size];
    eoi->i.tag = GOTO;
    eoi->i.link = eoi;

    optimize( ins );
    for(j = 0; j < re->size;){
        ins[j].i.marked = false;
        if( ins[j].i.tag == CHAR ) {
            j = (Ins *)ins[j].i.link - ins;
        } else {
            j++;
        }
    }

    dfa = DFA_new( ins, re->size, 0, 256, rep );
    DFA_emit( dfa, o );
    DFA_delete( dfa );
    free( ins );
}

Action *Action_new_Match( State *s )
{
    Action  *a = malloc( sizeof( Action ) );

    a->type = MATCHACT;
    a->state = s;
    s->action = a;
    return( a );
}

Action *Action_new_Enter( State *s, uint l )
{
    Action  *a = malloc( sizeof( Action ) );

    a->type = ENTERACT;
    a->state = s;
    a->u.Enter.label = l;
    s->action = a;
    return( a );
}

Action *Action_new_Save( State *s, uint i )
{
    Action  *a = malloc( sizeof( Action ) );

    a->type = SAVEMATCHACT;
    a->state = s;
    a->u.SaveMatch.selector = i;
    s->action = a;
    return( a );
}

Action *Action_new_Move( State *s )
{
    Action  *a = malloc( sizeof( Action ) );

    a->type = MOVEACT;
    a->state = s;
    s->action = a;
    return( a );
}

Action *Action_new_Rule( State *s, RegExp *r ) /* RuleOp */
{
    Action  *a = malloc( sizeof( Action ) );

    a->type = RULEACT;
    a->state = s;
    a->u.Rule.rule = r;
    s->action = a;
    return( a );
}

Action *Action_new_Accept( State *s, uint n, uint *sv, State **r )
{
    Action  *a = malloc( sizeof( Action ) );

    a->type = ACCEPTACT;
    a->state = s;
    a->u.Accept.nRules = n;
    a->u.Accept.saves = sv;
    a->u.Accept.rules = r;
    s->action = a;
    return( a );
}

