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
* Description:  NE segment processing functions.
*
****************************************************************************/


#include "global.h"
#include <errno.h>
#include "rcerrors.h"
#include "rcrtns.h"
#include "rccore_2.h"
#include "exeutil.h"
#include "exeseg.h"
#include "exeres.h"

#include "clibext.h"


static RcStatus allocSegTable( SegTable *seg, int *err_code )
/***********************************************************/
{
    int     tablesize;

    tablesize = seg->NumSegs * sizeof( segment_record );
    seg->Segments = RESALLOC( tablesize );
    if( seg->Segments == NULL ) {
        *err_code = errno;
        return( RS_NO_MEM );
    } else {
        return( RS_OK );
    }
} /* allocSegTable */

static RcStatus readSegTable( FILE *fp, uint_32 offset, segment_record *segments, size_t size )
/**********************************************************************************************
 * NB when an error occurs this function must return without altering errno
 */
{
    size_t          numread;

    if( RESSEEK( fp, offset, SEEK_SET ) )
        return( RS_READ_ERROR );
    size *= sizeof( *segments );
    numread = RESREAD( fp, segments, size );
    if( numread != size ) {
        return( RESIOERR( fp, numread ) ? RS_READ_ERROR : RS_READ_INCMPLT );
    }
    return( RS_OK );

} /* readSegTable */

RcStatus AllocAndReadWINSegTables( ExeFileInfo *src, ExeFileInfo *dst, int *err_code )
/************************************************************************************/
{
    RcStatus            ret;
    uint_32             segments_off;
    SegTable            *src_seg;
    SegTable            *dst_seg;
    size_t              num_segs;

    src_seg = &(src->u.NEInfo.Seg);
    dst_seg = &(dst->u.NEInfo.Seg);

    segments_off = src->WinHeadOffset + src->u.NEInfo.WinHead.segment_off;

    num_segs = src->u.NEInfo.WinHead.segments;
    src_seg->NumSegs = num_segs;
    dst_seg->NumSegs = num_segs;
    ret = allocSegTable( src_seg, err_code );
    if( ret != RS_OK )
        return( ret );
    ret = allocSegTable( dst_seg, err_code );
    if( ret != RS_OK )
        return( ret );

    ret = readSegTable( src->fp, segments_off, src_seg->Segments, num_segs );
    if( ret != RS_OK ) {
        *err_code = errno;
        return( ret );
    }
    ret = readSegTable( src->fp, segments_off, dst_seg->Segments, num_segs );
    *err_code = errno;
    return( ret );

} /* AllocAndReadWINSegTables */

RcStatus AllocAndReadOS2SegTables( ExeFileInfo *src, ExeFileInfo *dst, ResFileInfo *res, int *err_code )
/******************************************************************************************************/
{
    RcStatus            ret;
    int                 src_res;
    int                 dst_res;
    uint_32             segments_off;
    SegTable            *src_seg;
    SegTable            *dst_seg;
    size_t              num_segs;

    src_res = src->u.NEInfo.WinHead.resource;
    src_seg = &(src->u.NEInfo.Seg);
    dst_seg = &(dst->u.NEInfo.Seg);
    dst_res = ComputeOS2ResSegCount( res->Dir );

    segments_off = src->WinHeadOffset + src->u.NEInfo.WinHead.segment_off;

    num_segs = src->u.NEInfo.WinHead.segments;
    if( num_segs < src_res )
        return( RS_BAD_FILE_FMT );

    src_seg->NumSegs = num_segs;
    dst_seg->NumSegs = num_segs - src_res + dst_res;
    src_seg->NumOS2ResSegs = src_res;
    dst_seg->NumOS2ResSegs = dst_res;

    ret = allocSegTable( src_seg, err_code );
    if( ret != RS_OK )
        return( ret );

    ret = allocSegTable( dst_seg, err_code );
    if( ret != RS_OK )
        return( ret );

    ret = readSegTable( src->fp, segments_off, src_seg->Segments, num_segs );
    if( ret != RS_OK ) {
        *err_code = errno;
        return( ret );
    }
    if( num_segs > dst_seg->NumSegs )
        num_segs = dst_seg->NumSegs;
    ret = readSegTable( src->fp, segments_off, dst_seg->Segments, num_segs );
    *err_code = errno;
    return( ret );
} /* AllocAndReadOS2SegTables */


/********* WARNING *********************
 * Hard coded constant. The value of sizeof( os2_reloc_item ) is to hard
 * to get from  wl  because of the other files that would have to be included
 * in order to get that structure
 */
#define OS_RELOC_ITEM_SIZE      8

uint_32 ComputeSegmentSize( FILE *fp, SegTable *segs, int shift_count )
/**********************************************************************/
{
    segment_record  *currseg;
    segment_record  *afterlast;
    uint_32         length;
    uint_16         numrelocs;

    length = 0;
    for( currseg = segs->Segments, afterlast = segs->Segments + segs->NumSegs; currseg < afterlast; currseg++ ) {
        length += currseg->size;
        if( currseg->info & SEG_RELOC ) {
            if( RESSEEK( fp, (((long)currseg->address) << (long)shift_count) + currseg->size, SEEK_SET ) )
                return( 0 );
            if( RESREAD( fp, &numrelocs, sizeof( numrelocs ) ) != sizeof( numrelocs ) )
                return( 0 );
            length += (unsigned_32)( (unsigned_32)numrelocs * (unsigned_32)OS_RELOC_ITEM_SIZE );
        }
    }

    return( length );

} /* ComputeSegmentSize */

static bool myCopyExeData( ExeFileInfo *src, ExeFileInfo *dst, uint_32 length )
{
    switch( CopyExeData( src->fp, dst->fp, length ) ) {
    case RS_OK:
    case RS_PARAM_ERROR:
        return( false );
    case RS_READ_ERROR:
        RcError( ERR_READING_EXE, src->name, strerror( errno ) );
        break;
    case RS_READ_INCMPLT:
        RcError( ERR_UNEXPECTED_EOF, src->name );
        break;
    case RS_WRITE_ERROR:
        RcError( ERR_WRITTING_FILE, dst->name, strerror( errno ) );
        break;
    default:
        RcError( ERR_INTERNAL, INTERR_UNKNOWN_RCSTATUS );
        break;
    }
    return( true );
}


static CpSegRc copyOneSegment( const segment_record *src_seg,
            segment_record *dst_seg, ExeFileInfo *src, ExeFileInfo *dst,
            int src_shift_count, int dst_shift_count, bool pad_end )
{
    CpSegRc         ret;
    bool            error;
    size_t          numread;
    uint_16         numrelocs;
    long            dst_offset;
    long            align_amount;
    uint_32         seg_len = 0L;
    char            zero;

    zero = 0;
    error = false;
    ret = CPSEG_OK;
    /*
     * check if this is a segment that has no image in the exe file
     */
    if( src_seg->address != 0 ) {
        /*
         * align in the dst file so that shift_count will be valid
         */
        dst_offset = RESTELL( dst->fp );
        if( dst_offset == -1 ) {
            error = true;
            RcError( ERR_WRITTING_FILE, dst->name, strerror( errno ) );
        }
        if( !error ) {
            align_amount = AlignAmount( dst_offset, dst_shift_count );
            if( RESSEEK( dst->fp, align_amount, SEEK_CUR ) ) {
                error = true;
                RcError( ERR_WRITTING_FILE, dst->name, strerror( errno ) );
            }
            dst_offset += align_amount;
        }
        /*
         * move in the in file to the start of the segment
         */
        if( !error ) {
            /*
             * convert the address to a long before shifting it
             */
            if( RESSEEK( src->fp, (long)src_seg->address << src_shift_count, SEEK_SET ) ) {
                error = true;
                RcError( ERR_READING_EXE, src->name, strerror( errno ) );
            }
        }

        if( !error ) {
            if( src_seg->size == 0 ) {
                seg_len = 0x10000L;
            } else {
                seg_len = src_seg->size;
            }
            error = myCopyExeData( src, dst, seg_len );
        }

        if( (src_seg->info & SEG_RELOC) && !error ) {
            /*
             * read the number of relocation items
             */
            numread = RESREAD( src->fp, &numrelocs, sizeof( numrelocs ) );
            if( numread != sizeof( numrelocs ) ) {
                error = true;
                if( RESIOERR( src->fp, numread ) ) {
                    RcError( ERR_READING_EXE, src->name, strerror( errno ) );
                } else {
                    RcError( ERR_UNEXPECTED_EOF, src->name );
                }
            } else {
                if( RESWRITE( dst->fp, &numrelocs, sizeof( numrelocs ) ) != sizeof( numrelocs ) ) {
                    error = true;
                    RcError( ERR_WRITTING_FILE, dst->name, strerror( errno ) );
                }
            }
            /*
             * copy the relocation information
             */
            if( !error ) {
                error = myCopyExeData( src, dst, numrelocs * OS_RELOC_ITEM_SIZE );
            }
            if( numrelocs * OS_RELOC_ITEM_SIZE + seg_len > 0x10000L ) {
                ret = CPSEG_SEG_TOO_BIG;
            }
        }

        if( pad_end && ret != CPSEG_SEG_TOO_BIG && !error ) {
            align_amount = AlignAmount( RESTELL( dst->fp ), dst_shift_count );
            /*
             * make sure there is room for the memory arena header
             */
            if( align_amount < 16 ) {
                align_amount += 16;
            }
            if( RESSEEK( dst->fp, align_amount - 1, SEEK_CUR ) ) {
                error = true;
                RcError( ERR_WRITTING_FILE, dst->name );
            } else {
                /*
                 * write something out so if we have just seeked past the
                 * end of the file the file's size will be adjusted
                 * appropriately
                 */
                if( RESWRITE( dst->fp, &zero, 1 ) != 1 ) {
                    error = true;
                    RcError( ERR_WRITTING_FILE, dst->name );
                }
            }
        }
    } else {
        dst_offset = 0;
    }
    /*
     * copy the segment record to dst_seg
     */
    if( !error ) {
        dst_seg->size = src_seg->size;
        dst_seg->info = src_seg->info;
        dst_seg->min = src_seg->min;
        dst_seg->address = dst_offset >> dst_shift_count;
    }

    if( error ) {
        ret = CPSEG_ERROR;
    }

    return( ret );
} /* copyOneSegment */


CpSegRc CopyWINSegments( ExeFileInfo *src, ExeFileInfo *dst, uint_16 sect2mask, uint_16 sect2bits, bool sect2 )
/**************************************************************************************************************
 * Note: sect2 must be either 1 (do section 2) or 0 (do section 1)
 * CopyWINSegments should be called twice, once with sect2 false, and once with
 * it true. The values of sect2mask and sect2bits should be the same for both
 * calls. The segment table for the temporary file will not be properly
 * filled in until after the second call
 */
{
    segment_record      *src_seg;
    segment_record      *dst_seg;
    bool                padend;
    int                 num_segs;
    int                 src_shift_count;
    int                 dst_shift_count;
    int                 currseg;
    CpSegRc             cponeret;
    CpSegRc             ret;

    src_seg = src->u.NEInfo.Seg.Segments;
    dst_seg = dst->u.NEInfo.Seg.Segments;
    num_segs = src->u.NEInfo.Seg.NumSegs;
    src_shift_count = src->u.NEInfo.WinHead.align;
    dst_shift_count = dst->u.NEInfo.WinHead.align;

    ret = CPSEG_OK;
    cponeret = CPSEG_OK;
    padend = !sect2;

    for( currseg = 0; currseg < num_segs; ++currseg ) {
        /*
         * if the bits are unequal and this is section 1 --> copy segment
         * if the bits are equal and this is section 2   --> copy segment
         * otherwise                                     --> do nothing
         */
        if( ARE_BITS_EQUAL( sect2mask, sect2bits, src_seg->info ) == sect2 ) {
            cponeret = copyOneSegment( src_seg, dst_seg, src, dst,
                                src_shift_count, dst_shift_count, padend );
            if( cponeret == CPSEG_SEG_TOO_BIG ) {
                padend = false;
                ret = CPSEG_SEG_TOO_BIG;
            }
            CheckDebugOffset( src );
            CheckDebugOffset( dst );
        }
        if( cponeret == CPSEG_ERROR )
            break;
        /*
         * mark section 1 segments as preload
         */
        if( !sect2 ) {
            dst_seg->info |= SEG_PRELOAD;
        }
        src_seg++;
        dst_seg++;
    }

    if( cponeret == CPSEG_ERROR ) {
        ret = CPSEG_ERROR;
        dst->u.NEInfo.Seg.NumSegs = 0;
    } else {
        dst->u.NEInfo.Seg.NumSegs = src->u.NEInfo.Seg.NumSegs;
    }

    return( ret );

} /* CopyWINSegments */

CpSegRc CopyOS2Segments( ExeFileInfo *src, ExeFileInfo *dst )
/************************************************************
 * Akin to CopyWINSegments() only much, much simpler - just copies all segments
 * without messing with them in any way. Only called once. Won't copy
 * resource segments.
 */
{
    segment_record      *src_seg;
    segment_record      *dst_seg;
    int                 num_exesegs;
    int                 src_shift_count;
    int                 dst_shift_count;
    int                 currseg;
    CpSegRc             cponeret;
    CpSegRc             ret;

    src_seg = src->u.NEInfo.Seg.Segments;
    dst_seg = dst->u.NEInfo.Seg.Segments;
    num_exesegs = src->u.NEInfo.Seg.NumSegs - src->u.NEInfo.Seg.NumOS2ResSegs;
    src_shift_count = src->u.NEInfo.WinHead.align;
    dst_shift_count = dst->u.NEInfo.WinHead.align;

    ret = CPSEG_OK;
    cponeret = CPSEG_OK;

    for( currseg = 0; currseg < num_exesegs; ++currseg ) {
        cponeret = copyOneSegment( src_seg, dst_seg, src, dst,
                                src_shift_count, dst_shift_count, false );
        if( cponeret == CPSEG_SEG_TOO_BIG ) {
            ret = CPSEG_SEG_TOO_BIG;
        }
        CheckDebugOffset( src );
        CheckDebugOffset( dst );
        if( cponeret == CPSEG_ERROR ) {
            break;
        }
        src_seg++;
        dst_seg++;
    }

    if( cponeret == CPSEG_ERROR ) {
        ret = CPSEG_ERROR;
        dst->u.NEInfo.Seg.NumSegs = 0;
    } else if( src->u.NEInfo.Seg.NumOS2ResSegs ) {
        uint_32     end_offset;

        /*
         * Must seek past the last segment in src file
         */
        src_seg += src->u.NEInfo.Seg.NumOS2ResSegs - 1;
        end_offset = (uint_32)src_seg->address << src_shift_count;
        if( src_seg->size == 0 ) {
            end_offset += 0x10000;
        } else {
            end_offset += src_seg->size;
        }
        if( RESSEEK( src->fp, end_offset, SEEK_SET ) ) {
            ret = CPSEG_ERROR;
            RcError( ERR_READING_EXE, src->name, strerror( errno ) );
        }
        CheckDebugOffset( src );
    }

    return( ret );
} /* CopyOS2Segments */
