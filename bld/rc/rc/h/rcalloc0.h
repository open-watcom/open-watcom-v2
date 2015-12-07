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


#ifdef RCMEM_DEBUG

#define RCMEM_STARTBYTE      0x94
#define RCMEM_ENDBYTE        0xA1
#define RCMEM_GARBAGEBYTE    0xE2

typedef struct DebugMemInfo {
    size_t          size;
    unsigned char   startbyte;
} DebugMemInfo;

#endif

typedef struct FreeListInfo {
#ifdef RCMEM_DEBUG
    DebugMemInfo        dbg;
#endif
    union {
        struct FreeListInfo *next;
        unsigned char       data[1];
    } u;
} FreeListInfo;

typedef struct HeapList {
    struct HeapList     *next;
} HeapList;

typedef struct HeapHandle {
    HeapList        *list;
    size_t          heapsize;
    size_t          blocksize;
    FreeListInfo    *freeList;
} HeapHandle;

extern void         RCMemLayer0Free( void *mem, HeapHandle *heap );
extern HeapHandle   *RCMemLayer0NewHeap( size_t heapsize, size_t blocks_per_heap );
extern void         RCMemLayer0ShutDown( HeapHandle *heap );

#ifdef RCMEM_DEBUG
extern void         *RCMemLayer0Malloc( HeapHandle *heap, size_t size );
#else
extern void         *RCMemLayer0Malloc( HeapHandle *heap );
#endif
