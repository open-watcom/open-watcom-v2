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
* Description:  Scoreboarding routines specific to the 386.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "score.h"
#include "data.h"
#include "namelist.h"


extern  void    ScInitRegs( score *sc )
/**************************************
    Add some register equality "truths" to the scoreboard "sc"
*/
{
    int xs;
    int ds;

    ds = AllocRegName(HW_DS)->r.reg_index;
    if( _IsntTargetModel( FLOATING_DS ) ) {
        if( _IsntTargetModel( FLOATING_ES ) ) {
            xs = AllocRegName(HW_ES)->r.reg_index;
            if( RegsEqual( sc, xs, ds ) == FALSE ) {
                RegInsert( sc, xs, ds );
            }
        }
        if( _IsntTargetModel( FLOATING_SS ) ) {
            xs = AllocRegName(HW_SS)->r.reg_index;
            if( RegsEqual( sc, xs, ds ) == FALSE ) {
                RegInsert( sc, xs, ds );
            }
        }
    }
}


extern  void    AddRegs( void )
/******************************
    Add some registers to the N_REGISTER list, so that we can do
    scoreboarding on them
*/
{
    AllocRegName( HW_DS );
    AllocRegName( HW_ES );
    AllocRegName( HW_SS );
}


extern  void    ScoreSegments( score *sc )
/*****************************************
    Do special scoreboarding on segment registers.
*/
{
    sc = sc;
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
        if( !HW_Equal( reg2, HW_DS ) && !HW_Equal( reg2, HW_SS ) ) {
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
    ins = ins;
    return( FALSE );
}


extern  bool    CanReplace( instruction *ins )
/*********************************************
    an 8086ism
*/
{
    ins = ins;
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
