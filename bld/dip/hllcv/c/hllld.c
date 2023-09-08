/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  HLL/CV symbolic information loading and unloading.
*
****************************************************************************/


#include "hllinfo.h"
#include "bool.h"
#include "exepe.h"
#include "exeflat.h"
#include "exedos.h"
#include "exeos2.h"


typedef union {
    char            signature[HLL_SIG_SIZE]; /* ASSUMES >= 4 */
    unsigned_16     signature16;
    unsigned_32     signature32;
    pe_exe_header   pe;
    pe_object       sh;
    hll_tailer      hdr;
    debug_directory dbg_dir;
}               hll_buf;

/*
 * WD looks for this symbol to determine module bitness
 */
int __nullarea;
#ifdef __WATCOMC__
#pragma aux __nullarea "*";
#endif

imp_image_handle        *ImageList;

static void Cleanup( imp_image_handle *iih )
/*******************************************
 * Frees resources associated with a image handle.
 */
{
    imp_image_handle    **owner;
    imp_image_handle    *curr;
    unsigned            blocks;
    unsigned            i;

    /*
     * unlink it
     */
    for( owner = &ImageList; (curr = *owner) != NULL; owner = &curr->next_image ) {
        if( curr == iih ) {
            *owner = curr->next_image;
            break;
        }
    }
    /*
     * free memory
     */
    if( iih->directory != NULL ) {
        blocks = BLOCK_FACTOR( iih->dir_count, DIRECTORY_BLOCK_ENTRIES );
        for( i = 0; i < blocks; ++i ) {
            if( iih->directory[i] != NULL ) {
                DCFree( iih->directory[i] );
            }
        }
        DCFree( iih->directory );
    }
    DCFree( iih->segments );
    VMFini( iih );
}

static dip_status LoadDirectory( imp_image_handle *iih, unsigned long offent )
/*****************************************************************************
 * Loads the HLL directory.
 *
 * 'offent' is the file offset of the first directory entry.
 */
{
    unsigned                block_count;
    unsigned                i;
    unsigned                left;
    size_t                  block_size;
    unsigned                num;

    /*
     * Read the directory entries. We're using a two-level table here,
     * probably to avoid allocating big chunks of memory...
     */
    block_count = BLOCK_FACTOR( iih->dir_count, DIRECTORY_BLOCK_ENTRIES );
    iih->directory = DCAlloc( block_count * sizeof( void * ) );
    if( iih->directory == NULL ) {
        return( DS_ERR | DS_NO_MEM );
    }
    memset( iih->directory, 0, block_count * sizeof( void * ) );
    /*
     * skip to the first entry
     */
    if( DCSeek( iih->sym_fp, offent, DIG_SEEK_ORG ) ) {
        return( DS_ERR | DS_FSEEK_FAILED );
    }

    i = 0;
    left = iih->dir_count;
    do {
        num = left;
        if( num > DIRECTORY_BLOCK_ENTRIES ) {
            num = DIRECTORY_BLOCK_ENTRIES;
        }
        block_size = num * sizeof( hll_dir_entry );
        iih->directory[i] = DCAlloc( block_size );
        if( iih->directory[i] == NULL ) {
            return( DS_ERR | DS_NO_MEM );
        }
        if( iih->format_lvl >= HLL_LVL_NB04 ) {
            if( DCRead( iih->sym_fp, iih->directory[i], block_size ) != block_size ) {
                return( DS_ERR | DS_FREAD_FAILED );
            }
        } else {
            /*
             * Slow but simple.
             */
            unsigned        j;
            hll_dir_entry   *ent = iih->directory[i];

            for( j = 0; j < num; j++, ent++ ) {
                cv3_dir_entry cv3ent;
                if( DCRead( iih->sym_fp, &cv3ent, sizeof( cv3ent ) ) != sizeof( cv3ent ) ) {
                    return( DS_ERR | DS_FREAD_FAILED );
                }
                ent->subsection = cv3ent.subsection;
                ent->iMod       = cv3ent.iMod;
                ent->lfo        = cv3ent.lfo;
                ent->cb         = cv3ent.cb;
            }
        }
        ++i;
        left -= num;
    } while( left != 0 );
    return( DS_OK );
}

static bool IsHllSignature( hll_trailer *hdr )
/*********************************************
 * Checks for HLL signature.
 * (BUF must pointer to 4 valid bytes.)
 */
{
    return( !memcmp( hdr, HLL_NB04, HLL_SIG_SIZE )
         || !memcmp( hdr, HLL_NB02, HLL_SIG_SIZE )
         || !memcmp( hdr, HLL_NB00, HLL_SIG_SIZE ) );
}

static dip_status FoundHLLSignature( imp_image_handle *iih, unsigned long off, unsigned long size )
/**************************************************************************************************
 * Validates the signatures of a HLL debug info block, determining
 * the length if necessary.
 */
{
    dip_status          ds;
    hll_trailer         hdr;
    unsigned long       off_dirent, off_trailer;

    /*
     * read the header.
     */
    ds = DCReadAt( iih->sym_fp, &hdr, sizeof( hdr ), off );
    if( ds & DS_ERR) {
        return( ds );
    }
    if( !IsHllSignature( &hdr ) ) {
        return( DS_FAIL );
    }
    /*
     * Read the directory info - both to verify it and to find the trailer.
     */
    off_dirent = off + hdr.offset;
    if( !memcmp( hdr.signature, HLL_NB04, HLL_SIG_SIZE ) ) {
        hll_dirinfo     dir_hdr;

        ds = DCReadAt( iih->sym_fp, &dir_hdr, sizeof( dir_hdr ), off_dirent );
        if( ds & DS_ERR) {
            return( ds );
        }
        if( dir_hdr.cbDirHeader != sizeof( hll_dirinfo )
         || dir_hdr.cbDirEntry != sizeof( hll_dir_entry ) ) {
            return( DS_FAIL );
        }
        iih->dir_count = dir_hdr.cDir;
        off_dirent += sizeof( dir_hdr );
        off_trailer = off_dirent + sizeof( hll_dir_entry ) * dir_hdr.cDir;
    } else {
        cv3_dirinfo     dir_hdr;

        /*
         * Old CV3 directory.
         */
        ds = DCReadAt( iih->sym_fp, &dir_hdr, sizeof( dir_hdr ), off_dirent );
        if( ds & DS_ERR) {
            return( ds );
        }
        iih->dir_count = dir_hdr.cDir;
        off_dirent += sizeof( dir_hdr );
        off_trailer = off_dirent + sizeof( cv3_dir_entry ) * dir_hdr.cDir;
    }
    /*
     * is the trailer following the directory? It usually is with wlink.
     */
    ds = DCReadAt( iih->sym_fp, &hdr, sizeof( hdr ), off_trailer );
    if( ds & DS_ERR) {
        return( ds );
    }
    if( !IsHllSignature( &hdr ) ) {
        unsigned long   cur;
        unsigned        overlap = 0;

        /*
         * No it isn't, seek from the end (off + size).
         * Adjust the length first.
         */
        DCSeek( iih->sym_fp, 0, DIG_SEEK_END );
        cur = DCTell( iih->sym_fp );
        if( cur > size + off && size + off > size ) {
            cur = off + size;
        }

        hdr.signature[0] = 0;
        do {
            char        buf[1024 + sizeof( hdr )];
            size_t      to_read;
            char        *ptr;

            /*
             * read block
             */
            to_read = 1024 + overlap;
            cur    -= 1024;
            if( cur  < off_trailer ) {
                to_read += off_trailer - cur;
                cur = off_trailer;
            }
            if( to_read < sizeof( hdr) ) {
                return( DS_FAIL );
            }
            ds = DCReadAt( iih->sym_fp, buf, to_read, cur );
            if( ds & DS_ERR ) {
                return( ds );
            }
            /*
             * search it
             */
            for( ptr = &buf[to_read - sizeof( hdr )]; ptr >= &buf[0]; ptr-- ) {
                if( IsHllSignature( (hll_trailer *)ptr ) ) {
                    off_trailer = cur + ptr - &buf[0];
                    hdr = *(hll_trailer *)ptr;
                    break;
                }
            }
            /*
             * next
             */
            overlap = sizeof( hdr );
        } while( hdr.signature[0] == 0 );
    }
    /*
     * Validate the trailer offset (=size).
     */
    if( off_trailer == off
     || hdr.offset != off_trailer - off + sizeof( hdr ) ) {
        return( DS_FAIL );
    }
    /*
     * We're good.
     */
    iih->bias = off;
    iih->size = off_trailer - off + sizeof( hdr );
    if( !memcmp( hdr.signature, HLL_NB04, HLL_SIG_SIZE ) ) {
        iih->format_lvl = HLL_LVL_NB04;
    } else if( !memcmp( hdr.signature, HLL_NB02, HLL_SIG_SIZE ) ) {
        iih->format_lvl = HLL_LVL_NB02;
    } else if( !memcmp( hdr.signature, HLL_NB00, HLL_SIG_SIZE ) ) {
        iih->format_lvl = iih->is_32bit ? HLL_LVL_NB00_32BIT : HLL_LVL_NB00;
    } else {
        hllConfused();
    }
    /*
     * Since we already know where the directory is, we load it here.
     */
    ds = LoadDirectory( iih, off_dirent );
    return( ds );
}

static dip_status FindHLLInPEImage( imp_image_handle *iih, unsigned long ne_header_off )
/***************************************************************************************
 * Deals with 32-bit PE images.
 */
{
    dip_status          ds;
    unsigned_32         debug_rva;
    unsigned_32         debug_len;
    unsigned_32         image_base;
    unsigned            i;
    unsigned_32         sh_off;
    hll_buf             buf;

    /*
     * read the header
     */
    ds = DCReadAt( iih->sym_fp, &buf.pe, PE_HDR_SIZE, ne_header_off );
    if( ds & DS_ERR ) {
        return( ds );
    }
    ds = DCReadAt( iih->sym_fp, (char *)&buf.pe + PE_HDR_SIZE, PE_OPT_SIZE( buf.pe ), ne_header_off + PE_HDR_SIZE );
    if( ds & DS_ERR ) {
        return( ds );
    }

    debug_rva = PE_DIRECTORY( buf.pe, PE_TBL_DEBUG ).rva;
    debug_len = PE_DIRECTORY( buf.pe, PE_TBL_DEBUG ).size;
    if( !debug_rva || !debug_len ) {
        return( DS_FAIL );
    }
    iih->is_32bit = 1;
    image_base = PE( buf.pe, image_base );
    /*
     * Translate the rva to a file offset and read necessary
     * segment information at the same time.
     */
    iih->seg_count = buf.pe.fheader.num_objects;
    iih->segments = DCAlloc( sizeof( hllinfo_seg ) * buf.pe.fheader.num_objects );
    if( iih->segments == NULL ) {
        return( DS_ERR | DS_NO_MEM );
    }

    sh_off = ne_header_off + PE_SIZE( buf.pe );
    for( i = 0; i < iih->seg_count; i++, sh_off += sizeof( buf.sh ) ) {
        ds = DCReadAt( iih->sym_fp, &buf.sh, sizeof( buf.sh ), sh_off );
        if( ds & DS_ERR ) {
            return( ds );
        }
        /*
         * collect segment info.
         */
        iih->segments[i].is_executable = !!( buf.sh.flags & PE_OBJ_CODE );
        iih->segments[i].ovl = 0;
        iih->segments[i].map.offset = 0;
        iih->segments[i].map.segment = i + 1;
        iih->segments[i].size = buf.sh.virtual_size; // FIXME: alignment?
        iih->segments[i].address = buf.sh.rva + image_base;
        /*
         * is the debug directory section?
         */
        if( !iih->bias
         && debug_rva - buf.sh.rva < buf.sh.virtual_size ) {
            unsigned_32 debug_off;
            int         left;

            debug_off = buf.sh.physical_offset + debug_rva - buf.sh.rva;
            /*
             * The IBM linker screw up here. It will omit the debug
             * directory and put the debug info there instead.
             * So, before scanning we'll have to check for any HLL sign.
             */
            ds = DCReadAt( iih->sym_fp, &buf.dbg_dir, sizeof( buf.dbg_dir ), debug_off );
            if( ds & DS_ERR ) {
                return( ds );
            }
            if( IsHllSignature( &buf.hdr ) ) {
                ds = FoundHLLSignature( iih, debug_off, debug_len );
            } else {
                left = debug_len / sizeof( debug_directory );
                if( left < 16 )
                    left = 16;
                for( ;; ) {
                    if( buf.dbg_dir.debug_type == DEBUG_TYPE_CODEVIEW ) {
                        /*
                         * found something?
                         */
                        ds = FoundHLLSignature( iih, buf.dbg_dir.data_seek, buf.dbg_dir.data_seek );
                        if( ds == DS_OK || ds & DS_ERR ) {
                            break;
                        }
                    }
                    /*
                     * next
                     */
                    --left;
                    if( left <= 0) {
                        break;
                    }
                    if( DCRead( iih->sym_fp, &buf.dbg_dir, sizeof( buf.dbg_dir ) ) != sizeof( buf.dbg_dir ) ) {
                        break;
                    }
                }
            }
            if( ds & DS_ERR ) {
                return( ds );
            }
        }
    }

    return( iih->bias ? DS_OK : DS_FAIL );
}

static dip_status FindHLLInLXImage( imp_image_handle *iih, unsigned long ne_header_off )
/***************************************************************************************
 * Deals with LX and LE images.
 * We must try grab information from the object table.
 */
{
    union  {
        os2_flat_header flat;
        object_record   obj;
    }                   buf;
    dip_status          ds;

    /*
     * read the header
     */
    ds = DCReadAt( iih->sym_fp, &buf.flat, sizeof( buf.flat ), ne_header_off );
    if( ds & DS_ERR ) {
        return( ds );
    }
    ds = DS_FAIL;
    if( buf.flat.debug_off && buf.flat.debug_len ) {
        iih->is_32bit = 1;
        ds = FoundHLLSignature( iih, buf.flat.debug_off, buf.flat.debug_len );
        if( ds == DS_OK ) {
            unsigned i;

            /*
             * Get segment info from the object table.
             */
            if( DCSeek( iih->sym_fp, buf.flat.objtab_off + ne_header_off, DIG_SEEK_ORG ) ) {
                return( DS_ERR | DS_FSEEK_FAILED );
            }

            iih->seg_count = buf.flat.num_objects;
            iih->segments = DCAlloc( sizeof( hllinfo_seg ) * iih->seg_count );
            if( iih->segments == NULL ) {
                return( DS_ERR | DS_NO_MEM );
            }

            for( i = 0; i < iih->seg_count; i++ ) {
                if( DCRead( iih->sym_fp, &buf.obj, sizeof( buf.obj ) ) != sizeof( buf.obj )) {
                    return( DS_ERR | DS_FREAD_FAILED );
                }
                iih->segments[i].is_executable = !!( buf.obj.flags & OBJ_EXECUTABLE );
                iih->segments[i].is_16bit = !( buf.obj.flags & OBJ_BIG );
                iih->segments[i].ovl = 0;
                iih->segments[i].map.offset = 0;
                iih->segments[i].map.segment = i + 1;
                iih->segments[i].size = buf.obj.size;
                iih->segments[i].address = buf.obj.addr;
            }
        }
    }
    return( ds );
}

static dip_status TryFindInImage( imp_image_handle *iih )
/********************************************************
 * Tries to find the HLL/CV debug info in a image which format we know.
 * This function knows about LX, LE, PE and watcom symfile images.
 */
{
    hll_buf             buf;
    bool                have_mz_header = false;
    unsigned_32         ne_header_off;
    dip_status          ds;

    /*
     * Read the image header.
     */
    ds = DCReadAt( iih->sym_fp, &buf.signature, sizeof( buf.signature ), 0 );
    if( ds & DS_ERR ) {
        return( ds );
    }
    ne_header_off = 0;
    if( buf.signature16 == DOS_SIGNATURE ) {
        have_mz_header = true;
        /*
         * read the new exe header
         */
        ds = DCReadAt( iih->sym_fp, &ne_header_off, sizeof( ne_header_off ), NE_HEADER_OFFSET );
        if( ds & DS_ERR ) {
            return( ds );
        }
        ds = DCReadAt( iih->sym_fp, &buf.signature, sizeof( buf.signature ), ne_header_off );
        if( ds & DS_ERR ) {
            return( ds );
        }
    }
    /*
     * LE/LX executable - Use the debug_off/len members of the header.
     */
    if( buf.signature16 == EXESIGN_LX
     || buf.signature16 == EXESIGN_LE ) {
        return( FindHLLInLXImage( iih, ne_header_off ) );
    }
    /*
     * PE executable - Use or scan the debug directory.
     */
    if( buf.signature32 == EXESIGN_PE ) {
        return( FindHLLInPEImage( iih, ne_header_off ) );
    }
    /*
     * NE and MZ executables do not contain any special information
     * in the header. TryFindTrailer() will have to pick up the debug data.
     */
    if( (buf.signature16 == EXESIGN_NE) || have_mz_header ) {
        iih->is_32bit = 0;
    }
    /*
     * ELF executable - ??????
     */
    /*
     * A watcom .sym file.
     */
    if( IsHllSignature( &buf.hdr ) ) {
        return( FoundHLLSignature( iih, ne_header_off, ~0UL ) );
    }
    /*
     * no idea what this is..
     */
    return( DS_FAIL );
}

static dip_status TryFindTrailer( imp_image_handle *iih )
/********************************************************
 * Checks if there is a tailing HLL signature in the file.
 *
 * This may work not work on executable images because of file alignments
 * imposed by the linker (ilink at least). However, TryFindInImage will
 * deal with those formats, so it doesn't really matter here.
 */
{
    hll_trailer         hdr;
    unsigned long       pos;

    if( DCSeek( iih->sym_fp, DIG_SEEK_POSBACK( sizeof( hdr ) ), DIG_SEEK_END ) ) {
        return( DS_ERR | DS_FSEEK_FAILED );
    }
    pos = DCTell( iih->sym_fp );
    if( DCRead( iih->sym_fp, fp & hdr, sizeof( hdr ) ) != sizeof( hdr ) ) {
        return( DS_ERR | DS_FREAD_FAILED );
    }
    if( !IsHllSignature( &hdr ) ) {
        return( DS_FAIL );
    }

    pos -= hdr.offset - sizeof( hdr );
    return( FoundHLLSignature( iih, pos, hdr.offset - sizeof( hdr ) ) );
}

static dip_status FindHLL( imp_image_handle *iih )
/*************************************************
 * Tries to locate HLL debug info within the specified file.
 *
 * Returns DS_OK if found, with '*offp' set to the offset of the debug info
 * (relative to the start of the file) and '*sizep' set to the size of it.
 */
{
    dip_status      ds;

    ds = TryFindInImage( iih );
    if( ds & DS_ERR )
        return( ds );
    if( ds != DS_OK ) {
        ds = TryFindTrailer( iih );
    }
    return( ds );
}

#if 0
static walk_result FindCompUnit( imp_image_handle *iih,
                                 hll_dir_entry *cde, void *d )
{
    cs_compile          **rec = d;

    if( cde->subsection != hll_sstModules )
        return( WR_CONTINUE );
    *rec = GetCompInfo( iih, cde->iMod );
    if( *rec == NULL )
        return( WR_CONTINUE );
    return( WR_STOP );
}
#endif

dip_status DIPIMPENTRY( LoadInfo )( FILE *fp, imp_image_handle *iih )
/********************************************************************
 * Load debug info if it's in the HLL/CV format.
 */
{
    dip_status      ds;

    /*
     * Init the module handle.
     */
    memset( iih, 0, sizeof( *iih ) );
    iih->arch = DIG_ARCH_X86;                  /* No known non-x86 support */
    iih->sym_fp = fp;
    iih->is_32bit = 1;
    iih->format_lvl = HLL_LVL_NB04;
    iih->next_image = ImageList;
    ImageList = iih;
    /*
     * Read basic HLL and executable image bits.
     */
    ds = FindHLL( iih );
    if( ds == DS_OK ) {
        ds = VMInit( iih, iih->size );
    }
    /*
     * Make sure we've got segment mappings. (FIXME: scan module headers)
     */
    if( ds == DS_OK && !iih->segments ) {
        iih->seg_count = 2;
        iih->segments = DCAlloc( sizeof( hllinfo_seg ) * iih->seg_count );
        if( iih->segments != NULL ) {
            memset( iih->segments, 0, sizeof( hllinfo_seg ) * iih->seg_count );
            iih->segments[0].is_executable = 1;
            iih->segments[0].map.segment = 1;
            iih->segments[1].map.segment = 2;
            iih->segments[0].size = iih->segments[1].size = 0x00400000; /* 4MB */
            iih->segments[0].address = iih->segments[1].address = 0;
        } else{
            ds = DS_ERR | DS_NO_MEM;
        }
    }
    /*
     * Locate global types. If none, we've types per module.
     */
    if( ds == DS_OK ) {
#if 0
/* FIXME */
        hll_debug_dir *cde;
        cde = FindDirEntry( iih, IMH_GBL, sstGlobalTypes );
        if( cde != NULL ) {
            hdr = VMBlock( iih, cde->lfo, sizeof( *hdr ) );
            if( hdr == NULL ) {
                Cleanup( iih );
                return( DS_ERR | DS_FAIL );
            }
            iih->types_base = cde->lfo
                + offsetof( cv_sst_global_types_header, offType )
                + hdr->cType * sizeof( hdr->offType[0] );
        }
#endif
    }
    iih->sym_fp = NULL;
    /*
     * We're done - clean up on failure.
     */
    if( ds != DS_OK ) {
        DCStatus( ds );
        Cleanup( iih );
    }
    return( ds );
}

void DIPIMPENTRY( MapInfo )( imp_image_handle *iih, void *d )
/************************************************************
 * Lets the DIP user setup segment mappings.
 */
{
    unsigned    i;

    for( i = 0; i < iih->seg_count; i++ ) {
        DCMapAddr( &iih->segments[i].map, d );
    }
}

bool hllIsSegExecutable( imp_image_handle *iih, unsigned segment )
/*****************************************************************
 * Checks if a segment is executable.
 */
{
    if( segment <= iih->seg_count && iih->segments[segment - 1].is_executable ) {
        return( true );
    }
    return( false );
}

void hllMapLogical( imp_image_handle *iih, address *a )
/******************************************************
 * Maps an address.
 */
{
    if( a->mach.segment <= iih->seg_count ) {
        hllinfo_seg *seg = &iih->segments[a->mach.segment - 1];
        a->mach.segment = seg->map.segment;
        a->mach.offset += seg->map.offset;
        a->sect_id = seg->ovl;
        a->indirect = 1;
    }
}

void DIPIMPENTRY( UnloadInfo )( imp_image_handle *iih )
/******************************************************
 * Free the image.
 */
{
    Cleanup( iih );
}
