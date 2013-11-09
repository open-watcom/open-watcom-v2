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
* Description:  PowerPC parameter passing processing.
*
****************************************************************************/


#include "cgstd.h"
#include "cgdefs.h"
#include "coderep.h"
#include "procdef.h"
#include "cgdefs.h"
#include "typedef.h"
#include "types.h"
#include "regset.h"
#include "zoiks.h"
#include "ppcparm.h"
#include "feprotos.h"


extern  hw_reg_set      InLineParm(hw_reg_set,hw_reg_set);
extern  hw_reg_set      *ParmChoices(type_class_def);
extern  hw_reg_set      ParmRegConflicts(hw_reg_set);
extern  type_class_def  TypeClass( type_def * );

static  hw_reg_set      floatRegs[] = {
    HW_D_1( HW_F1 ),
    HW_D_1( HW_F2 ),
    HW_D_1( HW_F3 ),
    HW_D_1( HW_F4 ),
    HW_D_1( HW_F5 ),
    HW_D_1( HW_F6 ),
    HW_D_1( HW_F7 ),
    HW_D_1( HW_F8 ),
    HW_D_1( HW_F9 ),
    HW_D_1( HW_F10 ),
    HW_D_1( HW_F11 ),
    HW_D_1( HW_F12 ),
    HW_D_1( HW_F13 ),
    HW_D_1( HW_EMPTY )
};

static  hw_reg_set      scalarRegs[] = {
    HW_D_1( HW_D3 ),
    HW_D_1( HW_D4 ),
    HW_D_1( HW_D5 ),
    HW_D_1( HW_D6 ),
    HW_D_1( HW_D7 ),
    HW_D_1( HW_D8 ),
    HW_D_1( HW_D9 ),
    HW_D_1( HW_D10 ),
    HW_D_1( HW_EMPTY )
};


extern  void            InitPPCParmState( call_state *state ) {
/*************************************************************/

    state->parm.gr = FIRST_SCALAR_PARM_REG;
    state->parm.fr = FIRST_FLOAT_PARM_REG;
}

extern  type_length     ParmAlignment( type_def *tipe ) {
/*******************************************************/

    type_class_def      class;

    class = TypeClass( tipe );
    if( class == XX ) {
        if( tipe->length > 7 ) {
            return( 8 );
        }
    } else if( class == FD ) {
        return( 8 );
    } else if( class == U8 || class == I8 ) {
        return( 8 );
    }
    return( 4 );
}

static  hw_reg_set      floatRegSet( int index ) {
/************************************************/

    assert( index >= FIRST_FLOAT_PARM_REG && index <= LAST_FLOAT_PARM_REG );
    return( floatRegs[ index - FIRST_FLOAT_PARM_REG ] );
}

static  hw_reg_set      scalarRegSet( int index ) {
/*************************************************/

    assert( index >= FIRST_SCALAR_PARM_REG && index <= LAST_SCALAR_PARM_REG );
    return( scalarRegs[ index - FIRST_SCALAR_PARM_REG ] );
}

extern  hw_reg_set      ParmReg( type_class_def class, type_length len, type_length alignment, call_state *state ) {
/******************************************************************************************************************/

    hw_reg_set          parm;

    len = len;
    parm = HW_EMPTY;
    if( state->parm.offset & ( alignment - 1 ) ) {
        // we cheat here - know it's 4-byte aligned and needs to be 8-byte aligned
        assert( ( state->parm.offset & 0x03 ) == 0 );
        assert( alignment == 8 );
        state->parm.gr += 1;
        state->parm.offset += 4;
    }
    if( _IsFloating( class ) ) {
        // if we are passing on stack it was busted up in AssgnParms prior to this
        if( state->parm.fr <= LAST_FLOAT_PARM_REG ) {
            state->parm.fr += 1;
            state->parm.gr += 2;
            state->parm.offset += 8;
            parm = floatRegSet( state->parm.fr - 1 );
        }
    } else if( _IsI64( class ) ) {
        if( state->parm.gr <= (LAST_SCALAR_PARM_REG - 1) ) {
            state->parm.gr += 2;
            state->parm.offset += 8;
            parm = scalarRegSet( state->parm.gr - 2 );
            HW_TurnOn( parm, scalarRegSet( state->parm.gr - 1 ) );
        }
    } else {
        if( state->parm.gr <= LAST_SCALAR_PARM_REG ) {
            state->parm.gr += 1;
            state->parm.offset += 4;
            parm = scalarRegSet( state->parm.gr - 1 );
        }
    }
    HW_TurnOn( state->parm.used, parm );
    return( parm );
}
