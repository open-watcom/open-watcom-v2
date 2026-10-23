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
* Description:  Generate deterministic run-time parser tables from the
*               LALR automaton. Logical state rows are canonicalized,
*               encoded relative to previously emitted parent rows, packed
*               using the original table representation, and written in
*               compact or standard form.
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#define SetAction(c,i)  ((c)->action |= (ACTION_USED | (i)))
#define IsBase(c)       ((c)->action & ACTION_BASE)
#define SetBase(c)      ((c)->action |= ACTION_BASE)

#define BLOCK           512

typedef struct a_table {
    token_n         token;
    action_n        action;
} a_table;

typedef struct row_entry {
    token_n         token;
    action_n        action;
} row_entry;

typedef struct state_row {
    unsigned        count;
    row_entry       *entry;
    action_n        default_action;
} state_row;

typedef struct table_builder {
    base_n          avail;
    base_n          used;
    a_table         *table;
} table_builder;


static int tokenCompare( token_n left, token_n right )
{
    return( left < right ? -1 : left > right ? 1 : 0 );
}

/*
 * Ordinary nonterminals precede internal symbols. Within each group the
 * symbol name defines the order, independently of %type declaration order.
 */
static bool nonterminalBefore( const a_sym *left, const a_sym *right )
{
    bool        left_internal;
    bool        right_internal;

    left_internal = left->name[0] == '$';
    right_internal = right->name[0] == '$';
    if( left_internal != right_internal ) {
        return( !left_internal );
    }
    return( strcmp( left->name, right->name ) < 0 );
}

static token_n assignNonterminalTokens( token_n token )
{
    a_sym       **order;
    a_sym       *current;
    sym_n       count;
    sym_n       i;
    sym_n       j;

    count = nsym - nterm;
    if( count == 0 ) {
        return( token );
    }
    order = MemAllocSafe( count * sizeof( *order ) );
    for( i = 0; i < count; ++i ) {
        order[i] = symtab[nterm + i];
    }
    for( i = 1; i < count; ++i ) {
        current = order[i];
        j = i;
        while( j > 0 && nonterminalBefore( current, order[j - 1] ) ) {
            order[j] = order[j - 1];
            --j;
        }
        order[j] = current;
    }
    for( i = 0; i < count; ++i ) {
        order[i]->token = token++;
    }
    MemFree( order );
    return( token );
}

/* Canonical production order: LHS, RHS, shorter RHS first, then pidx. */
static int productionCompare( const a_pro *left, const a_pro *right )
{
    const an_item   *li;
    const an_item   *ri;
    int             result;

    result = tokenCompare( left->sym->token, right->sym->token );
    if( result != 0 ) {
        return( result );
    }
    li = left->items;
    ri = right->items;
    for( ;; ) {
        if( li->u.sym == NULL || ri->u.sym == NULL ) {
            if( li->u.sym != NULL ) {
                return( 1 );
            }
            if( ri->u.sym != NULL ) {
                return( -1 );
            }
            break;
        }
        result = tokenCompare( li->u.sym->token, ri->u.sym->token );
        if( result != 0 ) {
            return( result );
        }
        ++li;
        ++ri;
    }
    return( left->pidx < right->pidx ? -1 : left->pidx > right->pidx ? 1 : 0 );
}

static unsigned *calculateRuleRanks( void )
{
    rule_n          *order;
    unsigned        *rank;
    rule_n          current;
    rule_n          i;
    rule_n          j;

    order = MemAllocSafe( npro * sizeof( *order ) );
    rank = MemAllocSafe( npro * sizeof( *rank ) );
    for( i = 0; i < npro; ++i ) {
        order[i] = i;
    }
    for( i = 1; i < npro; ++i ) {
        current = order[i];
        j = i;
        while( j > 0 && productionCompare( protab[current], protab[order[j - 1]] ) < 0 ) {
            order[j] = order[j - 1];
            --j;
        }
        order[j] = current;
    }
    for( i = 0; i < npro; ++i ) {
        rank[order[i]] = i;
    }
    MemFree( order );
    return( rank );
}

/*
 * Canonical state order: breadth-first from the start state, visiting
 * outgoing transitions in symbol-token order.
 */
static void calculateStateOrder( action_n *order )
{
    a_shift_action  **transitions;
    a_shift_action  *shift;
    a_shift_action  *current;
    bool            *seen;
    unsigned        head;
    unsigned        count;
    unsigned        n;
    unsigned        i;
    unsigned        j;
    action_n        sidx;

    seen = MemCAllocSafe( nstate, sizeof( *seen ) );
    transitions = MemAllocSafe( ( nsym + 1 ) * sizeof( *transitions ) );
    count = 0;
    sidx = startstate->sidx;
    order[count++] = sidx;
    seen[sidx] = true;
    for( head = 0; head < count; ++head ) {
        n = 0;
        for( shift = statetab[order[head]]->trans; shift->sym != NULL; ++shift ) {
            current = shift;
            j = n++;
            while( j > 0 && transitions[j - 1]->sym->token > current->sym->token ) {
                transitions[j] = transitions[j - 1];
                --j;
            }
            transitions[j] = current;
        }
        for( i = 0; i < n; ++i ) {
            sidx = transitions[i]->state->sidx;
            if( !seen[sidx] ) {
                seen[sidx] = true;
                order[count++] = sidx;
            }
        }
    }
    /* LALR states should all be reachable; keep a deterministic fallback. */
    for( sidx = 0; sidx < nstate; ++sidx ) {
        if( !seen[sidx] ) {
            order[count++] = sidx;
        }
    }
    MemFree( transitions );
    MemFree( seen );
}

/*
 * Build final logical state rows. Later assignments win, preserving the
 * conflict-resolution semantics of the original generator.
 *
 * The default reduction is selected from the final action row rather than
 * directly from the original follow sets, because later actions may replace
 * earlier ones. Equal counts are resolved by canonical production rank.
 */
static state_row *buildStateRows( const unsigned *rule_rank, action_n error,
                                  token_n token_count, unsigned *default_count )
{
    state_row       *rows;
    state_row       *row;
    action_n        *actions;
    token_n         *touched;
    unsigned        touched_count;
    unsigned        best_count;
    unsigned        best_rank;
    unsigned        count;
    unsigned        i;
    unsigned        j;
    unsigned        position;
    action_n        best;
    action_n        reduction;
    action_n        sidx;
    token_n         token;
    a_state         *state;
    a_shift_action  *shift;
    a_reduce_action *reduce;
    bitnum          *member;

    rows = MemCAllocSafe( nstate, sizeof( *rows ) );
    actions = MemAllocSafe( token_count * sizeof( *actions ) );
    touched = MemAllocSafe( token_count * sizeof( *touched ) );
    for( token = 0; token < token_count; ++token ) {
        actions[token] = error;
    }
    *default_count = 0;
    for( sidx = 0; sidx < nstate; ++sidx ) {
        state = statetab[sidx];
        touched_count = 0;
        for( shift = state->trans; shift->sym != NULL; ++shift ) {
            token = shift->sym->token;
            if( actions[token] == error ) {
                touched[touched_count++] = token;
            }
            actions[token] = shift->state->sidx;
        }
        for( reduce = state->redun; reduce->pro != NULL; ++reduce ) {
            reduction = reduce->pro->pidx + nstate;
            for( member = Members( reduce->follow ); member-- != setmembers; ) {
                token = symtab[*member]->token;
                if( actions[token] == error ) {
                    touched[touched_count++] = token;
                }
                actions[token] = reduction;
            }
        }

        best = error;
        best_count = 0;
        best_rank = 0;
        for( reduce = state->redun; reduce->pro != NULL; ++reduce ) {
            reduction = reduce->pro->pidx + nstate;
            count = 0;
            for( i = 0; i < touched_count; ++i ) {
                if( actions[touched[i]] == reduction ) {
                    ++count;
                }
            }
            if( count != 0 && ( count > best_count
              || ( count == best_count && rule_rank[reduce->pro->pidx] < best_rank ) ) ) {
                best = reduction;
                best_count = count;
                best_rank = rule_rank[reduce->pro->pidx];
            }
        }

        row = &rows[sidx];
        row->default_action = best;
        if( best != error ) {
            ++*default_count;
        }
        row->count = 0;
        for( i = 0; i < touched_count; ++i ) {
            if( actions[touched[i]] != best ) {
                ++row->count;
            }
        }
        row->entry = MemAllocSafe( ( row->count + 1 ) * sizeof( *row->entry ) );
        j = 0;
        for( i = 0; i < touched_count; ++i ) {
            token = touched[i];
            if( actions[token] != best ) {
                position = j++;
                while( position > 0 && row->entry[position - 1].token > token ) {
                    row->entry[position] = row->entry[position - 1];
                    --position;
                }
                row->entry[position].token = token;
                row->entry[position].action = actions[token];
            }
        }
        for( i = 0; i < touched_count; ++i ) {
            actions[touched[i]] = error;
        }
    }
    MemFree( touched );
    MemFree( actions );
    return( rows );
}

static void freeStateRows( state_row *rows )
{
    action_n        sidx;

    for( sidx = 0; sidx < nstate; ++sidx ) {
        MemFree( rows[sidx].entry );
    }
    MemFree( rows );
}

static unsigned rowDifference( const state_row *row, const state_row *parent,
                               bool include_default )
{
    unsigned        i;
    unsigned        j;
    unsigned        difference;

    i = 0;
    j = 0;
    difference = 0;
    while( i < row->count && j < parent->count ) {
        if( row->entry[i].token < parent->entry[j].token ) {
            ++difference;
            ++i;
        } else if( row->entry[i].token > parent->entry[j].token ) {
            ++difference;
            ++j;
        } else {
            if( row->entry[i].action != parent->entry[j].action ) {
                ++difference;
            }
            ++i;
            ++j;
        }
    }
    difference += row->count - i;
    difference += parent->count - j;
    if( include_default && row->default_action != parent->default_action ) {
        ++difference;
    }
    return( difference );
}

/*
 * Select the cheapest previously emitted canonical row as the parent.
 *
 * Restricting parents to earlier canonical rows guarantees an acyclic parent
 * relation and makes the result independent of LALR state numbers. This is
 * deliberately a greedy optimization: it keeps the algorithm simple while
 * producing substantially smaller tables than the original order-dependent
 * implementation.
 */
static void selectParents( const state_row *rows, const action_n *order,
                           action_n error, bool compact, action_n *parent,
                           unsigned *parent_count )
{
    unsigned        rank;
    unsigned        candidate_rank;
    unsigned        best_cost;
    unsigned        cost;
    action_n        state;
    action_n        candidate;

    *parent_count = 0;
    for( rank = 0; rank < nstate; ++rank ) {
        state = order[rank];
        best_cost = rows[state].count;
        if( !compact && rows[state].default_action != error ) {
            ++best_cost;
        }
        parent[state] = nstate;
        for( candidate_rank = 0; candidate_rank < rank; ++candidate_rank ) {
            candidate = order[candidate_rank];
            cost = rowDifference( &rows[state], &rows[candidate], !compact );
            if( !compact ) {
                ++cost;       /* explicit parent-token entry */
            }
            if( cost < best_cost ) {
                best_cost = cost;
                parent[state] = candidate;
            }
        }
        if( parent[state] != nstate ) {
            ++*parent_count;
        }
    }
}

/*
 * Build the entries which differ from the selected parent row. Missing
 * entries represent the error action. Parent and default pseudo-tokens are
 * added only for the non-compact run-time table format.
 */
static unsigned buildPhysicalRow( row_entry *output, const state_row *row,
                                  const state_row *parent_row,
                                  action_n parent_state, action_n error,
                                  token_n default_token, token_n parent_token,
                                  bool compact )
{
    unsigned        i;
    unsigned        j;
    unsigned        count;
    token_n         token;
    action_n        action;
    action_n        inherited;
    action_n        parent_default;

    i = 0;
    j = 0;
    count = 0;
    while( i < row->count || ( parent_row != NULL && j < parent_row->count ) ) {
        if( parent_row == NULL || j >= parent_row->count
          || ( i < row->count && row->entry[i].token < parent_row->entry[j].token ) ) {
            token = row->entry[i].token;
            action = row->entry[i].action;
            inherited = error;
            ++i;
        } else if( i >= row->count || row->entry[i].token > parent_row->entry[j].token ) {
            token = parent_row->entry[j].token;
            action = error;
            inherited = parent_row->entry[j].action;
            ++j;
        } else {
            token = row->entry[i].token;
            action = row->entry[i].action;
            inherited = parent_row->entry[j].action;
            ++i;
            ++j;
        }
        if( action != inherited ) {
            output[count].token = token;
            output[count].action = action;
            ++count;
        }
    }
    if( !compact ) {
        if( parent_row != NULL ) {
            output[count].token = parent_token;
            output[count].action = parent_state;
            ++count;
        }
        parent_default = parent_row != NULL ? parent_row->default_action : error;
        if( row->default_action != parent_default ) {
            output[count].token = default_token;
            output[count].action = row->default_action;
            ++count;
        }
    }
    return( count );
}

static void expandTable( table_builder *builder, base_n new_size )
{
    if( builder->avail < new_size ) {
        builder->avail = __ROUND_UP_SIZE( new_size, BLOCK );
        builder->table = MemReallocSafe( builder->table,
                                         builder->avail * sizeof( *builder->table ) );
    }
}

static void initializeNewCells( table_builder *builder, base_n old_avail )
{
    while( old_avail < builder->avail ) {
        builder->table[old_avail].token = TOKEN_IMPOSSIBLE;
        builder->table[old_avail].action = ACTION_NONE;
        ++old_avail;
    }
}

static base_n addToTable( table_builder *builder, const row_entry *entry,
                          unsigned count )
{
    base_n          start;
    base_n          old_avail;
    base_n          end;
    token_n         max_token;
    unsigned        i;
    a_table         *cell;
    a_table         *base;

    /*
     * Empty rows deliberately still reserve a base cell. This preserves the
     * run-time table contract and keeps every state base valid.
     */
    max_token = 0;
    for( i = 0; i < count; ++i ) {
        if( max_token < entry[i].token ) {
            max_token = entry[i].token;
        }
    }
    for( start = 0; ; ++start ) {
        old_avail = builder->avail;
        expandTable( builder, start + max_token + 1 );
        initializeNewCells( builder, old_avail );
        base = builder->table + start;
        if( !IsBase( base ) ) {
            for( i = 0; i < count; ++i ) {
                if( IsUsed( base + entry[i].token ) ) {
                    break;
                }
            }
            if( i == count ) {
                break;
            }
        }
    }
    SetBase( base );
    for( i = 0; i < count; ++i ) {
        cell = base + entry[i].token;
        if( !bigflag && entry[i].token >= UCHAR_MAX ) {
            msg( "too many tokens!\n" );
        }
        if( ( entry[i].action & ACTION_MASK ) != entry[i].action ) {
            printf( "Error: token action 0x%X for token %d is higher than 0x3FFF!\n",
                    entry[i].action, entry[i].token );
        }
        SetToken( cell, entry[i].token );
        SetAction( cell, entry[i].action );
    }
    end = start + max_token + 1;
    if( builder->used < end ) {
        builder->used = end;
    }
    return( start );
}

static base_n addCompactRow( table_builder *builder, const row_entry *entry,
                             unsigned count, action_n default_action )
{
    base_n          start;
    unsigned        i;

    start = builder->used;
    expandTable( builder, builder->used + count + 1 );
    builder->table[builder->used].token = 0;
    builder->table[builder->used].action = default_action;
    ++builder->used;
    for( i = 0; i < count; ++i ) {
        if( entry[i].token > 0x0FFF ) {
            printf( "Error: token index 0x%X for item %u is higher than 0x0FFF!\n",
                    entry[i].token, (unsigned)start );
        }
        if( entry[i].action > 0x0FFF ) {
            printf( "Error: token action 0x%X for item %u is higher than 0x0FFF!\n",
                    entry[i].action, (unsigned)start );
        }
        builder->table[builder->used].token = entry[i].token;
        builder->table[builder->used].action = entry[i].action;
        ++builder->used;
    }
    return( start );
}

/*
 * Convert an internal action to the representation used by the run-time
 * parser tables: shifts are encoded as state bases and reductions relative
 * to the end of the action table.
 */
static unsigned encodeAction( action_n action, const base_n *base, base_n used )
{
    if( action < nstate ) {
        return( base[action] );
    }
    return( action - nstate + used );
}

void genobj( FILE *fp )
{
    table_builder   builder;
    state_row       *rows;
    row_entry       *physical;
    unsigned        *rule_rank;
    action_n        *state_order;
    action_n        *parent;
    base_n          *base;
    value_size      token_size;
    token_n         ntoken;
    token_n         dtoken;
    token_n         ptoken;
    token_n         parent_base;
    action_n        error;
    action_n        state;
    action_n        parent_state;
    action_n        default_action;
    const state_row *parent_row;
    unsigned        rank;
    unsigned        count;
    unsigned        num_default;
    unsigned        num_parent;
    unsigned        shift;
    unsigned        i;
    unsigned        encoded;
    base_n          start;
    base_n          end;
    rule_n          pidx;
    an_item         *item;

    memset( &builder, 0, sizeof( builder ) );
    token_size = ( bigflag || compactflag ) ? FITS_A_WORD : FITS_A_BYTE;

    ntoken = FirstNonTerminalTokenValue();
    dtoken = ntoken++;
    ptoken = ntoken++;
    ntoken = assignNonterminalTokens( ntoken );
    error = nstate + npro;

    rule_rank = calculateRuleRanks();
    rows = buildStateRows( rule_rank, error, ntoken, &num_default );

    state_order = MemAllocSafe( nstate * sizeof( *state_order ) );
    calculateStateOrder( state_order );
    parent = MemAllocSafe( nstate * sizeof( *parent ) );
    selectParents( rows, state_order, error, compactflag, parent, &num_parent );

    base = MemAllocSafe( nstate * sizeof( *base ) );
    /*
     * A difference row can contain all entries from both the state and
     * its parent, plus the parent and default pseudo-tokens.
     */
    physical = MemAllocSafe( ( 2 * ntoken + 2 ) * sizeof( *physical ) );

    for( rank = 0; rank < nstate; ++rank ) {
        state = state_order[rank];
        parent_state = parent[state];
        parent_row = parent_state == nstate ? NULL : &rows[parent_state];
        count = buildPhysicalRow( physical, &rows[state], parent_row, parent_state,
                                  error, dtoken, ptoken, compactflag );
        if( compactflag ) {
            if( parent_row == NULL ) {
                default_action = error;
            } else {
                default_action = parent_row->default_action;
            }
            if( rows[state].default_action == default_action ) {
                default_action = ACTION_NONE;
            } else {
                default_action = rows[state].default_action;
            }
            base[state] = addCompactRow( &builder, physical, count, default_action );
        } else {
            base[state] = addToTable( &builder, physical, count );
        }
    }

    putambigs( fp, base );

    putnum( fp, "YYNOACTION", error - nstate + builder.used );
    putnum( fp, "YYEOFTOKEN", eofsym->token );
    putnum( fp, "YYERRTOKEN", errsym->token );
    parent_base = 0;
    shift = 0;
    if( compactflag ) {
        parent_base = builder.used + npro;
        if( parent_base < ntoken ) {
            parent_base = ntoken;
        }
        putnum( fp, "YYPARENT", parent_base );
        shift = 8;
        for( i = 256; i < builder.used; i <<= 1 ) {
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
    putnum( fp, "YYUSED", builder.used );

    if( compactflag ) {
        begtab( fp, "YYPACKTYPE", "yyacttab" );
        for( rank = 0; rank < nstate; ++rank ) {
            state = state_order[rank];
            start = base[state];
            end = rank + 1 < nstate ? base[state_order[rank + 1]] : builder.used;
            if( parent[state] == nstate ) {
                encoded = builder.used + parent_base;
            } else {
                encoded = base[parent[state]] + parent_base;
            }
            default_action = builder.table[start].action;
            if( default_action != ACTION_NONE ) {
                default_action = encodeAction( default_action, base, builder.used );
            }
            putcompact( fp, encoded, default_action );
            for( i = start + 1; i < end; ++i ) {
                encoded = encodeAction( builder.table[i].action, base, builder.used );
                putcompact( fp, builder.table[i].token, encoded );
            }
        }
        endtab( fp );

        begtab( fp, "YYPRODTYPE", "yyprodtab" );
        for( pidx = 0; pidx < npro; ++pidx ) {
            i = 0;
            for( item = protab[pidx]->items; item->u.sym != NULL; ++item ) {
                ++i;
            }
            puttab( fp, FITS_A_WORD, ( i << shift ) + protab[pidx]->sym->token );
        }
        endtab( fp );
    } else {
        begtab( fp, "YYCHKTYPE", "yychktab" );
        for( i = 0; i < builder.used; ++i ) {
            puttab( fp, token_size, Token( builder.table + i ) );
        }
        endtab( fp );

        begtab( fp, "YYACTTYPE", "yyacttab" );
        for( i = 0; i < builder.used; ++i ) {
            state = Action( builder.table + i );
            puttab( fp, FITS_A_WORD, encodeAction( state, base, builder.used ) );
        }
        endtab( fp );

        begtab( fp, "YYPLENTYPE", "yyplentab" );
        for( pidx = 0; pidx < npro; ++pidx ) {
            for( item = protab[pidx]->items; item->u.sym != NULL; ) {
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

    dumpstatistic( "bytes used in tables", bytesused );
    dumpstatistic( "states with defaults", num_default );
    dumpstatistic( "states with parents", num_parent );
    puttokennames( fp, dtoken, token_size );

    freeStateRows( rows );
    MemFree( physical );
    MemFree( builder.table );
    MemFree( base );
    MemFree( parent );
    MemFree( state_order );
    MemFree( rule_rank );
    MemFree( protab );
    MemFree( symtab );
}
