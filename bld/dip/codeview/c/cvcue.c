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
* Description:  CodeView source line cues.
*
****************************************************************************/


#include "cvinfo.h"
#include "walloca.h"
#include "watcom.h"


typedef struct {
    unsigned_32 start;
    unsigned_32 end;
} off_range;

walk_result DIPIMPENTRY( WalkFileList )( imp_image_handle *iih, imp_mod_handle imh,
                            DIP_IMP_CUE_WALKER *wk, imp_cue_handle *icueh, void *d )
{
    cv_directory_entry                  *cde;
    cv_sst_src_module_header            *hdr;
    cv_sst_src_module_file_table        *fp;
    unsigned_32                         *file_off;
    unsigned                            file_tab_size;
    unsigned                            file_tab_count;
    unsigned                            i;
    walk_result                         wr;

    wr = WR_CONTINUE;
    if( imh != IMH_GBL ) {
        cde = FindDirEntry( iih, imh, sstSrcModule );
        if( cde != NULL ) {
            hdr = VMBlock( iih, cde->lfo, sizeof( *hdr ) );
            if( hdr == NULL ) {
                wr = WR_FAIL;
            } else {
                file_tab_count = hdr->cFile;
                file_tab_size = file_tab_count * sizeof( unsigned_32 );
                hdr = VMBlock( iih, cde->lfo, sizeof( *hdr ) + file_tab_size );
                /*
                 * Make a copy of the file table offset so that we don't have to worry
                 * about the VM system throwing it out.
                 */
                file_off = walloca( file_tab_size );
                memcpy( file_off, &hdr->baseSrcFile[0], file_tab_size );
                icueh->imh = imh;
                for( i = 0; i < file_tab_count; ++i ) {
                    icueh->pair = 0;
                    icueh->file = cde->lfo + file_off[i];
                    fp = VMBlock( iih, icueh->file, sizeof( *fp ) );
                    if( fp == NULL ) {
                        wr = WR_FAIL;
                        break;
                    }
                    icueh->line = cde->lfo + fp->baseSrcLn[0];
                    wr = wk( iih, icueh, d );
                    if( wr != WR_CONTINUE ) {
                        break;
                    }
                }
            }
        }
    }
    return( wr );
}

imp_mod_handle DIPIMPENTRY( CueMod )( imp_image_handle *iih, imp_cue_handle *icueh )
{
    /* unused parameters */ (void)iih;

    return( icueh->imh );
}

size_t DIPIMPENTRY( CueFile )( imp_image_handle *iih, imp_cue_handle *icueh, char *buff, size_t buff_size )
{
    void                                *p;
    unsigned_16                         name_len;
    cv_sst_src_module_file_table        *fp;
    virt_mem                            offset;


    offset = icueh->file;
    fp = VMBlock( iih, offset, sizeof( *fp ) );
    if( fp == NULL )
        return( 0 );
    offset += offsetof( cv_sst_src_module_file_table, baseSrcLn )
                    + (fp->cSeg * (sizeof( unsigned_32 ) * 3));
    p = VMBlock( iih, offset, sizeof( unsigned_16 ) );
    if( p == NULL )
        return( 0 );
    /*
     * Doc says the length is unsigned_16, cvpack says unsigned_8.
     * testing on real files confirm unsigned_8
     */
    name_len = *(unsigned_8 *)p;
    p = VMBlock( iih, offset + sizeof( unsigned_8 ), name_len );
    if( p == NULL )
        return( 0 );
    return( NameCopy( buff, p, buff_size, name_len ) );
}

cue_fileid DIPIMPENTRY( CueFileId )( imp_image_handle *iih, imp_cue_handle *icueh )
{
    /* unused parameters */ (void)iih;

    return( icueh->file );
}

unsigned long DIPIMPENTRY( CueLine )( imp_image_handle *iih, imp_cue_handle *icueh )
{
    cv_sst_src_module_line_number       *lp;
    unsigned long                       offset;
    unsigned_16                         *num;

    lp = VMBlock( iih, icueh->line, sizeof( *lp ) );
    if( lp == NULL )
        return( 0 );
    if( lp->cPair == 0 )
        return( 0 );
    offset = offsetof( cv_sst_src_module_line_number, offset )
                + (unsigned long)lp->cPair * sizeof( unsigned_32 )
                + icueh->pair * sizeof( unsigned_16 );
    num = VMBlock( iih, icueh->line + offset, sizeof( unsigned_16 ) );
    if( num == NULL )
        return( 0 );
    return( *num );
}

unsigned DIPIMPENTRY( CueColumn )( imp_image_handle *iih, imp_cue_handle *icueh )
{
    /* unused parameters */ (void)iih; (void)icueh;

    return( 0 );
}

address DIPIMPENTRY( CueAddr )( imp_image_handle *iih, imp_cue_handle *icueh )
{
    cv_sst_src_module_line_number       *lp;
    address                             addr;
    unsigned long                       offset;
    unsigned_32                         *off_p;

    lp = VMBlock( iih, icueh->line, sizeof( *lp ) );
    if( lp == NULL )
        return( NilAddr );
    if( lp->cPair == 0 )
        return( NilAddr );
    addr.mach.segment = lp->Seg;
    offset = offsetof( cv_sst_src_module_line_number, offset ) + (unsigned long)icueh->pair * sizeof( unsigned_32 );
    off_p = VMBlock( iih, icueh->line + offset, sizeof( unsigned_16 ) );
    if( off_p == NULL )
        return( NilAddr );
    addr.mach.offset = *off_p;
    MapLogical( iih, &addr );
    return( addr );
}

static dip_status AdjForward( imp_image_handle *iih, unsigned long bias, imp_cue_handle *icueh )
{
    cv_sst_src_module_line_number       *lp;
    cv_sst_src_module_file_table        *fp;
    dip_status                          ds;
    unsigned                            i;

    ds = DS_OK;
    icueh->pair++;
    for( ;; ) {
        lp = VMBlock( iih, icueh->line, sizeof( *lp ) );
        if( lp == NULL )
            return( DS_ERR | DS_FAIL );
        if( icueh->pair < lp->cPair )
            return( ds );
        fp = VMBlock( iih, icueh->file, sizeof( *fp ) );
        if( fp == NULL )
            return( DS_ERR | DS_FAIL );
        fp = VMBlock( iih, icueh->file, sizeof( *fp ) + fp->cSeg * sizeof( unsigned_32 ) );
        if( fp == NULL )
            return( DS_ERR | DS_FAIL );
        i = 0;
        for( ;; ) {
            if( (fp->baseSrcLn[i] + bias) == icueh->line )
                break;
            ++i;
        }
        if( ++i >= fp->cSeg ) {
            i = 0;
            ds = DS_WRAPPED;
        }
        icueh->line = fp->baseSrcLn[i] + bias;
        icueh->pair = 0;
    }
}

static dip_status AdjBackward( imp_image_handle *iih, unsigned long bias, imp_cue_handle *icueh )
{
    cv_sst_src_module_line_number       *lp;
    cv_sst_src_module_file_table        *fp;
    dip_status                          ds;
    unsigned                            i;

    ds = DS_OK;
    icueh->pair--;
    lp = VMBlock( iih, icueh->line, sizeof( *lp ) );
    if( lp == NULL )
        return( DS_ERR | DS_FAIL );
    for( ;; ) {
        /*
         * if icueh->pair went negative, the following compare will fail
         * because of unsigned comparison
         */
        if( icueh->pair < lp->cPair )
            return( ds );
        fp = VMBlock( iih, icueh->file, sizeof( *fp ) );
        if( fp == NULL )
            return( DS_ERR | DS_FAIL );
        fp = VMBlock( iih, icueh->file, sizeof( *fp ) + fp->cSeg * sizeof( unsigned_32 ) );
        if( fp == NULL )
            return( DS_ERR | DS_FAIL );
        i = 0;
        for( ;; ) {
            if( (fp->baseSrcLn[i] + bias) == icueh->line )
                break;
            ++i;
        }
        if( --i >= fp->cSeg ) {
            i = fp->cSeg - 1;
            ds = DS_WRAPPED;
        }
        icueh->line = fp->baseSrcLn[i] + bias;
        lp = VMBlock( iih, icueh->line, sizeof( *lp ) );
        if( lp == NULL )
            return( DS_ERR | DS_FAIL );
        icueh->pair = lp->cPair - 1;
    }
}

dip_status DIPIMPENTRY( CueAdjust )( imp_image_handle *iih, imp_cue_handle *src_icueh,
                                                int adj, imp_cue_handle *dst_icueh )
{
    cv_directory_entry  *cde;
    dip_status          ds;
    dip_status          ret_ds;

    cde = FindDirEntry( iih, src_icueh->imh, sstSrcModule );
    if( cde == NULL ) {
        DCStatus( DS_ERR | DS_INFO_INVALID );
        return( DS_ERR | DS_INFO_INVALID );
    }
    *dst_icueh = *src_icueh;
    ret_ds = DS_OK;
    while( adj > 0 ) {
        ds = AdjForward( iih, cde->lfo, dst_icueh );
        if( ds & DS_ERR )
            return( ds );
        if( ds != DS_OK )
            ret_ds = ds;
        --adj;
    }
    while( adj < 0 ) {
        ds = AdjBackward( iih, cde->lfo, dst_icueh );
        if( ds & DS_ERR )
            return( ds );
        if( ds != DS_OK )
            ret_ds = ds;
        ++adj;
    }
    return( ret_ds );
}

search_result DIPIMPENTRY( LineCue )( imp_image_handle *iih, imp_mod_handle imh, cue_fileid file,
                                    unsigned long line, unsigned column, imp_cue_handle *icueh )
{
    cv_directory_entry                  *cde;
    cv_sst_src_module_header            *hdr;
    cv_sst_src_module_file_table        *fp;
    cv_sst_src_module_line_number       *lp;
    unsigned_32                         *line_off;
    unsigned                            num_segs;
    unsigned                            seg_idx;
    unsigned long                       best_line;
    virt_mem                            line_base;
    virt_mem                            num_base;
    unsigned                            num_pairs;
    unsigned                            pair;
    unsigned_16                         *line_number;

    /* unused parameters */ (void)column;

    cde = FindDirEntry( iih, imh, sstSrcModule );
    if( cde == NULL )
        return( SR_NONE );
    if( file == 0 ) {
        hdr = VMBlock( iih, cde->lfo, sizeof( *hdr ) );
        if( hdr == NULL )
            return( SR_NONE );
        file = hdr->baseSrcFile[0] + cde->lfo;
    }
    icueh->imh = imh;
    icueh->file = file;
    fp = VMBlock( iih, file, sizeof( *fp ) );
    if( fp == NULL )
        return( SR_NONE );
    num_segs = fp->cSeg;
    fp = VMBlock( iih, file, sizeof( *fp ) + num_segs * sizeof( unsigned_32 ) );
    if( fp == NULL )
        return( SR_NONE );
    /*
     * make a copy of the line/offset table so that we don't have to worry
     * about the VM system throwing it out
     */
    line_off = walloca( num_segs * sizeof( unsigned_32 ) );
    memcpy( line_off, &fp->baseSrcLn[0], num_segs * sizeof( unsigned_32 ) );
    best_line = (unsigned long)-1L;
    for( seg_idx = 0; seg_idx < num_segs; ++seg_idx ) {
        line_base = line_off[seg_idx] + cde->lfo;
        lp = VMBlock( iih, line_base, sizeof( *lp ) );
        if( lp == NULL )
            return( SR_NONE );
        num_pairs = lp->cPair;
        num_base = line_base
                + offsetof( cv_sst_src_module_line_number, offset )
                + (unsigned long)num_pairs * sizeof( unsigned_32 );
        for( pair = 0; pair < num_pairs; ++pair ) {
            line_number = VMBlock( iih, num_base, sizeof( *line_number ) );
            if( *line_number >= line && *line_number < best_line ) {
                best_line = *line_number;
                icueh->line = line_base;
                icueh->pair = pair;
                if( best_line == line ) {
                    return( SR_EXACT );
                }
            }
            num_base += sizeof( *line_number );
        }
    }
    if( best_line == (unsigned long)-1L )
        return( SR_NONE );
    return( SR_CLOSEST );
}

#define NO_IDX  ((unsigned)-1)

static unsigned SearchOffsets( imp_image_handle *iih, virt_mem base, unsigned num_off,
                            addr_off want_off, addr_off *best_off, addr_off adj_off )
{
    int         lo_idx;
    int         hi_idx;
    int         idx;
//    addr_off    lo_off;
    addr_off    hi_off;
    addr_off    off;
    addr_off    *offp;

#define VMADDR( i )     (base + (i)*sizeof(addr_off))

    lo_idx = 0;
    hi_idx = num_off - 1;
    offp = VMBlock( iih, VMADDR( lo_idx ), sizeof( *offp ) );
    if( offp == NULL )
        return( NO_IDX );
//    lo_off = *offp + adj_off;
    offp = VMBlock( iih, VMADDR( hi_idx ), sizeof( *offp ) );
    if( offp == NULL )
        return( NO_IDX );
    hi_off = *offp + adj_off;
    while( lo_idx <= hi_idx ) {
        idx = (lo_idx + hi_idx ) >> 1;
        offp = VMBlock( iih, VMADDR( idx ), sizeof( *offp ) );
        if( offp == NULL )
            return( NO_IDX );
        off = *offp + adj_off;
        if( want_off < off ) {
            hi_idx = idx - 1;
            offp = VMBlock( iih, VMADDR( hi_idx ), sizeof( *offp ) );
            if( offp == NULL )
                return( NO_IDX );
            hi_off = *offp + adj_off;
        } else if( want_off > off ) {
            lo_idx = idx + 1;
            offp = VMBlock( iih, VMADDR( lo_idx ), sizeof( *offp ) );
            if( offp == NULL )
                return( NO_IDX );
//            lo_off = *offp + adj_off;
        } else {
            hi_idx = idx;
            hi_off = off;
            break;
        }
    }
    if( hi_idx < 0 )
        return( NO_IDX );
    if( hi_off > *best_off )
        return( NO_IDX );
    *best_off = hi_off;
    return( hi_idx );
}

static search_result SearchFile( imp_image_handle *iih, address addr, imp_cue_handle *icueh,
                          virt_mem file_base, cv_directory_entry *cde, addr_off *best_offset )
{
    cv_sst_src_module_file_table        *fp;
    cv_sst_src_module_line_number       *lp;
    virt_mem                            line_base;
    off_range                           *ranges;
    unsigned_32                         *lines;
    unsigned                            num_segs;
    unsigned                            seg_idx;
    address                             curr_addr;
    unsigned                            pair;

    fp = VMBlock( iih, file_base, sizeof( *fp ) );
    if( fp == NULL )
        return( SR_NONE );
    num_segs = fp->cSeg;
    fp = VMBlock( iih, file_base, sizeof( *fp )
            + num_segs * (sizeof( *lines ) + sizeof( *ranges ) ) );
    if( fp == NULL )
        return( SR_NONE );
    ranges = walloca( num_segs * sizeof( *ranges ) );
    lines  = walloca( num_segs * sizeof( *lines ) );
    memcpy( lines, &fp->baseSrcLn[0], num_segs * sizeof( *lines ) );
    memcpy( ranges, &fp->baseSrcLn[num_segs], num_segs * sizeof( *ranges ) );
    for( seg_idx = 0; seg_idx < num_segs; ++seg_idx ) {
        line_base = lines[seg_idx] + cde->lfo;
        lp = VMBlock( iih, line_base, sizeof( *lp ) );
        if( lp == NULL )
            return( SR_NONE );
        curr_addr.mach.segment = lp->Seg;
        curr_addr.mach.offset  = 0;
        MapLogical( iih, &curr_addr );
        if( DCSameAddrSpace( curr_addr, addr ) != DS_OK )
            continue;
        if( (ranges[seg_idx].start != 0 || ranges[seg_idx].end != 0)
          && (addr.mach.offset < ranges[seg_idx].start + curr_addr.mach.offset
          /*
           * The next condition is commented out. Digital Mars tools are known to
           * emit buggy CV4 data where the upper range does not cover all code,
           * causing us to fail finding last addresses within a module.
           */
          /*|| addr.mach.offset > ranges[seg_idx].end + curr_addr.mach.offset */ ) ) {
            continue;
        }
        pair = SearchOffsets( iih, line_base +
            offsetof( cv_sst_src_module_line_number, offset[0] ),
            lp->cPair, addr.mach.offset, best_offset, curr_addr.mach.offset );
        if( pair != NO_IDX ) {
            icueh->file = file_base;
            icueh->line = line_base;
            icueh->pair = pair;
            if( *best_offset == addr.mach.offset ) {
                return( SR_EXACT );
            }
        }
    }
    /*
     * We abuse the SR_FAIL return code to really mean SR_CONTINUE (ie. continue
     * searching other files). A SR_CONTINUE code is not defined because it does
     * not make sense as a return value for DIPImpAddrCue()
     */
    return( SR_FAIL );
}

search_result DIPIMPENTRY( AddrCue )( imp_image_handle *iih, imp_mod_handle imh,
                                        address addr, imp_cue_handle *icueh )
{
    cv_directory_entry                  *cde;
    cv_sst_src_module_header            *hdr;
    unsigned_32                         *files;
    virt_mem                            file_base;
    unsigned                            num_files;
    unsigned                            file_idx;
    addr_off                            best_offset;
    unsigned                            file_tab_size;
    search_result                       rc;

    cde = FindDirEntry( iih, imh, sstSrcModule );
    if( cde == NULL )
        return( SR_NONE );
    hdr = VMBlock( iih, cde->lfo, sizeof( *hdr ) );
    if( hdr == NULL )
        return( SR_NONE );
    icueh->imh = imh;
    num_files = hdr->cFile;
    file_tab_size = num_files * sizeof( unsigned_32 );
    hdr = VMBlock( iih, cde->lfo, sizeof( *hdr ) + file_tab_size );
    files = walloca( file_tab_size );
    memcpy( files, &hdr->baseSrcFile[0], file_tab_size );
    best_offset = (addr_off)-1L;
    for( file_idx = 0; file_idx < num_files; ++file_idx ) {
        file_base = files[file_idx] + cde->lfo;
        rc = SearchFile( iih, addr, icueh, file_base, cde, &best_offset );
        if( rc != SR_FAIL ) {
            return( rc );   /* see comment in SearchFile above */
        }
    }
    if( best_offset == (addr_off)-1L )
        return( SR_NONE );
    return( SR_CLOSEST );
}

int DIPIMPENTRY( CueCmp )( imp_image_handle *iih, imp_cue_handle *icueh1, imp_cue_handle *icueh2 )
{
    /* unused parameters */ (void)iih;

    if( icueh1->imh < icueh2->imh )
        return( -1 );
    if( icueh1->imh > icueh2->imh )
        return( 1 );
    if( icueh1->line < icueh2->line )
        return( -1 );
    if( icueh1->line > icueh2->line )
        return( 1 );
    if( icueh1->pair < icueh2->pair )
        return( -1 );
    if( icueh1->pair > icueh2->pair )
        return( 1 );
    return( 0 );
}
