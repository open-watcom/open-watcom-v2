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


#include "regsetop.h"
#include "vergen.h"
#include "funits.h"

#define C           0x01u /*  constant */
#define R           0x02u /*  register */
#define M           0x04u /*  memory */
#define U           0x08u /*  unknown whether reg or memory */
#define ANY         (C | R | M | U)
#define OP__MUL     0x10u

#define RESULT_MUL  1u
#define C_R         C
#define R_R         R
#define M_R         M
#define U_R         U
#define OP1_MUL     OP__MUL
#define C_1         (C * OP1_MUL)
#define R_1         (R * OP1_MUL)
#define M_1         (M * OP1_MUL)
#define U_1         (U * OP1_MUL)
#define OP2_MUL     (OP1_MUL * OP__MUL)
#define C_2         (C * OP2_MUL)
#define R_2         (R * OP2_MUL)
#define M_2         (M * OP2_MUL)
#define U_2         (U * OP2_MUL)

#define OTHER_MUL   (OP2_MUL * OP__MUL)
#define EQ_R1       (R * OTHER_MUL)
#define EQ_R2       (C * OTHER_MUL)
#define NONE        (EQ_R1 | EQ_R2)
#define SETS_SC     (M * OTHER_MUL)
#define PRESERVE    (U * OTHER_MUL)
#define SETS_CC     (SETS_SC | PRESERVE)

#define BOTH_EQ     0
#define NO_CC       0
#define MASK_CC     (SETS_CC)

typedef unsigned_32     operand_types;

#define _Bin( op1, op2, res, match )    \
        ( (op1)*OP1_MUL | (op2)*OP2_MUL | \
        (res)*RESULT_MUL | (match) )

#define _BinSC( op1, op2, res, match )  \
        ( (op1)*OP1_MUL | (op2)*OP2_MUL | \
        (res)*RESULT_MUL | (match) | SETS_SC )

#define _BinPP( op1, op2, res, match )  \
        ( (op1)*OP1_MUL | (op2)*OP2_MUL | \
        (res)*RESULT_MUL | (match) |  PRESERVE )

#define _BinCC( op1, op2, res, match )  \
        ( (op1)*OP1_MUL | (op2)*OP2_MUL | \
        (res)*RESULT_MUL | (match) | SETS_CC )

#define _Side( op1, op2 )       \
        ( (op1)*OP1_MUL | (op2)*OP2_MUL | \
        ANY*RESULT_MUL | NONE )

#define _SidCC( op1, op2 )      \
        ( (op1)*OP1_MUL | (op2)*OP2_MUL | \
        ANY*RESULT_MUL | NONE | SETS_CC )

#define _SidSC( op1, op2 )      \
        ( (op1)*OP1_MUL | (op2)*OP2_MUL | \
        ANY*RESULT_MUL | NONE | SETS_SC )

#define _SidPP( op1, op2 )      \
        ( (op1)*OP1_MUL | (op2)*OP2_MUL | \
        ANY*RESULT_MUL | NONE | PRESERVE )

#define _Un( op, res, match )   \
        ( (op)*OP1_MUL | ANY*OP2_MUL | \
        (res)*RESULT_MUL | (match) )

#define _UnCC( op, res, match ) \
        ( (op)*OP1_MUL | ANY*OP2_MUL | \
        (res)*RESULT_MUL | (match) | SETS_CC )

#define _UnSC( op, res, match ) \
        ( (op)*OP1_MUL | ANY*OP2_MUL | \
        (res)*RESULT_MUL | (match) | SETS_SC )

#define _UnPP( op, res, match ) \
        ( (op)*OP1_MUL | ANY*OP2_MUL | \
        (res)*RESULT_MUL | (match) | PRESERVE )

#define _None() NONE

#define _OE(t,v,r,g,f) { t, v, g, r, f }

typedef struct opcode_entry {
        operand_types           op_type;
        vertype                 verify;         /*  verification routine */
        gentype                 generate;       /*  generation routine */
        op_reg_set_index        reg_set;
        futype                  func_unit;      /*  functional unit info */
} opcode_entry;
