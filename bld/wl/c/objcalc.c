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


/*
 *  OBJCALC : address calculation for pass 2 of WLINK
 *
 */

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
#include "wlnkmsg.h"
#include "overlays.h"
#include "dbgall.h"
#include "objcalc.h"
#include "ring.h"
#include "mapio.h"
#include "virtmem.h"


static char *   FloatNames[] = { "FIWRQQ", "FIDRQQ", "FIERQQ", "FICRQQ",
    "FISRQQ", "FIARQQ" };

static char *   IgnoreNames[] = { "FJCRQQ", "FJSRQQ", "FJARQQ" };

static unsigned long NumMapSyms;

static seg_leader *     FindASeg( class_entry *, char * );
static void             AllocSeg( seg_leader * );

extern void CheckClassOrder( void )
/*********************************/
/* Reorder the classes if DOSSEG flag set */
{
    SortSegments();
    if( !(LinkState & DOSSEG_FLAG) ) return;
    ReOrderClasses( Root );
}

static void ReOrderAreas( OVL_AREA *ovl )
{
    for( ; ovl != NULL; ovl = ovl->next_area ) {
        ReOrderClasses( ovl->sections );
    }
}

enum class_orders {
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
    ORD_STACK,
    ORD_LAST = ORD_STACK,
    ORD_FIRST = 0
};

static void ReOrderClasses( section *sec )
/****************************************/
// rebuild the class list using the microsoft DOS segment ordering.
// This builds various classes into separate rings, and then joins them
// together.
{
    class_entry *       rings[ORD_LAST+1];
    class_entry *       nextcl;
    class_entry *       currcl;
    class_entry **      owner;
    seg_leader *        currseg;
    seg_leader *        prevseg;
    char *              name;
    int                 i;
    int                 ord;

    while( sec != NULL ) {
        for( i = ORD_FIRST; i <= ORD_LAST; ++i ) {
            rings[i] = NULL;
        }
        currcl = sec->classlist;
        while( currcl != NULL ) {
            nextcl = currcl->next_class;
            currcl->next_class = NULL;
            CheckClassUninitialized( currcl );
            if( FmtData.type & (MK_NOVELL | MK_PHAR_LAP | MK_OS2_LX)
                             && !(currcl->flags & CLASS_32BIT) ) {
                ord = ORD_REALMODE;
            } else {
                name = currcl->name;
                if( currcl->flags & CLASS_CODE ) {
                    if( stricmp( name, OvlMgrClass ) == 0 ) {
                        ord = ORD_OVLMGR;
                    } else if( stricmp( name, BegCodeClassName ) == 0 ) {
                        ord = ORD_BEGCODE;
                    } else {
                        ord = ORD_CODE;
                    }
                } else if( currcl->segs == NULL ||
                       currcl->segs->group == NULL ||
                       currcl->segs->group != DataGroup) {
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
                    } else if( currcl->flags & CLASS_LXDATA_SEEN ) {
                        ord = ORD_INITDGROUP;
                    } else {
                        ord = ORD_UNINITDGROUP;
                    }
                }
            }
            /* add the class to the appropriate ring */
            /* 'rings[ord]' points to the _last_ element of the ring */
            if( rings[ord] == NULL ) {
                currcl->next_class = currcl;
            } else {
                currcl->next_class = rings[ord]->next_class;
                rings[ord]->next_class = currcl;
            }
            rings[ord] = currcl;
            currcl = nextcl;
        }

        /* move the BEGTEXT segment in CODE class to front of segment list */
        currcl = rings[ORD_CODE];
        if( currcl != NULL ) {
            do {
                prevseg = currcl->segs;
                if( prevseg == NULL ) break;
                currseg = prevseg->next_seg;
                for( ;; ) {
                    if( stricmp(currseg->segname, BegTextSegName) == 0 ) {
                        RingPromote( &currcl->segs, currseg, prevseg );
                        break;
                    }
                    if( currseg == currcl->segs ) break;
                    prevseg = currseg;
                    currseg = currseg->next_seg;
                }
                currcl = currcl->next_class;
            } while( currcl != rings[ORD_CODE] );
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
        ReOrderAreas( sec->areas );
        sec = sec->next_sect;
    }
}

static bool CheckLxdataSeen( seg_leader *seg, void *dummy )
/*********************************************************/
{
    dummy = dummy;
    if( seg->info & SEG_LXDATA_SEEN ) {
        seg->class->flags |= CLASS_LXDATA_SEEN;
        return TRUE;
    }
    return FALSE;
}

static void CheckClassUninitialized( class_entry * currcl )
/*********************************************************/
{
    RingLookup( currcl->segs, CheckLxdataSeen, NULL );
}

static void InsertPrevRing( seg_leader **list, seg_leader *curr,
                            seg_leader *prev)
/**************************************************************/
{
    if( prev == NULL ) {
        RingPush( list, curr );
    } else {
        RingInsert( list, curr, prev );
    }
}

static void SortSegments( void )
/******************************/
// reorder segments based on COFF name grouping
{
    class_entry *       currcl;
    seg_leader *        prev;
    seg_leader *        curr;
    seg_leader *        comp;
    seg_leader *        newlist;
    char *              dollarpos;
    size_t              currlen;
    size_t              complen;
    bool                foundgroup;     // so we don't search for OMF only.
    bool                foundmatch;
    bool                added;


    for( currcl = Root->classlist; currcl != NULL; currcl = currcl->next_class){
        foundgroup = FALSE;
        newlist = NULL;
        curr = RingPop( &currcl->segs );
        while( curr != NULL ) {
            dollarpos = strchr( curr->segname, '$' );
            if( dollarpos != NULL ) {
                currlen = dollarpos - curr->segname;
                foundgroup = TRUE;
            } else {
                currlen = strlen( curr->segname );
            }
            added = FALSE;
            if( foundgroup ) {
                prev = NULL;
                comp = RingStep( newlist, NULL );
                foundmatch = FALSE;
                while( comp != NULL ) {
                    complen = strcspn( comp->segname, "$" );
                    if( complen == currlen
                         && memcmp(comp->segname,curr->segname,complen) == 0 ) {
                        foundmatch = TRUE;
                        if( strcmp( comp->segname + complen,
                                    curr->segname + currlen ) > 0 ) {
                            InsertPrevRing( &newlist, curr, prev );
                            added = TRUE;
                            break;
                        }
                    } else if( foundmatch ) {
                        InsertPrevRing( &newlist, curr, prev );
                        added = TRUE;
                        break;
                    }
                    prev = comp;
                    comp = RingStep( newlist, comp );
                }
            }
            if( !added ) {
                RingAppend( &newlist, curr );
            }
            curr = RingPop( &currcl->segs );
        }
        currcl->segs = newlist;
    }
}

#define CODECL_SIZE (sizeof( CodeClassName ) - 1)

extern bool IsCodeClass( char *name, unsigned namelen )
/*****************************************************/
{
    return( namelen >= CODECL_SIZE &&
        memicmp( name+namelen-CODECL_SIZE, CodeClassName, CODECL_SIZE ) == 0 );
}

#define CONSTCL_SIZE (sizeof( ConstClassName ) - 1)

extern bool IsConstClass( char *name, unsigned namelen )
/******************************************************/
{
    return( namelen >= CONSTCL_SIZE &&
        memicmp(name+namelen-CONSTCL_SIZE, ConstClassName, CONSTCL_SIZE ) == 0);
}

#define STACKCL_SIZE (sizeof( StackClassName ) - 1)

extern bool IsStackClass( char *name, unsigned namelen )
/******************************************************/
{
    return( namelen >= STACKCL_SIZE &&
        memicmp(name+namelen-STACKCL_SIZE, StackClassName, STACKCL_SIZE ) == 0);
}

/* -----------------------Allocating Segments-------------------------------- */

static void AddUpSegData( segdata *sdata )
/****************************************/
{
    seg_leader *leader;
    offset      align_size;

    if( sdata->isdead ) return;
    leader = sdata->u.leader;
    align_size = CAlign( leader->size, sdata->align );
    sdata->a.delta = align_size;
    leader->size = align_size + sdata->length;
    if( sdata->align > leader->align ) {
        leader->align = sdata->align;
    }
}

static void CalcSeg( seg_leader *seg )
/************************************/
{
    RingWalk( seg->pieces, AddUpSegData );
}

extern void CalcSegSizes( void )
/******************************/
{
    WalkLeaders( CalcSeg );
}

static bool SetGroupInitSize( segdata *sdata, offset *delta )
/***********************************************************/
{
    if( !sdata->isuninit && sdata->length > 0 && !sdata->isdead ) {
        sdata->u.leader->group->size = *delta + sdata->a.delta + sdata->length;
    }
    return FALSE;
}

static void CalcInitSize( seg_leader *seg )
/*****************************************/
{
    offset      delta;

    if( seg->group != NULL ) {
        delta = GetLeaderDelta( seg );
        RingLookup( seg->pieces, SetGroupInitSize, &delta );
    }
}

extern void CalcAddresses( void )
/*******************************/
/* Calculate the starting address in the file of each segment. */
{
    offset      size;
    group_entry *grp;
    offset      flat;

    DEBUG(( DBG_OLD, "CalcAddresses()" ));
    if( FmtData.base == NO_BASE_SPEC ) {
        if( FmtData.type & MK_PE ) {
            FmtData.base = PE_DEFAULT_BASE;
        } else if( FmtData.type & MK_QNX_FLAT ) {
            FmtData.base = ROUND_UP( StackSize + QNX_DEFAULT_BASE, 4 * 1024 );
        } else if( FmtData.type & MK_OS2_FLAT ) {
            FmtData.base = FLAT_GRANULARITY;
        } else if( FmtData.type & MK_ELF ) {
            FmtData.base = 0x8048000;
        } else {
            FmtData.base = 0;
        }
    }
    DBIPreAddrCalc();
    CurrSect = Root;
    if( FmtData.type & MK_PROT_MODE ) {
        AllocFileSegs();
        if( FmtData.objalign == NO_BASE_SPEC ) {
            if( FmtData.type & MK_PE ) {
                if( !(LinkState & HAVE_I86_CODE) ) {
                    FmtData.objalign = (64*1024UL);
                } else {
                    FmtData.objalign = 4*1024;
                }
            } else if( FmtData.type & MK_QNX ) {
                FmtData.objalign = QNX_GROUP_ALIGN;
            } else if( IS_PPC_OS2 ) {
                // Development temporarly on hold:
                // FmtData.objalign = 1024;
            } else if( FmtData.type & MK_ELF ) {
                FmtData.objalign = 4*1024;
            } else {
                FmtData.objalign = FLAT_GRANULARITY;
            }
        }
        if( FmtData.type & MK_SPLIT_DATA ) {
            FindUninitDataStart();
        }
    }
    StartMemMap();
    AllocClasses( Root->classlist );
    if( FmtData.type & (MK_REAL_MODE|MK_FLAT|MK_ID_SPLIT) ) {
        if( FmtData.type & MK_OVERLAYS ) {
            CalcOvl();
        }
        CalcGrpAddr( Groups );
        CalcGrpAddr( AbsGroups );
    } else if ( FmtData.type & (MK_PE | MK_OS2_FLAT | MK_QNX_FLAT | MK_ELF) ) {
        if( FmtData.type & MK_PE ) {
            flat = GetPEHeaderSize();
        } else if( FmtData.type & MK_ELF ) {
            flat = GetElfHeaderSize();
        } else {
            flat = FmtData.base;
        }
        for( grp = Groups; grp != NULL; grp = grp->next_group ) {
            size = grp->totalsize;
            grp->linear = flat;
            if( FmtData.type & MK_SPLIT_DATA && grp == DataGroup
                                        && FmtData.dgroupsplitseg != NULL ) {
                size -= StackSize;
            }
            flat = ROUND_UP( flat + size, FmtData.objalign );
        }
        if( FmtData.type & (MK_QNX_FLAT|MK_PE) ) {
            ReallocFileSegs();
        }
    } else if( FmtData.type & MK_QNX ) {
        ReallocFileSegs();
    }
    DBIAddrStart();
    DBIAddrOvlStart();
    WalkLeaders( CalcInitSize );
    DefinePublics();
}

static void AllocFileSegs( void )
/*******************************/
{
    group_entry         *currgrp;
    unsigned            seg_num;

    seg_num = 1;
    for( currgrp = Groups; currgrp != NULL; currgrp = currgrp->next_group ){
        if( FmtData.type & MK_FLAT ) {
            currgrp->grp_addr.seg = 1;   // only segment 1 in flat mem.model
        } else if( FmtData.type & MK_ID_SPLIT ) {
            if( currgrp->segflags & SEG_DATA ) {
                currgrp->grp_addr.seg = DATA_SEGMENT;
            } else {
                currgrp->grp_addr.seg = CODE_SEGMENT;
            }
        } else if( FmtData.type & MK_QNX ) {
            currgrp->grp_addr.seg = ToQNXSel( seg_num++ );
        } else if( FmtData.type & MK_PHAR_MULTISEG ) {
            currgrp->grp_addr.seg = (seg_num << 3) | 4;
            seg_num++;
        } else {
            currgrp->grp_addr.seg = seg_num++;
        }
        currgrp->grp_addr.off = 0;
    }
}

static void SetLeaderSeg( seg_leader *seg )
/*****************************************/
{
    if( !(seg->info & SEG_ABSOLUTE) ) {
        seg->seg_addr.seg = seg->group->grp_addr.seg;
    }
}

static void ReallocFileSegs( void )
/*********************************/
/* in QNX & NT we can't have any size 0 physical segments, so after we have
 * calculated the size of everything we have to go back and reallocate the
 * segment numbers so there aren't any "gaps" where the 0 size physical
 * segments (groups) are. */
{
    group_entry *       currgrp;
    class_entry *       class;
    unsigned            seg_num;

    seg_num = 1;
    for( currgrp = Groups; currgrp != NULL; currgrp = currgrp->next_group ){
        if( currgrp->totalsize != 0 ) {
            if( FmtData.type & MK_QNX ) {
                currgrp->grp_addr.seg = ToQNXSel( seg_num++ );
            } else {
                currgrp->grp_addr.seg = seg_num++;
            }
        } else {
            NumGroups--;        /* <-- to make life easier in loadxxx */
        }
    }
    for( class = Root->classlist; class != NULL; class = class->next_class ){
        if( !(class->flags & CLASS_HANDS_OFF) ) {
            RingWalk( class->segs, SetLeaderSeg );
        }
    }
}

static void FindUninitDataStart( void )
/*************************************/
/* for some formats we have to split the uninitialized data off of the rest of
 * DGROUP. So - this finds the start of the uninitialized portion of DGROUP */
{
    class_entry *       class;
    bool                setnext;

    setnext = TRUE;
    FmtData.dgroupsplitseg = NULL;
    FmtData.bsspad = 0;
    if( !(LinkState & DOSSEG_FLAG) ) return;
    class = Root->classlist;
    while( class != NULL ) {
        if( !(class->flags & (CLASS_HANDS_OFF) ) ) {
            if( class->flags & CLASS_LXDATA_SEEN ) {
                setnext = TRUE;
            } else if( setnext ) {
                FmtData.dgroupsplitseg = RingFirst( class->segs );
                setnext = FALSE;
            }
        }
        class = class->next_class;
    }
    if( setnext ) {             //last one was had an LXDATA or no segs.
        FmtData.dgroupsplitseg = NULL;
    }
}

typedef struct  {
    unsigned_32 grp_addr;
    unsigned_32 end_addr;
    group_entry *currgrp;
    bool        first_time;
} grpaddrinfo;


static bool FindEndAddr( seg_leader *seg, grpaddrinfo *info )
/***********************************************************/
{
    unsigned_32 seg_addr;

    if( FmtData.type & MK_REAL_MODE ) {
        seg_addr = MK_REAL_ADDR( seg->seg_addr.seg, seg->seg_addr.off );
    } else {
        seg_addr = seg->seg_addr.off;
    }
    if( info->first_time ) {
        info->currgrp->grp_addr = seg->seg_addr;
        info->grp_addr = seg_addr;
        info->end_addr = seg_addr + seg->size;
        info->first_time = FALSE;
    } else {
        if( info->grp_addr > seg_addr ) {
            info->currgrp->grp_addr = seg->seg_addr;
            info->grp_addr = seg_addr;
        }
        if( info->end_addr < seg_addr + seg->size ) {
            info->end_addr = seg_addr + seg->size;
        }
    }
    return FALSE;
}

static void CalcGrpAddr( group_entry *currgrp )
/*********************************************/
/* Find lowest segment within group (the group's address)
 * not useful for OS/2 16-bit mode. */
{
    grpaddrinfo info;

    while( currgrp != NULL ) {
        info.currgrp = currgrp;
        info.first_time = TRUE;
        Ring2Lookup( currgrp->leaders, FindEndAddr, &info );
        if( FmtData.type & MK_REAL_MODE
                && info.end_addr - info.grp_addr > 64*1024L ) {
            LnkMsg( ERR+MSG_GROUP_TOO_BIG, "sl", currgrp->sym->name,
                                 info.end_addr - info.grp_addr - 64*1024L );
            info.grp_addr = info.end_addr - 64*1024L-1;
        }
        currgrp->totalsize = info.end_addr - info.grp_addr;
        currgrp = currgrp->next_group;
    }
}

extern void AllocClasses( class_entry *class )
/********************************************/
/* Allocate all classes in the list */
{
    targ_addr       save;
    unsigned_32     size;

    while( class != NULL ) {
        DEBUG(( DBG_OLD, "Allocating class %s", class->name ));
        if( class->flags & CLASS_HANDS_OFF ) {
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
            RingWalk( class->segs, AllocSeg );
        }
        class = class->next_class;
    }
}


static void AllocSeg( seg_leader *seg )
/*************************************/
/* Allocate a segment (process all segments in a given class) */
{
    if( !(seg->info & SEG_ABSOLUTE) ) {
        if( IS_DBG_DWARF( seg ) ) {
            CurrLoc.off = 0;
        }
        CurrentSeg = seg;
        NewSegment( seg );
        DEBUG(( DBG_OLD, "- segment %s allocated", seg->segname ));
    }
}

extern offset GetLeaderDelta( seg_leader *leader )
/************************************************/
{
    return SUB_ADDR( leader->seg_addr, leader->group->grp_addr );
}

extern void ConvertToFrame( targ_addr *addr, segment frame )
/**********************************************************/
{
    unsigned long   off;

    if( FmtData.type & MK_REAL_MODE ) {
        off = MK_REAL_ADDR( (int)(addr->seg - frame), addr->off );
        if( off > 0x10000 ) {
            LnkMsg( LOC+ERR+MSG_FRAME_INVALID, "ax", addr, frame );
        }
        addr->off = off;
    }
    addr->seg = frame;
}

/* -------------------------Defining Publics--------------------------------- */


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
        WriteSegs( Root->classlist );                       /* TAI */
        WritePubHead();
    }
    StartMapSort();
    ProcPubs( Root->mods, Root );
    if( FmtData.type & MK_REAL_MODE ) {
        ProcOvlSectPubs( Root );
    }
    ProcPubs( LibModules, Root );
    FinishMapSort();
    if( FmtData.type & MK_OVERLAYS ) {
        ProcOvlPubs();
    }
    if( MapFlags & MAP_FLAG ) {
        if( MapFlags & MAP_VERBOSE ) {
            WriteModSegs();
        }
        StopMapBuffering();
    }
    FindFloatSyms();
}

extern void ProcPubs( mod_entry *head, section *sect )
/****************************************************/
{
    for( CurrMod = head; CurrMod != NULL; CurrMod = CurrMod->n.next_mod ) {
        DoPubs( sect );
    }
}

static int SymAddrCompare( const void *a, const void *b )
/*******************************************************/
{
    symbol * left;
    symbol * right;

    left = *((symbol **) a);
    right = *((symbol **) b);
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

extern void StartMapSort( void )
/******************************/
{
    NumMapSyms = 0;
}

extern void FinishMapSort( void )
/*******************************/
{
    symbol **   symarray;
    symbol **   currsym;
    symbol *    sym;
    bool        ok;

    if( MapFlags & MAP_GLOBAL && NumMapSyms > 0 ) {
        symarray = NULL;
        if( NumMapSyms < (UINT_MAX / sizeof(symbol *)) - 1 ) {
            _LnkAlloc( symarray, NumMapSyms * sizeof(symbol *) );
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
            WriteSymArray( symarray, (unsigned) NumMapSyms );
            _LnkFree( symarray );
        }
    }
}

static void WriteSymArray( symbol ** symarray, unsigned num )
/***********************************************************/
{
    if( MapFlags & MAP_ALPHA ) {
        qsort( symarray, num, sizeof(symbol *), SymAlphaCompare );
    } else {
        qsort( symarray, num, sizeof(symbol *), SymAddrCompare );
    }
    do {
        XReportSymAddr( *symarray );
        symarray++;
        num--;
    } while( num > 0 );
}

typedef struct {
    symbol **   symarray;
    unsigned    num;
    unsigned    first : 1;
} pubdefinfo;

static bool DefPubSym( symbol *pub, pubdefinfo *info )
/****************************************************/
{
    segdata *   seg;
    seg_leader *leader;
    offset      off;
    unsigned_16 frame;
    signed_32   temp;

    if( pub->info & (SYM_DEAD|SYM_IS_ALTDEF) ) return FALSE;
    if( IS_SYM_ALIAS(pub) ) return FALSE;
    if( IS_SYM_IMPORTED(pub) ) return FALSE;
    seg = pub->p.seg;
    if( seg != NULL ) {
        leader = seg->u.leader;
        /* address in symbol table is actually signed_32 offset
           from segdata zero */
        if( seg->isabs || leader->dbgtype != NOT_DEBUGGING_INFO ) {
            XDefSymAddr( pub, pub->addr.off + seg->a.delta
                             + leader->seg_addr.off, leader->seg_addr.seg );
        } else {
            temp = pub->addr.off;
            temp += seg->a.delta;
            temp += SUB_ADDR( leader->seg_addr, leader->group->grp_addr );
            frame = leader->group->grp_addr.seg;
            off = temp + leader->group->grp_addr.off;
            XDefSymAddr( pub, off, frame );
            DBIGenGlobal( pub, leader->class->section );
        }
    }
    if( MapFlags & MAP_FLAG && !SkipSymbol( pub ) ) {
        if( info->first && !(MapFlags & MAP_GLOBAL) ) {
            WritePubModHead();
            info->first = FALSE;
        }
        if( MapFlags & MAP_SORT ) {
            if( MapFlags & MAP_GLOBAL ) {
                NumMapSyms++;
                pub->info |= SYM_MAP_GLOBAL;
            } else {
                info->symarray[info->num] = pub;
                info->num++;
            }
        } else {
            XReportSymAddr( pub );
        }
    }
    return FALSE;
}

extern void DoPubs( section *sect )
/*********************************/
/* Process public definitions for an object file. */
{
    pubdefinfo  info;

    if( CurrMod->modinfo & MOD_NEED_PASS_2 &&
        !(CurrMod->modinfo & MOD_IMPORT_LIB) ) {
        DBIAddModule( CurrMod, sect );
    }
    info.symarray = NULL;
    if( MapFlags & MAP_SORT && !(MapFlags & MAP_GLOBAL)
                            && CurrMod->publist != NULL ) {
        _ChkAlloc( info.symarray,
                        Ring2Count(CurrMod->publist) * sizeof(symbol *) );
    }
    info.num = 0;
    info.first = TRUE;
    Ring2Lookup( CurrMod->publist, DefPubSym, &info );
    if( info.num > 0 ) {
        WriteSymArray( info.symarray, info.num );
    }
    if( info.symarray != NULL ) {
        _LnkFree( info.symarray );
    }
}

static void SetReadOnly( seg_leader *seg )
/****************************************/
{
    if( seg->class->flags & CLASS_READ_ONLY ) {
        seg->segflags |= SEG_READ_ONLY;
    }
}

extern void SetSegFlags( seg_flags * flag_list )
/**********************************************/
{
    seg_flags *     next_one;
    seg_leader *    leader;
    seg_flags *     start;
    class_entry *   class;

    class = Root->classlist;
    while( class != NULL ) {
        RingWalk( class->segs, SetReadOnly );
        class = class->next_class;
    }
    start = flag_list;
    while( flag_list != NULL ) {    // process all class type def'ns first.
        if( flag_list->type == SEGFLAG_CODE || flag_list->type == SEGFLAG_DATA){
            FillTypeFlags( flag_list->flags, flag_list->type );
        }
        flag_list = flag_list->next;
    }
    flag_list = start;
    while( flag_list != NULL ) {    // process all class def'ns second.
        if( flag_list->type == SEGFLAG_CLASS ) {
            FillClassFlags( flag_list->name, flag_list->flags );
        }
        flag_list = flag_list->next;
    }
    flag_list = start;
    while( flag_list != NULL ) {    // now process individual segments
        if( flag_list->type == SEGFLAG_SEGMENT ) {
            leader = FindASeg( Root->classlist, flag_list->name );
            if( leader == NULL ) {
                LnkMsg( WRN + MSG_SEG_NAME_NOT_FOUND, "s", flag_list->name );
            } else {
                leader->segflags = flag_list->flags;
            }
        }
        next_one = flag_list->next;
        _LnkFree( flag_list->name );
        _LnkFree( flag_list );
        flag_list = next_one;
    }
}

static bool SegNameCmp( seg_leader *seg, char *seg_name )
/*******************************************************/
{
    return stricmp( seg->segname, seg_name ) == 0;
}

static seg_leader * FindASeg( class_entry *class, char *seg_name )
/****************************************************************/
{
    seg_leader *seg;

    while( class != NULL ) {
        seg = RingLookup( class->segs, SegNameCmp, seg_name );
        if( seg != NULL ) return seg;
        class = class->next_class;
    }
    return( NULL );
}

static bool SetClassFlag( seg_leader *seg, unsigned_16 *flags )
/*************************************************************/
{
    seg->segflags = *flags;
    return FALSE;
}

static void FillClassFlags( char *name, unsigned_16 flags )
/*********************************************************/
{
    class_entry *   class;

    class = Root->classlist;
    while( class != NULL ) {
        if( stricmp( class->name, name ) == 0 ) {
            RingLookup( class->segs, SetClassFlag, &flags );
            return;
        }
        class = class->next_class;
    }
// if it has made it our here, no class has been found.
    LnkMsg( WRN + MSG_CLASS_NAME_NOT_FOUND, "s", name );
}

static void FillTypeFlags( unsigned_16 flags, segflag_type type )
/***************************************************************/
{
    class_entry *class;
    class_status clflags;

    clflags = 0;
    if( type == SEGFLAG_CODE ) {
        clflags = CLASS_CODE;
    }
    class = Root->classlist;
    while( class != NULL ) {
        if( clflags == (class->flags & CLASS_CODE) ) {
            RingLookup( class->segs, SetClassFlag, &flags );
        }
        class = class->next_class;
    }
}

static void FindFloatSyms( void )
/*******************************/
// this finds the floating point fixup symbols and marks them.
{
    int         index;
    symbol *    sym;

    ClearFloatBits();
    for( index = 0; index < (sizeof( FloatNames ) / sizeof( FloatNames[0] ));
            index++ ) {
        sym = FindISymbol( FloatNames[ index ] );
        if( sym != NULL ) {
            SET_FFIX_VALUE( sym, index + 1 );
        }
    }
    for( index = 0; index < (sizeof( IgnoreNames ) / sizeof( IgnoreNames[0] ));
            index++ ) {
        sym = FindISymbol( IgnoreNames[ index ] );
        if( sym != NULL ) {
            SET_FFIX_VALUE( sym, FFIX_IGNORE );
        }
    }
}
