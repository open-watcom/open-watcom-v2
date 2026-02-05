/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023-2026 The Open Watcom Contributors. All Rights Reserved.
*    Portions Copyright (c) 1983-2002 Sygggg, Inc. All Rights Reserved.
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


void putnum( FILE *fp, char *name, int i )
{
    fprintf( fp, "#define\t%-20s\t%d\n", name, i );
}

static void preamble( FILE *fp )
{
    action_n        sidx;

    putnum( fp, "SHIFT", -1 );
    putnum( fp, "ERROR", -2 );
    fprintf( fp, "\n"
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

    for( sidx = 0; sidx < nstate; ++sidx ) {
        fprintf( fp, "static int YYNEAR state%d( struct parse_stack *, unsigned );\n", sidx );
    }
}

static void prolog( FILE *fp, action_n sidx )
{
    an_item         **item;

    fprintf( fp, "\nint YYNEAR state%d( parse_stack * yysp, unsigned token )\n/*\n", sidx );
    for( item = statetab[sidx]->items; *item != NULL; ++item ) {
        showitem( *item, " ." );
    }
    fprintf( fp, "*/\n{\n" );
}

#if 0
static void copyact( a_pro *pro, char *indent )
{
    char            *s;
    char            *type;
    int             i;
    a_sym           *lhs;
    an_item         *rhs;
    unsigned        n;
    int             only_default_type;

    if( pro->action == NULL )
        return;   // Default action is noop
    lhs = pro->sym;
    rhs = pro->items;
    fprintf( fp, "%s/* %s <-", indent, lhs->name );
    for( n = 0; rhs[n].p.sym != NULL; ++n ) {
        fprintf( fp, " %s", rhs[n].p.sym->name );
    }
    fprintf( fp, " */\n%s{ YYSTYPE yyval;\n%s\t", indent, indent );
    only_default_type = true;
    for( s = pro->action; *s != '\0'; ) {
        if( *s == '$' ) {
            if( *++s == '<' ) {
                for( type = ++s; *s != '>'; ++s ) {
                    if( *s == '\0' ) {
                        msg( "Bad type specifier.\n" );
                    }
                }
                *s++ = '\0';
                only_default_type = false;
            } else {
                type = NULL;
            }
            if( *s == '$' ) {
                fprintf( fp, "yyval", *s );
                if( type == NULL ) {
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
                fprintf( fp, "yysp[%d].v", i - 1 ); // Adjust yysp first
                if( type == NULL && i >= 1 ) {
                    type = rhs[i - 1].p.sym->type;
                }
            }
            if( type != NULL ) {
                fprintf( fp, ".%s", type );
            }
        } else if( *s == '\n' ) {
            fprintf( fp, "\n\t%s", indent );
            ++s;
        } else {
            fputc( *s++, fp );
        }
    }
    type = lhs->type;
    if( only_default_type && type != NULL ) {
        fprintf( fp, "\n\t%syysp[0].v.%s = yyval.%s;", indent, type, type );
    } else {
        fprintf( fp, "\n\t%syysp[0].v = yyval;", indent );
    }
    fprintf( fp, "\n%s};\n", indent );
}
#endif

static a_state *unique_shift( a_pro *reduced )
/*********************************************
 * See if there is a unique shift when this state is reduced
 */
{
    a_state         *shift_to;
    a_state         *test;
    a_shift_action  *saction;
    action_n        sidx;

    shift_to = NULL;
    for( sidx = 0; sidx < nstate; ++sidx ) {
        test = statetab[sidx];
        for( saction = test->trans; saction->sym != NULL; ++saction ) {
            if( saction->sym == reduced->sym ) {
                /*
                 * Found something that uses this lhs
                 */
                if( shift_to == NULL || shift_to == saction->state ) {
                    /*
                     * This is the first one or it matches the first one
                     */
                    shift_to = saction->state;
                } else {
                    return( NULL );     // Not unique
                }
            }
        }
    }
    return( shift_to );
}

static void reduce( FILE *fp, action_n production, action_n error )
{
    int             plen;
    an_item         *item;
    a_pro           *pro;
    a_state         *shift_to;
    rule_n          pidx;

    if( production == error ) {
        fprintf( fp, "\treturn( ERROR );\n" );
    } else {
        pidx = production - nstate;     // Convert to 0 base
        pro = protab[pidx];
        for( item = pro->items, plen = 0; item->p.sym != NULL; ++item ) {
            ++plen;
        }
        if( plen != 0 ) {
            fprintf( fp, "\tyysp -= %d;\n", plen );
        }
#if 0
        copyact( pro, "\t" );
#endif
//        fprintf( fp, "\tactions( %d, yysp );\n", pidx );
        if( (shift_to = unique_shift( pro )) != NULL ) {
            fprintf( fp, "\tyysp[0].state = state%d;\n", shift_to->sidx );
        } else {
            fprintf( fp, "\t(*yysp[-1].state) ( yysp, %d );\n", pro->sym->token );
        }
        fprintf( fp, "\treturn( %d );\n", plen );
    }
}

static void epilog( FILE *fp )
{
    fprintf( fp, "}\n" );
}

static void gencode( FILE *fp, action_n sidx, token_n *toklist, token_n *s, action_n *actions,
                        token_n default_token, token_n parent_token, action_n error )
{
    action_n        default_action;
    action_n        todo;
    short           token;
    sym_n           sym_idx;
    int             switched;

    prolog( fp, sidx );
    default_action = 0;
    switched = false;
    for( ; toklist < s; ++toklist ) {
        token = *toklist;
        todo = actions[token] ;
        if( token == default_token ) {
            default_action = todo;
        } else if( token != parent_token ) {
            if( ! switched ) {
                fprintf( fp, "    switch( token ) {\n" );
                switched = true;
            }

            for( sym_idx = 0; sym_idx < nsym; ++sym_idx ) {
                if( symtab[sym_idx]->token == token ) {
                    break;
                }
            }
            if( sym_idx == nsym ) {
                fprintf( fp, "    case %d:\n", token );
            } else if( symtab[sym_idx]->name[0] == '\'' ) {
                fprintf( fp, "    case %s:\n", symtab[sym_idx]->name );
            } else {
                fprintf( fp, "    case %d: /* %s */\n", token, symtab[sym_idx]->name );
            }
            if( todo >= nstate ) {
                /*
                 * Reduction or error
                 */
                reduce( fp, todo, error );
            } else {
                /*
                 * Shift
                 */
                fprintf( fp, "\tyysp[0].state = state%d;\n", todo );
                fprintf( fp, "\tbreak;\n" );
            }
        }
    }
    if( switched ) {
        fprintf( fp, "    default: ;\n" );
    }
    todo = actions[parent_token];
    if( todo != error ) {
        /*
         * There is a parent production
         * For now, try parents only when there is no default action
         */
        fprintf( fp, "\treturn( state%d( yysp, token ) );\n", todo );
    } else if( default_action != 0 ) {
        reduce( fp, default_action, error );
    } else {
        fprintf( fp, "\treturn( ERROR );\n" );
    }
    if( switched ) {
        fprintf( fp, "    }\n    return( SHIFT );\n" );
    }
    epilog( fp );
}

#if 0
static void putambig( FILE *fp, int i, int state, int token )
{
    fprintf( fp, "#define\tYYAMBIGS%u\t\t%d\n", i, state );
    fprintf( fp, "#define\tYYAMBIGT%u\t\t%d\n", i, token );
}

static void print_token( int token )
{
    sym_n           sym_idx;

    for( sym_idx = 0; sym_idx < nsym; ++sym_idx ) {
        if( symtab[sym_idx]->token == token ) {
            break;
        }
    }
    if( sym_idx == nsym ) {
        printf( " %d", token );
    } else {
        printf( " %s", symtab[sym_idx]->name );
    }
}
#endif

void genobj( FILE *fp )
{
    token_n         *tokens;
    action_n        *actions;
//    short           *base;
    action_n        *other;
    action_n        *parent;
    token_n         *size;
    token_n         *p;
    token_n         *q;
    token_n         *r;
    token_n         *s;
    action_n        error;
    token_n         tokval;
    action_n        redun;
    token_n         *test;
    token_n         *best;
#if 1
    token_n         *same;
    token_n         *diff;
#endif
    bitnum          *mp;
    a_sym           *sym;
    a_pro           *pro;
    a_state         *state;
    a_shift_action  *saction;
    a_reduce_action *raction;
    token_n         i;
    action_n        sidx;
    action_n        sidx2;
    sym_n           sym_idx;
    token_n         ntoken;
    token_n         dtoken;
    token_n         ptoken;
    unsigned        num_default;
    unsigned        num_parent;
    unsigned        max_savings;
    unsigned        savings;

    num_default = num_parent = 0;
    ntoken = FirstNonTerminalTokenValue();
    dtoken = ntoken++;
    ptoken = ntoken++;
    for( sym_idx = nterm; sym_idx < nsym; ++sym_idx ) {
        symtab[sym_idx]->token = ntoken++;
    }

    error = nstate + npro;
    actions = CALLOC( ntoken, action_n );
    for( i = 0; i < ntoken; ++i ) {
        actions[i] = error;
    }
    preamble( fp );
    tokens = CALLOC( ntoken, token_n );
    test = CALLOC( ntoken, token_n );
    best = CALLOC( ntoken, token_n );
//    base = CALLOC( nstate, short );
    other = CALLOC( nstate, action_n );
    parent = CALLOC( nstate, action_n );
    size = CALLOC( nstate, token_n );
    for( sidx = nstate; sidx-- > 0; ) {
        for( i = 0; i < ntoken; ++i ) {
            actions[i] = error;
        }
        state = statetab[sidx];
        q = tokens;
        for( saction = state->trans; (sym = saction->sym) != NULL; ++saction ) {
            tokval = sym->token;
            *q++ = tokval;
            actions[tokval] = saction->state->sidx;
        }
        max_savings = 0;
        for( raction = state->redun; (pro = raction->pro) != NULL; ++raction ) {
            mp = Members( raction->follow );
            savings = mp - setmembers;
            if( savings == 0 )
                continue;
            redun = pro->pidx + nstate;
            if( max_savings < savings ) {
                max_savings = savings;
                r = q;
            }
            protab[pro->pidx]->used = true;
            while( mp-- != setmembers ) {
                sym_idx = *mp;
                tokval = symtab[sym_idx]->token;
                *q++ = tokval;
                actions[tokval] = redun;
            }
        }
        if( max_savings ) {
            tokval = other[sidx] = actions[*r];
            *q++ = dtoken;
            actions[dtoken] = tokval;
            p = r;
            while( max_savings-- > 0 )
                actions[*p++] = error;
            while( p < q )
                *r++ = *p++;
            q = r;
            ++num_default;
        } else {
            other[sidx] = error;
        }
        r = q;
        size[sidx] = r - tokens;
        max_savings = 0;
        parent[sidx] = nstate;
        for( sidx2 = nstate; --sidx2 > sidx; ) {
            /*
             * FOR NOW -- only use parent if no default here or same default
             */
            if( other[sidx] != error && other[sidx] != other[sidx2] )
                continue;
            savings = 0;
            state = statetab[sidx2];
            p = test;
            q = test + ntoken;
            for( saction = state->trans; (sym = saction->sym) != NULL; ++saction ) {
                tokval = sym->token;
                if( actions[tokval] == saction->state->sidx ) {
                    ++savings;
                    *p++ = tokval;
                } else {
                    if( actions[tokval] == error )
                        --savings;
                    *--q = tokval;
                }
            }
            for( raction = state->redun; (pro = raction->pro) != NULL; ++raction ) {
                if( (redun = pro->pidx + nstate) == other[sidx2] )
                    redun = error;
                redun = pro->pidx + nstate;
                for( mp = Members( raction->follow ); mp-- != setmembers; ) {
                    sym_idx = *mp;
                    tokval = symtab[sym_idx]->token;
                    if( actions[tokval] == redun ) {
                        ++savings;
                        *p++ = tokval;
                    } else {
                        if( actions[tokval] == error )
                            --savings;
                        *--q = tokval;
                    }
                }
            }
            if( other[sidx2] != error ) {
                if( other[sidx2] == other[sidx] ) {
                    ++savings;
                    *p++ = dtoken;
                } else {
                    *--q = dtoken;
                }
            }
#if 0
            printf( "state %d calling state %d saves %d:", sidx, sidx2, savings );
            for( s = test; s < p; ++s ) {
                print_token( *s );
            }
            printf( " costs" );
            for( s = test + ntoken; --s >= q; ) {
                if( actions[*s] == error ) {
                    print_token( *s );
                }
            }
            printf( "\n" );
#endif
            if( max_savings < savings ) {
                max_savings = savings;
                same = p;
                diff = q;
                s = test;  test = best;  best = s;
                parent[sidx] = sidx2;
            }
        }
        if( max_savings < 1 ) { // Could raise threshold for performance
            s = r;
        } else {
            ++num_parent;
            s = tokens;
            p = same;
            while( p-- > best )
                actions[*p] = error;
            for( q = tokens; q < r; ++q ) {
                if( actions[*q] != error ) {
                    *s++ = *q;
                }
            }
            p = best + ntoken;
            while( p-- > diff ) {
                if( actions[*p] == error ) {
                    *s++ = *p;
                }
            }
            tokval = parent[sidx];
            *s++ = ptoken;
            actions[ptoken] = tokval;
        }
        gencode( fp, sidx, tokens, s, actions, dtoken, ptoken, error );
        while( s-- > tokens ) {
            actions[*s] = error;
        }
    }
#if 0
    for( i = 0; i < nambig; ++i ) {
        putambig( fp, i, base[ambiguities[i].state], ambiguities[i].token );
    }
#else
    putambigs( fp, NULL );
#endif
    putnum( fp, "YYNOACTION", ( error - nstate ) + dtoken );
    putnum( fp, "YYEOFTOKEN", eofsym->token );
    putnum( fp, "YYERRTOKEN", errsym->token );
    putnum( fp, "YYERR", errstate->sidx );
    fprintf( fp, "#define YYSTART   state%d\n", startstate->sidx );
    fprintf( fp, "#define YYSTOP    state%d\n", eofsym->state->sidx );
    printf( "%u states, %u with defaults, %u with parents\n", nstate, num_default, num_parent );
}
