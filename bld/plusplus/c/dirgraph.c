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


#include "plusplus.h"
#include "dirgraph.h"
#include "ring.h"

#ifndef NDEBUG
    #include "errdefns.h"
#endif


DIRGRAPH_EDGE *DgrfAddEdge(     // ADD EDGE TO GRAPH
    DIRGRAPH_CTL *ctl,          // - control information
    DIRGRAPH_NODE *source,      // - source object
    DIRGRAPH_NODE *target )     // - target object
{
    DIRGRAPH_EDGE *edge;        // - new edge
    DIRGRAPH_EDGE *srch;        // - old edge

    edge = NULL;
    RingIterBeg( source->edges, srch ) {
        if( srch->target == target ) {
            edge = srch;
            break;
        }
    } RingIterEnd( srch )
    if( edge == NULL ) {
        edge = (*ctl->vft->alloc_edge)( ctl );
        edge->target = target;
        RingAppend( &source->edges, edge );
        edge = (*ctl->vft->init_edge)( ctl, edge );
    } else {
        edge = (*ctl->vft->dup_edge)( ctl, edge );
    }
    return edge;
}


DIRGRAPH_NODE *DgrfAddNode(     // ADD NODE TO GRAPH, IF NOT ALREADY THERE
    DIRGRAPH_CTL *ctl,          // - control information
    DIRGRAPH_OBJECT object )    // - object to be added
{
    DIRGRAPH_NODE *node;        // - new node

    node = DgrfFindNode( ctl, object );
    if( node == NULL ) {
        node = (*ctl->vft->alloc_node)( ctl );
        node->object = object;
        node->edges = NULL;
        node = (*ctl->vft->init_node)( ctl, node );
        RingAppend( &ctl->objects, node );
    }
    return node;
}


DIRGRAPH_NODE *DgrfFindNode(    // FIND NODE IN GRAPH, GIVEN OBJECT
    DIRGRAPH_CTL *ctl,          // - control information
    DIRGRAPH_OBJECT object )    // - object to be added
{
    DIRGRAPH_NODE *node;        // - new node
    DIRGRAPH_NODE *srch;        // - old node

    node = NULL;
    RingIterBeg( ctl->objects, srch ) {
        if( srch->object == object ) {
            node = srch;
            break;
        }
    } RingIterEnd( srch )
    return node;
}


void DgrfConstruct(             // CONSTRUCTOR FOR DIRGRAPH_CTL
    DIRGRAPH_CTL *ctl,          // - control information
    DIRGRAPH_VFT *vft )         // - virtual functions
{
    ctl->objects = NULL;
    ctl->vft = vft;
    (*ctl->vft->init_ctl)( ctl );
}


static boolean free_edge(       // FREE AN EDGE
    DIRGRAPH_CTL *ctl,          // - control information
    DIRGRAPH_EDGE *edge )       // - edge to be freed
{
    (*ctl->vft->free_edge)( ctl, edge );
    return FALSE;
}


static boolean free_node(       // FREE A NODE
    DIRGRAPH_CTL *ctl,          // - control information
    DIRGRAPH_NODE *node )       // - node to be freed
{
    DgrfWalkEdges( ctl, node, free_edge );
    (*ctl->vft->free_node)( ctl, node );
    return FALSE;
}


void DgrfDestruct(              // DESTRUCTOR FOR DIRGRAPH_CTL
    DIRGRAPH_CTL *ctl )         // - control information
{
    DgrfWalkObjects( ctl, &free_node );
    ctl->objects = NULL;
    (*ctl->vft->fini_ctl)( ctl );
}


boolean DgrfWalkEdges(          // WALK ALL EDGES FROM OBJECT
    DIRGRAPH_CTL *ctl,          // - control information
    DIRGRAPH_NODE *node,        // - node
    boolean (*walker)           // - walking routine
        ( DIRGRAPH_CTL *        // - - control information
        , DIRGRAPH_EDGE * ) )   // - - EDGE
{
    DIRGRAPH_EDGE *edge;        // - current edge
    boolean retn;               // - TRUE ==> walking terminated

    retn = FALSE;
    RingIterBegSafe( node->edges, edge ){
        retn = (*walker)( ctl, edge );
        if( retn ) break;
    } RingIterEndSafe( edge )
    return retn;
}


boolean DgrfWalkObjects(        // WALK ALL OBJECTS
    DIRGRAPH_CTL *ctl,          // - control information
    boolean (*walker)           // - walking routine
        ( DIRGRAPH_CTL *        // - - control information
        , DIRGRAPH_NODE * ) )   // - - node
{
    DIRGRAPH_NODE *node;        // - current object
    boolean retn;               // - TRUE ==> walking terminated

    retn = FALSE;
    RingIterBegSafe( ctl->objects, node ) {
        retn = (*walker)( ctl, node );
        if( retn ) break;
    } RingIterEndSafe( node )
    return retn;
}


static boolean pruneEdge(       // PRUNE EDGE FROM GRAPH
    DIRGRAPH_CTL *ctl,          // - control information
    DIRGRAPH_EDGE *edge )       // - edge to be pruned
{
    (*ctl->vft->prune_edge)( ctl, edge );
    (*ctl->vft->free_edge)( ctl, edge );
    return FALSE;
}


void DgrfPruneNode(             // PRUNE NODE (AND EMANATING EDGES)
    DIRGRAPH_CTL *ctl,          // - control information
    DIRGRAPH_NODE *node )       // - node
{
    DgrfWalkEdges( ctl, node, &pruneEdge );
    (*ctl->vft->prune_node)( ctl, node );
    RingPrune( &ctl->objects, node );
    (*ctl->vft->free_node)( ctl, node );
}
