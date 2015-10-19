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


#include <stdlib.h>
#include <ctype.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgerr.h"
#include "dbghook.h"
#include "dbgio.h"
#include "spawn.h"
#include "dui.h"
#include "dbgutil.h"
#include "dbgmain.h"


extern void             NewLang( const char *lang );
extern bool             IsInternalMod( mod_handle );
extern bool             HookPendingPush( void );


static bool ProcPending( void )
{
    for( ;; ) {
        for( ;; ) {
            if( InpStack == NULL ) return( FALSE );
            if( InpStack->type & INP_NEW_LANG ) {
                InpStack->type &= ~INP_NEW_LANG;
                if( (CodeAddrMod != NO_MOD) && !IsInternalMod( CodeAddrMod ) ) {
                    NewLang( ModSrcLang( CodeAddrMod ) );
                }
            }
            if( InpStack->type & INP_STOP_PURGE ) return( FALSE );
            if( InpStack->type & INP_NO_CMD ) break;
            if( CurrToken == T_LINE_SEPARATOR ) break;
            _SwitchSet( SW_CMD_INTERACTIVE, !(InpStack->type & (INP_BREAK_POINT|INP_HOOK)));
            _SwitchSet( SW_IN_REPLAY_MODE, (InpStack->type & INP_REPLAYED) );
            ProcACmd();
        }
        if( InpStack->rtn( InpStack->handle, INP_RTN_EOL ) ) continue;
        PopInpStack();
    }
}

void DoProcPending( void )
{
    while( ProcPending() ) ;
    _SwitchOn( SW_CMD_INTERACTIVE );
    _SwitchOff( SW_IN_REPLAY_MODE );
}

bool ProcInput( void )
{
    bool        ret;

    _SwitchOn( SW_WND_DOING_INPUT );
    ret = Spawn( DoProcPending );
    DUIProcPendingPaint();
    _SwitchOff( SW_WND_DOING_INPUT );
    return( ret );
}

void DoInput( void )
{
    if( _IsOff( SW_WND_DOING_INPUT ) ) {
        for ( ;; ) {
            if( ProcInput() ) break;
            if( !HookPendingPush() ) break;
        }
    }
}
