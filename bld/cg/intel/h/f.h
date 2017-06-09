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


/* Any changes to this file must be reflected in 386FUNIT.C */
#ifdef FPU_586
/*   enum       units               unit_stall  opnd_stall          */
_F_( FU_NO,     F_NONE,             0,          0       )   /* NO   */
_F_( FU_ALU1,   F_ALU1,             1,          2       )   /* ALU1 */
_F_( FU_ALUX,   F_ALU1 | F_ALU2,    1,          2       )   /* ALUX */
_F_( FU_IMUL,   F_ALU1,             11,         11      )   /* IMUL */
_F_( FU_IDIV,   F_ALU1,             24,         24      )   /* IDIV */
_F_( FU_FOP,    F_ALU1,             1,          1       )   /* FOP  */
_F_( FU_FADD,   F_ALU1,             1,          3       )   /* FADD */
_F_( FU_FMUL,   F_ALU1,             2,          3       )   /* FMUL */
_F_( FU_FDIV,   F_FOP,              32,         32      )   /* FDIV */
_F_( FU_TRIG,   F_ALU1,             69,         69      )   /* TRIG */
_F_( FU_CALL,   F_NONE,             0,          0       )   /* CALL */
#else
/*   enum       units               unit_stall  opnd_stall          */
_F_( FU_NO,     F_NONE,             0,          0       )   /* NO   */
_F_( FU_ALU1,   F_ALU1,             1,          2       )   /* ALU1 */
_F_( FU_ALUX,   F_ALU1 | F_ALU2,    1,          2       )   /* ALUX */
_F_( FU_IMUL,   F_ALU1,             11,         11      )   /* IMUL */
_F_( FU_IDIV,   F_ALU1,             24,         24      )   /* IDIV */
_F_( FU_FOP,    F_FOP,              8,          8       )   /* FOP  */
_F_( FU_FADD,   F_FOP,              10,         10      )   /* FADD */
_F_( FU_FMUL,   F_FOP,              16,         16      )   /* FMUL */
_F_( FU_FDIV,   F_FOP,              73,         73      )   /* FDIV */
_F_( FU_TRIG,   F_FOP,              241,        241     )   /* TRIG */
_F_( FU_CALL,   F_NONE,             0,          0       )   /* CALL */
#endif
