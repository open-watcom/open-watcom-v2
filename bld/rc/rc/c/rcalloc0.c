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
* Description:  Resource compiler memory layer 0.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "rcalloc0.h"
#include "errors.h"
#ifdef RCMEM_DEBUG
#include "errprt.h"
#endif

static void RCMemLayer0InitFreeList( FreeListInfo *freelist, size_t heapsize, size_t blocks_per_heap )
/****************************************************************************************************/
{
    size_t      i;

    for( i = 0; i < blocks_per_heap - 1; i++ ) {
#ifdef RCMEM_DEBUG
        freelist->u.next = (FreeListInfo *)( freelist->u.data + heapsize + 1 );
#else
        freelist->u.next = (FreeListInfo *)( freelist->u.data + heapsize );
#endif
        freelist = freelist->u.next;
    }
    freelist->u.next = NULL;
}

static HeapList *RCMemLayer0AddToHeap( size_t heapsize, size_t blocks_per_heap )
/******************************************************************************/
{
    FreeListInfo    *freelist;
    HeapList        *data;
    size_t          allocsize;

#ifdef RCMEM_DEBUG
    allocsize = ( sizeof( DebugMemInfo ) + heapsize + 1 ) * blocks_per_heap + sizeof( HeapList );
#else
    allocsize = heapsize * blocks_per_heap + sizeof( HeapList );
#endif
    data = malloc( allocsize );
#ifdef RCMEM_DEBUG
    memset( data, RCMEM_GARBAGEBYTE, allocsize );
#endif
    if( data == NULL ) {
        RcFatalError( ERR_OUT_OF_MEMORY );
    }
    data->next = NULL;
    freelist = (FreeListInfo *)( (char *)data + sizeof( HeapList ) );
    RCMemLayer0InitFreeList( freelist, heapsize, blocks_per_heap );

    return( data );
}

extern HeapHandle *RCMemLayer0NewHeap( size_t heapsize, size_t blocks_per_heap )
/******************************************************************************/
{
    HeapHandle         *heap;

    heap = malloc( sizeof( HeapHandle ) );
    if( heap == NULL ) {
        RcFatalError( ERR_OUT_OF_MEMORY );
    }
    heap->list = NULL;
    heap->freeList = NULL;
    heap->heapsize = heapsize;
    heap->blocksize = blocks_per_heap;

    return( heap );
}

#ifdef RCMEM_DEBUG
extern void *RCMemLayer0Malloc( HeapHandle *heap, size_t size )
#else
extern void *RCMemLayer0Malloc( HeapHandle *heap )
#endif
/************************************************/
{
    HeapList           *newheap;
    FreeListInfo       *freemem;

    if( heap->freeList == NULL ) {
        newheap = RCMemLayer0AddToHeap( heap->heapsize, heap->blocksize );
        newheap->next = heap->list;
        heap->list = newheap;
        heap->freeList = (FreeListInfo *)( (char *)newheap + sizeof( HeapList ) );
    }
    freemem = heap->freeList;
    heap->freeList = freemem->u.next;
#ifdef RCMEM_DEBUG
    freemem->dbg.size = size;
    freemem->dbg.startbyte = RCMEM_STARTBYTE;
    freemem->u.data[size] = RCMEM_ENDBYTE;
#endif
    return( freemem->u.data );
}

extern void RCMemLayer0Free( void *mem, HeapHandle *heap )
/********************************************************/
{
    FreeListInfo        *freemem;

#ifdef RCMEM_DEBUG
    if( mem == NULL ) {
        RcMsgFprintf( stderr, NULL, "Free NULL pointer\n" );
    }
#endif
    freemem = (FreeListInfo *)( (char *)mem - offsetof( FreeListInfo, u.data ) );
#ifdef RCMEM_DEBUG
    if( ((unsigned char *)mem)[freemem->dbg.size] != RCMEM_ENDBYTE ) {
        RcMsgFprintf( stderr, NULL, "(%x) Memory Overrun\n", mem );
    }
    freemem->dbg.startbyte = !RCMEM_STARTBYTE;
#endif
    freemem->u.next = heap->freeList;
    heap->freeList = freemem;
}

#ifdef RCMEM_DEBUG

static void RCMemLayer0CheckUnfreed( HeapHandle *heap, char *freelist )
/*********************************************************************/
{
    size_t          i;

    for( i = 0; i < heap->blocksize; i++ ) {
        if( freelist->dbg.startbyte == RCMEM_STARTBYTE ) {
            RcMsgFprintf( stderr, NULL,
                        "Unfreed Memory Detected (0x%x bytes at 0x%x)\n",
                        freelist->dbg.size, freelist->u.data );
            freelist->dbg.startbyte = !RCMEM_STARTBYTE;
        }
        freelist += sizeof( DebugMemInfo ) + heap->heapsize + 1;
    }
}

#endif

extern void RCMemLayer0ShutDown( HeapHandle *heap )
/*************************************************/
{
    HeapList         *curnode;
    HeapList         *nextnode;

    for( curnode = heap->list; curnode != NULL; curnode = nextnode ) {
        nextnode = curnode->next;
#ifdef RCMEM_DEBUG
        RCMemLayer0CheckUnfreed( heap, (char *)curnode + sizeof( HeapList ) );
#endif
        free( curnode );
    }
    free( heap );
}
