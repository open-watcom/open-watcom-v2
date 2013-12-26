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
* Description:  Scoreboarding routines specific to the i86.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "score.h"
#include "data.h"
#include "makeins.h"
#include "namelist.h"

extern  name            *NearSegment( void );
extern  hw_reg_set      HighReg( hw_reg_set );
extern  hw_reg_set      LowReg( hw_reg_set );
extern  bool            IsIndexReg( hw_reg_set, type_class_def, bool );


extern  score_reg       **ScoreList;

#define S_MAX   3
#define S_DS    0
#define S_SS    1
#define S_ES    2

#define I_MAX   3
#define I_BX    0
#define I_DI    1
#define I_SI    2

static  name    *PtrRegs[S_MAX][I_MAX];
static  name    *DS;
static  name    *SS;
static  name    *ES;



extern  void    ScInitRegs( score *sc )
/**************************************
    Add some register equality "truths" to the scoreboard "sc"
*/
{
    int     ss;
    int     ds;

    if( _IsntTargetModel( FLOATING_DS | FLOATING_SS ) ) {
        ss = AllocRegName(HW_SS)->r.reg_index;
        ds = AllocRegName(HW_DS)->r.reg_index;
        if( RegsEqual( sc, ss, ds ) == FALSE ) {
            RegInsert( sc, ss, ds );
        }
    }
}


static  name    *NewRegName( hw_reg_set reg )
/********************************************
    Allocate a new "far pointer" register an set its class
*/
{
    name        *reg_name;

    reg_name = AllocRegName( reg );
    reg_name->n.name_class = CP;
    return( reg_name );
}


extern  void    AddRegs( void )
/******************************
    Add some registers to the N_REGISTER list, so that we can do
    scoreboarding on them
*/
{
    hw_reg_set  lo_part;
    name        *reg_name;
    int         i;
    int         j;

    DS = AllocRegName( HW_DS );
    SS = AllocRegName( HW_SS );
/*      89-01-03*/
    ES = AllocRegName( HW_ES );
    for( i = S_MAX; i-- > 0; ) {
        for( j = I_MAX; j-- > 0; ) {
            PtrRegs[i][j] = NULL;
        }
    }
    for( reg_name = Names[N_REGISTER]; reg_name != NULL; reg_name = reg_name->n.next_name ) {
        if( IsIndexReg( reg_name->r.reg, CP, FALSE ) ) {
            if( HW_COvlap( reg_name->r.reg, HW_DS )
             || HW_COvlap( reg_name->r.reg, HW_ES )
             || HW_COvlap( reg_name->r.reg, HW_SS ) ) {
                lo_part = LowReg( reg_name->r.reg );
                if( HW_COvlap( lo_part, HW_BX ) ) {
                    PtrRegs[S_DS][I_BX] = NewRegName( HW_DS_BX );
                    PtrRegs[S_SS][I_BX] = NewRegName( HW_SS_BX );
                    PtrRegs[S_ES][I_BX] = NewRegName( HW_ES_BX );
                } else if( HW_COvlap( lo_part, HW_SI ) ) {
                    PtrRegs[S_DS][I_SI] = NewRegName( HW_DS_SI );
                    PtrRegs[S_SS][I_SI] = NewRegName( HW_SS_SI );
                    PtrRegs[S_ES][I_SI] = NewRegName( HW_ES_SI );
                } else if( HW_COvlap( lo_part, HW_DI ) ) {
                    PtrRegs[S_DS][I_DI] = NewRegName( HW_DS_DI );
                    PtrRegs[S_SS][I_DI] = NewRegName( HW_SS_DI );
                    PtrRegs[S_ES][I_DI] = NewRegName( HW_ES_DI );
                }
            }
        }
    }
/*      89-01-03*/
}


extern  void    ScoreSegments( score *sc )
/*****************************************
    Do special scoreboarding on segment registers.  Given that BX = DI,
    for example, we know that SS:BX = SS:DI, and DS:BX = DS:DI.
*/
{
    score       *ds;
    score       *xs;
    int         i;
    name        *dst;
    name        *src;

    ds = &sc[ DS->r.reg_index ];
    for( xs = ds->next_reg; xs != ds; xs = xs->next_reg ) {
        if( xs->index == SS->r.reg_index ) {
            for( i = I_MAX; i-- > 0; ) {
                dst = PtrRegs[S_SS][i];
                src = PtrRegs[S_DS][i];
                if( dst != NULL && src != NULL ) {
                    if( !RegsEqual( sc, dst->r.reg_index, src->r.reg_index ) ) {
                        RegInsert( sc, dst->r.reg_index, src->r.reg_index );
                    }
                }
            }
        } else if( xs->index == ES->r.reg_index ) {
            for( i = I_MAX; i-- > 0; ) {
                dst = PtrRegs[S_ES][i];
                src = PtrRegs[S_DS][i];
                if( dst != NULL && src != NULL ) {
                    if( !RegsEqual( sc, dst->r.reg_index, src->r.reg_index ) ) {
                        RegInsert( sc, dst->r.reg_index, src->r.reg_index );
                    }
                }
            }
        }
    }
}


extern  bool    ScAddOk( hw_reg_set reg1, hw_reg_set reg2 )
/**********************************************************
    Is it ok to say that "reg1" = "reg2"?  This is not ok for
    unalterable registers since there may be hidden modifications of
    these registers.
*/
{
    if( HW_Ovlap( reg1, CurrProc->state.unalterable ) ) {
        if( !HW_CEqual( reg1, HW_DS ) && !HW_CEqual( reg1, HW_SS ) ) {
            return( FALSE );
        }
    }
    if( HW_Ovlap( reg2, CurrProc->state.unalterable ) ) {
        if( !HW_CEqual( reg2, HW_DS ) && !HW_CEqual( reg2, HW_SS ) ) {
            return( FALSE );
        }
    }
    return( TRUE );
}


extern  bool    ScConvert( instruction *ins )
/********************************************
    Get rid of instructions like CBW if the high part is not used in the
    next instruction.
*/
{
    hw_reg_set  tmp;

    if( ins->u.gen_table->generate == G_SIGNEX ) {
        tmp = HighReg( ins->result->r.reg );
        if( !HW_Ovlap( ins->head.next->head.live.regs, tmp ) ) {
            FreeIns( ins ); /* get rid of the pesky cwd or cbw instruction!*/
            return( TRUE );
        }
    }
    return( FALSE );
}


extern  bool    CanReplace( instruction *ins )
/*********************************************
    Are we allowed to replace any of the operands of "ins" with
    different registers?  For long shifts the answer is no since CX is a
    must for the loop counter.
*/
{
    if( ( ins->head.opcode == OP_LSHIFT
       || ins->head.opcode == OP_RSHIFT )
      && ( ins->type_class == U4
       || ins->type_class == I4 ) ) return( FALSE );
    return( TRUE );
}


extern  bool    ScRealRegister( name *reg )
/******************************************
    Return "TRUE" if "reg" is a real machine register and not some
    monstrosity like AX:DX:BX used for calls.
*/
{
    return( reg->n.name_class != XX );
}
