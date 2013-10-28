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
* Description:  Internal clib heap management interface.
*
****************************************************************************/


/* NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE
 *
 *  This file must be identical to the file heap.h used by the clib
 *  or the memory management for the large model debugger under dos may fail
 */

#include <dos.h>

typedef unsigned int            tag;
typedef struct freelistp        frl;
typedef struct freelistp __near *frlptr;
typedef struct freelist __far   *farfrlptr;
typedef unsigned char __near    *PTR;
typedef unsigned char __far     *FARPTR;

struct freelist {
    tag                 len;    /* length of block in free list */
    unsigned int        prev;   /* offset of previous block in free list */
    unsigned int        next;   /* offset of next block in free list */
};
struct heapblk {
    tag                 heaplen;        /* size of heap (0 = 64K) */
    unsigned short      prevseg;        /* segment selector for previous heap */
    unsigned short      nextseg;        /* segment selector for next heap */
    unsigned int        rover;          /* roving pointer into free list */
    unsigned int        b4rover;        /* largest block before rover */
    unsigned int        largest_blk;    /* largest block in the heap  */
    unsigned int        numalloc;       /* number of allocated blocks in heap */
    unsigned int        numfree;        /* number of free blocks in the heap */
    struct freelist     freehead;       /* listhead of free blocks in heap */
};

struct freelistp {
    tag                 len;
    frlptr              prev;
    frlptr              next;
};
struct heapblkp {
    tag                 heaplen;
    unsigned short      prevseg;
    unsigned short      nextseg;
    frlptr              rover;
    unsigned int        b4rover;
    unsigned int        largest_blk;
    unsigned int        numalloc;
    unsigned int        numfree;
    frl                 freehead;
};

struct heapstart {
    struct heapblk      h;
    struct freelist     first;
};
struct heapend {
    tag                 last_tag;
    struct freelist     last;
};

extern unsigned _curbrk;
extern frlptr near __nheap;
extern frlptr near __nheapbeg;
extern struct heapblkp near __nheapblk;
extern unsigned short __fheap;

extern unsigned __AllocSeg( unsigned int __amt );
extern unsigned __GrowSeg( unsigned short __seg, unsigned int __amt );
extern unsigned __ShrinkSeg( unsigned short __seg, unsigned int __amt );
extern size_t __LastFree( void );
extern int __ExpandDGROUP( unsigned int __amt );
extern int __FHeapWalk( struct _heapinfo *entry, unsigned seg );
extern int __NHeapWalk( struct _heapinfo *entry, frlptr heapbeg );

extern void near *__brk( unsigned );

#define _DGroup()       FP_SEG((&__nheap))

extern  unsigned __MemAllocator( unsigned __sz, unsigned __seg, unsigned __off);
extern  void     __MemFree( unsigned __ptr, unsigned __seg, unsigned __off );
#ifdef __386__
 #pragma aux __MemAllocator "*" parm [eax] [edx] [ebx];
 #pragma aux __MemFree      "*" parm [eax] [edx] [ebx];
#else
 #pragma aux __MemAllocator "*" parm [ax] [dx] [bx];
 #pragma aux __MemFree      "*" parm [ax] [dx] [bx];
#endif

#define PARAS_IN_64K    (0x1000)
#define ROUND           (sizeof(int)-1)
#define END_TAG         (~0)
