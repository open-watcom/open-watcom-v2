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
* Description:  NE segment processing functions.
*
****************************************************************************/


#include "wio.h"
#include "global.h"
#include "errors.h"
#include "rcrtns.h"
#include "clibext.h"
#include "rccore.h"
#include "exeutil.h"
#include "exeseg.h"

static RcStatus allocSegTable( SegTable *seg, int *err_code )
{
    int     tablesize;

    tablesize = seg->NumSegs * sizeof(segment_record);
    seg->Segments = RCALLOC( tablesize );
    if( seg->Segments == NULL ) {
        *err_code = errno;
        return( RS_NO_MEM );
    } else {
        return( RS_OK );
    }
} /* allocSegTable */

/*
 * readSegTable
 * NB when an error occurs this function must return without altering errno
 */
static RcStatus readSegTable( WResFileID handle, uint_32 offset, SegTable * seg )
{
    int             tablesize;
    WResFileSSize   numread;

    tablesize = seg->NumSegs * sizeof(segment_record);

    if( RCSEEK( handle, offset, SEEK_SET ) == -1 )
        return( RS_READ_ERROR );
    numread = RCREAD( handle, seg->Segments, tablesize );
    if( numread != tablesize ) {
        return( RCIOERR( handle, numread ) ? RS_READ_ERROR : RS_READ_INCMPLT );
    }
    return( RS_OK );

} /* readSegTable */

extern RcStatus AllocAndReadWINSegTables( int *err_code )
{
    RcStatus            ret;
    WResFileID          oldhandle;
    uint_32             head_offset;
    SegTable            *oldseg;
    SegTable            *tmpseg;
    os2_exe_header      *head;

    oldseg = &(Pass2Info.OldFile.u.NEInfo.Seg);
    oldhandle = Pass2Info.OldFile.Handle;
    tmpseg = &(Pass2Info.TmpFile.u.NEInfo.Seg);

    head = &(Pass2Info.OldFile.u.NEInfo.WinHead);
    head_offset = Pass2Info.OldFile.WinHeadOffset;

    oldseg->NumSegs = head->segments;
    tmpseg->NumSegs = head->segments;
    ret = allocSegTable( oldseg, err_code );
    if( ret != RS_OK )
        return( ret );
    ret = allocSegTable( tmpseg, err_code );
    if( ret != RS_OK )
        return( ret );

    ret = readSegTable( oldhandle, head_offset + head->segment_off, oldseg );
    if( ret != RS_OK ){
        *err_code = errno;
        return( ret );
    }
    ret = readSegTable( oldhandle, head_offset + head->segment_off, tmpseg );
    *err_code = errno;
    return( ret );

} /* AllocAndReadWINSegTables */


extern RcStatus AllocAndReadOS2SegTables( int *err_code )
{
    RcStatus            ret;
    WResFileID          oldhandle;
    int                 oldres;
    int                 newres;
    uint_32             head_offset;
    SegTable            *oldseg;
    SegTable            *tmpseg;
    os2_exe_header      *head;

    oldres = Pass2Info.OldFile.u.NEInfo.WinHead.resource;
    oldseg = &(Pass2Info.OldFile.u.NEInfo.Seg);
    oldhandle = Pass2Info.OldFile.Handle;
    tmpseg = &(Pass2Info.TmpFile.u.NEInfo.Seg);
    newres = ComputeOS2ResSegCount( Pass2Info.ResFiles->Dir );

    head = &(Pass2Info.OldFile.u.NEInfo.WinHead);
    head_offset = Pass2Info.OldFile.WinHeadOffset;

    if( (int_32)head->segments - oldres < 0 )
        return( RS_BAD_FILE_FMT );

    oldseg->NumSegs = head->segments;
    tmpseg->NumSegs = oldseg->NumSegs - oldres + newres;
    oldseg->NumOS2ResSegs = oldres;
    tmpseg->NumOS2ResSegs = newres;

    ret = allocSegTable( oldseg, err_code );
    if( ret != RS_OK )
        return( ret );

    ret = allocSegTable( tmpseg, err_code );
    if( ret != RS_OK )
        return( ret );

    ret = readSegTable( oldhandle, head_offset + head->segment_off, oldseg );
    if( ret != RS_OK ) {
        *err_code = errno;
        return( ret );
    }
    ret = readSegTable( oldhandle, head_offset + head->segment_off, tmpseg );
    *err_code = errno;
    return( ret );
} /* AllocAndReadOS2SegTables */


/********* WARNING *********/
/* Hard coded constant. The value of sizeof(os2_reloc_item) is to hard */
/* to get from  wl  because of the other files that would have to be included */
/* in order to get that structure */
#define OS_RELOC_ITEM_SIZE      8

extern uint_32 ComputeSegmentSize( WResFileID handle, SegTable * segs, int shift_count )
{
    segment_record *    currseg;
    segment_record *    afterlast;
    uint_32             length;
    WResFileSSize       numread;
    uint_16             num_relocs;

    length = 0;
    for (currseg = segs->Segments, afterlast = segs->Segments + segs->NumSegs;
            currseg < afterlast; currseg++) {
        length += currseg->size;
        if (currseg->info & SEG_RELOC) {
            if( RCSEEK( handle, (((long)currseg->address) << (long)shift_count) + currseg->size, SEEK_SET ) == -1 )
                return( 0 );
            numread = RCREAD( handle, &num_relocs, sizeof(uint_16) );
            if( numread != sizeof(uint_16) )
                return( 0 );
            length += (unsigned_32)num_relocs * (unsigned_32)OS_RELOC_ITEM_SIZE;
        }
    }

    return( length );

} /* ComputeSegmentSize */

static bool myCopyExeData( ExeFileInfo *inexe, ExeFileInfo *outexe, uint_32 length )
{
    switch( CopyExeData( inexe->Handle, outexe->Handle, length ) ) {
    case RS_OK:
    case RS_PARAM_ERROR:
        return( false );
    case RS_READ_ERROR:
        RcError( ERR_READING_EXE, inexe->name, strerror( errno ) );
        break;
    case RS_READ_INCMPLT:
        RcError( ERR_UNEXPECTED_EOF, inexe->name );
        break;
    case RS_WRITE_ERROR:
        RcError( ERR_WRITTING_FILE, outexe->name, strerror( errno ) );
        break;
    default:
        RcError( ERR_INTERNAL, INTERR_UNKNOWN_RCSTATUS );
        break;
    }
    return( true );
}


static CpSegRc copyOneSegment( const segment_record * inseg,
            segment_record * outseg, ExeFileInfo *inexe, ExeFileInfo *outexe,
            int old_shift_count, int new_shift_count, bool pad_end )
{
    CpSegRc         ret;
    bool            error;
    WResFileSSize   numread;
    uint_16         numrelocs;
    long            out_offset;
    long            align_amount;
    uint_32         seg_len = 0L;
    char            dum;

    dum = 0;
    error = false;
    ret = CPSEG_OK;

    /* check if this is a segment that has no image in the exe file */
    if (inseg->address != 0) {
        /* align in the out file so that shift_count will be valid */
        out_offset = RCTELL( outexe->Handle );
        if( out_offset == -1 ) {
            error = true;
            RcError( ERR_WRITTING_FILE, outexe->name, strerror( errno ) );
        }
        if (!error) {
            align_amount = AlignAmount( out_offset, new_shift_count );
            if( RCSEEK( outexe->Handle, align_amount, SEEK_CUR ) == -1 ) {
                error = true;
                RcError( ERR_WRITTING_FILE, outexe->name, strerror( errno ) );
            }
            out_offset += align_amount;
        }

        /* move in the in file to the start of the segment */
        if (!error) {
            /* convert the address to a long before shifting it */
            if( RCSEEK( inexe->Handle, (long)inseg->address << old_shift_count, SEEK_SET ) == -1 ) {
                error = true;
                RcError( ERR_READING_EXE, inexe->name, strerror( errno ) );
            }
        }

        if (!error) {
            if (inseg->size == 0) {
                seg_len = 0x10000L;
            } else {
                seg_len = inseg->size;
            }
            error = myCopyExeData( inexe, outexe, seg_len );
        }

        if (inseg->info & SEG_RELOC && !error) {
            /* read the number of relocation items */
            numread = RCREAD( inexe->Handle, &numrelocs, sizeof(uint_16) );
            if( numread != sizeof( uint_16 ) ) {
                error = true;
                if( RCIOERR( inexe->Handle, numread ) ) {
                    RcError( ERR_READING_EXE, inexe->name, strerror( errno ) );
                } else {
                    RcError( ERR_UNEXPECTED_EOF, inexe->name );
                }
            } else {
                if( RCWRITE( outexe->Handle, &numrelocs, sizeof(uint_16) ) != sizeof( uint_16 ) ) {
                    error = true;
                    RcError( ERR_WRITTING_FILE, outexe->name, strerror( errno ) );
                }
            }
            /* copy the relocation information */
            if (!error) {
                error = myCopyExeData( inexe, outexe, numrelocs * OS_RELOC_ITEM_SIZE );
            }
            if (numrelocs * OS_RELOC_ITEM_SIZE + seg_len > 0x10000L) {
                ret = CPSEG_SEG_TOO_BIG;
            }
        }

        if (pad_end && ret != CPSEG_SEG_TOO_BIG && !error) {
            align_amount = AlignAmount( RCTELL( outexe->Handle ),
                                        new_shift_count );
            /* make sure there is room for the memory arena header */
            if (align_amount < 16) {
                align_amount += 16;
            }
            if( RCSEEK( outexe->Handle, align_amount - 1, SEEK_CUR ) == -1 ) {
                error = true;
                RcError( ERR_WRITTING_FILE, outexe->name );
            } else {
                /* write something out so if we have just seeked past the
                 * end of the file the file's size will be adjusted
                 * appropriately */
                if( RCWRITE( outexe->Handle, &dum, 1 ) != 1 ) {
                    error = true;
                    RcError( ERR_WRITTING_FILE, outexe->name );
                }
            }
        }
    } else {
        out_offset = 0;
    }

    /* copy the segment record to outseg */
    if (!error) {
        outseg->size = inseg->size;
        outseg->info = inseg->info;
        outseg->min = inseg->min;
        outseg->address = out_offset >> new_shift_count;
    }

    if (error) {
        ret = CPSEG_ERROR;
    }

    return( ret );
} /* copyOneSegment */


extern CpSegRc CopyWINSegments( uint_16 sect2mask, uint_16 sect2bits, bool sect2 )
/********************************************************************************/
/* Note: sect2 must be either 1 (do section 2) or 0 (do section 1) */
/* CopyWINSegments should be called twice, once with sect2 false, and once with */
/* it true. The values of sect2mask and sect2bits should be the same for both */
/* calls. The segment table for the temporary file will not be properly */
/* filled in until after the second call */
{
    segment_record *    oldseg;
    segment_record *    tmpseg;
    bool                padend;
    int                 num_segs;
    ExeFileInfo         *old_exe_info;
    ExeFileInfo         *tmp_exe_info;
    int                 old_shift_count;
    int                 new_shift_count;
    int                 currseg;
    CpSegRc             cponeret;
    CpSegRc             ret;

    oldseg = Pass2Info.OldFile.u.NEInfo.Seg.Segments;
    tmpseg = Pass2Info.TmpFile.u.NEInfo.Seg.Segments;
    num_segs = Pass2Info.OldFile.u.NEInfo.Seg.NumSegs;
    old_exe_info = &Pass2Info.OldFile;
    tmp_exe_info = &Pass2Info.TmpFile;
    old_shift_count = Pass2Info.OldFile.u.NEInfo.WinHead.align;
    new_shift_count = Pass2Info.TmpFile.u.NEInfo.WinHead.align;

    ret = CPSEG_OK;
    cponeret = CPSEG_OK;
    padend = !sect2;

    for( currseg = 0; currseg < num_segs; ++currseg ) {
        /* if the bits are unequal and this is section 1 --> copy segment */
        /* if the bits are equal and this is section 2   --> copy segment */
        /* otherwise                                     --> do nothing */
        if( ARE_BITS_EQUAL( sect2mask, sect2bits, oldseg->info ) == sect2 ) {
            cponeret = copyOneSegment( oldseg, tmpseg, old_exe_info,
                                        tmp_exe_info, old_shift_count,
                                        new_shift_count, padend );
            if (cponeret == CPSEG_SEG_TOO_BIG) {
                padend = FALSE;
                ret = CPSEG_SEG_TOO_BIG;
            }
            CheckDebugOffset( &(Pass2Info.OldFile) );
            CheckDebugOffset( &(Pass2Info.TmpFile) );
        }
        if( cponeret == CPSEG_ERROR )
            break;
        /* mark section 1 segments as preload */
        if( !sect2 ) {
            tmpseg->info |= SEG_PRELOAD;
        }
        oldseg++;
        tmpseg++;
    }

    if( cponeret == CPSEG_ERROR ) {
        ret = CPSEG_ERROR;
        Pass2Info.TmpFile.u.NEInfo.Seg.NumSegs = 0;
    } else {
        Pass2Info.TmpFile.u.NEInfo.Seg.NumSegs = Pass2Info.OldFile.u.NEInfo.Seg.NumSegs;
    }

    return( ret );

} /* CopyWINSegments */

extern CpSegRc CopyOS2Segments( void )
/*************************************
 * Akin to CopyWINSegments() only much, much simpler - just copies all segments
 * without messing with them in any way. Only called once. Won't copy
 * resource segments.
 */
{
    segment_record      *oldseg;
    segment_record      *tmpseg;
    int                 num_segs;
    SegTable            *old_seg_tbl;
    ExeFileInfo         *old_exe_info;
    ExeFileInfo         *tmp_exe_info;
    int                 old_shift_count;
    int                 new_shift_count;
    int                 currseg;
    CpSegRc             cponeret;
    CpSegRc             ret;

    old_seg_tbl = &Pass2Info.TmpFile.u.NEInfo.Seg;
    oldseg = old_seg_tbl->Segments;
    tmpseg = Pass2Info.TmpFile.u.NEInfo.Seg.Segments;
    num_segs = old_seg_tbl->NumSegs - old_seg_tbl->NumOS2ResSegs;
    old_exe_info = &Pass2Info.OldFile;
    tmp_exe_info = &Pass2Info.TmpFile;
    old_shift_count = Pass2Info.OldFile.u.NEInfo.WinHead.align;
    new_shift_count = Pass2Info.TmpFile.u.NEInfo.WinHead.align;

    ret = CPSEG_OK;
    cponeret = CPSEG_OK;

    for( currseg = 0; currseg < num_segs; ++currseg ) {
        cponeret = copyOneSegment( oldseg, tmpseg, old_exe_info,
                                    tmp_exe_info, old_shift_count,
                                    new_shift_count, FALSE );
        if( cponeret == CPSEG_SEG_TOO_BIG ) {
            ret = CPSEG_SEG_TOO_BIG;
        }
        CheckDebugOffset( &(Pass2Info.OldFile) );
        CheckDebugOffset( &(Pass2Info.TmpFile) );
        if( cponeret == CPSEG_ERROR ) {
            break;
        }
        oldseg++;
        tmpseg++;
    }

    if( cponeret == CPSEG_ERROR ) {
        ret = CPSEG_ERROR;
        Pass2Info.TmpFile.u.NEInfo.Seg.NumSegs = 0;
    } else if( old_seg_tbl->NumOS2ResSegs ) {
        uint_32     end_offset;

        /* Must seek past the last segment in old file */
        oldseg += old_seg_tbl->NumOS2ResSegs - 1;
        end_offset = (uint_32)oldseg->address << old_shift_count;
        if( oldseg->size == 0 )
            end_offset += 0x10000;
        else
            end_offset += oldseg->size;

        if( RCSEEK( old_exe_info->Handle, end_offset, SEEK_SET ) == -1 ) {
            ret = CPSEG_ERROR;
            RcError( ERR_READING_EXE, old_exe_info->name, strerror( errno ) );
        }
        CheckDebugOffset( &(Pass2Info.OldFile) );
    }

    return( ret );
} /* CopyOS2Segments */
