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
* Description:  Resource Compiler pass 2.
*
****************************************************************************/


#include <stddef.h>
#include <errno.h>
#include "global.h"
#include "rcerrors.h"
#include "rcstrblk.h"
#include "rcrtns.h"
#include "rccore_2.h"
#include "exeutil.h"
#include "exeseg.h"
#include "exeres.h"
#include "exeobj.h"
#include "exerespe.h"
#include "exelxobj.h"
#include "exereslx.h"

#include "clibext.h"


bool    StopInvoked = false;

static RcStatus copyStubFile( ExeFileInfo *src, ExeFileInfo *dst, int *err_code )
/********************************************************************************
 * copy from the begining of the file to the start of the win exe header
 */
{
    RcStatus    ret;

    ret = CopyExeData( src->fp, dst->fp, src->WinHeadOffset );
    *err_code = errno;
    return( ret );
} /* copyStubFile */

static RcStatus seekPastResTable( ExeFileInfo *src, ExeFileInfo *dst, int *err_code )
{
    long            winheadoffset;
    long            seekamount;
    uint_16         restab_size;

    if( src->u.NEInfo.WinHead.target == TARGET_OS2 ) {
        restab_size = dst->u.NEInfo.OS2Res.table_size;
    } else {
        restab_size = dst->u.NEInfo.Res.Dir.TableSize;
    }

    seekamount = sizeof( os2_exe_header ) +
                    dst->u.NEInfo.Seg.NumSegs * sizeof( segment_record ) +
                    restab_size +
                    dst->u.NEInfo.Res.Str.StringBlockSize;
    if( RESSEEK( dst->fp, seekamount, SEEK_CUR ) ) {
        *err_code = errno;
        return( RS_READ_ERROR );
    }
    winheadoffset = RESTELL( dst->fp );
    dst->WinHeadOffset = winheadoffset;
    return( RS_OK );

} /* seekPastResTable */

static RcStatus copyOtherTables( ExeFileInfo *src, ExeFileInfo *dst, int *err_code )
{
    uint_32         tablelen;
    RcStatus        ret;

    /*
     * the other tables start at the resident names table and end at the end
     * of the non-resident names table
     */
    tablelen = (src->u.NEInfo.WinHead.nonres_off + src->u.NEInfo.WinHead.nonres_size) - ( src->WinHeadOffset + src->u.NEInfo.WinHead.resident_off );

    if( RESSEEK( src->fp, src->WinHeadOffset + src->u.NEInfo.WinHead.resident_off, SEEK_SET ) ) {
        *err_code = errno;
        return( RS_READ_ERROR );
    }

    ret = CopyExeData( src->fp, dst->fp, tablelen );
    *err_code = errno;
    return( ret );
} /* copyOtherTables */

static int computeShiftCount( ExeFileInfo *src, ExeFileInfo *dst, ResFileInfo *res )
{
    uint_32     filelen;
    int         num_segs;
    int         shift_count;

    filelen = src->u.NEInfo.WinHead.nonres_off +
            src->u.NEInfo.WinHead.nonres_size +
            dst->u.NEInfo.Res.Dir.TableSize +
            dst->u.NEInfo.Res.Str.StringBlockSize;
    filelen += ComputeSegmentSize( src->fp, &(dst->u.NEInfo.Seg), src->u.NEInfo.WinHead.align );
    if( ! CmdLineParms.NoResFile ) {
        filelen += ComputeWINResourceSize( res->Dir );
    }

    num_segs = src->u.NEInfo.WinHead.segments;
    if( ! CmdLineParms.NoResFile ) {
        num_segs += WResGetNumResources( res->Dir );
    }

    shift_count = FindShiftCount( filelen, num_segs );
    if( shift_count == 0 ) {
        shift_count = 1;        /* MS thinks 0 == 9 for shift count */
    }

    if( shift_count > src->u.NEInfo.WinHead.align ) {
        return( shift_count );
    } else {
        return( src->u.NEInfo.WinHead.align );
    }

} /* computeShiftCount */

static void checkShiftCount( ExeFileInfo *src, ExeFileInfo *dst, ResFileInfo *res )
{
    dst->u.NEInfo.WinHead.align = computeShiftCount( src, dst, res );
    dst->u.NEInfo.Res.Dir.ResShiftCount = dst->u.NEInfo.WinHead.align;
} /* checkShiftCount */

static bool copyWINBody( ExeFileInfo *src, ExeFileInfo *dst, ResFileInfo *res )
{
    uint_16             sect2mask = 0;
    uint_16             sect2bits = 0;
    uint_16             shift_count;
    long                gangloadstart;
    long                gangloadlen;
    CpSegRc             copy_segs_ret;
    bool                use_gangload = false;

    switch( CmdLineParms.SegmentSorting ) {
    case SEG_SORT_NONE:
        /*
         * all segments in section 2
         */
        sect2mask = 0;
        sect2bits = 0;
        use_gangload = false;
        dst->u.NEInfo.WinHead.align = src->u.NEInfo.WinHead.align;
        dst->u.NEInfo.Res.Dir.ResShiftCount = computeShiftCount( src, dst, res );
        break;
    case SEG_SORT_PRELOAD_ONLY:
        /*
         * all load on call segments in section 2
         */
        sect2mask = SEG_PRELOAD;
        sect2bits = 0;
        use_gangload = true;
        checkShiftCount( src, dst, res );
        break;
    case SEG_SORT_MANY:
        /*
         * only load on call, discardable, code segments in section 2
         */
        sect2mask = SEG_DATA | SEG_PRELOAD | SEG_DISCARD;
        sect2bits = SEG_DISCARD;
        /*
         * set the entry segment to be preload
         */
        {
            segment_record      *seg;       /* these two are here because a */
            uint_16             entry_seg;  /* 71 character field reference is hard to read */
            seg = src->u.NEInfo.Seg.Segments;
            entry_seg = src->u.NEInfo.WinHead.entrynum;
            seg[entry_seg].info |= SEG_PRELOAD;
        }

        use_gangload = true;
        checkShiftCount( src, dst, res );
        break;
    default:
        break;
    }
    /*
     * third arg to Copy???? is false --> copy section one
     */
    gangloadstart = RESTELL( dst->fp );
    gangloadstart += AlignAmount( gangloadstart, dst->u.NEInfo.Res.Dir.ResShiftCount );
    copy_segs_ret = CopyWINSegments( src, dst, sect2mask, sect2bits, false );
    switch( copy_segs_ret ) {
    case CPSEG_SEG_TOO_BIG:
        if( use_gangload ) {
            RcWarning( ERR_NO_GANGLOAD );
            use_gangload = false;
        }
        break;
    case CPSEG_ERROR:
        return( true );
    case CPSEG_OK:
    default:
        break;
    }
    if( ! CmdLineParms.NoResFile ) {
        if( CopyWINResources( dst, res, sect2mask, sect2bits, false ) != RS_OK ) {
            return( true );
        }
    }
    gangloadlen = RESTELL( dst->fp ) - gangloadstart;
    /*
     * third arg to Copy???? is true  --> copy section two
     */
    copy_segs_ret = CopyWINSegments( src, dst, sect2mask, sect2bits, true );
    if( copy_segs_ret == CPSEG_ERROR ) {
        return( true );
    }
    if( !CmdLineParms.NoResFile ) {
        if( CopyWINResources( dst, res, sect2mask, sect2bits, true ) != RS_OK ) {
            return( true );
        }
    }

    if( use_gangload ) {
        shift_count = dst->u.NEInfo.WinHead.align;
        dst->u.NEInfo.WinHead.gangstart = gangloadstart >> shift_count;
        dst->u.NEInfo.WinHead.ganglength = gangloadlen >> shift_count;
        dst->u.NEInfo.WinHead.otherflags |= WIN_GANGLOAD_PRESENT;
    } else {
        dst->u.NEInfo.WinHead.gangstart = 0;
        dst->u.NEInfo.WinHead.ganglength = 0;
        dst->u.NEInfo.WinHead.otherflags &= ~WIN_GANGLOAD_PRESENT;
    }

    return( false );

} /* copyBody */

static bool copyOS2Body( ExeFileInfo *src, ExeFileInfo *dst, ResFileInfo *res )
{
    CpSegRc             copy_segs_ret;
    unsigned_16         align;

    /*
     * OS/2 does not use separate alignment for resources
     */
    align = src->u.NEInfo.WinHead.align;
    dst->u.NEInfo.WinHead.align = align;
    dst->u.NEInfo.Res.Dir.ResShiftCount = align;

    copy_segs_ret = CopyOS2Segments( src, dst );
    if( copy_segs_ret == CPSEG_ERROR ) {
        return( true );
    }
    if( !CmdLineParms.NoResFile ) {
        if( CopyOS2Resources( dst, res ) != RS_OK ) {
            return( true );
        }
    }

    return( false );
} /* copyOS2Body */

static RcStatus copyDebugInfo( ExeFileInfo *src, ExeFileInfo *dst )
/******************************************************************
 * NB when an error occurs this function must return without altering errno
 */
{
    if( RESSEEK( src->fp, src->DebugOffset, SEEK_SET ) )
        return( RS_READ_ERROR );
    if( RESSEEK( dst->fp, dst->DebugOffset, SEEK_SET ) )
        return( RS_WRITE_ERROR );
    return( CopyExeDataTilEOF( src->fp, dst->fp ) );

} /* copyDebugInfo */

static RcStatus writeHeadAndTables( ExeFileInfo *src, ExeFileInfo *dst, int *err_code )
{
    uint_16         tableshift;     /* amount the tables are shifted in the dst file */
    uint_16         info;           /* os2_exe_header.info */
    RcStatus        ret;

    /*
     * set the info flag for the dst executable from the one flag and
     * the command line options given
     */
    info = src->u.NEInfo.WinHead.info;
    if( CmdLineParms.PrivateDLL ) {
        info |= WIN_PRIVATE_DLL;
    }
    if( CmdLineParms.GlobalMemEMS ) {
        info |= WIN_EMS_GLOBAL_MEM;
    }
    if( CmdLineParms.EMSInstance ) {
        info |= WIN_EMS_BANK_INSTANCE;
    }
    if( CmdLineParms.EMSDirect ) {
        info |= WIN_USES_EMS_DIRECT;
    }
    if( CmdLineParms.ProtModeOnly ) {
        info |= OS2_PROT_MODE_ONLY;
    }
    /*
     * copy the fields in the os2_exe_header then change some of them
     */
    dst->WinHeadOffset = src->WinHeadOffset;
    /*
     * copy the WinHead fields up to, but excluding, the segment_off field
     */
    memcpy( &(dst->u.NEInfo.WinHead), &(src->u.NEInfo.WinHead),
            offsetof(os2_exe_header, segment_off) );
    dst->u.NEInfo.WinHead.info = info;
    tableshift = dst->u.NEInfo.Res.Dir.TableSize +
                dst->u.NEInfo.Res.Str.StringBlockSize -
                (src->u.NEInfo.WinHead.resident_off - src->u.NEInfo.WinHead.resource_off );
    dst->u.NEInfo.WinHead.entry_off = src->u.NEInfo.WinHead.entry_off + tableshift;
    dst->u.NEInfo.WinHead.resident_off = src->u.NEInfo.WinHead.resident_off + tableshift;
    dst->u.NEInfo.WinHead.module_off = src->u.NEInfo.WinHead.module_off + tableshift;
    dst->u.NEInfo.WinHead.import_off = src->u.NEInfo.WinHead.import_off + tableshift;
    dst->u.NEInfo.WinHead.nonres_off = src->u.NEInfo.WinHead.nonres_off + tableshift;
    dst->u.NEInfo.WinHead.segment_off = sizeof( os2_exe_header );
    dst->u.NEInfo.WinHead.resource_off = dst->u.NEInfo.WinHead.segment_off +
                                dst->u.NEInfo.Seg.NumSegs * sizeof( segment_record );
    dst->u.NEInfo.WinHead.movable = src->u.NEInfo.WinHead.movable;
    dst->u.NEInfo.WinHead.resource = dst->u.NEInfo.Res.Dir.NumResources;
    dst->u.NEInfo.WinHead.target = src->u.NEInfo.WinHead.target;
    /* |= the next one since the WIN_GANGLOAD_PRESENT flag may be set */
    dst->u.NEInfo.WinHead.otherflags |= src->u.NEInfo.WinHead.otherflags;
    dst->u.NEInfo.WinHead.swaparea =   0;      /* What is this field for? */
    if( CmdLineParms.VersionStamp30 ) {
        dst->u.NEInfo.WinHead.expver = VERSION_30_STAMP;
    } else {
        dst->u.NEInfo.WinHead.expver = VERSION_31_STAMP;
    }
    /*
     * seek to the start of the os2_exe_header in dst
     */
    if( RESSEEK( dst->fp, dst->WinHeadOffset, SEEK_SET ) ) {
        *err_code = errno;
        return( RS_WRITE_ERROR );
    }
    /*
     * write the header
     */
    if( RESWRITE( dst->fp, &(dst->u.NEInfo.WinHead), sizeof( os2_exe_header ) ) != sizeof( os2_exe_header ) ) {
        *err_code = errno;
        return( RS_WRITE_ERROR );
    }
    /*
     * write the segment table
     */
    if( dst->u.NEInfo.Seg.NumSegs > 0 ) {
        size_t  numwrite;

        numwrite = dst->u.NEInfo.Seg.NumSegs * sizeof( segment_record );
        if( RESWRITE( dst->fp, dst->u.NEInfo.Seg.Segments, numwrite ) != numwrite ) {
            *err_code = errno;
            return( RS_WRITE_ERROR );
        }
    }
    /*
     * write the resource table
     */
    ret = WriteWINResTable( dst->fp, &(dst->u.NEInfo.Res), err_code );
    return( ret );

} /* writeHeadAndTables */

static RcStatus writeOS2HeadAndTables( ExeFileInfo *src, ExeFileInfo *dst, int *err_code )
/*****************************************************************************************
 * Processing OS/2 NE modules is very very similar to Windows NE processing
 * but there are enough differences in detail to warrant separate
 * implementation to keep the two cleaner.
 */
{
    uint_16         tableshift;     /* amount the tables are shifted in the dst file */
    RcStatus        ret;

    /*
     * copy the fields in the os2_exe_header then change some of them
     */
    dst->WinHeadOffset = src->WinHeadOffset;
    /*
     * copy the WinHead fields up to, but excluding, the segment_off field
     */
    memcpy( &(dst->u.NEInfo.WinHead), &(src->u.NEInfo.WinHead), offsetof(os2_exe_header, segment_off) );
    dst->u.NEInfo.WinHead.info = src->u.NEInfo.WinHead.info;
    dst->u.NEInfo.WinHead.segment_off = sizeof( os2_exe_header );
    dst->u.NEInfo.WinHead.resource_off = dst->u.NEInfo.WinHead.segment_off +
                                dst->u.NEInfo.Seg.NumSegs * sizeof( segment_record );
    tableshift = dst->u.NEInfo.OS2Res.table_size -
                (src->u.NEInfo.WinHead.resident_off - src->u.NEInfo.WinHead.resource_off) +
                (dst->u.NEInfo.WinHead.resource_off - src->u.NEInfo.WinHead.resource_off);
    dst->u.NEInfo.WinHead.entry_off = src->u.NEInfo.WinHead.entry_off + tableshift;
    dst->u.NEInfo.WinHead.resident_off = src->u.NEInfo.WinHead.resident_off + tableshift;
    dst->u.NEInfo.WinHead.module_off = src->u.NEInfo.WinHead.module_off + tableshift;
    dst->u.NEInfo.WinHead.import_off = src->u.NEInfo.WinHead.import_off + tableshift;
    dst->u.NEInfo.WinHead.nonres_off = src->u.NEInfo.WinHead.nonres_off + tableshift;
    dst->u.NEInfo.WinHead.movable    = src->u.NEInfo.WinHead.movable;
    dst->u.NEInfo.WinHead.resource   = dst->u.NEInfo.OS2Res.num_res_segs;
    dst->u.NEInfo.WinHead.target     = src->u.NEInfo.WinHead.target;
    dst->u.NEInfo.WinHead.otherflags = src->u.NEInfo.WinHead.otherflags;
    dst->u.NEInfo.WinHead.segments   = dst->u.NEInfo.Seg.NumSegs;
    /*
     * seek to the start of the os2_exe_header in dst
     */
    if( RESSEEK( dst->fp, dst->WinHeadOffset, SEEK_SET ) ) {
        *err_code = errno;
        return( RS_WRITE_ERROR );
    }
    /*
     * write the header
     */
    if( RESWRITE( dst->fp, &(dst->u.NEInfo.WinHead), sizeof( os2_exe_header ) ) != sizeof( os2_exe_header ) ) {
        *err_code = errno;
        return( RS_WRITE_ERROR );
    }
    /*
     * write the segment table
     */
    if( dst->u.NEInfo.Seg.NumSegs > 0 ) {
        size_t  numwrite;

        numwrite = dst->u.NEInfo.Seg.NumSegs * sizeof( segment_record );
        if( RESWRITE( dst->fp, dst->u.NEInfo.Seg.Segments, numwrite ) != numwrite ) {
            *err_code = errno;
            return( RS_WRITE_ERROR );
        }
    }
    /*
     * write the resource table
     */
    ret = WriteOS2ResTable( dst->fp, &(dst->u.NEInfo.OS2Res), err_code );
    return( ret );

} /* writeOS2HeadAndTables */

static RcStatus findEndOfResources( ExeFileInfo *src, int *err_code )
/********************************************************************
 * if this exe already contains resources find the end of them so we don't
 * copy them with debug information.  Otherwise the file will grow whenever
 * a resource file is added
 */
{
    size_t                      numread;
    unsigned                    i;
    uint_16                     alignshift;
    uint_32                     end;
    uint_32                     tmp;
    resource_type_record        typeinfo;
    resource_record             nameinfo;

    if( src->u.NEInfo.WinHead.resource_off == src->u.NEInfo.WinHead.resident_off ) {
        return( RS_OK );
    }

    if( RESSEEK( src->fp, src->WinHeadOffset + src->u.NEInfo.WinHead.resource_off, SEEK_SET ) ) {
        *err_code = errno;
        return( RS_READ_ERROR );
    }

    numread = RESREAD( src->fp, &alignshift, sizeof( alignshift ) );
    if( numread != sizeof( alignshift ) ) {
        *err_code = errno;
        return( RESIOERR( src->fp, numread ) ? RS_READ_ERROR : RS_READ_INCMPLT );
    }
    alignshift = 1 << alignshift;

    numread = RESREAD( src->fp, &typeinfo, sizeof( typeinfo ) );
    if( numread != sizeof( typeinfo ) )  {
        *err_code = errno;
        return( RESIOERR( src->fp, numread ) ? RS_READ_ERROR : RS_READ_INCMPLT );
    }
    end = 0;
    while( typeinfo.type != 0 ) {
        for( i = typeinfo.num_resources; i > 0 ; --i ) {
            numread = RESREAD( src->fp, &nameinfo, sizeof( nameinfo ) );
            if( numread != sizeof( nameinfo ) ) {
                *err_code = errno;
                return( RESIOERR( src->fp, numread ) ? RS_READ_ERROR : RS_READ_INCMPLT );
            }
            tmp = nameinfo.offset + nameinfo.length;
            if( end < tmp ) {
                end = tmp;
            }
        }
        numread = RESREAD( src->fp, &typeinfo, sizeof( typeinfo ) );
        if( numread != sizeof( typeinfo ) ) {
            *err_code = errno;
            return( RESIOERR( src->fp, numread ) ? RS_READ_ERROR : RS_READ_INCMPLT );
        }
    }
    end *= alignshift;
    if( src->DebugOffset < end ) {
        src->DebugOffset = end;
    }
    return( RS_OK );
}

static RcStatus writePEHeadAndObjTable( ExeFileInfo *dst )
/*********************************************************
 * NB when an error occurs this function must return without altering errno
 */
{
    pe_object       *last_object;
    int             i;
    uint_32         image_size;
    int             num_objects;
    unsigned_32     object_align;
    pe_exe_header   *pehdr;

    /*
     * adjust the image size in the header
     */
    pehdr = dst->u.PEInfo.WinHead;
    num_objects = pehdr->fheader.num_objects;
    object_align = PE( *pehdr, object_align );
    last_object = &dst->u.PEInfo.Objects[num_objects - 1];
    image_size = last_object->rva + last_object->physical_size;
    image_size = ALIGN_VALUE( image_size, object_align );
    PE( *pehdr, image_size ) = image_size;

    if( RESSEEK( dst->fp, dst->WinHeadOffset, SEEK_SET ) )
        return( RS_WRITE_ERROR );

    if( RESWRITE( dst->fp, pehdr, PE_SIZE( *pehdr ) ) != PE_SIZE( *pehdr ) ) {
        return( RS_WRITE_ERROR );
    }

    for( i = 0; i < num_objects; i++ ) {
        if( RESWRITE( dst->fp, dst->u.PEInfo.Objects + i, sizeof( pe_object ) ) != sizeof( pe_object ) ) {
            return( RS_WRITE_ERROR );
        }
    }

    return( RS_OK );

} /* writePEHeadAndObjTable */

bool MergeResExeWINNE( ExeFileInfo *src, ExeFileInfo *dst, ResFileInfo *res )
/****************************************************************************
 * Windows NE files store resources in a special data structure. OS/2 NE
 * modules are quite different and store each resource in its own data
 * segment(s). The OS/2 resource table is completely different as well and
 * only contains resource types/IDs.
 */
{
    RcStatus        ret;
    bool            error;
    int             err_code;

    ret = copyStubFile( src, dst, &err_code );
    if( ret != RS_OK )
        goto REPORT_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    ret = AllocAndReadWINSegTables( src, dst, &err_code );
    if( ret != RS_OK )
        goto REPORT_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    InitWINResTable( dst, res );

    ret = seekPastResTable( src, dst, &err_code );
    if( ret != RS_OK )
        goto REPORT_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    ret = findEndOfResources( src, &err_code );
    if( ret != RS_OK )
        goto REPORT_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    ret = copyOtherTables( src, dst, &err_code );
    if( ret != RS_OK )
        goto REPORT_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    error = copyWINBody( src, dst, res );
    if( error )
        goto HANDLE_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    ret = copyDebugInfo( src, dst );
    if( ret != RS_OK ) {
        err_code = errno;
        goto REPORT_ERROR;
    }
    if( StopInvoked )
        goto STOP_ERROR;

    ret = writeHeadAndTables( src, dst, &err_code );
    if( ret != RS_OK )
        goto REPORT_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    return( true );

REPORT_ERROR:
    switch( ret ) {
    case RS_READ_ERROR:
        RcError( ERR_READING_EXE, src->name, strerror( err_code ) );
        break;
    case RS_READ_INCMPLT:
        RcError( ERR_UNEXPECTED_EOF, src->name );
        break;
    case RS_WRITE_ERROR:
        RcError( ERR_WRITTING_TMP, dst->name, strerror( err_code ) );
        break;
    case RS_NO_MEM:
        break;
    default:
        RcError( ERR_INTERNAL, INTERR_UNKNOWN_RCSTATUS );
    }
    /* fall through */
HANDLE_ERROR:
    return( false );

STOP_ERROR:
    RcFatalError( ERR_STOP_REQUESTED );
#if !defined( __WATCOMC__ )
    return( false );
#endif
} /* MergeResExeWINNE */


bool MergeResExeOS2NE( ExeFileInfo *src, ExeFileInfo *dst, ResFileInfo *res )
/***************************************************************************/
{
    RcStatus        ret;
    bool            error;
    int             err_code;

    ret = copyStubFile( src, dst, &err_code );
    if( ret != RS_OK )
        goto REPORT_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    ret = InitOS2ResTable( dst, res, &err_code );
    if( ret != RS_OK )
        goto REPORT_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    ret = AllocAndReadOS2SegTables( src, dst, res, &err_code );
    if( ret != RS_OK )
        goto REPORT_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    ret = seekPastResTable( src, dst, &err_code );
    if( ret != RS_OK )
        goto REPORT_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    ret = copyOtherTables( src, dst, &err_code );
    if( ret != RS_OK )
        goto REPORT_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    error = copyOS2Body( src, dst, res );
    if( error )
        goto HANDLE_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    ret = copyDebugInfo( src, dst );
    if( ret != RS_OK ) {
        err_code = errno;
        goto REPORT_ERROR;
    }
    if( StopInvoked )
        goto STOP_ERROR;

    ret = writeOS2HeadAndTables( src, dst, &err_code );
    if( ret != RS_OK )
        goto REPORT_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    return( true );

REPORT_ERROR:
    switch( ret ) {
    case RS_READ_ERROR:
        RcError( ERR_READING_EXE, src->name, strerror( err_code ) );
        break;
    case RS_READ_INCMPLT:
        RcError( ERR_UNEXPECTED_EOF, src->name );
        break;
    case RS_WRITE_ERROR:
        RcError( ERR_WRITTING_TMP, dst->name, strerror( err_code ) );
        break;
    case RS_NO_MEM:
        break;
    default:
        RcError( ERR_INTERNAL, INTERR_UNKNOWN_RCSTATUS );
    }
    /* fall through */
HANDLE_ERROR:
    return( false );

STOP_ERROR:
    RcFatalError( ERR_STOP_REQUESTED );
#if !defined( __WATCOMC__ )
    return( false );
#endif
} /* MergeResExeOS2NE */


static RcStatus updateDebugDirectory( ExeFileInfo *src, ExeFileInfo *dst )
{
    pe_va               src_rva;
    pe_va               dst_rva;
    unsigned_32         debug_size;
    long                src_offset;
    long                dst_offset;
    size_t              numread;
    unsigned            debug_cnt;
    unsigned            read_cnt;
    size_t              read_size;
    unsigned            i;
    debug_directory     *entry;
    pe_exe_header       *dst_pehdr;
    pe_exe_header       *src_pehdr;

    dst_pehdr = dst->u.PEInfo.WinHead;
    src_pehdr = src->u.PEInfo.WinHead;
    dst_rva = PE_DIRECTORY( *dst_pehdr, PE_TBL_DEBUG ).rva;
    debug_size = PE_DIRECTORY( *src_pehdr, PE_TBL_DEBUG ).size;
    src_rva = PE_DIRECTORY( *src_pehdr, PE_TBL_DEBUG ).rva;

    if( src_rva == 0 )
        return( RS_OK );
    src_offset = OffsetFromRVA( src, src_rva );
    dst_offset = OffsetFromRVA( dst, dst_rva );
    if( src_offset == 0xFFFFFFFF || dst_offset == 0xFFFFFFFF ) {
        return( RS_BAD_FILE_FMT );
    }
    if( RESSEEK( dst->fp, dst_offset, SEEK_SET ) )
        return( RS_WRITE_ERROR );
    if( RESSEEK( src->fp, src_offset, SEEK_SET ) )
        return( RS_READ_ERROR );
    for( debug_cnt = debug_size / sizeof( debug_directory ); debug_cnt > 0; debug_cnt -= read_cnt ) {
        read_cnt = IO_BUFFER_SIZE / sizeof( debug_directory);
        if( read_cnt > debug_cnt )
            read_cnt = debug_cnt;
        read_size = read_cnt * sizeof( debug_directory );
        numread = RESREAD( src->fp, Pass2Info.IoBuffer, read_size );
        if( numread != read_size )
            return( RESIOERR( src->fp, numread ) ? RS_READ_ERROR : RS_READ_INCMPLT );
        entry = Pass2Info.IoBuffer;
        for( i = 0; i < read_cnt; i++ ) {
            if( entry[i].data_seek >= src->DebugOffset ) {
                entry[i].data_seek += dst->DebugOffset - src->DebugOffset;
            }
        }
        if( RESWRITE( dst->fp, Pass2Info.IoBuffer, read_size ) != read_size ) {
            return( RS_WRITE_ERROR );
        }
    }
    return( RS_OK );
} /* updateDebugDirectory */


bool MergeResExePE( ExeFileInfo *src, ExeFileInfo *dst, ResFileInfo *resfiles )
/*****************************************************************************/
{
    RcStatus    ret;
    bool        error;
    int         err_code;

    ret = copyStubFile( src, dst, &err_code );
    if( ret != RS_OK )
        goto REPORT_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    error = CopyExeObjects( src, dst );
    if( error )
        goto HANDLE_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    error = RcBuildPEResourceObject( dst, resfiles );
    if( error )
        goto HANDLE_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    ret = copyDebugInfo( src, dst );
    if( ret != RS_OK ) {
        err_code = errno;
        goto REPORT_ERROR;
    }
    if( StopInvoked )
        goto STOP_ERROR;

    ret = writePEHeadAndObjTable( dst );
    if( ret != RS_OK ) {
        err_code = errno;
        goto REPORT_ERROR;
    }
    if( StopInvoked )
        goto STOP_ERROR;

    ret = updateDebugDirectory( src, dst );
    if( ret != RS_OK ) {
        err_code = errno;
        goto REPORT_ERROR;
    }
    if( StopInvoked )
        goto STOP_ERROR;

    return( true );

REPORT_ERROR:
    switch( ret ) {
    case RS_READ_ERROR:
        RcError( ERR_READING_EXE, src->name, strerror( err_code )  );
        break;
    case RS_READ_INCMPLT:
        RcError( ERR_UNEXPECTED_EOF, src->name );
        break;
    case RS_WRITE_ERROR:
        RcError( ERR_WRITTING_TMP, dst->name, strerror( err_code ) );
        break;
    case RS_BAD_FILE_FMT:
        RcError( ERR_NOT_VALID_EXE, src->name );
        break;
    case RS_NO_MEM:
        break;
    default:
        RcError( ERR_INTERNAL, INTERR_UNKNOWN_RCSTATUS );
    }
    /* fall through */
HANDLE_ERROR:
    return( false );

STOP_ERROR:
    RcFatalError( ERR_STOP_REQUESTED );
#if !defined( __WATCOMC__ )
    return( false );
#endif
} /* MergeResExePE */


static RcStatus writeLXHeadAndTables( ExeFileInfo *dst )
/*******************************************************
 * NB when an error occurs this function must return without altering errno
 */
{
    size_t          length;
    unsigned        i;

    if( RESSEEK( dst->fp, dst->WinHeadOffset, SEEK_SET ) )
        return( RS_WRITE_ERROR );
    /*
     * write LX header
     */
    if( RESWRITE( dst->fp, &dst->u.LXInfo.OS2Head, sizeof( os2_flat_header ) ) != sizeof( os2_flat_header ) )
        return( RS_WRITE_ERROR );
    /*
     * write object table
     */
    length = dst->u.LXInfo.OS2Head.num_objects * sizeof( object_record );
    if( RESWRITE( dst->fp, dst->u.LXInfo.Objects, length ) != length )
        return( RS_WRITE_ERROR );
    /*
     * write page table
     */
    length = dst->u.LXInfo.OS2Head.num_pages * sizeof( lx_map_entry );
    if( RESWRITE( dst->fp, dst->u.LXInfo.Pages, length ) != length )
        return( RS_WRITE_ERROR );
    /*
     * write resource table
     */
    for( i = 0; i < dst->u.LXInfo.OS2Head.num_rsrcs; ++i ) {
        if( RESWRITE( dst->fp, &dst->u.LXInfo.Res.resources[i].resource, sizeof( flat_res_table ) ) != sizeof( flat_res_table ) ) {
            return( RS_WRITE_ERROR );
        }
    }

    return( RS_OK );
} /* writeLXHeadAndTables */


static RcStatus copyLXNonresData( ExeFileInfo *src, ExeFileInfo *dst )
/*********************************************************************
 * copyLXNonresData
 * NB when an error occurs this function must return without altering errno
 */
{
    RcStatus            ret;

    dst->u.LXInfo.OS2Head.nonres_size = src->u.LXInfo.OS2Head.nonres_size;
    dst->u.LXInfo.OS2Head.nonres_cksum = src->u.LXInfo.OS2Head.nonres_cksum;

    if( src->u.LXInfo.OS2Head.nonres_size == 0 ) {
        dst->u.LXInfo.OS2Head.nonres_off = 0;
        return( RS_OK );
    }
    /*
     * DebugOffset is pointing to the current EOF
     */
    dst->u.LXInfo.OS2Head.nonres_off = dst->DebugOffset;

    if( RESSEEK( src->fp, src->u.LXInfo.OS2Head.nonres_off, SEEK_SET ) )
        return( RS_READ_ERROR );
    if( RESSEEK( dst->fp, dst->u.LXInfo.OS2Head.nonres_off, SEEK_SET ) )
        return( RS_WRITE_ERROR );

    ret = CopyExeData( src->fp, dst->fp, src->u.LXInfo.OS2Head.nonres_size );
    /*
     * Make DebugOffset point to dst EOF
     */
    CheckDebugOffset( dst );
    return( ret );
} /* copyLXNonresData */


static RcStatus copyLXDebugInfo( ExeFileInfo *src, ExeFileInfo *dst )
/********************************************************************
 * copyLXDebugInfo
 * NB when an error occurs this function must return without altering errno
 */
{
    if( src->u.LXInfo.OS2Head.debug_len == 0 ) {
        dst->u.LXInfo.OS2Head.debug_off = 0;
        dst->u.LXInfo.OS2Head.debug_len = 0;
        return( RS_OK );
    }
    dst->u.LXInfo.OS2Head.debug_len = src->u.LXInfo.OS2Head.debug_len;
    /*
     * DebugOffset is pointing to the current EOF
     */
    dst->u.LXInfo.OS2Head.debug_off = dst->DebugOffset;

    if( RESSEEK( src->fp, src->u.LXInfo.OS2Head.debug_off, SEEK_SET ) )
        return( RS_READ_ERROR );
    if( RESSEEK( dst->fp, dst->u.LXInfo.OS2Head.debug_off, SEEK_SET ) )
        return( RS_WRITE_ERROR );
    return( CopyExeDataTilEOF( src->fp, dst->fp ) );
} /* copyLXDebugInfo */


bool MergeResExeLX( ExeFileInfo *src, ExeFileInfo *dst, ResFileInfo *resfiles )
/*****************************************************************************/
{
    RcStatus    ret;
    bool        error;
    int         err_code;

    ret = copyStubFile( src, dst, &err_code );
    if( ret != RS_OK )
        goto REPORT_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    error = RcBuildLXResourceObjects( dst, resfiles );
    if( error )
        goto HANDLE_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    error = CopyLXExeObjects( src, dst );
    if( error )
        goto HANDLE_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    ret = RcWriteLXResourceObjects( dst, resfiles );
    if( ret != RS_OK ) {
        err_code = errno;
        goto REPORT_ERROR;
    }

    ret = copyLXNonresData( src, dst );
    if( ret != RS_OK ) {
        err_code = errno;
        goto REPORT_ERROR;
    }
    if( StopInvoked )
        goto STOP_ERROR;

    ret = copyLXDebugInfo( src, dst );
    if( ret != RS_OK ) {
        err_code = errno;
        goto REPORT_ERROR;
    }
    if( StopInvoked )
        goto STOP_ERROR;

    ret = writeLXHeadAndTables( dst );
    if( ret != RS_OK ) {
        err_code = errno;
        goto REPORT_ERROR;
    }
    if( StopInvoked )
        goto STOP_ERROR;

    return( true );

REPORT_ERROR:
    switch( ret ) {
    case RS_READ_ERROR:
        RcError( ERR_READING_EXE, src->name, strerror( err_code )  );
        break;
    case RS_READ_INCMPLT:
        RcError( ERR_UNEXPECTED_EOF, src->name );
        break;
    case RS_WRITE_ERROR:
        RcError( ERR_WRITTING_TMP, dst->name, strerror( err_code ) );
        break;
    case RS_BAD_FILE_FMT:
        RcError( ERR_NOT_VALID_EXE, src->name );
        break;
    case RS_NO_MEM:
        break;
    default:
        RcError( ERR_INTERNAL, INTERR_UNKNOWN_RCSTATUS );
    }
    /* fall through */
HANDLE_ERROR:
    return( false );

STOP_ERROR:
    RcFatalError( ERR_STOP_REQUESTED );
#if !defined( __WATCOMC__ )
    return( false );
#endif
} /* MergeResExeLX */
