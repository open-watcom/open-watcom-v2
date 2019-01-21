/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2018 The Open Watcom Contributors. All Rights Reserved.
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


#include "_cgstd.h"
#include "coderep.h"
#include "zoiks.h"
#include "data.h"
#include "types.h"
#include "rgtbl.h"
#include "parm.h"
#include "feprotos.h"


hw_reg_set      ParmInLineReg( parm_state *state )
/************************************************/
{
    hw_reg_set  regs;

    regs = *state->curr_entry;
    if( !HW_CEqual( regs, HW_EMPTY ) ) {
        state->curr_entry++;
    }
    regs = InLineParm( regs, state->used );
    HW_TurnOn( state->used, regs );
    return( regs );
}

type_length      ParmMem( type_length length, type_length alignment, call_state *state )
/**************************************************************************************/
{
    type_length     offset;
    type_length     word_size;

    word_size = REG_SIZE;
#if _TARGET & _TARG_80386
    if( _RoutineIsFar16( state->attr ) ) {
        // use 16-bit word size for rounding
        word_size >>= 1;
    }
#endif
    offset = state->parm.offset;
    if( offset & ( alignment - 1 ) ) {
        offset = _RoundUp( offset, alignment );
    }
    state->parm.offset = offset + _RoundUp( length, word_size );
    if( state->parm.offset > MaxStack ) {
        MaxStack = state->parm.offset;
    }
    return( offset );
}

type_def        *QParmType( cg_sym_handle func, cg_sym_handle parm, type_def *tipe )
/**********************************************************************************/
{
    return( TypeAddress( FEParmType( func, parm, tipe->refno ) ) );
}
