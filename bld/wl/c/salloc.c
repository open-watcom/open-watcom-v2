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


#include "linkstd.h"
#include "pcobj.h"
#include "msg.h"
#include "wlnkmsg.h"
#include "loadpe.h"
#include "ring.h"
#include "objcalc.h"

static targ_addr    DataLoc;
static targ_addr    CodeLoc;

extern void ResetAddr( void )
/***************************/
{
    CurrLoc.seg = 0;
    CurrLoc.off = 0;
    CurrentSeg = NULL;
}

extern void NormalizeAddr( void )
/*******************************/
{
    unsigned_32 new_seg;

    DbgAssert( !(FmtData.type & MK_PROT_MODE) );

    if( CurrSect == NonSect || !FmtData.u.dos.ovl_short ) {
        new_seg = (CurrLoc.off >> 4) + CurrLoc.seg;
        if( new_seg > 0xFFFF ) {
            LnkMsg( ERR+MSG_APP_TOO_BIG_FOR_DOS, NULL );
        }
        CurrLoc.seg = new_seg;
        CurrLoc.off &= 0x0f;
    }
}


static offset BumpUp( offset ptr, offset size )
/*********************************************/
{
    ptr += size;
    if( CurrentSeg != NULL && !(CurrentSeg->info & USE_32) && ptr > 0x10000 ) {
        LnkMsg( ERR+MSG_SEG_TOO_BIG, "sl", CurrentSeg->segname,
                (unsigned long)(ptr-0x10000) );
    }
    return( ptr );
}


extern void AddSize( offset size )
/********************************/
{
    CurrSect->size += size;
    CurrLoc.off = BumpUp( CurrLoc.off, size );
}

extern offset CAlign( offset off, unsigned align )
/************************************************/
/* this aligns to 2^align */
{
    unsigned      part;

    if( align > 0 ) {
        align = 1 << align;
        part = off & (align - 1);
        if( part != 0 ) {
            off = BumpUp( off, align - part );
        }
    }
    return( off );
}

extern void Align( byte align )
/*****************************/
{
    offset  off;

    off = CAlign( CurrLoc.off, align );
    AddSize( off - CurrLoc.off );
}

extern void MAlign( byte align )
/******************************/
{
    CurrLoc.off = CAlign( CurrLoc.off, align );
}

extern void StartMemMap( void )
/*****************************/
{
    if( FmtData.type & MK_ID_SPLIT ) {
        CodeLoc.seg = CODE_SEGMENT;
        CodeLoc.off = 0;
        DataLoc.seg = DATA_SEGMENT;
        DataLoc.off = 0;
    } else {
        CurrLoc.seg = (FmtData.type & MK_PROT_MODE) ? 1 : 0;
        CurrLoc.off = 0;
        if( FmtData.type & MK_FLAT ) {
            CurrLoc.off = FmtData.base;
        }
    }
}

static targ_addr * GetIDLoc( group_entry *group )
/***********************************************/
/* return a pointer to the current address for ID split format */
{
    targ_addr * retval;

    if( group->segflags & SEG_DATA ) {
        retval = &DataLoc;
    } else {
        retval = &CodeLoc;
    }
    return retval;
}

extern void NewSegment( seg_leader *seg )
/***************************************/
{
    group_entry *group;
    targ_addr * loc;
    offset      off;
    bool        auto_group;

    group = seg->group;
    if( seg->dbgtype != NOT_DEBUGGING_INFO ) {
        CurrentSeg = NULL;
        Align( seg->align );
        seg->seg_addr = CurrLoc;
        AddSize( seg->size );
    } else if( FmtData.type & MK_REAL_MODE ) {
        if( group->isautogrp && Ring2First(group->leaders) != seg ) {
            auto_group = TRUE; /* auto-group segment, but not first one */
        } else {
            auto_group = FALSE;
        }
        if( !auto_group ) {
            NormalizeAddr();    /*  to prevent overflows on the alignment */
        }
        off = CAlign( CurrLoc.off, seg->align );
        group->totalsize += off - CurrLoc.off;
        AddSize( off - CurrLoc.off );
        if( !auto_group ) {
            NormalizeAddr();    /*  to normalize address of segment */
        }
        seg->seg_addr = CurrLoc;
        AddSize( seg->size );
        group->totalsize += seg->size;
    } else if( FmtData.type & (MK_FLAT | MK_ID_SPLIT) ) {
        if( FmtData.type & MK_ID_SPLIT ) {
            loc = GetIDLoc( group );
            CurrLoc = *loc;
        }
        off = CAlign( CurrLoc.off, seg->align );
        group->totalsize += off - CurrLoc.off;
        AddSize( off - CurrLoc.off );
        seg->seg_addr = CurrLoc;
        AddSize( seg->size );
        group->totalsize += seg->size;
        if( FmtData.type & MK_ID_SPLIT ) {
            loc = GetIDLoc( group );
            *loc = CurrLoc;
        }
    } else {
        CurrLoc.seg = group->grp_addr.seg;
        seg->seg_addr.seg = CurrLoc.seg;
        CurrLoc.off = group->totalsize;
        if( FmtData.type & MK_SPLIT_DATA && seg == FmtData.dgroupsplitseg ){
            FmtData.bsspad = ROUND_UP(CurrLoc.off, FmtData.objalign)
                                        - CurrLoc.off;
            AddSize( FmtData.bsspad + PE_BSS_SHIFT );
        }
        if( seg->size == 0  && group->isautogrp ) {
            seg->seg_addr.off = CurrLoc.off;
        } else {
            Align( seg->align );
            seg->seg_addr.off = CurrLoc.off;
            AddSize( seg->size );
            group->totalsize = CurrLoc.off;
        }
    }
}
