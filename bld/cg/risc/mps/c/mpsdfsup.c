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
* Description:  MIPS specific DWARF support routines.
*
****************************************************************************/


#include "standard.h"
#include "hostsys.h"
#include "coderep.h"
#include "pattern.h"
#include "procdef.h"
#include "cgdefs.h"
#include "symdbg.h"
#include "model.h"
#include "ocentry.h"
#include "zoiks.h"
#include "cgaux.h"
#include "typedef.h"
#include "dbgstrct.h"
#include <stdio.h>
#include <stdarg.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>
#include "dw.h"
#include "mipsregn.h"
#include "dwarf.h"
#include "dfdbg.h"

typedef enum {
    #define DW_REG( __n  )   DW_MIPS_##__n,
    #include "dwregmps.h"
    DW_REG( MAX )
    #undef DW_REG
} dw_regs;

extern  void            DBLocFini( dbg_loc loc );
extern  hw_reg_set      StackReg( void );
extern  uint_8          RegTrans( hw_reg_set reg );

extern  dw_client       Client;


extern  uint DFRegMap( hw_reg_set hw_reg )
/****************************************/
{
    dw_regs           ret;

    ret = RegTrans( hw_reg );
    return( ret );
}


extern  void   DFOutReg( dw_loc_id locid, name *reg )
/***************************************************/
{
    dw_regs     regnum;

    regnum = MIPSRegN( reg );
    DWLocReg( Client, locid, regnum );
}


extern  void DFOutRegInd( dw_loc_id locid, name *reg )
/****************************************************/
{
    dw_regs     regnum;

    regnum = MIPSRegN( reg );
    DWLocOp( Client, locid, DW_LOC_breg, regnum, 0 );
}


extern dw_regs DFStkReg( void )
/*****************************/
{
    dw_regs    ret;
    hw_reg_set stk;

    stk = StackReg();
    ret = RegTrans( stk );
    return( ret );
}
