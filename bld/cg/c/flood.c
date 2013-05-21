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


#include <assert.h>
#include "cgstd.h"
#include "coderep.h"
#include "conflict.h"
#include "opcodes.h"
#include "cgdefs.h"
#include "model.h"

typedef bool (*flood_func)( block *, void * );

typedef struct stupid_struct_so_I_can_use_safe_recurse {
    block       *blk;
    flood_func  func;
    void        *parm;
} flood_parms;

#define _Visited( blk )         ( ( (blk)->class & BLOCK_VISITED ) != EMPTY )
#define _UnVisited( blk )       ( (blk)->class &= ~BLOCK_VISITED )
#define _MarkVisited( blk )     ( (blk)->class |=  BLOCK_VISITED )

extern  void    ClearBlockBits( block_class );
extern  pointer SafeRecurse(pointer(*)(),pointer);

static pointer doFloodForward( void *fp ) {

    block       *next;
    block_num   i;
    block_num   n;
    flood_parms new_parms;
    flood_parms *p = fp;

    new_parms = *p;
    n = p->blk->targets;
    for( i = 0; i < n; i++ ) {
        next = p->blk->edge[ i ].destination;
        if( _Visited( next ) ) continue;
        if( p->func( next, p->parm ) == FALSE ) break;
        _MarkVisited( next );
        new_parms.blk = next;
        SafeRecurse( doFloodForward, &new_parms );
    }
    return NULL;
}

extern void FloodForward( block *blk, flood_func func, void *parm ) {
    flood_parms parms;

    ClearBlockBits( BLOCK_VISITED );
    parms.blk = blk;
    parms.func = func;
    parms.parm = parm;
    doFloodForward( &parms );
}

static pointer doFloodBackward( pointer fp ) {

    block       *next;
    block_edge  *edge;
    flood_parms new_parms;
    flood_parms *p = fp;

    new_parms = *p;
    for( edge = p->blk->input_edges; edge != NULL; edge = edge->next_source ) {
        next = edge->source;
        if( _Visited( next ) ) continue;
        if( p->func( next, p->parm ) == FALSE ) break;
        _MarkVisited( next );
        new_parms.blk = next;
        SafeRecurse( doFloodBackward, &new_parms );
    }
    return NULL;
}

extern void FloodBackwards( block *start, flood_func func, void *parm ) {
/************************************************************************
    Flood backwards in the flow graph, calling the given function for each
    block as it is visited
*/
    flood_parms parms;

    ClearBlockBits( BLOCK_VISITED );
    parms.blk = start;
    parms.func = func;
    parms.parm = parm;
    doFloodBackward( &parms );
}
