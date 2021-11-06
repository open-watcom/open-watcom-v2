/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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


/*
 * Comments for heap implementation.
 */

#ifdef _M_IX86
#include <i86.h>
#include "extender.h"
#endif


#if !defined(__DOS_EXT__) && defined(__DOS_386__) && !defined(__CALL21__)
#define __DOS_EXT__
#endif

#define FAR2NEAR(t,f)   ((t __near *)(long)(f))

#define BLK2CSTG(f)     ((unsigned)((unsigned)(f) + TAG_SIZE))
#define CSTG2BLK(p)     ((unsigned)((unsigned)(p) - TAG_SIZE))

#if defined( __DOS_EXT__ )
#define DPMI2BLK(h)     ((heapblk_nptr)(h + 1))
#define BLK2DPMI(h)     (((dpmi_hdr *)h) - 1)
#endif

#define TAG_SIZE        (sizeof( tag ))
#if defined( _M_I86 )
    #define HEAP_ROUND_SIZE (TAG_SIZE)
#else
    #define HEAP_ROUND_SIZE (TAG_SIZE + TAG_SIZE)
#endif
#define __ROUND_UP_SIZE_HEAP(s)     __ROUND_UP_SIZE( s + TAG_SIZE, HEAP_ROUND_SIZE )
#define __ROUND_DOWN_SIZE_HEAP(s)   __ROUND_DOWN_SIZE( s - TAG_SIZE, HEAP_ROUND_SIZE )
#define FRL_SIZE                    __ROUND_UP_SIZE( sizeof( freelist ), HEAP_ROUND_SIZE )

#if defined( _M_IX86 )
 #define _DGroup()          _FP_SEG((&__nheapbeg))
#else
 #define _DGroup()          0
#endif

#define __HM_SUCCESS        0
#define __HM_FAIL           1
#define __HM_TRYGROW        2

#define PARAS_IN_64K        (0x1000)
#define END_TAG             (/*0x....ffff*/ ~0U)
#define OVERFLOW_64K(x)     ((x) > 0x10000)

#define GET_BLK_SIZE(p)             ((p)->len & ~1U)
#define IS_BLK_INUSE(p)             (((p)->len & 1) != 0)
#define SET_BLK_SIZE_INUSE(p,s)     (p)->len = ((s) | 1)
#define SET_BLK_INUSE(p)            (p)->len |= 1
#define IS_BLK_END(p)               ((p)->len == END_TAG)
#define SET_BLK_END(p)              (p)->len = END_TAG

#define NEXT_BLK(p)                 ((unsigned)(p) + (p)->len)
#define NEXT_BLK_A(p)               ((unsigned)(p) + GET_BLK_SIZE(p))

#ifdef _M_I86
#define BHEAP(s)            ((heapblk __based(s) *)0)
#define FRLPTR(s)           freelist __based(s) *
#else
#define FRLPTR(s)           freelist_nptr
#endif

#define SET_HEAP_END(s,p)   ((FRLPTR(s))(p))->len = END_TAG; ((FRLPTR(s))(p))->prev.offs = 0

#define IS_IN_HEAP(m,h)     ((unsigned)(h) <= (unsigned)(m) && (unsigned)(m) < (unsigned)NEXT_BLK((h)))

#define memcpy_i86      "shr cx,1"  "rep movsw" "adc cx,cx"   "rep movsb"
#define memcpy_386      "shr ecx,1" "rep movsw" "adc ecx,ecx" "rep movsb"

#define memset_i86      "mov ah,al" "shr cx,1"  "rep stosw" "adc cx,cx"   "rep stosb"
#define memset_386      "mov ah,al" "shr ecx,1" "rep stosw" "adc ecx,ecx" "rep stosb"

/*
** NOTE: the size of these data structures is critical to the alignemnt
**       of the pointers returned by malloc().
*/

typedef unsigned int    tag;

typedef struct heapblk  heapblk;
typedef struct freelist freelist;

typedef heapblk         _WCNEAR *heapblk_nptr;
typedef freelist        _WCNEAR *freelist_nptr;
typedef freelist        _WCFAR *freelist_fptr;

typedef union heapptr {
    __segment           segm;
    heapblk_nptr        nptr;
} heapptr;

typedef union freeptr {
    freelist_nptr       nptr;
    unsigned int        offs;
} freeptr;

struct freelist {
    tag                 len;        /* length of block in free list */
    freeptr             prev;       /* offset of previous block in free list */
    freeptr             next;       /* offset of next block in free list */
};

struct heapblk {
    tag                 len;                /* size of heap (0 = 64K) */
    heapptr             prev;               /* segment selector/offset for previous heap */
    heapptr             next;               /* segment selector/offset for next heap */
    freeptr             rover;              /* roving pointer into free list */
    unsigned int        b4rover;            /* largest block before rover */
    unsigned int        largest_blk;        /* largest block in the heap  */
    unsigned int        numalloc;           /* number of allocated blocks in heap */
    unsigned int        numfree;            /* number of free blocks in the heap */
    freelist            freehead;           /* listhead of free blocks in heap */
#if defined(__OS2__) && !defined(_M_I86)
    unsigned int        used_obj_any :1;    /* allocated with OBJ_ANY - block may be in high memory */
#endif
};

typedef struct heapstart {
    heapblk             h;
    freelist            first;
} heapstart;

typedef struct heapend {
    tag                 last_tag;
    freelist            last;
} heapend;

typedef void            _WCNEAR *void_nptr;
typedef void            _WCFAR *void_fptr;
typedef void            _WCHUGE *void_hptr;

typedef unsigned char   _WCNEAR *PTR;
typedef unsigned char   _WCFAR *FARPTR;

#ifdef _M_I86
typedef heapblk         __based(void) *heap_bptr;
typedef void            __based(void) *void_bptr;
#else
typedef heapblk_nptr    heap_bptr;
typedef void_nptr       void_bptr;
#endif

#ifdef __DOS_EXT__
typedef struct dpmi_hdr {
    unsigned long       dpmi_handle;
    tag                 dos_seg_value;  // 0 => DPMI block, else DOS segment
} dpmi_hdr;
#endif

extern heapblk_nptr     _WCNEAR __nheapbeg;
#if defined( _M_I86 )
extern __segment        __fheapbeg;
extern __segment        __bheapbeg;
extern __segment        __fheapRover;
#endif
extern int              __heap_enabled;
extern unsigned int     __LargestSizeB4Rover;
extern heapblk_nptr     __MiniHeapRover;
extern unsigned int     __LargestSizeB4MiniHeapRover;
extern heapblk_nptr     __MiniHeapFreeRover;

#if defined(__OS2__) && !defined(_M_I86)
extern unsigned char    _os2_use_obj_any;           // Prefer high memory heap block
extern unsigned char    _os2_obj_any_supported;     // DosAllocMem supports OBJ_ANY
#endif

extern size_t           __LastFree( void );
extern int              __NHeapWalk( struct _heapinfo *entry, heapblk_nptr start );
extern int              __ExpandDGROUP( unsigned int __amt );
extern void             __UnlinkNHeap( heapblk_nptr heap, heapblk_nptr prev_heap, heapblk_nptr next_heap );

#if defined( _M_I86 )
extern int              __HeapManager_expand( __segment seg, void_bptr cstg, size_t req_size, size_t *growth_size );
extern  void_bptr       __MemAllocator( unsigned __size, __segment __seg, heap_bptr __heap );
extern  void            __MemFree( void_bptr __cstg, __segment __seg, heap_bptr __heap );
#else
extern int              __HeapManager_expand( void_bptr cstg, size_t req_size, size_t *growth_size );
extern  void_bptr       __MemAllocator( unsigned __size, heap_bptr __heap );
extern  void            __MemFree( void_bptr __cstg, heap_bptr __heap );
#endif
#if defined( _M_I86 )
  #pragma aux __MemAllocator "*" __parm [__ax] [__dx] [__bx]
  #pragma aux __MemFree      "*" __parm [__ax] [__dx] [__bx]
#elif defined( _M_IX86 )
  #pragma aux __MemAllocator "*" __parm [__eax] [__edx]
  #pragma aux __MemFree      "*" __parm [__eax] [__edx]
#endif

#if defined( _M_I86 )
extern __segment        __AllocSeg( unsigned int __amt );
extern int              __GrowSeg( __segment __seg, unsigned int __amt );
extern int              __FreeSeg( __segment __seg );
extern int              __HeapWalk( struct _heapinfo *entry, __segment seg, __segment one_heap );
extern int              __HeapMin( __segment __seg, __segment one_heap );
extern int              __HeapSet( __segment __seg, unsigned fill );
extern void             _WCFAR __HeapInit( heapblk_nptr start, unsigned int amount );
#endif

#if defined( __DOS_EXT__ )
extern void             *__ReAllocDPMIBlock( freelist_nptr p1, unsigned req_size );
extern void             *__ExpandDPMIBlock( freelist_nptr, unsigned );
#endif

#if defined(__OS2__) && !defined(_M_I86) || defined(__WINDOWS__) || defined(__NT__) || \
    defined(__CALL21__) || defined(__RDOS__) || defined(__DOS_EXT__)
extern int              __nheapshrink( void );
#endif

#if defined( __QNX__ )
extern void             __setcbrk( unsigned offset );
#endif
