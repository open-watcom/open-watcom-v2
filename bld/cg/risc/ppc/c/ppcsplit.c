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
#include "vergen.h"
#include "tables.h"
#include "pattern.h"
#include "rtclass.h"
#include "zoiks.h"
#include "model.h"
#include <assert.h>

extern  name            *AllocMemory(pointer,type_length,cg_class,type_class_def);
extern  name            *AllocIndex(name*,name*,type_length,type_class_def);
extern  name            *AllocS32Const(signed_32);
extern  name            *AllocRegName(hw_reg_set);
extern  name            *AllocTemp(type_class_def);
extern  name            *AllocAddrConst(name*,int,constant_class,type_class_def);
extern  name            *ScaleIndex(name*,name*,type_length,type_class_def,type_length,int,i_flags);
extern  name            *STempOffset(name*,type_length,type_class_def,type_length);

extern  void            SuffixIns(instruction*,instruction*);
extern  void            PrefixIns(instruction*,instruction*);
extern  void            ReplIns(instruction*,instruction*);
extern  label_handle    RTLabel(int);
extern  void            ChangeType(instruction*,type_class_def);
extern  void            FreeIns( instruction * );

extern  instruction     *MakeNary(opcode_defs,name*,name*,name*,type_class_def,type_class_def,int);
extern  instruction     *MakeBinary(opcode_defs,name*,name*,name*,type_class_def);
extern  instruction     *MakeMove(name*,name*,type_class_def);
extern  instruction     *MakeConvert(name*,name*,type_class_def,type_class_def);
extern  instruction     *MakeUnary(opcode_defs,name*,name*,type_class_def);
extern  instruction     *MakeCondition(opcode_defs,name*,name*,int,int,type_class_def);
extern  instruction     *NewIns( int );

extern  void            UpdateLive( instruction *, instruction * );
extern  name            *OffsetMem( name *, type_length, type_class_def );

extern  type_length     TypeClassSize[];
extern  type_class_def  Unsigned[];

extern instruction      *rCONSTLOAD( instruction *ins ) {
/*******************************************************/

    unsigned_32         low;
    unsigned_32         high;
    unsigned_32         c;
    name                *high_part;
    name                *temp;
    instruction         *first_ins;
    instruction         *new_ins;
    type_class_def      class;

    assert( ins->operands[ 0 ]->n.class == N_CONSTANT );
    assert( ins->operands[ 0 ]->c.const_type == CONS_ABSOLUTE );

    class = ins->type_class;
    c = ins->operands[ 0 ]->c.int_value;
    high = ( c >> 16 ) & 0xffff;
    low = c & 0xffff;
    high_part = AllocAddrConst( NULL, high, CONS_HIGH_ADDR, class );
    temp = AllocTemp( class );
    first_ins = MakeMove( high_part, temp, class );
    PrefixIns( ins, first_ins );
    new_ins = MakeBinary( OP_OR, temp, AllocS32Const( low ), ins->result, class );
    ReplIns( ins, new_ins );
    UpdateLive( first_ins, new_ins );
    return( first_ins );
}

extern  instruction     *rMOD2DIV( instruction *ins ) {
/*****************************************************/

    instruction         *first_ins;
    instruction         *new_ins;

    first_ins = MakeBinary( OP_DIV, ins->operands[ 0 ], ins->operands[ 1 ], ins->result, ins->type_class );
    PrefixIns( ins, first_ins );
    new_ins = MakeBinary( OP_MUL, ins->operands[ 1 ], ins->result, ins->result, ins->type_class );
    PrefixIns( ins, new_ins );
    new_ins = MakeBinary( OP_SUB, ins->operands[ 0 ], ins->result, ins->result, ins->type_class );
    ReplIns( ins, new_ins );
    UpdateLive( first_ins, new_ins );
    return( first_ins );
}
