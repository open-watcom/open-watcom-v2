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
#include "trptypes.h"

extern TRAPENTRY_FUNC( InputHook );
extern TRAPENTRY_FUNC( InfoFunction );
extern TRAPENTRY_FUNC( UnLockInput );
extern TRAPENTRY_FUNC( SetHardMode );
extern TRAPENTRY_FUNC( HardModeCheck );
extern TRAPENTRY_FUNC( GetHwndFunc );

void TRAPENTRY InfoFunction( HWND hwnd )
{
    hwnd = hwnd;
}

void TRAPENTRY InputHook( hook_fn *ptr )
{
    ptr = ptr;
}

bool TRAPENTRY HardModeCheck( void )
{
    return( false );
}
/*
 * GetHwndFunc - inform trap file of gui debugger being used
 */

HWND TRAPENTRY GetHwndFunc( void )
{
    return( (HWND)NULL );
}

/*
 * SetHardMode - force hard mode
 */
void TRAPENTRY SetHardMode( bool force )
{
    force=force;
}

/*
 * UnLockInput - unlock input from the debugger
 */
void TRAPENTRY UnLockInput( void )
{
}
