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


#include "variety.h"

#if defined(_M_IX86)
#include <i86.h>
#endif

#if (__WATCOMC__ < 900)
 #define __segment      unsigned short
#endif

#if !defined(__DOS_EXT__)
#if defined(__386__) &&                 \
   !defined(__WINDOWS_386__) &&         \
   !defined(__WINDOWS__) &&             \
   !defined(__OS2__) &&                 \
   !defined(__NT__) &&                  \
   !defined(__OSI__) &&                 \
   !defined(__QNX__)
#define __DOS_EXT__
#endif
#endif

typedef unsigned int                tag;
typedef struct freelistp            frl;
typedef struct freelistp _WCNEAR    *frlptr;
typedef struct freelist _WCFAR      *farfrlptr;
typedef unsigned char _WCNEAR       *PTR;
typedef unsigned char _WCFAR        *FARPTR;
typedef struct miniheapblkp _WCNEAR *mheapptr;

/*
** NOTE: the size of these data structures is critical to the alignemnt
**       of the pointers returned by malloc().
*/
struct freelist {
    tag                 len;    /* length of block in free list */
    unsigned int        prev;   /* offset of previous block in free list */
    unsigned int        next;   /* offset of next block in free list */
};
struct heapblk {
    tag                 heaplen;        /* size of heap (0 = 64K) */
    unsigned int        prevseg;        /* segment selector for previous heap */
    unsigned int        nextseg;        /* segment selector for next heap */
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
    unsigned int        prevseg;
    unsigned int        nextseg;
    frlptr              rover;
    unsigned int        b4rover;
    unsigned int        largest_blk;
    unsigned int        numalloc;
    unsigned int        numfree;
    frl                 freehead;
};

struct miniheapblkp {
    tag                 len;
    mheapptr            prev;
    mheapptr            next;
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

#ifdef __DOS_EXT__
struct dpmi_hdr {
    unsigned long       dpmi_handle;
    tag                 dos_seg_value;  // 0 => DPMI block, else DOS segment
};
#endif

extern unsigned                         _curbrk;
extern unsigned char                    __nheap_clean;
extern mheapptr _WCNEAR                 __nheapbeg;
extern unsigned char                    __fheap_clean;
#if defined(_M_IX86)
extern __segment                        __fheap;
extern __segment                        __bheap;
extern __segment                        __fheapRover;
#endif
extern int                              __heap_enabled;
extern unsigned int                     __LargestSizeB4Rover;
extern struct miniheapblkp _WCNEAR      *__MiniHeapRover;
extern unsigned int                     __LargestSizeB4MiniHeapRover;
extern struct miniheapblkp _WCNEAR      *__MiniHeapFreeRover;

extern size_t __LastFree( void );
extern int __NHeapWalk( struct _heapinfo *entry, mheapptr heapbeg );
extern int __ExpandDGROUP( unsigned int __amt );
#if defined(_M_IX86)
extern unsigned __AllocSeg( unsigned int __amt );
extern unsigned __GrowSeg( __segment __seg, unsigned int __amt );
extern int      __FreeSeg( __segment seg );
extern int __HeapWalk( struct _heapinfo *entry, __segment seg, unsigned all );
extern int __HeapMin( __segment seg, unsigned one_seg );
extern int __HeapSet( __segment seg, unsigned fill );
#endif

_WCRTLINK extern void _WCNEAR *__brk( unsigned );

#if defined(__AXP__) || defined(__PPC__)
 #define _DGroup()      0
#else
 #define _DGroup()      FP_SEG((&__nheapbeg))
#endif
// __IsCtsNHeap() is used to determine whether the operating system provides
// a continuous near heap block. __ExpandDGroup should slice for more near
// heap under those operating systems with __IsCtsNHeap() == 1.
#if defined(__WARP__) ||        \
    defined(__NT__) ||          \
    defined(__WINDOWS_386__) || \
    defined(__WINDOWS_286__)
 #define __IsCtsNHeap() 0
#elif defined(__DOS_EXT__)
 #define __IsCtsNHeap() ((_IsRationalZeroBase() || _IsCodeBuilder()) ? 0 : 1)
#else
 #define __IsCtsNHeap() 1
#endif

extern  unsigned __MemAllocator( unsigned __sz, unsigned __seg, unsigned __off);
extern  void     __MemFree( unsigned __ptr, unsigned __seg, unsigned __off );
#if defined(__AXP__) || defined(__PPC__)
 // not needed for alpha
#elif defined(__386__)
 #pragma aux __MemAllocator "*" parm [eax] [edx] [ebx];
 #pragma aux __MemFree      "*" parm [eax] [edx] [ebx];
#else
 #pragma aux __MemAllocator "*" parm [ax] [dx] [bx];
 #pragma aux __MemFree      "*" parm [ax] [dx] [bx];
#endif

#define PARAS_IN_64K    (0x1000)
#define END_TAG         (~0)

#define TAG_SIZE        (sizeof(tag))
#if defined(__386__) || defined(__AXP__) || defined(__PPC__)
    #define ROUND_SIZE  (TAG_SIZE+TAG_SIZE-1)
#elif defined(M_I86)
    #define ROUND_SIZE  (TAG_SIZE-1)
#else
    #error platform not supported
#endif
#define FRL_SIZE        ((sizeof(frl)+ROUND_SIZE)&~ROUND_SIZE)

#define __HM_SUCCESS    0
#define __HM_FAIL       1
#define __HM_TRYGROW    2

