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
#include "score.h"
#include "model.h"
#include "pattern.h"
#include "procdef.h"
#include "vergen.h"
#include "opcodes.h"

#include "s37score.def"

extern  void            RegInsert(score*,int,int);
extern  bool            RegsEqual(score*,int,int);
extern  name            *AllocRegName(hw_reg_set);
extern  bool            IsRegPair(hw_reg_set);


extern  proc_def        *CurrProc;

extern  bool    MultiIns( instruction *ins ) {
/*********************************************
    Is ins part of a two instruction sequence, like SUB, SBB?
    select wacks it's opernad so we don't want it removed - MJC
*/
    if( ins->head.opcode == OP_SELECT ) return( TRUE );
    return( FALSE );
}


extern  void    ScInitRegs( score *sc ) {
/****************************************
    Add some register equality "truths" to the scoreboard "sc"
*/
    sc = sc;
}


extern  void    AddRegs() {
/**************************
    Add some registers to the N_REGISTER list, so that we can do
    scoreboarding on them
*/

}


extern  void    ScoreSegments( score *sc ) {
/*******************************************
    Do special scoreboarding on segment registers.
*/
    sc = sc;
}


extern  bool    ScAddOk( hw_reg_set reg1, hw_reg_set reg2 ) {
/************************************************************
    Is it ok to say that "reg1" = "reg2".
*/
    reg1=reg1;reg2=reg2;
    return( TRUE );
}


extern  bool    ScConvert( instruction *ins ) {
/**********************************************
    an 8086ism
*/
    ins = ins;
    return( FALSE );
}


extern  bool    CanReplace( instruction *ins ) {
/***********************************************
    an 8086ism
*/
    ins = ins;
    return( TRUE );
}

static hw_reg_set RegNames[17] = {
    HW_D( HW_G0 ),
    HW_D( HW_G1 ),
    HW_D( HW_G2 ),
    HW_D( HW_G3 ),
    HW_D( HW_G4 ),
    HW_D( HW_G5 ),
    HW_D( HW_G6 ),
    HW_D( HW_G7 ),
    HW_D( HW_G8 ),
    HW_D( HW_G9 ),
    HW_D( HW_G10 ),
    HW_D( HW_G11 ),
    HW_D( HW_G12 ),
    HW_D( HW_G13 ),
    HW_D( HW_G14 ),
    HW_D( HW_G15 ),
    HW_D( HW_EMPTY )
};


extern  bool    ScRealRegister( name *reg ) {
/********************************************
    Return "TRUE" if "reg" is a real machine register and not some
    monstrosity like G0:G11:G13 used for calls.
*/
    hw_reg_set  regs;
    reg_num i;
    int count;

    if( reg->n.name_class != XX ) return( TRUE );
    regs  = reg->r.reg;
    HW_CTurnOff( regs, HW_UNUSED );
    i = 0;
    count = 0;
    while( i < 16 ) {
        if( HW_Ovlap( regs, RegNames[i] ) ){
            count++;
        }
        i++;
    }
    return(  count <= 2 );
}
