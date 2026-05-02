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


#include <time.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "globals.h"
#include "parser.h"
#include "dfa.h"
#include "mem.h"


static Symbol *sym_first = NULL;

static void RegExp_compile( RegExp *re, Char *rep, Ins *i );

static void Symbol_init( Symbol *sym, const SubStr *str )
{
    sym->next = sym_first;
    Str_init( &sym->name, str );
    sym->re = NULL;
    sym_first = sym;
}

static void Symbol_fini( Symbol *sym )
{
    if( sym->re != NULL ) {
        MemFree( sym->re );
        sym->re = NULL;
    }
    Str_fini( &sym->name );
}

static Symbol *Symbol_new( const SubStr *str )
{
    Symbol *sym;

    sym = MemAlloc( sizeof( *sym ) );
    Symbol_init( sym, str );
    return( sym );
}

void Symbol_delete_all( void )
{
    Symbol *sym;

    for( sym = sym_first; sym != NULL; sym = sym->next ) {
        Symbol_fini( sym );
    }
    sym_first = NULL;
}

Symbol *Symbol_find( SubStr str )
{
    Symbol *sym;

    for( sym = sym_first; sym != NULL; sym = sym->next ) {
        if( SubStr_eq( &sym->name, &str ) ) {
            return( sym );
        }
    }
    return( Symbol_new( &str ) );
}

static Range *Range_new( Char l, Char u )
{
    Range   *r;

    r = MemAlloc( sizeof( Range ) );
    r->next = NULL;
    r->lb = l;
    r->ub = u;
    return( r );
}

static Range *Range_new_copy( Range *r )
{
    return( Range_new( r->lb, r->ub ) );
}

static void Range_delete( Range *r )
{
    Range *next;

    while( r != NULL ) {
        next = r->next;
        MemFree( r );
        r = next;
    }
}

static Range *doUnion( Range *r1, Range *r2 )
{
    Range   *r;
    Range   **rP;

    rP = &r;
    for( ;; ) {
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
                r1 = r1->next;
                if( r1 == NULL ) {
                    Char ub = 0;
                    for( ; r2 != NULL && r2->lb <= s->ub; r2 = r2->next )
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
                r2 = r2->next;
                if( r2 == NULL ) {
                    Char ub = 0;
                    for( ; r1 != NULL && r1->lb <= s->ub; r1 = r1->next )
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
    Range   *r;
    Range   *s;
    Range   **rP;

    rP = &r;
    for( ; r1 != NULL ; r1 = r1->next ) {
        Char lb = r1->lb;
        for( ; r2 != NULL && r2->ub <= r1->lb; r2 = r2->next )
            ;
        for( ; r2 != NULL && r2->lb <  r1->ub; r2 = r2->next ) {
            if( lb < r2->lb ) {
                *rP = s = Range_new( lb, r2->lb );
                rP = &s->next;
            }
            lb = r2->ub;
            if( lb >= r1->ub ) {
                goto noMore;
            }
        }
        *rP = s = Range_new( lb, r1->ub );
        rP = &s->next;
noMore: ;
    }
    *rP = NULL;
    return( r );
}

static Char unescape( SubStr *s )
{
    Char    c;
    Char    v;

    s->len--;
    if( (c = *s->str++) != '\\'
      || s->len == 0 )
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
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
        v = c - '0';
        for( ; s->len != 0 && '0' <= (c = *s->str) && c <= '7'; s->len--, s->str++ )
            v = v * 8 + ( c - '0' );
        return( v );
    default:
        return( c );
    }
}

static Range *getRange( SubStr *s )
{
    Char    lb;
    Char    ub;

    lb = unescape( s );
    if( s->len < 2
      || *s->str != '-' ) {
        ub = lb;
    } else {
        s->len--;
        s->str++;
        ub = unescape( s );
        if( ub < lb ) {
            Char tmp;
            tmp = lb;
            lb = ub;
            ub = tmp;
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

static uint AltOp_fixedLength( RegExp *re )
{
    uint l1;
    uint l2;

    l1 = RegExp_fixedLength( re->u.AltOp.exp1 );
    /* XXX? Should be exp2? */
    l2 = RegExp_fixedLength( re->u.AltOp.exp2 );
    if( l1 != l2
      || l1 == ~0u )
        return( ~0u );
    return( l1 );
}

static uint CatOp_fixedLength( RegExp *re )
{
    uint l1;
    uint l2;

    if( (l1 = RegExp_fixedLength( re->u.CatOp.exp1 )) != ~0u ) {
        if( (l2 = RegExp_fixedLength( re->u.CatOp.exp2 )) != ~0u ) {
            return( l1 + l2 );
        }
    }
    return( ~0u );
}

uint RegExp_fixedLength( RegExp *re )
{
    switch( re->type ) {
    case NULLOP:
        return( 0 );
    case MATCHOP:
        return( 1 );
    case ALTOP:
        return( AltOp_fixedLength( re ) );
    case CATOP:
        return( CatOp_fixedLength( re ) );
    default:
        break;
    }
    return( ~0u );
}

static void RegExp_calcSize( RegExp *re, Char *rep )
{
    Range   *r;
    Char    c;

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
    Char    c;

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

static void MatchOp_delete( RegExp *re )
{
    if( re->u.MatchOp.match != NULL ) {
        Range_delete( re->u.MatchOp.match );
        re->u.MatchOp.match = NULL;
    }
}

static void MatchOp_split( RegExp *re, CharSet *s )
{
    Range   *r;
    Char    c;

    for( r = re->u.MatchOp.match; r != NULL; r = r->next ) {
        for( c = r->lb; c < r->ub; ++c ) {
            CharPtn *x = s->rep[c], *a = x->nxt;

            if( a == NULL ) {
                if( x->card == 1 )
                    continue;
                a = s->freeHead;
                x->nxt = a;
                s->freeHead = s->freeHead->nxt;
                if( s->freeHead == NULL )
                    s->freeTail = &s->freeHead;
                a->nxt = NULL;
                x->fix = s->fix;
                s->fix = x;
            }
            if( --(x->card) == 0 ) {
                *s->freeTail = x;
                s->freeTail = &x->nxt;
                *(s->freeTail) = NULL;
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

static RegExp *RegExp_new_MatchOp( Range *r )
{
    RegExp  *re;

    re = MemAlloc( sizeof( *re ) );
    re->type = MATCHOP;
    re->u.MatchOp.match = r;
    return( re );
}

static RegExp *RegExp_new_AltOp( RegExp *re1, RegExp *re2 )
{
    RegExp  *re;

    re = MemAlloc( sizeof( *re ) );
    re->type = ALTOP;
    re->u.AltOp.exp1 = re1;
    re->u.AltOp.exp2 = re2;
    return( re );
}

RegExp *RegExp_new_RuleOp( RegExp *re_exp, RegExp *re_ctx, Token *t, uint a )
{
    RegExp  *re;

    re = MemAlloc( sizeof( RegExp ) );
    re->type = RULEOP;
    re->u.RuleOp.exp = re_exp;
    re->u.RuleOp.ctx = re_ctx;
    re->u.RuleOp.ins = NULL;
    re->u.RuleOp.accept = a;
    re->u.RuleOp.code = t;
    return( re );
}

RegExp *RegExp_new_NullOp( void )
{
    RegExp  *re;

    re = MemAlloc( sizeof( RegExp ) );
    re->type = NULLOP;
    return( re );
}

RegExp *RegExp_new_CatOp( RegExp *re1, RegExp *re2 )
{
    RegExp  *re;

    re = MemAlloc( sizeof( RegExp ) );
    re->type = CATOP;
    re->u.CatOp.exp1 = re1;
    re->u.CatOp.exp2 = re2;
    return( re );
}

RegExp *RegExp_new_CloseOp( RegExp *re_exp )
{
    RegExp  *re;

    re = MemAlloc( sizeof( RegExp ) );
    re->type = CLOSEOP;
    re->u.CloseOp.exp = re_exp;
    return( re );
}

RegExp *RegExp_new_CloseVOp( RegExp *re_exp, int lb, int ub )
{
    RegExp  *re;

    re = MemAlloc( sizeof( RegExp ) );
    re->type = CLOSEVOP;
    re->u.CloseVOp.exp = re_exp;
    re->u.CloseVOp.min = lb;
    re->u.CloseVOp.max = ub;
    return( re );
}

static RegExp *merge( RegExp *re1, RegExp *re2 )
{
    if( re1 == NULL )
        return( re2 );
    if( re2 == NULL )
        return( re1 );
    return( RegExp_new_MatchOp( doUnion( re1->u.MatchOp.match, re2->u.MatchOp.match ) ) );
}

static RegExp *RegExp_isA( RegExp *re, RegExpType t )
{
    return( ( re->type == t ) ? re : NULL );
}

RegExp *MkDiff( RegExp *re_i1, RegExp *re_i2 )
{
    RegExp  *re_o1;
    RegExp  *re_o2;
    RegExp  *re;
    Range   *r;

    re = NULL;
    re_o1 = RegExp_isA( re_i1, MATCHOP );
    if( re_o1 != NULL ) {
        re_o2 = RegExp_isA( re_i2, MATCHOP );
        if( re_o2 != NULL ) {
            r = doDiff( re_o1->u.MatchOp.match, re_o2->u.MatchOp.match );
            if( r != NULL ) {
                re = RegExp_new_MatchOp( r );
            } else {
                re = RegExp_new_NullOp();
            }
        }
    }
    RegExp_delete( re_i1 );
    RegExp_delete( re_i2 );
    return( re );
}

static RegExp *doAlt( RegExp *re1, RegExp *re2 )
{
    RegExp  *re;

    if( re1 == NULL ) {
        re = re2;
    } else if( re2 == NULL ) {
        re = re1;
    } else {
        re = RegExp_new_AltOp( re1, re2 );
        RegExp_delete( re1 );
        RegExp_delete( re2 );
    }
    return( re );
}

RegExp *MkAlt( RegExp *re_i1, RegExp *re_i2 )
{
    RegExp  *re;
    RegExp  *re_o1;
    RegExp  *re_o2;

    if( (re = RegExp_isA( re_i1, ALTOP )) != NULL ) {
        if( (re_o1 = RegExp_isA( re->u.AltOp.exp1, MATCHOP )) != NULL ) {
            re_i1 = re->u.AltOp.exp2;
        }
    } else if( (re_o1 = RegExp_isA( re_i1, MATCHOP )) != NULL ) {
        re_i1 = NULL;
    }
    if( (re = RegExp_isA( re_i2, ALTOP )) != NULL ) {
        if( (re_o2 = RegExp_isA( re->u.AltOp.exp1, MATCHOP )) != NULL ) {
            re_i2 = re->u.AltOp.exp2;
        }
    } else if( (re_o2 = RegExp_isA( re_i2, MATCHOP )) != NULL ) {
        re_i2 = NULL;
    }
    return( doAlt( merge( re_o1, re_o2 ), doAlt( re_i1, re_i2 ) ) );
}
static RegExp *matchChar( Char c )
{
    return( RegExp_new_MatchOp( Range_new( c, c + 1 ) ) );
}

RegExp *StringToRE( SubStr s )
{
    RegExp  *re;

    s.len -= 2;
    s.str += 1;
    if( s.len == 0 )
        return( RegExp_new_NullOp() );
    re = matchChar( unescape( &s ) );
    while( s.len > 0 )
        re = RegExp_new_CatOp( re, matchChar( unescape( &s ) ) );
    return( re );
}

RegExp *RangeToRE( SubStr s )
{
    Range   *r;

    s.len -= 2;
    s.str += 1;
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
        } else if( i->i.tag == GOTO
          || i->i.tag == FORK ) {
            Ins *target = (Ins *)i->i.link;
            optimize( target );
            if( target->i.tag == GOTO )
                i->i.link = ( target->i.link == target ) ? i : target;
            if( i->i.tag == FORK ) {
                Ins *follow = (Ins *)&i[1];
                optimize( follow );
                if( follow->i.tag == GOTO
                  && follow->i.link == follow ) {
                    i->i.tag = GOTO;
                } else if( i->i.link == i ) {
                    i->i.tag = GOTO;
                    i->i.link = follow;
                }
            }
            break;
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

static void AltOp_delete( RegExp *re )
{
    RegExp_delete( re->u.AltOp.exp1 );
    RegExp_delete( re->u.AltOp.exp2 );
}

static void RegExp_compile( RegExp *re, Char *rep, Ins *i )
{
    Ins     *jumppoint;
    int     st;

    st = 0;
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
            if( re->u.CloseVOp.max < 0
              && st == 0 ) {
                i->i.tag = FORK;
                i->i.link = i - re->u.CloseVOp.exp->size;
                i++;
            }
        }
        break;
    }
}

void RegExp_delete( RegExp *re )
{
    if( re == NULL )
        return;
    switch( re->type ) {
    case NULLOP:
        break;
    case MATCHOP:
        MatchOp_delete( re );
        break;
    case RULEOP:
        RegExp_delete( re->u.RuleOp.exp );
        RegExp_delete( re->u.RuleOp.ctx );
        break;
    case ALTOP:
        AltOp_delete( re );
        break;
    case CATOP:
        RegExp_delete( re->u.CatOp.exp1 );
        RegExp_delete( re->u.CatOp.exp2 );
        break;
    case CLOSEOP:
        RegExp_delete( re->u.CloseOp.exp );
        break;
    case CLOSEVOP:
        RegExp_delete( re->u.CloseVOp.exp );
        break;
    }
}

void GenCode( FILE *o, RegExp *re )
{
    CharSet     cs;
    uint        j;
    Char        rep[NCHARS];
    Ins         *ins;
    Ins         *eoi;
    DFA         *dfa;
    static unsigned nstate = 0;

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
    for( j = 0; j < NCHARS; ++j ) {
        if( cs.rep[j]->nxt == NULL )
            cs.rep[j]->nxt = &cs.ptn[j];
        rep[j] = (Char)( cs.rep[j]->nxt - &cs.ptn[0] );
    }

    RegExp_calcSize( re, rep );
    ins = MemAlloc( ( re->size + 1 ) * sizeof( Ins ) );
    memset( ins, 0, ( re->size + 1 ) * sizeof( Ins ) );
    RegExp_compile( re, rep, ins );
    eoi = &ins[re->size];
    eoi->i.tag = GOTO;
    eoi->i.link = eoi;

    optimize( ins );
    for( j = 0; j < re->size; ) {
        ins[j].i.marked = false;
        if( ins[j].i.tag == CHAR ) {
            j = (uint)( (Ins *)ins[j].i.link - ins );
        } else {
            j++;
        }
    }

    dfa = DFA_new( ins, re->size, 0, 256, rep, nstate );
    DFA_emit( dfa, o );
    nstate = dfa->nStates;
    DFA_delete( dfa );
    MemFree( ins );
}

Action *Action_new_Match( State *st )
{
    Action  *a;

    a = MemAlloc( sizeof( Action ) );
    a->type = MATCHACT;
    a->state = st;
    st->action = a;
    return( a );
}

Action *Action_new_Enter( State *st )
{
    Action  *a;

    a = MemAlloc( sizeof( Action ) );
    a->type = ENTERACT;
    a->state = st;
    a->u.Enter.label = st->label;
    st->action = a;
    return( a );
}

Action *Action_new_Save( State *st, uint i )
{
    Action  *a;

    bUsedYYAccept = true;
    a = MemAlloc( sizeof( Action ) );
    a->type = SAVEMATCHACT;
    a->state = st;
    a->u.SaveMatch.selector = i;
    st->action = a;
    return( a );
}

Action *Action_new_Move( State *st )
{
    Action  *a;

    a = MemAlloc( sizeof( Action ) );
    a->type = MOVEACT;
    a->state = st;
    st->action = a;
    return( a );
}

Action *Action_new_Rule( State *st, RegExp *rule ) /* RuleOp */
{
    Action  *a;

    a = MemAlloc( sizeof( Action ) );
    a->type = RULEACT;
    a->state = st;
    a->u.Rule.rule = rule;
    st->action = a;
    return( a );
}

Action *Action_new_Accept( State *st, uint n, uint *sv, State **rules )
{
    Action  *a;

    a = MemAlloc( sizeof( Action ) );
    a->type = ACCEPTACT;
    a->state = st;
    a->u.Accept.nRules = n;
    a->u.Accept.saves = sv;
    a->u.Accept.rules = rules;
    st->action = a;
    return( a );
}

void Action_delete( State *st )
{
    if( st->action != NULL ) {
        MemFree( st->action );
        st->action = NULL;
    }
}
