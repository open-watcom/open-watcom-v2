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
#include "opcodes.h"
#include "procdef.h"
#include "model.h"
#include "cgmem.h"
#include "addrname.h"
#include "cgdefs.h"
#include "cgaux.h"
#include "regset.h"
#include "rtclass.h"
#include "zoiks.h"
#include "makeins.h"
#include "display.h"
#include "cgprotos.h"
#include "rtrtn.h"

extern  name            *SAllocMemory(pointer,type_length,cg_class,type_class_def,type_length);
extern  name            *AddrConst(name*,segment_id,constant_class);
extern  name            *AllocRegName(hw_reg_set);
extern  name            *AllocTemp(type_class_def);
extern  name            *NearSegment(void);
extern  name            *SAllocIndex(name*,name*,type_length,type_class_def,type_length);
extern  name            *SAllocTemp(type_class_def,type_length);
extern  seg_id          AskBackSeg(void);
extern  seg_id          SetOP(seg_id);
extern  type_class_def  TypeClass(type_def*);
extern  void            AddIns(instruction*);
extern  void            DataLabel(label_handle);
extern  name            *GenIns(an);
extern  name            *AllocS32Const(signed_32);
extern  an              MakeTempAddr(name*,type_def*);
extern  bool            AssgnParms(cn,bool);
extern  type_class_def  AddCallBlock(sym_handle,type_def*);
extern  void            AddCallIns(instruction*,cn);
extern  name            *SegmentPart(name*);
extern  name            *OffsetPart(name*);
extern  name            *DoParmDecl(sym_handle,type_def*,hw_reg_set);
extern  hw_reg_set      ReturnReg(type_class_def,bool);
extern  type_length     PushSize(type_length);
extern  type_def        *QParmType(sym_handle,sym_handle,type_def*);
extern  name            *AllocMemory(pointer,type_length,cg_class,type_class_def);
extern  void            ReverseParmNodeList(pn *);
extern  name            *STempOffset(name*,type_length,type_class_def,type_length);
extern  void            SuffixIns( instruction *, instruction * );
extern  name            *BGNewTemp( type_def *tipe );
extern  void            BGDone( an );


extern  type_def        *TypeInteger;
extern  type_def        *TypeNone;
extern  type_def        *TypeProcParm;
extern  type_class_def  ClassPointer;
extern  proc_def        *CurrProc;
extern  type_length     MaxStack;
extern  block           *CurrBlock;


extern  type_length     TypeClassSize[];

static  void    AddCall( instruction *ins, cn call );

#if _TARGET & _TARG_80386
static  void    Far16Parms( cn call ) {
/*************************************/

    instruction         *ins;
    type_length         parm_size;
    pn                  parm, next;
    instruction         *call_ins;
    name                *eax;
    name                *ecx;
    name                *esi;
    label_handle        lbl;
    type_length         offset;
    name                *parmlist;
    call_state          *state;
    rt_class            thunk_rtn;

    call_ins = call->ins;
    parm_size = 0;
    state = call->state;
    for( parm = call->parms; parm != NULL; parm = parm->next ) {
        parm_size += (( parm->name->tipe->length ) + 1) & ~1;
    }
    parmlist = SAllocTemp( XX, parm_size );
    parmlist->v.usage |= NEEDS_MEMORY+USE_IN_ANOTHER_BLOCK+USE_ADDRESS;
    offset = 0;
    for( parm = call->parms; parm != NULL; parm = parm->next ) {
        parm->name->u.ins->result = STempOffset( parmlist, offset,
                                                 TypeClass( parm->name->tipe ),
                                                 parm->name->tipe->length );
        offset += (( parm->name->tipe->length ) + 1) & ~1;
    }
    for( parm = call->parms; parm != NULL; parm = next ) {
        parm->name->format = NF_ADDR;
        BGDone( parm->name );
        next = parm->next;
        CGFree( parm );
    }
    eax = AllocRegName( HW_EAX );
    ecx = AllocRegName( HW_ECX );
    esi = AllocRegName( HW_ESI );
    HW_TurnOn( state->parm.used, eax->r.reg );
    HW_TurnOn( state->parm.used, ecx->r.reg );
    HW_TurnOn( state->parm.used, esi->r.reg );
    ins = MakeMove( AllocS32Const( parm_size ), ecx, WD );
    AddIns( ins );
    ins = MakeUnary( OP_LA, parmlist, esi, WD );
    AddIns( ins );
    if( ins->head.opcode == OP_CALL ) {
        ins = MakeUnary( OP_LA, call->name->u.name, eax, WD );
    } else {
        ins = MakeMove( GenIns( call->name ), eax, WD );
        call_ins->head.opcode = OP_CALL;
    }
    call_ins->num_operands = 2;
    AddIns( ins );
    if( call_ins->type_class == XX ) {
        if( state->attr & ROUTINE_ALLOCS_RETURN ) {
            thunk_rtn = RT_Far16Cdecl;
        } else {
            thunk_rtn = RT_Far16Pascal;
        }
    } else {
        thunk_rtn = RT_Far16Func;
    }
    lbl = RTLabel( thunk_rtn );
    call->name->u.name = AllocMemory( lbl, 0, CG_LBL, WD );
    call_ins->flags.call_flags |= CALL_FAR16 | CALL_POPS_PARMS;
    call_ins->operands[CALL_OP_USED] = AllocRegName( state->parm.used );
    call_ins->operands[CALL_OP_POPS] = AllocS32Const( 0 );
    call_ins->zap = &call_ins->operands[CALL_OP_USED]->r;
}
#endif


extern  an      BGCall( cn call, bool use_return, bool in_line ) {
/****************************************************************/

    instruction         *call_ins;
    call_state          *state;
    name                *ret_ptr = NULL;
    name                *result;
    name                *temp;
    name                *reg_name;
    instruction         *ret_ins = NULL;
    hw_reg_set          return_reg;
    hw_reg_set          zap_reg;

    if( call->name->tipe == TypeProcParm ) {
        SaveDisplay( OP_PUSH );
    }

    state = call->state;
    result = BGNewTemp( call->tipe );
    call_ins = call->ins;

/*   If we have a return value that won't fit in a register*/
/*   pass a pointer to result as the first parm*/

    if( call_ins->type_class == XX ) {
        if( _RoutineIsFar16( state->attr ) ) {
            if( state->attr & ROUTINE_ALLOCS_RETURN ) {
                HW_CAsgn( state->return_reg, HW_EAX );
            } else {
                HW_CAsgn( state->return_reg, HW_EBX );
            }
        }
        if( ( state->attr & ROUTINE_ALLOCS_RETURN ) == 0 ) {
            if( HW_CEqual( state->return_reg, HW_EMPTY ) ) {
                ret_ptr = AllocTemp( WD );
            } else {
                ret_ptr = AllocRegName( state->return_reg );
            }
            ret_ins = MakeUnary( OP_LA, result, ret_ptr, WD );
            HW_TurnOn( state->parm.used, state->return_reg );
            call_ins->flags.call_flags |= CALL_RETURNS_STRUCT;
        }
    }
    if( _IsTargetModel(FLOATING_DS) && (state->attr&ROUTINE_NEEDS_DS_LOADED) ) {
        HW_CTurnOn( state->parm.used, HW_DS );
    }
    if( _RoutineIsFar16( state->attr ) ) {
        #if _TARGET & _TARG_80386
            Far16Parms( call );
        #endif
    } else {
        if( AssgnParms( call, in_line ) ) {
            if( state->attr & ROUTINE_REMOVES_PARMS ) {
                call_ins->flags.call_flags |= CALL_POPS_PARMS;
            }
        }
    }

    if( state->attr & (ROUTINE_MODIFIES_NO_MEMORY | ROUTINE_NEVER_RETURNS) ) {
        /* a routine that never returns can not write any memory as far
            as this routine is concerned */
        call_ins->flags.call_flags |= CALL_WRITES_NO_MEMORY;
    }
    if( state->attr & ROUTINE_READS_NO_MEMORY ) {
        call_ins->flags.call_flags |= CALL_READS_NO_MEMORY;
    }
    if( state->attr & ROUTINE_NEVER_RETURNS ) {
        call_ins->flags.call_flags |= CALL_ABORTS;
    }
    if( _RoutineIsInterrupt( state->attr ) ) {
        call_ins->flags.call_flags |= CALL_INTERRUPT | CALL_POPS_PARMS;
    }
    if( use_return == FALSE ) {
        call_ins->flags.call_flags |= CALL_IGNORES_RETURN;
    }
    if( call_ins->type_class == XX ) {
        reg_name = AllocRegName( state->return_reg );
        if( state->attr & ROUTINE_ALLOCS_RETURN ) {
            call_ins->result = reg_name;
            AddCall( call_ins, call );
            if( use_return ) {
                temp = AllocTemp( WD ); /* assume near pointer*/
                AddIns( MakeMove( reg_name, temp, WD ) );
                temp = SAllocIndex( temp, NULL, 0,
                                    result->n.name_class, call->tipe->length );
                AddIns( MakeMove( temp, result, result->n.name_class ) );
            }
        } else {
            call_ins->result = result;
            AddIns( ret_ins );
            if( HW_CEqual( state->return_reg, HW_EMPTY ) ) {
                AddIns( MakeUnary( OP_PUSH, ret_ptr, NULL, WD ) );
                state->parm.offset += TypeClassSize[ WD ];
                call_ins->operands[ CALL_OP_POPS ] =
                        AllocS32Const( call_ins->operands[ CALL_OP_POPS ]->c.int_value + TypeClassSize[ WD ] );
                if( state->attr & ROUTINE_REMOVES_PARMS ) {
                    call_ins->flags.call_flags |= CALL_POPS_PARMS;
                }
            }
            AddCall( call_ins, call );
        }
    } else {
        return_reg = state->return_reg;
        zap_reg = call_ins->zap->reg;
        HW_CTurnOn( zap_reg, HW_FLTS );
        HW_OnlyOn( return_reg, zap_reg );
        call_ins->result = AllocRegName( return_reg );
        reg_name = AllocRegName( state->return_reg );
        AddCall( call_ins, call );
        if( use_return ) {
            ret_ins = MakeMove( reg_name, result, result->n.name_class );
            if( HW_COvlap( reg_name->r.reg, HW_FLTS ) ) {
                ret_ins->stk_entry = 1;
                ret_ins->stk_exit = 0;
            }
            AddIns( ret_ins );
        }
    }
    if( state->parm.offset != 0 && ( state->attr & ROUTINE_REMOVES_PARMS ) == 0 ) {
        reg_name = AllocRegName( HW_SP );
        AddIns( MakeBinary( OP_ADD, reg_name,
                AllocS32Const( state->parm.offset ), reg_name, WD ) );
    }
    return( MakeTempAddr( result, call->tipe ) );
}


extern  void    BGProcDecl( sym_handle sym, type_def *tipe ) {
/************************************************************/

    hw_reg_set          reg;
    name                *temp;
    type_class_def      class;
    seg_id              old;
    label_handle        lbl;

    SaveTargetModel = TargetModel;
    class = AddCallBlock( sym, tipe );
    if( tipe != TypeNone ) {
        if( class == XX ) {
            if( CurrProc->state.attr & ROUTINE_ALLOCS_RETURN ) {
                old = SetOP( AskBackSeg() );
                lbl = AskForNewLabel();
                DataLabel( lbl );
                DGUBytes( tipe->length );
                CurrProc->targ.return_points = SAllocMemory( lbl, 0, CG_LBL,
                                                        TypeClass( tipe ),
                                                        tipe->length );
                SetOP( old );
            } else {
                reg = CurrProc->state.return_reg;
                if( HW_CEqual( reg, HW_EMPTY ) ) {
                    temp = DoParmDecl( NULL, TypeInteger, HW_EMPTY );
                } else {
                    temp = AllocTemp( WD );
                    temp->v.usage |= USE_IN_ANOTHER_BLOCK;
                    AddIns( MakeMove( AllocRegName( reg ), temp, WD ) );
                    HW_TurnOn( CurrProc->state.parm.used, reg );
                }
                CurrProc->targ.return_points = temp;
            }
        }
    }
    if( _RoutineIsInterrupt( CurrProc->state.attr ) ) {
        TargetModel |= FLOATING_SS;
    }
}


extern  name    *StReturn( an retval, type_def *tipe, instruction **pins ) {
/**************************************************************************/

    name        *retp;
    name        *ptr;
    name        *off;
    name        *seg;
    hw_reg_set  reg;

    if( CurrProc->state.attr & ROUTINE_ALLOCS_RETURN ) {
        retp = CurrProc->targ.return_points;
        AddIns( MakeUnary( OP_LA, retp,
                      AllocRegName( CurrProc->state.return_reg ), WD ) );
        *pins = NULL;
    } else {
        if( _IsTargetModel( FLOATING_SS ) || _IsTargetModel( FLOATING_DS ) ) {
            ptr = AllocTemp( CP );
            off = OffsetPart( ptr );
            seg = SegmentPart( ptr );
            AddIns( MakeMove( AllocRegName( HW_SS ), seg, U2 ) );
        } else {
            ptr = AllocTemp( WD );
            off = ptr;
        }
        AddIns( MakeMove( CurrProc->targ.return_points, off, WD ) );
        retp = SAllocIndex( ptr, NULL,
                           0, TypeClass( retval->tipe ), tipe->length );
        reg = ReturnReg( WD, FALSE );
        *pins = MakeMove( CurrProc->targ.return_points,
                          AllocRegName( reg ), WD );
        CurrProc->state.return_reg = reg;
    }
    return( retp );
}


static  void    AddCall( instruction *ins, cn call ) {
/****************************************************/

    name        *proc_name;

    if( _IsTargetModel(FLOATING_DS) && (call->state->attr&ROUTINE_NEEDS_DS_LOADED) ) {
        AddIns( MakeMove( NearSegment(), AllocRegName( HW_DS ), U2 ) );
    }
    if( call->name->tipe == TypeProcParm ) {
        proc_name = AllocTemp( ClassPointer );
/* what to do?        proc_name->usage |= USE_REGISTER;*/
        AddIns( MakeMove( ins->operands[CALL_OP_ADDR],
                          proc_name, ClassPointer ));
        ins->operands[CALL_OP_ADDR] = proc_name;
        SetDisplay( GenIns( call->name ) );
        AddIns( ins );
        SaveDisplay( OP_POP );
    } else {
        AddCallIns( ins, call );
    }
}


extern  reg_set_index   CallIPossible( instruction *ins ) {
/*********************************************************/


     if( ins->operands[ CALL_OP_ADDR ]->n.name_class == CP ) return( RL_ );
     if( ins->operands[ CALL_OP_ADDR ]->n.name_class == PT ) return( RL_ );
#if _TARGET & _TARG_80386
     return( RL_DOUBLE );
#else
     return( RL_WORD );
#endif
}


extern  void    InitTargProc( void ) {
/******************************/

    CurrProc->targ.stack_check = NULL;
    CurrProc->targ.push_local_size = 0;
    CurrProc->targ.debug = NULL;
    CurrProc->targ.return_points = NULL;
    CurrProc->targ.sp_frame = FALSE;
    CurrProc->targ.sp_align = FALSE;
    CurrProc->targ.has_fd_temps = FALSE;
    CurrProc->targ.never_sp_frame = FALSE;
    CurrProc->targ.tls_index = NULL;
}


extern  void    SaveToTargProc( void ) {
/********************************/

    CurrProc->targ.max_stack = MaxStack;
}


extern  void    RestoreFromTargProc( void ) {
/*************************************/

    MaxStack = CurrProc->targ.max_stack;
}


extern  void    PushInSameBlock( instruction *ins ) {
/***************************************************/

    ins = ins;
    #if ( _TARGET & _TARG_80386 )
        while( ins->head.opcode != OP_BLOCK ) {
            ins = ins->head.next;
        }
        if( _BLOCK( ins ) != CurrBlock ) {
            CurrProc->targ.never_sp_frame = TRUE;
        }
    #endif
}


extern  instruction *   PushOneParm( instruction *ins, name *curr,
                                     type_class_def class,
                                     type_length offset,
                                     call_state *state ) {
/**************************************************************/

    instruction *new;
    int         size;

    state = state;
    offset = offset;
    new = MakeUnary( OP_PUSH, curr, NULL, class );
    SuffixIns( ins, new );
    if( curr->n.class == N_CONSTANT ) {
        size = TypeClassSize[ class ];
    } else {
        size = curr->n.size;
    }
    return( new );
}


extern  void    PreCall( cn call ) {
/**********************************/

    call = call;
}


extern  void    PostCall( cn call ) {
/***********************************/

    call = call;
}

extern  type_def        *PassParmType( sym_handle func, type_def* tipe, call_class class ) {
/******************************************************************************************/

    if( class & FAR16_CALL ) return( tipe );
    return( QParmType( func, NULL, tipe ) );
}
