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
#include "vergen.h"
#include "cfloat.h"

#include "s37ver.def"

extern  bool            CFIsI16(cfloat*);
extern  bool            CFIsI32(cfloat*);
extern  bool            IsRegPair(hw_reg_set);
extern  hw_reg_set      LowReg(hw_reg_set);
extern  hw_reg_set      HighReg(hw_reg_set);
extern  bool            OtherVerify(vertype,instruction*,name*,name*,name*);

extern  bool    DoVerify( vertype kind, instruction *ins ) {
/**********************************************************/

    name        *op1;
    name        *op2;
    name        *result;
    hw_reg_set  tmp;

    result = ins->result;
    if( ins->num_operands != 0 ) {
        op1 = ins->operands[ 0 ];
        if( ins->num_operands != 1 ) {
            op2 = ins->operands[ 1 ];
        }
    }
    switch( kind ) {
    case V_OP2I2:
        /*
         * U2 is OK too. A U2 operand wouldn't be present in the instruction
         * unless the tree code decided it was ok for a halfword instruction
         * Kludgey? Maybe just a little.
         */
        if( op2->n.name_class == U2 ) return( TRUE );
        if( op2->n.name_class == I2 ) return( TRUE );
        return( FALSE );
    case V_OP1SMALL:
        if( op1->c.const_type != CONS_ABSOLUTE ) return( FALSE );
        if( op1->c.int_value < 0 ) return( FALSE );
        if( op1->c.int_value > 4095 ) return( FALSE );
        return( TRUE );
    case V_OP1TEMP:
        if( op1->n.class == N_TEMP ) return( TRUE );
        return( FALSE );
    case V_OP1ADDR:
    case V_OP1LOC:
        if( op1->n.class == N_MEMORY ) return( TRUE );
        return( FALSE );
    case V_OP2I2CON:
        if( op2->c.const_type == CONS_ABSOLUTE
         && CFIsI16( op2->c.value ) ) return( TRUE );
        return( FALSE );
    case V_LA2:
        if( HW_CEqual( op1->r.reg, HW_G0 ) ) return( FALSE );
        /* fall through ! */
    case V_OP2SMALL:
        if( op2->c.const_type != CONS_ABSOLUTE ) return( FALSE );
        if( op2->c.int_value < 0 ) return( FALSE );
        if( op2->c.int_value > 4095 ) return( FALSE );
        return( TRUE );
    case V_MULPAIR:
        if( !IsRegPair( result->r.reg ) ) return( FALSE );
        tmp = LowReg( result->r.reg );
        if( !HW_Equal( tmp, op1->r.reg ) ) return( FALSE );
        return( TRUE );
    case V_CONVPAIR:
        if( !IsRegPair( result->r.reg ) ) return( FALSE );
        tmp = HighReg( result->r.reg );
        if( !HW_Equal( tmp, op1->r.reg ) ) return( FALSE );
        return( TRUE );
    case V_SIZE_SMALL:
        if( op1->n.size > 256 ) return( FALSE );
        return( TRUE );
    case V_OP2BYTE4CONS:
        if( !CFIsI32( op2->c.value ) ) return( FALSE );
        if( ( op2->c.int_value & 0xFFFFFF00 ) == 0 ) return( TRUE );
        if( ( op2->c.int_value & 0xFFFF00FF ) == 0 ) return( TRUE );
        if( ( op2->c.int_value & 0xFF00FFFF ) == 0 ) return( TRUE );
        if( ( op2->c.int_value & 0x00FFFFFF ) == 0 ) return( TRUE );
        return( FALSE );
    case V_OP2BYTE2CONS:
        if( !CFIsI16( op2->c.value ) ) return( FALSE );
        if( ( op2->c.int_value & 0xFFFFFF00 ) == 0 ) return( TRUE );
        if( ( op2->c.int_value & 0xFFFF00FF ) == 0 ) return( TRUE );
        return( FALSE );
    case V_CMPEQ_OP2ZERO:
        if( !OtherVerify( V_CMPEQ, ins, op1, op2, result ) ) return( FALSE );
        if( !OtherVerify( V_OP2ZERO, ins, op1, op2, result ) ) return( FALSE );
        return( TRUE );
    default:
        return( OtherVerify( kind, ins, op1, op2, result ) );
    }
    return( FALSE );
}
