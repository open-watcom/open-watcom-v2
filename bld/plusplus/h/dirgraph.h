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


#ifndef __DIRGRAPH_H__
#define __DIRGRAPH_H__

// DIRGRAPH.H -- abstract definitions for directed-graph processing
//
// 93/03/15 -- J.W.Welch        -- defined

typedef struct dirgraph_vft DIRGRAPH_VFT;
typedef struct dirgraph_ctl DIRGRAPH_CTL;
typedef struct dirgraph_node DIRGRAPH_NODE;
typedef struct dirgraph_edge DIRGRAPH_EDGE;

typedef void *DIRGRAPH_OBJECT;


struct dirgraph_vft             // DIRGRAPH_VFT -- virtual functions
{
    void (*init_ctl)            // - initialize control information
        ( DIRGRAPH_CTL *        // - - control information
        );
    void (*fini_ctl)            // - finalize control information
        ( DIRGRAPH_CTL *        // - - control information
        );
    DIRGRAPH_NODE *(*alloc_node)// - allocate node
        ( DIRGRAPH_CTL *        // - - control information
        );
    DIRGRAPH_EDGE *(*alloc_edge)// - allocate edge
        ( DIRGRAPH_CTL *        // - - control information
        );
    void (*free_node)           // - free node
        ( DIRGRAPH_CTL *        // - - control information
        , DIRGRAPH_NODE *       // - - node to be freed
        );
    void (*free_edge)           // - free edge
        ( DIRGRAPH_CTL *        // - - control information
        , DIRGRAPH_EDGE *       // - - edge to be freed
        );
    DIRGRAPH_NODE *(*init_node) // - initialize non-abstract information
        ( DIRGRAPH_CTL *        // - - control information
        , DIRGRAPH_NODE *       // - - node to be initialized
        );
    DIRGRAPH_EDGE *(*init_edge) // - initialize non-abstract information
        ( DIRGRAPH_CTL *        // - - control information
        , DIRGRAPH_EDGE *       // - - edge to be initialized
        );
    DIRGRAPH_EDGE *(*dup_edge)  // - process allocated edge
        ( DIRGRAPH_CTL *        // - - control information
        , DIRGRAPH_EDGE *       // - - edge to be initialized
        );
    void (*prune_node)          // - prune node
        ( DIRGRAPH_CTL *        // - - control information
        , DIRGRAPH_NODE *       // - - node to be pruned
        );
    void (*prune_edge)          // - prune edge
        ( DIRGRAPH_CTL *        // - - control information
        , DIRGRAPH_EDGE *       // - - edge to be pruned
        );
};


struct dirgraph_ctl             // DIRGRAPH_CTL -- control information
{   DIRGRAPH_NODE *objects;     // - header for objects ring
    DIRGRAPH_VFT *vft;          // - virtual functions for graph
};


struct dirgraph_node            // DIRGRAPH_NODE -- node in graph
{   DIRGRAPH_NODE *next;        // - next in ring
    DIRGRAPH_OBJECT object;     // - object
    DIRGRAPH_EDGE *edges;       // - edges from object
};


struct dirgraph_edge            // DIRGRAPH_EDGE -- edge in graph
{   DIRGRAPH_EDGE *next;        // - next in ring from node
    DIRGRAPH_NODE *target;      // - target node in graph
};


// PROTOTYPES:


DIRGRAPH_EDGE *DgrfAddEdge(     // ADD EDGE TO GRAPH
    DIRGRAPH_CTL *ctl,          // - control information
    DIRGRAPH_NODE *source,      // - source object
    DIRGRAPH_NODE *target )     // - target object
;
DIRGRAPH_NODE *DgrfAddNode(     // ADD NODE TO GRAPH
    DIRGRAPH_CTL *ctl,          // - control information
    DIRGRAPH_OBJECT object )    // - object to be added
;
void DgrfConstruct(             // CONSTRUCTOR FOR DIRGRAPH_CTL
    DIRGRAPH_CTL *ctl,          // - control information
    DIRGRAPH_VFT *vft )         // - virtual functions
;
void DgrfDestruct(              // DESTRUCTOR FOR DIRGRAPH_CTL
    DIRGRAPH_CTL *ctl )         // - control information
;
DIRGRAPH_NODE *DgrfFindNode(    // FIND NODE IN GRAPH, GIVEN OBJECT
    DIRGRAPH_CTL *ctl,          // - control information
    DIRGRAPH_OBJECT object )    // - object to be added
;
void DgrfPruneNode(             // PRUNE NODE (AND EMANATING EDGES)
    DIRGRAPH_CTL *ctl,          // - control information
    DIRGRAPH_NODE *node )       // - node
;
boolean DgrfWalkEdges(          // WALK ALL EDGES FROM OBJECT
    DIRGRAPH_CTL *ctl,          // - control information
    DIRGRAPH_NODE *node,        // - node
    boolean (*walker)           // - walking routine
        ( DIRGRAPH_CTL *        // - - control information
        , DIRGRAPH_EDGE * ) )   // - - EDGE
;
boolean DgrfWalkObjects(        // WALK ALL OBJECTS
    DIRGRAPH_CTL *ctl,          // - control information
    boolean (*walker)           // - walking routine
        ( DIRGRAPH_CTL *        // - - control information
        , DIRGRAPH_NODE * ) )   // - - object
;


#endif
