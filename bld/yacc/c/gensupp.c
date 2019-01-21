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

unsigned long bytesused;

static unsigned tabcol;
static char *tablename;

token_n FirstNonTerminalTokenValue( void )
{
    index_n i;
    token_n j;
    token_n ntoken;

    ntoken = 0;
    for( i = 0; i < nterm; ++i ) {
        j = symtab[i]->token;
        if( j > ntoken ) {
            ntoken = j;
        }
    }
    if( nterm > 0 ) {
        ++ntoken;
    }
    return( ntoken );
}

static void putambig( FILE *fp, a_SR_conflict *ambig, base_n *base )
{
    conflict_id id;
    index_n ambig_state, ambig_state_based;
    index_n ambig_shift, ambig_shift_based;
    static char *msg[] = {
        "#define\tYYAMBIGS%u\t        %5d\t/* ambiguous state (%u) */\n",
        "#define\tYYAMBIGT%u\t        %5d\t/* token causing ambiguity */\n",
        "#define\tYYAMBIGH%u\t        %5d\t/* state to shift (%u) */\n",
        "#define\tYYAMBIGR%u\t        %5d\t/* rule to reduce */\n",
    };

    if( ambig->state == NULL ) {
        /* no S/R conflict was found */
        return;
    }
    ambig_state = ambig->state->sidx;
    ambig_shift = ambig->shift->sidx;
    if( base == NULL ) {
        ambig_state_based = ambig_state;
        ambig_shift_based = ambig_shift;
    } else {
        ambig_state_based = base[ambig_state];
        ambig_shift_based = base[ambig_shift];
    }
    id = ambig->id;
    fprintf( fp, msg[0], id, ambig_state_based, ambig_state );
    fprintf( fp, msg[1], id, ambig->sym->token );
    fprintf( fp, msg[2], id, ambig_shift_based, ambig_shift );
    fprintf( fp, msg[3], id, ambig->reduce );
}

void putambigs( FILE *fp, base_n *base )
{
    a_SR_conflict *ambig;

    for( ambig = ambiguousstates; ambig != NULL; ambig = ambig->next ) {
        putambig( fp, ambig, base );
    }
}

void putnum( FILE *fp, char *name, int i )
{
    fprintf( fp, "#define\t%-20s\t%5d\n", name, i );
}

void begtab( FILE *fp, char *tipe, char *name )
{
    fprintf( fp, "static const %s YYFAR %s[] = {", tipe, name );
    tablename = name;
    tabcol = 0;
}

void puttab( FILE *fp, value_size fits, unsigned i )
{
    char *format;
    unsigned mod;

    if( fits == FITS_A_BYTE ) {
        if(( i & 0x00ff ) != i ) {
            msg( "value cannot fit into %s table! (%x)", tablename, i );
        }
        bytesused += 1;
        format = "%3u";
        mod = 20;
    } else {
        bytesused += sizeof( short );
        format = "%5u";
        mod = 10;
    }
    if( tabcol ) {
        fprintf( fp, "," );
    }
    if( !(tabcol % mod) ) {
        fprintf( fp, "\n/* %4u */ ", tabcol );
    }
    fprintf( fp, format, i );
    ++tabcol;
}

void putcompact( FILE *fp, token_n token, action_n action )
{
    if( tabcol ) {
        fprintf( fp, "," );
    }
    if( !(tabcol % 5 ) ) {
        fprintf( fp, "\n/* %4u */ ", tabcol );
    }
    fprintf( fp, "{%4d,%2d,%3d}", token, action >> 8, action & 0xff );
    bytesused += 3;
    ++tabcol;
}

void endtab( FILE *fp )
{
    fprintf( fp, "\n};\n" );
}

void putcomment( FILE *fp, char *comment )
{
    fprintf( fp, "/* %s */\n", comment );
}

void puttokennames( FILE *fp, token_n dtoken, value_size token_size )
{
    unsigned rule_base;
    an_item *item;
    unsigned i;

    if( ! denseflag ) {
        return;
    }
    fprintf( fp, "#ifdef YYDEBUG\n" );
    rule_base = 0;
    begtab( fp, "unsigned short", "yyrulebase" );
    for( i = 0; i < npro; ++i ) {
        for( item = protab[i]->items; item->p.sym != NULL; ) {
            ++item;
        }
        puttab( fp, FITS_A_WORD, rule_base );
        rule_base += (unsigned)( item - protab[i]->items );
    }
    endtab( fp );
    begtab( fp, "YYTOKENTYPE", "yyrhstoks" );
    for( i = 0; i < npro; ++i ) {
        for( item = protab[i]->items; item->p.sym != NULL; ++item ) {
            puttab( fp, token_size, item->p.sym->token );
        }
    }
    endtab( fp );
    begtab( fp, "char YYFAR *", "yytoknames" );
    fputc( '\n', fp );
    for( i = 0; i < nsym; ++i ) {
        if( dtoken != 0 && symtab[i]->token == dtoken ) {
            fprintf( fp, "\"$dtoken\",\n" );
            fprintf( fp, "\"$ptoken\",\n" );
        }
        fprintf( fp, "\"%s\",\n", symtab[i]->name );
    }
    fprintf( fp, "\"\"" );
    endtab( fp );
    fprintf( fp, "#endif\n" );
}
