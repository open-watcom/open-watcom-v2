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
* Description:  Functions for creating binary and hex load files.
*
****************************************************************************/


#include <string.h>
#include <stdio.h>
#include "linkstd.h"
#include "ring.h"
#include "pcobj.h"
#include "newmem.h"
#include "msg.h"
#include "alloc.h"
#include "wlnkmsg.h"
#include "virtmem.h"
#include "fileio.h"
#include "objcalc.h"
#include "dbgall.h"
#include "loadraw.h"
#include "loadfile.h"


typedef offset  hex_offset;

typedef segment hex_segment;

typedef struct hex_addr {
    hex_offset      off;
    hex_segment     seg;
} hex_addr;

static bool WriteBinSegGroup( group_entry *group )
/*********************************************/
/* write the data for group to the loadfile */
/* returns true if the file should be repositioned */
{
    unsigned long       file_loc;
    section             *sect;
    bool                repos;
    outfilelist         *finfo;

    repos = false;
    if( group->size != 0 || (group->leaders->class->flags & CLASS_COPY) ) {
        sect = group->section;
        CurrSect = sect;
        finfo = sect->outfile;
        file_loc = GROUP_FILE_LOC( group );
        // Offset may make this go negative for NOEMIT classes or segments
        if( (long)file_loc >= 0 ) {
            if( file_loc > finfo->file_loc ) {
                PadLoad( file_loc - finfo->file_loc );
            } else if( file_loc != finfo->file_loc ) {
                SeekLoad( file_loc );
                repos = true;
            }
            DEBUG((DBG_LOADDOS, "group %a section %d to %l in %s",
                &group->grp_addr, sect->ovl_num, file_loc, finfo->fname ));
            file_loc += WriteDOSGroupLoad( group, repos );
            if( file_loc > finfo->file_loc ) {
                finfo->file_loc = file_loc;
            }
        }
    }
    return( repos );
}


void BinOutput( void )
/***************************/
{
    outfilelist         *fnode;
    group_entry         *group;
    bool                repos;
    unsigned_32         size;
    signed_32           diff;

    if( FmtData.type & (MK_PE | MK_QNX_FLAT | MK_OS2_FLAT | MK_ELF) ) {
        CurrSect = Root;        // needed for WriteInfo.
        Root->sect_addr = Groups->grp_addr;
        fnode = Root->outfile;
        fnode->file_loc = 0;
        Root->u.file_loc = Root->sect_addr.off - FmtData.output_offset;
        /* write groups */
        for( group = Groups; group != NULL; group = group->next_group ) {
            if (group->leaders->class->flags & CLASS_COPY ) {
                size = CalcGroupSize( group->leaders->class->DupClass->segs->group );
            } else {
                size = CalcGroupSize( group );
            }
            if( size ) {
                diff = ( FmtData.base + group->grp_addr.off + group->linear - FmtData.output_offset ) - PosLoad();
                if( diff < 0 ) {
                    LnkMsg( ERR + MSG_FIXED_LOC_BEFORE_CUR_LOC, "a", &(group->grp_addr));
                } else if( diff > 0 ) {
                    PadLoad( diff );
                }
                WriteGroupLoad( group );
            }
        }
    } else {
        OrderGroups( CompareDosSegments );
        CurrSect = Root;        // needed for WriteInfo.
        Root->sect_addr = Groups->grp_addr;
        fnode = Root->outfile;
        fnode->file_loc = 0;
        Root->u.file_loc = ( Root->sect_addr.seg << FmtData.SegShift ) + Root->sect_addr.off - FmtData.output_offset;
        /* write groups */
        for( group = Groups; group != NULL; group = group->next_group ) {
            repos = WriteBinSegGroup( group );
            if( repos ) {
                SeekLoad( fnode->file_loc );
            }
        }
    }
    DBIWrite();
}

// The following routine generates Intel Hex records using data buffered in LineBuf.
// A full record is 16 bytes of data.  Partial records are allowed when needed
// because of gaps in the loadfile data.  Initially segmented addressing is assumed
// with a starting segment value of 0 (Original 64K Intel Hex records).  If data
// is encountered above 64K, the routine outputs a segment record, seamlessly
// switching to segmented mode.  Segmented mode can use either the standard 4 bit
// shift (default) or the shift defined in FmtData.SegShift, or if set, the
// value in FmtData.HexSegShift, which is uniquely used by this function.  The
// purpose for this is that 24 bit addressing can be fully handled using segmented
// addressing (rather than extended linear addressing) if SegShift is 8.
// If data is encountered above what Segmented addressing can handle (1 M for default)
// the routine outputs an extended linear address record, seamlessly switching
// to a full 32 bit address range.  This approach provides full backward compatibility
// for systems which cannot read the newer formats when creating files that don't
// need the extra range.

#define HEXLEN 16   // number of bytes of data in a full record
static hex_addr     next_addr;
static bool         linear;
static char         lineBuf[HEXLEN];
static size_t       buf_offset;

static void WriteHexLine( void )
/******************************/
{

    char             str_buf[2 * HEXLEN + 15];
    unsigned_16      checksum;
    unsigned_16      i;
    unsigned_16      seg_shift = FmtData.output_hshift ? FmtData.HexSegShift : FmtData.SegShift;
    unsigned_16      curr_offs;

    if( ( next_addr.off + buf_offset >= ( 0x10000L << seg_shift ) ) && !linear ) {
        next_addr.seg = 0;
        linear = true;
    }

    if( !linear ) {
        if( next_addr.off - ( next_addr.seg << seg_shift ) + buf_offset > 0x10000L ) { // See if we need to output a segment record
            next_addr.seg = (hex_segment)( next_addr.off >> seg_shift );
            checksum = 4 + (( next_addr.seg >> 8 ) & 0xFF) + (next_addr.seg & 0xFF);
            sprintf( str_buf, ":02000002%04x%02x\r\n", next_addr.seg, (-checksum) & 0xFF );
            WriteLoad( str_buf, 17 );
        }
        curr_offs = (unsigned_16)( next_addr.off - ( next_addr.seg << seg_shift ) );
    } else {
        if( next_addr.off - ( next_addr.seg << 16 ) + buf_offset > 0x10000L ) { // See if we need to output
            next_addr.seg = (hex_segment)( next_addr.off >> 16 );               //   an extended linear record
            checksum = 6 + (( next_addr.seg >> 8 ) & 0xFF) + (next_addr.seg & 0xFF);
            sprintf( str_buf, ":02000004%04x%02x\r\n", next_addr.seg, (-checksum) & 0xFF );
            WriteLoad( str_buf, 17);
        }
        curr_offs = (unsigned_16)( next_addr.off - ( next_addr.seg << 16 ) );
    }

    sprintf( str_buf, ":%02x%04x00", (unsigned)buf_offset, curr_offs );         // Intel Hex header
    checksum = buf_offset + (( curr_offs >> 8 ) & 0xFF) + (curr_offs & 0xFF);   // Start checksum using above elements

    i = 0;
    for( ; buf_offset > 0; --buf_offset ) { // Do data bytes, leaving buf_offset 0 at end
        sprintf( str_buf + 9 + 2 * i, "%02x", lineBuf[i] );
        checksum += lineBuf[i++];
    }
    sprintf( str_buf + 9 + 2 * i, "%02x\r\n", (-checksum) & 0xFF );
    WriteLoad( str_buf, 13 + 2 * i );
}

static bool WriteHexData( void *_sdata, void *_offs )
/***************************************************/
{
    segdata          *sdata = _sdata;
    hex_offset       offs = *(hex_offset *)_offs + sdata->a.delta;
    offset           len = sdata->length;
    offset           src_offs;
    offset           piece;

    if( !sdata->isuninit && !sdata->isdead && len > 0 ) {
        if( offs != next_addr.off + buf_offset ) {  // Must start new record if address not contiguous
            if( offs < next_addr.off + buf_offset ) {
                targ_addr   addr;

                addr.off = offs;
                addr.seg = 0;
                LnkMsg( ERR + MSG_FIXED_LOC_BEFORE_CUR_LOC, "a", &addr );
            }
            if( buf_offset > 0 ) {          // If partial record in buffer, flush
                WriteHexLine();
            }
            next_addr.off = offs;
        }
        src_offs = 0;
        for( ; len > 0; len -= piece ) {    // Now lob out records
            piece = HEXLEN - buf_offset;    // Handle partially filled buffer
            if( piece > len ) {             // Handle not enough to fill buffer
                piece = len;
            }
            ReadInfo( sdata->u1.vm_ptr + src_offs, lineBuf + buf_offset, piece );
            buf_offset += piece;
            if( buf_offset == HEXLEN ) {
                WriteHexLine();             // Only write full buffers
                next_addr.off += HEXLEN;    // next_addr.off reflects start of line
            }                               // Partial records will be written later
            src_offs += piece;              //   if address is not contiguous
        }
    }
    return( false );
}

static bool DoHexLeader( void *seg, void *_grp_offs )
/***************************************************/
{
    if ( !(((seg_leader *)seg)->class->flags & CLASS_NOEMIT ||
           ((seg_leader *)seg)->segflags & SEG_NOEMIT) ) {
        hex_offset   offs = *(hex_offset *)_grp_offs + SEG_GROUP_DELTA( (seg_leader *)seg );

        RingLookup( ((seg_leader *)seg)->pieces, WriteHexData, &offs );
    }
    return( false );
}

static bool DoHexDupLeader( void *seg, void *_grp_offs )
/******************************************************/
{
    hex_offset offs = *(hex_offset *)_grp_offs + SEG_GROUP_DELTA( (seg_leader *)seg );

    RingLookup( ((seg_leader *)seg)->pieces, WriteHexData, &offs );
    return( false );
}

static void WriteStart( void )
/****************************/
{
    char        str_buf[22];
    unsigned_16 checksum;

    if( StartInfo.addr.off > 0xffff ) {
        checksum = 9 + ((StartInfo.addr.off >> 24) & 0xFF) + ((StartInfo.addr.off >> 16) & 0xFF) + ((StartInfo.addr.off >> 8) & 0xFF) + (StartInfo.addr.off & 0xFF);
        sprintf( str_buf, ":04000005%08lx%02x\r\n", (unsigned long)StartInfo.addr.off, (-checksum) & 0xFF );
    } else {
        checksum = 7 + ((StartInfo.addr.seg >> 8) & 0xFF) + (StartInfo.addr.seg & 0xFF) + ((StartInfo.addr.off >> 8) & 0xFF) + (StartInfo.addr.off & 0xFF);
        sprintf( str_buf, ":04000003%04x%04x%02x\r\n", (unsigned)StartInfo.addr.seg, (unsigned)StartInfo.addr.off, (-checksum) & 0xFF );
    }
    WriteLoad( str_buf, 21 );
}

typedef struct  {
    hex_offset      offs;
    group_entry     *lastgrp;  // used only for copy classes
} grpwriteinfo;

static bool WriteHexCopyGroups( void *_seg, void *_info )
/*******************************************************/
{
    // This is called by the outer level iteration looking for classes
    //  that have more than one group in them
    seg_leader * seg = _seg;
    grpwriteinfo *info = _info;

    if( info->lastgrp != seg->group ) {   // Only interate new groups
        info->lastgrp = seg->group;
        // Check each initialized segment in group
        Ring2Lookup( seg->group->leaders, DoHexDupLeader, &info->offs );
        info->offs += seg->group->totalsize;
    }
    return( false );
}


void HexOutput( void )
/********************/
{
    outfilelist         *fnode;
    group_entry         *group;
    group_entry         *wrkgrp;
    section             *sect;
#ifdef _INT_DEBUG
    outfilelist         *finfo;
#endif
    unsigned_32         size;
    class_entry         *class;
    grpwriteinfo        info;
//    unsigned long     file_loc;

    next_addr.off = 0;      // Start at absolute linear address 0
    next_addr.seg = 0;
    linear        = false;  //       in segmented mode
    buf_offset    = 0;

    if( FmtData.type & (MK_PE | MK_QNX_FLAT | MK_OS2_FLAT | MK_ELF) ) {
        CurrSect = Root;    // needed for WriteInfo.
        Root->sect_addr = Groups->grp_addr;
        fnode = Root->outfile;
        fnode->file_loc = 0;
        Root->u.file_loc = Root->sect_addr.off - FmtData.output_offset;
        /* write groups */
        for( group = Groups; group != NULL; group = group->next_group ) {
            class = group->leaders->class;
            if( class->flags & CLASS_COPY ) {
                wrkgrp = class->DupClass->segs->group;
            } else {
                wrkgrp = group;
            }
            size = CalcGroupSize( wrkgrp );
            if( size != 0 ) {
                info.offs = (group->grp_addr.off + group->linear - FmtData.output_offset);
                sect = wrkgrp->section;
                CurrSect = sect;
#ifdef _INT_DEBUG
                finfo = sect->outfile;
                DEBUG((DBG_LOADDOS, "group %a section %d to %l in %s",
                    &group->grp_addr, sect->ovl_num, info.offs, finfo->fname ));
#endif
                if( group->leaders->class->flags & CLASS_COPY ) {
                    Ring2Lookup( wrkgrp->leaders, DoHexDupLeader, &info.offs );
                } else {
                    Ring2Lookup( wrkgrp->leaders, DoHexLeader, &info.offs );
                }
            }
        }
    } else {
        OrderGroups( CompareDosSegments );
        CurrSect = Root;    // needed for WriteInfo.
        Root->sect_addr = Groups->grp_addr;
        fnode = Root->outfile;
        fnode->file_loc = 0;
        Root->u.file_loc = (Root->sect_addr.seg << FmtData.SegShift) + Root->sect_addr.off - FmtData.output_offset;
        /* write groups */
        for( group = Groups; group != NULL; group = group->next_group ) {
            class = group->leaders->class;
            if( class->flags & CLASS_COPY ) {
                sect = group->section;
                CurrSect = sect;
                info.offs = GROUP_SECTION_DELTA( group );
#ifdef _INT_DEBUG
                finfo = sect->outfile;
                DEBUG((DBG_LOADDOS, "group %a section %d to %l in %s",
                    &group->grp_addr, sect->ovl_num, info.offs, finfo->fname ));
#endif
                info.lastgrp = NULL;
                RingLookup( class->DupClass->segs->group->leaders, WriteHexCopyGroups, &info);
            } else {
                if( group->size != 0 ) {
                    sect = group->section;
                    CurrSect = sect;
                    info.offs = GROUP_SECTION_DELTA( group );
#ifdef _INT_DEBUG
                    finfo = sect->outfile;
                    DEBUG((DBG_LOADDOS, "group %a section %d to %l in %s",
                        &group->grp_addr, sect->ovl_num, info.offs, finfo->fname ));
#endif
                    Ring2Lookup( group->leaders, DoHexLeader, &info.offs );
                }
            }
        }
    }
    if( buf_offset > 0 ) {          // If partial record in buffer, flush
        WriteHexLine();
    }
    if( FmtData.output_start ) {
        WriteStart();
    }
    WriteLoad( ":00000001ff\r\n", 13 );
    DBIWrite();
}

static unsigned long WriteGroupLoadHex( group_entry *group )
{
    grpwriteinfo    info;
    class_entry     *class;
    unsigned long   file_loc;

    class = group->leaders->class;
    file_loc = PosLoad();
    info.offs = GROUP_SECTION_DELTA( group );
    // If group is a copy group, substitute source group(s) here
    if( class->flags & CLASS_COPY  ) {
        info.lastgrp = NULL; // so it will use the first group
        RingLookup( class->DupClass->segs->group->leaders, WriteHexCopyGroups, &info );
    } else {
        Ring2Lookup( group->leaders, DoHexLeader, &info.offs );
    }
    return( PosLoad() - file_loc );
}

extern void FiniRawLoadFile( void )
/*************************************/
{
    group_entry     *group;
    outfilelist     *fnode;
    unsigned long   file_loc;
    section         *sect;
    outfilelist     *finfo;

    DEBUG(( DBG_BASE, "Writing data" ));
    OrderGroups( CompareOffsets );
    CurrSect = Root;    // needed for WriteInfo.
    fnode = Root->outfile;
    fnode->file_loc = 0;
    Root->u.file_loc = 0;
    Root->sect_addr = Groups->grp_addr;
    if( FmtData.raw_hex_output ) {
        next_addr.off = 0;      // Start at absolute linear address 0
        next_addr.seg = 0;
        linear        = true;   // Linear mode
        buf_offset    = 0;
    }
    for( group = Groups; group != NULL; group = group->next_group ) {
        if( group->size ) {
            sect = group->section;
            CurrSect = sect;
            finfo = sect->outfile;
            file_loc = GROUP_FILE_LOC( group );
            if( FmtData.raw_hex_output ) {
                file_loc += WriteGroupLoadHex( group );
            } else {
                file_loc += WriteGroupLoad( group );
            }
            if( file_loc > finfo->file_loc ) {
                finfo->file_loc = file_loc;
            }
        }
    }
    if( SymFileName != NULL ) {
        DBIWrite();
    }
}
