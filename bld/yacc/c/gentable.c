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

#define INFINITY        32767

#define Token(c)        ((c).token)
#define SetToken(c,i)   ((c).token = (i))
#define IsUsed(c)       ((c).action & 0x4000)
#define Action(c)       ((c).action & 0x3fff)
#define SetAction(c,i)  ((c).action |= (0x4000|(i)))
#define IsBase(c)       ((c).action & 0x8000)
#define SetBase(c)      ((c).action |= 0x8000)
#define roundup(a,b)    ((((a)+(b)-1)/(b))*(b))

#define BLOCK           512

typedef struct a_table {
    short       token;
    short       action;
} a_table;

static int avail, used;
static a_table *table;

static int addtotable(  short *token,
                        short *end_token,
                        short *actions,
                        short default_token,
                        short parent_token )
{
    unsigned            start, max, i;
    short               default_action;
    short *             r;
    a_table *           t;

    if( compactflag ) {
        start = used++;         // Leave room for parent & default
        default_action = 0;
        for( ;; ++token ) {
            if( used >= avail ) {
                avail = roundup( used + 1, BLOCK );
                if( table ) {
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
                table[used].token = *token;
                table[used].action = actions[*token];
                ++used;
            }
        }
        table[start].token = actions[parent_token];
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
                if( table ) {
                    table = REALLOC( table, avail, a_table );
                } else {
                    table = MALLOC( avail, a_table );
                }
                while( i < avail ) {
                    table[i].token = TOKEN_IMPOSSIBLE;
                    table[i].action = 0;
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
    value_size token_size;
    short *tokens, *actions, *base, *other, *parent, *size;
    register short *p, *q, *r, *s;
    set_size *mp;
    short error, tokval, redun, *same, *diff, *test, *best;
    a_sym *sym;
    a_pro *pro;
    an_item *item;
    a_state *x;
    a_shift_action *tx;
    a_reduce_action *rx;
    int i, j, savings, min, ntoken, dtoken, ptoken;
    unsigned new_action, shift;
    unsigned parent_base;
    unsigned num_default, num_parent;

    if( fastflag ) {
        GenFastTables();
        return;
    }
    if( bigflag || compactflag ) {
        token_size = FITS_A_WORD;
    } else {
        token_size = FITS_A_BYTE;
    }
    num_default = num_parent = 0;
    ntoken = FirstNonTerminalTokenValue();
    dtoken = ntoken++;
    ptoken = ntoken++;
    for( i = nterm; i < nsym; ++i ) {
        symtab[i]->token = ntoken++;
    }

    error = nstate + npro;
    actions = CALLOC( ntoken, short );
    for( i = 0; i < ntoken; ++i ) {
        actions[i] = error;
    }
    tokens = CALLOC( ntoken, short );
    test = CALLOC( ntoken, short );
    best = CALLOC( ntoken, short );
    base = CALLOC( nstate, short );
    other = CALLOC( nstate, short );
    parent = CALLOC( nstate, short );
    size = CALLOC( nstate, short );
    same = NULL;
    r = NULL;
    diff = NULL;
    shift = 0;
    parent_base = 0;
    for( i = nstate; --i >= 0; ) {
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
            if( mp - setmembers > savings ) {
                savings = mp - setmembers;
                r = q;
            }
            while( --mp >= setmembers ) {
                tokval = symtab[*mp]->token;
                *q++ = tokval;
                actions[tokval] = redun;
            }
        }
        if( savings ) {
            tokval = actions[*r];
            other[i] = tokval;
            *q++ = dtoken;
            actions[dtoken] = tokval;
            p = r;
            while( --savings >= 0 )
                actions[*p++] = error;
            while( p < q )
                *r++ = *p++;
            q = r;
            ++num_default;
        } else {
            other[i] = error;
        }
        r = q;
        min = q - tokens;
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
                    for( mp = Members( rx->follow ); --mp >= setmembers; ) {
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
                savings = size[i] + size[j] - 2 * ( p - test );
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
            tokval = parent[i];
            *s++ = ptoken;
            actions[ptoken] = tokval;
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
                // First element in each state is default/parent
                -- j;
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
                        new_action += used;             // now convert to 'used' base
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
            puttab( FITS_A_BYTE, item - protab[i]->item );
        }
        endtab();
        begtab( "YYPLHSTYPE", "yyplhstab" );
        for( i = 0; i < npro; ++i ) {
            puttab( token_size, protab[i]->sym->token );
        }
        endtab();
    }
    dumpstatistic( "bytes used in tables", bytesused );
    dumpstatistic( "states with defaults", num_default );
    dumpstatistic( "states with parents", num_parent );
    puttokennames( dtoken, token_size );
    FREE( table );
    FREE( base );
    FREE( parent );
}
