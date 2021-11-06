/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Address calculation for pass 2 of wlink.
*
****************************************************************************/


#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "linkstd.h"
#include "pcobj.h"
#include "newmem.h"
#include "msg.h"
#include "alloc.h"
#include "specials.h"
#include "loados2.h"
#include "loadpe.h"
#include "loadqnx.h"
#include "loadelf.h"
#include "loadfile.h"
#include "wlnkmsg.h"
#include "overlays.h"
#include "dbgall.h"
#include "objcalc.h"
#include "ring.h"
#include "mapio.h"
#include "virtmem.h"
#include "load16m.h"
#include "objpass1.h"

#include "clibext.h"


typedef enum {
    ORD_REALMODE,
    ORD_BEGCODE,
    ORD_CODE,
    ORD_OVLMGR,
    ORD_OTHER,
    ORD_BEGDGROUP,
    ORD_DATA,
    ORD_INITDGROUP,
    ORD_UNINITDGROUP,
    ORD_BSS,
    ORD_STACK
} class_orders;

#define ORD_FIRST       0
#define ORD_LAST        ORD_STACK


typedef struct  {
    offset      grp_addr;
    offset      end_addr;
    group_entry *currgrp;
    group_entry *lastgrp;       // used only for copy classes
    boolbit     first_time  : 1;
} grpaddrinfo;


typedef struct {
    symbol      **symarray;
    section     *sect;
    size_t      num;
    boolbit     first   : 1;
} pubdefinfo;


static const struct {
    const char      *name;
    fix_fpp_type    win_fpp;
    fix_fpp_type    fpp;
} FloatPatches[] = {
    #define pick_fp(enum,name,alt_name,win,alt_win,others,alt_others) {name, win, others},
    #include "fppatche.h"
    #undef pick_fp
    #define pick_fp(enum,name,alt_name,win,alt_win,others,alt_others) {alt_name, alt_win, alt_others},
    #include "fppatche.h"
    #undef pick_fp
};


static unsigned long NumMapSyms;


static bool CheckLxdataSeen( void *_seg, void *dummy )
/****************************************************/
{
    seg_leader  *seg = _seg;

    /* unused parameters */ (void)dummy;

    if( seg->info & SEG_LXDATA_SEEN ) {
        seg->class->flags |= CLASS_LXDATA_SEEN;
        return( true );
    }
    return( false );
}

static void CheckClassUninitialized( class_entry *class )
/*******************************************************/
{
    RingLookup( class->segs, CheckLxdataSeen, NULL );
}

static void InsertPrevRing( seg_leader **list, seg_leader *curr, seg_leader *prev )
/*********************************************************************************/
{
    if( prev == NULL ) {
        RingPush( list, curr );
    } else {
        RingInsert( list, curr, prev );
    }
}

#define CODECL_SIZE ( sizeof( CodeClassName ) - 1 )

bool IsCodeClass( const char *name, size_t namelen )
/**************************************************/
{
    return( ( namelen >= CODECL_SIZE )
        && ( strnicmp( name + namelen - CODECL_SIZE, CodeClassName, CODECL_SIZE ) == 0 ) );
}

#define CONSTCL_SIZE ( sizeof( ConstClassName ) - 1 )

bool IsConstClass( const char *name, size_t namelen )
/***************************************************/
{
    return( ( namelen >= CONSTCL_SIZE )
        && ( strnicmp( name + namelen - CONSTCL_SIZE, ConstClassName, CONSTCL_SIZE ) == 0 ) );
}

#define STACKCL_SIZE ( sizeof( StackClassName ) - 1 )

bool IsStackClass( const char *name, size_t namelen )
/***************************************************/
{
    return( ( namelen >= STACKCL_SIZE )
        && ( stricmp( name, StackClassName ) == 0 ) );
}

/* -----------------------Allocating Segments-------------------------------- */

static void AddUpSegData( void *_sdata )
/**************************************/
{
    segdata     *sdata = _sdata;
    seg_leader  *leader;

    if( sdata->isdead )
        return;
    leader = sdata->u.leader;
    if( leader->info & SEG_ABSOLUTE ) {
        leader->vsize = sdata->a.delta = 0;
        if( leader->size < sdata->length ) {
            leader->vsize = leader->size = sdata->length;
        }
    } else {
        leader->vsize = sdata->a.delta = CAlign( leader->vsize, sdata->align );
        if( sdata->length > 0 ) {
            leader->vsize = leader->size = sdata->a.delta + sdata->length;
        }
    }
    if( leader->align < sdata->align ) {
        leader->align = sdata->align;
    }
}

static void CalcSeg( seg_leader *seg )
/************************************/
{
    RingWalk( seg->pieces, AddUpSegData );
}

void CalcSegSizes( void )
/******************************/
{
    WalkLeaders( CalcSeg );
}

static bool SetGroupInitSize( void *_sdata, void *_delta )
/********************************************************/
{
    segdata     *sdata = _sdata;
    offset      *delta = _delta;

    if( !sdata->isuninit && ( sdata->length > 0 ) && !sdata->isdead ) {
        sdata->u.leader->group->size = *delta + sdata->a.delta + sdata->length;
    }
    return( false );
}

static void CalcInitSize( seg_leader *seg )
/*****************************************/
{
    offset      delta;

    if( seg->group != NULL ) {
        delta = SEG_GROUP_DELTA( seg );
        RingLookup( seg->pieces, SetGroupInitSize, &delta );
    }
}

static bool setGroupSeg( group_entry *currgrp, unsigned seg_num )
/****************************************************************
 * return false if segment number is fixed
 * return true if segment number should be incremented
 */
{
#ifdef _DOS16M
    if( FmtData.type & MK_DOS16M ) {
        currgrp->grp_addr.seg = ToD16MSel( seg_num );
        return( true );
    }
#endif
#ifdef _NOVELL
    if( FmtData.type & MK_ID_SPLIT ) {
        if( currgrp->segflags & SEG_DATA ) {
            currgrp->grp_addr.seg = DATA_SEGMENT;
        } else {
            currgrp->grp_addr.seg = CODE_SEGMENT;
        }
        return( false );
    }
#endif
#ifdef _QNX
    if( FmtData.type & MK_QNX ) {
        currgrp->grp_addr.seg = ToQNXSel( seg_num );
        return( true );
    }
#endif
#if defined( _PHARLAP ) || defined( _ZDOS ) || defined( _RAW )
    if( FmtData.type & MK_FLAT_OFFS ) {
        currgrp->grp_addr.seg = seg_num;    // only segment 1 in flat mem. model
        return( false );
    }
#endif
#ifdef _PHARLAP
    if( FmtData.type & MK_PHAR_MULTISEG ) {
        currgrp->grp_addr.seg = ( seg_num << 3 ) | 4;
        return( true );
    }
#endif
    currgrp->grp_addr.seg = seg_num;
    return( true );
}

static void AllocFileSegs( void )
/*******************************/
{
    group_entry         *currgrp;
    unsigned            seg_num;

    seg_num = 1;
    for( currgrp = Groups; currgrp != NULL; currgrp = currgrp->next_group ){
        currgrp->grp_addr.off = 0;
        if( setGroupSeg( currgrp, seg_num ) ) {
            seg_num++;
        }
    }
}

static void SetLeaderSeg( void *_seg )
/*****************************************/
{
    seg_leader      *seg = _seg;

    if( (seg->info & SEG_ABSOLUTE) == 0 ) {
        seg->seg_addr.seg = seg->group->grp_addr.seg;
    }
}

static void ReallocFileSegs( void )
/*********************************/
/* In many cases we can't have any size 0 physical segments, so after we have
 * calculated the size of everything we have to go back and reallocate the
 * segment numbers so there aren't any "gaps" where the 0 size physical
 * segments (groups) are. */
{
    group_entry         *currgrp;
    class_entry         *class;
    unsigned            seg_num;

    seg_num = 1;
    for( currgrp = Groups; currgrp != NULL; currgrp = currgrp->next_group ){
        /*
         * segment number is also set for zero length group to be
         * segments in map file sorted properly even if they are not emited
         * into load file
         */
        if( setGroupSeg( currgrp, seg_num ) ) {
            // increment segment if possible for target and group is not zero length
            if( currgrp->totalsize != 0 ) {
                seg_num++;
            }
        }
        if( currgrp->totalsize == 0 ) {
            /* to make life easier in loadxxx remove zero length group from groups count */
            NumGroups--;
        }
    }
    for( class = Root->classlist; class != NULL; class = class->next_class ){
        if( (class->flags & CLASS_DEBUG_INFO) == 0 ) {
            RingWalk( class->segs, SetLeaderSeg );
        }
    }
}

static void FindUninitDataStart( void )
/*************************************/
/* for some formats we have to split the uninitialized data off of the rest of
 * DGROUP. So - this finds the start of the uninitialized portion of DGROUP */
{
    class_entry         *class;
    bool                setnext;

    setnext = true;
    FmtData.dgroupsplitseg = NULL;
    FmtData.bsspad = 0;
    if( (LinkState & LS_DOSSEG_FLAG) == 0 )
        return;
    for( class = Root->classlist; class != NULL; class = class->next_class ) {
        if( (class->flags & CLASS_DEBUG_INFO) == 0 ) {
            if( class->flags & CLASS_LXDATA_SEEN ) {
                setnext = true;
            } else if( setnext ) {
                FmtData.dgroupsplitseg = RingFirst( class->segs );
                setnext = false;
            }
        }
    }
    if( setnext ) {             //last one was had an LXDATA or no segs.
        FmtData.dgroupsplitseg = NULL;
    }
}

static bool FindEndAddr( void *_seg, void *_info )
/**************************************************/
{
    seg_leader  *seg  = _seg;
    grpaddrinfo *info = _info;
    offset      seg_addr;

    if( FmtData.type & MK_REAL_MODE ) {
        seg_addr = MK_REAL_ADDR( seg->seg_addr.seg, seg->seg_addr.off );
    } else {
        seg_addr = seg->seg_addr.off;
    }
    if( info->first_time ) {
        info->currgrp->grp_addr = seg->seg_addr;
        info->grp_addr = seg_addr;
        info->end_addr = seg_addr + seg->size;
        info->first_time = false;
    } else {
        if( info->grp_addr > seg_addr ) {
            info->currgrp->grp_addr = seg->seg_addr;
            info->grp_addr = seg_addr;
        }
        if( info->end_addr < seg_addr + seg->size ) {
            info->end_addr = seg_addr + seg->size;
        }
    }
    return( false );
}

static bool FindInitEndAddr( void *_seg, void *_info )
/******************************************************/
// Only use initialized data segments.  Copy doesn't need uninitialized segments
// This is really only advantageous if uninitialized segments are at the end
{
    seg_leader  *seg  = _seg;
    grpaddrinfo *info = _info;
    offset      seg_addr;

    if( FmtData.type & MK_REAL_MODE ) {
        seg_addr = MK_REAL_ADDR( seg->seg_addr.seg, seg->seg_addr.off );
    } else {
        seg_addr = seg->seg_addr.off;
    }
    if( seg->info & SEG_LXDATA_SEEN ) {
        if( info->first_time ) { // First time, use seg_addr values
            info->grp_addr = seg_addr;
            info->end_addr = seg_addr + seg->size;
            info->first_time = false;
        } else {  // If more segs found, use lowest start address and highest end address;
            if( info->grp_addr > seg_addr ) {
                info->grp_addr = seg_addr;
            }
            if( info->end_addr < seg_addr + seg->size ) {
                info->end_addr = seg_addr + seg->size;
            }
        }
    }
    return( false );
}

static bool FindCopyGroups( void *_seg, void *_info )
/************************************************/
{
    // This is called by the outer level iteration looking for classes
    //  that have more than one group in them
    seg_leader  *seg = _seg;
    grpaddrinfo *info = _info;

    if( info->lastgrp != seg->group ) {   // Only interate new groups
        info->lastgrp = seg->group;
        // Check each initialized segment in group
        Ring2Lookup( seg->group->leaders, FindInitEndAddr, info);
    }
    return( false );
}


static void AllocSeg( void *_seg )
/********************************/
/* Allocate a segment (process all segments in a given class) */
{
    seg_leader  *seg = _seg;

    if( (seg->info & SEG_ABSOLUTE) == 0 ) {
        if( IS_DBG_DWARF( seg ) ) {
            CurrLoc.off = 0;
        }
        CurrentSeg = seg;
        NewSegment( seg );
        DEBUG(( DBG_OLD, "- segment %s allocated", seg->segname.u.ptr ));
    }
}


static void CalcGrpAddr( group_entry *currgrp )
/*********************************************/
/* Find lowest segment within group (the group's address)
 * not useful for OS/2 16-bit mode. */
{
    grpaddrinfo     info;
    seg_leader      *seg;
    class_entry     *class;
    offset          addr;
    targ_addr       save;

    for( ; currgrp != NULL; currgrp = currgrp->next_group ) {
        info.currgrp = currgrp;
        info.first_time = true;
        seg = currgrp->leaders;
        class = seg->class;
        if( class->flags & CLASS_COPY ) {
            currgrp->grp_addr = seg->seg_addr; // Get address of real segment (there's only one)
            // For copy classes must check eash segment to see if it is in a new group
            // this could be the case with FAR_DATA class in large model
            info.lastgrp = NULL; // so it will use the first group
            RingLookup( class->DupClass->segs, FindCopyGroups, &info );
            currgrp->size = info.end_addr - info.grp_addr;
            currgrp->totalsize = currgrp->size;
            // for copy classes put it in class size, also, so map file can find it.
            seg->size = currgrp->totalsize;
            // Now must recompute addresses for all segments in all classes beyond this
            addr = (currgrp->grp_addr.seg << FmtData.SegShift) +
                   currgrp->grp_addr.off + currgrp->totalsize;
            CurrLoc.seg = addr >> FmtData.SegShift;
            CurrLoc.off = addr & FmtData.SegMask;
            while( (class = class->next_class) != NULL ) {
                if( class->flags & CLASS_FIXED ) {
                    save = class->BaseAddr;     // If class is fixed, can stop
                    ChkLocated( &save, true );  //   after making sure address
                    break;                      //   isn't already past here
                }
                if( (class->flags & CLASS_DEBUG_INFO) == 0 ) { // skip Debug classes, they've already been done
                    RingWalk( class->segs, AllocSeg );
                }
            }
        } else {
            Ring2Lookup( seg, FindEndAddr, &info );
            if( (FmtData.type & MK_REAL_MODE) && (seg->info & USE_32) == 0
                && (info.end_addr - info.grp_addr > _64KB) ) {
                LnkMsg( ERR+MSG_GROUP_TOO_BIG, "sl", currgrp->sym->name,
                        info.end_addr - info.grp_addr - _64KB );
            }
            currgrp->totalsize = info.end_addr - info.grp_addr;
        }
    }
}

void AllocClasses( section *sect )
/********************************/
/* Allocate all classes in the list */
{
    targ_addr       save;
    unsigned_32     size;
    class_entry     *class;

    for( class = sect->classlist; class != NULL; class = class->next_class ) {
        DEBUG(( DBG_OLD, "Allocating class %s", class->name.u.ptr ));
        if( class->flags & CLASS_DEBUG_INFO ) {
            /* don't *really* allocate room for these guys */
            save = CurrLoc;
            CurrLoc.off = 0;
            CurrLoc.seg = 0;
            size = CurrSect->size;
            RingWalk( class->segs, AllocSeg );
            DBIDefClass( class, CurrSect->size - size );
            CurrSect->size = size;
            CurrLoc = save;
        } else {
            if( FmtData.type & (MK_PE | MK_QNX_FLAT | MK_OS2_FLAT | MK_ELF) ) {
                // flat addresses
                if( class->flags & CLASS_FIXED ) {
                    class->segs->group->grp_addr.off = class->BaseAddr.off;
                    // Group inherits fixed address from class (only useful if it is first thing in group)
                }
            } else {
                // segmented
                save = class->BaseAddr;
                ChkLocated( &save, ( (class->flags & CLASS_FIXED) != 0 ) );    // Process fixed locations if any
            }
            RingWalk( class->segs, AllocSeg );
        }
    }
}


void ConvertToFrame( targ_addr *addr, segment frame, bool check_16bit )
/*********************************************************************/
{
    unsigned long   off;

    if( FmtData.type & MK_REAL_MODE ) {
        off = MK_REAL_ADDR( (int)( addr->seg - frame ), addr->off );
        if( check_16bit && ( off >= 0x10000 )) {
            LnkMsg( LOC+ERR+MSG_FRAME_INVALID, "Ax", addr, frame );
        }
        addr->off = off;
    }
    addr->seg = frame;
}

/* -------------------------Defining Publics--------------------------------- */


static void FindFloatSyms( void )
/*******************************/
// this finds the floating point patch symbols and marks them.
{
    int         index;
    symbol      *sym;

    for( sym = HeadSym; sym != NULL; sym = sym->link ) {
        SET_SYM_FPP( sym, FPP_NONE );
    }
    for( index = 0; index < ( sizeof( FloatPatches ) / sizeof( FloatPatches[0] ) ); index++ ) {
        if( FloatPatches[index].name != NULL ) {
            sym = FindISymbol( FloatPatches[index].name );
            if( sym != NULL ) {
                if( FmtData.type & MK_WINDOWS ) {
                    SET_SYM_FPP( sym, FloatPatches[index].win_fpp );
                } else {
                    SET_SYM_FPP( sym, FloatPatches[index].fpp );
                }
            }
        }
    }
}


static void DefinePublics( void )
/*******************************/
/* Define public symbols. */

{
    DEBUG(( DBG_OLD, "DefinePublics()" ));
    if( MapFlags & MAP_FLAG ) {
        StartMapBuffering();
        LnkMsg( INF+MSG_CREATE_MAP, NULL );
        WriteGroups();
        WriteMapNL( 1 );
        WriteSegs( Root );                       /* TAI */
        WritePubHead();
    }
    StartMapSort();
    ProcPubs( Root->mods, Root );
#ifdef _EXE
    if( FmtData.type & MK_OVERLAYS ) {
        OvlProcPubsSect( Root );
    }
#endif
    ProcPubs( LibModules, Root );
    FinishMapSort();
#ifdef _EXE
    if( FmtData.type & MK_OVERLAYS ) {
        OvlProcPubs();
    }
#endif
    if( MapFlags & MAP_FLAG ) {
        if( MapFlags & MAP_VERBOSE ) {
            WriteModSegs();
        }
        if( (MapFlags & MAP_LINES) == 0 ) {
            StopMapBuffering();
        }
    }
    FindFloatSyms();
}

void FiniMap( void )
/*******************************/
/* Finish map processing */
{
    if( MapFlags & MAP_FLAG ) {
        if( MapFlags & MAP_LINES ) {
            WriteMapLines();
            StopMapBuffering();
        }
    }
}

void ProcPubs( mod_entry *head, section *sect )
/****************************************************/
{
    for( CurrMod = head; CurrMod != NULL; CurrMod = CurrMod->n.next_mod ) {
        DoPubs( sect );
    }
}

static int SymAddrCompare( const void *a, const void *b )
/*******************************************************/
{
    symbol  *left;
    symbol  *right;

    left = *((symbol **)a);
    right = *((symbol **)b);
    if( left->addr.seg < right->addr.seg ) {
        return( -1 );
    } else if( left->addr.seg > right->addr.seg ) {
        return( 1 );
    } else {
        if( left->addr.off < right->addr.off ) {
            return( -1 );
        } else if( left->addr.off > right->addr.off ) {
            return( 1 );
        }
    }
    return( 0 );
}

void StartMapSort( void )
/******************************/
{
    NumMapSyms = 0;
}

static void WriteSymArray( symbol **symarray, size_t num )
/********************************************************/
{
    if( MapFlags & MAP_ALPHA ) {
        qsort( symarray, num, sizeof( symbol * ), SymAlphaCompare );
    } else {
        qsort( symarray, num, sizeof( symbol * ), SymAddrCompare );
    }
    do {
        XReportSymAddr( *symarray );
        symarray++;
        num--;
    } while( num > 0 );
}

void FinishMapSort( void )
/*******************************/
{
    symbol      **symarray;
    symbol      **currsym;
    symbol      *sym;
    bool        ok;

    if( (MapFlags & MAP_GLOBAL) && ( NumMapSyms > 0 ) ) {
        symarray = NULL;
        if( NumMapSyms < ( UINT_MAX / sizeof( symbol * ) ) - 1 ) {
            _LnkAlloc( symarray, NumMapSyms * sizeof( symbol * ) );
        }
        currsym = symarray;
        ok = symarray != NULL;
        for( sym = HeadSym; sym != NULL; sym = sym->link ) {
            if( sym->info & SYM_MAP_GLOBAL ) {
                sym->info &= ~SYM_MAP_GLOBAL;
                if( ok ) {
                    *currsym = sym;
                    currsym++;
                } else {
                    XReportSymAddr( sym );
                }
            }
        }
        if( !ok ) {
            LnkMsg( WRN+MSG_CANT_SORT_SYMBOLS, NULL );
        } else {
            WriteSymArray( symarray, NumMapSyms );
            _LnkFree( symarray );
        }
    }
}


static bool DefPubSym( void *_pub, void *_info )
/**********************************************/
{
    symbol      *sym = _pub;
    pubdefinfo  *info = _info;
    segdata     *seg;
    seg_leader  *leader;
    offset      temp;

    if( sym->info & (SYM_DEAD | SYM_IS_ALTDEF) )
        return( false );
    if( IS_SYM_ALIAS( sym ) )
        return( false );
    if( IS_SYM_IMPORTED( sym ) )
        return( false );
    seg = sym->p.seg;
    if( seg != NULL ) {
        leader = seg->u.leader;
        /* address in symbol table is actually signed_32 offset
           from segdata zero */
        if( seg->isabs || IS_DBG_INFO( leader ) ) {
            SET_SYM_ADDR( sym, sym->addr.off + seg->a.delta + leader->seg_addr.off, leader->seg_addr.seg );
        } else {
            temp = sym->addr.off;
            temp += seg->a.delta;
            temp += SEG_GROUP_DELTA( leader );
            SET_SYM_ADDR( sym, temp + leader->group->grp_addr.off, leader->group->grp_addr.seg );
            DBIGenGlobal( sym, info->sect );
        }
    }
    if( (MapFlags & MAP_FLAG) && !SkipSymbol( sym ) ) {
        if( info->first && (MapFlags & MAP_GLOBAL) == 0 ) {
            WritePubModHead();
            info->first = false;
        }
        if( MapFlags & MAP_SORT ) {
            if( MapFlags & MAP_GLOBAL ) {
                NumMapSyms++;
                sym->info |= SYM_MAP_GLOBAL;
            } else {
                info->symarray[info->num] = sym;
                info->num++;
            }
        } else {
            XReportSymAddr( sym );
        }
    }
    return( false );
}

void DoPubs( section *sect )
/*********************************/
/* Process public definitions for an object file. */
{
    pubdefinfo  info;

    if( (CurrMod->modinfo & MOD_NEED_PASS_2)
        && (CurrMod->modinfo & MOD_IMPORT_LIB) == 0 ) {
        DBIAddModule( CurrMod, sect );
    }
    info.symarray = NULL;
    if( (MapFlags & MAP_SORT)
        && (MapFlags & MAP_GLOBAL) == 0
        && ( CurrMod->publist != NULL ) ) {
        _ChkAlloc( info.symarray,
                        Ring2Count( CurrMod->publist ) * sizeof( symbol * ) );
    }
    info.num = 0;
    info.first = true;
    info.sect = sect;
    Ring2Lookup( CurrMod->publist, DefPubSym, &info );
    if( info.num > 0 ) {
        WriteSymArray( info.symarray, info.num );
    }
    if( info.symarray != NULL ) {
        _LnkFree( info.symarray );
    }
}

static void SetReadOnly( void *_seg )
/***********************************/
{
    seg_leader      *seg = _seg;

    if( seg->class->flags & CLASS_READ_ONLY ) {
        seg->segflags |= SEG_READ_ONLY;
    }
}

static void setDefBase( void )
/****************************/
{
#ifdef _OS2
    if( FmtData.type & MK_PE ) {
        FmtData.base = PE_DEFAULT_BASE;
        return;
    }
    if( FmtData.type & MK_OS2_FLAT ) {
        FmtData.base = FLAT_GRANULARITY;
        return;
    }
#endif
#ifdef _QNX
    if( FmtData.type & MK_QNX_FLAT ) {
        FmtData.base = ROUND_UP( StackSize + QNX_DEFAULT_BASE, _4KB );
        return;
    }
#endif
#ifdef _ELF
    if( FmtData.type & MK_ELF ) {
        if( LinkState & LS_HAVE_PPC_CODE ) {
            FmtData.base = 0x10000000;
        } else if( LinkState & LS_HAVE_MIPS_CODE ) {
            FmtData.base = 0x00400000;
        } else if( LinkState & LS_HAVE_X64_CODE ) {
            // TODO
            FmtData.base = 0x08048000;
        } else {
            FmtData.base = 0x08048000;
        }
        return;
    }
#endif
    FmtData.base = 0;
}

static void setDefObjAlign( void )
/********************************/
{
#ifdef _OS2
    if( FmtData.type & MK_PE ) {
        if( (LinkState & LS_HAVE_I86_CODE) ) {
            FmtData.objalign = _4KB;
        } else if( (LinkState & LS_HAVE_X64_CODE) ) {
            // TODO
            FmtData.objalign = _64KB;
        } else {
            FmtData.objalign = _64KB;
        }
        return;
    } else if( FmtData.type & MK_WIN_VXD ) {
        FmtData.objalign = _4KB;
        return;
    } else if( FmtData.type & MK_OS2 ) {
#if 0
        if( (LinkState & LS_HAVE_PPC_CODE) ) {
            // Development temporarly on hold:
            FmtData.objalign = _1KB;
            return;
        }
#endif
        FmtData.objalign = FLAT_GRANULARITY;
        return;
    }
#endif
#ifdef _QNX
    if( FmtData.type & MK_QNX ) {
        FmtData.objalign = QNX_GROUP_ALIGN;
        return;
    }
#endif
#ifdef _ELF
    if( FmtData.type & MK_ELF ) {
        FmtData.objalign = _4KB;
        return;
    }
#endif
    FmtData.objalign = _64KB;
}

static offset getFlatOffset( void )
/*********************************/
{
    /****************************************************
     * this must be always first to override target value
     */
#ifdef _RAW
    if( FmtData.output_raw || FmtData.output_hex ) {
        return( 0 );
    }
#endif
    /****************************************************/
#ifdef _OS2
    if( FmtData.type & MK_PE ) {
        return( GetPEHeaderSize() );
    }
#endif
#ifdef _ELF
    if( FmtData.type & MK_ELF ) {
        return( GetElfHeaderSize() );
    }
#endif
    return( FmtData.base );
}

void CalcAddresses( void )
/*******************************/
/* Calculate the starting address in the file of each segment. */
{
    offset          size;
    group_entry     *grp;
    offset          flat;

    DEBUG(( DBG_OLD, "CalcAddresses()" ));
    if( FmtData.base == NO_BASE_SPEC ) {
        setDefBase();
    }
    DBIPreAddrCalc();
    CurrSect = Root;
    if( FmtData.type & MK_PROT_MODE ) {
        AllocFileSegs();
        if( FmtData.objalign == NO_BASE_SPEC ) {
            setDefObjAlign();
        }
        if( FmtData.type & MK_SPLIT_DATA ) {
            FindUninitDataStart();
        }
    }
    StartMemMap();
    AllocClasses( Root );
    if( FmtData.type & (MK_REAL_MODE | MK_FLAT_OFFS | MK_ID_SPLIT) ) {
#ifdef _EXE
        if( FmtData.type & MK_OVERLAYS ) {
            OvlCalc();
        }
#endif
        CalcGrpAddr( Groups );
        CalcGrpAddr( AbsGroups );
#ifdef _DOS16M
    } else if( FmtData.type & MK_DOS16M ) {
        CalcGrpSegs();
#endif
    } else if( FmtData.type & (MK_PE | MK_OS2_FLAT | MK_QNX_FLAT | MK_ELF) ) {
        flat = getFlatOffset();
        for( grp = Groups; grp != NULL; grp = grp->next_group ) {
            size = grp->totalsize;
            if( grp->grp_addr.off > flat + FmtData.base) {
               // ORDER CLNAME name OFFSET option sets grp_addr,
               //   retrieve this information here and wrap into linear address
               flat = grp->grp_addr.off - FmtData.base;
               grp->grp_addr.off = 0;
            }
            grp->linear = flat;
            if(( grp == DataGroup ) && ( FmtData.dgroupsplitseg != NULL )) {
                if( StackSegPtr != NULL ) {
                    size -= StackSize;
                }
            }
            flat = ROUND_UP( flat + size, FmtData.objalign );
        }
        ReallocFileSegs();
    } else if( FmtData.type & (MK_QNX_16 | MK_OS2_16BIT) ) {
        ReallocFileSegs();
    }
    DBIAddrStart();
    WalkLeaders( CalcInitSize );
    DefinePublics();
}

static bool SetClassFlag( void *seg, void *flags )
/************************************************/
{
    ((seg_leader *)seg)->segflags = *(unsigned_16 *)flags;
    return( false );
}

static void FillClassFlags( char *name, unsigned_16 flags )
/*********************************************************/
{
    class_entry     *class;

    for( class = Root->classlist; class != NULL; class = class->next_class ) {
        if( stricmp( class->name.u.ptr, name ) == 0 ) {
            RingLookup( class->segs, SetClassFlag, &flags );
            return;
        }
    }
    // if it has made it our here, no class has been found.
    LnkMsg( WRN + MSG_CLASS_NAME_NOT_FOUND, "s", name );
}

static void FillTypeFlags( unsigned_16 flags, segflag_type type )
/***************************************************************/
{
    class_entry     *class;
    class_status    class_flags;

    class_flags = 0;
    if( type == SEGFLAG_CODE ) {
        class_flags = CLASS_CODE;
    }
    for( class = Root->classlist; class != NULL; class = class->next_class ) {
        if( class_flags == (class->flags & CLASS_CODE) ) {
            RingLookup( class->segs, SetClassFlag, &flags );
        }
    }
}


void SetSegFlags( xxx_seg_flags *flag_list )
/******************************************/
{
    xxx_seg_flags   *next_one;
    seg_leader      *leader;
    xxx_seg_flags   *start;
    class_entry     *class;

    for( class = Root->classlist; class != NULL; class = class->next_class ) {
        RingWalk( class->segs, SetReadOnly );
    }
    start = flag_list;
    // process all class type def'ns first.
    for( ; flag_list != NULL; flag_list = flag_list->next ) {
        if( ( flag_list->type == SEGFLAG_CODE )
          || ( flag_list->type == SEGFLAG_DATA ) ){
            FillTypeFlags( flag_list->flags, flag_list->type );
        }
    }
    // process all class def'ns second.
    for( flag_list = start; flag_list != NULL; flag_list = flag_list->next ) {
        if( flag_list->type == SEGFLAG_CLASS ) {
            FillClassFlags( flag_list->name, flag_list->flags );
        }
    }
    // now process individual segments
    for( flag_list = start; flag_list != NULL; flag_list = next_one ) {
        next_one = flag_list->next;
        if( flag_list->type == SEGFLAG_SEGMENT ) {
            leader = FindSegment( Root, flag_list->name );
            if( leader == NULL ) {
                LnkMsg( WRN + MSG_SEG_NAME_NOT_FOUND, "s", flag_list->name );
            } else {
                leader->segflags = flag_list->flags;
            }
        }
        _LnkFree( flag_list->name );
        _LnkFree( flag_list );
    }
}

static void SplitDGroupClasses( section *sec )
/********************************************/
// split classes in DGROUP
{
    class_entry         *class;
    class_entry         *newclass;
    seg_leader          *curr;
    seg_leader          *newlist;
    seg_leader          *oldlist;

    for( class = sec->classlist; class != NULL; class = class->next_class ){
        newlist = NULL;
        oldlist = NULL;
        while( (curr = RingPop( &class->segs )) != NULL ) {
            if( curr->group == DataGroup ) {
                RingAppend( &newlist, curr );
            } else {
                RingAppend( &oldlist, curr );
            }
        }
        if( oldlist != NULL ) {
            class->segs = oldlist;
            if( newlist != NULL ) {
                newclass = DuplicateClass( class );
                newclass->segs = newlist;
                class->next_class = newclass;
                class = newclass;
                for( curr = NULL; (curr = RingStep( newlist, curr )) != NULL; ) {
                    curr->class = newclass;
                }
            }
        } else if( newlist != NULL ) {
            class->segs = newlist;
        }
    }
}

static void ReOrderClasses( section *sec )
/****************************************/
// rebuild the class list using the Microsoft DOS segment ordering.
// This builds various classes into separate rings, and then joins them
// together.
{
    class_entry         *rings[ORD_LAST + 1];
    class_entry         *nextclass;
    class_entry         *class;
    class_entry         **owner;
    seg_leader          *currseg;
    seg_leader          *prevseg;
    char                *name;
    int                 i;
    int                 ord;

    for( i = ORD_FIRST; i <= ORD_LAST; ++i ) {
        rings[i] = NULL;
    }
    SplitDGroupClasses( sec );
    for( class = sec->classlist; class != NULL; class = nextclass ) {
        nextclass = class->next_class;  // Take class out of original ring
        class->next_class = NULL;
        CheckClassUninitialized( class );
        if( (FmtData.type & (MK_NOVELL | MK_PHAR_LAP | MK_OS2_LX))
            && (class->flags & CLASS_32BIT) == 0 ) {
            ord = ORD_REALMODE;
        } else {
            name = class->name.u.ptr;
            if( class->flags & CLASS_CODE ) {
                if( stricmp( name, OvlMgrClass ) == 0 ) {
                    ord = ORD_OVLMGR;
                } else if( stricmp( name, BegCodeClassName ) == 0 ) {
                    ord = ORD_BEGCODE;
                } else {
                    ord = ORD_CODE;
                }
            } else if( ( class->segs == NULL )
                || ( class->segs->group == NULL )
                || ( class->segs->group != DataGroup ) ) {
                ord = ORD_OTHER;
            } else {     // it's the DGROUP segments.
                if( stricmp( name, BSSClassName ) == 0 ) {
                    ord = ORD_BSS;
                } else if( stricmp( name, StackClassName ) == 0 ) {
                    ord = ORD_STACK;
                } else if( stricmp( name, BegDataClassName ) == 0 ) {
                    ord = ORD_BEGDGROUP;
                } else if( stricmp( name, DataClassName ) == 0 ) {
                    ord = ORD_DATA;
                } else if( class->flags & CLASS_LXDATA_SEEN ) {
                    ord = ORD_INITDGROUP;
                } else {
                    ord = ORD_UNINITDGROUP;
                }
            }
        }
        /* add the class to the appropriate ring */
        /* 'rings[ord]' points to the _last_ element of the ring */
        if( rings[ord] == NULL ) {
            class->next_class = class;
        } else {
            class->next_class = rings[ord]->next_class;
            rings[ord]->next_class = class;
        }
        rings[ord] = class;
    }

    /* move the BEGTEXT segment in CODE class to front of segment list */
    class = rings[ORD_CODE];
    if( class != NULL ) {
        do {
            prevseg = class->segs;
            if( prevseg == NULL )
                break;
            currseg = prevseg->next_seg;
            for( ;; ) {
                if( stricmp( currseg->segname.u.ptr, BegTextSegName ) == 0 ) {
                    RingPromote( &class->segs, currseg, prevseg );
                    break;
                }
                if( currseg == class->segs )
                    break;
                prevseg = currseg;
                currseg = currseg->next_seg;
            }
            class = class->next_class;
        } while( class != rings[ORD_CODE] );
    }

    /* now construct list out of the collected parts. */
    owner = &sec->classlist;
    for( i = ORD_FIRST; i <= ORD_LAST; ++i ) {
        if( rings[i] != NULL ) {
            *owner = rings[i]->next_class;
            owner = &rings[i]->next_class;
        }
    }
    *owner = NULL;
}

static void SortClasses( section *sec )
/****************************************/
// rebuild the class list in the order specified by the ORDER directive
// This builds various classes into separate rings, and then joins them
// together.  Pass class and segment address and output information
// to the class and segment structures.  Sort segments in a class if
// information is provided.
{
    class_entry         *DefaultRing;   // Where to put classes that don't match anything
    class_entry         *nextclass;
    class_entry         *class;
    class_entry         **NewRing;
    class_entry         **owner;
    seg_leader          *currseg;
    seg_leader          *prevseg;
    ORDER_CLASS         *MatchClass;
    ORDER_SEGMENT       *MatchSeg;

    DefaultRing = NULL;

    for( class = sec->classlist; class != NULL; class = nextclass ) {
        nextclass = class->next_class;  // Take class out of original ring
        class->next_class = NULL;
        CheckClassUninitialized( class );
        NewRing = &DefaultRing;  // In case no special class is found
        for( MatchClass = sec->orderlist; MatchClass != NULL; MatchClass = MatchClass->NextClass ) {
            if( stricmp( class->name.u.ptr, MatchClass->Name ) == 0 ) { // search order list for name match
                NewRing = &(MatchClass->Ring);   // if found save ptr to instance
                if( MatchClass->FixedAddr ) {    // and copy any flags or address from it
                    class->flags |= CLASS_FIXED;
                    class->BaseAddr = MatchClass->Base;
                    FmtData.base = 0;  // Otherwise PE will use default and blow up
                }
                if( MatchClass->NoEmit ) {
                    class->flags |= CLASS_NOEMIT;
                }
                break;
            }
        }
        // add the class to front of ring attached to order class, or to default ring
        if( *NewRing == NULL ) {
            class->next_class = class;
        } else {
            class->next_class = (*NewRing)->next_class;
            (*NewRing)->next_class = class;
        }
        *NewRing = class;
    }
    // Now re-arrange segments in any order classes for which we have segments specified
    for( MatchClass = sec->orderlist; MatchClass != NULL; MatchClass = MatchClass->NextClass ) {
        for( MatchSeg = MatchClass->SegList; MatchSeg != NULL; MatchSeg = MatchSeg->NextSeg ) {
            class = MatchClass->Ring;
            do {
                prevseg = class->segs;
                if( prevseg == NULL )
                    break;
                currseg = prevseg->next_seg;

                for( ;; ) {
                    if( stricmp( currseg->segname.u.ptr, MatchSeg->Name ) == 0 ) {
                        if( MatchSeg->FixedAddr ) {    // and copy any flags or address from it
                            currseg->segflags |= SEG_FIXED;
                            currseg->seg_addr = MatchSeg->Base;
                        }
                        if( MatchSeg->NoEmit ) {
                            currseg->segflags |= SEG_NOEMIT;
                        }
                        RingPromote( &class->segs, currseg, prevseg );
                        break;
                    }
                    if( currseg == class->segs ) {
                        break;
                    }
                    prevseg = currseg;
                    currseg = currseg->next_seg;
                }
                class = class->next_class;
            } while( class != MatchClass->Ring );
        }
    }
    /* now construct list out of the collected parts. */
    owner = &sec->classlist;
    for( MatchClass = sec->orderlist; MatchClass != NULL; MatchClass = MatchClass->NextClass ) {
        if( MatchClass->Ring != NULL ) {
            *owner = MatchClass->Ring->next_class;
            owner = &(MatchClass->Ring->next_class);
        }
    }
    if( DefaultRing != NULL ) { // Finish with unmatched ones
        *owner = DefaultRing->next_class;
        owner = &(DefaultRing->next_class);
    }
    *owner = NULL;
    // This has to happen after the class list is rebuilt, so it can be searched
    for( MatchClass = sec->orderlist; MatchClass != NULL; MatchClass = MatchClass->NextClass ) {
         if( MatchClass->Copy && MatchClass->Ring != NULL ) {   // If this is a duplicate destination, find the source
             for( class = sec->classlist; class != NULL; class = class->next_class ) {
                if( stricmp( MatchClass->SrcName, class->name.u.ptr ) == 0 ) {
                    MatchClass->Ring->DupClass = class;
                    MatchClass->Ring->flags |= CLASS_COPY;
                    break;
                }
            }
        }
    }

}

static void SortSegments( void )
/******************************/
// reorder segments based on COFF name grouping
{
    class_entry         *class;
    seg_leader          *prev;
    seg_leader          *curr;
    seg_leader          *comp;
    seg_leader          *newlist;
    char                *dollarpos;
    size_t              currlen;
    size_t              complen;
    bool                foundgroup;     // so we don't search for OMF only.
    bool                foundmatch;
    bool                added;


    for( class = Root->classlist; class != NULL; class = class->next_class ){
        foundgroup = false;
        newlist = NULL;
        for( curr = RingPop( &class->segs ); curr != NULL; curr = RingPop( &class->segs ) ) {
            dollarpos = strchr( curr->segname.u.ptr, '$' );
            if( dollarpos != NULL ) {
                currlen = dollarpos - curr->segname.u.ptr;
                foundgroup = true;
            } else {
                currlen = strlen( curr->segname.u.ptr );
            }
            added = false;
            if( foundgroup ) {
                prev = NULL;
                foundmatch = false;
                for( comp = NULL; (comp = RingStep( newlist, comp )) != NULL; ) {
                    complen = strcspn( comp->segname.u.ptr, "$" );
                    if( ( complen == currlen )
                        && ( memcmp( comp->segname.u.ptr, curr->segname.u.ptr, complen ) == 0 ) ) {
                        foundmatch = true;
                        if( strcmp( comp->segname.u.ptr + complen,
                                    curr->segname.u.ptr + currlen ) > 0 ) {
                            InsertPrevRing( &newlist, curr, prev );
                            added = true;
                            break;
                        }
                    } else if( foundmatch ) {
                        InsertPrevRing( &newlist, curr, prev );
                        added = true;
                        break;
                    }
                    prev = comp;
                }
            }
            if( !added ) {
                RingAppend( &newlist, curr );
            }
        }
        class->segs = newlist;
    }
}

void CheckClassOrder( void )
/*********************************/
/* Reorder the classes if DOSSEG flag set or ORDER directive given */
{
    SortSegments();
    if( LinkState & LS_SPEC_ORDER_FLAG ) {
       WalkAllSects( SortClasses );
    } else if( LinkState & LS_DOSSEG_FLAG ) {
       WalkAllSects( ReOrderClasses );
    }
}
