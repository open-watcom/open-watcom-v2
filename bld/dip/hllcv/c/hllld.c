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
* Description:  HLL/CV symbolic information loading and unloading.
*
****************************************************************************/


#include "hllinfo.h"
#include "bool.h"
#include "exepe.h"
#include "exeflat.h"
#include "exedos.h"
#include "exeos2.h"


/* WD looks for this symbol to determine module bitness */
int __nullarea;
#pragma aux __nullarea "*";

imp_image_handle        *ImageList;

/*
 * Frees resources associated with a image handle.
 */
static void Cleanup( imp_image_handle *ii )
{
    imp_image_handle    **owner;
    imp_image_handle    *curr;
    unsigned            blocks;
    unsigned            i;

    /* unlink it */
    owner = &ImageList;
    for( ;; ) {
        curr = *owner;
        if( curr == ii ) {
            break;
        }
        owner = &curr->next_image;
    }
    *owner = ii->next_image;

    /* free memory */
    if( ii->directory != NULL ) {
        blocks = BLOCK_FACTOR( ii->dir_count, DIRECTORY_BLOCK_ENTRIES );
        for( i = 0; i < blocks; ++i ) {
            if( ii->directory[i] != NULL ) {
                DCFree( ii->directory[i] );
            }
        }
        DCFree( ii->directory );
    }
    DCFree( ii->segments );
    VMFini( ii );
}

/*
 * Loads the HLL directory.
 *
 * 'offent' is the file offset of the first directory entry.
 */
static dip_status LoadDirectory( imp_image_handle *ii, unsigned long offent )
{
    unsigned                block_count;
    unsigned                i;
    unsigned                left;
    unsigned                block_size;
    unsigned                num;

    /*
     * Read the directory entries. We're using a two-level table here,
     * probably to avoid allocating big chunks of memory...
     */
    block_count = BLOCK_FACTOR( ii->dir_count, DIRECTORY_BLOCK_ENTRIES );
    ii->directory = DCAlloc( block_count * sizeof( void * ) );
    if( ii->directory == NULL ) {
        return( DS_ERR | DS_NO_MEM );
    }
    memset( ii->directory, 0, block_count * sizeof( void * ) );

    /* skip to the first entry */
    if( DCSeek( ii->sym_file, offent, DIG_ORG ) != offent) {
        return( DS_ERR | DS_FSEEK_FAILED );
    }

    i = 0;
    left = ii->dir_count;
    do {
        num = left;
        if( num > DIRECTORY_BLOCK_ENTRIES ) {
            num = DIRECTORY_BLOCK_ENTRIES;
        }
        block_size = num * sizeof( hll_dir_entry );
        ii->directory[i] = DCAlloc( block_size );
        if( ii->directory[i] == NULL ) {
            return( DS_ERR | DS_NO_MEM );
        }
        if( ii->format_lvl >= HLL_LVL_NB04 ) {
            if( DCRead( ii->sym_file, ii->directory[i], block_size ) != block_size ) {
                return( DS_ERR | DS_FREAD_FAILED );
            }
        } else {
            /* Slow but simple. */
            int             j;
            hll_dir_entry  *ent = ii->directory[i];

            for( j = 0; j < num; j++, ent++ ) {
                cv3_dir_entry cv3ent;
                if( DCRead( ii->sym_file, &cv3ent, sizeof( cv3ent ) ) != sizeof( cv3ent ) ) {
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

/*
 * Checks for HLL signature.
 * (BUF must pointer to 4 valid bytes.)
 */
static bool IsHllSignature( void *buf )
{
    return( !memcmp( buf, HLL_NB04, HLL_SIG_SIZE )
         || !memcmp( buf, HLL_NB02, HLL_SIG_SIZE )
         || !memcmp( buf, HLL_NB00, HLL_SIG_SIZE ) );
}

/*
 * Validates the signatures of a HLL debug info block, determining
 * the length if necessary.
 */
static dip_status FoundHLLSign( imp_image_handle *ii, unsigned long off,
                                unsigned long size )
{
    dip_status          rc;
    hll_trailer         hdr;
    unsigned long       off_dirent, off_trailer;

    /* read the header. */
    rc = DCReadAt( ii->sym_file, &hdr, sizeof( hdr ), off );
    if( rc & DS_ERR) {
        return rc;
    }
    if( !IsHllSignature( &hdr ) ) {
        return DS_FAIL;
    }

    /*
     * Read the directory info - both to verify it and to find the trailer.
     */
    off_dirent = off + hdr.offset;
    if( !memcmp( hdr.sig, HLL_NB04, HLL_SIG_SIZE ) ) {
        hll_dirinfo     dir_hdr;

        rc = DCReadAt( ii->sym_file, &dir_hdr, sizeof( dir_hdr ), off_dirent );
        if( rc & DS_ERR) {
            return rc;
        }
        if( dir_hdr.cbDirHeader != sizeof( hll_dirinfo )
         || dir_hdr.cbDirEntry != sizeof( hll_dir_entry ) ) {
            return DS_FAIL;
        }
        ii->dir_count = dir_hdr.cDir;
        off_dirent += sizeof( dir_hdr );
        off_trailer = off_dirent + sizeof( hll_dir_entry ) * dir_hdr.cDir;
    } else {
        /* Old CV3 directory. */
        cv3_dirinfo     dir_hdr;

        rc = DCReadAt( ii->sym_file, &dir_hdr, sizeof( dir_hdr ), off_dirent );
        if( rc & DS_ERR) {
            return rc;
        }
        ii->dir_count = dir_hdr.cDir;
        off_dirent += sizeof( dir_hdr );
        off_trailer = off_dirent + sizeof( cv3_dir_entry ) * dir_hdr.cDir;
    }

    /* is the trailer following the directory? It usually is with wlink. */
    rc = DCReadAt( ii->sym_file, &hdr, sizeof( hdr ), off_trailer );
    if( rc & DS_ERR) {
        return rc;
    }
    if( !IsHllSignature( &hdr ) ) {
        /*
         * No it isn't, seek from the end (off + size).
         * Adjust the length first.
         */
        long        cur;
        unsigned    overlap = 0;

        cur = DCSeek( ii->sym_file, 0, DIG_END );
        if( cur > size + off && size + off > size ) {
            cur = off + size;
        }

        hdr.sig[0] = 0;
        do {
            char        buf[1024 + sizeof( hdr )];
            unsigned    to_read;
            char       *ptr;

            /* read block */
            to_read = 1024 + overlap;
            cur    -= 1024;
            if( cur  < off_trailer ) {
                to_read += off_trailer - cur;
                cur = off_trailer;
            }
            if( to_read < sizeof( hdr) ) {
                return DS_FAIL;
            }
            rc = DCReadAt( ii->sym_file, buf, to_read, cur );
            if( rc & DS_ERR ) {
                return rc;
            }

            /* search it */
            for( ptr = &buf[to_read - sizeof( hdr )];
                 ptr >= &buf[0];
                 ptr--) {
                if( IsHllSignature(ptr) ) {
                    off_trailer = cur + ptr - &buf[0];
                    hdr = *(hll_trailer *)ptr;
                    break;
                }
            }

            /* next */
            overlap = sizeof( hdr );
        } while( hdr.sig[0] == 0 );
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
    ii->bias = off;
    ii->size = off_trailer - off + sizeof( hdr );
    if ( !memcmp( hdr.sig, HLL_NB04, HLL_SIG_SIZE ) ) {
        ii->format_lvl = HLL_LVL_NB04;
    } else if ( !memcmp( hdr.sig, HLL_NB02, HLL_SIG_SIZE ) ) {
        ii->format_lvl = HLL_LVL_NB02;
    } else if( !memcmp( hdr.sig, HLL_NB00, HLL_SIG_SIZE ) ) {
        ii->format_lvl = ii->is_32bit ? HLL_LVL_NB00_32BIT : HLL_LVL_NB00;
    } else {
        hllConfused();
    }

    /*
     * Since we already know where the directory is, we load it here.
     */
    rc = LoadDirectory( ii, off_dirent );
    return( rc );
}

/*
 * Deals with 32-bit PE images.
 */
static dip_status FindHLLInPEImage( imp_image_handle *ii, unsigned long nh_off )
{
    dip_status          rc;
    unsigned_32         debug_rva;
    unsigned_32         debug_len;
    unsigned_32         image_base;
    unsigned            i;
    unsigned_32         sh_off;
    union {
        char            sig[HLL_SIG_SIZE]; /* ASSUMES >= 4 */
        unsigned_16     sig_16;
        unsigned_32     sig_32;
        pe_header       pe;
        pe_object       sh;
        debug_directory dbg_dir;
    }                   buf;

    /* read the header */
    rc = DCReadAt( ii->sym_file, &buf.pe, sizeof( buf.pe ), nh_off );
    if( rc & DS_ERR ) {
        return( rc );
    }

    debug_rva = buf.pe.table[ PE_TBL_DEBUG ].rva;
    debug_len = buf.pe.table[ PE_TBL_DEBUG ].size;
    if( !debug_rva || !debug_len ) {
        return( DS_FAIL );
    }
    ii->is_32bit = 1;
    image_base = buf.pe.image_base;

    /*
     * Translate the rva to a file offset and read necessary
     * segment information at the same time.
     */
    ii->seg_count = buf.pe.num_objects;
    ii->segments = DCAlloc( sizeof( hllinfo_seg ) * buf.pe.num_objects );
    if( ii->segments == NULL ) {
        return( DS_ERR | DS_NO_MEM );
    }

    sh_off = nh_off /* vv -- watcom mixes the headers :-( */
           + offsetof( pe_header, flags ) + sizeof( buf.pe.flags )
           + buf.pe.nt_hdr_size;

    for ( i = 0; i < ii->seg_count; i++, sh_off += sizeof( buf.sh ) ) {
        rc = DCReadAt( ii->sym_file, &buf.sh, sizeof( buf.sh ), sh_off );
        if ( rc & DS_ERR ) {
            return( rc );
        }

        /* collect segment info. */
        ii->segments[i].is_executable = !!( buf.sh.flags & PE_OBJ_CODE );
        ii->segments[i].ovl = 0;
        ii->segments[i].map.offset = 0;
        ii->segments[i].map.segment = i + 1;
        ii->segments[i].size = buf.sh.virtual_size; // FIXME: alignment?
        ii->segments[i].address = buf.sh.rva + image_base;

        /* is the debug directory section? */
        if( !ii->bias
         && debug_rva - buf.sh.rva < buf.sh.virtual_size ) {
            unsigned_32 debug_off;
            int         left;
            debug_off = buf.sh.physical_offset + debug_rva - buf.sh.rva;

            /*
             * The IBM linker screw up here. It will omit the debug
             * directory and put the debug info there instead.
             * So, before scanning we'll have to check for any HLL sign.
             */
            rc = DCReadAt( ii->sym_file, &buf.dbg_dir, sizeof( buf.dbg_dir ), debug_off );
            if( rc & DS_ERR ) {
                return( rc );
            }
            if( IsHllSignature( &buf ) ) {
                rc = FoundHLLSign( ii, debug_off, debug_len );
            } else {
                left = debug_len / sizeof( debug_directory );
                if( left < 16 )
                    left = 16;
                for ( ;; ) {
                    if( buf.dbg_dir.debug_type == DEBUG_TYPE_CODEVIEW ) {
                        /* found something? */
                        rc = FoundHLLSign( ii, buf.dbg_dir.data_seek, buf.dbg_dir.data_seek );
                        if( rc == DS_OK || rc & DS_ERR ) {
                            break;
                        }
                    }

                    /* next */
                    --left;
                    if( left <= 0) {
                        break;
                    }
                    if ( DCRead( ii->sym_file, &buf.dbg_dir, sizeof( buf.dbg_dir ) )
                      != sizeof( buf.dbg_dir ) ) {
                        break;
                    }
                }
            }
            if( rc & DS_ERR ) {
                return( rc );
            }
        }
    }

    return( ii->bias ? DS_OK : DS_FAIL );
}

/*
 * Deals with LX and LE images.
 * We must try grab information from the object table.
 */
static dip_status FindHLLInLXImage( imp_image_handle *ii, unsigned long nh_off )
{
    union  {
        os2_flat_header flat;
        object_record   obj;
    }                   buf;
    dip_status          rc;

    /* read the header */
    rc = DCReadAt( ii->sym_file, &buf.flat, sizeof( buf.flat ), nh_off );
    if( rc & DS_ERR ) {
        return( rc );
    }
    rc = DS_FAIL;
    if( buf.flat.debug_off && buf.flat.debug_len ) {
        ii->is_32bit = 1;
        rc = FoundHLLSign( ii, buf.flat.debug_off, buf.flat.debug_len );
        if( rc == DS_OK ) {
            unsigned i;

            /*
             * Get segment info from the object table.
             */
            if ( DCSeek( ii->sym_file, buf.flat.objtab_off + nh_off, DIG_ORG )
              != buf.flat.objtab_off + nh_off ) {
                return( DS_ERR | DS_FSEEK_FAILED );
            }

            ii->seg_count = buf.flat.num_objects;
            ii->segments = DCAlloc( sizeof( hllinfo_seg ) * ii->seg_count );
            if( ii->segments == NULL ) {
                return( DS_ERR | DS_NO_MEM );
            }

            for( i = 0; i < ii->seg_count; i++ ) {
                if( DCRead( ii->sym_file, &buf.obj, sizeof( buf.obj ) )
                 != sizeof( buf.obj )) {
                    return( DS_ERR | DS_FREAD_FAILED );
                }
                ii->segments[i].is_executable = !!( buf.obj.flags & OBJ_EXECUTABLE );
                ii->segments[i].is_16bit = !( buf.obj.flags & OBJ_BIG );
                ii->segments[i].ovl = 0;
                ii->segments[i].map.offset = 0;
                ii->segments[i].map.segment = i + 1;
                ii->segments[i].size = buf.obj.size;
                ii->segments[i].address = buf.obj.addr;
            }
        }
    }
    return( rc );
}

/*
 * Tries to find the HLL/CV debug info in a image which format we know.
 * This function knows about LX, LE, PE and watcom symfile images.
 */
static dip_status TryFindInImage( imp_image_handle *ii )
{
    union {
        char            sig[HLL_SIG_SIZE]; /* ASSUMES >= 4 */
        unsigned_16     sig_16;
        unsigned_32     sig_32;
        pe_header       pe;
        pe_object       sh;
        debug_directory dbg_dir;
    }                   buf;
    bool                have_mz_header = FALSE;
    unsigned_32         nh_off;
    dip_status          rc;

    /* Read the image header. */
    rc = DCReadAt( ii->sym_file, &buf.sig, sizeof( buf.sig ), 0 );
    if( rc & DS_ERR ) {
        return( rc );
    }
    nh_off = 0;
    if( buf.sig_16 == DOS_SIGNATURE ) {
        have_mz_header = TRUE;
        /* read the new exe header */
        rc = DCReadAt( ii->sym_file, &nh_off, sizeof( nh_off ), NH_OFFSET );
        if( rc & DS_ERR ) {
            return( rc );
        }
        rc = DCReadAt( ii->sym_file, &buf.sig, sizeof( buf.sig ), nh_off );
        if( rc & DS_ERR ) {
            return( rc );
        }
    }

    /*
     * LE/LX executable - Use the debug_off/len members of the header.
     */
    if( buf.sig_16 == OSF_FLAT_LX_SIGNATURE
     || buf.sig_16 == OSF_FLAT_SIGNATURE ) {
        return( FindHLLInLXImage( ii, nh_off ) );
    }

    /*
     * PE executable - Use or scan the debug directory.
     */
    if( buf.sig_32 == PE_SIGNATURE ) {
        return( FindHLLInPEImage( ii, nh_off ) );
    }

    /*
     * NE and MZ executables do not contain any special information
     * in the header. TryFindTrailer() will have to pick up the debug data.
     */
    if( (buf.sig_16 == OS2_SIGNATURE_WORD) || have_mz_header ) {
        ii->is_32bit = 0;
    }

    /*
     * ELF executable - ??????
     */

    /*
     * A watcom .sym file.
     */
    if( IsHllSignature( &buf ) ) {
        return( FoundHLLSign( ii, nh_off, ~0UL ) );
    }

    /* no idea what this is.. */
    return( DS_FAIL );
}

/*
 * Checks if there is a tailing HLL signature in the file.
 *
 * This may work not work on executable images because of file alignments
 * imposed by the linker (ilink at least). However, TryFindInImage will
 * deal with those formats, so it doesn't really matter here.
 */
static dip_status TryFindTrailer( imp_image_handle *ii )
{
    hll_trailer         sig;
    unsigned long       pos;

    pos = DCSeek( ii->sym_file, -(long)sizeof( sig ), DIG_END );
    if( pos == -1UL ) {
        return( DS_ERR | DS_FSEEK_FAILED );
    }
    if( DCRead( ii->sym_file, &sig, sizeof( sig ) ) != sizeof( sig ) ) {
        return( DS_ERR | DS_FREAD_FAILED );
    }
    if( !IsHllSignature( &sig ) ) {
        return( DS_FAIL );
    }

    pos -= sig.offset - sizeof( sig );
    return FoundHLLSign( ii, pos, sig.offset - sizeof( sig ) );
}

/*
 * Tries to locate HLL debug info within the specified file.
 *
 * Returns DS_OK if found, with '*offp' set to the offset of the debug info
 * (relative to the start of the file) and '*sizep' set to the size of it.
 */
static dip_status FindHLL( imp_image_handle *ii )
{
    dip_status      ds = TryFindInImage( ii );

    if( ds & DS_ERR )
        return( ds );
    if( ds != DS_OK ) {
        ds = TryFindTrailer( ii );
    }
    return( ds );
}

#if 0
static walk_result FindCompUnit( imp_image_handle *ii,
                                 hll_dir_entry *cde, void *d )
{
    cs_compile          **rec = d;

    if( cde->subsection != hll_sstModules ) return( WR_CONTINUE );
    *rec = GetCompInfo( ii, cde->iMod );
    if( *rec == NULL ) return( WR_CONTINUE );
    return( WR_STOP );
}
#endif

/*
 * Load debug info if it's in the HLL/CV format.
 */
dip_status DIGENTRY DIPImpLoadInfo( dig_fhandle h, imp_image_handle *ii )

{
    dip_status      rc;

    /* Init the module handle. */
    memset( ii, 0, sizeof( *ii ) );
    ii->mad = MAD_X86;                  /* No known non-x86 support */
    ii->sym_file = h;
    ii->is_32bit = 1;
    ii->format_lvl = HLL_LVL_NB04;
    ii->next_image = ImageList;
    ImageList = ii;

    /* Read basic HLL and executable image bits. */
    rc = FindHLL( ii );
    if( rc == DS_OK ) {
        rc = VMInit( ii, ii->size );
    }

    /* Make sure we've got segment mappings. (FIXME: scan module headers) */
    if( rc == DS_OK && !ii->segments ) {
        ii->seg_count = 2;
        ii->segments = DCAlloc( sizeof( hllinfo_seg ) * ii->seg_count );
        if( ii->segments != NULL ) {
            memset( ii->segments, 0, sizeof( hllinfo_seg ) * ii->seg_count );
            ii->segments[0].is_executable = 1;
            ii->segments[0].map.segment = 1;
            ii->segments[1].map.segment = 2;
            ii->segments[0].size = ii->segments[1].size = 0x00400000; /* 4MB */
            ii->segments[0].address = ii->segments[1].address = 0;
        } else{
            rc = DS_ERR | DS_NO_MEM;
        }
    }

    /* Locate global types. If none, we've types per module. */
    if( rc == DS_OK ) {
        /* FIXME
        hll_debug_dir *cde;
        cde = FindDirEntry( ii, IMH_GBL, sstGlobalTypes );
        if( cde != NULL ) {
            hdr = VMBlock( ii, cde->lfo, sizeof( *hdr ) );
            if( hdr == NULL ) {
                Cleanup( ii );
                return( DS_ERR | DS_FAIL );
            }
            ii->types_base = cde->lfo
                + offsetof( cv_sst_global_types_header, offType )
                + hdr->cType * sizeof( hdr->offType[0] );
        }
        */
    }

    /* We're done - clean up on failure. */
    if( rc != DS_OK ) {
        DCStatus( rc );
        Cleanup( ii );
    }
    return( rc );
}

/*
 * Lets the DIP user setup segment mappings.
 */
void DIGENTRY DIPImpMapInfo( imp_image_handle *ii, void *d )
{
    unsigned    i;

    for( i = 0; i < ii->seg_count; i++ ) {
        DCMapAddr( &ii->segments[i].map, d );
    }
}

/*
 * Checks if a segment is executable.
 */
bool hllIsSegExecutable( imp_image_handle *ii, unsigned segment )
{
    if( segment - 1 < ii->seg_count
     && ii->segments[segment - 1].is_executable ) {
        return( TRUE );
    }
    return( FALSE );
}

/*
 * Maps an address.
 */
void hllMapLogical( imp_image_handle *ii, address *a )
{
    if( a->mach.segment - 1 < ii->seg_count ) {
        hllinfo_seg *seg = &ii->segments[a->mach.segment - 1];
        a->mach.segment = seg->map.segment;
        a->mach.offset += seg->map.offset;
        a->sect_id = seg->ovl;
        a->indirect = 1;
    }
}

/*
 * Free the image.
 */
void DIGENTRY DIPImpUnloadInfo( imp_image_handle *ii )
{
    Cleanup( ii );
    DCClose( ii->sym_file );
}
