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
* Description:  Pass 1 of Open Watcom linker.
*
****************************************************************************/


#include <string.h>
#include <stdlib.h>
#include "linkstd.h"
#include "msg.h"
#include "alloc.h"
#include "wlnkmsg.h"
#include "specials.h"
#include "objnode.h"
#include "objcalc.h"
#include "dbgall.h"
#include "overlays.h"
#include "mapio.h"
#include "cmdline.h"
#include "objstrip.h"
#include "cmdnov.h"
#include "cmdelf.h"
#include "impexp.h"
#include "ring.h"
#include "strtab.h"
#include "carve.h"
#include "permdata.h"
#include "virtmem.h"
#include "obj2supp.h"
#include "objpass2.h"
#include "objpass1.h"
#include "loadpe.h"

#define MAX_SEGMENT         0x10000

static seg_leader       *LastCodeSeg;    // last code segment in current module

static seg_leader   *MakeNewLeader( segdata *sdata, class_entry *class, unsigned_16 info );
static seg_leader   *FindALeader( segdata *sdata, class_entry *class, unsigned_16 info );
static void         DoAllocateSegment( segdata *sdata, char *clname );

void ResetObjPass1( void )
/************************/
{
    ObjFormat = 0;
}

void P1Start( void )
/*************************/
{
    LastCodeSeg = NULL;
}

static void DoSavedImport( symbol *sym )
/**************************************/
{
    dll_sym_info        *dll;
    length_name         modname;
    length_name         extname;

    if( FmtData.type & (MK_OS2 | MK_PE) ) {
        dll = sym->p.import;
        sym->p.import = NULL;
        sym->info &= ~SYM_DEFINED;
        modname.name = dll->m.modname;
        modname.len = strlen( modname.name );
        if( dll->isordinal ) {
            MSImportKeyword( sym, &modname, NULL, dll->u.ordinal );
        } else {
            extname.name = dll->u.entname;
            extname.len = strlen( extname.name );
            MSImportKeyword( sym, &modname, &extname, NOT_IMP_BY_ORDINAL );
        }
        FreeImport( dll );
    }
}

static void DoSavedExport( symbol *sym )
/**************************************/
{
    entry_export        *exp;

    if( FmtData.type & (MK_OS2 | MK_PE | MK_WIN_VXD) ) {
        exp = sym->e.export;
        exp->sym = sym;
        exp->impname = NULL;
        AddToExportList( exp );
    } else {
        AddNameTable( sym->name, strlen(sym->name), TRUE,
                      &FmtData.u.nov.exp.export );
    }
}

static bool StoreCDatData( void *_piece, void *_loc )
/***************************************************/
{
    comdat_piece *piece = _piece;
    virt_mem     *loc = _loc;

    PutInfo( *loc, piece->data, piece->length );
    *loc += piece->length;
    return( FALSE );
}

void StoreInfoData( comdat_info *info )
/********************************************/
{
    virt_mem    temp;

    info->sdata->u1.vm_ptr = AllocStg( info->sdata->length );
    temp = info->sdata->u1.vm_ptr;
    RingLookup( info->pieces, StoreCDatData, &temp );
}

static bool CheckVMemPieceDiff( void *_piece, void *_loc )
/********************************************************/
{
    comdat_piece *piece = _piece;
    virt_mem     *loc = _loc;
    bool         retval;

    retval = !CompareInfo( *loc, piece->data, piece->length );
    *loc += piece->length;
    return( retval );
}

static bool CheckMemPieceDiff( void *_piece, void *_loc )
/*******************************************************/
{
    comdat_piece *piece = _piece;
    char         **loc = _loc;
    bool         retval;

    retval = memcmp( *loc, piece->data, piece->length ) != 0;
    *loc += piece->length;
    return( retval );
}

static bool CheckSameData( symbol *sym, comdat_info *info )
/*********************************************************/
{
    virt_mem        vmem;
    char            *data;
    comdat_piece    *piece;

    if( sym->mod->modinfo & MOD_DONE_PASS_1 ) {
        vmem = sym->p.seg->u1.vm_ptr;
        piece = RingLookup( info->pieces, CheckVMemPieceDiff, &vmem );
    } else {
        data = GetSegContents( sym->p.seg, sym->p.seg->u1.vm_offs );
        piece = RingLookup( info->pieces, CheckMemPieceDiff, &data );
    }
    if( piece == NULL ) {       // found a match
        info->sym->u.datasym = sym;
        return( TRUE );
    }
    return( FALSE );
}

static bool CheckAltSym( void *_sym, void *_info )
/************************************************/
{
    comdat_info *info = _info;
    symbol      *sym = _sym;

    if( sym != info->sym && IS_SYM_COMDAT( sym ) && (sym->info & SYM_HAS_DATA) ) {
        return( CheckSameData( sym, info ) );
    }
    return( FALSE );
}

void InfoCDatAltDef( comdat_info *info )
/*********************************************/
{
    symbol      *mainsym;

    mainsym = info->sym;
    info->sym = AddAltDef( mainsym, SYM_COMDAT );
    info->sym->p.seg = info->sdata;
    info->sym->info |= info->flags & SYM_CDAT_SEL_MASK;
    if( !CheckSameData( mainsym, info ) ) {
        mainsym = RingLookup( mainsym->u.altdefs, CheckAltSym, info );
        if( mainsym == NULL ) {
            StoreInfoData( info );
            info->sym->info |= SYM_HAS_DATA;
        }
    }
}

static void AddCDatAltDef( segdata *sdata, symbol *sym, unsigned_8 *data,
                           sym_info flags )
/***********************************************************************/
{
    comdat_info     info;
    comdat_piece    piece;

    if( LinkFlags & INC_LINK_FLAG ) {
        info.sdata = sdata;
        info.sym = sym;
        info.flags = flags;
        info.pieces = NULL;
        piece.data = data;
        piece.length = sdata->length;
        piece.free_data = FALSE;
        RingAppend( &info.pieces, &piece );
        InfoCDatAltDef( &info );
    }
}

static void DoIncSymbol( void *_sym )
/***********************************/
{
    symbol      *sym = _sym;
    symbol      *mainsym;
    void        *data;
    sym_flags   flags;

    if( sym->info & SYM_IS_ALTDEF ) {
        flags = ST_CREATE | ST_REFERENCE;
        if( sym->info & SYM_STATIC ) {
            flags |= ST_STATIC;
        }
        mainsym = SymOp( flags, sym->name, strlen( sym->name ) );
        if( IS_SYM_NICOMDEF( sym ) ) {
            MakeCommunalSym( mainsym, sym->p.cdefsize, (sym->info & SYM_FAR_COMMUNAL) != 0, IS_SYM_COMM32( sym ) );
        } else if( IS_SYM_COMDAT( sym ) ) {
            if( sym->info & SYM_HAS_DATA ) {
                data = GetAltdefContents( sym->p.seg );
            } else {
                data = GetSegContents( sym->p.seg, sym->p.seg->u1.vm_offs );
            }
            sym->p.seg->isdead = FALSE;
            DefineComdat( sym->p.seg, mainsym, sym->addr.off, sym->info & SYM_CDAT_SEL_MASK, data );
        } else if( !(mainsym->info & SYM_DEFINED) )  {
            DoSavedImport( sym );       // FIXME can lose defs here.
        }
        CarveFree( CarveSymbol, sym );
    } else {
        if( !IS_SYM_COMDAT( sym ) ) {
            Ring2Append( &CurrMod->publist, sym );
        }
        if( sym->info & SYM_DEAD )
            return;
        if( IS_SYM_IMPORTED( sym ) ) {
            DoSavedImport( sym );
        } else if( IS_SYM_COMDAT( sym ) ) {
            DefineComdat( sym->p.seg, sym, sym->addr.off, sym->info & SYM_CDAT_SEL_MASK, GetSegContents(sym->p.seg, sym->p.seg->u1.vm_offs) );
        }
        if( sym->info & SYM_EXPORTED ) {
            DoSavedExport( sym );
        }
    }
}

unsigned long IncPass1( void )
/***********************************/
{
    segdata         *seglist;
    segdata         *seg;
    symbol          *publist;
    virt_mem_size   dataoff;
    unsigned        relocs;

    seglist = CurrMod->segs;
    CurrMod->segs = NULL;
    CurrMod->lines = NULL;
    for(;;) {
        seg = Ring2Pop( &seglist );
        if( seg == NULL )
            break;
        dataoff = seg->u1.vm_offs;
        DoAllocateSegment( seg, seg->o.clname );
        seg->o.mod = CurrMod;
        if( !seg->isuninit && !seg->isdead && !seg->iscdat ) {
            PutInfo( seg->u1.vm_ptr, GetSegContents( seg, dataoff ), seg->length );
            seg->u.leader->info |= SEG_LXDATA_SEEN;
        }
    }
    publist = CurrMod->publist;
    CurrMod->publist = NULL;
    Ring2Walk( publist, DoIncSymbol );
    relocs = CurrMod->relocs;
    PermStartMod( CurrMod );    // destroys currmod->relocs
    IterateModRelocs( relocs, CurrMod->sizerelocs, IncSaveRelocs );
    return( 0 );
}

static class_entry *FindNamedClass( char *name )
/***********************************************/
// NYI:  this doesn't take into account 16 & 32 bit classes with the same name.
{
    class_entry   *class;

    for( class = Root->classlist; class != NULL; class = class->next_class ) {
        if( stricmp( class->name, name ) == 0 ) {
            return( class );
        }
    }
    return( NULL );
}

static bool CmpSegName( void *leader, void *name )
/************************************************/
{
    return( stricmp( ((seg_leader *)leader)->segname, name ) == 0 );
}

static bool DefIncGroup( void *_def, void *_grouptab )
/****************************************************/
{
    incgroupdef     *def = _def;
    group_entry     ***grouptab = _grouptab;
    group_entry     *group;
    unsigned        index;
    char            **currname;
    class_entry     *class;
    seg_leader      *leader;

    group = GetGroup( def->grpname );
    currname = def->names;
    for( index = 0; index < def->numsegs; index++ ) {
        class = FindNamedClass( *currname );
        currname++;
        if( class != NULL ) {
            leader = RingLookup( class->segs, CmpSegName, *currname );
            if( leader != NULL ) {
                AddToGroup( group, leader );
            }
        }
        currname++;
    }
    **grouptab = group;
    (*grouptab)++;
    return( FALSE );
}

void DoIncGroupDefs( void )
/********************************/
{
    unsigned        numgroups;
    group_entry     **grouptab;

    numgroups = RingCount( IncGroupDefs );
    _ChkAlloc( IncGroups, sizeof( group_entry * ) * numgroups );
    grouptab = IncGroups;
    RingLookup( IncGroupDefs, DefIncGroup, &grouptab );
    RingFree( &IncGroupDefs );
    IncGroupDefs = NULL;
}

void Set64BitMode( void )
/******************************/
// make sure that the executable format is a 64-bit format.
{
    LinkState |= FMT_SEEN_64_BIT;
    if( !HintFormat( MK_ALLOW_64 ) ) {
        if( !(ObjFormat & FMT_TOLD_XXBIT) ) {
            ObjFormat |= FMT_TOLD_XXBIT;
            LnkMsg( WRN+MSG_FOUND_XXBIT_OBJ, "sd",
                        CurrMod->f.source->file->name, 64 );
        }
    }
}

void Set32BitMode( void )
/******************************/
// make sure that the executable format is a 32-bit format.
{
    LinkState |= FMT_SEEN_32_BIT;
    if( !HintFormat( MK_ALLOW_32 ) ) {
        if( !(ObjFormat & FMT_TOLD_XXBIT) ) {
            ObjFormat |= FMT_TOLD_XXBIT;
            LnkMsg( WRN+MSG_FOUND_XXBIT_OBJ, "sd",
                        CurrMod->f.source->file->name, 32 );
        }
    }
}

void Set16BitMode( void )
/******************************/
{
    if( !HintFormat( MK_ALLOW_16 ) ) {
        if( !(ObjFormat & FMT_TOLD_XXBIT) ) {
            ObjFormat |= FMT_TOLD_XXBIT;
            LnkMsg( WRN+MSG_FOUND_XXBIT_OBJ, "sd",
                    CurrMod->f.source->file->name, 16 );
        }
    }
}

void AllocateSegment( segnode *newseg, char *clname )
/**********************************************************/
// allocate a new segment (or new piece of a segment)
{
    DoAllocateSegment( newseg->entry, clname );
    newseg->info = newseg->entry->u.leader->info;
}

static void DoAllocateSegment( segdata *sdata, char *clname )
/***********************************************************/
{
    section         *sect;
    class_entry     *class;
    bool            isovlclass;

    isovlclass = FALSE;
    if( FmtData.type & MK_OVERLAYS ) {
        sdata->iscode = CheckOvlClass( clname, &isovlclass );
    }
    if( sdata->iscode ) {
        if( !sdata->is32bit ) {
            LinkState |= HAVE_16BIT_CODE;
        }
    }
    sect = DBIGetSect( clname );
    if( sect == NULL ) {
        if( !(FmtData.type & MK_OVERLAYS) || isovlclass ) {
            sect = CurrSect;
        } else {
            sect = NonSect;
        }
    }
    class = FindClass( sect, clname, ( sdata->is32bit != 0 ), ( sdata->iscode != 0 ) );
    AddSegment( sdata, class );
    if( isovlclass ) {
        sdata->u.leader->info |= SEG_OVERLAYED;
    }
    if( !sdata->isdead && !sdata->isuninit && !sdata->iscdat ) {
        sdata->u1.vm_ptr = AllocStg( sdata->length );
    }
}

static void CheckQNXSegMismatch( stateflag mask )
/***********************************************/
{
    if( (FmtData.type & MK_QNX) && (LinkState & mask)
                                && !FmtData.u.qnx.seen_mismatch ) {
        LnkMsg( WRN+LOC+MSG_CANNOT_HAVE_16_AND_32, NULL );
        FmtData.u.qnx.seen_mismatch = TRUE;
    }
}

void AddSegment( segdata *sd, class_entry *class )
/*******************************************************/
/* Add a segment to the segment list for an object file */
{
    unsigned_16     info;
    seg_leader      *leader;

    DEBUG((DBG_OLD,"- adding segment %s, class %s",sd->u.name, class->name ));
    DEBUG(( DBG_OLD, "- - size = %h, comb = %x, alignment = %x",
                      sd->length, sd->combine, sd->align ));
    info = 0;
    if( sd->is32bit ) {
        info |= USE_32;
    }
    if( class->flags & CLASS_CODE ) {
        info |= SEG_CODE;
    }
    if( sd->isabs ) {
        info |= SEG_ABSOLUTE;
        sd->isdefd = TRUE;
    }
    if( sd->isabs || sd->combine == COMBINE_INVALID ) {
        leader = MakeNewLeader( sd, class, info );
    } else {
        char    *seg_name = sd->u.name;

        leader = FindALeader( sd, class, info );
        if( ( (leader->info & USE_32) != (info & USE_32) ) &&
            !( (FmtData.type & MK_OS2_FLAT) && FmtData.u.os2.mixed1632 ) &&
            !(FmtData.type & MK_RAW) ) {
            char    *segname_16;
            char    *segname_32;

            if( info & USE_32 ) {
                segname_16 = leader->segname;
                segname_32 = seg_name;
            } else {
                segname_16 = seg_name;
                segname_32 = leader->segname;
            }
            LnkMsg( ERR+MSG_CANT_COMBINE_32_AND_16, "12", segname_32, segname_16 );
        }
    }
    leader->dbgtype = DBIColSeg( class );
    if( !IS_DBG_INFO( leader ) ) {
        if( sd->is32bit ) {
            Set32BitMode();
            CheckQNXSegMismatch( HAVE_16BIT_CODE );
        } else {
            Set16BitMode();
            CheckQNXSegMismatch( FMT_SEEN_32_BIT );
        }
    }
    if( DBISkip( leader ) ) {
        sd->isdead = TRUE;
    }
    if( sd->isabs ) {
        leader->seg_addr.off = 0;
        leader->seg_addr.seg = sd->frame;
    } else if( !sd->isdead ) {
        DBIAddLocal( leader, sd->length );
    }
}

class_entry *DuplicateClass( class_entry *old )
/*********************************************/
{
    class_entry *new;

    new = CarveAlloc( CarveClass );
    memcpy( new, old, sizeof( class_entry ) );
    new->name = AddBufferStringTable( &PermStrings, old->name, strlen( old->name ) + 1 );
    old->next_class = new;
    return( new );
}

class_entry *FindClass( section *sect, char *name, bool is32bit, bool iscode )
/****************************************************************************/
{
    class_entry     *currclass;
    class_entry     *lastclass;
    unsigned        namelen;
    class_status    cls_is32bit;

    cls_is32bit = ( is32bit ) ? CLASS_32BIT : 0;
    lastclass = sect->classlist;
    for( currclass = sect->classlist; currclass != NULL; currclass = currclass->next_class ) {
        if( stricmp( currclass->name, name ) == 0 && (currclass->flags & CLASS_32BIT) == cls_is32bit ) {
            return( currclass );
        }
        lastclass = currclass;
    }
    namelen = strlen( name );
    currclass = CarveAlloc( CarveClass );
    currclass->name = AddBufferStringTable( &PermStrings, name, namelen + 1 );
    currclass->segs = NULL;
    currclass->section = sect;
    currclass->flags = is32bit;
    currclass->next_class = NULL;
    if( lastclass == NULL ) {
        sect->classlist = currclass;
    } else {
        lastclass->next_class = currclass;
    }
    DBIColClass( currclass );
    if( iscode ) {
        currclass->flags |= CLASS_CODE;
    }
    if( IsConstClass( name, namelen ) ) {
        currclass->flags |= CLASS_READ_ONLY;
    }
    if( IsStackClass( name, namelen ) ) {
        currclass->flags |= CLASS_STACK;
    }
    return( currclass );
}

static void CheckForLast( seg_leader *seg, class_entry *class )
/*************************************************************/
// check to see if this segment should be the last one in an autogroup.
{
    if( (CurrMod->modinfo & MOD_LAST_SEG) && (class->flags & CLASS_CODE) ) {
        if( LastCodeSeg != NULL ) {             // more than one code seg
            LastCodeSeg->info &= ~LAST_SEGMENT; // so don't end at previous
        }
        LastCodeSeg = seg;
        seg->info |= LAST_SEGMENT;
    }
}

static bool CheckClassName( void *_seg, void *_sdata )
/****************************************************/
{
    seg_leader  *seg = _seg;
    segdata     *sdata = _sdata;

    return( stricmp( seg->segname, sdata->u.name ) == 0 &&
                                seg->combine != COMBINE_INVALID );
}

static void AddToLeader( seg_leader *seg, segdata *sdata )
/********************************************************/
{
    segdata     *first;
    offset      length;

    sdata->u.leader = seg;
    if( sdata->combine == COMBINE_COMMON ) {
        first = RingFirst( seg->pieces );
        length = max( first->length, sdata->length );
        if( first->isuninit ) {
            first->isdead = TRUE;
            RingPush( &seg->pieces, sdata );
            sdata->length = length;
        } else {
            sdata->isdead = TRUE;
            RingAppend( &seg->pieces, sdata );
            first->length = length;
        }
    } else {    // it must be COMBINE_ADD
        RingAppend( &seg->pieces, sdata );
    }
    Ring2Append( &CurrMod->segs, sdata );
}

static seg_leader *FindALeader( segdata *sdata, class_entry *class, unsigned_16 info )
/************************************************************************************/
{
    seg_leader  *leader;

    leader = RingLookup( class->segs, CheckClassName, sdata );
    if( leader == NULL ) {
        leader = MakeNewLeader( sdata, class, info );
    } else {
        AddToLeader( leader, sdata );
    }
    return( leader );
}

seg_leader *InitLeader( char *segname )
/*************************************/
{
    seg_leader  *seg;

    seg = CarveAlloc( CarveLeader );
    seg->next_seg = NULL;
    seg->grp_next = NULL;
    seg->pieces = NULL;
    seg->class = NULL;
    seg->size = 0;
    seg->num = 0;
    seg->seg_addr.off = 0;
    seg->seg_addr.seg = UNDEFINED;
    seg->group = NULL;
    seg->info = 0;
    seg->segname = AddStringStringTable( &PermStrings, segname );
    seg->dbgtype = NOT_DEBUGGING_INFO;
    seg->segflags = FmtData.def_seg_flags;
    return( seg );
}

void FreeLeader( void *seg )
/*********************************/
{
    RingWalk( ((seg_leader *)seg)->pieces, FreeSegData );
    CarveFree( CarveLeader, seg );
}

static seg_leader *MakeNewLeader( segdata *sdata, class_entry *class, unsigned_16 info )
/**************************************************************************************/
{
    seg_leader *leader;

    sdata->u.leader = leader = InitLeader( sdata->u.name );
    leader->align = sdata->align;
    leader->combine = sdata->combine;
    leader->class = class;
    leader->info = info;
    CheckForLast( leader, class );
    RingAppend( &class->segs, leader );
    RingAppend( &leader->pieces, sdata );
    Ring2Append( &CurrMod->segs, sdata );
    return( leader );
}

static bool CmpLeaderPtr( void *a, void *b )
/******************************************/
{
    return( a == b );
}

void AddToGroup( group_entry *group, seg_leader *seg )
/***********************************************************/
{
    if( Ring2Lookup( group->leaders, CmpLeaderPtr, seg ) )
        return;
    if( seg->group != NULL && seg->group != group ) {
        LnkMsg( LOC+ERR+MSG_SEG_IN_TWO_GROUPS, "123", seg->segname,
                                   seg->group->sym->name, group->sym->name );
        return;
    }
    if( ( group->leaders != NULL ) &&
        ( (group->leaders->info & USE_32) != (seg->info & USE_32) ) &&
        !( (FmtData.type & MK_OS2_FLAT) && FmtData.u.os2.mixed1632 ) &&
        !(FmtData.type & MK_RAW) ) {

        char    *segname_16;
        char    *segname_32;

        if( seg->info & USE_32 ) {
            segname_16 = group->leaders->segname;
            segname_32 = seg->segname;
        } else {
            segname_16 = seg->segname;
            segname_32 = group->leaders->segname;
        }
        LnkMsg( ERR+MSG_CANT_COMBINE_32_AND_16, "12", segname_32, segname_16 );
    }
    seg->group = group;
    Ring2Append( &group->leaders, seg );
}

void SetAddPubSym(symbol *sym, sym_info type, mod_entry *mod, offset off,
                         unsigned_16 frame )
/*************************************************************************/
{
    sym->mod = mod;
    SET_SYM_TYPE( sym, type );
    XDefSymAddr( sym, off, frame );
    Ring2Append( &mod->publist, sym );
}

void DefineSymbol( symbol *sym, segnode *seg, offset off,
                          unsigned_16 frame )
/**************************************************************/
// do the object file independent public symbol definition.
{
    unsigned        name_len;
    bool            frame_ok;
    sym_info        sym_type;

    if( seg != NULL ) {
        frame = 0;
    }
    name_len = strlen( sym->name );
    if( sym->addr.seg != UNDEFINED && !IS_SYM_COMMUNAL( sym ) ) {
        if( seg != NULL && sym->p.seg != NULL ) {
            frame_ok = (sym->p.seg->u.leader == seg->entry->u.leader);
            if( sym->p.seg->u.leader->combine != COMBINE_COMMON ) {
                frame_ok = FALSE;
            }
        } else if( sym->p.seg != NULL ) {
            frame_ok = FALSE;
        } else if( frame != sym->addr.seg ) {
            frame_ok = FALSE;
        } else {
            frame_ok = TRUE;
        }
        if( !(frame_ok && off == sym->addr.off) ) {
            ReportMultiple( sym, sym->name, name_len );
        }
    } else {
        sym_type = SYM_REGULAR;
        if( IS_SYM_IMPORTED( sym ) ) {
            sym = HashReplace( sym );
            if( (FmtData.type & MK_PE) && sym->p.import != NULL ) {
                dll_sym_info  *dll_info = sym->p.import;
                AddPEImportLocalSym( sym, dll_info->iatsym );
                sym_type |= SYM_REFERENCED;
                LnkMsg( WRN+MSG_IMPORT_LOCAL, "s", sym->name );
            }
        } else if( IS_SYM_COMMUNAL( sym ) ) {
            sym = HashReplace( sym );
            sym->p.seg = NULL;
        }

        ClearSymUnion( sym );
        SetAddPubSym(sym, sym_type, CurrMod, off, frame);
        sym->info &= ~SYM_DISTRIB;
        if( seg != NULL ) {
            if( LinkFlags & STRIP_CODE ) {
                DefStripSym( sym, seg->entry );
            }
            if( seg->info & SEG_CODE ) {
                if( (FmtData.type & MK_OVERLAYS) && FmtData.u.dos.distribute
                    && (LinkState & SEARCHING_LIBRARIES) ) {
                    sym->info |= SYM_DISTRIB;
                }
            }
            sym->p.seg = seg->entry;
            if( sym->p.seg->isabs )
                sym->info |= SYM_ABSOLUTE;
            TryDefVector( sym );
        } else {
            if( LinkFlags & STRIP_CODE ) {
                CleanStripInfo( sym );
            }
            sym->info |= SYM_ABSOLUTE;
            sym->p.seg = NULL;
        }
    }
}

static segdata *GetSegment( char *seg_name, char *class_name, char *group_name,
                            unsigned align, unsigned comb, bool use_16 )
/*****************************************************************************/
{
    section             *sect;
    class_entry         *class;
    group_entry         *group;
    unsigned_16         info;
    segdata             *sdata;
    seg_leader          *leader;

    if( FmtData.type & MK_OVERLAYS ) {
        sect = CheckOvlSect( class_name );
    } else {
        sect = Root;
    }
    class = FindClass( sect, class_name, !use_16, FALSE );
    info = 0;
    sdata = AllocSegData();
    sdata->u.name = seg_name;
    sdata->align = align;
    sdata->combine = comb;
    sdata->isuninit = TRUE;
    if( !use_16 ) {
        info |= USE_32;
        sdata->is32bit = TRUE;
    }
    leader = FindALeader( sdata, class, info );
    if( group_name != NULL ) {
        /* put in appropriate group */
        group = GetGroup( group_name );
        AddToGroup( group, leader );
    }
    return( sdata );
}

static void NearAllocCommunal( symbol *sym, unsigned size )
/*********************************************************/
{
    sym->p.seg = GetSegment( CommunalSegName, BSSClassName, DataGrpName,
                                     2, COMBINE_ADD, !IS_SYM_COMM32( sym ) );
    sym->p.seg->length = size;
}

static void FarAllocCommunal( symbol *sym, unsigned size )
/********************************************************/
{
    segdata     *seg;
    segdata     *first;

    first = NULL;
    for(;;) {
        seg = GetSegment( sym->name, FarDataClassName, NULL,
                          0, COMBINE_INVALID, !IS_SYM_COMM32( sym ) );
        if( first == NULL )
            first = seg;
        if( size < MAX_SEGMENT ) {
            seg->length = size;
            break;
        } else {
            seg->length = MAX_SEGMENT;
            size -= MAX_SEGMENT;
        }
    }
    sym->p.seg = first;
    sym->info |= SYM_DEFINED;
}

void AllocCommunal( symbol *sym, offset size )
/***************************************************/
{
    if( LinkFlags & STRIP_CODE ) {
        CleanStripInfo( sym );
    }
    if( sym->info & SYM_FAR_COMMUNAL ) {
        FarAllocCommunal( sym, size );
    } else {
        NearAllocCommunal( sym, size );
    }
}

symbol *MakeCommunalSym( symbol *sym, offset size, bool isfar,
                                 bool is32bit )
/*********************************************************************/
{
    sym_info    symtype;
    symbol      *altsym;

    if( is32bit ) {
        symtype = SYM_COMMUNAL_32;
    } else {
        symtype = SYM_COMMUNAL_16;
    }
    if( !(sym->info & SYM_DEFINED) || IS_SYM_IMPORTED( sym ) ) {
        if( IS_SYM_IMPORTED( sym ) ) {
            sym = HashReplace( sym );
        }
        ClearSymUnion( sym );
        SET_SYM_TYPE( sym, symtype );
        sym->info |= SYM_DEFINED;
        if( isfar ) {
            sym->info |= SYM_FAR_COMMUNAL;
        }
        AllocCommunal( sym, size );
        sym->addr.off = 0;
        sym->mod = CurrMod;
        Ring2Append( &CurrMod->publist, sym );
    } else {
        if( IS_SYM_NICOMDEF( sym ) ) {
            size = max( sym->p.seg->length, size );
            sym->p.seg->length = size;
        }
        altsym = AddAltDef( sym, symtype );
        if( LinkFlags & INC_LINK_FLAG ) {
            altsym->p.cdefsize = size;
        }
    }
    return( sym );
}

void CheckComdatSym( symbol *sym, sym_info flags )
/*******************************************************/
// check a comdat redefinition to see if it is OK
// NYI: SYM_CDAT_SEL_SIZE, SYM_CDAT_SEL_EXACT, & SYM_CDAT_SEL_ASSOC not yet
// handled properly.  no prob. under coff, but OMF makes it very hard...
{
    sym_info    symflags;

    symflags = sym->info & SYM_CDAT_SEL_MASK;
    if( flags == SYM_CDAT_SEL_NODUP || symflags == SYM_CDAT_SEL_NODUP ) {
        symflags = SYM_CDAT_SEL_NODUP;
    } else {
        symflags = max( flags, symflags );
    }
    if( symflags == SYM_CDAT_SEL_NODUP ) {
        ReportMultiple( sym, sym->name, strlen(sym->name) );
    }
}

void SetComdatSym( symbol *sym, segdata *sdata )
/*****************************************************/
{
    if( LinkFlags & STRIP_CODE ) {
        if( sdata->iscode ) {
            DefStripSym( sym, sdata );
        }
    }
    sym->info |= SYM_DEFINED;
    Ring2Append( &CurrMod->publist, sym );
    sym->addr.off = 0;
    sym->mod = CurrMod;
    SET_SYM_TYPE( sym, SYM_COMDAT );
    sym->p.seg = sdata;
}

void DefineComdat( segdata *sdata, symbol *sym, offset value,
                          sym_info select, unsigned_8 *data )
/******************************************************************/
{
    if( IS_SYM_REGULAR( sym ) && (sym->info & SYM_DEFINED) ) {
        AddCDatAltDef( sdata, sym, data, select );
        sdata->isdead = TRUE;
        return;
    }
    if( sym->mod != NULL && sym->mod != CurrMod ) {
        CheckComdatSym( sym, select );
        AddCDatAltDef( sdata, sym, data, select );
        sdata->isdead = TRUE;
    } else {
        if( (sym->info & SYM_DEFINED) && !IS_SYM_COMDAT( sym ) ) {
            sym = HashReplace( sym );
        }
        ClearSymUnion( sym );
        sym->info |= select;
        SetComdatSym( sym, sdata );
        sym->addr.off += value;
        sdata->u1.vm_ptr = AllocStg( sdata->length );

        if(NULL == data)
            PutInfoNulls( sdata->u1.vm_ptr, sdata->length );
        else
            PutInfo( sdata->u1.vm_ptr, data, sdata->length );
    }
}

void DefineLazyExtdef( symbol *sym, symbol *def, bool isweak )
/*******************************************************************/
/* handle the lazy and weak extdef comments */
{
    symbol      *defaultsym;

    if( !(sym->info & (SYM_DEFINED | SYM_EXPORTED)) && !IS_SYM_IMPORTED( sym )
                                                  && !IS_SYM_COMMUNAL( sym ) ) {
        if( IS_SYM_A_REF( sym ) && !IS_SYM_LINK_WEAK( sym ) ) {
            if( IS_SYM_VF_REF( sym ) ) {
                defaultsym = *sym->e.vfdata;
            } else {
                defaultsym = sym->e.def;
            }
            if( def != defaultsym ) {
                LnkMsg( LOC_REC+WRN+MSG_LAZY_EXTDEF_MISMATCH, "S",sym );
            }
        } else if( !(sym->info & SYM_OLDHAT) || IS_SYM_LINK_WEAK( sym ) ) {
            if( isweak ) {
                SET_SYM_TYPE( sym, SYM_WEAK_REF );
            } else {
                SET_SYM_TYPE( sym, SYM_LAZY_REF );
            }
            sym->e.def = def;
            if( LinkFlags & STRIP_CODE ) {
                DataRef( sym->e.def );  // default must not be removed
            }
        }
    }
}

static symbol **GetVFList( symbol *defsym, symbol *mainsym, bool generate,
                            vflistrtns *rtns )
/****************************************************************************/
/* get the conditional symbols list from the vftable record, adding edges
 * to the dead code graph if necessary */
{
    symbol      **liststart;
    symbol      **symlist;
    symbol      *condsym;
    void        *bufstart;
    char        *name;
    unsigned    count;

    liststart = NULL;
    bufstart = rtns->getstart();
    count = 2;          // 1 for the default extdef, and 1 for NULL.
    while( !rtns->isend() ) {
        name = rtns->getname();
        condsym = FindISymbol( name );
        if( condsym == NULL ) {
            condsym = MakeWeakExtdef( name, defsym );
        } else if( (condsym->info & SYM_DEFINED) && !IS_SYM_COMMUNAL( condsym ) ) {
            generate = FALSE;
            if( mainsym == NULL ) {
                break;
            }
        }
        if( (LinkFlags & STRIP_CODE) && mainsym != NULL ) {
            AddSymEdge( condsym, mainsym );
        }
        count++;
    }
    if( generate ) {
        _ChkAlloc( symlist, sizeof( symbol * ) * count );
        liststart = symlist;
        *symlist = defsym;
        symlist++;
        rtns->setstart( bufstart );
        while( !rtns->isend() ) {
            name = rtns->getname();
            *symlist = FindISymbol( name );
            symlist++;
        }
        *symlist = NULL;
    }
    return( liststart );
}

static void DefineVirtualFunction( symbol *sym, symbol *defsym, bool ispure,
                                   vflistrtns *rtns )
/***************************************************************************/
// change sym into a virtual function reference
{
    symbol      **symlist;

    symlist = GetVFList( defsym, sym, TRUE, rtns );
    sym->info |= SYM_VF_REFS_DONE;
    if( symlist != NULL ) {
        sym->e.vfdata = symlist;
        if( ispure ) {
            SET_SYM_TYPE( sym, SYM_PURE_REF );
        } else {
            SET_SYM_TYPE( sym, SYM_VF_REF );
        }
    } else {                    // might still need this if eliminated
        if( (LinkFlags & STRIP_CODE) && !(sym->info & SYM_EXPORTED) ) {
            sym->e.def = defsym;
        }
    }
    if( LinkFlags & STRIP_CODE ) {
        DataRef( defsym );
    }
}

void DefineVFTableRecord( symbol *sym, symbol *def, bool ispure,
                                 vflistrtns *rtns )
/**********************************************************************/
// process the watcom virtual function table information extension
{
    symbol      **symlist;
    symbol      **startlist;
    symbol      **oldlist;

    if( sym->info & SYM_DEFINED ) {
        /* if defined, still may have to keep track of conditional symbols
         * for dead code elimination */
        if( (LinkFlags & STRIP_CODE)
                        && !(sym->info & (SYM_VF_REFS_DONE | SYM_EXPORTED)) ) {
            GetVFList( def, sym, FALSE, rtns );
            sym->e.def = def;
            sym->info |= SYM_VF_REFS_DONE;
            DataRef( def );
        }
    } else if( !IS_SYM_IMPORTED( sym ) && !IS_SYM_COMMUNAL( sym ) ) {
        if( IS_SYM_VF_REF( sym ) ) {
            if( IS_SYM_PURE_REF( sym ) ^ ispure ) {
                LnkMsg( LOC_REC+WRN+MSG_VF_PURE_MISMATCH, "S", sym );
            }
            symlist = GetVFList( def, NULL, TRUE, rtns );
            if( symlist == NULL ) {
                if( !CheckVFList( sym ) ) {
                    LnkMsg( LOC_REC+WRN+MSG_VF_TABLE_MISMATCH, "S", sym );
                }
            } else {
                startlist = symlist;
                oldlist = sym->e.vfdata;
                for(;;) {
                    if( *oldlist != *symlist ) {
                        LnkMsg( LOC_REC+WRN+MSG_VF_TABLE_MISMATCH, "S", sym );
                        break;
                    }
                    if( *oldlist == NULL )
                        break;
                    oldlist++;
                    symlist++;
                }
                _LnkFree( startlist );
            }
        } else if( IS_SYM_A_REF( sym ) || !(sym->info & SYM_OLDHAT) ) {
            DefineVirtualFunction( sym, def, ispure, rtns );
        }
    }
}

void DefineVFReference( void *src, symbol *targ, bool issym )
/*******************************************************************/
{
    if( issym ) {
        AddSymEdge( src, targ );
    } else {
        if( ((segnode *)src)->info & SEG_CODE ) {
            AddEdge( (segdata *)((segnode *)src)->entry, targ );
        }
    }
}

void DefineReference( symbol *sym )
/*****************************************/
// we have an object file reference for sym
{
    if( FmtData.type & MK_OVERLAYS ) {
        TryRefVector( sym );
    }
    if( (FmtData.type & MK_NOVELL) && (LinkState & SEARCHING_LIBRARIES)
            && IS_SYM_IMPORTED( sym ) && (sym->info & SYM_CHECKED) ) {
        sym->info &= ~SYM_CHECKED;
        LinkState |= LIBRARIES_ADDED;   // force another pass thru libs
    }
}

group_entry *GetGroup( char *name )
/*****************************************/
/* Get group of specified name. */
{
    group_entry     *grp;

    grp = SearchGroups( name );
    if( grp == NULL ) {
        grp = AllocGroup( name, &Groups );
    }
    return( grp );
}

group_entry *SearchGroups( char *name )
/*********************************************/
/* Find group of specified name. */
{
    group_entry     *currgrp;

    for( currgrp = Groups; currgrp != NULL; currgrp = currgrp->next_group ) {
        if( stricmp( currgrp->sym->name, name ) == 0 ) {
            DEBUG(( DBG_OLD, "- group %s found at %x", name, currgrp ));
            return( currgrp );
        }
    }
    return( NULL );
}

void SetCurrSeg( segdata *seg, offset obj_offset, unsigned_8 *data )
/******************************************************************/
/* register a segment for the purposes of storing relocations */
{
    CurrRec.seg = seg;
    CurrRec.obj_offset = obj_offset;
    CurrRec.data = data;
}

bool SeenDLLRecord( void )
/*******************************/
{
    LinkState |= FMT_SEEN_IMPORT_CMT;
    if( !HintFormat( MK_OS2 | MK_PE | MK_ELF | MK_NOVELL ) ) {
        LnkMsg( LOC+WRN+MSG_DLL_WITH_386, NULL );
        return( FALSE );    /* Not OK to process import/export records. */
    } else {
        return( TRUE );
    }
}

void HandleImport( length_name *intname, length_name *modname, length_name *extname, ordinal_t ordinal )
/******************************************************************************************************/
// handle the import coment record
{
    symbol      *sym;

    sym = SymOp( ST_CREATE, intname->name, intname->len );
    if( !(sym->info & SYM_DEFINED) ) {
        if( CurrMod != NULL ) {
            Ring2Append( &CurrMod->publist, sym );
            sym->mod = CurrMod;
        }
        if( FmtData.type & (MK_OS2 | MK_PE | MK_WIN_VXD) ) {
            MSImportKeyword( sym, modname, extname, ordinal );
        } else {
            SET_SYM_TYPE( sym, SYM_IMPORTED );
            sym->info |= SYM_DEFINED | SYM_DCE_REF;
            if( FmtData.type & MK_NOVELL ) {
                SetNovImportSymbol( sym );
            } else {
                SetELFImportSymbol( sym );
            }
        }
    }
}

static void ExportSymbol( length_name *expname )
/**********************************************/
{
    symbol      *sym;

    sym = SymOp( ST_CREATE | ST_REFERENCE, expname->name, expname->len );
    sym->info |= SYM_EXPORTED;
    AddNameTable( expname->name, expname->len, TRUE, &FmtData.u.nov.exp.export );
}

void HandleExport( length_name *expname, length_name *intname, unsigned flags, ordinal_t ordinal )
/************************************************************************************************/
{
    if( FmtData.type & (MK_OS2 | MK_PE | MK_WIN_VXD) ) {
        MSExportKeyword( expname, intname, flags, ordinal );
    } else {
        ExportSymbol( expname );
    }
}

bool CheckVFList( symbol *sym )
/*************************************/
/* see if any of the conditional symbols for this symbol are defined */
{
    symbol      **symlist;

    if( sym->info & SYM_VF_MARKED ) {
        ConvertVFSym( sym );
        return( TRUE );
    }
    for( symlist = sym->e.vfdata + 1; *symlist != NULL; ++symlist ) {
        if( (*symlist)->info & (SYM_DEFINED | SYM_VF_MARKED) ) {
            ConvertVFSym( sym );
            return( TRUE );
        }
    }
    return( FALSE );
}
