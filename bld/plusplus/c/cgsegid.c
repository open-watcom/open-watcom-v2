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


#include "plusplus.h"

#include "compcfg.h"

#include "cgfront.h"
#include "ring.h"
#include "codegen.h"
#include "segment.h"


static target_offset_t cgSegIdAlign( SYMBOL sym, type_flag flags )
/****************************************************************/
{
    target_offset_t align;

    if( flags & TF1_HUGE ) {
        align = 16;
    } else {
        align = SegmentAlignment( sym->sym_type );
    }
    return( align );
}

static bool cgSegIdConst( SYMBOL sym, type_flag flags, SEGID_CONTROL control )
/****************************************************************************/
{
    if( (flags & TF1_CONST) == 0 ) {
        // symbol is not declared const
        return( false );
    }
    if( control & SI_NEEDS_CODE ) {
        // symbol is initialized via code in a module ctor
        return( false );
    }
    if( TypeRequiresRWMemory( sym->sym_type ) ) {
        return( false );
    }
    #if 0
    if( SymRequiresCtoring( sym ) ) {
        // symbol needs to be modified before being made "const"
        return( false );
    }
    if( SymRequiresDtoring( sym ) ) {
        // symbol is "const" but needs to be modified to be destructed
        return( false );
    }
    #endif
    return( true );
}

static fe_seg_id cgSegIdBased( SYMBOL sym, type_flag flags )
{
    fe_seg_id       segid;      // - segment id
    target_offset_t align;      // - alignment for symbol
    target_size_t   size;       // - size of symbol
    TYPE            base_mod;   // - __based modifier

    size = CgMemorySize( sym->sym_type );
#ifdef _CHECK_SIZE      // only defined if needed
    if( (flags & TF1_HUGE) == 0 ) {
        if( _CHECK_SIZE( size ) ) {
            CErr2p( ERR_DATA_TOO_BIG, sym );
            return( SEG_NULL );
        }
    }
#endif

    if( SymIsExtern( sym ) ) {
        // not defined in this compilation unit
        segid = SegmentImport();
    } else {
        segid = SEG_NULL;
        switch( flags & TF1_BASED ) {
        case TF1_BASED_STRING:
            segid = SegmentFindBased( sym->sym_type );
            break;
        case TF1_BASED_SELF:
        case TF1_BASED_VOID:
            break;
        case TF1_BASED_FETCH:
        case TF1_BASED_ADD:
            base_mod = BasedType( sym->sym_type );
            segid = CgSegIdData( (SYMBOL)base_mod->u.m.base, SI_DEFAULT );
            break;
        DbgDefault( "invalid based cgsegid call" );
        }
        if( segid != SEG_NULL ) {
            align = cgSegIdAlign( sym, flags );
            segid = SegmentAddSym( sym, segid, size, align );
        }
    }
    return( segid );
}

static fe_seg_id cgSegIdThread( SYMBOL sym, type_flag flags )
{
    fe_seg_id   segid;  // - segment id

    /* unused parameters */ (void)flags;

#ifdef _CHECK_SIZE      // only defined if needed
    if( (flags & TF1_HUGE) == 0 ) {
        target_size_t   size;       // - size of symbol
        size = CgMemorySize( sym->sym_type );
        if( _CHECK_SIZE( size ) ) {
            CErr2p( ERR_DATA_TOO_BIG, sym );
            return( SEG_NULL );
        }
    }
#endif

    if( SymIsExtern( sym ) ) {
        // not defined in this compilation unit
        segid = SegmentImport();
    } else {
        segid = SEG_TLS;
    }
    return( segid );
}

static fe_seg_id cgSegIdNearVariable( SYMBOL sym, type_flag flags,
    target_size_t size,
    SEGID_CONTROL control )
{
    fe_seg_id segid;
    target_offset_t align;

    align = cgSegIdAlign( sym, flags );
    if( SymIsExtern( sym ) ) {
        // somewhere in DGROUP (SEG_BSS,SEG_DATA,SEG_CONST2,SEG_STACK)
        // decision about NEAR/FAR done in cgSegIdVariable
        segid = SegmentAddSym( sym, SEG_DATA, size, align );
    } else if( control & SI_ALL_ZERO ) {
        // defined to be zero in this compilation unit
        segid = SegmentAddSym( sym, SEG_BSS, size, align );
    } else if( !cgSegIdConst( sym, flags, control ) ) {
        segid = SegmentAddSym( sym, SEG_DATA, size, align );
    } else {
        segid = SegmentAddSym( sym, SEG_CONST2, size, align );
    }
    return( segid );
}

static fe_seg_id cgSegIdFarVariable( SYMBOL sym, type_flag flags,
    target_size_t size, SEGID_CONTROL control )
{
    fe_seg_id segid;
    target_offset_t align;

    align = cgSegIdAlign( sym, flags );
    if( SymIsExtern( sym ) ) {
        // not defined in this compilation unit
        segid = SegmentImport();
    } else if( !cgSegIdConst( sym, flags, control ) ) {
        segid = SegmentAddFar( size, align );
    } else {
        segid = SegmentAddConstFar( size, align );
    }
    return( segid );
}

static fe_seg_id cgSegIdHugeVariable( SYMBOL sym, type_flag flags,
    target_size_t size, SEGID_CONTROL control )
{
    fe_seg_id segid;

    if( SymIsExtern( sym ) ) {
        segid = SegmentImport();
    } else if( cgSegIdConst( sym, flags, control ) ) {
        segid = SegmentAddConstHuge( size );
    } else {
        segid = SegmentAddHuge( size );
    }
    return( segid );
}

static fe_seg_id cgSegIdVariable( SYMBOL sym, type_flag flags, SEGID_CONTROL control )
{
    fe_seg_id segid;
    target_size_t size;

    size = CgMemorySize( sym->sym_type );
#ifdef _CHECK_SIZE      // only defined if needed
    if( (flags & TF1_HUGE) == 0 ) {
        if( _CHECK_SIZE( size ) ) {
            CErr2p( ERR_DATA_TOO_BIG, sym );
            return( SEG_NULL );
        }
    }
#endif
    if( flags & TF1_NEAR ) {
        segid = cgSegIdNearVariable( sym, flags, size, control );
    } else if( flags & TF1_HUGE ) {
        segid = cgSegIdHugeVariable( sym, flags, size, control );
    } else if( flags & TF1_FAR ) {
        segid = cgSegIdFarVariable( sym, flags, size, control );
    } else {
        bool assume_near = true;
        if( IsBigData() ) {
            if( flags & TF1_DEFAULT_FAR ) {
                assume_near = false;
            } else if( size == 0 || size > DataThreshold ) {
                assume_near = false;
            } else if( CompFlags.zc_switch_used ) {
                if( cgSegIdConst( sym, flags, control ) ) {
                    // symbol may have been placed in code segment
                    assume_near = false;
                }
            }
        }
        if( assume_near ) {
            segid = cgSegIdNearVariable( sym, flags, size, control );
        } else {
            segid = cgSegIdFarVariable( sym, flags, size, control );
        }
    }
    return( segid );
}

fe_seg_id CgSegIdFunction( SYMBOL sym )
/*************************************/
{
    fe_seg_id segid;
    type_flag flags;

    segid = sym->segid;
    if( segid == SEG_NULL ) {
        if( SymIsInitialized( sym ) ) {
            // defined in this compilation unit
            segid = SegmentForDefinedFunc( sym );
        } else {
            TypeGetActualFlags( sym->sym_type, &flags );
            if( flags & TF1_FAR ) {
                segid = SegmentImport();
            } else if( IsBigCode() ) {
                segid = SegmentImport();
            } else {
                segid = SegmentDefaultCode();
            }
        }
        sym->segid = segid;
    }
    return( segid );
}

static fe_seg_id cgSegIdAuto( SYMBOL sym )
{
    fe_seg_id segid;

    segid = sym->segid;
    if( segid == SEG_NULL ) {
        segid = SEG_STACK;
        sym->segid = segid;
    }
    return( segid );
}

fe_seg_id CgSegIdData( SYMBOL sym, SEGID_CONTROL control )
/********************************************************/
{
    fe_seg_id segid;
    type_flag flags;

    if( SymIsAutomatic( sym ) ) {
        segid = cgSegIdAuto( sym );
    } else if( SymIsFunction( sym ) ) {
        segid = CgSegIdFunction( sym );
    } else {
        segid = sym->segid;
        if( segid == SEG_NULL ) {
            TypeGetActualFlags( sym->sym_type, &flags );
            if( flags & TF1_THREAD ) {
                segid = cgSegIdThread( sym, flags );
            } else if( flags & TF1_BASED ) {
                segid = cgSegIdBased( sym, flags );
#if _CPU == _AXP || COMP_CFG_COFF == 1
            } else if( SymIsComdatData( sym ) ) {
                segid = SegmentAddComdatData( sym, control );
#endif
            } else {
                segid = cgSegIdVariable( sym, flags, control );
            }
            sym->segid = segid;
        }
        SegmentMarkUsed( segid );
    }
    return( segid );
}

fe_seg_id CgSegId( SYMBOL sym )
/*****************************/
{
    return( CgSegIdData( sym, SI_DEFAULT ) );
}
