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


#include <stdlib.h>
#include "dfdip.h"
#include "dfaddsym.h"
#include "dfsegs.h"

enum {
    OFF_PER_BLK = 256,
};
typedef struct {
    addr_off        map_offset;
    dr_handle       sym;
}off_info;

typedef struct off_blk{
    struct off_blk     *next;
    off_info            info[OFF_PER_BLK]; /*variable*/
}off_blk;

typedef struct seg_off{
    seg_entry  entry;
    off_blk    *head;
}seg_off;

typedef union {
    seg_entry entry;
    seg_off   off;
}seg_info; /* general form */

typedef struct{
    seg_blk_head   head;
    seg_off        data[SEG_PER_BLK];
}seg_blk_off;

typedef union {
    seg_blk_head head;
    seg_blk_off  off;
}seg_blk; /* general form */

static off_info *AddMapOffset( seg_off *ctl, off_info *new ){
/************************************************************/
// Add a new offset to the last block if full alloc a new one
// bump the item count
    off_blk     *blk;
    off_info    *next;
    unsigned_16 rem;    /*#entries in last blk */

    rem = ctl->entry.count % OFF_PER_BLK;
    blk = ctl->head;
    if( rem == 0 ){
        blk = DCAlloc( sizeof( *blk ) );
        blk->next = ctl->head;
        ctl->head = blk;
    }
    next = &blk->info[rem];
    *next = *new;
    ++ctl->entry.count;
    return( next );
}

static void InitSegOff(  seg_info *new ){
/***************************************/
    new->off.head = NULL;
}

static seg_blk *GetSegOffBlk( void ){
// Alloc a seg_info blk for seg routines
    seg_blk_off *new;

    new = DCAlloc( sizeof( *new ) );
    new->head.next = NULL;
    new->head.info  = &new->data[0].entry;
    return( new );
}

extern void AddAddrSym( seg_list *list, addrsym_info *new ){
/************************************************************/
// Add a new address to map
    static seg_ctl  SegCtl = { GetSegOffBlk, InitSegOff };
    off_info     data;
    seg_info *seg_map;

    data.map_offset = new->map_offset;
    data.sym = new->sym;
    seg_map = AddMapSeg( list, &SegCtl, new->map_seg );
    AddMapOffset( seg_map, &data );
}

typedef struct{
    unsigned_16 hi;
    off_info   *base;
    addr_off    key;
    unsigned_16 last;
}off_cmp;


static  long BlkOffSearch( off_cmp *cmp  ){
/****************************************/
// Do a B-search on the blk
    off_info    *curr;
    off_info    *base;
    addr_off    key;
    unsigned_16 lo;
    unsigned_16 mid;
    unsigned_16 hi;
    long        diff;

    key = cmp->key;
    base = cmp->base;
    hi = cmp->hi;
    lo = 0;
    for(;;){
        mid = (lo + hi)/2;
        curr = &base[mid];
        diff = (long)key - (long)curr->map_offset;
        if( mid == lo ){ /* fix up last cmp */
            break;
        }
        if( diff < 0 ){               // key < mid
            hi = mid;
        }else{                        // key > mid
            lo = mid;
        }
    }
    cmp->last = mid;
    cmp->base = curr;
    return( diff );
}
// This is a bit screwey  maybe I should do something like dfaddr
// so the blocks are in sorted order
extern  int  FindAddrSym( seg_list     *addr_map,
                          addr_ptr     *mach,
                          addrsym_info *ret   ){
/**********************************************/
    off_blk    *blk;
    off_info   *info;
    off_cmp     cmp;
    seg_info   *ctl;
    long       diff;
    long       last_find;

    cmp.key = mach->offset;
    ctl = FindRealSeg( addr_map, mach->segment );
    diff = -1;
    last_find = -1;
    if( ctl != NULL ){
        cmp.hi = ctl->entry.count % OFF_PER_BLK;
        if( cmp.hi == 0 ){
            cmp.hi = OFF_PER_BLK;
        }
        blk = ctl->off.head;
        while( blk != NULL ){
            cmp.base = &blk->info[0];
            diff =  BlkOffSearch( &cmp );
            if( diff >= 0 ){
                if( diff < last_find || last_find < 0 ){
                    info = cmp.base;
                    ret->map_offset = info->map_offset;
                    ret->sym = info->sym;
                    ret->map_seg = ctl->entry.real;
                    if( diff == 0 ){
                        return( diff );
                    }
                    last_find = diff;
                }
            }
            cmp.hi = OFF_PER_BLK;
            blk = blk->next;
        }
    }
    if( last_find < 0 ){
        last_find = -1;
    }else if( last_find > 0 ){
        last_find = 1;
    }
    return( last_find );
}

static int  OffCmp( off_info const *off1, off_info const *off2  ){
/*****************************************************************/
//Compare to offsets
    long   diff;

    diff = (long)off1->map_offset - (long)off2->map_offset;
    if( diff <  0 ){
        diff = -1;
    }else if( diff > 0 ){
        diff = 1;
    }
    return( diff );
}

static int SortOffsets( void *d, seg_info *ctl ){
/***************************************************/
//Sort a seg's offsets
    off_blk    *blk;
    unsigned_16 blk_count;

    d = d;
    blk = ctl->off.head;
    blk_count = ctl->entry.count % OFF_PER_BLK;
    if( blk_count == 0 ){
        blk_count = OFF_PER_BLK;
    }else{
        blk = DCRealloc( blk, PTRDIFF( &blk->info[blk_count], blk )  );
        ctl->off.head = blk;
    }
    while( blk != NULL ){
        qsort( &blk->info[0], blk_count, sizeof( blk->info[0] ), OffCmp );
        blk_count = OFF_PER_BLK;
        blk = blk->next;
    }
    return( TRUE );
}

extern  void    SortAddrSym( seg_list *ctl ){
/********************************************/
    SegWalk( ctl, SortOffsets, NULL );
    SortSegReal( ctl );
}

struct wlk_glue {
    void      *d;
    WLKADDRSYM fn;
};

static int WalkOffsets( struct wlk_glue *wlk, seg_info *ctl ){
/***************************************************/
//Sort a seg's offsets
    off_blk    *blk;
    unsigned_16 blk_count;
    off_info    *next;
    addrsym_info info;

    blk = ctl->off.head;
    blk_count = ctl->entry.count % OFF_PER_BLK;
    info.map_seg = ctl->entry.real;
    if( blk_count == 0 ){
        blk_count = OFF_PER_BLK;
    }
    while( blk != NULL ){
        next = &blk->info[0];
        while( blk_count > 0 ){
            info.map_offset = next->map_offset;
            info.sym = next->sym;
            if( !wlk->fn( wlk->d, &info ) )goto done;
            ++next;
            --blk_count;
        }
        blk_count = OFF_PER_BLK;
        blk = blk->next;
    }
    return( TRUE );
done:
    return( FALSE );
}


extern  int WlkAddrSyms( seg_list *ctl, WLKADDRSYM fn, void *d ){
/********************************************/
    struct wlk_glue wlk;

    wlk.fn = fn;
    wlk.d = d;
    return( SegWalk( ctl, WalkOffsets, &wlk ) );

}

extern void  InitAddrSym( seg_list *list ){
/*****************************************/
//Init seg_ctl with addr info
    InitSegList( list, sizeof( seg_off ) );
}

static int FreeSegOffsets( void *d, seg_info *curr ){
/***********************************************/
// Free all offset blocks for a segment
    off_blk    *blk, *old;

    d = d;
    blk = curr->off.head;
    while( blk != NULL ){
        old = blk;
        blk = blk->next;
        DCFree( old );
    }
    return( TRUE );
}


extern void  FiniAddrSym( seg_list *ctl ){
/*****************************************/
//Free all offset blocks for a segment
//Free all segment blocks

    SegWalk( ctl, FreeSegOffsets, NULL );
    FiniSegList( ctl );
}
