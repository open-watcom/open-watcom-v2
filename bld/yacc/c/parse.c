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
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "alloc.h"
#include "yacc.h"

#define BUF_INCR            500

#define INIT_RHS_SIZE       16

typedef enum {
    /* ASCII_MIN = 0x0000 */
    /* ASCII_MAX = 0x00FF */
    T_IDENTIFIER = 0x0100,  /* includes identifiers and literals */
    T_CIDENTIFIER,          /* identifier (but not literal) followed by colon */
    T_NUMBER,               /* -?[0-9]+ */
    T_MARK,                 /* %% */
    T_LCURL,                /* %{ */
    T_RCURL,                /* }% */
    T_AMBIG,                /* %keywords */
    T_KEYWORD_ID,
    T_LEFT,
    T_RIGHT,
    T_NONASSOC,
    T_TOKEN,
    T_PREC,
    T_TYPE,
    T_START,
    T_UNION,
    T_TYPENAME,
    T_EOF
} a_token;

typedef struct y_token {
    struct y_token  *next;
    token_n         value;
    char            name[1];
} y_token;

static void                 copycurl( void );
static void                 copyUniqueActions( void );
static void                 copyact( rule_n, a_sym *, a_sym **, unsigned, unsigned );
static void                 lineinfo( void );
static a_token              scan( unsigned used );
static void                 need( char *pat );
static int                  eatcrud( void );
static int                  nextc( void );
static int                  lastc( void );
static void                 addbuf( int ch );
static char                 *dupbuf( void );

int lineno = { 1 };

static unsigned             bufused;
static unsigned             bufmax;
static char                 *buf = { NULL };

static int                  ch = { ' ' };
static a_token              token;
static int                  value;

typedef struct uniq_case    uniq_case;
typedef struct rule_case    rule_case;
struct rule_case {
    rule_case   *next;
    a_sym       *lhs;
    rule_n      pnum;
};
struct uniq_case {
    uniq_case   *next;
    char        *action;
    rule_case   *rules;
};

static unsigned long        actionsCombined;
static uniq_case            *caseActions;

a_SR_conflict               *ambiguousstates;

#define TYPENAME_FIRST_CHAR(x) (isalpha(x)||x=='_')
#define TYPENAME_NEXT_CHAR(x) (isalpha(x)||isdigit(x)||x=='_'||x=='.')

static y_token  *tokens_head = NULL;
static y_token  *tokens_tail = NULL;

static a_token scan_typename( unsigned used )
{
    bufused = used;
    if( TYPENAME_FIRST_CHAR( ch ) ) {
        do {
            addbuf( ch );
            nextc();
        } while( TYPENAME_NEXT_CHAR( ch ) );
        token = T_TYPENAME;
    }
    addbuf( '\0' );
    value = 0;
    return( token );
}

static char *get_typename( char *src )
{
    int             c;

    c = *(unsigned char *)src;
    if( TYPENAME_FIRST_CHAR( c ) ) {
        do {
            c = *(unsigned char *)(++src);
        } while( TYPENAME_NEXT_CHAR( c ) );
    }
    return( src );
}

static a_sym *make_sym( char *name, token_n value )
{
    a_sym *p;

    p = addsym( name );
    p->token = value;
    return( p );
}

static a_SR_conflict *make_unique_ambiguity( a_sym *sym, unsigned index )
{
    a_SR_conflict   *am;

    for( am = ambiguousstates; am != NULL; am = am->next ) {
        if( am->id == index ) {
            if( am->sym != sym ) {
                msg( "ambiguity %u deals with %s, not %s.\n",
                     index, am->sym->name, sym->name );
                break;
            }
            return( am );
        }
    }
    am = MALLOC( 1, a_SR_conflict );
    am->next = ambiguousstates;
    am->sym = sym;
    am->id = index;
    am->state = NULL;
    am->shift = NULL;
    am->thread = NULL;
    am->reduce = 0;
    ambiguousstates = am;
    return( am );
}

static void tlist_remove( char *name )
{
    y_token *curr;
    y_token *last;

    last = NULL;
    for( curr = tokens_head; curr != NULL; curr = curr->next ) {
        if( strcmp( name, curr->name ) == 0 ) {
            if( last == NULL ) {
                tokens_head = curr->next;
            } else {
                last->next = curr->next;
            }
            if( curr->next == NULL ) {
                tokens_tail = last;
            }
            FREE( curr );
            break;
        }
        last = curr;
    }
}

static void tlist_add( char *name, token_n value )
{
    y_token *tmp;
    y_token *curr;

    for( curr = tokens_head; curr != NULL; curr = curr->next ) {
        if( strcmp( name, curr->name ) == 0 ) {
            curr->value = value;
            return;
        }
    }
    tmp = (y_token *)MALLOC( strlen( name ) + sizeof( y_token ), char );
    strcpy( tmp->name, name );
    tmp->value = value;
    tmp->next = NULL;
    if( tokens_head == NULL ) {
        tokens_head = tmp;
    }
    if( tokens_tail != NULL ) {
        tokens_tail->next = tmp;
    }
    tokens_tail = tmp;
}

static void dump_header( char *union_name )
{
    const char  *fmt;
    const char  *ttype;
    y_token     *t;
    y_token     *tmp;

    if( fastflag || bigflag || compactflag ) {
        ttype = "unsigned short";
    } else {
        ttype = "unsigned char";
    }
    if( enumflag ) {
        fprintf( tokout, "typedef enum yytokentype {\n" );
        fmt = "\t%-20s = 0x%02x,\n";
    } else {
        fmt = "#define %-20s 0x%02x\n";
    }
    t = tokens_head;
    while( t != NULL ) {
        fprintf( tokout, fmt, t->name, t->value );
        tmp = t;
        t = t->next;
        FREE( tmp );
    }
    if( enumflag ) {
        fprintf( tokout, "\tYTOKEN_ENUMSIZE_SETUP = (%s)-1\n", ttype );
        fprintf( tokout, "} yytokentype;\n" );
    } else {
        fprintf( tokout, "typedef %s yytokentype;\n", ttype );
    }
    if( union_name != NULL ) {
        fprintf( tokout, "#ifndef __YYSTYPE_DEFINED\n" );
        fprintf( tokout, "#define __YYSTYPE_DEFINED\n" );
        fprintf( tokout, "typedef union %s YYSTYPE;\n", union_name );
        fprintf( tokout, "#endif\n" );
        FREE( union_name );
    }
}

void defs( void )
{
    token_n     gentoken;
    a_sym       *sym;
    int         ctype;
    char        *dupbuf( void );
    char        *type;
    a_prec      prec;
    char        *union_name = NULL;

    eofsym = make_sym( "$eof", TOKEN_EOF );
    nosym = make_sym( "$impossible", TOKEN_IMPOSSIBLE );
    errsym = make_sym( "error", TOKEN_ERROR );
    if( denseflag ) {
        gentoken = TOKEN_DENSE_BASE;
    } else {
        gentoken = TOKEN_SPARSE_BASE;
    }
    scan( 0 );
    prec.prec = 0;
    prec.assoc = NON_ASSOC;
    for( ; token != T_MARK; ) {
        switch( token ) {
        case T_START:
            if( scan( 0 ) != T_IDENTIFIER )
                msg( "Identifier needed after %%start.\n" );
            startsym = addsym( buf );
            scan( 0 );
            break;
        case T_UNION:
            if( scan( 0 ) != '{' ) {
                msg( "Need '{' after %%union.\n" );
            }
            fprintf( actout, "#ifndef __YYSTYPE_DEFINED\n" );
            fprintf( actout, "#define __YYSTYPE_DEFINED\n" );
            fprintf( actout, "typedef union " );
            lineinfo();
            fprintf( actout, "%s YYSTYPE;\n", buf );
            fprintf( actout, "#endif\n" );
            if( union_name == NULL ) {
                union_name = MALLOC( strlen( buf ) + 1, char );
                strcpy( union_name, buf );
            } else {
                msg( "%union already defined\n" );
            }
            scan( 0 );
            break;
        case T_LCURL:
            lineinfo();
            copycurl();
            scan( 0 );
            break;
        case T_KEYWORD_ID:
            switch( scan( 0 ) ) {
            case T_IDENTIFIER:
                sym = addsym( buf );
                if( sym->token == 0 ) {
                    msg( "Token must be assigned number before %keyword_id\n" );
                }
                value = sym->token;
                break;
            case T_NUMBER:
                break;
            default:
                msg( "Expecting identifier or number.\n" );
            }
            keyword_id_low = value;
            switch( scan( 0 ) ) {
            case T_IDENTIFIER:
                sym = addsym( buf );
                if( sym->token == 0 ) {
                    msg( "Token must be assigned number before %keyword_id\n" );
                }
                value = sym->token;
                break;
            case T_NUMBER:
                break;
            default:
                msg( "Expecting identifier or number.\n" );
            }
            keyword_id_high = value;
            scan( 0 );
            break;
        case T_LEFT:
        case T_RIGHT:
        case T_NONASSOC:
            ++prec.prec;
            prec.assoc = value;
            // pass through
        case T_TOKEN:
        case T_TYPE:
            ctype = token;
            if( scan( 0 ) == '<' ) {
                if( scan_typename( 0 ) != T_TYPENAME ) {
                    msg( "Expecting type specifier.\n" );
                }
                type = dupbuf();
                if( scan( 0 ) != '>' ) {
                    msg( "Expecting '>'.\n" );
                }
                scan( 0 );
            } else {
                type = NULL;
            }
            while( token == T_IDENTIFIER ) {
                sym = addsym( buf );
                if( type != NULL && sym->type != NULL ) {
                    if( strcmp( sym->type, type ) != 0 ) {
                        msg( "'%s' type redeclared from '%s' to '%s'\n",
                            buf, sym->type, type );
                    }
                }
                sym->type = type;
                if( ctype == T_TYPE ) {
                    scan( 0 );
                } else {
                    if( sym->token == 0 ) {
                        sym->token = (token_n)value;
                    }
                    if( ctype != T_TOKEN ) {
                        sym->prec = prec;
                    }
                    if( scan( 0 ) == T_NUMBER ) {
                        if( sym->token != 0 ) {
                            if( sym->name[0] != '\'' ) {
                                tlist_remove( sym->name );
                            }
                        }
                        sym->token = (token_n)value;
                        scan( 0 );
                    }
                    if( sym->token == 0 ) {
                        sym->token = gentoken++;
                    }
                    if( sym->name[0] != '\'' ) {
                        tlist_add( sym->name, sym->token );
                    }
                }
                if( token == ',' ) {
                    scan( 0 );
                }
            }
            break;
        default:
            msg( "Incorrect syntax.\n" );
        }
    }
    scan( 0 );
    dump_header( union_name );
}

static bool scanambig( unsigned used, a_SR_conflict_list **list )
{
    bool                    absorbed_something;
    unsigned                index;
    a_sym                   *sym;
    a_SR_conflict           *am;
    a_SR_conflict_list      *en;

    absorbed_something = FALSE;
    for( ; token == T_AMBIG; ) {
        /* syntax is "%ambig <number> <token>" */
        /* token has already been scanned by scanprec() */
        if( scan( used ) != T_NUMBER || value < 0 ) {
            msg( "Expecting a non-negative number after %ambig.\n" );
            break;
        }
        index = value;
        if( scan( used ) != T_IDENTIFIER ) {
            msg( "Expecting a token name after %ambig <number>.\n" );
            break;
        }
        sym = findsym( buf );
        if( sym == NULL ) {
            msg( "Unknown token specified in %ambig directive.\n" );
            break;
        }
        if( sym->token == 0 ) {
            msg( "Non-terminal specified in %ambig directive.\n" );
            break;
        }
        scan( used );
        absorbed_something = TRUE;
        am = make_unique_ambiguity( sym, index );
        en = MALLOC( 1, a_SR_conflict_list );
        en->next = *list;
        en->thread = am->thread;
        en->pro = NULL;
        en->conflict = am;
        am->thread = en;
        *list = en;
    }
    return( absorbed_something );
}

static bool scanprec( unsigned used, a_sym **precsym )
{
    if( token != T_PREC )
        return( FALSE );
    if( scan( used ) != T_IDENTIFIER || (*precsym = findsym( buf )) == NULL || (*precsym)->token == 0 ) {
        msg( "Expecting a token after %prec.\n" );
    }
    scan( used );
    return( TRUE );
}

static void scanextra( unsigned used, a_sym **psym, a_SR_conflict_list **pSR )
{
    scan( used );
    for( ;; ) {
        if( !scanprec( used, psym ) && !scanambig( used, pSR ) ) {
            break;
        }
    }
}

static char *type_name( char *type )
{
    if( type == NULL ) {
        return( "** no type **" );
    }
    return( type );
}

void rules( void )
{
    a_sym               *lhs, *sym, *precsym;
    a_sym               **rhs;
    unsigned            nrhs;
    unsigned            maxrhs;
    a_pro               *pro;
    char                buffer[20];
    unsigned            i;
    int                 numacts;
    bool                action_defined;
    bool                unit_production;
    bool                not_token;
    a_SR_conflict_list  *list_of_ambiguities;
    a_SR_conflict_list  *am;

    ambiguousstates = NULL;
    maxrhs = INIT_RHS_SIZE;
    rhs = CALLOC( maxrhs, a_sym * );
    while( token == T_CIDENTIFIER ) {
        int sym_lineno = lineno;
        lhs = addsym( buf );
        if( lhs->token != 0 )
            msg( "%s used on lhs.\n", lhs->name );
        if( startsym == NULL )
            startsym = lhs;
        numacts = 0;
        do {
            action_defined = FALSE;
            precsym = NULL;
            list_of_ambiguities = NULL;
            nrhs = 0;
            scanextra( 0, &precsym, &list_of_ambiguities );
            for( ; token == '{' || token == T_IDENTIFIER; ) {
                if( token == '{' ) {
                    i = bufused;
                    scanextra( bufused, &precsym, &list_of_ambiguities );
                    numacts++;
                    if( token == '{' || token == T_IDENTIFIER ) {
                        sprintf( buffer, "$pro%d", npro );
                        sym = addsym( buffer );
                        copyact( npro, sym, rhs, nrhs, nrhs );
                        addpro( sym, rhs, 0 );
                    } else {
                        copyact( npro, lhs, rhs, 0, nrhs );
                        action_defined = TRUE;
                        break;
                    }
                    bufused -= i;
                    memcpy( buf, &buf[i], bufused );
                } else {
                    sym = addsym( buf );
                    if( value != 0 ) {
                        sym->token = (token_n)value;
                    }
                    if( sym->token != 0 )
                        precsym = sym;
                    scanextra( 0, &precsym, &list_of_ambiguities );
                }
                if( nrhs + 1 > maxrhs ) {
                    maxrhs *= 2;
                    rhs = REALLOC( rhs, maxrhs, a_sym * );
                }
                rhs[nrhs++] = sym;
            }
            unit_production = FALSE;
            if( !action_defined ) {
                if( nrhs > 0 ) {
                    /* { $$ = $1; } is default action */
                    if( defaultwarnflag ) {
                        char *type_lhs = type_name( lhs->type );
                        char *type_rhs = type_name( rhs[0]->type );
                        if( strcmp( type_rhs, type_lhs ) != 0 ) {
                            warn("default action would copy '%s <%s>' to '%s <%s>'\n",
                                rhs[0]->name, type_rhs, lhs->name, type_lhs );
                        }
                    }
                    if( nrhs == 1 ) {
                        unit_production = TRUE;
                    }
                } else {
                    if( sym_lineno == lineno && token == '|' ) {
                        warn( "unexpected epsilon reduction for '%s'?\n", lhs->name );
                    }
                }
            }
            pro = addpro( lhs, rhs, nrhs );
            if( unit_production ) {
                pro->unit = TRUE;
            }
            if( precsym != NULL ) {
                pro->prec = precsym->prec;
            }
            if( list_of_ambiguities ) {
                for( am = list_of_ambiguities; am != NULL; am = am->next ) {
                    am->pro = pro;
                }
                pro->SR_conflicts = list_of_ambiguities;
            }
            if( token == ';' ) {
                do {
                    scan( 0 );
                } while( token == ';' );
            } else if( token != '|' ) {
                if( token == T_CIDENTIFIER ) {
                    msg( "Missing ';'\n" );
                } else {
                    msg( "Incorrect syntax.\n" );
                }
            }
        } while( token == '|' );
    }
    FREE( rhs );

    not_token = FALSE;
    for( sym = symlist; sym != NULL; sym = sym->next ) {
        /* check for special symbols */
        if( sym == eofsym )
            continue;
        if( denseflag ) {
            if( sym->token != 0 && sym->token < TOKEN_DENSE_BASE ) {
                continue;
            }
        } else {
            if( sym->token != 0 && sym->token < TOKEN_SPARSE_BASE ) {
                continue;
            }
        }
        if( sym->pro != NULL && sym->token != 0 ) {
            not_token = TRUE;
            warn( "%s not defined as '%%token'.\n", sym->name );
        }
    }
    if( not_token ) {
        msg( "cannot continue (because of %%token problems)\n" );
    }
    copyUniqueActions();
}

static void copyfile( void )
{
    do {
        fputc( ch, actout );
    } while( nextc() != EOF );
}

void tail( void )
{
    if( token == T_MARK ) {
        copyfile();
    } else if( token != T_EOF ) {
        msg( "Expected end of file.\n" );
    }
}

static void copycurl( void )
{
    do {
        while( ch != '%' && ch != EOF ) {
            fputc( ch, actout );
            nextc();
        }
    } while( nextc() != '}' && ch != EOF );
    nextc();
}

static char *checkAttrib( char *s, char **ptype, char *buff, int *errs,
                          a_sym *lhs, a_sym **rhs, unsigned base, unsigned n )
{
    char        save;
    char        *type;
    int         err_count;
    long        il;

    err_count = 0;
    ++s;
    if( *s == '<' ) {
        ++s;
        type = s;
        s = get_typename( type );
        if( type == s || *s != '>' )  {
            ++err_count;
            msg( "Bad type specifier.\n" );
        }
        save = *s;
        *s = '\0';
        type = strdup( type );
        *s = save;
        ++s;
    } else {
        type = NULL;
    }
    if( *s == '$' ) {
        strcpy( buff, "yyval" );
        if( type == NULL && lhs->type != NULL ) {
            type = strdup( lhs->type );
        }
        ++s;
    } else {
        il = n + 1;
        if( *s == '-' || isdigit( *s ) ) {
            il = strtol( s, &s, 10 );
        }
        if( il > (long)n ) {
            ++err_count;
            msg( "Invalid $ parameter (%ld).\n", il );
        }
        il -= base + 1;
        sprintf( buff, "yyvp[%ld]", il );
        if( type == NULL && il >= 0 && rhs[il]->type != NULL ) {
            type = strdup( rhs[il]->type );
        }
    }
    *ptype = type;
    *errs = err_count;
    return( s );
}

static a_pro *findPro( a_sym *lhs, rule_n pnum )
{
    a_pro       *pro;

    for( pro = lhs->pro; pro != NULL; pro = pro->next ) {
        if( pro->pidx == pnum ) {
            return( pro );
        }
    }
    return( NULL );
}

static void copyUniqueActions( void )
{
    a_pro       *pro;
    char        *s;
    uniq_case   *c;
    uniq_case   *cnext;
    rule_case   *r;
    rule_case   *rnext;
    an_item     *item;

    for( c = caseActions; c != NULL; c = cnext ) {
        cnext = c->next;
        for( r = c->rules; r != NULL; r = rnext ) {
            rnext = r->next;
            fprintf( actout, "case %d:\n", r->pnum );
            pro = findPro( r->lhs, r->pnum );
            fprintf( actout, "/* %s <-", pro->sym->name );
            for( item = pro->items; item->p.sym != NULL; ++item ) {
                fprintf( actout, " %s", item->p.sym->name );
            }
            fprintf( actout, " */\n" );
            FREE( r );
        }
        for( s = c->action; *s != '\0'; ++s ) {
            fputc( *s, actout );
        }
        fprintf( actout, "\nbreak;\n" );
        FREE( c->action );
        FREE( c );
    }
}

static void addRuleToUniqueCase( uniq_case *p, rule_n pnum, a_sym *lhs )
{
    rule_case   *r;

    r = MALLOC( 1, rule_case );
    r->lhs = lhs;
    r->pnum = pnum;
    r->next = p->rules;
    p->rules = r;
}

static void insertUniqueAction( rule_n pnum, char *buf, a_sym *lhs )
{
    uniq_case   **p;
    uniq_case   *c;
    uniq_case   *n;

    p = &caseActions;
    for( c = *p; c != NULL; c = c->next ) {
        if( strcmp( c->action, buf ) == 0 ) {
            ++actionsCombined;
            addRuleToUniqueCase( c, pnum, lhs );
            /* promote to front */
            *p = c->next;
            c->next = caseActions;
            caseActions = c;
            FREE( buf );
            return;
        }
        p = &(c->next);
    }
    n = MALLOC( 1, uniq_case );
    n->action = buf;
    n->rules = NULL;
    n->next = *p;
    *p = n;
    addRuleToUniqueCase( n, pnum, lhs );
}

static char *strpcpy( char *d, char *s )
{
    while( (*d = *s++) != '\0' ) {
        ++d;
    }
    return( d );
}

static void copyact( rule_n pnum, a_sym *lhs, a_sym **rhs, unsigned base, unsigned n )
{
    char        *b;
    char        *p;
    char        *s;
    char        *type;
    unsigned    i;
    int         errs;
    int         total_errs;
    size_t      total_len;
    char        buff[80];

    if( ! lineflag ) {
        /* we don't need line numbers to correspond to the grammar */
        total_errs = 0;
        total_len = strlen( buf ) + 1;
        for( s = buf; *s != '\0'; ) {
            if( *s == '$' ) {
                s = checkAttrib( s, &type, buff, &errs, lhs, rhs, base, n );
                total_len += strlen( buff );
                if( type != NULL ) {
                    total_len += strlen( type ) + 1;
                }
                FREE( type );
                total_errs += errs;
            } else {
                ++s;
            }
        }
        if( total_errs == 0 ) {
            b = MALLOC( total_len, char );
            p = b;
            for( s = buf; *s != '\0'; ) {
                if( *s == '$' ) {
                    s = checkAttrib( s, &type, buff, &errs, lhs, rhs, base, n );
                    p = strpcpy( p, buff );
                    if( type != NULL ) {
                        *p++ = '.';
                        p = strpcpy( p, type );
                        FREE( type );
                    }
                } else {
                    *p++ = *s++;
                }
            }
            *p = '\0';
            insertUniqueAction( pnum, b, lhs );
        }
        return;
    }
    fprintf( actout, "case %d:\n", pnum );
    fprintf( actout, "/* %s <-", lhs->name );
    for( i = 0; i < n; ++i ) {
        fprintf( actout, " %s", rhs[i]->name );
    }
    fprintf( actout, " */\n" );
    lineinfo();
    for( s = buf; *s != '\0'; ) {
        if( *s == '$' ) {
            s = checkAttrib( s, &type, buff, &errs, lhs, rhs, base, n );
            fprintf( actout, "%s", buff );
            if( type != NULL ) {
                fprintf( actout, ".%s", type );
                FREE( type );
            }
        } else {
            fputc( *s++, actout );
        }
    }
    fprintf( actout, "\nbreak;\n" );
}

static void copybal( void )
{
    int         depth;

    depth = 1;
    do {
        addbuf( ch );
        nextc();
        if( lastc() == '/' ) {
            if( ch == '*' ) {
                /* copy a C style comment */
                for( ;; ) {
                    addbuf( ch );
                    nextc();
                    if( ch == EOF )
                        break;
                    if( ch == '/' && lastc() == '*' ) {
                        addbuf( ch );
                        nextc();
                        break;
                    }
                }
            } else if( ch == '/' ) {
                /* copy a C++ style comment */
                for( ;; ) {
                    addbuf( ch );
                    nextc();
                    if( ch == EOF )
                        break;
                    if( ch == '\n' ) {
                        addbuf( ch );
                        nextc();
                        break;
                    }
                }
            }
        }
        if( ch == '"' ) {
            /* copy a string */
            addbuf( ch );
            for( ;; ) {
                nextc();
                if( ch == EOF )
                    break;
                if( ch == '\n' ) {
                    msg( "string literal was not terminated by \" before end of line\n" );
                    break;
                }
                if( ch == '\\' ) {
                    addbuf( ch );
                    nextc();
                    addbuf( ch );
                } else {
                    if( ch == '"' )
                        break;
                    addbuf( ch );
                }
            }
        }
        if( ch == '\'' ) {
            /* copy a character constant */
            addbuf( ch );
            for( ;; ) {
                nextc();
                if( ch == EOF )
                    break;
                if( ch == '\n' ) {
                    msg( "character literal was not terminated by \" before end of line\n" );
                    break;
                }
                if( ch == '\\' ) {
                    addbuf( ch );
                    nextc();
                    addbuf( ch );
                } else {
                    if( ch == '\'' )
                        break;
                    addbuf( ch );
                }
            }
        }
        if( ch == '{' ) {
            ++depth;
        } else if( ch == '}' ) {
            --depth;
        }
    } while( depth > 0 && ch != EOF );
    addbuf( ch );
    addbuf( '\0' );
    nextc();
}

static void lineinfo( void )
{
    if( lineflag ) {
        fprintf( actout, "\n#line %d \"%s\"\n", lineno, srcname );
    }
}

static void addstr( char *p )
{
    while( *p ) {
        addbuf( *(unsigned char *)p );
        ++p;
    }
}

typedef struct xlat_entry {
    int         c;
    char        *x;
} xlat_entry;

static xlat_entry xlat[] = {
    { '~',      "TILDE" },
    { '`',      "BACKQUOTE" },
    { '!',      "EXCLAMATION" },
    { '@',      "AT" },
    { '#',      "SHARP" },
    { '$',      "DOLLAR" },
    { '%',      "PERCENT" },
    { '^',      "XOR" },
    { '&',      "AND" },
    { '*',      "TIMES" },
    { '(',      "LPAREN" },
    { ')',      "RPAREN" },
    { '-',      "MINUS" },
    { '+',      "PLUS" },
    { '=',      "EQUAL" },
    { '[',      "LSQUARE" },
    { ']',      "RSQUARE" },
    { '{',      "LBRACE" },
    { '}',      "RBRACE" },
    { '\\',     "BACKSLASH" },
    { '|',      "OR" },
    { ':',      "COLON" },
    { ';',      "SEMICOLON" },
    { '\'',     "QUOTE" },
    { '"',      "DQUOTE" },
    { '<',      "LT" },
    { '>',      "GT" },
    { '.',      "DOT" },
    { ',',      "COMMA" },
    { '/',      "DIVIDE" },
    { '?',      "QUESTION" },
    { '\0',     NULL }
};

static bool xlat_char( bool special, int c )
{
    xlat_entry  *t;
    char        buff[16];

    if( isalpha( c ) || isdigit( c ) || c == '_' ) {
        if( special ) {
            addbuf( '_' );
        }
        addbuf( c );
        return( FALSE );
    }
    /* NYI: add %translate 'c' XXXX in case user doesn't like our name */
    addbuf( '_' );
    for( t = xlat; t->x != NULL; ++t ) {
        if( t->c == c ) {
            addstr( t->x );
            return( TRUE );
        }
    }
    warn( "'x' token contains unknown character '%c' (\\x%x)\n", c, c );
    addbuf( 'X' );
    sprintf( buff, "%x", c );
    addstr( buff );
    return( TRUE );
}

static void xlat_token( void )
{
    bool special;

    addbuf( 'Y' );
    special = TRUE;
    for( ;; ) {
        nextc();
        if( ch == EOF || ch == '\n' ) {
            msg( "invalid 'x' token" );
            break;
        }
        if( ch == '\'' )
            break;
        if( ch == '\\' ) {
            special = xlat_char( special, ch );
            nextc();
        }
        special = xlat_char( special, ch );
    }
    addbuf( '\0' );
    value = 0;
    token = T_IDENTIFIER;
}

static a_token scan( unsigned used )
{
    bufused = used;
    eatcrud();
    if( isalpha( ch ) ) {
        for( ;; ) {
            addbuf( ch );
            nextc();
            if( isalpha( ch ) )
                continue;
            if( isdigit( ch ) )
                continue;
            if( ch == '_' )
                continue;
            if( ch == '.' )
                continue;
            if( ch == '-' )
                continue;
            break;
        }
        addbuf( '\0' );
        if( eatcrud() == ':' ) {
            nextc();
            token = T_CIDENTIFIER;
        } else {
            token = T_IDENTIFIER;
        }
        value = 0;
    } else if( isdigit( ch ) || ch == '-' ) {
        do {
            addbuf( ch );
        } while( isdigit( nextc() ) );
        addbuf( '\0' );
        token = T_NUMBER;
        value = atoi( buf );
    } else {
        switch( ch ) {
        case '\'':
            if( denseflag && ! translateflag ) {
                msg( "cannot use '+' style of tokens with the dense option\n" );
            }
            if( ! translateflag ) {
                addbuf( '\'' );
                nextc();
                addbuf( ch );
                if( ch == '\\' ) {
                    nextc();
                    addbuf( ch );
                    switch( ch ) {
                    case 'n':  ch = '\n'; break;
                    case 'r':  ch = '\r'; break;
                    case 't':  ch = '\t'; break;
                    case 'b':  ch = '\b'; break;
                    case 'f':  ch = '\f'; break;
                    case '\\': ch = '\\'; break;
                    case '\'': ch = '\''; break;
                    }
                }
                addbuf( '\'' );
                value = ch;
                token = T_IDENTIFIER;
                need( "'" );
            } else {
                xlat_token();
            }
            break;
        case '{':
            token = ch;
            copybal();
            break;
        case '<': case '>': case '|': case ';': case ',':
            token = ch;
            break;
        case EOF:
            token = T_EOF;
            break;
        case '%':
            switch( nextc() ) {
            case '%':
                token = T_MARK;
                break;
            case '{':
                token = T_LCURL;
                break;
            case '}':
                token = T_RCURL;
                break;
            case 'a':
                need( "mbig" );
                token = T_AMBIG;
                break;
            case 'k':
                need( "eyword_id" );
                token = T_KEYWORD_ID;
                break;
            case 'l':
                need( "eft" );
                token = T_LEFT;
                value = L_ASSOC;
                break;
            case 'n':
                need( "onassoc" );
                token = T_NONASSOC;
                value = NON_ASSOC;
                break;
            case 'p':
                need( "rec" );
                token = T_PREC;
                break;
            case 'r':
                need( "ight" );
                token = T_RIGHT;
                value = R_ASSOC;
                break;
            case 's':
                need( "tart" );
                token = T_START;
                break;
            case 't':
                nextc();
                if( ch == 'o' ) {
                    need( "ken" );
                    token = T_TOKEN;
                } else if( ch == 'y' ) {
                    need( "pe" );
                    token = T_TYPE;
                } else {
                    msg( "Expecting %%token or %%type.\n" );
                }
                break;
            case 'u':
                need( "nion" );
                token = T_UNION;
                break;
            default:
                msg( "Unrecognized %% token.\n" );
            }
            break;
        default:
            msg( "Bad token.\n" );
        }
        addbuf( '\0' );
        nextc();
    }
    return( token );
}

static void need( char *pat )
{
    while( *pat ) {
        if( nextc() != *(unsigned char *)pat++ ) {
            msg( "Expected '%c'\n", pat[-1] );
        }
    }
}

static int eatcrud( void )
{
    int prev;

    for( ;; ) {
        switch( ch ) {
        case ' ':
        case '\t':
        case '\r':
        case '\n':
        case '\f':
            nextc();
            break;
        case '/':
            if( nextc() != '*' ) {
                if( ch == '\n' ) {
                    --lineno;
                }
                ungetc( ch, yaccin );
                ch = '/';
                return( ch );
            }
            prev = '\0';
            for( ;; ) {
                if( nextc() == '/' && prev == '*' ) {
                    break;
                }
                if( ch == EOF ) {
                    return( EOF );
                }
                prev = ch;
            }
            nextc();
            break;
        default:
            return( ch );
        }
    }
}

static int nextc( void )
{
    if( (ch = fgetc( yaccin )) == '\r' ) {
        ch = fgetc( yaccin );
    }
    if( ch == '\n' ) {
        ++lineno;
    }
    return( ch );
}

static int lastc( void )
{
    if( bufused > 1 ) {
        return( (unsigned char)buf[bufused - 1] );
    }
    return( '\0' );
}

static void addbuf( int ch )
{
    if( bufused == bufmax ) {
        bufmax += BUF_INCR;
        if( buf != NULL ) {
            buf = REALLOC( buf, bufmax, char );
        } else {
            buf = MALLOC( bufmax, char );
        }
    }
    buf[bufused++] = (char)ch;
}

static char *dupbuf( void )
{
    char *str;

    str = MALLOC( bufused, char );
    memcpy( str, buf, bufused );
    bufused = 0;
    return( str );
}

void parsestats( void )
{
    dumpstatistic( "actions combined", actionsCombined );
}
