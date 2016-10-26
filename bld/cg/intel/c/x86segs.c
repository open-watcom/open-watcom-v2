/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2016 The Open Watcom Contributors. All Rights Reserved.
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


#include "cgstd.h"
#include "coderep.h"
#include "cgauxinf.h"
#include "zoiks.h"
#include "cgdefs.h"
#include "data.h"
#include "objout.h"
#include "namelist.h"
#include "x86objd.h"
#include "x86obj.h"
#include "split.h"
#include "feprotos.h"


extern  void    InitSegment( void ) {
/*****************************/
}


extern  void    FiniSegment( void ) {
/*****************************/
}


extern  hw_reg_set      CalcSegment( cg_sym_handle sym, cg_class class ) {
/*********************************************************************/

    fe_attr     attr;
    segment_id  id;
    hw_reg_set  *reg;

    if( class == CG_TBL || class == CG_VTB || class == CG_CLB ) {
        if( _IsTargetModel( FLAT_MODEL ) )
            return( HW_DS );
        return( HW_CS );
    }
    if( class == CG_FE ) {
        attr = FEAttr( sym );
    } else {
        attr = EMPTY;
    }
    id = AskSegID( sym, class );
    reg = FEAuxInfo( (pointer)(pointer_int)id, PEGGED_REGISTER );
    if( reg != NULL && !HW_CEqual( *reg, HW_EMPTY ) ) {
        if( HW_COvlap( *reg, HW_SEGS ) )
            return( *reg );
        FEMessage( MSG_BAD_PEG_REG, AskForLblSym( CurrProc->label ) );
    }
    if( class == CG_FE && (attr & FE_PROC) && (attr & FE_DLLIMPORT) == 0 ) {
#if _TARGET & _TARG_80386
        if( _IsTargetModel( FLAT_MODEL ) )
            return( HW_CS ); /* all have same CS */
        if( AskCodeSeg() == id ) {
            return( HW_CS );
        }
#endif
    } else {
        if( _IsTargetModel(FLAT_MODEL) && _IsntTargetModel(FLOATING_DS) )
            return(HW_DS);
        if( AskCodeSeg() == id ) {
            /* COMDAT's might be allocated by some other module */
            if( attr & FE_COMMON )
                return( HW_EMPTY );
            return( HW_CS ); /* 89-09-01 */
        }
        if( AskSegPrivate( id ) )
            return( HW_EMPTY );
        if( _IsntTargetModel( FLOATING_DS ) )
            return( HW_DS );
        if( !_RoutineIsInterrupt( CurrProc->state.attr ) && _IsntTargetModel( FLOATING_SS ) ) {
            return( HW_SS );
        }
    }
    return( HW_EMPTY );
}


extern  name    *AddrConst( name *value, segment_id seg, constant_class class ) {
/************************************************************************/

    hw_reg_set  reg;

    switch( class ) {
    case CONS_SEGMENT:
        HW_CAsgn( reg, HW_EMPTY );
        if( value != NULL ) {
            reg = CalcSegment( value->v.symbol, value->m.memory_type );
        }
        if( HW_CEqual( reg, HW_EMPTY ) ) {
            return( AllocAddrConst( value, seg, class, U2 ) );
        } else {
            return( AllocRegName( reg ) );
        }
    case CONS_OFFSET:
        return( AllocAddrConst( value, seg, class, WD ) );
    case CONS_ADDRESS:
        return( AllocAddrConst( value, seg, class, CP ) );
    default:
        _Zoiks( ZOIKS_115 );
        return( NULL );
    }
}

extern  bool    SegIsCS( name *op ) {
/*************************************/

    hw_reg_set  segreg;

    segreg = CalcSegment( op->v.symbol, op->m.memory_type );
    if( HW_CEqual( segreg, HW_CS ) )
        return( true );
    return( false );
}


extern  bool    SegIsSS( name *op ) {
/*************************************/

    hw_reg_set  segreg;

    segreg = CalcSegment( op->v.symbol, op->m.memory_type );
    if( HW_CEqual( segreg, HW_SS ) )
        return( true );
    if( HW_CEqual( segreg, HW_DS ) &&_IsntTargetModel( FLOATING_DS | FLOATING_SS ) )
        return( true );
    return( false );
}


extern  name    *GetSegment( name *op ) {
/*****************************************/

    hw_reg_set  reg;
    segment_id  seg;

    reg = CalcSegment( op->v.symbol, op->m.memory_type );
    if( !HW_CEqual( reg, HW_EMPTY ) )
        return( AllocRegName( reg ) );
    seg = AskSegID( op->v.symbol, op->m.memory_type );
    if( AskSegNear( seg ) )
        return( AddrConst( NULL, AskBackSeg(), CONS_SEGMENT ) );
    return( AddrConst( op, seg, CONS_SEGMENT ) );
}

extern  name    *NearSegment( void ) {
/******************************/

    if( _IsntTargetModel( FLOATING_DS ) )
        return( AllocRegName( HW_DS ) );
    if( _IsTargetModel( FLOATING_SS ) )
        return( AddrConst( NULL, AskBackSeg(), CONS_SEGMENT ) );/*means DGROUP*/
    return( AllocRegName( HW_SS ) );
}


extern  name    *SegName( name *op ) {
/**************************************/

    name        *idx;
    hw_reg_set  tmp;

    if( op->n.class == N_REGISTER ) {
        tmp = op->r.reg;
        HW_COnlyOn( tmp, HW_DS_ES_SS_CS );
        return( AllocRegName( tmp ) );
    }
    if( op->n.class == N_TEMP )
        return( AllocRegName( HW_SS ) );
    if( op->n.class == N_MEMORY )
        return( GetSegment( op ) );
    if( op->n.class == N_INDEXED ) {
        idx = op->i.index;
        if( op->i.base != NULL && idx->n.name_class != PT ) {
            // even though it's based - huge arrays cannot assume segment
            // of base is same as segment of the huge pointer - BBB Sept 14, 1995
            return( SegName( op->i.base ) );
        }
        if( idx->n.class == N_REGISTER ) {
            if( HW_CEqual( idx->r.reg, HW_SP )
             || HW_CEqual( idx->r.reg, HW_BP ) ) {
                return( AllocRegName( HW_SS ) );
            } else {
                _Zoiks( ZOIKS_022 );
                return( op );
            }
        } else if( idx->n.name_class == CP || idx->n.name_class == PT ) {
            return( SegmentPart( idx ) );
        } else {
            return( NearSegment() );
        }
    }
    return( op );
}


extern  cg_type NamePtrType( name *op ) {
/*************************************/

    cg_sym_handle sym;

    if( op->n.class == N_MEMORY ) {
        if( op->m.memory_type == CG_FE || op->m.memory_type == CG_BACK ) {
            sym = op->v.symbol;
            if( AskNameCode( sym, op->m.memory_type ) ) {
                if( op->m.memory_type == CG_FE ) {
                    if( FEAttr( sym ) & FE_PROC ) {
                        if( *(call_class *)FindAuxInfoSym( sym, CALL_CLASS ) & FAR_CALL )
                            return( TY_LONG_CODE_PTR );
                        return( TY_NEAR_CODE_PTR );
                    } else {
                        return( TY_HUGE_POINTER );
                    }
                } else {
                    return( TY_HUGE_POINTER );
                }
            } else if( AskSegPrivate( AskSegID( sym, op->m.memory_type ) )
                        || _IsTargetModel( FLOATING_DS ) ) {
                if( op->m.memory_type == CG_FE &&
                    ( FEAttr( sym ) & FE_ONESEG ) ) {
                    return( TY_POINTER );
                } else {
                    return( TY_HUGE_POINTER );
                }
            } else {
                return( TY_POINTER );
            }
        } else if( _IsTargetModel( FLOATING_DS ) || op->m.memory_type == CG_CLB ) {
            return( TY_HUGE_POINTER );
        } else {
            return( TY_POINTER );
        }
    } else if( op->n.class == N_TEMP ) {
        if( _IsTargetModel( FLOATING_SS ) || _IsTargetModel( FLOATING_DS ) ) {
            // can't have a stack > 64K - BBB 06/02/94
            return( TY_LONG_POINTER );
        } else {
            return( TY_POINTER );
        }
    } else if( _IsTargetModel( FLOATING_DS ) ) {
        return( TY_HUGE_POINTER );
    } else {
        return( TY_POINTER );
    }
}


extern  bool    SegOver( name *op ) {
/*************************************/

    segment_id  id;

    if( op->n.class == N_INDEXED ) {
        if( op->i.index->n.class != N_CONSTANT && op->i.index->n.size != WORD_SIZE )
            return( true );
        if( op->i.base != NULL )
            return( SegOver( op->i.base ) );
        if( _IsTargetModel( FLOATING_DS ) ) {
            return( true );
        }
    } else if( op->n.class == N_MEMORY ) {
        switch( op->m.memory_type ) {
        case CG_FE:
        case CG_BACK:
        case CG_LBL:
            id = AskSegID( op->v.symbol, op->m.memory_type );
            if( AskSegPrivate( id ) ) {
                return( true );
            }
            if( id == AskCodeSeg() && _IsntTargetModel( FLAT_MODEL ) ) {
                return( true );
            }
            if( _IsTargetModel( FLOATING_SS ) && _IsTargetModel( FLOATING_DS ) ) {
                return( true );
            }
            return( false );
        case CG_CLB:
            return( false );
        default:
            break;
        }
    }
    return( false );
}


extern  bool    LoadAToMove( instruction *ins ) {
/***********************************************/

    segment_id  seg;
    name        *op;

    if( _IsTargetModel( INDEXED_GLOBALS ) )
        return( false );
    if( ins->head.opcode != OP_LA && ins->head.opcode != OP_CAREFUL_LA )
        return( false );
    op = ins->operands[0];
    if( op->n.class != N_MEMORY )
        return( false );
    ins->head.opcode = OP_MOV;
    seg = AskSegID( op->v.symbol, op->m.memory_type );
    if( AskSegNear( seg ) ) {
        seg = AskBackSeg();
    }
    if( ins->type_class == WD || ins->type_class == SW ) {
        ins->operands[0] = AddrConst( op, seg, CONS_OFFSET );
    } else { /* must be a far pointer */
        ins->operands[0] = AddrConst( op, seg, CONS_ADDRESS );
    }
    return( true );
}

extern  bool                     IsUncacheableMemory( name *opnd ) {
/*******************************************************************
    Return true if we don't want to cache the global piece of memory
    represented by opnd in a register - this could be dangerous if the
    variable is not in DGROUP or if DS is not pegged - we would need to
    load a segment register before and after every call and no one has
    necessarily reserved one.
*/

    hw_reg_set          segreg;

    if( opnd->n.class != N_MEMORY )
        return( false );
    segreg = CalcSegment( opnd->v.symbol, opnd->m.memory_type );
    if( HW_CEqual( segreg, HW_SS ) && _IsntTargetModel( FLOATING_SS ) )
        return( false );
    if( HW_CEqual( segreg, HW_DS ) && _IsntTargetModel( FLOATING_DS ) )
        return( false );
    if( HW_CEqual( segreg, HW_CS ) )
        return( false );
    return( true );
}

