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


#include <stdlib.h>
#include <string.h>

#include "plusplus.h"
#include "cgfront.h"
#include "ring.h"
#include "codegen.h"
#include "errdefns.h"
#include "segment.h"


static target_offset_t cgSegIdAlign( SYMBOL sym, type_flag flags )
/****************************************************************/
{
    target_offset_t align;

    if( flags & TF1_HUGE ) {
        align = 16;
    } else {
        align = SegmentAlignment( sym );
    }
    return( align );
}

static boolean cgSegIdConst( SYMBOL sym, type_flag flags, SEGID_CONTROL control )
/*******************************************************************************/
{
    if(( flags & TF1_CONST ) == TF1_NULL ) {
        // symbol is not declared const
        return( FALSE );
    }
    if( control & SI_NEEDS_CODE ) {
        // symbol is initialized via code in a module ctor
        return( FALSE );
    }
    if( TypeRequiresRWMemory( sym->sym_type ) ) {
        return( FALSE );
    }
    #if 0
    if( SymRequiresCtoring( sym ) ) {
        // symbol needs to be modified before being made "const"
        return( FALSE );
    }
    if( SymRequiresDtoring( sym ) ) {
        // symbol is "const" but needs to be modified to be destructed
        return( FALSE );
    }
    #endif
    return( TRUE );
}

static fe_seg_id cgSegIdBased( SYMBOL sym, type_flag flags )
{
    fe_seg_id       id;         // - segment id
    target_offset_t align;      // - alignment for symbol
    target_size_t   size;       // - size of symbol
    TYPE            base_mod;   // - __based modifier

    size = CgMemorySize( sym->sym_type );
    #ifdef _CHECK_SIZE      // only defined if needed
        if( !(flags & TF1_HUGE) ) {
            if( _CHECK_SIZE( size ) ) {
                CErr2p( ERR_DATA_TOO_BIG, sym );
                return( SEG_NULL );
            }
        }
    #endif

    if( SymIsExtern( sym ) ) {
        // not defined in this compilation unit
        id = SegmentImport();
    } else {
        switch( flags & TF1_BASED ) {
        case TF1_BASED_STRING:
            id = SegmentFindBased( sym->sym_type );
            break;
        case TF1_BASED_SELF:
        case TF1_BASED_VOID:
            id = SEG_NULL;
            break;
        case TF1_BASED_FETCH:
        case TF1_BASED_ADD:
            base_mod = BasedType( sym->sym_type );
            id = CgSegIdData( (SYMBOL)base_mod->u.m.base, SI_DEFAULT );
            break;
        DbgDefault( "invalid based cgsegid call" );
        }
        if( id != SEG_NULL ) {
            align = cgSegIdAlign( sym, flags );
            id = SegmentAddSym( sym, id, size, align );
        }
    }
    return( id );
}

static fe_seg_id cgSegIdThread( SYMBOL sym, type_flag flags )
{
    fe_seg_id       id;         // - segment id

    flags = flags;
    #ifdef _CHECK_SIZE      // only defined if needed
        if( !(flags & TF1_HUGE) ) {
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
        id = SegmentImport();
    } else {
        id = SEG_TLS;
    }
    return( id );
}

static fe_seg_id cgSegIdNearVariable( SYMBOL sym, type_flag flags,
    target_size_t size,
    SEGID_CONTROL control )
{
    fe_seg_id id;
    target_offset_t align;

    align = cgSegIdAlign( sym, flags );
    if( SymIsExtern( sym ) ) {
        // somewhere in DGROUP (SEG_BSS,SEG_DATA,SEG_CONST2,SEG_STACK)
        // decision about NEAR/FAR done in cgSegIdVariable
        id = SegmentAddSym( sym, SEG_DATA, size, align );
    } else if( control & SI_ALL_ZERO ) {
        // defined to be zero in this compilation unit
        id = SegmentAddSym( sym, SEG_BSS, size, align );
    } else if( !cgSegIdConst( sym, flags, control ) ) {
        id = SegmentAddSym( sym, SEG_DATA, size, align );
    } else {
        id = SegmentAddSym( sym, SEG_CONST2, size, align );
    }
    return( id );
}

static fe_seg_id cgSegIdFarVariable( SYMBOL sym, type_flag flags,
    target_size_t size, SEGID_CONTROL control )
{
    fe_seg_id id;
    target_offset_t align;

    align = cgSegIdAlign( sym, flags );
    if( SymIsExtern( sym ) ) {
        // not defined in this compilation unit
        id = SegmentImport();
    } else if( !cgSegIdConst( sym, flags, control ) ) {
        id = SegmentAddFar( size, align );
    } else {
        id = SegmentAddConstFar( size, align );
    }
    return( id );
}

static fe_seg_id cgSegIdHugeVariable( SYMBOL sym, type_flag flags,
    target_size_t size, SEGID_CONTROL control )
{
    fe_seg_id id;

    if( SymIsExtern( sym ) ) {
        id = SegmentImport();
    } else if( cgSegIdConst( sym, flags, control ) ) {
        id = SegmentAddConstHuge( size );
    } else {
        id = SegmentAddHuge( size );
    }
    return( id );
}

static fe_seg_id cgSegIdVariable( SYMBOL sym, type_flag flags, SEGID_CONTROL control )
{
    fe_seg_id id;
    target_size_t size;

    size = CgMemorySize( sym->sym_type );
    #ifdef _CHECK_SIZE      // only defined if needed
        if( !(flags & TF1_HUGE) ) {
            if( _CHECK_SIZE( size ) ) {
                CErr2p( ERR_DATA_TOO_BIG, sym );
                return( SEG_NULL );
            }
        }
    #endif
    if( flags & TF1_NEAR ) {
        id = cgSegIdNearVariable( sym, flags, size, control );
    } else if( flags & TF1_HUGE ) {
        id = cgSegIdHugeVariable( sym, flags, size, control );
    } else if( flags & TF1_FAR ) {
        id = cgSegIdFarVariable( sym, flags, size, control );
    } else {
        boolean assume_near = TRUE;
        if( IsBigData() ) {
            if( flags & TF1_DEFAULT_FAR ) {
                assume_near = FALSE;
            } else if( size == 0 || size > DataThreshold ) {
                assume_near = FALSE;
            } else if( CompFlags.zc_switch_used ) {
                if( cgSegIdConst( sym, flags, control ) ) {
                    // symbol may have been placed in code segment
                    assume_near = FALSE;
                }
            }
        }
        if( assume_near ) {
            id = cgSegIdNearVariable( sym, flags, size, control );
        } else {
            id = cgSegIdFarVariable( sym, flags, size, control );
        }
    }
    return( id );
}

fe_seg_id CgSegIdFunction( SYMBOL sym )
/*************************************/
{
    fe_seg_id id;
    type_flag flags;

    id = sym->segid;
    if( id == SEG_NULL ) {
        if( SymIsInitialized( sym ) ) {
            // defined in this compilation unit
            id = SegmentForDefinedFunc( sym );
        } else {
            TypeGetActualFlags( sym->sym_type, &flags );
            if( flags & TF1_FAR ) {
                id = SegmentImport();
            } else if( IsBigCode() ) {
                id = SegmentImport();
            } else {
                id = SegmentDefaultCode();
            }
        }
        sym->segid = id;
    }
    return( id );
}

static fe_seg_id cgSegIdAuto( SYMBOL sym )
{
    fe_seg_id id;

    id = sym->segid;
    if( id == SEG_NULL ) {
        id = SEG_STACK;
        sym->segid = id;
    }
    return( id );
}

fe_seg_id CgSegIdData( SYMBOL sym, SEGID_CONTROL control )
/********************************************************/
{
    fe_seg_id id;
    type_flag flags;

    if( SymIsAutomatic( sym ) ) {
        id = cgSegIdAuto( sym );
    } else if( SymIsFunction( sym ) ) {
        id = CgSegIdFunction( sym );
    } else {
        id = sym->segid;
        if( id == SEG_NULL ) {
            TypeGetActualFlags( sym->sym_type, &flags );
            if( flags & TF1_THREAD ) {
                id = cgSegIdThread( sym, flags );
            } else if( flags & TF1_BASED ) {
                id = cgSegIdBased( sym, flags );
#if _CPU == _AXP || COMP_CFG_COFF == 1
            } else if( SymIsComdatData( sym ) ) {
                id = SegmentAddComdatData( sym, control );
#endif
            } else {
                id = cgSegIdVariable( sym, flags, control );
            }
            sym->segid = id;
        }
        SegmentMarkUsed( id );
    }
    return( id );
}

fe_seg_id CgSegId( SYMBOL sym )
/*****************************/
{
    return( CgSegIdData( sym, SI_DEFAULT ) );
}
