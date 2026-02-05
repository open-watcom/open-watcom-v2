/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023-2026 The Open Watcom Contributors. All Rights Reserved.
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
#include "roundmac.h"


#define ACTION_USED     ACTION_FLAG_1
#define ACTION_BASE     ACTION_FLAG_2

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
    if( avail < new_size ) {
        avail = __ROUND_UP_SIZE( new_size, BLOCK );
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
    base_n          start;
    base_n          i;
    unsigned        j;
    token_n         max;
    action_n        default_action;
    token_n         *r;
    a_table         *t;
    a_table         *tstart;
    action_n        actval;
    token_n         tokval;

    if( compactflag ) {
        start = used++;
        expand_table( used );   // Leave room for parent & default
        default_action = ACTION_NONE;
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
            if( max < *r ) {
                max = *r;
            }
        }
        for( start = 0; ; ++start ) {
            i = avail;
            expand_table( start + max + 1 );
            while( i < avail ) {
                table[i].token = TOKEN_IMPOSSIBLE;
                table[i].action = ACTION_NONE;
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
        j = start + max + 1;
        if( used < j ) {
            used = j;
        }
    }
    return( start );
}

void genobj( FILE *fp )
{
    value_size      token_size;
    action_n        *actions;
    action_n        *parent;
    action_n        *other;
    base_n          *base;
    token_n         *p;
    token_n         *q;
    token_n         *r;
    token_n         *s;
    token_n         *tokens;
    token_n         *same;
    token_n         *diff;
    token_n         *test;
    token_n         *best;
    bitnum          *mp;
    token_n         tokval;
    token_n         dtoken;
    token_n         ptoken;
    token_n         ntoken;
    action_n        actval;
    action_n        error;
    action_n        redun;
    action_n        new_action;
    a_sym           *sym;
    a_pro           *pro;
    an_item         *item;
    a_state         *state;
    a_shift_action  *saction;
    a_reduce_action *raction;
    unsigned        i;
    token_n         j;
    action_n        sidx;
    action_n        sidx2;
    rule_n          pidx;
    sym_n           sym_idx;
    unsigned        max_savings;
    unsigned        savings;
    unsigned        min_len;
    unsigned        len;
    unsigned        *size;
    unsigned        shift;
    token_n         parent_base;
    unsigned        num_default;
    unsigned        num_parent;

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
    for( sym_idx = nterm; sym_idx < nsym; ++sym_idx ) {
        symtab[sym_idx]->token = ntoken++;
    }
    actions = CALLOC( ntoken, action_n );
    error = nstate + npro;
    for( j = 0; j < ntoken; ++j ) {
        actions[j] = error;
    }
    tokens = CALLOC( ntoken, token_n );
    test = CALLOC( ntoken, token_n );
    best = CALLOC( ntoken, token_n );
    other = CALLOC( nstate, action_n );
    parent = CALLOC( nstate, action_n );
    size = CALLOC( nstate, unsigned );
    base = CALLOC( nstate, base_n );
    same = NULL;
    r = NULL;
    diff = NULL;
    used = 0;
    avail = 0;
    table = NULL;
    shift = 0;
    parent_base = 0;
    for( sidx = nstate; sidx-- > 0; ) {
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
            while( mp-- != setmembers ) {
                sym_idx = *mp;
                tokval = symtab[sym_idx]->token;
                *q++ = tokval;
                actions[tokval] = redun;
            }
        }
        if( max_savings ) {
            actval = actions[*r];
            other[sidx] = actval;
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
            other[sidx] = error;
        }
        r = q;
        min_len = (unsigned)( q - tokens );
        size[sidx] = min_len;
        parent[sidx] = nstate;
        for( sidx2 = nstate; --sidx2 > sidx; ) {
            if( abs( size[sidx2] - size[sidx] ) < min_len ) {
                state = statetab[sidx2];
                p = test;
                q = test + ntoken;
                for( saction = state->trans; (sym = saction->sym) != NULL; ++saction ) {
                    tokval = sym->token;
                    if( actions[tokval] == saction->state->sidx ) {
                       *p++ = tokval;
                    } else {
                       *--q = tokval;
                    }
                }
                for( raction = state->redun; (pro = raction->pro) != NULL; ++raction ) {
                    redun = pro->pidx + nstate;
                    if( redun == other[sidx2] )
                        redun = error;
                    for( mp = Members( raction->follow ); mp-- != setmembers; ) {
                        sym_idx = *mp;
                        tokval = symtab[sym_idx]->token;
                        if( actions[tokval] == redun ) {
                            *p++ = tokval;
                        } else {
                            *--q = tokval;
                        }
                    }
                }
                if( other[sidx2] != error ) {
                    if( other[sidx2] == other[sidx] ) {
                        *p++ = dtoken;
                    } else {
                        *--q = dtoken;
                    }
                }
                len = (unsigned)( size[sidx] + size[sidx2] - 2 * ( p - test ) );
                if( min_len > len ) {
                    min_len = len;
                    same = p;
                    diff = q;
                    s = test; test = best; best = s;
                    parent[sidx] = sidx2;
                }
            }
        }
        if( min_len >= size[sidx] ) {
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
            actval = parent[sidx];
            *s++ = ptoken;
            actions[ptoken] = actval;
        }
        base[sidx] = addtotable( tokens, s, actions, dtoken, ptoken );
        while( s-- > tokens ) {
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
    putnum( fp, "YYSTOP", base[eofsym->state->sidx] );
    putnum( fp, "YYERR", base[errstate->sidx] );
    putnum( fp, "YYUSED", used );

    if( compactflag ) {
        begtab( fp, "YYPACKTYPE", "yyacttab" );
        sidx = nstate;
        for( i = 0; i < used; ++i ) {
            new_action = table[i].action;
            if( i == base[sidx - 1] ) {
                --sidx;
                /*
                 * First element in each state is default/parent
                 */
                if( parent[sidx] == nstate ) {
                    /*
                     * No parent state
                     */
                    tokval = used + parent_base;
                } else {
                    tokval = base[parent[sidx]] + parent_base;
                }
                /*
                 * 0 indicates no default
                 */
                if( new_action != 0 ) {
                    if( new_action < nstate ) {
                        /*
                         * Shift
                         */
                        new_action = base[new_action];
                    } else {
                        /*
                         * Reduce
                         */
                        new_action -= nstate;   // convert to 0 based
                        new_action += used;     // now convert to 'used' base
                    }
                }
            } else {
                tokval = table[i].token;
                if( new_action < nstate ) {
                    /*
                     * Shift
                     */
                    new_action = base[new_action];
                } else {
                    /*
                     * Reduce
                     */
                    new_action -= nstate;       // convert to 0 based
                    new_action += used;         // now convert to 'used' base
                }
            }
            putcompact( fp, tokval, new_action );
        }
        endtab( fp );
        /*
         * Combine lengths & lhs into a single table
         */
        begtab( fp, "YYPRODTYPE", "yyprodtab" );
        for( pidx = 0; pidx < npro; ++pidx ) {
            i = 0;
            for( item = protab[pidx]->items; item->p.sym != NULL; ++item ) {
                ++i;
            }
            puttab( fp, FITS_A_WORD, (i << shift) + protab[pidx]->sym->token );
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
            sidx = Action( table + i );
            if( sidx < nstate ) {
                puttab( fp, FITS_A_WORD, base[sidx] );
            } else {
                puttab( fp, FITS_A_WORD, sidx - nstate + used );
            }
        }
        endtab( fp );
        begtab( fp, "YYPLENTYPE", "yyplentab" );
        for( pidx = 0; pidx < npro; ++pidx ) {
            for( item = protab[pidx]->items; item->p.sym != NULL; ) {
                ++item;
            }
            puttab( fp, FITS_A_BYTE, (unsigned)( item - protab[pidx]->items ) );
        }
        endtab( fp );
        begtab( fp, "YYPLHSTYPE", "yyplhstab" );
        for( pidx = 0; pidx < npro; ++pidx ) {
            puttab( fp, token_size, protab[pidx]->sym->token );
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
