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
* Description:  Resource compiler memory layer 1.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "rcalloc0.h"
#include "rcalloc1.h"
#include "errors.h"
#include "layer0.h" /* Get consistent definitions for TRUE/FALSE */
#ifdef RCMEM_DEBUG
#include "errprt.h"
#endif

#define NUM_HEAPS 4
const int BlocksPerHeap[] = { 2048, 2048, 2048, 1024 };
const int HeapSizes[] =     {   16,   32,   64, 1024 }; /* Ascending order */
HeapHandle       *Heaps[ NUM_HEAPS ];

// Need a long instead of a char for Alpha to work,
// as Alpha doesn't allow odd boundary memory accesses for non-chars.
// Pls note that there might be problems (for AXP) of this kind lying around.
// In fact, the RCMEM_DEBUG version is not working for AXP.     KH 5/17/95
typedef struct HeapId {
    // char      id;
    long      id;
} HeapId;

typedef struct BigMemList {
    struct BigMemList     *next;
    unsigned long          size;
} BigMemList;

BigMemList       *BigList;

static char RCMemGetHeapIndex( size_t size )
/******************************************/
{
    unsigned char     i;

    for( i = 0; i < NUM_HEAPS; i++ ) {
        if( size + sizeof( HeapId ) < HeapSizes[i] ) {
            return( i );
        }
    }
    return( BIGLIST_ID );
}

static void FreeBigListNode( void *mem, char freemem )
/****************************************************/
{
    BigMemList      *travptr;
    BigMemList      *prevnode;
    unsigned char   *memptr;
    unsigned long   headersize;

    headersize = sizeof( BigMemList ) + sizeof( HeapId );
    memptr = (unsigned char *)BigList + headersize;
    travptr = BigList->next;
    if( memptr == mem ) {
        if( freemem ) {
            free( BigList );
        }
        BigList = travptr;
        return;
    }
    travptr = BigList;
    prevnode = BigList;
    while( travptr != NULL ) {
        memptr = (unsigned char *)travptr + headersize;
        if( memptr == mem ) {
            prevnode->next = travptr->next;
#ifdef RCMEM_DEBUG
            if( *(memptr + travptr->size ) != RCMEM_ENDBYTE ) {
                RcMsgFprintf( stderr, NULL,
                            "(%x) Memory Overrun (biglist)\n", mem );
            }
#endif
            if( freemem ) {
                free( travptr );
            }
            break;
        }
        prevnode = travptr;
        travptr = travptr->next;
    }
    if( travptr == NULL ) {
        RcFatalError( ERR_INTERNAL, INTERR_MEM_FREE_FAILED );
    }
}

extern void RCMemLayer1Init( void )
/*********************************/
{
    int       i;

    for( i = 0; i < NUM_HEAPS; i++ ) {
        Heaps[i] = RCMemLayer0NewHeap( HeapSizes[i], BlocksPerHeap[i] );
    }
    BigList = NULL;
}

extern void *RCMemLayer1Malloc( size_t size )
/*******************************************/
{
    unsigned char   *mem;
    BigMemList      *memptr;
    HeapHandle      *handle;
    HeapId          *idptr;
    unsigned char   heapindex;
    unsigned long   headersize;

    heapindex = RCMemGetHeapIndex( size );
    if( heapindex == BIGLIST_ID ) {

        headersize = sizeof( BigMemList ) + sizeof( HeapId );
#ifdef RCMEM_DEBUG
        memptr = malloc( size + headersize + 1 );
#else
        memptr = malloc( size + headersize );
#endif
        if( memptr == NULL ) {
            RcFatalError( ERR_OUT_OF_MEMORY );
        }
        memptr->size = size;
        memptr->next = BigList;
        idptr = (HeapId *)( (char *)memptr + sizeof( BigMemList ) );
        idptr->id = BIGLIST_ID;
#ifdef RCMEM_DEBUG
        *((unsigned char *)memptr + size + headersize ) = RCMEM_ENDBYTE;
#endif
        mem = (unsigned char *)memptr + headersize;
        BigList = memptr;
    } else {
        handle = Heaps[ heapindex ];
#ifdef RCMEM_DEBUG
        mem = RCMemLayer0Malloc( handle, size + sizeof( HeapId ) );
#else
        mem = RCMemLayer0Malloc( handle );
#endif
        ((HeapId *)mem)->id = heapindex;
        mem += sizeof( HeapId );
    }

    return( mem );
}

extern void RCMemLayer1Free( void *mem )
/**************************************/
{
    char           *blockptr;
    HeapId         *heapid;

    if( mem == NULL ) {
        return;
    }
    blockptr = (char *)mem - sizeof( HeapId );
    heapid = (HeapId *)blockptr;
    if( heapid->id == BIGLIST_ID ) {
        FreeBigListNode( mem, TRUE );
    } else if( heapid->id < NUM_HEAPS ) {
        RCMemLayer0Free( blockptr, Heaps[ heapid->id ] );
    } else {
        RcFatalError( ERR_INTERNAL, INTERR_MEM_FREE_FAILED );
    }
}

extern void RCMemLayer1ShutDown( void )
/*************************************/
{
    BigMemList      *curnode;
    BigMemList      *nextnode;
    int              i;

    for( i = 0; i < NUM_HEAPS; i++ ) {
        RCMemLayer0ShutDown( Heaps[i] );
    }
    curnode = BigList;
    nextnode = BigList;
    while( nextnode != NULL ) {
        nextnode = curnode->next;
#ifdef RCMEM_DEBUG
        RcMsgFprintf( stderr, NULL,
                "Unfreed Memory Detected (0x%x bytes at 0x%x)\n",
                 curnode->size, (char *)curnode + sizeof( BigMemList ) );
#endif
        free( curnode );
        curnode = nextnode;
    }
}

extern void *RCMemLayer1Realloc( void *mem, size_t size )
/*******************************************************/
{
    char             *blockptr;
    BigMemList       *newbigptr;
    void             *newnode;
    HeapId           *heapid;
    HeapId           *idptr;
    BigMemList       *reallocptr;
    unsigned long     reallocsize;
    unsigned short    headersize;
#ifdef RCMEM_DEBUG
    DebugMemInfo     *debugmem;
#endif

    if( mem == NULL ) {     // emulate realloc() behaviour
        return( RCMemLayer1Malloc( size ) );
    }

    blockptr = (char *)mem - sizeof( HeapId );
    heapid = (HeapId *)blockptr;
    if( heapid->id == BIGLIST_ID ) {
        reallocptr = (BigMemList *)( (char *)heapid - sizeof( BigMemList ) );
        if( reallocptr->size < size ) {
            FreeBigListNode( mem, FALSE );
            headersize = sizeof( BigMemList ) + sizeof( HeapId );
            reallocsize = size + headersize;
#ifdef RCMEM_DEBUG
            reallocsize++;
#endif
            newnode = realloc( reallocptr, reallocsize );
            if( newnode == NULL ) {
                RcFatalError( ERR_OUT_OF_MEMORY );
            }
            newbigptr = (BigMemList *)newnode;
            newbigptr->next = BigList;
            newbigptr->size = size;
            idptr = (HeapId *)( (char *)newbigptr + sizeof( BigMemList ) );
            idptr->id = BIGLIST_ID;
            BigList = newbigptr;
#ifdef RCMEM_DEBUG
            *((unsigned char *)newbigptr + headersize + size ) = RCMEM_ENDBYTE;
#endif
            return( (char *)newbigptr + headersize );
        }
    } else if( heapid->id < NUM_HEAPS ) {
        if( size + sizeof( HeapId ) > HeapSizes[ heapid->id ] ) {
            newnode = RCMemLayer1Malloc( size );
            memcpy( newnode, mem, HeapSizes[ heapid->id ] - sizeof( HeapId ) );
            RCMemLayer1Free( mem );
            return( newnode );
        }
#ifdef RCMEM_DEBUG
        else {
            debugmem = (DebugMemInfo *)((char *)blockptr -
                                        sizeof( DebugMemInfo ) );
            debugmem->size = size + sizeof( HeapId );
            *((unsigned char *)mem + size) = RCMEM_ENDBYTE;
        }
#endif
    } else {
        RcFatalError( ERR_INTERNAL, INTERR_MEM_REALLOC_FAILED );
    }
    return( mem );
}
