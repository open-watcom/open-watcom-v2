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
* Description:  Generate calls to runtime support routines.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "zoiks.h"
#include "seldef.h"
#include "cgauxinf.h"
#include "makeins.h"
#include "convins.h"
#include "data.h"
#include "rtrtn.h"
#include "objout.h"
#include "namelist.h"
#include "regalloc.h"
#include "makeblk.h"
#include "rgtbl.h"
#include "insutil.h"
#include "rtcall.h"
#include "optab.h"


extern  name            *GenFloat( name *, type_class_def );
extern  name            *GenConstData( const void *, type_class_def );
extern  void            UpdateLive( instruction *, instruction * );
extern  void            MoveSegRes( instruction *, instruction * );
extern  bool            SegIsSS( name * );
extern  name            *GetSegment( name * );
extern  void            DelSeg( instruction * );
extern  void            MoveSegOp( instruction *, instruction *, int );
extern  conflict_node   *NameConflict( instruction *, name * );
extern  int             NumOperands( instruction * );
extern  name            *AddrConst( name *, int, constant_class );

/*
 * If you add a new routine, let John know as the debugger recognizes
 * these.
 */

/*
    OK, so I'm going to go to hell for this, it works don't it?
*/
#define RL_SI   RL_TEMP_INDEX

rtn_info RTInfo[] = {
    #define PICK(e,name,op,class,left,right,result) {name, op, class, left, right, result},
    #include "_rtinfo.h"
    #undef PICK
};

static  call_class     rt_cclass = 0;

static  struct STRUCT_byte_seq( 6 ) Scn1 = {
     6, false,
    {0xF2,              /*      repne           */
     0xAE,              /*      scasb           */
     0xD1, 0xE1,        /*      shl     cx,1    */
     0x89, 0xCF}        /*      mov     di,cx   */
};

static  struct STRUCT_byte_seq( 2 ) Scn2 = {
     2, false,
    {0xF2,              /*      repne           */
     0xAF}              /*      scasw           */
};

static  struct STRUCT_byte_seq( 18 ) Scn4 = {
     18, false,
    {0x83, 0xC7, 0x02,  /* L1:  add     d1,2    */
     0x49,              /* L2:  dec     cx      */
     0x74, 0x08,        /*      je      L3      */
     0xAF,              /*      scasw           */
     0x75, 0xF7,        /*      jne     L1      */
     0x92,              /*      xchg    dx,ax   */
     0xAF,              /*      scasw           */
     0x92,              /*      xchg    dx,ax   */
     0x75, 0xF5,        /*      jne     L2      */
     0xD1, 0xE1,        /* L3:  shl     cx,1    */
     0x89, 0xCF}        /*      mov     di,cx   */
};


const char  *AskRTName( rt_class rtindex )
/****************************************/
{
    return( RTInfo[rtindex].nam );
}


extern  name    *Addressable( name *cons, type_class_def class )
/***************************************************************
    make sure a floating point constant is addressable (dropped
    it into memory if it isnt)
*/
{
    unsigned_64         buffer;

    if( cons->n.class == N_CONSTANT ) {
        switch( class ) {
        case FD:
        case FS:
            return( GenFloat( cons, class ) );
        case U8:
        case I8:
            buffer.u._32[I64LO32] = cons->c.lo.int_value;
            buffer.u._32[I64HI32] = cons->c.hi.int_value;
            return( GenConstData( &buffer, class ) );
        default:
            Zoiks( ZOIKS_138 );
            break;
        }
        return( GenFloat( cons, class ) );
    }
    return( cons );
}


static rt_class CheckForPCS( instruction *ins, rt_class rtindex )
/****************************************************************
    check to see if pointer subtract is really pointer - pointer
    or pointer - integer (PCS = Pointer Constant Subtract)
*/
{
    if( rtindex == RT_PTS ) {
        if( ins->operands[1]->n.name_class != PT
         && ins->operands[1]->n.name_class != CP ) {
            rtindex = RT_PCS;
        }
    }
    return( rtindex );
}


bool    RTLeaveOp2( instruction *ins )
/*************************************
    return true if it's a bad idea to put op2 into a temporary since we're
    gonna take the bugger's address in rMAKECALL anyway for FDD, FDC, EDA, etc
*/
{
    switch( ins->type_class ) {
    case FD:
        if( _FPULevel( FPU_87 ) )
            return( false );
        break;
/* -- This is not true now - I8 math and parameters are kept in registers -- [RomanT]
    case I8:
    case U8:
        break;
*/
    default:
        return( false );
    }
    if( NumOperands( ins ) != 2 )
        return( false );
    return( true );
}


static  void    FlipIns( instruction *ins )
/******************************************
   maybe flip the const/mem into the second operand so it goes in local data
*/
{
    name        *temp;

    switch( ins->head.opcode ) {
    case OP_ADD:
    case OP_MUL:
        break;
    default:
        return;
    }
    switch( ins->type_class ) {
    case FD:
    case I8:
    case U8:
        break;
    default:
        return;
    }
    temp = ins->operands[0];
    if( temp->n.class == N_CONSTANT
     || temp->n.class == N_MEMORY
     || temp->n.class == N_INDEXED ) {
        ins->operands[0] = ins->operands[1];
        ins->operands[1] = temp;
    }
}


instruction     *rMAKECALL( instruction *ins )
/*********************************************
    turn an instruction into the approprate runtime call sequence, using
    the tables above to decide where parms go.
*/
{
    rtn_info            *info;
    label_handle        lbl;
    instruction         *left_ins;
    instruction         *new_ins;
    instruction         *la_ins;
    instruction         *last_ins;
    name                *reg_name;
    hw_reg_set          regs;
    hw_reg_set          all_regs;
    name                *temp;
    name                *also_used;
    conflict_node       *conf;
    hw_reg_set          tmp;
    type_class_def      parm2_class;
    rt_class            rtindex;

    if( !_IsConvert( ins ) ) {
        rtindex = CheckForPCS( ins, LookupRoutine( ins ) );
    } else { /* rtindex might be wrong if we ran out of memory in ExpandIns*/
        rtindex = LookupConvertRoutine( ins );
    }
    FlipIns( ins );
    info = &RTInfo[rtindex];
    regs = FirstReg( info->left );
    all_regs = regs;
    tmp = ReturnReg( WD, false );
    HW_TurnOn( all_regs, tmp );
    left_ins = MakeMove( ins->operands[0], AllocRegName( regs ), info->operand_class );
    ins->operands[0] = left_ins->result;
    MoveSegOp( ins, left_ins, 0 );
    PrefixIns( ins, left_ins );
    regs = FirstReg( info->right );
    also_used = NULL;
    if( !HW_CEqual( regs, HW_EMPTY ) ) {
        if( info->right == RL_8 ) {
            temp = ins->operands[1];
            if( temp->n.class==N_TEMP && ( temp->t.temp_flags & CONST_TEMP ) ) {
                temp = temp->v.symbol;
            }
            if( temp->n.class == N_CONSTANT ) {
                temp = Addressable( temp, info->operand_class );
                la_ins = MakeUnary( OP_CAREFUL_LA, temp, AllocRegName( HW_SI ), U2 );
                also_used = la_ins->operands[0];
                ins->operands[1] = la_ins->result;
                PrefixIns( ins, la_ins );
                if( !SegIsSS( temp ) ) {
                    new_ins = MakeMove( GetSegment( temp ), AllocRegName(HW_ES), U2 );
                    HW_CTurnOn( all_regs, HW_ES );
                    PrefixIns( ins, new_ins );
                    ++rtindex;
                }
            } else if( temp->n.class == N_TEMP ) {
                la_ins = MakeUnary( OP_CAREFUL_LA,temp, AllocRegName( HW_SI ),U2 );
                also_used = temp;
                ins->operands[1] = la_ins->result;
                MoveSegOp( ins, la_ins, 0 );
                DelSeg( la_ins );
                PrefixIns( ins, la_ins );
            } else if( ins->num_operands == 3 ) {
                new_ins = MakeMove( ins->operands[2], AllocRegName(HW_ES), U2 );
                ins->operands[2] = new_ins->result;
                PrefixIns( ins, new_ins );
                la_ins = MakeUnary( OP_CAREFUL_LA, temp,
                                    AllocRegName( HW_SI ), U2 );
                also_used = temp;
                ins->operands[1] = la_ins->result;
                MoveSegOp( ins, la_ins, 0 );
                DelSeg( la_ins );
                PrefixIns( ins, la_ins );
                HW_CTurnOn( all_regs, HW_ES );
                ++rtindex;
            } else if( ( temp->n.class == N_MEMORY && !SegIsSS( temp ) ) ||
                       ( temp->n.class == N_INDEXED && temp->i.base != NULL &&
                         !SegIsSS( temp->i.base ) ) ) {
                la_ins = MakeUnary( OP_CAREFUL_LA, ins->operands[1], AllocRegName( HW_ES_SI ), PT );
                also_used = ins->operands[1];
                ins->operands[1] = la_ins->result;
                PrefixIns( ins, la_ins );
                HW_CTurnOn( all_regs, HW_ES );
                ++rtindex;
            } else {
                la_ins = MakeUnary( OP_CAREFUL_LA, temp, AllocRegName( HW_SI ), U2 );
                also_used = temp;
                ins->operands[1] = la_ins->result;
                MoveSegOp( ins, la_ins, 0 );
                DelSeg( la_ins );
                PrefixIns( ins, la_ins );
            }
            HW_CTurnOn( all_regs, HW_SI );
        } else {
            /* If I knew how to turn a register list index into a type class,
               I'd do that, and avoid this if */
            if( info->right == RL_SI ) {
                new_ins = MakeMove( ins->operands[1], AllocRegName( regs ), U2 );
            } else {
                new_ins = MakeMove( ins->operands[1], AllocRegName( regs ), info->operand_class );
            }
            ins->operands[1] = new_ins->result;
            MoveSegOp( ins, new_ins, 0 );
            HW_TurnOn( all_regs, regs );
            PrefixIns( ins, new_ins );
        }
    } else if( NumOperands( ins ) == 2 ) {
        if( rtindex == RT_DPOWI ) {
            parm2_class = I4;
        } else {
            parm2_class = ins->type_class;
        }
        new_ins = MakeUnary( OP_PUSH, ins->operands[1], NULL, parm2_class );
        MoveSegOp( ins, new_ins, 0 );
        PrefixIns( ins, new_ins );
    }
    reg_name = AllocRegName( all_regs );
    lbl = RTLabel( rtindex );
    new_ins = NewIns( 3 );
    new_ins->head.opcode = OP_CALL;
    new_ins->type_class = ins->type_class;
    new_ins->operands[CALL_OP_USED] = reg_name;
    if( also_used != NULL ) {
        switch( also_used->n.class ) {
        case N_REGISTER:
        case N_CONSTANT:
            _Zoiks( ZOIKS_047 );
            break;
        case N_MEMORY:
        case N_TEMP:
            also_used->v.usage |= ( USE_MEMORY | NEEDS_MEMORY );
            conf = also_used->v.conflict;
            if( conf != NULL ) {
                InMemory( conf );
            }
            break;
        case N_INDEXED:            /* Dec-20-88*/
            also_used = reg_name;  /* - the index doesn't need an index register!*/
        default:
            break;
        }
        new_ins->operands[CALL_OP_USED2] = also_used;
    } else {
        new_ins->operands[CALL_OP_USED2] = reg_name;
    }
    new_ins->operands[CALL_OP_ADDR] = AllocMemory( lbl, 0, CG_LBL, ins->type_class );
    new_ins->result = NULL;
    new_ins->num_operands = 2;         /* special case for OP_CALL*/
    new_ins->zap = &AllocRegName( all_regs )->r;/* all parm regs could be zapped*/
    last_ins = new_ins;
    if( ins->result != NULL ) {
        regs = FirstReg( info->result );
        tmp = regs;
        HW_TurnOn( tmp, new_ins->zap->reg );
        new_ins->zap = &AllocRegName( tmp )->r;
        reg_name = AllocRegName( regs );
        new_ins->result = reg_name;
        last_ins = MakeMove( reg_name, ins->result, ins->type_class );
        ins->result = last_ins->operands[0];
        MoveSegRes( ins, last_ins );
        SuffixIns( ins, last_ins );
        ReplIns( ins, new_ins );
    } else {                /* comparison, still need conditional jumps*/
        ins->operands[0] = AllocIntConst( 0 );
        ins->operands[1] = AllocIntConst( 1 );
        DelSeg( ins );
        DoNothing( ins );               /* just conditional jumps for ins*/
        PrefixIns( ins, new_ins );
        new_ins->ins_flags |= INS_CC_USED;
        last_ins = ins;
    }
    UpdateLive( left_ins, last_ins );
    return( left_ins );
}


extern  name    *ScanCall( tbl_control *table, name *value, type_class_def class )
/*********************************************************************************
    generates a fake call to a rutime routine that looks up "value" in a table
    and jumps to the appropriate case, using either a pointer or index
    returned by the "routine". The "routine" will be generated inline later.
    See BEAuxInfo for the code sequences generated. That will explain
    how the jump destination is determined as well.
*/
{
    instruction *new_ins;
    name        *reg_name;
    name        *result;
    name        *label;
    hw_reg_set  tmp;
    name        *temp_result;
    rt_class    rtindex;

    switch( class ) {
    case U1:
        rtindex = RT_SCAN1;
        break;
    case U2:
        rtindex = RT_SCAN2;
        break;
    case U4:
        rtindex = RT_SCAN4;
        break;
    default:
        rtindex = RT_NOP;
        break;
    }

    reg_name = AllocRegName( FirstReg( RTInfo[rtindex].left ) );
    new_ins = MakeConvert( value, reg_name, class, value->n.name_class );
    AddIns( new_ins );

    reg_name = AllocRegName( HW_CX );
    new_ins = MakeMove( AllocRegName( HW_CS ), reg_name, U2 );
    AddIns( new_ins );
    new_ins = MakeMove( reg_name, AllocRegName( HW_ES ), U2 );
    AddIns( new_ins );
    new_ins = MakeMove( AllocIntConst( table->size + 1 ), reg_name, U2 );
    AddIns( new_ins );

    reg_name = AllocRegName( HW_DI );
    if( class == U4 ) {
        label = AllocMemory( table, -2, CG_VTB, U2 );
    } else {
        label = AllocMemory( table, 0, CG_VTB, U2 );
    }
    label = AddrConst( label, AskCodeSeg(), CONS_OFFSET );
    new_ins = MakeMove( label, reg_name, U2 );
    AddIns( new_ins );

    new_ins = NewIns( 3 );
    new_ins->head.opcode = OP_CALL;
    new_ins->type_class = U2;
    tmp = FirstReg( RTInfo[rtindex].left );
    HW_CTurnOn( tmp, HW_ES_DI );
    HW_CTurnOn( tmp, HW_CX );
    new_ins->operands[CALL_OP_USED] = AllocRegName( tmp );
    new_ins->operands[CALL_OP_USED2] = new_ins->operands[CALL_OP_USED];
    new_ins->operands[CALL_OP_ADDR] = AllocMemory( RTLabel( rtindex ), 0, CG_LBL, U2 );
    new_ins->result = NULL;
    new_ins->num_operands = 2;
    new_ins->zap = &AllocRegName( HW_CX_DI )->r;
    new_ins->result = reg_name;
    AddIns( new_ins );

    result = AllocMemory( table, 0, CG_TBL, U2 ); /* so table gets freed!*/
    if( class == U2 ) {
        result = AllocRegName( HW_ES_DI );
        result = AllocIndex( result, NULL, ( table->size - 1 ) * 2, U2 );
    } else {
        result = AllocIndex( reg_name, result, 0, U2 );
    }
    // this is here because we can get ourselves into trouble
    // by hoisting expressions into the spot between the call and
    // the OP_SELECT instruction at the end of the block if those
    // expressions expand to instructions which require ECX.
    // Same goes for EDI in the U4 case.        BBB - July, 1996
    temp_result = AllocTemp( WD );
    new_ins = MakeMove( result, temp_result, WD );
    AddIns( new_ins );
    return( temp_result );
}


extern  instruction     *rMAKEFNEG( instruction *ins )
/*****************************************************
    negating a floating point value which is in the 386 registers only
    needs to change the register containing the exponent, so this is
    handled as a special case rather than using rMAKERTCALL that would
    assume all of the registers containing the number were used
    and modified by the call.
*/
{
    rtn_info            *info;
    label_handle        lbl;
    instruction         *left_ins;
    instruction         *new_ins;
    instruction         *last_ins;
    name                *reg_name;
    name                *exp_reg;
    rt_class            rtindex;

    rtindex = CheckForPCS( ins, LookupRoutine( ins ) );
    lbl = RTLabel( rtindex );
    info = &RTInfo[rtindex];
    reg_name = AllocRegName( FirstReg( info->left ) );
    left_ins = MakeMove( ins->operands[0], reg_name, info->operand_class );
    ins->operands[0] = left_ins->result;
    MoveSegOp( ins, left_ins, 0 );
    PrefixIns( ins, left_ins );
    if( ins->type_class == FD ) { /* exponent in AX*/
        exp_reg = AllocRegName( HW_AX );
    } else {                           /* exponent in DX*/
        exp_reg = AllocRegName( HW_DX );
    }
    new_ins = NewIns( 3 );
    new_ins->head.opcode = OP_CALL;
    new_ins->type_class = U2;
    new_ins->operands[CALL_OP_USED] = exp_reg;
    new_ins->operands[CALL_OP_USED2] = exp_reg;
    new_ins->operands[CALL_OP_ADDR] = AllocMemory( lbl, 0, CG_LBL, U2 );
    new_ins->result = NULL;
    new_ins->num_operands = 2;
    new_ins->zap = &exp_reg->r;
    last_ins = new_ins;
    new_ins->result = exp_reg;
    last_ins = MakeMove( reg_name, ins->result, info->operand_class );
    ins->result = last_ins->operands[0];
    MoveSegRes( ins, last_ins );
    SuffixIns( ins, last_ins );
    ReplIns( ins, new_ins );
    UpdateLive( left_ins, last_ins );
    return( left_ins );
}


extern  pointer BEAuxInfo( pointer hdl, aux_class request )
/**********************************************************
    see ScanCall for explanation
*/
{
    switch( request ) {
    case AUX_LOOKUP:
        switch( FindRTLabel( hdl ) ) {
        case RT_SCAN1:
            return( &Scn1 );
        case RT_SCAN2:
            return( &Scn2 );
        case RT_SCAN4:
            return( &Scn4 );
        default:
            break;
        }
        break;
    case CALL_CLASS:
        return( &rt_cclass );
    case CALL_BYTES:
        return( hdl );
    default:
        _Zoiks( ZOIKS_128 );
        break;
    }
    return( NULL );
}
