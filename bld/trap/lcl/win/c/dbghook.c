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


#include <stdio.h>
#include <dos.h>
#include "wdebug.h"
#include "stdwin.h"

static HHOOK hookHandle;
//static FARPROC debugHookInstance;

/*
 * DebugHook - don't allow any hook functions in debugee to get called
 */
DWORD FAR PASCAL DebugHook( int ncode, WORD wparam, DWORD lparam )
{
    return( CallNextHookEx( hookHandle, ncode, wparam, lparam ) );
#if 0
    Out((OUT_HOOK,"In DebugHook ncode=%d wparm=%x lparm=%lx", ncode, wparam, lparam ));
    if( wparam != TaskAtFault || ncode < 0 ) {
        return( CallNextHookEx( hookHandle, ncode, wparam, lparam ) );
    }
    return( 1 );
#endif

} /* DebugHook */

/*
 * InitDebugHook - set up debug hook
 */
void InitDebugHook( void )
{

//  debugHookInstance = MakeProcInstance( (FARPROC)DebugHook, Instance );
//  hookHandle = SetWindowsHookEx( WH_DEBUG, debugHookInstance, Instance, NULL );
//  Out((OUT_HOOK,"InitDebugHook - instance = %Fp HookHandle = %8.8lx", debugHookInstance, hookHandle ));
} /* InitDebugHook */

/*
 * FiniDebugHook - finish debug hook
 */
void FiniDebugHook( void )
{

//  UnhookWindowsHookEx( hookHandle );
//  FreeProcInstance( debugHookInstance );

} /* FiniDebugHook */
