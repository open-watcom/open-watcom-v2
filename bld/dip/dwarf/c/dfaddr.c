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
* Description:  DWARF address space searching and management.
*
****************************************************************************/


#include "dfdip.h"
#include "dfaddr.h"
#include "dfsegs.h"
#include "enterdb.h"


typedef struct off_blk {
    struct off_blk  *next;
    off_info        info[OFF_PER_BLK];  /*variable*/
} off_blk;

typedef struct seg_off {
    seg_entry   entry;
    addr_seg    map;
    addr_off    map_diff;
    off_blk     *head;
} seg_off;

typedef union {
    seg_entry   entry;
    seg_off     off;
} seg_info; /* general form */

typedef struct {
    seg_blk_head    head;
    seg_off         data[SEG_PER_BLK];
} seg_blk_off;

typedef union {
    seg_blk_head    head;
    seg_blk_off     off;
} seg_blk;  /* general form */


static void InitSegOff( void *_new )
/**********************************/
{
    seg_info    *new = (seg_info *)_new;

    new->off.head = NULL;
}


static seg_blk_head *GetSegOffBlk( void )
/***************************************/
// Alloc a seg_info blk for seg routines
{
    seg_blk_off *new;

    new = DCAlloc( sizeof( *new ) );
    new->head.next = NULL;
    new->head.info  = &new->data[0].entry;
    return( (seg_blk_head*)new );
}


typedef struct {
    unsigned_16 hi;
    off_info    *base;
    addr_off    key;
    unsigned_16 last;
} off_cmp;

/*  JBS 2001/03/08

    Let's say the table looks like this.

    offset:32   len:32  map_offset:32   map_seg:16  imx:16
    [1000]      [10]    [x]             [x]         [x]
    [1010]      [22]    [x]             [x]         [x]
    [1032]      [10]    [x]             [x]         [x]
    [2000]      [14]    [x]             [x]         [x]

    If we look up offset 1000, then we return 0 (meaning a match was found).
    If we look up offset 1002, then we return 0 (meaning a match was found)
                            because it falls in the range of 1000-1009.
    If we look up offset 1050, then we return 1 (meaning no match was found)
                            because it does not fall in the range of 1032-1041
                            and it is below 2000.
    If we look up offset 0640, then we return -1 (meaning no match was found)
                            and it is below 1000.

    So 0 means "in range",
       1 means "above range", and
      -1 means "below range".

*/


static long BlkOffRangeSearch( off_cmp *cmp )
/*******************************************/
{
    unsigned_16     ctr;

    for( ctr = 0; ctr < cmp->hi; ctr++ ) {
        cmp->last = ctr;
        if( cmp->key  < cmp->base->offset ) return( -1 );
        if( cmp->key == cmp->base->offset ) return( 0 );
        if( cmp->key  < (cmp->base->offset + cmp->base->len) ) return( 0 );
        cmp->base++;
    }
    /* So the offset is greater than the current one but it could fall
       within the range of the current one
    */
    return( 1 );
}

/*  JBS 2001/03/09

    BlkOffSearch is like BlkOffRangeSearch except that it doesn't do range
    checking.

    If we look up offset 1002, then we return -1 (meaning it goes before 1010).
*/

static long BlkOffSearch( off_cmp *cmp )
/**************************************/
{
    unsigned_16     ctr;

    for( ctr = 0; ctr < cmp->hi; ctr++ ) {
        cmp->last = ctr;
        if( cmp->key  < cmp->base->offset ) return( -1 );
        if( cmp->key == cmp->base->offset ) return( 0 );
        cmp->base++;
    }
    /* So the offset is greater than the current one but it could fall
       within the range of the current one
    */
    return( 1 );
}

/*
    Mike's old flawed binary search (did not work with addresses >= 0x80000000)
*/

// static long BlkOffSearch( off_cmp *cmp ){
// /***************************************/
// // Do a B-search on the blk
//     off_info    *curr;
//     off_info    *base;
//     addr_off    key;
//     unsigned_16 lo;
//     unsigned_16 mid;
//     unsigned_16 hi;
//     long        diff;
//
//     key = cmp->key;
//     base = cmp->base;
//     hi = cmp->hi;
//     lo = 0;
//     for(;;){
//         mid = (lo + hi)/2;
//         curr = &base[mid];
//         diff = (long)key - (long)curr->offset;
//         if( mid == lo ){ /* fix up last cmp */
//             if( diff > 0 && diff < curr->len ){
//                 diff = 0;
//             }
//             break;
//         }
//         if( diff < 0 ){               // key < mid
//             hi = mid;
//         }else if( diff < curr->len ){ // key == mid
//             diff = 0;
//             break;
//         }else{                        // key > mid
//             lo = mid;
//         }
//     }
//     cmp->last = mid;
//     cmp->base = curr;
//     return( diff );
// }

static  void AddSortOffset( seg_info *ctl, off_info *new )
/********************************************************/
// blocks are in decreasing order
// within a block entries are in increasing order (history).
// it would be better to keep in decreasing order so we could shuffle up
// and not have mem overlap problems
{
    off_blk         *blk;
    off_info        *info;
    off_cmp         cmp;
    unsigned_16     rem;
    unsigned_16     blk_count;
    off_blk         *curr;
    off_blk         *next;
    long            diff;

    blk = ctl->off.head;
    rem = ctl->entry.count;
    rem = ctl->entry.count % OFF_PER_BLK;
    if( rem == 0 ){
        blk_count = OFF_PER_BLK;
    } else {
        blk_count = rem;
    }
    if( blk == NULL || new->offset > blk->info[blk_count-1].offset ) {
        if( rem == 0 ) {    // if higher than high add to first
            blk  = DCAlloc( sizeof( *blk ) );
            blk->next = ctl->off.head;
            ctl->off.head = blk;
            info = &blk->info[0];
        } else {
            info = &blk->info[rem];
        }
    } else {
        // blk->next == null => lower than low add to end block
        // Because we shuffle down  we have to find the block
        // where new will insert. This means it has to be >= the
        // next block.
        while( (next = blk->next) != NULL ) {
            if( new->offset >= next->info[OFF_PER_BLK-1].offset ) break;
            blk = next;
        }
        if( blk != ctl->off.head ) {
            blk_count = OFF_PER_BLK;
        }
        cmp.base = &blk->info[0];
        cmp.key = new->offset;
        cmp.hi = blk_count;
        diff = BlkOffSearch( &cmp );
        if( diff == 0 ) goto exit;
        if( diff > 0 ) {
            ++cmp.last; // if new > insert after
            ++cmp.base; //  last compare
        }
        curr = ctl->off.head;
        if( rem == 0 ) {    /* spill to new block */
            next = DCAlloc( sizeof( *next ) );
            next->next = ctl->off.head;
            ctl->off.head = next;
            next->info[0] = curr->info[OFF_PER_BLK-1];
            rem = OFF_PER_BLK-1;
        }
        while( curr != blk ) {  // shuffle free space down to blk
            next = curr->next;
            info = curr->info;
            memmove( &info[1], &info[0],
                     rem * sizeof( info[0]) );
            info[0] = next->info[OFF_PER_BLK-1];
            curr = next;
            next = curr->next;
            rem = OFF_PER_BLK-1;
        }
        blk_count = cmp.last;
        info = cmp.base; /* set at last compare */
        memmove( &info[1], &info[0],
                 (rem-blk_count) * sizeof( *info ) );
    }
    info[0] = *new;
    ++ctl->entry.count;
exit:
    return;
}


extern void AddMapAddr( seg_list *list, void *dcmap, off_info *new )
/******************************************************************/
// Add a new address to map
{
    static seg_ctl  SegCtl = { GetSegOffBlk, InitSegOff };
    addr_ptr        a;
    seg_info        *seg_map;

    a.segment = new->map_seg;
    a.offset  = new->map_offset;
    DCMapAddr( &a, dcmap );
    new->offset = a.offset;
    seg_map = (seg_info *)AddMapSeg( list, &SegCtl, a.segment );
    AddSortOffset( seg_map, new );
}


#ifdef DEBUG
static bool CheckInfo( seg_info *ctl )
/**Internal check to see if sorted***/
{
    unsigned_16     rem;
    unsigned_16     blk_count;
    off_info        *info;
    off_info        *last_info;
    off_blk         *curr;
    off_blk         *next;

    rem = ctl->entry.count % OFF_PER_BLK;
    if( rem == 0 ) {
        blk_count = OFF_PER_BLK;
    } else {
        blk_count = rem;
    }
    curr = ctl->off.head;
    while( curr != NULL ) { // shuffle free space down to blk
        next = curr->next;
        info = curr->info;
        if( next != NULL ){
            if( info->offset < next->info[OFF_PER_BLK-1].offset ) goto error;
        }
        last_info = info;
        ++info;
        --blk_count;
        while( blk_count > 0 ){
            if( info->offset < last_info->offset ) goto error;
            last_info = info;
            ++info;
            --blk_count;
        }
        blk_count = OFF_PER_BLK;
        curr = curr->next;
    }
    return( TRUE );
error:
    return( FALSE );
}


static int ChkOffsets( void *d, seg_info *ctl )
/*********************************************/
{
// Sort a seg's offsets
    d = d;
    if( !CheckInfo( ctl ) ) {
        EnterDebugger();
    }
    return( TRUE );
}

extern void DmpBlk( off_blk *blk, int count )
/***** Print contents of blk ***************/
{
    off_info    *info;

    myprintf( "blk %lx, %d \r\n", blk, count );
    info = blk->info;
    while( count > 0 ) {
        myprintf( "off %lx(%ld) map %x:%lx imx %d\r\n",
            info->offset,
            info->len,
            info->map_seg,
            info->map_offset,
            info->imx );
        ++info;
        --count;
    }
}
#endif


static  off_info *SearchBlkList( seg_info *ctl, addr_off offset )
/***************************************************************/
{
    off_blk         *blk;
    off_info        *info;
    off_cmp         cmp;
    unsigned_16     blk_count;

    info = NULL;
    blk = ctl->off.head;
    while( blk != NULL ) {
        if( offset >= blk->info[0].offset ) {
            if( blk == ctl->off.head ) {    /* only first block might not be full */
                blk_count = ctl->entry.count % OFF_PER_BLK;
                if( blk_count == 0 ) {
                     blk_count = OFF_PER_BLK;
                }
            } else {
                blk_count = OFF_PER_BLK;
            }
            cmp.hi = blk_count;
            cmp.key = offset;
            cmp.base = &blk->info[0];
            if( BlkOffRangeSearch( &cmp ) == 0 ) {
                info = cmp.base;
            }
            break;
        }
        blk = blk->next;
    }
    return( info );
}


typedef struct {
    address     *a;
    off_info    *info;
    address     seg_base;
} wlk_seg_offsets;

static int WlkSegInfos( void *_d, void *_curr )
/*********************************************/
{
    seg_info            *curr = (seg_info *)_curr;
    wlk_seg_offsets     *d = _d;
    int                 cont;

    d->seg_base.mach.segment = curr->entry.real;
    cont = TRUE;
    if( DCSameAddrSpace( *d->a, d->seg_base ) == DS_OK ) {
        d->info = SearchBlkList( curr, d->a->mach.offset );
        if( d->info != NULL ){
            cont = FALSE;
        }
    }
    return( cont );
}


extern  off_info *FindMapAddr( seg_list *addr_map, address *a )
/*************************************************************/
{
    off_info   *info;
//  seg_info   *ctl;

//  Had to do a walk cause real segs can be same addr space
//  Left original code in case I got a better idea.
//  info = NULL;
//  ctl = FindRealSeg( addr_map, a->mach.segment );
//  if( ctl != NULL ){
//      info = SearchBlkList( ctl, a->mach.offset );
//  }else{
        wlk_seg_offsets d;
        d.a = a;
        d.info = NULL;
        d.seg_base = NilAddr;
        SegWalk( addr_map, WlkSegInfos, &d );
        info = d.info;
//  }
    return( info );
}


extern  void    SortMapAddr( seg_list *ctl )
/******************************************/
{
#ifdef DEBUG
    SegWalk( ctl, ChkOffsets, NULL );
#endif
    SortSegReal( ctl );
}


extern int Real2Map( seg_list *ctl, address *what )
/*************************************************/
// convert a map address found in dbg to real address in image
{
    int         ret;
    off_info    *off;

    ret = FALSE;

    off = FindMapAddr( ctl, what );
    if( off != NULL ) {
        what->mach.offset -=  off->offset;
        what->mach.offset +=  off->map_offset;
        what->mach.segment =  off->map_seg;
        ret = TRUE;
    }
    return( ret );
}


extern void InitAddrInfo( seg_list *list )
/****************************************/
//Init seg_ctl with addr info
{
    InitSegList( list, sizeof( seg_off ) );
}


static int FreeSegOffsets( void *d, void *_curr )
/***********************************************/
// Free all offset blocks for a segment
{
    seg_info    *curr = (seg_info *)_curr;
    off_blk     *blk, *old;

    d = d;
    blk = curr->off.head;
    while( blk != NULL ) {
        old = blk;
        blk = blk->next;
        DCFree( old );
    }
    return( TRUE );
}


extern void FiniAddrInfo( seg_list *ctl )
/***************************************/
//Free all offset blocks for a segment
//Free all segment blocks
{
    SegWalk( ctl, FreeSegOffsets, NULL );
    FiniSegList( ctl );
}
