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

ins_name  const           FppTab1[] = {
    I_FADD,
    I_FMUL,
    I_FCOM,
    I_FCOMP,
    I_FSUB,
    I_FSUBR,
    I_FDIV,
    I_FDIVR,
    I_FLD,
    I_INVALID,
    I_FST,
    I_FSTP
};

ins_name  const           FppTab2[] = {
    I_FLDENV,
    I_FLDCW,
    I_FSTENV,
    I_FSTCW,
    I_INVALID,
    I_FLD,
    I_INVALID,
    I_FSTP,
    I_FRSTOR,
    I_INVALID,
    I_FSAVE,
    I_FSTSW,
    I_FBLD,
    I_FILD,
    I_FBSTP,
    I_FISTP
};

ins_name const            FppTab3[] = {
    I_FLD,              /* 0xd9 0xc0 */
    I_FXCH,
    I_INVALID,
    I_FSTP,             /* 0xd9 0xd8  redundant opcode */
    I_INVALID,
    I_INVALID,
    I_INVALID,
    I_INVALID,
    I_INVALID,          /* 0xdb 0xc0 */
    I_INVALID,
    I_INVALID,
    I_INVALID,
    I_INVALID,
    I_INVALID,
    I_INVALID,
    I_INVALID,
    I_FFREE,            /* 0xdd 0xc0 */
    I_FXCH,             /* 0xdd 0xc8  redundant opcode */
    I_FST,
    I_FSTP,
    I_FUCOM,
    I_FUCOMP,
    I_INVALID,
    I_INVALID,
    I_FFREE,            /* 0xdf 0xc0  redundant opcode */
    I_FXCH,             /* 0xdf 0xc8  redundant opcode */
    I_FST,              /* 0xdf 0xd0  redundant opcode */
    I_FSTP,             /* 0xdf 0xd8  redundant opcode */
    I_INVALID,
    I_INVALID,
    I_INVALID,
    I_INVALID
};

ins_name const            FppTab4[] = {
    I_FCHS,
    I_FABS,
    I_INVALID,
    I_INVALID,
    I_FTST,
    I_FXAM,
    I_INVALID,
    I_INVALID,
    I_FLD1,
    I_FLDL2T,
    I_FLDL2E,
    I_FLDPI,
    I_FLDLG2,
    I_FLDLN2,
    I_FLDZ,
    I_INVALID,
    I_F2XM1,
    I_FYL2X,
    I_FPTAN,
    I_FPATAN,
    I_FXTRACT,
    I_FPREM1,
    I_FDECSTP,
    I_FINCSTP,
    I_FPREM,
    I_FYL2XP1,
    I_FSQRT,
    I_FSINCOS,
    I_FRNDINT,
    I_FSCALE,
    I_FSIN,
    I_FCOS
};

ins_name const            FppTab5[] = {
    I_FENI,
    I_FDISI,
    I_FCLEX,
    I_FINIT,
    I_FSETPM,
    I_INVALID,
    I_INVALID,
    I_INVALID
};

ins_name const            FppTab6[] = {
    I_FADD,
    I_FMUL,
    I_FCOM,
    I_FCOMP,
    I_FSUB,
    I_FSUBR,
    I_FDIV,
    I_FDIVR
};

ins_name  const           FppTab7[] = {
    I_FADD,
    I_FMUL,
    I_FCOM,
    I_FCOMP,
    I_FSUBR,
    I_FSUB,
    I_FDIVR,
    I_FDIV
};
