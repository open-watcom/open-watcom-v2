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


typedef struct edge_entry {
    struct edge_entry   *next;
    block_edge          *edge;
} edge_entry;

typedef struct edge_stack {
    edge_entry          *top;
} edge_stack;

extern edge_stack   *EdgeStackInit( void );
extern void         EdgeStackFini( edge_stack * );
extern bool         EdgeStackEmpty( edge_stack * );
extern void         EdgeStackPush( edge_stack *, block_edge * );
extern block_edge   *EdgeStackPop( edge_stack * );
extern void         PointEdge( block_edge *edge, block *new_dest );
extern void         RemoveEdge( block_edge *edge );
extern void         MoveEdge( block_edge *edge, block *new_dest );
extern block        *SplitBlock( block *blk, instruction *ins );
