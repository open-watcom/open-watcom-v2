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
* Description: COMDEF -- support for Microsoft COMDEFs
*
****************************************************************************/

#include <string.h>
#include <stdlib.h>
#include "linkstd.h"
#include "virtmem.h"
#include "pcobj.h"
#include "alloc.h"
#include "specials.h"
#include "msg.h"
#include "wlnkmsg.h"
#include "objnode.h"
#include "objcalc.h"
#include "objpass1.h"
#include "objomf.h"
#include "overlays.h"
#include "objio.h"
#include "dbgall.h"
#include "objstrip.h"
#include "objcache.h"
#include "omfreloc.h"
#include "ring.h"
#include "wcomdef.h"

/* these are handy constants used when processing comdat records */
#define CDAT_ALLOC_MASK         0xF
#define CDAT_CONTINUATION       0x1
#define CDAT_ITERATED           0x2
#define CDAT_STATIC             0x4
#define CDAT_SELECT_MASK        0xF0
#define CDAT_SELECT_SHIFT       12

#if 0
static char *   CDatClassNames[] = {
        CodeClassName,
        FarDataClassName,
        CodeClassName,
        DataClassName
};

static seg_leader *     CDatSegments[4];
static unsigned         CDatSegNum;     /* for making the comdat seg. name */

/* keep a base-36 number after the COMDAT_SEG to differentiate the segments*/
/* 32-bit unsigned int gives a maximum of 7 digits in base 36. */
#define CDAT_SEG_NAME "COMDAT_SEG\0\0\0\0\0\0\0\0"
static char     CDatSegName[] = CDAT_SEG_NAME;

#define CDAT_SEG_NAME_LEN sizeof(CDatSegName)
#define CDAT_SEG_NUM_OFF  10
#endif

static comdat_info *    CDatList;
static comdat_info *    FreedInfos;
static comdat_piece *   FreedPieces;


void ResetComdef( void )
/**********************/
{
#if 0
    memcpy( CDatSegName, CDAT_SEG_NAME, CDAT_SEG_NAME_LEN );
    CDatSegments[0] = NULL;
    CDatSegments[1] = NULL;
    CDatSegments[2] = NULL;
    CDatSegments[3] = NULL;
    CDatSegNum = 0;
#endif
    CDatList = NULL;
    FreedInfos = NULL;
    FreedPieces = NULL;
}

static comdat_info * AllocCDatInfo( void )
/****************************************/
{
    comdat_info *       info;

    if( FreedInfos != NULL ) {
        info = FreedInfos;
        FreedInfos = info->next;
    } else {
        _Pass1Alloc( info, sizeof(comdat_info) );
    }
    info->pieces = NULL;
    return( info );
}

static void FreeCDatInfo( comdat_info * info )
/********************************************/
{
    info->next = FreedInfos;
    FreedInfos = info;
}

static comdat_piece * AllocCDatPiece( void )
/******************************************/
{
    comdat_piece *      piece;

    if( FreedPieces != NULL ) {
        piece = FreedPieces;
        FreedPieces = piece->next;
    } else {
        _Pass1Alloc( piece, sizeof(comdat_piece) );
    }
    memset( piece, 0, sizeof(comdat_piece) );
    return( piece );
}

static void FreeCDatPiece( void * piec )
/**************************************/
{
    comdat_piece *piece = piec;
    if( piece->free_data ) {
        _LnkFree( piece->data );
    }
    piece->next = FreedPieces;
    FreedPieces = piece;
}

static unsigned_32 GetLeaf( void )
/********************************/
{
    unsigned char       leaf;
    unsigned_32         value;

    value = 0;
    leaf = *ObjBuff++;
    if( leaf <= COMDEF_LEAF_SIZE ) {
        value = leaf;
    } else if( leaf == COMDEF_LEAF_2 ) {
        value = GET_U16_UN(ObjBuff);
        ObjBuff += sizeof( unsigned_16 );
    } else if( leaf == COMDEF_LEAF_3 ) {
        value = GET_U16_UN(ObjBuff);
        ObjBuff += sizeof( unsigned_16 );
        value += ( (unsigned_32)*ObjBuff++ ) << 16;
    } else if( leaf == COMDEF_LEAF_4 ) {
        value = GET_U32_UN(ObjBuff);
        ObjBuff += sizeof( unsigned_32 );
    }
    return( value );
}

static bool isCOMDEF32( void )
/****************************/
{
    SEGDATA *segs = CurrMod->segs;
    SEGDATA *seg = NULL;

    for(;;) {
        seg = Ring2Step( segs, seg );
        if( seg == NULL ) break;
        // none of these are generated for Dwarf debug info so
        // we should not get confused when we are 16-bit
        if( seg->isuninit || seg->iscdat || seg->iscode ) {
            return( seg->is32bit != 0 );
        }
    }
    return( (ObjFormat & FMT_32BIT_REC) != 0 );
}

void ProcComdef( bool isstatic )
/*************************************/
{
    bool                is32bit = isCOMDEF32();
    char *              sym_name;
    unsigned_8          sym_len;
    byte                kind;
    unsigned_32         size;
    symbol *            sym;
    extnode *           ext;
    sym_flags           flags;

    flags = ST_CREATE | ST_REFERENCE;
    if( isstatic ) {
        flags |= ST_STATIC;
    }
    while( ObjBuff < EOObjRec ) {
        sym_len = *ObjBuff++;
        sym_name = (char *)ObjBuff;
        ObjBuff += sym_len;
        SkipIdx();
        kind = *ObjBuff++;
        size = GetLeaf();
        if( kind == COMDEF_FAR ) {
            size *= GetLeaf();
        }
        sym = SymOp( flags, sym_name, sym_len );
        sym = MakeCommunalSym( sym, size, kind == COMDEF_FAR, is32bit );
        ext = AllocNode( ExtNodes );
        ext->entry = sym;
        ext->ovlref = 0;
    }
}

void ProcLinsym( void )
/****************************/
{
    list_of_names *     symname;
    symbol *            sym;
    bool                is32bit;
    unsigned            sym_len;

    ObjBuff++;          /* skip flags */
    symname = FindName( GetIdx() );
    sym_len = strlen( symname->name );
    sym = SymOp( ST_FIND | ST_STATIC, symname->name, sym_len );
    if( sym == NULL ) sym = SymOp( ST_FIND, symname->name, sym_len );
    if( sym == NULL ) {
        BadObject();
        return;
    }
    if( !IS_SYM_COMDAT(sym) ) return;
    is32bit = (ObjFormat & FMT_32BIT_REC) != 0;
    if( sym->mod == CurrMod && (sym->info & SYM_DEAD) == 0 ) {
        DBIAddLines( sym->p.seg, ObjBuff, EOObjRec - ObjBuff, is32bit );
    }
}

static void DoAltDef( comdat_info *info )
/***************************************/
{
    if( (LinkFlags & INC_LINK_FLAG) == 0 ) {
        FreeSegData( info->sdata );
    } else {
        Ring2Append( &CurrMod->segs, info->sdata );
        info->sdata->isdead = true;
        InfoCDatAltDef( info );
    }
}

static void FinishComdat( void *inf )
/***********************************/
{
    comdat_info *info = inf;
    if( info->flags & SYM_DEAD ) {      // is redefined
        DoAltDef( info );
    } else {
        StoreInfoData( info );
    }
    RingWalk( info->pieces, FreeCDatPiece );
    FreeCDatInfo( info );
}

void ResolveComdats( void )
/********************************/
{
    RingWalk( CDatList, FinishComdat );
    CDatList = NULL;
}

#if 0
static char * GetNewName( void )
/******************************/
/* get a new name for an automatically defined comdat segment */
{
    CDatSegNum++;
    ultoa( CDatSegNum, CDatSegName + CDAT_SEG_NUM_OFF, 36 );
    return CDatSegName;
}

static void AddToLinkerComdat( symbol *sym )
/******************************************/
/* this tries to add "size" bytes to an existing comdat segment, and if it
 * can't, it creates a new segment for the data */
{
    seg_leader *        leader;
    class_entry *       class;
    offset              seglen;
    section *           sect;
    segdata *           sdata;
    byte                alloc;
    byte                align;

    sdata = sym->p.seg;
    alloc = sdata->alloc;
    align = sdata->align;
    leader = CDatSegments[alloc];
    if( leader == NULL ) {
        sdata->u.name = GetNewName();
        sym->addr.off = 0;
        if( (FmtData.type & MK_OVERLAYS) == 0 || (alloc & 1) == 0 ) {
            sect = Root;
        } else {
            sect = NonSect;             /* data in an overlaid app */
        }
        class = FindClass( sect, CDatClassNames[alloc], alloc > 1, alloc & 1 );
    } else {
        class = leader->class;
        seglen = CAlign( leader->size, align ) + sdata->length;
        if( seglen > 0xFFFF && align <= 1 ) {
            sdata->u.name = GetNewName();
            sym->addr.off = 0;
        } else {
            sdata->u.name = leader->segname;
            sym->addr.off = seglen - sdata->length;
        }
    }
    AddSegment( sdata, class );
    sdata->u1.vm_ptr = AllocStg( sdata->length );
    CDatSegments[alloc] = sdata->u.leader;
    sym->p.seg = sdata;
}
#endif

static offset CountIDBlock( unsigned_8 **buffptr )
/************************************************/
/* this finds the length of an individual lidata block */
{
    offset      repeat;
    offset      size;
    unsigned_8  *buff;
    unsigned_16 count;

    buff = *buffptr;
    if( ObjFormat & FMT_MS_386 ) {
        _TargU32toHost( _GetU32( buff ), repeat );
        buff += sizeof( unsigned_32 );
    } else {
        _TargU16toHost( _GetU16( buff ), repeat );
        buff += sizeof( unsigned_16 );
    }
    _TargU16toHost( _GetU16( buff ), count );
    buff += sizeof(unsigned_16);
    if( count == 0 ) {  // this is followed by actual data
        repeat *= *buff;
        buff += *buff + 1;
    } else {            // it is a series of id blocks
        size = 0;
        while( count-- > 0 ) {
            size += CountIDBlock( &buff );
        }
        repeat *= size;
    }
    *buffptr = buff;
    return( repeat );
}

static offset CalcLIDataLength( void )
/************************************/
/* this finds the length of the equivalent of an lidata record */
{
    offset      total;
    unsigned_8  *buff;

    total = 0;
    buff = ObjBuff;
    while( buff < EOObjRec ) {
        total += CountIDBlock( &buff );
    }
    return( total );
}

static bool CompInfoSym( void *info, void *sym )
/**********************************************/
{
    return ((comdat_info *)info)->sym == (symbol *)sym;
}

#ifdef _INT_DEBUG
static bool CheckSameComdat( void *info, void *sym )
/***********************************************************/
{
    if( (symbol *)sym == ((comdat_info *)info)->sym ) {
        LnkMsg( LOC_REC+ERR+MSG_INTERNAL, "s", "duplicate comdat found" );
        LnkMsg( ERR+MSG_INTERNAL, "s", ((symbol *)sym)->name );
    }
    return false;
}
#endif

#define ST_COMDAT (ST_CREATE | ST_NOALIAS | ST_REFERENCE)

void ProcComdat( void )
/****************************/
/* process a comdat record */
{
    comdat_info *       info;
    comdat_piece *      piece;
    offset              dataoff;
    segdata *           sdata;
    segnode *           seg;
    list_of_names *     symname;
    symbol *            sym;
    unsigned            namelen;
    unsigned            attr;
    unsigned            flags;
    unsigned            align;
    unsigned            alloc;
    unsigned            segidx;
    bool                usealign;

    flags = *ObjBuff++;
    attr = *ObjBuff++;
    align = *ObjBuff++;
    if( align == 0 ) {
        usealign = true;
    } else {
        usealign = false;
        align = OMFAlignTab[align];
    }
    if( ObjFormat & FMT_32BIT_REC ) {
        _TargU32toHost( _GetU32UN( ObjBuff ), dataoff );
        ObjBuff += sizeof( unsigned_32 );
    } else {
        _TargU16toHost( _GetU16UN( ObjBuff ), dataoff );
        ObjBuff += sizeof( unsigned_16 );
    }
    SkipIdx();  /* not interested in the type index */
    alloc = attr & CDAT_ALLOC_MASK;
    if( alloc == 0 ) {  /* if explicit allocation */
        SkipIdx();                              /* get public base */
        segidx = GetIdx();
        if( segidx != 0 ) {
            seg = (segnode *) FindNode( SegNodes, segidx );
            if( usealign ) {
                align = seg->entry->align;
            }
        } else {
            BadObject();        // do not support absolute comdats
            return;             // NOTE: premature return
        }
    } else {    // do not support linker defined comdats
        BadObject();
        return;
    }
    piece = AllocCDatPiece();
    symname = FindName( GetIdx() );
    namelen = strlen( symname->name );
    if( flags & CDAT_STATIC ) {
        sym = SymOp( ST_COMDAT | ST_STATIC, symname->name, namelen );
    } else {
        sym = SymOp( ST_COMDAT, symname->name, namelen );
    }
    if( flags & CDAT_ITERATED ) {
        piece->length = CalcLIDataLength();
    } else {
        piece->length = EOObjRec - ObjBuff;
    }
    if( !CacheIsPerm() ) {
        _ChkAlloc( piece->data, piece->length );
        memcpy( piece->data, ObjBuff, piece->length );
        piece->free_data = true;
    } else {
        piece->data = ObjBuff;
    }
    if( flags & CDAT_CONTINUATION ) {
        info = RingLookup( CDatList, CompInfoSym, sym );
        RingAppend( &info->pieces, piece );
        info->sdata->length += piece->length;
    } else {
        sdata = AllocSegData();
        sdata->length = piece->length;
        sdata->combine = COMBINE_ADD;
        sdata->iscdat = true;
        sdata->isabs = seg == NULL;
        sdata->align = align;
        sdata->u.leader = seg->entry->u.leader;
        sdata->iscode = (seg->info & SEG_CODE) != 0;
        if( ObjFormat & FMT_32BIT_REC ) {
            sdata->is32bit = true;
        }
        info = AllocCDatInfo();
        info->sdata = sdata;
        info->sym = sym;
        info->flags = (attr & CDAT_SELECT_MASK) << CDAT_SELECT_SHIFT;
        info->flags |= SYM_DEAD;        // assume redefined
        RingAppend( &info->pieces, piece );
#ifdef _INT_DEBUG
        RingLookup( CDatList, CheckSameComdat, sym );
#endif
        RingAppend( &CDatList, info );
        if( IS_SYM_COMDAT(sym) ) {
            CheckComdatSym( sym, info->flags & ~SYM_DEAD );
        } else if( !(IS_SYM_REGULAR(sym) && (sym->info & SYM_DEFINED)) ) {
            if( sym->info & SYM_DEFINED ) {
                sym = HashReplace( sym );
            }
            ClearSymUnion( sym );
            if( alloc == 0 ) {  /* explicit */
                seg->entry->u.leader->info |= SEG_LXDATA_SEEN;
                RingAppend( &sdata->u.leader->pieces, sdata );
                Ring2Append( &CurrMod->segs, sdata );
#if 0
            } else {    /* add it to a linker defined segment */
                sdata->iscode = alloc & 1;
                sdata->alloc = --alloc;
                AddToLinkerComdat( sym );
#endif
            }
            sym->info |= (attr & CDAT_SELECT_MASK) << CDAT_SELECT_SHIFT;
            SetComdatSym( sym, sdata );
            info->flags &= ~SYM_DEAD;
        }
    }
    if( (info->flags & SYM_DEAD) && (LinkFlags & INC_LINK_FLAG) == 0 ) {
        ObjFormat |= FMT_IGNORE_FIXUPP;
    } else {
        ObjFormat &= ~(FMT_IGNORE_FIXUPP | FMT_IS_LIDATA);
    }
    SetCurrSeg( info->sdata, dataoff, piece->data );
}
