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
* Description:  General access routines for trap file.
*
****************************************************************************/


#include <stdlib.h>
#include <string.h>
#include "tinyio.h"
#include "wdebug.h"
#include "stdwin.h"
#include "winacc.h"
#include "trperr.h"

#pragma aux set_carry = 0xf9;
extern void set_carry(void);

volatile bool HaveKey;
int _info;

/*
 * The handler installed by SetEventHook uses non-standard calling convention.
 * Arguments are passed in ax and cx, and setting carry flag before exit
 * may cause the message to be discarded. Also, the routine has to set ds
 * to the proper value (ie. no multiple instances - but it may not be possible
 * to register multiple event hooks anyway). See Undocumented Windows.
 */ 
void __far __loadds DebuggerHookRtn( unsigned event, unsigned info )
{
    if( event == WM_KEYDOWN ) {
        HaveKey = TRUE;
        _info = info;
    }
    set_carry();
}

unsigned ReqRead_user_keyboard( void )
{
    DWORD               end_time;
    read_user_keyboard_req      *acc;
    read_user_keyboard_ret      *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->key = 0;
    HaveKey = FALSE;
    SetEventHook( DebuggerHookRtn );
    end_time = GetTickCount() + acc->wait*1000L;
    for( ;; ) {
        if( HaveKey ) break;
        if( (acc->wait > 0) && (GetTickCount() > end_time) ) break;
    }
    ret->key = _info;
    SetEventHook( NULL );
    return( sizeof( *ret ) );
}

unsigned ReqGet_err_text( void )
{
    static const char * const dosErrMsgs[] = {
        #undef ERR_CODES
        #include "dosmsgs.h"
    };
    static const char * const winErrMsgs[] = {
        #define pick( a,b ) b,
        #include "winerr.h"
    };
    get_err_text_req    *acc;
    char                *err_txt;

    acc = GetInPtr( 0 );
    err_txt = GetOutPtr( 0 );

    if( acc->err >= WINERR_FIRST ) {
        if( acc->err < WINERR_LAST ) {
            strcpy( err_txt, winErrMsgs[ acc->err - WINERR_FIRST ] );
        } else {
            strcpy( err_txt, TRP_ERR_unknown_system_error );
        }
    } else {
        strcpy( err_txt, dosErrMsgs[ acc->err ] );
    }
    return( strlen( err_txt ) + 1 );
}

char *GetExeExtensions( void )
{
    return( ExtensionList );
}

char *DOSEnvFind( char *src )
{
    char        *p;
    char        *env;


    env = GetDOSEnvironment();
    do {
        p = src;
        do {
            if( *p == '\0' && *env == '=' ) return( env + 1 );
        } while( *env++ == *p++ );
        while( *env++ != '\0' )
            ;
    } while( *env != '\0' );
    return( NULL );
}
