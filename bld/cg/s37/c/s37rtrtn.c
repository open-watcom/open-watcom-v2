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
#include "regset.h"
#include "rttable.h"
#include "rtclass.h"
#include "procdef.h"
#include "zoiks.h"
#include "seldef.h"

#include "s37rtrtn.def"

extern  name            *AllocRegName(hw_reg_set);
extern  name            *AllocMemory(pointer,type_length,cg_class,type_class_def);
extern  name            *AllocIntConst(int);
extern  instruction     *NewIns(int);
extern  name            *AddrConst(name*,segment_id,constant_class);
extern  name            *AllocIndex(name*,name*,type_length,type_class_def);
extern  instruction     *MakeMove(name*,name*,type_class_def);
extern  instruction     *MakeUnary(opcode_defs,name*,name*,type_class_def);
extern  instruction     *MakeConvert(name*,name*,type_class_def,type_class_def);
extern  label_handle    RTLabel(int);
extern  void            AddIns(instruction*);
extern  seg_id          AskBackSeg(void);
extern  name            *AllocTemp(type_class_def);
extern  hw_reg_set      RAReg();
extern  hw_reg_set      LNReg();

extern  proc_def        *CurrProc;

rtn_info RTInfo[RT_NOP-BEG_RTNS+1] = {
/* name    op            class   left            right           result*/
//"@@U2F",  OP_CONVERT,   U4,     RL_G0,  RL_,            RL_Y0,
//"@@I2F",  OP_CONVERT,   I4,     RL_G0,  RL_,            RL_Y0,
//"@@U2F",  OP_CONVERT,   U4,     RL_G0,  RL_,            RL_D0,
//"@@I2F",  OP_CONVERT,   I4,     RL_G0,  RL_,            RL_D0,
"@@S2I",  OP_CONVERT,   FS,     RL_Y0,  RL_,            RL_G0,
"@@RS2I", OP_ROUND,     FS,     RL_Y0,  RL_,            RL_G0,
"@@S2I",  OP_CONVERT,   FS,     RL_Y0,  RL_,            RL_G0,
"@@RS2I", OP_ROUND,     FS,     RL_Y0,  RL_,            RL_G0,
"@@D2I",  OP_CONVERT,   FD,     RL_D0,  RL_,            RL_G0,
"@@RD2I", OP_ROUND,     FD,     RL_D0,  RL_,            RL_G0,
"@@D2I",  OP_CONVERT,   FD,     RL_D0,  RL_,            RL_G0,
"@@RD2I", OP_ROUND,     FD,     RL_D0,  RL_,            RL_G0,
"@@STK",  OP_NOP,        0,     RL_,    RL_,            RL_,
"@@SCN1", OP_SELECT,     0,     RL_,    RL_,            RL_,
"@@SCN2", OP_SELECT,     0,     RL_,    RL_,            RL_,
"@@SCN4", OP_SELECT,     0,     RL_,    RL_,            RL_,
"@@OSPROL",OP_NOP,       0,     RL_,    RL_,            RL_,
"@@OSCALL",OP_NOP,       0,     RL_,    RL_,            RL_,
"@@OSEPIL",OP_NOP,       0,     RL_,    RL_,            RL_,
"@@OSSTK", OP_NOP,       0,     RL_,    RL_,            RL_,
"@@NOP",  OP_NOP,        0,     RL_,    RL_,            RL_ };

extern  char    *AskRTName( int rtindex ) {
/*****************************************/

    return( RTInfo[  rtindex  ].nam );
}


extern  name    *ScanCall( tbl_control *table, name *value,
                           type_class_def tipe ) {
/*************************************************
    generates a fake call to a rutime routine that looks up "value" in a table
    and jumps to the appropriate case, using either a pointer or index
    returned by the "routine". The "routine" will be generated inline later.
    See BEAuxInfo for the code sequences generated. That will explain
    how the jump destination is determined as well.
*/

    instruction *new_ins;
    name        *reg_name;
    name        *result;
    name        *label;
    hw_reg_set  tmp;

    switch( tipe ) {
    case U1:
        RoutineNum = RT_SCAN1 - BEG_RTNS;
        break;
    case U2:
        RoutineNum = RT_SCAN2 - BEG_RTNS;
        break;
    case U4:
        RoutineNum = RT_SCAN4 - BEG_RTNS;
        break;
    }

    reg_name = AllocRegName( HW_G0 );
    if( value->n.class == N_REGISTER || value->n.class == N_CONSTANT ) {
        new_ins = MakeMove( value, reg_name, WD );
    } else {
        new_ins = MakeConvert( value, reg_name, WD, value->n.name_class );
    }
    AddIns( new_ins );

    reg_name = AllocRegName( HW_G1 );
    new_ins = MakeMove( AllocIntConst( table->size ), reg_name, U4 );
    AddIns( new_ins );

    reg_name = AllocRegName( HW_G2 );
    label = AllocMemory( table, 0, CG_VTB, U4 );
    new_ins = MakeUnary( OP_LA, label, reg_name, WD );
    AddIns( new_ins );

    new_ins = NewIns( 3 );
    new_ins->head.opcode = OP_CALL;
    new_ins->type_class = U2;
    new_ins->operands[ CALL_OP_USED ] = AllocRegName( HW_G0_G1_G2 );
    new_ins->operands[ CALL_OP_USED2 ] = new_ins->operands[ CALL_OP_USED ];
    new_ins->operands[ CALL_OP_ADDR ] = AllocMemory( RTLabel(RoutineNum),
                                             0, CG_LBL, U4 );
    new_ins->result = NULL;
    new_ins->num_operands = 2;
    AddIns( new_ins );

    result = AllocMemory( table, 0, CG_TBL, U4 ); /* so table gets freed!*/
    tmp = RAReg();
    HW_TurnOn( tmp, LNReg() );
    HW_CTurnOn( tmp, HW_G0_G1_G2 );
    new_ins->zap = AllocRegName( tmp );
    new_ins->result = AllocRegName( HW_G1 );
    result = AllocIndex( new_ins->result, result, 0, U4 );
    return( result );
}


extern  void            AskRTRALN( int idx, char * ra, char * ln )
{
    switch( idx ) {
    case RT_OSCALL-BEG_RTNS:
    case RT_OSPROL-BEG_RTNS:
    case RT_OSEPIL-BEG_RTNS:
    case RT_OSSTKCHK-BEG_RTNS:
        *ra = 14;
        *ln = 15;
        break;
    default:
        *ra = 13;
        *ln = 11;
        break;
    }
}

extern  bool            RTLeaveOp2( instruction *ins ) {
/******************************************************/
    name        *op2;

    op2 = ins->operands[ 1 ];
    if( op2->n.name_class == U2 || op2->n.name_class == I2 ){
        if( ins->head.opcode == OP_MUL
         || ins->head.opcode == OP_SUB
         || ins->head.opcode == OP_ADD ){
            return( TRUE );
        }
    }

    return( FALSE );
}
