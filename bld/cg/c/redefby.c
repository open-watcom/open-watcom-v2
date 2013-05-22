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


#include "cgstd.h"
#include "coderep.h"
#include "zoiks.h"
#include "typedef.h"
#include "model.h"
#include "cgdefs.h"
#include "feprotos.h"

extern  hw_reg_set      DisplayReg(void);
extern  name            *DeAlias(name*);
extern  bool            AskNameROM(pointer,cg_class);

extern bool ReDefinedBy( instruction *, name * );
static bool ZapsIndexed( name *, name * );

extern  bool    TempsOverlap( name *name1, name *name2 ) {
/*********************************************************
    Do N_TEMP names "name1" and "name2" occupy the same memory?
*/

    type_length start_1;
    type_length end_1;
    type_length start_2;
    type_length end_2;

    start_1 = name1->v.offset;
    end_1 = start_1 + name1->n.size - 1;
    start_2 = name2->v.offset;
    end_2 = start_2 + name2->n.size - 1;
    if( start_2 > end_1 || end_2 < start_1 ) return( FALSE );
    return( TRUE );
}


static  bool    ZapsMemory( name *result, name *op, bool for_index ) {
/*********************************************************************
    Could redefining "result" redefine N_MEMORY name "op"?
*/

    switch( result->n.class ) {
    case N_TEMP:
        return( FALSE );
    case N_MEMORY:
        if( result->v.symbol != op->v.symbol ) return( FALSE );
        /* can be used for memory ops as well */
        if( for_index ) return( TRUE );
        return( TempsOverlap( result, op ) );
    case N_INDEXED:
        if( result->i.base == NULL ) {
            if( _IsModel( FORTRAN_ALIASING ) ) return( FALSE );
            if( op->v.usage & USE_ADDRESS ) return( TRUE );
            if( _IsModel( RELAX_ALIAS ) ) return( FALSE );
            return( TRUE );
        }
        if( result->i.base->n.class == N_TEMP ) {
            if( _IsModel( RELAX_ALIAS ) ) return( FALSE );
            return( TRUE );
        } else { /* it must be N_MEMORY*/
            return( ZapsMemory( result->i.base, op, TRUE ) );
        }
    case N_REGISTER:
        return( FALSE );
    default:
        _Zoiks( ZOIKS_023 );
        return( FALSE );
    }
}


static  bool    ZapsTemp( name *result, name *op, bool for_index ) {
/*******************************************************************
    Could redefining "result" redefine N_TEMP name "op"?
*/
    switch( result->n.class ) {
    case N_TEMP:
        if( DeAlias( result ) != DeAlias( op ) ) return( FALSE );
        if( for_index ) return( TRUE );
        return( TempsOverlap( op, result ) );
    case N_MEMORY:
        return( FALSE );
    case N_INDEXED:
        /* might be a structured move with a fake base */
        if( result->i.base != NULL && result->i.base->n.class == N_TEMP ) {
            if( DeAlias( result->i.base ) == DeAlias( op ) ) return( TRUE );
        }
        if( ( op->v.usage & USE_ADDRESS ) == 0 ) return( FALSE );
        if( result->i.base == NULL ) return( TRUE );
        /* bound index*/
        return( FALSE );
    case N_REGISTER:
        {
        hw_reg_set      tmp;

        tmp = DisplayReg();
        return( HW_Ovlap( result->r.reg, tmp ) );
        }
    default:
        _Zoiks( ZOIKS_023 );
        return( FALSE );
    }
}


static bool ZapsTheOp( name *result, name *op )
/*********************************************/
{
    if( result == NULL ) return( FALSE );
    switch( op->n.class ) {
    case N_MEMORY:
        if( op->v.usage & VAR_VOLATILE ) return( TRUE );
        return( ZapsMemory( result, op, FALSE ) );
    case N_INDEXED:
        if( op->i.index_flags & X_VOLATILE ) return( TRUE );
        return( ZapsIndexed( result, op ) );
    case N_TEMP:
        if( op->v.usage & VAR_VOLATILE ) return( TRUE );
        return( ZapsTemp( result, op, FALSE ) );
    case N_REGISTER:
        if( result->n.class == N_REGISTER
            && HW_Ovlap( result->r.reg, op->r.reg ) ) return( TRUE );
        return( FALSE );
    case N_CONSTANT:
        return( FALSE );
    default:
        _Zoiks( ZOIKS_023 );
        return( FALSE );
    }
}


static  bool    ZapsIndexed( name *result, name *op ) {
/******************************************************
    Could redefining "result" redefine N_INDEXED name "op"?
*/

    switch( result->n.class ) {
    case N_TEMP:
        result = DeAlias( result );
        if( op->i.base == NULL ) { /* free index */
            if( result->v.usage & USE_ADDRESS ) return( TRUE );
        } else if( op->i.base->n.class == N_TEMP ) {
            if( result == DeAlias( op->i.base ) ) return( TRUE );
        }
        if( op->i.index->n.class != N_TEMP ) return( FALSE );
        if( result == DeAlias( op->i.index ) ) return( TRUE );
        return( FALSE );
    case N_MEMORY:
        if( op->i.base == NULL ) {
            if( _IsModel( FORTRAN_ALIASING ) ) return( FALSE );
            if( op->v.usage & USE_ADDRESS ) return( TRUE );
            if( _IsModel( RELAX_ALIAS ) ) return( FALSE );
            return( TRUE );
        } else if( op->i.base->n.class == N_TEMP ) {
            if( _IsModel( RELAX_ALIAS ) ) return( FALSE );
            return( TRUE );
        } else {
            return( ZapsMemory( result, op->i.base, TRUE ) );
        }
    case N_INDEXED:
        if( result->i.base == op->i.base &&
            result->i.index == op->i.index &&
            result->i.index_flags == op->i.index_flags &&
            result->i.scale == op->i.scale &&
            ( result->i.constant >= op->i.constant + op->n.size ||
              op->i.constant >= result->i.constant + result->n.size ) ) {
            return( MAYBE ); // no overlap if index is the same
        }
        if( result->i.base == NULL ) return( TRUE );
        if( op->i.base == NULL ) return( TRUE );
        if( result->i.base->n.class != op->i.base->n.class ) return( FALSE );
        if( result->i.base->n.class == N_TEMP ) {
            return( ZapsTemp( result->i.base, op->i.base, TRUE ) );
        }
        return( ZapsMemory( result->i.base, op->i.base, TRUE ) );
    case N_REGISTER:
        if( op->i.base != NULL && ZapsTheOp( result, op->i.base ) ) {
            return( TRUE );
        }
        return( ZapsTheOp( result, op->i.index ) );
    default:
        _Zoiks( ZOIKS_023 );
        return( FALSE );
    }
}


extern  bool    NameIsConstant( name *op ) {
/******************************************/

    if( op->n.class == N_TEMP
            && ( op->v.usage & VAR_CONSTANT ) ) return( TRUE );
    if( op->n.class != N_MEMORY ) return( FALSE );
    if( op->v.usage & VAR_VOLATILE ) return( FALSE );
    if( op->v.usage & VAR_CONSTANT ) return( TRUE );
    if( op->m.memory_type == CG_FE
            && ( FEAttr( op->v.symbol ) & FE_CONSTANT) ) return( TRUE );
    return( AskNameROM( op->v.symbol, op->m.memory_type ) );
}

extern  bool    VisibleToCall( instruction *ins, name *op, bool modifies ) {
/***************************************************************************
    Is the operand 'op' visible to the code in invoked by the call 'ins'?
    The 'modifies' flag means we only care if the routine can modify 'op'.
*/

    switch( op->n.class ) {
    case N_MEMORY:
        if( modifies &&
            ins->flags.call_flags & CALL_WRITES_NO_MEMORY ) return( FALSE );
        if( _IsModel( FORTRAN_ALIASING ) ) {
            switch( op->m.memory_type ) {
            case CG_FE:
                if( !( FEAttr( op->v.symbol ) & FE_VISIBLE ) ) {
                    return( FALSE );
                }
                return( TRUE );
            case CG_BACK:
            case CG_TBL:
            case CG_VTB:
                return( FALSE );
            default:
                return( TRUE );
            }
        }
        return( TRUE );
    case N_INDEXED:
        if( op->i.base == NULL ) return( TRUE );
        return( ReDefinedBy( ins, op->i.base ) );
    case N_TEMP:
        if( op->v.usage & USE_ADDRESS ) return( TRUE );
        return( FALSE );
#if _TARGET & _TARG_AXP
    case N_REGISTER:
        // to make up for a brain-dead setjmp operation which only
        // does a partial restore of the register-state, we do not
        // schedule any instructions past a call which has the
        // CALL_IS_SETJMP flag set
        if( ins->flags.call_flags & CALL_IS_SETJMP ) return( TRUE );
        return( FALSE );
#endif
    }
    return( FALSE );
}

static  bool    ZappedBySTQ_U( instruction *ins, name *op ) {
/***********************************************************/
#if _TARGET & _TARG_AXP
    name        *temp;
    name        *base_1;
    name        *base_2;

    if( ins->head.opcode != OP_STORE_UNALIGNED ) return( FALSE );
    switch( op->n.class ) {
    case N_MEMORY:
    case N_INDEXED:
        return( TRUE );
        break;
    case N_TEMP:
        temp = ins->result;
        if( temp->n.class == N_INDEXED ) {
            if( temp->i.base == NULL ) {
                if( op->v.usage & USE_ADDRESS ) {
                    return( TRUE );
                }
            } else {
                if( temp->i.base->n.class == N_TEMP ) {
                    base_1 = DeAlias( op );
                    base_2 = DeAlias( temp->i.base );
                    if( base_1 == base_2 ) {
                        return( TRUE );
                    }
                }
            }
        }
        if( temp->n.class == N_TEMP ) {
            base_1 = DeAlias( op );
            base_2 = DeAlias( temp );
            if( base_1 == base_2 ) {
                // if op is within range of ins->result
                return( TRUE );
            }
        }
        break;
    }
#else
    op = op;
    ins = ins;
#endif
    return( FALSE );
}

extern  bool    ReDefinedBy( instruction *ins, name *op ) {
/**********************************************************
    Is it possible that operand "op" could be redefined by instruction "ins"?
*/

    bool        zaps;

    if( op->n.class == N_REGISTER ) {
        if( HW_Ovlap( ins->zap->reg, op->r.reg ) ) return( TRUE );
    }
    if( NameIsConstant( op ) ) return( FALSE );
    if( ZappedBySTQ_U( ins, op ) ) return( TRUE );
    zaps = ZapsTheOp( ins->result, op );
    if( zaps ) return( zaps );
    if( !_OpIsCall( ins->head.opcode ) ) return( FALSE );
    return( VisibleToCall( ins, op, TRUE ) );
}


extern  bool    IsVolatile( name *op ) {
/***************************************
    Is "op" volatile in the C sense of the word?
*/

    switch( op->n.class ) {
    case N_MEMORY:
        if( op->v.usage & VAR_VOLATILE ) return( TRUE );
        return( FALSE );
    case N_INDEXED:
        if( op->i.index_flags & X_VOLATILE ) return( TRUE );
        return( FALSE );
    case N_TEMP:
        if( op->v.usage & VAR_VOLATILE ) return( TRUE );
        return( FALSE );
    default:
        return( FALSE );
    }
}
