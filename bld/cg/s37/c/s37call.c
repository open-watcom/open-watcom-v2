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
#include "opcodes.h"
#include "procdef.h"
#include "model.h"
#include "addrname.h"
#include "cgdefs.h"
#include "hostsys.h"
#include "cgaux.h"
#include "rtclass.h"
#include "zoiks.h"
#include "makeins.h"
#include "s37call.def"

extern    type_def              *TypeNone;
extern    type_class_def        ClassPointer;
extern    proc_def              *CurrProc;
extern  type_length             TypeClassSize[];

extern  name            *AllocRegName(hw_reg_set);
extern  name            *AllocTemp(type_class_def);
extern  temp_name       *BGNewTemp(type_def*);
extern  type_def        *QParmType(type_def*);
extern  name            *AllocUserTemp(pointer,type_class_def);
extern  void            BGDone(an);
extern  an              MakeTempAddr(name*,type_def*);
extern  an              MakeAddrName(cg_class,sym_handle,type_def*);
extern  hw_reg_set      ParmReg(type_class_def,parm_state*);
extern  type_length     ParmMem(type_def*,parm_state*);
extern  hw_reg_set      ActualParmReg(hw_reg_set);
extern  void            AddIns(instruction*);
extern  type_class_def  TypeClass(type_def*);
extern  void            DbgParmLoc(name_def*);
extern  bool            AssgnParms(cn,bool);
extern  type_class_def  AddCallBlock(sym_handle,type_def*);
extern  void            AddCallIns(instruction*,cn);
extern  void            FreeCallNode(cn);
extern  name            *SAllocIndex(name*,name*,type_length,type_class_def,type_length);
extern  name            *SAllocTemp(type_class_def,type_length);
extern  name            *AllocIndex(name*,name*,type_length,type_class_def);
extern  hw_reg_set      StackReg(void);
extern  hw_reg_set      WordReg(int);
extern  hw_reg_set      FixedRegs(void);
extern  type_length     PushSize(type_length);
extern  name            *AllocS32Const(signed_32);
extern  name            *AllocIntConst(int);
extern  void            FEMessage(int,pointer);
extern  reg_num         RegTrans(hw_reg_set);
extern  hw_reg_set      CallZap(call_state*);
extern  label_handle    RTLabel(int);
extern  name            *AllocMemory(pointer,type_length,cg_class,type_class_def);
extern  name            *AllocAddrConst(name*,int,constant_class,type_class_def);

extern  an      BGCall( cn call, bool use_return, bool in_line ) {
/****************************************************************/

    instruction         *call_ins;
    instruction         *conv_ins;
    call_state          *state;
    name                *result;
    name                *call_result;
    an                  retv;

    AssgnParms( call, in_line );

    call_ins = call->ins;
    state = call->state;

    if( state->attr & ROUTINE_MODIFIES_NO_MEMORY ) {
        call_ins->flags.call_flags |= CALL_WRITES_NO_MEMORY;
    }
    if( state->attr & ROUTINE_READS_NO_MEMORY ) {
        call_ins->flags.call_flags |= CALL_READS_NO_MEMORY;
    }
    if( state->attr & ROUTINE_OS ) {
        call_ins->flags.call_flags |= CALL_OS_LINKAGE;
    }
    if( use_return == FALSE ) {
        call_ins->flags.call_flags |= CALL_IGNORES_RETURN;
    }

    result = BGNewTemp( call->tipe );
    if( call_ins->type_class == XX ) {
        call_ins->result = SAllocIndex( AllocRegName( StackReg() ), 0, 0, XX,
                                        call->tipe->length );
        call_result = call_ins->result;
    } else if( HW_CEqual( state->return_reg, HW_EMPTY ) ) {
        call_ins->result = AllocIndex( AllocRegName( StackReg() ),
                                       0, 0, call_ins->type_class );
        call_result = call_ins->result;
    } else {
        call_ins->result = AllocRegName( state->return_reg );
        call_result = NULL;
    }
    AddCallIns( call_ins, call );
    if( use_return ) {
        if( call_result == NULL ) {
            call_result = AllocRegName( state->return_reg );
        }
        if( call_ins->type_class != XX ){
            conv_ins = MakeConvert( call_result, result, result->n.name_class,
                                    call_ins->result->n.name_class );
        }else{
            conv_ins = MakeMove( call_result, result, XX );
        }
        AddIns( conv_ins );
    }
    retv = MakeTempAddr( result, call->tipe );
    FreeCallNode( call );
    return( retv );
}


extern  void    BGProcDecl( sym_handle sym, type_def *tipe ) {
/************************************************************/

    type_class_def      class;

    class = AddCallBlock( sym, tipe );
    SaveModel = Model;
}


extern  name    *StReturn( an retval, type_def *tipe, instruction **pins ) {
/**************************************************************************/

    name        *ret;

    if( tipe->length > CurrProc->state.parm.offset ) {
        CurrProc->state.parm.offset = tipe->length; /* make room on stack */
    }
    ret =  SAllocIndex( AllocRegName( RetValReg() ), NULL,
                   0, TypeClass( retval->tipe ), tipe->length );
    *pins = NULL;
    return( ret );
}

static  hw_reg_set      RetValReg() {
/************************************/

    return( WordReg( CurrProc->state.regs.AR ) );
}



extern  void    FPPushParms() {
/*****************************/
    /* Intentionally a stub */
}

extern  void    InitTargProc() {
/******************************/

    CurrProc->targ.far_local_size = 0;
}


extern  void    SaveToTargProc() {
/********************************/

}


extern  void    RestoreFromTargProc() {
/*************************************/

}

extern  instruction *   PushOneParm( instruction *ins, name *curr,
                                     type_class_def class,
                                     type_length *parm_offset ) {
/**************************************************************/

    instruction *new;
    name        *index;
    name        *temp;
    type_length offset;
    type_length size;

    ins = ins;
    index = AllocRegName( StackReg() );
    offset = *parm_offset;
    if( *parm_offset > 4096 ) {
        temp = AllocTemp( WD );
        AddIns( MakeMove( index, temp, WD ) );
        AddIns( MakeBinary( OP_ADD, temp, AllocS32Const( *parm_offset ),
                            temp, WD ) );
        offset = 0;
        index = temp;
    }
    if( curr->n.class == N_CONSTANT ) {
        size = TypeClassSize[ class ];
    } else {
        size = curr->n.size;
    }
    index = SAllocIndex( index, NULL, offset, class, size );
    new = MakeUnary( OP_MOV, curr, index, class );
    AddIns( new );
    *parm_offset += PushSize( size );
    return( new );
}


static  bool    ZappedBy( cn call, reg_num reg ) {
/***********************************************/

    hw_reg_set  zap, this;

    zap = CallZap( call->state );
    this = WordReg( reg );
    if( HW_Ovlap( zap, this ) ) return( TRUE );
    if( !( call->state->attr & ROUTINE_OS ) ) return( FALSE );
    if( call->state->regs.SA == reg ) return( TRUE );
    if( call->state->regs.PR == reg ) return( TRUE );
    return( FALSE );
}


static  bool    Used[16];

static  void    InitSaveRegs() {
/******************************/

    reg_num i;

    for( i = 0; i < 15; ++i ) {
        Used[ i ] = FALSE;
    }
}


static  bool    GoodSaveReg( cn call, reg_num reg ) {
/***********************************************/

    hw_reg_set  fixed, this;

    if( Used[ reg ] ) return( FALSE );
    if( ZappedBy( call, reg ) ) return( FALSE );
    this = WordReg( reg );
    fixed = FixedRegs();
    if( HW_Ovlap( this, fixed ) ) return( FALSE );
    Used[ reg ] = TRUE;
    return( TRUE );
}


static  reg_num     FindSaveReg( cn call ) {
/**************************************/

    reg_num         reg;

    if( GoodSaveReg( call, CurrProc->state.regs.RA ) ) {
        return( CurrProc->state.regs.RA );
    }
    if( GoodSaveReg( call, CurrProc->state.regs.LN ) ) {
        return( CurrProc->state.regs.LN );
    }
    for( reg = 0; reg <= 15; ++reg ) {
        if( GoodSaveReg( call, reg ) ) return( reg );
    }
    return( NO_REGISTER );
}


static  reg_num     SaveReg( cn call, reg_num to_save ) {
/***********************************************/

    reg_num         reg;

    if( !ZappedBy( call, to_save ) ) return( NO_REGISTER );
    reg = FindSaveReg( call );
    AddIns( MakeMove( AllocRegName( WordReg( to_save ) ),
                      AllocRegName( WordReg( reg ) ), WD ) );
    return( reg );
}


extern  void    PreCall( cn call ) {
/**********************************/

    name        *sp;
    name        *sa;
    name        *pr;
    hw_reg_set  used;
    type_length bump;
    call_state  *state;
    call_state  *cstate;

    InitSaveRegs();
    used = call->ins->operands[CALL_OP_USED]->r.reg;
    state = &CurrProc->state;
    cstate = call->state;
    if( cstate->regs.SP != state->regs.SP ) {
        FEMessage( MSG_FATAL, "Routines must have the same stack register" );
    }
    cstate->regs.sBR = SaveReg( call, state->regs.BR );
    cstate->regs.sAR = SaveReg( call, state->regs.AR );
    cstate->regs.sGP = NO_REGISTER;
    if( state->regs.GP != NO_REGISTER ) {
        cstate->regs.sGP = SaveReg( call, state->regs.GP );
    }
    cstate->regs.sPA = NO_REGISTER;
    if( state->regs.PA != state->regs.AR ) {
        cstate->regs.sPA = SaveReg( call, state->regs.PA );
    }
    cstate->regs.sSA = NO_REGISTER;
    if( state->attr & ROUTINE_OS ) {
        cstate->regs.sSA = SaveReg( call, state->regs.SA );
    }
    bump = 0;
    sp = AllocRegName( StackReg() );
    if( call->parm_size != 0 && ( cstate->attr & ROUTINE_OS ) ) {
        pr = AllocRegName( WordReg( cstate->regs.PR ) );
        HW_TurnOn( used, pr->r.reg );
        AddIns( MakeMove( sp, pr, WD ) );
        bump += call->parm_size;
    }
    sa = AllocRegName( WordReg( cstate->regs.SA ) );
    if( !( state->attr & ROUTINE_OS )
      && ( cstate->attr & ROUTINE_OS ) ) {
        HW_TurnOn( used, sa->r.reg );
        AddIns( MakeBinary( OP_ADD, sp, AllocS32Const( bump ), sa, PT ) );
        AddIns( MakeMove( AllocIntConst(0), AllocIndex(sa,NULL,4,WD), WD ) );
        bump += 18*4;
    }
    if( call->ins->head.opcode == OP_CALL && ( cstate->attr & ROUTINE_FUNC ) ) {
        AddIns( MakeMove( AllocAddrConst(call->name->u.name,0,CONS_ADDRESS,WD),
                          AllocIndex( sa, NULL, 0, WD ), WD ) );
        call->name->u.name =
                AllocMemory( RTLabel( RT_OSCALL - BEG_RTNS ), 0, CG_LBL, U4 );
    }
    if( bump ) {
        AddIns( MakeBinary( OP_ADD, sp, AllocS32Const( bump ), sp, PT ) );
    }
    call->ins->operands[CALL_OP_USED] = AllocRegName( used );
}


static  void    RestReg( reg_num reg, reg_num saved ) {
/*****************************************************/

    if( saved == NO_REGISTER ) return;
    AddIns( MakeMove( AllocRegName( WordReg( saved ) ),
                      AllocRegName( WordReg( reg ) ), WD ) );
}


extern  void    PostCall( cn call ) {
/***********************************/

    name        *reg;
    type_length size;
    call_state  *state;
    call_state  *cstate;
    reg_num     retval;
    hw_reg_set  fixed;

    state = &CurrProc->state;
    cstate = call->state;
    fixed = FixedRegs();
    if( !( call->ins->flags.call_flags & CALL_IGNORES_RETURN ) ){
        reg = call->ins->result;
        if( reg->n.class == N_INDEXED ){
            reg = reg->i.index;
        }
        if( reg->n.class != N_REGISTER ){
            FEMessage( MSG_FATAL, "Return value not index or reg" );
        }
        if( HW_Ovlap( fixed, reg->r.reg ) ){
            retval = SaveReg( call, RegTrans( reg->r.reg ) );
            call->state->return_reg = WordReg( retval );
        }
    }
    RestReg( state->regs.BR, cstate->regs.sBR );
    RestReg( state->regs.AR, cstate->regs.sAR );
    RestReg( state->regs.GP, cstate->regs.sGP );
    if( state->regs.PA != state->regs.AR ) {
        RestReg( state->regs.PA, cstate->regs.sPA );
    }
    if( state->attr & ROUTINE_OS ) {
        RestReg( state->regs.SA, cstate->regs.sSA );
    }
    if( cstate->attr & ROUTINE_OS ) {
        size = call->parm_size;
        if( !( state->attr & ROUTINE_OS ) ) size += 18*4;
        reg = AllocRegName( StackReg() );
        AddIns( MakeBinary( OP_SUB, reg, AllocS32Const( size ), reg, PT ) );
    }
}

extern  void    PushInSameBlock( instruction *ins ) {
/***************************************************/

    ins=ins;
}

extern  type_def        *PassParmType( type_def* tipe, call_class class ) {
/*************************************************************************/

    class = class;
    return( QParmType( tipe ) );
}
