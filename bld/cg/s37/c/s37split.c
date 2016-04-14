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
#include "typedef.h"
#include "opcodes.h"
#include "pattern.h"
#include "regset.h"
#include "model.h"
#include "vergen.h"
#include "zoiks.h"
#include "conflict.h"
#include "cfloat.h"
#include "makeins.h"
#include "s37split.def"

extern  instruction     *MoveConst(unsigned_32 ,name *,type_class_def );
extern  name            *AllocIntConst(int);
extern  name            *AllocS32Const(signed_32);
extern  name            *AllocConst(pointer);
extern  cfloat          *CFCnvU32F(unsigned_32);
extern  name            *AllocTemp(type_class_def);
extern  name            *SAllocTemp(type_class_def,type_length);
extern  name            *TempOffset(name*,type_length,type_class_def);
extern  void            PrefixIns(instruction*,instruction*);
extern  void            ReplIns(instruction*,instruction*);
extern  void            SuffixIns(instruction*,instruction*);
extern  name            *AllocAddrConst(name*,int,constant_class,type_class_def);
extern  void            ChangeType(instruction*,type_class_def);
extern  name            *ScaleIndex(name*,name*,type_length,type_class_def,type_length,int,i_flags);
extern  name            *AllocRegName(hw_reg_set);
extern  conflict_node   *GiveRegister(conflict_node*,bool);
extern  conflict_node   *NameConflict(instruction*,name*);
extern  hw_reg_set      LowReg(hw_reg_set);
extern  void            MarkPossible(instruction*,name*,reg_set_index);
extern  void            UpdateLive(instruction*,instruction*);
extern  name            *STempOffset(name*,type_length,type_class_def,type_length);
extern  memory_name     *SAllocMemory(pointer,type_length,cg_class,type_class_def,type_length);
extern  void            DoNothing(instruction*);
extern  name            *IndexToTemp(instruction*,name*);
extern  conflict_node   *IndexSplit(instruction*,name*);

extern  instruction     *rMOVRESREG(instruction*);
extern  instruction     *rLOADOP2(instruction*);
extern  instruction     *rSWAPCMP(instruction*);
extern  instruction     *rSWAPOPS(instruction*);
extern  instruction     *rUSEREGISTER(instruction*);
extern  instruction     *rDOCVT(instruction*);
extern  instruction     *rOP1REG(instruction*);
extern  instruction     *rOP1RESTEMP(instruction*);
extern  instruction     *rOP1MEM(instruction*);
extern  instruction     *rOP2MEM(instruction*);
extern  instruction     *rFORCERESMEM(instruction*);
extern  instruction     *rMOVEINDEX(instruction*);
extern  instruction     *rMAKEADD(instruction*);
extern  instruction     *rMAKEMOVE(instruction*);
extern  instruction     *rMAKENEG(instruction*);
extern  instruction     *rMAKESUB(instruction*);
extern  instruction     *rMOVOP2(instruction*);

extern    hw_reg_set    *RegSets[];
extern    op_regs       RegList[];
extern    byte          OptForSize;
extern    opcode_entry  MVCL[];
extern    opcode_entry  MVC[];
extern    opcode_entry  NC[];
extern    opcode_entry  OC[];
extern    opcode_entry  XC[];
extern    opcode_entry  CLC[];
extern    opcode_entry  Mul4G[];
extern    opcode_entry  Mul4I2G[];
extern    opcode_entry  Add4I2G[];
extern    opcode_entry  Sub4I2G[];
extern    opcode_entry  Mod4G[];
extern    opcode_entry  Div4G[];
extern    opcode_entry  S4to8G[];
extern    opcode_entry  S1to4G[];
extern    opcode_entry  S2to4G[];
extern    opcode_entry  Z1to4G[];
extern    opcode_entry  Z2to4G[];
extern    opcode_entry  CNormF[];
extern    type_length   TypeClassSize[];


extern instruction *(* ReduceTab[])() = {
/***************************************/

    rMOVRESREG,
    rLOADOP2,
    rSWAPCMP,
    rSWAPOPS,
    rUSEREGISTER,
    rPREPMUL,
    rPREPDIV,
    rDOCVT,
    rPREPCONV8,
    rCLRMOV8,
    rCLRMOV,
    rMOVEXT,
    rOP1REG,
    rPREPS1TO4,
    rPREPS2TO4,
    rPREPZ1TO4,
    rPREPZ2TO4,
    rMOVEADDR,
    rBIN2INT,
    rUN2INT,
    rSIDE2INT,
    rNEGDEC,
    rNOT2XOR,
    rOP1RESTEMP,
    rPREPMOD,
    rOP1MEM,
    rOP2MEM,
    rFORCERESMEM,
    rMVCL,
    rMOVEINDEX,
    rMAKEADD,
    rMAKEMOVE,
    rMAKENEG,
    rMAKESUB,
    rMOVOP2,
    rMAKEAND,
    rTEST1,
    rSTOD,
    rDTOS,
    rPREPI4I2,
    rPREPITOD,
    rPREPUTOD,
    rMEMDOWN,
    rMVC,
    rXC,
    rCLC,
    NULL
};


extern  name  *HighPart( name *tosplit, type_class_def class ) {
/*************************************************************/

    class = class; tosplit = tosplit;
    return( NULL );
}


extern  name  *LowPart( name *tosplit, type_class_def class ) {
/*************************************************************/

    class = class; tosplit = tosplit;
    return( NULL );
}


extern void CheckCC( instruction *ins1, instruction *ins ) {
/*********************************************************/

    ins1 = ins1; ins = ins;
}


extern  bool    UnChangeable( instruction *ins ) {
/************************************************/

    return( ins->table == MVCL );
}


static instruction *rPREPMUL( instruction *ins ) {
/***********************************************/

    instruction         *ins1;
    instruction         *ins2;
    name                *name1;
    name                *low;

    name1 = SAllocTemp( XX, 8 );
    low =  TempOffset( name1, WORD_SIZE, WD );
    ins1 = MakeMove( ins->operands[0], low, WD );
    ins->operands[0] = low;
    PrefixIns( ins, ins1 );
    ins2 = MakeMove( low, ins->result, WD );
    ins->result = name1;
    SuffixIns( ins, ins2 );
    ins->table = Mul4G;
    ins->u.gen_table = NULL;
    _SetTrue( NameConflict( ins, name1 ), NEVER_TOO_GREEDY );
    return( ins1 );
}

static instruction *rPREPI4I2( instruction *ins ) {
/**********************************************
    If you I4 op I2 is handled then send to a I4I2 table
*/
    if( ins->head.opcode == OP_MUL ){
        ins->table = Mul4I2G;
    }else if( ins->head.opcode == OP_ADD ){
        ins->table = Add4I2G;
    }else if( ins->head.opcode == OP_SUB ){
        ins->table = Sub4I2G;
    }else{
        Zoiks( ZOIKS_019 );
        ins->table = NULL;
    }
    ins->u.gen_table = NULL;
    return( ins );
}


static instruction *PrepModDiv( instruction *ins, int off, opcode_entry *tbl ) {
/******************************************************************************/

    instruction         *ins1;
    instruction         *ins2;
    name                *name1;
    name                *low;

    name1 = SAllocTemp( XX, 8 );
    ins1 = MakeConvert( ins->operands[0], name1, XX, ins->type_class );
    ins->operands[0] = name1;
    PrefixIns( ins, ins1 );
    low =  TempOffset( name1, off, WD );
    ins2 = MakeMove( low, ins->result, WD );
    ins->result = name1;
    SuffixIns( ins, ins2 );
    ins->table = tbl;
    _SetTrue( NameConflict( ins, name1 ), NEVER_TOO_GREEDY );
    return( ins1 );
}


static instruction *rPREPDIV( instruction *ins ) {
/***********************************************/

    return( PrepModDiv( ins, WORD_SIZE, Div4G ) );
}

static instruction *rPREPMOD( instruction *ins ) {
/***********************************************/

    return( PrepModDiv( ins, 0, Mod4G ) );
}

static instruction *rPREPCONV8( instruction *ins ) {
/**************************************************/
    instruction         *ins1;
    name                *high;

    high =  TempOffset( ins->result, 0, WD );
    ins1 = MakeMove( ins->operands[0], high, WD );
    ins->operands[0] = high;
    PrefixIns( ins, ins1 );
    ins->table = S4to8G; /* set to gen table */
    return( ins1 );
}


static instruction *rCLRMOV8( instruction *ins ) {
/**************************************************/

/* change a CONV to a clear high word move U4 to low */

    instruction         *ins1;
    instruction         *ins2;
    name                *high;
    name                *low;

    high =  TempOffset( ins->result, 0, WD );
    ins1 = MakeMove( AllocIntConst( 0 ), high, WD );
    PrefixIns( ins, ins1 );
    low =  TempOffset( ins->result, WORD_SIZE, WD );
    ins2 = MakeMove( ins->operands[0], low, WD );
    ReplIns( ins, ins2 );
    return( ins1 );
}


static instruction *rCLRMOV( instruction *ins ) {
/**************************************************/

/*  Assume unsigned CONV clear result then move value in */

    instruction         *ins1;
    instruction         *ins2;

    ins1 = MakeMove( AllocIntConst( 0 ), ins->result, ins->type_class );
    PrefixIns( ins, ins1 );
    ins2 = MakeMove( ins->operands[0], ins->result, ins->base_type_class );
    ReplIns( ins, ins2 );
    return( ins1 );
}


static instruction *rMOVEXT( instruction *ins ) {
/**************************************************/

/*  Assume CONV move then extend in */

    instruction         *ins1;
    instruction         *ins2;
    int                 shift;

    ins1 = MakeMove( ins->operands[0], ins->result, ins->base_type_class );
    PrefixIns( ins, ins1 );
    shift = (ins->result->n.size - ins->operands[0]->n.size)*8;
    /* mjc a bit sys dependent */
    ins2 = MakeBinary( OP_RSHIFT, ins->result, AllocIntConst( shift ),
                       ins->result, ins->type_class );
    ReplIns( ins, ins2 );
    return( ins1 );
}



static instruction *PrepCnvUp( instruction *ins, opcode_entry *table ) {
/**********************************************************************/

    name        *temp;
    instruction *new_ins;

    temp = AllocTemp( WD );
    new_ins = MakeMove( temp, ins->result, WD );
    ins->result = temp;
    SuffixIns( ins, new_ins );
    ins->table = table;
    ins->u.gen_table = NULL;
    _SetTrue( NameConflict( ins, temp ), NEVER_TOO_GREEDY );
    return( ins );
}


static instruction *rPREPS1TO4( instruction *ins ) {
/*************************************************/

    return( PrepCnvUp( ins, S1to4G ) );
}


static instruction *rPREPS2TO4( instruction *ins ) {
/*************************************************/

    return( PrepCnvUp( ins, S2to4G ) );
}


static instruction *rPREPZ1TO4( instruction *ins ) {
/*************************************************/

    return( PrepCnvUp( ins, Z1to4G ) );
}


static instruction *rPREPZ2TO4( instruction *ins ) {
/*************************************************/
    #if 0
    name        *temp;
    instruction *new_ins;

    temp = ins->operands[0];
    if( temp->n.class == N_TEMP ){
        ins =  rOP1MEM(ins);
    }
    if( temp->n.class == N_REGISTER ){
        temp = AllocTemp( U2 );
        temp->v.usage |= USE_MEMORY | NEEDS_MEMORY;
         new_ins = MakeMove( ins->operands[0], temp, U2 );
        ins->operands[ 0 ] = temp;
        PrefixIns( ins, new_ins );
        temp->v.usage |= USE_MEMORY | NEEDS_MEMORY;
        new_ins = ins;
    }
    #endif
    ins = PrepCnvUp( ins, Z2to4G );
    return( ins );
}


extern name  *FArea = NULL;
static  name *InitItoF( void ) {
/************************************************
    Add the ItoF work area used by converion routines
*/
    extern block *HeadBlock;
    instruction *start;
    instruction *ins;
    name        *low;
    name        *dwork;

    start = HeadBlock->ins.hd.next;
    dwork = SAllocTemp( FD, 8 );
    dwork->v.usage |= USE_MEMORY | NEEDS_MEMORY;
    low =  TempOffset( dwork, 0, WD );
    ins = MoveConst(  0x4E000000, low, WD );
    PrefixIns( start, ins );
    return( dwork );
}

static name *ItoFArea( void ){
    if( FArea == NULL ){
        FArea = InitItoF();
    }
    return( FArea );
}

static instruction *rPREPITOD( instruction *ins ) {
/*************************************************
    CONV    FD I4  op1 -> result =>
    XOR     op1 0X80000000 ->temp
    MOV     0x4E000000 -> high part of FD in mem
    MOV     temp -> low
    MOV     FD  -> temp FD
    SUB     TEMP - (FD)0X80000000 ->result
*/
    instruction         *ins1;
    instruction         *ins2;
    name                *dwork;
    name                *ipart;
    name                *temp;
    name                *theconst;

    temp = AllocTemp( WD );
    theconst =  AllocConst( CFCnvU32F(0x80000000  ) );
    ins1 = MakeBinary( OP_XOR, ins->operands[0],
                       theconst,
                       temp, WD );
    ins->operands[0] = temp;
    PrefixIns( ins, ins1 );
 #if 0    /* if we haven't pre-allocated a work area */
    dwork = SAllocTemp( FD, 8 );
    dwork->v.usage |= USE_MEMORY | NEEDS_MEMORY;
    ipart =  TempOffset( dwork, 0, WD );
    ins2 = MoveConst(  0x4E000000, ipart, WD );
    PrefixIns( ins, ins2 );
#else
    dwork = ItoFArea();
#endif
    ipart =  TempOffset( dwork, WORD_SIZE, WD );
    ins2 = MakeMove( temp, ipart, WD );
    ins->operands[0] = ipart;
    PrefixIns( ins, ins2 );
    temp = AllocTemp( FD );
    ins2 = MakeMove( dwork, temp, FD );
    ins->operands[0] = temp;
    PrefixIns( ins, ins2 );
    ins2  = MakeBinary( OP_SUB, temp, theconst, temp, FD );
    PrefixIns( ins, ins2 );
    if( ins->type_class == FS ){
        temp = TempOffset( temp, 0, FS );
    }
    ins2 = MakeMove( temp, ins->result, ins->type_class );
    ReplIns( ins, ins2 );
    _SetTrue( NameConflict( ins, temp ), NEVER_TOO_GREEDY );
    return( ins1 );
}

static instruction *rPREPUTOD( instruction *ins ) {
/*************************************************
    CONV    FD U4  op1 -> result  =>
    MOV     0x4E000000 -> high part of FD in mem
    MOV     temp -> low
    MOV     FD  -> temp FD
    ADD     TEMP + (FD)0 ->result  normalize
*/
    instruction         *ins1;
    instruction         *ins2;
    name                *dwork;
    name                *ipart;
    name                *temp;
    name                *result;

#if 0
    dwork = SAllocTemp( FD, 8 );
    dwork->v.usage |= USE_MEMORY | NEEDS_MEMORY;
    ipart =  TempOffset( dwork, 0, WD );
    ins1 = MoveConst(  0x4E000000, ipart, WD );
    PrefixIns( ins, ins2 );
#else
    dwork = ItoFArea();
#endif
    ipart =  TempOffset( dwork, WORD_SIZE, WD );
    ins1 = MakeMove( ins->operands[0], ipart, WD );
    ins->operands[0] = ipart;
    PrefixIns( ins, ins1 );
    temp = AllocTemp( FD );
    ins->operands[0] = dwork;
    result = ins->result;
    ins->result = temp;
    if( ins->type_class == FS ){
        temp = TempOffset( temp, 0, FS );
    }
    ins2 = MakeMove( temp, result, ins->type_class );
    SuffixIns( ins, ins2 );
    ins->table = CNormF;
    _SetTrue( NameConflict( ins, temp ), NEVER_TOO_GREEDY );
    return( ins1 );
}
static  instruction     *rMOVEADDR( instruction *ins ) {
/********************************************************/
    ins->head.opcode = OP_MOV;
    ins->operands[0] = AllocAddrConst( ins->operands[0], 0, CONS_ADDRESS, WD );
    ins->table = NULL;
    ins->u.gen_table = NULL;
    return( ins );
}

static  instruction     *rBIN2INT( instruction *ins ) {
/*****************************************************/

    instruction *op0_ins;
    instruction *op1_ins;
    instruction *res_ins;
    name        *t0;
    name        *t1;

    t0 = AllocTemp( I4 );
    if( ins->operands[ 0 ]->n.class == N_CONSTANT ) {
        op0_ins = NULL;
    } else {
        op0_ins = MakeConvert( ins->operands[ 0 ], t0, I4, ins->type_class );
        ins->operands[ 0 ] = t0;
        PrefixIns( ins, op0_ins );
    }

    if( ins->operands[ 1 ]->n.class == N_CONSTANT ) {
        op1_ins = NULL;
    } else {
        t1 = AllocTemp( I4 );
        op1_ins = MakeConvert( ins->operands[ 1 ], t1, I4, ins->type_class );
        ins->operands[ 1 ] = t1;
        PrefixIns( ins, op1_ins );
    }

    res_ins = MakeConvert( t0, ins->result, ins->type_class, I4 );
    ins->result = t0;
    SuffixIns( ins, res_ins );

    ChangeType( ins, I4 );

    if( op0_ins != NULL ) return( op0_ins );
    if( op1_ins != NULL ) return( op1_ins );
    return( ins );
}

static  instruction     *rUN2INT( instruction *ins ) {
/*****************************************************/

    instruction *op_ins;
    instruction *res_ins;
    name        *t0;

    t0 = AllocTemp( I4 );
    if( ins->operands[ 0 ]->n.class == N_CONSTANT ) {
        op_ins = NULL;
    } else {
        op_ins = MakeConvert( ins->operands[ 0 ], t0, I4, ins->type_class );
        ins->operands[ 0 ] = t0;
        PrefixIns( ins, op_ins );
    }

    res_ins = MakeConvert( t0, ins->result, ins->type_class, I4 );
    ins->result = t0;
    SuffixIns( ins, res_ins );

    ChangeType( ins, I4 );

    if( op_ins != NULL ) return( op_ins );
    return( ins );
}


static  instruction     *rSIDE2INT( instruction *ins ) {
/*****************************************************/

    instruction *op0_ins;
    instruction *op1_ins;
    name        *t0;
    name        *t1;

    t0 = AllocTemp( I4 );
    if( ins->operands[ 0 ]->n.class == N_CONSTANT ) {
        op0_ins = NULL;
    } else {
        op0_ins = MakeConvert( ins->operands[ 0 ], t0, I4, ins->type_class );
        ins->operands[ 0 ] = t0;
        PrefixIns( ins, op0_ins );
    }

    if( ins->operands[ 1 ]->n.class == N_CONSTANT ) {
        op1_ins = NULL;
    } else {
        t1 = AllocTemp( I4 );
        op1_ins = MakeConvert( ins->operands[ 1 ], t1, I4, ins->type_class );
        ins->operands[ 1 ] = t1;
        PrefixIns( ins, op1_ins );
    }

    ChangeType( ins, I4 );

    if( op0_ins != NULL ) return( op0_ins );
    if( op1_ins != NULL ) return( op1_ins );
    return( ins );
}


static  instruction     *rNEGDEC( instruction *ins ) {
/*****************************************************/

    instruction *neg_ins;
    name        *tmp;

    tmp = AllocTemp( I4 );
    neg_ins = MakeUnary( OP_NEGATE, ins->operands[ 0 ], tmp, I4 );

    ins->operands[ 0 ] = tmp;
    ins->operands[ 1 ] = AllocIntConst( 1 );
    ins->head.opcode = OP_SUB;
    ins->num_operands = 2;
    PrefixIns( ins, neg_ins );
    ins->table = NULL;

    return( neg_ins );
}


static  instruction     *rNOT2XOR( instruction *ins ) {
/*****************************************************/

    instruction *new_ins;

    new_ins = MakeBinary( OP_XOR, ins->operands[ 0 ],
                          AllocIntConst( -1 ), ins->result, U4 );
    ReplIns( ins, new_ins );
    return( new_ins );
}


static  instruction     *rMVCL( instruction *ins ) {
/*****************************************************/

    instruction *first_ins;
    instruction *new_ins;
    name        *name1;
    name        *name1_ptr;
    name        *name1_len;
    name        *name1_base;
    name        *name2;
    name        *name2_len;
    name        *name2_ptr;
    name        *name2_base;
    name        *op1;
    name        *op2;
    name        *res;
    type_length size;
    hw_reg_set  tmp;

    name1 = SAllocTemp( XX, 8 );
    name1_len = TempOffset( name1, WORD_SIZE, WD );
    name1_ptr = TempOffset( name1, 0, WD );
    name2 = SAllocTemp( XX, 8 );
    name2_len = TempOffset( name2, WORD_SIZE, WD );
    name2_ptr = TempOffset( name2, 0, WD );

    op1 = ins->operands[ 0 ];
    if( op1->n.class == N_MEMORY ) {
        name1_base = op1;
    } else if( op1->n.class == N_INDEXED ) {
        name1_base = op1->i.base;
    }
    first_ins = MakeUnary( OP_CAREFUL_LA, op1, name1_ptr, WD );
    res = ins->result;
    if( res->n.class == N_MEMORY ) {
        name2_base = res;
    } else if( res->n.class == N_INDEXED ) {
        name2_base = res->i.base;
    }
    size = op1->n.size;
    PrefixIns( ins, first_ins );
    new_ins = MakeUnary( OP_CAREFUL_LA, res, name2_ptr, WD );
    PrefixIns( ins, new_ins );
    new_ins = MakeMove( AllocS32Const( size ), name1_len, WD );
    PrefixIns( ins, new_ins );
    new_ins = MakeMove( name1_len, name2_len, WD );
    PrefixIns( ins, new_ins );
    ins->operands[ 0 ] = name1;
    ins->num_operands = 2;
    ins->operands[ 1 ] = name2;
    UpdateLive( first_ins, ins );
    MarkPossible( ins, name1, RL_PAIR );
    GiveRegister( NameConflict( ins, name1 ), true );
    MarkPossible( ins, name2, RL_PAIR );
    GiveRegister( NameConflict( ins, name2 ), true );
    op1 = ins->operands[ 0 ];
    op2 = ins->operands[ 1 ];
    tmp = op1->r.reg;
    HW_TurnOn( tmp, op2->r.reg );
    ins->zap = AllocRegName( tmp );
    ins->operands[ 1 ] = ins->zap;              /* for live info */
    ins->operands[ 0 ] = ScaleIndex( AllocRegName(LowReg(op1->r.reg)),
                                     name1_base, 0, XX, size, 0, X_FAKE_BASE );
    ins->result = ScaleIndex( AllocRegName(LowReg(op2->r.reg)),
                              name2_base, 0, XX, size, 0, X_FAKE_BASE );
    ins->table = MVCL;
    UpdateLive( first_ins, ins );
    return( first_ins );
}


static  void  DotheSplits( instruction *ins ) {
/*************************************************
*/
    int         i;
    name        *temp;

    i = ins->num_operands;
    while( --i >= 0 ) {
        temp = ins->operands[i];
        if( temp->n.class == N_INDEXED ) {
            if( temp->i.index->n.class != N_REGISTER ){
                _SetTrue( IndexSplit( ins, temp ), NEVER_TOO_GREEDY );
            }
        }
    }
    temp = ins->result;
    if( temp != NULL ){
        if( temp->n.class == N_INDEXED ) {
            if( temp->i.index->n.class != N_REGISTER ){
                _SetTrue( IndexSplit( ins, temp ), NEVER_TOO_GREEDY );
            }
        }
    }
}

static  instruction     *rMVC( instruction *ins ) {
/*************************************************
    force the index vars into regs for instructions that need two indexs
    op1 & result
*/

    DotheSplits( ins );
    ins->table = MVC;
    ins->u.gen_table = NULL;
    return( ins );
}

static  instruction     *rXC( instruction *ins ) {
/*************************************************
    force the index vars into regs for instructions that need two indexs
    op2 & result
*/

//  if( ins->operands[1]->n.class == N_INDEXED ) {
//      _SetTrue( IndexSplit( ins, ins->operands[0] ), NEVER_TOO_GREEDY );
//  }
    DotheSplits( ins );
    if( ins->head.opcode == OP_AND ){
        ins->table = NC;
    }else if( ins->head.opcode == OP_OR ){
        ins->table = OC;
    }else if( ins->head.opcode == OP_XOR ){
        ins->table = XC;
    }else{
        Zoiks( ZOIKS_019 );
        ins->table = NULL;
    }
    ins->u.gen_table = NULL;
    return( ins );
}

static  instruction     *rCLC( instruction *ins ) {
/*************************************************
    force the index vars into regs for instructions that need two indexs
    op1 & op2
*/

    DotheSplits( ins );
    ins->table = CLC;
    ins->u.gen_table = NULL;
    return( ins );
}

static  instruction     *rMAKEAND( instruction *ins ) {
/*****************************************************/

    instruction *first_ins;
    instruction *new_ins;
    name        *temp;

    /* in here ... operands[ 1 ] is always a constant! */
    temp = AllocTemp( WD );
    first_ins = MakeConvert( ins->operands[ 0 ], temp, WD, ins->type_class );
    ins->operands[ 0 ] = temp;
    PrefixIns( ins, first_ins );
    new_ins = MakeBinary( OP_AND, temp, ins->operands[ 1 ], temp, WD );
    new_ins->ins_flags |= INS_CC_USED;
    PrefixIns( ins, new_ins );
    DoNothing( ins );
    return( first_ins );
}

static  name   *MemCast( name *op,  type_class_def  tipe ){
/*********************************************************
    Cast in memory op as tipe
*/
    int         offset;
    int         tsize;

    tsize = TypeClassSize[ tipe ];
    offset = op->n.size-tsize;
    if( offset == 0 ){
        return( op );
    }
    switch( op->n.class ) {
    case N_TEMP:
        op = TempOffset( op, offset, tipe );
        break;
    case N_MEMORY:
        op = SAllocMemory( op->v.symbol, offset, op->m.memory_type,
         tipe, tsize );
        break;
    case N_INDEXED:
        op = ScaleIndex( op->i.index, op->i.base, op->i.constant+offset,
                          tipe, tsize, op->i.scale, op->i.index_flags );
        break;
    }
    return( op );
}

static  instruction     *rMEMDOWN( instruction *ins ){
/*****************************************************
    Change a convert of mem to mem into a move
*/
    instruction *new_ins;
    name        *cast;
    type_class_def  tipe;

    tipe = ins->type_class;
    cast = MemCast( ins->operands[0], tipe  );
    new_ins = MakeMove( cast, ins->result, tipe  );
    ReplIns(ins, new_ins );
    return( new_ins );
}

static  instruction     *rTEST1( instruction *ins ) {
/**************************************************/

    unsigned_32 cons;
    int         offset;
    name        *op1;

    cons = ins->operands[ 1 ]->c.int_value;
    offset = 0;
    op1 = ins->operands[ 0 ];
    offset = op1->n.size-1;
    while( ( cons & 0xFF ) == 0 ) {
        cons >>= 8;
        --offset;
    }
    ins->operands[ 1 ] = AllocIntConst( cons );
    switch( op1->n.class ) {
    case N_TEMP:
        op1 = TempOffset( op1, offset, U1 );
        break;
    case N_MEMORY:
        op1 = SAllocMemory( op1->v.symbol, offset, op1->m.memory_type, U1, 1 );
        break;
    case N_INDEXED:
        op1 = ScaleIndex( op1->i.index, op1->i.base, op1->i.constant+offset,
                          U1, 1, op1->i.scale, op1->i.index_flags );
        break;
    }
    ins->operands[ 0 ] = op1;
    ins->type_class = U1;
    ins->table = NULL;
    return( ins );
}

static  instruction     *rSTOD( instruction *ins ) {
/**************************************************/

    name        *temp;
    instruction *new_ins;
    instruction *first_ins;

    temp = AllocTemp( FD );
    new_ins = MakeMove( ins->operands[ 0 ], TempOffset( temp, 0, FS ), FS );
    ins->operands[ 0 ] = temp;
    PrefixIns( ins, new_ins );
    first_ins = MakeMove( AllocIntConst( 0 ), temp, FD );
    PrefixIns( new_ins, first_ins );
    ins->head.opcode = OP_MOV;
    ins->table = NULL;
    return( first_ins );
}


static  instruction     *rDTOS( instruction *ins ) {
/**************************************************/

    name        *temp;
    instruction *new_ins;

    temp = AllocTemp( FD );
    new_ins = MakeMove( ins->operands[ 0 ], temp, FD );
    ins->operands[ 0 ] = TempOffset( temp, 0, FS );
    PrefixIns( ins, new_ins );
    ins->head.opcode = OP_MOV;
    ins->table = NULL;
    return( new_ins );
}
