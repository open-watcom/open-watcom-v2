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


#include "standard.h"
#include "coderep.h"
#include "procdef.h"
#include "model.h"
#include "cgaux.h"
#include "sysmacro.h"
#include "typedef.h"

#include "s37reg.def"

extern  proc_def        *CurrProc;

extern  hw_reg_set      ReturnReg(type_class_def,call_attributes);
extern type_class_def   ReturnClass(type_def*,call_attributes);
extern  byte            *Copy(void*,void*,uint);
extern  hw_reg_set      FixedRegs();
extern  hw_reg_set      StackReg();
extern  hw_reg_set      DisplayReg();
extern  int             SizeDisplayReg();
extern  pointer         FEAuxInfo(pointer,int);
extern  void            FEMessage(int,pointer);
extern  hw_reg_set      WordReg(int);
extern  reg_num         RegTrans(hw_reg_set);


#define IsIn( r, r1, r2, r3, r4, r5 ) \
    ((r)==(r1)||(r)==(r2)||(r)==(r3)||(r)==(r4)||(r)==(r5))



static  void    FindReg( reg_num *find, call_registers *regs ) {
/**************************************************************/

    reg_num     i;

    for( i = 15; i != 0; --i ) {
        if( !IsIn( i, regs->GP, regs->SP, regs->LN, regs->RA, regs->PR )
         && !IsIn( i, regs->SA, regs->AR, regs->BR, regs->PA, NO_REGISTER ) ) {
            *find = i;
            return;
        }
    }
}


static  void    SetLinkage( aux_handle aux, call_state *state ) {
/***************************************************************/

    linkage_regs        *linkage;
    call_registers      *regs;

    state->attr = EMPTY;
    switch( *(call_class *)FEAuxInfo( aux, CALL_CLASS ) & LINKAGES ) {
    case LINKAGE_OS:
        state->attr = ROUTINE_OS;
        break;
    case LINKAGE_OSFUNC:
        state->attr = ROUTINE_OS + ROUTINE_FUNC;
        break;
    case LINKAGE_OSENTRY:
        state->attr = ROUTINE_OS + ROUTINE_ENTRY;
        break;
    default:
        break;
    }
    linkage = (linkage_regs *)FEAuxInfo( aux, LINKAGE_REGS );
    regs = &state->regs;
    regs->GP = RegTrans( linkage->gp );
    regs->SP = RegTrans( linkage->sp );
    regs->LN = RegTrans( linkage->ln );
    regs->RA = RegTrans( linkage->ra );
    regs->PR = RegTrans( linkage->pr );
    regs->SA = RegTrans( linkage->sa );
    if( !( state->attr & ROUTINE_OS ) ) {
        regs->SA = 16;
        regs->PR = 17;
    }
#define NO NO_REGISTER
    if( IsIn( regs->GP, regs->SP, regs->LN, regs->RA, regs->PR, regs->SA )
     || IsIn( regs->SP, regs->LN, regs->RA, regs->PR, regs->SA, NO       )
     || IsIn( regs->LN, regs->RA, regs->PR, regs->SA, NO,       NO       )
     || IsIn( regs->RA, regs->PR, regs->SA, NO,       NO,       NO       )
     || IsIn( regs->PR, regs->SA, NO,       NO,       NO,       NO       ) ) {
        FEMessage( MSG_BAD_LINKAGE, aux );
    }
    regs->AR = NO_REGISTER;
    regs->BR = NO_REGISTER;
    regs->PA = NO_REGISTER;
    FindReg( &regs->AR, regs );
    FindReg( &regs->BR, regs );
    if( state->attr & ROUTINE_OS ) {
        FindReg( &regs->PA, regs );
    } else {
        regs->PA = regs->AR;
    }
}


extern  type_class_def  CallState( aux_handle aux,
                                  type_def *tipe, call_state *state ) {
/*********************************************************************/

    type_class_def      class;
    uint                i;
    hw_reg_set          parms[ 10 ];
    hw_reg_set          *pregs;


    SetLinkage( aux, state );
    state->unalterable = FixedRegs();
    pregs = FEAuxInfo( aux, SAVE_REGS );
    HW_CAsgn( state->modify, HW_FULL );
    HW_TurnOff( state->modify, *pregs );
    HW_CTurnOff( state->modify, HW_UNUSED );
    state->used = state->modify;     /* anything not saved is used*/
    if( HW_Ovlap( state->used, state->unalterable ) ) {
        FEMessage( MSG_BAD_SAVE, aux );
    }
    Copy( FEAuxInfo( aux, PARM_REGS ), parms, 10*sizeof( hw_reg_set ) );
    i = 0;
    while( !HW_CEqual( parms[i], HW_EMPTY ) ) {
        if( HW_Ovlap( parms[i], state->unalterable ) ) {
            FEMessage( MSG_BAD_SAVE, aux );
        }
        HW_CTurnOff( parms[i], HW_UNUSED );
        i++;
    }
    i++;
    _Alloc( state->parm.table, i*sizeof( hw_reg_set ) );
    Copy( parms, state->parm.table, i*sizeof( hw_reg_set ) );
    _Alloc( state->parm.table, i*sizeof( hw_reg_set ) );
    Copy( parms, state->parm.table, i*sizeof( hw_reg_set ) );
    HW_CAsgn( state->parm.used, HW_EMPTY );
    state->parm.curr_entry = state->parm.table;
    state->parm.offset  = 0;
    class = ReturnClass( tipe, state->attr );
    UpdateReturn( state, tipe, class, aux );
    return( class );
}


extern  void    UpdateReturn( call_state *state, type_def *tipe,
                              type_class_def class, aux_handle aux ) {
/********************************************************************/

    hw_reg_set          *pregs;

    tipe=tipe;
    pregs = FEAuxInfo( aux, RETURN_REG );
    if( !HW_CEqual( *pregs, HW_EMPTY ) ) {
        state->return_reg = *pregs;
    } else {
        state->return_reg = ReturnReg( class, state->attr );
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
    }
    tmp = WordReg( state->regs.RA );
    HW_TurnOn( zap, tmp );
    tmp = WordReg( state->regs.LN );
    HW_TurnOn( zap, tmp );
    return( zap );
}


extern  hw_reg_set      SaveRegs() {
/**********************************/

    hw_reg_set tmp;
    tmp = MustSaveRegs();
    HW_OnlyOn( tmp, CurrProc->state.used );
    return( tmp );
}


extern  hw_reg_set      MustSaveRegs() {
/**************************************/

    hw_reg_set  save;
    hw_reg_set  tmp;

    HW_CAsgn( save, HW_FULL );
    HW_TurnOff( save, CurrProc->state.modify );
    HW_CTurnOff( save, HW_UNUSED );
    HW_TurnOff( save, CurrProc->state.return_reg );
    if( CurrProc->state.attr & ROUTINE_MODIFY_EXACT ) {
        HW_TurnOff( save, CurrProc->state.parm.used );
    }
    tmp = StackReg();
    HW_TurnOff( save, tmp );
    if( HW_CEqual( CurrProc->state.return_reg, HW_EMPTY ) ) {
        tmp = ReturnReg( WD, CurrProc->state.attr );
    HW_TurnOff( save, tmp );
    }
    tmp = CurrProc->state.unalterable;
    HW_TurnOff( tmp, DisplayReg() );
    HW_TurnOff( tmp, StackReg() );
    HW_TurnOff( save, tmp );
    return( save );
}
