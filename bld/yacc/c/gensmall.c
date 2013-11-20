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
static unsigned used, table_size;

void dump_define( char *name, int i )
{
    fprintf( actout, "#define\t%-20s\t%d\n", name, i );
}

static int tabcol;

static void begin_table( char *tipe, char *name )
{
    fprintf( actout, "static const %s YYFAR %s[] = {", tipe, name );
    tabcol = 0;
}

void puttab( value_size fits, int i )
{
    char *format;
    unsigned mod;

    if( fits == FITS_A_BYTE ) {
        if(( i & 0x00ff ) != i ) {
            msg( "value cannot fit into table! (%x)", i );
        }
        format = "%3d";
        mod = 20;
    } else {
        format = "%5d";
        mod = 13;
    }
    if( tabcol ) {
        fprintf( actout, "," );
    }
    if( !(tabcol % mod) ) {
        fprintf( actout, "\n" );
    }
    fprintf( actout, format, i );
    ++tabcol;
}

static void end_table( void )
{
    fprintf( actout, "\n};\n" );
}

void add_table( short token, short action )
{
    if( used == table_size ) {
        table_size += 64;
        table = REALLOC( table, table_size, a_entry );
    }
    table[used].token = token;
    table[used].action = action;
    ++used;
}


void dump_reduction( a_reduce_action *rx, unsigned *base )
{
    a_pro *pro;
    short *mp;

    pro = rx->pro;
    for( mp = Members( rx->follow ); --mp >= setmembers; ) {
        add_table( *mp, ACTION_REDUCE | pro->pidx );
        ++(*base);
    }
}

void genobj( void )
{
    int i;
    int ntoken;
    int this_token;
    int any_token;
    int action;
    short *p;
    short *mp;
    a_pro *pro;
    a_state *x;
    a_reduce_action *rx;
    a_reduce_action *default_reduction;
    a_shift_action *tx;
    a_sym *sym;
    an_item *item;
    unsigned max;
    unsigned sum;
    unsigned savings;
    unsigned base;
    unsigned rule_base;
    short *state_base;

    ntoken = 0;
    for( i = 0; i < nterm; ++i ) {
        this_token = symtab[i]->token;
        if( this_token > ntoken ) {
            ntoken = this_token;
        }
    }
    for( i = nterm; i < nsym; ++i ) {
        symtab[i]->token = ++ntoken;
    }
    any_token = ++ntoken;
    state_base = CALLOC( nstate, short );
    base = 0;
    max = 0;
    sum = 0;
    for( i = 0; i < nstate; ++i ) {
        state_base[i] = base;
        x = statetab[i];
        for( tx = x->trans; (sym = tx->sym) != NULL; ++tx ) {
            add_table( sym->idx, ACTION_SHIFT | tx->state->sidx );
            ++base;
        }
        default_reduction = NULL;
        savings = 0;
        for( rx = x->redun; rx->pro != NULL; ++rx ) {
            mp = Members( rx->follow );
            if( mp != setmembers ) {
                if( mp - setmembers > savings ) {
                    savings = mp - setmembers;
                    if( default_reduction != NULL ) {
                        dump_reduction( default_reduction, &base );
                    }
                    default_reduction = rx;
                } else {
                    dump_reduction( rx, &base );
                }
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
    dump_define( "YYANYTOKEN", any_token );
    dump_define( "YYEOFTOKEN", eofsym->token );
    dump_define( "YYSTART", startstate->sidx );
    begin_table( "YYACTTYPE", "yybasetab" );
    for( i = 0; i < nstate; ++i ) {
        puttab( FITS_A_WORD, state_base[i] );
    }
    end_table();
    begin_table( "YYCHKTYPE", "yychktab" );
    for( i = 0; i < used; ++i ) {
        puttab( FITS_A_BYTE, table[i].token );
    }
    end_table();
    begin_table( "YYACTTYPE", "yyacttab" );
    for( i = 0; i < used; ++i ) {
        puttab( FITS_A_WORD, table[i].action );
    }
    end_table();
    begin_table( "YYPLENTYPE", "yyplentab" );
    for( i = 0; i < npro; ++i ) {
        for( item = protab[i]->item; item->p.sym != NULL; ) {
            ++item;
        }
        puttab( FITS_A_BYTE, (int)( item - protab[i]->item ) );
    }
    end_table();
    begin_table( "YYPLHSTYPE", "yyplhstab" );
    for( i = 0; i < npro; ++i ) {
        puttab( FITS_A_BYTE, protab[i]->sym->token );
    }
    end_table();
    fprintf( actout, "#ifdef YYDEBUG\n" );
    rule_base = 0;
    begin_table( "unsigned short", "yyrulebase" );
    for( i = 0; i < npro; ++i ) {
        for( item = protab[i]->item; item->p.sym != NULL; ) {
            ++item;
        }
        puttab( FITS_A_WORD, rule_base );
        rule_base += (int)( item - protab[i]->item );
    }
    end_table();
    begin_table( "YYCHKTYPE", "yyrhstoks" );
    for( i = 0; i < npro; ++i ) {
        for( item = protab[i]->item; item->p.sym != NULL; ++item ) {
            puttab( FITS_A_BYTE, item->p.sym->token );
        }
    }
    end_table();
    begin_table( "char YYFAR *", "yytoknames" );
    fputc( '\n', actout );
    for( i = 0; i < nsym; ++i ) {
        fprintf( actout, "\"%s\",\n", symtab[i]->name );
    }
    fprintf( actout, "\"\"" );
    end_table();
    fprintf( actout, "#endif\n" );
}
