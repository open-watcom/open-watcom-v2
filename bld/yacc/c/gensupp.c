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

unsigned MaxTerminalTokenValue( void )
{
    unsigned i;
    unsigned j;
    unsigned ntoken;

    ntoken = 0;
    for( i = 0; i < nterm; ++i ) {
        j = symtab[i]->token;
        if( j > ntoken ) {
            ntoken = j;
        }
    }
    return( ntoken );
}

static void putambig( a_SR_conflict *ambig, short *base )
{
    int i;
    index_t ambig_state, ambig_state_based;
    index_t ambig_shift, ambig_shift_based;
    static char *msg[] = {
    "#define\tYYAMBIGS%u\t\t%d\t/* ambiguous state (%u) */\n",
    "#define\tYYAMBIGT%u\t\t%d\t/* token causing ambiguity */\n",
    "#define\tYYAMBIGH%u\t\t%d\t/* state to shift (%u) */\n",
    "#define\tYYAMBIGR%u\t\t%d\t/* rule to reduce */\n",
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
    i = ambig->id;
    fprintf( actout, msg[0], i, ambig_state_based, ambig_state );
    fprintf( actout, msg[1], i, ambig->sym->token );
    fprintf( actout, msg[2], i, ambig_shift_based, ambig_shift );
    fprintf( actout, msg[3], i, ambig->reduce );
}

void putambigs( short *base )
{
    a_SR_conflict *cx;

    for( cx = ambiguousstates; cx != NULL; cx = cx->next ) {
        putambig( cx, base );
    }
}

void putnum( char *name, int i )
{
    fprintf( actout, "#define\t%-20s\t%d\n", name, i );
}

void begtab( char *tipe, char *name )
{
    fprintf( actout, "static const %s YYFAR %s[] = {", tipe, name );
    tablename = name;
    tabcol = 0;
}

void puttab( value_size fits, int i )
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
        bytesused += sizeof(short);
        format = "%5u";
        mod = 10;
    }
    if( tabcol ) {
        fprintf( actout, "," );
    }
    if( !(tabcol % mod) ) {
        fprintf( actout, "\n/* %4u */ ", tabcol );
    }
    fprintf( actout, format, i );
    ++tabcol;
}

void putcompact( unsigned token, unsigned action )
{
    if( tabcol ) {
        fprintf( actout, "," );
    }
    if( !(tabcol % 5 ) ) {
        fprintf( actout, "\n/* %4u */ ", tabcol );
    }
    fprintf( actout, "{%4d,%2d,%3d}", token, action >> 8, action & 0xff );
    bytesused += 3;
    ++tabcol;
}

void endtab( void )
{
    fprintf( actout, "\n};\n" );
}

void putcomment( char *comment )
{
    fprintf( actout, "/* %s */\n", comment );
}

void puttokennames( int dtoken, value_size token_size )
{
    unsigned rule_base;
    an_item *item;
    unsigned i;

    if( ! denseflag ) {
        return;
    }
    fprintf( actout, "#ifdef YYDEBUG\n" );
    rule_base = 0;
    begtab( "unsigned short", "yyrulebase" );
    for( i = 0; i < npro; ++i ) {
        for( item = protab[i]->item; item->p.sym != NULL; ++item )
          /* do nothing */;
        puttab( FITS_A_WORD, rule_base );
        rule_base += item - protab[i]->item;
    }
    endtab();
    begtab( "YYTOKENTYPE", "yyrhstoks" );
    for( i = 0; i < npro; ++i ) {
        for( item = protab[i]->item; item->p.sym != NULL; ++item ) {
            puttab( token_size, item->p.sym->token );
        }
    }
    endtab();
    begtab( "char YYFAR *", "yytoknames" );
    fputc( '\n', actout );
    for( i = 0; i < nsym; ++i ) {
        if( dtoken != 0 && i == dtoken ) {
            fprintf( actout, "\"$dtoken\",\n" );
            fprintf( actout, "\"$ptoken\",\n" );
        }
        fprintf( actout, "\"%s\",\n", symtab[i]->name );
    }
    fprintf( actout, "\"\"" );
    endtab();
    fprintf( actout, "#endif\n" );
}
