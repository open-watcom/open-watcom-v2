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
* Description:  Update debugger's internal state.
*
****************************************************************************/


#include <stdlib.h>
#include <ctype.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgerr.h"
#include "dbglit.h"
#include "dbghook.h"
#include "dbgio.h"
#include "dui.h"
#include "dbgutil.h"
#include "dbgstk.h"
#include "dbgexpr.h"
#include "dbgbrk.h"
#include "dipimp.h"
#include "dipinter.h"
#include "dbgreg.h"
#include "dbgupdt.h"

extern void             SymCompFini( void );
extern address          GetCodeDot( void );


void DbgUpdate( update_list flags )
{
    mod_handle  prev_mod;
    input_stack *inp;

    UpdateFlags |= flags;
    if( flags & ( UP_SYM_CHANGE ) ) {
        BrkAddrRefresh();
    }
    if( flags & (UP_REG_CHANGE|UP_CSIP_CHANGE) ) {
        InitLC( &Context, TRUE );
    }
    if( flags & UP_NEW_PROGRAM ) {
        SetStackPos( &Context, 0 );
    }
    if( flags & (UP_SYM_CHANGE+UP_STACKPOS_CHANGE+UP_REG_CHANGE+UP_CSIP_CHANGE+UP_CSIP_JUMPED) ) {
        SymCompFini();
        prev_mod = ContextMod;
        if( DeAliasAddrMod( Context.execution, &ContextMod ) == SR_NONE ) ContextMod = NO_MOD;
        if( prev_mod != ContextMod ) {
            HookNotify( FALSE, HOOK_NEW_MODULE );
        }
    }
    if( flags & (UP_CODE_ADDR_CHANGE | UP_SYM_CHANGE ) ) {
        prev_mod = CodeAddrMod;
        if( DeAliasAddrMod( GetCodeDot(), &CodeAddrMod ) == SR_NONE ) {
            CodeAddrMod = NO_MOD;
        }
        if( prev_mod != CodeAddrMod ) {
            for( inp = InpStack; inp != NULL; inp = inp->link ) {
                inp->type |= INP_NEW_LANG;
            }
        }
    }
    DUIUpdate( flags );
}
