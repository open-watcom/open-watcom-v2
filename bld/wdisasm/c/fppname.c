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

/*
 * Floating point instructions
 */

char TABLE                    * TABLE FppName[] = {
    "f2xm1",
    "fabs",
    "fadd",
    "fiadd",
    "faddp",
    "fbld",
    "fbstp",
    "fchs",
    "fclex",
    "fcom",
    "ficom",
    "fcomp",
    "ficomp",
    "fcompp",
    "fdecstp",
    "fdisi",
    "fdiv",
    "fidiv",
    "fdivp",
    "fdivr",
    "fidivr",
    "fdivrp",
    "feni",
    "ffree",
    "fincstp",
    "finit",
    "fld",
    "fild",
    "fld1",
    "fldcw",
    "fldenv",
    "fldl2e",
    "fldl2t",
    "fldlg2",
    "fldln2",
    "fldpi",
    "fldz",
    "fmul",
    "fimul",
    "fmulp",
    "fnop",
    "fpatan",
    "fprem",
    "fptan",
    "frndint",
    "frstor",
    "fsave",
    "fscale",
    "fsqrt",
    "fst",
    "fist",
    "fstp",
    "fistp",
    "fstcw",
    "fstenv",
    "fstsw",
    "fsub",
    "fisub",
    "fsubp",
    "fsubr",
    "fisubr",
    "fsubrp",
    "ftst",
    "fxam",
    "fxch",
    "fxtract",
    "fyl2x",
    "fyl2xp1",
    "fsetpm",
    "fsin",
    "fcos",
    "fsincos",
    "fprem1",
    "fucom",
    "fucomp",
    "fucompp",
    0
};
