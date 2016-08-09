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
* Description:  Intel x86 instruction verification codes.
*
****************************************************************************/


_V_( V_NO ),
_V_( V_80186 ),
_V_( V_CMPEQ ),
_V_( V_DIFF_TYPES ),
_V_( V_HIGHEQLOW ),
_V_( V_OFFSETZERO ),
_V_( V_OP1ADDR ),
_V_( V_OP1LOC ),
_V_( V_OP1ONE ),
_V_( V_OP1ZERO ),
_V_( V_OP2HIGH_B_ZERO ),
_V_( V_OP2LOW_B_FF ),
_V_( V_OP2LOW_B_ZERO ),
_V_( V_OP2LOW_W_FFFF ),
_V_( V_OP2NEG ),
_V_( V_OP2ONE ),
_V_( V_OP2TWO ),
_V_( V_OP2ZERO ),
_V_( V_RESLOC ),
_V_( V_SAME_LOCN ),
_V_( V_BYTESHIFT ),
_V_( V_CYP2SHIFT ),
_V_( V_CYP4SHIFT ),
_V_( V_OP2HIGH_B_FF ),
_V_( V_OP2HIGH_W_FFFF ),
_V_( V_OP2HIGH_W_ZERO ),
_V_( V_OP2LOW_W_ZERO ),
_V_( V_U_TEST ),
_V_( V_CMPTRUE ),
_V_( V_CMPFALSE ),
_V_( V_OP2PTR ),
_V_( V_SPEED ),
_V_( V_SIZE ),
_V_( V_DIV_BUG ),
_V_( V_OP1SP ),
_V_( V_WORDREG_AVAIL ),
_V_( V_LEA ),
_V_( V_SWAP_GOOD ),
_V_( V_OP1RELOC ),
_V_( V_AC_BETTER ),
_V_( V_OP2_FFFFFFFF ),
_V_( V_LSHONE ),
_V_( V_SAME_TYPE ),
_V_( V_OP2POW2 ),
_V_( V_OP2POW2_286 ),
_V_( V_OP2TWO_SIZE ),
_V_( V_GOOD_CLR ),
_V_( V_LEA_GOOD ),
_V_( V_P5_FXCH ),
_V_( V_CYP_SEX ),
_V_( V_OP2HIGH_W_FFFF_REG ),
_V_( V_OP2HIGH_W_ZERO_REG ),
_V_( V_NEGATE ),
_V_( V_CONSTTEMP ),
_V_( V_INTCOMP ),
_V_( V_80386 ),
_V_( V_SWAP_GOOD_386 ),
_V_( V_SHIFT2BIG ),
_V_( V_CDQ ),
