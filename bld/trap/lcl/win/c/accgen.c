/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  General access routines for trap file.
*
****************************************************************************/


#include <stdlib.h>
#include <string.h>
#include "wdebug.h"
#include "stdwin.h"
#include "winctrl.h"
#include "trperr.h"
#include "winpath.h"
#include "winerr.h"


extern void set_carry( void );
#pragma aux set_carry = 0xf9;

volatile bool HaveKey;
int _info;

/*
 * The handler installed by SetEventHook uses non-standard calling convention.
 * Arguments are passed in ax and cx, and setting carry flag before exit
 * may cause the message to be discarded. Also, the routine has to set ds
 * to the proper value (ie. no multiple instances - but it may not be possible
 * to register multiple event hooks anyway). See Undocumented Windows.
 */

static event_hook_fn    DebuggerHookRtn;

static void __far __loadds DebuggerHookRtn( unsigned event, unsigned info )
{
    if( event == WM_KEYDOWN ) {
        HaveKey = true;
        _info = info;
    }
    set_carry();
}

trap_retval TRAP_CORE( Read_user_keyboard )( void )
{
    DWORD               end_time;
    read_user_keyboard_req      *acc;
    read_user_keyboard_ret      *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->key = 0;
    HaveKey = false;
    SetEventHook( DebuggerHookRtn );
    end_time = GetTickCount() + acc->wait*1000L;
    for( ;; ) {
        if( HaveKey )
            break;
        if( (acc->wait > 0) && (GetTickCount() > end_time) ) {
            break;
        }
    }
    ret->key = _info;
    SetEventHook( NULL );
    return( sizeof( *ret ) );
}

void DisableHookEvents( void )
{
    if( HookRtn != NULL ) {
        SetEventHook( NULL );
    }
}

void EnableHookEvents( void )
{
    if( HookRtn != NULL ) {
        SetEventHook( HookRtn );
    }
}

trap_retval TRAP_CORE( Get_err_text )( void )
{
    static const char * const doswinErrMsgs[] = {
        #define pick(a,b)   b,
        #include "dosmsgs.h"
        #include "winmsgs.h"
        #undef pick
    };
    get_err_text_req    *acc;
    char                *err_txt;

    acc = GetInPtr( 0 );
    err_txt = GetOutPtr( 0 );

    if( acc->err < ERR_LAST ) {
        StrCopyDst( doswinErrMsgs[acc->err], err_txt );
    } else {
        StrCopyDst( TRP_ERR_unknown_system_error, err_txt );
        ultoa( acc->err, err_txt + strlen( err_txt ), 16 );
    }
    return( strlen( err_txt ) + 1 );
}
