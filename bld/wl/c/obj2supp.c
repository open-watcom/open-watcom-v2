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
* Description:  Utilities for wlink specific parts of objpass2.
*
****************************************************************************/

#include <string.h>
#include "linkstd.h"
#include "reloc.h"
#include "pcobj.h"
#include "newmem.h"
#include "msg.h"
#include "alloc.h"
#include "mapio.h"
#include "exeos2.h"
#include "exeqnx.h"
#include "exeelf.h"
#include "loadfile.h"
#include "loados2.h"
#include "loadnov.h"
#include "loadqnx.h"
#include "loadelf.h"
#include "wlnkmsg.h"
#include "virtmem.h"
#include "objpass2.h"
#include "objstrip.h"
#include "objcalc.h"
#include "impexp.h"
#include "overlays.h"
#include "strtab.h"
#include "carve.h"
#include "permdata.h"
#include "dbgcomm.h"
#include "dbgall.h"
#include "dbgcv.h"
#include "toc.h"
#include "ring.h"
#include "obj2supp.h"

typedef struct fix_relo_data {
    byte        *data;
    offset      value;      /* value at location being patched */
    targ_addr   loc_addr;
    targ_addr   tgt_addr;
    fix_type    type;
    unsigned    ffix        : 3;
    unsigned    additive    : 1;
    unsigned    done        : 1;
    unsigned    imported    : 1;
    unsigned    os2_selfrel : 1;
} fix_relo_data;

typedef struct {
    fix_type    flags;
    offset      off;
    void        *target;
    void        *frame;
} fixupf_t;

typedef struct {
    fix_type    flags;
    offset      off;
    void        *target;
} fixup_t;

typedef struct {
    fix_type    flags;
    segdata     *sdata;
} sdata_t;

typedef struct {
    union {
        fixupf_t    fixupf;
        fixup_t     fixup;
        sdata_t     sdata;
    } u;
} save_fixup;

static offset           LastOptimized;  // offset last optimized.
static fix_type         LastOptType;
static segdata          *LastSegData;
static offset           FixupOverflow;

static unsigned         MapOS2FixType( fix_type type );
static void             PatchOffset( fix_relo_data *fix, offset val, bool isdelta );
static void             Relocate( offset off, fix_relo_data *fix, target_spec *target );


#define MAX_ADDEND_SIZE ( 2 * sizeof( unsigned_32 ) )

#define GET_S16( P )    (*(signed_16 *)(P))

#define FIX_POINTER_MASK ( FIX_BASE | FIX_HIGH | FIX_OFFSET_MASK )

void ResetObj2Supp( void )
/*******************************/
{
    FixupOverflow = 0;
    LastOptType   = 0;
    LastOptimized = 0xFFFFFFFF;
}

static void DCERelocRef( symbol *sym, bool isdata )
/*************************************************/
{
    if( isdata ) {
        if( !(sym->info & SYM_VF_REFS_DONE) ) {
            DataRef( sym );
        }
    } else {
        if( CurrRec.seg->ispdata || CurrRec.seg->isreldata ) {
            if( !IsTocSym( sym ) ) {       // make sure that if symbol goes,
                AddSymSegEdge( sym, CurrRec.seg ); // .pdata and .reldata go too
            }
        } else {
            AddEdge( CurrRec.seg, sym );
        }
    }
}

static void TraceFixup( fix_type type, target_spec *target )
/**********************************************************/
{
    bool        isovldata;          // TRUE if data and overlaying.
    bool        overlay;            // TRUE if doing overlays;

    overlay = ( (FmtData.type & MK_OVERLAYS) && !FmtData.u.dos.noindirect );
    if( (LinkFlags & STRIP_CODE) || overlay ) {
        isovldata = !(CurrRec.seg->u.leader->info & SEG_OVERLAYED);
        if( ObjFormat & FMT_UNSAFE_FIXUPP )
            isovldata = TRUE;
        if( target->type == FIX_TARGET_SEG ) {
            if( LinkFlags & STRIP_CODE ) {
                if( target->u.sdata->iscode && target->u.sdata != CurrRec.seg ) {
                    RefSeg( target->u.sdata );
                }
            }
        } else if( target->type == FIX_TARGET_EXT ) {
            if( LinkFlags & STRIP_CODE ) {
                DCERelocRef( target->u.sym, !CurrRec.seg->iscode );
            }
            if( !(type & FIX_REL) && overlay ) {
                switch( type & FIX_POINTER_MASK ) {
                case FIX_OFFSET_16:
                    if( FmtData.u.dos.ovl_short ) {
                        IndirectCall( target->u.sym );
                    }
                    break;
                case FIX_BASE:
                    IndirectCall( target->u.sym );
                    break;
                case FIX_BASE_OFFSET_16:
                    if( isovldata ) {
                        IndirectCall( target->u.sym );
                    }
                    break;
                }
            }  /* end if (notrelative && overlay) */
        } /* end if (a symbol/segment) */
    }
}

void RelocStartMod( void )
/*******************************/
{
    LastSegData = NULL;
}

static byte     OffsetSizes[] = { 0, 1, 2, 4, 4, 4, 4 };

static unsigned CalcFixupSize( fix_type type )
/********************************************/
{
    unsigned value;

    value = OffsetSizes[FIX_GET_OFFSET( type )];
    if( type & FIX_BASE ) {
        value += sizeof( unsigned_16 );
    }
    return( value );
}

static unsigned CalcAddendSize( fix_type fixtype )
/************************************************/
{
    if( fixtype & FIX_ADDEND_ZERO ) {
        return( 0 );
    } else if( (fixtype & (FIX_OFFSET_MASK | FIX_BASE)) == FIX_BASE_OFFSET_32 ) {
        return( MAX_ADDEND_SIZE );
    }
    return( sizeof( unsigned_32 ) );
}

static unsigned CalcSavedFixSize( fix_type fixtype )
/**************************************************/
{
    unsigned    retval;

    if( fixtype & FIX_CHANGE_SEG ) {
        retval = sizeof( sdata_t );
    } else if( FRAME_HAS_DATA( FIX_GET_FRAME( fixtype ) ) ) {
        retval = sizeof( fixupf_t ) + CalcAddendSize( fixtype );
    } else {
        retval = sizeof( fixup_t ) + CalcAddendSize( fixtype );
    }
    return( retval );
}

static void UpdateFramePtr( frame_spec *frame )
/*********************************************/
/* do any necessary work to get frame pointing to the right place */
{
    switch( frame->type ) {
    case FIX_FRAME_GRP:
        if( IS_FMT_INCREMENTAL( CurrMod->modinfo ) ) {
            DbgAssert( frame->u.val != 0 );
            frame->u.group = IncGroups[frame->u.val - 1];
        }
        break;
    case FIX_FRAME_EXT:
        frame->u.sym = UnaliasSym( ST_FIND, frame->u.sym );
        DbgAssert( frame->u.sym != NULL );
        break;
    }
}

static void UpdateTargetPtr( target_spec *target )
/************************************************/
/* do any necessary work to get target pointing to the right place */
{
    switch( target->type ) {
    case FIX_TARGET_GRP:
        if( IS_FMT_INCREMENTAL( CurrMod->modinfo ) ) {
            DbgAssert( target->u.val != 0 );
            target->u.group = IncGroups[target->u.val - 1];
        }
        break;
    case FIX_TARGET_EXT:
        target->u.sym = UnaliasSym( ST_FIND, target->u.sym );
        DbgAssert( target->u.sym != NULL );
        break;
    }
}

static void GetFrameAddr( frame_spec *frame, targ_addr *addr,
                          targ_addr *tgt_addr, offset off )
/**************************************************************/
{
    switch( frame->type ) {
    case FIX_FRAME_SEG:
        *addr = frame->u.sdata->u.leader->seg_addr;
        addr->off += frame->u.sdata->a.delta;
        break;
    case FIX_FRAME_GRP:
        *addr = frame->u.group->grp_addr;
        break;
    case FIX_FRAME_EXT:
        *addr = frame->u.sym->addr;
        break;
    case FIX_FRAME_ABS:
        addr->seg = frame->u.abs;
        break;
    case FIX_FRAME_LOC:
        *addr = CurrRec.addr;
        addr->off += off;
        break;
    case FIX_FRAME_TARG:
    case FIX_FRAME_FLAT:
        *addr = *tgt_addr;
        break;
    }
}

static void GetTargetAddr( target_spec *target, targ_addr *addr, offset off )
/***************************************************************************/
{
    switch( target->type ) {
    case FIX_TARGET_SEG:
        *addr = target->u.sdata->u.leader->seg_addr;
        addr->off += target->u.sdata->a.delta;
        break;
    case FIX_TARGET_GRP:
        *addr = target->u.group->grp_addr;
        break;
    case FIX_TARGET_EXT:
        *addr = target->u.sym->addr;
        break;
    case FIX_TARGET_ABS:
        addr->seg = target->u.abs;
        break;
    }
}

static void MapFramePtr( frame_spec *frame, void **targ )
/*******************************************************/
{
    switch( frame->type ) {
    case FIX_FRAME_SEG:
        *targ = CarveGetIndex( CarveSegData, *targ );
        break;
    case FIX_FRAME_GRP:
        *targ = (void *)(pointer_int)frame->u.group->num;
        break;
    case FIX_FRAME_EXT:
        *targ = CarveGetIndex( CarveSymbol, *targ );
        break;
    }
}

static void MapTargetPtr( target_spec *target, void **targ )
/**********************************************************/
{
    switch( target->type ) {
    case FIX_TARGET_SEG:
        *targ = CarveGetIndex( CarveSegData, *targ );
        break;
    case FIX_TARGET_GRP:
        *targ = (void *)(pointer_int)target->u.group->num;
        break;
    case FIX_TARGET_EXT:
        *targ = CarveGetIndex( CarveSymbol, *targ );
        break;
    }
}

static segdata *GetTargetSegData( target_spec *target )
/*****************************************************/
{
    segdata     *sdata;
    seg_leader  *leader;

    sdata = NULL;
    switch( target->type ) {
    case FIX_TARGET_SEG:
        sdata = target->u.sdata;
        break;
    case FIX_TARGET_GRP:
        leader = Ring2First( target->u.group->leaders );
        if( leader != NULL ) {
            sdata = Ring2First( leader->pieces );
        }
        break;
    case FIX_TARGET_EXT:
        sdata = target->u.sym->p.seg;
        break;
    }
    return( sdata );
}

static bool IsReadOnly( segdata *sdata )
/**************************************/
{
    return( sdata->iscode
        || ( sdata->u.leader->group != NULL )
        && (sdata->u.leader->group->segflags & SEG_READ_ONLY) );
}

static void CheckRWData( target_spec *target, targ_addr *addr )
/*************************************************************/
{
    symbol sym;

    if( (FmtData.type & MK_WINDOWS)
      && FmtData.u.os2.chk_seg_relocs
      && IsReadOnly( LastSegData ) ) {
        if( !IS_SYM_IMPORTED( target->u.sym ) && !IsReadOnly( GetTargetSegData( target ) ) ) {
            if( !IS_DBG_INFO( CurrRec.seg->u.leader ) ) {
                if( target->type == FIX_TARGET_SEG ) {
                    sym.name = target->u.sdata->u.leader->segname;
                    LnkMsg( LOC+WRN+MSG_RELOC_TO_RWDATA_SEG, "aS", addr, &sym );
                } else if( target->type == FIX_TARGET_EXT ) {
                    LnkMsg( LOC+WRN+MSG_RELOC_TO_RWDATA_SEG, "aS", addr, target->u.sym );
                }
            }
        }
    }
}

static bool IsTargAbsolute( target_spec *target )
/***********************************************/
{
    return( ( target->type == FIX_TARGET_SEG ) && target->u.sdata->isabs
        || ( target->type == FIX_TARGET_EXT ) && (target->u.sym->info & SYM_ABSOLUTE) );
}

static void BuildReloc( save_fixup *save, target_spec *target, frame_spec *frame )
/********************************************************************************/
{
    fix_relo_data   fix;
    targ_addr       faddr;
    fix_type        fixtype = save->u.fixup.flags;
    offset          off = save->u.fixup.off;

    faddr.off = 0;
    faddr.seg = 0;
    memset( &fix, 0, sizeof( fix_relo_data ) );        // to get all bitfields 0
    GetTargetAddr( target, &fix.tgt_addr, off );
    GetFrameAddr( frame, &faddr, &fix.tgt_addr, off );
    fix.type = fixtype;
    fix.loc_addr = CurrRec.addr;
    fix.loc_addr.off += off;

    if( target->type == FIX_TARGET_EXT ) {
        if( target->u.sym->info & SYM_TRACE ) {
            RecordTracedSym( target->u.sym );
        }
        if( !(target->u.sym->info & SYM_DEFINED) ) {
            ProcUndefined( target->u.sym );
            return;
        }
        fix.ffix = GET_FFIX_VALUE( target->u.sym );
        if( IS_SYM_IMPORTED( target->u.sym ) ) {
            if( FRAME_HAS_DATA( frame->type ) && ( target->u.sym != frame->u.sym ) ) {
                if( FmtData.type & (MK_NOVELL | MK_OS2_FLAT | MK_PE) ) {
                    fix.tgt_addr.seg = faddr.seg;
                    fix.tgt_addr.off = 0;
                } else {
                    LnkMsg( LOC+ERR+MSG_FRAME_EQ_TARGET, "a", &fix.loc_addr );
                }
            }
            fix.imported = TRUE;
        }
    }
    if( fixtype & FIX_BASE ) {
        if( FmtData.type & (MK_PROT_MODE & ~(MK_OS2_FLAT | MK_PE)) ) {
            if( faddr.seg != fix.tgt_addr.seg ) {
                if( FmtData.type & MK_ID_SPLIT ) {
                    LnkMsg( LOC+ERR+MSG_NOV_NO_CODE_DATA_RELOC, "a", &fix.loc_addr );
                } else {
                    LnkMsg( LOC+ERR+MSG_FRAME_EQ_TARGET, "a", &fix.loc_addr );
                }
            }
        }
        if( !fix.imported ) {
            CheckRWData( target, &fix.loc_addr );
        }
    }
    if( !fix.imported ) {
        if( fix.ffix == FFIX_NOT_A_FLOAT ) {
            ConvertToFrame( &fix.tgt_addr, faddr.seg, ( (fixtype & (FIX_OFFSET_8 | FIX_OFFSET_16)) != 0 ) );
        } else {
            fix.tgt_addr.seg = faddr.seg;
        }
    }
    if( (fixtype & (FIX_OFFSET_MASK | FIX_HIGH)) == FIX_HIGH_OFFSET_16 ) {
        fix.tgt_addr.off += FixupOverflow << 16;
    }
    if( IsTargAbsolute( target ) ) {
        fix.type |= FIX_ABS;
    }
    if( FmtData.type & MK_OVERLAYS ) {
        if( ( target->type == FIX_TARGET_EXT )
            && ( (fix.type & FIX_REL) == 0 || FmtData.u.dos.ovl_short ) 
            && target->u.sym->u.d.ovlref
            && ( (target->u.sym->u.d.ovlstate & OVL_VEC_MASK) == OVL_MAKE_VECTOR ) ) {
            // redirect target to appropriate vector entry
            GetVecAddr( target->u.sym->u.d.ovlref, &fix.tgt_addr );
        }
    } else if( FmtData.type & MK_PE ) {
        if( fix.imported ) {
            /*
                Under PE, the imported symbol address is set to the
                transfer code (JMP [xxxxx]) that is generated by the linker.
            */
            fix.tgt_addr = target->u.sym->addr;
            fix.imported = FALSE;
        }
    }

    Relocate( off, &fix, target );
}

unsigned IncExecRelocs( void *_save )
/******************************************/
{
    save_fixup  *save = _save;
    segdata     *sdata;
    target_spec target;
    frame_spec  frame;
    fix_type    fixtype = save->u.fixup.flags;

    if( fixtype & FIX_CHANGE_SEG ) {
        sdata = save->u.sdata.sdata;
        if( LinkFlags & INC_LINK_FLAG ) {
            save->u.sdata.sdata = CarveGetIndex( CarveSegData, sdata );
        }
        if( !sdata->isdead ) {
            LoadObj( sdata );
            LastSegData = sdata;
        } else {
            LastSegData = NULL;
        }
    } else {
        target.type = FIX_GET_TARGET( fixtype );
        target.u.ptr = save->u.fixup.target;
        frame.type = FIX_GET_FRAME( fixtype );
        if( FRAME_HAS_DATA( frame.type ) ) {
            frame.u.ptr = save->u.fixupf.frame;
        }
        UpdateTargetPtr( &target );
        UpdateFramePtr( &frame );
        if( LastSegData != NULL ) {
            BuildReloc( save, &target, &frame );
        }
        if( LinkFlags & INC_LINK_FLAG ) {
            MapTargetPtr( &target, &save->u.fixup.target );
            if( FRAME_HAS_DATA( frame.type ) ) {
                MapFramePtr( &frame, &save->u.fixupf.frame );
            }
        }
    }
    return( CalcSavedFixSize( fixtype ) );
}

static void FixFrameValue( frame_type type, void **targ )
/*******************************************************/
{
    switch( type ) {
    case FIX_FRAME_SEG:
        *targ = CarveMapIndex( CarveSegData, *targ );
        break;
    case FIX_FRAME_EXT:
        *targ = CarveMapIndex( CarveSymbol, *targ );
        break;
    }
}

static void FixTargetValue( target_type type, void **targ )
/*********************************************************/
{
    switch( type ) {
    case FIX_TARGET_SEG:
        *targ = CarveMapIndex( CarveSegData, *targ );
        break;
    case FIX_TARGET_EXT:
        *targ = CarveMapIndex( CarveSymbol, *targ );
        break;
    }
}

unsigned RelocMarkSyms( void *_fix )
/*****************************************/
{
    save_fixup  *fix = _fix;
    frame_type  frame;
    target_type target;
    symbol      *sym;
    fix_type    fixtype = fix->u.fixup.flags;

    if( fixtype & FIX_CHANGE_SEG ) {
        fix->u.sdata.sdata = CarveMapIndex( CarveSegData, fix->u.sdata.sdata );
    } else {
        frame = FIX_GET_FRAME( fixtype );
        if( FRAME_HAS_DATA( frame ) ) {
            FixFrameValue( frame, &fix->u.fixupf.frame );
        }
        target = FIX_GET_TARGET( fixtype );
        FixTargetValue( target, &fix->u.fixup.target );
        if( target == FIX_TARGET_EXT ) {
            sym = (symbol *)fix->u.fixup.target;
            sym->info |= SYM_RELOC_REFD;
            sym = UnaliasSym( ST_FIND, sym );
            sym->info |= SYM_RELOC_REFD;
        }
    }
    return( CalcSavedFixSize( fixtype ) );
}

static bool MemIsZero( unsigned_8 *mem, unsigned size )
/*****************************************************/
{
    while( size-- > 0 ) {
        if( *mem != 0 )
            return( FALSE );
        mem++;
    }
    return( TRUE );
}

void StoreFixup( offset off, fix_type type, frame_spec *frame, target_spec *target, offset addend )
/*************************************************************************************************/
{
    save_fixup      save;
    fix_relo_data   fix;
    unsigned        size;
    unsigned_8      buff[MAX_ADDEND_SIZE];

    if( LastSegData != CurrRec.seg ) {
        DbgAssert( CurrRec.seg != NULL );
        LastSegData = CurrRec.seg;
        save.u.fixup.flags = FIX_CHANGE_SEG;
        save.u.sdata.sdata = CurrRec.seg;
        PermSaveFixup( &save, sizeof( sdata_t ) );
    }
    save.u.fixup.flags = type | FIX_SET_TARGET( target->type ) | FIX_SET_FRAME( frame->type );
    save.u.fixup.off = off + CurrRec.obj_offset;
    save.u.fixup.target = target->u.ptr;
    if( ObjFormat & FMT_UNSAFE_FIXUPP ) {
        save.u.fixup.flags |= FIX_UNSAFE;
    }
    size = CalcFixupSize( type );
    if( CurrRec.data != NULL ) {
        memcpy( buff, CurrRec.data + off, size );
    } else {
        ReadInfo( CurrRec.seg->u1.vm_ptr + save.u.fixup.off, buff, size );
    }
    fix.type = type;
    fix.data = buff;
    if( (type & (FIX_OFFSET_MASK | FIX_HIGH)) == FIX_HIGH_OFFSET_16 ) {
        addend += FixupOverflow << 16;
    }
    PatchOffset( &fix, addend, TRUE );
    if( MemIsZero( buff, size ) ) {
        save.u.fixup.flags |= FIX_ADDEND_ZERO;
    }
    if( FRAME_HAS_DATA( frame->type ) ) {
        save.u.fixupf.frame = frame->u.ptr;
        PermSaveFixup( &save, sizeof( fixupf_t ) );
    } else {
        PermSaveFixup( &save, sizeof( fixup_t ) );
    }
    if( !(save.u.fixup.flags & FIX_ADDEND_ZERO) )  {
        PermSaveFixup( buff, CalcAddendSize( save.u.fixup.flags ) );
    }
    TraceFixup( save.u.fixup.flags, target );
    if( CurrRec.data != NULL ) {
        memcpy( CurrRec.data + off, buff, size );
    } else {
        PutInfo( CurrRec.seg->u1.vm_ptr + save.u.fixup.off, buff, size );
    }
}

unsigned IncSaveRelocs( void *_save )
/***********************************/
{
    save_fixup  *save = _save;
    segdata     *sdata;
    target_spec target;
    unsigned    fixsize;
    unsigned    datasize;
    char        *data;
    fix_type    fixtype = save->u.fixup.flags;

    fixsize = CalcSavedFixSize( fixtype );
    if( fixtype & FIX_CHANGE_SEG ) {
        sdata = save->u.sdata.sdata;
        if( !sdata->isdead ) {
            LastSegData = sdata;
        } else {
            LastSegData = NULL;
        }
    } else if( LastSegData != NULL ) {
        target.type = FIX_GET_TARGET( fixtype );
        target.u.ptr = save->u.fixup.target;
        datasize = CalcFixupSize( fixtype );
        if( fixtype & FIX_ADDEND_ZERO ) {
            PutInfoNulls( LastSegData->u1.vm_ptr + save->u.fixup.off, datasize  );
        } else {
            if( FRAME_HAS_DATA( FIX_GET_FRAME( fixtype ) ) ) {
                data = (char *)save + sizeof( fixupf_t );
            } else {
                data = (char *)save + sizeof( fixup_t );
            }
            PutInfo( LastSegData->u1.vm_ptr + save->u.fixup.off, data, datasize  );
        }
        TraceFixup( fixtype, &target );
    }
    PermSaveFixup( save, fixsize );
    return( fixsize );
}

static void DumpReloc( base_reloc *curr )
/***************************************/
{
    if( curr->isfloat ) {
        FloatReloc( &curr->item );
    } else if( curr->isqnxlinear ) {
        QNXLinearReloc( CurrRec.seg->u.leader->group, &curr->item );
    } else {
        WriteReloc( CurrRec.seg->u.leader->group, curr->fix_off,
                    &curr->item, curr->rel_size );
        if( FmtData.type & MK_OS2_FLAT ) {
            if( OSF_PAGE_SIZE - ( curr->fix_off & OSF_PAGE_MASK ) < curr->fix_size ) {
                /* stupid relocation has been split across two
                    pages, have to duplicate the entry */
                curr->item.os2f.fmt.r32_soff -= OSF_PAGE_SIZE;
                WriteReloc( CurrRec.seg->u.leader->group,
                            curr->fix_off + OSF_PAGE_SIZE, &curr->item,
                            curr->rel_size );
            }
        }
    }
}

static void InitReloc( base_reloc *reloc )
/****************************************/
{
    reloc->isfloat = FALSE;
    reloc->isqnxlinear = FALSE;
    reloc->rel_size = FmtRelocSize;
}

static unsigned FindGroupIdx( segment seg )
/******************************************/
{
    group_entry *group;
    unsigned    index;

    index = 1;
    for( group = Groups; group != NULL; group = group->next_group ) {
        if( group->grp_addr.seg == seg ) {
            return( index );
        }
        index++;
    }
    return( 0 );
}

static void PatchOffset( fix_relo_data *fix, offset val, bool isdelta )
/*********************************************************************/
/* patch offsets into the loaded object */
{
    byte        *code;
    offset      oldval;

    FixupOverflow = 0;
    if( val == 0 )
        return;
    code = fix->data;
    if( fix->type & FIX_SHIFT ) {
        if( val & 3 ) {
            LnkMsg( LOC+WRN+MSG_REL_NOT_ALIGNED, "a", &fix->loc_addr );
        }
        val >>= 2;
    }
    switch( fix->type & FIX_OFFSET_MASK ) {
    case FIX_OFFSET_8:
        if( fix->type & FIX_HIGH ) {
            val >>= 8;
        }
        PUT_U8( code, GET_U8( code ) + val );
        break;
    case FIX_OFFSET_16:
        if( fix->type & FIX_HIGH ) {
            val >>= 16;
        }                       // NOTE the fall through
    case FIX_NO_OFFSET:         // used for FIX_BASE
        if( fix->type & FIX_SIGNED ) {
            oldval = val;
            val += GET_S16( code );
            if( isdelta ) {
                if( (soffset)val > 0x7FFF ) {
                    val = val & 0xFFFF;
                    if( val > 0x7FFF ) {
                        FixupOverflow = 1;
                        val -= 0x10000;
                    }
                }
            } else {
                if( ( oldval >> 16 ) < ( val >> 16 ) ) {
                    FixupOverflow = 1;
                } else if( (fix->type & FIX_SIGNED)
                    && ( oldval >> 16 ) == ( val >> 16 )
                    && ( (signed_16)( val & 0xFFFF ) < 0 ) ) {
                    FixupOverflow = 1;
                }
            }
        } else {
            val += GET_U16( code );
        }
        PUT_U16( code, val );
        break;
    case FIX_OFFSET_21:
        oldval = GET_U32( code );
        val += oldval;
        oldval &= 0xFFE00000;
        val &= 0x001FFFFF;
        PUT_U32( code, oldval | val );
        break;
    case FIX_OFFSET_26:     // Processing is the same, except FIX_OFFSET_26
    case FIX_OFFSET_24:     // uses FIX_SHIFT (it's really a 28-bit offset)
        oldval = GET_U32( code );
        val += oldval;
        oldval &= 0xFC000000;
        val &= 0x03FFFFFF;
        PUT_U32( code, oldval | val );
        break;
    case FIX_OFFSET_32:
        PUT_U32( code, GET_U32( code ) + val );
        break;
    default:
        LnkMsg( LOC+ERR+MSG_BAD_RELOC_TYPE, NULL );
    }
}

static void MakeQNXFloatReloc( fix_relo_data *fix )
/*************************************************/
{
    base_reloc  new_reloc;

    if( FmtData.u.qnx.gen_seg_relocs ) {
        InitReloc( &new_reloc );
        new_reloc.isfloat = TRUE;
        new_reloc.item.qnx.reloc_offset = fix->loc_addr.off
                               | ( (unsigned_32)fix->ffix << 28 );
        new_reloc.item.qnx.segment = ToQNXIndex( fix->loc_addr.seg );
        DumpReloc( &new_reloc );
    }
}

static byte WinFFixMap[] = {
    0,
    WIN_FFIX_WR_SYMBOL,
    WIN_FFIX_DR_SYMBOL,
    WIN_FFIX_ES_OVERRIDE,
    WIN_FFIX_CS_OVERRIDE,
    WIN_FFIX_SS_OVERRIDE,
    WIN_FFIX_DS_OVERRIDE
};

static void MakeWindowsFloatReloc( fix_relo_data *fix )
/*****************************************************/
{
    base_reloc      new_reloc;
    os2_reloc_item  *os2item;

    InitReloc( &new_reloc );
    os2item = &new_reloc.item.os2;
    os2item->addr_type = MapOS2FixType( fix->type );
    os2item->reloc_offset = fix->loc_addr.off
                        - CurrRec.seg->u.leader->group->grp_addr.off;
    os2item->reloc_type = OSFIXUP | ADDITIVE;
    os2item->put.fltpt = WinFFixMap[fix->ffix];
    DumpReloc( &new_reloc );
}

static offset GetSegOff( segdata *sdata )
/***************************************/
{
    return( sdata->a.delta + sdata->u.leader->seg_addr.off );
}

static void CheckPartialRange( fix_relo_data *fix, offset off,
                               unsigned_32 mask, unsigned_32 topbit )
/*******************************************************************/
{
    unsigned_32 utemp;
    signed_32   temp;

    utemp = GET_U32( fix->data ) & mask;
    if( utemp & topbit ) {      // it is negative
        utemp |= ~mask; // do a bogus sign extension
    }
    temp = utemp;
    if( fix->type & FIX_SHIFT ) {
        temp += off / 4;
    } else {
        temp += off;
    }
    if( ( temp < -topbit ) || ( temp >= topbit ) ) {
        LnkMsg( LOC+ERR+MSG_FIXUP_OFF_RANGE, "a", &fix->loc_addr );
    }
}

static bool CheckSpecials( fix_relo_data *fix, target_spec *target )
/****************************************************************/
{
    offset      off;
    offset      pos;
    unsigned    fixsize;
    group_entry *group;
    segdata     *sdata;
    fix_type    special;

    if( FmtData.type & MK_ELF ) {
        if( !(fix->type & FIX_REL) )
            return( FALSE );
#if 0
    XXX: this is not the right thing to do for elf-i386
        if( fix->loc_addr.seg != fix->tgt_addr.seg )
            return( FALSE );
#endif
    }
    if( (FmtData.type & (MK_QNX | MK_WINDOWS))
        && ( fix->ffix != FFIX_NOT_A_FLOAT ) ) {
        if( fix->ffix != FFIX_IGNORE ) {
            if( FmtData.type & MK_QNX ) {
                MakeQNXFloatReloc( fix );
            } else {
                MakeWindowsFloatReloc( fix );
            }
        }
        return( TRUE );
    }
    special = fix->type & FIX_SPECIAL_MASK;
    if( ( special == FIX_TOC ) || ( special == FIX_TOCV ) ) {
        if( special == FIX_TOCV ) {
            DbgAssert( target->type == FIX_TARGET_EXT );
            pos = FindSymPosInTocv( target->u.sym );
        } else {
            if( target->type  == FIX_TARGET_EXT ) {
                pos = FindSymPosInToc( target->u.sym );
            } else {
                sdata = GetTargetSegData( target );
                off = fix->tgt_addr.off - GetSegOff( sdata );
                pos = FindSdataOffPosInToc( sdata, off );
            }
        }
        DbgVerify( ( pos % 4 ) == 0, "symbol not in toc" );
        if( fix->type & FIX_OFFSET_16 ) {
            PUT_U16( fix->data, pos );
        } else {
            PUT_U32( fix->data, pos );
        }
        return( TRUE );
    } else if( special == FIX_IFGLUE ) {
        if( ( target->type == FIX_TARGET_EXT ) && IS_SYM_IMPORTED( target->u.sym ) ) {
            enum { TOC_RESTORE_INSTRUCTION = 0x804b0004 };
            PUT_U32( fix->data, TOC_RESTORE_INSTRUCTION );
        }
        return( TRUE );
    }
    if( !(fix->type & FIX_REL) )
        return( FALSE );
    if( FmtData.type & MK_OS2_FLAT ) {
        /* OS/2 V2 relative fixups to imported items or other objects
            require special handling */
        if( fix->imported )
            return( FALSE );
        if( fix->loc_addr.seg != fix->tgt_addr.seg ) {
            return( FALSE );
        }
    }
    if( (fix->type & FIX_ABS) && !(FmtData.type & MK_QNX) ) {
        LnkMsg( LOC+ERR+MSG_BAD_ABS_FIXUP, "a", &fix->loc_addr );
        return( TRUE );
    }
    if( fix->type & FIX_BASE ) {
        LnkMsg( LOC+ERR+MSG_BAD_REL_FIXUP, "a", &fix->loc_addr );
        return( TRUE );
    }
    if( fix->imported ) {
        if( FmtData.type & MK_OS2_16BIT ) {  // can not get at a DLL relatively
            LnkMsg( LOC+ERR+MSG_DLL_IN_REL_RELOC, "a", &fix->loc_addr );
        } else if( FmtData.type & MK_ELF ) {
            return( FALSE );
        } else if( FmtData.type & MK_NOVELL ) {
            if( ( (fix->type & FIX_OFFSET_MASK) != FIX_OFFSET_32 )
                || (fix->type & FIX_BASE) ) {
                LnkMsg( LOC+ERR+MSG_BAD_IMP_REL_RELOC, "a", &fix->loc_addr );
            } else {                           // TRUE == isrelative.
                AddNovImpReloc( target->u.sym, fix->loc_addr.off, TRUE,
                                fix->loc_addr.seg == DATA_SEGMENT );
// I don't know why the novell linker does this, but it does.
                PatchOffset( fix, -4, TRUE );
            }
        }
        return( TRUE );
    }
    /* XXX: MK_ELF must not be included for non-i386 */
    if( FmtData.type & (MK_PROT_MODE & ~(MK_OS2_FLAT | MK_PE | MK_ELF)) ) {
        if( ( fix->loc_addr.seg != fix->tgt_addr.seg ) && !(fix->type & FIX_ABS) ) {
            //must have same file segment.
            if( FmtData.type & MK_ID_SPLIT ) {
                LnkMsg( LOC+ERR+MSG_NOV_NO_CODE_DATA_RELOC, "a",
                                                        &fix->loc_addr );
            } else {
                LnkMsg( LOC+ERR+MSG_REL_NOT_SAME_SEG, "a", &fix->loc_addr );
            }
            return( TRUE );
        }
    }
    if( FmtData.type & MK_LINEARIZE ) {
        group = FindGroup( fix->tgt_addr.seg );
        if( group == NULL ) {           // can happen for IAT symbols.
            off = 0;
        } else {
            off = group->linear;
        }
        group = FindGroup( fix->loc_addr.seg );
        if( group != NULL ) {
            off -= group->linear;
        }
        off += fix->tgt_addr.off - fix->loc_addr.off;
    } else if( FmtData.type & (MK_32BIT | MK_QNX) ) {
        off = fix->tgt_addr.off - fix->loc_addr.off;
        if( fix->type & FIX_ABS ) {
            off -= FindGroup( fix->loc_addr.seg )->linear;
        }
#ifdef _DOS16M
    } else if( FmtData.type & MK_DOS16M ) {
        off = SUB_16M_ADDR( fix->tgt_addr, fix->loc_addr );
#endif
    } else {
        off = SUB_ADDR( fix->tgt_addr, fix->loc_addr );
    }
    fixsize = CalcFixupSize( fix->type );
    if ( !(fix->type & FIX_NOADJ) ) {
        off -= fixsize;
    }
    if( fix->type == FIX_OFFSET_16 ) {
        if( off + ( fix->loc_addr.off + fixsize ) >= 0x10000 ) {
            LnkMsg( LOC+ERR+MSG_FIXUP_OFF_RANGE, "a", &fix->loc_addr );
        }
    } else if( fix->type == FIX_OFFSET_21 ) {
        CheckPartialRange( fix, off, 0x001FFFFF, 0x00100000 );
    } else if( fix->type == FIX_OFFSET_24 ) {   // NYI: repair PPC damage here
        CheckPartialRange( fix, off, 0x03FFFFFF, 0x02000000 );
    }
    PatchOffset( fix, off, TRUE );
    fix->os2_selfrel = (FmtData.type & MK_OS2_LX) && FmtData.u.os2.gen_rel_relocs;
    return( !( fix->os2_selfrel || (fix->type & FIX_ABS) ) );
}

static offset FindRealAddr( fix_relo_data *fix )
/**********************************************/
{
    group_entry *group;
    offset      off;
    bool        dbiflat;

    off = fix->tgt_addr.off;
    dbiflat = DBINoReloc( CurrRec.seg->u.leader );
    if( (fix->type & FIX_ABS)
        || dbiflat && !(CurrMod->modinfo & MOD_FLATTEN_DBI) ) {
        return( off );
    }
    if( (FmtData.type & (MK_OS2_FLAT | MK_LINEARIZE | MK_QNX_FLAT))
        && !(fix->type & FIX_SEC_REL) ) {
        if( (FmtData.type & (MK_OS2_LE | MK_WIN_VXD)) && !dbiflat )
            return( off );
       /*
            put the correct value in so that internal fixups
            don't have to be applied if everything loads in the
            right spot.
        */
        group = FindGroup( fix->tgt_addr.seg );
        if( group == NULL )                  // can happen with dbi relocs
            return( off );
        off += ( group->linear - group->grp_addr.off );
        if( FmtData.type & MK_LINEARIZE ) {
            if( !(fix->type & FIX_NO_BASE) ) {
                off += FmtData.base;
            }
            // want dbi addresses relative to 0, but not for ELF
            // (perhaps absolute for others too, later)
            if( dbiflat && !(FmtData.type & MK_ELF) ) {
                off -= Groups->linear;
            }
        }
        if( dbiflat && !(FmtData.type & MK_ELF) ) {
            off -= FmtData.base;
        }
    }
    return( off );
}

static void PatchData( fix_relo_data *fix )
/*****************************************/
{
    byte        *data;
    segment     segval;
    bool        isdbi;

    data = fix->data;
    fix->additive = FALSE;
    if( fix->imported && (FmtData.type & MK_OS2_FLAT) ) {
        fix->value = fix->tgt_addr.off;
    } else {
        fix->value = 0;
    }
    switch( fix->type & FIX_OFFSET_MASK ) {
    case FIX_OFFSET_8:
        fix->value += GET_U8( data );
        break;
    case FIX_OFFSET_16:
        fix->value += GET_U16( data );
        break;
    case FIX_OFFSET_32:
        fix->value += GET_U32( data );
        break;
    }
    /* Catch the ELF FIX_NOADJ cases CheckSpecials didn't handle. */
    /* NB: Other fixup types besides LX imports likely need the same treatment. */
    if( (fix->type & FIX_NOADJ) && fix->imported && (FmtData.type & MK_OS2_FLAT) ) {
        fix->value += CalcFixupSize( fix->type );
        fix->type  &= FIX_NOADJ;    /* This flag isn't interesting anymore. */
    }
    if( fix->value != 0 )
        fix->additive = TRUE;
    if( fix->type & FIX_REL )
        return;
    isdbi = DBINoReloc( CurrRec.seg->u.leader );
    if( fix->imported ) {
        if( isdbi ) {
            fix->tgt_addr.off = 0;      // crazy person has imports in the dbi
            fix->tgt_addr.seg = 0;
        } else {
            return;
        }
    }
    if( (fix->type & FIX_ABS) && !(FmtData.type & (MK_QNX | MK_DOS16M)) || isdbi ) {
        fix->done = TRUE;
    }
    if( !(fix->type & FIX_BASE) ) {     // it's offset only
        if( !( (FmtData.type & MK_WINDOWS) && (fix->type & FIX_LOADER_RES) ) ) {
            PatchOffset( fix, FindRealAddr( fix ), FALSE );
            if( !(FmtData.type & (MK_ELF | MK_QNX | MK_PE | MK_OS2_FLAT | MK_NOVELL | MK_PHAR_REX | MK_ZDOS | MK_RAW))
                || (FmtData.type & MK_OS2_LX) && !FmtData.u.os2.gen_int_relocs ) {
                fix->done = TRUE;
            }
        }
    } else {    // its a seg reloc and maybe an offset as well.
        if( (fix->type & FIX_OFFSET_MASK) != FIX_NO_OFFSET ) {
            if( !fix->done && (FmtData.type & (MK_OS2 | MK_WIN_VXD)) )
                return;
            PatchOffset( fix, FindRealAddr( fix ), FALSE );
            data += OffsetSizes[FIX_GET_OFFSET( fix->type )];
        }
        if( FmtData.type & MK_PROT_MODE ) {
            PUT_U16( data, 0 );
        }
        if( FmtData.type & (MK_DOS16M | MK_PHAR_MULTISEG) ) {
            PUT_U16( data, fix->tgt_addr.seg );
        } else if( fix->done || (FmtData.type & (MK_QNX | MK_DOS | MK_RDOS)) ) {
            if( FmtData.type & MK_RDOS ) {
                segval = GET_U16( data ) + fix->tgt_addr.seg;
                if( segval == FmtData.u.rdos.code_seg ) {
                    segval = FmtData.u.rdos.code_sel;
                } else if( segval == FmtData.u.rdos.data_seg ) {
                    segval = FmtData.u.rdos.data_sel;
                } else {
                    LnkMsg( LOC+ERR+MSG_BAD_RELOC_TYPE, NULL );
                }
                if( segval == 0 ) {            
                    LnkMsg( LOC+ERR+MSG_BAD_RELOC_TYPE, NULL );
                }
            } else if( isdbi && (LinkFlags & CV_DBI_FLAG) ) {    // FIXME
                segval = FindGroupIdx( fix->tgt_addr.seg );
            } else if( fix->type & FIX_ABS ) {
                /* MASM 5.1 stuffs abs seg length in displacement; ignore it like LINK. */
                segval = fix->tgt_addr.seg;
            } else {
                segval = GET_U16( data ) + fix->tgt_addr.seg;
            }
            PUT_U16( data, segval );
        }
    }
}

#define FAR_CALL_ID     0x9A
#define FAR_JMP_ID      0xEA
#define NOP_ID          0x90
#define SS_OVERRIDE     0x36
#define CS_OVERRIDE     0x3e
#define PUSHCS_ID       0x0e
#define NEAR_CALL_ID    0xe8
#define NEAR_JMP_ID     0xE9
#define MOV_AXAX_ID     0xC089

static bool FarCallOpt( fix_relo_data *fix )
/******************************************/
{
    byte        *code;
    unsigned_16 temp16 = 0;
    unsigned_32 temp32 = 0;
    byte        instruction;
    bool        is32bit;

/*
 * the sequence of instructions NOP  PUSH CS (near)CALL offset is much
 * faster than (far)CALL segment:offset. So whenever a far call ends up in
 * the same segment as the thing that it is calling, replace the far call
 * with the near call sequence. Note this can potentially wreck jump tables,
 * so rely on the code generator to say when it is safe to do so.
 * this also positions the call so that it returns on an even address
 * to make things go slightly faster. This uses segment overrides rather than
 * NOP's where possible since they are faster, and if there are two far
 * calls in a row, it detects this and uses segment overrides on both the
 * push and the call to avoid the pair of NOP's
 * this also replaces far JMP's with (near) JMP MOV AX,AX
 */

    // optimization is valid only for Intel CPU
    if( LinkState & (HAVE_MACHTYPE_MASK & ~HAVE_I86_CODE) )
        return( FALSE );
    if( fix->type & FIX_UNSAFE )
        return( FALSE );
    if( fix->imported )
        return( FALSE );
    if( fix->type & FIX_ABS )
        return( FALSE );
    if( (fix->type & FIX_POINTER_MASK) == FIX_BASE_OFFSET_16 ) {
        is32bit = FALSE;
    } else if( (fix->type & FIX_POINTER_MASK) == FIX_BASE_OFFSET_32 ) {
        is32bit = TRUE;
    } else {
        return( FALSE );
    }
    if( fix->data == CurrRec.data )  // can we assume this?
        return( FALSE );
    if( fix->loc_addr.seg != fix->tgt_addr.seg )
        return( FALSE );
    if( !(CurrRec.seg->canfarcall || CurrRec.seg->iscode) )
        return( FALSE );
    code = fix->data - 1;
    instruction = GET_U8( code );
    if( ( instruction == FAR_CALL_ID ) || ( instruction == FAR_JMP_ID ) ) {
        fix->done = TRUE;
        if( is32bit ) {
            temp32 = GET_U32( code + 1 );
        } else {
            temp16 = GET_U16( code + 1 );
        }
        if( FmtData.type & (MK_OS2 | MK_WIN_VXD) ) {
            if( is32bit ) {
                temp32 += fix->tgt_addr.off;     // haven't done this for OS/2
            } else {
                temp16 += fix->tgt_addr.off;     // haven't done this for OS/2
            }
        }
        if( instruction == FAR_JMP_ID ) {
            PUT_U8( code, NEAR_JMP_ID );
            if( is32bit ) {
                temp32 -= fix->loc_addr.off + 4;   //fix the offset
                PUT_U32( code + 1, temp32 );
                PUT_U16( code + 5, MOV_AXAX_ID );
            } else {
                temp16 -= fix->loc_addr.off + 2;   //fix the offset
                PUT_U16( code + 1, temp16 );
                PUT_U16( code + 3, MOV_AXAX_ID );
            }
        } else {
            PUT_U8( code, PUSHCS_ID );
            if( is32bit ) {
                temp32 -= fix->loc_addr.off + 6; //fix the offset
            } else {
                temp16 -= fix->loc_addr.off + 4; //fix the offset
            }
            if( fix->loc_addr.off & 0x1 ) {  // odd address, so put nop last
                PUT_U8( code + 1, NEAR_CALL_ID );
                if( is32bit ) {
                    PUT_U8( code + 6, NOP_ID );
                    temp32 += 1;            // adjust for different call loc.
                } else {
                    PUT_U8( code + 4, NOP_ID );
                    temp16 += 1;            // adjust for different call loc.
                }
                code -= 1;
                LastOptimized = fix->loc_addr.off;
                LastOptType = fix->type & FIX_POINTER_MASK;
            } else {
                if( ( LastOptType == FIX_BASE_OFFSET_16 )
                    && ( fix->loc_addr.off >= 5 )
                    && ( LastOptimized == fix->loc_addr.off - 5 )
                    || ( LastOptType == FIX_BASE_OFFSET_32 )
                    && ( fix->loc_addr.off >= 7 )
                    && ( LastOptimized == fix->loc_addr.off - 7 ) ) {
                    PUT_U8( code - 1, SS_OVERRIDE );
                }
                PUT_U8( code + 1, CS_OVERRIDE );
                PUT_U8( code + 2, NEAR_CALL_ID );
            }
            if( is32bit ) {
                PUT_U32( code + 3, temp32 );
            } else {
                PUT_U16( code + 3, temp16 );
            }
        }
        return( TRUE );
    }
    return( FALSE );
}

static void MakeBase( fix_relo_data *fix )
/****************************************/
{
    unsigned size;

    if( fix->type & FIX_BASE ) {
        size = OffsetSizes[FIX_GET_OFFSET( fix->type )];
        fix->loc_addr.off += size;
    }
}

/* 00h = Byte fixup (8-bits).
   01h = (undefined).
   02h = 16-bit Selector fixup (16-bits).
   03h = 16:16 Pointer fixup (32-bits).
   04h = (undefined).
   05h = 16-bit Offset fixup (16-bits).
   06h = 16:32 Pointer fixup (48-bits).
   07h = 32-bit Offset fixup (32-bits).
*/

static byte OS2OffsetFixTypeMap[] = { 1, 0, 5, 1, 7, 1};
static byte OS2SegmentedFixTypeMap[] = { 2, 1, 3, 1, 6, 1};

static unsigned MapOS2FixType( fix_type type )
/********************************************/
{
    unsigned    off;

    off = FIX_GET_OFFSET( type );
    if( type & FIX_BASE ) {
        return( OS2SegmentedFixTypeMap[off] );
    }
    return( OS2OffsetFixTypeMap[off] );
}

static void FmtReloc( fix_relo_data *fix, target_spec *tthread )
/**************************************************************/
{
    offset              off;
    base_reloc          new_reloc;
    targ_addr           target;
    group_entry         *grp;
    segdata             *seg;
    bool                save;
    byte                *fixptr;
    dll_sym_info        *dll;
    bool                freedll;
    fix_type            ftype;

    if( fix->done )
        return;
    if( (fix->type & FIX_ABS) && !(FmtData.type & (MK_QNX | MK_DOS16M))
        && !fix->imported )
        return;
    ftype = fix->type & (FIX_OFFSET_MASK | FIX_BASE);
    if( (FmtData.type & (MK_PHAR_SIMPLE | MK_PHAR_FLAT))
        || (FmtData.type & (MK_NOVELL | MK_ELF))
            && (LinkState & HAVE_I86_CODE) && (ftype != FIX_OFFSET_32)
        || (FmtData.type & MK_ELF) && !(LinkState & HAVE_I86_CODE)
            && (ftype & (FIX_BASE | FIX_OFFSET_8))
        || (FmtData.type & MK_PE) && (ftype & (FIX_BASE | FIX_OFFSET_8))
        || ((FmtData.type & (MK_PHAR_REX | MK_ZDOS | MK_RAW)) && (ftype != FIX_OFFSET_16)
            && (ftype != FIX_OFFSET_32)) ) {
        LnkMsg( LOC+ERR+MSG_INVALID_FLAT_RELOC, "a", &fix->loc_addr );
        return;
    }
    if( !(LinkState & MAKE_RELOCS) )
        return;
    if( FmtData.type & MK_QNX ) {
        if( ftype == FIX_OFFSET_32 ) {
            if( !FmtData.u.qnx.gen_linear_relocs ) {
                return;
            }
        } else if( ( ftype == FIX_BASE ) || ( ftype == FIX_BASE_OFFSET_16 ) ) {
            if( !FmtData.u.qnx.gen_seg_relocs ) {
                return;
            }
        } else if( ftype != FIX_BASE_OFFSET_32 ) {
            return;
        }
    }
    if( (fix->type & FIX_HIGH) && !(FmtData.type & MK_PE)
        && !(FmtData.type & MK_ELF) ) {
        LnkMsg( LOC+ERR+MSG_BAD_RELOC_TYPE, NULL );
        return;
    }
    DEBUG(( DBG_OLD, "relocation record being output" ));
    save = TRUE;                /* NOTE: copycat code in QNX section ! */
    InitReloc( &new_reloc );
    new_reloc.fix_size = CalcFixupSize( fix->type );
    new_reloc.fix_off = fix->loc_addr.off;
    off = fix->loc_addr.off;
    seg = CurrRec.seg;
    target = fix->tgt_addr;
    if( FmtData.type & MK_PE ) {
        unsigned_32     reltype;

        off += seg->u.leader->group->linear;
        if( fix->type & FIX_HIGH ) {
            reltype = PE_FIX_HIGHADJ;   // NYI: can be high when objalign ==
            new_reloc.rel_size = sizeof( high_pe_reloc_item );    // 0x10000
            new_reloc.item.hpe.low_off = (unsigned_16)target.off;
        } else if( ftype == FIX_OFFSET_16 ) {
            if( !(FmtData.objalign & 0xFFFF) ) {
                save = FALSE;
            }
            reltype = PE_FIX_LOW;
        } else {
            reltype = PE_FIX_HIGHLOW;
        }
        new_reloc.item.pe = ( off & OSF_PAGE_MASK ) | reltype;
    } else if( FmtData.type & MK_OS2_16BIT ) {
        os2_reloc_item  *os2item;

        fixptr = fix->data;
        os2item = &new_reloc.item.os2;
        os2item->addr_type = MapOS2FixType( fix->type );
        os2item->reloc_offset = off - seg->u.leader->group->grp_addr.off;
        if( !fix->imported ) {
            os2item->reloc_type = INTERNAL_REFERENCE;
            os2item->put.internal.rsrvd = 0;
            for( grp = Groups; grp != NULL; grp = grp->next_group ) {
                if( grp->grp_addr.seg == target.seg ) {
                    break;
                }
            }
            if( ( grp != NULL ) && (grp->segflags & SEG_MOVABLE) ) {
                os2item->put.internal.grp_num = (signed char)MOVABLE_ENTRY_PNT;
                os2item->put.internal.off = FindEntryOrdinal( target, grp );
            } else {
                os2item->put.internal.grp_num = target.seg;
                os2item->put.internal.off = target.off;
            }
        } else {
            dll = tthread->u.sym->p.import;
            if( !dll->isordinal ) {
                os2item->reloc_type = IMPORTED_NAME;
                os2item->put.name.impnam_off = dll->u.entry->num;
                os2item->put.name.modref_idx = dll->m.modnum->num;
            } else {
                os2item->reloc_type = IMPORTED_ORDINAL;
                os2item->put.ordinal.ord_num = dll->u.ordinal;
                os2item->put.ordinal.modref_idx = dll->m.modnum->num;
            }
        }
        if( fix->additive ) {
            os2item->reloc_type |= ADDITIVE;
        } else {
            PUT_U16( fixptr, 0xffff );          // end of chain indicator
        }
    } else if( FmtData.type & MK_OS2_FLAT )  {
        segdata *targseg;
        byte    flags;
        byte    fixtype;

        fixptr = new_reloc.item.os2f.buff;
        freedll = FALSE;
        dll = NULL;
        if( fix->type & FIX_REL ) {
            fixtype = OSF_32BIT_SELF_REL;
            if( fix->os2_selfrel && (tthread->type & FIX_FRAME_EXT) ) {
                freedll = FindOS2ExportSym( tthread->u.sym, &dll );
                if( dll != NULL ) {
                    fix->imported = TRUE;
                }
            }
        } else {
            fixtype = MapOS2FixType( fix->type );
        }
        if( IS_SYM_IMPORTED( tthread->u.sym ) ) {
            targseg = NULL;
        } else {
            targseg = GetTargetSegData( tthread );
        }
        if( ( targseg != NULL ) && !targseg->is32bit ) {
            switch( fixtype ) {
            case 2:     // 16-bit selector
            case 6:     // 16:32 pointer
                if( FIX_GET_FRAME( fix->type ) == FIX_FRAME_FLAT ) {
                    break;
                }
            case 3:     // 16:16 pointer           NOTE the fall through
                fixtype |= OSF_FIXUP_TO_ALIAS;  // YET more fall through
            case 5:     // 16-bit offset
                for( grp = Groups; grp != NULL; grp = grp->next_group ) {
                    if( grp->grp_addr.seg == target.seg ) {
                        break;
                    }
                }
                if( grp != NULL ) {
                    grp->u.miscflags |= SEG_16_ALIAS;
                }
                break;
            }
        }
        // ALWAYS set the alias flag for 16:16 pointers!
        if ( fixtype == 3 )
            fixtype |= OSF_FIXUP_TO_ALIAS;

        PUT_U8( fixptr, fixtype );
        flags = 0;
        fixptr += 2;       /* skip flags for now */
        grp = seg->u.leader->group;
        PUT_U16( fixptr, ( fix->loc_addr.off - grp->grp_addr.off ) & OSF_PAGE_MASK );
        fixptr += 2;
        if( !fix->imported ) {
            if( !fix->os2_selfrel ) {
                target.off += fix->value;
            }
            flags = INTERNAL_REFERENCE;
            if( target.seg > 0xFF ) {
                flags |= OSF_OBJMOD_16BITS;
                PUT_U16( fixptr, target.seg );
                fixptr += 1;
            } else {
                PUT_U8( fixptr, target.seg );
            }
            fixptr += 1;
            grp = FindGroup( target.seg );
            target.off -= grp->grp_addr.off;
            if( ftype != FIX_BASE ) {
                if( target.off > 0xFFFF ) {
                    flags |= OSF_TARGOFF_32BITS;
                    PUT_U32( fixptr, target.off );
                    fixptr += 2;
                } else {
                    PUT_U16( fixptr, target.off );
                }
                fixptr += 2;
            }
        } else {
            if( dll == NULL ) {
                dll = tthread->u.sym->p.import;
            }
            if( !dll->isordinal ) {
                flags = IMPORTED_NAME;
            } else {
                flags = IMPORTED_ORDINAL;
            }
            if( dll->m.modnum == NULL ) {
                PUT_U8( fixptr, 0 );
            } else if( dll->m.modnum->num > 0xFF ) {
                flags |= OSF_OBJMOD_16BITS;
                PUT_U16( fixptr, dll->m.modnum->num );
                fixptr += 1;
            } else {
                PUT_U8( fixptr, dll->m.modnum->num );
            }
            fixptr += 1;
            if( !dll->isordinal ) {
                if( dll->u.entry->num > 0xFFFF ) {
                    flags |= OSF_TARGOFF_32BITS;
                    PUT_U32( fixptr, dll->u.entry->num );
                    fixptr += 3;
                } else {
                    PUT_U16( fixptr, dll->u.entry->num );
                    fixptr += 1;
                }
            } else if( dll->u.ordinal > 0xFF ) {
                PUT_U16( fixptr, dll->u.ordinal );
                fixptr += 1;
            } else {
                flags |= OSF_IMPORD_8BITS;
                PUT_U8( fixptr, dll->u.ordinal );
            }
            fixptr += 1;
            if( fix->additive ) {
                flags |= OSF_ADDITIVE;
                switch( fix->type & FIX_OFFSET_MASK ) {
                case FIX_OFFSET_8:
                    PUT_U8( fix->data, 0 );
                    break;
                case FIX_OFFSET_16:
                    PUT_U16( fix->data, 0 );
                    break;
                case FIX_OFFSET_32:
                    PUT_U32( fix->data, 0 );
                    break;
                }
                if( fix->value > 0x7fff ) {
                    flags |= OSF_ADDITIVE32;
                    PUT_U32( fixptr, fix->value );
                    fixptr += 2;
                } else {
                    PUT_U16( fixptr, fix->value );
                }
                fixptr += 2;
            }
        }
        if( freedll )
            _LnkFree( dll );
        new_reloc.rel_size = fixptr - new_reloc.item.os2f.buff;
        new_reloc.item.os2f.fmt.nr_flags = flags;
    } else if( FmtData.type & MK_NOVELL ) {
        if( fix->imported ) {
            save = FALSE;
            AddNovImpReloc( tthread->u.sym, off, (fix->type & FIX_REL) != 0,
                             fix->loc_addr.seg == DATA_SEGMENT );
        } else {
            if( fix->loc_addr.seg != DATA_SEGMENT ) {
                off |= NOV_OFFSET_CODE_RELOC;
            }
            if( target.seg != DATA_SEGMENT ) {
                off |= NOV_TARGET_CODE_RELOC;
            }
            new_reloc.item.novell.reloc_offset = off;
        }
    } else if( FmtData.type & MK_QNX ) {
        bool    done;

        done = FALSE;
        if( ( ftype == FIX_OFFSET_32 ) || ( ftype == FIX_BASE_OFFSET_32 ) ) {
            if( FmtData.u.qnx.gen_linear_relocs ) {
                new_reloc.isqnxlinear = TRUE;
                new_reloc.item.qnxl.reloc_offset = fix->loc_addr.off;
                seg = GetTargetSegData( tthread );
                if( seg->iscode ) {
                    new_reloc.item.qnxl.reloc_offset |= 0x80000000;
                }
                if( (fix->type & FIX_ABS) && (fix->type & FIX_REL) ) {
                    new_reloc.item.qnxl.reloc_offset |= 0x40000000;
                }
                if( fix->type == FIX_BASE_OFFSET_32 ) {
                    DumpReloc( &new_reloc );
                    InitReloc( &new_reloc );
                    new_reloc.fix_size = CalcFixupSize( fix->type );
                    new_reloc.fix_off   = fix->loc_addr.off;
                } else {
                    done = TRUE;
                }
            }
        }
        if( !done ) {
            MakeBase( fix );
            new_reloc.item.qnx.reloc_offset = fix->loc_addr.off;
            new_reloc.item.qnx.segment = ToQNXIndex( fix->loc_addr.seg );
        }
    } else if( FmtData.type & MK_ELF ) {
        symbol *sym;

        if( LinkState & HAVE_I86_CODE ) {
            if( fix->type & FIX_REL ) {
                new_reloc.item.elf.info = R_386_PC32;
            } else {
                new_reloc.item.elf.info = R_386_32;
            }
        } else if( LinkState & HAVE_X64_CODE ) {
            // TODO
        } else if( LinkState & HAVE_PPC_CODE ) {
            if( fix->type & FIX_HIGH ) {
                new_reloc.item.elf.info = R_PPC_ADDR16_HI;
                new_reloc.item.elf.addend = (unsigned_16)target.off;
            } else if( ftype == FIX_OFFSET_16 ) {
                new_reloc.item.elf.info = R_PPC_ADDR16_LO;
                if( !(FmtData.objalign & 0xFFFF) ) {
                    save = FALSE;
                }
            } else {
                new_reloc.item.elf.info = R_PPC_REL32;
                LnkMsg( LOC + ERR + MSG_INVALID_FLAT_RELOC, "a", &fix->loc_addr );
            }
        } else if( LinkState & HAVE_MIPS_CODE ) {
            if( fix->type & FIX_HIGH ) {
                new_reloc.item.elf.info = R_MIPS_HI16;
                new_reloc.item.elf.addend = (unsigned_16)target.off;
            } else if( ftype == FIX_OFFSET_16 ) {
                new_reloc.item.elf.info = R_MIPS_LO16;
                if( !(FmtData.objalign & 0xFFFF) ) {
                    save = FALSE;
                }
            } else if( ftype == FIX_OFFSET_26 ) {
                new_reloc.item.elf.info = R_MIPS_26;
            } else if( ftype == FIX_OFFSET_32 ) {
                new_reloc.item.elf.info = R_MIPS_REL32;
            } else {
                new_reloc.item.elf.info = R_MIPS_REL32;
                LnkMsg( LOC + ERR + MSG_INVALID_FLAT_RELOC, "a", &fix->loc_addr );
            }
        }
        sym = tthread->u.sym;
        if( IS_SYM_ALIAS( sym ) && (sym->info & SYM_WAS_LAZY) ) {
            save = FALSE;
        } else if( (tthread->type & FIX_FRAME_EXT) && IsSymElfImpExp( sym ) ) {
            new_reloc.item.elf.addend = 0;
        } else {
            seg = GetTargetSegData( tthread );
            if( seg == NULL ) {
                save = FALSE;
            } else {
                grp = seg->u.leader->group;
                sym = grp->sym;
                new_reloc.item.elf.addend = target.off - grp->grp_addr.off;
            }
        }
        if( save ) {
            new_reloc.item.elf.info |= FindElfSymIdx( sym ) << 8;
            new_reloc.item.elf.reloc_offset = off;
        }
    } else if( FmtData.type & MK_PHAR_REX ) {
        if( ftype == FIX_OFFSET_32 ) {
            off |= 0x80000000;
        }
        new_reloc.item.rex.reloc_offset = off;
    } else if( FmtData.type & MK_ZDOS ) {
        new_reloc.item.zdos.reloc_offset = off;
    } else if( FmtData.type & MK_COM ) {
        save = FALSE;
        LnkMsg( LOC+WRN+MSG_SEG_RELOC_OUT, "a", &fix->loc_addr );
    } else if( FmtData.type & MK_PHAR_MULTISEG ) {
        MakeBase( fix );
        new_reloc.item.pms.offset = fix->loc_addr.off;
        grp = seg->u.leader->group;
        new_reloc.item.pms.segment = grp->grp_addr.seg;
    } else {
        MakeBase( fix );
        new_reloc.item.dos.addr.off = fix->loc_addr.off;
        grp = seg->u.leader->group;
        new_reloc.item.dos.addr.seg = grp->grp_addr.seg;
        if( grp->section != Root ) {
            new_reloc.item.dos.addr.seg -= grp->section->sect_addr.seg;
        }
    }
    if( save ) {
        DumpReloc( &new_reloc );
    }
}

static void Relocate( offset off, fix_relo_data *fix, target_spec *target )
/***********************************************************************/
{
    int         shift;
    unsigned    datasize;
    unsigned_8  addbuf[MAX_ADDEND_SIZE + 2];

    shift = 0;
    datasize = CalcFixupSize( fix->type );

    fix->done = FALSE;
    fix->os2_selfrel = FALSE;
    fix->data = addbuf;

    if( (LinkFlags & FAR_CALLS_FLAG) && (LinkState & HAVE_I86_CODE) ) {

        /*
         * it is necessary to copy also two bytes before reloc position to addbuf
         * because these two bytes can be changed during FAR CALL optimization
         */

        addbuf[0] = 0;
        addbuf[1] = 0;
        shift = ( off < 2 ) ? off : 2;
        datasize += shift;
        fix->data += 2;
    }
    ReadInfo( CurrRec.seg->u1.vm_ptr + off - shift, fix->data - shift, datasize );

    if( !CheckSpecials( fix, target ) ) {
        PatchData( fix );
        if( (LinkFlags & FAR_CALLS_FLAG) && (LinkState & HAVE_I86_CODE) )
            FarCallOpt( fix );
        FmtReloc( fix, target );
    }
    PutInfo( CurrRec.seg->u1.vm_ptr + off - shift, fix->data - shift, datasize );
}
