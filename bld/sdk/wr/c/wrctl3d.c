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
* Description:  Cover functions to avoid dependency on CTL3D DLL.
*
****************************************************************************/


#include <wwindows.h>
#include "wrglbl.h"
#include "ctl3dcvr.h"
#include "wrctl3d.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
// #define WR_USE_3D

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

int WRAPI WRCtl3DInit( HINSTANCE inst )
{
    return( CvrCtl3DInit( inst ) );
}

void WRAPI WRCtl3DFini( HINSTANCE inst )
{
    CvrCtl3DFini( inst );
}

bool WRAPI WRCtl3dSubclassDlg( HWND hwnd, WORD w )
{
    return( CvrCtl3dSubclassDlg( hwnd, w ) );
}

bool WRAPI WRCtl3dSubclassDlgAll( HWND hwnd )
{
    return( CvrCtl3dSubclassDlgAll( hwnd ) );
}

bool WRAPI WRCtl3dSubclassDlgEx( HWND hwnd, DWORD dw )
{
    return( CvrCtl3dSubclassDlgEx( hwnd, dw ) );
}

bool WRAPI WRCtl3dSubclassDlgExAll( HWND hwnd )
{
    return( CvrCtl3dSubclassDlgExAll( hwnd ) );
}

WORD WRAPI WRCtl3dGetVer( void )
{
    return( CvrCtl3dGetVer() );
}

bool WRAPI WRCtl3dEnabled( void )
{
    return( CvrCtl3dEnabled() );
}

HBRUSH WRAPI WRCtl3dCtlColor( HDC dc, LONG l )
{
    return( CvrCtl3dCtlColor( dc, l ) );
}

HBRUSH WRAPI WRCtl3dCtlColorEx( UINT wm, WPARAM wParam, LPARAM lParam )
{
    return( CvrCtl3dCtlColorEx( wm, wParam, lParam ) );
}

bool WRAPI WRCtl3dColorChange( void )
{
    return( CvrCtl3dColorChange() );
}

bool WRAPI WRCtl3dSubclassCtl( HWND hwnd )
{
    return( CvrCtl3dSubclassCtl( hwnd ) );
}

LONG WRAPI WRCtl3dDlgFramePaint( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
{
    return( CvrCtl3dDlgFramePaint( hwnd, msg, wp, lp ) );
}

bool WRAPI WRCtl3dAutoSubclass(HANDLE hndl )
{
    return( CvrCtl3dAutoSubclass( hndl ) );
}

bool WRAPI WRCtl3dRegister( HANDLE hndl )
{
    return( CvrCtl3dRegister( hndl ) );
}

bool WRAPI WRCtl3dUnregister( HANDLE inst )
{
    return( CvrCtl3dUnregister( inst ) );
}

void WRAPI WRCtl3dWinIniChange( void )
{
    CvrCtl3dWinIniChange();
}
