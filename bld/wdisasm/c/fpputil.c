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


#include "disasm.h"

#define _Register( var )        ( var & 0x07 )

static  float_type      Fptype;


ins_name GetFppCode()
/*******************/

{
    uint_8          op;
    uint_8          op_a;
    uint_8          op_b;
    ins_name        fp;

    fp = I_INVALID;
    Fptype = FT_NULL;
    op_a = ( Opcode & 0x01 );
    op_b = ( SecondByte & 0x38 ) >> 3;
    op = ( op_a << 3 ) | op_b;
    if( ( SecondByte & 0xc0 ) == 0xc0 ) {      /* non memory */
        if( Opcode == 0xd9 && op >= 12 ) {
            fp = FppTab4[ SecondByte & 0x1f ];
        } else if( Opcode == 0xd9 && SecondByte == 0xd0 ) {
            fp = I_FNOP;
        } else if( Opcode == 0xda && SecondByte == 0xe9 ) {
            fp = I_FUCOMPP;
        } else if( (Opcode & 0xfe) == 0xda && (SecondByte & 0xe0) == 0xc0 ) {
            Fptype = FT_STACK2D;
            switch( op ) {
            case 0x0: fp = I_FCMOVB; break;
            case 0x1: fp = I_FCMOVE; break;
            case 0x2: fp = I_FCMOVBE; break;
            case 0x3: fp = I_FCMOVU; break;
            case 0x0+8: fp = I_FCMOVNB; break;
            case 0x1+8: fp = I_FCMOVNE; break;
            case 0x2+8: fp = I_FCMOVNBE; break;
            case 0x3+8: fp = I_FCMOVNU; break;
            }
        } else if( Opcode == 0xdb && op_b == 0x6 ) {
            fp = I_FCOMI;
            Fptype = FT_STACK2D;
        } else if( Opcode == 0xdb && op_b == 0x5 ) {
            fp = I_FUCOMI;
            Fptype = FT_STACK2D;
        } else if( Opcode == 0xdf && op_b == 0x6 ) {
            fp = I_FCOMIP;
            Fptype = FT_STACK2D;
        } else if( Opcode == 0xdf && op_b == 0x5 ) {
            fp = I_FUCOMIP;
            Fptype = FT_STACK2D;
        } else if( (op >= 8 && op <= 11) || Opcode == 0xdd ) {
            Fptype = FT_STACK;
            fp = FppTab3[ op_b + ( Opcode & 6 ) * 4 ];
        } else if( op == 12 && op_b == 4 ) {
            if( Opcode == 0xdf && SecondByte == 0xe0 ) {
                fp = I_FSTSW;
                Fptype = FT_AX;
            } else if( Opcode == 0xdb ) {
                fp = FppTab5[ SecondByte & 7 ];
            }

        } else if( op >= 8 ) {
            /* illegal opcode */
            fp = I_INVALID;
        } else {        /* op in 0..7 */
            if( Opcode == 0xd8 ) {
                fp = FppTab6[ op ];
            } else {
                fp = FppTab7[ op ];
            }
            if( op == 2 || op == 3 ) {
                if( Opcode == 0xde ) {
                    if( SecondByte == 0xd9 ) {
                        fp = I_FCOMPP;
                    } else if( fp == I_FCOM ) {
                        fp = I_FCOMP;
                    } else {
                        fp = I_INVALID;
                    }
                }
                Fptype = FT_STACK;
            } else {
                Fptype = FT_STACK2;
                if( Opcode == 0xde ) {
                    fp += 2;    /* eg FADD -> FADDP */
                }
            }
        }
    } else {
        if( op >= 12 ) {
            Fptype = FT_MOD_RM;
            op_a = ( Opcode & 0x06 ) << 1;
            op = ( op & 0x03 ) | op_a;
            fp = FppTab2[ op ];
        } else {
            Fptype = FT_MF_MOD_RM;
            fp = FppTab1[ op ];
            if( ( Opcode & 0x02 ) && ( fp != I_INVALID ) ) {
                ++fp;
            }
        }
        CurrIns.mem_ref_size = 1;
        switch( Opcode & 7 ) {
        case 1:
            if( op_b > 3 ) break;
        case 0:
            CurrIns.mem_ref_size = 4;
            break;
        case 3:
            if( op_b > 3 ) {
                CurrIns.mem_ref_size = 10;
                break;
            }
        case 2:
            CurrIns.mem_ref_size = 4;
            break;
        case 5:
            if( op_b > 3 ) break;
        case 4:
            CurrIns.mem_ref_size = 8;
            break;
        case 7:
            if( op_b > 3 ) {
                CurrIns.mem_ref_size = ( op_b & 1 ) ? 8 : 10;
                break;
            }
        case 6:
            CurrIns.mem_ref_size = 2;
            break;
        }
    }
    return( fp );
}


void GetFppOp()
/*************/

{
    switch( Fptype ) {
    case FT_NULL:
        GetDataByte();
        break;
    case FT_MOD_RM:     /* fadd +3[SI] */
        CurrIns.num_oper = 1;
        ModRMOp( OP_1, FALSE );
        if( Opcode == 0xdb ) {
            CurrIns.modifier = MOD_TREAL;
        } else if( Opcode == 0xdf ) {
            if( SecondByte & 0x08 ) {
                CurrIns.modifier = MOD_LREAL;     /* 64 bit integer */
            } else {
                CurrIns.modifier = MOD_TREAL;     /* BCD */
            }
        }
        break;
    case FT_MF_MOD_RM:
        CurrIns.num_oper = 1;
        ModRMOp( OP_1, FALSE );
        CurrIns.modifier = ( Opcode & 0x06 ) >> 1;
        break;
    case FT_STACK:
        CurrIns.num_oper = 1;
        CurrIns.op[ OP_1 ].mode = ADDR_REG;
        CurrIns.op[ OP_1 ].size = 1;
        CurrIns.op[ OP_1 ].base = FIRST_FP_REG + _Register( SecondByte );
        GetDataByte();
        break;
    case FT_STACK2:
        CurrIns.num_oper = 2;
        CurrIns.op[ OP_1 ].mode = ADDR_REG;
        CurrIns.op[ OP_1 ].size = 1;
        CurrIns.op[ OP_2 ].mode = ADDR_REG;
        GetDataByte();
        if( Opcode & 0x04 ) {
            CurrIns.op[ OP_1 ].base = FIRST_FP_REG + _Register( SecondByte );
            CurrIns.op[ OP_2 ].base = ST_REG;
        } else {
            CurrIns.op[ OP_1 ].base = ST_REG;
            CurrIns.op[ OP_2 ].base = FIRST_FP_REG + _Register( SecondByte );
        }
        break;
    case FT_STACK2D:
        CurrIns.num_oper = 2;
        CurrIns.op[ OP_1 ].mode = ADDR_REG;
        CurrIns.op[ OP_1 ].size = 1;
        CurrIns.op[ OP_2 ].mode = ADDR_REG;
        GetDataByte();
        CurrIns.op[ OP_1 ].base = ST_REG;
        CurrIns.op[ OP_2 ].base = FIRST_FP_REG + _Register( SecondByte );
        break;
    case FT_AX:
        CurrIns.num_oper = 1;
        CurrIns.op[ OP_1 ].mode = ADDR_REG;
        CurrIns.op[ OP_1 ].size = 1;
        CurrIns.op[ OP_1 ].base = AX_REG;
        GetDataByte();
        break;
    }
}
