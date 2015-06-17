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
* Description:  Windows 386 Supervisor WinMain code (16-bit).
*
****************************************************************************/


#include <stddef.h>
#include <windows.h>
#include "winext.h"
#include "wininit.h"

extern DWORD ReturnCode;

extern void PASCAL InvokeWin32( void );

int PASCAL WinMain( HINSTANCE thishandle, HINSTANCE prevhandle, LPSTR cmdline,
                    int cmdshow )
{
    if( !Init32BitTask( thishandle, prevhandle, cmdline, cmdshow ) ) {
        return( FALSE );
    }

    InvokeWin32();
    Fini( 0 );
    return( (int) ReturnCode );

} /* WinMain */
