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
#include <ctype.h>
#include "yacc.h"
#include "yaccins.h"
#include "alloc.h"

enum {
    ACTION_AMBIGUOUS    = 0x8000,
    ACTION_SHIFT        = 0x4000,
    ACTION_REDUCE       = 0x0000,
    ACTION_STATE        = 0x3fff,
};

typedef struct {
    short       token;
    short       action;
} a_entry;

static a_entry *table;
static unsigned used;
static unsigned table_size;

void dump_define( FILE *fp, char *name, int i )
{
    fprintf( fp, "#define\t%-20s\t%d\n", name, i );
}

static int tabcol;

static void begin_table( FILE *fp, char *tipe, char *name )
{
    fprintf( fp, "static const %s YYFAR %s[] = {", tipe, name );
    tabcol = 0;
}

void puttab( FILE *fp, value_size fits, unsigned i )
{
    char *format;
    unsigned mod;

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


static void dump_reduction( a_reduce_action *rx, unsigned *base )
{
    a_pro *pro;
    set_size *mp;

    pro = rx->pro;
    for( mp = Members( rx->follow ); mp-- != setmembers; ) {
        add_table( *mp, ACTION_REDUCE | pro->pidx );
        ++(*base);
    }
}

void genobj( FILE *fp )
{
    int i;
    int ntoken;
    int this_token;
    int any_token;
    int action;
    short *p;
    set_size *mp;
    a_pro *pro;
    a_state *x;
    a_reduce_action *rx;
    a_reduce_action *default_reduction;
    a_shift_action *tx;
    a_sym *sym;
    an_item *item;
    unsigned max;
    unsigned sum;
    set_size savings;
    set_size max_savings;
    unsigned base;
    unsigned rule_base;
    short *state_base;

    ntoken = FirstNonTerminalTokenValue();
    for( i = nterm; i < nsym; ++i ) {
        symtab[i]->token = ntoken++;
    }
    any_token = ntoken;
    state_base = CALLOC( nstate, short );
    base = 0;
    max = 0;
    sum = 0;
    used = 0;
    table_size = 0;
    table = NULL;
    for( i = 0; i < nstate; ++i ) {
        state_base[i] = base;
        x = statetab[i];
        for( tx = x->trans; (sym = tx->sym) != NULL; ++tx ) {
            add_table( sym->idx, ACTION_SHIFT | tx->state->sidx );
            ++base;
        }
        default_reduction = NULL;
        max_savings = 0;
        for( rx = x->redun; rx->pro != NULL; ++rx ) {
            if( (savings = Members( rx->follow ) - setmembers) == 0 )
                continue;
            if( max_savings < savings ) {
                max_savings = savings;
                if( default_reduction != NULL ) {
                    dump_reduction( default_reduction, &base );
                }
                default_reduction = rx;
            } else {
                dump_reduction( rx, &base );
            }
        }
        if( default_reduction != NULL ) {
            pro = default_reduction->pro;
            action = ACTION_REDUCE | pro->pidx;
        } else {
            action = ACTION_SHIFT | 0;
        }
        add_table( any_token, action );
        ++base;
        sum += base - state_base[i];
        if( base - state_base[i] > max ) {
            max = base - state_base[i];
        }
    }
    printf( "avg: %u max: %u\n", sum / nstate, max );
    dump_define( fp, "YYANYTOKEN", any_token );
    dump_define( fp, "YYEOFTOKEN", eofsym->token );
    dump_define( fp, "YYSTART", startstate->sidx );
    begin_table( fp, "YYACTTYPE", "yybasetab" );
    for( i = 0; i < nstate; ++i ) {
        puttab( fp, FITS_A_WORD, state_base[i] );
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
    for( i = 0; i < npro; ++i ) {
        for( item = protab[i]->item; item->p.sym != NULL; ) {
            ++item;
        }
        puttab( fp, FITS_A_BYTE, (unsigned)( item - protab[i]->item ) );
    }
    end_table( fp );
    begin_table( fp, "YYPLHSTYPE", "yyplhstab" );
    for( i = 0; i < npro; ++i ) {
        puttab( fp, FITS_A_BYTE, protab[i]->sym->token );
    }
    end_table( fp );
    fprintf( fp, "#ifdef YYDEBUG\n" );
    rule_base = 0;
    begin_table( fp, "unsigned short", "yyrulebase" );
    for( i = 0; i < npro; ++i ) {
        for( item = protab[i]->item; item->p.sym != NULL; ) {
            ++item;
        }
        puttab( fp, FITS_A_WORD, rule_base );
        rule_base += (int)( item - protab[i]->item );
    }
    end_table( fp );
    begin_table( fp, "YYCHKTYPE", "yyrhstoks" );
    for( i = 0; i < npro; ++i ) {
        for( item = protab[i]->item; item->p.sym != NULL; ++item ) {
            puttab( fp, FITS_A_BYTE, item->p.sym->token );
        }
    }
    end_table( fp );
    begin_table( fp, "char YYFAR *", "yytoknames" );
    fputc( '\n', fp );
    for( i = 0; i < nsym; ++i ) {
        fprintf( fp, "\"%s\",\n", symtab[i]->name );
    }
    fprintf( fp, "\"\"" );
    end_table( fp );
    fprintf( fp, "#endif\n" );
    FREE( table );
}
