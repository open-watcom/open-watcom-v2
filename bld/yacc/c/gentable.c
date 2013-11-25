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
#include "yacc.h"
#include "alloc.h"

#define ACTION_USED     0x4000
#define ACTION_BASE     0x8000

#define Token(c)        ((c).token)
#define SetToken(c,i)   ((c).token = (i))
#define IsUsed(c)       ((c).action & ACTION_USED)
#define Action(c)       ((c).action & ACTION_MASK)
#define SetAction(c,i)  ((c).action |= (ACTION_USED|(i)))
#define IsBase(c)       ((c).action & ACTION_BASE)
#define SetBase(c)      ((c).action |= ACTION_BASE)
#define roundup(a,b)    ((((a)+(b)-1)/(b))*(b))

#define BLOCK           512

typedef struct a_table {
    token_n         token;
    action_n        action;
} a_table;

static unsigned     avail;
static unsigned     used;
static a_table      *table;

static unsigned addtotable( token_n *token, token_n *end_token, action_n *actions,
                            token_n default_token, token_n parent_token )
{
    unsigned            start, i;
    token_n             max;
    action_n            default_action;
    token_n             *r;
    a_table             *t;
    action_n            actval;
    token_n             tokval;

    if( compactflag ) {
        start = used++;         // Leave room for parent & default
        default_action = ACTION_NULL;
        for( ;; ++token ) {
            if( used >= avail ) {
                avail = roundup( used + 1, BLOCK );
                if( table != NULL ) {
                    table = REALLOC( table, avail, a_table );
                } else {
                    table = MALLOC( avail, a_table );
                }
            }
            if( token == end_token )
                break;
            if( *token == default_token ) {
                default_action = actions[*token];
            } else if( *token != parent_token ) {
                tokval = *token;
                actval = actions[tokval];
                if( tokval > 0x0FFF ) {
                    printf( "Error: token index 0x%X for item %d is higher then 0x0FFF !\n", tokval, start );
                }
                if( actval > 0x0FFF ) {
                    printf( "Error: token action 0x%X for item %d is higher then 0x0FFF !\n", actval, start );
                }
                table[used].token = tokval;
                table[used].action = actval;
                ++used;
            }
        }
        actval = actions[parent_token];
        if( actval > 0x0FFF ) {
            printf( "Error: parent action 0x%X for item %d is higher then 0x0FFF !\n", actval, start );
        }
        if( default_action > 0x0FFF ) {
            printf( "Error: default action 0x%X for item %d is higher then 0x0FFF !\n", default_action, start );
        }
        table[start].token = (token_n)actval;
        table[start].action = default_action;
    } else {
        max = *token;
        for( r = token + 1; r < end_token; ++r ) {
            if( *r > max ) {
                max = *r;
            }
        }
        for( start = 0; ; ++start ) {
            if( start + max >= avail ) {
                i = avail;
                avail = roundup( start + max + 1, BLOCK );
                if( table != NULL ) {
                    table = REALLOC( table, avail, a_table );
                } else {
                    table = MALLOC( avail, a_table );
                }
                while( i < avail ) {
                    table[i].token = TOKEN_IMPOSSIBLE;
                    table[i].action = ACTION_NULL;
                    ++i;
                }
            }
            if( !IsBase( table[start] ) ) {
                for( r = token; r < end_token; ++r ) {
                    if( IsUsed( table[start + *r] ) ) {
                        goto contin2;
                    }
                }
                break;
            }
contin2:;
        }
        SetBase( table[start] );
        for( r = token; r < end_token; ++r ) {
            t = &table[start + *r];
            if( ! bigflag ) {
                if( *r >= UCHAR_MAX ) {
                    msg( "too many tokens!\n" );
                }
            }
            SetToken( *t, *r );
            actval = actions[*r];
            if( (actval & ACTION_MASK) != actval ) {
                printf( "Error: token action 0x%X for token %d is higher then 0x3FFF !\n", actval, *r );
            }
            SetAction( *t, actions[*r] );
        }
        i = start + max + 1;
        if( i > used ) {
            used = i;
        }
    }
    return( start );
}

void genobj( void )
{
    value_size  token_size;
    action_n    *actions, *parent, *other;
    base_n      *base;
    token_n     *p, *q, *r, *s;
    token_n     *tokens, *same, *diff, *test, *best;
    set_size    *mp;
    token_n     tokval, dtoken, ptoken, ntoken;
    action_n    actval, error, redun, new_action;
    a_sym       *sym;
    a_pro       *pro;
    an_item     *item;
    a_state     *x;
    a_shift_action *tx;
    a_reduce_action *rx;
    index_n     i, j;
    set_size    savings, min, *size;
    set_size    shift;
    unsigned    parent_base;
    unsigned    num_default, num_parent;

    if( fastflag ) {
        GenFastTables();
        return;
    }
    if( bigflag || compactflag ) {
        token_size = FITS_A_WORD;
    } else {
        token_size = FITS_A_BYTE;
    }
    num_default = 0;
    num_parent = 0;

    ntoken = FirstNonTerminalTokenValue();
    dtoken = ntoken++;
    ptoken = ntoken++;
    for( i = nterm; i < nsym; ++i ) {
        symtab[i]->token = ntoken++;
    }
    actions = CALLOC( ntoken, action_n );
    error = nstate + npro;
    for( i = 0; i < ntoken; ++i ) {
        actions[i] = error;
    }
    tokens = CALLOC( ntoken, token_n );
    test = CALLOC( ntoken, token_n );
    best = CALLOC( ntoken, token_n );
    other = CALLOC( nstate, action_n );
    parent = CALLOC( nstate, action_n );
    size = CALLOC( nstate, set_size );
    base = CALLOC( nstate, base_n );
    same = NULL;
    r = NULL;
    diff = NULL;
    used = 0;
    avail = 0;
    table = NULL;
    shift = 0;
    parent_base = 0;
    for( i = nstate; i > 0; ) {
        --i;
        x = statetab[i];
        q = tokens;
        for( tx = x->trans; (sym = tx->sym) != NULL; ++tx ) {
            *q++ = sym->token;
            actions[sym->token] = tx->state->sidx;
        }
        savings = 0;
        for( rx = x->redun; (pro = rx->pro) != NULL; ++rx ) {
            redun = pro->pidx + nstate;
            mp = Members( rx->follow );
            if( (set_size)( mp - setmembers ) > savings ) {
                savings = (set_size)( mp - setmembers );
                r = q;
            }
            while( mp != setmembers ) {
                --mp;
                tokval = symtab[*mp]->token;
                *q++ = tokval;
                actions[tokval] = redun;
            }
        }
        if( savings ) {
            actval = actions[*r];
            other[i] = actval;
            *q++ = dtoken;
            actions[dtoken] = actval;
            p = r;
            while( savings-- > 0 )
                actions[*p++] = error;
            while( p < q )
                *r++ = *p++;
            q = r;
            ++num_default;
        } else {
            other[i] = error;
        }
        r = q;
        min = (set_size)( q - tokens );
        size[i] = min;
        parent[i] = nstate;
        for( j = nstate; --j > i; ) {
            if( abs( size[j] - size[i] ) < min ) {
                x = statetab[j];
                p = test;
                q = test + ntoken;
                for( tx = x->trans; (sym = tx->sym) != NULL; ++tx ) {
                    if( actions[sym->token] == tx->state->sidx ) {
                       *p++ = sym->token;
                    } else {
                       *--q = sym->token;
                    }
                }
                for( rx = x->redun; (pro = rx->pro) != NULL; ++rx ) {
                    redun = pro->pidx + nstate;
                    if( redun == other[j] )
                        redun = error;
                    for( mp = Members( rx->follow ); mp != setmembers; ) {
                        --mp;
                        tokval = symtab[*mp]->token;
                        if( actions[tokval] == redun ) {
                            *p++ = tokval;
                        } else {
                            *--q = tokval;
                        }
                    }
                }
                if( other[j] != error ) {
                    if( other[j] == other[i] ) {
                        *p++ = dtoken;
                    } else {
                        *--q = dtoken;
                    }
                }
                savings = (set_size)( size[i] + size[j] - 2 * ( p - test ) );
                if( savings < min ) {
                    min = savings;
                    same = p;
                    diff = q;
                    s = test; test = best; best = s;
                    parent[i] = j;
                }
            }
        }
        if( min >= size[i] ) {
            s = r;
        } else {
            ++num_parent;
            s = tokens;
            p = same;
            while( --p >= best )
                actions[*p] = error;
            for( q = tokens; q < r; ++q ) {
                if( actions[*q] != error ) {
                    *s++ = *q;
                }
            }
            p = best + ntoken;
            while( --p >= diff ) {
                if( actions[*p] == error ) {
                    *s++ = *p;
                }
            }
            actval = parent[i];
            *s++ = ptoken;
            actions[ptoken] = actval;
        }
        base[i] = addtotable( tokens, s, actions, dtoken, ptoken );
        while( --s >= tokens ) {
            actions[*s] = error;
        }
    }
    FREE( actions );
    FREE( tokens );
    FREE( test );
    FREE( best );
    FREE( other );
    FREE( size );

    putambigs( base );

    putnum( "YYNOACTION", error - nstate + used );
    putnum( "YYEOFTOKEN", eofsym->token );
    putnum( "YYERRTOKEN", errsym->token );
    putnum( "YYETOKEN", errsym->token );
    if( compactflag ) {
        parent_base = used + npro;
        putnum( "YYPARENT", parent_base );
        shift = 8;
        for( i = 256; i < used; i <<= 1 ) {
            ++shift;
        }
        putnum( "YYPRODSIZE", shift );
    } else {
        putnum( "YYPTOKEN", ptoken );
        putnum( "YYDTOKEN", dtoken );
    }
    putnum( "YYSTART", base[startstate->sidx] );
    putnum( "YYSTOP", base[eofsym->enter->sidx] );
    putnum( "YYERR", base[errstate->sidx] );
    putnum( "YYUSED", used );

    if( compactflag ) {
        begtab( "YYPACKTYPE", "yyacttab" );
        j = nstate;
        for( i = 0; i < used; ++i ) {
            new_action = table[i].action;
            if( i == base[j - 1] ) {
                --j;
                // First element in each state is default/parent
                if( parent[j] == nstate ) {
                    // No parent state
                    tokval = used + parent_base;
                } else {
                    tokval = base[parent[j]] + parent_base;
                }
                // 0 indicates no default
                if( new_action != 0 ) {
                    if( new_action < nstate ) {
                        // Shift
                        new_action = base[new_action];
                    } else {
                        // Reduce
                        new_action -= nstate;   // convert to 0 based
                        new_action += used;     // now convert to 'used' base
                    }
                }
            } else {
                tokval = table[i].token;
                if( new_action < nstate ) {
                    // Shift
                    new_action = base[new_action];
                } else {
                    // Reduce
                    new_action -= nstate;       // convert to 0 based
                    new_action += used;         // now convert to 'used' base
                }
            }
            putcompact( tokval, new_action );
        }
        endtab();
        // Combine lengths & lhs into a single table
        begtab( "YYPRODTYPE", "yyprodtab" );
        for( i = 0; i < npro; ++i ) {
            j = 0;
            for( item = protab[i]->item; item->p.sym != NULL; ++item ) {
                ++j;
            }
            puttab( FITS_A_WORD, (j << shift) + protab[i]->sym->token );
        }
        endtab();
    } else {
        begtab( "YYCHKTYPE", "yychktab" );
        for( i = 0; i < used; ++i ) {
            puttab( token_size, Token( table[i] ) );
        }
        endtab();
        begtab( "YYACTTYPE", "yyacttab" );
        for( i = 0; i < used; ++i ) {
            j = Action( table[i] );
            if( j < nstate ) {
                puttab( FITS_A_WORD, base[j] );
            } else {
                puttab( FITS_A_WORD, j - nstate + used );
            }
        }
        endtab();
        begtab( "YYPLENTYPE", "yyplentab" );
        for( i = 0; i < npro; ++i ) {
            for( item = protab[i]->item; item->p.sym != NULL; ) {
                ++item;
            }
            puttab( FITS_A_BYTE, (unsigned)( item - protab[i]->item ) );
        }
        endtab();
        begtab( "YYPLHSTYPE", "yyplhstab" );
        for( i = 0; i < npro; ++i ) {
            puttab( token_size, protab[i]->sym->token );
        }
        endtab();
    }
    FREE( table );
    FREE( base );
    FREE( parent );

    dumpstatistic( "bytes used in tables", bytesused );
    dumpstatistic( "states with defaults", num_default );
    dumpstatistic( "states with parents", num_parent );

    puttokennames( dtoken, token_size );
}
