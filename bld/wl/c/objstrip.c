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
* Description:  Dead code elimination routines.
*
****************************************************************************/


#include <string.h>
#include "walloca.h"
#include "linkstd.h"
#include "alloc.h"
#include "objstrip.h"

/* data structure used to keep track of all the edges in the call graph */

typedef struct edgelist {
    struct edgelist *   next;
    union {
        segdata *       seg;
        symbol *        sym;
    } u;
    unsigned            issym : 1;      // true if contains a symbol
    unsigned            reverse_dir : 1;// true if edge points in the opposite
                                        // direction
} edgelist;

static edgelist * FreedEdges;

/* some handy routines for dealing with edges. */

void ResetObjStrip( void )
/************************/
{
    FreedEdges = NULL;
}

static edgelist * AllocEdge( void )
/*********************************/
{
    edgelist *  edge;

    if( FreedEdges != NULL ) {
        edge = FreedEdges;
        FreedEdges = edge->next;
    } else {
        _Pass1Alloc( edge, sizeof( edgelist ) );
    }
    memset( edge, 0, sizeof( *edge ) );
    return( edge );
}

static void FreeEdge( edgelist * edge )
/*************************************/
/* keep track of free edges on our own free list */
{
    edge->next = FreedEdges;
    FreedEdges = edge;
}

static void FreeEdgeList( edgelist * edge )
/*****************************************/
/* free a list of edges. */
{
    edgelist *  listend;

    if( edge == NULL )
        return;
    for( listend = edge; listend->next != NULL; ) {
        listend = listend->next;
    }
    listend->next = FreedEdges;
    FreedEdges = edge;
}

static void PruneNonSymEdges( symbol * sym )
/******************************************/
/* remove any edges from the list which don't point to symbols */
{
    edgelist *  list;
    edgelist *  next;

    list = sym->p.edges;
    sym->p.edges = NULL;
    for( ;list != NULL; list = next ) {
        next = list->next;
        if( list->issym ) {
            list->next = sym->p.edges;
            sym->p.edges = list;
        } else {
            FreeEdge( list );
        }
    }
}

void RefSeg( segdata * seg )
/*********************************/
/* mark a segment as being referenced, and chase through the segments that it
 * points to make sure they are also marked */
{
    edgelist *  edge;
    edgelist *  next;

    if( seg->isrefd || seg->visited )
        return;
//  if( !seg->iscode ) return;
    seg->isrefd = true;
    seg->visited = true;
    for( edge = seg->a.refs; edge != NULL; edge = next ) {
        DbgAssert( edge->issym == 0 );
        next = edge->next;
        RefSeg( edge->u.seg );
        FreeEdge( edge );
    }
    seg->a.refs = NULL;
    seg->visited = false;
}

void DataRef( symbol * sym )
/*********************************/
/* symbol referenced from data, so make sure it is included */
{
    if( (LinkState & HAVE_PPC_CODE) && (FmtData.type & MK_PE) ) {
        size_t      len = strlen( sym->name ) + 3;
        char        *s = alloca( len );

        s[ 0 ] = s[ 1 ] = '.';
        strcpy( s + 2, sym->name );
        sym = SymOp( ST_FIND, s, len - 1 );
    }
    if( (sym->info & SYM_DEFINED) && !IS_SYM_IMPORTED( sym ) ) {
        RefSeg( sym->p.seg );
    }
    sym->info |= SYM_DCE_REF;
}

void AddEdge( segdata * seg, symbol * sym )
/************************************************/
/* reference from a segment to a symbol */
{
    edgelist *  edge;

    if( sym->info & SYM_DEFINED ) {
        if( !IS_SYM_IMPORTED(sym) && seg != sym->p.seg && sym->p.seg != NULL ) {
            if( seg->isrefd || !seg->iscode ) {
                RefSeg( sym->p.seg );
            } else if( !sym->p.seg->isrefd ) {
                edge = AllocEdge();
                edge->u.seg = sym->p.seg;
                edge->issym = false;
                edge->next = seg->a.refs;
                seg->a.refs = edge;
            }
        }
    } else {    /* symbol is undefined */
        if( seg->isrefd || !seg->iscode ) {
            sym->info |= SYM_DCE_REF;
            PruneNonSymEdges( sym );
        } else {
            edge = AllocEdge();
            edge->u.seg = seg;
            edge->next = sym->p.edges;
            edge->issym = false;
            edge->reverse_dir = true;
            sym->p.edges = edge;
        }
    }
}

void AddSymEdge( symbol * srcsym, symbol * targsym )
/*********************************************************/
/* make a reference from one symbol to another */
{
    edgelist *  edge;

    if( srcsym->info & SYM_DEFINED ) {
        AddEdge( srcsym->p.seg, targsym );
    } else {
        edge = AllocEdge();
        edge->u.sym = targsym;
        edge->issym = true;
        edge->next = srcsym->p.edges;
        srcsym->p.edges = edge;
    }
}

void AddSymSegEdge( symbol *srcsym, segdata *targsdata )
/*********************************************************/
/* make a reference from symbol to segment */
{
    edgelist *  edge;

    if( IS_SYM_IMPORTED(srcsym) ) {
        RefSeg(targsdata);
        return;
    }
    edge = AllocEdge();
    edge->u.seg = targsdata;
    edge->issym = false;
    if( srcsym->info & SYM_DEFINED ) {
        edge->next = targsdata->a.refs;
        targsdata->a.refs = edge;
    } else {
        edge->next = srcsym->p.edges;
        srcsym->p.edges = edge;
    }
}

void DefStripSym( symbol * sym, segdata * seg )
/****************************************************/
/* Handle the effects of this symbol on the function call graph */
/* remember - the symbol contains a list of references to it's defining segment
 * so, if it's segment is ref'd, none of the references matter. if it is not,
 * and the referencing seg. is not, we have to put the edge on the ref'ing seg.
*/
{
    edgelist *  list;
    edgelist *  next;

    if( sym->info & SYM_DCE_REF ) {
        RefSeg( seg );
    }
    for( list = sym->p.edges; list != NULL; list = next ) {
        next = list->next;
        if( list->issym ) {
            DbgAssert(list->reverse_dir == 0); // for now this cannot happen
            AddEdge( seg, list->u.sym );
            FreeEdge( list );
        } else {
            segdata *src, *trg;
            if( list->reverse_dir ) {
                src = list->u.seg;
                trg = seg;
            } else {
                src = seg;
                trg = list->u.seg;
            }
            if( trg->isrefd ) { /* seg is already refd, so don't need the edge*/
                FreeEdge( list );
            } else {
                if( src->isrefd ) {
                    RefSeg( trg );
                    FreeEdge( list );   /* no edge needed to ref'd seg. */
                } else {
                    list->reverse_dir = 0;
                    list->u.seg = trg;  // make it point to this target.
                    list->next = src->a.refs;   // & add it to the ref list
                    src->a.refs = list;
                }
            }
        }
    }
    sym->p.seg = seg;
}

void DefStripImpSym( symbol * sym )
/****************************************/
/* Imported symbols can now reference just like normal symbols, except that
 (at the time of this writing) they don't have a segment like normal
 symbols. This is really the only reason why I did not want to use
 DefStripSym instead. If later on they gain segments, you might want
 to use DefStripSym instead. Note also that at the time of this writing,
 imported symbols are always included (which should probably be changed
 in the future).
*/
{
    edgelist *  list;
    edgelist *  next;

    for( list = sym->p.edges; list != NULL; list = next ) {
        next = list->next;
        if( list->reverse_dir == 0 ) {
            if( list->issym ) {
                list->u.sym->info |= SYM_DCE_REF;
            } else {
                RefSeg(list->u.seg);
            }
        }
        FreeEdge( list );
    }
    sym->p.edges = NULL;
}

void CleanStripInfo( symbol * sym )
/****************************************/
/* remove any stripping information from a symbol which does not need it */
{
    FreeEdgeList( sym->p.edges );
    sym->p.edges = NULL;
}
