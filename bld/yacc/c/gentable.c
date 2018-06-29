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

#define Token(c)        ((c)->token)
#define SetToken(c,i)   ((c)->token = (i))
#define IsUsed(c)       ((c)->action & ACTION_USED)
#define Action(c)       ((c)->action & ACTION_MASK)
#define SetAction(c,i)  ((c)->action |= (ACTION_USED|(i)))
#define IsBase(c)       ((c)->action & ACTION_BASE)
#define SetBase(c)      ((c)->action |= ACTION_BASE)

#define BLOCK           512

typedef struct a_table {
    token_n         token;
    action_n        action;
} a_table;

static base_n       avail;
static base_n       used;
static a_table      *table;

static void expand_table( base_n new_size )
{
    if( new_size > avail ) {
        avail = _RoundUp( new_size, BLOCK );
        if( table != NULL ) {
            table = REALLOC( table, avail, a_table );
        } else {
            table = MALLOC( avail, a_table );
        }
    }
}

static base_n addtotable( token_n *tokens, token_n *end_token, action_n *actions,
                            token_n default_token, token_n parent_token )
{
    base_n              start, i;
    token_n             max;
    action_n            default_action;
    token_n             *r;
    a_table             *t;
    a_table             *tstart;
    action_n            actval;
    token_n             tokval;

    if( compactflag ) {
        start = used++;
        expand_table( used );   // Leave room for parent & default
        default_action = ACTION_NULL;
        for( r = tokens; r < end_token; ++r ) {
            tokval = *r;
            if( tokval == default_token ) {
                default_action = actions[tokval];
            } else if( tokval != parent_token ) {
                actval = actions[tokval];
                if( tokval > 0x0FFF ) {
                    printf( "Error: token index 0x%X for item %d is higher then 0x0FFF !\n", tokval, start );
                }
                if( actval > 0x0FFF ) {
                    printf( "Error: token action 0x%X for item %d is higher then 0x0FFF !\n", actval, start );
                }
                expand_table( used + 1 );
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
        max = *tokens;
        for( r = tokens + 1; r < end_token; ++r ) {
            if( *r > max ) {
                max = *r;
            }
        }
        for( start = 0; ; ++start ) {
            i = avail;
            expand_table( start + max + 1 );
            while( i < avail ) {
                table[i].token = TOKEN_IMPOSSIBLE;
                table[i].action = ACTION_NULL;
                ++i;
            }
            tstart = table + start;
            if( !IsBase( tstart ) ) {
                for( r = tokens; r < end_token; ++r ) {
                    if( IsUsed( tstart + *r ) ) {
                        break;
                    }
                }
                if( r >= end_token ) {
                    break;
                }
            }
        }
        SetBase( tstart );
        for( r = tokens; r < end_token; ++r ) {
            tokval = *r;
            t = tstart + tokval;
            if( ! bigflag ) {
                if( tokval >= UCHAR_MAX ) {
                    msg( "too many tokens!\n" );
                }
            }
            SetToken( t, tokval );
            actval = actions[tokval];
            if( (actval & ACTION_MASK) != actval ) {
                printf( "Error: token action 0x%X for token %d is higher then 0x3FFF !\n", actval, tokval );
            }
            SetAction( t, actval );
        }
        i = start + max + 1;
        if( i > used ) {
            used = i;
        }
    }
    return( start );
}

void genobj( FILE *fp )
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
    set_size    max_savings;
    set_size    savings;
    set_size    min_len;
    set_size    len;
    set_size    *size;
    set_size    shift;
    token_n     parent_base;
    unsigned    num_default, num_parent;

    if( fastflag ) {
        GenFastTables( fp );
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
        max_savings = 0;
        for( rx = x->redun; (pro = rx->pro) != NULL; ++rx ) {
            if( (savings = (set_size)((mp = Members( rx->follow )) - setmembers)) == 0 )
                continue;
            redun = pro->pidx + nstate;
            if( max_savings < savings ) {
                max_savings = savings;
                r = q;
            }
            while( mp-- != setmembers ) {
                tokval = symtab[*mp]->token;
                *q++ = tokval;
                actions[tokval] = redun;
            }
        }
        if( max_savings ) {
            actval = actions[*r];
            other[i] = actval;
            *q++ = dtoken;
            actions[dtoken] = actval;
            p = r;
            while( max_savings-- > 0 )
                actions[*p++] = error;
            while( p < q )
                *r++ = *p++;
            q = r;
            ++num_default;
        } else {
            other[i] = error;
        }
        r = q;
        min_len = (set_size)( q - tokens );
        size[i] = min_len;
        parent[i] = nstate;
        for( j = nstate; --j > i; ) {
            if( abs( size[j] - size[i] ) < min_len ) {
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
                    for( mp = Members( rx->follow ); mp-- != setmembers; ) {
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
                len = (set_size)( size[i] + size[j] - 2 * ( p - test ) );
                if( min_len > len ) {
                    min_len = len;
                    same = p;
                    diff = q;
                    s = test; test = best; best = s;
                    parent[i] = j;
                }
            }
        }
        if( min_len >= size[i] ) {
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

    putambigs( fp, base );

    putnum( fp, "YYNOACTION", error - nstate + used );
    putnum( fp, "YYEOFTOKEN", eofsym->token );
    putnum( fp, "YYERRTOKEN", errsym->token );
    if( compactflag ) {
        parent_base = used + npro;
        putnum( fp, "YYPARENT", parent_base );
        shift = 8;
        for( i = 256; i < used; i <<= 1 ) {
            ++shift;
        }
        putnum( fp, "YYPRODSIZE", shift );
    } else {
        putnum( fp, "YYPARTOKEN", ptoken );
        putnum( fp, "YYDEFTOKEN", dtoken );
    }
    putnum( fp, "YYSTART", base[startstate->sidx] );
    putnum( fp, "YYSTOP", base[eofsym->enter->sidx] );
    putnum( fp, "YYERR", base[errstate->sidx] );
    putnum( fp, "YYUSED", used );

    if( compactflag ) {
        begtab( fp, "YYPACKTYPE", "yyacttab" );
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
            putcompact( fp, tokval, new_action );
        }
        endtab( fp );
        // Combine lengths & lhs into a single table
        begtab( fp, "YYPRODTYPE", "yyprodtab" );
        for( i = 0; i < npro; ++i ) {
            j = 0;
            for( item = protab[i]->items; item->p.sym != NULL; ++item ) {
                ++j;
            }
            puttab( fp, FITS_A_WORD, (j << shift) + protab[i]->sym->token );
        }
        endtab( fp );
    } else {
        begtab( fp, "YYCHKTYPE", "yychktab" );
        for( i = 0; i < used; ++i ) {
            puttab( fp, token_size, Token( table + i ) );
        }
        endtab( fp );
        begtab( fp, "YYACTTYPE", "yyacttab" );
        for( i = 0; i < used; ++i ) {
            j = Action( table + i );
            if( j < nstate ) {
                puttab( fp, FITS_A_WORD, base[j] );
            } else {
                puttab( fp, FITS_A_WORD, j - nstate + used );
            }
        }
        endtab( fp );
        begtab( fp, "YYPLENTYPE", "yyplentab" );
        for( i = 0; i < npro; ++i ) {
            for( item = protab[i]->items; item->p.sym != NULL; ) {
                ++item;
            }
            puttab( fp, FITS_A_BYTE, (unsigned)( item - protab[i]->items ) );
        }
        endtab( fp );
        begtab( fp, "YYPLHSTYPE", "yyplhstab" );
        for( i = 0; i < npro; ++i ) {
            puttab( fp, token_size, protab[i]->sym->token );
        }
        endtab( fp );
    }
    FREE( table );
    FREE( base );
    FREE( parent );

    dumpstatistic( "bytes used in tables", bytesused );
    dumpstatistic( "states with defaults", num_default );
    dumpstatistic( "states with parents", num_parent );

    puttokennames( fp, dtoken, token_size );

    FREE( protab );
    FREE( symtab );
}
