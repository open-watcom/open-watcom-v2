/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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


#include "_cgstd.h"
#include "coderep.h"
#include "cgaux.h"
#include "cgauxcc.h"
#include "cgmem.h"
#include "data.h"
#include "utils.h"
#include "rgtbl.h"
#include "typemap.h"
#include "makeblk.h"
#include "bldcall.h"
#include "feprotos.h"


#define _NPX( x ) (((x) & ROUTINE_NO_8087_RETURNS) == 0)

type_class_def  CallState( aux_handle aux, type_def *tipe, call_state *state )
/****************************************************************************/
{
    call_class          cclass;
    call_class_target   cclass_target;
    type_class_def      type_class;
    uint                i;
    hw_reg_set          parms[10];
    hw_reg_set          *parm_src;
    hw_reg_set          *parm_dst;
    hw_reg_set          *pregs;
    hw_reg_set          tmp;

    state->unalterable = FixedRegs();
    pregs = FEAuxInfo( aux, FEINF_SAVE_REGS );
    HW_CAsgn( state->modify, HW_FULL );
    HW_TurnOff( state->modify, *pregs );
    HW_CTurnOff( state->modify, HW_UNUSED );
    state->used = state->modify;     /* anything not saved is used*/
    tmp = state->used;
    HW_TurnOff( tmp, StackReg() );
    HW_CTurnOff( tmp, HW_xBP ); /* should be able to call routine which modifies [E]BP */
    if( HW_Ovlap( state->unalterable, tmp ) ) {
        FEMessage( FEMSG_BAD_SAVE, aux );
    }
    state->attr = ROUTINE_REMOVES_PARMS;
    cclass = (call_class)(pointer_uint)FEAuxInfo( aux, FEINF_CALL_CLASS );
    cclass_target = (call_class_target)(pointer_uint)FEAuxInfo( aux, FEINF_CALL_CLASS_TARGET );
    if( cclass_target & FECALL_X86_INTERRUPT ) {
        state->attr |= ROUTINE_INTERRUPT;
    } else if( cclass_target & FECALL_X86_FAR_CALL ) {
        state->attr |= ROUTINE_LONG;
    } else if( cclass_target & FECALL_X86_FAR16_CALL ) {
        state->attr |= ROUTINE_FAR16;
    }
    if( cclass & FECALL_GEN_CALLER_POPS ) {
        state->attr &= ~ROUTINE_REMOVES_PARMS;
    }
    if( cclass & FECALL_GEN_ABORTS ) {
        state->attr |= ROUTINE_NEVER_RETURNS_ABORTS;
    }
    if( cclass & FECALL_GEN_NORETURN ) {
        state->attr |= ROUTINE_NEVER_RETURNS_NORETURN;
    }
    if( cclass_target & FECALL_X86_ROUTINE_RETURN ) {
        state->attr |= ROUTINE_ALLOCS_RETURN;
    }
    if( cclass_target & FECALL_X86_NO_STRUCT_REG_RETURNS ) {
        state->attr |= ROUTINE_NO_STRUCT_REG_RETURNS;
    }
    if( cclass_target & FECALL_X86_NO_FLOAT_REG_RETURNS ) {
        state->attr |= ROUTINE_NO_FLOAT_REG_RETURNS;
        state->attr |= ROUTINE_NO_8087_RETURNS;
    }
    if( cclass_target & FECALL_X86_NO_8087_RETURNS ) {
        state->attr |= ROUTINE_NO_8087_RETURNS;
    }
    if( cclass_target & FECALL_X86_MODIFY_EXACT ) {
        state->attr |= ROUTINE_MODIFY_EXACT;
    }
    if( cclass & FECALL_GEN_NO_MEMORY_CHANGED ) {
        state->attr |= ROUTINE_MODIFIES_NO_MEMORY;
    }
    if( cclass & FECALL_GEN_NO_MEMORY_READ ) {
        state->attr |= ROUTINE_READS_NO_MEMORY;
    }
    if( cclass_target & FECALL_X86_LOAD_DS_ON_ENTRY ) {
        state->attr |= ROUTINE_LOADS_DS;
    }
    if( cclass_target & FECALL_X86_LOAD_DS_ON_CALL ) {
        state->attr |= ROUTINE_NEEDS_DS_LOADED;
    }
    if( cclass_target & FECALL_X86_PARMS_STACK_RESERVE ) {
        state->attr |= ROUTINE_STACK_RESERVE;
    }
    if( cclass_target & FECALL_X86_PARMS_PREFER_REGS ) {
        state->attr |= ROUTINE_PREFER_REGS;
    }
    if( cclass_target & FECALL_X86_FARSS ) {
        state->attr |= ROUTINE_FARSS;
    }
    if( cclass_target & FECALL_X86_NEEDS_BP_CHAIN ) {
        state->attr |= ROUTINE_NEEDS_BP_CHAIN;
    }
    if( state == &CurrProc->state ) {
        if( cclass_target & (FECALL_X86_GENERATE_STACK_FRAME | FECALL_X86_PROLOG_HOOKS | FECALL_X86_EPILOG_HOOKS) ) {
            CurrProc->prolog_state |= PST_PROLOG_FAT;
            state->attr |= ROUTINE_NEEDS_PROLOG;
        }
        if( cclass_target & FECALL_X86_PROLOG_HOOKS ) {
            CurrProc->prolog_state |= PST_PROLOG_HOOKS;
        }
        if( cclass_target & FECALL_X86_EPILOG_HOOKS ) {
            CurrProc->prolog_state |= PST_EPILOG_HOOKS;
        }
        if( cclass_target & FECALL_X86_PROLOG_FAT_WINDOWS ) {
            CurrProc->prolog_state |= PST_PROLOG_FAT;
        }
        if( cclass_target & FECALL_X86_EMIT_FUNCTION_NAME ) {
            CurrProc->prolog_state |= PST_FUNCTION_NAME;
        }
        if( cclass_target & FECALL_X86_THUNK_PROLOG ) {
            CurrProc->prolog_state |= PST_PROLOG_THUNK;
        }
        if( cclass_target & FECALL_X86_GROW_STACK ) {
            CurrProc->prolog_state |= PST_GROW_STACK;
        }
        if( cclass_target & FECALL_X86_TOUCH_STACK ) {
            CurrProc->prolog_state |= PST_TOUCH_STACK;
        }
        if( cclass_target & FECALL_X86_LOAD_RDOSDEV_ON_ENTRY ) {
            CurrProc->prolog_state |= PST_PROLOG_RDOSDEV;
        }
    }
    type_class = ReturnTypeClass( tipe, state->attr );
    i = 0;
    parm_dst = &parms[0];
    for( parm_src = FEAuxInfo( aux, FEINF_PARM_REGS ); !HW_CEqual( *parm_src, HW_EMPTY ); ++parm_src ) {
        *parm_dst = *parm_src;
        if( HW_Ovlap( *parm_dst, state->unalterable ) ) {
            FEMessage( FEMSG_BAD_SAVE, aux );
        }
        HW_CTurnOff( *parm_dst, HW_UNUSED );
        parm_dst++;
        i++;
    }
    *parm_dst = *parm_src;
    i++;
    state->parm.table = CGAlloc( i*sizeof( hw_reg_set ) );
    Copy( parms, state->parm.table, i * sizeof( hw_reg_set ) );
    HW_CAsgn( state->parm.used, HW_EMPTY );
    state->parm.curr_entry = state->parm.table;
    state->parm.offset  = 0;
    if( tipe == TypeNone ) {
        HW_CAsgn( state->return_reg, HW_EMPTY );
    } else if( type_class == XX ) {
        if( cclass_target & FECALL_X86_SPECIAL_STRUCT_RETURN ) {
            pregs = FEAuxInfo( aux, FEINF_STRETURN_REG );
            state->return_reg = *pregs;
            state->attr |= ROUTINE_HAS_SPECIAL_RETURN;
        } else {
            state->return_reg = StructReg();
        }
        if( (state->attr & ROUTINE_ALLOCS_RETURN) == 0 ) {
            tmp = ReturnReg( WD, false );
            HW_TurnOn( state->modify, tmp );
        }
    } else {
        if( cclass_target & FECALL_X86_SPECIAL_RETURN ) {
            pregs = FEAuxInfo( aux, FEINF_RETURN_REG );
            state->return_reg = *pregs;
            state->attr |= ROUTINE_HAS_SPECIAL_RETURN;
        } else {
            state->return_reg = ReturnReg( type_class, _NPX( state->attr ) );
        }
    }
    UpdateReturn( state, tipe, type_class, aux );
    return( type_class );
}


void    UpdateReturn( call_state *state, type_def *tipe, type_class_def type_class, aux_handle aux )
/**************************************************************************************************/
{
    hw_reg_set  normal;

    if( _FPULevel( FPU_87 )
      && _NPX( state->attr )
      && (tipe->attr & TYPE_FLOAT) ) {
        HW_COnlyOn( state->return_reg, HW_ST0 );
    } else {
        HW_CTurnOff( state->return_reg, HW_FLTS );
    }
    if( tipe == TypeNone ) {
        if( HW_CEqual( state->return_reg, HW_EMPTY ) )
            return;
        FEMessage( FEMSG_BAD_RETURN_REGISTER, aux );
        HW_CAsgn( state->return_reg, HW_EMPTY );
        state->attr &= ~ROUTINE_HAS_SPECIAL_RETURN;
    } else if( type_class == XX ) {
        normal = ReturnReg( WD, _NPX( state->attr ) );
        if( HW_Equal( state->return_reg, normal ) )
            return;
        if( HW_CEqual( state->return_reg, HW_EMPTY ) )
            return;
//        if( !HW_Ovlap( state->return_reg, state->unalterable )
//          && IsRegClass( state->return_reg, WD ) )
//            return;
        if( IsRegClass( state->return_reg, WD ) )
            return;
        FEMessage( FEMSG_BAD_RETURN_REGISTER, aux );
        state->return_reg = normal;
        state->attr &= ~ROUTINE_HAS_SPECIAL_RETURN;
    } else {
        normal = ReturnReg( type_class, _NPX( state->attr ) );
        if( HW_Equal( state->return_reg, normal ) )
            return;
//        if( !HW_Ovlap( state->return_reg, state->unalterable )
//          && IsRegClass( state->return_reg, type_class ) )
//            return;
        if( IsRegClass( state->return_reg, type_class ) )
            return;
        FEMessage( FEMSG_BAD_RETURN_REGISTER, aux );
        state->return_reg = normal;
        state->attr &= ~ROUTINE_HAS_SPECIAL_RETURN;
    }
}


hw_reg_set      CallZap( call_state *state )
/******************************************/
{
    hw_reg_set  zap;
    hw_reg_set  tmp;

    zap = state->modify;
    if( (state->attr & ROUTINE_MODIFY_EXACT) == 0 ) {
        HW_TurnOn( zap, state->parm.used );
        HW_TurnOn( zap, state->return_reg );
        zap = FullReg( zap );
        tmp = ReturnReg( WD, _NPX( state->attr ) );
        HW_TurnOn( zap, tmp );
    }
    return( zap );
}



hw_reg_set      MustSaveRegs( void )
/**********************************/
{
    hw_reg_set  save;
    hw_reg_set  tmp;

    HW_CAsgn( save, HW_FULL );
    HW_TurnOff( save, CurrProc->state.modify );
    HW_CTurnOff( save, HW_UNUSED );
    if( CurrProc->state.attr & ROUTINE_MODIFY_EXACT ) {
        HW_TurnOff( save, CurrProc->state.return_reg );
    } else {
        tmp = CurrProc->state.parm.used;
        HW_TurnOn( tmp, CurrProc->state.return_reg );
        tmp = FullReg( tmp );
        HW_TurnOff( save, tmp );
    }
    tmp = StackReg();
    HW_TurnOff( save, tmp );
    if( HW_CEqual( CurrProc->state.return_reg, HW_EMPTY ) ) {
        tmp = ReturnReg( WD, _NPX( CurrProc->state.attr ) );
        HW_TurnOff( save, tmp );
    }
    tmp = CurrProc->state.unalterable;
    HW_TurnOff( tmp, DisplayReg() );
    HW_TurnOff( tmp, StackReg() );
    HW_TurnOff( save, tmp );
    return( save );
}

hw_reg_set      SaveRegs( void )
/******************************/
{
    hw_reg_set  save;
    save = MustSaveRegs();
    HW_OnlyOn( save, CurrProc->state.used );
    return( save );
}

