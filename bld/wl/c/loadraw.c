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


static bool WriteBinSegGroup( group_entry *group )
/*********************************************/
/* write the data for group to the loadfile */
/* returns TRUE if the file should be repositioned */
{
    unsigned long       loc;
    section             *sect;
    bool                repos;
    outfilelist         *finfo;

    repos = FALSE;
    if( group->size != 0 || group->leaders->class->flags & CLASS_COPY ) {
        sect = group->section;
        CurrSect = sect;
        finfo = sect->outfile;
        loc = SUB_ADDR( group->grp_addr, sect->sect_addr ) + sect->u.file_loc;
        if ( (long)loc >= 0) {  // Offset may make this go negative for NOEMIT classes or segments
            if( loc > finfo->file_loc ) {
                PadLoad( loc - finfo->file_loc );
            } else if( loc != finfo->file_loc ) {
                SeekLoad( loc );
                repos = TRUE;
            }
            DEBUG((DBG_LOADDOS, "group %a section %d to %l in %s",
                &group->grp_addr, sect->ovl_num, loc, finfo->fname ));
            loc += WriteDOSGroupLoad( group, repos );
            if( loc > finfo->file_loc ) {
                finfo->file_loc = loc;
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
                diff = (FmtData.base + group->grp_addr.off + group->linear
                       - FmtData.output_offset) - PosLoad();
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
        Root->u.file_loc = (Root->sect_addr.seg << FmtData.SegShift) + Root->sect_addr.off - FmtData.output_offset;
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
static unsigned_32      nextAddr;
static unsigned_16      seg;
static bool             linear;
static char             lineBuf[HEXLEN];
static unsigned_32      bufOfs;

static void WriteHexLine( void )
/******************************/
{

    char            str_buf[2 * HEXLEN + 15];
    unsigned_16      checksum;
    unsigned_16      i;
    unsigned_16      seg_shift = FmtData.output_hshift ? FmtData.HexSegShift : FmtData.SegShift;
    unsigned_16      offset;

    if( nextAddr + bufOfs >= (0x10000L << seg_shift) && !linear ) {
        linear = 1;
        seg = 0;
    }

    if( !linear ) {
        if( nextAddr - (seg << seg_shift) + bufOfs > 0x10000L ) { // See if we need to output a segment record
            seg = (unsigned int)(nextAddr >> seg_shift);
            sprintf( str_buf, ":02000002%04x%02x\r\n", seg, (-(4 + (seg >> 8) + seg & 0xFF)) & 0xFF );
            WriteLoad( str_buf, 17 );
        }
        offset = (unsigned int)(nextAddr - (seg << seg_shift));
    } else {
        if( nextAddr - (seg << 16) + bufOfs > 0x10000L ) {  // See if we need to output
            seg = (unsigned int)(nextAddr >> 16);           //   an extended linear record
            sprintf( str_buf, ":02000004%04x%02x\r\n", seg, (-(6 + (seg >> 8) + seg & 0xFF)) & 0xFF );
            WriteLoad( str_buf, 17);
        }
        offset = (unsigned int)(nextAddr - (seg << 16));
    }

    sprintf( str_buf, ":%02x%04x00", bufOfs, offset );      // Intel Hex header
    checksum = bufOfs + (offset >> 8) + (offset & 0xFF);    // Start checksum using above elements

    i = 0;
    while( bufOfs ) { // Do data bytes, leaving BufOfs 0 at end
        sprintf( str_buf + 9 + 2 * i, "%02x", lineBuf[i] );
        checksum += lineBuf[i++];
        --bufOfs;
    }
    sprintf( str_buf + 9 + 2 * i, "%02x\r\n", (-checksum) & 0xFF );
    WriteLoad( str_buf, 13 + 2 * i );
}

static bool WriteHexData( void *_sdata, void *_addr )
/***************************************************/
{
    segdata         *sdata = _sdata;
    unsigned_32      pieceAddr = *(unsigned long *)_addr + sdata->a.delta;
    unsigned_16      len = sdata->length;
    unsigned_16      offset;
    unsigned_16      piece;

    if( !sdata->isuninit && !sdata->isdead && len > 0 ) {
        if( pieceAddr != nextAddr + bufOfs ) { // Must start new record if address not contiguous
            if( pieceAddr < nextAddr + bufOfs ) {
                LnkMsg( ERR + MSG_FIXED_LOC_BEFORE_CUR_LOC, "a", (targ_addr*)_addr);
            }
            if( bufOfs ) {              // If partial record in buffer, flush
                WriteHexLine();
            }
            nextAddr = pieceAddr;
        }
        offset = 0;
        while( len ) {                  // Now lob out records
            piece = HEXLEN - bufOfs;    // Handle partially filled buffer
            if( piece > len ) {         // Handle not enough to fill buffer
                piece = len;
            }
            ReadInfo( sdata->u1.vm_ptr + offset, lineBuf + bufOfs, piece );
            bufOfs += piece;
            if( bufOfs == HEXLEN ) {
                WriteHexLine();         // Only write full buffers
                nextAddr += HEXLEN;     // NextAddr reflects start of line
            }                           // Partial records will be written later
            offset += piece;            //   if address is not contiguous
            len -= piece;
        }
    }
    return( FALSE );
}

static bool DoHexLeader( void *seg, void *addr )
/**********************************************/
{
    if ( !(((seg_leader *)seg)->class->flags & CLASS_NOEMIT ||
           ((seg_leader *)seg)->segflags & SEG_NOEMIT) ) {
        unsigned long   segaddr = *(unsigned long *)addr + GetLeaderDelta( seg );

        RingLookup( ((seg_leader *)seg)->pieces, WriteHexData, &segaddr );
    }
    return( FALSE );
}

static bool DoHexDupLeader( void *seg, void *addr )
/************************************************/
{
    unsigned_32  segaddr = *(unsigned long *)addr + GetLeaderDelta( seg );

    RingLookup( ((seg_leader *)seg)->pieces, WriteHexData, &segaddr );
    return( FALSE );
}

void WriteStart( void )
/*********************/
{
    char    str_buf[22];

    if( StartInfo.addr.off > 0xffff ) {
        sprintf( str_buf, ":04000005%08lx%02x\r\n", (unsigned long)StartInfo.addr.off,
                (-(9 + (StartInfo.addr.off >> 24) + ((StartInfo.addr.off >> 16) & 0xFF) +
                ((StartInfo.addr.off >> 8) & 0xFF) + (StartInfo.addr.off & 0xFF) )) & 0xFF );
    } else {
        sprintf( str_buf, ":04000003%04x%04x%02x\r\n", (unsigned)StartInfo.addr.seg, (unsigned)StartInfo.addr.off,
                (-(7 + (StartInfo.addr.seg >> 8) + (StartInfo.addr.seg & 0xFF) +
                (StartInfo.addr.off >> 8) + (StartInfo.addr.off & 0xFF) )) & 0xFF );
    }
    WriteLoad( str_buf, 21 );
}

typedef struct  {
    unsigned_32 addr;
    group_entry *lastgrp;  // used only for copy classes
} grpwriteinfo;

static bool WriteHexCopyGroups( void *_seg, void *_info )
/************************************************/
{
    // This is called by the outer level iteration looking for classes
    //  that have more than one group in them
    seg_leader * seg = _seg;
    grpwriteinfo *info = _info;

    if( info->lastgrp != seg->group ) {   // Only interate new groups
        info->lastgrp = seg->group;
        // Check each initialized segment in group
        Ring2Lookup( seg->group->leaders, DoHexDupLeader, &info->addr);
        info->addr += seg->group->totalsize;
    }
    return( FALSE );
}


void HexOutput( void )
/***************************/
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

    nextAddr = 0L;  // Start at absolute linear address 0
    linear   = 0;   //       in segmented mode
    seg      = 0;
    bufOfs   = 0;

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
                info.addr = (group->grp_addr.off + group->linear - FmtData.output_offset);
                sect = wrkgrp->section;
                CurrSect = sect;
#ifdef _INT_DEBUG
                finfo = sect->outfile;
#endif
                DEBUG((DBG_LOADDOS, "group %a section %d to %l in %s",
                    &group->grp_addr, sect->ovl_num, info.addr, finfo->fname ));
                if( group->leaders->class->flags & CLASS_COPY ) {
                    Ring2Lookup( wrkgrp->leaders, DoHexDupLeader, &info.addr );
                } else {
                    Ring2Lookup( wrkgrp->leaders, DoHexLeader, &info.addr );
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
#ifdef _INT_DEBUG
                finfo = sect->outfile;
#endif
                info.addr = SUB_ADDR( group->grp_addr, sect->sect_addr ) + sect->u.file_loc;
                DEBUG((DBG_LOADDOS, "group %a section %d to %l in %s",
                    &group->grp_addr, sect->ovl_num, info.addr, finfo->fname ));
                info.lastgrp = NULL;
                RingLookup( class->DupClass->segs->group->leaders, WriteHexCopyGroups, &info);
            } else {
                if( group->size != 0 ) {
                    sect = group->section;
                    CurrSect = sect;
#ifdef _INT_DEBUG
                    finfo = sect->outfile;
#endif
                    info.addr = SUB_ADDR( group->grp_addr, sect->sect_addr ) + sect->u.file_loc;
                    DEBUG((DBG_LOADDOS, "group %a section %d to %l in %s",
                        &group->grp_addr, sect->ovl_num, info.addr, finfo->fname ));
                    Ring2Lookup( group->leaders, DoHexLeader, &info.addr );
               }
            }
        }
    }
    if( bufOfs ) {              // If partial record in buffer, flush
        WriteHexLine();
    }
    if( FmtData.output_start ) {
       WriteStart();
    }
    WriteLoad( ":00000001ff\r\n", 13 );
    DBIWrite();
}

unsigned long WriteGroupLoadHex( group_entry *group )
{
    grpwriteinfo    info;
    class_entry     *class;

    class = group->leaders->class;
    info.addr = PosLoad();
    // If group is a copy group, substitute source group(s) here
    if( class->flags & CLASS_COPY  ) {
        info.lastgrp = NULL; // so it will use the first group
        RingLookup( class->DupClass->segs->group->leaders, WriteHexCopyGroups, &info );
    } else {
        Ring2Lookup( group->leaders, DoHexLeader, &(info.addr) );
    }
    return( PosLoad() - info.addr );
}

extern void FiniRawLoadFile( void )
/*************************************/
{
    group_entry     *group;
    outfilelist     *fnode;
    unsigned long   loc;
    section         *sect;
    outfilelist     *finfo;

    DEBUG(( DBG_BASE, "Writing data" ));
    OrderGroups( CompareOffsets );
    CurrSect = Root;    // needed for WriteInfo.
    fnode = Root->outfile;
    fnode->file_loc = 0;
    Root->u.file_loc = 0;
    Root->sect_addr = Groups->grp_addr;
    group = Groups;
    if( FmtData.raw_hex_output ) {
        nextAddr = 0;   // Start at absolute linear address 0
        linear   = 1;   // Linear mode
        seg      = 0;
        bufOfs   = 0;
    }
    while( group != NULL ) {
        if( group->size ) {
            sect = group->section;
            CurrSect = sect;
            finfo = sect->outfile;
            loc = SUB_ADDR( group->grp_addr, sect->sect_addr ) + sect->u.file_loc;
            if( FmtData.raw_hex_output )
                loc += WriteGroupLoadHex( group );
            else
                loc += WriteGroupLoad( group );
            if( loc > finfo->file_loc )
                finfo->file_loc = loc;
        }
        group = group->next_group;
    }
    if( SymFileName != NULL )
        DBIWrite();
}
