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
#include <ctype.h>
#include "yacc.h"
#include "yaccins.h"
#include "alloc.h"


#define ACTION_REDUCE       ACTION_NONE
#define ACTION_SHIFT        ACTION_FLAG_1
#define ACTION_AMBIGUOUS    ACTION_FLAG_2

typedef struct {
    short       token;
    short       action;
} a_entry;

static a_entry      *table;
static unsigned     used;
static unsigned     table_size;
static int          tabcol;

static void dump_define( FILE *fp, const char *name, int i )
{
    fprintf( fp, "#define\t%-20s\t%d\n", name, i );
}

static void begin_table( FILE *fp, const char *tipe, const char *name )
{
    fprintf( fp, "static const %s YYFAR %s[] = {", tipe, name );
    tabcol = 0;
}

void puttab( FILE *fp, value_size fits, unsigned i )
{
    const char      *format;
    unsigned        mod;

    if( fits == FITS_A_BYTE ) {
        if(( i & 0x00ff ) != i ) {
            msg( "value cannot fit into table! (%x)", i );
        }
        format = "%3u";
        mod = 20;
    } else {
        format = "%5u";
        mod = 13;
    }
    if( tabcol ) {
        fprintf( fp, "," );
    }
    if( !(tabcol % mod) ) {
        fprintf( fp, "\n" );
    }
    fprintf( fp, format, i );
    ++tabcol;
}

static void end_table( FILE *fp )
{
    fprintf( fp, "\n};\n" );
}

static void add_table( short token, short action )
{
    if( used == table_size ) {
        table_size += 64;
        table = REALLOC( table, table_size, a_entry );
    }
    table[used].token = token;
    table[used].action = action;
    ++used;
}


static unsigned dump_reduction( a_reduce_action *raction )
{
    a_pro           *pro;
    bitnum          *mp;
    short           idx;
    unsigned        size;

    size = 0;
    pro = raction->pro;
    for( mp = Members( raction->follow ); mp-- != setmembers; ) {
        idx = *mp;
        add_table( idx, pro->pidx | ACTION_REDUCE );
        ++size;
    }
    return( size );
}

void genobj( FILE *fp )
{
    unsigned        i;
    action_n        sidx;
    rule_n          pidx;
    sym_n           sym_idx;
    int             ntoken;
    int             any_token;
    int             action;
    a_pro           *pro;
    a_state         *state;
    a_reduce_action *raction;
    a_reduce_action *default_reduction;
    a_shift_action  *saction;
    a_sym           *sym;
    an_item         *item;
    unsigned        max;
    unsigned        sum;
    unsigned        savings;
    unsigned        max_savings;
    unsigned        base;
    unsigned        rule_base;
    short           *state_base;
    unsigned        size;

    ntoken = FirstNonTerminalTokenValue();
    for( sym_idx = nterm; sym_idx < nsym; ++sym_idx ) {
        symtab[sym_idx]->token = ntoken++;
    }
    any_token = ntoken;
    state_base = CALLOC( nstate, short );
    base = 0;
    max = 0;
    sum = 0;
    used = 0;
    table_size = 0;
    table = NULL;
    for( sidx = 0; sidx < nstate; ++sidx ) {
        state_base[sidx] = base;
        state = statetab[sidx];
        for( saction = state->trans; (sym = saction->sym) != NULL; ++saction ) {
            add_table( sym->idx, saction->state->sidx | ACTION_SHIFT );
            ++base;
        }
        default_reduction = NULL;
        max_savings = 0;
        for( raction = state->redun; raction->pro != NULL; ++raction ) {
            savings = Members( raction->follow ) - setmembers;
            if( savings == 0 )
                continue;
            if( max_savings < savings ) {
                max_savings = savings;
                if( default_reduction != NULL ) {
                    base += dump_reduction( default_reduction );
                }
                default_reduction = raction;
            } else {
                base += dump_reduction( raction );
            }
        }
        if( default_reduction != NULL ) {
            pro = default_reduction->pro;
            action = pro->pidx | ACTION_REDUCE;
        } else {
            action = ACTION_SHIFT;
        }
        add_table( any_token, action );
        ++base;
        size = base - state_base[sidx];
        sum += size;
        if( max < size ) {
            max = size;
        }
    }
    printf( "avg: %u max: %u\n", sum / nstate, max );
    dump_define( fp, "YYANYTOKEN", any_token );
    dump_define( fp, "YYEOFTOKEN", eofsym->token );
    dump_define( fp, "YYSTART", startstate->sidx );
    begin_table( fp, "YYACTTYPE", "yybasetab" );
    for( sidx = 0; sidx < nstate; ++sidx ) {
        puttab( fp, FITS_A_WORD, state_base[sidx] );
    }
    end_table( fp );
    begin_table( fp, "YYCHKTYPE", "yychktab" );
    for( i = 0; i < used; ++i ) {
        puttab( fp, FITS_A_BYTE, table[i].token );
    }
    end_table( fp );
    begin_table( fp, "YYACTTYPE", "yyacttab" );
    for( i = 0; i < used; ++i ) {
        puttab( fp, FITS_A_WORD, table[i].action );
    }
    end_table( fp );
    begin_table( fp, "YYPLENTYPE", "yyplentab" );
    for( pidx = 0; pidx < npro; ++pidx ) {
        for( item = protab[pidx]->items; item->p.sym != NULL; ) {
            ++item;
        }
        puttab( fp, FITS_A_BYTE, (unsigned)( item - protab[pidx]->items ) );
    }
    end_table( fp );
    begin_table( fp, "YYPLHSTYPE", "yyplhstab" );
    for( pidx = 0; pidx < npro; ++pidx ) {
        puttab( fp, FITS_A_BYTE, protab[pidx]->sym->token );
    }
    end_table( fp );
    fprintf( fp, "#ifdef YYDEBUG\n" );
    rule_base = 0;
    begin_table( fp, "unsigned short", "yyrulebase" );
    for( pidx = 0; pidx < npro; ++pidx ) {
        for( item = protab[pidx]->items; item->p.sym != NULL; ) {
            ++item;
        }
        puttab( fp, FITS_A_WORD, rule_base );
        rule_base += (int)( item - protab[pidx]->items );
    }
    end_table( fp );
    begin_table( fp, "YYCHKTYPE", "yyrhstoks" );
    for( pidx = 0; pidx < npro; ++pidx ) {
        for( item = protab[pidx]->items; item->p.sym != NULL; ++item ) {
            puttab( fp, FITS_A_BYTE, item->p.sym->token );
        }
    }
    end_table( fp );
    begin_table( fp, "char YYFAR *", "yytoknames" );
    fputc( '\n', fp );
    for( sym_idx = 0; sym_idx < nsym; ++sym_idx ) {
        fprintf( fp, "\"%s\",\n", symtab[sym_idx]->name );
    }
    fprintf( fp, "\"\"" );
    end_table( fp );
    fprintf( fp, "#endif\n" );
    FREE( table );
}
