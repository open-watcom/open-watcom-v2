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
* Description:  PowerPC register classification and conventions.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "cgaux.h"
#include "cgmem.h"
#include "data.h"
#include "utils.h"
#include "feprotos.h"


extern  hw_reg_set      ReturnReg(type_class_def);
extern  hw_reg_set      *ParmRegs( void );
extern type_class_def   ReturnClass(type_def*,call_attributes);
extern  hw_reg_set      FixedRegs( void );
extern  hw_reg_set      StackReg( void );
extern  hw_reg_set      DisplayReg( void );
extern  int             SizeDisplayReg( void );
extern  hw_reg_set      ReturnAddrReg(void);
extern  void            InitPPCParmState( call_state * );
extern  void            UpdateReturn( call_state *, type_def *, type_class_def, aux_handle );


extern hw_reg_set SavedRegs( void )
/*********************************/
{
    hw_reg_set          saved;

    HW_CAsgn( saved, HW_EMPTY );
    HW_CTurnOn( saved, HW_R14 );
    HW_CTurnOn( saved, HW_R15 );
    HW_CTurnOn( saved, HW_R16 );
    HW_CTurnOn( saved, HW_R17 );
    HW_CTurnOn( saved, HW_R18 );
    HW_CTurnOn( saved, HW_R19 );
    HW_CTurnOn( saved, HW_R20 );
    HW_CTurnOn( saved, HW_R21 );
    HW_CTurnOn( saved, HW_R22 );
    HW_CTurnOn( saved, HW_R23 );
    HW_CTurnOn( saved, HW_R24 );
    HW_CTurnOn( saved, HW_R25 );
    HW_CTurnOn( saved, HW_R26 );
    HW_CTurnOn( saved, HW_R27 );
    HW_CTurnOn( saved, HW_R28 );
    HW_CTurnOn( saved, HW_R29 );
    HW_CTurnOn( saved, HW_R30 );
    HW_CTurnOn( saved, HW_R31 );
    HW_CTurnOn( saved, HW_F14 );
    HW_CTurnOn( saved, HW_F15 );
    HW_CTurnOn( saved, HW_F16 );
    HW_CTurnOn( saved, HW_F17 );
    HW_CTurnOn( saved, HW_F18 );
    HW_CTurnOn( saved, HW_F19 );
    HW_CTurnOn( saved, HW_F20 );
    HW_CTurnOn( saved, HW_F21 );
    HW_CTurnOn( saved, HW_F22 );
    HW_CTurnOn( saved, HW_F23 );
    HW_CTurnOn( saved, HW_F24 );
    HW_CTurnOn( saved, HW_F25 );
    HW_CTurnOn( saved, HW_F26 );
    HW_CTurnOn( saved, HW_F27 );
    HW_CTurnOn( saved, HW_F28 );
    HW_CTurnOn( saved, HW_F29 );
    HW_CTurnOn( saved, HW_F30 );
    HW_CTurnOn( saved, HW_F31 );
    return( saved );
}

extern  type_class_def  CallState( aux_handle aux,
                                  type_def *tipe, call_state *state )
/*******************************************************************/
{
    type_class_def      class;
    uint                i;
    hw_reg_set          parms[ 24 ];
    hw_reg_set          *parm_src;
    hw_reg_set          *parm_dst;

    state->unalterable = FixedRegs();
    HW_CAsgn( state->modify, HW_FULL );
    HW_TurnOff( state->modify, SavedRegs() );
    HW_CTurnOff( state->modify, HW_UNUSED );
    state->used = state->modify;     /* anything not saved is used*/
    state->attr = 0;
    i = 0;
    parm_src = ParmRegs();
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
    Copy( parms, state->parm.table, i * sizeof( hw_reg_set ) );
    HW_CAsgn( state->parm.used, HW_EMPTY );
    state->parm.curr_entry = state->parm.table;
    state->parm.offset  = 0;
    InitPPCParmState( state );
    class = ReturnClass( tipe, state->attr );
    if( *(call_class *)FEAuxInfo( aux, CALL_CLASS ) & HAS_VARARGS ) {
        state->attr |= ROUTINE_HAS_VARARGS;
    }
    UpdateReturn( state, tipe, class, aux );
    return( class );
}


extern  void    UpdateReturn( call_state *state, type_def *tipe,
                              type_class_def class, aux_handle aux )
/******************************************************************/
{
    tipe = tipe; aux = aux;
    state->return_reg = ReturnReg( class );
}

extern  hw_reg_set      RAReg( void )
/***********************************/
{
    return( HW_EMPTY );
}

extern  hw_reg_set      CallZap( call_state *state )
/**************************************************/
{
    hw_reg_set  zap;
    hw_reg_set  tmp;

    zap = state->modify;
    if( ( state->attr & ROUTINE_MODIFY_EXACT ) == EMPTY ) {
        HW_TurnOn( zap, state->parm.used );
        HW_TurnOn( zap, state->return_reg );
        HW_TurnOn( zap, ReturnAddrReg() );
        tmp = ReturnReg( WD );
        HW_TurnOn( zap, tmp );
    }
    return( zap );
}

extern  hw_reg_set      MustSaveRegs( void )
/******************************************/
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
        HW_TurnOff( save, tmp );
    }
    tmp = StackReg();
    HW_TurnOff( save, tmp );
    if( HW_CEqual( CurrProc->state.return_reg, HW_EMPTY ) ) {
        tmp = ReturnReg( WD );
        HW_TurnOff( save, tmp );
    }
    tmp = CurrProc->state.unalterable;
    HW_TurnOff( tmp, DisplayReg() );
    HW_TurnOff( tmp, StackReg() );
    HW_TurnOff( save, tmp );
    return( save );
}

extern  hw_reg_set      SaveRegs( void )
/**************************************/
{
    hw_reg_set   save;

    save = MustSaveRegs();
    HW_OnlyOn( save, CurrProc->state.used );
    return( save );
}

extern  bool            IsStackReg( name *n )
/*******************************************/
{
    if( n == NULL ) return( FALSE );
    if( n->n.class != N_REGISTER ) return( FALSE );
    if( !HW_CEqual( n->r.reg, HW_R1 ) &&
        !HW_CEqual( n->r.reg, HW_D1 ) ) return( FALSE );
    return( TRUE );
}

extern  hw_reg_set      HighOffsetReg( hw_reg_set regs )
/******************************************************/
{
    regs = regs;
    return( HW_EMPTY );
}

extern  hw_reg_set      LowOffsetReg( hw_reg_set regs )
/*****************************************************/
{
    regs = regs;
    return( HW_EMPTY );
}
