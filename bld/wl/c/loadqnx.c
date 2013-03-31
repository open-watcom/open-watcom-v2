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
* Description:  Routines for creating QNX load files.
*
****************************************************************************/


#include <string.h>
#include "walloca.h"
#include "linkstd.h"
#include "ring.h"
#include "alloc.h"
#include "msg.h"
#include "wlnkmsg.h"
#include "exeqnx.h"
#include "loadqnx.h"
#include "reloc.h"
#include "specials.h"
#include "objcalc.h"
#include "virtmem.h"
#include "fileio.h"
#include "dbgall.h"
#include "loadfile.h"

static offset           AmountWritten;
static bool             InVerifySegment;
static lmf_rw_end       RWEndRec;
static group_entry *    CurrGroup;


#define QNX_MAX_DATA_SIZE (QNX_MAX_REC_SIZE - sizeof(lmf_data))
#define VERIFY_END (VERIFY_OFFSET + sizeof(RWEndRec.verify))

static void WriteLoadRec( void )
/******************************/
{
    lmf_record      record;
    lmf_data        data;

    record.rec_type = LMF_LOAD_REC;
    record.reserved = record.spare = 0;
    if( CurrGroup->size - AmountWritten > QNX_MAX_DATA_SIZE ){
        record.data_nbytes = QNX_MAX_REC_SIZE;
    } else {
        record.data_nbytes = CurrGroup->size - AmountWritten + sizeof(lmf_data);
    }
    WriteLoad( &record, sizeof( lmf_record ) );
    data.segment = QNX_SEL_NUM( CurrGroup->grp_addr.seg );
    data.offset = AmountWritten;
    WriteLoad( &data, sizeof( lmf_data ) );
}

static void ZeroLoad( virt_mem dummy, unsigned long length )
/**********************************************************/
{
    dummy = dummy;
    PadLoad( length );
}

static void CopyLoad( virt_mem data, unsigned long length )
/*********************************************************/
{
    WriteInfo( data, length );
}

static void WriteQNXInfo( void (*wrfn)(virt_mem, unsigned long), virt_mem data,
                          unsigned long length )
/*****************************************************************************/
{
    unsigned    bufmod;
    unsigned    adjust;

    bufmod = AmountWritten % QNX_MAX_DATA_SIZE;
    while(( bufmod + length ) >= QNX_MAX_DATA_SIZE ) {
        adjust = QNX_MAX_DATA_SIZE - bufmod;
        wrfn( data, adjust );
        AmountWritten += adjust;
        length -= adjust;
        data += adjust;
        bufmod = 0;
        WriteLoadRec();
    }
    if( length > 0 ) {
        wrfn( data, length );
        AmountWritten += length;
    }
}

static void CopyVerify( char *targ, virt_mem src, unsigned long size )
/********************************************************************/
{
    ReadInfo( src, targ, size );
}

static void ZeroVerify( char *targ, virt_mem dummy, unsigned long size )
/**********************************************************************/
{
    dummy = dummy;
    memset( targ, 0, size );
}

static void SetVerifyInfo( void (*wrfn)(char *, virt_mem, unsigned long),
                           virt_mem data, unsigned long length )
/************************************************************************/
{
    offset      newstart;
    char *      targ;

    if( AmountWritten + length <= VERIFY_OFFSET ) return;
    if( AmountWritten >= VERIFY_END ) return;
    newstart = AmountWritten;
    if( AmountWritten < VERIFY_OFFSET ) {
        data += VERIFY_OFFSET - AmountWritten;
        newstart = VERIFY_OFFSET;
    }
    if( newstart + length >= VERIFY_END ) {
        length = VERIFY_END - newstart;
        InVerifySegment = FALSE;
    }
    targ = (char *)(&RWEndRec.verify);
    if( AmountWritten > VERIFY_OFFSET ) {
        targ += AmountWritten - VERIFY_OFFSET;
    }
    wrfn( targ, data, length );
}

static bool WriteSegData( void *_sdata, void *_start )
/****************************************************/
{
    segdata *sdata = _sdata;
    unsigned long *start = _start;
    unsigned long newpos;
    signed long pad;

    if( !sdata->isuninit && !sdata->isdead && sdata->length > 0 ) {
        newpos = *start + sdata->a.delta;
        pad = newpos - AmountWritten;
        DbgAssert( pad >= 0 );
        if( pad > 0 ) {
            if( InVerifySegment ) {
                SetVerifyInfo( ZeroVerify, 0, pad );
            }
            WriteQNXInfo( ZeroLoad, 0, pad );
        }
        if( InVerifySegment ) {
            SetVerifyInfo( CopyVerify, sdata->data, sdata->length );
        }
        WriteQNXInfo( CopyLoad, sdata->data, sdata->length );
    }
    return FALSE;
}

static void DoGroupLeader( void *_seg )
/*************************************/
{
    offset      start;
    seg_leader *seg = _seg;

    start = GetLeaderDelta( seg );
    RingLookup( seg->pieces, WriteSegData, &start );
}

static void WriteQNXGroup( group_entry *grp, unsigned_32 *segments )
/******************************************************************/
{
    segment     seg;

    seg = QNX_SEL_NUM( grp->grp_addr.seg );
    segments[seg] = grp->totalsize | ((unsigned_32)grp->u.qnxflags << 28);
    if( StackSegPtr != NULL && grp == StackSegPtr->group ) {
        segments[seg] -= StackSize;      // stack size gets
    }                                    // added on by QNX loader.
    if( grp->size == 0 ) return;
    AmountWritten = 0;
    CurrGroup = grp;
    WriteLoadRec();
    Ring2Walk( grp->leaders, DoGroupLeader );
}

static void WriteQNXRelocs( RELOC_INFO *head, unsigned lmf_type, unsigned_16 seg )
/********************************************************************************/
{
    lmf_record          record;
    unsigned_32         pos;
    unsigned_32         size;
    bool                islinear;
    unsigned            adjust;

    adjust = 0;
    record.reserved = record.spare = 0;
    record.rec_type = lmf_type;
    islinear = (lmf_type == LMF_LINEAR_FIXUP_REC);
    if( islinear ) {
        adjust = 2;
    }
    pos = PosLoad();    /* get current position */
    while( head != NULL ) {
        SeekLoad( pos + sizeof(lmf_record) );
        if( islinear ) {
            WriteLoad( &seg, sizeof(unsigned_16) );
        }
        size = DumpMaxRelocList( &head, QNX_MAX_FIXUPS - adjust ) + adjust;
        SeekLoad( pos );
        record.data_nbytes = size;
        WriteLoad( &record, sizeof( lmf_record ) );
        pos += size + sizeof( lmf_record );
    }
    SeekLoad( pos );
}

static void WriteQNXData( unsigned_32 * segments )
/************************************************/
{
    group_entry *   grp;
    lmf_record      record;
    bool            first;

    first = TRUE;
    InVerifySegment = FALSE;
    /* write all read/write segment data */
    RWEndRec.signature = 0;
    for( grp = Groups; grp != NULL; grp = grp->next_group ){
        if( grp->u.qnxflags == QNX_READ_WRITE && grp->totalsize != 0 ) {
            if( first && grp->size >= VERIFY_END ) {
                InVerifySegment = TRUE;
                first = FALSE;
            }
            WriteQNXGroup( grp, segments );
        }
        RWEndRec.signature += grp->totalsize;
        RWEndRec.signature <<= 3;
        RWEndRec.signature ^= 0x32476235;
    }
    record.reserved = record.spare = 0;
    record.rec_type = LMF_RW_END_REC;
    record.data_nbytes = sizeof( RWEndRec );
    WriteLoad( &record, sizeof( lmf_record ) );
    RWEndRec.verify ^= (unsigned_16) ~0;
    WriteLoad( &RWEndRec, sizeof( RWEndRec ) );
    /* write all read only segment data */
    for( grp = Groups; grp != NULL; grp = grp->next_group ){
        if( grp->u.qnxflags != QNX_READ_WRITE && grp->totalsize != 0 ) {
            WriteQNXGroup( grp, segments );
        }
        WriteQNXRelocs( grp->g.grp_relocs, LMF_LINEAR_FIXUP_REC,
                        QNX_SEL_NUM(grp->grp_addr.seg) );
    }
}


static bool CheckQNXGrpFlag( void *_seg, void *_grp )
/**************************************************************/
{
    seg_leader     *seg = _seg;
    group_entry    *grp = _grp;
    unsigned_16     sflags;

    sflags = seg->segflags;

// the default value for segflags is set to SEG_LEVEL_3 (0xC00) for OS/2
// and SEG_MOVABLE (0x10) for windows. Since the highest value that can be
// specified for a QNX seg flag is 4, if sflags >= 0x10, there was no QNX value
// specified.

    if( sflags < 0x10 ) {
        if( !(sflags & 1) ) {     // if can read/write or exec/read
            grp->u.qnxflags &= ~1;       // can for all segments.
            return TRUE;                // no need to check others
        }
    } else {
// make segments read/write or exec/read unless every segment is specifically
// set otherwise.
        grp->u.qnxflags &= ~1;
        return TRUE;
    }
    return FALSE;
}

static void SetQNXGroupFlags( void )
/**********************************/
// This goes through the groups, setting the flag word to be compatible with
// the flag words that are specified in the segments.
{
    group_entry *   group;

    for( group = Groups; group != NULL; group = group->next_group ) {
        if( group->segflags & SEG_DATA ) {
            group->u.qnxflags = QNX_READ_ONLY;      // 1
        } else {
            group->u.qnxflags = QNX_EXEC_ONLY;     // 3
        }
        Ring2Lookup( group->leaders, CheckQNXGrpFlag, group );
    }
}

void SetQNXSegFlags( void )
/********************************/
{
    SetSegFlags( (seg_flags *) FmtData.u.qnx.seg_flags );
    FmtData.u.qnx.seg_flags = NULL;        // segsegflags frees the list.
    SetQNXGroupFlags();
}

static void WriteQNXResource( void )
/**********************************/
{
    unsigned long   len;
    lmf_record      rec;
    f_handle        file;
    lmf_resource    resource;
    void *          buf;

    if( FmtData.resource != NULL ) {
        rec.reserved = 0;
        rec.spare = 0;
        rec.rec_type = LMF_RESOURCE_REC;
        memset( &resource, 0, sizeof( lmf_resource ) );
        if( FmtData.res_name_only ) {
            file = QObjOpen( FmtData.resource );
            if( file == NIL_FHANDLE ) {
                PrintIOError( WRN+MSG_CANT_OPEN_NO_REASON, "s",
                                        FmtData.resource );
                return;
            }
            len = QFileSize( file );
            if( len + sizeof(lmf_resource) > QNX_MAX_REC_SIZE ) {
                LnkMsg( WRN+MSG_RESOURCE_TOO_BIG, "s", FmtData.resource );
                return;
            }
            _ChkAlloc( buf, len );
            rec.data_nbytes = len + sizeof( lmf_resource );
            WriteLoad( &rec, sizeof( lmf_record ) );
            WriteLoad( &resource, sizeof( lmf_resource ) );
            QRead( file, buf, len, FmtData.resource );
            WriteLoad( buf, len );
            _LnkFree( buf );
            QClose( file, FmtData.resource );
        } else {
            len = strlen( FmtData.resource );
            FmtData.resource[len] = '\n';
            len++;
            rec.data_nbytes = len + sizeof( lmf_resource );
            WriteLoad( &rec, sizeof( lmf_record ) );
            WriteLoad( &resource, sizeof( lmf_resource ) );
            WriteLoad( FmtData.resource, len );
        }
    }
}

void FiniQNXLoadFile( void )
/*********************************/
{
    unsigned_32 *   segments;
    unsigned_16     nbytes;
    lmf_record      record;
    lmf_header      header;
    lmf_eof         trailer;
    unsigned        adseg;

    CurrSect = Root;
    if( FmtData.type & MK_QNX_FLAT ) {
        if( FmtData.base < StackSize ) {
            LnkMsg( WRN+MSG_QNX_BASE_LT_STACK, NULL );
        }
    }
    nbytes = NumGroups * sizeof( unsigned_32 );
    segments = (unsigned_32 *) alloca( nbytes );
    SeekLoad( sizeof(lmf_header) + nbytes + sizeof(lmf_record) );
    WriteQNXResource();
    WriteQNXData( segments );
    WriteQNXRelocs( Root->reloclist, LMF_FIXUP_REC, 0 );
    WriteQNXRelocs( FloatFixups, LMF_8087_FIXUP_REC, 0 );
    record.reserved = record.spare = 0;
    record.rec_type = LMF_IMAGE_END_REC;
    record.data_nbytes = sizeof( trailer );
    WriteLoad( &record, sizeof( lmf_record ) );
    memset( &trailer, 0, sizeof( trailer ) );
    WriteLoad( &trailer, sizeof( trailer ) );
    DBIWrite();

    SeekLoad( 0 );
    record.rec_type = LMF_HEADER_REC;
    record.data_nbytes = sizeof( lmf_header ) + nbytes;
    WriteLoad( &record, sizeof( lmf_record ) );
    memset( &header, 0, sizeof( header ) );
    header.version = QNX_VERSION;
    header.cflags = (FmtData.u.qnx.flags
                | (FmtData.u.qnx.priv_level << QNX_PRIV_SHIFT))
                & QNX_FLAG_MASK;
    if( LinkState & FMT_SEEN_32_BIT ) {
        header.cflags |= _TCF_32BIT;
    }
    if( FmtData.type & MK_QNX_FLAT ) {
        header.cflags |= _TCF_FLAT;
    }
    header.fpu = 0;     //NYI: need to set 87, 287, 387
    if( FmtData.cpu_type == 0 ) {        // no model specified, so assume...
        if( LinkState & FMT_SEEN_32_BIT ) {
            header.cpu = 386;
        } else {
            header.cpu = 86;
        }
    } else {
        header.cpu = (FmtData.cpu_type * 100) + 86;
    }
    header.code_index = QNX_SEL_NUM( StartInfo.addr.seg );
    header.stack_index = QNX_SEL_NUM( StackAddr.seg );
    adseg = header.stack_index;
    if( DataGroup != NULL ) {
        adseg = QNX_SEL_NUM( DataGroup->grp_addr.seg );
    }
    header.heap_index = adseg;  // all in DGROUP.
    header.argv_index = adseg;
    header.code_offset = StartInfo.addr.off;
    header.stack_nbytes = StackSize;
    header.heap_nbytes = FmtData.u.qnx.heapsize;
    header.image_base = FmtData.base;
    WriteLoad( &header, sizeof( lmf_header ) );
    WriteLoad( segments, nbytes );
}

unsigned_16 ToQNXSel( unsigned_16 seg )
/********************************************/
{
    return( QNX_SELECTOR( seg - 1 ) );
}

unsigned_16 ToQNXIndex( unsigned_16 sel )
/**********************************************/
{
    return( QNX_SEL_NUM( sel ) );
}
