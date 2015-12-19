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


#include "dfdip.h"
#include "dfsegs.h"


extern void  InitSegList( seg_list *ctl, unsigned_16 item_size ){
/*********************************/

    ctl->head = NULL;
    ctl->item_size = item_size;
    ctl->count = 0;
}

void FiniSegList( seg_list *ctl )
/*******************************/
// Free segment blocks
{
    seg_blk_head *blk, *next;

    for( blk = ctl->head; blk != NULL; blk = next ) {
        next = blk->next;
        DCFree( blk );
    }
    ctl->head = NULL;
    ctl->count = 0;
}

#define INFO_ITEM( base, index ) (seg_entry *)( (char *)base+ index*item_size )

seg_entry *AddMapSeg( seg_list *list, seg_ctl *ctl, addr_seg seg )
/****************************************************************/
{
    seg_blk_head   *blk;
    seg_entry      *info;
    unsigned_16     item_size;
    unsigned_16     blk_count;
    unsigned_16     rem;

    item_size = list->item_size;
    rem = list->count % SEG_PER_BLK;
    if( rem == 0 ){
        blk_count = SEG_PER_BLK;
    }else{
        blk_count = rem;
    }
    for( blk = list->head; blk != NULL; blk = blk->next ) {
        info = blk->info;
        while( blk_count > 0 ){
            if( info->real == seg ){
                goto exit_rtn;
            }
            info = INFO_ITEM( info, 1 );
            --blk_count;
        }
        blk_count = SEG_PER_BLK;
    }
    if( rem == 0 ){ /* new block */
        blk = ctl->alloc();
        blk->next = list->head;
        list->head = blk;
    } else {    /* use head block */
        blk = list->head;
    }
    info = blk->info;
    info = INFO_ITEM( info, rem );
    info->real = seg;
    info->count = 0;
    ctl->init( info );
    ++list->count;
exit_rtn:
    return( info );

}

seg_entry *FindMapSeg( seg_list *list, addr_seg seg )
/***************************************************/
{
    seg_blk_head   *blk;
    seg_entry      *info;
    unsigned_16     item_size;
    unsigned_16     blk_count;

    item_size = list->item_size;
    blk_count = list->count % SEG_PER_BLK;
    if( blk_count == 0 ){
        blk_count = SEG_PER_BLK;
    }
    for( blk  = list->head; blk != NULL; blk = blk->next ) {
        info = blk->info;
        while( blk_count > 0 ){
            if( info->real == seg ){
                goto exit_rtn;
            }
            info = INFO_ITEM( info, 1 );
            --blk_count;
        }
        blk_count = SEG_PER_BLK;
    }
    info = NULL;
exit_rtn:
    return( info );

}

typedef struct{
    unsigned_16 item_size;
    unsigned_16 hi;
    seg_entry   *base;
    word        key;
    unsigned_16 last;
}seg_cmp;


static  int  BlkSegSearch( seg_cmp *cmp  ){
// return > 0 and low index
//       == 0 and  = index
//       <  0 and  index == 0 off low end
    seg_entry    *curr;
    seg_entry    *base;
    unsigned_16 item_size;
    unsigned_16 lo;
    unsigned_16 mid;
    unsigned_16 hi;
    long        diff;

    hi = cmp->hi;
    item_size = cmp->item_size;
    base = cmp->base;
    lo = 0;
    for(;;){
        mid = MIDIDX16( lo, hi );
        curr = INFO_ITEM( base, mid ); // compare keys
        diff = (long)cmp->key - (long)curr->real;
        if( mid == lo )break;
        if( diff < 0 ){       // key < mid
            hi = mid;
        }else if( diff > 0 ){ // key > mid
            lo = mid;
        }else{                // key == mid
            break;
        }
    }
    cmp->last = mid;
    cmp->base = curr;
    return( diff );
}


seg_entry *FindRealSeg( seg_list *ctl, addr_seg seg )
/***************************************************/
// Find real seg seg_entry
{
    seg_entry      *info;
    seg_blk_head   *blk;
    seg_cmp         cmp;
    unsigned_16     blk_count;

    cmp.item_size = ctl->item_size;
    blk_count = ctl->count % SEG_PER_BLK;
    if( blk_count == 0 ){
        blk_count = SEG_PER_BLK;
    }
    cmp.hi = blk_count;  /* only first block not full */
    cmp.key = seg;
    info = NULL;
    for( blk = ctl->head; blk != NULL; blk = blk->next ) {
        cmp.base = blk->info;
        if( BlkSegSearch( &cmp )== 0 ){
            info = cmp.base;
            break;
        }
        cmp.hi = SEG_PER_BLK;
    }
    return( info );
}


bool SegWalk( seg_list *ctl, SEGWLK wlk, void *d )
/************************************************/
// Walk all the segments with wlk( d, info )
{
    seg_blk_head    *blk;
    seg_entry       *info;
    unsigned_16     blk_count;
    unsigned_16     item_size;


    item_size = ctl->item_size;
    blk_count = ctl->count % SEG_PER_BLK;
    if( blk_count == 0 ){  /* first block may be short */
        blk_count = SEG_PER_BLK;
    }
    for( blk = ctl->head; blk != NULL; blk = blk->next ) {
        info = blk->info;
        while( blk_count > 0 ){
            if( !wlk( d, info  ) )goto end_wlk;
            info = INFO_ITEM( info, 1 );
            --blk_count;
        }
        blk_count = SEG_PER_BLK;
    }
    return( TRUE );
end_wlk:
    return( FALSE );
}

static int  SegCmp( void const *_seg1, void const *_seg2 )
/********************************************************/
//Compare segments
{
    seg_entry const *seg1 = _seg1;
    seg_entry const *seg2 = _seg2;
    int diff;

    diff = (int)seg1->real - (int)seg2->real;
    return( diff );
}

void    SortSegReal( seg_list *ctl )
/**********************************/
//Sort a seg's real address
{
    seg_blk_head   *blk;
    unsigned_16     blk_count;
    unsigned_16     item_size;

    blk_count = ctl->count % SEG_PER_BLK;
    blk = ctl->head;
    item_size = ctl->item_size;
    if( blk_count == 0 ){  /* first block may be short */
        blk_count = SEG_PER_BLK;
    } else {
        seg_entry      *info;

        info = blk->info;
        blk = DCRealloc( blk, PTRDIFF( INFO_ITEM( info, blk_count ), blk ) );
        ctl->head = blk;
    }
    while( blk != NULL ){
        qsort( blk->info, blk_count, item_size, SegCmp );
        blk_count = SEG_PER_BLK;
        blk = blk->next;
    }
}

