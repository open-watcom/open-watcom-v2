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
#if defined(__WINDOWS_286__) || defined(__NT__)
  #include <windows.h>
#elif defined(__WINDOWS_386__)
  #include "windpmi.h"
#elif defined(__OS2__)
  #include <wos2.h>
#elif defined(__RDOS__)
  #include <rdos.h>
#endif
#include "rtdata.h"
#if defined(__WARP__)
  #include "rtinit.h"
#endif
#include "heapacc.h"
#include "heap.h"
#if defined(__DOS_EXT__) || defined(__CALL21__)
  #include "tinyio.h"
#endif


#if defined(__WARP__)
#define BLKSIZE_ALIGN_MASK      0xFFFF  // 64kB
#else
#define BLKSIZE_ALIGN_MASK      0x0FFF  // 4kB
#endif

static frlptr __LinkUpNewMHeap( mheapptr );

#if defined(__DOS_EXT__)

extern  int SegmentLimit( void );
#pragma aux SegmentLimit        = \
        "xor    eax,eax"        \
        "mov    ax,ds"          \
        "lsl    eax,ax"         \
        "inc    eax"            \
        value                   [eax] \
        modify exact            [eax];

static void __unlink( mheapptr miniheapptr )
{
    mheapptr            prev_link;
    mheapptr            next_link;

    if( __nheapbeg == miniheapptr ) {
        __nheapbeg = miniheapptr->next;
    }
    if( miniheapptr == __MiniHeapRover ) {
        __MiniHeapRover = miniheapptr->prev;
        if( __MiniHeapRover == NULL ) {
            __MiniHeapRover = __nheapbeg;
            __LargestSizeB4MiniHeapRover = 0;
        }
    }
    if( miniheapptr == __MiniHeapFreeRover ) {
        __MiniHeapFreeRover = 0;
    }
    prev_link = miniheapptr->prev;
    next_link = miniheapptr->next;
    if( prev_link != NULL )
        prev_link->next = next_link;
    if( next_link != NULL ) {
        next_link->prev = prev_link;
    }
}

void __FreeDPMIBlocks( void )
{
    mheapptr    mhp;
    dpmi_hdr    *dpmi;

    mhp = __nheapbeg;
    while( mhp != NULL ) {
        // see if the last free entry has the full size of
        // the DPMI block ( - overhead).  If it is then we can give this
        // DPMI block back to the DPMI host.
        if( (mhp->freehead.prev)->len + sizeof( miniheapblkp ) == mhp->len ) {
            mheapptr    pnext;

            dpmi = ((dpmi_hdr *)mhp) - 1;
            pnext = mhp->next;
            __unlink( mhp );
            mhp = pnext;
            if( dpmi->dos_seg_value == 0 ) {    // if DPMI block
                TinyDPMIFree( dpmi->dpmi_handle );
            } else {                            // else DOS block below 1MB
                TinyFreeBlock( dpmi->dos_seg_value );
            }
        } else {
            mhp = mhp->next;
        }
    }
}

void *__ReAllocDPMIBlock( frlptr p1, unsigned req_size )
{
    mheapptr        mhp;
    dpmi_hdr        *dpmi;
    dpmi_hdr        *prev_dpmi;
    unsigned        size;
    frlptr          flp, flp2;

    if( !__heap_enabled )
        return( 0 );
    __FreeDPMIBlocks();
    prev_dpmi = NULL;
    for( mhp = __nheapbeg; mhp; mhp = mhp->next ) {
        if( ((PTR)mhp + sizeof( miniheapblkp ) == (PTR)p1) && (mhp->numalloc == 1) ) {
            // The mini-heap contains only this memblk
            __unlink( mhp );
            dpmi = ((dpmi_hdr *)mhp) - 1;
            if( dpmi->dos_seg_value != 0 )
                return( NULL );
            size = mhp->len + sizeof( dpmi_hdr ) + TAG_SIZE;
            size += ( req_size - (p1->len - TAG_SIZE) );
            size += BLKSIZE_ALIGN_MASK;
            size &= ~BLKSIZE_ALIGN_MASK;
            prev_dpmi = dpmi;
            dpmi = TinyDPMIRealloc( dpmi, size );
            if( dpmi == NULL ) {
                dpmi = prev_dpmi;
                return( NULL );         // indicate resize failed
            }
            dpmi->dos_seg_value = 0;
            mhp = (mheapptr)( dpmi + 1 );
            mhp->len = size - sizeof( dpmi_hdr ) - TAG_SIZE;
            flp = __LinkUpNewMHeap( mhp );
            mhp->numalloc = 1;

            // round up to even number
            req_size = (req_size + 1) & ~1;
            size = flp->len - req_size;
            if( size >= FRL_SIZE ) {    // Enough to spare a free block
                flp->len = req_size | 1;// adjust size and set allocated bit
                // Make up a free block at the end
                flp2 = (frlptr)((PTR)flp + req_size);
                flp2->len = size | 1;
                ++mhp->numalloc;
                mhp->largest_blk = 0;
                _nfree( (PTR)flp2 + TAG_SIZE );
            } else {
                flp->len |= 1; // set allocated bit
            }
            return( flp );
        }
    }
    return( NULL );
}
#endif

static frlptr __LinkUpNewMHeap( mheapptr p1 ) // originally __AddNewHeap()
{
    mheapptr    p2;
    mheapptr    p2_prev;
    tag         *last_tag;
    unsigned    amount;

    /* insert into ordered heap list (14-jun-91 AFS) */
    /* logic wasn't inserting heaps in proper ascending order */
    /* (09-nov-93 Fred) */
    p2_prev = NULL;
    for( p2 = __nheapbeg; p2 != NULL; p2 = p2->next ) {
        if( p1 < p2 )
            break;
        p2_prev = p2;
    }
    /* ascending order should be: p2_prev < p1 < p2  */
    /* except for special cases when p2_prev and/or p2 are NULL */
    p1->prev = p2_prev;
    p1->next = p2;
    if( p2_prev != NULL ) {
        p2_prev->next = p1;
    } else {            /* add p1 to beginning of heap */
        __nheapbeg = p1;
    }
    if( p2 != NULL ) {
        /* insert before 'p2' (list is non-empty) */
        p2->prev = p1;
    }
    amount = p1->len - sizeof( miniheapblkp );
    /* Fill out the new miniheap descriptor */
    p1->freehead.len = 0;
    p1->freehead.prev = &p1->freehead;
    p1->freehead.next = &p1->freehead;
    p1->rover = &p1->freehead;
    p1->b4rover = 0;
    p1->numalloc = 0;
    p1->numfree  = 0;
    p1++;
    ((frlptr)p1)->len = amount;
    /* fix up end of heap links */
    last_tag = (tag *) ( (PTR)p1 + amount );
    *last_tag = END_TAG;
    return( (frlptr)p1 );
}

#if !( defined(__WINDOWS__) || defined(__WARP__) || defined(__NT__) )
size_t __LastFree( void )    /* used by nheapgrow to know about adjustment */
{
    frlptr      p1;
    unsigned    brk_value;

    if( __nheapbeg == NULL ) {      /* no heap? can't have free blocks */
        return( 0 );
    }
    p1 = __nheapbeg->freehead.prev; /* point to last free block */
    brk_value = (unsigned)((PTR)p1 + p1->len + TAG_SIZE );
  #if defined(__DOS_EXT__)
    if( _IsPharLap() && !_IsFlashTek() )
        _curbrk = SegmentLimit();
  #endif
    if( brk_value == _curbrk ) {    /* if last free block is at the end */
        return( p1->len );
    }
    return( 0 );
}
#endif

#if defined(__DOS_EXT__) && !defined(__CALL21__)
static void *RationalAlloc( size_t size )
{
    dpmi_hdr        *dpmi;
    mheapptr        mhp;
    tiny_ret_t      save_DOS_block;
    tiny_ret_t      DOS_block;

    __FreeDPMIBlocks();
    /* size is a multiple of 4k */
    dpmi = TinyDPMIAlloc( size );
    if( dpmi != NULL ) {
        mhp = (mheapptr)( dpmi + 1 );
        mhp->len = size - sizeof( dpmi_hdr );
        dpmi->dos_seg_value = 0;        // indicate DPMI block
        return( (void *)mhp );
    }
    if( __minreal & 0xfff00000 ) {
        /* checks for users that want >1M real memory saved */
        __minreal = 0xfffff;
    }
    if( size > 0x00010000 ) {
        /* cannot allocate more than 64k from DOS real memory */
        return( NULL );
    }
    save_DOS_block = TinyAllocBlock(( __minreal >> 4 ) | 1 );
    if( TINY_OK( save_DOS_block ) ) {
        DOS_block = TinyAllocBlock( size >> 4 );
        TinyFreeBlock( save_DOS_block );
        if( TINY_OK( DOS_block ) ) {
            dpmi = (dpmi_hdr *)TinyDPMIBase( DOS_block );
            dpmi->dos_seg_value = DOS_block;
            mhp = (mheapptr)( dpmi + 1 );
            mhp->len = size - sizeof( dpmi_hdr );
            return( (void *)mhp );
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
#if !( defined(__WINDOWS__) || defined(__WARP__) || defined(__NT__) )
    unsigned last_free_amt;
#endif

    amt = old_amount;
    amt = ( amt + TAG_SIZE + ROUND_SIZE) & ~ROUND_SIZE;
    if( amt < old_amount ) {
        return( 0 );
    }
#if !( defined(__WINDOWS__) || defined(__WARP__) || defined(__NT__) )
  #if defined(__DOS_EXT__)
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
  #if defined(__DOS_EXT__)
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
    amt += ( (TAG_SIZE) + sizeof( frl ) + sizeof( miniheapblkp ) );
    if( amt < *amount )
        return( 0 );            // Report request too large
    if( amt < _amblksiz ) {
        /*
          _amblksiz may not be even so round down to an even number
          nb. pathological case: where _amblksiz == 0xffff, we don't
                                 want the usual round up to even
        */
        amt = _amblksiz & ~1u;
    }
#if defined(__WINDOWS_386__) || defined(__WARP__) || defined(__NT__) \
  || defined(__CALL21__) || defined(__DOS_EXT__) || defined(__RDOS__)
    /* make sure amount is a multiple of 4k/64k */
    *amount = amt;
    amt += BLKSIZE_ALIGN_MASK;
    if( amt < *amount )
        return( 0 );
    amt &= ~BLKSIZE_ALIGN_MASK;
#endif
    *amount = amt;
    return( *amount != 0 );
}

#if defined(__WINDOWS__) || defined(__WARP__) || defined(__NT__) \
  || defined(__CALL21__) || defined(__DOS_EXT__) || defined(__RDOS__)
static int __CreateNewNHeap( unsigned amount )
{
    mheapptr        p1;
    frlptr          flp;
    unsigned        brk_value;
  #if defined(__WARP__)
    ULONG           os2_alloc_flags;
  #endif

    if( !__heap_enabled )
        return( 0 );
    if( _curbrk == ~1u )
        return( 0 );
    if( __AdjustAmount( &amount ) == 0 )
        return( 0 );
  #if defined(__WINDOWS_286__)
    brk_value = (unsigned)LocalAlloc( LMEM_FIXED, amount );
    if( brk_value == 0 ) {
        return( 0 );
    }
  #elif defined(__WINDOWS_386__)
    brk_value = (unsigned)DPMIAlloc( amount );
    if( brk_value == 0 ) {
        return( 0 );
    }
  #elif defined(__WARP__)
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
  #elif defined(__NT__)
    brk_value = (unsigned)VirtualAlloc( NULL, amount, MEM_COMMIT,
                                        PAGE_EXECUTE_READWRITE );
    //brk_value = (unsigned) LocalAlloc( LMEM_FIXED, amount );
    if( brk_value == 0 ) {
        return( 0 );
    }
  #elif defined(__CALL21__)
    {
        tag _WCNEAR *tmp_tag;

        tmp_tag = (tag _WCNEAR *)TinyMemAlloc( amount );
        if( tmp_tag == NULL ) {
            return( 0 );
        }
        /* make sure it will not look like the end of a heap */
        tmp_tag[0] = ! END_TAG;
        brk_value = (unsigned)&tmp_tag[2];
        amount -= 2 * TAG_SIZE; // 11-jun-95, subtract extra tag
    }
  #elif defined(__DOS_EXT__)
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
  #elif defined(__RDOS__)
    brk_value = (unsigned)RdosAllocateMem( amount );
    if( brk_value == 0 ) {
        return( 0 );
    }
  #endif
    if( amount - TAG_SIZE > amount ) {
        return( 0 );
    } else {
        amount -= TAG_SIZE;
    }
    if( amount < sizeof( miniheapblkp ) + sizeof( frl ) ) {
        /* there isn't enough for a heap block (struct miniheapblkp) and
           one free block (frl) */
        return( 0 );
    }
    /* we've got a new heap block */
    p1 = (mheapptr)brk_value;
    p1->len = amount;
  #if defined(__WARP__)
    // Remeber if block was allocated with OBJ_ANY - may be in high memory
    p1->used_obj_any = ( _os2_obj_any_supported && _os2_use_obj_any );
  #endif
    // Now link it up
    flp = __LinkUpNewMHeap( p1 );
    amount = flp->len;
    /* build a block for _nfree() */
    flp->len = amount | 1;
    ++p1->numalloc;
    p1->largest_blk = 0;
    _nfree( (PTR)flp + TAG_SIZE );
    return( 1 );
}
#endif

int __ExpandDGROUP( unsigned amount )
{
#if defined(__WINDOWS__) || defined(__WARP__) || defined(__NT__) \
  || defined(__CALL21__) || defined(__RDOS__)
    // first try to free any available storage
    _nheapshrink();
    return( __CreateNewNHeap( amount ) );
#else
    mheapptr    p1;
    frlptr      flp;
    unsigned    brk_value;
    tag         *last_tag;
    unsigned    new_brk_value;
    void        _WCNEAR *brk_ret;

  #if defined(__DOS_EXT__)
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
  #if defined(__DOS_EXT__)
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
    } else {
        amount -= TAG_SIZE;
    }
    for( p1 = __nheapbeg; p1 != NULL; p1 = p1->next ) {
        if( p1->next == NULL )
            break;
        if( (unsigned)p1 <= brk_value && ((unsigned)p1) + p1->len + TAG_SIZE >= brk_value ) {
            break;
        }
    }
    if( (p1 != NULL) && ((brk_value - TAG_SIZE) == (unsigned)( (PTR)p1 + p1->len) ) ) {
        /* we are extending the previous heap block (slicing) */
        /* nb. account for the end-of-heap tag */
        brk_value -= TAG_SIZE;
        amount += TAG_SIZE;
        flp = (frlptr) brk_value;
        /* adjust current entry in heap list */
        p1->len += amount;
        /* fix up end of heap links */
        last_tag = (tag *) ( (PTR)flp + amount );
        last_tag[0] = END_TAG;
    } else {
        if( amount < sizeof( miniheapblkp ) + sizeof( frl ) ) {
        /*  there isn't enough for a heap block (struct miniheapblkp) and
            one free block (frl) */
            return( 0 );
        }
        // Initializing the near heap if __nheapbeg == NULL,
        // otherwise, a new mini-heap is getting linked up
        p1 = (mheapptr)brk_value;
        p1->len = amount;
        flp = __LinkUpNewMHeap( p1 );
        amount = flp->len;
    }
    /* build a block for _nfree() */
    flp->len = amount | 1;
    ++p1->numalloc;                         /* 28-dec-90 */
    p1->largest_blk = ~0;    /* set to largest value to be safe */
    _nfree( (PTR)flp + TAG_SIZE );
    return( 1 );
#endif
}

#if defined(__WARP__)
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
