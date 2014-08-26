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


#include "precomp.h"
#include "wglbl.h"
#include "wctl3d.h"
#include "wrdll.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
// #define W_USE_3D

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

bool WCtl3DInit( HINSTANCE inst )
{
#if !defined( W_USE_3D )
    _wtouch( inst );
    return( TRUE );
#else
    bool ok;

    ok = WRCtl3dRegister( inst );
    if( ok ) {
        ok = WRCtl3dAutoSubclass( inst );
    }

    return( ok );
#endif
}

void WCtl3DFini( HINSTANCE inst )
{
#if !defined( W_USE_3D )
    _wtouch( inst );
#else
    WRCtl3dUnregister( inst );
#endif
}

void WCtl3dColorChange( void )
{
#if defined( W_USE_3D )
    WRCtl3dColorChange();
#endif
}

void WCtl3dSubclassDlg( HWND win, WORD w )
{
#if !defined( W_USE_3D )
    _wtouch( win );
    _wtouch( w );
#else
    WRCtl3dSubclassDlg( win, w );
#endif
}

void WCtl3dSubclassDlgAll( HWND win )
{
#if !defined( W_USE_3D )
    _wtouch( win );
#else
    WRCtl3dSubclassDlgAll( win );
#endif
}

HBRUSH WCtl3dCtlColorEx( UINT msg, WPARAM wp, LPARAM lp )
{
#if !defined( W_USE_3D )
    _wtouch( msg );
    _wtouch( wp );
    _wtouch( lp );

    return( (HBRUSH)NULL );
#else
    return( WRCtl3dCtlColorEx( msg, wp, lp ) );
#endif
}
