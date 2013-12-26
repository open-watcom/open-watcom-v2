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
* Description:  MIPS specific CPU functional units.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "indvars.h"
#include "model.h"
#include "sched.h"

enum {
    F_NONE      = 0x0000,
    F_ALU       = 0x0001,
    F_FOP       = 0x0004,
    F_LDSTR     = 0x0008,
};

static FU_entry FUnits[] = {
    /* units                    unit_stall  opnd_stall */
    F_NONE,                     0,          0,      /* NO */
    F_ALU,                      1,          1,      /* ALU */
    F_ALU,                      1,          1,      /* LDSTR */
};


FU_entry *FUEntry( instruction *ins )
{
    return( &FUnits[ins->u.gen_table->func_unit] );
}
