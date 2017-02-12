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


#if defined( __WARP__ )
#define BLKSIZE_ALIGN           0x10000 // 64kB
#else
#define BLKSIZE_ALIGN           0x1000  // 4kB
#endif

#if defined( __DOS_EXT__ )
extern  int SegmentLimit( void );
#pragma aux SegmentLimit        = \
        "xor    eax,eax"        \
        "mov    ax,ds"          \
        "lsl    eax,ax"         \
        "inc    eax"            \
        value                   [eax] \
        modify exact            [eax]
#endif

static frlptr __LinkUpNewMHeap( mheapptr heap1 ) // originally __AddNewHeap()
{
    mheapptr    heap2;
    mheapptr    prev_heap2;
    frlptr      frl;
    unsigned    amount;

    /* insert into ordered heap list */
    /* logic wasn't inserting heaps in proper ascending order */
    prev_heap2 = NULL;
    for( heap2 = __nheapbeg; heap2 != NULL; heap2 = heap2->next ) {
        if( heap1 < heap2 )
            break;
        prev_heap2 = heap2;
    }
    /* ascending order should be: prev_heap2 < heap1 < heap2  */
    /* except for special cases when prev_heap2 and/or heap2 are NULL */
    heap1->prev = prev_heap2;
    heap1->next = heap2;
    if( prev_heap2 != NULL ) {
        prev_heap2->next = heap1;
    } else {            /* add heap1 to beginning of heap */
        __nheapbeg = heap1;
    }
    if( heap2 != NULL ) {
        /* insert before 'heap2' (list is non-empty) */
        heap2->prev = heap1;
    }
    amount = heap1->len - sizeof( miniheapblkp );
    /* Fill out the new miniheap descriptor */
    heap1->freehead.len = 0;
    heap1->freehead.prev = &heap1->freehead;
    heap1->freehead.next = &heap1->freehead;
    heap1->rover = &heap1->freehead;
    heap1->b4rover = 0;
    heap1->numalloc = 0;
    heap1->numfree  = 0;
    frl = (frlptr)( heap1 + 1 );
    frl->len = amount;
    /* fix up end of heap links */
    SET_BLK_END( (frlptr)NEXT_BLK( frl ) );
    return( frl );
}

#if defined( __DOS_EXT__ )

static void __unlink( mheapptr heap )
{
    mheapptr            prev_heap;
    mheapptr            next_heap;

    if( __nheapbeg == heap ) {
        __nheapbeg = heap->next;
    }
    if( heap == __MiniHeapRover ) {
        __MiniHeapRover = heap->prev;
        if( __MiniHeapRover == NULL ) {
            __MiniHeapRover = __nheapbeg;
            __LargestSizeB4MiniHeapRover = 0;
        }
    }
    if( heap == __MiniHeapFreeRover ) {
        __MiniHeapFreeRover = NULL;
    }
    prev_heap = heap->prev;
    next_heap = heap->next;
    if( prev_heap != NULL )
        prev_heap->next = next_heap;
    if( next_heap != NULL ) {
        next_heap->prev = prev_heap;
    }
}

void __FreeDPMIBlocks( void )
{
    mheapptr    heap;
    dpmi_hdr    *dpmi;
    mheapptr    next_heap;

    for( heap = __nheapbeg; heap != NULL; heap = next_heap ) {
        next_heap = heap->next;
        // see if the last free entry has the full size of
        // the DPMI block ( - overhead).  If it is then we can give this
        // DPMI block back to the DPMI host.
        if( heap->freehead.prev->len + sizeof( miniheapblkp ) == heap->len ) {
            dpmi = ((dpmi_hdr *)heap) - 1;
            __unlink( heap );
            if( dpmi->dos_seg_value == 0 ) {    // if DPMI block
                TinyDPMIFree( dpmi->dpmi_handle );
            } else {                            // else DOS block below 1MB
                TinyFreeBlock( dpmi->dos_seg_value );
            }
        }
    }
}

void *__ReAllocDPMIBlock( frlptr old_frl, unsigned req_size )
{
    mheapptr        heap;
    dpmi_hdr        *dpmi;
    dpmi_hdr        *prev_dpmi;
    unsigned        size;
    frlptr          new_frl, frl2;

    if( !__heap_enabled )
        return( 0 );
    __FreeDPMIBlocks();
    prev_dpmi = NULL;
    for( heap = __nheapbeg; heap != NULL; heap = heap->next ) {
        if( ((PTR)heap + sizeof( miniheapblkp ) == (PTR)old_frl) && (heap->numalloc == 1) ) {
            // The mini-heap contains only this memblk
            __unlink( heap );
            dpmi = ((dpmi_hdr *)heap) - 1;
            if( dpmi->dos_seg_value != 0 )
                return( NULL );
            size = __ROUND_UP_SIZE( heap->len + sizeof( dpmi_hdr ) + TAG_SIZE + req_size - old_frl->len + TAG_SIZE, BLKSIZE_ALIGN );
            prev_dpmi = dpmi;
            dpmi = TinyDPMIRealloc( dpmi, size );
            if( dpmi == NULL ) {
                dpmi = prev_dpmi;
                return( NULL );         // indicate resize failed
            }
            dpmi->dos_seg_value = 0;
            heap = (mheapptr)( dpmi + 1 );
            heap->len = size - sizeof( dpmi_hdr ) - TAG_SIZE;
            new_frl = __LinkUpNewMHeap( heap );
            heap->numalloc = 1;

            // round up to even number
            req_size = __ROUND_UP_SIZE( req_size, 2 );
            size = new_frl->len - req_size;
            if( size >= FRL_SIZE ) {    // Enough to spare a free block
                SET_BLK_SIZE_INUSE( new_frl, req_size );// adjust size and set allocated bit
                // Make up a free block at the end
                frl2 = (frlptr)( (PTR)new_frl + req_size );
                SET_BLK_SIZE_INUSE( frl2, size );
                heap->numalloc++;
                heap->largest_blk = 0;
                _nfree( (void _WCNEAR *)BLK2CPTR( frl2 ) );
            } else {
                SET_BLK_INUSE( new_frl );   // set allocated bit
            }
            return( new_frl );
        }
    }
    return( NULL );
}
#endif

#if !( defined( __WINDOWS__ ) || defined( __WARP__ ) || defined( __NT__ ) )
size_t __LastFree( void )    /* used by nheapgrow to know about adjustment */
{
    frlptr      last_frl;
    unsigned    brk_value;

    if( __nheapbeg == NULL ) {      /* no heap? can't have free blocks */
        return( 0 );
    }
    last_frl = __nheapbeg->freehead.prev; /* point to last free block */
    brk_value = BLK2CPTR( NEXT_BLK( last_frl ) );
  #if defined( __DOS_EXT__ )
    if( _IsPharLap() && !_IsFlashTek() )
        _curbrk = SegmentLimit();
  #endif
    if( brk_value == _curbrk ) {    /* if last free block is at the end */
        return( last_frl->len );
    }
    return( 0 );
}
#endif

#if defined( __DOS_EXT__ ) && !defined( __CALL21__ )
static void *RationalAlloc( size_t size )
{
    dpmi_hdr        *dpmi;
    mheapptr        heap;
    tiny_ret_t      save_DOS_block;
    tiny_ret_t      DOS_block;

    __FreeDPMIBlocks();
    /* size is a multiple of 4k */
    dpmi = TinyDPMIAlloc( size );
    if( dpmi != NULL ) {
        heap = (mheapptr)( dpmi + 1 );
        heap->len = size - sizeof( dpmi_hdr );
        dpmi->dos_seg_value = 0;        // indicate DPMI block
        return( (void *)heap );
    }
    if( __minreal & 0xfff00000 ) {
        /* checks for users that want >1M real memory saved */
        __minreal = 0xfffff;
    }
    if( size > 0x00010000 ) {
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
            heap = (mheapptr)( dpmi + 1 );
            heap->len = size - sizeof( dpmi_hdr );
            return( (void *)heap );
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
    unsigned old_amount = *amount;
    unsigned amt;
#if !( defined( __WINDOWS__ ) || defined( __WARP__ ) || defined( __NT__ ) )
    unsigned last_free_amt;
#endif

    amt = old_amount;
    amt = __ROUND_UP_SIZE( amt + TAG_SIZE, ROUND_SIZE );
    if( amt < old_amount ) {
        return( 0 );
    }
#if !( defined( __WINDOWS__ ) || defined( __WARP__ ) || defined( __NT__ ) )
  #if defined( __DOS_EXT__ )
    if( !__IsCtsNHeap() ) {
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
      extra amounts        (22-feb-91 AFS)

       (1) adding a new heap needs:
           frl                    free block req'd for _nmalloc request
                                  (frl is the MINIMUM because the block
                                  may be freed)
           tag                    end of miniheap descriptor
           struct miniheapblkp    start of miniheap descriptor
       (2) extending heap needs:
           tag               free block req'd for _nmalloc request
    */
    *amount = amt;
    amt += ( (TAG_SIZE) + sizeof( freelistp ) + sizeof( miniheapblkp ) );
    if( amt < *amount )
        return( 0 );            // Report request too large
    if( amt < _amblksiz ) {
        /*
          _amblksiz may not be even so round down to an even number
          nb. pathological case: where _amblksiz == 0xffff, we don't
                                 want the usual round up to even
        */
        amt = __ROUND_DOWN_SIZE( _amblksiz, 2 );
    }
#if defined( __WINDOWS_386__ ) || defined( __WARP__ ) || defined( __NT__ ) \
  || defined( __CALL21__ ) || defined( __DOS_EXT__ ) || defined( __RDOS__ )
    /* make sure amount is a multiple of 4k/64k */
    *amount = amt;
    amt = __ROUND_UP_SIZE( amt, BLKSIZE_ALIGN );
    if( amt < *amount )
        return( 0 );
#endif
    *amount = amt;
    return( *amount != 0 );
}

#if defined( __WINDOWS__ ) || defined( __WARP__ ) || defined( __NT__ ) \
  || defined( __CALL21__ ) || defined( __DOS_EXT__ ) || defined( __RDOS__ )
static int __CreateNewNHeap( unsigned amount )
{
    mheapptr        heap;
    frlptr          frl;
    unsigned        brk_value;
  #if defined( __WARP__ )
    ULONG           os2_alloc_flags;
  #endif

    if( !__heap_enabled )
        return( 0 );
    if( _curbrk == ~1u )
        return( 0 );
    if( __AdjustAmount( &amount ) == 0 )
        return( 0 );
  #if defined( __WINDOWS_286__ )
    brk_value = (unsigned)LocalAlloc( LMEM_FIXED, amount );
    if( brk_value == 0 ) {
        return( 0 );
    }
  #elif defined( __WINDOWS_386__ )
    brk_value = (unsigned)DPMIAlloc( amount );
    if( brk_value == 0 ) {
        return( 0 );
    }
  #elif defined( __WARP__ )
    {
        PBYTE           p;
        APIRET          apiret;

        os2_alloc_flags = PAG_COMMIT | PAG_READ | PAG_WRITE;
        if( _os2_obj_any_supported && _os2_use_obj_any ) {
            os2_alloc_flags |= OBJ_ANY;
        }
        apiret = DosAllocMem( (PPVOID)&p, amount, os2_alloc_flags );
        if( apiret )
            return( 0 );

        brk_value = (unsigned)p;
    }
  #elif defined( __NT__ )
    brk_value = (unsigned)VirtualAlloc( NULL, amount, MEM_COMMIT, PAGE_EXECUTE_READWRITE );
    //brk_value = (unsigned) LocalAlloc( LMEM_FIXED, amount );
    if( brk_value == 0 ) {
        return( 0 );
    }
  #elif defined( __CALL21__ )
    {
        tag _WCNEAR *tmp_tag;

        tmp_tag = (tag _WCNEAR *)TinyMemAlloc( amount );
        if( tmp_tag == NULL ) {
            return( 0 );
        }
        /* make sure it will not look like the end of a heap */
        tmp_tag[0] = ! END_TAG;
        brk_value = (unsigned)&tmp_tag[2];
        amount -= 2 * TAG_SIZE; // subtract extra tag
    }
  #elif defined( __DOS_EXT__ )
    // if( !__IsCtsNHeap() ) {
    {
        tag         *tmp_tag;

        if( _IsRational() ) {
            tmp_tag = RationalAlloc( amount );
            if( tmp_tag ) {
                amount = *tmp_tag;
            }
        } else {    /* CodeBuilder */
            tmp_tag = TinyCBAlloc( amount );
            amount -= TAG_SIZE;
        }
        if( tmp_tag == NULL ) {
            return( 0 );
        }
        brk_value = (unsigned)tmp_tag;
    }
    // Pharlap, RSI/non-zero can never call this function
  #elif defined( __RDOS__ )
    brk_value = (unsigned)RdosAllocateMem( amount );
    if( brk_value == 0 ) {
        return( 0 );
    }
  #endif
    if( amount - TAG_SIZE > amount ) {
        return( 0 );
    }
    amount -= TAG_SIZE;
    if( amount < sizeof( miniheapblkp ) + sizeof( freelistp ) ) {
        /* there isn't enough for a heap block (struct miniheapblkp) and
           one free block (frl) */
        return( 0 );
    }
    /* we've got a new heap block */
    heap = (mheapptr)brk_value;
    heap->len = amount;
  #if defined( __WARP__ )
    // Remeber if block was allocated with OBJ_ANY - may be in high memory
    heap->used_obj_any = ( _os2_obj_any_supported && _os2_use_obj_any );
  #endif
    // Now link it up
    frl = __LinkUpNewMHeap( heap );
    amount = frl->len;
    /* build a block for _nfree() */
    SET_BLK_SIZE_INUSE( frl, amount );
    heap->numalloc++;
    heap->largest_blk = 0;
    _nfree( (void _WCNEAR *)BLK2CPTR( frl ) );
    return( 1 );
}
#endif

int __ExpandDGROUP( unsigned amount )
{
#if defined( __WINDOWS__ ) || defined( __WARP__ ) || defined( __NT__ ) \
  || defined( __CALL21__ ) || defined( __RDOS__ )
    // first try to free any available storage
    _nheapshrink();
    return( __CreateNewNHeap( amount ) );
#else
    mheapptr    heap;
    frlptr      frl;
    unsigned    brk_value;
    unsigned    new_brk_value;
    void        _WCNEAR *brk_ret;

  #if defined( __DOS_EXT__ )
    if( !__IsCtsNHeap() ) {
        return( __CreateNewNHeap( amount ) );   // Won't slice either
    }
    // Rational non-zero based system should go through.
  #endif
    if( !__heap_enabled )
        return( 0 );
    if( _curbrk == ~1u )
        return( 0 );
    if( __AdjustAmount( &amount ) == 0 )
        return( 0 );
  #if defined( __DOS_EXT__ )
    if( _IsPharLap() && !_IsFlashTek() ) {
        _curbrk = SegmentLimit();
    }
  #endif
    new_brk_value = amount + _curbrk;
    if( new_brk_value < _curbrk ) {
        new_brk_value = ~1u;
    }
    brk_ret = __brk( new_brk_value );
    if( brk_ret == (void _WCNEAR *)-1 ) {
        return( 0 );
    }
    brk_value = (unsigned)brk_ret;
    if( brk_value >  /*0xfff8*/ ~7u ) {
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
    for( heap = __nheapbeg; heap != NULL; heap = heap->next ) {
        if( heap->next == NULL )
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
        frl = (frlptr)CPTR2BLK( brk_value );
        SET_BLK_END( (frlptr)( (PTR)frl + amount ) );
    } else {
        if( amount < sizeof( miniheapblkp ) + sizeof( freelistp ) ) {
            /*  there isn't enough for a heap block (struct miniheapblkp) and
                one free block (freelistp) */
            return( 0 );
        }
        // Initializing the near heap if __nheapbeg == NULL,
        // otherwise, a new mini-heap is getting linked up
        heap = (mheapptr)brk_value;
        heap->len = amount;
        frl = __LinkUpNewMHeap( heap );
        amount = frl->len;
    }
    /* build a block for _nfree() */
    SET_BLK_SIZE_INUSE( frl, amount );
    heap->numalloc++;
    heap->largest_blk = ~0;    /* set to largest value to be safe */
    _nfree( (void _WCNEAR *)BLK2CPTR( frl ) );
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
    }
}

AXI( _check_os2_obj_any_support, INIT_PRIORITY_PROGRAM - 8 )
#endif
