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


#include <io.h>
#include <string.h>
#include "watcom.h"
#include "exeos2.h"
#include "global.h"
#include "errors.h"
#include "rcmem.h"
#include "rcstr.h"
#include "exeseg.h"
#include "exeres.h"
#include "exeobj.h"
#include "exerespe.h"
#include "exeutil.h"
#include "pass2.h"
#include "iortns.h"

/*
 * copyStubFile - copy from the begining of the file to the start of
 *                the win exe header
 */
static int copyStubFile( int *err_code )
{
    RcStatus    error;

    error = CopyExeData( Pass2Info.OldFile.Handle, Pass2Info.TmpFile.Handle,
                         Pass2Info.OldFile.WinHeadOffset );
    *err_code = errno;
    return( error );
} /* copyStubFile */

static RcStatus seekPastResTable( int *err_code )
{
    int             winheadoffset;
    int             seekamount;
    ExeFileInfo *   tmpexe;

    tmpexe = &(Pass2Info.TmpFile);

    seekamount = sizeof(os2_exe_header) +
                    tmpexe->u.NEInfo.Seg.NumSegs * sizeof(segment_record) +
                    tmpexe->u.NEInfo.Res.Dir.TableSize +
                    tmpexe->u.NEInfo.Res.Str.StringBlockSize;
    winheadoffset = RcSeek( tmpexe->Handle, seekamount, SEEK_CUR );
    if( winheadoffset == -1 ) {
        *err_code = errno;
        return( RS_READ_ERROR );
    } else {
        tmpexe->WinHeadOffset = winheadoffset;
        return( RS_OK );
    }

} /* seekPastResTable */

static int copyOtherTables( int *err_code )
{
    int                 tablelen;
    os2_exe_header *    oldhead;
    uint_32             oldoffset;
    int                 oldhdl;
    int                 seek_rc;
    RcStatus            ret;

    oldhead = &(Pass2Info.OldFile.u.NEInfo.WinHead);
    oldoffset = Pass2Info.OldFile.WinHeadOffset;
    oldhdl = Pass2Info.OldFile.Handle;

    /* the other tables start at the resident names table and end at the end */
    /* of the non-resident names table */
    tablelen = (oldhead->nonres_off + oldhead->nonres_size) -
                (oldhead->resident_off + oldoffset);

    seek_rc = RcSeek( oldhdl, oldhead->resident_off + oldoffset, SEEK_SET );
    if( seek_rc == -1) {
        *err_code = errno;
        return( RS_READ_ERROR );
    }

    ret = CopyExeData( Pass2Info.OldFile.Handle, Pass2Info.TmpFile.Handle,
                tablelen );
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
        filelen += ComputeResourceSize( Pass2Info.ResFiles->Dir );
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
    uint_16             sect2mask;
    uint_16             sect2bits;
    uint_16             shift_count;
    uint_32             gangloadstart;
    uint_32             gangloadlen;
    CpSegRc             copy_segs_ret;
    bool                use_gangload;
    int                 error;

    tmp = &Pass2Info.TmpFile.u.NEInfo;

    switch (CmdLineParms.SegmentSorting) {
    case SEG_SORT_NONE:     /* all segments in section 2 */
        sect2mask = 0;
        sect2bits = 0;
        use_gangload = FALSE;
        Pass2Info.TmpFile.u.NEInfo.WinHead.align =
                            Pass2Info.OldFile.u.NEInfo.WinHead.align;
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
    }

    /* third arg to Copy???? is FALSE --> copy section one */
    gangloadstart = RcTell( Pass2Info.TmpFile.Handle );
    gangloadstart += AlignAmount( gangloadstart, tmp->Res.Dir.ResShiftCount );
    copy_segs_ret = CopySegments( sect2mask, sect2bits, FALSE );
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
        break;
    }
    if( ! CmdLineParms.NoResFile ) {
        error = CopyResources( sect2mask, sect2bits, FALSE );
        if( error) return( TRUE  );
    }
    gangloadlen = RcTell( Pass2Info.TmpFile.Handle ) - gangloadstart;

    /* third arg to Copy???? is TRUE  --> copy section two */
    copy_segs_ret = CopySegments( sect2mask, sect2bits, TRUE );
    if( copy_segs_ret == CPSEG_ERROR) {
        return( TRUE  );
    }
    if( !CmdLineParms.NoResFile ) {
        error = CopyResources( sect2mask, sect2bits, TRUE );
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

/*
 * copyDebugInfo
 * NB when an error occurs this function must return without altering errno
 */
static RcStatus copyDebugInfo( void )
{
    long            seek_rc;
    ExeFileInfo *   old;
    ExeFileInfo *   tmp;

    old = &(Pass2Info.OldFile);
    tmp = &(Pass2Info.TmpFile);

    seek_rc = RcSeek( old->Handle, old->DebugOffset, SEEK_SET );
    if( seek_rc == -1) return( RS_READ_ERROR );
    seek_rc = RcSeek( tmp->Handle, tmp->DebugOffset, SEEK_SET );
    if( seek_rc == -1) return( RS_WRITE_ERROR );
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
    long            seekrc;
    int             numwrote;
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
    tmpne->WinHead.segment_off = sizeof(os2_exe_header);
    tmpne->WinHead.resource_off = tmpne->WinHead.segment_off +
                                tmpne->Seg.NumSegs * sizeof(segment_record);
    tmpne->WinHead.movable = oldne->WinHead.movable;
    tmpne->WinHead.resource = tmpne->Res.Dir.NumResources;
    tmpne->WinHead.target = oldne->WinHead.target;
    /* |= the next one since the WIN_GANGLOAD_PRESENT flag may be set */
    tmpne->WinHead.otherflags |= oldne->WinHead.otherflags;
    tmpne->WinHead.swaparea =   0;      /* What is this field for? */
    tmpne->WinHead.expver = CmdLineParms.VersionStamp;

    /* seek to the start of the os2_exe_header in tmpfile */
    seekrc = RcSeek( tmpfile->Handle, tmpfile->WinHeadOffset, SEEK_SET );
    if( seekrc == -1 ) {
        *err_code = errno;
        return( RS_WRITE_ERROR );
    }

    /* write the header */
    numwrote = RcWrite( tmpfile->Handle, &(tmpne->WinHead),
                sizeof(os2_exe_header) );
    if( numwrote != sizeof(os2_exe_header) ) {
        *err_code = errno;
        return( RS_WRITE_ERROR );
    }

    /* write the segment table */
    if( tmpne->Seg.NumSegs > 0 ) {
        numwrote = RcWrite( tmpfile->Handle, tmpne->Seg.Segments,
                    tmpne->Seg.NumSegs * sizeof(segment_record) );
        if( numwrote != tmpne->Seg.NumSegs * sizeof(segment_record)  ) {
            *err_code = errno;
            return( RS_WRITE_ERROR );
        }
    }

    /* write the resource table */
    error = WriteResTable( tmpfile->Handle, &(tmpne->Res), err_code );
    return( error );

} /* writeHeadAndTables */

static int findEndOfResources( int *err_code )
/* if this exe already contains resources find the end of them so we don't
   copy them with debug information.  Otherwise the file will grow whenever
   a resource file is added */
{
    NEExeInfo                   *oldneinfo;
    uint_32                     *debugoffset;
    int                         oldhdl;
    int                         rc;
    unsigned                    i;
    uint_32                     oldoffset;
    uint_16                     alignshift;
    uint_32                     end;
    uint_32                     tmp;
    resource_type_record        typeinfo;
    resource_record             nameinfo;

    end = 0;
    oldoffset = Pass2Info.OldFile.WinHeadOffset;
    oldneinfo = &Pass2Info.OldFile.u.NEInfo;
    oldhdl = Pass2Info.OldFile.Handle;
    debugoffset = &Pass2Info.OldFile.DebugOffset;

    if( oldneinfo->WinHead.resource_off == oldneinfo->WinHead.resident_off ) {
        return( RS_OK );
    }

    rc = RcSeek( oldhdl, oldneinfo->WinHead.resource_off + oldoffset,
                SEEK_SET );
    if( rc == -1 ) {
        *err_code = errno;
        return( RS_READ_ERROR );
    }

    rc = RcRead( oldhdl, &alignshift, sizeof( uint_16 ) );
    if( rc != sizeof( uint_16 ) ) {
        *err_code = errno;
        if( rc == -1 ) {
            return( RS_READ_ERROR );
        } else {
            return( RS_READ_INCMPLT );
        }
    }
    alignshift = 1 << alignshift;

    rc = RcRead( oldhdl, &typeinfo, sizeof( resource_type_record ) );
    if( rc != sizeof( resource_type_record ) )  {
        *err_code = errno;
        if( rc == -1 ) {
            return( RS_READ_ERROR );
        } else {
            return( RS_READ_INCMPLT );
        }
    }
    while( typeinfo.type != 0 ) {
        for( i=0; i < typeinfo.num_resources; i++ ) {
            rc = RcRead( oldhdl, &nameinfo, sizeof( resource_record ) );
            if( rc != sizeof( resource_record ) ) {
                *err_code = errno;
                if( rc == -1 ) {
                    return( RS_READ_ERROR );
                } else {
                    return( RS_READ_INCMPLT );
                }
            }
            tmp = nameinfo.offset + nameinfo.length;
            if( tmp > end ) end = tmp;
        }
        rc = RcRead( oldhdl, &typeinfo, sizeof( resource_type_record ) );
        if( rc != sizeof( resource_type_record ) ) {
            *err_code = errno;
            if( rc == -1 ) {
                return( RS_READ_ERROR );
            } else {
                return( RS_READ_INCMPLT );
            }
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
    ExeFileInfo *   tmp;
    PEExeInfo *     pe_info;
    pe_object *     last_object;
    long            seek_rc;
    int             num_wrote;
    int             obj_num;
    uint_32         image_size;

    tmp = &Pass2Info.TmpFile;
    pe_info = &tmp->u.PEInfo;

    /* adjust the image size in the header */
    last_object = &pe_info->Objects[ pe_info->WinHead->num_objects - 1 ];
    image_size = last_object->rva + last_object->physical_size;
    image_size = ALIGN_VALUE( image_size, pe_info->WinHead->object_align );
    pe_info->WinHead->image_size = image_size;

    seek_rc = RcSeek( tmp->Handle, tmp->WinHeadOffset, SEEK_SET );
    if( seek_rc == -1 ) return( RS_WRITE_ERROR );

    num_wrote = RcWrite( tmp->Handle, pe_info->WinHead, sizeof(pe_header) );
    if( num_wrote != sizeof(pe_header) ) return( RS_WRITE_ERROR );

    for( obj_num = 0; obj_num < pe_info->WinHead->num_objects; obj_num++ ) {
        num_wrote = RcWrite( tmp->Handle, pe_info->Objects + obj_num,
                            sizeof(pe_object) );
        if( num_wrote != sizeof(pe_object) ) return( RS_WRITE_ERROR );
    }

    return( RS_OK );

} /* writePEHeadAndObjTable */

extern int MergeResExeNE( void )
{
    RcStatus        error;
    int             err_code;

    error = copyStubFile( &err_code );
    if( error != RS_OK ) goto HANDLE_ERROR;
    if( StopInvoked ) goto STOP_ERROR;

    error = AllocAndReadSegTables( &err_code );
    if( error != RS_OK ) goto HANDLE_ERROR;
    if( StopInvoked ) goto STOP_ERROR;

    InitResTable();

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
                 strerror( err_code )  );
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
#if defined( __ALPHA__ ) || defined( UNIX )
    return( FALSE );
#endif

} /* MergeResExeNE */

extern RcStatus updateDebugDirectory( void ) {

    ExeFileInfo         *tmp;
    ExeFileInfo         *old;
    pe_va               old_rva;
    pe_va               tmp_rva;
    unsigned_32         debug_size;
    unsigned_32         old_offset;
    unsigned_32         tmp_offset;
    long int            io_rc;
    unsigned            debug_cnt;
    unsigned            read_cnt;
    unsigned            read_size;
    unsigned            i;
    debug_directory     *entry;


    tmp = &Pass2Info.TmpFile;
    old = &Pass2Info.OldFile;

    debug_size = old->u.PEInfo.WinHead->table[PE_TBL_DEBUG].size;
    old_rva = old->u.PEInfo.WinHead->table[PE_TBL_DEBUG].rva;
    tmp_rva = tmp->u.PEInfo.WinHead->table[PE_TBL_DEBUG].rva;

    if( old_rva == 0 ) return( RS_OK );
    old_offset = OffsetFromRVA( old, old_rva );
    tmp_offset = OffsetFromRVA( tmp, tmp_rva );
    if( old_offset == 0xFFFFFFFF || tmp_offset == 0xFFFFFFFF ) {
        return( RS_BAD_FILE_FMT );
    }
    io_rc = RcSeek( tmp->Handle, tmp_offset, SEEK_SET );
    if( io_rc == -1 ) return( RS_WRITE_ERROR );
    io_rc = RcSeek( old->Handle, old_offset, SEEK_SET );
    if( io_rc == -1 ) return( RS_READ_ERROR );
    debug_cnt = debug_size /sizeof( debug_directory );
    while( debug_cnt > 0 ) {
        read_cnt = min( IO_BUFFER_SIZE / sizeof( debug_directory), debug_cnt );
        read_size = read_cnt * sizeof( debug_directory );
        io_rc = RcRead( old->Handle, Pass2Info.IoBuffer, read_size );
        if( io_rc == -1 ) return( RS_READ_ERROR );
        if( io_rc != read_size ) return( RS_READ_INCMPLT );
        entry = Pass2Info.IoBuffer;
        for( i=0; i < read_cnt; i++ ) {
            if( entry[i].data_seek >= old->DebugOffset ) {
                entry[i].data_seek += tmp->DebugOffset - old->DebugOffset;
            }
        }
        io_rc = RcWrite( tmp->Handle, Pass2Info.IoBuffer, read_size );
        if( io_rc != read_size ) return( RS_WRITE_ERROR );
        debug_cnt -= read_cnt;
    }
    return( RS_OK );
} /* updateDebugDirectory */

extern int MergeResExePE( void )
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
#if defined( __ALPHA__ ) || defined( UNIX )
    return( FALSE );
#endif

} /* MergeResExePE */
