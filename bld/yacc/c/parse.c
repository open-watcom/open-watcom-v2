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

typedef enum {
    IDENTIFIER = 256,   /* includes identifiers and literals */
    C_IDENTIFIER,       /* identifier (but not literal) followed by colon */
    NUMBER,             /* -?[0-9]+ */
    MARK,               /* %% */
    LCURL,              /* %{ */
    RCURL,              /* }% */
    AMBIG,              /* %keywords */
    KEYWORD_ID,
    LEFT,
    RIGHT,
    NONASSOC,
    TOKEN,
    PREC,
    TYPE,
    START,
    UNION,
    TYPENAME,
} a_token;

static void                 copycurl( void );
static void                 copyUniqueActions( void );
static void                 copyact( int, a_sym *, a_sym **, unsigned, unsigned );
static void                 lineinfo( void );
static a_token              scan( unsigned used );
static void                 need( char *pat );
static int                  eatcrud( void );
static int                  nextc( void );
static int                  lastc( void );
static void                 addbuf( int ch );
static char                 *dupbuf( void );

int lineno = { 1 };

#define BUF_INCR            500

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
    int         pnum;
    a_sym       *lhs;
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

static a_token scan_typename( unsigned used )
{
    bufused = used;
    if( TYPENAME_FIRST_CHAR( ch ) ) {
        do {
            addbuf( ch );
            nextc();
        } while( TYPENAME_NEXT_CHAR( ch ) );
        token = TYPENAME;
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

static a_sym *make_sym( char *name, token_t value )
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

void defs( void )
{
    token_t     gentoken;
    a_sym       *sym;
    int         ctype;
    char        *dupbuf( void );
    char        *type;
    a_prec      prec;

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
    for( ; token != MARK; ) {
        switch( token ) {
        case START:
            if( scan( 0 ) != IDENTIFIER )
                msg( "Identifier needed after %%start.\n" );
            startsym = addsym( buf );
            scan( 0 );
            break;
        case UNION:
            if( scan( 0 ) != '{' ) {
                msg( "Need '{' after %%union.\n" );
            }
            fprintf( actout, "#ifndef __YYSTYPE_DEFINED\n" );
            fprintf( actout, "#define __YYSTYPE_DEFINED\n" );
            fprintf( actout, "typedef union " );
            lineinfo();
            fprintf( actout, "%s YYSTYPE;\n", buf );
            fprintf( actout, "#endif\n" );
            fprintf( tokout, "#ifndef __YYSTYPE_DEFINED\n" );
            fprintf( tokout, "#define __YYSTYPE_DEFINED\n" );
            fprintf( tokout, "typedef union %s YYSTYPE;\n", buf );
            fprintf( tokout, "#endif\n" );
            scan( 0 );
            break;
        case LCURL:
            lineinfo();
            copycurl();
            scan( 0 );
            break;
        case KEYWORD_ID:
            switch( scan( 0 ) ) {
            case IDENTIFIER:
                sym = addsym( buf );
                if( sym->token == 0 ) {
                    msg( "Token must be assigned number before %keyword_id\n" );
                }
                value = sym->token;
                break;
            case NUMBER:
                break;
            default:
                msg( "Expecting identifier or number.\n" );
            }
            keyword_id_low = value;
            switch( scan( 0 ) ) {
            case IDENTIFIER:
                sym = addsym( buf );
                if( sym->token == 0 ) {
                    msg( "Token must be assigned number before %keyword_id\n" );
                }
                value = sym->token;
                break;
            case NUMBER:
                break;
            default:
                msg( "Expecting identifier or number.\n" );
            }
            keyword_id_high = value;
            scan( 0 );
            break;
        case LEFT:
        case RIGHT:
        case NONASSOC:
            ++prec.prec;
            prec.assoc = value;
            // pass through
        case TOKEN:
        case TYPE:
            ctype = token;
            if( scan( 0 ) == '<' ) {
                if( scan_typename( 0 ) != TYPENAME ) {
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
            while( token == IDENTIFIER ) {
                sym = addsym( buf );
                if( type != NULL && sym->type != NULL ) {
                    if( strcmp( sym->type, type ) != 0 ) {
                        msg( "'%s' type redeclared from '%s' to '%s'\n",
                            buf, sym->type, type );
                    }
                }
                sym->type = type;
                if( ctype == TYPE ) {
                    scan( 0 );
                } else {
                    if( sym->token == 0 ) {
                        sym->token = value;
                    }
                    if( ctype != TOKEN ) {
                        sym->prec = prec;
                    }
                    if( scan( 0 ) == NUMBER ) {
                        if( sym->token ) {
                            if( sym->name[0] != '\'' ) {
                                fprintf( tokout, "#undef\t%-20s\n", sym->name );
                            }
                        }
                        sym->token = value;
                        scan( 0 );
                    }
                    if( sym->token == 0 ) {
                        sym->token = gentoken++;
                    }
                    if( sym->name[0] != '\'' ) {
                        fprintf( tokout, "#define\t%-20s\t0x%02x\n", sym->name, sym->token );
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
}

static int scanambig( unsigned used, a_SR_conflict_list **list )
{
    int                     absorbed_something;
    unsigned                index;
    a_sym                   *sym;
    a_SR_conflict           *am;
    a_SR_conflict_list      *en;

    absorbed_something = FALSE;
    for( ; token == AMBIG; ) {
        /* syntax is "%ambig <number> <token>" */
        /* token has already been scanned by scanprec() */
        if( scan( used ) != NUMBER || value < 0 ) {
            msg( "Expecting a non-negative number after %ambig.\n" );
            break;
        }
        index = value;
        if( scan( used ) != IDENTIFIER ) {
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

static int scanprec( unsigned used, a_sym **precsym )
{
    if( token != PREC )
        return( FALSE );
    if( scan( used ) != IDENTIFIER || (*precsym = findsym( buf )) == NULL || !(*precsym)->token ) {
        msg( "Expecting a token after %prec.\n" );
    }
    scan( used );
    return( TRUE );
}

static void scanextra( unsigned used, a_sym **psym, a_SR_conflict_list **pSR )
{
    scan( used );
    for( ;; ) {
        if( ! scanprec( used, psym ) && ! scanambig( used, pSR ) ) {
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
    unsigned            maxrhs = { 16 };
    a_pro               *pro;
    char                buffer[20];
    unsigned            i;
    int                 numacts;
    bool                action_defined;
    bool                unit_production;
    bool                not_token;
    a_SR_conflict_list  *list_of_ambiguities;
    a_SR_conflict_list  *am;

    rhs = CALLOC( maxrhs, a_sym * );
    while( token == C_IDENTIFIER ) {
        int sym_lineno = lineno;
        lhs = addsym( buf );
        if( lhs->token )
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
            for( ; token == '{' || token == IDENTIFIER; ) {
                if( nrhs + 2 > maxrhs )
                    rhs = REALLOC( rhs, maxrhs *= 2, a_sym * );
                if( token == '{' ) {
                    i = bufused;
                    scanextra( bufused, &precsym, &list_of_ambiguities );
                    numacts++;
                    if( token == '{' || token == IDENTIFIER ) {
                        sprintf( buffer, "$pro%d", npro );
                        sym = addsym( buffer );
                        copyact( npro, sym, rhs, nrhs, nrhs );
                        addpro( sym, rhs, 0 );
                    } else {
                        copyact( npro, lhs, rhs, 0, nrhs );
                        action_defined = TRUE;
                        break;
                    }
                    memcpy( buf, &buf[i], bufused -= i );
                } else {
                    sym = addsym( buf );
                    if( value )
                        sym->token = value;
                    if( sym->token )
                        precsym = sym;
                    scanextra( 0, &precsym, &list_of_ambiguities );
                }
                rhs[nrhs++] = sym;
            }
            unit_production = FALSE;
            if( ! action_defined ) {
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
                if( token == C_IDENTIFIER ) {
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
    if( token == MARK ) {
        copyfile();
    } else if( token != EOF ) {
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
        if( il >= 0 && il > (long)n ) {
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

static a_pro *findPro( a_sym *lhs, unsigned pnum )
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
    uniq_case   *nc;
    rule_case   *r;
    rule_case   *nr;
    an_item     *first_item;
    an_item     *item;

    for( c = caseActions; c != NULL; c = nc ) {
        nc = c->next;
        for( r = c->rules; r != NULL; r = nr ) {
            nr = r->next;
            fprintf( actout, "case %d:\n", r->pnum );
            pro = findPro( r->lhs, r->pnum );
            fprintf( actout, "/* %s <-", pro->sym->name );
            first_item = pro->item;
            for( item = first_item; item->p.sym != NULL; ++item ) {
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

static void addRuleToUniqueCase( uniq_case *p, int pnum, a_sym *lhs )
{
    rule_case   *r;

    r = MALLOC(1,rule_case);
    r->lhs = lhs;
    r->pnum = pnum;
    r->next = p->rules;
    p->rules = r;
}

static void insertUniqueAction( int pnum, char *buf, a_sym *lhs )
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
    n = MALLOC(1,uniq_case);
    n->action = buf;
    n->rules = NULL;
    n->next = *p;
    *p = n;
    addRuleToUniqueCase( n, pnum, lhs );
}

static char *strpcpy( char *d, char *s )
{
    while( (*d = *s++) ) {
        ++d;
    }
    return( d );
}

static void copyact( int pnum, a_sym *lhs, a_sym **rhs, unsigned base, unsigned n )
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

static int xlat_char( int special, int c )
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
    int special;

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
    token = IDENTIFIER;
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
            token = C_IDENTIFIER;
        } else {
            token = IDENTIFIER;
        }
        value = 0;
    } else if( isdigit( ch ) || ch == '-' ) {
        do {
            addbuf( ch );
        } while( isdigit( nextc() ) );
        addbuf( '\0' );
        token = NUMBER;
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
                token = IDENTIFIER;
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
        case EOF:
            token = ch;
            break;
        case '%':
            switch( nextc() ) {
            case '%':
                token = MARK;
                break;
            case '{':
                token = LCURL;
                break;
            case '}':
                token = RCURL;
                break;
            case 'a':
                need( "mbig" );
                token = AMBIG;
                break;
            case 'k':
                need( "eyword_id" );
                token = KEYWORD_ID;
                break;
            case 'l':
                need( "eft" );
                token = LEFT;
                value = L_ASSOC;
                break;
            case 'n':
                need( "onassoc" );
                token = NONASSOC;
                value = NON_ASSOC;
                break;
            case 'p':
                need( "rec" );
                token = PREC;
                break;
            case 'r':
                need( "ight" );
                token = RIGHT;
                value = R_ASSOC;
                break;
            case 's':
                need( "tart" );
                token = START;
                break;
            case 't':
                nextc();
                if( ch == 'o' ) {
                    need( "ken" );
                    token = TOKEN;
                } else if( ch == 'y' ) {
                    need( "pe" );
                    token = TYPE;
                } else {
                    msg( "Expecting %%token or %%type.\n" );
                }
                break;
            case 'u':
                need( "nion" );
                token = UNION;
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
    buf[bufused++] = ch;
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
