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

walk_result     DIGENTRY DIPImpWalkFileList( imp_image_handle *ii,
                    imp_mod_handle im, IMP_CUE_WKR *wk, imp_cue_handle *ic,
                    void *d )
{
    cv_directory_entry                  *cde;
    cv_sst_src_module_header            *hdr;
    cv_sst_src_module_file_table        *fp;
    unsigned_32                         *file_off;
    unsigned                            file_tab_size;
    unsigned                            file_tab_count;
    unsigned                            i;
    walk_result                         wr;

    if( im == IMH_GBL )
        return( WR_CONTINUE );

    cde = FindDirEntry( ii, im, sstSrcModule );
    if( cde == NULL )
        return(  WR_CONTINUE );
    hdr = VMBlock( ii, cde->lfo, sizeof( *hdr ) );
    if( hdr == NULL ) return( WR_FAIL );
    file_tab_count = hdr->cFile;
    file_tab_size = file_tab_count * sizeof( unsigned_32 );
    hdr = VMBlock( ii, cde->lfo, sizeof( *hdr ) + file_tab_size );
    /*
        Make a copy of the file table offset so that we don't have to worry
        about the VM system throwing it out.
    */
    file_off = __alloca( file_tab_size );
    memcpy( file_off, &hdr->baseSrcFile[0], file_tab_size );
    ic->im = im;
    for( i = 0; i < file_tab_count; ++i ) {
        ic->pair = 0;
        ic->file = cde->lfo + file_off[i];
        fp = VMBlock( ii, ic->file, sizeof( *fp ) );
        if( fp == NULL ) return( WR_FAIL );
        ic->line = cde->lfo + fp->baseSrcLn[0];
        wr = wk( ii, ic, d );
        if( wr != WR_CONTINUE ) return( wr );
    }
    return( WR_CONTINUE );
}

imp_mod_handle  DIGENTRY DIPImpCueMod( imp_image_handle *ii,
                                imp_cue_handle *ic )
{
    ii = ii;
    return( ic->im );
}

unsigned        DIGENTRY DIPImpCueFile( imp_image_handle *ii,
                        imp_cue_handle *ic, char *buff, unsigned buff_size )
{
    void                                *p;
    unsigned_16                         name_len;
    cv_sst_src_module_file_table        *fp;
    virt_mem                            offset;


    offset = ic->file;
    fp = VMBlock( ii, offset, sizeof( *fp ) );
    if( fp == 0 ) return( 0 );
    offset += offsetof( cv_sst_src_module_file_table, baseSrcLn )
                    + (fp->cSeg * (sizeof( unsigned_32 ) * 3));
    p = VMBlock( ii, offset, sizeof( unsigned_16 ) );
    if( p == NULL ) return( 0 );
    /* Doc says the length is unsigned_16, cvpack says 8. */
    name_len = *(unsigned_8 *)p;
    p = VMBlock( ii, offset + sizeof( unsigned_8 ), name_len );
    if( p == NULL ) return( 0 );
    return( NameCopy( buff, p, buff_size, name_len ) );
}

cue_fileid  DIGENTRY DIPImpCueFileId( imp_image_handle *ii,
                        imp_cue_handle *ic )
{
    ii = ii;
    return( ic->file );
}

unsigned long   DIGENTRY DIPImpCueLine( imp_image_handle *ii,
                        imp_cue_handle *ic )
{
    cv_sst_src_module_line_number       *lp;
    unsigned long                       offset;
    unsigned_16                         *num;

    lp = VMBlock( ii, ic->line, sizeof( *lp ) );
    if( lp == NULL ) return( 0 );
    if( lp->cPair == 0 ) return( 0 );
    offset = offsetof( cv_sst_src_module_line_number, offset )
                + (unsigned long)lp->cPair * sizeof( unsigned_32 )
                + ic->pair * sizeof( unsigned_16 );
    num = VMBlock( ii, ic->line + offset, sizeof( unsigned_16 ) );
    if( num == NULL ) return( 0 );
    return( *num );
}

unsigned        DIGENTRY DIPImpCueColumn( imp_image_handle *ii, imp_cue_handle *ic )
{
    ii = ii; ic = ic;
    return( 0 );
}

address         DIGENTRY DIPImpCueAddr( imp_image_handle *ii,
                        imp_cue_handle *ic )
{
    cv_sst_src_module_line_number       *lp;
    address                             addr;
    unsigned long                       offset;
    unsigned_32                         *off_p;

    lp = VMBlock( ii, ic->line, sizeof( *lp ) );
    if( lp == NULL ) return( NilAddr );
    if( lp->cPair == 0 ) return( NilAddr );
    addr.mach.segment = lp->Seg;
    offset = offsetof( cv_sst_src_module_line_number, offset )
                + (unsigned long)ic->pair * sizeof( unsigned_32 );
    off_p = VMBlock( ii, ic->line + offset, sizeof( unsigned_16 ) );
    if( off_p == NULL ) return( NilAddr );
    addr.mach.offset = *off_p;
    MapLogical( ii, &addr );
    return( addr );
}

static dip_status AdjForward( imp_image_handle *ii, unsigned long bias,
                                imp_cue_handle *ic )
{
    cv_sst_src_module_line_number       *lp;
    cv_sst_src_module_file_table        *fp;
    dip_status                          ds;
    unsigned                            i;

    ds = DS_OK;
    ic->pair++;
    for( ;; ) {
        lp = VMBlock( ii, ic->line, sizeof( *lp ) );
        if( lp == NULL ) return( DS_ERR|DS_FAIL );
        if( ic->pair < lp->cPair ) return( ds );
        fp = VMBlock( ii, ic->file, sizeof( *fp ) );
        if( fp == NULL ) return( DS_ERR|DS_FAIL );
        fp = VMBlock( ii, ic->file, sizeof( *fp ) + fp->cSeg * sizeof( unsigned_32 ) );
        if( fp == NULL ) return( DS_ERR|DS_FAIL );
        i = 0;
        for( ;; ) {
            if( (fp->baseSrcLn[i] + bias) == ic->line ) break;
            ++i;
        }
        if( ++i >= fp->cSeg ) {
            i = 0;
            ds = DS_WRAPPED;
        }
        ic->line = fp->baseSrcLn[i] + bias;
        ic->pair = 0;
    }
}

static dip_status AdjBackward( imp_image_handle *ii, unsigned long bias,
                                imp_cue_handle *ic )
{
    cv_sst_src_module_line_number       *lp;
    cv_sst_src_module_file_table        *fp;
    dip_status                          ds;
    unsigned                            i;

    ds = DS_OK;
    ic->pair--;
    lp = VMBlock( ii, ic->line, sizeof( *lp ) );
    if( lp == NULL ) return( DS_ERR|DS_FAIL );
    for( ;; ) {
        /* if ic->pair went negative, the following compare will fail
           because of unsigned comparison */
        if( ic->pair < lp->cPair ) return( ds );
        fp = VMBlock( ii, ic->file, sizeof( *fp ) );
        if( fp == NULL ) return( DS_ERR|DS_FAIL );
        fp = VMBlock( ii, ic->file, sizeof( *fp ) + fp->cSeg * sizeof( unsigned_32 ) );
        if( fp == NULL ) return( DS_ERR|DS_FAIL );
        i = 0;
        for( ;; ) {
            if( (fp->baseSrcLn[i] + bias) == ic->line ) break;
            ++i;
        }
        if( --i >= fp->cSeg ) {
            i = fp->cSeg - 1;
            ds = DS_WRAPPED;
        }
        ic->line = fp->baseSrcLn[i] + bias;
        lp = VMBlock( ii, ic->line, sizeof( *lp ) );
        if( lp == NULL ) return( DS_ERR|DS_FAIL );
        ic->pair = lp->cPair - 1;
    }
}

dip_status      DIGENTRY DIPImpCueAdjust( imp_image_handle *ii,
                imp_cue_handle *ic, int adj, imp_cue_handle *aic )
{
    cv_directory_entry  *cde;
    dip_status          status;
    dip_status          ok;

    cde = FindDirEntry( ii, ic->im, sstSrcModule );
    if( cde == NULL ) {
        DCStatus( DS_ERR|DS_INFO_INVALID );
        return( DS_ERR|DS_INFO_INVALID );
    }
    *aic = *ic;
    ok = DS_OK;
    while( adj > 0 ) {
        status = AdjForward( ii, cde->lfo, aic );
        if( status & DS_ERR ) return( status );
        if( status != DS_OK ) ok = status;
        --adj;
    }
    while( adj < 0 ) {
        status = AdjBackward( ii, cde->lfo, aic );
        if( status & DS_ERR ) return( status );
        if( status != DS_OK ) ok = status;
        ++adj;
    }
    return( ok );
}

search_result   DIGENTRY DIPImpLineCue( imp_image_handle *ii,
                imp_mod_handle im, cue_fileid file, unsigned long line,
                unsigned column, imp_cue_handle *ic )
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

    column = column;
    cde = FindDirEntry( ii, im, sstSrcModule );
    if( cde == NULL ) return( SR_NONE );
    if( file == 0 ) {
        hdr = VMBlock( ii, cde->lfo, sizeof( *hdr ) );
        if( hdr == NULL ) return( SR_NONE );
        file = hdr->baseSrcFile[0] + cde->lfo;
    }
    ic->im = im;
    ic->file = file;
    fp = VMBlock( ii, file, sizeof( *fp ) );
    if( fp == NULL ) return( SR_NONE );
    num_segs = fp->cSeg;
    fp = VMBlock( ii, file, sizeof( *fp ) + num_segs * sizeof( unsigned_32 ) );
    if( fp == NULL ) return( SR_NONE );
    /* make a copy of the line/offset table so that we don't have to worry
       about the VM system throwing it out */
    line_off = __alloca( num_segs * sizeof( unsigned_32 ) );
    memcpy( line_off, &fp->baseSrcLn[0], num_segs * sizeof( unsigned_32 ) );
    best_line = -1UL;
    for( seg_idx = 0; seg_idx < num_segs; ++seg_idx ) {
        line_base = line_off[seg_idx] + cde->lfo;
        lp = VMBlock( ii, line_base, sizeof( *lp ) );
        if( lp == NULL ) return( SR_NONE );
        num_pairs = lp->cPair;
        num_base = line_base
                + offsetof( cv_sst_src_module_line_number, offset )
                + (unsigned long)num_pairs * sizeof( unsigned_32 );
        for( pair = 0; pair < num_pairs; ++pair ) {
            line_number = VMBlock( ii, num_base, sizeof( *line_number ) );
            if( *line_number >= line && *line_number < best_line ) {
                best_line = *line_number;
                ic->line = line_base;
                ic->pair = pair;
                if( best_line == line ) return( SR_EXACT );
            }
            num_base += sizeof( *line_number );
        }
    }
    if( best_line == -1UL ) return( SR_NONE );
    return( SR_CLOSEST );
}

#define NO_IDX  ((unsigned)-1)

unsigned SearchOffsets( imp_image_handle *ii, virt_mem base,
                        unsigned num_off,
                        addr_off  want_off,
                        addr_off *best_off,
                        addr_off  adj_off )
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
    offp = VMBlock( ii, VMADDR( lo_idx ), sizeof( *offp ) );
    if( offp == NULL ) return( NO_IDX );
//    lo_off = *offp + adj_off;
    offp = VMBlock( ii, VMADDR( hi_idx ), sizeof( *offp ) );
    if( offp == NULL ) return( NO_IDX );
    hi_off = *offp + adj_off;
    while( lo_idx <= hi_idx ) {
        idx = (lo_idx + hi_idx ) >> 1;
        offp = VMBlock( ii, VMADDR( idx ), sizeof( *offp ) );
        if( offp == NULL ) return( NO_IDX );
        off = *offp + adj_off;
        if( want_off < off ) {
            hi_idx = idx - 1;
            offp = VMBlock( ii, VMADDR( hi_idx ), sizeof( *offp ) );
            if( offp == NULL ) return( NO_IDX );
            hi_off = *offp + adj_off;
        } else if( want_off > off ) {
            lo_idx = idx + 1;
            offp = VMBlock( ii, VMADDR( lo_idx ), sizeof( *offp ) );
            if( offp == NULL ) return( NO_IDX );
//            lo_off = *offp + adj_off;
        } else {
            hi_idx = idx;
            hi_off = off;
            break;
        }
    }
    if( hi_idx < 0 ) return( NO_IDX );
    if( hi_off > *best_off ) return( NO_IDX );
    *best_off = hi_off;
    return( hi_idx );
}

search_result SearchFile( imp_image_handle              *ii,
                          address                       addr,
                          imp_cue_handle                *ic,
                          virt_mem                      file_base,
                          cv_directory_entry            *cde,
                          addr_off                      *best_offset)
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

    fp = VMBlock( ii, file_base, sizeof( *fp ) );
    if( fp == NULL ) return( SR_NONE );
    num_segs = fp->cSeg;
    fp = VMBlock( ii, file_base, sizeof( *fp )
            + num_segs * (sizeof( unsigned_32 ) + sizeof( off_range ) ) );
    if( fp == NULL ) return( SR_NONE );
    ranges = __alloca( num_segs * sizeof( *ranges ) );
    lines  = __alloca( num_segs * sizeof( *lines ) );
    memcpy( lines, &fp->baseSrcLn[0], num_segs * sizeof( *lines ) );
    memcpy( ranges, &fp->baseSrcLn[num_segs], num_segs * sizeof( *ranges ) );
    for( seg_idx = 0; seg_idx < num_segs; ++seg_idx ) {
        line_base = lines[seg_idx] + cde->lfo;
        lp = VMBlock( ii, line_base, sizeof( *lp ) );
        if( lp == NULL ) return( SR_NONE );
        curr_addr.mach.segment = lp->Seg;
        curr_addr.mach.offset  = 0;
        MapLogical( ii, &curr_addr );
        if( DCSameAddrSpace( curr_addr, addr ) != DS_OK ) continue;
        if( (ranges[seg_idx].start != 0 || ranges[seg_idx].end != 0)
          && (addr.mach.offset < ranges[seg_idx].start + curr_addr.mach.offset
          /* The next condition is commented out. Digital Mars tools are known to
           * emit buggy CV4 data where the upper range does not cover all code,
           * causing us to fail finding last addresses within a module.
           */
          /*|| addr.mach.offset > ranges[seg_idx].end + curr_addr.mach.offset */ ) ) {
            continue;
        }
        pair = SearchOffsets( ii, line_base +
            offsetof( cv_sst_src_module_line_number, offset[0] ),
            lp->cPair, addr.mach.offset, best_offset, curr_addr.mach.offset );
        if( pair != NO_IDX ) {
            ic->file = file_base;
            ic->line = line_base;
            ic->pair = pair;
            if( *best_offset == addr.mach.offset ) return( SR_EXACT );
        }
    }
    /* We abuse the SR_FAIL return code to really mean SR_CONTINUE (ie. continue
     * searching other files). A SR_CONTINUE code is not defined because it does
     * not make sense as a return value for DIPImpAddrCue()
     */
    return( SR_FAIL );
}

search_result   DIGENTRY DIPImpAddrCue( imp_image_handle *ii,
                imp_mod_handle im, address addr, imp_cue_handle *ic )
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

    cde = FindDirEntry( ii, im, sstSrcModule );
    if( cde == NULL )
        return( SR_NONE );
    hdr = VMBlock( ii, cde->lfo, sizeof( *hdr ) );
    if( hdr == NULL )
        return( SR_NONE );
    ic->im = im;
    num_files = hdr->cFile;
    file_tab_size = num_files * sizeof( unsigned_32 );
    hdr = VMBlock( ii, cde->lfo, sizeof( *hdr ) + file_tab_size );
    files = __alloca( file_tab_size );
    memcpy( files, &hdr->baseSrcFile[0], file_tab_size );
    best_offset = (addr_off)-1UL;
    for( file_idx = 0; file_idx < num_files; ++file_idx ) {
        file_base = files[file_idx] + cde->lfo;
        rc = SearchFile( ii, addr, ic, file_base, cde, &best_offset );
        if( rc != SR_FAIL ) return( rc );   /* see comment in SearchFile above */
    }
    if( best_offset == (addr_off)-1UL ) return( SR_NONE );
    return( SR_CLOSEST );
}

int DIGENTRY DIPImpCueCmp( imp_image_handle *ii, imp_cue_handle *ic1,
                                imp_cue_handle *ic2 )
{
    ii = ii;
    if( ic1->im != ic2->im )
        return( ic1->im - ic2->im );
    if( ic1->line < ic2->line )
        return( -1 );
    if( ic1->line > ic2->line )
        return( +1 );
    return( ic1->pair - ic2->pair );
}
