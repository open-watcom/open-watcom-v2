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

/*
 * copyStubFile - copy from the begining of the file to the start of
 *                the win exe header
 */
static RcStatus copyStubFile( int *err_code )
{
    RcStatus    ret;

    ret = CopyExeData( Pass2Info.OldFile.fp, Pass2Info.TmpFile.fp, Pass2Info.OldFile.WinHeadOffset );
    *err_code = errno;
    return( ret );
} /* copyStubFile */

static RcStatus seekPastResTable( int *err_code )
{
    long            winheadoffset;
    long            seekamount;
    ExeFileInfo     *dst;
    uint_16         restab_size;

    dst = &(Pass2Info.TmpFile);

    if( Pass2Info.OldFile.u.NEInfo.WinHead.target == TARGET_OS2 ) {
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

static RcStatus copyOtherTables( int *err_code )
{
    uint_32         tablelen;
    os2_exe_header  *src_head;
    uint_32         src_offset;
    RcStatus        ret;

    src_head = &(Pass2Info.OldFile.u.NEInfo.WinHead);
    src_offset = Pass2Info.OldFile.WinHeadOffset;

    /* the other tables start at the resident names table and end at the end */
    /* of the non-resident names table */
    tablelen = (src_head->nonres_off + src_head->nonres_size) - ( src_head->resident_off + src_offset );

    if( RESSEEK( Pass2Info.OldFile.fp, src_head->resident_off + src_offset, SEEK_SET ) ) {
        *err_code = errno;
        return( RS_READ_ERROR );
    }

    ret = CopyExeData( Pass2Info.OldFile.fp, Pass2Info.TmpFile.fp, tablelen );
    *err_code = errno;
    return( ret );
} /* copyOtherTables */

static int computeShiftCount( ResFileInfo *res )
{
    NEExeInfo * src_ne;
    NEExeInfo * dst_ne;
    uint_32     filelen;
    int         num_segs;
    int         shift_count;

    src_ne = &Pass2Info.OldFile.u.NEInfo;
    dst_ne = &Pass2Info.TmpFile.u.NEInfo;

    filelen = src_ne->WinHead.nonres_off +
            src_ne->WinHead.nonres_size +
            dst_ne->Res.Dir.TableSize +
            dst_ne->Res.Str.StringBlockSize;
    filelen += ComputeSegmentSize( Pass2Info.OldFile.fp, &(dst_ne->Seg), src_ne->WinHead.align );
    if( ! CmdLineParms.NoResFile ) {
        filelen += ComputeWINResourceSize( res->Dir );
    }

    num_segs = src_ne->WinHead.segments;
    if( ! CmdLineParms.NoResFile ) {
        num_segs += WResGetNumResources( res->Dir );
    }

    shift_count = FindShiftCount( filelen, num_segs );
    if( shift_count == 0 ) {
        shift_count = 1;        /* MS thinks 0 == 9 for shift count */
    }

    if( shift_count > src_ne->WinHead.align ) {
        return( shift_count );
    } else {
        return( src_ne->WinHead.align );
    }

} /* computeShiftCount */

static void checkShiftCount( ResFileInfo *res )
{
    Pass2Info.TmpFile.u.NEInfo.WinHead.align = computeShiftCount( res );
    Pass2Info.TmpFile.u.NEInfo.Res.Dir.ResShiftCount =
                                Pass2Info.TmpFile.u.NEInfo.WinHead.align;
} /* checkShiftCount */

static bool copyWINBody( ResFileInfo *res )
{
    NEExeInfo *         dst_ne;
    uint_16             sect2mask = 0;
    uint_16             sect2bits = 0;
    uint_16             shift_count;
    long                gangloadstart;
    long                gangloadlen;
    CpSegRc             copy_segs_ret;
    bool                use_gangload = false;

    dst_ne = &Pass2Info.TmpFile.u.NEInfo;

    switch( CmdLineParms.SegmentSorting ) {
    case SEG_SORT_NONE:     /* all segments in section 2 */
        sect2mask = 0;
        sect2bits = 0;
        use_gangload = false;
        Pass2Info.TmpFile.u.NEInfo.WinHead.align = Pass2Info.OldFile.u.NEInfo.WinHead.align;
        dst_ne->Res.Dir.ResShiftCount = computeShiftCount( res );
        break;
    case SEG_SORT_PRELOAD_ONLY:  /* all load on call segments in section 2 */
        sect2mask = SEG_PRELOAD;
        sect2bits = 0;
        use_gangload = true;
        checkShiftCount( res );
        break;
    case SEG_SORT_MANY:     /* only load on call, discardable, code segments in section 2 */
        sect2mask = SEG_DATA | SEG_PRELOAD | SEG_DISCARD;
        sect2bits = SEG_DISCARD;

        /* set the entry segment to be preload */
        {
            segment_record *    seg;        /* these two are here because a */
            uint_16             entry_seg;  /* 71 character field reference is hard to read */
            seg = Pass2Info.OldFile.u.NEInfo.Seg.Segments;
            entry_seg = Pass2Info.OldFile.u.NEInfo.WinHead.entrynum;
            seg[entry_seg].info |= SEG_PRELOAD;
        }

        use_gangload = true;
        checkShiftCount( res );
        break;
    default:
        break;
    }

    /* third arg to Copy???? is false --> copy section one */
    gangloadstart = RESTELL( Pass2Info.TmpFile.fp );
    gangloadstart += AlignAmount( gangloadstart, dst_ne->Res.Dir.ResShiftCount );
    copy_segs_ret = CopyWINSegments( sect2mask, sect2bits, false );
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
        if( CopyWINResources( res, sect2mask, sect2bits, false ) != RS_OK ) {
            return( true );
        }
    }
    gangloadlen = RESTELL( Pass2Info.TmpFile.fp ) - gangloadstart;

    /* third arg to Copy???? is true  --> copy section two */
    copy_segs_ret = CopyWINSegments( sect2mask, sect2bits, true );
    if( copy_segs_ret == CPSEG_ERROR ) {
        return( true );
    }
    if( !CmdLineParms.NoResFile ) {
        if( CopyWINResources( res, sect2mask, sect2bits, true ) != RS_OK ) {
            return( true );
        }
    }

    if( use_gangload ) {
        shift_count = dst_ne->WinHead.align;
        dst_ne->WinHead.gangstart = gangloadstart >> shift_count;
        dst_ne->WinHead.ganglength = gangloadlen >> shift_count;
        dst_ne->WinHead.otherflags |= WIN_GANGLOAD_PRESENT;
    } else {
        dst_ne->WinHead.gangstart = 0;
        dst_ne->WinHead.ganglength = 0;
        dst_ne->WinHead.otherflags &= ~WIN_GANGLOAD_PRESENT;
    }

    return( false );

} /* copyBody */

static bool copyOS2Body( ResFileInfo *res )
{
    NEExeInfo           *dst_ne;
    CpSegRc             copy_segs_ret;
    unsigned_16         align;

    dst_ne = &Pass2Info.TmpFile.u.NEInfo;

    /* OS/2 does not use separate alignment for resources */
    align = Pass2Info.OldFile.u.NEInfo.WinHead.align;
    Pass2Info.TmpFile.u.NEInfo.WinHead.align = align;
    dst_ne->Res.Dir.ResShiftCount = align;

    copy_segs_ret = CopyOS2Segments();
    if( copy_segs_ret == CPSEG_ERROR ) {
        return( true );
    }
    if( !CmdLineParms.NoResFile ) {
        if( CopyOS2Resources( res ) != RS_OK ) {
            return( true );
        }
    }

    return( false );
} /* copyOS2Body */

/*
 * copyDebugInfo
 * NB when an error occurs this function must return without altering errno
 */
static RcStatus copyDebugInfo( void )
{
    ExeFileInfo *src;
    ExeFileInfo *dst;

    src = &(Pass2Info.OldFile);
    dst = &(Pass2Info.TmpFile);

    if( RESSEEK( src->fp, src->DebugOffset, SEEK_SET ) )
        return( RS_READ_ERROR );
    if( RESSEEK( dst->fp, dst->DebugOffset, SEEK_SET ) )
        return( RS_WRITE_ERROR );
    return( CopyExeDataTilEOF( src->fp, dst->fp ) );

} /* copyDebugInfo */

static RcStatus writeHeadAndTables( int *err_code )
{
    ExeFileInfo *   src;
    ExeFileInfo *   dst;
    NEExeInfo *     src_ne;
    NEExeInfo *     dst_ne;
    uint_16         tableshift;     /* amount the tables are shifted in the tmp file */
    uint_16         info;           /* os2_exe_header.info */
    RcStatus        ret;

    src = &(Pass2Info.OldFile);
    src_ne = &src->u.NEInfo;
    dst = &(Pass2Info.TmpFile);
    dst_ne = &dst->u.NEInfo;

    /* set the info flag for the dst executable from the one flag and */
    /* the command line options given */
    info = src_ne->WinHead.info;
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

    /* copy the fields in the os2_exe_header then change some of them */
    dst->WinHeadOffset = src->WinHeadOffset;
    /* copy the WinHead fields up to, but excluding, the segment_off field */
    memcpy( &(dst_ne->WinHead), &(src_ne->WinHead),
            offsetof(os2_exe_header, segment_off) );
    dst_ne->WinHead.info = info;
    tableshift = dst_ne->Res.Dir.TableSize +
                dst_ne->Res.Str.StringBlockSize -
                (src_ne->WinHead.resident_off - src_ne->WinHead.resource_off );
    dst_ne->WinHead.entry_off = src_ne->WinHead.entry_off + tableshift;
    dst_ne->WinHead.resident_off = src_ne->WinHead.resident_off + tableshift;
    dst_ne->WinHead.module_off = src_ne->WinHead.module_off + tableshift;
    dst_ne->WinHead.import_off = src_ne->WinHead.import_off + tableshift;
    dst_ne->WinHead.nonres_off = src_ne->WinHead.nonres_off + tableshift;
    dst_ne->WinHead.segment_off = sizeof( os2_exe_header );
    dst_ne->WinHead.resource_off = dst_ne->WinHead.segment_off +
                                dst_ne->Seg.NumSegs * sizeof( segment_record );
    dst_ne->WinHead.movable = src_ne->WinHead.movable;
    dst_ne->WinHead.resource = dst_ne->Res.Dir.NumResources;
    dst_ne->WinHead.target = src_ne->WinHead.target;
    /* |= the next one since the WIN_GANGLOAD_PRESENT flag may be set */
    dst_ne->WinHead.otherflags |= src_ne->WinHead.otherflags;
    dst_ne->WinHead.swaparea =   0;      /* What is this field for? */
    if( CmdLineParms.VersionStamp30 ) {
        dst_ne->WinHead.expver = VERSION_30_STAMP;
    } else {
        dst_ne->WinHead.expver = VERSION_31_STAMP;
    }

    /* seek to the start of the os2_exe_header in dst */
    if( RESSEEK( dst->fp, dst->WinHeadOffset, SEEK_SET ) ) {
        *err_code = errno;
        return( RS_WRITE_ERROR );
    }

    /* write the header */
    if( RESWRITE( dst->fp, &(dst_ne->WinHead), sizeof( os2_exe_header ) ) != sizeof( os2_exe_header ) ) {
        *err_code = errno;
        return( RS_WRITE_ERROR );
    }

    /* write the segment table */
    if( dst_ne->Seg.NumSegs > 0 ) {
        size_t  numwrite;

        numwrite = dst_ne->Seg.NumSegs * sizeof( segment_record );
        if( RESWRITE( dst->fp, dst_ne->Seg.Segments, numwrite ) != numwrite ) {
            *err_code = errno;
            return( RS_WRITE_ERROR );
        }
    }

    /* write the resource table */
    ret = WriteWINResTable( dst->fp, &(dst_ne->Res), err_code );
    return( ret );

} /* writeHeadAndTables */

/*
 * Processing OS/2 NE modules is very very similar to Windows NE processing
 * but there are enough differences in detail to warrant separate
 * implementation to keep the two cleaner.
 */
static RcStatus writeOS2HeadAndTables( int *err_code )
{
    ExeFileInfo *   src;
    ExeFileInfo *   dst;
    NEExeInfo *     src_ne;
    NEExeInfo *     dst_ne;
    uint_16         tableshift;     /* amount the tables are shifted in the tmp file */
    RcStatus        ret;

    src = &(Pass2Info.OldFile);
    src_ne = &src->u.NEInfo;
    dst = &(Pass2Info.TmpFile);
    dst_ne = &dst->u.NEInfo;

    /* copy the fields in the os2_exe_header then change some of them */
    dst->WinHeadOffset = src->WinHeadOffset;
    /* copy the WinHead fields up to, but excluding, the segment_off field */
    memcpy( &(dst_ne->WinHead), &(src_ne->WinHead),
            offsetof(os2_exe_header, segment_off) );
    dst_ne->WinHead.info = src_ne->WinHead.info;
    dst_ne->WinHead.segment_off = sizeof( os2_exe_header );
    dst_ne->WinHead.resource_off = dst_ne->WinHead.segment_off +
                                dst_ne->Seg.NumSegs * sizeof( segment_record );
    tableshift = dst_ne->OS2Res.table_size -
                (src_ne->WinHead.resident_off - src_ne->WinHead.resource_off) +
                (dst_ne->WinHead.resource_off - src_ne->WinHead.resource_off);
    dst_ne->WinHead.entry_off = src_ne->WinHead.entry_off + tableshift;
    dst_ne->WinHead.resident_off = src_ne->WinHead.resident_off + tableshift;
    dst_ne->WinHead.module_off = src_ne->WinHead.module_off + tableshift;
    dst_ne->WinHead.import_off = src_ne->WinHead.import_off + tableshift;
    dst_ne->WinHead.nonres_off = src_ne->WinHead.nonres_off + tableshift;
    dst_ne->WinHead.movable    = src_ne->WinHead.movable;
    dst_ne->WinHead.resource   = dst_ne->OS2Res.num_res_segs;
    dst_ne->WinHead.target     = src_ne->WinHead.target;
    dst_ne->WinHead.otherflags = src_ne->WinHead.otherflags;
    dst_ne->WinHead.segments   = dst_ne->Seg.NumSegs;

    /* seek to the start of the os2_exe_header in dst */
    if( RESSEEK( dst->fp, dst->WinHeadOffset, SEEK_SET ) ) {
        *err_code = errno;
        return( RS_WRITE_ERROR );
    }

    /* write the header */
    if( RESWRITE( dst->fp, &(dst_ne->WinHead), sizeof( os2_exe_header ) ) != sizeof( os2_exe_header ) ) {
        *err_code = errno;
        return( RS_WRITE_ERROR );
    }

    /* write the segment table */
    if( dst_ne->Seg.NumSegs > 0 ) {
        size_t  numwrite;

        numwrite = dst_ne->Seg.NumSegs * sizeof( segment_record );
        if( RESWRITE( dst->fp, dst_ne->Seg.Segments, numwrite ) != numwrite ) {
            *err_code = errno;
            return( RS_WRITE_ERROR );
        }
    }

    /* write the resource table */
    ret = WriteOS2ResTable( dst->fp, &(dst_ne->OS2Res), err_code );
    return( ret );

} /* writeOS2HeadAndTables */

static RcStatus findEndOfResources( int *err_code )
/* if this exe already contains resources find the end of them so we don't
   copy them with debug information.  Otherwise the file will grow whenever
   a resource file is added */
{
    NEExeInfo                   *src_ne;
    uint_32                     *src_debugoffset;
    FILE                        *src_fp;
    size_t                      numread;
    unsigned                    i;
    long                        src_offset;
    uint_16                     alignshift;
    uint_32                     end;
    uint_32                     tmp;
    resource_type_record        typeinfo;
    resource_record             nameinfo;

    end = 0;
    src_offset = Pass2Info.OldFile.WinHeadOffset;
    src_ne = &Pass2Info.OldFile.u.NEInfo;
    src_fp = Pass2Info.OldFile.fp;
    src_debugoffset = &Pass2Info.OldFile.DebugOffset;

    if( src_ne->WinHead.resource_off == src_ne->WinHead.resident_off ) {
        return( RS_OK );
    }

    if( RESSEEK( src_fp, src_ne->WinHead.resource_off + src_offset, SEEK_SET ) ) {
        *err_code = errno;
        return( RS_READ_ERROR );
    }

    numread = RESREAD( src_fp, &alignshift, sizeof( alignshift ) );
    if( numread != sizeof( alignshift ) ) {
        *err_code = errno;
        return( RESIOERR( src_fp, numread ) ? RS_READ_ERROR : RS_READ_INCMPLT );
    }
    alignshift = 1 << alignshift;

    numread = RESREAD( src_fp, &typeinfo, sizeof( typeinfo ) );
    if( numread != sizeof( typeinfo ) )  {
        *err_code = errno;
        return( RESIOERR( src_fp, numread ) ? RS_READ_ERROR : RS_READ_INCMPLT );
    }
    while( typeinfo.type != 0 ) {
        for( i = typeinfo.num_resources; i > 0 ; --i ) {
            numread = RESREAD( src_fp, &nameinfo, sizeof( nameinfo ) );
            if( numread != sizeof( nameinfo ) ) {
                *err_code = errno;
                return( RESIOERR( src_fp, numread ) ? RS_READ_ERROR : RS_READ_INCMPLT );
            }
            tmp = nameinfo.offset + nameinfo.length;
            if( tmp > end ) {
                end = tmp;
            }
        }
        numread = RESREAD( src_fp, &typeinfo, sizeof( typeinfo ) );
        if( numread != sizeof( typeinfo ) ) {
            *err_code = errno;
            return( RESIOERR( src_fp, numread ) ? RS_READ_ERROR : RS_READ_INCMPLT );
        }
    }
    end *= alignshift;
    if( end > *src_debugoffset ) {
        *src_debugoffset = end;
    }
    return( RS_OK );
}

/*
 * writePEHeadAndObjTable
 * NB when an error occurs this function must return without altering errno
 */
static RcStatus writePEHeadAndObjTable( void )
{
    ExeFileInfo     *dst;
    pe_object       *last_object;
    int             i;
    uint_32         image_size;
    int             num_objects;
    unsigned_32     object_align;
    pe_exe_header   *pehdr;

    dst = &Pass2Info.TmpFile;

    /* adjust the image size in the header */
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

/*
 * Windows NE files store resources in a special data structure. OS/2 NE
 * modules are quite different and store each resource in its own data
 * segment(s). The OS/2 resource table is completely different as well and
 * only contains resource types/IDs.
 */
bool MergeResExeWINNE( ResFileInfo *res )
{
    RcStatus        ret;
    bool            error;
    int             err_code;

    ret = copyStubFile( &err_code );
    if( ret != RS_OK )
        goto REPORT_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    ret = AllocAndReadWINSegTables( &err_code );
    if( ret != RS_OK )
        goto REPORT_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    InitWINResTable( res );

    ret = seekPastResTable( &err_code );
    if( ret != RS_OK )
        goto REPORT_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    ret = findEndOfResources( &err_code );
    if( ret != RS_OK )
        goto REPORT_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    ret = copyOtherTables( &err_code );
    if( ret != RS_OK )
        goto REPORT_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    error = copyWINBody( res );
    if( error )
        goto HANDLE_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    ret = copyDebugInfo();
    if( ret != RS_OK ) {
        err_code = errno;
        goto REPORT_ERROR;
    }
    if( StopInvoked )
        goto STOP_ERROR;

    ret = writeHeadAndTables( &err_code );
    if( ret != RS_OK )
        goto REPORT_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    return( true );

REPORT_ERROR:
    switch( ret ) {
    case RS_READ_ERROR:
        RcError( ERR_READING_EXE, Pass2Info.OldFile.name, strerror( err_code ) );
        break;
    case RS_READ_INCMPLT:
        RcError( ERR_UNEXPECTED_EOF, Pass2Info.OldFile.name );
        break;
    case RS_WRITE_ERROR:
        RcError( ERR_WRITTING_TMP, Pass2Info.TmpFile.name, strerror( err_code ) );
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


bool MergeResExeOS2NE( ResFileInfo *res )
{
    RcStatus        ret;
    bool            error;
    int             err_code;

    ret = copyStubFile( &err_code );
    if( ret != RS_OK )
        goto REPORT_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    ret = InitOS2ResTable( res, &err_code );
    if( ret != RS_OK )
        goto REPORT_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    ret = AllocAndReadOS2SegTables( res, &err_code );
    if( ret != RS_OK )
        goto REPORT_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    ret = seekPastResTable( &err_code );
    if( ret != RS_OK )
        goto REPORT_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    ret = copyOtherTables( &err_code );
    if( ret != RS_OK )
        goto REPORT_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    error = copyOS2Body( res );
    if( error )
        goto HANDLE_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    ret = copyDebugInfo();
    if( ret != RS_OK ) {
        err_code = errno;
        goto REPORT_ERROR;
    }
    if( StopInvoked )
        goto STOP_ERROR;

    ret = writeOS2HeadAndTables( &err_code );
    if( ret != RS_OK )
        goto REPORT_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    return( true );

REPORT_ERROR:
    switch( ret ) {
    case RS_READ_ERROR:
        RcError( ERR_READING_EXE, Pass2Info.OldFile.name, strerror( err_code ) );
        break;
    case RS_READ_INCMPLT:
        RcError( ERR_UNEXPECTED_EOF, Pass2Info.OldFile.name );
        break;
    case RS_WRITE_ERROR:
        RcError( ERR_WRITTING_TMP, Pass2Info.TmpFile.name, strerror( err_code ) );
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


static RcStatus updateDebugDirectory( void )
{
    ExeFileInfo         *dst;
    ExeFileInfo         *src;
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

    dst = &Pass2Info.TmpFile;
    src = &Pass2Info.OldFile;
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


bool MergeResExePE( ResFileInfo *res )
{
    RcStatus    ret;
    bool        error;
    int         err_code;

    ret = copyStubFile( &err_code );
    if( ret != RS_OK )
        goto REPORT_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    error = CopyExeObjects();
    if( error )
        goto HANDLE_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    error = RcBuildPEResourceObject( res );
    if( error )
        goto HANDLE_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    ret = copyDebugInfo();
    if( ret != RS_OK ) {
        err_code = errno;
        goto REPORT_ERROR;
    }
    if( StopInvoked )
        goto STOP_ERROR;

    ret = writePEHeadAndObjTable();
    if( ret != RS_OK ) {
        err_code = errno;
        goto REPORT_ERROR;
    }
    if( StopInvoked )
        goto STOP_ERROR;

    ret = updateDebugDirectory();
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
        RcError( ERR_READING_EXE, Pass2Info.OldFile.name, strerror( err_code )  );
        break;
    case RS_READ_INCMPLT:
        RcError( ERR_UNEXPECTED_EOF, Pass2Info.OldFile.name );
        break;
    case RS_WRITE_ERROR:
        RcError( ERR_WRITTING_TMP, Pass2Info.TmpFile.name, strerror( err_code ) );
        break;
    case RS_BAD_FILE_FMT:
        RcError( ERR_NOT_VALID_EXE, Pass2Info.OldFile.name );
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


/*
 * writeLXHeadAndTables
 * NB when an error occurs this function must return without altering errno
 */
static RcStatus writeLXHeadAndTables( void )
{
    ExeFileInfo     *dst;
    LXExeInfo       *dst_lx;
    size_t          length;
    long            offset;
    unsigned        i;

    dst = &Pass2Info.TmpFile;
    dst_lx = &dst->u.LXInfo;

    offset = sizeof( os2_flat_header );
    if( RESSEEK( dst->fp, dst->WinHeadOffset + offset, SEEK_SET ) )
        return( RS_WRITE_ERROR );

    // write object table
    length = dst_lx->OS2Head.num_objects * sizeof( object_record );
    if( RESWRITE( dst->fp, dst_lx->Objects, length ) != length )
        return( RS_WRITE_ERROR );

    // write page table
    offset += length;
    length = dst_lx->OS2Head.num_pages * sizeof( lx_map_entry );
    if( RESWRITE( dst->fp, dst_lx->Pages, length ) != length )
        return( RS_WRITE_ERROR );

    // write resource table
    offset += length;
    for( i = 0; i < dst_lx->OS2Head.num_rsrcs; ++i ) {
        if( RESWRITE( dst->fp, &dst_lx->Res.resources[i].resource, sizeof( flat_res_table ) ) != sizeof( flat_res_table ) ) {
            return( RS_WRITE_ERROR );
        }
    }

    // finally write LX header
    if( RESSEEK( dst->fp, dst->WinHeadOffset, SEEK_SET ) )
        return( RS_WRITE_ERROR );

    if( RESWRITE( dst->fp, &dst_lx->OS2Head, sizeof( os2_flat_header ) ) != sizeof( os2_flat_header ) )
        return( RS_WRITE_ERROR );

    return( RS_OK );
} /* writeLXHeadAndTables */


/*
 * copyLXNonresData
 * NB when an error occurs this function must return without altering errno
 */
static RcStatus copyLXNonresData( void )
{
    ExeFileInfo         *src;
    ExeFileInfo         *dst;
    os2_flat_header     *src_head;
    os2_flat_header     *dst_head;
    RcStatus            ret;

    src = &(Pass2Info.OldFile);
    dst = &(Pass2Info.TmpFile);
    src_head = &src->u.LXInfo.OS2Head;
    dst_head = &dst->u.LXInfo.OS2Head;

    dst_head->nonres_size = src_head->nonres_size;
    dst_head->nonres_cksum = src_head->nonres_cksum;

    if( src_head->nonres_size == 0 ) {
        dst_head->nonres_off = 0;
        return( RS_OK );
    }

    // DebugOffset is pointing to the current EOF
    dst_head->nonres_off = dst->DebugOffset;

    if( RESSEEK( src->fp, src_head->nonres_off, SEEK_SET ) )
        return( RS_READ_ERROR );
    if( RESSEEK( dst->fp, dst->DebugOffset, SEEK_SET ) )
        return( RS_WRITE_ERROR );

    ret = CopyExeData( Pass2Info.OldFile.fp, Pass2Info.TmpFile.fp, src_head->nonres_size );

    // Make DebugOffset point to dst EOF
    CheckDebugOffset( dst );
    return( ret );
} /* copyLXNonresData */


/*
 * copyLXDebugInfo
 * NB when an error occurs this function must return without altering errno
 */
static RcStatus copyLXDebugInfo( void )
{
    ExeFileInfo         *src;
    ExeFileInfo         *dst;
    os2_flat_header     *src_head;
    os2_flat_header     *dst_head;

    src = &(Pass2Info.OldFile);
    dst = &(Pass2Info.TmpFile);
    src_head = &src->u.LXInfo.OS2Head;
    dst_head = &dst->u.LXInfo.OS2Head;

    if( src_head->debug_len == 0 ) {
        dst_head->debug_off = 0;
        dst_head->debug_len = 0;
        return( RS_OK );
    }
    dst_head->debug_off = dst->DebugOffset;
    dst_head->debug_len = src_head->debug_len;

    if( RESSEEK( src->fp, src_head->debug_off, SEEK_SET ) )
        return( RS_READ_ERROR );
    if( RESSEEK( dst->fp, dst->DebugOffset, SEEK_SET ) )
        return( RS_WRITE_ERROR );
    return( CopyExeDataTilEOF( src->fp, dst->fp ) );
} /* copyLXDebugInfo */


bool MergeResExeLX( ResFileInfo *res )
{
    RcStatus    ret;
    bool        error;
    int         err_code;

    ret = copyStubFile( &err_code );
    if( ret != RS_OK )
        goto REPORT_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    error = RcBuildLXResourceObjects( res );
    if( error )
        goto HANDLE_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    error = CopyLXExeObjects( &Pass2Info.OldFile, &Pass2Info.TmpFile );
    if( error )
        goto HANDLE_ERROR;
    if( StopInvoked )
        goto STOP_ERROR;

    ret = RcWriteLXResourceObjects( res );
    if( ret != RS_OK ) {
        err_code = errno;
        goto REPORT_ERROR;
    }

    ret = copyLXNonresData();
    if( ret != RS_OK ) {
        err_code = errno;
        goto REPORT_ERROR;
    }
    if( StopInvoked )
        goto STOP_ERROR;

    ret = copyLXDebugInfo();
    if( ret != RS_OK ) {
        err_code = errno;
        goto REPORT_ERROR;
    }
    if( StopInvoked )
        goto STOP_ERROR;

    ret = writeLXHeadAndTables();
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
        RcError( ERR_READING_EXE, Pass2Info.OldFile.name, strerror( err_code )  );
        break;
    case RS_READ_INCMPLT:
        RcError( ERR_UNEXPECTED_EOF, Pass2Info.OldFile.name );
        break;
    case RS_WRITE_ERROR:
        RcError( ERR_WRITTING_TMP, Pass2Info.TmpFile.name, strerror( err_code ) );
        break;
    case RS_BAD_FILE_FMT:
        RcError( ERR_NOT_VALID_EXE, Pass2Info.OldFile.name );
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
