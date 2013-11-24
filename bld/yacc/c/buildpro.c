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
#include <string.h>
#include "alloc.h"
#include "yacc.h"

index_n npro;
index_n nsym;
index_n nterm;
index_n nvble;
index_n nitem;

a_sym   **symtab, *symlist, *startsym, *eofsym, *goalsym, *nosym, *errsym;
a_pro   **protab, *startpro;

a_sym   *addsym( char *s );
a_pro   *addpro( a_sym *sym, a_sym **rhs, int n );

void buildpro( void )
{
    a_sym       *sym;
    a_sym       *rhs[2];
    a_pro       *pro;
    an_item     *item;
    index_n     i;
    index_n     j;

    if( startsym == NULL ) {
        msg( "No grammar specified.\n" );
    }
    /* construct: $start <- <start_symbol> $eof */
    rhs[0] = startsym;
    rhs[1] = eofsym;
    goalsym = addsym( "$start" );
    addpro( goalsym, rhs, 2 );
    startsym = goalsym;
    startpro = startsym->pro;
    nterm = 0;
    nvble = 0;
    for( sym = symlist; sym != NULL; sym = sym->next ) {
        if( sym->pro != NULL ) {
            nvble++;
            for( pro = sym->pro; pro != NULL; pro = pro->next ) {
                ++nitem;
                for( item = pro->item; item->p.sym != NULL; ++item ) {
                    ++nitem;
                }
            }
        } else {
            nterm++;
        }
    }
    nsym = nterm + nvble;
    symtab = CALLOC( nsym, a_sym * );
    protab = CALLOC( npro, a_pro * );
    i = 0;
    j = 0;
    for( sym = symlist; sym != NULL; sym = sym->next ) {
        if( sym->pro != NULL ) {
            sym->idx = nterm + j++;
            for( pro = sym->pro; pro != NULL; pro = pro->next ) {
                protab[pro->pidx] = pro;
            }
        } else {
            sym->idx = i++;
        }
        symtab[sym->idx] = sym;
    }
}

static a_sym **findsymptr( char *s )
{
    a_sym       **sym;

    for( sym = &symlist; *sym != NULL; sym = &(*sym)->next ) {
        if( strcmp( s, (*sym)->name ) == 0 ) {
            break;
        }
    }
    return( sym );
}

a_sym *findsym( char *s )
{
    return( *findsymptr( s ) );
}

a_sym *addsym( char *s )
{
    a_sym       **sym;
    a_sym       *p;

    sym = findsymptr( s );
    if( *sym == NULL ) {
        p = CALLOC( 1, a_sym );
        p->name = strdup( s );
        *sym = p;
    }
    return( *sym );
}

a_pro *addpro( a_sym *sym, a_sym **rhs, int n )
{
    a_pro       *pro;
    size_t      amt;
    int         i;

    amt = sizeof( a_pro ) + n * sizeof( an_item );
    pro = (a_pro *)CALLOC( amt, char );
    pro->pidx = npro++;
    for( i = 0; i < n; ++i ) {
        pro->item[i].p.sym = rhs[i];
    }
    pro->item[n + 0].p.sym = NULL;
    pro->item[n + 1].p.pro = pro;
    pro->sym = sym;
    pro->next = sym->pro;
    pro->SR_conflicts = NULL;
    pro->used = FALSE;
    pro->unit = FALSE;
    sym->pro = pro;
    return( pro );
}

void showpro( void )
{
    index_n     i;

    for( i = 0; i < npro; ++i ) {
        showitem( protab[i]->item, "" );
    }
}

void showitem( an_item *p, char *dot )
{
    an_item     *q;
    a_pro       *pro;

    for( q = p; q->p.sym != NULL; ) {
        ++q;
    }
    pro = q[1].p.pro;
    printf( "%3d (%03x): %s <-", pro->pidx, pro->pidx, pro->sym->name );
    for( q = pro->item; ; ++q ) {
        if( q == p ) {
            printf( "%s", dot );
        }
        if( q->p.sym == NULL )
            break;
        printf( " %s", q->p.sym->name );
    }
    if( pro->unit ) {
        printf( " (unit production)" );
    }
    printf( "\n" );
}

void show_unused( void )
{
    unsigned    count;
    index_n     i;

    count = 0;
    for( i = 0; i < npro; ++i ) {
        if( protab[i]->sym == goalsym )
            continue;
        if( protab[i]->used == FALSE ) {
            ++count;
        }
    }
    dumpstatistic( "number of rules not reduced", count );
    if( count ) {
        for( i = 0; i < npro; ++i ) {
            if( protab[i]->sym == goalsym )
                continue;
            if( protab[i]->used == FALSE ) {
                showitem( protab[i]->item, "" );
            }
        }
    }
}
