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
* Description:  Execute a user routine.
*
****************************************************************************/


#include <stdlib.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgstk.h"
#include "dbgitem.h"
#include "madinter.h"
#include "dbgexec.h"
#include "dbgexpr4.h"
#include "dbgexpr.h"
#include "dbgloc.h"
#include "dbgcall2.h"


extern stack_entry      *ExprSP;


extern void             DbgUpdate( update_list );
extern stack_entry      *StkEntry( int );
extern void             SetRegIP( address );
extern void             SetRegSP( address );
extern address          GetRegSP( void );
extern machine_state    *AllocMachState( void );
extern void             CopyMachState( machine_state *, machine_state * );
extern void             FreeMachState( machine_state *);
extern void             AddrFix( address * );


static machine_state    *FreezeRegSet = NULL;
static location_context FreezeContext;
static mod_handle       FreezeContextMod;

/*
 * FreezeRegs -- freeze a copy of the register values
 */

void FreezeRegs( void )
{
    FreezeContextMod = ContextMod;
    FreezeContext = Context;
    FreezeRegSet = AllocMachState();
    CopyMachState( DbgRegs, FreezeRegSet );
}

/*
 * UnFreezeRegs -- restore a frozen register set
 */

void UnFreezeRegs( void )
{
    if( FreezeRegSet != NULL ) {
        CopyMachState( FreezeRegSet, DbgRegs );
        Context = FreezeContext;
        FreeMachState( FreezeRegSet );
        DbgUpdate( UP_MEM_CHANGE );
        FreezeRegSet = NULL;
        ContextMod = FreezeContextMod;
    }
}

/*
 * CallRoutine -- execute the user routine
 */

static bool CallRoutine( void )
{
    unsigned    trap;
    address     sp;

    sp = GetRegSP();
    for( ;; ) {
        trap = ExecProg( FALSE, TRUE, FALSE );
        if( !(trap & COND_BREAK) ) {
            ReportTrap( trap, FALSE );
            return( FALSE );
        }
        if( MADTraceHaveRecursed( sp, &DbgRegs->mr ) != MS_OK ) {
            return( TRUE );
        }
    }
}


bool PerformExplicitCall( address start, mad_string ctype, unsigned num_parms )
{
    bool                ret;
    stack_entry         *src;
    address             stack;
    unsigned            align;
    unsigned long       amount;
    mad_type_info       mti;

    stack = GetRegSP();
    GetMADTypeDefaultAt( stack, MTK_INTEGER, &mti );
    align = mti.b.bits / BITS_PER_BYTE;
    for( ; num_parms != 0; --num_parms ) {
        if( ExprSP->v.loc.e[0].type!=LT_ADDR && ExprSP->v.loc.e[0].u.p==NULL ) {
            /* push item */
            src = StkEntry( 1 );
            amount = (src->info.size + (align-1)) & -align;
            if( _IsOff( SW_STACK_GROWS_UP ) ) {
                stack.mach.offset -= amount;
            }
            LocationCreate( &ExprSP->v.loc, LT_ADDR, &stack );
            if( _IsOn( SW_STACK_GROWS_UP ) ) {
                stack.mach.offset += amount;
            }
            ExprSP->info = src->info;
            ExprSP->flags |= SF_LOCATION;
            ExprSP->th = NULL;
        }
        SwapStack( 1 );
        DoAssign();
        PopEntry();
    }
    AddrFix( &start );
    SetRegSP( stack );
    MADCallBuildFrame( ctype, start, start, &DbgRegs->mr, &DbgRegs->mr );
    DbgTmpBrk.loc.addr = start;
    NullStatus( &DbgTmpBrk );
    DbgTmpBrk.status.b.active = TRUE;
    ret = CallRoutine();
    NullStatus( &DbgTmpBrk );
    return( ret );
}

bool PerformCall( address start, bool far_rtn, unsigned num_parms )
{
    const mad_string    *list;

    list = MADCallTypeList();
    if( far_rtn && list[0] != MAD_MSTR_NIL ) ++list;
    return( PerformExplicitCall( start, list[0], num_parms ) );
}
