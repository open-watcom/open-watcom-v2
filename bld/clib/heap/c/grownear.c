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
* Description:  Heap growing routines - allocate near heap memory from OS.
*
****************************************************************************/


/*
 * Comments for heap implementation.
 *
 * brk value should be pointer to the start/end of heap blok (heap structure) ???
 *
 * CALL21 memory block have 2 * TAG reserved on the begining, heap block start after this ??? need review
 * DPMI memory block have dpmi_hdr reserved on the begining, heap block start after this
 *
 */

#include "dll.h"        // needs to be first
#include "variety.h"
#include <stddef.h>
#include <stdlib.h>
#include <malloc.h>
#if defined( __WINDOWS_286__ ) || defined( __NT__ )
  #include <windows.h>
#elif defined( __WINDOWS_386__ )
  #include "windpmi.h"
#elif defined( __OS2__ )
  #include <wos2.h>
#elif defined( __RDOS__ )
  #include <rdos.h>
#endif
#include "rtdata.h"
#if defined( __WARP__ )
  #include "rtinit.h"
#endif
#include "heapacc.h"
#include "heap.h"
#if defined( __DOS_EXT__ ) || defined( __CALL21__ )
  #include "tinyio.h"
#endif


#define FIRST_FRL(h)    ((freelist_nptr)(h + 1))
#define FRLPTRADD(p,o)  (freelist_nptr)((PTR)(p)+(o))

#if defined( __DOS_EXT__ )
extern  int SegmentLimit( void );
#pragma aux SegmentLimit = \
        "xor    eax,eax"    \
        "mov    ax,ds"      \
        "lsl    eax,ax"     \
        "inc    eax"        \
    __parm              [] \
    __value             [__eax] \
    __modify __exact    [__eax]
#endif

static freelist_nptr __LinkUpNewNHeap( heapblk_nptr heap )
{
    heapblk_nptr    heap2;
    heapblk_nptr    prev_heap2;

    /* insert into ordered heap list */
    /* ascending order should be: prev_heap2 < heap < heap2  */
    /* exceptions for special cases when prev_heap2 and/or heap2 are NULL */
    prev_heap2 = NULL;
    for( heap2 = __nheapbeg; heap2 != NULL; heap2 = heap2->next.nptr ) {
        if( heap < heap2 ) {
            /* insert before 'heap2' (list is non-empty) */
            heap2->prev.nptr = heap;
            break;
        }
        prev_heap2 = heap2;
    }
    heap->prev.nptr = prev_heap2;
    heap->next.nptr = heap2;
    if( prev_heap2 != NULL ) {
        prev_heap2->next.nptr = heap;
    } else {
        /* add heap to beginning of heap list */
        __nheapbeg = heap;
    }
    /* Fill out the new miniheap descriptor */
    heap->freehead.len = 0;
    heap->freehead.prev.nptr = &heap->freehead;
    heap->freehead.next.nptr = &heap->freehead;
    heap->rover.nptr = &heap->freehead;
    heap->b4rover = 0;
    heap->numalloc = 0;
    heap->numfree  = 0;
    FIRST_FRL( heap )->len = heap->len - sizeof( heapblk );
    /* fix up end of heap links */
    SET_BLK_END( (freelist_nptr)NEXT_BLK( FIRST_FRL( heap ) ) );
    return( FIRST_FRL( heap ) );
}

#if defined( __DOS_EXT__ )

void_nptr __ReAllocDPMIBlock( freelist_nptr frl_old, unsigned req_size )
{
    heapblk_nptr    heap;
    dpmi_hdr        *dpmi;
//    dpmi_hdr        *prev_dpmi;
    unsigned        size;
    freelist_nptr   frl_new, frl2;

    if( !__heap_enabled )
        return( NULL );
    __nheapshrink();
//    prev_dpmi = NULL;
    for( heap = __nheapbeg; heap != NULL; heap = heap->next.nptr ) {
        if( ((PTR)heap + sizeof( heapblk ) == (PTR)frl_old) && (heap->numalloc == 1) ) {
            heapblk_nptr    prev_heap;
            heapblk_nptr    next_heap;

            // The mini-heap contains only this memblk
            dpmi = BLK2DPMI( heap );
            if( dpmi->dos_seg_value != 0 )
                return( NULL );
            prev_heap = heap->prev.nptr;
            next_heap = heap->next.nptr;
            size = __ROUND_UP_SIZE_4K( heap->len + sizeof( dpmi_hdr ) + TAG_SIZE + req_size - frl_old->len + TAG_SIZE );
//            prev_dpmi = dpmi;
            dpmi = TinyDPMIRealloc( dpmi, size );
            if( dpmi == NULL ) {
//                dpmi = prev_dpmi;
                return( NULL );         // indicate resize failed
            }
            dpmi->dos_seg_value = 0;
            __UnlinkNHeap( heap, prev_heap, next_heap );
            heap = DPMI2BLK( dpmi );
            heap->len = size - sizeof( dpmi_hdr ) - TAG_SIZE;
            frl_new = __LinkUpNewNHeap( heap );
            heap->numalloc = 1;

            req_size = __ROUND_UP_SIZE( req_size, 2 );  /* make even */
            size = frl_new->len - req_size;
            if( size >= FRL_SIZE ) {    // Enough to spare a free block
                SET_BLK_SIZE_INUSE( frl_new, req_size );// adjust size and set allocated bit
                // Make up a free block at the end
                frl2 = FRLPTRADD( frl_new, req_size );
                SET_BLK_SIZE_INUSE( frl2, size );
                heap->numalloc++;
                heap->largest_blk = 0;
                _nfree( (void_nptr)BLK2CPTR( frl2 ) );
            } else {
                SET_BLK_INUSE( frl_new );   // set allocated bit
            }
            return( frl_new );
        }
    }
    return( NULL );
}
#endif

#if !( defined( __WINDOWS__ ) || defined( __WARP__ ) || defined( __NT__ ) )
size_t __LastFree( void )    /* used by nheapgrow to know about adjustment */
{
    freelist_nptr   frl_last;
    unsigned        brk_value;

    if( __nheapbeg == NULL ) {      /* no heap? can't have free blocks */
        return( 0 );
    }
    frl_last = __nheapbeg->freehead.prev.nptr; /* point to last free block */
    brk_value = BLK2CPTR( NEXT_BLK( frl_last ) );
  #if defined( __DOS_EXT__ )
    if( _IsPharLap() && !_IsFlashTek() )
        _curbrk = SegmentLimit();
  #endif
    if( brk_value == _curbrk ) {    /* if last free block is at the end */
        return( frl_last->len );
    }
    return( 0 );
}
#endif

#if defined( __DOS_EXT__ )
static heapblk_nptr RationalAlloc( size_t size )
{
    dpmi_hdr        *dpmi;
    heapblk_nptr    heap;
    tiny_ret_t      save_DOS_block;
    tiny_ret_t      DOS_block;

    __nheapshrink();
    /* size is a multiple of 4k */
    dpmi = TinyDPMIAlloc( size );
    if( dpmi != NULL ) {
        dpmi->dos_seg_value = 0;        // indicate DPMI block
        heap = DPMI2BLK( dpmi );
        heap->len = size - sizeof( dpmi_hdr );
        return( heap );
    }
    if( __minreal & 0xfff00000 ) {
        /* checks for users that want >1M real memory saved */
        __minreal = 0xfffff;
    }
    if( OVERFLOW_64K( size ) ) {
        /* cannot allocate more than 64k from DOS real memory */
        return( NULL );
    }
    save_DOS_block = TinyAllocBlock( __ROUND_DOWN_SIZE_TO_PARA( __minreal ) | 1 );
    if( TINY_OK( save_DOS_block ) ) {
        DOS_block = TinyAllocBlock( __ROUND_DOWN_SIZE_TO_PARA( size ) );
        TinyFreeBlock( save_DOS_block );
        if( TINY_OK( DOS_block ) ) {
            dpmi = (dpmi_hdr *)TinyDPMIBase( DOS_block );
            dpmi->dos_seg_value = DOS_block;
            heap = DPMI2BLK( dpmi );
            heap->len = size - sizeof( dpmi_hdr );
            return( heap );
        }
    }
    return( NULL );
}
#endif

/**
 * Adjust block to match platform minimums and maximums
 * @return 1 if adjust OK, 0 if adjustment fails
 */

static int __AdjustAmount( unsigned *amount )
{
    unsigned old_amount;
    unsigned amt;
#if !( defined( __WINDOWS__ ) || defined( __WARP__ ) || defined( __NT__ ) )
    unsigned last_free_amt;
#endif

    old_amount = *amount;
    amt = __ROUND_UP_SIZE_HEAP( old_amount );
    if( amt < old_amount ) {
        return( 0 );
    }
#if !( defined( __WINDOWS__ ) || defined( __WARP__ ) || defined( __NT__ ) )
  #if defined( __DOS_EXT__ )
    if( _IsRationalZeroBase() || _IsCodeBuilder() ) {
        // Allocating extra to identify the dpmi block
        amt += sizeof( dpmi_hdr );
    } else {
  #endif
        last_free_amt = __LastFree();   /* adjust for last free block */
        if( last_free_amt >= amt ) {
            amt = 0;
        } else {
            amt -= last_free_amt;
        }
  #if defined( __DOS_EXT__ )
    }
  #endif
#endif
    /* amount is even here */
    /*
      extra amounts

       (1) adding a new heap needs:
           frl              free block req'd for _nmalloc request
                              (frl is the MINIMUM because the block
                               may be freed)
           tag              end of miniheap descriptor
           struct heapblk   start of miniheap descriptor
       (2) extending heap needs:
           tag              free block req'd for _nmalloc request
    */
    old_amount = amt;
    amt += ( (TAG_SIZE) + sizeof( freelist ) + sizeof( heapblk ) );
    if( amt < old_amount )
        return( 0 );            // Report request too large
    if( amt < _RWD_amblksiz ) {
        /*
          _RWD_amblksiz may not be even so round down to an even number
          nb. pathological case: where _RWD_amblksiz == 0xffff, we don't
                                 want the usual round up to even
        */
        amt = __ROUND_DOWN_SIZE( _RWD_amblksiz, 2 );
    }
#if defined( __WARP__ )
    /* make sure amount is a multiple of 64k */
    old_amount = amt;
    amt = __ROUND_UP_SIZE_64K( amt );
    if( amt < old_amount )
        return( 0 );
#elif defined( __WINDOWS_386__ ) || defined( __NT__ ) || defined( __CALL21__ ) \
        || defined( __DOS_EXT__ ) || defined( __RDOS__ )
    /* make sure amount is a multiple of 4k */
    old_amount = amt;
    amt = __ROUND_UP_SIZE_4K( amt );
    if( amt < old_amount )
        return( 0 );
#endif
    *amount = amt;
    return( amt != 0 );
}

#if defined( __WINDOWS__ ) || defined( __WARP__ ) || defined( __NT__ ) \
  || defined( __CALL21__ ) || defined( __DOS_EXT__ ) || defined( __RDOS__ )

static heapblk_nptr __GetMemFromSystem( unsigned *amount )
{
    unsigned    brk_value = 0;

  #if defined( __WINDOWS_286__ )
    brk_value = (unsigned)LocalAlloc( LMEM_FIXED, *amount );
  #elif defined( __WINDOWS_386__ )
    brk_value = (unsigned)DPMIAlloc( *amount );
  #elif defined( __WARP__ )
    {
        PBYTE           p;
        ULONG           os2_alloc_flags;

        os2_alloc_flags = PAG_COMMIT | PAG_READ | PAG_WRITE;
        if( _os2_obj_any_supported && _os2_use_obj_any ) {
            os2_alloc_flags |= OBJ_ANY;
        }
        if( DosAllocMem( (PPVOID)&p, *amount, os2_alloc_flags ) == 0 ) {
            brk_value = (unsigned)p;
        }
    }
  #elif defined( __NT__ )
    brk_value = (unsigned)VirtualAlloc( NULL, *amount, MEM_COMMIT, PAGE_EXECUTE_READWRITE );
    //brk_value = (unsigned) LocalAlloc( LMEM_FIXED, *amount );
  #elif defined( __CALL21__ )
    brk_value = (unsigned)TinyMemAlloc( *amount );
    if( brk_value != 0 ) {
        /* make sure it will not look like the end of a heap */
        *(tag _WCNEAR *)brk_value = !END_TAG;
        brk_value += 2 * TAG_SIZE;
        *amount -= 2 * TAG_SIZE;    // subtract extra tag
    }
  #elif defined( __DOS_EXT__ )
    if( _IsRationalZeroBase() ) {
        brk_value = (unsigned)RationalAlloc( *amount );
        if( brk_value != 0 ) {
            *amount = ((heapblk_nptr)brk_value)->len;
        }
    } else if( _IsCodeBuilder() ) {
        brk_value = (unsigned)TinyCBAlloc( *amount );
        if( brk_value != 0 ) {
            *amount -= TAG_SIZE;
        }
//    } else {
        // Pharlap, RSI/non-zero should never call this function
    }
  #elif defined( __RDOS__ )
    brk_value = (unsigned)RdosAllocateMem( *amount );
  #endif
    return( (heapblk_nptr)brk_value );
}

static int __CreateNewNHeap( unsigned amount )
{
    heapblk_nptr    heap;
    freelist_nptr   frl;

    // first try to free any available storage
    __nheapshrink();
    heap = __GetMemFromSystem( &amount );
    if( heap == NULL ) {
        return( 0 );
    }
    /* we've got a new heap block */
    heap->len = amount - TAG_SIZE;
  #if defined( __WARP__ )
    // Remeber if block was allocated with OBJ_ANY - may be in high memory
    heap->used_obj_any = ( _os2_obj_any_supported && _os2_use_obj_any );
  #endif
    // Now link it up
    frl = __LinkUpNewNHeap( heap );
    /* build a block for _nfree() */
    SET_BLK_INUSE( frl );
    heap->numalloc++;
    heap->largest_blk = 0;
    _nfree( (void_nptr)BLK2CPTR( frl ) );
    return( 1 );
}
#endif

int __ExpandDGROUP( unsigned amount )
{
#if defined( __WINDOWS__ ) || defined( __WARP__ ) || defined( __NT__ ) \
  || defined( __CALL21__ ) || defined( __RDOS__ )
#else
    heapblk_nptr    heap;
    freelist_nptr   frl;
    unsigned        brk_value;
    unsigned        new_brk_value;
#endif

    if( !__heap_enabled )
        return( 0 );
    if( _curbrk == /*0x....fffe*/ ~1U )
        return( 0 );
    if( __AdjustAmount( &amount ) == 0 )
        return( 0 );
#if defined( __WINDOWS__ ) || defined( __WARP__ ) || defined( __NT__ ) \
  || defined( __CALL21__ ) || defined( __RDOS__ )
    return( __CreateNewNHeap( amount ) );
#else
  #if defined( __DOS_EXT__ )
    if( _IsRationalZeroBase() || _IsCodeBuilder() ) {
        return( __CreateNewNHeap( amount ) );
    } else if( _IsPharLap() && !_IsFlashTek() ) {
        _curbrk = SegmentLimit();
//    } else {
        // Rational non-zero based system should go through.
    }
  #endif
    new_brk_value = amount + _curbrk;
    if( new_brk_value < _curbrk ) {
        new_brk_value = /*0x....fffe*/ ~1U;
    }
    brk_value = (unsigned)__brk( new_brk_value );
    if( brk_value == /*0x....ffff*/ ~0U ) {
        return( 0 );
    }
    if( brk_value >  /*0x....fff8*/ ~7U ) {
        return( 0 );
    }
    if( new_brk_value <= brk_value ) {
        return( 0 );
    }
    amount = new_brk_value - brk_value;
    if( amount - TAG_SIZE > amount ) {
        return( 0 );
    }
    amount -= TAG_SIZE;
    for( heap = __nheapbeg; heap != NULL; heap = heap->next.nptr ) {
        if( heap->next.nptr == NULL )
            break;
        if( (unsigned)heap <= brk_value && BLK2CPTR( NEXT_BLK( heap ) ) >= brk_value ) {
            break;
        }
    }
    if( ( heap != NULL ) && CPTR2BLK( brk_value ) == NEXT_BLK( heap ) ) {
        /* we are extending the previous heap block (slicing) */
        /* nb. account for the end-of-heap tag */
        amount += TAG_SIZE;
        /* adjust current entry in heap list */
        heap->len += amount;
        /* fix up end of heap links */
        frl = (freelist_nptr)CPTR2BLK( brk_value );
        frl->len = amount;
        SET_BLK_END( (freelist_nptr)NEXT_BLK( frl ) );
    } else {
        if( amount < sizeof( heapblk ) + sizeof( freelist ) ) {
            /* there isn't enough for a heap block (struct heapblk) and
               one free block (freelist) */
            return( 0 );
        }
        // Initializing the near heap if __nheapbeg == NULL,
        // otherwise, a new mini-heap is getting linked up
        heap = (heapblk_nptr)brk_value;
        heap->len = amount;
        frl = __LinkUpNewNHeap( heap );
    }
    /* build a block for _nfree() */
    SET_BLK_INUSE( frl );
    heap->numalloc++;
    heap->largest_blk = /*0x....ffff*/ ~0U;     /* set to largest value to be safe */
    _nfree( (void_nptr)BLK2CPTR( frl ) );
    return( 1 );
#endif
}

#if defined( __WARP__ )
unsigned char _os2_obj_any_supported = FALSE;

static void _check_os2_obj_any_support( void )
{
    PBYTE           p;
    APIRET          apiret;

    _os2_obj_any_supported = TRUE;
    apiret = DosAllocMem( (PPVOID)&p, 1, PAG_COMMIT | PAG_READ | OBJ_ANY );
    if( apiret == ERROR_INVALID_PARAMETER ) {
        _os2_obj_any_supported = FALSE;
    } else if( apiret == 0 ) {
        DosFreeMem( p );
    }
}

AXI( _check_os2_obj_any_support, INIT_PRIORITY_PROGRAM - 8 )
#endif
