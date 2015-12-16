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


#include "dipwat.h"
#include "watmod.h"
#include "watldsym.h"
#include "wataddr.h"


static unsigned         NonSectStart;
static unsigned         NonSectEnd;

static struct {
    imp_image_handle    *ii;
    mem_block           block;
    imp_mod_handle      im;
} AddrModCache;

static struct {
    imp_image_handle    *ii;
    imp_mod_handle      im;
    unsigned long       off;
    mem_block           block;
} SegBlockCache;

#define IS_NONSECT( seg )   ((seg) >= NonSectStart && (seg) < NonSectEnd)

#define SEG_NOT_FOUND       0xffff
#define GET_SEG_INFO( blk, off )   \
        ((seg_info *)((char *)blk->info+(unsigned)(off)))
#define NEXT_SEG_INFO( ptr )        \
        ((seg_info *) ( (char *)&ptr->addr[0] +     \
                        GET_NUM_SEGS( ptr ) * sizeof( addr_info ) ))

#define GET_ADDR_INFO( blk, off )  \
        ((addr_info *)((char *)blk->info+(unsigned)(off)))

/*
 * the top bit in the num field is used as a flag to identify data
 * therefore it must be discarded when determining the number of
 * addr_info structures present.
 */

#define GET_NUM_SEGS( ptr ) \
        ( ( ( seg_info * ) ptr )->num & SEG_NUM_MASK )


/*
 * AdjustAddrs -- adjust the addresses to reflect loader relocations
 */

void AdjustAddrInit( void )
{
    NonSectStart = 0xffff;
    NonSectEnd = 0;
}

void AdjustAddrs( section_info *inf )
{
    seg_info *          ptr;
    seg_info *          end;
    addr_info *         curr;
    unsigned            i;
    info_block          *blk;
    addr_off            off;
    addr_seg            last;
    address             place;          //NYI: place is a temp kludge

    AddrModCache.ii = NULL;
    SegBlockCache.ii = NULL;
    if( inf->addr_info == NULL ) 
        return;
    place.sect_id = inf->sect_id;
    place.mach.segment = 0xffff;
    place.mach.offset = 0;
    for( blk = inf->addr_info; blk != NULL; blk = blk->next ) {
        end = GET_SEG_INFO( blk, blk->size );
        for( ptr=GET_SEG_INFO(blk,0); ptr < end; ptr = NEXT_SEG_INFO(ptr) ) {
            AddressMap( inf->ctl, &ptr->base );
            off = ptr->base.offset;
            curr = ptr->addr;
            for( i = GET_NUM_SEGS( ptr ); i != 0; --i, ++curr ) {
                if( curr->mod != (word)-1 ) {
                    if( inf->ctl->v2 ) {
                        curr->mod = ModOff2Idx( inf, curr->mod );
                    }
                    curr->mod += inf->mod_base_idx;
                }
                off += curr->size;
            }
            last = ptr->base.segment + ((off + 0x0f) >> 4);
            if( ptr->num & ~SEG_NUM_MASK ) {
                if( ptr->base.segment < NonSectStart ) {
                    NonSectStart = ptr->base.segment;
                }
                if( last > NonSectEnd ) {
                    NonSectEnd = last;
                }
            }
            if( ptr->base.segment < place.mach.segment ) {
                place.mach.segment = ptr->base.segment;
            }
            if( last > place.mach.offset ) {
                place.mach.offset = last;
            }
        }
    }
}


/*
 * FindAddrInfo -- return the module and mem_block that addr is in
 */

static dip_status SectFindAddrInfo( section_info *inf, address addr,
                                imp_mod_handle *im, mem_block *code )
{
    seg_info *     ptr;
    seg_info *     end;
    addr_info *    info;
    addr_off         curr_off;
    unsigned         num;
    address          base;
    info_block          *blk;

    base.sect_id = inf->sect_id;
    base.indirect = 1;
    for( blk = inf->addr_info; blk != NULL; blk = blk->next ) {
        ptr = GET_SEG_INFO( blk, 0 );
        end = GET_SEG_INFO( blk, blk->size );
        for( ;; ) {
            for( ;; ) {
                if( ptr >= end )
                    goto next_block;
                base.mach = ptr->base;
                if( base.mach.segment == addr.mach.segment )
                    break;
                ptr = NEXT_SEG_INFO( ptr );
            }
            curr_off = ptr->base.offset;
            if( curr_off > addr.mach.offset )
                goto next_block;
            info = ptr->addr;
            for( num = GET_NUM_SEGS( ptr ); num != 0; --num ) {
                curr_off += info->size;
                if( curr_off > addr.mach.offset ) {
                    if( info->mod == (word)-1 )
                        goto next_block;
                    *im = IDX2IMH( info->mod );
                    code->len = info->size;
                    code->start.mach.offset = curr_off - code->len;
                    code->start.mach.segment = ptr->base.segment;
                    return( DS_OK );
                }
                ++info;
            }
            ptr = NEXT_SEG_INFO( ptr );
        }
next_block:     ;
    }
    return( DS_FAIL );
}


static dip_status FindAddrInfo( imp_image_handle *ii, address addr,
                        imp_mod_handle *im, mem_block *code )
{
    section_info        *inf;
    unsigned            count;

    inf = ii->sect;
    for( count = ii->num_sects; count > 0; --count, ++inf ) {
        if( addr.sect_id == inf->sect_id || IS_NONSECT( addr.mach.segment ) ) {
            if( SectFindAddrInfo( inf, addr, im, code ) == DS_OK ) {
                code->start.sect_id = inf->sect_id;
                code->start.indirect = 1;
                return( DS_OK );
            }
        }
    }
    return( DS_FAIL );
}


/*
 * FindModBase -- return the start address for the code for a module
 */

address FindModBase( imp_image_handle *ii, imp_mod_handle im )
{
    seg_info            *ptr;
    seg_info            *end;
    addr_info           *info;
    unsigned            num;
    address             base;
    section_info        *inf;
    info_block          *blk;

    inf = ii->sect;
    base.sect_id = inf->sect_id;
    base.indirect = 1;
    for( blk = inf->addr_info; blk != NULL; blk = blk->next ) {
        ptr = GET_SEG_INFO( blk, 0 );
        end = GET_SEG_INFO( blk, blk->size );
        do {
            base.mach = ptr->base;
            info = ptr->addr;
            for( num = GET_NUM_SEGS( ptr ); num != 0; --num ) {
                if( IDX2IMH( info->mod ) == im )
                    return( base );
                base.mach.offset += info->size;
                ++info;
            }
            ptr = NEXT_SEG_INFO( ptr );
        } while( ptr < end );
    }
    return( NilAddr );
}

/*
 * FindSegBlock -- return the mem_block for a segment
 */

mem_block FindSegBlock( imp_image_handle *ii, imp_mod_handle im,
                         unsigned long offset )
{
    seg_info            *ptr;
    seg_info            *next;
    addr_info           *info;
    addr_info           *curr;
    section_info        *inf;
    info_block          *blk;

    if( offset == 0 ) {
        mem_block       block;

        block.start = NilAddr;
        return( block );
    }
    if( SegBlockCache.ii == ii
     && SegBlockCache.im == im
     && SegBlockCache.off == offset ) {
        return( SegBlockCache.block );
    }
    SegBlockCache.ii = ii;
    SegBlockCache.im = im;
    SegBlockCache.off = offset;
    inf = FindInfo( ii, im );
    SegBlockCache.block.start.sect_id = inf->sect_id;
    blk = inf->addr_info;
    for( ;; ) {
        if( offset < blk->size )
            break;
        offset -= blk->size;
        blk = blk->next;
    }
    info = GET_ADDR_INFO( blk, (unsigned)offset );
    ptr = GET_SEG_INFO( blk, 0 );
    for( ;; ) {
        next = NEXT_SEG_INFO( ptr );
        if( (seg_info *)info < next )
            break;
        ptr = next;
    }
    SegBlockCache.block.start.mach = ptr->base;
    SegBlockCache.block.start.indirect = 1;
    for( curr = ptr->addr; curr != info; ++curr ) {
        SegBlockCache.block.start.mach.offset += curr->size;
    }
    SegBlockCache.block.len = curr->size;
    return( SegBlockCache.block );
}


unsigned AddrInfoSplit( info_block *curr, section_info *inf )
{
    seg_info *  start;
    seg_info *  seg;
    seg_info *  next;
    seg_info *  end;

    inf = inf;
    start = GET_SEG_INFO( curr, 0 );
    end = GET_SEG_INFO( curr, curr->size );
    seg = start;
    for( ;; ) {
        if( ((byte *)end - (byte *)seg) < sizeof( seg_info ) )
            break;
        next = NEXT_SEG_INFO( seg );
        if( next >= end ) {
            break;
        }
    }
    return( (byte *)seg - (byte *)start );
}


void AddrInfoFini( section_info *inf )
{
    inf = inf;
    AddrModCache.ii = NULL;
    SegBlockCache.ii = NULL;
}


/*
 * DIPImpAddrMod -- return the mod_handle for the given address
 */

search_result DIGENTRY DIPImpAddrMod( imp_image_handle *ii, address addr, imp_mod_handle *im )
{
    mem_block       code;

    if( AddrModCache.ii == ii
     && AddrModCache.block.start.mach.offset < addr.mach.offset
     && AddrModCache.block.start.mach.offset+AddrModCache.block.len > addr.mach.offset
     && DCSameAddrSpace( AddrModCache.block.start, addr ) == DS_OK ) {
        *im = AddrModCache.im;
        return( SR_EXACT );
    }
    if( FindAddrInfo( ii, addr, im, &code ) == DS_OK ) {
        AddrModCache.block = code;
        AddrModCache.im = *im;
        AddrModCache.ii = ii;
        return( SR_EXACT );
    }
    return( SR_NONE );
}

address DIGENTRY DIPImpModAddr( imp_image_handle *ii, imp_mod_handle im )
{
    return( FindModBase( ii, im ) );
}
