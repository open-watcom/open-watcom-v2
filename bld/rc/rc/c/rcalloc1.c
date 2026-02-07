/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2026      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Resource compiler memory layer 1.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bool.h"
#include "rcalloc0.h"
#include "rcalloc1.h"
#include "rcerrors.h"
#ifdef RCMEM_DEBUG
#include "errprt.h"
#endif


#define HEAPIDX_VAL(x)      (((HeapId *)((char *)(x)-sizeof(HeapId)))->idx)

#define _Layer0to1Size(x)   ((x)-sizeof(HeapId))
#define _Layer1to0Size(x)   ((x)+sizeof(HeapId))
#define _Layer0to1Ptr(x)    ((unsigned char*)(x)+sizeof(HeapId))
#define _Layer1to0Ptr(x)    ((unsigned char*)(x)-sizeof(HeapId))

typedef unsigned char   HeapIndex;

/*
 * Need a long instead of a char for Alpha to work,
 * as Alpha doesn't allow odd boundary memory accesses for non-chars.
 * Pls note that there might be problems (for AXP) of this kind lying around.
 * In fact, the RCMEM_DEBUG version is not working for AXP.
 */
typedef union HeapId {
    void            *__FILLER;
    HeapIndex       idx;
} HeapId;

typedef struct BigMemList {
    struct BigMemList   *next;
    size_t              size;
} BigMemList;

static const unsigned   BlocksPerHeap0[] = { 2048, 2048, 2048, 1024 };
static const unsigned   Heap0Sizes[] =     {   16,   32,   64, 1024 }; /* Ascending order */

#define NUM_HEAPS       4

static heap_handle      Heaps[NUM_HEAPS];

#define BIGLIST_ID      0x3F  /* NUM_HEAPS <= Some sentinel < 0xFF */

static BigMemList       *BigList;

static HeapIndex RCMemGetHeapIndex( size_t size )
/***********************************************/
{
    HeapIndex   idx;

    for( idx = 0; idx < NUM_HEAPS; idx++ ) {
        if( size < _Layer0to1Size( Heap0Sizes[idx] ) ) {
            return( idx );
        }
    }
    return( BIGLIST_ID );
}

static void FreeBigListNode( void *mem, bool freemem )
/****************************************************/
{
    BigMemList      *travptr;
    BigMemList      *prevnode;
    void            *memptr;
    size_t          headersize;

    headersize = sizeof( BigMemList ) + sizeof( HeapId );
    memptr = (char *)BigList + headersize;
    travptr = BigList->next;
    if( memptr == mem ) {
        if( freemem ) {
            free( BigList );
        }
        BigList = travptr;
        return;
    }
    prevnode = BigList;
    for( travptr = BigList; travptr != NULL; travptr = travptr->next ) {
        memptr = (char *)travptr + headersize;
        if( memptr == mem ) {
            prevnode->next = travptr->next;
#ifdef RCMEM_DEBUG
            if( ((unsigned char *)memptr)[travptr->size] != RCMEM_ENDBYTE ) {
                RcMsgFprintf( NULL, "(%x) Memory Overrun (biglist)\n", mem );
            }
#endif
            if( freemem ) {
                free( travptr );
            }
            break;
        }
        prevnode = travptr;
    }
    if( travptr == NULL ) {
        RcFatalError( ERR_INTERNAL, INTERR_MEM_FREE_FAILED );
    }
}

void RCMemLayer1Init( void )
/**************************/
{
    HeapIndex   idx;

    for( idx = 0; idx < NUM_HEAPS; idx++ ) {
        Heaps[idx] = RCMemLayer0NewHeap( Heap0Sizes[idx], BlocksPerHeap0[idx] );
    }
    BigList = NULL;
}

void *RCMemLayer1Malloc( size_t size )
/************************************/
{
    void            *mem;
    heap_handle     handle;
    HeapIndex       idx;
    size_t          headersize;

    idx = RCMemGetHeapIndex( size );
    if( idx == BIGLIST_ID ) {
        BigMemList  *newmem;

        headersize = sizeof( BigMemList ) + sizeof( HeapId );
#ifdef RCMEM_DEBUG
        newmem = malloc( size + headersize + 1 );
#else
        newmem = malloc( size + headersize );
#endif
        if( newmem == NULL ) {
            RcFatalError( ERR_OUT_OF_MEMORY );
        }
        newmem->size = size;
        newmem->next = BigList;
        BigList = newmem;

        mem = (char *)newmem + headersize;
        HEAPIDX_VAL( mem ) = BIGLIST_ID;
#ifdef RCMEM_DEBUG
        ((unsigned char *)mem)[size] = RCMEM_ENDBYTE;
#endif
    } else {
        void        *newmem;

        handle = Heaps[idx];
#ifdef RCMEM_DEBUG
        newmem = RCMemLayer0Malloc( handle, _Layer1to0Size( size ) );
#else
        newmem = RCMemLayer0Malloc( handle );
#endif
        mem = _Layer0to1Ptr( newmem );
        HEAPIDX_VAL( mem ) = idx;
    }

    return( mem );
}

void RCMemLayer1Free( void *mem )
/*******************************/
{
    HeapIndex      idx;

    if( mem == NULL ) {
        return;
    }
    idx = HEAPIDX_VAL( mem );
    if( idx == BIGLIST_ID ) {
        FreeBigListNode( mem, true );
    } else if( idx < NUM_HEAPS ) {
        RCMemLayer0Free( _Layer1to0Ptr( mem ), Heaps[idx] );
    } else {
        RcFatalError( ERR_INTERNAL, INTERR_MEM_FREE_FAILED );
    }
}

void RCMemLayer1ShutDown( void )
/******************************/
{
    BigMemList      *curnode;
    BigMemList      *nextnode;
    HeapIndex       idx;

    for( idx = 0; idx < NUM_HEAPS; idx++ ) {
        RCMemLayer0ShutDown( Heaps[idx] );
    }
    for( curnode = BigList; curnode != NULL; curnode = nextnode ) {
        nextnode = curnode->next;
#ifdef RCMEM_DEBUG
        RcMsgFprintf( NULL,
                "Unfreed Memory Detected (0x%x bytes at 0x%x)\n",
                 curnode->size, (char *)curnode + sizeof( BigMemList ) );
#endif
        free( curnode );
    }
}

void *RCMemLayer1Realloc( void *mem, size_t size )
/************************************************/
{
    HeapIndex       idx;
    BigMemList      *reallocptr;
    size_t          reallocsize;
    unsigned short  headersize;

    if( mem == NULL ) {     // emulate realloc() behaviour
        return( RCMemLayer1Malloc( size ) );
    }
    idx = HEAPIDX_VAL( mem );
    if( idx == BIGLIST_ID ) {
        BigMemList  *newmem;

        headersize = sizeof( BigMemList ) + sizeof( HeapId );
        reallocptr = (BigMemList *)( (char *)mem - headersize );
        if( reallocptr->size < size ) {
            FreeBigListNode( mem, false );
#ifdef RCMEM_DEBUG
            reallocsize = size + headersize + 1;
#else
            reallocsize = size + headersize;
#endif
            newmem = (BigMemList *)realloc( reallocptr, reallocsize );
            if( newmem == NULL ) {
                RcFatalError( ERR_OUT_OF_MEMORY );
            }
            newmem->size = size;
            newmem->next = BigList;
            BigList = newmem;

            mem = (char *)newmem + headersize;
            HEAPIDX_VAL( mem ) = BIGLIST_ID;
#ifdef RCMEM_DEBUG
            ((unsigned char *)mem)[size] = RCMEM_ENDBYTE;
#endif
            return( mem );
        }
    } else if( idx < NUM_HEAPS ) {
        if( size > _Layer0to1Size( Heap0Sizes[idx] ) ) {
            void    *newmem;

            newmem = RCMemLayer1Malloc( size );
            memcpy( newmem, mem, _Layer0to1Size( Heap0Sizes[idx] ) );
            RCMemLayer1Free( mem );
            return( newmem );
        }
#ifdef RCMEM_DEBUG
        RCMemLayer0Size( _Layer1to0Ptr( mem ), _Layer1to0Size( size ) );
#endif
    } else {
        RcFatalError( ERR_INTERNAL, INTERR_MEM_REALLOC_FAILED );
    }
    return( mem );
}
