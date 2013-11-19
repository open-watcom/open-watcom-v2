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
#include <limits.h>
#include <ctype.h>
#include "yacc.h"
#include "alloc.h"

extern FILE *actout;

extern char *loadpath;

extern a_sym **symtab, *symlist, *startsym, *eofsym, *errsym, *goalsym, *nosym;
extern a_pro **protab;

extern a_state **statetab, *statelist, **statetail, *startstate, *errstate;

extern unsigned wperset;
extern short *setmembers;

static void putnum( char *name, int i )
{
    fprintf( actout, "#define\t%-20s\t%d\n", name, i );
}

static void preamble()
{
    int         i;

    putnum( "SHIFT", -1 );
    putnum( "ERROR", -2 );
    fprintf( actout, "\n"
                     "#ifndef YYSTYPE\n"
                     "    #define YYSTYPE int\n"
                     "#endif\n"
                     "\n"
                     "typedef struct parse_stack {\n"
                     "    int (YYNEAR *state) (struct parse_stack *, unsigned );\n"
                     "    YYSTYPE v;\n"
                     "} parse_stack;\n\n"
                     "\n"
                     );

    for( i = 0; i < nstate; ++ i ) {
        fprintf( actout,
                 "static int YYNEAR state%d( struct parse_stack *, unsigned );\n", i );
    }
}

static void prolog( int i )
{
    a_name              name;
    a_state *           x;

    fprintf( actout, "\nint YYNEAR state%d( parse_stack * yysp, unsigned token )\n/*\n", i );
    x = statetab[i];
    for( name.item = x->name.item; *name.item != NULL; ++name.item ) {
        showitem( actout, *name.item, " ." );
    }
    fprintf( actout, "*/\n{\n" );
}

static void copyact( a_pro * pro, char * indent )
{
    char        *s;
    char        *type;
    int         i;
    a_sym *     lhs;
    an_item *   rhs;
    unsigned    n;
    int         only_default_type;

    if( pro->action == NULL ) return;   // Default action is noop
    lhs = pro->sym;
    rhs = pro->item;
    fprintf( actout, "%s/* %s <-", indent, lhs->name );
    for( n = 0; rhs[n].p.sym != NULL; ++ n ) {
        fprintf( actout, " %s", rhs[n].p.sym->name );
    }
    fprintf( actout, " */\n%s{ YYSTYPE yyval;\n%s\t", indent, indent );
    only_default_type = TRUE;
    for( s = pro->action; *s != '\0'; ){
        if( *s == '$' ){
            if( *++s == '<' ){
                for( type = ++s; *s != '>'; ++s ) {
                    if( *s == '\0' ) {
                        msg( "Bad type specifier.\n" );
                    }
                }
                *s++ = '\0';
                only_default_type = FALSE;
            } else {
                type = NULL;
            }
            if( *s == '$' ){
                fprintf( actout, "yyval", *s );
                if( !type ) {
                    type = lhs->type;
                }
                ++s;
            } else {
                if( *s == '-' || isdigit( *s ) ) {
                    i = strtol( s, &s, 10 );
                }
                if( i >= 0 && i > n ) {
                    msg( "Invalid $ parameter.\n" );
                }
                fprintf( actout, "yysp[%d].v", i - 1 ); // Adjust yysp first
                if( !type && i >= 1 ) {
                    type = rhs[i - 1].p.sym->type;
                }
            }
            if( type ) {
                fprintf( actout, ".%s", type );
            }
        } else if( *s == '\n' ) {
            fprintf( actout, "\n\t%s", indent );
            ++s;
        } else {
            fputc( *s++, actout );
        }
    }
    type = lhs->type;
    if( only_default_type && (type = lhs->type) != NULL ) {
        fprintf( actout, "\n\t%syysp[0].v.%s = yyval.%s;", indent, type, type );
    } else {
        fprintf( actout, "\n\t%syysp[0].v = yyval;", indent );
    }
    fprintf( actout, "\n%s};\n", indent );
}

static a_state * unique_shift( a_pro * reduced )
{
    // See if there is a unique shift when this state is reduced

    a_state *           shift_to;
    a_state *           test;
    a_shift_action *    tx;
    int                 i;

    shift_to = NULL;
    for( i = 0; i < nstate; ++ i ) {
        test = statetab[i];
        for( tx = test->trans; tx->sym != NULL; ++tx ) {
            if( tx->sym == reduced->sym ) {
                // Found something that uses this lhs
                if( shift_to == NULL || shift_to == tx->state ) {
                    // This is the first one or it matches the first one
                    shift_to = tx->state;
                } else {
                    return( NULL );     // Not unique
                }
            }
        }
    }
    return( shift_to );
}

static void reduce( int production, int error )
{
    int                 plen;
    an_item *           item;
    a_pro *             pro;
    a_state *           shift_to;

    if( production == error ) {
        fprintf( actout, "\treturn( ERROR );\n" );
    } else {
        production -= nstate;           // Convert to 0 base
        pro = protab[production];
        for( item = pro->item, plen = 0; item->p.sym != NULL; ++item ) {
            ++plen;
        }
        if( plen != 0 ) {
            fprintf( actout, "\tyysp -= %d;\n", plen );
        }
        copyact( pro, "\t" );
        // fprintf( actout, "\tactions( %d, yysp );\n", production );
        if( (shift_to = unique_shift( pro )) != NULL ) {
            fprintf( actout, "\tyysp[0].state = state%d;\n", shift_to->sidx );
        } else {
            fprintf( actout, "\t(*yysp[-1].state) ( yysp, %d );\n",
                                    pro->sym->token );
        }
        fprintf( actout, "\treturn( %d );\n", plen );
    }
}

static void gencode( int statenum, short *toklist, short *s, short *action,
                        short default_token, short parent_token, short error )
{
    short               default_action;
    short               todo;
    short               token;
    int                 symnum;
    int                 switched;

    prolog( statenum );
    default_action = 0;
    switched = FALSE;
    for( ; toklist < s; ++ toklist ) {
        token = *toklist;
        todo = action[token] ;
        if( token == default_token ) {
            default_action = todo;
        } else if( token != parent_token ) {
            if( ! switched ) {
                fprintf( actout, "    switch( token ) {\n" );
                switched = TRUE;
            }

            for( symnum = 0; symnum < nsym; ++ symnum ) {
                if( symtab[symnum]->token == token ) break;
            }
            if( symnum == nsym ) {
                fprintf( actout, "    case %d:\n", token );
            } else if( symtab[symnum]->name[0] == '\'' ) {
                fprintf( actout, "    case %s:\n", symtab[symnum]->name );
            } else {
                fprintf( actout, "    case %d: /* %s */\n", token,
                                    symtab[symnum]->name );
            }
            if( todo >= nstate ) {
                // Reduction or error
                reduce( todo, error );
            } else {
                // Shift
                fprintf( actout, "\tyysp[0].state = state%d;\n", todo );
                fprintf( actout, "\tbreak;\n" );
            }
        }
    }
    if( switched ) {
        fprintf( actout, "    default: ;\n" );
    }
    todo = action[parent_token];
    if( todo != error ) {
        // There is a parent production
        // For now, try parents only when there is no default action
        fprintf( actout, "\treturn( state%d( yysp, token ) );\n", todo );
    } else if( default_action != 0 ) {
        reduce( default_action, error );
    } else {
        fprintf( actout, "\treturn( ERROR );\n" );
    }
    if( switched ) {
        fprintf( actout, "    }\n    return( SHIFT );\n" );
    }
    epilog();
}

static void epilog()
{
    fprintf( actout, "}\n" );
}

static void putambig( int i, int state, int token )
{
    fprintf( actout, "#define\tYYAMBIGS%u\t\t%d\n", i, state );
    fprintf( actout, "#define\tYYAMBIGT%u\t\t%d\n", i, token );
}

static void print_token( int token )
{
    int symnum;

    for( symnum = 0; symnum < nsym; ++ symnum ) {
        if( symtab[symnum]->token == token ) break;
    }
    if( symnum == nsym ) {
        printf( " %d", token );
    } else {
        printf( " %s", symtab[symnum]->name );
    }
}

void genobj( void )
{
    short *token, *action, *base, *other, *parent, *size;
    register short *p, *q, *r, *s;
    short error, tokval, redun, *test, *best;
#if 1
    short *same, *diff;
#endif
    short *mp;
    a_sym *sym;
    a_pro *pro;
    a_state *x;
    a_shift_action *tx;
    a_reduce_action *rx;
    int i, j, savings, max, ntoken, dtoken, ptoken;
    unsigned num_default, num_parent;

    num_default = num_parent = 0;
    ntoken = 0;
    for( i = 0; i < nterm; ++i ) {
        if( (j = symtab[i]->token) > ntoken ) {
            ntoken = j;
        }
    }
    dtoken = ++ntoken;
    ptoken = ++ntoken;
    for( i = nterm; i < nsym; ++i ) {
        symtab[i]->token = ++ntoken;
    }
    ++ntoken;

    error = nstate + npro;
    action = CALLOC( ntoken, short );
    for( i = 0; i < ntoken; ++i ) {
        action[i] = error;
    }
    preamble();
    token = CALLOC( ntoken, short );
    test = CALLOC( ntoken, short );
    best = CALLOC( ntoken, short );
    base = CALLOC( nstate, short );
    other = CALLOC( nstate, short );
    parent = CALLOC( nstate, short );
    size = CALLOC( nstate, short );
    for( i = nstate; --i >= 0; ) {
        for( s = action + ntoken; --s >= action; ) {
            *s = error;
        }
        x = statetab[i];
        q = token;
        for( tx = x->trans; (sym = tx->sym) != NULL; ++tx ) {
            tokval = sym->token;
            *q++ = tokval;
            action[tokval] = tx->state->sidx;
        }
        savings = 0;
        for( rx = x->redun; (pro = rx->pro) != NULL; ++rx ) {
            redun = pro->pidx + nstate;
            mp = Members( rx->follow );
            if( mp - setmembers > savings ) {
                savings = mp - setmembers;
                r = q;
            }
            if( mp - setmembers ) {
                protab[pro->pidx]->used = TRUE;
            }
            while( --mp >= setmembers ) {
                tokval = symtab[*mp]->token;
                *q++ = tokval;
                action[tokval] = redun;
            }
        }
        if( savings ) {
            tokval = other[i] = action[*r];
            *q++ = dtoken;
            action[dtoken] = tokval;
            p = r;
            while( --savings >= 0 )
                action[*p++] = error;
            while( p < q )
                *r++ = *p++;
            q = r;
            ++ num_default;
        } else
            other[i] = error;
        r = q;
        size[i] = r - token;
        max = 0;
        parent[i] = nstate;
        for( j = nstate; --j > i; ) {
            // FOR NOW -- only use parent if no default here or same default
            if( other[i] != error && other[i] != other[j] ) continue;
            savings = 0;
            x = statetab[j];
            q = (p = test) + ntoken;
            for( tx = x->trans; (sym = tx->sym) != NULL; ++tx )
                if( action[sym->token] == tx->state->sidx ) {
                    ++ savings;
                    *p++ = sym->token;
                } else {
                    if( action[sym->token] == error ) -- savings;
                    *--q = sym->token;
                }
            for( rx = x->redun; (pro = rx->pro) != NULL; ++rx ) {
                if( (redun = pro->pidx + nstate) == other[j] )
                    redun = error;
                redun = pro->pidx + nstate;
                for( mp = Members( rx->follow ); --mp >= setmembers; ) {
                    tokval = symtab[*mp]->token;
                    if( action[tokval] == redun ) {
                        ++ savings;
                        *p++ = tokval;
                    } else {
                        if( action[tokval] == error ) -- savings;
                        *--q = tokval;
                    }
                }
            }
            if( other[j] != error ) {
                if( other[j] == other[i] ) {
                    ++ savings;
                    *p++ = dtoken;
                } else {
                    *--q = dtoken;
                }
            }
#if 0
            printf( "state %d calling state %d saves %d:",
                        i, j, savings );
            for( s = test; s < p; ++ s ) {
                print_token( *s );
            }
            printf( " costs" );
            for( s = test + ntoken; --s >= q; ) {
                if( action[*s] == error ) {
                    print_token( *s );
                }
            }
            printf( "\n" );
#endif
            if( savings > max ) {
                max = savings;
                same = p;
                diff = q;
                s = test;  test = best;  best = s;
                parent[i] = j;
            }
        }
        if( max < 1 ) { // Could raise threshold for performance
            s = r;
        } else {
            ++ num_parent;
            s = token;
            p = same;
            while( --p >= best )
                action[*p] = error;
            for( q = token; q < r; ++q )
                if( action[*q] != error )
                    *s++ = *q;
            p = best + ntoken;
            while( --p >= diff )
                if( action[*p] == error )
                    *s++ = *p;
            tokval = parent[i];
            *s++ = ptoken;
            action[ptoken] = tokval;
        }
        gencode( i, token, s, action, dtoken, ptoken, error );
        while( --s >= token )
            action[*s] = error;
    }
    for( i = 0; i < nambig; ++i ) {
        putambig( i, base[ambiguities[i].state], ambiguities[i].token );
    }
    putnum( "YYNOACTION", error - nstate + dtoken );
    putnum( "YYEOFTOKEN", eofsym->token );
    putnum( "YYERRTOKEN", errsym->token );
    putnum( "YYETOKEN", errsym->token );
    putnum( "YYERR", errstate->sidx );
    fprintf( actout, "#define YYSTART   state%d\n", startstate->sidx );
    fprintf( actout, "#define YYSTOP    state%d\n", eofsym->enter->sidx );
    printf( "%u states, %u with defaults, %u with parents\n",
                nstate, num_default, num_parent );
}
