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
#include <string.h>
#include <dos.h>
#include "drwatcom.h"

/*
 * CheckIsWin32App - check if task is a win 32 app
 */
bool CheckIsWin32App( HANDLE htask )
{
    TASKENTRY   te;

    Win32CS = Win32DS = 0;
    if( !MyTaskFindHandle( &te, htask ) ) {
        return( false );
    }
    return( CheckIsModuleWin32App( te.hModule, &Win32DS, &Win32CS, &Win32InitialEIP ) );

} /* CheckIsWin32App */


/*
 * DoGlobalEntryHandle
 */
BOOL DoGlobalEntryHandle( GLOBALENTRY *ge, HANDLE hmem )
{
    if( (WORD)hmem == Win32CS ) {
        ge->hBlock = hmem;
        ge->hOwner = DTTaskEntry.hModule;
        ge->wData = 1;
        return( 1 );
    } else if( (WORD)hmem == Win32DS ) {
        ge->hBlock = hmem;
        ge->hOwner = DTTaskEntry.hModule;
        ge->wData = 2;
        return( 1 );
    } else {
        return( MyGlobalEntryHandle( ge, hmem ) );
    }

} /* DoGlobalEntryHandle */


/*
 * DoGlobalEntryModule
 */
BOOL DoGlobalEntryModule( GLOBALENTRY *ge, HMODULE hmod, WORD seg )
{
    if( hmod == DTTaskEntry.hModule && IsWin32App ) {
        if( seg == 1 ) {
            ge->hBlock = (HGLOBAL)Win32CS;
            ge->dwSize = 1;
            ge->dwBlockSize = GetASelectorLimit( Win32CS );
            return( 1 );
        } else if( seg == 2 ) {
            ge->hBlock = (HGLOBAL)Win32DS;
            ge->dwSize = 1;
            ge->dwBlockSize = GetASelectorLimit( Win32DS );
            return( 1 );
        }
        ge->hBlock = 0;
        ge->dwSize = 0;
        return( 0 );
    } else {
        return( MyGlobalEntryModule( ge, hmod, seg ) );
    }

} /* DoGlobalEntryModule */
