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
* Description:  Heap library configuration for various platforms.
*
****************************************************************************/


#ifdef _M_IX86
#include <i86.h>
#include "extender.h"
#endif

#ifdef _M_I86
#define BASED_SEG_PTR(s)    __based(s) *
#define BASED_VOID_PTR      __based(void) *
#else
#define BASED_SEG_PTR(s)    _WCNEAR *
#define BASED_VOID_PTR      _WCNEAR *
#endif

typedef unsigned int    tag;
typedef unsigned char   _WCNEAR *PTR;
typedef unsigned char   _WCFAR *FARPTR;

/*
** NOTE: the size of these data structures is critical to the alignemnt
**       of the pointers returned by malloc().
*/
typedef struct freelist {
    tag                 len;    /* length of block in free list */
    unsigned int        prev;   /* offset of previous block in free list */
    unsigned int        next;   /* offset of next block in free list */
} freelist;

typedef freelist        _WCFAR *farfrlptr;

typedef struct heapblk {
    tag                 heaplen;        /* size of heap (0 = 64K) */
#if defined( _M_I86 )
    __segment           prevseg;        /* segment selector for previous heap */
    __segment           nextseg;        /* segment selector for next heap */
#elif defined( _M_IX86 )
    __segment           prevseg;        /* segment selector for previous heap */
    __segment           dummy1;         /* not used, match miniheapblkp size */
    __segment           nextseg;        /* segment selector for next heap */
    __segment           dummy2;         /* not used, match miniheapblkp size */
#else
    unsigned int        dummy1;         /* not used, match miniheapblkp size */
    unsigned int        dummy2;         /* not used, match miniheapblkp size */
#endif
    unsigned int        rover;          /* roving pointer into free list */
    unsigned int        b4rover;        /* largest block before rover */
    unsigned int        largest_blk;    /* largest block in the heap  */
    unsigned int        numalloc;       /* number of allocated blocks in heap */
    unsigned int        numfree;        /* number of free blocks in the heap */
    freelist            freehead;       /* listhead of free blocks in heap */
#if defined(__WARP__)
    unsigned int        spare;          /* not used, match miniheapblkp size */
#endif
} heapblk;

typedef struct freelistp {
    tag                 len;
    struct freelistp    _WCNEAR *prev;
    struct freelistp    _WCNEAR *next;
} freelistp;

typedef freelistp       frl;
typedef freelistp       _WCNEAR *frlptr;

typedef struct heapblkp {
    tag                 heaplen;
#if defined( _M_I86 )
    __segment           prevseg;        /* segment selector for previous heap */
    __segment           nextseg;        /* segment selector for next heap */
#elif defined( _M_IX86 )
    __segment           prevseg;        /* segment selector for previous heap */
    __segment           dummy1;         /* not used, match miniheapblkp size */
    __segment           nextseg;        /* segment selector for next heap */
    __segment           dummy2;         /* not used, match miniheapblkp size */
#else
    unsigned int        dummy1;         /* not used, match miniheapblkp size */
    unsigned int        dummy2;         /* not used, match miniheapblkp size */
#endif
    frlptr              rover;
    unsigned int        b4rover;
    unsigned int        largest_blk;
    unsigned int        numalloc;
    unsigned int        numfree;
    frl                 freehead;
#if defined(__WARP__)
    unsigned int        spare;          /* not used, match miniheapblkp size */
#endif
} heapblkp;

typedef struct miniheapblkp {
    tag                 len;
    struct miniheapblkp _WCNEAR *prev;
    struct miniheapblkp _WCNEAR *next;
    frlptr              rover;
    unsigned int        b4rover;
    unsigned int        largest_blk;
    unsigned int        numalloc;
    unsigned int        numfree;
    frl                 freehead;
#if defined(__WARP__)
    unsigned int        used_obj_any    :1; /* allocated with OBJ_ANY - block may be in high memory */
#endif
} miniheapblkp;

typedef miniheapblkp    _WCNEAR *mheapptr;

typedef struct heapstart {
    heapblk             h;
    freelist            first;
} heapstart;

typedef struct heapend {
    tag                 last_tag;
    freelist            last;
} heapend;

#ifdef __DOS_EXT__
typedef struct dpmi_hdr {
    unsigned long       dpmi_handle;
    tag                 dos_seg_value;  // 0 => DPMI block, else DOS segment
} dpmi_hdr;
#endif

extern mheapptr         _WCNEAR __nheapbeg;
#if defined(_M_IX86)
extern __segment        __fheap;
extern __segment        __bheap;
extern __segment        __fheapRover;
#endif
extern int              __heap_enabled;
extern unsigned int     __LargestSizeB4Rover;
extern miniheapblkp     _WCNEAR *__MiniHeapRover;
extern unsigned int     __LargestSizeB4MiniHeapRover;
extern miniheapblkp     _WCNEAR *__MiniHeapFreeRover;

extern size_t           __LastFree( void );
extern int              __NHeapWalk( struct _heapinfo *entry, mheapptr heapbeg );
extern int              __ExpandDGROUP( unsigned int __amt );
#if defined(_M_IX86)
extern __segment        __AllocSeg( unsigned int __amt );
extern int              __GrowSeg( __segment __seg, unsigned int __amt );
extern int              __FreeSeg( __segment __seg );
extern int              __HeapWalk( struct _heapinfo *entry, __segment __seg, unsigned all );
extern int              __HeapMin( __segment __seg, unsigned one_seg );
extern int              __HeapSet( __segment __seg, unsigned fill );
#endif

#if defined(__DOS_EXT__)
extern void             __FreeDPMIBlocks( void );
extern void             *__ReAllocDPMIBlock( frlptr p1, unsigned req_size );
extern void             *__ExpandDPMIBlock( frlptr, unsigned );
#endif

extern int              __HeapManager_expand( __segment seg, unsigned offset,
                            size_t req_size, size_t *growth_size );

#if defined(_M_I86)
extern void             _WCFAR __HeapInit( void _WCNEAR *start, unsigned int amount );
#endif

extern void             _WCNEAR *__brk( unsigned );

#if defined(_M_IX86)
 #define _DGroup()      FP_SEG((&__nheapbeg))
#else
 #define _DGroup()      0
#endif
// __IsCtsNHeap() is used to determine whether the operating system provides
// a continuous near heap block. __ExpandDGroup should slice for more near
// heap under those operating systems with __IsCtsNHeap() == 1.
#if defined(__WARP__) || defined(__NT__) || defined(__WINDOWS__) || defined(__RDOS__)
 #define __IsCtsNHeap() (0)
#elif defined(__DOS_EXT__)
 #define __IsCtsNHeap() (!(_IsRationalZeroBase() || _IsCodeBuilder()))
#else
 #define __IsCtsNHeap() (1)
#endif

extern  unsigned        __MemAllocator( unsigned __sz, __segment __seg, unsigned __off );
extern  void            __MemFree( unsigned __ptr, __segment __seg, unsigned __off );
#if defined(_M_IX86)
 #if defined(_M_I86)
  #pragma aux __MemAllocator "*" parm [ax] [dx] [bx];
  #pragma aux __MemFree      "*" parm [ax] [dx] [bx];
 #else
  #pragma aux __MemAllocator "*" parm [eax] [dx] [ebx];
  #pragma aux __MemFree      "*" parm [eax] [dx] [ebx];
 #endif
#endif

#define PARAS_IN_64K    (0x1000)
#define END_TAG         (~0)

#define TAG_SIZE        (sizeof(tag))
#if defined( _M_I86 )
    #define ROUND_SIZE  (TAG_SIZE-1)
#else
    #define ROUND_SIZE  (TAG_SIZE+TAG_SIZE-1)
#endif
#define FRL_SIZE        ((sizeof(frl)+ROUND_SIZE)&~ROUND_SIZE)

#define __HM_SUCCESS    0
#define __HM_FAIL       1
#define __HM_TRYGROW    2

#if defined(__WARP__)
extern unsigned char    _os2_use_obj_any;           // Prefer high memory heap block
extern unsigned char    _os2_obj_any_supported;     // DosAllocMem supports OBJ_ANY
#endif
