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
* Description:  Win386 debugging routines.
*
****************************************************************************/


#include "commonui.h"
#include "bool.h"
#include "wdebug.h"

#define global_di386
#include "di386cli.h"

static HANDLE   wint32DLL;

/*
 * Start386Debug - initialize for 32-bit debugging
 */
bool Start386Debug( void )
{
    WDebug386 = false;
    if( CheckWin386Debug() == WGOD_VERSION ) {
        wint32DLL = LoadLibrary( "wint32.dll" );
        if( (UINT)wint32DLL >= 32 ) {
            DoneWithInterrupt = (LPVOID)GetProcAddress( wint32DLL, "DoneWithInterrupt" );
            GetDebugInterruptData = (LPVOID)GetProcAddress( wint32DLL, "GetDebugInterruptData" );
            ResetDebugInterrupts32 = (LPVOID)GetProcAddress( wint32DLL, "ResetDebugInterrupts32" );
            SetDebugInterrupts32 = (LPVOID)GetProcAddress( wint32DLL, "SetDebugInterrupts32" );
            IsDebuggerExecuting = (LPVOID)GetProcAddress( wint32DLL, "IsDebuggerExecuting" );
            DebuggerIsExecuting = (LPVOID)GetProcAddress( wint32DLL, "DebuggerIsExecuting" );

            if( SetDebugInterrupts32() ) {
                WDebug386 = true;
            } else {
                FreeLibrary( wint32DLL );
            }
        }
    }
    return( WDebug386 );

} /* Start386Debug */


/*
 * Done386Debug - done with 386 debug handling
 */
void Done386Debug( void )
{
    if( WDebug386 ) {
        ResetDebugInterrupts32();
        FreeLibrary( wint32DLL );
        WDebug386 = false;
    }

} /* Done386Debug */
