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

static void Cleanup( imp_image_handle *iih )
{
    imp_image_handle    **owner;
    imp_image_handle    *curr;
    unsigned            blocks;
    unsigned            i;

    for( owner = &ImageList; (curr = *owner) != NULL; owner = &curr->next_image ) {
        if( curr == iih ) {
            /* if found then remove it from list */
            *owner = curr->next_image;
            break;
        }
    }
    /* destroy entry */
    if( iih->directory != NULL ) {
        blocks = BLOCK_FACTOR( iih->dir_count, DIRECTORY_BLOCK_ENTRIES );
        for( i = 0; i < blocks; ++i ) {
            if( iih->directory[i] != NULL ) {
                DCFree( iih->directory[i] );
            }
        }
        DCFree( iih->directory );
    }
    DCFree( iih->mapping );
    VMFini( iih );
}

static dip_status TryFindPE( FILE *fp, unsigned long *offp, unsigned long *sizep )
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

    if( DCSeek( fp, 0, DIG_ORG ) ) {
        return( DS_ERR | DS_FSEEK_FAILED );
    }
    if( DCRead( fp, &hdr.dos, sizeof( hdr.dos ) ) != sizeof( hdr.dos ) ) {
        return( DS_ERR | DS_FREAD_FAILED );
    }
    if( hdr.dos.signature != DOS_SIGNATURE ) {
        return( DS_FAIL );
    }
    if( DCSeek( fp, NH_OFFSET, DIG_ORG ) ) {
        return( DS_ERR | DS_FSEEK_FAILED );
    }
    if( DCRead( fp, &nh_off, sizeof( nh_off ) ) != sizeof( nh_off ) ) {
        return( DS_ERR | DS_FREAD_FAILED );
    }
    if( DCSeek( fp, nh_off, DIG_ORG ) ) {
        return( DS_ERR | DS_FSEEK_FAILED );
    }
    if( DCRead( fp, &hdr.pe, sizeof( hdr.pe ) ) != sizeof( hdr.pe ) ) {
        return( DS_FAIL );
    }
    if( hdr.pe.signature != PE_SIGNATURE ) {
        return( DS_FAIL );
    }
    if( hdr.pe.table[PE_TBL_DEBUG].rva == 0 ) {
        return( DS_FAIL );
    }
    debug_rva = (hdr.pe.table[PE_TBL_DEBUG].rva / hdr.pe.object_align)*
                                hdr.pe.object_align;

    section_off = nh_off + offsetof( pe_header, flags ) +
                        sizeof( hdr.pe.flags ) + hdr.pe.nt_hdr_size;

    if( DCSeek( fp, section_off, DIG_ORG ) ) {
        return( DS_ERR | DS_FSEEK_FAILED );
    }
    for( i = 0; i < hdr.pe.num_objects; i++ ) {
        if( DCRead( fp, &obj, sizeof( obj ) ) != sizeof( obj ) ) {
            return( DS_ERR | DS_FREAD_FAILED );
        }
        if( obj.rva == debug_rva ) {
            debug_rva = obj.physical_offset +
                            hdr.pe.table[PE_TBL_DEBUG].rva - debug_rva;
            if( DCSeek( fp, debug_rva, DIG_ORG ) ) {
                return( DS_ERR | DS_FSEEK_FAILED );
            }
            if( DCRead( fp, &dir, sizeof( dir ) ) != sizeof( dir ) ) {
                return( DS_ERR | DS_FREAD_FAILED );
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

static dip_status TryFindTrailer( FILE *fp, unsigned long *offp, unsigned long *sizep )
{
    cv_trailer          sig;
    unsigned long       pos;

    if( DCSeek( fp, DIG_SEEK_POSBACK( sizeof( sig ) ), DIG_END ) ) {
        return( DS_ERR | DS_FSEEK_FAILED );
    }
    pos = DCTell( fp );
    if( DCRead( fp, &sig, sizeof( sig ) ) != sizeof( sig ) ) {
        return( DS_ERR | DS_FREAD_FAILED );
    }
    if( memcmp( sig.sig, CV4_NB09, sizeof( sig.sig ) ) != 0 ) {
        return( DS_FAIL );
    }
    *sizep = sig.offset - sizeof( sig );
    *offp = pos - *sizep;
    return( DS_OK );
}

static dip_status FindCV( FILE *fp, unsigned long *offp, unsigned long *sizep )
{
    char        sig[CV_SIG_SIZE];
    dip_status  ds;

    ds = TryFindPE( fp, offp, sizep );
    if( ds & DS_ERR )
        return( ds );
    if( ds != DS_OK ) {
        ds = TryFindTrailer( fp, offp, sizep );
        if( ds != DS_OK ) {
            return( ds );
        }
    }
    if( DCSeek( fp, *offp, DIG_ORG ) ) {
        return( DS_ERR | DS_FSEEK_FAILED );
    }
    if( DCRead( fp, sig, sizeof( sig ) ) != sizeof( sig ) ) {
        return( DS_ERR | DS_FREAD_FAILED );
    }
    if( memcmp( sig, CV4_NB09, sizeof( sig ) ) != 0 ) {
        return( DS_FAIL );
    }
    return( DS_OK );
}

static dip_status LoadDirectory( imp_image_handle *iih, unsigned long off )
{
    unsigned_32                 directory;
    cv_subsection_directory     dir_header;
    unsigned                    block_count;
    unsigned                    i;
    unsigned                    left;
    size_t                      block_size;
    unsigned                    num;

    if( DCSeek( iih->sym_fp, off, DIG_ORG ) ) {
        return( DS_ERR | DS_FSEEK_FAILED );
    }
    if( DCRead( iih->sym_fp, &directory, sizeof( directory ) ) != sizeof( directory ) ) {
        return( DS_ERR | DS_FREAD_FAILED );
    }
    if( DCSeek( iih->sym_fp, iih->bias + directory, DIG_ORG ) ) {
        return( DS_ERR | DS_FSEEK_FAILED );
    }
    if( DCRead( iih->sym_fp, &dir_header, sizeof( dir_header ) ) != sizeof( dir_header ) ) {
        return( DS_ERR | DS_FREAD_FAILED );
    }
    if( dir_header.cbDirHeader != sizeof( dir_header )
      || dir_header.cbDirEntry  != sizeof( cv_directory_entry ) ) {
        return( DS_ERR | DS_INFO_INVALID );
    }
    iih->dir_count = dir_header.cDir;
    block_count = BLOCK_FACTOR( iih->dir_count, DIRECTORY_BLOCK_ENTRIES );
    iih->directory = DCAlloc( block_count * sizeof( cv_directory_entry * ) );
    if( iih->directory == NULL ) {
        return( DS_ERR | DS_NO_MEM );
    }
    memset( iih->directory, 0, block_count * sizeof( cv_directory_entry * ) );
    i = 0;
    left = iih->dir_count;
    for( ;; ) {
        num = left;
        if( num > DIRECTORY_BLOCK_ENTRIES )
            num = DIRECTORY_BLOCK_ENTRIES;
        block_size = num * sizeof( cv_directory_entry );
        iih->directory[i] = DCAlloc( block_size );
        if( iih->directory[i] == NULL ) {
            return( DS_ERR | DS_NO_MEM );
        }
        if( DCRead( iih->sym_fp, iih->directory[i], block_size ) != block_size ) {
            return( DS_ERR | DS_FREAD_FAILED );
        }
        ++i;
        left -= num;
        if( left == 0 ) {
            break;
        }
    }
    return( DS_OK );
}

static dip_status LoadMapping( imp_image_handle *iih )
{
    cv_directory_entry  *cde;
    cv_sst_seg_map      *map;
    size_t              size;

    cde = FindDirEntry( iih, IMH_GBL, sstSegMap );
    if( cde == NULL )
        return( DS_ERR | DS_INFO_INVALID );
    map = VMBlock( iih, cde->lfo, cde->cb );
    if( map == NULL )
        return( DS_ERR | DS_FAIL );
    size = map->cSegLog * sizeof( map->segdesc[0] );
    iih->mapping = DCAlloc( size );
    if( iih->mapping == NULL )
        return( DS_ERR | DS_NO_MEM );
    map = VMBlock( iih, cde->lfo, cde->cb ); /* malloc might have unloaded */
    memcpy( iih->mapping, &map->segdesc[0], size );
    iih->map_count = map->cSegLog;
    return( DS_OK );
}

static walk_result FindCompUnit( imp_image_handle *iih,
                                cv_directory_entry *cde, void *d )
{
    cs_compile          **rec = d;

    if( cde->subsection != sstModule )
        return( WR_CONTINUE );
    *rec = GetCompInfo( iih, cde->iMod );
    if( *rec == NULL )
        return( WR_CONTINUE );
    return( WR_STOP );
}

static dip_status SetMADType( imp_image_handle *iih )
{
    cs_compile                  *rec;
    walk_result                 wr;

    wr = WalkDirList( iih, &FindCompUnit, &rec );
    if( wr != WR_STOP )
        return( DS_OK );
    switch( rec->machine & 0xf0 ) {
    case MACH_INTEL_8080:
        iih->arch = DIG_ARCH_X86;
        break;
    case MACH_DECALPHA:
        iih->arch = DIG_ARCH_AXP;
        break;
    default:
        return( DS_ERR | DS_INFO_INVALID );
    }
    return( DS_OK );
}

dip_status DIPIMPENTRY( LoadInfo )( FILE *fp, imp_image_handle *iih )
{
    dip_status                  ds;
    unsigned long               off;
    unsigned long               size;
    cv_directory_entry          *cde;
    cv_sst_global_types_header  *hdr;

    memset( iih, 0, sizeof( *iih ) );
    ds = FindCV( fp, &off, &size );
    if( ds != DS_OK )
        return( ds );
    iih->bias = off;
    ds = VMInit( iih, size );
    if( ds != DS_OK )
        return( ds );
    iih->sym_fp = fp;
    iih->next_image = ImageList;
    ImageList = iih;
    ds = LoadDirectory( iih, off + CV_SIG_SIZE );
    if( ds == DS_OK ) {
        ds = LoadMapping( iih );
        if( ds == DS_OK ) {
            cde = FindDirEntry( iih, IMH_GBL, sstGlobalTypes );
            if( cde != NULL ) {
                hdr = VMBlock( iih, cde->lfo, sizeof( *hdr ) );
                if( hdr == NULL ) {
                    ds = DS_ERR | DS_FAIL;
                } else {
                    iih->types_base = cde->lfo
                        + offsetof(cv_sst_global_types_header, offType )
                        + hdr->cType * sizeof( hdr->offType[0] );
                }
            }
            if( ds == DS_OK ) {
                ds = SetMADType( iih );
                if( ds == DS_OK ) {
                    return( DS_OK );
                }
            }
        }
    }
    iih->sym_fp = NULL;
    DCStatus( ds );
    Cleanup( iih );
    return( ds );
}

void DIPIMPENTRY( MapInfo )( imp_image_handle *iih, void *d )
{
    unsigned    i;
    addr_ptr    addr;

    for( i = 0; i < iih->map_count; ++i ) {
        addr.segment = iih->mapping[i].frame;
        addr.offset  = iih->mapping[i].offset;
        DCMapAddr( &addr, d );
        iih->mapping[i].frame  = addr.segment;
        iih->mapping[i].offset = addr.offset;
    }
}

dip_status SegIsExecutable( imp_image_handle *iih, unsigned log )
{
    seg_desc    *map;

    map = &iih->mapping[log-1];
    return( map->u.b.fExecute ? DS_OK : DS_FAIL );
}

void MapLogical( imp_image_handle *iih, address *a )
{
    seg_desc    *map;

    map = &iih->mapping[a->mach.segment-1];
    a->mach.segment = map->frame;
    a->mach.offset += map->offset;
    a->sect_id = map->ovl;
    a->indirect = 1;
}

void DIPIMPENTRY( UnloadInfo )( imp_image_handle *iih )
{
    Cleanup( iih );
}
