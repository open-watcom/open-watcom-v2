/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2017 The Open Watcom Contributors. All Rights Reserved.
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
#include "wataddr.h"
#include "watlcl.h"
#include "watgbl.h"
#include "wattype.h"
#include "watmod.h"
#include "watldsym.h"
#include "digcli.h"


static void FreeInfBlks( info_block *blk )
{
    info_block          *next;

    for( ; blk != NULL; blk = next ) {
        next = blk->next;
        DCFree( blk );
    }
}

/*
 * DIPImpUnloadInfo -- unload the symbolic information for an image
 */

static void UnloadInfo( imp_image_handle *iih )
{
    section_info        *inf;
    unsigned            i;

    inf = iih->sect;
    if( inf != NULL ) {
        ClearTypeCache( iih );
        for( i = iih->num_sects; i > 0; --i, ++inf ) {
            GblSymFini( inf );
            FreeInfBlks( inf->gbl );
            ModInfoFini( inf );
            FreeInfBlks( inf->mod_info );
            AddrInfoFini( inf );
            FreeInfBlks( inf->addr_info );
        }
    }
    DCFree( iih->lang );
}

void DIPIMPENTRY( UnloadInfo )( imp_image_handle *iih )
{
    InfoClear( iih );
    UnloadInfo( iih );
}


/*
 * GetBlockInfo -- get permanent information into memory
 */

static dip_status GetBlockInfo( imp_image_handle *iih, section_info *new, unsigned long off,
                            dword size, info_block **owner,
                            unsigned (*split)(imp_image_handle *, info_block *, section_info *) )
{
    size_t              split_size;
    info_block          *curr;
    size_t              block_size;

    *owner = NULL;
    if( size == 0 )
        return( DS_OK );
    block_size = INFO_MAX_BLOCK;
    for( ;; ) {
        if( block_size > size )
            block_size = size;
        curr = DCAlloc( sizeof( info_block ) - 1 + block_size );
        if( curr == NULL ) {
            DCStatus( DS_ERR | DS_NO_MEM );
            return( DS_ERR | DS_NO_MEM );
        }
        *owner = curr;
        curr->size = block_size;
        curr->next = NULL;
        curr->link = NULL;
        if( InfoRead( iih->sym_fp, off, block_size, curr->info ) != DS_OK )
            return( DS_ERR | DS_INFO_INVALID );
        if( block_size == size )
            return( DS_OK );
        split_size = split( iih, curr, new );
        curr = DCRealloc( curr, ( sizeof( info_block ) - 1 ) + split_size );
        curr->size = split_size;
        off += split_size;
        size -= split_size;
        owner = &curr->next;
    }
}


/*
 * GetNumSect - find the number of sections for this load
 */

static dip_status GetNumSect( FILE *fp, unsigned long curr, unsigned long end, unsigned *count )
{
    section_dbg_header  header;

    *count = 0;
    while( curr < end ) {
        if( DCRead( fp, &header, sizeof( header ) ) != sizeof( header ) ) {
            DCStatus( DS_ERR | DS_INFO_INVALID );
            return( DS_ERR | DS_INFO_INVALID );
        }
        /* if there are no modules in the section, it's a 'placekeeper' section
            for the linker overlay structure -- just ignore it */
        if( header.mod_offset != header.gbl_offset ) {
            if( header.mod_offset > header.gbl_offset ) {
                DCStatus( DS_ERR | DS_INFO_INVALID );
                return( DS_ERR | DS_INFO_INVALID );
            }
            if( header.gbl_offset > header.addr_offset ) {
                DCStatus( DS_ERR | DS_INFO_INVALID );
                return( DS_ERR | DS_INFO_INVALID );
            }
            if( header.addr_offset >= header.section_size ) {
                DCStatus( DS_ERR | DS_INFO_INVALID );
                return( DS_ERR | DS_INFO_INVALID );
            }
        }
        (*count)++;
        curr += header.section_size;
        DCSeek( fp, curr, DIG_ORG );
    }
    if( curr > end ) {
        DCStatus( DS_ERR | DS_INFO_INVALID );
        return( DS_ERR | DS_INFO_INVALID );
    }
    return( DS_OK );
}

/*
 * This function assumes that section 0 is the lowest numbered section and
 * that section numbers are contiguous
 */

static dip_status ProcSectionsInfo( imp_image_handle *iih, unsigned num_sects )
{
    section_dbg_header  header;
    section_info        *new;
    dip_status          ds;
    unsigned long       pos;

    pos = DCTell( iih->sym_fp );
    while( num_sects-- > 0 ) {
        DCRead( iih->sym_fp, &header, sizeof( header ) );
        new = iih->sect + header.section_id;
        new->sect_id = header.section_id;
        new->mod_info = NULL;
        new->addr_info = NULL;
        new->gbl = NULL;
        new->dmnd_link = NULL;
        /* if there are no modules in the section, it's a 'placekeeper' section
            for the linker overlay structure -- just ignore it */
        if( header.mod_offset != header.gbl_offset ) {
            ds = GetBlockInfo( iih, new, header.mod_offset + pos,
                                header.gbl_offset - header.mod_offset,
                                &new->mod_info, &ModInfoSplit );
            if( ds != DS_OK )
                return( ds );
            ds = GetBlockInfo( iih, new, header.gbl_offset + pos,
                                header.addr_offset - header.gbl_offset,
                                &new->gbl, &GblSymSplit );
            if( ds != DS_OK )
                return( ds );
            ds = GetBlockInfo( iih, new, header.addr_offset + pos,
                                header.section_size - header.addr_offset,
                                &new->addr_info, &AddrInfoSplit );
            if( ds != DS_OK )
                return( ds );
            ds = MakeGblLst( iih, new );
            if( ds != DS_OK )
                return( ds );
            ds = AdjustMods( iih, new, pos );
            if( ds != DS_OK ) {
                return( ds );
            }
        }
        iih->num_sects++;
        pos += header.section_size;
        if( DCSeek( iih->sym_fp, pos, DIG_ORG ) ) {
            DCStatus( DS_ERR | DS_INFO_INVALID );
            return( DS_ERR | DS_INFO_INVALID );
        }
    }
    return( DS_OK );
}


static dip_status DoPermInfo( imp_image_handle *iih )
{
    master_dbg_header   header;
    dip_status          ds;
    unsigned long       end;
    unsigned long       curr;
    unsigned            num_segs;
    unsigned            num_sects;
    bool                v2;
    char                *new;

    if( DCSeek( iih->sym_fp, DIG_SEEK_POSBACK( sizeof( header ) ), DIG_END ) )
        return( DS_FAIL );
    end = DCTell( iih->sym_fp );
    if( DCRead( iih->sym_fp, &header, sizeof( header ) ) != sizeof( header ) ) {
        return( DS_FAIL );
    }
    while( header.signature == FOX_SIGNATURE1
            || header.signature == FOX_SIGNATURE2
            || header.signature == WAT_RES_SIG ) {
        if( header.debug_size > end ) {
            DCStatus( DS_ERR | DS_INFO_INVALID );
            return( DS_ERR | DS_INFO_INVALID );
        }
        end -= header.debug_size;
        DCSeek( iih->sym_fp, end, DIG_ORG );
        DCRead( iih->sym_fp, &header, sizeof( header ) );
    }
    if( header.signature != WAT_DBG_SIGNATURE )
        return( DS_FAIL );
    switch( header.exe_major_ver ) {
    case EXE_MAJOR_VERSION:
        v2 = false;
        break;
    case OLD_EXE_MAJOR_VERSION:
        v2 = true;
        break;
    default:
        DCStatus( DS_ERR | DS_INFO_BAD_VERSION );
        return( DS_ERR | DS_INFO_BAD_VERSION );
    }
    if( header.exe_minor_ver > EXE_MINOR_VERSION ) {
        DCStatus( DS_ERR | DS_INFO_BAD_VERSION );
        return( DS_ERR | DS_INFO_BAD_VERSION );
    }
    if( header.obj_major_ver != OBJ_MAJOR_VERSION ) {
        DCStatus( DS_ERR | DS_INFO_BAD_VERSION );
        return( DS_ERR | DS_INFO_BAD_VERSION );
    }
    if( header.obj_minor_ver > OBJ_MINOR_VERSION ) {
        DCStatus( DS_ERR | DS_INFO_BAD_VERSION );
        return( DS_ERR | DS_INFO_BAD_VERSION );
    }
    if( ( end + sizeof( header ) ) < header.debug_size ) {
        DCStatus( DS_ERR | DS_INFO_INVALID );
        return( DS_ERR | DS_INFO_INVALID );
    }
    num_segs = header.segment_size / sizeof( addr_seg );
    DCSeek( iih->sym_fp, header.lang_size + header.segment_size - header.debug_size, DIG_CUR );
    curr = DCTell( iih->sym_fp );
    ds = GetNumSect( iih->sym_fp, curr, end, &num_sects );
    if( ds != DS_OK )
        return( ds );
    new = DCAlloc( header.lang_size
                + num_segs * ( sizeof( addr_seg ) + sizeof( addr_ptr ) )
                + num_sects * sizeof( section_info ) );
    if( new == NULL ) {
        DCStatus( DS_ERR | DS_NO_MEM );
        return( DS_ERR | DS_NO_MEM );
    }
    iih->v2 = v2;
    iih->lang = new;
    iih->num_segs = num_segs;
    iih->map_segs = (void *)( new + header.lang_size );
    iih->real_segs = (void *)( iih->map_segs + num_segs );
    iih->sect = (void *)( iih->real_segs + num_segs );
    iih->num_sects = 0;
    DCSeek( iih->sym_fp, curr - header.lang_size - header.segment_size, DIG_ORG );
    if( DCRead( iih->sym_fp, iih->lang, header.lang_size ) != header.lang_size ) {
        DCStatus( DS_ERR | DS_INFO_INVALID );
        return( DS_ERR | DS_INFO_INVALID );
    }
    if( DCRead( iih->sym_fp, iih->map_segs, header.segment_size ) != header.segment_size ) {
        DCStatus( DS_ERR | DS_INFO_INVALID );
        return( DS_ERR | DS_INFO_INVALID );
    }
    ds = ProcSectionsInfo( iih, num_sects );
    if( ds != DS_OK ) {
        return( ds );
    }
    SetModBase( iih );
    return( InitDemand( iih ) );
}

/*
 * DIPImpLoadInfo -- process symbol table info on end of .exe file
 */
dip_status DIPIMPENTRY( LoadInfo )( FILE *fp, imp_image_handle *iih )
{
    dip_status          ds;

    iih->sym_fp = fp;
    iih->sect = NULL;
    iih->lang = NULL;
    ds = DoPermInfo( iih );
    iih->sym_fp = NULL;
    if( ds != DS_OK )
        UnloadInfo( iih );
    return( ds );
}


/*
 * InfoRead -- read demand information from disk
 */

dip_status InfoRead( FILE *fp, unsigned long offset, size_t size, void *buff )
{
    if( DCSeek( fp, offset, DIG_ORG ) ) {
        DCStatus( DS_ERR | DS_FSEEK_FAILED );
        return( DS_ERR | DS_FSEEK_FAILED );
    }
    if( DCRead( fp, buff, size ) != size ) {
        DCStatus( DS_ERR | DS_FREAD_FAILED );
        return( DS_ERR | DS_FREAD_FAILED );
    }
    return( DS_OK );
}



/*
 * DIPImpMapInfo -- change all map addresses into real addresses
 */


void DIPIMPENTRY( MapInfo )( imp_image_handle *iih, void *d )
{
    unsigned        i;

    for( i = 0; i < iih->num_segs; ++i ) {
        iih->real_segs[i].offset = 0;
        iih->real_segs[i].segment = iih->map_segs[i];
        DCMapAddr( iih->real_segs + i, d );
    }
    AdjustAddrInit();
    for( i = 0; i < iih->num_sects; ++i ) {
        AdjustAddrs( iih, i );
        AdjustSyms( iih, i );
    }
}


/*
 * AddressMap - take a map address and turn it into a real address
 */

void AddressMap( imp_image_handle *iih, addr_ptr *addr )
{
    unsigned            i;

    /* could probably binary search this */
    for( i = 0; i < iih->num_segs; ++i ) {
        if( addr->segment == iih->map_segs[i] ) {
            addr->segment = iih->real_segs[i].segment;
            addr->offset += iih->real_segs[i].offset;
            break;
        }
    }
}
