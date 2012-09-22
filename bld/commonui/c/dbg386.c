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


#include "precomp.h"
#include "wdebug.h"
#include "di386.h"

void            (FAR PASCAL *DoneWithInterrupt)( LPVOID );
int             (FAR PASCAL *GetDebugInterruptData)( LPVOID );
void            (FAR PASCAL *ResetDebugInterrupts32)( void );
int             (FAR PASCAL *SetDebugInterrupts32)( void );
int             (FAR PASCAL *IsDebuggerExecuting)( void );
int             (FAR PASCAL *DebuggerIsExecuting)( int );
static HANDLE   wint32DLL;
extern BOOL     WDebug386;

/*
 * Start386Debug - initialize for 32-bit debugging
 */
void Start386Debug( void )
{
    if( CheckWin386Debug() == WGOD_VERSION ) {
        WDebug386 = TRUE;
    }
    if( WDebug386 ) {
        wint32DLL = LoadLibrary( "wint32.dll" );
        if( (UINT)wint32DLL < 32 ) {
            WDebug386 = FALSE;
        } else {
            DoneWithInterrupt = (LPVOID)GetProcAddress( wint32DLL, "DoneWithInterrupt" );
            GetDebugInterruptData = (LPVOID)GetProcAddress( wint32DLL, "GetDebugInterruptData" );
            ResetDebugInterrupts32 = (LPVOID)GetProcAddress( wint32DLL, "ResetDebugInterrupts32" );
            SetDebugInterrupts32 = (LPVOID)GetProcAddress( wint32DLL, "SetDebugInterrupts32" );
            IsDebuggerExecuting = (LPVOID)GetProcAddress( wint32DLL, "IsDebuggerExecuting" );
            DebuggerIsExecuting = (LPVOID)GetProcAddress( wint32DLL, "DebuggerIsExecuting" );

            if( !SetDebugInterrupts32() ) {
                WDebug386 = FALSE;
                FreeLibrary( wint32DLL );
            }
        }
    }

} /* Start386Debug */


/*
 * Done386Debug - done with 386 debug handling
 */
void Done386Debug( void )
{
    if( WDebug386 ) {
        ResetDebugInterrupts32();
        FreeLibrary( wint32DLL );
        WDebug386 = FALSE;
    }

} /* Done386Debug */
