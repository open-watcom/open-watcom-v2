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


#include "cvinfo.h"
#include "exepe.h"
#include "exedos.h"

/* WD looks for this symbol to determine module bitness */
#if !defined( __WINDOWS__ )
int __nullarea;
#if defined( __WATCOMC__ )
#pragma aux __nullarea "*";
#endif
#endif

/*
        Loading/unloading symbolic information.
*/

imp_image_handle        *ImageList;

static void Cleanup( imp_image_handle *ii )
{
    imp_image_handle    **owner;
    imp_image_handle    *curr;
    unsigned            blocks;
    unsigned            i;

    owner = &ImageList;
    for( ;; ) {
        curr = *owner;
        if( curr == ii ) break;
        owner = &curr->next_image;
    }
    *owner = ii->next_image;
    if( ii->directory != NULL ) {
        blocks = BLOCK_FACTOR( ii->dir_count, DIRECTORY_BLOCK_ENTRIES );
        for( i = 0; i < blocks; ++i ) {
            if( ii->directory[i] != NULL ) {
                DCFree( ii->directory[i] );
            }
        }
        DCFree( ii->directory );
    }
    DCFree( ii->mapping );
    VMFini( ii );
}

static dip_status TryFindPE( dig_fhandle h, unsigned long *offp,
                                unsigned long *sizep )
{
    union {
        dos_exe_header  dos;
        pe_header       pe;
    }                   hdr;
    pe_object           obj;
    unsigned_32         nh_off;
    unsigned_32         section_off;
    unsigned            i;
    unsigned_32         debug_rva;
    debug_directory     dir;

    if( DCSeek( h, 0, DIG_ORG ) != 0 ) {
        return( DS_ERR|DS_FSEEK_FAILED );
    }
    if( DCRead( h, &hdr.dos, sizeof( hdr.dos ) ) != sizeof( hdr.dos ) ) {
        return( DS_ERR|DS_FREAD_FAILED );
    }
    if( hdr.dos.signature != DOS_SIGNATURE ) {
        return( DS_FAIL );
    }
    if( DCSeek( h, NH_OFFSET, DIG_ORG ) != NH_OFFSET ) {
        return( DS_ERR|DS_FSEEK_FAILED );
    }
    if( DCRead( h, &nh_off, sizeof( nh_off ) ) != sizeof( nh_off ) ) {
        return( DS_ERR|DS_FREAD_FAILED );
    }
    if( DCSeek( h, nh_off, DIG_ORG ) != nh_off ) {
        return( DS_ERR|DS_FSEEK_FAILED );
    }
    if( DCRead( h, &hdr.pe, sizeof( hdr.pe ) ) != sizeof( hdr.pe ) ) {
        return( DS_FAIL );
    }
    if( hdr.pe.signature != PE_SIGNATURE ) {
        return( DS_FAIL );
    }
    if( hdr.pe.table[ PE_TBL_DEBUG ].rva == 0 ) {
        return( DS_FAIL );
    }
    debug_rva = (hdr.pe.table[ PE_TBL_DEBUG ].rva / hdr.pe.object_align)*
                                hdr.pe.object_align;

    section_off = nh_off + offsetof( pe_header, flags ) +
                        sizeof( hdr.pe.flags ) + hdr.pe.nt_hdr_size;

    if( DCSeek( h, section_off, DIG_ORG ) != section_off ) {
        return( DS_ERR|DS_FSEEK_FAILED );
    }
    for( i=0; i < hdr.pe.num_objects; i++ ) {
        if( DCRead( h, &obj, sizeof( obj ) ) != sizeof( obj ) ) {
            return( DS_ERR|DS_FREAD_FAILED );
        }
        if( obj.rva == debug_rva ) {
            debug_rva = obj.physical_offset +
                            hdr.pe.table[ PE_TBL_DEBUG ].rva - debug_rva;
            if( DCSeek( h, debug_rva, DIG_ORG ) != debug_rva ) {
                return( DS_ERR|DS_FSEEK_FAILED );
            }
            if( DCRead( h, &dir, sizeof( dir ) ) != sizeof( dir ) ) {
                return( DS_ERR|DS_FREAD_FAILED );
            }
            if( dir.debug_type != DEBUG_TYPE_CODEVIEW ) {
                return( DS_FAIL );
            }
            *offp = dir.data_seek;
            *sizep = dir.debug_size;
        }
    }
    return( DS_FAIL );
}

static dip_status TryFindTrailer( dig_fhandle h, unsigned long *offp,
                                        unsigned long *sizep )
{
    cv_trailer          sig;
    unsigned long       pos;

    pos = DCSeek( h, DCSEEK_POSBACK( sizeof( sig ) ), DIG_END );
    if( pos == DCSEEK_ERROR ) {
        return( DS_ERR|DS_FSEEK_FAILED );
    }
    if( DCRead( h, &sig, sizeof( sig ) ) != sizeof( sig ) ) {
        return( DS_ERR|DS_FREAD_FAILED );
    }
    if( memcmp( sig.sig, CV4_NB09, sizeof( sig.sig ) ) != 0 ) {
        return( DS_FAIL );
    }
    *sizep = sig.offset - sizeof( sig );
    *offp = pos - *sizep;
    return( DS_OK );
}

static dip_status FindCV( dig_fhandle h, unsigned long *offp,
                                unsigned long *sizep )
{
    char        sig[CV_SIG_SIZE];
    dip_status  ds;

    ds = TryFindPE( h, offp, sizep );
    if( ds & DS_ERR ) return( ds );
    if( ds != DS_OK ) {
        ds = TryFindTrailer( h, offp, sizep );
        if( ds != DS_OK ) return( ds );
    }
    if( DCSeek( h, *offp, DIG_ORG ) != *offp ) {
        return( DS_ERR|DS_FSEEK_FAILED );
    }
    if( DCRead( h, sig, sizeof( sig ) ) != sizeof( sig ) ) {
        return( DS_ERR|DS_FREAD_FAILED );
    }
    if( memcmp( sig, CV4_NB09, sizeof( sig ) ) != 0 ) {
        return( DS_FAIL );
    }
    return( DS_OK );
}

static dip_status LoadDirectory( imp_image_handle *ii, unsigned long off )
{
    unsigned_32                 directory;
    cv_subsection_directory     dir_header;
    unsigned                    block_count;
    unsigned                    i;
    unsigned                    left;
    unsigned                    block_size;
    unsigned                    num;

    if( DCSeek( ii->sym_file, off, DIG_ORG ) != off ) {
        return( DS_ERR|DS_FSEEK_FAILED );
    }
    if( DCRead( ii->sym_file, &directory, sizeof( directory ) ) != sizeof( directory ) ) {
        return( DS_ERR|DS_FREAD_FAILED );
    }
    if( DCSeek( ii->sym_file, ii->bias + directory, DIG_ORG ) != (ii->bias + directory) ) {
        return( DS_ERR|DS_FSEEK_FAILED );
    }
    if( DCRead( ii->sym_file, &dir_header, sizeof( dir_header ) ) != sizeof( dir_header ) ) {
        return( DS_ERR|DS_FREAD_FAILED );
    }
    if( dir_header.cbDirHeader != sizeof( dir_header )
     || dir_header.cbDirEntry  != sizeof( cv_directory_entry ) ) {
        return( DS_ERR|DS_INFO_INVALID );
    }
    ii->dir_count = dir_header.cDir;
    block_count = BLOCK_FACTOR( ii->dir_count, DIRECTORY_BLOCK_ENTRIES );
    ii->directory = DCAlloc( block_count * sizeof( cv_directory_entry * ) );
    if( ii->directory == NULL ) {
        return( DS_ERR|DS_NO_MEM );
    }
    memset( ii->directory, 0, block_count * sizeof( cv_directory_entry * ) );
    i = 0;
    left = ii->dir_count;
    for( ;; ) {
        num = left;
        if( num > DIRECTORY_BLOCK_ENTRIES ) num = DIRECTORY_BLOCK_ENTRIES;
        block_size = num * sizeof( cv_directory_entry );
        ii->directory[i] = DCAlloc( block_size );
        if( ii->directory[i] == NULL ) {
            return( DS_ERR|DS_NO_MEM );
        }
        if( DCRead( ii->sym_file, ii->directory[i], block_size ) != block_size ) {
            return( DS_ERR|DS_FREAD_FAILED );
        }
        ++i;
        left -= num;
        if( left == 0 ) break;
    }
    return( DS_OK );
}

static dip_status LoadMapping( imp_image_handle *ii )
{
    cv_directory_entry  *cde;
    cv_sst_seg_map      *map;
    unsigned            size;

    cde = FindDirEntry( ii, IMH_GBL, sstSegMap );
    if( cde == NULL ) return( DS_ERR|DS_INFO_INVALID );
    map = VMBlock( ii, cde->lfo, cde->cb );
    if( map == NULL ) return( DS_ERR|DS_FAIL );
    size = map->cSegLog * sizeof( map->segdesc[0] );
    ii->mapping = DCAlloc( size );
    if( ii->mapping == NULL ) return( DS_ERR|DS_NO_MEM );
    map = VMBlock( ii, cde->lfo, cde->cb ); /* malloc might have unloaded */
    memcpy( ii->mapping, &map->segdesc[0], size );
    ii->map_count = map->cSegLog;
    return( DS_OK );
}

static walk_result FindCompUnit( imp_image_handle *ii,
                                cv_directory_entry *cde, void *d )
{
    cs_compile          **rec = d;

    if( cde->subsection != sstModule ) return( WR_CONTINUE );
    *rec = GetCompInfo( ii, cde->iMod );
    if( *rec == NULL ) return( WR_CONTINUE );
    return( WR_STOP );
}

static dip_status SetMADType( imp_image_handle *ii )
{
    cs_compile                  *rec;
    walk_result                 wr;

    wr = WalkDirList( ii, &FindCompUnit, &rec );
    if( wr != WR_STOP ) return( DS_OK );
    switch( rec->machine & 0xf0 ) {
    case MACH_INTEL_8080:
        ii->mad = MAD_X86;
        break;
    case MACH_DECALPHA:
        ii->mad = MAD_AXP;
        break;
    default:
        return( DS_ERR|DS_INFO_INVALID );
    }
    return( DS_OK );
}

dip_status      DIGENTRY DIPImpLoadInfo( dig_fhandle h, imp_image_handle *ii )
{
    dip_status                  ds;
    unsigned long               off;
    unsigned long               size;
    cv_directory_entry          *cde;
    cv_sst_global_types_header  *hdr;

    memset( ii, 0, sizeof( *ii ) );
    ds = FindCV( h, &off, &size );
    if( ds != DS_OK ) return( ds );
    ii->sym_file = h;
    ii->bias = off;
    ds = VMInit( ii, size );
    if( ds != DS_OK ) return( ds );
    ii->next_image = ImageList;
    ImageList = ii;
    ds = LoadDirectory( ii, off + CV_SIG_SIZE );
    if( ds != DS_OK ) {
        DCStatus( ds );
        Cleanup( ii );
        return( ds );
    }
    ds = LoadMapping( ii );
    if( ds != DS_OK ) {
        DCStatus( ds );
        Cleanup( ii );
        return( ds );
    }
    cde = FindDirEntry( ii, IMH_GBL, sstGlobalTypes );
    if( cde != NULL ) {
        hdr = VMBlock( ii, cde->lfo, sizeof( *hdr ) );
        if( hdr == NULL ) {
            Cleanup( ii );
            return( DS_ERR|DS_FAIL );
        }
        ii->types_base = cde->lfo
            + offsetof(cv_sst_global_types_header, offType )
            + hdr->cType * sizeof( hdr->offType[0] );
    }
    ds = SetMADType( ii );
    if( ds != DS_OK ) {
        DCStatus( ds );
        Cleanup( ii );
        return( ds );
    }
    return( DS_OK );
}

void            DIGENTRY DIPImpMapInfo( imp_image_handle *ii, void *d )
{
    unsigned    i;
    addr_ptr    addr;

    for( i = 0; i < ii->map_count; ++i ) {
        addr.segment = ii->mapping[i].frame;
        addr.offset  = ii->mapping[i].offset;
        DCMapAddr( &addr, d );
        ii->mapping[i].frame  = addr.segment;
        ii->mapping[i].offset = addr.offset;
    }
}

dip_status SegIsExecutable( imp_image_handle *ii, unsigned log )
{
    seg_desc    *map;

    map = &ii->mapping[log-1];
    return( map->u.b.fExecute ? DS_OK : DS_FAIL );
}

void MapLogical( imp_image_handle *ii, address *a )
{
    seg_desc    *map;

    map = &ii->mapping[a->mach.segment-1];
    a->mach.segment = map->frame;
    a->mach.offset += map->offset;
    a->sect_id = map->ovl;
    a->indirect = 1;
}

void            DIGENTRY DIPImpUnloadInfo( imp_image_handle *ii )
{
    Cleanup( ii );
    DCClose( ii->sym_file );
}
