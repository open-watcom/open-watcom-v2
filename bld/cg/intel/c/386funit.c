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
#include "indvars.h"
#include "model.h"
#include "inssched.h"

enum {
    F_NONE      = 0x0000,
    F_ALU1      = 0x0001,
    F_ALU2      = 0x0002,
    F_FOP       = 0x0004,
};

/*
    NOTE: The 486 table is the same as the 586 for the integer units,
    and like the real machine for the floating point units. That means
    that even if someone optimizes for the 486, the integer ops should
    be OK for the 586, but we'll still get the floating/integer instruction
    intermixing that makes best use of the 486 F.P. asynchronous execution.
*/
static const FU_entry FUnits486[] = {
    /* units                    unit_stall  opnd_stall */
   {F_NONE,                     0,          0},     /* NO */
   {F_ALU1,                     1,          2},     /* ALU1 */
   {F_ALU1 | F_ALU2,            1,          2},     /* ALUX */
   {F_ALU1,                     11,         11},    /* IMUL */
   {F_ALU1,                     24,         24},    /* IDIV */
   {F_FOP,                      8,          8},     /* FOP */
   {F_FOP,                      10,         10},    /* FADD */
   {F_FOP,                      16,         16},    /* FMUL */
   {F_FOP,                      73,         73},    /* FDIV */
   {F_FOP,                      241,        241},   /* TRIG */
   {F_NONE,                     0,          0},     /* CALL */
};

static const FU_entry FUnits586[] = {
    /* units                    unit_stall  opnd_stall */
   {F_NONE,                     0,          0},     /* NO */
   {F_ALU1,                     1,          2},     /* ALU1 */
   {F_ALU1 | F_ALU2,            1,          2},     /* ALUX */
   {F_ALU1,                     11,         11},    /* IMUL */
   {F_ALU1,                     24,         24},    /* IDIV */
   {F_ALU1,                     1,          1},     /* FOP */
   {F_ALU1,                     1,          3},     /* FADD */
   {F_ALU1,                     2,          3},     /* FMUL */
   {F_FOP,                      32,         32},    /* FDIV */
   {F_ALU1,                     69,         69},    /* TRIG */
   {F_NONE,                     0,          0},     /* CALL */
};


const FU_entry *FUEntry( instruction *ins )
{
    if( _CPULevel( CPU_586 ) ) {
        return( &FUnits586[ ins->u.gen_table->func_unit ] );
    } else {
        return( &FUnits486[ ins->u.gen_table->func_unit ] );
    }
}
