/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023      The Open Watcom Contributors. All Rights Reserved.
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
    imp_image_handle    *iih;
    mem_block           block;
    imp_mod_handle      imh;
} AddrModCache;

static struct {
    imp_image_handle    *iih;
    imp_mod_handle      imh;
    unsigned long       off;
    mem_block           block;
} SegBlockCache;

#define IS_NONSECT( seg )   ((seg) >= NonSectStart && (seg) < NonSectEnd)

#define SEG_NOT_FOUND       0xffff
#define GET_SEG_INFO( blk, off )   \
        ((seg_dbg_info *)((char *)blk->info+(off)))
#define NEXT_SEG_INFO( ptr )        \
        ((seg_dbg_info *)((char *)ptr->addr+GET_SEG_COUNT(ptr)*sizeof(addr_dbg_info)))

#define GET_ADDR_INFO( blk, off )  \
        ((addr_dbg_info *)((char *)blk->info+(off)))

/*
 * the top bit in the count field is used as a flag to identify data
 * therefore it must be discarded when determining the number of
 * addr_dbg_info structures present.
 */
#define GET_SEG_COUNT( ptr )    (((seg_dbg_info *)ptr)->count & SEG_COUNT_MASK)


void AdjustAddrInit( void )
/**************************
 * adjust the addresses to reflect loader relocations
 */
{
    NonSectStart = 0xffff;
    NonSectEnd = 0;
}

void AdjustAddrs( imp_image_handle *iih, unsigned sectno )
{
    seg_dbg_info        *ptr;
    seg_dbg_info        *end;
    addr_dbg_info       *curr;
    unsigned            i;
    info_block          *blk;
    addr_off            off;
    addr_seg            last;
    address             place;          //NYI: place is a temp kludge
    section_info        *inf;

    inf = iih->sect + sectno;
    AddrModCache.iih = NULL;
    SegBlockCache.iih = NULL;
    if( inf->addr_info == NULL )
        return;
    place.sect_id = inf->sect_id;
    place.mach.segment = 0xffff;
    place.mach.offset = 0;
    for( blk = inf->addr_info; blk != NULL; blk = blk->next ) {
        end = GET_SEG_INFO( blk, blk->size );
        for( ptr = GET_SEG_INFO( blk, 0 ); ptr < end; ptr = NEXT_SEG_INFO( ptr ) ) {
            AddressMap( iih, &ptr->base );
            off = ptr->base.offset;
            curr = ptr->addr;
            for( i = GET_SEG_COUNT( ptr ); i != 0; --i, ++curr ) {
                if( curr->mod != (word)-1 ) {
                    if( iih->v2 ) {
                        curr->mod = ModOff2Idx( inf, curr->mod );
                    }
                    curr->mod += inf->mod_base_idx;
                }
                off += curr->size;
            }
            last = ptr->base.segment + ((off + 0x0f) >> 4);
            if( ptr->count & ~SEG_COUNT_MASK ) {
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


static dip_status SectFindAddrInfo( section_info *inf, address addr,
                                imp_mod_handle *imh, mem_block *code )
/*********************************************************************
 * return the module and mem_block that addr is in
 */
{
    seg_dbg_info    *ptr;
    seg_dbg_info    *end;
    addr_dbg_info   *info;
    addr_off        curr_off;
    unsigned        count;
    address         base;
    info_block      *blk;

    base.sect_id = inf->sect_id;
    base.indirect = true;
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
            for( count = GET_SEG_COUNT( ptr ); count != 0; --count ) {
                curr_off += info->size;
                if( curr_off > addr.mach.offset ) {
                    if( info->mod == (word)-1 )
                        goto next_block;
                    *imh = IDX2IMH( info->mod );
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


static dip_status FindAddrInfo( imp_image_handle *iih, address addr,
                        imp_mod_handle *imh, mem_block *code )
{
    section_info        *inf;
    unsigned            count;

    inf = iih->sect;
    for( count = iih->num_sects; count > 0; --count, ++inf ) {
        if( addr.sect_id == inf->sect_id || IS_NONSECT( addr.mach.segment ) ) {
            if( SectFindAddrInfo( inf, addr, imh, code ) == DS_OK ) {
                code->start.sect_id = inf->sect_id;
                code->start.indirect = true;
                return( DS_OK );
            }
        }
    }
    return( DS_FAIL );
}


address FindModBase( imp_image_handle *iih, imp_mod_handle imh )
/***************************************************************
 * return the start address for the code for a module
 */
{
    seg_dbg_info        *ptr;
    seg_dbg_info        *end;
    addr_dbg_info       *info;
    unsigned            count;
    address             base;
    section_info        *inf;
    info_block          *blk;

    inf = iih->sect;
    base.sect_id = inf->sect_id;
    base.indirect = true;
    for( blk = inf->addr_info; blk != NULL; blk = blk->next ) {
        ptr = GET_SEG_INFO( blk, 0 );
        end = GET_SEG_INFO( blk, blk->size );
        do {
            base.mach = ptr->base;
            info = ptr->addr;
            for( count = GET_SEG_COUNT( ptr ); count != 0; --count ) {
                if( IDX2IMH( info->mod ) == imh )
                    return( base );
                base.mach.offset += info->size;
                ++info;
            }
            ptr = NEXT_SEG_INFO( ptr );
        } while( ptr < end );
    }
    return( NilAddr );
}

mem_block FindSegBlock( imp_image_handle *iih, imp_mod_handle imh, unsigned long offset )
/****************************************************************************************
 * return the mem_block for a segment
 */
{
    seg_dbg_info        *ptr;
    seg_dbg_info        *next;
    addr_dbg_info       *info;
    addr_dbg_info       *curr;
    section_info        *inf;
    info_block          *blk;
    mem_block           block;

    if( offset == 0 ) {
        block.start = NilAddr;
        block.len = 0;
        return( block );
    }
    if( SegBlockCache.iih == iih
      && SegBlockCache.imh == imh
      && SegBlockCache.off == offset ) {
        return( SegBlockCache.block );
    }
    SegBlockCache.iih = iih;
    SegBlockCache.imh = imh;
    SegBlockCache.off = offset;
    inf = FindInfo( iih, imh );
    SegBlockCache.block.start.sect_id = inf->sect_id;
    for( blk = inf->addr_info; blk->size <= offset; blk = blk->next ) {
        offset -= blk->size;
    }
    info = GET_ADDR_INFO( blk, (size_t)offset );
    for( ptr = GET_SEG_INFO( blk, 0 ); (next = NEXT_SEG_INFO( ptr )) <= (seg_dbg_info *)info; ptr = next )
        ;
    SegBlockCache.block.start.mach = ptr->base;
    SegBlockCache.block.start.indirect = true;
    for( curr = ptr->addr; curr != info; ++curr ) {
        SegBlockCache.block.start.mach.offset += curr->size;
    }
    SegBlockCache.block.len = curr->size;
    return( SegBlockCache.block );
}


unsigned AddrInfoSplit( imp_image_handle *iih, info_block *blk, section_info *inf )
{
    seg_dbg_info    *start;
    seg_dbg_info    *seg;
    seg_dbg_info    *next;
    seg_dbg_info    *end;

    /* unused parameters */ (void)iih; (void)inf;

    start = GET_SEG_INFO( blk, 0 );
    end = GET_SEG_INFO( blk, blk->size );
    for( seg = start; ((byte *)end - (byte *)seg) >= sizeof( seg_dbg_info ); seg = next ) {
        next = NEXT_SEG_INFO( seg );
        if( next >= end ) {
            break;
        }
    }
    return( (byte *)seg - (byte *)start );
}


void AddrInfoFini( section_info *inf )
{
    /* unused parameters */ (void)inf;

    AddrModCache.iih = NULL;
    SegBlockCache.iih = NULL;
}


/*
 * DIPImpAddrMod
 */
search_result DIPIMPENTRY( AddrMod )( imp_image_handle *iih, address addr, imp_mod_handle *imh )
/***********************************************************************************************
 * return the mod_handle for the given address
 */
{
    mem_block       code;

    if( AddrModCache.iih == iih
     && AddrModCache.block.start.mach.offset < addr.mach.offset
     && AddrModCache.block.start.mach.offset+AddrModCache.block.len > addr.mach.offset
     && DCSameAddrSpace( AddrModCache.block.start, addr ) == DS_OK ) {
        *imh = AddrModCache.imh;
        return( SR_EXACT );
    }
    if( FindAddrInfo( iih, addr, imh, &code ) == DS_OK ) {
        AddrModCache.block = code;
        AddrModCache.imh = *imh;
        AddrModCache.iih = iih;
        return( SR_EXACT );
    }
    return( SR_NONE );
}

address DIPIMPENTRY( ModAddr )( imp_image_handle *iih, imp_mod_handle imh )
{
    return( FindModBase( iih, imh ) );
}
