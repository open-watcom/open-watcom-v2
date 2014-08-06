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
* Description:  Resource Compiler pass 2.
*
****************************************************************************/


#include "wio.h"
#include "global.h"
#include "errors.h"
#include "rcstrblk.h"
#include "rcrtns.h"
#include "clibext.h"
#include "rccore.h"
#include "exeutil.h"
#include "exeseg.h"
#include "exeres.h"
#include "exeobj.h"
#include "exerespe.h"
#include "exelxobj.h"
#include "exereslx.h"

/*
 * copyStubFile - copy from the begining of the file to the start of
 *                the win exe header
 */
static int copyStubFile( int *err_code )
{
    RcStatus    error;

    error = CopyExeData( Pass2Info.OldFile.Handle, Pass2Info.TmpFile.Handle, Pass2Info.OldFile.WinHeadOffset );
    *err_code = errno;
    return( error );
} /* copyStubFile */

static RcStatus seekPastResTable( int *err_code )
{
    long            winheadoffset;
    long            seekamount;
    ExeFileInfo     *tmpexe;
    uint_16         res_tbl_size;

    tmpexe = &(Pass2Info.TmpFile);

    if( Pass2Info.OldFile.u.NEInfo.WinHead.target == TARGET_OS2 )
        res_tbl_size = tmpexe->u.NEInfo.OS2Res.table_size;
    else
        res_tbl_size = tmpexe->u.NEInfo.Res.Dir.TableSize;

    seekamount = sizeof( os2_exe_header ) +
                    tmpexe->u.NEInfo.Seg.NumSegs * sizeof( segment_record ) +
                    res_tbl_size +
                    tmpexe->u.NEInfo.Res.Str.StringBlockSize;
    if( RCSEEK( tmpexe->Handle, seekamount, SEEK_CUR ) == -1 ) {
        *err_code = errno;
        return( RS_READ_ERROR );
    }
    winheadoffset = RCTELL( tmpexe->Handle );
    tmpexe->WinHeadOffset = winheadoffset;
    return( RS_OK );

} /* seekPastResTable */

static int copyOtherTables( int *err_code )
{
    uint_32         tablelen;
    os2_exe_header  *oldhead;
    uint_32         oldoffset;
    WResFileID      oldhandle;
    RcStatus        ret;

    oldhead = &(Pass2Info.OldFile.u.NEInfo.WinHead);
    oldoffset = Pass2Info.OldFile.WinHeadOffset;
    oldhandle = Pass2Info.OldFile.Handle;

    /* the other tables start at the resident names table and end at the end */
    /* of the non-resident names table */
    tablelen = (oldhead->nonres_off + oldhead->nonres_size) - ( oldhead->resident_off + oldoffset );

    if( RCSEEK( oldhandle, oldhead->resident_off + oldoffset, SEEK_SET ) == -1 ) {
        *err_code = errno;
        return( RS_READ_ERROR );
    }

    ret = CopyExeData( Pass2Info.OldFile.Handle, Pass2Info.TmpFile.Handle, tablelen );
    *err_code = errno;
    return( ret );
} /* copyOtherTables */

static int computeShiftCount( void )
{
    NEExeInfo * old;
    NEExeInfo * tmp;
    uint_32     filelen;
    int         num_segs;
    int         shift_count;

    old = &Pass2Info.OldFile.u.NEInfo;
    tmp = &Pass2Info.TmpFile.u.NEInfo;

    filelen = old->WinHead.nonres_off +
            old->WinHead.nonres_size +
            tmp->Res.Dir.TableSize +
            tmp->Res.Str.StringBlockSize;
    filelen += ComputeSegmentSize( Pass2Info.OldFile.Handle,
                    &(tmp->Seg), old->WinHead.align );
    if( ! CmdLineParms.NoResFile ) {
        filelen += ComputeWINResourceSize( Pass2Info.ResFiles->Dir );
    }

    num_segs = old->WinHead.segments;
    if( ! CmdLineParms.NoResFile ) {
        num_segs += WResGetNumResources( Pass2Info.ResFiles->Dir );
    }

    shift_count = FindShiftCount( filelen, num_segs );
    if( shift_count == 0 ) {
        shift_count = 1;        /* MS thinks 0 == 9 for shift count */
    }

    if( shift_count > old->WinHead.align ) {
        return( shift_count );
    } else {
        return( old->WinHead.align );
    }

} /* computeShiftCount */

static void checkShiftCount( void )
{
    Pass2Info.TmpFile.u.NEInfo.WinHead.align = computeShiftCount();
    Pass2Info.TmpFile.u.NEInfo.Res.Dir.ResShiftCount =
                                Pass2Info.TmpFile.u.NEInfo.WinHead.align;
} /* checkShiftCount */

static int copyBody( void )
{
    NEExeInfo *         tmp;
    uint_16             sect2mask = 0;
    uint_16             sect2bits = 0;
    uint_16             shift_count;
    long                gangloadstart;
    long                gangloadlen;
    CpSegRc             copy_segs_ret;
    bool                use_gangload = FALSE;
    int                 error;

    tmp = &Pass2Info.TmpFile.u.NEInfo;

    switch (CmdLineParms.SegmentSorting) {
    case SEG_SORT_NONE:     /* all segments in section 2 */
        sect2mask = 0;
        sect2bits = 0;
        use_gangload = FALSE;
        Pass2Info.TmpFile.u.NEInfo.WinHead.align = Pass2Info.OldFile.u.NEInfo.WinHead.align;
        tmp->Res.Dir.ResShiftCount = computeShiftCount();
        break;
    case SEG_SORT_PRELOAD_ONLY:  /* all load on call segments in section 2 */
        sect2mask = SEG_PRELOAD;
        sect2bits = 0;
        use_gangload = TRUE;
        checkShiftCount();
        break;
    case SEG_SORT_MANY:     /* only load on call, discardable, code segments */
                            /* in section 2 */
        sect2mask = SEG_DATA | SEG_PRELOAD | SEG_DISCARD;
        sect2bits = SEG_DISCARD;

        /* set the entry segment to be preload */
        {
            segment_record *    seg;        /* these two are here because a */
            uint_16             entry_seg;  /* 71 character field reference */
                                            /* is hard to read */
            seg = Pass2Info.OldFile.u.NEInfo.Seg.Segments;
            entry_seg = Pass2Info.OldFile.u.NEInfo.WinHead.entrynum;
            seg[ entry_seg ].info |= SEG_PRELOAD;
        }

        use_gangload = TRUE;
        checkShiftCount();
        break;
    default:
        break;
    }

    /* third arg to Copy???? is FALSE --> copy section one */
    gangloadstart = RCTELL( Pass2Info.TmpFile.Handle );
    gangloadstart += AlignAmount( gangloadstart, tmp->Res.Dir.ResShiftCount );
    copy_segs_ret = CopyWINSegments( sect2mask, sect2bits, FALSE );
    switch (copy_segs_ret) {
    case CPSEG_SEG_TOO_BIG:
        if( use_gangload ) {
            RcWarning( ERR_NO_GANGLOAD );
            use_gangload = FALSE;
        }
        break;
    case CPSEG_ERROR:
        return( TRUE );
    case CPSEG_OK:
    default:
        break;
    }
    if( ! CmdLineParms.NoResFile ) {
        error = CopyWINResources( sect2mask, sect2bits, FALSE );
        if( error) return( TRUE  );
    }
    gangloadlen = RCTELL( Pass2Info.TmpFile.Handle ) - gangloadstart;

    /* third arg to Copy???? is TRUE  --> copy section two */
    copy_segs_ret = CopyWINSegments( sect2mask, sect2bits, TRUE );
    if( copy_segs_ret == CPSEG_ERROR) {
        return( TRUE  );
    }
    if( !CmdLineParms.NoResFile ) {
        error = CopyWINResources( sect2mask, sect2bits, TRUE );
        if( error) return( TRUE  );
    }

    if( use_gangload ) {
        shift_count = tmp->WinHead.align;
        tmp->WinHead.gangstart = gangloadstart >> shift_count;
        tmp->WinHead.ganglength = gangloadlen >> shift_count;
        tmp->WinHead.otherflags |= WIN_GANGLOAD_PRESENT;
    } else {
        tmp->WinHead.gangstart = 0;
        tmp->WinHead.ganglength = 0;
        tmp->WinHead.otherflags &= ~WIN_GANGLOAD_PRESENT;
    }

    return( FALSE );

} /* copyBody */

static int copyOS2Body( void )
{
    NEExeInfo           *tmp;
    CpSegRc             copy_segs_ret;
    int                 error;
    unsigned_16         align;

    tmp = &Pass2Info.TmpFile.u.NEInfo;

    /* OS/2 does not use separate alignment for resources */
    align = Pass2Info.OldFile.u.NEInfo.WinHead.align;
    Pass2Info.TmpFile.u.NEInfo.WinHead.align = align;
    tmp->Res.Dir.ResShiftCount = align;

    copy_segs_ret = CopyOS2Segments();
    if( copy_segs_ret == CPSEG_ERROR ) {
        return( TRUE );
    }
    if( !CmdLineParms.NoResFile ) {
        error = CopyOS2Resources();
        if( error )
            return( TRUE );
    }

    return( FALSE );
} /* copyOS2Body */

/*
 * copyDebugInfo
 * NB when an error occurs this function must return without altering errno
 */
static RcStatus copyDebugInfo( void )
{
    ExeFileInfo *old;
    ExeFileInfo *tmp;

    old = &(Pass2Info.OldFile);
    tmp = &(Pass2Info.TmpFile);

    if( RCSEEK( old->Handle, old->DebugOffset, SEEK_SET ) == -1 )
        return( RS_READ_ERROR );
    if( RCSEEK( tmp->Handle, tmp->DebugOffset, SEEK_SET ) == -1 )
        return( RS_WRITE_ERROR );
    return( CopyExeDataTilEOF( old->Handle, tmp->Handle ) );

} /* copyDebugInfo */

static int writeHeadAndTables( int *err_code )
{
    ExeFileInfo *   oldfile;
    ExeFileInfo *   tmpfile;
    NEExeInfo *     oldne;
    NEExeInfo *     tmpne;
    uint_16         tableshift;     /* amount the tables are shifted in the */
                                    /* tmp file */
    uint_16         info;           /* os2_exe_header.info */
    int             error;

    oldfile = &(Pass2Info.OldFile);
    oldne = &oldfile->u.NEInfo;
    tmpfile = &(Pass2Info.TmpFile);
    tmpne = &tmpfile->u.NEInfo;

    /* set the info flag for the new executable from the one flag and */
    /* the command line options given */
    info = oldne->WinHead.info;
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
    tmpfile->WinHeadOffset = oldfile->WinHeadOffset;
    /* copy the WinHead fields up to, but excluding, the segment_off field */
    memcpy( &(tmpne->WinHead), &(oldne->WinHead),
            offsetof(os2_exe_header, segment_off) );
    tmpne->WinHead.info = info;
    tableshift = tmpne->Res.Dir.TableSize +
                tmpne->Res.Str.StringBlockSize -
                (oldne->WinHead.resident_off - oldne->WinHead.resource_off );
    tmpne->WinHead.entry_off = oldne->WinHead.entry_off + tableshift;
    tmpne->WinHead.resident_off = oldne->WinHead.resident_off + tableshift;
    tmpne->WinHead.module_off = oldne->WinHead.module_off + tableshift;
    tmpne->WinHead.import_off = oldne->WinHead.import_off + tableshift;
    tmpne->WinHead.nonres_off = oldne->WinHead.nonres_off + tableshift;
    tmpne->WinHead.segment_off = sizeof( os2_exe_header );
    tmpne->WinHead.resource_off = tmpne->WinHead.segment_off +
                                tmpne->Seg.NumSegs * sizeof( segment_record );
    tmpne->WinHead.movable = oldne->WinHead.movable;
    tmpne->WinHead.resource = tmpne->Res.Dir.NumResources;
    tmpne->WinHead.target = oldne->WinHead.target;
    /* |= the next one since the WIN_GANGLOAD_PRESENT flag may be set */
    tmpne->WinHead.otherflags |= oldne->WinHead.otherflags;
    tmpne->WinHead.swaparea =   0;      /* What is this field for? */
    tmpne->WinHead.expver = CmdLineParms.VersionStamp;

    /* seek to the start of the os2_exe_header in tmpfile */
    if( RCSEEK( tmpfile->Handle, tmpfile->WinHeadOffset, SEEK_SET ) == -1 ) {
        *err_code = errno;
        return( RS_WRITE_ERROR );
    }

    /* write the header */
    if( RCWRITE( tmpfile->Handle, &(tmpne->WinHead), sizeof( os2_exe_header ) ) != sizeof( os2_exe_header ) ) {
        *err_code = errno;
        return( RS_WRITE_ERROR );
    }

    /* write the segment table */
    if( tmpne->Seg.NumSegs > 0 ) {
        unsigned  numwrite;

        numwrite = tmpne->Seg.NumSegs * sizeof( segment_record );
        if( RCWRITE( tmpfile->Handle, tmpne->Seg.Segments, numwrite ) != numwrite ) {
            *err_code = errno;
            return( RS_WRITE_ERROR );
        }
    }

    /* write the resource table */
    error = WriteWINResTable( tmpfile->Handle, &(tmpne->Res), err_code );
    return( error );

} /* writeHeadAndTables */

/*
 * Processing OS/2 NE modules is very very similar to Windows NE processing
 * but there are enough differences in detail to warrant separate
 * implementation to keep the two cleaner.
 */
static int writeOS2HeadAndTables( int *err_code )
{
    ExeFileInfo *   oldfile;
    ExeFileInfo *   tmpfile;
    NEExeInfo *     oldne;
    NEExeInfo *     tmpne;
    uint_16         tableshift;     /* amount the tables are shifted in the */
                                    /* tmp file */
    int             error;

    oldfile = &(Pass2Info.OldFile);
    oldne = &oldfile->u.NEInfo;
    tmpfile = &(Pass2Info.TmpFile);
    tmpne = &tmpfile->u.NEInfo;

    /* copy the fields in the os2_exe_header then change some of them */
    tmpfile->WinHeadOffset = oldfile->WinHeadOffset;
    /* copy the WinHead fields up to, but excluding, the segment_off field */
    memcpy( &(tmpne->WinHead), &(oldne->WinHead),
            offsetof(os2_exe_header, segment_off) );
    tmpne->WinHead.info = oldne->WinHead.info;
    tmpne->WinHead.segment_off = sizeof( os2_exe_header );
    tmpne->WinHead.resource_off = tmpne->WinHead.segment_off +
                                tmpne->Seg.NumSegs * sizeof( segment_record );
    tableshift = tmpne->OS2Res.table_size -
                (oldne->WinHead.resident_off - oldne->WinHead.resource_off) +
                (tmpne->WinHead.resource_off - oldne->WinHead.resource_off);
    tmpne->WinHead.entry_off = oldne->WinHead.entry_off + tableshift;
    tmpne->WinHead.resident_off = oldne->WinHead.resident_off + tableshift;
    tmpne->WinHead.module_off = oldne->WinHead.module_off + tableshift;
    tmpne->WinHead.import_off = oldne->WinHead.import_off + tableshift;
    tmpne->WinHead.nonres_off = oldne->WinHead.nonres_off + tableshift;
    tmpne->WinHead.movable    = oldne->WinHead.movable;
    tmpne->WinHead.resource   = tmpne->OS2Res.num_res_segs;
    tmpne->WinHead.target     = oldne->WinHead.target;
    tmpne->WinHead.otherflags = oldne->WinHead.otherflags;
    tmpne->WinHead.segments   = tmpne->Seg.NumSegs;

    /* seek to the start of the os2_exe_header in tmpfile */
    if( RCSEEK( tmpfile->Handle, tmpfile->WinHeadOffset, SEEK_SET ) == -1 ) {
        *err_code = errno;
        return( RS_WRITE_ERROR );
    }

    /* write the header */
    if( RCWRITE( tmpfile->Handle, &(tmpne->WinHead), sizeof( os2_exe_header ) ) != sizeof( os2_exe_header ) ) {
        *err_code = errno;
        return( RS_WRITE_ERROR );
    }

    /* write the segment table */
    if( tmpne->Seg.NumSegs > 0 ) {
        unsigned  numwrite;

        numwrite = tmpne->Seg.NumSegs * sizeof( segment_record );
        if( RCWRITE( tmpfile->Handle, tmpne->Seg.Segments, numwrite ) != numwrite ) {
            *err_code = errno;
            return( RS_WRITE_ERROR );
        }
    }

    /* write the resource table */
    error = WriteOS2ResTable( tmpfile->Handle, &(tmpne->OS2Res), err_code );
    return( error );

} /* writeOS2HeadAndTables */

static int findEndOfResources( int *err_code )
/* if this exe already contains resources find the end of them so we don't
   copy them with debug information.  Otherwise the file will grow whenever
   a resource file is added */
{
    NEExeInfo                   *oldneinfo;
    uint_32                     *debugoffset;
    WResFileID                  oldhandle;
    WResFileSSize               numread;
    unsigned                    i;
    long                        oldoffset;
    uint_16                     alignshift;
    uint_32                     end;
    uint_32                     tmp;
    resource_type_record        typeinfo;
    resource_record             nameinfo;

    end = 0;
    oldoffset = Pass2Info.OldFile.WinHeadOffset;
    oldneinfo = &Pass2Info.OldFile.u.NEInfo;
    oldhandle = Pass2Info.OldFile.Handle;
    debugoffset = &Pass2Info.OldFile.DebugOffset;

    if( oldneinfo->WinHead.resource_off == oldneinfo->WinHead.resident_off ) {
        return( RS_OK );
    }

    if( RCSEEK( oldhandle, oldneinfo->WinHead.resource_off + oldoffset, SEEK_SET ) == -1 ) {
        *err_code = errno;
        return( RS_READ_ERROR );
    }

    numread = RCREAD( oldhandle, &alignshift, sizeof( uint_16 ) );
    if( numread != sizeof( uint_16 ) ) {
        *err_code = errno;
        return( RCIOERR( oldhandle, numread ) ? RS_READ_ERROR : RS_READ_INCMPLT );
    }
    alignshift = 1 << alignshift;

    numread = RCREAD( oldhandle, &typeinfo, sizeof( resource_type_record ) );
    if( numread != sizeof( resource_type_record ) )  {
        *err_code = errno;
        return( RCIOERR( oldhandle, numread ) ? RS_READ_ERROR : RS_READ_INCMPLT );
    }
    while( typeinfo.type != 0 ) {
        for( i=0; i < typeinfo.num_resources; i++ ) {
            numread = RCREAD( oldhandle, &nameinfo, sizeof( resource_record ) );
            if( numread != sizeof( resource_record ) ) {
                *err_code = errno;
                return( RCIOERR( oldhandle, numread ) ? RS_READ_ERROR : RS_READ_INCMPLT );
            }
            tmp = nameinfo.offset + nameinfo.length;
            if( tmp > end ) end = tmp;
        }
        numread = RCREAD( oldhandle, &typeinfo, sizeof( resource_type_record ) );
        if( numread != sizeof( resource_type_record ) ) {
            *err_code = errno;
            return( RCIOERR( oldhandle, numread ) ? RS_READ_ERROR : RS_READ_INCMPLT );
        }
    }
    end *= alignshift;
    if( end > *debugoffset ) {
        *debugoffset = end;
    }
    return( RS_OK );
}

/*
 * writePEHeadAndObjTable
 * NB when an error occurs this function must return without altering errno
 */
static RcStatus writePEHeadAndObjTable( void )
{
    ExeFileInfo     *tmp;
    pe_object       *last_object;
    int             obj_num;
    uint_32         image_size;
    int             num_objects;
    unsigned_32     object_align;
    exe_pe_header   *pehdr;

    tmp = &Pass2Info.TmpFile;

    /* adjust the image size in the header */
    pehdr = tmp->u.PEInfo.WinHead;
    if( IS_PE64( *pehdr ) ) {
        num_objects = PE64( *pehdr ).num_objects;
        object_align = PE64( *pehdr ).object_align;
    } else {
        num_objects = PE32( *pehdr ).num_objects;
        object_align = PE32( *pehdr ).object_align;
    }
    last_object = &tmp->u.PEInfo.Objects[num_objects - 1];
    image_size = last_object->rva + last_object->physical_size;
    image_size = ALIGN_VALUE( image_size, object_align );
    if( IS_PE64( *pehdr ) ) {
        PE64( *pehdr ).image_size = image_size;
    } else {
        PE32( *pehdr ).image_size = image_size;
    }

    if( RCSEEK( tmp->Handle, tmp->WinHeadOffset, SEEK_SET ) == -1 )
        return( RS_WRITE_ERROR );

    if( IS_PE64( *pehdr ) ) {
        if( RCWRITE( tmp->Handle, &PE64( *pehdr ), sizeof( pe_header64 ) ) != sizeof( pe_header64 ) ) {
            return( RS_WRITE_ERROR );
        }
    } else {
        if( RCWRITE( tmp->Handle, &PE32( *pehdr ), sizeof( pe_header ) ) != sizeof( pe_header ) ) {
            return( RS_WRITE_ERROR );
        }
    }

    for( obj_num = 0; obj_num < num_objects; obj_num++ ) {
        if( RCWRITE( tmp->Handle, tmp->u.PEInfo.Objects + obj_num, sizeof( pe_object ) ) != sizeof( pe_object ) ) {
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
int MergeResExeWINNE( void )
{
    RcStatus        error;
    int             err_code;

    error = copyStubFile( &err_code );
    if( error != RS_OK ) goto HANDLE_ERROR;
    if( StopInvoked ) goto STOP_ERROR;

    error = AllocAndReadWINSegTables( &err_code );
    if( error != RS_OK ) goto HANDLE_ERROR;
    if( StopInvoked ) goto STOP_ERROR;

    InitWINResTable();

    error = seekPastResTable( &err_code );
    if( error != RS_OK ) goto HANDLE_ERROR;
    if( StopInvoked ) goto STOP_ERROR;

    error = findEndOfResources( &err_code );
    if( error != RS_OK ) goto HANDLE_ERROR;
    if( StopInvoked ) goto STOP_ERROR;

    error = copyOtherTables( &err_code );
    if( error != RS_OK ) goto HANDLE_ERROR;
    if( StopInvoked ) goto STOP_ERROR;

    error = copyBody();
    if( error ) return( FALSE );
    if( StopInvoked ) goto STOP_ERROR;

    error = copyDebugInfo();
    if( error != RS_OK ) {
        err_code = errno;
        goto HANDLE_ERROR;
    }
    if( StopInvoked ) goto STOP_ERROR;

    error = writeHeadAndTables( &err_code );
    if( error != RS_OK ) goto HANDLE_ERROR;
    if( StopInvoked ) goto STOP_ERROR;

    return( TRUE );

HANDLE_ERROR:
    switch( error ) {
    case RS_READ_ERROR:
        RcError( ERR_READING_EXE, Pass2Info.OldFile.name,
                 strerror( err_code ) );
        break;
    case RS_READ_INCMPLT:
        RcError( ERR_UNEXPECTED_EOF, Pass2Info.OldFile.name );
        break;
    case RS_WRITE_ERROR:
        RcError( ERR_WRITTING_TMP, Pass2Info.TmpFile.name,
                 strerror( err_code ) );
        break;
    case RS_NO_MEM:
        break;
    default:
       RcError( ERR_INTERNAL, INTERR_UNKNOWN_RCSTATUS );
    }
    return( FALSE );

STOP_ERROR:
    RcFatalError( ERR_STOP_REQUESTED );
#if !defined( __WATCOMC__ )
    return( FALSE );
#endif
} /* MergeResExeWINNE */


int MergeResExeOS2NE( void )
{
    RcStatus        error;
    int             err_code;

    error = copyStubFile( &err_code );
    if( error != RS_OK ) goto HANDLE_ERROR;
    if( StopInvoked ) goto STOP_ERROR;

    error = InitOS2ResTable( &err_code );
    if( error != RS_OK ) goto HANDLE_ERROR;
    if( StopInvoked ) goto STOP_ERROR;

    error = AllocAndReadOS2SegTables( &err_code );
    if( error != RS_OK ) goto HANDLE_ERROR;
    if( StopInvoked ) goto STOP_ERROR;

    error = seekPastResTable( &err_code );
    if( error != RS_OK ) goto HANDLE_ERROR;
    if( StopInvoked ) goto STOP_ERROR;

    error = copyOtherTables( &err_code );
    if( error != RS_OK ) goto HANDLE_ERROR;
    if( StopInvoked ) goto STOP_ERROR;

    error = copyOS2Body();
    if( error ) return( FALSE );
    if( StopInvoked ) goto STOP_ERROR;

    error = copyDebugInfo();
    if( error != RS_OK ) {
        err_code = errno;
        goto HANDLE_ERROR;
    }
    if( StopInvoked ) goto STOP_ERROR;

    error = writeOS2HeadAndTables( &err_code );
    if( error != RS_OK ) goto HANDLE_ERROR;
    if( StopInvoked ) goto STOP_ERROR;
    return( TRUE );

HANDLE_ERROR:
    switch( error ) {
    case RS_READ_ERROR:
        RcError( ERR_READING_EXE, Pass2Info.OldFile.name,
                 strerror( err_code ) );
        break;
    case RS_READ_INCMPLT:
        RcError( ERR_UNEXPECTED_EOF, Pass2Info.OldFile.name );
        break;
    case RS_WRITE_ERROR:
        RcError( ERR_WRITTING_TMP, Pass2Info.TmpFile.name,
                 strerror( err_code ) );
        break;
    case RS_NO_MEM:
        break;
    default:
       RcError( ERR_INTERNAL, INTERR_UNKNOWN_RCSTATUS );
    }
    return( FALSE );

STOP_ERROR:
    RcFatalError( ERR_STOP_REQUESTED );
#if !defined( __WATCOMC__ )
    return( FALSE );
#endif
} /* MergeResExeOS2NE */


static RcStatus updateDebugDirectory( void )
{
    ExeFileInfo         *tmp;
    ExeFileInfo         *old;
    pe_va               old_rva;
    pe_va               tmp_rva;
    unsigned_32         debug_size;
    long                old_offset;
    long                tmp_offset;
    WResFileSSize       numread;
    unsigned            debug_cnt;
    unsigned            read_cnt;
    unsigned            read_size;
    unsigned            i;
    debug_directory     *entry;
    pe_hdr_table_entry  *old_table;
    exe_pe_header       *tmp_pehdr;
    exe_pe_header       *old_pehdr;

    tmp = &Pass2Info.TmpFile;
    old = &Pass2Info.OldFile;
    tmp_pehdr = tmp->u.PEInfo.WinHead;
    old_pehdr = tmp->u.PEInfo.WinHead;
    if( IS_PE64( *tmp_pehdr ) ) {
        tmp_rva = PE64( *tmp_pehdr ).table[PE_TBL_DEBUG].rva;
    } else {
        tmp_rva = PE32( *tmp_pehdr ).table[PE_TBL_DEBUG].rva;
    }
    if( IS_PE64( *old_pehdr ) ) {
        old_table = PE64( *old_pehdr ).table;
    } else {
        old_table = PE32( *old_pehdr ).table;
    }
    debug_size = old_table[PE_TBL_DEBUG].size;
    old_rva = old_table[PE_TBL_DEBUG].rva;

    if( old_rva == 0 ) return( RS_OK );
    old_offset = OffsetFromRVA( old, old_rva );
    tmp_offset = OffsetFromRVA( tmp, tmp_rva );
    if( old_offset == 0xFFFFFFFF || tmp_offset == 0xFFFFFFFF ) {
        return( RS_BAD_FILE_FMT );
    }
    if( RCSEEK( tmp->Handle, tmp_offset, SEEK_SET ) == -1 )
        return( RS_WRITE_ERROR );
    if( RCSEEK( old->Handle, old_offset, SEEK_SET ) == -1 )
        return( RS_READ_ERROR );
    debug_cnt = debug_size / sizeof( debug_directory );
    while( debug_cnt > 0 ) {
        read_cnt = IO_BUFFER_SIZE / sizeof( debug_directory);
        if( read_cnt > debug_cnt )
            read_cnt = debug_cnt;
        read_size = read_cnt * sizeof( debug_directory );
        numread = RCREAD( old->Handle, Pass2Info.IoBuffer, read_size );
        if( numread != read_size )
            return( RCIOERR( old->Handle, numread ) ? RS_READ_ERROR : RS_READ_INCMPLT );
        entry = Pass2Info.IoBuffer;
        for( i=0; i < read_cnt; i++ ) {
            if( entry[i].data_seek >= old->DebugOffset ) {
                entry[i].data_seek += tmp->DebugOffset - old->DebugOffset;
            }
        }
        if( RCWRITE( tmp->Handle, Pass2Info.IoBuffer, read_size ) != read_size )
            return( RS_WRITE_ERROR );
        debug_cnt -= read_cnt;
    }
    return( RS_OK );
} /* updateDebugDirectory */


int MergeResExePE( void )
{
    RcStatus    error;
    int         err_code;

    error = copyStubFile( &err_code );
    if( error != RS_OK ) goto REPORT_ERROR;
    if( StopInvoked ) goto STOP_ERROR;

    error = CopyExeObjects();
    if( error ) goto HANDLE_ERROR;
    if( StopInvoked ) goto STOP_ERROR;

    error = RcBuildResourceObject();
    if( error ) goto HANDLE_ERROR;
    if( StopInvoked ) goto STOP_ERROR;

    error = copyDebugInfo();
    if( error != RS_OK ) {
        err_code = errno;
        goto REPORT_ERROR;
    }
    if( StopInvoked ) goto STOP_ERROR;

    error = writePEHeadAndObjTable();
    if( error != RS_OK ) {
        err_code = errno;
        goto REPORT_ERROR;
    }
    if( StopInvoked ) goto STOP_ERROR;

    error = updateDebugDirectory();
    if( error != RS_OK ) {
        err_code = errno;
        goto REPORT_ERROR;
    }
    if( StopInvoked ) goto STOP_ERROR;

    return( TRUE );

REPORT_ERROR:
    switch( error ) {
    case RS_READ_ERROR:
        RcError( ERR_READING_EXE, Pass2Info.OldFile.name,
                 strerror( err_code )  );
        break;
    case RS_READ_INCMPLT:
        RcError( ERR_UNEXPECTED_EOF, Pass2Info.OldFile.name );
        break;
    case RS_WRITE_ERROR:
        RcError( ERR_WRITTING_TMP, Pass2Info.TmpFile.name,
                 strerror( err_code ) );
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
    return( FALSE );

STOP_ERROR:
    RcFatalError( ERR_STOP_REQUESTED );
#if !defined( __WATCOMC__ )
    return( FALSE );
#endif
} /* MergeResExePE */


/*
 * writeLXHeadAndTables
 * NB when an error occurs this function must return without altering errno
 */
static RcStatus writeLXHeadAndTables( void )
{
    ExeFileInfo     *tmp;
    LXExeInfo       *lx_info;
    unsigned        length;
    long            offset;
    unsigned        i;

    tmp = &Pass2Info.TmpFile;
    lx_info = &tmp->u.LXInfo;

    offset = sizeof( os2_flat_header );
    if( RCSEEK( tmp->Handle, tmp->WinHeadOffset + offset, SEEK_SET ) == -1 )
        return( RS_WRITE_ERROR );

    // write object table
    length = lx_info->OS2Head.num_objects * sizeof( object_record );
    if( RCWRITE( tmp->Handle, lx_info->Objects, length ) != length )
        return( RS_WRITE_ERROR );

    // write page table
    offset += length;
    length = lx_info->OS2Head.num_pages * sizeof( lx_map_entry );
    if( RCWRITE( tmp->Handle, lx_info->Pages, length ) != length )
        return( RS_WRITE_ERROR );

    // write resource table
    offset += length;
    for( i = 0; i < lx_info->OS2Head.num_rsrcs; ++i ) {
        if( RCWRITE( tmp->Handle, &lx_info->Res.resources[i].resource, sizeof( flat_res_table ) ) != sizeof( flat_res_table ) ) {
            return( RS_WRITE_ERROR );
        }
    }

    // finally write LX header
    if( RCSEEK( tmp->Handle, tmp->WinHeadOffset, SEEK_SET ) == -1 )
        return( RS_WRITE_ERROR );

    if( RCWRITE( tmp->Handle, &lx_info->OS2Head, sizeof( os2_flat_header ) ) != sizeof( os2_flat_header ) )
        return( RS_WRITE_ERROR );

    return( RS_OK );
} /* writeLXHeadAndTables */


/*
 * copyLXNonresData
 * NB when an error occurs this function must return without altering errno
 */
static RcStatus copyLXNonresData( void )
{
    ExeFileInfo         *old;
    ExeFileInfo         *tmp;
    os2_flat_header     *old_head;
    os2_flat_header     *new_head;
    RcStatus            error;

    old = &(Pass2Info.OldFile);
    tmp = &(Pass2Info.TmpFile);
    old_head = &old->u.LXInfo.OS2Head;
    new_head = &tmp->u.LXInfo.OS2Head;

    new_head->nonres_size = old_head->nonres_size;
    new_head->nonres_cksum = old_head->nonres_cksum;

    if( old_head->nonres_size == 0 ) {
        new_head->nonres_off = 0;
        return( RS_OK );
    }

    // DebugOffset is pointing to the current EOF
    new_head->nonres_off = tmp->DebugOffset;

    if( RCSEEK( old->Handle, old_head->nonres_off, SEEK_SET ) == -1 )
        return( RS_READ_ERROR );
    if( RCSEEK( tmp->Handle, tmp->DebugOffset, SEEK_SET ) == -1 )
        return( RS_WRITE_ERROR );

    error = CopyExeData( Pass2Info.OldFile.Handle, Pass2Info.TmpFile.Handle, old_head->nonres_size );

    // Make DebugOffset point to new EOF
    CheckDebugOffset( tmp );
    return( error );
} /* copyLXNonresData */


/*
 * copyLXDebugInfo
 * NB when an error occurs this function must return without altering errno
 */
static RcStatus copyLXDebugInfo( void )
{
    ExeFileInfo         *old;
    ExeFileInfo         *tmp;
    os2_flat_header     *old_head;
    os2_flat_header     *new_head;

    old = &(Pass2Info.OldFile);
    tmp = &(Pass2Info.TmpFile);
    old_head = &old->u.LXInfo.OS2Head;
    new_head = &tmp->u.LXInfo.OS2Head;

    if( old_head->debug_len == 0 ) {
        new_head->debug_off = 0;
        new_head->debug_len = 0;
        return( RS_OK );
    }
    new_head->debug_off = tmp->DebugOffset;
    new_head->debug_len = old_head->debug_len;

    if( RCSEEK( old->Handle, old_head->debug_off, SEEK_SET ) == -1 )
        return( RS_READ_ERROR );
    if( RCSEEK( tmp->Handle, tmp->DebugOffset, SEEK_SET ) == -1 )
        return( RS_WRITE_ERROR );
    return( CopyExeDataTilEOF( old->Handle, tmp->Handle ) );
} /* copyLXDebugInfo */


int MergeResExeLX( void )
{
    RcStatus    error;
    int         err_code;

    error = copyStubFile( &err_code );
    if( error != RS_OK ) goto REPORT_ERROR;
    if( StopInvoked ) goto STOP_ERROR;

    error = RcBuildLXResourceObjects();
    if( error ) goto HANDLE_ERROR;
    if( StopInvoked ) goto STOP_ERROR;

    error = CopyLXExeObjects();
    if( error ) goto HANDLE_ERROR;
    if( StopInvoked ) goto STOP_ERROR;

    error = RcWriteLXResourceObjects();
    if( error != RS_OK ) {
        err_code = errno;
        goto REPORT_ERROR;
    }

    error = copyLXNonresData();
    if( error != RS_OK ) {
        err_code = errno;
        goto REPORT_ERROR;
    }
    if( StopInvoked ) goto STOP_ERROR;

    error = copyLXDebugInfo();
    if( error != RS_OK ) {
        err_code = errno;
        goto REPORT_ERROR;
    }
    if( StopInvoked ) goto STOP_ERROR;

    error = writeLXHeadAndTables();
    if( error != RS_OK ) {
        err_code = errno;
        goto REPORT_ERROR;
    }
    if( StopInvoked ) goto STOP_ERROR;

    return( TRUE );

REPORT_ERROR:
    switch( error ) {
    case RS_READ_ERROR:
        RcError( ERR_READING_EXE, Pass2Info.OldFile.name,
                 strerror( err_code )  );
        break;
    case RS_READ_INCMPLT:
        RcError( ERR_UNEXPECTED_EOF, Pass2Info.OldFile.name );
        break;
    case RS_WRITE_ERROR:
        RcError( ERR_WRITTING_TMP, Pass2Info.TmpFile.name,
                 strerror( err_code ) );
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
    return( FALSE );

STOP_ERROR:
    RcFatalError( ERR_STOP_REQUESTED );
#if !defined( __WATCOMC__ )
    return( FALSE );
#endif
} /* MergeResExeLX */
