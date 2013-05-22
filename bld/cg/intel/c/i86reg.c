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
#include "cgdefs.h"
#include "coderep.h"
#include "cgaux.h"
#include "cgmem.h"
#include "data.h"
#include "feprotos.h"

extern  hw_reg_set      FullReg(hw_reg_set);
extern  bool            IsRegClass(hw_reg_set,type_class_def);
extern  hw_reg_set      ReturnReg(type_class_def,bool);
extern  hw_reg_set      StructReg( void );
extern  byte            *Copy(void*,void*,uint);
extern  type_class_def  ReturnClass(type_def*,call_attributes);
extern  hw_reg_set      FixedRegs( void );
extern  hw_reg_set      StackReg( void );
extern  hw_reg_set      DisplayReg( void );
extern  int             SizeDisplayReg( void );
extern  hw_reg_set      AllCacheRegs( void );

/* forward declaration */
extern  void            UpdateReturn( call_state *state, type_def *tipe,
                                      type_class_def class, aux_handle aux );

#define _NPX( x ) ( !( (x) & ROUTINE_NO_8087_RETURNS ) )

extern  type_class_def  CallState( aux_handle aux,
                                  type_def *tipe, call_state *state ) {
/*********************************************************************/

    call_class          cclass;
    type_class_def      class;
    uint                i;
    hw_reg_set          parms[ 10 ];
    hw_reg_set          *parm_src;
    hw_reg_set          *parm_dst;
    hw_reg_set          *pregs;
    hw_reg_set          tmp;

    state->unalterable = FixedRegs();
    pregs = FEAuxInfo( aux, SAVE_REGS );
    HW_CAsgn( state->modify, HW_FULL );
    HW_TurnOff( state->modify, *pregs );
    HW_CTurnOff( state->modify, HW_UNUSED );
    state->used = state->modify;     /* anything not saved is used*/
    tmp = state->used;
    HW_TurnOff( tmp, StackReg() );
    HW_CTurnOff( tmp, HW_BP );  // should be able to call routine which modifies BP
    if( HW_Ovlap( state->unalterable, tmp ) ) {
        FEMessage( MSG_BAD_SAVE, aux );
    }
    state->attr = ROUTINE_REMOVES_PARMS;
    cclass = *(call_class *)FEAuxInfo( aux, CALL_CLASS );
    if( cclass & INTERRUPT ) {
        state->attr |= ROUTINE_INTERRUPT;
    } else if( cclass & FAR_CALL ) {
        state->attr |= ROUTINE_LONG;
    } else if( cclass & FAR16_CALL ) {
        state->attr |= ROUTINE_FAR16;
    }
    if( cclass & CALLER_POPS ) {
        state->attr &= ~ROUTINE_REMOVES_PARMS;
    }
    if( cclass & SUICIDAL ) {
        state->attr |= ROUTINE_NEVER_RETURNS;
    }
    if( cclass & ROUTINE_RETURN ) {
        state->attr |= ROUTINE_ALLOCS_RETURN;
    }
    if( cclass & NO_STRUCT_REG_RETURNS ) {
        state->attr |= ROUTINE_NO_STRUCT_REG_RETURNS;
    }
    if( cclass & NO_FLOAT_REG_RETURNS ) {
        state->attr |= ROUTINE_NO_FLOAT_REG_RETURNS;
        state->attr |= ROUTINE_NO_8087_RETURNS;
    }
    if( cclass & NO_8087_RETURNS ) {
        state->attr |= ROUTINE_NO_8087_RETURNS;
    }
    if( cclass & MODIFY_EXACT ) {
        state->attr |= ROUTINE_MODIFY_EXACT;
    }
    if( cclass & NO_MEMORY_CHANGED ) {
        state->attr |= ROUTINE_MODIFIES_NO_MEMORY;
    }
    if( cclass & NO_MEMORY_READ ) {
        state->attr |= ROUTINE_READS_NO_MEMORY;
    }
    if( cclass & LOAD_DS_ON_ENTRY ) {
        state->attr |= ROUTINE_LOADS_DS;
    }
    if( cclass & LOAD_DS_ON_CALL ) {
        state->attr |= ROUTINE_NEEDS_DS_LOADED;
    }
    if( cclass & PARMS_STACK_RESERVE ) {
        state->attr |= ROUTINE_STACK_RESERVE;
    }
    if( cclass & PARMS_PREFER_REGS ) {
        state->attr |= ROUTINE_PREFER_REGS;
    }
    if( state == &CurrProc->state ) {
        if( cclass & ( GENERATE_STACK_FRAME | PROLOG_HOOKS | EPILOG_HOOKS ) ) {
            CurrProc->prolog_state |= GENERATE_FAT_PROLOG;
            state->attr |= ROUTINE_NEEDS_PROLOG;
        }
        if( cclass & PROLOG_HOOKS ) {
            CurrProc->prolog_state |= GENERATE_PROLOG_HOOKS;
        }
        if( cclass & EPILOG_HOOKS ) {
            CurrProc->prolog_state |= GENERATE_EPILOG_HOOKS;
        }
        if( cclass & FAT_WINDOWS_PROLOG ) {
            CurrProc->prolog_state |= GENERATE_FAT_PROLOG;
        }
        if( cclass & EMIT_FUNCTION_NAME ) {
            CurrProc->prolog_state |= GENERATE_FUNCTION_NAME;
        }
        if( cclass & THUNK_PROLOG ) {
            CurrProc->prolog_state |= GENERATE_THUNK_PROLOG;
        }
        if( cclass & GROW_STACK ) {
            CurrProc->prolog_state |= GENERATE_GROW_STACK;
        }
        if( cclass & TOUCH_STACK ) {
            CurrProc->prolog_state |= GENERATE_TOUCH_STACK;
        }
        if( cclass & LOAD_RDOSDEV_ON_ENTRY ) {
            CurrProc->prolog_state |= GENERATE_RDOSDEV_PROLOG;
        }
    }
    class = ReturnClass( tipe, state->attr );
    i = 0;
    parm_src = FEAuxInfo( aux, PARM_REGS );
    parm_dst = &parms[ 0 ];
    for(;;) {
        *parm_dst = *parm_src;
        if( HW_CEqual( *parm_dst, HW_EMPTY ) ) break;
        if( HW_Ovlap( *parm_dst, state->unalterable ) ) {
            FEMessage( MSG_BAD_SAVE, aux );
        }
        HW_CTurnOff( *parm_dst, HW_UNUSED );
        parm_dst++;
        parm_src++;
        i++;
    }
    i++;
    state->parm.table = CGAlloc( i*sizeof( hw_reg_set ) );
    Copy( parms, state->parm.table, i*sizeof( hw_reg_set ) );
    HW_CAsgn( state->parm.used, HW_EMPTY );
    state->parm.curr_entry = state->parm.table;
    state->parm.offset  = 0;
    if( tipe == TypeNone ) {
        HW_CAsgn( state->return_reg, HW_EMPTY );
    } else if( class == XX ) {
        if( cclass & SPECIAL_STRUCT_RETURN ) {
            pregs = FEAuxInfo( aux, STRETURN_REG );
            state->return_reg = *pregs;
            state->attr |= ROUTINE_HAS_SPECIAL_RETURN;
        } else {
            state->return_reg = StructReg();
        }
        if( !( state->attr & ROUTINE_ALLOCS_RETURN ) ) {
            tmp = ReturnReg( WD, FALSE );
            HW_TurnOn( state->modify, tmp );
        }
    } else {
        if( cclass & SPECIAL_RETURN ) {
            pregs = FEAuxInfo( aux, RETURN_REG );
            state->return_reg = *pregs;
            state->attr |= ROUTINE_HAS_SPECIAL_RETURN;
        } else {
            state->return_reg = ReturnReg( class, _NPX( state->attr ) );
        }
    }
    UpdateReturn( state, tipe, class, aux );
    return( class );
}


extern  void    UpdateReturn( call_state *state, type_def *tipe,
                              type_class_def class, aux_handle aux ) {
/********************************************************************/

    hw_reg_set  normal;

    if( _FPULevel( FPU_87 ) && _NPX( state->attr )
      && ( ( tipe->attr & TYPE_FLOAT ) != EMPTY ) ) {
        HW_COnlyOn( state->return_reg, HW_ST0 );
    } else {
        HW_CTurnOff( state->return_reg, HW_FLTS );
    }
    if( tipe == TypeNone ) {
        if( HW_CEqual( state->return_reg, HW_EMPTY ) ) return;
        FEMessage( MSG_BAD_RETURN_REGISTER, aux );
        HW_CAsgn( state->return_reg, HW_EMPTY );
        state->attr &= ~ROUTINE_HAS_SPECIAL_RETURN;
    } else if( class == XX ) {
        normal = ReturnReg( WD, _NPX( state->attr ) );
        if( HW_Equal( state->return_reg, normal ) ) return;
        if( HW_CEqual( state->return_reg, HW_EMPTY ) ) return;
        // if( !HW_Ovlap( state->return_reg, state->unalterable ) &&
        //    IsRegClass( state->return_reg, WD ) ) return;
        if( IsRegClass( state->return_reg, WD ) ) return;
        FEMessage( MSG_BAD_RETURN_REGISTER, aux );
        state->return_reg = normal;
        state->attr &= ~ROUTINE_HAS_SPECIAL_RETURN;
    } else {
        normal = ReturnReg( class, _NPX( state->attr ) );
        if( HW_Equal( state->return_reg, normal ) ) return;
        // if( !HW_Ovlap( state->return_reg, state->unalterable ) &&
        //    IsRegClass( state->return_reg, class ) ) return;
        if( IsRegClass( state->return_reg, class ) ) return;
        FEMessage( MSG_BAD_RETURN_REGISTER, aux );
        state->return_reg = normal;
        state->attr &= ~ROUTINE_HAS_SPECIAL_RETURN;
    }
}


extern  hw_reg_set      CallZap( call_state *state ) {
/****************************************************/

    hw_reg_set  zap;
    hw_reg_set  tmp;

    zap = state->modify;
    if( ( state->attr & ROUTINE_MODIFY_EXACT ) == EMPTY ) {
        HW_TurnOn( zap, state->parm.used );
        HW_TurnOn( zap, state->return_reg );
        zap = FullReg( zap );
        tmp = ReturnReg( WD, _NPX( state->attr ) );
        HW_TurnOn( zap, tmp );
    }
    return( zap );
}



extern  hw_reg_set      MustSaveRegs( void ) {
/**************************************/

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

extern  hw_reg_set      SaveRegs( void ) {
/**********************************/

   hw_reg_set   save;
    save = MustSaveRegs();
    HW_OnlyOn( save, CurrProc->state.used );
    return( save );
}

