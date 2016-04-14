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
* Description:  DWARF DIP symbol address management.
*
****************************************************************************/


#include "dfdip.h"
#include "dfaddsym.h"
#include "dfsegs.h"


typedef struct {
    addr_off        map_offset;
    dr_handle       sym;
} off_info;

typedef struct off_blk{
    struct off_blk  *next;
    off_info        info[OFF_PER_BLK]; /*variable*/
} off_blk;

typedef struct seg_off {
    seg_entry   entry;
    off_blk     *head;
} seg_off;

typedef union {
    seg_entry   entry;
    seg_off     off;
} seg_info; /* general form */

typedef struct{
    seg_blk_head    head;
    seg_off         data[SEG_PER_BLK];
} seg_blk_off;

typedef union {
    seg_blk_head    head;
    seg_blk_off     off;
} seg_blk;  /* general form */

typedef struct {
    unsigned_16     hi;
    off_info        *base;
    addr_off        key;
    unsigned_16     last;
} off_cmp;

static off_info *AddMapOffset( seg_off *ctl, off_info *new )
/**********************************************************/
// Add a new offset to the last block if full alloc a new one
// bump the item count
{
    off_blk         *blk;
    off_info        *next;
    unsigned_16     rem;    /*#entries in last blk */

    rem = ctl->entry.count % OFF_PER_BLK;
    blk = ctl->head;
    if( rem == 0 ){
        blk = DCAlloc( sizeof( *blk ) );
        blk->next = ctl->head;
        ctl->head = blk;
    }
    next = blk->info + rem;
    *next = *new;
    ++ctl->entry.count;
    return( next );
}


static void InitSegOff( void *_newseg )
/*************************************/
{
    seg_info    *newseg = (seg_info *)_newseg;

    newseg->off.head = NULL;
}


static seg_blk_head *GetSegOffBlk( void )
/*************************************/
// Alloc a seg_info blk for seg routines
{
    seg_blk_off     *new;

    new = DCAlloc( sizeof( *new ) );
    new->head.next = NULL;
    new->head.info = &new->data[0].entry;
    return( (seg_blk_head*)new );
}


extern void AddAddrSym( seg_list *list, addrsym_info *new )
/*********************************************************/
// Add a new address to map
{
    static seg_ctl  SegCtl = { GetSegOffBlk, InitSegOff };
    off_info        data;
    seg_info        *seg_map;

    data.map_offset = new->map_offset;
    data.sym = new->sym;
    seg_map  = (seg_info *)AddMapSeg( list, &SegCtl, new->map_seg );
    AddMapOffset( &seg_map->off, &data );
}


static  long BlkOffSearch( off_cmp *cmp )
/***************************************/
// Do a B-search on the blk
{
    off_info        *curr;
    off_info        *base;
    addr_off        key;
    unsigned_16     lo;
    unsigned_16     mid;
    unsigned_16     hi;

    key = cmp->key;
    base = cmp->base;
    hi = cmp->hi;
    lo = 0;
    for( ;; ) {
        mid = MIDIDX16( lo, hi );
        curr = base + mid;
        if( mid == lo ) { /* fix up last cmp */
            break;
        }
        if( key < curr->map_offset ) {  // key < mid
            hi = mid;
        } else {                        // key > mid
            lo = mid;
        }
    }
    cmp->last = mid;
    cmp->base = curr;
    return( key - curr->map_offset );
}

// This is a bit screwy, maybe I should do something like dfaddr
// so the blocks are in sorted order
extern  int  FindAddrSym( seg_list     *addr_map,
                          addr_ptr     *mach,
                          addrsym_info *ret   )
/***********************************************/
{
    off_blk     *blk;
    off_info    *info;
    off_cmp     cmp;
    seg_info    *ctl;
    long        diff;
    long        last_find;

    cmp.key = mach->offset;
    ctl = (seg_info *)FindRealSeg( addr_map, mach->segment );
    diff = -1;
    last_find = -1;
    if( ctl != NULL ) {
        cmp.hi = ctl->entry.count % OFF_PER_BLK;
        if( cmp.hi == 0 ) {
            cmp.hi = OFF_PER_BLK;
        }
        for( blk = ctl->off.head; blk != NULL; blk = blk->next ) {
            cmp.base = blk->info;
            diff = BlkOffSearch( &cmp );
            if( diff >= 0 ) {
                if( diff < last_find || last_find < 0 ) {
                    info = cmp.base;
                    ret->map_offset = info->map_offset;
                    ret->sym = info->sym;
                    ret->map_seg = ctl->entry.real;
                    if( diff == 0 ) {
                        return( 0 );
                    }
                    last_find = diff;
                }
            }
            cmp.hi = OFF_PER_BLK;
        }
    }
    if( last_find < 0 )
        return( -1 );
    if( last_find > 0 )
        return( 1 );
    return( 0 );
}


static int  OffCmp( void const *_off1, void const *_off2 )
/********************************************************/
// Compare to offsets
{
    off_info const  *off1 = _off1;
    off_info const  *off2 = _off2;

    if( off1->map_offset < off2->map_offset )
        return( -1 );
    if( off1->map_offset > off2->map_offset )
        return( 1 );
    return( 0 );
}


static bool SortOffsets( void *d, void *_ctl )
/********************************************/
// Sort a seg's offsets
{
    seg_info        *ctl = (seg_info *)_ctl;
    off_blk         *blk;
    unsigned_16     blk_count;

    d = d;
    blk = ctl->off.head;
    blk_count = ctl->entry.count % OFF_PER_BLK;
    if( blk_count == 0 ) {
        blk_count = OFF_PER_BLK;
    } else {
        blk = DCRealloc( blk, PTRDIFF( blk->info + blk_count, blk )  );
        ctl->off.head = blk;
    }
    while( blk != NULL ) {
        qsort( blk->info, blk_count, sizeof( blk->info[0] ), OffCmp );
        blk_count = OFF_PER_BLK;
        blk = blk->next;
    }
    return( true );
}


extern  void    SortAddrSym( seg_list *ctl )
/******************************************/
{
    SegWalk( ctl, SortOffsets, NULL );
    SortSegReal( ctl );
}


struct wlk_glue {
    void        *d;
    WLKADDRSYM  fn;
};

static bool WalkOffsets( void *_wlk, void *_ctl )
/***********************************************/
// Sort a seg's offsets
{
    seg_info            *ctl = (seg_info *)_ctl;
    struct wlk_glue     *wlk = _wlk;
    off_blk             *blk;
    unsigned_16         blk_count;
    off_info            *next;
    addrsym_info        info;

    blk = ctl->off.head;
    blk_count = ctl->entry.count % OFF_PER_BLK;
    info.map_seg = ctl->entry.real;
    if( blk_count == 0 ) {
        blk_count = OFF_PER_BLK;
    }
    while( blk != NULL ) {
        next = blk->info;
        while( blk_count > 0 ) {
            info.map_offset = next->map_offset;
            info.sym = next->sym;
            if( !wlk->fn( wlk->d, &info ) )
                goto done;
            ++next;
            --blk_count;
        }
        blk_count = OFF_PER_BLK;
        blk = blk->next;
    }
    return( true );
done:
    return( false );
}


bool WlkAddrSyms( seg_list *ctl, WLKADDRSYM fn, void *d )
/*******************************************************/
{
    struct wlk_glue     wlk;

    wlk.fn = fn;
    wlk.d = d;
    return( SegWalk( ctl, WalkOffsets, &wlk ) );
}


extern void  InitAddrSym( seg_list *list )
/****************************************/
// Init seg_ctl with addr info
{
    InitSegList( list, sizeof( seg_off ) );
}


static bool FreeSegOffsets( void *d, void *_curr )
/************************************************/
// Free all offset blocks for a segment
{
    seg_info    *curr = (seg_info *)_curr;
    off_blk     *blk, *next;

    d = d;
    for( blk = curr->off.head; blk != NULL; blk = next ) {
        next = blk->next;
        DCFree( blk );
    }
    return( true );
}


extern void  FiniAddrSym( seg_list *ctl )
/***************************************/
//Free all offset blocks for a segment
//Free all segment blocks
{
    SegWalk( ctl, FreeSegOffsets, NULL );
    FiniSegList( ctl );
}
