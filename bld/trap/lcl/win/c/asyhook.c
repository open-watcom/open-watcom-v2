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


#include <windows.h>
#include "stdwin.h"

/* don't call this directly */
extern void far AsynchHook( void );     // from myhook.asm

/* these are referenced in myhook.asm */
FARPROC  OldHook = NULL;
//FARPROC  My_timer_proc;
char volatile AsynchStop;

/* this is an undocumented function from the KERNEL. see Undocument Windows */
extern FARPROC FAR PASCAL ToolHelpHook( FARPROC new_hook );

#if 0
extern void __export CALLBACK MyTimer( HWND wnd, UINT msg,
                UINT timer, DWORD time )
/********************************************************/
{
    msg = msg;
    timer = timer;
    time = time;

    if( AsyncStop ) {
        AsyncStop = FALSE;
        MessageBox( wnd, "Stop Already", "Async-stop", MB_SYSTEMMODAL|MB_OK|MB_ICONINFORMATION);
    }
}
#endif


extern void InitASynchHook()
//extern void InitHook( HWND wnd, HINSTANCE inst )
/**********************************************/
{
    OldHook = ToolHelpHook( (FARPROC)AsynchHook );
//  My_timer_proc = (FARPROC) MakeProcInstance( MyTimer, inst );
//  SetTimer( wnd, 1, 100, My_timer_proc );
} /* InitHook */

extern void FiniASynchHook()
//extern void FiniASynchHook( HWND wnd )
/******************************/
{
//  KillTimer( wnd, 1 );
//  FreeProcInstance( My_timer_proc );
    ToolHelpHook( OldHook );
} /* ShutHook */
